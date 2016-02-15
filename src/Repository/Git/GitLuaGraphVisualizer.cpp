#include "Repository/Git/GitLuaGraphVisualizer.h"

#include "Data/Graph.h"

#include "GitLib/GitVersion.h"
#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitFile.h"
#include "GitLib/GitFunction.h"

#include "LuaGraph/LuaGraph.h"

#include <QMap>
#include <QStringList>

Repository::Git::GitLuaGraphVisualizer::GitLuaGraphVisualizer( Data::Graph *currentGraph, Repository::Git::GitEvolutionGraph *evolutionGraph, osg::ref_ptr<osg::Camera> camera )
        : currentGraph( currentGraph ), evolutionGraph( evolutionGraph ), luaGraph( Lua::LuaGraph::getInstance() ), camera( camera ) {
        if( this->currentGraph->getTypes()->size() == 0 ) {
            addDefaultTypes();
            addCustomTypes();
        }
}

void Repository::Git::GitLuaGraphVisualizer::visualize( bool next ) {
    // ziskame verziu nastavenu vo vizualizovanom grafe
    int currentVersion = this->currentGraph->getCurrentVersion();

    // ak je next false, tak zvysime verziu ( potrebujeme dalsiu verziu o sucasnej, aby sme vedeli, co vymanazat/pridat )
    if( !next ) {
        currentVersion++;
    }

    Repository::Git::GitVersion* version = this->evolutionGraph->getVersion( currentVersion );
    QMap<QString, Repository::Git::GitFile*> changedFiles = version->getChangedFiles();

    QString rootIdentifier = "directory;" + this->evolutionGraph->getFilePath();

    Lua::LuaNode* luaRoot = luaGraph->findNodeByLuaIdentifier( rootIdentifier );

    osg::ref_ptr<Data::Node> root = this->currentGraph->findNodeByLuaIdentifier( rootIdentifier );

    if( !root ) {
        qDebug() << "Vytvaram root uzol v grafe";
        root = this->currentGraph->addNode( luaRoot->getId(), this->evolutionGraph->getFilePath(), currentGraph->getTypesByName( "node" ).at( 0 ) );
        root->setLuaIdentifier( rootIdentifier );
    }

    this->evolutionGraph->addLuaNodesMapping( rootIdentifier, luaRoot->getId() );

    for( QMap<QString, Repository::Git::GitFile*>::iterator iterator = changedFiles.begin(); iterator != changedFiles.end(); ++iterator ) {

        switch( iterator.value()->getType() ) {
        case Repository::Git::GitType::ADDED:
            if( next ) {
                addFileToGraph( iterator.value(), rootIdentifier );
            } else {
                removeFileFromGraph( iterator.value(), next );
            }
            break;
        case Repository::Git::GitType::REMOVED:
            if( next ) {
                removeFileFromGraph( iterator.value(), next );
            } else {
                addFileToGraph( iterator.value(), rootIdentifier );
            }
            break;
        case Repository::Git::GitType::MODIFIED:
//            qDebug() << "MODIFYING" << iterator.value()->getIndetifier();
            break;
        default:
            qDebug() << "CHYBNY TYP PRE SUBOR" << iterator.value()->getIndetifier();
            break;
        }

        processFunctionsFromFile( iterator.value(), next );
    }

    processRemovedNodesAndEdges();
    updateCurrentGraphNodesId();
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
            newNode = this->currentGraph->addNode( luaNode->getId(), luaNode->getLabel(), this->currentGraph->getTypesByName( "node" ).at( 0 ) );
            newNode->setLuaIdentifier( newIdentifier );    
        } else {
            newNode->setType( this->currentGraph->getTypesByName( "node" ).at( 0 ) );
            this->evolutionGraph->removeRemovedNodeOrEdge( newIdentifier );
        }
        this->evolutionGraph->addLuaNodesMapping( newIdentifier, luaNode->getId() );

        this->evolutionGraph->addNodeOccurence( newIdentifier );

        QString edgeIdentifier =  identifier + "+" + newIdentifier;

        Lua::LuaEdge* luaEdge = this->luaGraph->findEdgeByLuaIdentifier( edgeIdentifier );
        osg::ref_ptr<Data::Edge> newEdge = this->currentGraph->findEdgeByLuaIdentifier( edgeIdentifier );
        if( !newEdge ) {
//            qDebug() << "ADDING edge to Graph" << edgeIdentifier;
            newEdge = this->currentGraph->addEdge( luaEdge->getId(), luaEdge->getLabel(), node, newNode, this->currentGraph->getTypesByName( "edge" ).at( 0 ), false );
            newEdge->setLuaIdentifier( edgeIdentifier );
            newEdge->setCamera( this->camera );
        }
        this->evolutionGraph->addLuaEdgesMapping( edgeIdentifier, luaEdge->getId() );
        setEdgeParams( newEdge, luaEdge, osg::Vec4f( 1, 1, 1, 1 ) );

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
                qDebug() << "ADDED" << file->getIndetifier() << "->" << function->getIdentifier();
                addFunctionToGraph( function, file->getIndetifier() );
            } else {
                qDebug() << "REMOVED" << file->getIndetifier() << "->" << function->getIdentifier();
                removeFunctionFromGraph( function, file->getIndetifier(), next );
            }
            break;

        case Repository::Git::GitType::REMOVED :

            if( next ) {
                qDebug() << "REMOVED" << file->getIndetifier() << "->" << function->getIdentifier();
                removeFunctionFromGraph( function, file->getIndetifier(), next );
            } else {
                qDebug() << "ADDED" << file->getIndetifier() << "->" << function->getIdentifier();
                addFunctionToGraph( function, file->getIndetifier() );
            }
            break;

        case Repository::Git::GitType::MODIFIED :
