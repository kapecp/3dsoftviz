#pragma once

#include "JavaAst.h"
#include "../SoftTree.h"

namespace Importer {
namespace Parsing {
class JavaParser
{
public:
	JavaParser();
	bool Parse( const QString& javaProjectDirectory, SoftTree& softTree, QString& errorMessage );
};

QString GetQualifiedName( const NodeQualifiedName* qualifiedName );
Modifier::ModifierEnum GetClassOrInterfaceModifier( const NodeClassOrInterfaceModifier* classOrInterfaceModifier );
QString GetTypeArguments( const NodeTypeArguments* typeArguments );
QString GenTypeArray( uint count );
QString GetType( const NodeType* type );
QString GetBlockContent( const NodeBlock* block );
Modifier::ModifierEnum GetModifier( const NodeModifier* modifier );
QList<Parameter> GetParametersFromFormalParameterList( const NodeFormalParameterList* fpl );
Method GetMethodFromMethodDeclaration( const NodeMethodDeclaration* methodDeclaration, Modifier::ModifierEnum modifier );
QVector<Attribute> GetAttributeFromFieldDeclaration( const NodeFieldDeclaration* fieldDeclaration, Modifier::ModifierEnum modifier );
Method GetConstructorFromConstructorDeclaration( const NodeConstructorDeclaration* constructorDeclaration, Modifier::ModifierEnum modifier );
vector<Attribute> GetAttributeFromConstDeclaration( const NodeConstDeclaration* constDeclaration, Modifier::ModifierEnum modifier );
Method GetMethodFromInterfaceMethodDeclaration( const NodeInterfaceMethodDeclaration* interfaceMethodDeclaration, Modifier::ModifierEnum modifier );
}
}
