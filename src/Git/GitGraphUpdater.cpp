#include "Git/GitGraphUpdater.h"

#include "Data/Graph.h"
#include "Data/Node.h"
#include "Data/Edge.h"

#include "Manager/Manager.h"

#include "Git/GitEvolutionGraph.h"
#include "Git/GitFile.h"
#include "Git/GitType.h"
#include "Git/GitVersion.h"

#include <QDebug>

Git::GitGraphUpdater::GitGraphUpdater( int currentVersion, Git::GitEvolutionGraph *evolutionGraph, Data::Graph *activeGraph ) : currentVersion( currentVersion ), evolutionGraph( evolutionGraph ), activeGraph( activeGraph ) {
    this->moreVersionChanged = false;
}

Data::Graph* Git::GitGraphUpdater::nextVersion() {
    // zistime zmenene subory v dalsej verzii, s ktorymi budeme dalej pracovat
    QList<Git::GitFile*> gitFiles = this->getEvolutionGraph()->getVersion( this->getCurrentVersion() + 1 )->getChangedFiles();
    Data::Node* lAuthorNode = nullptr;
    QString lAuthorName = nullptr;

    if( !moreVersionChanged ) {
        // upratanie predchadzajucej verzie
        removeAuthorEdgesFromGraph( this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getAuthor(), this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getChangedFiles() );
        this->getActiveGraph()->removeNode( this->getActiveGraph()->findNodeByName( this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getAuthor() ) );

        lAuthorName = this->getEvolutionGraph()->getVersion( this->currentVersion + 1 )->getAuthor();

        //    qDebug() << "Pocet pridanych suborov" << this->getEvolutionGraph()->getVersion( this->getCurrentVersion() + 1 )->getGitFilesByType( Git::GitType::ADDED ).size();
        //    qDebug() << "Pocet vymazanych suborov" << this->getEvolutionGraph()->getVersion( this->getCurrentVersion() + 1 )->getGitFilesByType( Git::GitType::REMOVED ).size();
        //    qDebug() << "Pocet modifikovanych suborov" << this->getEvolutionGraph()->getVersion( this->getCurrentVersion() + 1 )->getGitFilesByType( Git::GitType::MODIFIED ).size();

        lAuthorNode = this->getActiveGraph()->findNodeByName( lAuthorName );
        if( !lAuthorNode ) {
            lAuthorNode = this->getActiveGraph()->addNode( lAuthorName, this->getActiveGraph()->getTypesByName( "author" ).at( 0 ) );
            lAuthorNode->setLabelText( lAuthorNode->Data::AbsNode::getName() );
            lAuthorNode->showLabel( true );
        }
    }

    bool ok = true;
    foreach( Git::GitFile* gitFile, gitFiles ){
        QStringList lList = gitFile->getFilepath().split( "/" );
        QString lPom = "";

        for( int i = 0; i < lList.size(); i++ ) {
            lPom += lList.at( i );
            lList.replace( i, lPom );
            lPom += "/";
        }
        switch( gitFile->getType() ) {
        // ak bol pridany v dalsej verzii, tak ho pridame do grafu
        case Git::GitType::ADDED :
            addNodesToGraph( lList );
            addEdgesToGraph( lList );
            break;
        // ak bol vymazany v dalsej verzii, tak ho vymazeme z grafu
        case Git::GitType::REMOVED :
//            removeNodesFromGraph( lList );
            removeEdgesFromGraph( lList );
            removeNodesFromGraph( lList );
            break;
        case Git::GitType::MODIFIED :
            modifyNodesInGraph( lList );
            break;
        }
    }

    if( !moreVersionChanged ) {
        addAuthorEdgesToGraph( lAuthorName, this->getEvolutionGraph()->getVersion( this->getCurrentVersion() + 1 )->getChangedFiles() );
    }

    return this->getActiveGraph();
}

