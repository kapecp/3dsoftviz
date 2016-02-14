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

    for( QMap<qlonglong, Lua::LuaNode*>::iterator iterator = this->luaGraph->getNodes()->begin(); iterator != this->luaGraph->getNodes()->end(); ++iterator ) {
        if( versionFiles.contains( iterator.value()->getIdentifier()  ) ) {
            Lua::LuaNode* node = iterator.value();

            QMap<QString, Repository::Git::GitFunction*> functions = QMap<QString, Repository::Git::GitFunction*>();

            Repository::Git::GitFile* file = new Repository::Git::GitFile( node->getIdentifier(), node->getIdentifier().split(";").at( 1 ), Repository::Git::GitType::MODIFIED );

            int counter = 0;
            foreach( qlonglong incidenceId, node->getIncidences() ) {
                Lua::LuaIncidence* incidence = this->luaGraph->getIncidences()->value( incidenceId );

                // Ak je incidence orientovany a outGoing je false, tak ide o hranu zo suboru na globalFunction
                // a pridam k danemu suboru vsetky global funkcie spolu s ich modulmi
                if( incidence->getOriented() && !incidence->getOutGoing() ) {
                    Lua::LuaEdge* edge = this->luaGraph->getEdges()->value( incidence->getEdgeNodePair().first );
                    Lua::LuaNode* pairNode = nullptr;

                    Lua::LuaIncidence* otherIncidence = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 0 ) );
                    if( incidence == otherIncidence ) {
                        otherIncidence = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 1 ) );
                    }

                    pairNode = this->luaGraph->getNodes()->value( otherIncidence->getEdgeNodePair().second );

                    QString functionIdentifier =  edge->getIdentifier().replace( "+", "" ).replace( node->getIdentifier(), "" );

                    // pri ojedinelych pripadoch sa stane pripad, kedy ide hrana do tohto isteho uzla
                    // zatial work around
                    if( functionIdentifier == "" ) {
                        functionIdentifier = node->getIdentifier();
                    }
//                    qDebug() << "Identifikator" << functionIdentifier;

                    Repository::Git::GitFunction* function = functions.value( functionIdentifier );

                    if( !function ) {

                        function = new Repository::Git::GitFunction();
                        QStringList list = functionIdentifier.split( ";" );

                        // Ak sa pocet tokenov z identifikatora rovna 2, tak globalna funkcia nema global modul
                        // inak nenastavim modul
                        if( list.size() == 2 ) {
                            function->setName( list.at( 1 ) );
                        } else {
                            function->setName( list.at( 2 ) );
                            function->setModule( list.at( 1 ) );
                        }

                        function->setType( Repository::Git::GitType::NONE );
                        function->setId( pairNode->getId() );

                        if( !QString::compare( QString::fromStdString( pairNode->getParams()["type"].asString() ), "globalFunction" ) ) {
                            function->setFunctionType( Repository::Git::GitFunctionType::GLOBALFUNCTION );
                        } else {
                            qDebug() << "CHYBA V MOJEJ HLAVE!!!!! pri dedikcii globalnej funckii";
                        }

                        functions.insert( functionIdentifier, function );
                    }

                    file->addGitFunction( function );
                    counter++;

                } else {
                    //ToDo Tie, ktore nie su orientovane, treba skontrolovat dalsi edge a druhy node, ak je rovny function tak ho dalej spracuj, inak nespracuj dany node
                    Lua::LuaEdge* edge = this->luaGraph->getEdges()->value( incidence->getEdgeNodePair().first );
                    Lua::LuaNode* pairNode = nullptr;

                    Lua::LuaIncidence* otherIncidence = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 0 ) );
                    if( incidence == otherIncidence ) {
                        otherIncidence = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 1 ) );
                    }

                    pairNode = this->luaGraph->getNodes()->value( otherIncidence->getEdgeNodePair().second );

                    // Pokracujeme v spracovani len ak ide o hranu s lokalnou funkciou
                    if( !QString::compare( QString::fromStdString( pairNode->getParams()["type"].asString() ), "function") ) {
                        QString functionIdentifier = edge->getIdentifier().replace( "+", "" ).replace( node->getIdentifier(), "" );

                        // pri ojedinelych pripadoch sa stane pripad, kedy ide hrana do tohto isteho uzla
                        // zatial work around
                        if( functionIdentifier == "" ) {
                            functionIdentifier = node->getIdentifier();
                        }


                        Repository::Git::GitFunction* function =  functions.value( functionIdentifier );

                        if( !function ) {
                            function = new Repository::Git::GitFunction();

                            QStringList list = functionIdentifier.split( ";" );

                            function->setName( list.at( 1 ) + ";" + list.at( 2 ) );

                            function->setType( Repository::Git::GitType::NONE );
                            function->setId( pairNode->getId() );


                            function->setFunctionType( Repository::Git::GitFunctionType::LOCALFUNCTION );
                            functions.insert( functionIdentifier, function );
                        }

                        file->addGitFunction( function );

                        foreach( qlonglong functionIncidenceId, pairNode->getIncidences() ) {
                            Lua::LuaIncidence* functionIncidence = this->luaGraph->getIncidences()->value( functionIncidenceId );

                            if( functionIncidence->getOriented() && !functionIncidence->getOutGoing() ) {
                                Lua::LuaEdge* functionEdge = this->luaGraph->getEdges()->value( functionIncidence->getEdgeNodePair().first );
                                Lua::LuaNode* functionPairNode = nullptr;

                                Lua::LuaIncidence* functionOtherIncidence = this->luaGraph->getIncidences()->value( functionEdge->getIncidences().at( 0 ) );

                                if( functionIncidence == functionOtherIncidence ) {
                                    functionOtherIncidence = this->luaGraph->getIncidences()->value( functionEdge->getIncidences().at( 1 ) );
                                }

                                functionPairNode = this->luaGraph->getNodes()->value( functionOtherIncidence->getEdgeNodePair().second );

                                QString innerFunctionIdentifier =  functionEdge->getIdentifier().replace( "+", "" ).replace( pairNode->getIdentifier(), "" );
                                // pri ojedinelych pripadoch sa stane pripad, kedy ide hrana do tohto isteho uzla
                                // zatial work around
                                if( innerFunctionIdentifier == "" ) {
                                    innerFunctionIdentifier = pairNode->getIdentifier();
                                }

                                Repository::Git::GitFunction* innerFunction = functions.value( innerFunctionIdentifier );

                                if( !innerFunction ) {
                                    innerFunction = new Repository::Git::GitFunction();
                                    QStringList list = innerFunctionIdentifier.split( ";" );

                                    if( !QString::compare( QString::fromStdString( functionPairNode->getParams()["type"].asString() ), "function" ) ) {
                                        innerFunction->setName( list.at( 1 ) + ";" + list.at( 2 ) );
                                        innerFunction->setFunctionType( Repository::Git::GitFunctionType::LOCALFUNCTION );
                                    } else {
                                        if( list.size() == 2 ) {
                                            innerFunction->setName( list.at( 1 ) );
                                        } else {
                                            innerFunction->setName( list.at( 2 ) );
                                            innerFunction->setModule( list.at( 1 ) );
                                        }
                                        innerFunction->setFunctionType( Repository::Git::GitFunctionType::GLOBALFUNCTION );
                                    }
                                    innerFunction->setId( functionPairNode->getId() );
                                    innerFunction->setType( Repository::Git::GitType::NONE );
                                }

                                functions.insert( innerFunctionIdentifier, innerFunction );
                                function->addFunctionCaller( innerFunction );
                                innerFunction->addFunctionCallee( function );
                            }
                        }
                    }

                }

            }




            //ToDo Pridat metodu, ktora porovna sucasny file s ulozenym file a ulozi ich rozdiel do evolutionGraph
            Repository::Git::GitFile* oldFile = evolutionGraph->getLatestGitFileCallTree().value( file->getIndetifier() );
            compareFilesAndSaveToEvolutionGraph( file, oldFile );
            evolutionGraph->addLatestGitFileCallTree( file->getIndetifier(), file );

