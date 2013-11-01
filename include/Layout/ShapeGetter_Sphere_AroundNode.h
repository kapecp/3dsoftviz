#ifndef Layout_ShapeGetter_Sphere_AroundNode_H
#define Layout_ShapeGetter_Sphere_AroundNode_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeGetter.h"
#include "Layout/Shape_Sphere.h"
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

namespace Layout {

/**
 * \brief Returning a sphere.
 * Center is defined by the center of the node provided in the constructor. The radius
 * and restriction policy are constant and specified in the constructor.
 * Center source specifies the way of getting the sphere center from the node center:
 * - NODE_CURRENT_POSITION - the current node position is captured (where the node is placed
 * in the scene and where it is currently visible)
 * - NODE_TARGET_POSITION - the latest position computed by the layout algorithm (the node
 * is moving to this position)
 */
class ShapeGetter_Sphere_AroundNode : public ShapeGetter {

public:

	enum SphereCenterSource {
		NODE_CURRENT_POSITION,
		NODE_TARGET_POSITION,
	};

	ShapeGetter_Sphere_AroundNode (
		osg::ref_ptr<Data::Node> node,
		float radius,
		Shape_Sphere::RestrictionPolicy restrictionPolicy,
		SphereCenterSource centerSource
	);

	/***/
	virtual ~ShapeGetter_Sphere_AroundNode (void) {};

	virtual QSharedPointer<Shape> getShape (void);

        virtual QSet<Data::Node *  > getNodesOfShape();

private:

	osg::ref_ptr<Data::Node> node_;
	float radius_;

	Shape_Sphere::RestrictionPolicy restrictionPolicy_;
	SphereCenterSource centerSource_;

}; // class

} // namespace

#endif // Layout_ShapeGetter_Sphere_AroundNode_H
