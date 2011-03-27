#include "Importer/GraphOperations.h"

namespace Importer {

GraphOperations::GraphOperations (
	Data::Graph &graph
) : graph_ (graph)
{
}

bool GraphOperations::addDefaultTypes (
	Data::Type *&edgeType,
	Data::Type *&nodeType
) {
	bool ok = true;

	if (ok) {
		edgeType = graph_.addType ("edge");

		ok = (NULL != edgeType);
	}

	if (ok) {
		nodeType = graph_.addType ("node");

		ok = (NULL != nodeType);
	}

	return ok;
}

} // namespace