/*
            for( QMap<QString, Repository::Git::GitFunction*>::iterator  iterator = file->getGitFunctions()->begin(); iterator != file->getGitFunctions()->end(); ++iterator ) {
                Repository::Git::GitFunction* function = iterator.value();

                qDebug() << file->getIndetifier() << "->" <<  function->getIdentifier();

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

//    Repository::Git::GitUtils::getModifiedLuaNodesFromVersion( this->evolutionGraph, this->versionNumber );
    version->setIsLoaded( true );

}

void Repository::Git::GitLuaGraphAnalyzer::compareFilesAndSaveToEvolutionGraph( Repository::Git::GitFile *newFile, Repository::Git::GitFile *oldFile ) {

    Repository::Git::GitFile* file = this->evolutionGraph->getVersion( this->versionNumber )->getGitFileByIdentifier( newFile->getIndetifier() );

    QMap<QString, bool>* functions = new QMap<QString, bool>();

    for( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = newFile->getGitFunctions()->begin(); iterator != newFile->getGitFunctions()->end(); ++iterator ) {
        if( iterator.value()->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
            if( functions->contains( iterator.value()->getIdentifier() ) ) {
                continue;
            } else {
                functions->insert( iterator.value()->getIdentifier(), true );
            }
        }

        if( oldFile != nullptr ) {
            compareFunctions( iterator.value(), oldFile->getGitFunctions()->value( iterator.value()->getIdentifier() ), functions, file->getIndetifier(), file );
        } else {
            compareFunctions( iterator.value(), nullptr, functions, file->getIndetifier(), file );
        }
    }

    if( oldFile != nullptr ) {
        for( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = oldFile->getGitFunctions()->begin(); iterator != oldFile->getGitFunctions()->end(); ++iterator ) {
            if( iterator.value()->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
                if( functions->contains( iterator.value()->getIdentifier() ) ) {
                 continue;
                } else {
                    functions->insert( iterator.value()->getIdentifier(), true );
                }
            }

            compareFunctions( newFile->getGitFunctions()->value( iterator.value()->getIdentifier() ), iterator.value(), functions, newFile->getIndetifier(), file );
        }
    }
/*
    for( QMap<QString, Repository::Git::GitFunction*>::iterator  iterator = file->getGitFunctions()->begin(); iterator != file->getGitFunctions()->end(); ++iterator ) {
        Repository::Git::GitFunction* function = iterator.value();

        qDebug() << file->getIndetifier() << "->" <<  function->getIdentifier() << function->getTypeAsString();

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
    delete functions;
}

void Repository::Git::GitLuaGraphAnalyzer::compareFunctions( Repository::Git::GitFunction *newFunction, Repository::Git::GitFunction *oldFunction, QMap<QString, bool> *functions, QString masterIdentifier, Repository::Git::GitFile* masterFile ) {

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


