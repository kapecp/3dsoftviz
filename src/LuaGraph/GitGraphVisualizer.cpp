#include "LuaGraph/GitGraphVisualizer.h"
#include "LuaGraph/HyperGraphVisualizer.h"
#include "LuaGraph/LuaGraphVisualizer.h"
#include "LuaGraph/LuaGraph.h"
#include "Importer/GraphOperations.h"
#include "LuaGraph/LuaIncidence.h"

#include "Data/Graph.h"

#include "Manager/Manager.h"
#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitVersion.h"
#include "GitLib/GitFile.h"
#include "GitLib/GitFunction.h"

#include <QMap>

#include "limits"
#include <iostream>

#include <QDebug>

Lua::GitGraphVisualizer::GitGraphVisualizer( Data::Graph* graph, osg::ref_ptr<osg::Camera> camera )
    : Lua::LuaGraphVisualizer( graph, camera ) {}

void Lua::GitGraphVisualizer::visualize()
{
    Repository::Git::GitEvolutionGraph* evolutionGraph =  Manager::GraphManager::getInstance()->getActiveEvolutionGraph();
    int currentVersion = Manager::GraphManager::getInstance()->getActiveGraph()->getCurrentVersion();
    Repository::Git::GitVersion* version = evolutionGraph->getVersion( currentVersion );
    QList<Repository::Git::GitFile*> versionFiles = version->getChangedFiles();

    Lua::LuaGraph* g = Lua::LuaGraph::loadEvoGraph( evolutionGraph->getFilePath() );
    //    g->printGraph();
    QMap<QString, Repository::Git::GitFunction*>* functions = new QMap<QString, Repository::Git::GitFunction*>();


    //    qDebug() << "Zaciatok vykreslovania GitLuaGraphu";
    for ( QMap<qlonglong, Lua::LuaNode*>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i ) {
        // ziskame referenciu na existujuci uzol v grafe alebo NULL
        osg::ref_ptr<Data::Node> n = currentGraph->findNodeByLuaIdentifier( i.value()->getIdentifier() );

        // ak sa uzol v grafe nenachadza, tak ho pridame do grafu a nastavime mu LuaIdentifier
        if( !n ) {
            n = currentGraph->addNode( i.key() , i.value()->getLabel(), nodeType );
            n->setLuaIdentifier( i.value()->getIdentifier() );
        }

        setNodeParams( n, i.value(), osg::Vec4f( 1,1,1,1 ), 8 );
        evolutionGraph->addLuaNodesMapping( i.value()->getIdentifier(), i.value()->getId() );

        // ziskame LuaNode pre sucasnu hodnotu iteratora nad vsetkymi LuaNode
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
            foreach( Repository::Git::GitFile* currentFile, versionFiles ) {
                if( !QString::compare( currentFile->getFilepath(), file ) ) {
                    // ziskame referenciu na funkciu alebo NULL
                    Repository::Git::GitFunction* function = functions->value( node->getIdentifier() );

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
                        functions->insert(node->getIdentifier(), function);

                        // zistime vsetky orientovane hrany z LuaNode
                        foreach( qlonglong id, node->getIncidences() ) {
                            Lua::LuaIncidence* incidence = g->getIncidences()->value( id );

                            // ak je incidence orientovany a outGoing ma nastavena na false, v tom
                            // pripade ide hrana z daneho uzlu (funkcie) a pridame novu funkciu
                            if( incidence->getOriented() && !incidence->getOutGoing() ) {
                                Lua::LuaEdge* edge = g->getEdges()->value( incidence->getEdgeNodePair().first );
                                Lua::LuaNode* otherNode = nullptr;
                                Lua::LuaIncidence* incid = g->getIncidences()->value( edge->getIncidences().at( 0 ) );
                                if( incid == incidence ) {
                                    incid = g->getIncidences()->value( edge->getIncidences().at( 1 ) );
                                    otherNode = g->getNodes()->value( incid->getEdgeNodePair().second );
                                } else {
                                    otherNode = g->getNodes()->value( incid->getEdgeNodePair().second );
                                }


                                QString functionIdentifier = edge->getIdentifier().replace( "+", "" ).replace( node->getIdentifier(), "" );
                                Repository::Git::GitFunction* funct = functions->value( functionIdentifier );
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
                                    }
                                    funct->setFunctionType( Repository::Git::GitFunctionType::GLOBALFUNCTION );
                                }

                                function->addFunctionCaller( funct );
                                funct->addFunctionCallee( function );
                                functions->insert( otherNode->getIdentifier(), funct );
                            }
                        }
                    } else {
                        function->setId( node->getId() );
                    }
                }
            }
        }

        if( !QString::compare( QString::fromStdString( node->getParams()["type"].asString() ), "globalModule" ) ) {
            foreach( qlonglong id ,node->getIncidences() ) {
                Lua::LuaIncidence* incidence = g->getIncidences()->value( id );
                Lua::LuaEdge* edge =  g->getEdges()->value( incidence->getEdgeNodePair().first );
                Lua::LuaNode* otherNode = nullptr;
                Lua::LuaIncidence* incid = g->getIncidences()->value( edge->getIncidences().at( 0 ) );
                if( incid == incidence ) {
                    incid = g->getIncidences()->value( edge->getIncidences().at( 1 ) );
                    otherNode = g->getNodes()->value( incid->getEdgeNodePair().second );
                } else {
                    otherNode = g->getNodes()->value( incid->getEdgeNodePair().second );
                }

                Repository::Git::GitFunction* function = functions->value( otherNode->getIdentifier() );
                if( function ) {
                    function->setModule( node->getLabel() );
                }
            }
        }

        if( !QString::compare( QString::fromStdString( node->getParams()["type"].asString() ), "file" ) ) {
            QStringList list = node->getIdentifier().split(":");
            QString file = list.at( 1 );
            foreach( Repository::Git::GitFile* currentFile, versionFiles ) {
                if( !QString::compare( currentFile->getFilepath(), file ) ) {
                    foreach(qlonglong id ,node->getIncidences() ) {
                        Lua::LuaIncidence* incidence = g->getIncidences()->value( id );
                        if( incidence->getOriented() && !incidence->getOutGoing() ) {
                            Lua::LuaEdge* edge = g->getEdges()->value( incidence->getEdgeNodePair().first );
                            Lua::LuaNode* otherNode = nullptr;
                            Lua::LuaIncidence* incid = g->getIncidences()->value( edge->getIncidences().at( 0 ) );
                            if( incid == incidence ) {
                                incid = g->getIncidences()->value( edge->getIncidences().at( 1 ) );
                                otherNode = g->getNodes()->value( incid->getEdgeNodePair().second );
                            } else {
                                otherNode = g->getNodes()->value( incid->getEdgeNodePair().second );
                            }


                            QString functionIdentifier = edge->getIdentifier().replace( "+", "" ).replace( node->getIdentifier(), "" );
                            Repository::Git::GitFunction* funct = functions->value( functionIdentifier );
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
                                }
                                funct->setFunctionType( Repository::Git::GitFunctionType::GLOBALFUNCTION );
                            }

                            functions->insert( otherNode->getIdentifier(), funct );
                            currentFile->addGitFunction( funct );
                        }
                    }
                } else {
//                    function->setId( node->getId() );
                }
            }
        }
    }



