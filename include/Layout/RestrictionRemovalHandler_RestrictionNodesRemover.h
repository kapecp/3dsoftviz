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

class RestrictionRemovalHandler_RestrictionNodesRemover : public RestrictionRemovalHandler {

public:

	typedef QList<osg::ref_ptr<Data::Node> > NodesListType;

	RestrictionRemovalHandler_RestrictionNodesRemover (
		Data::Graph & graph,
		NodesListType nodesToRemove
	);

	virtual void afterRestrictionRemoved (void);

	/***/
	virtual ~RestrictionRemovalHandler_RestrictionNodesRemover (void) {};

private:

	Data::Graph & graph_;
	NodesListType nodesToRemove_;

}; // class

} // namespace

#endif // Layout_RestrictionRemovalHandler_RestrictionNodesRemover_H
