#include "Git/GitGraphImporter.h"

#include "Importer/GraphOperations.h"
#include "Importer/ReadNodesStore.h"
#include "Manager/Manager.h"
#include "Data/Graph.h"
#include "Git/GitVersion.h"
#include "Git/GitEvolutionGraph.h"
#include "Git/GitFile.h"

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
    settings->insert("textureFile", Util::ApplicationConfig::get()->getValue("Viewer.Textures.AuthorEdge"));
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

    QList<Git::GitFile*> lAddedGitFiles = Manager::GraphManager::getInstance()->getActiveEvolutionGraph()->getVersion( 10 )->getGitFilesByType( Git::GitType::ADDED );
    QString author = Manager::GraphManager::getInstance()->activeEvolutionGraph->getVersion( 10 )->getAuthor();
    int countVersions = Manager::GraphManager::getInstance()->activeEvolutionGraph->getVersions().size();

    ok = makeGraph( lAddedGitFiles );

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
        qDebug() << "addNode vratilo" << ok;

        ok = addEdge( list );
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

        QString lPath = lNodeName;
        QString val;
        if( i == ( list.size() - 1 ) ) {
            val = "file";
        } else {
            val = "dir";
            if( i == 0 ) {
                val = "root";
            }
        }

        QList<Data::Type*> lTypes = this->context->getGraph().getTypesByName( val );
        lType = lTypes.at( 0 );

        if( ok ) {
            lNode = this->context->getGraph().addNode( lNodeName, lType );
            //ToDo Data::Node->setPath( lNodeName ) a Data::Node->setNameFromPath()

            if( QString::compare( val, "root" ) == 0 ) {
                lNode->setFixed( true );
            }

            ok = lNode.valid();
            this->context->getInfoHandler().reportError(ok, "Unable to add new node.");
        }

        if( ok ) {
            this->readNodes->addNode( lNodeName, lNode );
        }
    }
    return ok;
}

bool Importer::GitGraphImporter::addEdge( QStringList &list ) {
    bool ok = true;

    QString edgePath = "/project/";
    for( int i = 0; i < list.size() - 1; i++ ) {
        QString lNodeNameFrom = list.at( i );
        QString lNodeNameTo = list.at( i + 1 );
    }
    return ok;
}

