#ifndef Layout_Shape_Plane_H
#define Layout_Shape_Plane_H
//-----------------------------------------------------------------------------
#include "Layout/Shape.h"
//-----------------------------------------------------------------------------
#include <osg/Vec3f>
//-----------------------------------------------------------------------------

namespace Layout {

/**
 * [immutable]
 * Defined by:
 * - normal vector = [a, b, c]
 * - d
 *
 * ax + by + cz + d = 0
 */
class Shape_Plane : public Shape {

public:

	/***/
	Shape_Plane (
			const osg::Vec3f normalVector,
			const float d
			);

	/***/
	virtual ~Shape_Plane (void) {}

	osg::Vec3f & getNormalVector (void);

	float getD (void);

	/**
	 * [visitor pattern]
	 */
	virtual void accept (
			ShapeVisitor & visitor
			);

private:

	osg::Vec3f normalVector_;
	float d_;

}; // class

} // namespace

#endif // Layout_Shape_Plane_H