//            qDebug() << "MODIFIED" << file->getIndetifier() << "->" << function->getIdentifier();
            break;
        default:
            qDebug() << "CHYBA V" << file->getIndetifier() << "->" << function->getIdentifier();
            break;
        }

        for( QMap<QString, Repository::Git::GitFunction*>::iterator iter =  function->getFunctionCallers()->begin(); iter != function->getFunctionCallers()->end(); ++iter ) {
            Repository::Git::GitFunction* innerFunction = iter.value();

            switch( innerFunction->getType() ) {
            case Repository::Git::GitType::ADDED :

                if( next ) {
                    qDebug() << "ADDED" << file->getIndetifier() << "->" << function->getIdentifier() << "->" << innerFunction->getIdentifier();
                    addFunctionToGraph( innerFunction, function->getIdentifier() );
                } else {
                    qDebug() << "REMOVED" << file->getIndetifier() << "->" << function->getIdentifier() << "->" << innerFunction->getIdentifier();
                    removeFunctionFromGraph( innerFunction, function->getIdentifier(), next );
                }
                break;
            case Repository::Git::GitType::REMOVED :

                if( next ) {
                    qDebug() << "REMOVED" << file->getIndetifier() << "->" << function->getIdentifier() << "->" << innerFunction->getIdentifier();
                    removeFunctionFromGraph( innerFunction, function->getIdentifier(), next );
                } else {
                    qDebug() << "ADDED" << file->getIndetifier() << "->" << function->getIdentifier() << "->" << innerFunction->getIdentifier();
                    addFunctionToGraph( innerFunction, function->getIdentifier() );
                }
                break;
            case Repository::Git::GitType::MODIFIED :
//                qDebug() << "MODIFIED" << file->getIndetifier() << "->" << function->getIdentifier() << "->" << innerFunction->getIdentifier();
                break;
            default:
                qDebug() << "CHYBA V" << file->getIndetifier() << "->" << function->getIdentifier() << "->"  << innerFunction->getIdentifier();
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
        node->setLuaIdentifier( luaNode->getIdentifier() );
    } else {
        node->setType( this->currentGraph->getTypesByName( "node" ).at( 0 ) );
        this->evolutionGraph->removeRemovedNodeOrEdge( function->getIdentifier() );
    }
    this->evolutionGraph->addLuaNodesMapping( luaNode->getIdentifier(), luaNode->getId() );

    this->evolutionGraph->addNodeOccurence( luaNode->getIdentifier() );

    QString edgeIdentifier = masterIdentifier + "+" + function->getIdentifier();

    Lua::LuaEdge* luaEdge = this->luaGraph->findEdgeByLuaIdentifier( edgeIdentifier );
    osg::ref_ptr<Data::Edge> edge = this->currentGraph->findEdgeByLuaIdentifier( edgeIdentifier );
    if( !edge ) {
        Lua::LuaIncidence* incidence = this->luaGraph->getIncidences()->value( luaEdge->getIncidences().at( 0 ) );
        if( incidence->getOriented() ) {
            if( incidence->getOutGoing() ) {
                edge = this->currentGraph->addEdge( luaEdge->getId(), luaEdge->getLabel(), node, masterNode, this->currentGraph->getTypesByName( "edge" ).at( 0 ), true );
            } else {
                edge = this->currentGraph->addEdge( luaEdge->getId(), luaEdge->getLabel(), masterNode, node, this->currentGraph->getTypesByName( "edge" ).at( 0 ), true );
            }
        } else {
            edge = this->currentGraph->addEdge( luaEdge->getId(), luaEdge->getLabel(), masterNode, node, this->currentGraph->getTypesByName( "edge" ).at( 0 ), false );
        }
        edge->setLuaIdentifier( edgeIdentifier );
        edge->setCamera( this->camera );
    } else {
        this->evolutionGraph->removeRemovedNodeOrEdge( edgeIdentifier );
    }

    setEdgeParams( edge, luaEdge, osg::Vec4f( 1, 1, 1, 1 ) );

    this->evolutionGraph->addLuaEdgesMapping( edgeIdentifier, luaEdge->getId() );

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
        moduleNode->setLuaIdentifier( luaModuleNode->getIdentifier() );
    } else {
        moduleNode->setType( this->currentGraph->getTypesByName( "node" ).at( 0 ) );
        this->evolutionGraph->removeRemovedNodeOrEdge( moduleIdentifier );
    }

    this->evolutionGraph->addLuaNodesMapping( luaModuleNode->getIdentifier(), luaModuleNode->getId() );
    this->evolutionGraph->addNodeOccurence( luaModuleNode->getIdentifier() );

    QString edgeIdentifier =  function->getIdentifier() + "+" + moduleNode->getLuaIdentifier();

    Lua::LuaEdge* luaEdge = this->luaGraph->findEdgeByLuaIdentifier( edgeIdentifier );
    osg::ref_ptr<Data::Edge> edge = this->currentGraph->findEdgeByLuaIdentifier( edgeIdentifier );
    if( !edge ) {
        edge = this->currentGraph->addEdge( luaEdge->getId(), luaEdge->getLabel(), functionNode, moduleNode, this->currentGraph->getTypesByName( "edge" ).at( 0 ), false );
        edge->setLuaIdentifier( edgeIdentifier );
        edge->setCamera( this->camera );
    } else {
        this->evolutionGraph->removeRemovedNodeOrEdge( edgeIdentifier );
    }

    setEdgeParams( edge, luaEdge, osg::Vec4f( 1, 1, 1, 1 ) );

    this->evolutionGraph->addLuaEdgesMapping( edgeIdentifier, luaEdge->getId() );

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

        if( this->evolutionGraph->removeNodeOccurence( newIdentifier ) <= 0 ) {
//            qDebug() << newIdentifier << "stored to removedFiles";
            if( next ) {
                this->evolutionGraph->addRemovedNodeOrEdge( newIdentifier, this->currentGraph->getCurrentVersion() );
            } else {
                this->evolutionGraph->addRemovedNodeOrEdge( newIdentifier, -1 );
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
        this->evolutionGraph->addRemovedNodeOrEdge( edgeIdentifier, this->currentGraph->getCurrentVersion() );
    }
    if( this->evolutionGraph->removeNodeOccurence( function->getIdentifier() ) <= 0 ) {
//        qDebug() << function->getIdentifier() << "stored to removedFiles";
        if( next ) {
            this->evolutionGraph->addRemovedNodeOrEdge( function->getIdentifier(), this->currentGraph->getCurrentVersion() );
        } else {
            this->evolutionGraph->addRemovedNodeOrEdge( function->getIdentifier(), -1 );
        }
    }


    return true;
}

