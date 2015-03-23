#ifndef Importer_StreamImporter_H
#define Importer_StreamImporter_H
//-----------------------------------------------------------------------------
#include "Importer/ImporterContext.h"
//-----------------------------------------------------------------------------

namespace Importer {

/**
 * \brief Imports graph data from stream.
 * [interface]
 * Implementations of this interface perform import of graph data from streams.
 * Each implementation should handle one stream data format.
 */
class StreamImporter
{

public:

	/***/
	virtual ~StreamImporter( void ) {}

	/**
	 * \brief Performs import.
	 * \param[in,out] context Provides input (stream) and output (graph, import handler) objects for the method.
	 * \return ( = success)
	 * Operates over importer context.
	 * Reads from stream specified in the context.
	 * Modifies graph data in the context.
	 * Uses info handler in the context to inform about the progress.
	 */
	virtual bool import(
		ImporterContext& context
	) = 0;

}; // class

} // namespace

#endif // Importer_StreamImporter_H
