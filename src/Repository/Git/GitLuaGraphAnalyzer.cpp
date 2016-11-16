#include "Repository/Git/GitLuaGraphAnalyzer.h"

#include "LuaGraph/LuaGraph.h"
#include "LuaGraph/LuaNode.h"
#include "LuaGraph/LuaEdge.h"
#include "LuaGraph/LuaIncidence.h"

#include "GitLib/GitVersion.h"
#include "GitLib/GitFile.h"
#include "GitLib/GitFunction.h"
#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitUtils.h"
#include "GitLib/GitFileLoader.h"
#include "GitLib/GitFileDiffBlock.h"
#include "GitLib/GitFileDiffBlockLine.h"
#include "GitLib/GitMetaData.h"

#include <QDebug>
#include <QFile>
#include <QList>
#include <QTextStream>

#include <limits>
#include <string>

Repository::Git::GitLuaGraphAnalyzer::GitLuaGraphAnalyzer()
	: luaGraph( Lua::LuaGraph::getInstance() ), evolutionGraph( nullptr ), versionNumber( -1 ), functions( new QMap<QString, Repository::Git::GitFunction*>() )
{
}

Repository::Git::GitLuaGraphAnalyzer::GitLuaGraphAnalyzer( Lua::LuaGraph* luaGraph, Repository::Git::GitEvolutionGraph* evolutionGraph )
	: luaGraph( luaGraph ), evolutionGraph( evolutionGraph ), versionNumber( -1 ), functions( new QMap<QString, Repository::Git::GitFunction*>() )
{

}

Repository::Git::GitLuaGraphAnalyzer::~GitLuaGraphAnalyzer()
{
//    qDebug() << "Destruktor GitLuaGraphAnalyzer";
	delete functions;
}

