#ifndef Importer_GXLImporter_H
#define Importer_GXLImporter_H
//-----------------------------------------------------------------------------
#include "Importer/StreamImporter.h"
//-----------------------------------------------------------------------------

namespace Importer {

/**
 * \brief Imports graph data from data in GXL format.
 */
class GXLImporter
	: public StreamImporter {

public:

	/***/
	virtual ~GXLImporter (void) {};

	/***/
	virtual bool import (
		ImporterContext &context
	);

}; // class

} // namespace

#endif // Importer_GXLImporter_H
