#include "Layout/ShapeGetter_SphereSurface_ByTwoNodes.h"
//-----------------------------------------------------------------------------
#include "Layout/Shape_SphereSurface.h"
#include "Data/Node.h"
//-----------------------------------------------------------------------------

namespace Layout {

ShapeGetter_SphereSurface_ByTwoNodes::ShapeGetter_SphereSurface_ByTwoNodes (
	osg::ref_ptr<Data::Node> centerNode,
	osg::ref_ptr<Data::Node> surfaceNode
) : centerNode_ (centerNode),
	surfaceNode_ (surfaceNode)
{
	// nothing
}

QSharedPointer<Shape> ShapeGetter_SphereSurface_ByTwoNodes::getShape (void) {
	return QSharedPointer<Shape> (
		new Shape_SphereSurface (
			centerNode_->targetPosition (),
			(centerNode_->targetPosition () - surfaceNode_->targetPosition ()).length ()
		)
	);
}

QSet<Data::Node * > ShapeGetter_SphereSurface_ByTwoNodes::getNodesOfShape(){
   QSet<Data::Node *  > nodes;
    nodes.insert (centerNode_.get());
    nodes.insert(surfaceNode_.get());
    return nodes;
}

} // namespace