bool Repository::Git::GitLuaGraphVisualizer::removeModuleFromGlobalFunction( Repository::Git::GitFunction *function, bool next ) {

    QString moduleIdentifier =  "globalModule;" + function->getModule();

    if( next ) {
        QString edgeIdentifier = function->getIdentifier() + "+" + moduleIdentifier;
        this->evolutionGraph->addRemovedNodeOrEdge( edgeIdentifier, this->currentGraph->getCurrentVersion() );
    }
    if( this->evolutionGraph->removeNodeOccurence( moduleIdentifier ) <= 0 ) {
//        qDebug() << moduleIdentifier << "stored to removedFiles";
        if( next ) {
            this->evolutionGraph->addRemovedNodeOrEdge( moduleIdentifier, this->currentGraph->getCurrentVersion() );
        } else {
            this->evolutionGraph->addRemovedNodeOrEdge( moduleIdentifier, -1 );
        }
    }

    return true;
}

void Repository::Git::GitLuaGraphVisualizer::processRemovedNodesAndEdges() {
    QList<QString> removedNodesAndEdges = QList<QString>();

    for( QMap<QString, int>::iterator iterator = this->evolutionGraph->getRemovedNodesAndEdge()->begin(); iterator != this->evolutionGraph->getRemovedNodesAndEdge()->end(); ++iterator ) {
            QString identifier = iterator.key();
            bool passedLifespan = false;
            int difference = this->currentGraph->getCurrentVersion() - iterator.value();
            if( iterator.value() != -1 ) {
                if( difference >= this->evolutionGraph->getLifespan() ) {
                    passedLifespan = true;
                }
            } else {
                passedLifespan = true;
            }

            // Ak sa nenachadza v identifikatori "+" takide o node, inak edge
            if( iterator.key().indexOf( "+" ) == -1 ) {
                osg::ref_ptr<Data::Node> node = this->currentGraph->findNodeByLuaIdentifier( iterator.key() );

                if( this->evolutionGraph->getNodeOccurence().value( identifier ) <= 0 ) {
//                    qDebug() << "Occurence 0" << iterator.key();
                    if( passedLifespan ) {
                        this->currentGraph->removeNode( node );
                        removedNodesAndEdges.append( identifier );
                    } else {
                        node->setType( this->currentGraph->getTypesByName( "removedNode" ).at( 0 ) );
                        node->reloadConfig();
                        node->showLabel( true );
                    }
                } else {
//                    qDebug() << "occurence >0" << iterator.key();
                    removedNodesAndEdges.append( identifier );
                }
            } else {
                if( !passedLifespan ) {
                    osg::ref_ptr<Data::Edge> edge = this->currentGraph->findEdgeByLuaIdentifier( identifier );
                    edge->setEdgeColor( osg::Vec4f( 1, 0, 0, 1 ) );
                } else {
                    removedNodesAndEdges.append( identifier );
                }
            }



    }

    foreach( QString identifier, removedNodesAndEdges ) {
        this->evolutionGraph->removeRemovedNodeOrEdge( identifier );
    }

}

