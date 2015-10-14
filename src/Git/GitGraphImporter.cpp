#include "Git/GitGraphImporter.h"

#include "Importer/GraphOperations.h"
#include "Importer/ReadNodesStore.h"
#include "Importer/ReadEdgesStore.h"
#include "Manager/Manager.h"
#include "Data/Graph.h"
#include "Git/GitVersion.h"
#include "Git/GitEvolutionGraph.h"
#include "Git/GitFile.h"
#include "Core/Core.h"
#include "QOSG/MessageWindows.h"

#include <QDebug>
#include <QDir>

using namespace std;

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
    settings->insert("color.R", "1");
    settings->insert("color.G", "1");
    settings->insert("color.B", "0");
    settings->insert("color.A", "1");
    settings->insert("scale", Util::ApplicationConfig::get()->getValue("Viewer.Textures.DefaultNodeScale"));
    settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.Node"));
    this->context->getGraph().addType("file", settings);

    settings = new QMap<QString, QString>;
    settings->insert("color.R", "0");
    settings->insert("color.G", "1");
    settings->insert("color.B", "0");
    settings->insert("color.A", "1");
    settings->insert("scale", Util::ApplicationConfig::get()->getValue("Viewer.Textures.DefaultNodeScale"));
    settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.Node"));
    this->context->getGraph().addType("dir", settings);

    settings = new QMap<QString, QString>;
    settings->insert("color.R", "1");
    settings->insert("color.G", "0");
    settings->insert("color.B", "0");
    settings->insert("color.A", "1");
    settings->insert("scale", Util::ApplicationConfig::get()->getValue("Viewer.Textures.DefaultNodeScale"));
    settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.Node"));
    this->context->getGraph().addType("root", settings);

    settings = new QMap<QString, QString>;
    settings->insert("color.R", "1");
    settings->insert("color.G", "1");
    settings->insert("color.B", "1");
    settings->insert("color.A", "1");
    settings->insert("scale", Util::ApplicationConfig::get()->getValue("Viewer.Textures.AuthorNodeScale"));
    settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.Author"));
    this->context->getGraph().addType("author", settings);

    settings = new QMap<QString, QString>;
    settings->insert("color.R", "1");
    settings->insert("color.G", "1");
    settings->insert("color.B", "1");
    settings->insert("color.A", "0");
    settings->insert("scale", Util::ApplicationConfig::get()->getValue("Viewer.Textures.DefaultNodeScale"));
    settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.OrientedEdge"));
    this->context->getGraph().addType("author_edge", settings);

    settings = new QMap<QString, QString>;
    settings->insert("color.R", "0");
    settings->insert("color.G", "1");
    settings->insert("color.B", "1");
    settings->insert("color.A", "1");
    settings->insert("scale", Util::ApplicationConfig::get()->getValue("Viewer.Textures.DefaultNodeScale"));
    settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.Node"));
    this->context->getGraph().addType("newE", settings);

    qDebug() << "Settings done";

    QList<Git::GitFile*> lAddedGitFiles = Manager::GraphManager::getInstance()->getActiveEvolutionGraph()->getVersion( 17 )->getChangedFiles();
    QString lAuthor = Manager::GraphManager::getInstance()->activeEvolutionGraph->getVersion( 17 )->getAuthor();
    int lCountVersions = Manager::GraphManager::getInstance()->activeEvolutionGraph->getVersions().size();

//    for( int i = 0; i < 100; i++ ) {
//        qDebug() << i << Manager::GraphManager::getInstance()->activeEvolutionGraph->getVersion( i )->getChangedFiles().size();
//    }

    ok = makeGraph( lAddedGitFiles );
    Data::Type* lType;
    QList<Data::Type*> lTypes = this->context->getGraph().getTypesByName( "author" );
    lType = lTypes.at( 0 );

    Data::Node* lAuthorNode = this->context->getGraph().addNode( lAuthor, lType );
/*
    lAuthorNode->setLabelText( lAuthor );
    lAuthorNode->showLabel( true );

    // toto netusim co robi:D
    int emptys = 5;
    int pluss;
    if( lCountVersions > 900 ) {
        pluss = 40;
    }
    else if( lCountVersions > 300 ) {
        pluss = 30;
    }
    else if( lCountVersions > 100 ) {
        pluss = 20;
    }
    else {
        pluss = 10;
    }

    lTypes = this->context->getGraph().getTypesByName( "author_edge" );
    lType = lTypes.at( 0 );
*/
    Data::Node* lRoot = this->context->getGraph().findNodeByName( "projekt" );
    Data::Edge* lRootEdge = this->context->getGraph().addEdge( lAuthor + lRoot->Data::AbsNode::getName(), lAuthorNode, lRoot, this->edgeType, true );
