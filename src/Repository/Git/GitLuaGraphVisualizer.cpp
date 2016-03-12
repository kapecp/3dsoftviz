#include "Repository/Git/GitLuaGraphVisualizer.h"

#include "Data/Graph.h"

#include "GitLib/GitVersion.h"
#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitFile.h"
#include "GitLib/GitFunction.h"
#include "GitLib/GitMetaData.h"

#include "LuaGraph/LuaGraph.h"

#include <QMap>
#include <QStringList>

Repository::Git::GitLuaGraphVisualizer::GitLuaGraphVisualizer( Data::Graph *currentGraph, Repository::Git::GitEvolutionGraph *evolutionGraph, osg::ref_ptr<osg::Camera> camera, bool showLuaStats )
        : currentGraph( currentGraph ), evolutionGraph( evolutionGraph ), luaGraph( Lua::LuaGraph::getInstance() ), showLuaStats( showLuaStats ), isChangingRepresentation( false ), camera( camera ) {
        if( this->currentGraph->getTypes()->size() == 0 ) {
            addDefaultTypes();
            addCustomTypes();
        }
}

void Repository::Git::GitLuaGraphVisualizer::visualize( bool next ) {

    reloadNodeRepresentation( this->showLuaStats );

    // ziskame verziu nastavenu vo vizualizovanom grafe
    int currentVersion = this->currentGraph->getCurrentVersion();

    Repository::Git::GitVersion* version = this->evolutionGraph->getVersion( currentVersion );
    QMap<QString, Repository::Git::GitFile*> changedFiles = *version->getChangedFiles();

    QString rootIdentifier = "directory;" + this->evolutionGraph->getFilePath();

    Lua::LuaNode* luaRoot = luaGraph->findNodeByLuaIdentifier( rootIdentifier );

    osg::ref_ptr<Data::Node> root = this->currentGraph->findNodeByLuaIdentifier( rootIdentifier );

    if( !root ) {
        qDebug() << "Vytvaram root uzol v grafe";
        root = this->currentGraph->addNode( luaRoot->getId(), this->evolutionGraph->getFilePath(), currentGraph->getTypesByName( "node" ).at( 0 ) );
        root->setLuaIdentifier( rootIdentifier );

        osg::ref_ptr<Data::Node> metaRoot = this->currentGraph->addNode( std::numeric_limits<qlonglong>::max(), "rootAnchor", this->currentGraph->getNodeMetaType(), osg::Vec3( 0, 0, 500 ) );
        metaRoot->setLuaIdentifier( "metaRoot" );
        metaRoot->setColor( osg::Vec4( 0, 0, 0, 0 ) );

        QString metaEdge = "meta+" + rootIdentifier;
        osg::ref_ptr<Data::Edge> metaLink = this->currentGraph->addEdge( metaEdge, root, metaRoot, this->currentGraph->getEdgeMetaType(), false );
        metaLink->setLuaIdentifier( metaEdge );
        metaLink->setEdgeColor( osg::Vec4f( 0, 0, 0, 0 ) );
        metaLink->setInvisible( true );

        osg::ref_ptr<Data::Node> metaFunction = this->currentGraph->addNode( std::numeric_limits<qlonglong>::max() - 1, "functionAnchor", this->currentGraph->getNodeMetaType(), osg::Vec3( 0, 0, -500 ) );
        metaFunction->setLuaIdentifier( "metaFunction" );
        metaFunction->setColor( osg::Vec4( 0, 0, 0, 0 ) );
    }

    this->evolutionGraph->getMetaDataFromIdentifier( rootIdentifier )->setLuaMapping( luaRoot->getId() );

    // ak je next false, tak zvysime verziu ( potrebujeme dalsiu verziu o sucasnej, aby sme vedeli, co vymanazat/pridat )
    if( !next ) {
        int rollBackVersion = currentVersion + 1;
//        this->currentGraph->setCurrentVersion( rollBackVersion );

        Repository::Git::GitVersion* version = this->evolutionGraph->getVersion( rollBackVersion );
        QMap<QString, Repository::Git::GitFile*> addedFiles = *version->getChangedFiles();
        for( QMap<QString, Repository::Git::GitFile*>::iterator iterator = addedFiles.begin(); iterator != addedFiles.end(); ++iterator ) {
            switch( iterator.value()->getType() ) {
            case Repository::Git::GitType::ADDED:
                removeFileFromGraph( iterator.value(), next );
                break;
            case Repository::Git::GitType::REMOVED:
                addFileToGraph( iterator.value(), rootIdentifier );
                break;
            }

            processFunctionsFromFile( iterator.value(), next );
        }        
    }


    for( QMap<QString, Repository::Git::GitFile*>::iterator iterator = changedFiles.begin(); iterator != changedFiles.end(); ++iterator ) {

        switch( iterator.value()->getType() ) {
        case Repository::Git::GitType::ADDED:
//            if( next ) {
                addFileToGraph( iterator.value(), rootIdentifier );
//            } else {
//                qDebug() << "MAZEM" << iterator.value()->getIdentifier();
//                removeFileFromGraph( iterator.value(), next );
//            }
            break;
        case Repository::Git::GitType::REMOVED:
//            if( next ) {
//                qDebug() << "MAZEM" << iterator.value()->getIdentifier();
                removeFileFromGraph( iterator.value(), true );
//            } else {
//                addFileToGraph( iterator.value(), rootIdentifier );
//            }
            break;
        case Repository::Git::GitType::MODIFIED:
//            qDebug() << "MODIFYING" << iterator.value()->getIdentifier();
            this->evolutionGraph->addChangedNodeOrEdge( iterator.value()->getIdentifier(), Repository::Git::GitType::MODIFIED );
            this->evolutionGraph->getMetaDataFromIdentifier( iterator.value()->getIdentifier() )->setChangedVersion( this->currentGraph->getCurrentVersion() );
            break;
        default:
            qDebug() << "CHYBNY TYP PRE SUBOR" << iterator.value()->getIdentifier();
            break;
        }

        processFunctionsFromFile( iterator.value(), true );
    }

    updateCurrentGraphNodesId();
    processChangedNodesAndEdges();
}

