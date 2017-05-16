#include "Repository/Git/GitLuaGraphVisualizer.h"

#include "Data/Graph.h"

#include "GitLib/GitVersion.h"
#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitFile.h"
#include "GitLib/GitFunction.h"
#include "GitLib/GitMetaData.h"
#include "GitLib/GitUtils.h"
#include "GitLib/GitMetrics.h"

#include "LuaGraph/LuaGraph.h"

#include <QMap>
#include <QStringList>
#include <QDebug>

#include <limits>
#include <string>

Repository::Git::GitLuaGraphVisualizer::GitLuaGraphVisualizer( Data::Graph* currentGraph, Repository::Git::GitEvolutionGraph* evolutionGraph, osg::ref_ptr<osg::Camera> camera, int showLuaStats )
	: currentGraph( currentGraph ), evolutionGraph( evolutionGraph ), luaGraph( Lua::LuaGraph::getInstance() ), showLuaStats( showLuaStats ), camera( camera )
{

	// Ak sa grafe nenachadzaju ziadne typy, tak ich pridame
	if ( this->currentGraph->getTypes()->size() == 0 ) {
		addDefaultTypes();
		addCustomTypes();
	}
}

void Repository::Git::GitLuaGraphVisualizer::visualize( bool next )
{

	// ziskame verziu nastavenu vo vizualizovanom grafe
	int currentVersion = this->currentGraph->getCurrentVersion();

	// Nacitame si aktualnu verziu a jej zmenene subory
	Repository::Git::GitVersion* version = this->evolutionGraph->getVersion( currentVersion );
	QMap<QString, Repository::Git::GitFile*> changedFiles = *version->getChangedFiles();

	// Vytvorime identifikator root uzlu
	QString rootIdentifier = "directory;" + this->evolutionGraph->getFilePath();

	// Ziskame prisluchajuci lua root uzol
	Lua::LuaNode* luaRoot = luaGraph->findNodeByLuaIdentifier( rootIdentifier );

	// Ziskame root uzol z grafu, ak existuje
	osg::ref_ptr<Data::Node> root = this->currentGraph->findNodeByLuaIdentifier( rootIdentifier );

	// Ak root este neexistuje v grafe, tak ho pridam spolu s meta uzlami
	if ( !root ) {
		// Vytvorenie root uzla pre repozitar
		root = this->currentGraph->addNode( luaRoot->getId(), this->evolutionGraph->getFilePath(), currentGraph->getTypesByName( "node" ).at( 0 ) );
		root->setLuaIdentifier( rootIdentifier );

		// Vytvorenie meta uzla pre root repozitaru
		osg::ref_ptr<Data::Node> metaRoot = this->currentGraph->addNode( std::numeric_limits<qlonglong>::max(), "rootAnchor", this->currentGraph->getNodeMetaType(), osg::Vec3( 0, 0, 500 ) );
		metaRoot->setLuaIdentifier( "metaRoot" );
		metaRoot->setColor( osg::Vec4( 0, 0, 0, 0 ) );

		// Vytvorenie meta hrany medzi meta root a root
		QString metaEdge = "meta+" + rootIdentifier;
		osg::ref_ptr<Data::Edge> metaLink = this->currentGraph->addEdge( metaEdge, root, metaRoot, this->currentGraph->getEdgeMetaType(), false );
		metaLink->setLuaIdentifier( metaEdge );
		metaLink->setEdgeColor( osg::Vec4f( 0, 0, 0, 0 ) );
		metaLink->setInvisible( true );

		// Vytvorenie meta uzla pre pritahovanie lokalnych funkcii
		osg::ref_ptr<Data::Node> metaFunction = this->currentGraph->addNode( std::numeric_limits<qlonglong>::max() - 1, "functionAnchor", this->currentGraph->getNodeMetaType(), osg::Vec3( 0, 0, -500 ) );
		metaFunction->setLuaIdentifier( "metaFunction" );
		metaFunction->setColor( osg::Vec4( 0, 0, 0, 0 ) );
	}

	// Ulozenie mapovania lua uzla na uzol v grafe
	this->evolutionGraph->getMetaDataFromIdentifier( rootIdentifier )->setLuaMapping( luaRoot->getId() );

	// Ak zobrazujeme predchadzajucu verziu, tak graf upracame, aby neobsahoval veci z vyssej verzie
	if ( !next ) {
		// Nastavime verziu o jednu viac
		int rollBackVersion = currentVersion + 1;

		// Nacitame verziu a ziskame jej zmenene subory
		Repository::Git::GitVersion* version = this->evolutionGraph->getVersion( rollBackVersion );
		QMap<QString, Repository::Git::GitFile*> addedFiles = *version->getChangedFiles();

		// Pre zmeneny subor spravime negovanie predchadzajucej verzie
		for ( QMap<QString, Repository::Git::GitFile*>::iterator iterator = addedFiles.begin(); iterator != addedFiles.end(); ++iterator ) {
			switch ( iterator.value()->getType() ) {
				case Repository::Git::GitType::ADDED:
					removeFileFromGraph( iterator.value(), next );
//                this->evolutionGraph->getMetaDataFromIdentifier( iterator.value()->getIdentifier() )->decreaseChangedCount();
					break;
				case Repository::Git::GitType::REMOVED:
					addFileToGraph( iterator.value(), rootIdentifier );
//                this->evolutionGraph->getMetaDataFromIdentifier( iterator.value()->getIdentifier() )->increaseChangedCount();
					break;
				case Repository::Git::GitType::MODIFIED:
//                this->evolutionGraph->getMetaDataFromIdentifier( iterator.value()->getIdentifier() )->decreaseChangedCount();
					break;
				case Repository::Git::GitType::NONE:
					//do nothing
					break;
				default:
					qDebug() << "CHYBNY TYP PRE SUBOR" << iterator.value()->getIdentifier();
			}

			processFunctionsFromFile( iterator.value(), next );
		}
	}

	// Spracuje subory z aktualnej verzie
	for ( QMap<QString, Repository::Git::GitFile*>::iterator iterator = changedFiles.begin(); iterator != changedFiles.end(); ++iterator ) {
		switch ( iterator.value()->getType() ) {
			case Repository::Git::GitType::ADDED:
				addFileToGraph( iterator.value(), rootIdentifier );
//            this->evolutionGraph->getMetaDataFromIdentifier( iterator.value()->getIdentifier() )->increaseChangedCount();
				break;
			case Repository::Git::GitType::REMOVED:
				removeFileFromGraph( iterator.value(), true );
//            this->evolutionGraph->getMetaDataFromIdentifier( iterator.value()->getIdentifier() )->decreaseChangedCount();
				break;
			case Repository::Git::GitType::MODIFIED:
				this->evolutionGraph->addChangedNodeOrEdge( iterator.value()->getIdentifier(), Repository::Git::GitType::MODIFIED );
				this->evolutionGraph->getMetaDataFromIdentifier( iterator.value()->getIdentifier() )->setChangedVersion( this->currentGraph->getCurrentVersion() );
//            this->evolutionGraph->getMetaDataFromIdentifier( iterator.value()->getIdentifier() )->increaseChangedCount();
				break;
			case Repository::Git::GitType::NONE:
				//do nothing
				break;
			default:
				qDebug() << "CHYBNY TYP PRE SUBOR" << iterator.value()->getIdentifier();
		}

		// Spracuj funkcie, ktore sa zmenili v danom subore
		processFunctionsFromFile( iterator.value(), true );
	}

	// Aktualizuj ID uzlov v grafe na aktualne hodnoty
	updateCurrentGraphNodesId();

	// Znovu nacitaj reprezentaciu uzlov v grafe pre zvolenu vizualizaciu
	if ( this->showLuaStats != this->CHANGES ) {
		reloadNodeRepresentation( this->showLuaStats );
	}
	// Spracuj vizualizaciu zmenenych uzlov v grafe
	processChangedNodesAndEdges();

	if ( this->showLuaStats == this->CHANGES ) {
		if ( this->filterAuthor == "All" ) {
			filterVisualizationByAuthor( this->filterAuthor );
		}

		if ( this->filterAuthor == "Authors" ) {
			reloadNodeRepresentation( this->CHANGES );
			filterVisualizationByAuthor( this->filterFile );
		}

		if ( this->filterAuthor == "Structure" ) {
			reloadNodeRepresentation( this->CHANGES );
			filterVisualizationByStructure( this->filterFile );
		}
	}
}

