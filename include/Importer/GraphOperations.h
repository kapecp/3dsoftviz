#ifndef Importer_GraphOperations_H
#define Importer_GraphOperations_H
//-----------------------------------------------------------------------------
#include "Data/Graph.h"
//-----------------------------------------------------------------------------

namespace Importer {

/**
 * \brief Provides operations performed on the graph object used by the importers.
 */
class GraphOperations {

public:

	GraphOperations (
		Data::Graph &graph
	);

	/***/
	virtual ~GraphOperations (void) {};

	bool addDefaultTypes (
		Data::Type *&edgeType,
		Data::Type *&nodeType
	);

private:

	Data::Graph &graph_;

}; // class

} // namespace

#endif // Importer_GraphOperations_H
