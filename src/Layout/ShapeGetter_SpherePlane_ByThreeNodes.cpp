#include "Layout/ShapeGetter_SpherePlane_ByThreeNodes.h"
//-----------------------------------------------------------------------------

namespace Layout {

ShapeGetter_SpherePlane_ByThreeNodes::ShapeGetter_SpherePlane_ByThreeNodes(
	osg::ref_ptr<Data::Node> node1,
	osg::ref_ptr<Data::Node> node2,
	osg::ref_ptr<Data::Node> node3
) : node1_( node1 ),
	node2_( node2 ),
	node3_( node3 )
{
	// nothing
}

QSharedPointer<Shape> ShapeGetter_SpherePlane_ByThreeNodes::getShape( void )
{

	QSharedPointer<Layout::ShapeGetter> sphereSurfaceGetter =
		QSharedPointer<Layout::ShapeGetter> ( new ShapeGetter_SphereSurface_ByTwoNodes( node1_, node2_ ) );
	QSharedPointer<Layout::ShapeGetter> planeGetter =
		QSharedPointer<Layout::ShapeGetter>( new ShapeGetter_Plane_ByThreeNodes( node1_, node2_, node3_ ) );


	QSharedPointer<Shape_Composite> composite = QSharedPointer<Shape_Composite>( new Shape_Composite );
	composite->addShape( sphereSurfaceGetter->getShape() );
	composite->addShape( planeGetter->getShape() );

	return composite;
}

QSet<Data::Node* > ShapeGetter_SpherePlane_ByThreeNodes::getNodesOfShape()
{
	QSet<Data::Node*  >  nodes;
	nodes.insert( node1_.get() );
	nodes.insert( node2_.get() );
	nodes.insert( node3_.get() );
	return nodes;
}

} // namespace
