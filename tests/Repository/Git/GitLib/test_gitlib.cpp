#include <igloo/igloo_alt.h>
using namespace igloo;

#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitFile.h"
#include "GitLib/GitFileDiffBlock.h"
#include "GitLib/GitFileDiffBlockLine.h"
#include "GitLib/GitFileLoader.h"
#include "GitLib/GitVersion.h"

#include <QList>
#include <QStringList>


Describe( a_git_file )
{
    It( type_should_be_NONE )
	{
        Assert::That( file.getType(), Equals( Repository::Git::GitType::NONE ) );
	}

    It( type_should_be_ADDED )
    {
        file = Repository::Git::GitFile( "", "", Repository::Git::GitType::ADDED );
        Assert::That( file.getType(), Equals( Repository::Git::GitType::ADDED ) );
    }

    It( type_should_be_REMOVED )
    {
        file = Repository::Git::GitFile( "", "", Repository::Git::GitType::REMOVED );
        Assert::That( file.getType(), Equals( Repository::Git::GitType::REMOVED ) );
    }

    It( type_should_be_MODIFIED )
    {
        file = Repository::Git::GitFile( "", "", Repository::Git::GitType::MODIFIED );
        Assert::That( file.getType(), Equals( Repository::Git::GitType::MODIFIED ) );
    }

    It( filename_should_be_empty )
    {
        Assert::That( file.getFilename().toStdString(), Is().Empty() );
    }

    It( filename_should_be_not_empty )
    {
        file = Repository::Git::GitFile( "filename", "/filepath", Repository::Git::GitType::ADDED );
        Assert::That( file.getFilename().toStdString(), Is().Not().Empty() );
    }

    It( filepath_should_be_empty )
    {
        Assert::That( file.getFilepath().toStdString(), Is().Empty() );
    }

    It( filepath_should_be_not_empty )
    {
        file = Repository::Git::GitFile( "filename", "/filepath", Repository::Git::GitType::ADDED );
        Assert::That( file.getFilepath().toStdString(), Is().Not().Empty() );
    }

    It( diffblocks_should_be_empty )
    {
        Assert::That( file.getGitFileDiffBlocks().isEmpty(), IsTrue() );
    }

    It( diffblocks_should_be_not_empty )
    {
        Repository::Git::GitFileDiffBlock *diffBlock = new Repository::Git::GitFileDiffBlock();
        file.addGitFileDiffBlock( diffBlock );
        Assert::That( file.getGitFileDiffBlocks().isEmpty(), IsFalse() );
    }

    It( diffblocks_should_be_set )
    {
        QList<Repository::Git::GitFileDiffBlock*> diffBlocks = QList<Repository::Git::GitFileDiffBlock*>();
        Repository::Git::GitFileDiffBlock* diffBlock1 = new Repository::Git::GitFileDiffBlock();
        Repository::Git::GitFileDiffBlock* diffBlock2 = new Repository::Git::GitFileDiffBlock();

        diffBlocks.append( diffBlock1 );
        diffBlocks.append( diffBlock2 );

        Assert::That( file.getGitFileDiffBlocks().isEmpty(), IsTrue() );

        file.setGitFileDiffBlocks( diffBlocks );
        Assert::That( file.getGitFileDiffBlocks().isEmpty(), IsFalse() );
    }

    Repository::Git::GitFile file;
};

