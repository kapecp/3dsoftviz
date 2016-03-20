#include <igloo/igloo_alt.h>
using namespace igloo;

#include "GitLib/GitMetaData.h"
#include "GitLib/GitFile.h"

Describe( a_git_meta_data ) {

    It( occurence_should_be_default ) {
        Assert::That( metaData.getOccurence(), Equals( 0 ) );
    }

    It( occurence_shoudl_be_modified ) {
        metaData.increaseOccurence();
        Assert::That( metaData.getOccurence(), Equals( 1 ) );
        metaData.decreaseOccurence();
        Assert::That( metaData.getOccurence(), Equals( 0 ) );
    }

    It( last_diff_version_should_be_default ) {
        Assert::That( metaData.getLastDiffVersion().toStdString(), Is().Empty() );
    }

    It( last_diff_version_should_be_set ) {
        metaData.setLastDiffVersion( "1" );
        Assert::That( metaData.getLastDiffVersion().toStdString(), Is().Not().Empty() );
    }

    It( last_function_interval_should_be_default ) {
        Assert::That( metaData.getLastFunctionInterval().toStdString(), Is().Empty() );
    }

    It( last_function_interval_should_be_set ) {
        metaData.setLastFunctionInterval( "11-12" );
        Assert::That( metaData.getLastFunctionInterval().toStdString(), Is().Not().Empty() );
    }

    It( lua_mapping_should_be_default ) {
        Assert::That( metaData.getLuaMapping(), Equals( -1 ) );
    }

    It( lua_mapping_should_be_set ) {
        metaData.setLuaMapping( 12 );
        Assert::That( metaData.getLuaMapping(), Equals( 12 ) );
    }

    It( first_occurence_in_graph_should_be_default ) {
        Assert::That( metaData.getFirstOccurence(), Equals( -1 ) );
    }

    It( first_occurence_in_graph_should_be_set ) {
        metaData.setFirstOccurence( 10 );
        Assert::That( metaData.getFirstOccurence(), Equals( 10 ) );
    }

    It( changed_version_should_be_default ) {
        Assert::That( metaData.getChangedVersion(), Equals( -1 ) );
    }

    It( changed_version_should_be_set ) {
        metaData.setChangedVersion( 2 );
        Assert::That( metaData.getChangedVersion(), Equals( 2 ) );
    }

    It( changed_version_should_be_clear ) {
        changed_version_should_be_set();
        metaData.clearChangedVersion();
        Assert::That( metaData.getChangedVersion(), Equals( -1 ) );
    }

    It( call_tree_should_be_empty ) {
        if( !metaData.getCallTree() ) {
            Assert::That( true );
        } else {
            Assert::That( false );
        }
    }

    It( call_tree_should_be_set ) {
        Repository::Git::GitFile* callTree = new Repository::Git::GitFile();
        metaData.setCallTree( callTree );
        if( metaData.getCallTree() ) {
            Assert::That( true );
        } else {
            Assert::That( false );
        }
    }

    Repository::Git::GitMetaData metaData;
};
