#include <igloo/igloo_alt.h>
using namespace igloo;

#include "GitLib/GitFile.h"
#include "GitLib/GitVersion.h"

#include <QList>
#include <QMap>

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
        Assert::That( version.getChangedFiles()->isEmpty(), IsTrue() );
    }

    It( changed_files_should_contain_item )
    {
        Repository::Git::GitFile *file = new Repository::Git::GitFile();
        version.addChangedFile( file );
        Assert::That( version.getChangedFiles()->isEmpty(), IsFalse() );
    }

    It( changed_files_should_be_set ) {
        QMap<QString, Repository::Git::GitFile*> *map = new QMap<QString, Repository::Git::GitFile*>();
        Repository::Git::GitFile *file = new Repository::Git::GitFile( "filename.lua", "/path/to/the/filename.lua", Repository::Git::GitType::ADDED );
        map->insert( file->getIdentifier(), file );
        Assert::That( version.getChangedFiles()->size(), Equals( 0 ) );
        version.setChangedFiles( map );
        Assert::That( version.getChangedFiles()->size(), Equals( 1 ) );
    }

    It( changed_files_should_contain_file_with_type )
    {
        Repository::Git::GitFile *file1 = new Repository::Git::GitFile( "filenameA.lua", "/path/to/the/filenameA.lua", Repository::Git::GitType::ADDED );
        Repository::Git::GitFile *file2 = new Repository::Git::GitFile( "filenameR.lua", "/path/to/the/filenameR.lua", Repository::Git::GitType::REMOVED );
        Repository::Git::GitFile *file3 = new Repository::Git::GitFile( "filenameM.lua", "/path/to/the/filenameM.lua", Repository::Git::GitType::MODIFIED );

        version.addChangedFile( file1 );
        version.addChangedFile( file2 );
        version.addChangedFile( file3 );

        Assert::That( version.getGitFilesByType( Repository::Git::GitType::ADDED )->size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByType( Repository::Git::GitType::REMOVED )->size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByType( Repository::Git::GitType::MODIFIED )->size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByType( Repository::Git::GitType::NONE )->size(), Equals( 0 ) );
    }

    It( changed_files_should_contain_file_with_extension )
    {
        Repository::Git::GitFile *file1 = new Repository::Git::GitFile( "filename.lua", "/path/to/the/filename.lua", Repository::Git::GitType::ADDED );
        Repository::Git::GitFile *file2 = new Repository::Git::GitFile( "filename.cpp", "/path/to/the/filename.cpp", Repository::Git::GitType::REMOVED );
        Repository::Git::GitFile *file3 = new Repository::Git::GitFile( "filename.h", "/path/to/the/filename.h", Repository::Git::GitType::MODIFIED );

        version.addChangedFile( file1 );
        version.addChangedFile( file2 );
        version.addChangedFile( file3 );

        Assert::That( version.getGitFilesByExtension( "lua" )->size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByExtension( "cpp" )->size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByExtension( "h" )->size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByExtension( "lua,cpp" )->size(), Equals( 2 ) );
        Assert::That( version.getGitFilesByExtension( "lua,h" )->size(), Equals( 2 ) );
        Assert::That( version.getGitFilesByExtension( "h,cpp" )->size(), Equals( 2 ) );
        Assert::That( version.getGitFilesByExtension( "lua,cpp,h" )->size(), Equals( 3 ) );
        Assert::That( version.getGitFilesByExtension( "xml" )->size(), Equals( 0 ) );
    }

    It( changed_files_should_cotain_file_with_type_and_extension )
    {
        Repository::Git::GitFile *file1 = new Repository::Git::GitFile( "filename.lua", "/path/to/the/filename.lua", Repository::Git::GitType::ADDED );
        Repository::Git::GitFile *file2 = new Repository::Git::GitFile( "filename.cpp", "/path/to/the/filename.cpp", Repository::Git::GitType::REMOVED );
        Repository::Git::GitFile *file3 = new Repository::Git::GitFile( "filename.h", "/path/to/the/filename.h", Repository::Git::GitType::MODIFIED );

        version.addChangedFile( file1 );
        version.addChangedFile( file2 );
        version.addChangedFile( file3 );

        Assert::That( version.getGitFilesByTypeAndExtension( "lua", Repository::Git::GitType::ADDED )->size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "cpp,h", Repository::Git::GitType::ADDED )->size(), Equals( 0 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "lua,cpp,h", Repository::Git::GitType::ADDED )->size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "xml", Repository::Git::GitType::ADDED )->size(), Equals( 0 ) );

        Assert::That( version.getGitFilesByTypeAndExtension( "lua", Repository::Git::GitType::REMOVED )->size(), Equals( 0 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "cpp,h", Repository::Git::GitType::REMOVED )->size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "lua,cpp,h", Repository::Git::GitType::REMOVED )->size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "xml", Repository::Git::GitType::REMOVED )->size(), Equals( 0 ) );

        Assert::That( version.getGitFilesByTypeAndExtension( "lua", Repository::Git::GitType::MODIFIED)->size(), Equals( 0 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "cpp,h", Repository::Git::GitType::MODIFIED)->size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "lua,cpp,h", Repository::Git::GitType::MODIFIED )->size(), Equals( 1 ) );
        Assert::That( version.getGitFilesByTypeAndExtension( "xml", Repository::Git::GitType::MODIFIED )->size(), Equals( 0 ) );
    }

    It( version_should_contain_file_with_identifier )
    {
        Repository::Git::GitFile *file1 = new Repository::Git::GitFile( "filename.lua", "/path/to/the/filename.lua", Repository::Git::GitType::ADDED );

        version.addChangedFile( file1 );

        Assert::That( version.getGitFileByIdentifier( "file;/path/to/the/filename.lua" )->getFilename().toStdString(), Equals( "filename.lua" ) );
    }

    It( version_should_not_contain_file_with_identifier )
    {
        if( !version.getGitFileByIdentifier( "file;/path/to/the/filename.lua" ) ) {
            Assert::That( true );
        } else {
            Assert::That( false );
        }
    }

    It( version_is_analyzed ) {
        version.setIsLoaded( true );
        Assert::That( version.getIsLoaded(), Is().True() );
    }

    It( version_is_not_analyzed ) {
        Assert::That( version.getIsLoaded(), Is().False() );
    }

    Repository::Git::GitVersion version;
};
