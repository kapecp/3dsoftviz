#ifndef Layout_ShapeGetter_Sphere_AroundNode_H
#define Layout_ShapeGetter_Sphere_AroundNode_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeGetter.h"
//-----------------------------------------------------------------------------
#include "Data/Node.h"
//-----------------------------------------------------------------------------

namespace Layout {

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

private:

	osg::ref_ptr<Data::Node> node_;
	float radius_;

	Shape_Sphere::RestrictionPolicy restrictionPolicy_;
	SphereCenterSource centerSource_;

}; // class

} // namespace

#endif // Layout_ShapeGetter_Sphere_AroundNode_H