void Repository::Git::GitLuaGraphAnalyzer::analyze()
{
	Repository::Git::GitVersion* version =  this->evolutionGraph->getVersion( this->versionNumber );
	QMap<QString, Repository::Git::GitFile*> versionFiles = *version->getChangedFiles();

	// Prejdem kazdy nacitani LuaNode a skontrolujem, ci sa nenachadza medzi zmenenymi subormi v danej verzii
	for ( QMap<qlonglong, Lua::LuaNode*>::iterator iterator = this->luaGraph->getNodes()->begin(); iterator != this->luaGraph->getNodes()->end(); ++iterator ) {
		qDebug() << iterator.value()->getIdentifier();
		// Ak sa LuaNode nachadza v zmenenych suborov, tak zistim rozdiel podstromu suboru voci predchadzajucej verzii suboru
		if ( versionFiles.contains( iterator.value()->getIdentifier() ) ) {
			Lua::LuaNode* node = iterator.value();

			// Mapa na ulozenie funkcii pouzitych v danom subore
			QMap<QString, Repository::Git::GitFunction*> functions = QMap<QString, Repository::Git::GitFunction*>();

			// Git file, ktory zobrazuje sucasnu verziu suboru s jeho podstromom funkcii
			Repository::Git::GitFile* file = new Repository::Git::GitFile( node->getIdentifier(), node->getIdentifier().split( ";" ).at( 1 ), Repository::Git::GitType::MODIFIED );

			int counter = 0;

			// prejdem vsetky incidence, ktore obsahuje LuaNode a pridame vsetky funkcie, ktore su volanie zo suboru
			foreach ( qlonglong incidenceId, node->getIncidences() ) {
				Lua::LuaIncidence* incidence = this->luaGraph->getIncidences()->value( incidenceId );

				// Ak je incidence orientovany a outGoing je false, tak ide o hranu zo suboru na globalFunction
				// a pridam k danemu suboru vsetky global funkcie spolu s ich modulmi
				if ( incidence->getOriented() && !incidence->getOutGoing() ) {
					Lua::LuaEdge* edge = this->luaGraph->getEdges()->value( incidence->getEdgeNodePair().first );
					Lua::LuaNode* pairNode = nullptr;

					// Zistim druhy incidence, ktory obsahuje druhy LuaNode pre danu hranu
					Lua::LuaIncidence* otherIncidence = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 0 ) );
					if ( incidence == otherIncidence ) {
						otherIncidence = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 1 ) );
					}

					// LuaNode, ku ktoremu smeruje hrana zo suboru
					pairNode = this->luaGraph->getNodes()->value( otherIncidence->getEdgeNodePair().second );

					// Ziskam identifikator globalnej funkcie
					QString functionIdentifier =  pairNode->getIdentifier();

					// Zistim, ci sa identifikator globalnej funkcie nachadza uz v ulozenych funkciach pre dany subor
					Repository::Git::GitFunction* function = functions.value( functionIdentifier );

					// Ak sa globalna funkcia este nenachadza v mape funkcii, tak ju vytvorime pre dany subor
					if ( !function ) {
						// Inicializujem globalnu funkciu
						function = new Repository::Git::GitFunction();

						// Rozlozim identifikator cez oddelovac ";"
						QStringList list = functionIdentifier.split( ";" );

						// Ak sa pocet tokenov z identifikatora rovna 2, tak globalna funkcia nema global modul
						// inak nenastavim modul
						if ( list.size() == 2 ) {
							function->setName( list.at( 1 ) );
						}
						else {
							function->setName( list.at( 2 ) );
							function->setModule( list.at( 1 ) );
						}

						// Typ globalnej funkcie nas nezaujima pri analyze, preto nastavim na NONE
						function->setType( Repository::Git::GitType::NONE );
						function->setId( pairNode->getId() );

						// Kontrola, ze ide o globalnu funkciu, ak by nebola tak sa vypise chybova sprava
						if ( !QString::compare( QString::fromStdString( pairNode->getParams().getValue()["type"].asString() ), "globalFunction" ) ) {
							function->setFunctionType( Repository::Git::GitFunctionType::GLOBALFUNCTION );
						}
						else {
							qDebug() << "FUNKCIA NIE JE GLOBALNA. CHYBA V ANALYZE DAT";
						}

						// Vytvorenu globalnu funkciu pridame do vsetkych funkcii pre dany subor
						functions.insert( functionIdentifier, function );
					}

					// Globalnu funkciu pridame do zoznamu funkcii obsiahnutych v subore
					file->addGitFunction( function );
					counter++;

				}
				else {
					// Incidence nie je orientovany alebo nie je Out going
					// V tomto pripade ide o hranu na DIR(tato moznost nas v analyze nezaujima) alebo hrana k lokalnej funkcii
					Lua::LuaEdge* edge = this->luaGraph->getEdges()->value( incidence->getEdgeNodePair().first );
					Lua::LuaNode* pairNode = nullptr;

					// Zistim druhy incidence, ktory obsahuje druhy LuaNode pre danu hranu
					Lua::LuaIncidence* otherIncidence = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 0 ) );
					if ( incidence == otherIncidence ) {
						otherIncidence = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 1 ) );
					}

					// LuaNode, ku ktoremu smeruje hrana zo suboru
					pairNode = this->luaGraph->getNodes()->value( otherIncidence->getEdgeNodePair().second );

					// Pokracujeme v spracovani len ak ide o hranu s lokalnou funkciou
					if ( !QString::compare( QString::fromStdString( pairNode->getParams().getValue()["type"].asString() ), "function" ) ) {
						// Ziskam identifikator lokalnej funkcie
						QString functionIdentifier = pairNode->getIdentifier();

						// Zistim, ci sa identifikator lokalnej funkcie nachadza uz v ulozenych funkciach pre dany subor
						Repository::Git::GitFunction* function =  functions.value( functionIdentifier );

						// Ak sa lokalna funkcia este nenachadza v mape funkcii, tak ju vytvorime pre dany subor
						if ( !function ) {
							// Inicializujem lokalnu funkciu
							function = new Repository::Git::GitFunction();

							// Rozlozim identifikator cez oddelovac ";"
							QStringList list = functionIdentifier.split( ";" );

							// Meno funkcie sa sklada za cesty suboru, v ktorom sa nachadza funckia(1) a nazvu funkcie (2)
							function->setName( list.at( 1 ) + ";" + list.at( 2 ) );

							// Typ globalnej funkcie nas nezaujima pri analyze, preto nastavim na NONE
							function->setType( Repository::Git::GitType::NONE );
							function->setId( pairNode->getId() );

							// Kontrola, ze ide o lokalnu funkciu, ak by nebola tak sa vypise chybova sprava
							if ( !QString::compare( QString::fromStdString( pairNode->getParams().getValue()["type"].asString() ), "function" ) ) {
								function->setFunctionType( Repository::Git::GitFunctionType::LOCALFUNCTION );
							}
							else {
								qDebug() << "FUNKCIA NIE JE LOKALNA. CHYBA V ANALYZE DAT";
							}

							// Vytvorenu lokalnu funkciu pridame do vsetkych funkcii pre dany subor
							functions.insert( functionIdentifier, function );
						}

						// Lokalnu funkciu pridame do zoznamu funkcii obsiahnutych v subore
						file->addGitFunction( function );

						// Kedze ide o lokalnu funkciu, tak ta moze volat dalsie funkcie (lokalne, globalne)
						// Prejdem vsetky hrany, ktore idu z lokalnej funkcie a su orientovane a out going = false
						foreach ( qlonglong functionIncidenceId, pairNode->getIncidences() ) {
							Lua::LuaIncidence* functionIncidence = this->luaGraph->getIncidences()->value( functionIncidenceId );

							// Iba ak je incidence orientovany a out going nastaveny na false, tak pokracujem v dalsej analyze
							if ( functionIncidence->getOriented() && !functionIncidence->getOutGoing() ) {
								Lua::LuaEdge* functionEdge = this->luaGraph->getEdges()->value( functionIncidence->getEdgeNodePair().first );
								Lua::LuaNode* functionPairNode = nullptr;

								// Zistim druhy incidence, ktory obsahuje druhy LuaNode pre danu hranu
								Lua::LuaIncidence* functionOtherIncidence = this->luaGraph->getIncidences()->value( functionEdge->getIncidences().at( 0 ) );
								if ( functionIncidence == functionOtherIncidence ) {
									functionOtherIncidence = this->luaGraph->getIncidences()->value( functionEdge->getIncidences().at( 1 ) );
								}

								// LuaNode, ku ktoremu smeruje hrana zo suboru
								functionPairNode = this->luaGraph->getNodes()->value( functionOtherIncidence->getEdgeNodePair().second );

								// Ziskam identifikator funkcie(moze ist o lokalnu, aj globalnu)
								QString innerFunctionIdentifier = functionPairNode->getIdentifier();

								// Zistim, ci sa identifikator funkcie nachadza uz v ulozenych funkciach pre dany subor
								Repository::Git::GitFunction* innerFunction = functions.value( innerFunctionIdentifier );

								// Ak sa funkcia este nenachadza v mape funkcii, tak ju vytvorime pre dany subor
								if ( !innerFunction ) {
									// Inicializujem funkciu
									innerFunction = new Repository::Git::GitFunction();

									// Rozlozim identifikator cez oddelovac ";"
									QStringList list = innerFunctionIdentifier.split( ";" );

									// Zistim akeho typu je funkcia(lokalna, globalna) a podla toho nastavim funkciu
									if ( !QString::compare( QString::fromStdString( functionPairNode->getParams().getValue()["type"].asString() ), "function" ) ) {
										// Meno funkcie sa sklada za cesty suboru, v ktorom sa nachadza funckia(1) a nazvu funkcie (2)
										innerFunction->setName( list.at( 1 ) + ";" + list.at( 2 ) );
										innerFunction->setFunctionType( Repository::Git::GitFunctionType::LOCALFUNCTION );
									}
									else {
										// Ak sa pocet tokenov z identifikatora rovna 2, tak globalna funkcia nema global modul
										// inak nenastavim modul
										if ( list.size() == 2 ) {
											innerFunction->setName( list.at( 1 ) );
										}
										else {
											innerFunction->setName( list.at( 2 ) );
											innerFunction->setModule( list.at( 1 ) );
										}
										innerFunction->setFunctionType( Repository::Git::GitFunctionType::GLOBALFUNCTION );
									}

									// Typ funkcie nas nezaujima pri analyze, preto nastavim na NONE
									innerFunction->setType( Repository::Git::GitType::NONE );
									innerFunction->setId( functionPairNode->getId() );

									// Vytvorenu funkciu pridame do vsetkych funkcii pre dany subor
									functions.insert( innerFunctionIdentifier, innerFunction );
								}

								// Pridam vztah k materskej/vnorenej funkcii z vnorenej/materskej funkcii
								function->addFunctionCaller( innerFunction );
								innerFunction->addFunctionCallee( function );
							}
						}
					}

				}

			}

			findFunctionRowsFromFile( file );

			// Ziskam starsiu verziu daneho suboru
			Repository::Git::GitFile* oldFile = evolutionGraph->getMetaDataFromIdentifier( file->getIdentifier() )->getCallTree();

			// Porovnam aktualnu verziu suboru s predoslou verziou
			compareFilesAndSaveToEvolutionGraph( file, oldFile );

			// Nahradim predoslu verziu sucasnou v mape verzii suborov a ich podstromov
			this->evolutionGraph->getMetaDataFromIdentifier( file->getIdentifier() )->setCallTree( file );
			this->evolutionGraph->getMetaDataFromIdentifier( file->getIdentifier() )->setLastDiffVersion( version->getCommitId() );

			/*
						for( QMap<QString, Repository::Git::GitFunction*>::iterator  iterator = file->getGitFunctions()->begin(); iterator != file->getGitFunctions()->end(); ++iterator ) {
							Repository::Git::GitFunction* function = iterator.value();

							qDebug() << file->getIdentifier() << "->" <<  function->getIdentifier();

							for( QMap<QString, Repository::Git::GitFunction*>::iterator iter = function->getFunctionCallers()->begin(); iter != function->getFunctionCallers()->end(); ++iter ) {
								Repository::Git::GitFunction* innerFunction = iter.value();
								qDebug() << "Caller ->" << innerFunction->getIdentifier();
							}

							for( QMap<QString, Repository::Git::GitFunction*>::iterator iter = function->getFunctionCallees()->begin(); iter != function->getFunctionCallees()->end(); ++iter ) {
								Repository::Git::GitFunction* innerFunction = iter.value();
								qDebug() << "Callee ->" << innerFunction->getIdentifier();
							}
						}
			*/
		}
	}

	// Spracovanie suborov, ktore maju typ REMOVED
	QMap<QString, Repository::Git::GitFile*>* removedFiles = this->evolutionGraph->getVersion( this->getVersionNumber() )->getGitFilesByType( Repository::Git::GitType::REMOVED );
	for ( QMap<QString, Repository::Git::GitFile*>::iterator iterator = removedFiles->begin(); iterator != removedFiles->end(); ++iterator ) {
		Repository::Git::GitFile* oldFile = this->evolutionGraph->getMetaDataFromIdentifier( iterator.value()->getIdentifier() )->getCallTree();
		compareFilesAndSaveToEvolutionGraph( nullptr, oldFile );
	}

	// Nastav, ze verzia bola nacitana
	version->setIsLoaded( true );

}