bool Repository::Git::GitLuaGraphVisualizer::addFileToGraph( Repository::Git::GitFile* file, QString rootIdentifier )
{
	// Rozdel cestu k suboru cez znak "/"
	QStringList list = file->getFilepath().split( "/" );

	// Ziskaj identifikator a jeho uzol z grafu
	QString identifier = rootIdentifier;
	osg::ref_ptr<Data::Node> node = this->currentGraph->findNodeByLuaIdentifier( identifier );

	// Nahrad jednotlive fragmenty cesty k suboru za ich relativne cesty
	QString string = "";
	for ( int i = 0; i < list.size(); i++ ) {
		string += list.at( i );
		list.replace( i, string );
		string += "/";
	}

	// Zvysime pocet modifikacii v subore
//    this->evolutionGraph->getMetaDataFromIdentifier( file->getIdentifier() )->increaseChangedCount();

	// Pre kazdu relativnu cestu z cesty k suboru vytvor identifikator suboru/adresara a pridaj ho do grafu, ak neexistuje
	for ( int i = 0; i < list.size(); i++ ) {

		// Ziskame identifikator subor/adresara, pricom o subore hovorime iba pri poslednom prvku zoznamu
		QString newIdentifier = "";
		if ( i < list.size() - 1 ) {
			newIdentifier = "directory;" + list.at( i );
		}
		else {
			newIdentifier = "file;" + list.at( i );
		}

		// Ziskame lua uzol a uzol z grafu, ak existuju
		Lua::LuaNode* luaNode = this->luaGraph->findNodeByLuaIdentifier( newIdentifier );
		osg::ref_ptr<Data::Node> newNode = this->currentGraph->findNodeByLuaIdentifier( newIdentifier );

		// Ak neexistuje uzol v grafe, tak ho vytvorime,
		// inak zresetujeme jeho typ a odstranime ho zo zoznamu zmenenych uzlov a hran -> tato situacia nastane pri vizualizacii odstranenych uzlov, kedy pocas zobrazenia odstraneneho
		// uzla sa znovu prida dany uzol, tz. uzol existuje v grafe a len mu zmenime vizual a zabezpecime, aby nebol odstraneny podla predchadzajuceho pravidla
		if ( !newNode ) {
			// Ak existuje prislusny lua uzol, tak ID uzla v grafe bude totozny s lua uzlom,
			// inak nastavime genericke ID
			if ( luaNode ) {
				newNode = this->currentGraph->addNode( luaNode->getId(), luaNode->getLabel(), this->currentGraph->getTypesByName( "node" ).at( 0 ) );
			}
			else {
				newNode = this->currentGraph->addNode( list.at( i ), this->currentGraph->getTypesByName( "node" ).at( 0 ) );
			}

			// Ulozime unique identifikator do uzla a nastavime prvy vyskyt v grafe
			newNode->setLuaIdentifier( newIdentifier );
			this->evolutionGraph->getMetaDataFromIdentifier( newIdentifier )->setFirstOccurence( this->currentGraph->getCurrentVersion() );
		}
		else {
			// Zresetujeme typ uzla a odstranine ho zo zoznamu zmenenych uzlov a hran
			newNode->setType( this->currentGraph->getTypesByName( "node" ).at( 0 ) );
			this->evolutionGraph->removeChangedNodeOrEdge( newIdentifier );
		}

		// Evidujeme zmenu pre dany uzol a nastavime typ zmeny za ADDED
		this->evolutionGraph->addChangedNodeOrEdge( newIdentifier, Repository::Git::GitType::ADDED );
		// Nastavime do metadat verziu zmeny
		this->evolutionGraph->getMetaDataFromIdentifier( newIdentifier )->setChangedVersion( this->currentGraph->getCurrentVersion() );

		// Ak lua uzol existuje, tak uzol mapujeme na lua ID,
		// inak mapujeme na genericke ID
		if ( luaNode ) {
			this->evolutionGraph->getMetaDataFromIdentifier( newIdentifier )->setLuaMapping( luaNode->getId() );
		}
		else {
			this->evolutionGraph->getMetaDataFromIdentifier( newIdentifier )->setLuaMapping( newNode->getId() );
		}

		// Vytvorime identifikator hrany
		QString edgeIdentifier =  identifier + "+" + newIdentifier;

		// Ziskame lua hrany a hrany z grafu, ak existuju
		Lua::LuaEdge* luaEdge = this->luaGraph->findEdgeByLuaIdentifier( edgeIdentifier );
		osg::ref_ptr<Data::Edge> newEdge = this->currentGraph->findEdgeByLuaIdentifier( edgeIdentifier );

		// Ak hrana v grafe neexistuje, tak ju vytvorime,
		// v opacnom pripade, tak isto ako u uzla danu hranu vymazeme zo zoznamu zmenenych hran a uzlov
		if ( !newEdge ) {
			// Ak existije lua hrana, tak nastavime ID hrany podla lua hrany,
			// v opacnom pripade generujeme ID
			if ( luaEdge ) {
				newEdge = this->currentGraph->addEdge( luaEdge->getId(), luaEdge->getLabel(), node, newNode, this->currentGraph->getTypesByName( "edge" ).at( 0 ), false );
			}
			else {
				newEdge = this->currentGraph->addEdge( edgeIdentifier, node, newNode, this->currentGraph->getTypesByName( "edge" ).at( 0 ), false );
			}

			// Nastavime hrane v grafe unique identifikator, pridelime kameru a nastavime prvy vyskyt v grafe
			newEdge->setLuaIdentifier( edgeIdentifier );
			newEdge->setCamera( this->camera );
			this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setFirstOccurence( this->currentGraph->getCurrentVersion() );
		}
		else {
			// Vymazeme hranu zo zoznamu zmenenych uzlov a hran
			this->evolutionGraph->removeChangedNodeOrEdge( edgeIdentifier );

		}

		// Evidujeme zmeny pre danu hranu a nastavime typ zmeny na ADDED
		this->evolutionGraph->addChangedNodeOrEdge( edgeIdentifier, Repository::Git::GitType::ADDED );
		// Nastavime do metadat verziu zmeny
		this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setChangedVersion( this->currentGraph->getCurrentVersion() );

		// Ak existuje lua hrana nastavime ID podla lua hrany,
		// v opacnom pripade zvolime genericke id
		if ( luaEdge ) {
			this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setLuaMapping( luaEdge->getId() );
		}
		else {
			this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setLuaMapping( newEdge->getId() );
		}

		// Zamenime ID a uzly pre dalsiu iteraciu
		identifier = newIdentifier;
		node = newNode;

	}

	return true;
}

