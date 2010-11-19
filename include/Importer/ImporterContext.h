#ifndef Importer_ImporterContext_H
#define Importer_ImporterContext_H
//-----------------------------------------------------------------------------
#include "Data/Graph.h"
#include "Importer/ImportInfoHandler.h"
//-----------------------------------------------------------------------------
#include <istream>
//-----------------------------------------------------------------------------

namespace Importer {
class ImporterContext {

public:

	ImporterContext (
		std::iostream &stream,
		Data::Graph &graph,
		ImportInfoHandler &importInfoHandler
	);

	virtual ~ImporterContext (void) {};

	std::iostream &getStream (void);

	Data::Graph &getGraph (void);

	ImportInfoHandler &getInfoHandler (void);

private:

	std::iostream &stream_;

	Data::Graph &graph_;

	ImportInfoHandler &infoHandler_;

}; // class
} // namespace

#endif // Importer_ImporterContext_H