void Repository::Git::GitLuaGraphAnalyzer::compareFilesAndSaveToEvolutionGraph( Repository::Git::GitFile* newFile, Repository::Git::GitFile* oldFile )
{
	// Ziskam subor, v ktorom su ulozene vsetky metadata v evolucnom grafe pre dany subor
	Repository::Git::GitFile* file = nullptr;
	if ( newFile != nullptr ) {
		file = this->evolutionGraph->getVersion( this->versionNumber )->getGitFileByIdentifier( newFile->getIdentifier() );
	}
	else {
		file = this->evolutionGraph->getVersion( this->versionNumber )->getGitFileByIdentifier( oldFile->getIdentifier() );
	}

	if ( file->getType() == Repository::Git::GitType::MODIFIED ) {
		Repository::Git::GitFileLoader loader = Repository::Git::GitFileLoader( this->evolutionGraph->getFilePath(), "" );
		loader.getDiffInfo( file, this->evolutionGraph->getVersion( this->versionNumber )->getCommitId(), this->evolutionGraph->getMetaDataFromIdentifier( file->getIdentifier() )->getLastDiffVersion() );
//        foreach( Repository::Git::GitFileDiffBlock* block, file->getGitFileDiffBlocks() ) {
//            block->printInfo();
//            qDebug() << "Previous";
//            block->printPreviousInfo();
//        }
	}

	// Vytvorim mapu lokalnych funkcii na ulozenie uz spracovanych lokalnych funkcii
	QMap<QString, bool>* functions = new QMap<QString, bool>();

	// Prejdem funkcie ulozene v novom subore a skontrolujem, ci sa dany identifikator nachadza v starsej verzii a nasledne porovnam tieto dve funkcie
	if ( newFile != nullptr ) {
		for ( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = newFile->getGitFunctions()->begin(); iterator != newFile->getGitFunctions()->end(); ++iterator ) {
			// Ak ide o lokalnu funkciu, tak pridam jeho identifikator do mapy lokalnych funkcii, ak sa tam nenachadza. Ak sa nachadza, tak pokracujem na dalsiu funkciu
			if ( iterator.value()->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
				// Ak sa lokalna funkcia nachadza v mape lokalnych funkcii, tak preskocim spracovanie danej lokalnej funkcie
				// Inak ju pridam do mapy lokalnych funkcii
				if ( functions->contains( iterator.value()->getIdentifier() ) ) {
					continue;
				}
				else {
					functions->insert( iterator.value()->getIdentifier(), true );
				}
			}

			// Ak este starsia verzia suboru nebola vytvorena, tak kontrolujeme voci nullptr
			if ( oldFile != nullptr ) {
				compareFunctions( iterator.value(), oldFile->getGitFunctions()->value( iterator.value()->getIdentifier() ), functions, file->getIdentifier(), file );
			}
			else {
				compareFunctions( iterator.value(), nullptr, functions, file->getIdentifier(), file );
			}
		}
	}

	// Ak existuje starsia verzia suboru, tak prejdem funkcie ulozene v starom subore a skontrolujem, ci sa dany identifikator nachadza v novej verzii a nasledne porovname tieto dve funkcie
	if ( oldFile != nullptr ) {
		// Prejdem funkcie ulozene v starom subore a skontrolujem, ci sa dany identifikator nachadza v novej verzii a nasledne porovnam tieto dve funkcie
		for ( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = oldFile->getGitFunctions()->begin(); iterator != oldFile->getGitFunctions()->end(); ++iterator ) {
			// Ak ide o lokalnu funkciu, tak pridam jeho identifikator do mapy lokalnych funkcii, ak sa tam nenechadza. Ak sa nachadza, tak pokracujem na dalsiu funkciu.
			if ( iterator.value()->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
				// Ak sa lokalna funkcia nachadza v mape lokalnych funkcii, tak preskocim spracovanie danej lokalnej funkcie
				// Inak ju pridam do mapy lokalnych funkcii
				if ( functions->contains( iterator.value()->getIdentifier() ) ) {
					continue;
				}
				else {
					functions->insert( iterator.value()->getIdentifier(), true );
				}
			}
			if ( newFile != nullptr ) {
				compareFunctions( newFile->getGitFunctions()->value( iterator.value()->getIdentifier() ), iterator.value(), functions, file->getIdentifier(), file );
			}
			else {
				compareFunctions( nullptr, iterator.value(), functions, file->getIdentifier(), file );
			}
		}
	}
	/*
		for( QMap<QString, Repository::Git::GitFunction*>::iterator  iterator = file->getGitFunctions()->begin(); iterator != file->getGitFunctions()->end(); ++iterator ) {
			Repository::Git::GitFunction* function = iterator.value();

			qDebug() << file->getIdentifier() << "->" <<  function->getIdentifier() << function->getTypeAsString();

			for( QMap<QString, Repository::Git::GitFunction*>::iterator iter = function->getFunctionCallers()->begin(); iter != function->getFunctionCallers()->end(); ++iter ) {
				Repository::Git::GitFunction* innerFunction = iter.value();
				qDebug() << "Caller ->" << innerFunction->getIdentifier() << innerFunction->getTypeAsString();
			}

			for( QMap<QString, Repository::Git::GitFunction*>::iterator iter = function->getFunctionCallees()->begin(); iter != function->getFunctionCallees()->end(); ++iter ) {
				Repository::Git::GitFunction* innerFunction = iter.value();
				qDebug() << "Callee ->" << innerFunction->getIdentifier() << innerFunction->getTypeAsString();
			}
		}
	*/
//    qDebug() << file->getIdentifier();
	if ( file->getType() == Repository::Git::GitType::REMOVED ) {
		qDebug() << "VYMAZANY SUBOR" << file->getIdentifier();
		qDebug() << "FUNCTION COUNT" << file->getGitFunctions()->size() << "/" << oldFile->getGitFunctions()->size();
	}

	// Vymazem identifikatory funkcii
	delete functions;
}

