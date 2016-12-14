
#include "Importer/Parsing/SoftTree.h"
#include <map>

#define TABS(_count) QString(_count, '\t')

namespace Importer {
namespace Parsing {
// Modifier

QString Modifier::ToString( ModifierEnum modifier )
{
	static const std::map<ModifierEnum, QString> enumToStr {
		{ PUBLIC, "public" },
		{ PROTECTED, "protected" },
		{ PRIVATE, "private" }
	};

	auto found = enumToStr.find( modifier );
	return found != enumToStr.end() ? found->second : QString();
}

// Attribute

QString Attribute::ToString( int tabs ) const
{
	auto modifierStr = Modifier::ToString( modifier );
	return TABS( tabs ) + ( modifierStr.isEmpty() ? "" : modifierStr + " " ) + type + " " + name;
}

// Parameter

QString Parameter::ToString( int tabs ) const
{
	return TABS( tabs ) + type + " " + name;
}

// Method

QString Method::ToString( int tabs ) const
{
	QString params;
	for ( const auto& p : parameters ) {
		params += ( params.isEmpty() ? "" : ", " ) + p.ToString( 0 );
	}
	auto modifierStr = Modifier::ToString( modifier );
	return TABS( tabs ) + ( modifierStr.isEmpty() ? "" : modifierStr + " " ) + ( IsConstructor() ? "" : returnType + " " ) + name + "(" + params + ")" + ( IsInterface() ? "" : "\n" + TABS( tabs ) + content );
}

bool Method::IsConstructor() const
{
	return returnType.isEmpty();
}

bool Method::IsInterface() const
{
	return content.isEmpty();
}

bool Method::HasResult() const
{
	return !returnType.isEmpty() && returnType.toLower() != "void";
}

uint Method::GetLineOfCodes() const
{
	return static_cast<unsigned int>( content.count( '\n' ) );
}

// Class

Class::Class()
	: modifier( Modifier::UNKNOWN )
{
}

QString Class::ToString( int tabs ) const
{
	QString s = TABS( tabs ) + "class " + name + "\n" + TABS( tabs ) + "{\n";
	for ( const auto& attribute : attributes ) {
		s += attribute.ToString( tabs + 1 ) + ";\n";
	}
	if ( !attributes.empty() ) {
		s += '\n';
	}
	bool mFirst = true;
	for ( const auto& method : methods ) {
		if ( !mFirst ) {
			s += "\n\n";
		}
		s += method.ToString( tabs + 1 ) + ( method.IsInterface() ? ";" : "" );
		mFirst = false;
	}
	s += "\n" + TABS( tabs ) + "}";
	return s;
}

// EnumConstant

QString EnumConstant::ToString( int tabs ) const
{
	return TABS( tabs ) + name;
}

// Enum

Enum::Enum()
	: modifier( Modifier::UNKNOWN )
{
}

QString Enum::ToString( int tabs ) const
{
	QString s = TABS( tabs ) + "enum " + name + "\n" + TABS( tabs ) + "{\n";
	bool eFirst = true;
	for ( const auto& e : enums ) {
		if ( !eFirst ) {
			s += ",\n";
		}
		s += e.ToString( tabs + 1 );
		eFirst = false;
	}
	if ( !enums.empty() ) {
		s += ";\n\n";
	}
	for ( const auto& attribute : attributes ) {
		s += attribute.ToString( tabs + 1 ) + ";\n";
	}
	if ( !attributes.empty() ) {
		s += '\n';
	}
	bool mFirst = true;
	for ( const auto& method : methods ) {
		if ( !mFirst ) {
			s += "\n\n";
		}
		s += method.ToString( tabs + 1 ) + ( method.IsInterface() ? ";" : "" );
		mFirst = false;
	}
	s += "\n" + TABS( tabs ) + "}";
	return s;
}

// Interface

Interface::Interface()
	: modifier( Modifier::UNKNOWN )
{
}

QString Interface::ToString( int tabs ) const
{
	QString s = TABS( tabs ) + "interface " + name + "\n" + TABS( tabs ) + "{\n";
	for ( const auto& attribute : attributes ) {
		s += attribute.ToString( tabs + 1 ) + ";\n";
	}
	if ( !attributes.empty() ) {
		s += '\n';
	}
	bool mFirst = true;
	for ( const auto& method : methods ) {
		if ( !mFirst ) {
			s += "\n\n";
		}
		s += method.ToString( tabs + 1 ) + ( method.IsInterface() ? ";" : "" );
		mFirst = false;
	}
	s += "\n" + TABS( tabs ) + "}";
	return s;
}

// Namespace

QString Namespace::ToString( int tabs ) const
{
	QString s = "namespace " + ( name.isEmpty() ? "(default)" : name ) + " --> \n\n";
	for ( const auto& c : classes ) {
		s += c.ToString( tabs ) + "\n\n";
	}
	for ( const auto& e : enums ) {
		s += e.ToString( tabs ) + "\n\n";
	}
	for ( const auto& i : interfaces ) {
		s += i.ToString( tabs ) + "\n\n";
	}
	return s;
}

// SoftTree

QString SoftTree::ToString() const
{
	QString s;
	foreach ( const Namespace& _namespace, namespaces ) {
		s += _namespace.ToString( 0 ) + '\n';
	}
	return s;
}
}
}