bool Repository::Git::GitLuaGraphVisualizer::processFunctionsFromFile( Repository::Git::GitFile* file, bool next )
{

	// Spracujeme funkcie, ktore obsahuje dany subor podla ich typu a sposobu prechodu
	for ( QMap<QString, Repository::Git::GitFunction*>::iterator iterator =  file->getGitFunctions()->begin(); iterator != file->getGitFunctions()->end(); ++iterator ) {
		// Ziskame funkciu zo suboru
		Repository::Git::GitFunction* function = iterator.value();

		switch ( function->getType() ) {
			case Repository::Git::GitType::ADDED :
				if ( next ) {
					addFunctionToGraph( function, file->getIdentifier() );
					this->evolutionGraph->getMetaDataFromIdentifier( function->getIdentifier() )->increaseChangedCount();
				}
				else {
					removeFunctionFromGraph( function, file->getIdentifier(), next );
					this->evolutionGraph->getMetaDataFromIdentifier( function->getIdentifier() )->decreaseChangedCount();
				}
				break;
			case Repository::Git::GitType::REMOVED :
				if ( next ) {
					removeFunctionFromGraph( function, file->getIdentifier(), next );
					this->evolutionGraph->getMetaDataFromIdentifier( function->getIdentifier() )->decreaseChangedCount();
				}
				else {
					addFunctionToGraph( function, file->getIdentifier() );
					this->evolutionGraph->getMetaDataFromIdentifier( function->getIdentifier() )->increaseChangedCount();
				}
				break;
			case Repository::Git::GitType::MODIFIED :
				this->evolutionGraph->addChangedNodeOrEdge( function->getIdentifier(), Repository::Git::GitType::MODIFIED );
				this->evolutionGraph->getMetaDataFromIdentifier( function->getIdentifier() )->setChangedVersion( this->currentGraph->getCurrentVersion() );
				this->evolutionGraph->getMetaDataFromIdentifier( function->getIdentifier() )->increaseChangedCount();
				break;
			case Repository::Git::GitType::NONE :
				//do nothing
				break;
			default:
				qDebug() << "CHYBA V" << file->getIdentifier() << "->" << function->getIdentifier();
		}

		// Pre kazdu funkciu je potrebne prejst aj jej volania
		for ( QMap<QString, Repository::Git::GitFunction*>::iterator iter =  function->getFunctionCallers()->begin(); iter != function->getFunctionCallers()->end(); ++iter ) {
			// Ziskame volanu funkciu
			Repository::Git::GitFunction* innerFunction = iter.value();

			switch ( innerFunction->getType() ) {
				case Repository::Git::GitType::ADDED :

					if ( next ) {
						addFunctionToGraph( innerFunction, function->getIdentifier() );
						this->evolutionGraph->getMetaDataFromIdentifier( innerFunction->getIdentifier() )->increaseChangedCount();
					}
					else {
						removeFunctionFromGraph( innerFunction, function->getIdentifier(), next );
						this->evolutionGraph->getMetaDataFromIdentifier( innerFunction->getIdentifier() )->decreaseChangedCount();
					}
					break;
				case Repository::Git::GitType::REMOVED :

					if ( next ) {
						removeFunctionFromGraph( innerFunction, function->getIdentifier(), next );
						this->evolutionGraph->getMetaDataFromIdentifier( innerFunction->getIdentifier() )->decreaseChangedCount();
					}
					else {
						addFunctionToGraph( innerFunction, function->getIdentifier() );
						this->evolutionGraph->getMetaDataFromIdentifier( innerFunction->getIdentifier() )->increaseChangedCount();
					}
					break;
				case Repository::Git::GitType::MODIFIED :
					this->evolutionGraph->addChangedNodeOrEdge( innerFunction->getIdentifier(), Repository::Git::GitType::MODIFIED );
					this->evolutionGraph->getMetaDataFromIdentifier( innerFunction->getIdentifier() )->setChangedVersion( this->currentGraph->getCurrentVersion() );
					this->evolutionGraph->getMetaDataFromIdentifier( innerFunction->getIdentifier() )->increaseChangedCount();
					break;
				case Repository::Git::GitType::NONE :
					//do nothing
					break;
				default:
					qDebug() << "CHYBA V" << file->getIdentifier() << "->" << function->getIdentifier() << "->"  << innerFunction->getIdentifier();
			}
		}
	}

	return true;
}

bool Repository::Git::GitLuaGraphVisualizer::addFunctionToGraph( Repository::Git::GitFunction* function, QString masterIdentifier )
{
	// Ziskame uzol, ku ktoremu funkcia patri
	osg::ref_ptr<Data::Node> masterNode = this->currentGraph->findNodeByLuaIdentifier( masterIdentifier );

	// Ziskame Lua uzol a uzol v grafu, ak existuju
	Lua::LuaNode* luaNode = this->luaGraph->findNodeByLuaIdentifier( function->getIdentifier() );
	osg::ref_ptr<Data::Node> node = this->currentGraph->findNodeByLuaIdentifier( function->getIdentifier() );

	// Ak uzol v grafe neexistuje, tak ho vytvorime,
	// v opacnom pripade, zresetujeme uzol do povodnej podoby a odstranimeho zo zoznamu zmenenych uzlov a hran
	if ( !node ) {
		// Ak Lua uzol neexistuje, musela nastat chyba, pretoze o funkciach vieme, len z lua uzlov(lua grafu)
		if ( !luaNode ) {
			qDebug() << "id" << function->getId() << function->getIdentifier() << function->getTypeAsString() << function->getFunctionTypeAsString();
			return false;
		}

		// Vytvorime uzol v grafe, nastavime mu unique identifikator a pridame do metadat info o prvom vyskyte v grafe
		node =  this->currentGraph->addNode( luaNode->getId(), luaNode->getLabel(), this->currentGraph->getTypesByName( "node" ).at( 0 ) );
		node->setLuaIdentifier( function->getIdentifier() );
		this->evolutionGraph->getMetaDataFromIdentifier( function->getIdentifier() )->setFirstOccurence( this->currentGraph->getCurrentVersion() );

		// Ak ide o lokalnu funkciu, tak ju pomocou meta hrany pritiahnem k meta uzlu pre lokalne funkcie
		if ( !QString::compare( QString::fromStdString( luaNode->getParams().getValue()["type"].asString() ), "function" ) ) {
			QString metaEdge = "meta+" + luaNode->getIdentifier();
			osg::ref_ptr<Data::Edge> metaLink = this->currentGraph->addEdge( metaEdge, node, this->currentGraph->getMetaNodes()->value( std::numeric_limits<qlonglong>::max() - 1 ), this->currentGraph->getEdgeMetaType(), false );
			metaLink->setLuaIdentifier( metaEdge );
			metaLink->setEdgeColor( osg::Vec4( 0, 0, 0, 0 ) );
			metaLink->setInvisible( true );
			metaLink->setEdgeStrength( 0.1f );
		}
	}
	else {
		// Zresetujeme uzol do povodnej podoby a odstranime ho zo zoznamu zmeneych uzlov a hran
		node->setType( this->currentGraph->getTypesByName( "node" ).at( 0 ) );
		this->evolutionGraph->removeChangedNodeOrEdge( function->getIdentifier() );
	}

	// Pridame do zoznamu zmenenych uzlov a hran s typom ADDED
	this->evolutionGraph->addChangedNodeOrEdge( luaNode->getIdentifier(), Repository::Git::GitType::ADDED );
	// Ulozime do metadat verziu zmeny
	this->evolutionGraph->getMetaDataFromIdentifier( luaNode->getIdentifier() )->setChangedVersion( this->currentGraph->getCurrentVersion() );
	// Nastavime mapovanie ID uzla na Lua ID
	this->evolutionGraph->getMetaDataFromIdentifier( luaNode->getIdentifier() )->setLuaMapping( luaNode->getId() );
	// Zvysime pocet modifikacii funkcie
//    this->evolutionGraph->getMetaDataFromIdentifier( luaNode->getIdentifier() )->increaseChangedCount();

	// Vytvorime identifikator hrany
	QString edgeIdentifier = masterIdentifier + "+" + function->getIdentifier();

	// Ziskame Lua hranu a hranu z grafu, ak existuju
	Lua::LuaEdge* luaEdge = this->luaGraph->findEdgeByLuaIdentifier( edgeIdentifier );
	osg::ref_ptr<Data::Edge> edge = this->currentGraph->findEdgeByLuaIdentifier( edgeIdentifier );

	// Ak hrana v grafe neexistuje, tak ju vytvorime
	// v opacnom pripade, ju vymazeme zo zoznamu zmenenych uzlov a hran
	if ( !edge ) {
		// Ziskame incidence danej hrany
		Lua::LuaIncidence* incidence = this->luaGraph->getIncidences()->value( luaEdge->getIncidences().at( 0 ) );

		// Ak je incidence oznaceny ako orientovany, tak zistime orientaciu hrany a vytvorime hrany,
		// v opacnom pripade vytvorime neorientovanu hranu
		if ( incidence->getOriented() ) {
			// Ak je incidence nastaveny ako outGoing, tak hrana ide z uzla funkcie do master uzla,
			// v opacnom pripade z master uzla do uzla funkcie
			if ( incidence->getOutGoing() ) {
				edge = this->currentGraph->addEdge( luaEdge->getId(), luaEdge->getLabel(), node, masterNode, this->currentGraph->getTypesByName( "edge" ).at( 0 ), true );
			}
			else {
				edge = this->currentGraph->addEdge( luaEdge->getId(), luaEdge->getLabel(), masterNode, node, this->currentGraph->getTypesByName( "edge" ).at( 0 ), true );
			}
		}
		else {
			// Vytvorime neorientovanu hranu
			edge = this->currentGraph->addEdge( luaEdge->getId(), luaEdge->getLabel(), masterNode, node, this->currentGraph->getTypesByName( "edge" ).at( 0 ), false );
		}

		// Nastavime unique identifikator hrany, kameru a do metadat ulozime prvy vyskyt v grafe
		edge->setLuaIdentifier( edgeIdentifier );
		edge->setCamera( this->camera );
		this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setFirstOccurence( this->currentGraph->getCurrentVersion() );
	}
	else {
		// Odstranime hranu zo zoznamu zmenenych uzlov a hran
		this->evolutionGraph->removeChangedNodeOrEdge( edgeIdentifier );
	}

	// Pridame hranu do zoznamu zmenenych uzlov a hran s typom ADDED
	this->evolutionGraph->addChangedNodeOrEdge( edgeIdentifier, Repository::Git::GitType::ADDED );
	// Nastavime do metadat verziu zmeny
	this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setChangedVersion( this->currentGraph->getCurrentVersion() );
	// Nastavime mapovanie ID uzla na Lua ID
	this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setLuaMapping( luaEdge->getId() );

	// Ak ide o globalnu funkciu a sucasne obsahuje modul, tak pridame modul do grafu
	if ( function->getFunctionType() == Repository::Git::GitFunctionType::GLOBALFUNCTION && function->getModule() != "" ) {
		addModuleFromGlobalFunction( function );
	}


	return true;
}

