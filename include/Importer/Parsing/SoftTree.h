#pragma once

#include <QVector>
#include <QString>

namespace Importer {
namespace Parsing {
class Modifier
{
private:
	Modifier();

public:
	enum ModifierEnum { UNKNOWN, PUBLIC, PROTECTED, PRIVATE };
	static QString ToString( ModifierEnum modifier );
};

struct Attribute {
	Modifier::ModifierEnum modifier;
	QString type;
	QString name;

	QString ToString( int tabs ) const;
};

struct Parameter {
	QString type;
	QString name;

	QString ToString( int tabs ) const;
};

struct Method {
	Modifier::ModifierEnum modifier;
	QString returnType;
	QString name;
	QList<Parameter> parameters;
	QString content;

	QString ToString( int tabs ) const;
	bool IsConstructor() const;
	bool IsInterface() const;
	bool HasResult() const;
	uint GetLineOfCodes() const;
};

struct Class {
	Modifier::ModifierEnum modifier;
	QString name;
	QList<Attribute> attributes;
	QList<Method> methods;

	Class();
	QString ToString( int tabs ) const;
};

struct EnumConstant {
	QString name;

	QString ToString( int tabs ) const;
};

struct Enum {
	Modifier::ModifierEnum modifier;
	QString name;
	QList<EnumConstant> enums;
	QList<Attribute> attributes;
	QList<Method> methods;

	Enum();
	QString ToString( int tabs ) const;
};

struct Interface {
	Modifier::ModifierEnum modifier;
	QString name;
	QList<Attribute> attributes;
	QList<Method> methods;

	Interface();
	QString ToString( int tabs ) const;
};

struct Namespace {
	QString name;
	QList<Class> classes;
	QList<Enum> enums;
	QList<Interface> interfaces;

	QString ToString( int tabs ) const;
};

struct SoftTree {
	QList<Namespace> namespaces;

	QString ToString() const;
};
}
}