void Repository::Git::GitLuaGraphAnalyzer::compareFunctions( Repository::Git::GitFunction* newFunction, Repository::Git::GitFunction* oldFunction, QMap<QString, bool>* functions, QString masterIdentifier, Repository::Git::GitFile* masterFile )
{

	// Ak su obe funkcie rozdielne od nullptr
	if ( newFunction != nullptr && oldFunction != nullptr ) {
		if ( !( newFunction->getFunctionType() == Repository::Git::GitFunctionType::GLOBALFUNCTION ) ) {

			// Spracujeme vsetky funkcie, ktore vola nova funkcia
			for ( QMap<QString, Repository::Git::GitFunction*>::iterator iterator =  newFunction->getFunctionCallers()->begin(); iterator != newFunction->getFunctionCallers()->end(); ++iterator ) {

				// Ak sa nova funkcia nenachadza v zozname volanych funkcii starej funkcie, tak spravime klon tejto funkcie do evolucneho grafu
				if ( !oldFunction->getFunctionCallers()->contains( iterator.value()->getIdentifier() ) ) {

					// Spravime kopiu novej funkcie
					Repository::Git::GitFunction* addedFunction = new Repository::Git::GitFunction();

					addedFunction->setName( iterator.value()->getName() );
					addedFunction->setModule( iterator.value()->getModule() );
					addedFunction->setId( iterator.value()->getId() );
					addedFunction->setType( Repository::Git::GitType::ADDED );
					addedFunction->setFunctionType( iterator.value()->getFunctionType() );

					// Ziskame lokalnu funkciu, z ktore bola nova funkcia volana, ak existuje
					Repository::Git::GitFunction* cloneNewFunction = masterFile->getGitFunctions()->value( newFunction->getIdentifier() );

					// Ak neexistuje, tak spravime klon tejto funkcie
					if ( !cloneNewFunction ) {
						cloneNewFunction = new Repository::Git::GitFunction();
						cloneNewFunction->setName( newFunction->getName() );
						cloneNewFunction->setId( newFunction->getId() );
						cloneNewFunction->setModule( newFunction->getModule() );
						cloneNewFunction->setType( Repository::Git::GitType::MODIFIED );
						cloneNewFunction->setFunctionType( newFunction->getFunctionType() );

						masterFile->addGitFunction( cloneNewFunction );
					}

					// Prepojime dane funkcie
					cloneNewFunction->addFunctionCaller( addedFunction );
					addedFunction->addFunctionCallee( cloneNewFunction );

				}
			}

			for ( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = oldFunction->getFunctionCallers()->begin(); iterator != oldFunction->getFunctionCallers()->end(); ++iterator ) {
				if ( !newFunction->getFunctionCallers()->contains( iterator.value()->getIdentifier() ) ) {
//                    qDebug() << iterator.value()->getIdentifier() << "sa nenachadza v" << newFunction->getIdentifier() << "IS REMOVED!!!";

					Repository::Git::GitFunction* removedFunction = new Repository::Git::GitFunction();

					removedFunction->setName( iterator.value()->getName() );
					removedFunction->setModule( iterator.value()->getModule() );
					removedFunction->setId( iterator.value()->getId() );
					removedFunction->setType( Repository::Git::GitType::REMOVED );
					removedFunction->setFunctionType( iterator.value()->getFunctionType() );

					Repository::Git::GitFunction* cloneNewFunction = masterFile->getGitFunctions()->value( newFunction->getIdentifier() );

					if ( !cloneNewFunction ) {
						cloneNewFunction = new Repository::Git::GitFunction();
						cloneNewFunction->setName( newFunction->getName() );
						cloneNewFunction->setId( newFunction->getId() );
						cloneNewFunction->setModule( newFunction->getModule() );
						cloneNewFunction->setType( Repository::Git::GitType::MODIFIED );
						cloneNewFunction->setFunctionType( newFunction->getFunctionType() );

						masterFile->addGitFunction( cloneNewFunction );
					}

					cloneNewFunction->addFunctionCaller( removedFunction );
					removedFunction->addFunctionCallee( cloneNewFunction );

				}
			}

			// Ak obsahuje tak musime skontrolovat, ci sa nemodifikovala nejaka lokalna funkcia
			int result = calculateRealResult( newFunction->getId() );

			int functionStart = newFunction->getFunctionRowNumber();
			int functionEnd = functionStart + result;
//            qDebug() << luaFunction->getIdentifier() << functionStart << "-" << functionEnd << "(" << result << ")";

			bool isFound = false;

			int oldFunctionStart = -1;
			int oldFunctionEnd = -1;

			QString oldFunctionInterval = this->evolutionGraph->getMetaDataFromIdentifier( newFunction->getIdentifier() )->getLastFunctionInterval();
			if ( oldFunctionInterval != nullptr && oldFunctionInterval != "" ) {
				oldFunctionStart = oldFunctionInterval.split( "-" ).at( 0 ).toInt();
				oldFunctionEnd = oldFunctionInterval.split( "-" ).at( 1 ).toInt();
			}

			foreach ( Repository::Git::GitFileDiffBlock* block, masterFile->getGitFileDiffBlocks() ) {
				int blockStart = block->getAddStart();
				int blockEnd = block->getAddStart() + block->getAddCount();
				if ( intervalsIntersects( functionStart, functionEnd, blockStart, blockEnd ) ) {
//                    qDebug() << "-" << block->getRemoveStart() << "," << block->getRemoveCount() << " +" << block->getAddStart() << "," << block->getAddStart();
					foreach ( Repository::Git::GitFileDiffBlockLine* line, block->getGitFileDiffBlockLines() ) {
						if ( line->getLineType() == Repository::Git::GitType::ADDED ) {
							if ( line->getLineNumber() <= functionEnd && line->getLineNumber() >= functionStart ) {
								Repository::Git::GitFunction* function = masterFile->getGitFunctions()->value( newFunction->getIdentifier() );

								if ( !function ) {
									function = new Repository::Git::GitFunction();
									function->setId( newFunction->getId() );
									function->setModule( newFunction->getModule() );
									function->setName( newFunction->getName() );
									function->setType( Repository::Git::GitType::MODIFIED );
									function->setFunctionType( newFunction->getFunctionType() );
									function->setFunctionRowNumber( newFunction->getFunctionRowNumber() );

									masterFile->addGitFunction( function );
								}
								else {
									if ( function->getType() != Repository::Git::GitType::MODIFIED ) {
										qDebug() << function->getIdentifier() << "je ulozeny inak ako MODIFIED";
									}
								}

//                                qDebug() << newFunction->getIdentifier() << "was MODIFIED!!!!!!!!!!!";
								isFound = true;
								break;
							}
						}

						if ( line->getLineType() == Repository::Git::GitType::REMOVED ) {
							if ( oldFunctionStart > -1 && oldFunctionEnd > -1 ) {
								if ( line->getLineNumber() <= oldFunctionEnd && line->getLineNumber() >= oldFunctionStart ) {
									Repository::Git::GitFunction* function = masterFile->getGitFunctions()->value( newFunction->getIdentifier() );

									if ( !function ) {
										function = new Repository::Git::GitFunction();
										function->setId( newFunction->getId() );
										function->setModule( newFunction->getModule() );
										function->setName( newFunction->getName() );
										function->setType( Repository::Git::GitType::MODIFIED );
										function->setFunctionType( newFunction->getFunctionType() );
										function->setFunctionRowNumber( newFunction->getFunctionRowNumber() );

										masterFile->addGitFunction( function );
									}
									else {
										if ( function->getType() != Repository::Git::GitType::MODIFIED ) {
											qDebug() << function->getIdentifier() << "je ulozeny inak ako MODIFIED";
										}
									}

//                                qDebug() << newFunction->getIdentifier() << "was MODIFIED!!!!!!!!!!!";
									isFound = true;
									break;
								}
							}

						}
					}
				}

				if ( isFound ) {
					break;
				}
			}

			this->evolutionGraph->getMetaDataFromIdentifier( newFunction->getIdentifier() )->setLastFunctionInterval( QString::number( functionStart ) + "-" + QString::number( functionEnd ) );

		}
	}
	else  if ( newFunction == nullptr ) {
//        qDebug() << "REMOVED ->" << oldFunction->getIdentifier() << "from" << masterIdentifier;

		Repository::Git::GitFunction* masterFileFunction = new Repository::Git::GitFunction();

		masterFileFunction->setName( oldFunction->getName() );
		masterFileFunction->setModule( oldFunction->getModule() );
		masterFileFunction->setFunctionType( oldFunction->getFunctionType() );
		masterFileFunction->setType( Repository::Git::GitType::REMOVED );
		masterFileFunction->setId( oldFunction->getId() );

		masterFile->addGitFunction( masterFileFunction );

		if ( oldFunction->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION && oldFunction->getFunctionCallers()->size() > 0 ) {
			for ( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = oldFunction->getFunctionCallers()->begin(); iterator != oldFunction->getFunctionCallers()->end(); ++iterator ) {
//                qDebug() << "REMOVED ->" << iterator.value()->getIdentifier() << "from" << oldFunction->getIdentifier();

				Repository::Git::GitFunction* innerFunction = new Repository::Git::GitFunction();
				innerFunction->setName( iterator.value()->getName() );
				innerFunction->setModule( iterator.value()->getModule() );
				innerFunction->setFunctionType( iterator.value()->getFunctionType() );
				innerFunction->setType( Repository::Git::GitType::REMOVED );
				innerFunction->setId( iterator.value()->getId() );

				masterFileFunction->addFunctionCaller( innerFunction );
				innerFunction->addFunctionCallee( masterFileFunction );
			}
		}

		if ( oldFunction->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
			if ( this->evolutionGraph->getMetaDataFromIdentifier( oldFunction->getIdentifier() )->getLastFunctionInterval() != "" ) {
				this->evolutionGraph->getMetaDataFromIdentifier( oldFunction->getIdentifier() )->setLastFunctionInterval( "" );
			}
		}
	}
	else {
//        qDebug() << "ADDED ->" << newFunction->getIdentifier() << "from" << masterIdentifier;
		Repository::Git::GitFunction* masterFileFunction = new Repository::Git::GitFunction();

		masterFileFunction->setName( newFunction->getName() );
		masterFileFunction->setModule( newFunction->getModule() );
		masterFileFunction->setFunctionType( newFunction->getFunctionType() );
		masterFileFunction->setType( Repository::Git::GitType::ADDED );
		masterFileFunction->setId( newFunction->getId() );

		masterFile->addGitFunction( masterFileFunction );

		if ( newFunction->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION && newFunction->getFunctionCallers()->size() > 0 ) {
			for ( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = newFunction->getFunctionCallers()->begin(); iterator != newFunction->getFunctionCallers()->end(); ++iterator ) {
//                qDebug() << "ADDED ->" << iterator.value()->getIdentifier() << "from" << newFunction->getIdentifier();

				Repository::Git::GitFunction* innerFunction = new Repository::Git::GitFunction();
				innerFunction->setName( iterator.value()->getName() );
				innerFunction->setModule( iterator.value()->getModule() );
				innerFunction->setFunctionType( iterator.value()->getFunctionType() );
				innerFunction->setType( Repository::Git::GitType::ADDED );
				innerFunction->setId( iterator.value()->getId() );

				masterFileFunction->addFunctionCaller( innerFunction );
				innerFunction->addFunctionCallee( masterFileFunction );
			}
		}

		if ( newFunction->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
			int result = calculateRealResult( newFunction->getId() );
			QString interval = QString::number( newFunction->getFunctionRowNumber() ) + "-" + QString::number( newFunction->getFunctionRowNumber() + result );
			this->evolutionGraph->getMetaDataFromIdentifier( newFunction->getIdentifier() )->setLastFunctionInterval( interval );
		}
	}
}

