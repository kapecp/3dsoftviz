#pragma once

#include "ParserlibBase.h"

namespace Importer {
namespace Parsing {
using parserlib::set;
using parserlib::nl;
using parserlib::expr;
using parserlib::rule;
using parserlib::term;
using parserlib::range;
using parserlib::any;

#pragma region externs

extern rule expressionBracket;
extern rule expressionRound;
extern rule expression;
extern rule typeArguments;
extern rule block;
extern rule variableInitializer;

#pragma endregion

#pragma region defines

#define JAVA_LETTER (range('a', 'z') | range('A', 'Z') | set("$_"))
#define JAVA_DIGIT range('0', '9')
#define JAVA_LETTER_OR_DIGIT (JAVA_LETTER | JAVA_DIGIT)
#define NEWLINE nl(expr("\r\n") | "\n\r" | '\n' | '\r')
#define WHITECHAR (' ' | set("\t\v\f"))
#define ESCAPE_AND_COMMENT_CHAR (expr("\\t") | "\\b" | "\\n" | "\\r" | "\\f" | "\\'" | "\\\"" | "\\\\" | "//" | "/*" | "*/")
#define ANY_CHAR any()
#define EVERYTHING_TO(endChar) term(*(!(endChar) >> (NEWLINE | ANY_CHAR)))
#define CHAR_EXPR (expr("'") >> (ESCAPE_AND_COMMENT_CHAR | ANY_CHAR) >> "'")
#define STRING_EXPR (expr('"') >> *(ESCAPE_AND_COMMENT_CHAR | !expr('"') >> (NEWLINE | ANY_CHAR)) >> '"')
#define STRING term(CHAR_EXPR | STRING_EXPR)
#define LINE_COMMENT (expr("//") >> EVERYTHING_TO(NEWLINE) >> NEWLINE)
#define COMMENT (expr("/*") >> EVERYTHING_TO(expr("*/")) >> "*/")
#define EVERYTHING_VALID_TO(endChar) term(*(STRING | LINE_COMMENT | COMMENT | !(endChar) >> (NEWLINE | ANY_CHAR)))

#define CLASS_OR_INTERFACE_MODIFIER expr("public") | "protected" | "private" | "static" | "abstract" | "final" | "strictfp"
#define MODIFIER CLASS_OR_INTERFACE_MODIFIER | expr("native") | "synchronized" | "transient" | "volatile"

#pragma endregion

/* !!! grammar support only limited Java features !!! */

/*
LIMITATIONS -->
*/

// test expression
// { textBefore { text2Before { { text3aInner } text3After } text2continue1 { text3Before { text3bInner } } text2continue2 {text3cInner} text2After } textAfter }

rule whitespace = *( WHITECHAR | NEWLINE | LINE_COMMENT | COMMENT );
rule expressionBracketBefore = EVERYTHING_TO( set( "{}" ) );
rule expressionBracketAfter = EVERYTHING_TO( set( "{}" ) );
rule expressionBracketWithTextAfter = expressionBracket >> expressionBracketAfter;
rule expressionBracket = '{' >> expressionBracketBefore >> *expressionBracketWithTextAfter >> '}';
rule expressionRoundBefore = EVERYTHING_TO( set( "()" ) );
rule expressionRoundAfter = EVERYTHING_TO( set( "()" ) );
rule expressionRoundWithTextAfter = expressionRound >> expressionRoundAfter;
rule expressionRound = '(' >> expressionRoundBefore >> *expressionRoundWithTextAfter >> ')';
rule expressionString = STRING >> expression;
rule expressionPostfix = -expressionString >> -( expressionRound >> -( '.' >> expression ) ) >> -( expressionBracket | EVERYTHING_TO( expr( ";" ) ) );
rule expression = EVERYTHING_TO( set( "();{}\"" ) ) >> expressionPostfix;
rule expressionList = expression >> *( ',' >> expression );
rule identifier = term( JAVA_LETTER >> *JAVA_LETTER_OR_DIGIT );
rule qualifiedName = identifier >> *( '.' >> identifier );
rule annotationIdentifier = term( !( MODIFIER | "interface" ) >> JAVA_LETTER >> *JAVA_LETTER_OR_DIGIT );
rule annotationName = annotationIdentifier >> *( '.' >> annotationIdentifier );
rule annotationSequenceBefore = EVERYTHING_TO( set( "()" ) );
rule annotationSequenceAfter = EVERYTHING_TO( set( "()" ) );
rule annotationSequence = '(' >> annotationSequenceBefore >> -annotationSequence >> annotationSequenceAfter >> ')';
rule annotation = expr( '@' ) >> annotationName >> -annotationSequence;
rule packageDeclaration = /**annotation >>*/ "package" >> qualifiedName >> ';';
rule importDeclaration = expr( "import" ) >> -expr( "static" ) >> qualifiedName >> -expr( ".*" ) >> ';';
rule classOrInterfaceModifierBasic = CLASS_OR_INTERFACE_MODIFIER;
rule classOrInterfaceModifier = classOrInterfaceModifierBasic | annotation;
rule typeArgumentsBefore = EVERYTHING_TO( set( "<>" ) );
rule typeArgumentsAfter = EVERYTHING_TO( set( "<>" ) );
rule typeArgumentsWithTextAfter = typeArguments >> typeArgumentsAfter;
rule typeArguments = '<' >> typeArgumentsBefore >> *typeArgumentsWithTextAfter >> '>';
rule classOrInterfaceTypePart = identifier >> -typeArguments;
rule classOrInterfaceType = classOrInterfaceTypePart >> *( '.' >> classOrInterfaceTypePart );
rule primitiveType = expr( "boolean" ) | "char" | "byte" | "short" | "int" | "long" | "float" | "double";
rule typeArray = expr( '[' ) >> ']';
rule type = ( classOrInterfaceType | primitiveType ) >> *typeArray;
rule typeBound = type >> *( '&' >> type );
rule typeParameter = identifier >> -( "extends" >> typeBound );
rule typeParameters = "<" >> typeParameter >> *( ',' >> typeParameter ) >> ">";
rule classDeclarationExtend = "extends" >> type;
rule typeList = type >> *( ',' >> type );
rule classDeclarationImplement = "implements" >> typeList;
rule blockTextBefore = EVERYTHING_VALID_TO( set( "{}" ) );
rule blockTextAfter = EVERYTHING_VALID_TO( set( "{}" ) );
rule blockWithTextAfter = block >> blockTextAfter;
rule block = term( '{' >> blockTextBefore ) >> *blockWithTextAfter >> '}';
rule modifier = MODIFIER;
rule variableModifier = "final" | annotation;
rule variableDeclaratorId = identifier >> *typeArray;
rule formalParameter = *variableModifier >> type >> variableDeclaratorId;
rule lastFormalParameter = *variableModifier >> type >> "..." >> variableDeclaratorId;
rule formalParameterList = ( formalParameter >> *( ',' >> formalParameter ) >> -( ',' >> lastFormalParameter ) ) | lastFormalParameter;
rule formalParameters = expr( '(' ) >> -formalParameterList >> ')';
rule qualifiedNameList = qualifiedName >> *( ',' >> qualifiedName );
rule methodBody = ';' | block;
rule methodDeclaration = ( type | "void" ) >> identifier >> formalParameters >> *( expr( '[' ) >> ']' ) >> -( "throws" >> qualifiedNameList ) >> methodBody;
rule genericMethodDeclaration = typeParameters >> methodDeclaration;
rule arrayInitializer = expr( '{' ) >> -( variableInitializer >> *( ',' >> variableInitializer ) >> -( ',' ) ) >> '}';
rule variableInitializer = arrayInitializer | expression;
rule variableDeclarator = variableDeclaratorId >> -( '=' >> variableInitializer );
rule variableDeclarators = variableDeclarator >> *( ',' >> variableDeclarator );
rule fieldDeclaration = type >> variableDeclarators >> ';';
rule constructorBody = block;
rule constructorDeclaration = identifier >> formalParameters >> -( "throws" >> qualifiedNameList ) >> constructorBody;
rule genericConstructorDeclaration = typeParameters >> constructorDeclaration;
rule constantDeclarator = identifier >> *typeArray >> '=' >> variableInitializer;
rule constDeclaration = type >> constantDeclarator >> *( ',' >> constantDeclarator ) >> ';';
rule interfaceMethodDeclaration = ( type | "void" ) >> identifier >> formalParameters >> *( expr( '[' ) >> ']' ) >> -( "throws" >> qualifiedNameList ) >> ';';
rule genericInterfaceMethodDeclaration = typeParameters >> interfaceMethodDeclaration;
rule interfaceBodyInner = block;
rule interfaceDeclarationInner = "interface" >> identifier >> -typeParameters >> -( "extends" >> typeList ) >> interfaceBodyInner;
rule annotationTypeBody = block;
rule annotationTypeDeclarationInner = expr( '@' ) >> "interface" >> identifier >> annotationTypeBody;
rule classBodyInner = block;
rule classDeclarationInner = "class" >> identifier >> -typeParameters >> -classDeclarationExtend >> -classDeclarationImplement >> classBodyInner;
rule enumConstantsAndBodyDeclarationsInner = block;
rule enumDeclarationInner = expr( "enum" ) >> identifier >> -( "implements" >> typeList ) >> enumConstantsAndBodyDeclarationsInner;
rule memberDeclaration = *annotation >> *modifier >> ( methodDeclaration | genericMethodDeclaration | fieldDeclaration | constructorDeclaration | genericConstructorDeclaration | interfaceDeclarationInner | annotationTypeDeclarationInner | classDeclarationInner | enumDeclarationInner );
rule classBodyDeclaration = expr( ';' ) | ( -expr( "static" ) >> block ) | memberDeclaration;
rule classBody = expr( '{' ) >> *classBodyDeclaration >> '}';
rule classDeclaration = "class" >> identifier >> -typeParameters >> -classDeclarationExtend >> -classDeclarationImplement >> classBody;
rule arguments = expr( '(' ) >> -expressionList >> ')';
rule enumConstant = /**annotation >>*/ identifier >> -arguments >> -classBody;
rule enumConstants = enumConstant >> *( ',' >> enumConstant );
rule enumBodyDeclarations = ';' >> *classBodyDeclaration;
rule enumDeclaration = expr( "enum" ) >> identifier >> -( "implements" >> typeList ) >> '{' >> -enumConstants >> -expr( ',' ) >> -enumBodyDeclarations >> '}';
rule interfaceMemberDeclaration = *annotation >> *modifier >> ( constDeclaration | interfaceMethodDeclaration | genericInterfaceMethodDeclaration | interfaceDeclarationInner | annotationTypeDeclarationInner | classDeclarationInner | enumDeclarationInner );
rule interfaceBodyDeclaration = interfaceMemberDeclaration | ';';
rule interfaceBody = expr( '{' ) >> *interfaceBodyDeclaration >> '}';
rule interfaceDeclaration = "interface" >> identifier >> -typeParameters >> -( "extends" >> typeList ) >> interfaceBody;
rule annotationTypeDeclaration = expr( '@' ) >> "interface" >> identifier >> annotationTypeBody;
rule typeDeclaration = *classOrInterfaceModifier >> ( classDeclaration | enumDeclaration | interfaceDeclaration | annotationTypeDeclaration ) | ';';
rule compilationUnit = -packageDeclaration >> *importDeclaration >> *typeDeclaration;
}
}
