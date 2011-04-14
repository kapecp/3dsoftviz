#ifndef Layout_Shape_Sphere_H
#define Layout_Shape_Sphere_H
//-----------------------------------------------------------------------------
#include "Layout/Shape_AbstractSphere.h"
//-----------------------------------------------------------------------------
#include <osg/Vec3f>
//-----------------------------------------------------------------------------

namespace Layout {

class Shape_Sphere : public Shape_AbstractSphere {

public:

	enum RestrictionPolicy {
		RANDOM_DISTANCE_FROM_CENTER,
		SURFACE
	};

	Shape_Sphere (
		const osg::Vec3f center,
		const float radius,
		RestrictionPolicy restrictionPolicy
	);

	/***/
	virtual ~Shape_Sphere (void) {};

	virtual void accept (
		ShapeVisitor & visitor
	);

	RestrictionPolicy getRestrictionPolicy ();

private:

	RestrictionPolicy restrictionPolicy_;

}; // class

} // namespace

#endif // Layout_Shape_Sphere_H