bool Repository::Git::GitLuaGraphAnalyzer::intervalsIntersects( int firstStart, int firstEnd, int secondStart, int secondEnd )
{

	if ( firstStart < secondStart ) {
		if ( firstEnd >= secondStart || firstEnd >= secondEnd ) {
//            qDebug() << "intervalsIntersects" << firstStart << firstEnd << secondStart << secondEnd;
			return true;
		}
	}
	else if ( secondStart < firstStart ) {
		if ( secondEnd >= firstStart || secondEnd >= firstEnd ) {
//            qDebug() << "intervalsIntersects" << firstStart << firstEnd << secondStart << secondEnd;
			return true;
		}
	}
	else {
//        qDebug() << "intervalsIntersects" << firstStart << firstEnd << secondStart << secondEnd;
		return true;
	}

	return false;
}

int Repository::Git::GitLuaGraphAnalyzer::calculateRealResult( qlonglong luaId )
{
	Lua::LuaNode* node = this->luaGraph->getNodes()->value( luaId );
	int blank = static_cast<int>( node->getParams().getValue()["metrics"].asTable()["LOC"].asTable()["lines_blank"].asNumber() );
	int nonempty = static_cast<int>( node->getParams().getValue()["metrics"].asTable()["LOC"].asTable()["lines_nonempty"].asNumber() );

	int realBlank = ( blank - nonempty - 1 ) / 2;
	return nonempty + realBlank + 1;
}