bool Repository::Git::GitLuaGraphVisualizer::addFileToGraph( Repository::Git::GitFile *file, QString rootIdentifier ) {
    QStringList list = file->getFilepath().split( "/" );

    QString identifier = rootIdentifier;
    osg::ref_ptr<Data::Node> node = this->currentGraph->findNodeByLuaIdentifier( identifier );

    QString string = "";
    for( int i = 0; i < list.size(); i++ ) {
        string += list.at( i );
        list.replace( i, string );
        string += "/";
    }

    for( int i = 0; i < list.size(); i++ ) {
        QString newIdentifier = "";
        if( i < list.size() - 1 ) {
            newIdentifier = "directory;" + list.at( i );
        } else {
            newIdentifier = "file;" + list.at( i );
        }

        Lua::LuaNode* luaNode = this->luaGraph->findNodeByLuaIdentifier( newIdentifier );
        osg::ref_ptr<Data::Node> newNode = this->currentGraph->findNodeByLuaIdentifier( newIdentifier );
        if( !newNode ) {
//            qDebug() << "ADDING node to Graph" << newIdentifier;
            if( luaNode ) {
                newNode = this->currentGraph->addNode( luaNode->getId(), luaNode->getLabel(), this->currentGraph->getTypesByName( "node" ).at( 0 ) );
//                newNode = this->currentGraph->addNode( luaNode->getLabel(), this->currentGraph->getTypesByName( "node" ).at( 0 ) );
            } else {
                newNode = this->currentGraph->addNode( list.at( i ), this->currentGraph->getTypesByName( "node" ).at( 0 ) );
            }
            newNode->setLuaIdentifier( newIdentifier );
            this->evolutionGraph->getMetaDataFromIdentifier( newIdentifier )->setFirstOccurence( this->currentGraph->getCurrentVersion() );
        } else {
            newNode->setType( this->currentGraph->getTypesByName( "node" ).at( 0 ) );
            this->evolutionGraph->removeChangedNodeOrEdge( newIdentifier );
        }

        this->evolutionGraph->addChangedNodeOrEdge( newIdentifier, Repository::Git::GitType::ADDED );
        this->evolutionGraph->getMetaDataFromIdentifier( newIdentifier )->setChangedVersion( this->currentGraph->getCurrentVersion() );

        if( luaNode ) {
            this->evolutionGraph->getMetaDataFromIdentifier( newIdentifier )->setLuaMapping( luaNode->getId() );
        } else {
            this->evolutionGraph->getMetaDataFromIdentifier( newIdentifier )->setLuaMapping( newNode->getId() );
        }

        if( this->isChangingRepresentation ) {
//            this->evolutionGraph->getMetaDataFromIdentifier( newIdentifier )->increaseOccurence();
        }

        QString edgeIdentifier =  identifier + "+" + newIdentifier;

        Lua::LuaEdge* luaEdge = this->luaGraph->findEdgeByLuaIdentifier( edgeIdentifier );
        osg::ref_ptr<Data::Edge> newEdge = this->currentGraph->findEdgeByLuaIdentifier( edgeIdentifier );
        if( !newEdge ) {
//            qDebug() << "ADDING edge to Graph" << edgeIdentifier;
            if( luaEdge ) {
                newEdge = this->currentGraph->addEdge( luaEdge->getId(), luaEdge->getLabel(), node, newNode, this->currentGraph->getTypesByName( "edge" ).at( 0 ), false );
//                newEdge = this->currentGraph->addEdge( luaEdge->getLabel(), node, newNode, this->currentGraph->getTypesByName( "edge" ).at( 0 ), false );
            } else {
                newEdge = this->currentGraph->addEdge( edgeIdentifier, node, newNode, this->currentGraph->getTypesByName( "edge" ).at( 0 ), false );
            }
            newEdge->setLuaIdentifier( edgeIdentifier );
            newEdge->setCamera( this->camera );
            this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setFirstOccurence( this->currentGraph->getCurrentVersion() );
        } else {
            this->evolutionGraph->removeChangedNodeOrEdge( edgeIdentifier );

        }

        this->evolutionGraph->addChangedNodeOrEdge( edgeIdentifier, Repository::Git::GitType::ADDED );
        this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setChangedVersion( this->currentGraph->getCurrentVersion() );

        if( luaEdge ) {
            this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setLuaMapping( luaEdge->getId() );
            if( showLuaStats ) {
                setEdgeParams( newEdge, luaEdge, osg::Vec4f( 1, 1, 1, 1 ) );
            }
        } else {
            this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setLuaMapping( newEdge->getId() );
        }

        identifier = newIdentifier;
        node = newNode;
    }

    return true;
}

