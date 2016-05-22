#pragma once

#include "JavaAst.h"
#include "../SoftTree.h"

namespace Importer {
namespace Parsing {
class JavaParser
{
public:
	JavaParser();
	bool Parse( const QString& javaProjectDirectory, SoftTree& softTree, QString& errorMessage = QString() );
};
}
}
