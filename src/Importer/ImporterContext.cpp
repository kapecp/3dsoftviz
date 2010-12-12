#include "Importer/ImporterContext.h"

namespace Importer {

ImporterContext::ImporterContext (
	std::iostream &stream,
	Data::Graph &graph,
	ImportInfoHandler &infoHandler
) : stream_ (stream),
	graph_ (graph),
	infoHandler_ (infoHandler)
{
}

std::iostream &ImporterContext::getStream (void) {
	return stream_;
}

Data::Graph &ImporterContext::getGraph (void) {
	return graph_;
}

ImportInfoHandler &ImporterContext::getInfoHandler (void) {
	return infoHandler_;
}

} // namespace
