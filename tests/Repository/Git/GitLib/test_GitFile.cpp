#include <igloo/igloo_alt.h>
using namespace igloo;

#include "GitLib/GitFile.h"
#include "GitLib/GitFileDiffBlock.h"

#include <QList>

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