/*
    QString lNodeName = "empty";
    QString lEdgeName = "";

    Data::Node* lNewFirst = this->context->getGraph().findNodeByName( "projekt" );

    while( emptys > 0 ) {
        lNodeName += "|";
        Data::Node* lNewSecond = this->context->getGraph().addNode( lNodeName, lType );
        lNewSecond->setLabelText("");

        Data::Edge * edge = this->context->getGraph().addEdge(
                    lEdgeName,
                    lNewFirst,
                    lNewSecond,
                    lType,
                    true
                    );
        emptys--;
        lNewFirst = lNewSecond;
    }

    while( pluss > 0 ) {
        lNodeName += "|";
        Data::Node* lNewSecond = this->context->getGraph().addNode( lNodeName, lType );
        lNewSecond->setLabelText("");

        Data::Edge * edge = this->context->getGraph().addEdge(
                    lEdgeName,
                    lNewFirst,
                    lNewSecond,
                    lType,
                    true
                    );
        pluss--;
        lNewFirst = lNewSecond;
    }

    Data::Edge* lEdge;

    lEdge = this->context->getGraph().addEdge(
            lEdgeName,
            lNewFirst,
            lAuthorNode,
            lType,
            true
    );
*/


    return ok;
}

bool Importer::GitGraphImporter::makeGraph( QList<Git::GitFile*> gitFiles ) {
    bool ok = true;

    for( int i = 0; i < gitFiles.size();i++ ) {
        QString line = gitFiles.at( i )->getFilepath();

        QStringList list = line.split("/");
        QString pom = "";
        for( int k = 0; k < list.size(); k++ ) {
            pom += list.at( k );
            list.replace( k, pom );
            pom += "/";
        }

        ok = addNode( list );
        ok = addEdge( list );
        AppCore::Core::getInstance()->messageWindows->setProgressBarValue( (int)( ( (double)( i + 1 ) / (double) gitFiles.size() ) * 100 ) );
//        qDebug() << "Progress: " << (int)( ( (double)( i + 1 ) / (double) gitFiles.size() ) * 100 ) ;
    }

    return ok;
}

bool Importer::GitGraphImporter::addNode( QStringList &list ) {
    bool ok = true;

    for( int i = 0; i < list.size(); i++ ) {
        QString lNodeName = list.at( i );
        ok = !lNodeName.isEmpty();
        this->context->getInfoHandler().reportError( ok, "Node ID can not be empty" );

        osg::ref_ptr<Data::Node> lNode( NULL );
        if( this->readNodes->get( lNodeName ) ) {
            ok = false;
        }

        Data::Type* lType = NULL;

//        QString lPath = lNodeName;
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
            //ToDo Data::Node->setPath( lNodeName ) a Data::Node->setNameFromPath()

            if( QString::compare( lVal, "root" ) == 0 ) {
                lNode->setFixed( true );
            }

            ok = lNode.valid();
            this->context->getInfoHandler().reportError(ok, "Unable to add new node.");
        }

        if( ok ) {
            lNode->setLabelText( lNodeName );
            lNode->showLabel( true );
            this->readNodes->addNode( lNodeName, lNode );
        }
    }
    return ok;
}

bool Importer::GitGraphImporter::addEdge( QStringList &list ) {
    bool ok = true;

    for( int i = 0; i < list.size() - 1; i++ ) {
        QString lNodeNameFrom = list.at( i );
        QString lNodeNameTo = list.at( i + 1 );
        QString lEdgeName = lNodeNameFrom + lNodeNameTo;
        osg::ref_ptr<Data::Edge> lEdge( NULL );
        bool exist = true;

        if( this->readEdges->get( lEdgeName ) ) {
            exist = false;
        }

//        qDebug() << lEdgeName << exist;

        if( exist && ok) {
            ok = this->readNodes->contains( lNodeNameFrom );
            this->context->getInfoHandler().reportError( ok, "Edge references invalid source node.");
        }

        if( exist && ok ) {
            ok = this->readNodes->contains( lNodeNameTo );
            this->context->getInfoHandler().reportError( ok, "Edge references invalid source node.");
        }

        bool oriented = false;

        if( exist && ok ) {
            lEdge = this->context->getGraph().addEdge( lEdgeName, this->readNodes->get( lNodeNameFrom ), this->readNodes->get( lNodeNameTo ), this->edgeType, oriented );
        }

        if( exist && ok ) {
            this->readEdges->addEdge( lEdgeName, lEdge );
        }
    }
    return ok;
}