void Repository::Git::GitLuaGraphVisualizer::updateCurrentGraphNodesId() {
//    qDebug() << "UPDATE current graph Nodes";
    for( QMap<qlonglong, osg::ref_ptr<Data::Node>>::iterator iterator = this->currentGraph->getNodes()->begin(); iterator != this->currentGraph->getNodes()->end(); ++iterator ) {
//        qDebug() << "UPDATING Node" << iterator.value()->getLuaIdentifier();
        Lua::LuaNode* node = this->luaGraph->findNodeByLuaIdentifier( iterator.value()->getLuaIdentifier() );
        if( node ) {
            iterator.value()->setId( node->getId() );
            setNodeParams( iterator.value(), node, osg::Vec4f( 1, 1, 1, 1 ), 8 );
            this->evolutionGraph->addLuaNodesMapping( node->getIdentifier(), node->getId() );
        }
    }
}

void Repository::Git::GitLuaGraphVisualizer::setNodeParams( osg::ref_ptr<Data::Node> node, Lua::LuaGraphObject *obj, osg::Vec4f defColor, float defSize ) {
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
    settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
    settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.RemoveNode" ) );;
    this->currentGraph->addType( "removedNode", settings );

    settings = new QMap<QString, QString>;
    settings->insert( "color.R", "1" );
    settings->insert( "color.G", "0" );
    settings->insert( "color.B", "0" );
    settings->insert( "color.A", "1" );
    settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultEdgeScale" ) );
    settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Edge" ) );;
    this->currentGraph->addType( "removedEdge", settings );
}
