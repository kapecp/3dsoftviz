#pragma once

#include <QString>
#include <sstream>
#include <vector>
#include <parserlib.hpp>

#define CONNECT_RULE_AST(ClassName, RuleName) parserlib::ast<ClassName> _##RuleName(RuleName)

namespace Importer {
namespace Parsing {
using std::vector;
using std::list;
using parserlib::ast_ptr;
using parserlib::ast_list;
using parserlib::ast_node;
using parserlib::ast_stack;
using parserlib::ast_container;
using parserlib::rule;

typedef unsigned int uint;

template <typename T, bool _OPTIONAL = false>
class AstValue : public ast_ptr <T, _OPTIONAL>
{
public:
	explicit AstValue( T* obj = 0 )
		: ast_ptr<T, _OPTIONAL>( obj )
	{
	}

	explicit AstValue( const ast_ptr<T, _OPTIONAL>& src )
		: ast_ptr<T, _OPTIONAL>( src )
	{
	}
};

template <typename T>
class AstList : public ast_list <T>
{
public:
	AstList()
		: ast_list<T>()
	{
	}

	explicit AstList( const ast_list<T>& src )
		: ast_list<T>( src )
	{
	}

	/*const container& GetItems() const
	{
	    return objects();
	}*/

	vector<T*> ToStdVector() const
	{
		return vector<T*>( this->objects().begin(), this->objects().end() );
	}

	const T* At( uint index ) const
	{
		auto iter = this->objects().begin();
		advance( iter, index );
		return *iter;
	}

	const T* First() const
	{
		return this->objects().front();
	}

	uint GetCount() const
	{
		return this->objects().size();
	}

	typename list<T*>::const_iterator begin() const
	{
		return this->objects().begin();
	}

	typename list<T*>::const_iterator end() const
	{
		return this->objects().end();
	}

	bool Empty() const
	{
		return this->objects().empty();
	}
};

class NodeTerm : public ast_node
{
	template <class T> friend class ast;

private:
	QString value;

public:
	NodeTerm()
		: ast_node()
	{
	}

	explicit NodeTerm( const ast_node& n )
		: ast_node( n )
	{
	}

	explicit NodeTerm( const QString& val )
		: ast_node(), value( val )
	{
	}

	virtual ~NodeTerm()
	{
	}

	void construct( ast_stack& st )
	{
		value = QString::fromStdString( std::string( m_begin.m_it, m_end.m_it ) );
	}


	virtual QString GetValue() const
	{
		return value;
	}

	void SetValue( const QString& str )
	{
		value = str;
	}
};

class NodeContainer : public ast_container
{
public:
	NodeContainer()
		: ast_container()
	{
	}

	explicit NodeContainer( const ast_container& src )
		: ast_container( src )
	{
	}
};
}
}