bool Repository::Git::GitLuaGraphVisualizer::addModuleFromGlobalFunction( Repository::Git::GitFunction* function )
{
	// Ziskame uzol z grafu pre funkciu
	osg::ref_ptr<Data::Node> functionNode = this->currentGraph->findNodeByLuaIdentifier( function->getIdentifier() );

	// Ak funkcia neexistuje, ide o chybu a skoncim spracovani
	if ( !functionNode ) {
		qDebug() << "AddModuleFromGlobalFunction neexistuje node pre global function";
		return false;
	}

	// Vytvorime identifikator modulu
	QString moduleIdentifier =  "globalModule;" + function->getModule();

	// Ziskame Lua uzol a uzol z grafu, ak existuju
	Lua::LuaNode* luaModuleNode = this->luaGraph->findNodeByLuaIdentifier( moduleIdentifier );
	osg::ref_ptr<Data::Node> moduleNode = this->currentGraph->findNodeByLuaIdentifier( moduleIdentifier );

	// Ak uzol v grafe neexistuje, tak ho vytvorim,
	// inak zresetujem typ a odstranim ho zo zoznamu zmenenych uzlov a hran
	if ( !moduleNode ) {
		// Vytvorim uzol v grafe
		moduleNode = this->currentGraph->addNode( luaModuleNode->getId(), luaModuleNode->getLabel(), this->currentGraph->getTypesByName( "node" ).at( 0 ) );

		// Nastavime unique identifikator uzla a jeho prvy vyskyt v grafe
		moduleNode->setLuaIdentifier( moduleIdentifier );
		this->evolutionGraph->getMetaDataFromIdentifier( moduleIdentifier )->setFirstOccurence( this->currentGraph->getCurrentVersion() );
	}
	else {
		// Zresetujeme typ a odstranime uzol zo zoznamu zmenenych uzlov a hran
		moduleNode->setType( this->currentGraph->getTypesByName( "node" ).at( 0 ) );
		this->evolutionGraph->removeChangedNodeOrEdge( moduleIdentifier );
	}

	// Pridame uzol do zoznamu zmenenych uzlov a hran s typom ADDED
	this->evolutionGraph->addChangedNodeOrEdge( luaModuleNode->getIdentifier(), Repository::Git::GitType::ADDED );
	// Nastavime do metadat verziu zmeny
	this->evolutionGraph->getMetaDataFromIdentifier( luaModuleNode->getIdentifier() )->setChangedVersion( this->currentGraph->getCurrentVersion() );
	// Nastavime mapovanie ID uzla na Lua ID
	this->evolutionGraph->getMetaDataFromIdentifier( luaModuleNode->getIdentifier() )->setLuaMapping( luaModuleNode->getId() );

	// Vytvorime identifikator hrany
	QString edgeIdentifier =  function->getIdentifier() + "+" + moduleNode->getLuaIdentifier();

	// Ziskame Lua hranu a hranu v grafe, ak existuju
	Lua::LuaEdge* luaEdge = this->luaGraph->findEdgeByLuaIdentifier( edgeIdentifier );
	osg::ref_ptr<Data::Edge> edge = this->currentGraph->findEdgeByLuaIdentifier( edgeIdentifier );

	// Ak hrana v grafe neexistuje, tak ju vytvorime
	// v opacnom pripade, ju odstranime zo zoznamu zmenenych uzlov a hran
	if ( !edge ) {
		// Vytvorime neorientovanu hranu v grafe
		edge = this->currentGraph->addEdge( luaEdge->getId(), luaEdge->getLabel(), functionNode, moduleNode, this->currentGraph->getTypesByName( "edge" ).at( 0 ), false );

		// Nastavime unique identifikator hrany, pridelime kameru a nastavime prvy vyskyt hrany v grafe
		edge->setLuaIdentifier( edgeIdentifier );
		edge->setCamera( this->camera );
		this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setFirstOccurence( this->currentGraph->getCurrentVersion() );
	}
	else {
		// Odstranime uzol zo zoznamu zmenenych uzlov a hran
		this->evolutionGraph->removeChangedNodeOrEdge( edgeIdentifier );
	}

	// Pridame hranu do zoznamu zmenenych uzlov a hran s typom ADDED
	this->evolutionGraph->addChangedNodeOrEdge( edgeIdentifier, Repository::Git::GitType::ADDED );
	// Nastavime do metadat verziu zmeny
	this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setChangedVersion( this->currentGraph->getCurrentVersion() );
	// Namapujeme ID uzla na Lua ID
	this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setLuaMapping( luaEdge->getId() );

	return true;
}

