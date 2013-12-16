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
	setInvisible(false);
}


QSharedPointer<Shape> ShapeGetter_Plane_ByThreeNodes::getShape (void) {
	osg::Vec3f pointA = node1_->targetPosition ();
	osg::Vec3f pointB = node2_->targetPosition ();
	osg::Vec3f pointC = node3_->targetPosition ();

	osg::Vec3f n = (pointB - pointA) ^ (pointC - pointA);
	float d = - (n.x () * pointA.x () + n.y () * pointA.y () + n.z () * pointA.z ());

	QSharedPointer<Shape>shape = QSharedPointer<Shape> (new Shape_Plane (n,d));
	if (isInvisible())
		shape->setInvisible(true);
	else shape->setInvisible(false);

	return shape;
}

QSet<Data::Node * > ShapeGetter_Plane_ByThreeNodes::getNodesOfShape(){
	QSet<Data::Node * > nodes;
	nodes.insert (node1_.get());
	nodes.insert(node2_.get());
	nodes.insert(node3_.get());
	return nodes;
}

} // namespace
