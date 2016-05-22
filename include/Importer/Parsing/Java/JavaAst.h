#pragma once

#include "ParserlibBase.h"

namespace Importer {
namespace Parsing {
#pragma region forward declaration

struct NodeExpressionBracket;
struct NodeExpressionRound;
struct NodeExpression;
struct NodeTypeArguments;
struct NodeBlock;
struct NodeVariableInitializer;

#pragma endregion

#define astVal(type) const type*
#define astList(type) const AstList<type>&

struct NodeExpressionBracketBefore : NodeTerm {
};

struct NodeExpressionBracketAfter : NodeTerm {
};

struct NodeExpressionBracketWithTextAfter : NodeContainer {
	AstValue<NodeExpressionBracket> expressionBracket;
	AstValue<NodeExpressionBracketAfter> expressionBracketAfter;
};

struct NodeExpressionBracket : NodeContainer {
	AstValue<NodeExpressionBracketBefore> expressionBracketBefore;
	AstList<NodeExpressionBracketWithTextAfter> expressionBracketWithTextAfter;
};

struct NodeExpressionRoundBefore : NodeTerm {
};

struct NodeExpressionRoundAfter : NodeTerm {
};

struct NodeExpressionRoundWithTextAfter : NodeContainer {
	AstValue<NodeExpressionRound> expressionRound;
	AstValue<NodeExpressionRoundAfter> expressionRoundAfter;
};

struct NodeExpressionRound : NodeContainer {
	AstValue<NodeExpressionRoundBefore> expressionRoundBefore;
	AstList<NodeExpressionRoundWithTextAfter> expressionRoundWithTextAfter;
};

struct NodeExpressionString : NodeContainer {
	AstValue<NodeExpression> expression;
};

struct NodeExpressionPostfix : NodeContainer {
	AstValue<NodeExpressionString, true> expressionString;
	AstValue<NodeExpressionRound, true> expressionRound;
	AstValue<NodeExpressionBracket, true> expressionBracket;
	AstValue<NodeExpression, true> expression;
};

struct NodeExpression : NodeContainer {
	AstValue<NodeExpressionPostfix> expressionPostfix;
};

struct NodeExpressionList : NodeContainer {
	AstList<NodeExpression> expression;
};

struct NodeIdentifier : NodeTerm {
};

struct NodeQualifiedName : NodeContainer {
	AstList<NodeIdentifier> identifier;
};

struct NodeAnnotationIdentifier : NodeTerm {
};

struct NodeAnnotationName : NodeContainer {
	AstList<NodeAnnotationIdentifier> annotationIdentifier;
};

struct NodeAnnotationSequenceBefore : NodeTerm {
};

struct NodeAnnotationSequenceAfter : NodeTerm {
};

struct NodeAnnotationSequence : NodeContainer {
	AstValue<NodeAnnotationSequenceBefore> annotationSequenceBefore;
	AstValue<NodeAnnotationSequence, true> annotationSequence;
	AstValue<NodeAnnotationSequenceAfter> annotationSequenceAfter;
};

struct NodeAnnotation : NodeContainer {
	AstValue<NodeAnnotationName> annotationName;
	AstValue<NodeAnnotationSequence, true> annotationSequence;
};

struct NodePackageDeclaration : NodeContainer {
	//AstList<NodeAnnotation> annotation;
	AstValue<NodeQualifiedName> qualifiedName;
};

struct NodeImportDeclaration : NodeContainer {
	AstValue<NodeQualifiedName> qualifiedName;
};

struct NodeClassOrInterfaceModifierBasic : NodeTerm {
};

struct NodeClassOrInterfaceModifier : NodeContainer {
	AstValue<NodeClassOrInterfaceModifierBasic, true> classOrInterfaceModifierBasic;
	AstList<NodeAnnotation> annotation;
};

struct NodeTypeArgumentsBefore : NodeTerm {
};

struct NodeTypeArgumentsAfter : NodeTerm {
};

struct NodeTypeArgumentsWithTextAfter : NodeContainer {
	AstValue<NodeTypeArguments> typeArguments;
	AstValue<NodeTypeArgumentsAfter> typeArgumentsAfter;
};

struct NodeTypeArguments : NodeContainer {
	AstValue<NodeTypeArgumentsBefore> typeArgumentsBefore;
	AstList<NodeTypeArgumentsWithTextAfter> typeArgumentsWithTextAfter;
};

struct NodeClassOrInterfaceTypePart : NodeContainer {
	AstValue<NodeIdentifier> identifier;
	AstValue<NodeTypeArguments, true> typeArguments;
};

struct NodeClassOrInterfaceType : NodeContainer {
	AstList<NodeClassOrInterfaceTypePart> classOrInterfaceTypePart;
};

struct NodePrimitiveType : NodeTerm {
};

struct NodeTypeArray : NodeTerm {
};

struct NodeType : NodeContainer {
	AstValue<NodeClassOrInterfaceType, true> classOrInterfaceType;
	AstValue<NodePrimitiveType, true> primitiveType;
	AstList<NodeTypeArray> typeArray;
};

struct NodeTypeBound : NodeContainer {
	AstList<NodeType> type;
};

struct NodeTypeParameter : NodeContainer {
	AstValue<NodeIdentifier> identifier;
	AstValue<NodeTypeBound, true> typeBound;
};

struct NodeTypeParameters : NodeContainer {
	AstList<NodeTypeParameter> typeParameter;
};

struct NodeClassDeclarationExtend : NodeContainer {
	AstValue<NodeType> type;
};

struct NodeTypeList : NodeContainer {
	AstList<NodeType> type;
};

struct NodeClassDeclarationImplement : NodeContainer {
	AstValue<NodeTypeList> typeList;
};

struct NodeBlockTextBefore : NodeTerm {
};

struct NodeBlockTextAfter : NodeTerm {
};

struct NodeBlockWithTextAfter : NodeContainer {
	AstValue<NodeBlock> block;
	AstValue<NodeBlockTextAfter> blockTextAfter;
};

struct NodeBlock : NodeContainer {
	AstValue<NodeBlockTextBefore> blockTextBefore;
	AstList<NodeBlockWithTextAfter> blockWithTextAfter;
};

struct NodeModifier : NodeTerm {
};

struct NodeVariableModifier : NodeContainer {
	AstValue<NodeAnnotation, true> annotation;
};

struct NodeVariableDeclaratorId : NodeContainer {
	AstValue<NodeIdentifier> identifier;
	AstList<NodeTypeArray> typeArray;
};

struct NodeFormalParameter : NodeContainer {
	AstList<NodeVariableModifier> variableModifier;
	AstValue<NodeType> type;
	AstValue<NodeVariableDeclaratorId> variableDeclaratorId;
};

struct NodeLastFormalParameter : NodeContainer {
	AstList<NodeVariableModifier> variableModifier;
	AstValue<NodeType> type;
	AstValue<NodeVariableDeclaratorId> variableDeclaratorId;
};

struct NodeFormalParameterList : NodeContainer {
	AstList<NodeFormalParameter> formalParameter;
	AstValue<NodeLastFormalParameter, true> lastFormalParameter;
};

struct NodeFormalParameters : NodeContainer {
	AstValue<NodeFormalParameterList, true> formalParameterList;
};

struct NodeQualifiedNameList : NodeContainer {
	AstList<NodeQualifiedName> qualifiedName;
};

struct NodeMethodBody : NodeContainer {
	AstValue<NodeBlock, true> block;
};

struct NodeMethodDeclaration : NodeContainer {
	AstValue<NodeType, true> type;
	AstValue<NodeIdentifier> identifier;
	AstValue<NodeFormalParameters> formalParameters;
	AstValue<NodeQualifiedNameList, true> qualifiedNameList;
	AstValue<NodeMethodBody> methodBody;
};

struct NodeGenericMethodDeclaration : NodeContainer {
	AstValue<NodeTypeParameters> typeParameters;
	AstValue<NodeMethodDeclaration> methodDeclaration;
};

struct NodeArrayInitializer : NodeContainer {
	AstList<NodeVariableInitializer> variableInitializer;
};

struct NodeVariableInitializer : NodeContainer {
	AstValue<NodeArrayInitializer, true> arrayInitializer;
	AstValue<NodeExpression, true> expression;
};

struct NodeVariableDeclarator : NodeContainer {
	AstValue<NodeVariableDeclaratorId> variableDeclaratorId;
	AstValue<NodeVariableInitializer, true> variableInitializer;
};

struct NodeVariableDeclarators : NodeContainer {
	AstList<NodeVariableDeclarator> variableDeclarator;
};

struct NodeFieldDeclaration : NodeContainer {
	AstValue<NodeType> type;
	AstValue<NodeVariableDeclarators> variableDeclarators;
};

struct NodeConstructorBody : NodeContainer {
	AstValue<NodeBlock> block;
};

struct NodeConstructorDeclaration : NodeContainer {
	AstValue<NodeIdentifier> identifier;
	AstValue<NodeFormalParameters> formalParameters;
	AstValue<NodeQualifiedNameList, true> qualifiedNameList;
	AstValue<NodeConstructorBody> constructorBody;
};

struct NodeGenericConstructorDeclaration : NodeContainer {
	AstValue<NodeTypeParameters> typeParameters;
	AstValue<NodeConstructorDeclaration> constructorDeclaration;
};

struct NodeConstantDeclarator : NodeContainer {
	AstValue<NodeIdentifier> identifier;
	AstList<NodeTypeArray> typeArray;
	AstValue<NodeVariableInitializer> variableInitializer;
};

struct NodeConstDeclaration : NodeContainer {
	AstValue<NodeType> type;
	AstList<NodeConstantDeclarator> constantDeclarator;
};

struct NodeInterfaceMethodDeclaration : NodeContainer {
	AstValue<NodeType, true> type;
	AstValue<NodeIdentifier> identifier;
	AstValue<NodeFormalParameters> formalParameters;
	AstValue<NodeQualifiedNameList, true> qualifiedNameList;
};

struct NodeGenericInterfaceMethodDeclaration : NodeContainer {
	AstValue<NodeTypeParameters> typeParameters;
	AstValue<NodeInterfaceMethodDeclaration> interfaceMethodDeclaration;
};

struct NodeInterfaceBodyInner : NodeContainer {
	AstValue<NodeBlock> block;
};

struct NodeInterfaceDeclarationInner : NodeContainer {
	AstValue<NodeIdentifier> identifier;
	AstValue<NodeTypeParameters, true> typeParameters;
	AstValue<NodeTypeList, true> typeList;
	AstValue<NodeInterfaceBodyInner> interfaceBodyInner;
};

struct NodeAnnotationTypeBody : NodeContainer {
	AstValue<NodeBlock> block;
};

struct NodeAnnotationTypeDeclarationInner : NodeContainer {
	AstValue<NodeIdentifier> identifier;
	AstValue<NodeAnnotationTypeBody> annotationTypeBody;
};

struct NodeClassBodyInner : NodeContainer {
	AstValue<NodeBlock> block;
};

struct NodeClassDeclarationInner : NodeContainer {
	AstValue<NodeIdentifier> identifier;
	AstValue<NodeTypeParameters, true> typeParameters;
	AstValue<NodeClassDeclarationExtend, true> classDeclarationExtend;
	AstValue<NodeClassDeclarationImplement, true> classDeclarationImplement;
	AstValue<NodeClassBodyInner> classBodyInner;
};

struct NodeEnumConstantsAndBodyDeclarationsInner : NodeContainer {
	AstValue<NodeBlock> block;
};

struct NodeEnumDeclarationInner : NodeContainer {
	AstValue<NodeIdentifier> identifier;
	AstValue<NodeTypeList, true> typeList;
	AstValue<NodeEnumConstantsAndBodyDeclarationsInner> enumConstantsAndBodyDeclarationsInner;
};

struct NodeMemberDeclaration : NodeContainer {
	AstList<NodeAnnotation> annotation;
	AstList<NodeModifier> modifier;
	AstValue<NodeMethodDeclaration, true> methodDeclaration;
	AstValue<NodeGenericMethodDeclaration, true> genericMethodDeclaration;
	AstValue<NodeFieldDeclaration, true> fieldDeclaration;
	AstValue<NodeConstructorDeclaration, true> constructorDeclaration;
	AstValue<NodeGenericConstructorDeclaration, true> genericConstructorDeclaration;
	AstValue<NodeInterfaceDeclarationInner, true> interfaceDeclarationInner;
	AstValue<NodeAnnotationTypeDeclarationInner, true> annotationTypeDeclarationInner;
	AstValue<NodeClassDeclarationInner, true> classDeclarationInner;
	AstValue<NodeEnumDeclarationInner, true> enumDeclarationInner;
};

struct NodeClassBodyDeclaration : NodeContainer {
	AstValue<NodeBlock, true> block;
	AstValue<NodeMemberDeclaration, true> memberDeclaration;
};

struct NodeClassBody : NodeContainer {
	AstList<NodeClassBodyDeclaration> classBodyDeclaration;
};

struct NodeClassDeclaration : NodeContainer {
	AstValue<NodeIdentifier> identifier;
	AstValue<NodeTypeParameters, true> typeParameters;
	AstValue<NodeClassDeclarationExtend, true> classDeclarationExtend;
	AstValue<NodeClassDeclarationImplement, true> classDeclarationImplement;
	AstValue<NodeClassBody> classBody;
};

struct NodeArguments : NodeContainer {
	AstValue<NodeExpressionList, true> expressionList;
};

struct NodeEnumConstant : NodeContainer {
	//AstList<NodeAnnotation> annotation;
	AstValue<NodeIdentifier> identifier;
	AstValue<NodeArguments, true> arguments;
	AstValue<NodeClassBody, true> classBody;
};

struct NodeEnumConstants : NodeContainer {
	AstList<NodeEnumConstant> enumConstant;
};

struct NodeEnumBodyDeclarations : NodeContainer {
	AstList<NodeClassBodyDeclaration> classBodyDeclarations;
};

struct NodeEnumDeclaration : NodeContainer {
	AstValue<NodeIdentifier> identifier;
	AstValue<NodeTypeList, true> typeList;
	AstValue<NodeEnumConstants, true> enumConstants;
	AstValue<NodeEnumBodyDeclarations, true> enumBodyDeclarations;
};

struct NodeInterfaceMemberDeclaration : NodeContainer {
	AstList<NodeAnnotation> annotation;
	AstList<NodeModifier> modifier;
	AstValue<NodeConstDeclaration, true> constDeclaration;
	AstValue<NodeInterfaceMethodDeclaration, true> interfaceMethodDeclaration;
	AstValue<NodeGenericInterfaceMethodDeclaration, true> genericInterfaceMethodDeclaration;
	AstValue<NodeInterfaceDeclarationInner, true> interfaceDeclarationInner;
	AstValue<NodeAnnotationTypeDeclarationInner, true> annotationTypeDeclarationInner;
	AstValue<NodeClassDeclarationInner, true> classDeclarationInner;
	AstValue<NodeEnumDeclarationInner, true> enumDeclarationInner;
};

struct NodeInterfaceBodyDeclaration : NodeContainer {
	AstValue<NodeInterfaceMemberDeclaration, true> interfaceMemberDeclaration;
};

struct NodeInterfaceBody : NodeContainer {
	AstList<NodeInterfaceBodyDeclaration> interfaceBodyDeclaration;
};

struct NodeInterfaceDeclaration : NodeContainer {
	AstValue<NodeIdentifier> identifier;
	AstValue<NodeTypeParameters, true> typeParameters;
	AstValue<NodeTypeList, true> typeList;
	AstValue<NodeInterfaceBody> interfaceBody;
};

struct NodeAnnotationTypeDeclaration : NodeContainer {
	AstValue<NodeIdentifier> identifier;
	AstValue<NodeAnnotationTypeBody> annotationTypeBody;
};

struct NodeTypeDeclaration : NodeContainer {
	AstList<NodeClassOrInterfaceModifier> classOrInterfaceModifier;
	AstValue<NodeClassDeclaration, true> classDeclaration;
	AstValue<NodeEnumDeclaration, true> enumDeclaration;
	AstValue<NodeInterfaceDeclaration, true> interfaceDeclaration;
	AstValue<NodeAnnotationTypeDeclaration, true> annotationTypeDeclaration;
};

struct SourceFileCompilationUnit : NodeContainer {
	AstList<NodePackageDeclaration> packageDeclaration;
	AstList<NodeImportDeclaration> importDeclaration;
	AstList<NodeTypeDeclaration> typeDeclaration;
};
}
}