Describe( a_git_diff_block ) {

    It( addStart_should_be_default )
    {
        Assert::That( diffBlock.getAddStart(), Equals( 1 ) );
    }

    It( addStart_should_be_set )
    {
        diffBlock.setAddStart( 5 ) ;
        Assert::That( diffBlock.getAddStart(), Equals( 5 ) );
    }

    It( addCount_should_be_default )
    {
        Assert::That( diffBlock.getAddCount(), Equals( 0 ) );
    }

    It( addCount_should_be_set )
    {
        diffBlock.setAddCount( 5 ) ;
        Assert::That( diffBlock.getAddCount(), Equals( 5 ) );
    }

    It( removeStart_should_be_default )
    {
        Assert::That( diffBlock.getRemoveStart(), Equals( 0 ) );
    }

    It( removeStart_should_be_set )
    {
        diffBlock.setRemoveStart( 5 ) ;
        Assert::That( diffBlock.getRemoveStart(), Equals( 5 ) );
    }

    It( removeCount_should_be_default )
    {
        Assert::That( diffBlock.getRemoveCount(), Equals( 0 ) );
    }

    It( removeCount_should_be_set )
    {
        diffBlock.setRemoveCount( 5 ) ;
        Assert::That( diffBlock.getRemoveCount(), Equals( 5 ) );
    }

    It( lines_should_be_empty )
    {
        Assert::That( diffBlock.getGitFileDiffBlockLines().isEmpty(), IsTrue() );
    }

    It( lines_should_be_not_empty )
    {
        Repository::Git::GitFileDiffBlockLine *line = new Repository::Git::GitFileDiffBlockLine();
        diffBlock.addGitFileDiffBlockLine( line );
        Assert::That( diffBlock.getGitFileDiffBlockLines().isEmpty(), IsFalse() );
    }

    It( addCount_should_be_increased )
    {
        Assert::That( diffBlock.getAddCount(), Equals( 0 ) );
        diffBlock.incAddCount();
        Assert::That( diffBlock.getAddCount(), Equals( 1 ) );
    }

    Repository::Git::GitFileDiffBlock diffBlock;
};

Describe( a_git_diff_block_line ) {
    It( type_should_be_MODIFIED )
    {
        Assert::That( line.getLineType(), Equals( Repository::Git::GitType::MODIFIED ) );
    }

    It( type_should_be_ADDED )
    {
        line = Repository::Git::GitFileDiffBlockLine( "line text", 1, Repository::Git::GitType::ADDED );
        Assert::That( line.getLineType(), Equals( Repository::Git::GitType::ADDED ) );
    }

    It( type_should_be_REMOVED )
    {
        line = Repository::Git::GitFileDiffBlockLine( "line text", 1, Repository::Git::GitType::REMOVED );
        Assert::That( line.getLineType(), Equals( Repository::Git::GitType::REMOVED ) );
    }

    It( type_should_be_NONE )
    {
        line = Repository::Git::GitFileDiffBlockLine( "line text", 1, Repository::Git::GitType::NONE );
        Assert::That( line.getLineType(), Equals( Repository::Git::GitType::NONE ) );
    }

    It( line_should_be_empty )
    {
        Assert::That( line.getLine().toStdString(), Is().Empty() );
    }

    It( line_should_be_not_empty )
    {
        line = Repository::Git::GitFileDiffBlockLine( "line text", 1, Repository::Git::GitType::ADDED );
        Assert::That( line.getLine().toStdString(), Is().Not().Empty() );
    }

    It( line_number_should_be_default )
    {
        Assert::That( line.getLineNumber(), Equals( 0 ) );
    }

    It(line_number_should_be_set)
    {
        line.setLineNumber( 1 );
        Assert::That( line.getLineNumber(), Equals( 1 ) );
    }

    Repository::Git::GitFileDiffBlockLine line;
};

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

    Repository::Git::GitEvolutionGraph graph;
};