bool Repository::Git::GitLuaGraphVisualizer::removeFileFromGraph( Repository::Git::GitFile* file, bool next )
{
	// Rozdelime cestu k suboru pomocou "/"
	QStringList list = file->getFilepath().split( "/" );

	// Ziskame jednotlive relativne cesty k suboru
	QString string = "";
	for ( int i = 0; i < list.size(); i++ ) {
		string += list.at( i );
		list.replace( i, string );
		string += "/";
	}

	// Spracujeme kazdu relativnu cestu k suboru
	for ( int i = 0; i < list.size(); i++ ) {
		// Ziskame identifikator
		QString newIdentifier = "";
		if ( i < list.size() - 1 ) {
			newIdentifier = "directory;" + list.at( i );
		}
		else {
			newIdentifier = "file;" + list.at( i );
		}

		// Ziskame znizeni vyskyt daneho uzla
		int occurence = this->evolutionGraph->getMetaDataFromIdentifier( newIdentifier )->decreaseOccurence();

		// Ak je vyskyt mensi alebo rovny 0, tak ho pridame do zoznamu zmenenych uzlov a hran
		if ( occurence <= 0 ) {

			// Ak ide o zmenu verzie dopredu, tak nastavime verziu zmeny na aktualnu verziu
			// v opacnom pripade, nastavime verziu zmeny na -1, co umozni okamzite vymazanie z grafu
			if ( next ) {
				this->evolutionGraph->getMetaDataFromIdentifier( newIdentifier )->setChangedVersion( this->currentGraph->getCurrentVersion() );
				this->evolutionGraph->addChangedNodeOrEdge( newIdentifier, Repository::Git::GitType::REMOVED );
			}
			else {
				this->evolutionGraph->getMetaDataFromIdentifier( newIdentifier )->clearChangedVersion();
				this->evolutionGraph->addChangedNodeOrEdge( newIdentifier, Repository::Git::GitType::REMOVED );
			}
		}
	}

	return true;
}

bool Repository::Git::GitLuaGraphVisualizer::removeFunctionFromGraph( Repository::Git::GitFunction* function, QString masterIdentifier, bool next )
{
	// Ak ide o globalnu funkciu a funkcia obsahuje modul, tak sa pokusime vymazat modul
	if ( function->getFunctionType() == Repository::Git::GitFunctionType::GLOBALFUNCTION && function->getModule() != "" ) {
		removeModuleFromGlobalFunction( function, next );
	}

	// Ak ide o zmeny verzie dopredu, tak vytvorime identifikator hrany, nastavime verziu zmeny a typ REMOVED
	if ( next ) {
		QString edgeIdentifier = masterIdentifier + "+" + function->getIdentifier();
		this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setChangedVersion( this->currentGraph->getCurrentVersion() );
		this->evolutionGraph->addChangedNodeOrEdge( edgeIdentifier, Repository::Git::GitType::REMOVED );
	}

	// Ziskame znizeny vyskyt uzla
	int occurence = this->evolutionGraph->getMetaDataFromIdentifier( function->getIdentifier() )->decreaseOccurence();

	// Ak je vyskyt mensi alebo rovny 0, tak ho pridame do zoznamu zmenenych uzlov a hran
	if ( occurence <= 0 ) {

		// Ak ide o zmenu verzie dopredu, tak nastavime verziu zmeny na aktualnu verziu,
		// v opacnom pripade, nastavime verziu zmeny na -1, co umozni okamzite vymazanie z grafu
		if ( next ) {
			this->evolutionGraph->getMetaDataFromIdentifier( function->getIdentifier() )->setChangedVersion( this->currentGraph->getCurrentVersion() );
			this->evolutionGraph->addChangedNodeOrEdge( function->getIdentifier(), Repository::Git::GitType::REMOVED );
		}
		else {
			this->evolutionGraph->getMetaDataFromIdentifier( function->getIdentifier() )->clearChangedVersion();
			this->evolutionGraph->addChangedNodeOrEdge( function->getIdentifier(), Repository::Git::GitType::REMOVED );
		}
	}


	return true;
}

bool Repository::Git::GitLuaGraphVisualizer::removeModuleFromGlobalFunction( Repository::Git::GitFunction* function, bool next )
{
	// Vytvorime identifikator modulu
	QString moduleIdentifier =  "globalModule;" + function->getModule();

	// Ak ide o zmenu verzie dopredu, tak vytvorime identifikator hrany, natavime verziu zmeny a typ REMOVED
	if ( next ) {
		QString edgeIdentifier = function->getIdentifier() + "+" + moduleIdentifier;
		this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setChangedVersion( this->currentGraph->getCurrentVersion() );
		this->evolutionGraph->addChangedNodeOrEdge( edgeIdentifier, Repository::Git::GitType::REMOVED );
	}

	// Ziskame znizeny vyskyt uzla
	int occurence = this->evolutionGraph->getMetaDataFromIdentifier( moduleIdentifier )->decreaseOccurence();

	// Ak je vyskyt mensi alebo rovny 0, tak ho pridame do zoznamu zmenenych uzlov a hran
	if ( occurence <= 0 ) {
		// Ak ide o zmenu verzie dopredu, tak nastavime verziu zmeny na aktualnu verziu,
		// v opacnom pripade, nastavime verziu zmeny na -1, co umozni okamzite vymazanie z grafu
		if ( next ) {
			this->evolutionGraph->getMetaDataFromIdentifier( moduleIdentifier )->setChangedVersion( this->currentGraph->getCurrentVersion() );
			this->evolutionGraph->addChangedNodeOrEdge( moduleIdentifier, Repository::Git::GitType::REMOVED );
		}
		else {
			this->evolutionGraph->getMetaDataFromIdentifier( moduleIdentifier )->clearChangedVersion();
			this->evolutionGraph->addChangedNodeOrEdge( moduleIdentifier, Repository::Git::GitType::REMOVED );
		}
	}

	return true;
}

