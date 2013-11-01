#include "Layout/ShapeGetter_Sphere_ByTwoNodes.h"
//-----------------------------------------------------------------------------
#include "Layout/Shape_Sphere.h"
#include "Data/Node.h"
//-----------------------------------------------------------------------------

namespace Layout {

ShapeGetter_Sphere_ByTwoNodes::ShapeGetter_Sphere_ByTwoNodes (
	osg::ref_ptr<Data::Node> centerNode,
	osg::ref_ptr<Data::Node> surfaceNode
) : centerNode_ (centerNode),
	surfaceNode_ (surfaceNode)
{
	// nothing
}

QSharedPointer<Shape> ShapeGetter_Sphere_ByTwoNodes::getShape (void) {
	return QSharedPointer<Shape> (
		new Shape_Sphere (
			centerNode_->targetPosition (),
			(centerNode_->targetPosition () - surfaceNode_->targetPosition ()).length (),
			Shape_Sphere::RANDOM_DISTANCE_FROM_CENTER
		)
	);
}

 QSet<Data::Node * > ShapeGetter_Sphere_ByTwoNodes::getNodesOfShape(){
    QSet<Data::Node * > nodes;
    nodes.insert (centerNode_.get());
    nodes.insert (surfaceNode_.get());
    return nodes;
}

} // namespace
