#include "Layout/ShapeGetter_Plane_ByThreeNodes.h"
//-----------------------------------------------------------------------------
#include "Layout/Shape_Plane.h"
//-----------------------------------------------------------------------------

namespace Layout {

ShapeGetter_Plane_ByThreeNodes::ShapeGetter_Plane_ByThreeNodes (
	osg::ref_ptr<Data::Node> node1,
	osg::ref_ptr<Data::Node> node2,
	osg::ref_ptr<Data::Node> node3
) : node1_ (node1),
	node2_ (node2),
	node3_ (node3)
{
	// nothing
}

QSharedPointer<Shape> ShapeGetter_Plane_ByThreeNodes::getShape (void) {
	osg::Vec3f pointA = node1_->getTargetPosition ();
	osg::Vec3f pointB = node2_->getTargetPosition ();
	osg::Vec3f pointC = node3_->getTargetPosition ();

	osg::Vec3f n = (pointB - pointA) ^ (pointC - pointA);
	float d = n.x () * pointA.x () + n.y () * pointA.y () + n.z () * pointA.z ();

	return QSharedPointer<Shape> (
		new Shape_Plane (
			n,
			d
		)
	);
}

} // namespace