void Repository::Git::GitLuaGraphVisualizer::processChangedNodesAndEdges()
{

	// Zoznam uzlov a hran, ktore po spracovani odstranime zo zoznamu zmenenych uzlov a hran
	QList<QString> removedNodesAndEdges = QList<QString>();

	// Kazdy uzol alebo hranu zo zoznamu zmenenych uzlov a hran spracujeme
	for ( QMap<QString, Repository::Git::GitType>::iterator iterator = this->evolutionGraph->getChangedNodesAndEdge()->begin(); iterator != this->evolutionGraph->getChangedNodesAndEdge()->end(); ++iterator ) {

		// Ziskame identifikator
		QString identifier = iterator.key();

		// Inicializujeme pomocne premenne
		bool passedLifespan = false;
		int difference;


		switch ( iterator.value() ) {
			case Repository::Git::GitType::ADDED:

				// Ak ide o identifikator uzla, tak sa index rovna -1, inak ide o hranu
				// v opacnom pripade ide o hranu
				if ( identifier.indexOf( "+" ) == -1 ) {

					// Ziskame uzol z grafu
					osg::ref_ptr<Data::Node> node = this->currentGraph->findNodeByLuaIdentifier( identifier );

					// Ziskame Lua uzol
					Lua::LuaNode* luaNode = this->luaGraph->findNodeByLuaIdentifier( identifier );

					// Ak sa nemaju zobrazovat Lua udaje v grafe, upravim zobrazenie uzla
					// v opacnom pripade zobrazime Lua udaje
					switch ( this->showLuaStats ) {
						case LUA_STATS:

							// Ak Lua uzol existuje, tak zmenime zobrazenie uzla na Lua udaje
							if ( luaNode ) {
								setNodeParams( node, luaNode, osg::Vec4f( 1, 1, 1, 1 ), 8 );
							}
							break;
						case DIFFERENCE_MAP:
							// Ak sa zmenena verzia rovna sucasnej verzii, tak zmenim tvar node na +,
							// inak nastavime zobrazenie na default tvar a vymazeme uzol zo zoznamu zmenenych uzlov a hran
							if ( this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getChangedVersion() == this->currentGraph->getCurrentVersion() ) {

								// Ak prvy vyskyt uzla je rovny aktualnej verzii, tak zmenime tvar uzla na +,
								// v opacnom pripade, nechame defaul zobrazenie uzla
								if ( this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getFirstOccurence() == this->currentGraph->getCurrentVersion() ) {

									// Nastavime zobrazenie uzla na tvar +
									node->setType( this->currentGraph->getTypesByName( "addedNode" ).at( 0 ) );
									node->setColor( osg::Vec4f( 0, 1, 0, 1 ) );
								}
								else {

									// Nastavime zobrazenie uzla na default tvar
									node->setType( this->currentGraph->getTypesByName( "clearNode" ).at( 0 ) );
									node->setColor( osg::Vec4f( 1, 1, 1, 1 ) );
								}
							}
							else {

								// Nastavime zobrazenie uzla na default tvar a vymazeme uzol zo zoznamu zmenenych uzlov a hran
								node->setType( this->currentGraph->getTypesByName( "clearNode" ).at( 0 ) );
								node->setColor( osg::Vec4f( 1, 1, 1, 1 ) );
								removedNodesAndEdges.append( identifier );
							}

							// Zresetujeme vizualizaciu uzla
							node->reloadConfig();
							node->showLabel( true, false );
							break;
						case CHANGES:
//                    node->setType( this->currentGraph->getTypesByName( "clearNode" ).at( 0 ) );
//                    node->setColor( osg::Vec4f( 1, 0, 0, 1 ) );
							break;
						default:
							qDebug() << "PROCESUJEM NEEXISTUJUCI STAV";
					}
				}
				else {

					// Ziskame hranu z grafu, ak existuje
					osg::ref_ptr<Data::Edge> edge = this->currentGraph->findEdgeByLuaIdentifier( identifier );

					switch ( this->showLuaStats ) {
						case LUA_STATS:
							break;
						case DIFFERENCE_MAP:
							// Ak sa verzia zmeny a aktualna verzia zhoduju, tak zafarbime hranu na zeleno,
							// v opacnom pripade zobrazime bielu hranu a vymazeme hranu zo zoznamu zmenenych uzlov a hran
							if ( this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getChangedVersion() == this->currentGraph->getCurrentVersion() ) {
								edge->setEdgeColor( osg::Vec4f( 0, 1, 0, 1 ) );
							}
							else {
//								edge->setEdgeColor( osg::Vec4f( 1, 1, 1, 1 ) );
								removedNodesAndEdges.append( identifier );
							}
							break;
						case CHANGES:
//                    edge->setEdgeColor( osg::Vec4f( 1, 1, 1, 1 ) );
							break;
						default:
							qDebug() << "PROCESUJEM NEEXISTUJUCI STAV";
							break;
					}
				}
				break;
			case Repository::Git::GitType::MODIFIED:

				// Ak ide o identifikator uzla, tak sa index rovna -1,
				// zmenu hrany nevieme zistit
				if ( identifier.indexOf( "+" ) == -1 ) {

					// Ziskame uzol z grafu, ak existuje
					osg::ref_ptr<Data::Node> node =  this->currentGraph->findNodeByLuaIdentifier( identifier );

					// Ak uzol existuje a spracujeme volbu zobrazenia
					if ( node ) {
						// Ziskame Lua uzol, ak existuje
						Lua::LuaNode* luaNode = this->luaGraph->findNodeByLuaIdentifier( identifier );

						switch ( showLuaStats ) {
							case LUA_STATS:

								// Ak uzol existuje, tak zobrazime Lua udaje
								if ( luaNode ) {
									setNodeParams( node, luaNode, osg::Vec4f( 1, 1, 1, 1 ), 8 );
								}
								break;
							case DIFFERENCE_MAP:
								// Ak sa verzia zmeny zhoduje s aktualnou verziou, tak zobrazime uzol nazlto
								// v opacnom pripade, zobrazime biely uzol a vymazeme uzol zo zoznamu zmenenych uzlov a hran
								if ( this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getChangedVersion() == this->currentGraph->getCurrentVersion() ) {
									node->setType( this->currentGraph->getTypesByName( "modifiedNode" ).at( 0 ) );
									node->setColor( osg::Vec4f( 1, 1, 0, 1 ) );
								}
								else {
									node->setType( this->currentGraph->getTypesByName( "clearNode" ).at( 0 ) );
									node->setColor( osg::Vec4f( 1, 1, 1, 1 ) );
									removedNodesAndEdges.append( identifier );
								}

								// Zresetujeme uzol
								node->reloadConfig();
								node->showLabel( true, false );

								break;
							case CHANGES:
//                        node->setType( this->currentGraph->getTypesByName( "clearNode" ).at( 0 ) );
//                        node->setColor( osg::Vec4f( 1, 0, 1, 0 ) );
								break;
							default:
								qDebug() << "Unhandled LuaStat";
						}
					}
				}
				break;
			case Repository::Git::GitType::REMOVED:

				// Ziskame rozdiel aktualnej verzie a verzie zmeny
				difference = this->currentGraph->getCurrentVersion() - this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getChangedVersion();

				// Ak ide verzia zmeny rozna od -1, tak skontrolujeme povolenu dlzku zivota uzla s rozdielom
				// v opacnom pripade nastavime, ze uzol presiahol svoju dlzku zivota
				if ( this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getChangedVersion() != -1 ) {

					// Ak rozdiel je vacsi alebo rovny povolenej dlzke zivota uzla, tak nastavime, ze uzol presiahol svoju dlzku zivota
					if ( difference >= this->evolutionGraph->getLifespan() ) {
						passedLifespan = true;
					}
				}
				else {
					passedLifespan = true;
				}


				// Ak ide o identifikator uzla, tak sa index rovna -1,
				// v opacnom pripade ide o hranu
				if ( iterator.key().indexOf( "+" ) == -1 ) {

					// Ziskame uzol z grafu, ak existuje
					osg::ref_ptr<Data::Node> node = this->currentGraph->findNodeByLuaIdentifier( iterator.key() );

					// Ak uzol existuje, skontrolujem, ci pocet vyskytov v grafe je mensi alebo rovny 0,
					// v opacnom pripade ho odstranim zo zoznamu zmenenych uzlov, lebo uz neexistuje
					if ( node ) {
						if ( this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getOccurence() <= 0 ) {

							// Ak presiahol svoju dlzku zivota, tak vymazeme uzol z grafy a aj zo zoznamu zmenenych uzlov a hran,
							// v opacnom pripade zmenime tvar na X
							if ( passedLifespan ) {
								this->currentGraph->removeNode( node );
								removedNodesAndEdges.append( identifier );
							}
							else {
								// Zmenime tvar na X
								node->setType( this->currentGraph->getTypesByName( "removedNode" ).at( 0 ) );
								node->setColor( osg::Vec4f( 1, 1, 1, 1 ) );

								// Zresetujeme uzol
								node->reloadConfig();
								node->showLabel( true, false );
							}
						}
						else {

							// Vyskyt uzlu este je kladny, tak ho vymazeme zo zoznamu zmenenych uzlov a hran
							removedNodesAndEdges.append( identifier );
						}
					}
					else {

						// Uzol uz neexistuje, odstranime ho zo zoznamu zmenenych uzlov a hran
						removedNodesAndEdges.append( identifier );
					}
				}
				else {

					// Ziskam hranu z grafu, ak existuje
					osg::ref_ptr<Data::Edge> edge = this->currentGraph->findEdgeByLuaIdentifier( identifier );

					// Ak hrana existuje, tak ju skontrolujem, ci ma este ostat v grafe,
					// v opacnom pripade, ju vymazem zo zoznamu zmenenych uzlov a hran
					if ( edge ) {

						// Ak presiahla dlzku svojho zivota, tak ju vymazeme z grafu a zo zoznamu zmenenych uzlov a hran,
						// inak zmenime jej farbu na cervenu
						if ( passedLifespan ) {
							removedNodesAndEdges.append( identifier );
							this->currentGraph->removeEdge( edge );
						}
						else {
							edge->setEdgeColor( osg::Vec4f( 1, 0, 0, 1 ) );
						}
					}
					else {

						// Hrana sa uz v grafe nevyskytuje, tak ju vymazeme zo zoznamu zmenenych uzlov a hran
						removedNodesAndEdges.append( identifier );
					}
				}
				break;
			case Repository::Git::GitType::NONE:
				//do nothing
				break;
			default:
				qDebug() << "Unknown GitType";
		}
	}

	// Kazdy zaznam odstranime zo zoznamu zmenenych uzlov a hran a nastavime verziu zmeny na -1
	foreach ( QString identifier, removedNodesAndEdges ) {
		this->evolutionGraph->removeChangedNodeOrEdge( identifier );
		this->evolutionGraph->getMetaDataFromIdentifier( identifier )->clearChangedVersion();
	}
}