/*
    for( QMap<QString, Repository::Git::GitFunction*>::iterator iter = functions->begin(); iter != functions->end(); ++iter ) {
        qDebug() << iter.value()->printContent();
        foreach( Repository::Git::GitFunction* function, iter.value()->getFunctionCallees() ) {
            qDebug() << "Callee" << function->printContent();
        }
        foreach( Repository::Git::GitFunction* function, iter.value()->getFunctionCallers() ) {
            qDebug() << "Caller" << function->printContent();
        }
    }

    qDebug() << "Ulozene funkcie" << functions->size() ;
*/
    for ( QMap<qlonglong, Lua::LuaEdge*>::iterator i = g->getEdges()->begin(); i != g->getEdges()->end(); ++i ) {
        Lua::LuaEdge* edge = i.value();
        QString identifier = edge->getIdentifier();
        //        qDebug() << identifier;
        if ( i.value()->getIncidences().size() != 2 ) {
            throw new std::runtime_error( "Not a simple graph" );
        }
        LuaIncidence* const incid1 = g->getIncidences()->value( i.value()->getIncidences()[0] );
        LuaIncidence* const incid2 = g->getIncidences()->value( i.value()->getIncidences()[1] );
        osg::ref_ptr<Data::Node> srcNode = currentGraph->findNodeByLuaIdentifier( g->getNodes()->find( incid1->getEdgeNodePair().second ).value()->getIdentifier() );
        osg::ref_ptr<Data::Node> dstNode = currentGraph->findNodeByLuaIdentifier( g->getNodes()->find( incid2->getEdgeNodePair().second ).value()->getIdentifier() );
        osg::ref_ptr<Data::Edge> newEdge = currentGraph->findEdgeByLuaIdentifier( identifier );
        if( !newEdge ) {
            if ( incid1->getOriented() ) {
                if ( incid1->getOutGoing() ) {
                    newEdge = currentGraph->addEdge( i.key(), i.value()->getLabel(), dstNode, srcNode, edgeType, true );
                }
                else {
                    newEdge = currentGraph->addEdge( i.key(), i.value()->getLabel(), srcNode, dstNode, edgeType, true );
                }
            }
            else {
                newEdge = currentGraph->addEdge( i.key(), i.value()->getLabel(), srcNode, dstNode, edgeType, false );
            }
            newEdge->setLuaIdentifier( identifier );
        }

        newEdge->setCamera( camera );
        setEdgeParams( newEdge, i.value(), osg::Vec4f( 1,1,1,1 ) );
        evolutionGraph->addLuaEdgesMapping( i.value()->getIdentifier(), i.value()->getId() );
    }
    g->setObserver( this );
    /*
    QString metaNodeName = "metaNode";
    QString metaEdgeName = "metaEdge";
    osg::ref_ptr<Data::Node> filesAnchor = currentGraph->addNode( std::numeric_limits<qlonglong>::max(), metaNodeName, currentGraph->getNodeMetaType(), osg::Vec3( 0, 0, 500 ) );
    osg::ref_ptr<Data::Node> functionsAnchor = currentGraph->addNode( std::numeric_limits<qlonglong>::max() - 1, metaNodeName, currentGraph->getNodeMetaType(), osg::Vec3( 0, 0, -500 ) );
    filesAnchor->setColor( osg::Vec4( 0,0,0,0 ) );
    functionsAnchor->setColor( osg::Vec4( 0,0,0,0 ) );

    for ( QMap<qlonglong, Lua::LuaNode*>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i ) {
        if ( i.value()->getParams().type() == 0 ) {
            continue;
        }
        if ( i.value()->getParams()["root"]== true ) {
            osg::ref_ptr<Data::Node> root = currentGraph->getNodes()->value( i.key() );
            osg::ref_ptr<Data::Edge> metaLink = currentGraph->addEdge( metaEdgeName, root, filesAnchor, currentGraph->getEdgeMetaType(), false );
            metaLink->setEdgeColor( osg::Vec4( 0,0,0,0 ) );
            metaLink->setInvisible( true );
        }
        if ( i.value()->getParams()["type"] == "function" ) {
            osg::ref_ptr<Data::Node> func = currentGraph->getNodes()->value( i.key() );
            osg::ref_ptr<Data::Edge> metaLink = currentGraph->addEdge( metaEdgeName, func, functionsAnchor, currentGraph->getEdgeMetaType(), false );
            metaLink->setEdgeColor( osg::Vec4( 0,0,0,0 ) );
            metaLink->setInvisible( true );
            metaLink->setEdgeStrength( 0.1f );
        }
    }*/
}

void Lua::GitGraphVisualizer::onUpdate()
{
    std::cout << "SimpleGraph update called" << std::endl;
    Lua::LuaGraph* g = Lua::LuaGraph::loadGraph();
    for ( QMap<qlonglong, Lua::LuaNode*>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i ) {
        osg::ref_ptr<Data::Node> n = currentGraph->getNodes()->value( i.key() );
        setNodeParams( n, i.value(), osg::Vec4f( 1,1,1,1 ), 8 );
    }
    for ( QMap<qlonglong, Lua::LuaEdge*>::iterator i = g->getEdges()->begin(); i != g->getEdges()->end(); ++i ) {
        osg::ref_ptr<Data::Edge> e = currentGraph->getEdges()->value( i.key() );
        setEdgeParams( e, i.value(), osg::Vec4f( 1,1,1,1 ) );
        e->reloadLabel();
    }
}



