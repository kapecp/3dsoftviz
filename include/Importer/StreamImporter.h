#ifndef Importer_StreamImporter_H
#define Importer_StreamImporter_H
//-----------------------------------------------------------------------------
#include "Importer/ImporterContext.h"
//-----------------------------------------------------------------------------

namespace Importer {
class StreamImporter {

public:

	virtual ~StreamImporter (void) {};

	virtual bool import (
		ImporterContext &context
	) = 0;

}; // class
} // namespace

#endif // Importer_StreamImporter_H
