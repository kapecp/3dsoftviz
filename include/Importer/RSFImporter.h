#ifndef Importer_RSFImporter_H
#define Importer_RSFImporter_H
//-----------------------------------------------------------------------------
#include "Importer/StreamImporter.h"
//-----------------------------------------------------------------------------

namespace Importer {

/**
 * \brief Imports graph data from data in RSF format.
 */
class RSFImporter
	: public StreamImporter {

public:

	/***/
	virtual ~RSFImporter (void) {};

	/***/
	virtual bool import (
		ImporterContext &context
	);

}; // class

} // namespace

#endif // Importer_RSFImporter_H
