#include <igloo/igloo_alt.h>
using namespace igloo;

#include "GitLib/GitFileDiffBlock.h"
#include "GitLib/GitFileDiffBlockLine.h"

#include <QList>

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
