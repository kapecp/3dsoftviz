#include <igloo/igloo_alt.h>
using namespace igloo;

#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitVersion.h"
#include "GitLib/GitMetaData.h"

#include <QList>
#include <QStringList>

Describe( a_git_evolution_graph ) {
    It( filepath_should_be_empty )
    {
        Assert::That( graph.getFilePath().toStdString(), Is().Empty() );
    }

    It( filepath_should_be_not_empty )
    {
        graph.setFilePath( "/filepath" );
        Assert::That( graph.getFilePath().toStdString(), Is().Not().Empty() );
    }

    It( versions_should_be_empty )
    {
        Assert::That( graph.getVersions().isEmpty(), IsTrue() );
    }

    It( versions_should_be_not_empty )
    {
        QList<Repository::Git::GitVersion*> versions = QList<Repository::Git::GitVersion*>();
        Repository::Git::GitVersion *version = new Repository::Git::GitVersion();

        versions.append( version );
        graph.setVersions( versions );
        Assert::That( graph.getVersions().isEmpty(), IsFalse() );
    }

    It( lifespan_should_be_default )
    {
        Assert::That( graph.getLifespan(), Equals( 0 ) );
    }

    It( lifespan_should_be_set )
    {
        graph.setLifespan( 1 );
        Assert::That( graph.getLifespan(), Equals( 1 ) );
    }

    It( removed_files_should_be_empty )
    {
        Assert::That( graph.getRemovedFiles().isEmpty(), IsTrue() );
    }

    It( removed_files_should_add_items )
    {
        QString path = "/filepath/to/hell";
        QStringList list = path.split( "/" );

        graph.addRemovedFiles( list, 1 );
        Assert::That( graph.getRemovedFiles().isEmpty(), IsFalse() );

        Assert::That( graph.getRemovedFiles().contains( "filepath" ) );
    }

    It( removed_files_should_remove_item ) {
        QString path = "/filepath/to/hell";
        QStringList list = path.split( "/" );
        path = "";
        for( int i = 0; i < list.size(); i++ ) {
            path += list.at( i );
            list.replace( i, path );
            path += "/";
        }

        graph.addRemovedFiles( list, 1 );
        Assert::That( graph.getRemovedFiles().isEmpty(), IsFalse() );

        graph.removeRemovedFiles( list.at( 1 ) );
        Assert::That( graph.getRemovedFiles().contains( list.at( 1 ) ), IsFalse() );
    }

    It( versions_should_return_specific_version )
    {
        QList<Repository::Git::GitVersion*> versions = QList<Repository::Git::GitVersion*>();
        Repository::Git::GitVersion *version1 = new Repository::Git::GitVersion( "1" );
        Repository::Git::GitVersion *version2 = new Repository::Git::GitVersion( "2" );
        Repository::Git::GitVersion *version3 = new Repository::Git::GitVersion( "3" );
        Repository::Git::GitVersion *version4 = new Repository::Git::GitVersion( "4" );

        versions.append( version1 );
        versions.append( version2 );
        versions.append( version3 );
        versions.append( version4 );


        graph.setVersions( versions );

        Assert::That( graph.getVersion( 0 )->getCommitId().toStdString(), Equals( "1" ) );
        Assert::That( graph.getVersion( 1 )->getCommitId().toStdString(), Equals( "2" ) );
        Assert::That( graph.getVersion( 2 )->getCommitId().toStdString(), Equals( "3" ) );
        Assert::That( graph.getVersion( 3 )->getCommitId().toStdString(), Equals( "4" ) );
    }

    It( meta_data_should_be_inicialized ) {
        if( graph.getMetaDataFromIdentifier( "aa" ) ) {
            Assert::That( true );
        } else {
            Assert::That( false );
        }
    }

    It( changed_nodes_and_edges_should_be_empty ) {
        Assert::That( graph.getChangedNodesAndEdge()->size(), Equals( 0 ) );
    }

    It( changed_nodes_and_edges_add_specific_entry ) {
        graph.addChangedNodeOrEdge( "identifier", Repository::Git::GitType::ADDED );
        Assert::That( graph.getChangedNodesAndEdge()->size(), Equals( 1 ) );
    }

    It( changed_nodes_and_edges_delete_specific_entry ) {
        graph.addChangedNodeOrEdge( "identifier", Repository::Git::GitType::ADDED );
        Assert::That( graph.getChangedNodesAndEdge()->size(), Equals( 1 ) );
        graph.removeChangedNodeOrEdge( "identifier" );
        Assert::That( graph.getChangedNodesAndEdge()->size(), Equals( 0 ) );
    }

    Repository::Git::GitEvolutionGraph graph;
};
