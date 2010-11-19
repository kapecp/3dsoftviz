#ifndef Importer_GraphMLImporter_H
#define Importer_GraphMLImporter_H
//-----------------------------------------------------------------------------
#include "Importer/StreamImporter.h"
//-----------------------------------------------------------------------------

namespace Importer {

/**
 * \brief Imports graph data from data in GraphML format.
 */
class GraphMLImporter
	: public StreamImporter {

public:

	/***/
	virtual ~GraphMLImporter (void) {};

	/***/
	virtual bool import (
		ImporterContext &context
	);

}; // class

} // namespace

#endif // Importer_GraphMLImporter_H