bool Repository::Git::GitLuaGraphVisualizer::processFunctionsFromFile( Repository::Git::GitFile* file, bool next ) {

    for( QMap<QString, Repository::Git::GitFunction*>::iterator iterator =  file->getGitFunctions()->begin(); iterator != file->getGitFunctions()->end(); ++iterator ) {
        Repository::Git::GitFunction* function = iterator.value();

        switch( function->getType() ) {
        case Repository::Git::GitType::ADDED :

            if( next ) {
//                qDebug() << "ADDED" << file->getIdentifier() << "->" << function->getIdentifier();
                addFunctionToGraph( function, file->getIdentifier() );
            } else {
//                qDebug() << "REMOVED" << file->getIdentifier() << "->" << function->getIdentifier();
                removeFunctionFromGraph( function, file->getIdentifier(), next );
            }
            break;

        case Repository::Git::GitType::REMOVED :

            if( next ) {
//                qDebug() << "REMOVED" << file->getIdentifier() << "->" << function->getIdentifier();
                removeFunctionFromGraph( function, file->getIdentifier(), next );
            } else {
//                qDebug() << "ADDED" << file->getIdentifier() << "->" << function->getIdentifier();
                addFunctionToGraph( function, file->getIdentifier() );
            }
            break;

        case Repository::Git::GitType::MODIFIED :
//            qDebug() << "MODIFIED" << file->getIdentifier() << "->" << function->getIdentifier();
            this->evolutionGraph->addChangedNodeOrEdge( function->getIdentifier(), Repository::Git::GitType::MODIFIED );
            this->evolutionGraph->getMetaDataFromIdentifier( function->getIdentifier() )->setChangedVersion( this->currentGraph->getCurrentVersion() );
            break;
        default:
            qDebug() << "CHYBA V" << file->getIdentifier() << "->" << function->getIdentifier();
            break;
        }

        for( QMap<QString, Repository::Git::GitFunction*>::iterator iter =  function->getFunctionCallers()->begin(); iter != function->getFunctionCallers()->end(); ++iter ) {
            Repository::Git::GitFunction* innerFunction = iter.value();

            switch( innerFunction->getType() ) {
            case Repository::Git::GitType::ADDED :

                if( next ) {
//                    qDebug() << "ADDED" << file->getIdentifier() << "->" << function->getIdentifier() << "->" << innerFunction->getIdentifier();
                    addFunctionToGraph( innerFunction, function->getIdentifier() );
                } else {
//                    qDebug() << "REMOVED" << file->getIdentifier() << "->" << function->getIdentifier() << "->" << innerFunction->getIdentifier();
                    removeFunctionFromGraph( innerFunction, function->getIdentifier(), next );
                }
                break;
            case Repository::Git::GitType::REMOVED :

                if( next ) {
//                    qDebug() << "REMOVED" << file->getIdentifier() << "->" << function->getIdentifier() << "->" << innerFunction->getIdentifier();
                    removeFunctionFromGraph( innerFunction, function->getIdentifier(), next );
                } else {
//                    qDebug() << "ADDED" << file->getIdentifier() << "->" << function->getIdentifier() << "->" << innerFunction->getIdentifier();
                    addFunctionToGraph( innerFunction, function->getIdentifier() );
                }
                break;
            case Repository::Git::GitType::MODIFIED :
//                qDebug() << "MODIFIED" << file->getIdentifier() << "->" << function->getIdentifier() << "->" << innerFunction->getIdentifier();
                this->evolutionGraph->addChangedNodeOrEdge( innerFunction->getIdentifier(), Repository::Git::GitType::MODIFIED );
            this->evolutionGraph->getMetaDataFromIdentifier( innerFunction->getIdentifier() )->setChangedVersion( this->currentGraph->getCurrentVersion() );
                break;
            default:
                qDebug() << "CHYBA V" << file->getIdentifier() << "->" << function->getIdentifier() << "->"  << innerFunction->getIdentifier();
                break;
            }
        }
    }

    return true;
}