void Repository::Git::GitLuaGraphVisualizer::changeNodeRepresentation()
{
	visualize( true );
}

void Repository::Git::GitLuaGraphVisualizer::reloadNodeRepresentation( int showLuaStats )
{

	// Pre kazdy uzol v grafe aktualizujeme reprezentaciu uzla
	for ( QMap<qlonglong, osg::ref_ptr<Data::Node>>::iterator iterator = this->currentGraph->getNodes()->begin(); iterator != this->currentGraph->getNodes()->end(); ++iterator ) {
		// Ziskame Lua uzol, ak existuje
		Lua::LuaNode* luaNode = this->luaGraph->getNodes()->value( iterator.value()->getId() );

		switch ( showLuaStats ) {
			case LUA_STATS:
//            qDebug() << "Setting lua_stats -" << iterator.value()->getLuaIdentifier();

				// Ak uzol existuje, zobrazime Lua udaje
				if ( luaNode ) {
					setNodeParams( iterator.value(), luaNode, osg::Vec4f( 1, 1, 1, 1 ), 8 );
				}

				iterator.value()->setInvisible( false );
				break;
			case DIFFERENCE_MAP:
				// Nastavime default farbu a zresetujeme uzol
//            qDebug() << "Setting difference_map -" << iterator.value()->getLuaIdentifier();
				iterator.value()->setType( this->currentGraph->getTypesByName( "clearNode" ).at( 0 ) );
//				iterator.value()->setColor( osg::Vec4f( 0.75, 0.75, 0.75, 1 ) );
				iterator.value()->setColor( osg::Vec4f( 1, 1, 1, 1 ) );
				iterator.value()->setInvisible( false );
				iterator.value()->reloadConfig();
				break;
			case CHANGES:
//            iterator.value()->setType( this->currentGraph->getTypesByName( "clearNode" ).at( 0 ) );
//            if( this->evolutionGraph->getMetaDataFromIdentifier( iterator.value()->getLuaIdentifier() )->getChangedCount() ) {
//                iterator.value()->setColor( osg::Vec4f( 1, 0, 0, 1 ) );
//            } else {
//                iterator.value()->setColor( osg::Vec4f( 0, 0, 1, 1 ) );
//            }
//            iterator.value()->reloadConfig();
//            qDebug() << "Setting invisible -" << iterator.value()->getLuaIdentifier();
				iterator.value()->setInvisible( true );
				iterator.value()->showLabel( false, false );
				break;
			default:
				qDebug() << "Unhandled LuaStat";
		}


	}

	// Pre kazdu hranu v grafe aktualizujeme reprezentaciu hrany
	for ( QMap<qlonglong, osg::ref_ptr<Data::Edge>>::iterator iterator = this->currentGraph->getEdges()->begin(); iterator != this->currentGraph->getEdges()->end(); ++iterator ) {
		// Ziskame Lua uzol, ak existuje
		Lua::LuaEdge* luaEdge = this->luaGraph->findEdgeByLuaIdentifier( iterator.value()->getLuaIdentifier() );

		switch ( showLuaStats ) {
			case LUA_STATS:
				// Ak uzol existuje, zobrazime Lua udaje
				if ( luaEdge ) {
					setEdgeParams( iterator.value(), luaEdge, osg::Vec4f( 1, 1, 1, 1 ) );
				}
				iterator.value()->setInvisible( false );
				break;
			case DIFFERENCE_MAP:
				// Nastavime default farbu
				iterator.value()->setInvisible( false );
//				iterator.value()->setEdgeColor( osg::Vec4f( 0.2, 0.2, 0.2, 1 ) );
				iterator.value()->setEdgeColor( osg::Vec4f( 1, 1, 1, 1 ) );
				iterator.value()->reloadColor();
				break;
			case CHANGES:
				iterator.value()->setInvisible( true );
				break;
			default:
				qDebug() << "Unhandled LuaStat";
		}
	}

	// zobraz root
	Data::Node* root = this->currentGraph->findNodeByLuaIdentifier( "directory;" + this->evolutionGraph->getFilePath() );
	if ( root ) {
		root->setInvisible( false );
	}
}

void Repository::Git::GitLuaGraphVisualizer::filterVisualizationByAuthor( QString author )
{
	qDebug() << "filterVisualizationByAuthor" << author;
	Repository::Git::GitMetrics metrics = Repository::Git::GitMetrics( this->evolutionGraph );
	QList<QString> files;
	if ( !QString::compare( "All", author ) ) {
		qDebug() << "getAllFiles";
		files = metrics.getAllFiles();
	}
	else {
		qDebug() << "getFilesFromAuthor" << author << "and position" << this->currentGraph->getCurrentVersion() + 1;
		files = metrics.getFilesFromAuthor( author, this->currentGraph->getCurrentVersion() + 1 );
	}

	qDebug() << "Metrics module returned" << files.size() << "files";

	QSet<QString> nodeIdentifiers = QSet<QString>();
	QSet<QString> edgeIdentifiers = QSet<QString>();

	foreach ( QString fileIdentifier, files ) {
//        qDebug() << fileIdentifier;
		QString identifier = fileIdentifier.replace( "file;", "" );
		QStringList list = identifier.split( "/" );

		QString previousIdentifier = "directory;" + this->evolutionGraph->getFilePath();

		identifier = "";
		for ( int i = 0; i < list.size(); i++ ) {
			identifier += list.at( i );

			QString insertIdentifier = "";
			// Vytvorime identifikator pre danu uroven cesty k suboru
			if ( i == list.size() - 1 ) {
				insertIdentifier = "file;" + identifier;
			}
			else {
				insertIdentifier = "directory;" + identifier;
			}

			// pridame identifikator suboru/adresara
			nodeIdentifiers.insert( insertIdentifier );

			// pridame identifikator hrany
			edgeIdentifiers.insert( previousIdentifier + "+" + insertIdentifier );

			// nastavime sucasny identifikator suboru/adresara ako predchadzajuci
			previousIdentifier = insertIdentifier;

			identifier += "/";

			if ( insertIdentifier.contains( "file;" ) ) {
				QList<QString> functions;
				QList<QString> functionConnectors;

				if ( this->filterFile == "All" ) {
					functions = metrics.getFunctionsFromFile( insertIdentifier );
					functionConnectors = metrics.getFunctionConnectorsFromFile( insertIdentifier );
				}
				else {
					functions = metrics.getFunctionsFromFile( insertIdentifier, this->filterFile );
					functionConnectors = metrics.getFunctionConnectorsFromFile( insertIdentifier, this->filterFile );
				}

				foreach ( QString functionIdentifier, functions ) {
					nodeIdentifiers.insert( functionIdentifier );
				}


				foreach ( QString connectorIdentifier, functionConnectors ) {
					edgeIdentifiers.insert( connectorIdentifier );
				}
			}
		}
	}

	foreach ( QString identifier, nodeIdentifiers ) {
		Data::Node* node = this->currentGraph->findNodeByLuaIdentifier( identifier );
		if ( node ) {
//            qDebug() << identifier;
			node->setInvisible( false );
			node->showLabel( true, false );
		}
		else {
			// ToDo mozno nejaky chybovy vypis :D a mozno nie
		}
	}

	foreach ( QString identifier, edgeIdentifiers ) {
		Data::Edge* edge = this->currentGraph->findEdgeByLuaIdentifier( identifier );
		if ( edge ) {
			edge->setInvisible( false );
		}
		else {
			// ToDo mozno nejaky chybovy vypis :D a mozno nie
		}
	}
}