Data::Graph* Git::GitGraphUpdater::previousVersion() {
    // zistime si zmenene subory v aktualnej verzii, s ktorymi budeme dalej pracovat
    QList<Git::GitFile*> gitFiles = this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getChangedFiles();
    Data::Node* lAuthorNode = nullptr;
    QString lAuthorName = nullptr;

    if( !moreVersionChanged ) {
        // upratanie sucasnej verzie
        removeAuthorEdgesFromGraph( this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getAuthor(), this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getChangedFiles() );
        this->getActiveGraph()->removeNode( this->getActiveGraph()->findNodeByName( this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getAuthor() ) );

        lAuthorName = this->getEvolutionGraph()->getVersion( this->currentVersion - 1 )->getAuthor();

        qDebug() << "Pocet pridanych suborov" << this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getGitFilesByType( Git::GitType::ADDED ).size();
        qDebug() << "Pocet vymazanych suborov" << this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getGitFilesByType( Git::GitType::REMOVED ).size();
        qDebug() << "Pocet modifikovanych suborov" << this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getGitFilesByType( Git::GitType::MODIFIED ).size();

        lAuthorNode = this->getActiveGraph()->findNodeByName( lAuthorName );
        if( !lAuthorNode ) {
            lAuthorNode = this->getActiveGraph()->addNode( lAuthorName, this->getActiveGraph()->getTypesByName( "author" ).at( 0 ) );
            lAuthorNode->setLabelText( lAuthorNode->Data::AbsNode::getName() );
            lAuthorNode->showLabel( true );
        }
    }

    bool ok = true;
    foreach( Git::GitFile* gitFile, gitFiles ){
        QStringList lList = gitFile->getFilepath().split( "/" );
        QString lPom = "";

        for( int i = 0; i < lList.size(); i++ ) {
            lPom += lList.at( i );
            lList.replace( i, lPom );
            lPom += "/";
        }

        switch( gitFile->getType() ) {
        // ak bol pridany v aktualnej verzii, tak ho vymazeme z grafu
        case Git::GitType::ADDED :
//            removeNodesFromGraph( lList );
            removeEdgesFromGraph( lList );
            removeNodesFromGraph( lList );
            break;
        // ak bol vymazany v aktualnej verzii, tak ho pridame do grafu
        case Git::GitType::REMOVED :
            addNodesToGraph( lList );
            addEdgesToGraph( lList );
            break;
        case Git::GitType::MODIFIED :
            modifyNodesInGraph( lList );
            break;
        }
    }

    if( !moreVersionChanged ) {
        addAuthorEdgesToGraph( lAuthorName, this->getEvolutionGraph()->getVersion( this->getCurrentVersion() - 1 )->getChangedFiles() );
    }
    return this->getActiveGraph();
}

Data::Graph* Git::GitGraphUpdater::changeToVersion( int toVersion ) {
    int inc = 0;
    if( this->getCurrentVersion() > toVersion ) {
        inc = -1;
    } else {
        inc = 1;
    }
    int startVersion = this->getCurrentVersion();
    qDebug() << "Change version" << this->getCurrentVersion() << "to" << toVersion;
    Manager::GraphManager::getInstance()->showProgressBar();

    // upratanie zmien
    removeAuthorEdgesFromGraph( this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getAuthor(), this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getChangedFiles() );
    this->getActiveGraph()->removeNode( this->getActiveGraph()->findNodeByName( this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getAuthor() ) );

    this->moreVersionChanged = true;
    for( int i = this->getCurrentVersion(); i != toVersion; i += inc ) {
        qDebug() << this->getCurrentVersion() << ":" << i << "->" << i + inc;
        if( inc == -1 ) {
            this->previousVersion();
        } else {
            this->nextVersion();
        }
        this->setCurrentVersion( this->getCurrentVersion() + inc );
        Manager::GraphManager::getInstance()->setProgressBarValue( (int)( ( (double)( abs( i - startVersion ) ) / (double) abs( toVersion - startVersion ) ) * 100 ) );
    }
    this->moreVersionChanged = false;

    // pridanie vazieb na novu verziu
    QString lAuthorName = this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getAuthor();
    Data::Node* lAuthorNode = this->getActiveGraph()->findNodeByName( lAuthorName );

    if( !lAuthorNode ) {
        lAuthorNode = this->getActiveGraph()->addNode( lAuthorName, this->getActiveGraph()->getTypesByName( "author" ).at( 0 ) );
        lAuthorNode->setLabelText( lAuthorNode->Data::AbsNode::getName() );
        lAuthorNode->showLabel( true );
    }
    addAuthorEdgesToGraph( lAuthorName, this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getChangedFiles() );

    Manager::GraphManager::getInstance()->closeProgressBar();

    qDebug() << "GitGraphUpdater current version" << this->getCurrentVersion();

    return this->getActiveGraph();
}

