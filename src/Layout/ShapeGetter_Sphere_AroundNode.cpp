#include "Layout/ShapeGetter_Sphere_AroundNode.h"
//-----------------------------------------------------------------------------
#include "Layout/Shape_Sphere.h"
#include "Util/ApplicationConfig.h"
#include "Data/Node.h"
//-----------------------------------------------------------------------------
#include <osg/Vec3>
#include <qset.h>
//-----------------------------------------------------------------------------

namespace Layout {

ShapeGetter_Sphere_AroundNode::ShapeGetter_Sphere_AroundNode (
		osg::ref_ptr<Data::Node> node,
		float radius,
		Shape_Sphere::RestrictionPolicy restrictionPolicy,
		SphereCenterSource centerSource
		) : node_ (node),
	radius_ (radius),
	restrictionPolicy_ (restrictionPolicy),
	centerSource_ (centerSource)
{
	// nothing
}

QSharedPointer<Shape> ShapeGetter_Sphere_AroundNode::getShape (void) {
	osg::Vec3 center;

	switch (centerSource_) {
	case NODE_CURRENT_POSITION:
	{
		float graphScale = Util::ApplicationConfig::get()->getValue("Viewer.Display.NodeDistanceScale").toFloat();
		center = node_->getCurrentPosition () / graphScale;
	}
		break;
	case NODE_TARGET_POSITION:
		center = node_->targetPosition ();
		break;
	default:
		center = osg::Vec3 (0, 0, 0);
	}

	return QSharedPointer<Shape> (
				new Shape_Sphere (
					center,
					radius_,
					restrictionPolicy_
					)
				);
}

QSet<Data::Node *  > ShapeGetter_Sphere_AroundNode::getNodesOfShape(){
	QSet<Data::Node * >nodes;
	nodes.insert (node_.get());
	return nodes;
}

} // namespace
