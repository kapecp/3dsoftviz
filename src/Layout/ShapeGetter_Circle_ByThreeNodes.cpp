#include "Layout/ShapeGetter_Circle_ByThreeNodes.h"
//-----------------------------------------------------------------------------
#include "Layout/Shape_Plane.h"
//-----------------------------------------------------------------------------

namespace Layout {

ShapeGetter_Circle_ByThreeNodes::ShapeGetter_Circle_ByThreeNodes (
		osg::ref_ptr<Data::Node> node1,
		osg::ref_ptr<Data::Node> node2,
		osg::ref_ptr<Data::Node> node3
		) : node1_ (node1),
	node2_ (node2),
	node3_ (node3)
{
	// nothing
}

QSharedPointer<Shape> ShapeGetter_Circle_ByThreeNodes::getShape (void) {
	QSharedPointer<Layout::ShapeGetter> sphereSurfaceGetter =
			QSharedPointer<Layout::ShapeGetter> (new ShapeGetter_SphereSurface_ByTwoNodes (node1_, node2_));
	QSharedPointer<Layout::ShapeGetter> planeGetter =
			QSharedPointer<Layout::ShapeGetter>(new ShapeGetter_Plane_ByThreeNodes (node1_, node2_, node3_));


	QSharedPointer<Shape_Intersection> intersection = QSharedPointer<Shape_Intersection>(new Shape_Intersection);
	intersection->setCompositeType(Shape_Composite::CompositeType::CIRCLE);
	intersection->addShape(sphereSurfaceGetter->getShape());
	intersection->addShape(planeGetter->getShape());

	return intersection;

}

QSet<Data::Node *  >ShapeGetter_Circle_ByThreeNodes::getNodesOfShape(){
	QSet<Data::Node * > nodes;
	nodes.insert (node1_.get());
	nodes.insert(node2_.get());
	nodes.insert(node3_.get());
	return nodes;
}

} // namespace