Describe( a_git_version ) {
    It( commit_id_should_be_empty )
    {
        Assert::That( version.getCommitId().toStdString(), Is().Empty() );
    }

    It( commit_id_should_be_not_empty )
    {
        version = Repository::Git::GitVersion( "1" );
        Assert::That( version.getCommitId().toStdString(), Is().Not().Empty() );
    }

    It( author_should_be_empty )
    {
        Assert::That( version.getAuthor().toStdString(), Is().Empty() );
    }

    It( author_should_be_not_empty )
    {
        version.setAuthor( "autor" );
        Assert::That( version.getAuthor().toStdString(), Is().Not().Empty() );
    }

    It( date_should_be_empty )
    {
        Assert::That( version.getDate().toStdString(), Is().Empty() );
    }

    It( date_should_be_not_empty )
    {
        version.setDate( "date" );
        Assert::That( version.getDate().toStdString(), Is().Not().Empty() );
    }

    It( changed_files_should_be_empty )
    {
        Assert::That( version.getChangedFiles().isEmpty(), IsTrue() );
    }

    It( changed_files_should_contain_item )
    {
        Repository::Git::GitFile *file = new Repository::Git::GitFile();
        version.addChangedFile( file );
        Assert::That( version.getChangedFiles().isEmpty(), IsFalse() );
    }

    It( changed_files_should_contain_file_with_type )
    {
        Repository::Git::GitFile *file1 = new Repository::Git::GitFile( "filename.lua", "/path/to/the/file", Repository::Git::GitType::ADDED );
        Repository::Git::GitFile *file2 = new Repository::Git::GitFile( "filename.lua", "/path/to/the/file", Repository::Git::GitType::REMOVED );
        Repository::Git::GitFile *file3 = new Repository::Git::GitFile( "filename.lua", "/path/to/the/file", Repository::Git::GitType::MODIFIED );

        version.addChangedFile( file1 );
        version.addChangedFile( file2 );
        version.addChangedFile( file3 );

        Assert::That( version.getGitFilesByType( Repository::Git::GitType::ADDED ).size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByType( Repository::Git::GitType::REMOVED ).size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByType( Repository::Git::GitType::MODIFIED ).size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByType( Repository::Git::GitType::NONE ).size(), Equals( 0 ) );
    }

    It( changed_files_should_contain_file_with_extension )
    {
        Repository::Git::GitFile *file1 = new Repository::Git::GitFile( "filename.lua", "/path/to/the/file", Repository::Git::GitType::ADDED );
        Repository::Git::GitFile *file2 = new Repository::Git::GitFile( "filename.cpp", "/path/to/the/file", Repository::Git::GitType::REMOVED );
        Repository::Git::GitFile *file3 = new Repository::Git::GitFile( "filename.h", "/path/to/the/file", Repository::Git::GitType::MODIFIED );

        version.addChangedFile( file1 );
        version.addChangedFile( file2 );
        version.addChangedFile( file3 );

        Assert::That( version.getGitFilesByExtension( "lua" ).size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByExtension( "cpp" ).size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByExtension( "h" ).size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByExtension( "lua,cpp" ).size(), Equals( 2 ) );
        Assert::That( version.getGitFilesByExtension( "lua,h" ).size(), Equals( 2 ) );
        Assert::That( version.getGitFilesByExtension( "h,cpp" ).size(), Equals( 2 ) );
        Assert::That( version.getGitFilesByExtension( "lua,cpp,h" ).size(), Equals( 3 ) );
        Assert::That( version.getGitFilesByExtension( "xml" ).size(), Equals( 0 ) );
    }

    It( changed_files_should_cotain_file_with_type_and_extension )
    {
        Repository::Git::GitFile *file1 = new Repository::Git::GitFile( "filename.lua", "/path/to/the/file", Repository::Git::GitType::ADDED );
        Repository::Git::GitFile *file2 = new Repository::Git::GitFile( "filename.cpp", "/path/to/the/file", Repository::Git::GitType::REMOVED );
        Repository::Git::GitFile *file3 = new Repository::Git::GitFile( "filename.h", "/path/to/the/file", Repository::Git::GitType::MODIFIED );

        version.addChangedFile( file1 );
        version.addChangedFile( file2 );
        version.addChangedFile( file3 );

        Assert::That( version.getGitFilesByTypeAndExtension( "lua", Repository::Git::GitType::ADDED ).size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "cpp,h", Repository::Git::GitType::ADDED ).size(), Equals( 0 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "lua,cpp,h", Repository::Git::GitType::ADDED ).size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "xml", Repository::Git::GitType::ADDED ).size(), Equals( 0 ) );

        Assert::That( version.getGitFilesByTypeAndExtension( "lua", Repository::Git::GitType::REMOVED ).size(), Equals( 0 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "cpp,h", Repository::Git::GitType::REMOVED ).size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "lua,cpp,h", Repository::Git::GitType::REMOVED ).size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "xml", Repository::Git::GitType::REMOVED ).size(), Equals( 0 ) );

        Assert::That( version.getGitFilesByTypeAndExtension( "lua", Repository::Git::GitType::MODIFIED).size(), Equals( 0 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "cpp,h", Repository::Git::GitType::MODIFIED).size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "lua,cpp,h", Repository::Git::GitType::MODIFIED ).size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "xml", Repository::Git::GitType::MODIFIED ).size(), Equals( 0 ) );
    }

    Repository::Git::GitVersion version;
};

Describe( a_git_file_loader ) {
    //ToDo - vytvorit dummy file na testovanie
};

int main(int argc, char *argv[])
{
	return TestRunner::RunAllTests(argc, argv);
}