void Git::GitGraphUpdater::addNodesToGraph( QStringList list ) {
    bool ok = true;

    for( int i = 0; i < list.size(); i++ ) {
        bool exist = false;
        QString lNodeName = list.at( i );
        ok = !lNodeName.isEmpty();

        osg::ref_ptr<Data::Node> lNode( nullptr );
        if( this->getActiveGraph()->findNodeByName( lNodeName) ) {
            exist = true;
        }

        if( !exist ) {
            Data::Type* lType( nullptr );
            QString lVal;
            if( i == list.size() - 1 ) {
                lVal = "file";
            } else {
                lVal = "dir";
                if( i == 0 ) {
                    lVal = "root";
                }
            }

            lType = this->getActiveGraph()->getTypesByName( lVal ).at( 0 );

            if( ok ) {
                lNode = this->getActiveGraph()->addNode( lNodeName, lType );

                if( QString::compare( lVal, "root" ) == 0 ) {
                    lNode->setFixed( true );
                }

                lNode->setLabelText( lNodeName );
                lNode->showLabel( true );
            }
        }
    }

}

void Git::GitGraphUpdater::addEdgesToGraph( QStringList list ) {
    bool ok = true;

    Data::Type* lType = this->getActiveGraph()->getTypesByName( "Edge" ).at( 0 );

    for( int i = 0; i < list.size() - 1; i++ ) {
        bool exist = false;
        osg::ref_ptr<Data::Edge> lEdge( nullptr );

        QString lNodeNameFrom = list.at( i );
        QString lNodeNameTo = list.at( i + 1 );
        QString lEdgeName = lNodeNameFrom + lNodeNameTo;

        this->getActiveGraph()->addEdgeOccurence( lEdgeName );

        if( this->getActiveGraph()->findEdgeByName( lEdgeName ) ) {
            exist =  true;
        }

        if( !exist ) {
            if( ok ) {
                ok = this->getActiveGraph()->findNodeByName( lNodeNameFrom );
            }

            if( ok ) {
                ok = this->getActiveGraph()->findNodeByName( lNodeNameTo );
            }

            bool oriented =  false;

            if( ok ) {
                lEdge = this->getActiveGraph()->addEdge( lEdgeName, this->getActiveGraph()->findNodeByName( lNodeNameFrom), this->getActiveGraph()->findNodeByName( lNodeNameTo), lType, oriented );
            }

        }
    }


}

void Git::GitGraphUpdater::addAuthorEdgesToGraph( QString authorName, QList<Git::GitFile*> gitFiles ){
    int count = 0;
    foreach ( Git::GitFile* gitFile, gitFiles ) {
        QString lEdgeName = authorName + gitFile->getFilepath();
        bool exist = true;

        if( exist ) {
            exist = this->getActiveGraph()->findNodeByName( gitFile->getFilepath() );
        }

        if( exist && !this->getActiveGraph()->findEdgeByName( lEdgeName ) ) {
            Data::Type* lType = this->getActiveGraph()->getTypesByName( "authorEdge" ).at( 0 );
            this->getActiveGraph()->addEdge( lEdgeName, this->getActiveGraph()->findNodeByName( authorName ), this->getActiveGraph()->findNodeByName( gitFile->getFilepath() ), lType, true );
            count++;
        }
    }
}

void Git::GitGraphUpdater::removeAuthorEdgesFromGraph( QString authorName, QList<Git::GitFile*> gitFiles ){
    foreach ( Git::GitFile* gitFile, gitFiles ) {
        QString lEdgeName = authorName + gitFile->getFilepath();
        Data::Edge* lEdge = this->getActiveGraph()->findEdgeByName( lEdgeName );
        if( lEdge ) {
            this->getActiveGraph()->removeEdge( lEdge );
        }
    }

}

void Git::GitGraphUpdater::removeNodesFromGraph( QStringList list ) {
    for( int i =  0; i < list.size() - 1; i++ ) {
        QString lNodeNameFrom = list.at( i );
        QString lNodeNameTo = list.at( i + 1 );
        QString lEdgeName = lNodeNameFrom + lNodeNameTo;
        if( !this->getActiveGraph()->getEdgeOccurence().value( lEdgeName ) ) {
            this->getActiveGraph()->removeNode( this->getActiveGraph()->findNodeByName( lNodeNameTo ) );
        }
    }
}

void Git::GitGraphUpdater::removeEdgesFromGraph( QStringList list ) {
    for( int i = 0; i < list.size() - 1 ; i++ ) {
        QString lNodeNameFrom = list.at( i );
        QString lNodeNameTo =  list.at( i + 1 );
        QString lEdgeName =  lNodeNameFrom + lNodeNameTo;
        if( !this->getActiveGraph()->removeEdgeOccurence( lEdgeName ) ) {
            this->getActiveGraph()->removeEdge( this->getActiveGraph()->findEdgeByName( lEdgeName ) );
        }
    }
}

void Git::GitGraphUpdater::modifyNodesInGraph( QStringList list ) {

}
