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
    if( this->masterEvolutionGraph == nullptr ) {
        this->masterEvolutionGraph = evolutionGraph;
    }
}

Repository::Git::GitEvolutionGraph* Repository::Git::GitEvolutionGraphManager::getEvolutionGraphByExtension( QString extension ) {
    Repository::Git::GitEvolutionGraph* result = cloneEvolutionGraph();

    return result;
}

Repository::Git::GitEvolutionGraph* Repository::Git::GitEvolutionGraphManager::getEvolutionGraphByAuthor( QString authorName ) {
    Repository::Git::GitEvolutionGraph* result = cloneEvolutionGraph();

    return result;
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

    for( QMap<QString, Repository::Git::GitFile*>::iterator iterator = version->getChangedFiles().begin(); iterator != version->getChangedFiles().end(); ++iterator ) {
        result->addChangedFile( cloneFile( iterator.value() ) );
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