bool Repository::Git::GitLuaGraphVisualizer::addFunctionToGraph( Repository::Git::GitFunction *function, QString masterIdentifier ) {

    osg::ref_ptr<Data::Node> masterNode = this->currentGraph->findNodeByLuaIdentifier( masterIdentifier );

    Lua::LuaNode* luaNode = this->luaGraph->findNodeByLuaIdentifier( function->getIdentifier() );
    osg::ref_ptr<Data::Node> node = this->currentGraph->findNodeByLuaIdentifier( function->getIdentifier() );
    if( !node ) {
        if( !luaNode ) {
            qDebug() << "id" << function->getId() << function->getIdentifier() << function->getTypeAsString() << function->getFunctionTypeAsString();
        }
        node =  this->currentGraph->addNode( luaNode->getId(), luaNode->getLabel(), this->currentGraph->getTypesByName( "node" ).at( 0 ) );
//        node =  this->currentGraph->addNode( luaNode->getLabel(), this->currentGraph->getTypesByName( "node" ).at( 0 ) );
        node->setLuaIdentifier( function->getIdentifier() );
        this->evolutionGraph->getMetaDataFromIdentifier( function->getIdentifier() )->setFirstOccurence( this->currentGraph->getCurrentVersion() );

        if( !QString::compare( QString::fromStdString( luaNode->getParams()["type"].asString() ), "function" ) ) {
            QString metaEdge = "meta+" + luaNode->getIdentifier();
            osg::ref_ptr<Data::Edge> metaLink = this->currentGraph->addEdge( metaEdge, node, this->currentGraph->getMetaNodes()->value( std::numeric_limits<qlonglong>::max() - 1 ), this->currentGraph->getEdgeMetaType(), false );
            metaLink->setLuaIdentifier( metaEdge );
            metaLink->setEdgeColor( osg::Vec4( 0, 0, 0, 0 ) );
            metaLink->setInvisible( true );
            metaLink->setEdgeStrength( 0.1f );
        }
    } else {
        node->setType( this->currentGraph->getTypesByName( "node" ).at( 0 ) );
        this->evolutionGraph->removeChangedNodeOrEdge( function->getIdentifier() );
    }

    this->evolutionGraph->addChangedNodeOrEdge( luaNode->getIdentifier(), Repository::Git::GitType::ADDED );
    this->evolutionGraph->getMetaDataFromIdentifier( luaNode->getIdentifier() )->setChangedVersion( this->currentGraph->getCurrentVersion() );

    this->evolutionGraph->getMetaDataFromIdentifier( luaNode->getIdentifier() )->setLuaMapping( luaNode->getId() );

    if( this->isChangingRepresentation ) {
//        this->evolutionGraph->getMetaDataFromIdentifier( luaNode->getIdentifier() )->increaseOccurence();
    }
    QString edgeIdentifier = masterIdentifier + "+" + function->getIdentifier();

    Lua::LuaEdge* luaEdge = this->luaGraph->findEdgeByLuaIdentifier( edgeIdentifier );
    osg::ref_ptr<Data::Edge> edge = this->currentGraph->findEdgeByLuaIdentifier( edgeIdentifier );
    if( !edge ) {
        Lua::LuaIncidence* incidence = this->luaGraph->getIncidences()->value( luaEdge->getIncidences().at( 0 ) );
        if( incidence->getOriented() ) {
            if( incidence->getOutGoing() ) {
                edge = this->currentGraph->addEdge( luaEdge->getId(), luaEdge->getLabel(), node, masterNode, this->currentGraph->getTypesByName( "edge" ).at( 0 ), true );
//                edge = this->currentGraph->addEdge( luaEdge->getLabel(), node, masterNode, this->currentGraph->getTypesByName( "edge" ).at( 0 ), true );
            } else {
                edge = this->currentGraph->addEdge( luaEdge->getId(), luaEdge->getLabel(), masterNode, node, this->currentGraph->getTypesByName( "edge" ).at( 0 ), true );
//                edge = this->currentGraph->addEdge( luaEdge->getLabel(), masterNode, node, this->currentGraph->getTypesByName( "edge" ).at( 0 ), true );
            }
        } else {
            edge = this->currentGraph->addEdge( luaEdge->getId(), luaEdge->getLabel(), masterNode, node, this->currentGraph->getTypesByName( "edge" ).at( 0 ), false );
//            edge = this->currentGraph->addEdge( luaEdge->getLabel(), masterNode, node, this->currentGraph->getTypesByName( "edge" ).at( 0 ), false );
        }
        edge->setLuaIdentifier( edgeIdentifier );
        edge->setCamera( this->camera );
        this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setFirstOccurence( this->currentGraph->getCurrentVersion() );
    } else {
        this->evolutionGraph->removeChangedNodeOrEdge( edgeIdentifier );
    }

    this->evolutionGraph->addChangedNodeOrEdge( edgeIdentifier, Repository::Git::GitType::ADDED );
    this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setChangedVersion( this->currentGraph->getCurrentVersion() );

    if( showLuaStats ) {
        setEdgeParams( edge, luaEdge, osg::Vec4f( 1, 1, 1, 1 ) );
    }

    this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setLuaMapping( luaEdge->getId() );

    if( function->getFunctionType() == Repository::Git::GitFunctionType::GLOBALFUNCTION && function->getModule() != "" ) {
        addModuleFromGlobalFunction( function );
    }

    return true;
}