void Repository::Git::GitLuaGraphVisualizer::filterVisualizationByStructure( QString structure )
{
	qDebug() << "filterVisualizationByStructure" << structure;
	Repository::Git::GitMetrics metrics = Repository::Git::GitMetrics( this->evolutionGraph );
	QList<QString> files = metrics.getFilesToPosition( this->currentGraph->getCurrentVersion() + 1 );

	qDebug() << "Metrics module returned" << files.size() << "files";

	QSet<QString> nodeIdentifiers = QSet<QString>();
	QSet<QString> edgeIdentifiers = QSet<QString>();

	foreach ( QString fileIdentifier, files ) {
//        qDebug() << fileIdentifier;
		QString identifier = fileIdentifier.replace( "file;", "" );
		QStringList list = identifier.split( "/" );

		QString previousIdentifier = "directory;" + this->evolutionGraph->getFilePath();

		identifier = "";
		for ( int i = 0; i < list.size(); i++ ) {
			identifier += list.at( i );

			QString insertIdentifier = "";
			// Vytvorime identifikator pre danu uroven cesty k suboru
			if ( i == list.size() - 1 ) {
				insertIdentifier = "file;" + identifier;
			}
			else {
				insertIdentifier = "directory;" + identifier;
			}

			// pridame identifikator suboru/adresara
			nodeIdentifiers.insert( insertIdentifier );

			// pridame identifikator hrany
			edgeIdentifiers.insert( previousIdentifier + "+" + insertIdentifier );

			// nastavime sucasny identifikator suboru/adresara ako predchadzajuci
			previousIdentifier = insertIdentifier;

			identifier += "/";

			if ( insertIdentifier.contains( "file;" ) && QString::compare( "Files", structure ) ) {
				QList<QString> functions;
				QList<QString> functionConnectors;

				int level = 0;

				if ( structure == "Modules" ) {
					level = 2;
				}
				else if ( structure == "Global Functions" ) {
					level = 1;
				}

				functions = metrics.getFunctionsFromFile( insertIdentifier, level );
				functionConnectors = metrics.getFunctionConnectorsFromFile( insertIdentifier, level );


				foreach ( QString functionIdentifier, functions ) {
					nodeIdentifiers.insert( functionIdentifier );
				}


				foreach ( QString connectorIdentifier, functionConnectors ) {
					edgeIdentifiers.insert( connectorIdentifier );
				}
			}
		}
	}

	foreach ( QString identifier, nodeIdentifiers ) {
		Data::Node* node = this->currentGraph->findNodeByLuaIdentifier( identifier );
		if ( node ) {
//            qDebug() << identifier;
			node->setInvisible( false );
			node->showLabel( true, false );
		}
		else {
			// ToDo mozno nejaky chybovy vypis :D a mozno nie
		}
	}

	foreach ( QString identifier, edgeIdentifiers ) {
		Data::Edge* edge = this->currentGraph->findEdgeByLuaIdentifier( identifier );
		if ( edge ) {
			edge->setInvisible( false );
		}
		else {
			// ToDo mozno nejaky chybovy vypis :D a mozno nie
		}
	}

//    qDebug() << "Pocet node" << nodeIdentifiers.size();
//    qDebug() << "Pocet edge" << edgeIdentifiers.size();
}

void Repository::Git::GitLuaGraphVisualizer::updateCurrentGraphNodesId()
{

	// Pre kazdy uzol v grafe sa pokusime aktualizovat ID uzla
	for ( QMap<qlonglong, osg::ref_ptr<Data::Node>>::iterator iterator = this->currentGraph->getNodes()->begin(); iterator != this->currentGraph->getNodes()->end(); ++iterator ) {

		// Ziskame Lua uzol, ak existuje
		Lua::LuaNode* node = this->luaGraph->findNodeByLuaIdentifier( iterator.value()->getLuaIdentifier() );

		// Ak existuje, tak zmenime Id uzla na Lua Id a namapujeme ho do metadat
		if ( node ) {
			iterator.value()->setId( node->getId() );
			this->evolutionGraph->getMetaDataFromIdentifier( node->getIdentifier() )->setLuaMapping( node->getId() );
		}
	}
}

void Repository::Git::GitLuaGraphVisualizer::setNodeParams( osg::ref_ptr<Data::Node> node, Lua::LuaGraphObject* obj, osg::Vec4f defColor, float defSize )
{

	// Nastavenie Lua udajov pre uzol a zresetovanie uzla
	node->setType( this->currentGraph->getTypesByName( "node" ).at( 0 ) );
	node->setLabelText( obj->getLabel() );
	node->Data::AbsNode::setName( obj->getLabel() );
	float r = obj->getFloatParam( "color", "R", defColor.r() );
	float g = obj->getFloatParam( "color", "G", defColor.g() );
	float b = obj->getFloatParam( "color", "B", defColor.b() );
	float a = obj->getFloatParam( "color", "A", defColor.a() );
	node->setColor( osg::Vec4( r,g,b,a ) );
	float scale = obj->getFloatParam( "size", defSize );
	node->setScale( scale );
	node->reloadConfig();
	node->showLabel( true, false );
}

void Repository::Git::GitLuaGraphVisualizer::setEdgeParams( osg::ref_ptr<Data::Edge> edge, Lua::LuaGraphObject* obj, osg::Vec4f defColor )
{

	// Nastavenie Lua udajov pre hranu
	edge->Data::AbsEdge::setName( obj->getLabel() );
	float r = obj->getFloatParam( "color", "R", defColor.r() );// * 0.6;
	float g = obj->getFloatParam( "color", "G", defColor.g() );// * 0.6;
	float b = obj->getFloatParam( "color", "B", defColor.b() );// * 0.6;
	float a = obj->getFloatParam( "color", "A", defColor.a() );
	edge->setEdgeColor( osg::Vec4( r,g,b,a ) );
	edge->setEdgeStrength( obj->getFloatParam( "edgeStrength", 1 ) );
	edge->reloadColor();
}

void Repository::Git::GitLuaGraphVisualizer::addDefaultTypes()
{
	this->currentGraph->addType( "edge" );
	this->currentGraph->addType( "node" );
}

void Repository::Git::GitLuaGraphVisualizer::addCustomTypes()
{
	QMap<QString, QString>* settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.EvolutionNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.RemoveNode" ) );
	this->currentGraph->addType( "removedNode", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.EvolutionEdgeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Edge" ) );;
	this->currentGraph->addType( "removedEdge", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "0" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "0" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.EvolutionNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.AddedNode" ) );
	this->currentGraph->addType( "addedNode", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "0" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.EvolutionNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
	this->currentGraph->addType( "modifiedNode", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
	this->currentGraph->addType( "clearNode", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
	this->currentGraph->addType( "clearNode", settings );
}
