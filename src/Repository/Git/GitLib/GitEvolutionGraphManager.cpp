#include "GitLib/GitEvolutionGraphManager.h"

#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitFile.h"
#include "GitLib/GitVersion.h"
#include "GitLib/GitFileDiffBlock.h"
#include "GitLib/GitFileDiffBlockLine.h"

Repository::Git::GitEvolutionGraphManager* Repository::Git::GitEvolutionGraphManager::instance;
Repository::Git::GitEvolutionGraph* Repository::Git::GitEvolutionGraphManager::masterEvolutionGraph;

Repository::Git::GitEvolutionGraphManager::GitEvolutionGraphManager() {

}

Repository::Git::GitEvolutionGraphManager::~GitEvolutionGraphManager() {
    qDebug() << "Destruktor GitEvolutionGraphManager";
    delete masterEvolutionGraph;
    delete instance;
}

Repository::Git::GitEvolutionGraphManager* Repository::Git::GitEvolutionGraphManager::getInstance()  {
    if( instance == nullptr ) {
        instance = new Repository::Git::GitEvolutionGraphManager();
    }

    return instance;
}

void Repository::Git::GitEvolutionGraphManager::setEvolutionGraph( Repository::Git::GitEvolutionGraph *evolutionGraph ) {
    this->masterEvolutionGraph = evolutionGraph;
}

Repository::Git::GitEvolutionGraph* Repository::Git::GitEvolutionGraphManager::getEvolutionGraphByExtension( QString extensions ) {
    Repository::Git::GitEvolutionGraph* result = cloneEvolutionGraph();
    QList<int> remove = QList<int>();

    for( int i = 0; i < result->getVersions().size(); i++) {
        Repository::Git::GitVersion* version = result->getVersions().at( i );
        QMap<QString, Repository::Git::GitFile*>* changedFiles = filterVersionByExtensions( version, extensions );
        if( changedFiles->size() > 0 ){
            version->setChangedFiles( changedFiles );
        } else {
            remove.append( i );
        }
    }

    if( remove.size() > 0 ) {
        for( int i = remove.size() - 1; i >= 0; i-- ) {
            result->removeVersionAt( remove.at( i ) );
        }
    }

    return result;
}

Repository::Git::GitEvolutionGraph* Repository::Git::GitEvolutionGraphManager::getEvolutionGraphByAuthor( QString authorName ) {
    Repository::Git::GitEvolutionGraph* result = cloneEvolutionGraph();

    result->setVersions( filterVersionByAuthor( result, authorName ) );

    qDebug() << "Pocet verzii s autorom" << authorName << result->getVersions().size();

    return result;
}

QMap<QString, Repository::Git::GitFile*>* Repository::Git::GitEvolutionGraphManager::filterVersionByExtensions( Repository::Git::GitVersion *version, QString extensions ){
    QMap<QString, Repository::Git::GitFile*>* result = new QMap<QString, Repository::Git::GitFile*>();

        // Pre kazdy git file skontrolujem, ci ma jednu z extensions
    for( QMap<QString, Repository::Git::GitFile*>::iterator iterator = version->getChangedFiles()->begin(); iterator != version->getChangedFiles()->end(); ++iterator ) {
        Repository::Git::GitFile* gitFile = iterator.value();

        // Zistim extension
        QString extension = gitFile->getFilename().mid( gitFile->getFilename().lastIndexOf( "." ) + 1 );

        // Ak sa extension nachadza v extensions, tak do vysledku pridam git file
        if ( extensions.indexOf( extension ) > -1 ) {
            result->insert( gitFile->getIdentifier(), gitFile );
        }
    }

    return result;
}

QList<Repository::Git::GitVersion*> Repository::Git::GitEvolutionGraphManager::filterVersionByAuthor( Repository::Git::GitEvolutionGraph* evolutionGraph, QString author ) {
    QList<Repository::Git::GitVersion*> versions = QList<Repository::Git::GitVersion*>();

    foreach( Repository::Git::GitVersion* version , evolutionGraph->getVersions() ) {
        if( !QString::compare( version->getAuthor(), author ) ) {
            versions.append( version );
        }
    }

    return versions;
}

Repository::Git::GitFileDiffBlockLine* Repository::Git::GitEvolutionGraphManager::cloneDiffBlockLine( Repository::Git::GitFileDiffBlockLine *line ) {
    Repository::Git::GitFileDiffBlockLine* result = new Repository::Git::GitFileDiffBlockLine( line->getLine(), line->getLineNumber(), line->getLineType() );

    return result;
}

Repository::Git::GitFileDiffBlock* Repository::Git::GitEvolutionGraphManager::cloneDiffBlock( Repository::Git::GitFileDiffBlock *block ) {
    Repository::Git::GitFileDiffBlock* result = new Repository::Git::GitFileDiffBlock();

    result->setAddStart( block->getAddStart() );
    result->setAddCount( block->getAddCount() );
    result->setRemoveStart( block->getRemoveStart() );
    result->setRemoveCount( block->getRemoveCount() );

    foreach( Repository::Git::GitFileDiffBlockLine* line, block->getGitFileDiffBlockLines() ) {
        result->addGitFileDiffBlockLine( line );
    }

    return result;
}

Repository::Git::GitFile* Repository::Git::GitEvolutionGraphManager::cloneFile( Repository::Git::GitFile *file ) {
    Repository::Git::GitFile* result = new Repository::Git::GitFile( file->getFilename(), file->getFilepath(), file->getType() );

    foreach( Repository::Git::GitFileDiffBlock* block, file->getGitFileDiffBlocks() ) {
        result->addGitFileDiffBlock( cloneDiffBlock( block ) );
    }


    return result;
}

Repository::Git::GitVersion* Repository::Git::GitEvolutionGraphManager::cloneVersion( Repository::Git::GitVersion *version ) {
    Repository::Git::GitVersion* result = new Repository::Git::GitVersion();

    result->setCommitId( version->getCommitId() );
    result->setAuthor( version->getAuthor() );
    result->setDate( version->getDate() );

    int count = 0;
    for( QMap<QString, Repository::Git::GitFile*>::iterator iterator = version->getChangedFiles()->begin(); iterator != version->getChangedFiles()->end(); ++iterator ) {
        count++;
        Repository::Git::GitFile* file = iterator.value();
        result->addChangedFile( cloneFile( file ) );
    }

    return result;
}

Repository::Git::GitEvolutionGraph* Repository::Git::GitEvolutionGraphManager::cloneEvolutionGraph() {
    Repository::Git::GitEvolutionGraph* result = new Repository::Git::GitEvolutionGraph();
    result->setFilePath( masterEvolutionGraph->getFilePath() );

    QList<Repository::Git::GitVersion*> versions = QList<Repository::Git::GitVersion*>();

    foreach( Repository::Git::GitVersion* version, masterEvolutionGraph->getVersions() ) {
        versions.append( cloneVersion( version ) );
    }

    result->setVersions( versions );

    return result;
}