bool Repository::Git::GitLuaGraphVisualizer::addModuleFromGlobalFunction( Repository::Git::GitFunction *function ) {

    osg::ref_ptr<Data::Node> functionNode = this->currentGraph->findNodeByLuaIdentifier( function->getIdentifier() );
    if( !functionNode ) {
        qDebug() << "AddModuleFromGlobalFunction neexistuje node pre global function";
        return false;
    }

    QString moduleIdentifier =  "globalModule;" + function->getModule();

    Lua::LuaNode* luaModuleNode = this->luaGraph->findNodeByLuaIdentifier( moduleIdentifier );
    osg::ref_ptr<Data::Node> moduleNode = this->currentGraph->findNodeByLuaIdentifier( moduleIdentifier );
    if( !moduleNode ) {
        moduleNode = this->currentGraph->addNode( luaModuleNode->getId(), luaModuleNode->getLabel(), this->currentGraph->getTypesByName( "node" ).at( 0 ) );
//        moduleNode = this->currentGraph->addNode( luaModuleNode->getLabel(), this->currentGraph->getTypesByName( "node" ).at( 0 ) );
        moduleNode->setLuaIdentifier( moduleIdentifier );
        this->evolutionGraph->getMetaDataFromIdentifier( moduleIdentifier )->setFirstOccurence( this->currentGraph->getCurrentVersion() );
    } else {
        moduleNode->setType( this->currentGraph->getTypesByName( "node" ).at( 0 ) );
        this->evolutionGraph->removeChangedNodeOrEdge( moduleIdentifier );
    }

    this->evolutionGraph->addChangedNodeOrEdge( luaModuleNode->getIdentifier(), Repository::Git::GitType::ADDED );
    this->evolutionGraph->getMetaDataFromIdentifier( luaModuleNode->getIdentifier() )->setChangedVersion( this->currentGraph->getCurrentVersion() );

    this->evolutionGraph->getMetaDataFromIdentifier( luaModuleNode->getIdentifier() )->setLuaMapping( luaModuleNode->getId() );

    if( this->isChangingRepresentation ) {
//        this->evolutionGraph->getMetaDataFromIdentifier( luaModuleNode->getIdentifier() )->increaseOccurence();
    }

    QString edgeIdentifier =  function->getIdentifier() + "+" + moduleNode->getLuaIdentifier();

    Lua::LuaEdge* luaEdge = this->luaGraph->findEdgeByLuaIdentifier( edgeIdentifier );
    osg::ref_ptr<Data::Edge> edge = this->currentGraph->findEdgeByLuaIdentifier( edgeIdentifier );
    if( !edge ) {
        edge = this->currentGraph->addEdge( luaEdge->getId(), luaEdge->getLabel(), functionNode, moduleNode, this->currentGraph->getTypesByName( "edge" ).at( 0 ), false );
//        edge = this->currentGraph->addEdge( luaEdge->getLabel(), functionNode, moduleNode, this->currentGraph->getTypesByName( "edge" ).at( 0 ), false );
        edge->setLuaIdentifier( edgeIdentifier );
        edge->setCamera( this->camera );
    } else {
        this->evolutionGraph->removeChangedNodeOrEdge( edgeIdentifier );
    }

    this->evolutionGraph->addChangedNodeOrEdge( edgeIdentifier, Repository::Git::GitType::ADDED );
    this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setChangedVersion( this->currentGraph->getCurrentVersion() );

    if( showLuaStats ) {
        setEdgeParams( edge, luaEdge, osg::Vec4f( 1, 1, 1, 1 ) );
    }
    this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setLuaMapping( luaEdge->getId() );

    return true;
}

bool Repository::Git::GitLuaGraphVisualizer::removeFileFromGraph( Repository::Git::GitFile* file, bool next ) {
    QStringList list = file->getFilepath().split( "/" );

    QString string = "";
    for( int i = 0; i < list.size(); i++ ) {
        string += list.at( i );
        list.replace( i, string );
        string += "/";
    }

    for( int i = 0; i < list.size(); i++ ) {
        QString newIdentifier = "";
        if( i < list.size() - 1 ) {
            newIdentifier = "directory;" + list.at( i );
        } else {
            newIdentifier = "file;" + list.at( i );
        }

        if( this->isChangingRepresentation ) {
//            this->evolutionGraph->getMetaDataFromIdentifier( newIdentifier )->increaseOccurence();
        }

        int occurence = this->evolutionGraph->getMetaDataFromIdentifier( newIdentifier )->decreaseOccurence();

//        qDcd ebug() << occurence << newIdentifier;

        if( occurence <= 0 ) {
//            qDebug() << newIdentifier << "stored to removedFiles";
            if( next ) {
                this->evolutionGraph->getMetaDataFromIdentifier( newIdentifier )->setChangedVersion( this->currentGraph->getCurrentVersion() );
                this->evolutionGraph->addChangedNodeOrEdge( newIdentifier, Repository::Git::GitType::REMOVED );
            } else {
                this->evolutionGraph->getMetaDataFromIdentifier( newIdentifier )->clearChangedVersion();
                this->evolutionGraph->addChangedNodeOrEdge( newIdentifier, Repository::Git::GitType::REMOVED );
            }
        }
    }

    return true;
}

