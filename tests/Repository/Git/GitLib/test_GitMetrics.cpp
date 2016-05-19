#include <igloo/igloo_alt.h>
using namespace igloo;

#include "GitLib/GitMetrics.h"
#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitVersion.h"
#include "GitLib/GitFile.h"

#include <QList>

Describe( a_git_metrics ) {

    void SetUp() {
        Repository::Git::GitEvolutionGraph* evolutionGraph = new Repository::Git::GitEvolutionGraph( "path" );
        Repository::Git::GitVersion* version1 = new Repository::Git::GitVersion( "1" );

        Repository::Git::GitFile* file11 = new Repository::Git::GitFile("11", "/to/11", Repository::Git::GitType::ADDED );
        Repository::Git::GitFile* file12 = new Repository::Git::GitFile("12", "/to/12", Repository::Git::GitType::ADDED );
        Repository::Git::GitFile* file13 = new Repository::Git::GitFile("13", "/to/13", Repository::Git::GitType::ADDED );

        version1->addChangedFile( file11 );
        version1->addChangedFile( file12 );
        version1->addChangedFile( file13 );

        version1->setAuthor( "me" );

        Repository::Git::GitVersion* version2 = new Repository::Git::GitVersion( "2" );

        Repository::Git::GitFile* file21 = new Repository::Git::GitFile("21", "/to/21", Repository::Git::GitType::ADDED );
        Repository::Git::GitFile* file22 = new Repository::Git::GitFile("11", "/to/11", Repository::Git::GitType::MODIFIED );
        Repository::Git::GitFile* file23 = new Repository::Git::GitFile("12", "/to/12", Repository::Git::GitType::REMOVED );
        Repository::Git::GitFile* file24 = new Repository::Git::GitFile("24", "/to/24", Repository::Git::GitType::ADDED );

        version2->addChangedFile( file21 );
        version2->addChangedFile( file22 );
        version2->addChangedFile( file23 );
        version2->addChangedFile( file24 );

        version2->setAuthor( "me" );

        Repository::Git::GitVersion* version3 = new Repository::Git::GitVersion( "3" );

        Repository::Git::GitFile* file31 = new Repository::Git::GitFile("13", "/to/13", Repository::Git::GitType::REMOVED );
        Repository::Git::GitFile* file32 = new Repository::Git::GitFile("21", "/to/21", Repository::Git::GitType::MODIFIED );
        Repository::Git::GitFile* file33 = new Repository::Git::GitFile("11", "/to/11", Repository::Git::GitType::MODIFIED );

        version3->addChangedFile( file31 );
        version3->addChangedFile( file32 );
        version3->addChangedFile( file33 );

        version3->setAuthor( "you" );

        Repository::Git::GitVersion* version4 = new Repository::Git::GitVersion( "4" );

        Repository::Git::GitFile* file41 = new Repository::Git::GitFile("41", "/to/41", Repository::Git::GitType::ADDED );
        Repository::Git::GitFile* file42 = new Repository::Git::GitFile("11", "/to/11", Repository::Git::GitType::MODIFIED );
        Repository::Git::GitFile* file43 = new Repository::Git::GitFile("21", "/to/21", Repository::Git::GitType::REMOVED );
        Repository::Git::GitFile* file44 = new Repository::Git::GitFile("12", "/to/12", Repository::Git::GitType::ADDED );

        version4->addChangedFile( file41 );
        version4->addChangedFile( file42 );
        version4->addChangedFile( file43 );
        version4->addChangedFile( file44 );

        version4->setAuthor( "me" );

        QList<Repository::Git::GitVersion*> versions = QList<Repository::Git::GitVersion*>();

        versions.append( version1 );
        versions.append( version2 );
        versions.append( version3 );
        versions.append( version4 );

        evolutionGraph->setVersions( versions );

        this->metrics.setEvolutionGraph( evolutionGraph );
    }

    It( evolution_graph_should_contains_versions ) {
        Assert::That( metrics.getEvolutionGraph()->getFilePath().toStdString(), Equals( "path" ) );
    }

    It( get_changed_count_for_whole_evolution_graph ) {
        evolution_graph_should_contains_versions();

        Assert::That( metrics.getFileChangedCount( "file;/to/11" ), Equals( 4 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/12" ), Equals( 1 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/13" ), Equals( 0 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/21" ), Equals( 0 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/24" ), Equals( 1 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/41" ), Equals( 1 ) );
    }

    It( get_changed_count_for_count_and_start ) {
        evolution_graph_should_contains_versions();

        Assert::That( metrics.getFileChangedCount( "file;/to/11", 4 ), Equals( 4 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", 3 ), Equals( 3 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", 2 ), Equals( 2 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", 1 ), Equals( 1 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", 0 ), Equals( 0 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", -1 ), Equals( -1 ) );

        Assert::That( metrics.getFileChangedCount( "file;/to/11", 3, 1 ), Equals( 3 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", 3, 2 ), Equals( 2 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", 3, 3 ), Equals( 1 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", 3, 4 ), Equals( 0 ) );

        Assert::That( metrics.getFileChangedCount( "file;/to/11", 3, -1 ), Equals( -1 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", 3, 5 ), Equals( -1 ) );

        Assert::That( metrics.getFileChangedCount( "file;/to/11", -3, 3 ), Equals( 3 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", -4, 3 ), Equals( -1 ) );

        Assert::That( metrics.getFileChangedCount( "file;/to/11", -1, 1 ), Equals( 1 ) );
    }

    It( get_changed_count_for_count_and_start_commit_id ) {
        evolution_graph_should_contains_versions();

        Assert::That( metrics.getFileChangedCount( "file;/to/11", 4, "1" ), Equals( 4 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", 3, "2" ), Equals( 3 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", 3, "3" ), Equals( 2 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", 3, "4" ), Equals( 1 ) );

        Assert::That( metrics.getFileChangedCount( "file;/to/11", 3, "-1" ), Equals( -1 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", 3, "5" ), Equals( -1 ) );

        Assert::That( metrics.getFileChangedCount( "file;/to/11", -3, "4" ), Equals( 3 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", -3, "3" ), Equals( -1 ) );
    }

    It( get_changed_count_for_end_commit_id_and_start_commit_id ) {
        evolution_graph_should_contains_versions();

        Assert::That( metrics.getFileChangedCount( "file;/to/11", "1", "4" ), Equals( 4 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", "1", "3" ), Equals( 3 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", "1", "2" ), Equals( 2 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", "1", "1" ), Equals( 1 ) );

        Assert::That( metrics.getFileChangedCount( "file;/to/11", "4", "1" ), Equals( 4 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", "4", "2" ), Equals( 3 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", "4", "3" ), Equals( 2 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", "4", "4" ), Equals( 1 ) );

        Assert::That( metrics.getFileChangedCount( "file;/to/11", "4", "-1" ), Equals( -1 ) );
        Assert::That( metrics.getFileChangedCount( "file;/to/11", "5", "2" ), Equals( -1 ) );
    }

    It( get_author_count_for_whole_evolution_graph ) {
        evolution_graph_should_contains_versions();

        Assert::That( metrics.getAuthorCount( "me" ), Equals( 3 ) );
        Assert::That( metrics.getAuthorCount( "you" ), Equals( 1 ) );
    }

    It( get_author_count_for_count_and_start ) {
        evolution_graph_should_contains_versions();

        Assert::That( metrics.getAuthorCount( "me", 4 ), Equals( 3 ) );
        Assert::That( metrics.getAuthorCount( "me", 3 ), Equals( 2 ) );
        Assert::That( metrics.getAuthorCount( "me", 2 ), Equals( 2 ) );
        Assert::That( metrics.getAuthorCount( "me", 1 ), Equals( 1 ) );
        Assert::That( metrics.getAuthorCount( "me", 0 ), Equals( 0 ) );

        Assert::That( metrics.getAuthorCount( "me", -1 ), Equals( -1 ) );
        Assert::That( metrics.getAuthorCount( "me", 5 ), Equals( 3 ) );

        Assert::That( metrics.getAuthorCount( "me", 3, 1 ), Equals( 2 ) );
        Assert::That( metrics.getAuthorCount( "me", 1, 2 ), Equals( 0 ) );

        Assert::That( metrics.getAuthorCount( "me", -4, 4 ), Equals( 3 ) );
        Assert::That( metrics.getAuthorCount( "me", -3, 4 ), Equals( 2 ) );
        Assert::That( metrics.getAuthorCount( "me", -2, 4 ), Equals( 1 ) );

        Assert::That( metrics.getAuthorCount( "me", -3, 2 ), Equals( -1 ) );
    }


    It( get_author_count_for_count_and_start_commit_id ) {
        evolution_graph_should_contains_versions();

        Assert::That( metrics.getAuthorCount( "me", 4, "1" ), Equals( 3 ) );
        Assert::That( metrics.getAuthorCount( "me", 4, "2" ), Equals( 2 ) );
        Assert::That( metrics.getAuthorCount( "me", 4, "3" ), Equals( 1 ) );
        Assert::That( metrics.getAuthorCount( "me", 4, "4" ), Equals( 1 ) );

        Assert::That( metrics.getAuthorCount( "me", 4, "5" ), Equals( -1 ) );
        Assert::That( metrics.getAuthorCount( "me", 4, "-1" ), Equals( -1 ) );

        Assert::That( metrics.getAuthorCount( "me", -3, "4" ), Equals( 2 ) );
        Assert::That( metrics.getAuthorCount( "me", -3, "3" ), Equals( -1 ) );
    }

    It( get_author_count_for_end_commit_id_and_start_commit_id ) {
        evolution_graph_should_contains_versions();

        Assert::That( metrics.getAuthorCount( "me", "1", "4" ), Equals( 3 ) );
        Assert::That( metrics.getAuthorCount( "me", "1", "3" ), Equals( 2 ) );
        Assert::That( metrics.getAuthorCount( "me", "1", "2" ), Equals( 2 ) );
        Assert::That( metrics.getAuthorCount( "me", "1", "1" ), Equals( 1 ) );

        Assert::That( metrics.getAuthorCount( "me", "4", "1" ), Equals( 3 ) );
        Assert::That( metrics.getAuthorCount( "me", "4", "2" ), Equals( 2 ) );
        Assert::That( metrics.getAuthorCount( "me", "4", "3" ), Equals( 1 ) );
        Assert::That( metrics.getAuthorCount( "me", "4", "4" ), Equals( 1 ) );

        Assert::That( metrics.getAuthorCount( "me", "4", "-1" ), Equals( -1 ) );
        Assert::That( metrics.getAuthorCount( "me", "5", "2" ), Equals( -1 ) );
    }

    It( get_author_list_for_whole_evolution_graph ) {
        evolution_graph_should_contains_versions();

        Assert::That( metrics.getAuthorList().size(), Equals( 2 ) );
    }

    It( get_author_list_for_position ) {
        evolution_graph_should_contains_versions();

        Assert::That( metrics.getAuthorList( 2 ).size(), Equals( 1 ) );
        Assert::That( metrics.getAuthorList( 3 ).size(), Equals( 2 ) );
    }

    It( get_files_from_author ) {
        evolution_graph_should_contains_versions();

        Assert::That( metrics.getFilesFromAuthor( "me" ).size(), Equals( 6 ) );
        Assert::That( metrics.getFilesFromAuthor( "you" ).size(), Equals( 3 ) );
    }

    It( get_files_from_author_with_position ) {
        evolution_graph_should_contains_versions();

        Assert::That( metrics.getFilesFromAuthor( "me", 3 ).size(), Equals( 5 ) );
        Assert::That( metrics.getFilesFromAuthor( "you", 2 ).size(), Equals( 0 ) );
    }

    It( get_all_files ) {
        evolution_graph_should_contains_versions();

        Assert::That( metrics.getAllFiles().size(), Equals( 6 ) );
    }

    Repository::Git::GitMetrics metrics;
};