void Repository::Git::GitLuaGraphAnalyzer::findFunctionRowsFromFile( Repository::Git::GitFile* file )
{
	QMap<QString, int> functionToByte = QMap<QString, int>();
	int min = std::numeric_limits<int>::max();
	for ( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = file->getGitFunctions()->begin(); iterator != file->getGitFunctions()->end(); ++iterator ) {
		Repository::Git::GitFunction* function = iterator.value();
		if ( function->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
			Lua::LuaNode* node = this->luaGraph->getNodes()->value( function->getId() );
			int position = static_cast<int>( node->getParams().getValue()["position"].asNumber() );
			min = position < min ? position : min;
			functionToByte.insert( function->getIdentifier(), position );
//            qDebug() << function->getIdentifier() << position;
		}
	}

	QString filepath = this->evolutionGraph->getFilePath().replace( '\\' , '/' ) + "/" + file->getFilepath();

//    qDebug() << filepath;

	QFile ioFile( filepath );

	if ( ioFile.open( QIODevice::ReadOnly ) ) {
		QTextStream reader( &ioFile );
		QString line;
		int count = 0;
		int row = 0;

		while ( !reader.atEnd() ) {
			line = reader.readLine();
			row++;
			count += line.size() + 2;

			if ( min <= count ) {
				QList<QString> remove = QList<QString>();
				min = std::numeric_limits<int>::max();
				for ( QMap<QString,int>::iterator i = functionToByte.begin(); i != functionToByte.end(); ++i ) {
					if ( i.value() <= count ) {
						file->getGitFunctions()->value( i.key() )->setFunctionRowNumber( row );
//                        qDebug() << file->getGitFunctions()->value( i.key() )->getIdentifier() << row;
						remove.append( i.key() );
					}
					else {
						min = i.value() < min ? i.value() : min;
					}
				}

				foreach ( QString string, remove ) {
					functionToByte.remove( string );
				}
			}
		}

//        qDebug() << filepath << count;
	}

	ioFile.close();

}


