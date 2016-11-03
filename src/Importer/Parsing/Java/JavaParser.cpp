
#include "Importer/Parsing/Java/JavaGrammar.h"
#include "Importer/Parsing/Java/JavaParser.h"
#include <fstream>
#include <sstream>
#include <QDirIterator>
#include <QTextStream>

// #define FOREACH(_it, _collection) for (auto _it = _collection.begin(); _it != _collection.end(); ++_it)

namespace Importer {
namespace Parsing {
//using namespace std;
//using namespace parserlib;

struct CompileResult {
	QString file;
	QString errorMessage;
	SourceFileCompilationUnit* astRoot;
	CompileResult() : astRoot( NULL )
	{
	}
};

void ProcessParseResult( const vector<CompileResult>& compileResults, SoftTree& softTree );

JavaParser::JavaParser()
{
#if defined(_WIN32) || defined(WIN32) || defined(WIN64)
	#pragma region connect rules <-> ast
#endif
	CONNECT_RULE_AST( NodeExpressionBracketBefore, expressionBracketBefore );
	CONNECT_RULE_AST( NodeExpressionBracketAfter, expressionBracketAfter );
	CONNECT_RULE_AST( NodeExpressionBracketWithTextAfter, expressionBracketWithTextAfter );
	CONNECT_RULE_AST( NodeExpressionBracket, expressionBracket );
	CONNECT_RULE_AST( NodeExpressionRoundBefore, expressionRoundBefore );
	CONNECT_RULE_AST( NodeExpressionRoundAfter, expressionRoundAfter );
	CONNECT_RULE_AST( NodeExpressionRoundWithTextAfter, expressionRoundWithTextAfter );
	CONNECT_RULE_AST( NodeExpressionRound, expressionRound );
	CONNECT_RULE_AST( NodeExpressionString, expressionString );
	CONNECT_RULE_AST( NodeExpressionPostfix, expressionPostfix );
	CONNECT_RULE_AST( NodeExpression, expression );
	CONNECT_RULE_AST( NodeExpressionList, expressionList );
	CONNECT_RULE_AST( NodeIdentifier, identifier );
	CONNECT_RULE_AST( NodeQualifiedName, qualifiedName );
	CONNECT_RULE_AST( NodeAnnotationIdentifier, annotationIdentifier );
	CONNECT_RULE_AST( NodeAnnotationName, annotationName );
	CONNECT_RULE_AST( NodeAnnotationSequenceBefore, annotationSequenceBefore );
	CONNECT_RULE_AST( NodeAnnotationSequenceAfter, annotationSequenceAfter );
	CONNECT_RULE_AST( NodeAnnotationSequence, annotationSequence );
	CONNECT_RULE_AST( NodeAnnotation, annotation );
	CONNECT_RULE_AST( NodePackageDeclaration, packageDeclaration );
	CONNECT_RULE_AST( NodeImportDeclaration, importDeclaration );
	CONNECT_RULE_AST( NodeClassOrInterfaceModifierBasic, classOrInterfaceModifierBasic );
	CONNECT_RULE_AST( NodeClassOrInterfaceModifier, classOrInterfaceModifier );
	CONNECT_RULE_AST( NodeTypeArgumentsBefore, typeArgumentsBefore );
	CONNECT_RULE_AST( NodeTypeArgumentsAfter, typeArgumentsAfter );
	CONNECT_RULE_AST( NodeTypeArgumentsWithTextAfter, typeArgumentsWithTextAfter );
	CONNECT_RULE_AST( NodeTypeArguments, typeArguments );
	CONNECT_RULE_AST( NodeClassOrInterfaceTypePart, classOrInterfaceTypePart );
	CONNECT_RULE_AST( NodeClassOrInterfaceType, classOrInterfaceType );
	CONNECT_RULE_AST( NodePrimitiveType, primitiveType );
	CONNECT_RULE_AST( NodeTypeArray, typeArray );
	CONNECT_RULE_AST( NodeType, type );
	CONNECT_RULE_AST( NodeTypeBound, typeBound );
	CONNECT_RULE_AST( NodeTypeParameter, typeParameter );
	CONNECT_RULE_AST( NodeTypeParameters, typeParameters );
	CONNECT_RULE_AST( NodeClassDeclarationExtend, classDeclarationExtend );
	CONNECT_RULE_AST( NodeTypeList, typeList );
	CONNECT_RULE_AST( NodeClassDeclarationImplement, classDeclarationImplement );
	CONNECT_RULE_AST( NodeBlockTextBefore, blockTextBefore );
	CONNECT_RULE_AST( NodeBlockTextAfter, blockTextAfter );
	CONNECT_RULE_AST( NodeBlockWithTextAfter, blockWithTextAfter );
	CONNECT_RULE_AST( NodeBlock, block );
	CONNECT_RULE_AST( NodeModifier, modifier );
	CONNECT_RULE_AST( NodeVariableModifier, variableModifier );
	CONNECT_RULE_AST( NodeVariableDeclaratorId, variableDeclaratorId );
	CONNECT_RULE_AST( NodeFormalParameter, formalParameter );
	CONNECT_RULE_AST( NodeLastFormalParameter, lastFormalParameter );
	CONNECT_RULE_AST( NodeFormalParameterList, formalParameterList );
	CONNECT_RULE_AST( NodeFormalParameters, formalParameters );
	CONNECT_RULE_AST( NodeQualifiedNameList, qualifiedNameList );
	CONNECT_RULE_AST( NodeMethodBody, methodBody );
	CONNECT_RULE_AST( NodeMethodDeclaration, methodDeclaration );
	CONNECT_RULE_AST( NodeGenericMethodDeclaration, genericMethodDeclaration );
	CONNECT_RULE_AST( NodeArrayInitializer, arrayInitializer );
	CONNECT_RULE_AST( NodeVariableInitializer, variableInitializer );
	CONNECT_RULE_AST( NodeVariableDeclarator, variableDeclarator );
	CONNECT_RULE_AST( NodeVariableDeclarators, variableDeclarators );
	CONNECT_RULE_AST( NodeFieldDeclaration, fieldDeclaration );
	CONNECT_RULE_AST( NodeConstructorBody, constructorBody );
	CONNECT_RULE_AST( NodeConstructorDeclaration, constructorDeclaration );
	CONNECT_RULE_AST( NodeGenericConstructorDeclaration, genericConstructorDeclaration );
	CONNECT_RULE_AST( NodeConstantDeclarator, constantDeclarator );
	CONNECT_RULE_AST( NodeConstDeclaration, constDeclaration );
	CONNECT_RULE_AST( NodeInterfaceMethodDeclaration, interfaceMethodDeclaration );
	CONNECT_RULE_AST( NodeGenericInterfaceMethodDeclaration, genericInterfaceMethodDeclaration );
	CONNECT_RULE_AST( NodeInterfaceBodyInner, interfaceBodyInner );
	CONNECT_RULE_AST( NodeInterfaceDeclarationInner, interfaceDeclarationInner );
	CONNECT_RULE_AST( NodeAnnotationTypeBody, annotationTypeBody );
	CONNECT_RULE_AST( NodeAnnotationTypeDeclarationInner, annotationTypeDeclarationInner );
	CONNECT_RULE_AST( NodeClassBodyInner, classBodyInner );
	CONNECT_RULE_AST( NodeClassDeclarationInner, classDeclarationInner );
	CONNECT_RULE_AST( NodeEnumConstantsAndBodyDeclarationsInner, enumConstantsAndBodyDeclarationsInner );
	CONNECT_RULE_AST( NodeEnumDeclarationInner, enumDeclarationInner );
	CONNECT_RULE_AST( NodeMemberDeclaration, memberDeclaration );
	CONNECT_RULE_AST( NodeClassBodyDeclaration, classBodyDeclaration );
	CONNECT_RULE_AST( NodeClassBody, classBody );
	CONNECT_RULE_AST( NodeClassDeclaration, classDeclaration );
	CONNECT_RULE_AST( NodeArguments, arguments );
	CONNECT_RULE_AST( NodeEnumConstant, enumConstant );
	CONNECT_RULE_AST( NodeEnumConstants, enumConstants );
	CONNECT_RULE_AST( NodeEnumBodyDeclarations, enumBodyDeclarations );
	CONNECT_RULE_AST( NodeEnumDeclaration, enumDeclaration );
	CONNECT_RULE_AST( NodeInterfaceMemberDeclaration, interfaceMemberDeclaration );
	CONNECT_RULE_AST( NodeInterfaceBodyDeclaration, interfaceBodyDeclaration );
	CONNECT_RULE_AST( NodeInterfaceBody, interfaceBody );
	CONNECT_RULE_AST( NodeInterfaceDeclaration, interfaceDeclaration );
	CONNECT_RULE_AST( NodeAnnotationTypeDeclaration, annotationTypeDeclaration );
	CONNECT_RULE_AST( NodeTypeDeclaration, typeDeclaration );
	CONNECT_RULE_AST( SourceFileCompilationUnit, compilationUnit );

#if defined(_WIN32) || defined(WIN32) || defined(WIN64)
	#pragma endregion
#endif
}

bool JavaParser::Parse( const QString& javaProjectDirectory, SoftTree& softTree,  QString& errorMessage )
{
	QDirIterator dirIt( javaProjectDirectory, QStringList( "*.java" ), QDir::Files, QDirIterator::Subdirectories );
	vector<CompileResult> compileResults;
	while ( dirIt.hasNext() ) {
		QString filePath = dirIt.next();

		CompileResult cr;
		cr.file = filePath;

		QFile f( filePath );
		if ( !f.open( QFile::ReadOnly | QFile::Text ) ) {
			cr.errorMessage = "Cannot open file";
			continue;
		}
		QString fileContent = QTextStream( &f ).readAll();
		auto fileContentStd = fileContent.toStdString();
		parserlib::input input( fileContentStd.begin(), fileContentStd.end() );
		bool pass = false;
		parserlib::error_list errors;
		try {
			pass = parserlib::parse( input, compilationUnit, whitespace, errors, cr.astRoot );
		}
		catch ( std::exception& ex ) {
			cr.errorMessage = ex.what();
			compileResults.push_back( cr );
			continue;
		}
		if ( !pass ) {
			std::ostringstream oss;
			oss << "    found " << errors.size() << " " << ( errors.size() > 1 ? "errors" : "error" ) << ": ";
			errors.sort();
			for ( auto it = errors.begin(); it != errors.end(); ++it ) {
				const auto& e = *it;
				oss << "line " << e.m_begin.m_line << ", col " << e.m_begin.m_col << ": ";
				oss << endl;
			}
			cr.errorMessage = QString::fromStdString( oss.str() );
		}
		compileResults.push_back( cr );
	}

	foreach ( const CompileResult& cr, compileResults ) {
		if ( !cr.errorMessage.isEmpty() ) {
			if ( !errorMessage.isEmpty() ) {
				errorMessage += "\n";
			}
			errorMessage += cr.file + ":\n" + cr.errorMessage;
		}
	}

	if ( errorMessage.isEmpty() ) {
		ProcessParseResult( compileResults, softTree );
		return true;
	}
	else {
		return false;
	}
}

QString GetQualifiedName( const NodeQualifiedName* qualifiedName )
{
	QString s;
	for ( const auto& i : qualifiedName->identifier ) {
		s += ( s.isEmpty() ? "" : "." ) + i->GetValue();
	}
	return s;
}

Modifier::ModifierEnum GetClassOrInterfaceModifier( const NodeClassOrInterfaceModifier* classOrInterfaceModifier )
{
	if ( classOrInterfaceModifier->classOrInterfaceModifierBasic != nullptr ) {
		QString coim = classOrInterfaceModifier->classOrInterfaceModifierBasic->GetValue();
		QString upperCoim = coim.toUpper();
		if ( upperCoim == "PUBLIC" ) {
			return Modifier::PUBLIC;
		}
		if ( upperCoim == "PROTECTED" ) {
			return Modifier::PROTECTED;
		}
		if ( upperCoim == "PRIVATE" ) {
			return Modifier::PRIVATE;
		}
	}
	return Modifier::UNKNOWN;
}

QString GetTypeArguments( const NodeTypeArguments* typeArguments )
{
	QString s;
	s += "<";
	s += typeArguments->typeArgumentsBefore->GetValue();
	for ( const auto& t : typeArguments->typeArgumentsWithTextAfter ) {
		s += GetTypeArguments( t->typeArguments );
		s += t->typeArgumentsAfter->GetValue();
	}
	s += ">";
	return s;
}

QString GenTypeArray( uint count )
{
	QString s;
	for ( uint i = 0; i < count; ++i ) {
		s += "[]";
	}
	return s;
}

QString GetType( const NodeType* type )
{
	QString s;
	if ( type ) {
		if ( type->classOrInterfaceType != nullptr ) {
			for ( const NodeClassOrInterfaceTypePart* coitp : type->classOrInterfaceType->classOrInterfaceTypePart ) {
				if ( !s.isEmpty() ) {
					s += '.';
				}
				s += coitp->identifier->GetValue();
				if ( coitp->typeArguments != nullptr ) {
					s += GetTypeArguments( coitp->typeArguments );
				}
			}
		}
		else if ( type->primitiveType != nullptr ) {
			s += type->primitiveType->GetValue();
		}
		else {
			return s;
		}
		s += GenTypeArray( type->typeArray.GetCount() );
	}
	else {
		s = "void";
	}
	return s;
}

QString GetBlockContent( const NodeBlock* block )
{
	QString s;
	s += "{";
	s += block->blockTextBefore->GetValue();
	for ( const auto& b : block->blockWithTextAfter ) {
		s += GetBlockContent( b->block );
		s += b->blockTextAfter->GetValue();
	}
	s += "}";
	return s;
}

Modifier::ModifierEnum GetModifier( const NodeModifier* modifier )
{
	QString coim = modifier->GetValue();
	QString upperCoim = coim.toUpper();
	if ( upperCoim == "PUBLIC" ) {
		return Modifier::PUBLIC;
	}
	else if ( upperCoim == "PROTECTED" ) {
		return Modifier::PROTECTED;
	}
	else if ( upperCoim == "PRIVATE" ) {
		return Modifier::PRIVATE;
	}
	else {
		return Modifier::UNKNOWN;
	}
}

QList<Parameter> GetParametersFromFormalParameterList( const NodeFormalParameterList* fpl )
{
	QList<Parameter> parameters;
	for ( const NodeFormalParameter* fp : fpl->formalParameter ) {
		Parameter parameter;
		parameter.type = GetType( fp->type ) + GenTypeArray( fp->variableDeclaratorId->typeArray.GetCount() );
		parameter.name = fp->variableDeclaratorId->identifier->GetValue();
		parameters.push_back( parameter );
	}
	if ( fpl->lastFormalParameter != nullptr ) {
		Parameter parameter;
		parameter.type = GetType( fpl->lastFormalParameter->type ) + GenTypeArray( fpl->lastFormalParameter->variableDeclaratorId->typeArray.GetCount() );
		parameter.name = fpl->lastFormalParameter->variableDeclaratorId->identifier->GetValue();
		parameters.push_back( parameter );
	}
	return parameters;
}

Method GetMethodFromMethodDeclaration( const NodeMethodDeclaration* methodDeclaration, Modifier::ModifierEnum modifier )
{
	Method method;
	method.modifier = modifier;
	method.returnType = GetType( methodDeclaration->type );
	method.name = methodDeclaration->identifier->GetValue();
	if ( methodDeclaration->formalParameters->formalParameterList != nullptr ) {
		method.parameters = GetParametersFromFormalParameterList( methodDeclaration->formalParameters->formalParameterList );
	}
	method.content = methodDeclaration->methodBody->block != nullptr ? GetBlockContent( methodDeclaration->methodBody->block ) : QString();
	return method;
}

QVector<Attribute> GetAttributeFromFieldDeclaration( const NodeFieldDeclaration* fieldDeclaration, Modifier::ModifierEnum modifier )
{
	QVector<Attribute> attributes;
	auto type = GetType( fieldDeclaration->type );
	for ( const NodeVariableDeclarator* vdi : fieldDeclaration->variableDeclarators->variableDeclarator ) {
		Attribute attribute;
		auto typeArray = GenTypeArray( vdi->variableDeclaratorId->typeArray.GetCount() );
		attribute.modifier = modifier;
		attribute.type = type + typeArray;
		attribute.name = vdi->variableDeclaratorId->identifier->GetValue();
		attributes.push_back( attribute );
	}
	return attributes;
}

Method GetConstructorFromConstructorDeclaration( const NodeConstructorDeclaration* constructorDeclaration, Modifier::ModifierEnum modifier )
{
	Method method;
	method.modifier = modifier;
	method.name = constructorDeclaration->identifier->GetValue();
	if ( constructorDeclaration->formalParameters->formalParameterList != nullptr ) {
		method.parameters = GetParametersFromFormalParameterList( constructorDeclaration->formalParameters->formalParameterList );
	}
	method.content = GetBlockContent( constructorDeclaration->constructorBody->block );
	return method;
}

vector<Attribute> GetAttributeFromConstDeclaration( const NodeConstDeclaration* constDeclaration, Modifier::ModifierEnum modifier )
{
	vector<Attribute> attributes;
	auto type = GetType( constDeclaration->type );
	for ( const NodeConstantDeclarator* cd : constDeclaration->constantDeclarator ) {
		Attribute attribute;
		auto typeArray = GenTypeArray( cd->typeArray.GetCount() );
		attribute.modifier = modifier;
		attribute.type = type + typeArray;
		attribute.name = cd->identifier->GetValue();
		attributes.push_back( attribute );
	}
	return attributes;
}

Method GetMethodFromInterfaceMethodDeclaration( const NodeInterfaceMethodDeclaration* interfaceMethodDeclaration, Modifier::ModifierEnum modifier )
{
	Method method;
	method.modifier = modifier;
	method.returnType = GetType( interfaceMethodDeclaration->type );
	method.name = interfaceMethodDeclaration->identifier->GetValue();
	if ( interfaceMethodDeclaration->formalParameters->formalParameterList != nullptr ) {
		method.parameters = GetParametersFromFormalParameterList( interfaceMethodDeclaration->formalParameters->formalParameterList );
	}
	return method;
}

void ProcessParseResult( const vector<CompileResult>& compileResults, SoftTree& softTree )
{
	QMap<QString, Namespace> javaPackages;

	for ( const auto& cr : compileResults ) {
		const SourceFileCompilationUnit* javaAstRoot = cr.astRoot;

		QString packageName;
		if ( !javaAstRoot->packageDeclaration.Empty() ) {
			packageName = GetQualifiedName( javaAstRoot->packageDeclaration.First()->qualifiedName );
		}
		javaPackages[packageName].name = packageName;

		for ( const NodeTypeDeclaration* typeDecl : javaAstRoot->typeDeclaration ) {
			Modifier::ModifierEnum modifierForTypeDecl = Modifier::UNKNOWN;
			for ( const NodeClassOrInterfaceModifier* coim : typeDecl->classOrInterfaceModifier ) {
				modifierForTypeDecl = GetClassOrInterfaceModifier( coim );
				if ( modifierForTypeDecl != Modifier::UNKNOWN ) {
					break;
				}
			}
			if ( typeDecl->classDeclaration != nullptr ) {
				Class class1;
				class1.modifier = modifierForTypeDecl;
				class1.name = typeDecl->classDeclaration->identifier->GetValue();
				for ( const NodeClassBodyDeclaration* cbd : typeDecl->classDeclaration->classBody->classBodyDeclaration ) {
					if ( cbd->memberDeclaration == nullptr ) {
						continue;
					}

					Modifier::ModifierEnum modifierForClassMember = Modifier::UNKNOWN;
					for ( const NodeModifier* m : cbd->memberDeclaration->modifier ) {
						modifierForClassMember = GetModifier( m );
						if ( modifierForClassMember != Modifier::UNKNOWN ) {
							break;
						}
					}

					if ( cbd->memberDeclaration->methodDeclaration != nullptr ) {
						class1.methods.push_back( GetMethodFromMethodDeclaration( cbd->memberDeclaration->methodDeclaration, modifierForClassMember ) );
					}
					else if ( cbd->memberDeclaration->genericMethodDeclaration != nullptr ) {
						class1.methods.push_back( GetMethodFromMethodDeclaration( cbd->memberDeclaration->genericMethodDeclaration->methodDeclaration, modifierForClassMember ) );
					}
					else if ( cbd->memberDeclaration->fieldDeclaration != nullptr ) {
						auto fields = GetAttributeFromFieldDeclaration( cbd->memberDeclaration->fieldDeclaration, modifierForClassMember );
						for ( const auto& f : fields ) {
							class1.attributes.push_back( f );
						}
					}
					else if ( cbd->memberDeclaration->constructorDeclaration != nullptr ) {
						class1.methods.push_back( GetConstructorFromConstructorDeclaration( cbd->memberDeclaration->constructorDeclaration, modifierForClassMember ) );
					}
					else if ( cbd->memberDeclaration->genericConstructorDeclaration != nullptr ) {
						class1.methods.push_back( GetConstructorFromConstructorDeclaration( cbd->memberDeclaration->genericConstructorDeclaration->constructorDeclaration, modifierForClassMember ) );
					}
				}
				javaPackages[packageName].classes.push_back( class1 );
			}
			else if ( typeDecl->enumDeclaration != nullptr ) {
				Enum enum1;
				enum1.modifier = modifierForTypeDecl;
				enum1.name = typeDecl->enumDeclaration->identifier->GetValue();
				if ( typeDecl->enumDeclaration->enumConstants != nullptr ) {
					for ( const NodeEnumConstant* ec : typeDecl->enumDeclaration->enumConstants->enumConstant ) {
						EnumConstant enumConstant;
						enumConstant.name = ec->identifier->GetValue();
						enum1.enums.push_back( enumConstant );
					}
				}
				if ( typeDecl->enumDeclaration->enumBodyDeclarations != nullptr ) {
					for ( const NodeClassBodyDeclaration* cbd : typeDecl->enumDeclaration->enumBodyDeclarations->classBodyDeclarations ) {
						if ( cbd->memberDeclaration == nullptr ) {
							continue;
						}

						Modifier::ModifierEnum modifierForEnumMember = Modifier::UNKNOWN;
						for ( const NodeModifier* m : cbd->memberDeclaration->modifier ) {
							modifierForEnumMember = GetModifier( m );
							if ( modifierForEnumMember != Modifier::UNKNOWN ) {
								break;
							}
						}

						if ( cbd->memberDeclaration->methodDeclaration != nullptr ) {
							enum1.methods.push_back( GetMethodFromMethodDeclaration( cbd->memberDeclaration->methodDeclaration, modifierForEnumMember ) );
						}
						else if ( cbd->memberDeclaration->genericMethodDeclaration != nullptr ) {
							enum1.methods.push_back( GetMethodFromMethodDeclaration( cbd->memberDeclaration->genericMethodDeclaration->methodDeclaration, modifierForEnumMember ) );
						}
						else if ( cbd->memberDeclaration->fieldDeclaration != nullptr ) {
							auto fields = GetAttributeFromFieldDeclaration( cbd->memberDeclaration->fieldDeclaration, modifierForEnumMember );
							for ( const auto& f : fields ) {
								enum1.attributes.push_back( f );
							}
						}
						else if ( cbd->memberDeclaration->constructorDeclaration != nullptr ) {
							enum1.methods.push_back( GetConstructorFromConstructorDeclaration( cbd->memberDeclaration->constructorDeclaration, modifierForEnumMember ) );
						}
						else if ( cbd->memberDeclaration->genericConstructorDeclaration != nullptr ) {
							enum1.methods.push_back( GetConstructorFromConstructorDeclaration( cbd->memberDeclaration->genericConstructorDeclaration->constructorDeclaration, modifierForEnumMember ) );
						}
					}
				}
				javaPackages[packageName].enums.push_back( enum1 );
			}
			else if ( typeDecl->interfaceDeclaration != nullptr ) {
				Interface interface1;
				interface1.modifier = modifierForTypeDecl;
				interface1.name = typeDecl->interfaceDeclaration->identifier->GetValue();
				for ( const NodeInterfaceBodyDeclaration* ibd : typeDecl->interfaceDeclaration->interfaceBody->interfaceBodyDeclaration ) {
					if ( ibd->interfaceMemberDeclaration != nullptr ) {
						Modifier::ModifierEnum modifierForInterfaceMember = Modifier::UNKNOWN;
						for ( const NodeModifier* m : ibd->interfaceMemberDeclaration->modifier ) {
							modifierForInterfaceMember = GetModifier( m );
							if ( modifierForInterfaceMember != Modifier::UNKNOWN ) {
								break;
							}
						}
						if ( ibd->interfaceMemberDeclaration->constDeclaration != nullptr ) {
							auto fields = GetAttributeFromConstDeclaration( ibd->interfaceMemberDeclaration->constDeclaration, modifierForInterfaceMember );
							for ( const auto& f : fields ) {
								interface1.attributes.push_back( f );
							}
						}
						else if ( ibd->interfaceMemberDeclaration->interfaceMethodDeclaration != nullptr ) {
							interface1.methods.push_back( GetMethodFromInterfaceMethodDeclaration( ibd->interfaceMemberDeclaration->interfaceMethodDeclaration, modifierForInterfaceMember ) );
						}
						else if ( ibd->interfaceMemberDeclaration->genericInterfaceMethodDeclaration != nullptr ) {
							interface1.methods.push_back( GetMethodFromInterfaceMethodDeclaration( ibd->interfaceMemberDeclaration->genericInterfaceMethodDeclaration->interfaceMethodDeclaration, modifierForInterfaceMember ) );
						}
					}
				}
				javaPackages[packageName].interfaces.push_back( interface1 );
			}
		}
	}

	for ( const auto& javaPackage : javaPackages ) {
		softTree.namespaces.push_back( javaPackage );
	}
}
}
}
