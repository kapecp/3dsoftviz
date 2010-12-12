#ifndef Importer_ImporterContext_H
#define Importer_ImporterContext_H
//-----------------------------------------------------------------------------
#include "Data/Graph.h"
#include "Importer/ImportInfoHandler.h"
//-----------------------------------------------------------------------------
#include <istream>
//-----------------------------------------------------------------------------

namespace Importer {

/**
 * \brief Contains object for the importer to work with.
 * Data manipulated by the importer during the import process:
 * - input: input data (stream)
 * - output: graph root object (graph), progress info (info handler)
 */
class ImporterContext {

public:

	/**
	 * \brief Creates new context object containing provided references.
	 * \param[in] stream Input stream.
	 * \param[in] graph Graph root object.
	 * \param[in] importInfoHandler Import progress info handler.
	 */
	ImporterContext (
		std::iostream &stream,
		Data::Graph &graph,
		ImportInfoHandler &importInfoHandler
	);

	/***/
	virtual ~ImporterContext (void) {};

	/**
	 * \brief \link ImporterContext::stream_ [getter] \endlink
	 */
	std::iostream &getStream (void);

	/**
	 * \brief \link ImporterContext::graph_ [getter] \endlink
	 */
	Data::Graph &getGraph (void);

	/**
	 * \brief \link ImporterContext::infoHandler_ [getter] \endlink
	 */
	ImportInfoHandler &getInfoHandler (void);

private:

	/**
	 * \brief Stream to read data from.
	 */
	std::iostream &stream_;

	/**
	 * \brief Graph to fill according to data read from the stream.
	 */
	Data::Graph &graph_;

	/**
	 * \brief Used by the importer to report status of the import (progress, messages, errors).
	 */
	ImportInfoHandler &infoHandler_;

}; // class

} // namespace

#endif // Importer_ImporterContext_H
