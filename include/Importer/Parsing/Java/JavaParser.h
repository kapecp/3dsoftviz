#pragma once

#include "JavaAst.h"
#include "../SoftTree.h"

namespace Importer {
namespace Parsing {
class JavaParser
{
public:
	JavaParser();
	bool Parse( const QString& javaProjectDirectory, SoftTree& softTree, const QString& errorMessage = QString() );
};
}
}
