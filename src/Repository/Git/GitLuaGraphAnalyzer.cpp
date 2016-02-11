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

            // ToDo Vytvorit GitFile, do ktoreho ulozim call tree

            int counter = 0;
            foreach( qlonglong incidenceId, node->getIncidences() ) {
                Lua::LuaIncidence* incidence = this->luaGraph->getIncidences()->value( incidenceId );
                if( incidence->getOriented() ) {
                    //ToDo Orientovane smeruju na globalFunction, treba pridat
                } else {
                    //ToDo Tie, ktore nie su orientovane, treba skontrolovat dalsi edge a druhy node, ak je rovny function tak ho dalej spracuj, inak nespracuj dany node
                }

            }

            qDebug() << node->getIdentifier() << "s poctom incidence" << node->getIncidences().size() << "/" << counter;
        }
    }

    /*
    Repository::Git::GitVersion* version = this->evolutionGraph->getVersion( this->versionNumber );
    QMap<QString, Repository::Git::GitFile*> versionFiles = version->getChangedFiles();

    for( QMap<qlonglong, Lua::LuaNode*>::iterator i = this->luaGraph->getNodes()->begin(); i != this->luaGraph->getNodes()->end(); ++i ) {
        // ziskam LuaNode pre sucasnu hodnotu iteratora nad vsetkymi LuaNodes v Lua grafe
        Lua::LuaNode* node = i.value();

        // ak je LuaNode typu FUNCTION, tak najdeme v adekvatnej verzii evolucneho grafu subor, na ktory je funkcia naviazana
        // a pripojime takto ziskanu funkciu danemu suboru
        if( !QString::compare( QString::fromStdString( node->getParams()["type"].asString() ), "function" ) ) {
            // rozdelime identifikator funkcie cez ":" a dostanem relativnu cestu k suboru od rootu projektu na pozicii 1 a nazov funkcie na pozicii 2
            QStringList list = node->getIdentifier().split(":");
            QString file = list.at( 1 );
            QString functionName = list.at( 2 );

            // Najdeme subor z aktualnej verzie evolucneho grafu, ktory sa rovna zistenemu suboru z identifikatora
            // a vytvorime/aktualizujeme funkciu
            for( QMap<QString, Repository::Git::GitFile*>::iterator iterator = versionFiles.begin(); iterator != versionFiles.end(); ++iterator ) {
                Repository::Git::GitFile* currentFile = iterator.value();

                if( !QString::compare( currentFile->getFilepath(), file ) ) {
                    // ziskame referenciu na funkciu alebo NULL
                    Repository::Git::GitFunction* function = nullptr;

                    // ak funkcia este nie je vytvorena, tak ju vytvorime
                    if( !function ) {
                        //                        qDebug() << "Vytvaram" << node->getIdentifier();
                        function = new Repository::Git::GitFunction();
                        if( currentFile->getType() == Repository::Git::GitType::ADDED ) {
                            function->setType( Repository::Git::GitType::ADDED );
                        } else {
                            function->setType( Repository::Git::GitType::NONE );
                        }

                        function->setName( file + ":" + functionName );
                        function->setId( node->getId() );
                        function->setFunctionType( Repository::Git::GitFunctionType::LOCALFUNCTION );

                        // pridame vytvorenu funkciu do funkcii v subore
                        currentFile->addGitFunction( function );
//                        qDebug() << currentFile->getFilename() << "->" << function->getIdentifier();

                        // zistime vsetky orientovane hrany z LuaNode
                        foreach( qlonglong id, node->getIncidences() ) {
                            Lua::LuaIncidence* incidence = this->luaGraph->getIncidences()->value( id );

                            // ak je incidence orientovany a outGoing ma nastavena na false, v tom
                            // pripade ide hrana z daneho uzlu (funkcie) a pridame novu funkciu
                            if( incidence->getOriented() && !incidence->getOutGoing() ) {
                                Lua::LuaEdge* edge = this->luaGraph->getEdges()->value( incidence->getEdgeNodePair().first );
                                Lua::LuaNode* otherNode = nullptr;
                                Lua::LuaIncidence* incid = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 0 ) );
                                if( incid == incidence ) {
                                    incid = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 1 ) );
                                }

                                otherNode = this->luaGraph->getNodes()->value( incid->getEdgeNodePair().second );

                                QString functionIdentifier = edge->getIdentifier().replace( "+", "" ).replace( node->getIdentifier(), "" );
                                Repository::Git::GitFunction* funct = nullptr;
//                                qDebug() << node->getIdentifier() << "->" << functionIdentifier << " - " << edge->getIdentifier();

                                if( !funct ) {
//                                    qDebug() << "Vytvaram" << functionIdentifier;
                                    funct = new Repository::Git::GitFunction();
                                    QStringList split = functionIdentifier.split( ":" );
                                    if( split.size() == 2 ) {
                                        funct->setName( split.at( 1 ) );
                                    } else {
                                        funct->setName( split.at( 2 ) );
                                    }

                                    funct->setType( Repository::Git::GitType::NONE );
                                    funct->setId( otherNode->getId() );
                                    if( !QString::compare( QString::fromStdString( otherNode->getParams()["type"].asString() ), "function" ) ) {
                                        funct->setFunctionType( Repository::Git::GitFunctionType::LOCALFUNCTION );
                                    } else {
                                        funct->setFunctionType( Repository::Git::GitFunctionType::GLOBALFUNCTION );

                                        foreach( qlonglong incId, otherNode->getIncidences() ) {
                                            Lua::LuaIncidence* moduleInc = this->luaGraph->getIncidences()->value( incId );

                                            if( !moduleInc->getOriented() ) {
                                                Lua::LuaEdge* moduleEdge = this->luaGraph->getEdges()->value( moduleInc->getEdgeNodePair().first );
                                                Lua::LuaNode* moduleNode = nullptr;
                                                Lua::LuaIncidence* otherIncidence = this->luaGraph->getIncidences()->value( moduleEdge->getIncidences().at( 0 ) );
                                                if( moduleInc == otherIncidence ) {
                                                    otherIncidence = this->luaGraph->getIncidences()->value( moduleEdge->getIncidences().at( 1 ) );
                                                }

                                                moduleNode = this->luaGraph->getNodes()->value( otherIncidence->getEdgeNodePair().second );
                                                funct->setModule( moduleNode->getLabel() );
                                            }
                                        }
                                    }

                                }

                                function->addFunctionCaller( funct );
                                funct->addFunctionCallee( function );
                            }
                        }
                    } else {
                        function->setId( node->getId() );
                    }
                }
            }
        }

        if( !QString::compare( QString::fromStdString( node->getParams()["type"].asString() ), "file" ) ) {
            QStringList list = node->getIdentifier().split(":");
            QString file = list.at( 1 );
            foreach( Repository::Git::GitFile* currentFile, versionFiles ) {
                if( !QString::compare( currentFile->getFilepath(), file ) ) {
                    foreach(qlonglong id ,node->getIncidences() ) {
                        Lua::LuaIncidence* incidence = this->luaGraph->getIncidences()->value( id );
                        if( incidence->getOriented() && !incidence->getOutGoing() ) {
                            Lua::LuaEdge* edge = this->luaGraph->getEdges()->value( incidence->getEdgeNodePair().first );
                            Lua::LuaNode* otherNode = nullptr;
                            Lua::LuaIncidence* incid = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 0 ) );
                            if( incid == incidence ) {
                                incid = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 1 ) );
                                otherNode = this->luaGraph->getNodes()->value( incid->getEdgeNodePair().second );
                            } else {
                                otherNode = this->luaGraph->getNodes()->value( incid->getEdgeNodePair().second );
                            }


                            QString functionIdentifier = edge->getIdentifier().replace( "+", "" ).replace( node->getIdentifier(), "" );
                            Repository::Git::GitFunction* funct = nullptr;
//                            qDebug() << node->getIdentifier() << "->" << functionIdentifier << " - " << edge->getIdentifier();

                            if( !funct ) {
                                //                                    qDebug() << "Vytvaram" << functionIdentifier;
                                funct = new Repository::Git::GitFunction();
                                QStringList split = functionIdentifier.split( ":" );
                                if( split.size() == 2 ) {
                                    funct->setName( split.at( 1 ) );
                                } else {
                                    funct->setName( split.at( 2 ) );
                                }

                                funct->setType( Repository::Git::GitType::NONE );
                                funct->setId( otherNode->getId() );
                                if( !QString::compare( QString::fromStdString( otherNode->getParams()["type"].asString() ), "function" ) ) {
                                    funct->setFunctionType( Repository::Git::GitFunctionType::LOCALFUNCTION );
                                } else {
                                    funct->setFunctionType( Repository::Git::GitFunctionType::GLOBALFUNCTION );

                                    foreach( qlonglong incId, otherNode->getIncidences() ) {
                                            Lua::LuaIncidence* moduleInc = this->luaGraph->getIncidences()->value( incId );

                                            if( !moduleInc->getOriented() ) {
                                                Lua::LuaEdge* moduleEdge = this->luaGraph->getEdges()->value( moduleInc->getEdgeNodePair().first );
                                                Lua::LuaNode* moduleNode = nullptr;
                                                Lua::LuaIncidence* otherIncidence = this->luaGraph->getIncidences()->value( moduleEdge->getIncidences().at( 0 ) );
                                                if( moduleInc == otherIncidence ) {
                                                    otherIncidence = this->luaGraph->getIncidences()->value( moduleEdge->getIncidences().at( 1 ) );
                                                }

                                                moduleNode = this->luaGraph->getNodes()->value( otherIncidence->getEdgeNodePair().second );
                                                funct->setModule( moduleNode->getLabel() );
                                            }
                                        }
                                }
                            }

//                            qDebug() << currentFile->getFilename() << "->" << funct->getIdentifier();
                            currentFile->addGitFunction( funct );
                        }
                    }
                } else {
//                    function->setId( node->getId() );
                }
            }
        }
    }

//    Repository::Git::GitUtils::getModifiedLuaNodesFromVersion( this->evolutionGraph, this->versionNumber );
    version->setIsLoaded( true );
    */
}


