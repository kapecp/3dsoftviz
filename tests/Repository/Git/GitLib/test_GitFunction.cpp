#include <igloo/igloo_alt.h>
using namespace igloo;

#include "GitLib/GitFunction.h"

#include <QMap>

Describe( a_git_function ) {

    It( name_should_be_default ) {
        Assert::That( function.getName().toStdString(), Is().Empty() );
    }

    It( name_should_be_set ) {
        function.setName( "name" );
        Assert::That( function.getName().toStdString(), Is().Not().Empty() );
    }

    It( id_should_be_default ) {
        Assert::That( function.getId(), Equals( -1 ) );
    }

    It( id_should_be_set ) {
        function.setId( 1 );
        Assert::That( function.getId(), Equals( 1 ) );
    }

    It( function_row_number_should_be_default ) {
        Assert::That( function.getFunctionRowNumber(), Equals( -1 ) );
    }

    It( function_row_number_should_be_set ) {
        function.setFunctionRowNumber( 2 );
        Assert::That( function.getFunctionRowNumber(), Equals( 2 ) );
    }

    It( type_should_be_default ) {
        Assert::That( function.getType(), Equals( Repository::Git::GitType::ADDED ) );
    }

    It( type_should_be_set ) {
        function.setType( Repository::Git::GitType::REMOVED );
        Assert::That( function.getType(), Equals( Repository::Git::GitType::REMOVED ) );
    }

    It( function_type_should_be_default ) {
        Assert::That( function.getFunctionType(), Equals( Repository::Git::GitFunctionType::LOCALFUNCTION ) );
    }

    It( function_type_should_be_set ) {
        function.setFunctionType( Repository::Git::GitFunctionType::GLOBALFUNCTION );
        Assert::That( function.getFunctionType(), Equals( Repository::Git::GitFunctionType::GLOBALFUNCTION ) );
    }

    It( module_should_be_default ) {
        Assert::That( function.getModule().toStdString(), Is().Empty() );
    }

    It( module_should_be_set ) {
        function.setModule( "module" );
        Assert::That( function.getModule().toStdString(), Is().Not().Empty() );
    }

    It( identifier_should_be_default ) {
        Assert::That( function.getIdentifier().toStdString(), Equals( "function;" ) );
    }

    It( identifier_should_be_set ) {
        function.setName( "print" );
        Assert::That( function.getIdentifier().toStdString(), Equals( "function;print" ) );
    }

    It( identifier_should_contain_module ) {
        function.setModule( "module" );
        Assert::That( function.getIdentifier().toStdString(), Equals( "function;module;" ) );
    }

    It( identifier_should_by_global_function ) {
        function.setFunctionType( Repository::Git::GitFunctionType::GLOBALFUNCTION );
        Assert::That( function.getIdentifier().toStdString(), Equals( "globalFunction;" ) );
    }

    It( identifier_should_be_full ) {
        function.setModule( "module" );
        function.setName( "print" );
        Assert::That( function.getIdentifier().toStdString(), Equals( "function;module;print" ) );
    }

    It( function_callees_should_be_empty ) {
        Assert::That( function.getFunctionCallees()->size(), Equals( 0 ) );
    }

    It( function_callees_add_specific_function ) {
        Repository::Git::GitFunction* calleeFunction = new Repository::Git::GitFunction( "function" );
        function.addFunctionCallee( calleeFunction );
        Assert::That( function.getFunctionCallees()->size(), Equals( 1 ) );
    }

    It( function_callees_remove_specific_function ) {
        Repository::Git::GitFunction* calleeFunction = new Repository::Git::GitFunction( "function" );
        function.addFunctionCallee( calleeFunction );
        Assert::That( function.getFunctionCallees()->size(), Equals( 1 ) );

        function.removeFunctionCallee( function.getFunctionCallees()->value( calleeFunction->getIdentifier() ) );
        Assert::That( function.getFunctionCallees()->size(), Equals( 0 ) );
    }

    It( function_callees_should_be_set ) {
        QMap<QString, Repository::Git::GitFunction*>* map = new QMap<QString, Repository::Git::GitFunction*>();
        Repository::Git::GitFunction* calleeFunction1 = new Repository::Git::GitFunction( "function1" );
        Repository::Git::GitFunction* calleeFunction2 = new Repository::Git::GitFunction( "function2" );
        map->insert( calleeFunction1->getIdentifier(), calleeFunction1 );
        map->insert( calleeFunction2->getIdentifier(), calleeFunction2 );

        Assert::That( function.getFunctionCallees()->size(), Equals( 0 ) );
        function.setFunctionCallees( map );

        Assert::That( function.getFunctionCallees()->size(), Equals( 2 ) );
    }

    It( function_callers_should_be_empty ) {
        Assert::That( function.getFunctionCallers()->size(), Equals( 0 ) );
    }

    It( function_callers_add_specific_function ) {
        Repository::Git::GitFunction* callerFunction = new Repository::Git::GitFunction( "function" );
        function.addFunctionCaller( callerFunction );
        Assert::That( function.getFunctionCallers()->size(), Equals( 1 ) );
    }

    It( function_callers_remove_specific_function ) {
        Repository::Git::GitFunction* callerFunction = new Repository::Git::GitFunction( "function" );
        function.addFunctionCaller( callerFunction );
        Assert::That( function.getFunctionCallers()->size(), Equals( 1 ) );

        function.removeFunctionCaller( function.getFunctionCallers()->value( callerFunction->getIdentifier() ) );
        Assert::That( function.getFunctionCallers()->size(), Equals( 0 ) );
    }

    It( function_callers_should_be_set ) {
        QMap<QString, Repository::Git::GitFunction*>* map = new QMap<QString, Repository::Git::GitFunction*>();
        Repository::Git::GitFunction* callerFunction1 = new Repository::Git::GitFunction( "function1" );
        Repository::Git::GitFunction* callerFunction2 = new Repository::Git::GitFunction( "function2" );
        map->insert( callerFunction1->getIdentifier(), callerFunction1 );
        map->insert( callerFunction2->getIdentifier(), callerFunction2 );

        Assert::That( function.getFunctionCallers()->size(), Equals( 0 ) );
        function.setFunctionCallers( map );

        Assert::That( function.getFunctionCallers()->size(), Equals( 2 ) );
    }

    Repository::Git::GitFunction function;
};
