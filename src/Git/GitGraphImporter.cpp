#include "Git/GitGraphImporter.h"

#include "Importer/GraphOperations.h"
#include "Importer/ReadNodesStore.h"
#include "Importer/ReadEdgesStore.h"

#include "Manager/Manager.h"

#include "Data/Graph.h"

#include "Git/GitVersion.h"
#include "Git/GitEvolutionGraph.h"
#include "Git/GitFile.h"

#include <QDebug>
#include <QDir>

bool Importer::GitGraphImporter::import( ImporterContext &context ) {
    // context
    this->context = &context;

    // helpers
    this->filepath = this->context->getFilePath();
    this->graphOp.reset( new GraphOperations( this->context->getGraph() ) );
    this->readNodes.reset( new Importer::ReadNodesStore() );
    this->readEdges.reset( new Importer::ReadEdgesStore() );
    this->usedEdge;
    this->usedNode;

    // default types
    this->edgeType = NULL;
    this->nodeType = NULL;
    this->graphOp->addDefaultTypes( edgeType, nodeType );

    bool ok = true;

    QMap<QString, QString> *settings = new QMap<QString, QString>;
    settings->insert( "color.R", "1" );
    settings->insert( "color.G", "1" );
    settings->insert( "color.B", "0" );
    settings->insert( "color.A", "1" );
    settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
    settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
    this->context->getGraph().addType( "file", settings );

    settings = new QMap<QString, QString>;
    settings->insert( "color.R", "0" );
    settings->insert( "color.G", "1" );
    settings->insert( "color.B", "0" );
    settings->insert( "color.A", "1" );
    settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
    settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
    this->context->getGraph().addType( "dir", settings );

    settings = new QMap<QString, QString>;
    settings->insert( "color.R", "1" );
    settings->insert( "color.G", "0" );
    settings->insert( "color.B", "0" );
    settings->insert( "color.A", "1" );
    settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
    settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
    this->context->getGraph().addType( "root", settings );

    settings = new QMap<QString, QString>;
    settings->insert( "color.R", "1" );
    settings->insert( "color.G", "1" );
    settings->insert( "color.B", "1" );
    settings->insert( "color.A", "1" );
    settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.AuthorNodeScale" ) );
    settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Author" ) );
    this->context->getGraph().addType( "author", settings );

    settings = new QMap<QString, QString>;
    settings->insert( "color.R", "0" );
    settings->insert( "color.G", "0" );
    settings->insert( "color.B", "1" );
    settings->insert( "color.A", "1" );
    settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
    settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.OrientedEdge" ) );
    this->context->getGraph().addType( "authorEdge", settings );

    settings = new QMap<QString, QString>;
    settings->insert( "color.R", "1" );
    settings->insert( "color.G", "1" );
    settings->insert( "color.B", "1" );
    settings->insert( "color.A", "1" );
    settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
    settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Edge" ) );
    this->context->getGraph().addType( "Edge", settings );

    settings = new QMap<QString, QString>;
    settings->insert( "color.R", "0" );
    settings->insert( "color.G", "1" );
    settings->insert( "color.B", "1" );
    settings->insert( "color.A", "1" );
    settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
    settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
    this->context->getGraph().addType( "newE", settings );

    QList<Git::GitFile*> lAddedGitFiles = Manager::GraphManager::getInstance()->getActiveEvolutionGraph()->getVersion( 0 )->getGitFilesByType( Git::GitType::ADDED );
    QString lAuthor = Manager::GraphManager::getInstance()->getActiveEvolutionGraph()->getVersion( 0 )->getAuthor();

    ok = makeGraph( lAddedGitFiles );
    Data::Type* lType;
    QList<Data::Type*> lTypes = this->context->getGraph().getTypesByName( "author" );
    lType = lTypes.at( 0 );

    Data::Node* lAuthorNode = this->context->getGraph().addNode( lAuthor, lType );
    lAuthorNode->setLabelText( lAuthor );
    lAuthorNode->showLabel( true );
    readNodes->addNode( lAuthor, lAuthorNode );

    lTypes = this->context->getGraph().getTypesByName( "authorEdge" );
    lType = lTypes.at( 0 );

    foreach( Git::GitFile* file, lAddedGitFiles ) {
        osg::ref_ptr<Data::Node> osgNode = readNodes->get( file->getFilepath() );
        if( osgNode ) {
            osg::ref_ptr<Data::Edge> newEdge = this->context->getGraph().addEdge( lAuthorNode->Data::AbsNode::getName() + osgNode.get()->Data::AbsNode::getName(), readNodes->get( lAuthor ), osgNode, lType, true );
            readEdges->addEdge( lAuthorNode->Data::AbsNode::getName() + osgNode->Data::AbsNode::getName(), newEdge );
        }
    }

    return ok;
}

bool Importer::GitGraphImporter::makeGraph( QList<Git::GitFile*> gitFiles ) {
    bool ok = true;

    for( int i = 0; i < gitFiles.size();i++ ) {
        QString line = gitFiles.at( i )->getFilepath();

        QStringList list = line.split( "/" );
        QString pom = "";
        for( int k = 0; k < list.size(); k++ ) {
            pom += list.at( k );
            list.replace( k, pom );
            pom += "/";
        }

        ok = addNode( list );
        ok = addEdge( list );
        Manager::GraphManager::getInstance()->setProgressBarValue(  (int)( ( (double)( i + 1 ) / (double) gitFiles.size() ) * 100 ) );
    }

    return ok;
}

