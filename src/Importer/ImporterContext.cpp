#include "Importer/ImporterContext.h"

namespace Importer {

ImporterContext::ImporterContext (
	QIODevice &stream,
	Data::Graph &graph,
	ImportInfoHandler &infoHandler
) : stream_ (stream),
	graph_ (graph),
	infoHandler_ (infoHandler)
{
}

QIODevice &ImporterContext::getStream (void) {
	return stream_;
}

Data::Graph &ImporterContext::getGraph (void) {
	return graph_;
}

ImportInfoHandler &ImporterContext::getInfoHandler (void) {
	return infoHandler_;
}

} // namespace
