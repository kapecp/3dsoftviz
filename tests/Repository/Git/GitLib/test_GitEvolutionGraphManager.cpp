#include <igloo/igloo_alt.h>
using namespace igloo;

#include "GitLib/GitEvolutionGraphManager.h"
#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitVersion.h"
#include "GitLib/GitFile.h"

#include <QDebug>

Describe( a_git_evolution_graph_manager ) {

    void SetUp() {
        Repository::Git::GitEvolutionGraph* evolutionGraph = new Repository::Git::GitEvolutionGraph( "/path/to/the/repository" );
        Repository::Git::GitVersion* version1 = new Repository::Git::GitVersion( "1" );
        Repository::Git::GitVersion* version2 = new Repository::Git::GitVersion( "2" );
        QList<Repository::Git::GitVersion*> versions = QList<Repository::Git::GitVersion*>();

        // Set up version1
        Repository::Git::GitFile* file11 = new Repository::Git::GitFile( "filenameA.lua", "path/to/the/filenameA.lua", Repository::Git::GitType::ADDED );
        Repository::Git::GitFile* file12 = new Repository::Git::GitFile( "filenameR.lua", "path/to/the/filenameR.lua", Repository::Git::GitType::REMOVED );
        Repository::Git::GitFile* file13 = new Repository::Git::GitFile( "filenameM.cpp", "path/to/the/file/filenameM.cpp", Repository::Git::GitType::MODIFIED );

        version1->addChangedFile( file11 );
        version1->addChangedFile( file12 );
        version1->addChangedFile( file13 );

        version1->setAuthor( "Michael" );

        // Set up version2
        Repository::Git::GitFile* file21 = new Repository::Git::GitFile( "filenameA.cpp", "path/to/the/another/filenameA.cpp", Repository::Git::GitType::ADDED );
        Repository::Git::GitFile* file22 = new Repository::Git::GitFile( "filenameR.cpp", "path/to/the/another/filenameR.cpp", Repository::Git::GitType::REMOVED );
        Repository::Git::GitFile* file23 = new Repository::Git::GitFile( "filenameM.cpp", "path/to/the/another/file/filenameM.cpp", Repository::Git::GitType::MODIFIED );

        version2->addChangedFile( file21 );
        version2->addChangedFile( file22 );
        version2->addChangedFile( file23 );

        version2->setAuthor( "Martin" );

        // Combine constructed instances
        versions.append( version1 );
        versions.append( version2 );

        evolutionGraph->setVersions( versions );

        Repository::Git::GitEvolutionGraphManager::getInstance()->setEvolutionGraph( evolutionGraph );
    }

    It( evolution_graph_manager_should_contains_versions ) {
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->getFilteredEvolutionGraph()->getVersions().size(), Equals( 2 ) );
    }

    It( evolution_graph_manager_filter_graph_by_extension ) {
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->filterByExtension( "lua" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 1 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->filterByExtension( "cpp" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 2 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->filterByExtension( "cpp,lua" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 2 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->filterByExtension( "h" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 0 ) );
    }

    It( evolution_graph_manager_filter_graph_by_author ) {
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->filterByAuthor( "Martin" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 1 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->filterByAuthor( "Michael" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 1 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->filterByAuthor( "Peter" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 0 ) );
    }

    It( evolution_graph_manager_exclude_directories ) {
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->excludeDirectories( "file" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 2 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->excludeDirectories( "the" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 0 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->excludeDirectories( "another,file" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 1 ) );
    }

    It( evolution_graph_manager_exclude_directories_and_filter_by_author ) {
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->excludeDirectories( "another,file" )->filterByAuthor( "Michael" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 1 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->excludeDirectories( "another,file" )->filterByAuthor( "Martin" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 0 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->filterByAuthor( "Michael" )->excludeDirectories( "another,file" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 1 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->filterByAuthor( "Martin" )->excludeDirectories( "another,file" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 0 ) );
    }

    It( evolution_graph_manager_exclude_directories_and_filter_by_extension ) {
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->excludeDirectories( "another,file" )->filterByExtension( "lua" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 1 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->excludeDirectories( "another,file" )->filterByExtension( "cpp" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 0 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->excludeDirectories( "file" )->filterByExtension( "lua,cpp" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 2 ) );
    }

    It( evolution_graph_manager_filter_graph_by_author_and_by_extension ) {
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->filterByAuthor( "Michael" )->filterByExtension( "lua" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 1 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->filterByAuthor( "Martin" )->filterByExtension( "lua" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 0 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->filterByAuthor( "Martin" )->filterByExtension( "lua,cpp" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 1 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->filterByAuthor( "Martin" )->filterByExtension( "cpp" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 1 ) );
    }

    It( evolution_graph_manager_exclude_directories_and_filter_by_all ) {
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->excludeDirectories( "file" )->filterByAuthor( "Michael" )->filterByExtension( "lua" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 1 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->excludeDirectories( "file" )->filterByAuthor( "Michael" )->filterByExtension( "cpp" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 0 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->excludeDirectories( "another" )->filterByAuthor( "Michael" )->filterByExtension( "cpp" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 1 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->excludeDirectories( "another" )->filterByAuthor( "Michael" )->filterByExtension( "lua,cpp" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 1 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->excludeDirectories( "another" )->filterByAuthor( "Martin" )->filterByExtension( "cpp" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 0 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->excludeDirectories( "file" )->filterByAuthor( "Martin" )->filterByExtension( "cpp" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 1 ) );
        Assert::That( Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->excludeDirectories( "file" )->filterByAuthor( "Martin" )->filterByExtension( "lua" )->getFilteredEvolutionGraph()->getVersions().size(), Equals( 0 ) );
    }

};
