#pragma once

#include "SoftTree.h"
#include <QVector>
#include <QMap>

namespace Importer {
namespace Parsing {
typedef unsigned int uint;

struct GetterSetterDirectAttrAccess {
	int callingMethodIndex;
	int accessedAttributeIndex;
};

struct MethodInvocation {
	int callingMethodIndex;
	QList<int> calledMethodsIndexes;
};

struct InvocationGraph {
	QList<GetterSetterDirectAttrAccess> gettersSetters;
	QList<MethodInvocation> internalMethods;
	QList<MethodInvocation> interfaceMethods;
	QList<MethodInvocation> constructors;

	static InvocationGraph AnalyzeClass( const Class& class1 )
	{
		static const QString RETURN_SPACE_STR( "return " );
		static const QString THIS_DOT_STR( "this." );

		InvocationGraph res;

		QMap<QString, GetterSetterDirectAttrAccess> gettersSettersMap;
		QMap<QString, MethodInvocation> internalMethodsMap;
		QMap<QString, MethodInvocation> interfaceMethodsMap;
		QMap<QString, MethodInvocation> constructorsMap;
		QMap<QString, int> allAttributesName;
		QMap<QString, int> allMethodsName;

		for ( int i = 0; i < class1.attributes.count(); ++i ) {
			allAttributesName.insert( class1.attributes[i].name, i );
		}

		for ( int i = 0; i < class1.methods.count(); ++i ) {
			allMethodsName.insert( class1.methods[i].name, i );
		}

		QStringList methodList;
		for ( const auto& method : class1.methods ) {
			int callingMethodIndex = allMethodsName.value( method.name );
			// cekni ci je to getter setter - nazov get/set, vnutri return atribut - spoj ho z atributom s ktorym pracuje
			bool getter = method.name.startsWith( "get", Qt::CaseInsensitive );
			bool setter = method.name.startsWith( "set", Qt::CaseInsensitive );
			if ( getter || setter ) {
				QRegExp re;
				if ( getter ) {
					re.setPattern( "return\\s+(this\\s*.\\s*)?\\w+\\s*;" );
				}
				else { /*setter*/
					re.setPattern( "(this\\s*.\\s*)?\\w+\\s*=\\s*\\w+\\s*;" );
				}
				int pos = 0;
				int accessedAttributeIndex = -1;
				while ( ( pos = re.indexIn( method.content, pos ) ) != -1 && accessedAttributeIndex == -1 ) {
					auto returnCaptured = re.cap();
					QString returnName;
					if ( getter ) {
						returnCaptured = returnCaptured.mid( returnCaptured.indexOf( RETURN_SPACE_STR ) + RETURN_SPACE_STR.length() );
						returnCaptured = returnCaptured.remove( ' ' );
						if ( returnCaptured.contains( THIS_DOT_STR ) ) {
							returnCaptured = returnCaptured.mid( returnCaptured.indexOf( THIS_DOT_STR ) + THIS_DOT_STR.length() );
						}
						returnName = returnCaptured.left( returnCaptured.length() - 1 );
					}
					else { // setter
						returnCaptured = returnCaptured.remove( ' ' );
						if ( returnCaptured.contains( THIS_DOT_STR ) ) {
							returnCaptured = returnCaptured.mid( returnCaptured.indexOf( THIS_DOT_STR ) + THIS_DOT_STR.length() );
						}
						returnName = returnCaptured.left( returnCaptured.indexOf( '=' ) );
					}
					accessedAttributeIndex = allAttributesName.value( returnName, -1 );
					pos += re.matchedLength();
				}
				gettersSettersMap.insert( method.name, { callingMethodIndex, accessedAttributeIndex } );
			}
			else if ( method.IsConstructor() ) {
				constructorsMap.insert( method.name, { callingMethodIndex, QList<int>() } );
			}
			else if ( method.modifier == Modifier::PUBLIC ) {
				interfaceMethodsMap.insert( method.name, { callingMethodIndex, QList<int>() } );
			}
			else { // protected, private
				internalMethodsMap.insert( method.name, { callingMethodIndex, QList<int>() } );
			}

			// cekni ci je to constructor, public, private/protected metoda a podla toho zarad
			// cekni volania metod, get/setterov, atributov a podla toho napln asociacie - cekuj relativne k zoznamom znamych metod v mapach
		}

		res.gettersSetters = gettersSettersMap.values();
		res.internalMethods = internalMethodsMap.values();
		res.interfaceMethods = interfaceMethodsMap.values();
		res.constructors = constructorsMap.values();

		return res;
	}
};
}
}