bool Repository::Git::GitLuaGraphVisualizer::removeFunctionFromGraph( Repository::Git::GitFunction *function, QString masterIdentifier, bool next ) {

    if( function->getFunctionType() == Repository::Git::GitFunctionType::GLOBALFUNCTION && function->getModule() != "" ) {
        removeModuleFromGlobalFunction( function, next );
    }

    if( next ){
        QString edgeIdentifier = masterIdentifier + "+" + function->getIdentifier();
        this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setChangedVersion( this->currentGraph->getCurrentVersion() );
        this->evolutionGraph->addChangedNodeOrEdge( edgeIdentifier, Repository::Git::GitType::REMOVED );
    }

    if( this->isChangingRepresentation ) {
//        this->evolutionGraph->getMetaDataFromIdentifier( function->getIdentifier() )->increaseOccurence();
    }

    int occurence = this->evolutionGraph->getMetaDataFromIdentifier( function->getIdentifier() )->decreaseOccurence();

//    qDebug() << occurence << function->getIdentifier();

    if( occurence <= 0 ) {
//        qDebug() << function->getIdentifier() << "stored to removedFiles";
        if( next ) {
            this->evolutionGraph->getMetaDataFromIdentifier( function->getIdentifier() )->setChangedVersion( this->currentGraph->getCurrentVersion() );
            this->evolutionGraph->addChangedNodeOrEdge( function->getIdentifier(), Repository::Git::GitType::REMOVED );
        } else {
            this->evolutionGraph->getMetaDataFromIdentifier( function->getIdentifier() )->clearChangedVersion();
            this->evolutionGraph->addChangedNodeOrEdge( function->getIdentifier(), Repository::Git::GitType::REMOVED );
        }
    }


    return true;
}

bool Repository::Git::GitLuaGraphVisualizer::removeModuleFromGlobalFunction( Repository::Git::GitFunction *function, bool next ) {

    QString moduleIdentifier =  "globalModule;" + function->getModule();

    if( next ) {
        QString edgeIdentifier = function->getIdentifier() + "+" + moduleIdentifier;
        this->evolutionGraph->getMetaDataFromIdentifier( edgeIdentifier )->setChangedVersion( this->currentGraph->getCurrentVersion() );
        this->evolutionGraph->addChangedNodeOrEdge( edgeIdentifier, Repository::Git::GitType::REMOVED );
    }

    if( this->isChangingRepresentation ) {
//        this->evolutionGraph->getMetaDataFromIdentifier( moduleIdentifier )->increaseOccurence();
    }

    int occurence = this->evolutionGraph->getMetaDataFromIdentifier( moduleIdentifier )->decreaseOccurence();

//    qDebug() << occurence << moduleIdentifier;

    if( occurence <= 0 ) {
//        qDebug() << moduleIdentifier << "stored to removedFiles";
        if( next ) {
            this->evolutionGraph->getMetaDataFromIdentifier( moduleIdentifier )->setChangedVersion( this->currentGraph->getCurrentVersion() );
            this->evolutionGraph->addChangedNodeOrEdge( moduleIdentifier, Repository::Git::GitType::REMOVED );
        } else {
            this->evolutionGraph->getMetaDataFromIdentifier( moduleIdentifier )->clearChangedVersion();
            this->evolutionGraph->addChangedNodeOrEdge( moduleIdentifier, Repository::Git::GitType::REMOVED );
        }
    }

    return true;
}

