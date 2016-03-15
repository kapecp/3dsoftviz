#ifndef GITFUNCTIONTYPE_H
#define GITFUNCTIONTYPE_H

#include "Repository/Git/GitLib/GitExport.h"

namespace Repository {

namespace Git {

#if defined(_MSC_VER)
enum GitFunctionType {
#else
enum class GitFunctionType {
#endif
    GLOBALFUNCTION,
    LOCALFUNCTION
}; // enum
} // namespace
}
#endif // GITFUNCTIONTYPE_H
