#ifndef GITTYPE_H
#define GITTYPE_H

#include "Repository/Git/GitLib/GitExport.h"

namespace Git {

enum GitType {
	ADDED,
	REMOVED,
	MODIFIED,
	NONE
}; // enum
} // namespace
#endif // GITTYPE_H
