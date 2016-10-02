#include <igloo/igloo_alt.h>
using namespace igloo;

#include "GitLib/GitFileDiffBlockLine.h"

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
