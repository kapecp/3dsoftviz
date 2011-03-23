#include "Layout/ShapeGetter_Sphere_AroundNode.h"
//-----------------------------------------------------------------------------
#include "Layout/Shape_Sphere.h"
//-----------------------------------------------------------------------------

namespace Layout {

ShapeGetter_Sphere_AroundNode::ShapeGetter_Sphere_AroundNode (
	osg::ref_ptr<Data::Node> node,
	float radius
) : node_ (node),
	radius_ (radius)
{
	// nothing
}

QSharedPointer<Shape> ShapeGetter_Sphere_AroundNode::getShape (void) {
	return QSharedPointer<Shape> (
		new Shape_Sphere (
			node_->getCurrentPosition (false),
			radius_
		)
	);
}

} // namespace
