#include "Layout/RestrictionRemovalHandler_RestrictionNodesRemover.h"
//-----------------------------------------------------------------------------

namespace Layout {

RestrictionRemovalHandler_RestrictionNodesRemover::RestrictionRemovalHandler_RestrictionNodesRemover (
	Data::Graph & graph,
	NodesListType nodesToRemove
) : graph_ (graph),
    nodesToRemove_ (nodesToRemove)
{
}

void RestrictionRemovalHandler_RestrictionNodesRemover::afterRestrictionRemoved (void) {
	for (NodesListType::iterator it = nodesToRemove_.begin (); it != nodesToRemove_.end (); ++it) {
		graph_.removeNode (*it);
	}
}

} // namespace
