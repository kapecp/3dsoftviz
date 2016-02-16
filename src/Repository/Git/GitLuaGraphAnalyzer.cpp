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

#include <QDebug>
#include <QList>

Repository::Git::GitLuaGraphAnalyzer::GitLuaGraphAnalyzer()
    : luaGraph( Lua::LuaGraph::getInstance() ), evolutionGraph( nullptr ), versionNumber( -1 ), functions( new QMap<QString, Repository::Git::GitFunction*>() )
{
}

Repository::Git::GitLuaGraphAnalyzer::GitLuaGraphAnalyzer( Lua::LuaGraph *luaGraph, Repository::Git::GitEvolutionGraph *evolutionGraph )
    : luaGraph( luaGraph ), evolutionGraph( evolutionGraph ), versionNumber( -1 ), functions( new QMap<QString, Repository::Git::GitFunction*>() )
{

}

Repository::Git::GitLuaGraphAnalyzer::~GitLuaGraphAnalyzer() {
    qDebug() << "Destruktor GitLuaGraphAnalyzer";
    delete functions;
}

void Repository::Git::GitLuaGraphAnalyzer::analyze() {
    Repository::Git::GitVersion* version =  this->evolutionGraph->getVersion( this->versionNumber );
    QMap<QString, Repository::Git::GitFile*> versionFiles = version->getChangedFiles();

    // Prejdem kazdy nacitani LuaNode a skontrolujem, ci sa nenachadza medzi zmenenymi subormi v danej verzii
    for( QMap<qlonglong, Lua::LuaNode*>::iterator iterator = this->luaGraph->getNodes()->begin(); iterator != this->luaGraph->getNodes()->end(); ++iterator ) {
        // Ak sa LuaNode nachadza v zmenenych suborov, tak zistim rozdiel podstromu suboru voci predchadzajucej verzii suboru
        if( versionFiles.contains( iterator.value()->getIdentifier()  ) ) {
            Lua::LuaNode* node = iterator.value();

            // Mapa na ulozenie funkcii pouzitych v danom subore
            QMap<QString, Repository::Git::GitFunction*> functions = QMap<QString, Repository::Git::GitFunction*>();

            // Git file, ktory zobrazuje sucasnu verziu suboru s jeho podstromom funkcii
            Repository::Git::GitFile* file = new Repository::Git::GitFile( node->getIdentifier(), node->getIdentifier().split(";").at( 1 ), Repository::Git::GitType::MODIFIED );

            int counter = 0;

            // prejdem vsetky incidence, ktore obsahuje LuaNode a pridame vsetky funkcie, ktore su volanie zo suboru
            foreach( qlonglong incidenceId, node->getIncidences() ) {
                Lua::LuaIncidence* incidence = this->luaGraph->getIncidences()->value( incidenceId );

                // Ak je incidence orientovany a outGoing je false, tak ide o hranu zo suboru na globalFunction
                // a pridam k danemu suboru vsetky global funkcie spolu s ich modulmi
                if( incidence->getOriented() && !incidence->getOutGoing() ) {
                    Lua::LuaEdge* edge = this->luaGraph->getEdges()->value( incidence->getEdgeNodePair().first );
                    Lua::LuaNode* pairNode = nullptr;

                    // Zistim druhy incidence, ktory obsahuje druhy LuaNode pre danu hranu
                    Lua::LuaIncidence* otherIncidence = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 0 ) );
                    if( incidence == otherIncidence ) {
                        otherIncidence = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 1 ) );
                    }

                    // LuaNode, ku ktoremu smeruje hrana zo suboru
                    pairNode = this->luaGraph->getNodes()->value( otherIncidence->getEdgeNodePair().second );

                    // Ziskam identifikator globalnej funkcie
                    QString functionIdentifier =  pairNode->getIdentifier();
/*                    QString functionIdentifier =  edge->getIdentifier().replace( "+", "" ).replace( node->getIdentifier(), "" );

                    // pri ojedinelych pripadoch sa stane pripad, kedy ide hrana do tohto isteho uzla
                    // zatial work around
                    if( functionIdentifier == "" ) {
                        functionIdentifier = node->getIdentifier();
                    }
//                    qDebug() << "Identifikator" << functionIdentifier;
*/
                    // Zistim, ci sa identifikator globalnej funkcie nachadza uz v ulozenych funkciach pre dany subor
                    Repository::Git::GitFunction* function = functions.value( functionIdentifier );

                    // Ak sa globalna funkcia este nenachadza v mape funkcii, tak ju vytvorime pre dany subor
                    if( !function ) {
                        // Inicializujem globalnu funkciu
                        function = new Repository::Git::GitFunction();

                        // Rozlozim identifikator cez oddelovac ";"
                        QStringList list = functionIdentifier.split( ";" );

                        // Ak sa pocet tokenov z identifikatora rovna 2, tak globalna funkcia nema global modul
                        // inak nenastavim modul
                        if( list.size() == 2 ) {
                            function->setName( list.at( 1 ) );
                        } else {
                            function->setName( list.at( 2 ) );
                            function->setModule( list.at( 1 ) );
                        }

                        // Typ globalnej funkcie nas nezaujima pri analyze, preto nastavim na NONE
                        function->setType( Repository::Git::GitType::NONE );
                        function->setId( pairNode->getId() );

                        // Kontrola, ze ide o globalnu funkciu, ak by nebola tak sa vypise chybova sprava
                        if( !QString::compare( QString::fromStdString( pairNode->getParams()["type"].asString() ), "globalFunction" ) ) {
                            function->setFunctionType( Repository::Git::GitFunctionType::GLOBALFUNCTION );
                        } else {
                            qDebug() << "FUNKCIA NIE JE GLOBALNA. CHYBA V ANALYZE DAT";
                        }

                        // Vytvorenu globalnu funkciu pridame do vsetkych funkcii pre dany subor
                        functions.insert( functionIdentifier, function );
                    }

                    // Globalnu funkciu pridame do zoznamu funkcii obsiahnutych v subore
                    file->addGitFunction( function );
                    counter++;

                } else {
                    // Incidence nie je orientovany alebo nie je Out going
                    // V tomto pripade ide o hranu na DIR(tato moznost nas v analyze nezaujima) alebo hrana k lokalnej funkcii
                    Lua::LuaEdge* edge = this->luaGraph->getEdges()->value( incidence->getEdgeNodePair().first );
                    Lua::LuaNode* pairNode = nullptr;

                    // Zistim druhy incidence, ktory obsahuje druhy LuaNode pre danu hranu
                    Lua::LuaIncidence* otherIncidence = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 0 ) );
                    if( incidence == otherIncidence ) {
                        otherIncidence = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 1 ) );
                    }

                    // LuaNode, ku ktoremu smeruje hrana zo suboru
                    pairNode = this->luaGraph->getNodes()->value( otherIncidence->getEdgeNodePair().second );

                    // Pokracujeme v spracovani len ak ide o hranu s lokalnou funkciou
                    if( !QString::compare( QString::fromStdString( pairNode->getParams()["type"].asString() ), "function") ) {
                        // Ziskam identifikator lokalnej funkcie
                        QString functionIdentifier = pairNode->getIdentifier();

                        /*QString functionIdentifier = edge->getIdentifier().replace( "+", "" ).replace( node->getIdentifier(), "" );

                        // pri ojedinelych pripadoch sa stane pripad, kedy ide hrana do tohto isteho uzla
                        // zatial work around
                        if( functionIdentifier == "" ) {
                            functionIdentifier = node->getIdentifier();
                        }*/

                        // Zistim, ci sa identifikator lokalnej funkcie nachadza uz v ulozenych funkciach pre dany subor
                        Repository::Git::GitFunction* function =  functions.value( functionIdentifier );

                        // Ak sa lokalna funkcia este nenachadza v mape funkcii, tak ju vytvorime pre dany subor
                        if( !function ) {
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
                            if( !QString::compare( QString::fromStdString( pairNode->getParams()["type"].asString() ), "function" ) ) {
                                function->setFunctionType( Repository::Git::GitFunctionType::LOCALFUNCTION );
                            } else {
                                qDebug() << "FUNKCIA NIE JE LOKALNA. CHYBA V ANALYZE DAT";
                            }

                            // Vytvorenu lokalnu funkciu pridame do vsetkych funkcii pre dany subor
                            functions.insert( functionIdentifier, function );
                        }

                        // Lokalnu funkciu pridame do zoznamu funkcii obsiahnutych v subore
                        file->addGitFunction( function );

                        // Kedze ide o lokalnu funkciu, tak ta moze volat dalsie funkcie (lokalne, globalne)
                        // Prejdem vsetky hrany, ktore idu z lokalnej funkcie a su orientovane a out going = false
                        foreach( qlonglong functionIncidenceId, pairNode->getIncidences() ) {
                            Lua::LuaIncidence* functionIncidence = this->luaGraph->getIncidences()->value( functionIncidenceId );

                            // Iba ak je incidence orientovany a out going nastaveny na false, tak pokracujem v dalsej analyze
                            if( functionIncidence->getOriented() && !functionIncidence->getOutGoing() ) {
                                Lua::LuaEdge* functionEdge = this->luaGraph->getEdges()->value( functionIncidence->getEdgeNodePair().first );
                                Lua::LuaNode* functionPairNode = nullptr;

                                // Zistim druhy incidence, ktory obsahuje druhy LuaNode pre danu hranu
                                Lua::LuaIncidence* functionOtherIncidence = this->luaGraph->getIncidences()->value( functionEdge->getIncidences().at( 0 ) );
                                if( functionIncidence == functionOtherIncidence ) {
                                    functionOtherIncidence = this->luaGraph->getIncidences()->value( functionEdge->getIncidences().at( 1 ) );
                                }

                                // LuaNode, ku ktoremu smeruje hrana zo suboru
                                functionPairNode = this->luaGraph->getNodes()->value( functionOtherIncidence->getEdgeNodePair().second );

                                // Ziskam identifikator funkcie(moze ist o lokalnu, aj globalnu)
                                QString innerFunctionIdentifier =  functionPairNode->getIdentifier();
                                /*QString innerFunctionIdentifier =  functionEdge->getIdentifier().replace( "+", "" ).replace( pairNode->getIdentifier(), "" );
                                // pri ojedinelych pripadoch sa stane pripad, kedy ide hrana do tohto isteho uzla
                                // zatial work around
                                if( innerFunctionIdentifier == "" ) {
                                    innerFunctionIdentifier = pairNode->getIdentifier();
                                }*/

                                // Zistim, ci sa identifikator funkcie nachadza uz v ulozenych funkciach pre dany subor
                                Repository::Git::GitFunction* innerFunction = functions.value( innerFunctionIdentifier );

                                // Ak sa funkcia este nenachadza v mape funkcii, tak ju vytvorime pre dany subor
                                if( !innerFunction ) {
                                    // Inicializujem funkciu
                                    innerFunction = new Repository::Git::GitFunction();

                                    // Rozlozim identifikator cez oddelovac ";"
                                    QStringList list = innerFunctionIdentifier.split( ";" );

                                    // Zistim akeho typu je funkcia(lokalna, globalna) a podla toho nastavim funkciu
                                    if( !QString::compare( QString::fromStdString( functionPairNode->getParams()["type"].asString() ), "function" ) ) {
                                        // Meno funkcie sa sklada za cesty suboru, v ktorom sa nachadza funckia(1) a nazvu funkcie (2)
                                        innerFunction->setName( list.at( 1 ) + ";" + list.at( 2 ) );
                                        innerFunction->setFunctionType( Repository::Git::GitFunctionType::LOCALFUNCTION );
                                    } else {
                                        // Ak sa pocet tokenov z identifikatora rovna 2, tak globalna funkcia nema global modul
                                        // inak nenastavim modul
                                        if( list.size() == 2 ) {
                                            innerFunction->setName( list.at( 1 ) );
                                        } else {
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

            // Ziskam starsiu verziu daneho suboru
            Repository::Git::GitFile* oldFile = evolutionGraph->getLatestGitFileCallTree().value( file->getIdentifier() );

            // Porovnam aktualnu verziu suboru s predoslou verziou
            compareFilesAndSaveToEvolutionGraph( file, oldFile );

            // Nahradim predoslu verziu sucasnou v mape verzii suborov a ich podstromov
            evolutionGraph->addLatestGitFileCallTree( file->getIdentifier(), file );

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
//            qDebug() << node->getIdentifier() << "s poctom incidence" << node->getIncidences().size() << "/" << counter << "/" << file->getGitFunctions()->size();
        }
    }

    // Spracovanie suborov, ktore maju typ REMOVED
    QMap<QString, Repository::Git::GitFile*> removedFiles = this->evolutionGraph->getVersion( this->getVersionNumber() )->getGitFilesByType( Repository::Git::GitType::REMOVED );
    for( QMap<QString, Repository::Git::GitFile*>::iterator iterator = removedFiles.begin(); iterator != removedFiles.end(); ++iterator ) {
        Repository::Git::GitFile* oldFile = this->evolutionGraph->getLatestGitFileCallTree().value( iterator.value()->getIdentifier() );
        compareFilesAndSaveToEvolutionGraph( nullptr, oldFile );
    }

    // Nastav, ze verzia bola nacitana
    version->setIsLoaded( true );

}

void Repository::Git::GitLuaGraphAnalyzer::compareFilesAndSaveToEvolutionGraph( Repository::Git::GitFile *newFile, Repository::Git::GitFile *oldFile ) {
    // Ziskam subor, v ktorom su ulozene vsetky metadata v evolucnom grafe pre dany subor
    Repository::Git::GitFile* file = nullptr;
    if( newFile != nullptr ) {
        file = this->evolutionGraph->getVersion( this->versionNumber )->getGitFileByIdentifier( newFile->getIdentifier() );
    } else {
        file = this->evolutionGraph->getVersion( this->versionNumber )->getGitFileByIdentifier( oldFile->getIdentifier() );
    }

    // Vytvorim mapu lokalnych funkcii na ulozenie uz spracovanych lokalnych funkcii
    QMap<QString, bool>* functions = new QMap<QString, bool>();

    // Prejdem funkcie ulozene v novom subore a skontrolujem, ci sa dany identifikator nachadza v starsej verzii a nasledne porovnam tieto dve funkcie
    if( newFile != nullptr ) {
        for( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = newFile->getGitFunctions()->begin(); iterator != newFile->getGitFunctions()->end(); ++iterator ) {
            // Ak ide o lokalnu funkciu, tak pridam jeho identifikator do mapy lokalnych funkcii, ak sa tam nenachadza. Ak sa nachadza, tak pokracujem na dalsiu funkciu
            if( iterator.value()->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
                // Ak sa lokalna funkcia nachadza v mape lokalnych funkcii, tak preskocim spracovanie danej lokalnej funkcie
                // Inak ju pridam do mapy lokalnych funkcii
                if( functions->contains( iterator.value()->getIdentifier() ) ) {
                    continue;
                } else {
                    functions->insert( iterator.value()->getIdentifier(), true );
                }
            }

            // Ak este starsia verzia suboru nebola vytvorena, tak kontrolujeme voci nullptr
            if( oldFile != nullptr ) {
                compareFunctions( iterator.value(), oldFile->getGitFunctions()->value( iterator.value()->getIdentifier() ), functions, file->getIdentifier(), file );
            } else {
                compareFunctions( iterator.value(), nullptr, functions, file->getIdentifier(), file );
            }
        }
    }

    // Ak existuje starsia verzia suboru, tak prejdem funkcie ulozene v starom subore a skontrolujem, ci sa dany identifikator nachadza v novej verzii a nasledne porovname tieto dve funkcie
    if( oldFile != nullptr ) {
        // Prejdem funkcie ulozene v starom subore a skontrolujem, ci sa dany identifikator nachadza v novej verzii a nasledne porovnam tieto dve funkcie
        for( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = oldFile->getGitFunctions()->begin(); iterator != oldFile->getGitFunctions()->end(); ++iterator ) {
            // Ak ide o lokalnu funkciu, tak pridam jeho identifikator do mapy lokalnych funkcii, ak sa tam nenechadza. Ak sa nachadza, tak pokracujem na dalsiu funkciu.
            if( iterator.value()->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
                // Ak sa lokalna funkcia nachadza v mape lokalnych funkcii, tak preskocim spracovanie danej lokalnej funkcie
                // Inak ju pridam do mapy lokalnych funkcii
                if( functions->contains( iterator.value()->getIdentifier() ) ) {
                 continue;
                } else {
                    functions->insert( iterator.value()->getIdentifier(), true );
                }
            }
            if( newFile != nullptr ) {
                compareFunctions( newFile->getGitFunctions()->value( iterator.value()->getIdentifier() ), iterator.value(), functions, file->getIdentifier(), file );
            } else {
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
    if( file->getType() == Repository::Git::GitType::REMOVED ) {
        qDebug() << "VYMAZANY SUBOR" << file->getIdentifier();
        qDebug() << "FUNCTION COUNT" << file->getGitFunctions()->size() << "/" << oldFile->getGitFunctions()->size();
    }

    // Vymazem identifikatory funkcii
    delete functions;
}

void Repository::Git::GitLuaGraphAnalyzer::compareFunctions( Repository::Git::GitFunction *newFunction, Repository::Git::GitFunction *oldFunction, QMap<QString, bool> *functions, QString masterIdentifier, Repository::Git::GitFile* masterFile ) {

    // Ak su obe funkcie rozdielne od nullptr
    if( newFunction != nullptr && oldFunction != nullptr ) {
        if( newFunction->getFunctionType() == Repository::Git::GitFunctionType::GLOBALFUNCTION ) {
            // ToDo Tu sme uz asi skoncili, lebo zmeny v globalnych funkciach nevieme dohladat ani pomocou inspekcie AST
        } else {
            // ToDo Ide o lokalnu funkciu, ktoru treba porovnat s jej predchadzajucou verziou
            // Zistit viem zatial len rozdiel, stav MODIFIED este nedokazem zistit, bez zaciatkov jednotlivych lokalnych funkcii

            // pomocna premenna na zistenie, ci sa lokalna funkcia zmenila
            bool isChanged = false;

            for( QMap<QString, Repository::Git::GitFunction*>::iterator iterator =  newFunction->getFunctionCallers()->begin(); iterator != newFunction->getFunctionCallers()->end(); ++iterator ) {
                if( !oldFunction->getFunctionCallers()->contains( iterator.value()->getIdentifier() ) ) {
//                    qDebug() << iterator.value()->getIdentifier() << "sa nenachadza v" << oldFunction->getIdentifier() << "IS ADDED!!!";

                    Repository::Git::GitFunction* addedFunction = new Repository::Git::GitFunction();

                    addedFunction->setName( iterator.value()->getName() );
                    addedFunction->setModule( iterator.value()->getModule() );
                    addedFunction->setId( iterator.value()->getId() );
                    addedFunction->setType( Repository::Git::GitType::ADDED );
                    addedFunction->setFunctionType( iterator.value()->getFunctionType() );

                    Repository::Git::GitFunction* cloneNewFunction = masterFile->getGitFunctions()->value( newFunction->getIdentifier() );

                    if( !cloneNewFunction ) {
                        cloneNewFunction = new Repository::Git::GitFunction();
                        cloneNewFunction->setName( newFunction->getName() );
                        cloneNewFunction->setId( newFunction->getId() );
                        cloneNewFunction->setModule( newFunction->getModule() );
                        cloneNewFunction->setType( Repository::Git::GitType::MODIFIED );
                        cloneNewFunction->setFunctionType( newFunction->getFunctionType() );

                        masterFile->addGitFunction( cloneNewFunction );
                    }

                    cloneNewFunction->addFunctionCaller( addedFunction );
                    addedFunction->addFunctionCallee( cloneNewFunction );

                    isChanged = true;
                }
            }

            for( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = oldFunction->getFunctionCallers()->begin(); iterator != oldFunction->getFunctionCallers()->end(); ++iterator ) {
                if( !newFunction->getFunctionCallers()->contains( iterator.value()->getIdentifier() ) ) {
//                    qDebug() << iterator.value()->getIdentifier() << "sa nenachadza v" << newFunction->getIdentifier() << "IS REMOVED!!!";

                    Repository::Git::GitFunction* removedFunction = new Repository::Git::GitFunction();

                    removedFunction->setName( iterator.value()->getName() );
                    removedFunction->setModule( iterator.value()->getModule() );
                    removedFunction->setId( iterator.value()->getId() );
                    removedFunction->setType( Repository::Git::GitType::REMOVED );
                    removedFunction->setFunctionType( iterator.value()->getFunctionType() );

                    Repository::Git::GitFunction* cloneNewFunction = masterFile->getGitFunctions()->value( newFunction->getIdentifier() );

                    if( !cloneNewFunction ) {
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

                    isChanged = true;
                }
            }

            if( !isChanged ) {
//                qDebug() << newFunction->getIdentifier() << "MAY BE MODIFIED OR AFFECTED!!!";
            }

        }
    }
    else  if( newFunction == nullptr ) {
//        qDebug() << "REMOVED ->" << oldFunction->getIdentifier() << "from" << masterIdentifier;

        Repository::Git::GitFunction* masterFileFunction = new Repository::Git::GitFunction();

        masterFileFunction->setName( oldFunction->getName() );
        masterFileFunction->setModule( oldFunction->getModule() );
        masterFileFunction->setFunctionType( oldFunction->getFunctionType() );
        masterFileFunction->setType( Repository::Git::GitType::REMOVED );
        masterFileFunction->setId( oldFunction->getId() );

        masterFile->addGitFunction( masterFileFunction );

        if( oldFunction->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION && oldFunction->getFunctionCallers()->size() > 0 ) {
            for( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = oldFunction->getFunctionCallers()->begin(); iterator != oldFunction->getFunctionCallers()->end(); ++iterator ) {
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
    } else {
//        qDebug() << "ADDED ->" << newFunction->getIdentifier() << "from" << masterIdentifier;
        Repository::Git::GitFunction* masterFileFunction = new Repository::Git::GitFunction();

        masterFileFunction->setName( newFunction->getName() );
        masterFileFunction->setModule( newFunction->getModule() );
        masterFileFunction->setFunctionType( newFunction->getFunctionType() );
        masterFileFunction->setType( Repository::Git::GitType::ADDED );
        masterFileFunction->setId( newFunction->getId() );

        masterFile->addGitFunction( masterFileFunction );

        if( newFunction->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION && newFunction->getFunctionCallers()->size() > 0 ) {
            for( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = newFunction->getFunctionCallers()->begin(); iterator != newFunction->getFunctionCallers()->end(); ++iterator ) {
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
    }
}