bool Importer::GitGraphImporter::addNode( QStringList &list ) {
    bool ok = true;

    for( int i = 0; i < list.size(); i++ ) {
        bool exist = true;
        QString lNodeName = list.at( i );
        ok = !lNodeName.isEmpty();
        this->context->getInfoHandler().reportError( ok, "Node ID can not be empty" );

        osg::ref_ptr<Data::Node> lNode( NULL );
        if( this->readNodes->contains( lNodeName ) ) {
            exist = false;
        }
        if( exist ) {
            Data::Type* lType = NULL;

            QString lVal;
            if( i == ( list.size() - 1 ) ) {
                lVal = "file";
            } else {
                lVal = "dir";
                if( i == 0 ) {
                    lVal = "root";
                }
            }

            QList<Data::Type*> lTypes = this->context->getGraph().getTypesByName( lVal );
            lType = lTypes.at( 0 );

            if( ok ) {
                lNode = this->context->getGraph().addNode( lNodeName, lType );
//                qDebug() << "velkost nodes" << this->context->getGraph().getNodes()->size();

                if( QString::compare( lVal, "root" ) == 0 ) {
                    lNode->setFixed( true );
                }

                lNode->setLabelText( lNodeName );
                lNode->showLabel( true );
                this->readNodes->addNode( lNodeName, lNode );
            }
        }
    }
    return ok;
}

bool Importer::GitGraphImporter::addEdge( QStringList &list ) {
    bool ok = true;

    Data::Type* lType = this->context->getGraph().getTypesByName( "Edge" ).at( 0 );

    for( int i = 0; i < list.size() - 1; i++ ) {
        QString lNodeNameFrom = list.at( i );
        QString lNodeNameTo = list.at( i + 1 );
        QString lEdgeName = lNodeNameFrom + lNodeNameTo;
        osg::ref_ptr<Data::Edge> lEdge( NULL );
        bool exist = true;

        this->context->getGraph().addEdgeOccurence( lEdgeName );

        if( this->readEdges->contains( lEdgeName ) ) {
            exist = false;
        }

        if( exist ) {
            if( ok ) {
                ok = this->readNodes->contains( lNodeNameFrom );
                this->context->getInfoHandler().reportError( ok, "Edge references invalid source node.");
            }

            if( ok ) {
                ok = this->readNodes->contains( lNodeNameTo );
                this->context->getInfoHandler().reportError( ok, "Edge references invalid source node.");
            }

            bool oriented = false;

            if( ok ) {
                lEdge = this->context->getGraph().addEdge( lEdgeName, this->readNodes->get( lNodeNameFrom ), this->readNodes->get( lNodeNameTo ), lType, oriented );
//                qDebug() << "velkost edges" << this->context->getGraph().getEdges()->size();
                this->readEdges->addEdge( lEdgeName, lEdge );
            }
        }
    }
    return ok;
}

bool Importer::GitGraphImporter::updateGraph( QList<Git::GitFile *> gitFiles ) {
    bool ok = true;
    Data::Graph* lGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    foreach( Git::GitFile* gitFile, gitFiles ) {
        QString lFilepath = gitFile->getFilepath();
        qDebug() << lFilepath;

        QStringList lList = lFilepath.split( "/" );
        QString lPom = "";
        for( int i = 0; i < lList.size(); i++ ) {
            lPom += lList.at( i );
            lList.replace( i, lPom );
            lPom += "/";
        }

        for( int i = 0; i < lList.size(); i++ ) {
            bool exist = true;
            QString lNodeName = lList.at( i );
            ok =  !lNodeName.isEmpty();

            osg::ref_ptr<Data::Node> lNode( nullptr );
            if( lGraph->findNodeByName( lNodeName ) ) {
                exist =  false;
            }

            if( exist ) {
                Data::Type* lType = nullptr;
                QString lVal;
                if( i == lList.size() - 1 ) {
                    lVal = "file";
                } else {
                    lVal = "dir";
                    if( i == 0 ) {
                        lVal = "root";
                    }
                }

                QList<Data::Type*> lTypes = lGraph->getTypesByName( lVal );
                lType = lTypes.at( 0 );

                if( ok ) {
                    qDebug() << "new Node";
                    lNode = lGraph->addNode( lNodeName, lType );

                    if( QString::compare( lVal, "root" ) == 0 ) {
                        lNode->setFixed( true );
                    }

                    lNode->setLabelText( lNodeName );
                    lNode->showLabel( true );
                }
            }
        }

        Data::Type* lType = lGraph->getTypesByName( "Edge" ).at( 0 );

        for( int i = 0; i < lList.size() - 1; i++ ) {
            QString lNodeNameFrom = lList.at( i );
            QString lNodeNameTo = lList.at( i + 1 );
            QString lEdgeName = lNodeNameFrom + lNodeNameTo;
            osg::ref_ptr<Data::Edge> lEdge( NULL );
            bool exist = true;

            if( lGraph->findEdgeByName( lEdgeName ) ) {
                exist = false;
            }

            if( exist ) {
                if( ok ) {
                    ok = lGraph->findNodeByName( lNodeNameFrom );
                }

                if( ok ) {
                    ok = lGraph->findNodeByName( lNodeNameTo );
                }

                bool oriented = false;

                if( ok ) {
                    lEdge = lGraph->addEdge( lEdgeName, lGraph->findNodeByName( lNodeNameFrom ), lGraph->findNodeByName( lNodeNameTo ), lType, oriented );
                }
            }
        }

    }
    return ok;
}

