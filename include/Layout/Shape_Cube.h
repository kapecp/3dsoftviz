#ifndef Layout_Shape_Cube_H
#define Layout_Shape_Cube_H
//-----------------------------------------------------------------------------
#include "Layout/Shape_AbstractCube.h"
//-----------------------------------------------------------------------------
#include <osg/Vec3f>
//-----------------------------------------------------------------------------

namespace Layout {

/**
 * [immutable]
 * Restriction policy defines the way of computing restricted position,
 * if the original position does not conform the restriction defined by the
 * shape.
 * RANDOM_DISTANCE_FROM_CENTER - distance of the new position from the sphere center is random
 * SURFACE - the new position is a point on the sphere surface
 */
class Shape_Cube : public Shape_AbstractCube
{

public:

	enum RestrictionPolicy {
		RANDOM_DISTANCE_FROM_CENTER,
		SURFACE
	};

	Shape_Cube(
		const osg::Vec3f center,
		const osg::Vec3f surfaceX,
		const osg::Vec3f surfaceY,
		const osg::Vec3f surfaceZ,
		RestrictionPolicy restrictionPolicy
	);

	/***/
	virtual ~Shape_Cube( void ) {}

	/**
	 * [visitor pattern]
	 */
	virtual void accept(
		ShapeVisitor& visitor
	);

	RestrictionPolicy getRestrictionPolicy();

private:

	RestrictionPolicy restrictionPolicy_;

}; // class

} // namespace

#endif // Layout_Shape_Cube_H
