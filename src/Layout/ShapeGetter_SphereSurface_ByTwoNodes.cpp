#include "Layout/ShapeGetter_SphereSurface_ByTwoNodes.h"
//-----------------------------------------------------------------------------
#include "Layout/Shape_SphereSurface.h"
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
			centerNode_->getTargetPosition (),
			(centerNode_->getTargetPosition () - surfaceNode_->getTargetPosition ()).length ()
		)
	);
}

} // namespace
