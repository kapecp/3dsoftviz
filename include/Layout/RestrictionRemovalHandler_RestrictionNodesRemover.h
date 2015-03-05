#ifndef Layout_RestrictionRemovalHandler_RestrictionNodesRemover_H
#define Layout_RestrictionRemovalHandler_RestrictionNodesRemover_H
//-----------------------------------------------------------------------------
#include "Data/Node.h"
#include "Data/Graph.h"
#include "Layout/RestrictionRemovalHandler.h"
//-----------------------------------------------------------------------------
#include <QList>
#include <osg/ref_ptr>
//-----------------------------------------------------------------------------

namespace Layout {

/**
 * \brief Removes nodes from a graph when notified. Nodes and graph are specified in constructor parameters.
 * Used to remove nodes which are used to change the restriction (e.g. by defining the sphere center and radius)
 * which has been added by the user using GUI.
 */
class RestrictionRemovalHandler_RestrictionNodesRemover : public RestrictionRemovalHandler
{

public:

	typedef QList<osg::ref_ptr<Data::Node> > NodesListType;

	/***/
	RestrictionRemovalHandler_RestrictionNodesRemover(
		Data::Graph& graph,
		NodesListType nodesToRemove
	);

	RestrictionRemovalHandler_RestrictionNodesRemover(
		Data::Graph& graph,
		NodesListType nodesToRemove,
		NodesListType ignoredNodes
	);

	/***/
	virtual void afterRestrictionRemoved( void );

	/***/
	virtual ~RestrictionRemovalHandler_RestrictionNodesRemover( void ) {}

private:

	Data::Graph& graph_;
	NodesListType nodesToRemove_;
	NodesListType ignoredNodes_;

}; // class

} // namespace

#endif // Layout_RestrictionRemovalHandler_RestrictionNodesRemover_H
