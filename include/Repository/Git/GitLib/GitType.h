#ifndef GITTYPE_H
#define GITTYPE_H

#include "Repository/Git/GitLib/GitExport.h"

namespace Repository {

namespace Git {

#if defined(_MSC_VER)
enum GitType {
#else
enum class GitType {
#endif
	ADDED,
	REMOVED,
	MODIFIED,
	NONE
}; // enum
} // namespace
}
#endif // GITTYPE_H