void Repository::Git::GitLuaGraphVisualizer::processChangedNodesAndEdges() {
    QList<QString> removedNodesAndEdges = QList<QString>();

    for( QMap<QString, Repository::Git::GitType>::iterator iterator = this->evolutionGraph->getChangedNodesAndEdge()->begin(); iterator != this->evolutionGraph->getChangedNodesAndEdge()->end(); ++iterator ) {
        QString identifier = iterator.key();
        bool passedLifespan = false;
        int difference;

//        qDebug() << identifier << iterator.value() << this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getChangedVersion();

        switch( iterator.value() ) {
        case Repository::Git::GitType::ADDED:
            // Ak ide o identifikator uzla, tak sa index rovna -1, inak ide o hranu
            if( identifier.indexOf( "+" ) == -1 ) {
                osg::ref_ptr<Data::Node> node = this->currentGraph->findNodeByLuaIdentifier( identifier );

                // Ak sa nemaju zobrazovat Lua udaje v grafe, upravim zobrazenie uzla
                if( !this->showLuaStats ) {
                    // Ak sa zmenena verzia rovna sucasnej verzii, tak zmenim tvar node na +, inak vratim povodnu reprezentaciu uzla
                    if( this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getChangedVersion() == this->currentGraph->getCurrentVersion() ) {
                        if( this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getFirstOccurence() == this->currentGraph->getCurrentVersion() ) {
                            node->setType( this->currentGraph->getTypesByName( "addedNode" ).at( 0 ) );
                            node->setColor( osg::Vec4f( 0, 1, 0, 1 ) );
                        } else {
                            node->setType( this->currentGraph->getTypesByName( "clearNode" ).at( 0 ) );
                            node->setColor( osg::Vec4f( 1, 1, 1, 1 ) );
                        }
                    } else {
                        node->setType( this->currentGraph->getTypesByName( "clearNode" ).at( 0 ) );
                        node->setColor( osg::Vec4f( 1, 1, 1, 1 ) );
                        removedNodesAndEdges.append( identifier );
                    }

                    node->reloadConfig();
                    node->showLabel( true );
                } else {
                    Lua::LuaNode* luaNode = this->luaGraph->findNodeByLuaIdentifier( identifier );
                    if( luaNode ) {
                        setNodeParams( node, luaNode, osg::Vec4f( 1, 1, 1, 1 ), 8 );
                    }
                }

            } else {
                osg::ref_ptr<Data::Edge> edge = this->currentGraph->findEdgeByLuaIdentifier( identifier );
                if( !this->showLuaStats && edge ) {
                    if( this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getChangedVersion() == this->currentGraph->getCurrentVersion() ) {
                        edge->setEdgeColor( osg::Vec4f( 0, 1, 0, 1 ) );
                    } else {
                        edge->setEdgeColor( osg::Vec4f( 1, 1, 1, 1 ) );
                        removedNodesAndEdges.append( identifier );
                    }
                }
            }

            break;
        case Repository::Git::GitType::MODIFIED:

            if( identifier.indexOf( "+" ) == -1 ) {
                osg::ref_ptr<Data::Node> node =  this->currentGraph->findNodeByLuaIdentifier( identifier );
                if( !this->showLuaStats && node ) {
                    if( this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getChangedVersion() == this->currentGraph->getCurrentVersion() ) {
                        node->setType( this->currentGraph->getTypesByName( "modifiedNode" ).at( 0 ) );
                        node->setColor( osg::Vec4f( 1, 1, 0, 1 ) );
                    } else {
                        node->setType( this->currentGraph->getTypesByName( "clearNode" ).at( 0 ) );
                        node->setColor( osg::Vec4f( 1, 1, 1, 1 ) );
                        removedNodesAndEdges.append( identifier );
                    }
                    node->reloadConfig();
                    node->showLabel( true );
                } else {
                    Lua::LuaNode* luaNode = this->luaGraph->findNodeByLuaIdentifier( identifier );
                    if( luaNode ) {
                        setNodeParams( node, luaNode, osg::Vec4f( 1, 1, 1, 1 ), 8 );
                    }
                }
            }

            break;
        case Repository::Git::GitType::REMOVED:
            difference = this->currentGraph->getCurrentVersion() - this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getChangedVersion();
//            qDebug() << identifier << this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getChangedVersion();
            if( this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getChangedVersion() != -1 ) {
                if( difference >= this->evolutionGraph->getLifespan() ) {
                    passedLifespan = true;
                }
            } else {
                passedLifespan = true;
            }

            // Ak sa nenachadza v identifikatori "+" takide o node, inak edge
            if( iterator.key().indexOf( "+" ) == -1 ) {
                osg::ref_ptr<Data::Node> node = this->currentGraph->findNodeByLuaIdentifier( iterator.key() );

//                qDebug() << this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getOccurence() << iterator.key();

                if( this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getOccurence() <= 0 ) {
//                    qDebug() << this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getOccurence() << iterator.key();
                    if( passedLifespan ) {
                        this->currentGraph->removeNode( node );
                        removedNodesAndEdges.append( identifier );
                    } else {
                        node->setType( this->currentGraph->getTypesByName( "removedNode" ).at( 0 ) );
                        node->setColor( osg::Vec4f( 1, 1, 1, 1 ) );
                        node->reloadConfig();
                        node->showLabel( true );
                    }
                } else {
//                    qDebug() << this->evolutionGraph->getMetaDataFromIdentifier( identifier )->getOccurence() << iterator.key();
                    removedNodesAndEdges.append( identifier );
                }
            } else {
                osg::ref_ptr<Data::Edge> edge = this->currentGraph->findEdgeByLuaIdentifier( identifier );
                if( !passedLifespan ) {
                    edge->setEdgeColor( osg::Vec4f( 1, 0, 0, 1 ) );
                } else {
                    this->currentGraph->removeEdge( edge);
                    removedNodesAndEdges.append( identifier );
                }
            }

            break;
        case Repository::Git::GitType::NONE:

            break;
        default:
            qDebug() << "Unknown GitType";
            return;
        }
    }

    foreach( QString identifier, removedNodesAndEdges ) {
        this->evolutionGraph->removeChangedNodeOrEdge( identifier );
        this->evolutionGraph->getMetaDataFromIdentifier( identifier )->clearChangedVersion();
    }

}

void Repository::Git::GitLuaGraphVisualizer::changeNodeRepresentation( bool showLuaStats ) {

//    reloadNodeRepresentation( showLuaStats );

    this->isChangingRepresentation = true;
    visualize( true );
    this->isChangingRepresentation = false;

}

void Repository::Git::GitLuaGraphVisualizer::reloadNodeRepresentation( bool showLuaStats ) {
    for( QMap<qlonglong, osg::ref_ptr<Data::Node>>::iterator iterator = this->currentGraph->getNodes()->begin(); iterator != this->currentGraph->getNodes()->end(); ++iterator ) {
        if( showLuaStats ) {
            Lua::LuaNode* luaNode = this->luaGraph->getNodes()->value( iterator.value()->getId() );
            if( luaNode ) {
                setNodeParams( iterator.value(), luaNode, osg::Vec4f( 1, 1, 1, 1 ), 8 );
//                iterator.value()->reloadConfig();
            }
        } else {
            iterator.value()->setColor( osg::Vec4f( 1, 1, 1, 1 ) );
            iterator.value()->reloadConfig();
        }
    }
    for( QMap<qlonglong, osg::ref_ptr<Data::Edge>>::iterator iterator = this->currentGraph->getEdges()->begin(); iterator != this->currentGraph->getEdges()->end(); ++iterator ) {
        if( showLuaStats ) {
            Lua::LuaEdge* luaEdge = this->luaGraph->findEdgeByLuaIdentifier( iterator.value()->getLuaIdentifier() );
            if( luaEdge ) {
                setEdgeParams( iterator.value(), luaEdge, osg::Vec4f( 1, 1, 1, 1 ) );
            }
        } else {
            iterator.value()->setEdgeColor( osg::Vec4f( 1, 1, 1, 1 ) );
//            iterator.value()->reloadConfig();
        }
    }
}

void Repository::Git::GitLuaGraphVisualizer::updateCurrentGraphNodesId() {
//    qDebug() << "UPDATE current graph Nodes";
    for( QMap<qlonglong, osg::ref_ptr<Data::Node>>::iterator iterator = this->currentGraph->getNodes()->begin(); iterator != this->currentGraph->getNodes()->end(); ++iterator ) {
        Lua::LuaNode* node = this->luaGraph->findNodeByLuaIdentifier( iterator.value()->getLuaIdentifier() );
        if( this->showLuaStats ) {
            if( node ) {
                setNodeParams( iterator.value(), node, osg::Vec4f( 1, 1, 1, 1 ), 8 );
            }
        } else {
            iterator.value()->setType( this->currentGraph->getTypesByName( "clearNode" ).at( 0 ) );
            iterator.value()->reloadConfig();
        }

        if( node ) {
            iterator.value()->setId( node->getId() );
            this->evolutionGraph->getMetaDataFromIdentifier( node->getIdentifier() )->setLuaMapping( node->getId() );
        }
    }
}

void Repository::Git::GitLuaGraphVisualizer::setNodeParams( osg::ref_ptr<Data::Node> node, Lua::LuaGraphObject *obj, osg::Vec4f defColor, float defSize ) {
    node->setType( this->currentGraph->getTypesByName( "node" ).at( 0 ) );
    node->setLabelText( obj->getLabel() );
    node->Data::AbsNode::setName( obj->getLabel() );
    float r = obj->getFloatParam( "colorR", defColor.r() );
    float g = obj->getFloatParam( "colorG", defColor.g() );
    float b = obj->getFloatParam( "colorB", defColor.b() );
    float a = obj->getFloatParam( "colorA", defColor.a() );
    node->setColor( osg::Vec4( r,g,b,a ) );
    float scale = obj->getFloatParam( "size", defSize );
    node->setScale( scale );
    node->reloadConfig();
    node->showLabel( true );
}

void Repository::Git::GitLuaGraphVisualizer::setEdgeParams( osg::ref_ptr<Data::Edge> edge, Lua::LuaGraphObject *obj, osg::Vec4f defColor) {
    edge->Data::AbsEdge::setName( obj->getLabel() );
    float r = obj->getFloatParam( "colorR", defColor.r() );
    float g = obj->getFloatParam( "colorG", defColor.g() );
    float b = obj->getFloatParam( "colorB", defColor.b() );
    float a = obj->getFloatParam( "colorA", defColor.a() );
    edge->setEdgeColor( osg::Vec4( r,g,b,a ) );
    edge->setEdgeStrength( obj->getFloatParam( "edgeStrength", 1 ) );
//    edge->setEdgeDefaultColor();
    edge->reloadColor();
}

void Repository::Git::GitLuaGraphVisualizer::addDefaultTypes() {
    this->currentGraph->addType( "edge" );
    this->currentGraph->addType( "node" );
}

void Repository::Git::GitLuaGraphVisualizer::addCustomTypes() {
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
