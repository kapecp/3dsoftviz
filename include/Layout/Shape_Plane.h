#ifndef Layout_Shape_Plane_H
#define Layout_Shape_Plane_H
//-----------------------------------------------------------------------------
#include "Layout/Shape.h"
//-----------------------------------------------------------------------------
#include <osg/Vec3f>
//-----------------------------------------------------------------------------

namespace Layout {

class Shape_Plane : public Shape {

public:

	// normal vector = [a, b, c]
	// ax + by + cz = d
	Shape_Plane (
		const osg::Vec3f normalVector,
		const float d
	);

	/***/
	virtual ~Shape_Plane (void) {};

	osg::Vec3f & getNormalVector (void);

	float getD (void);

	virtual void accept (
		ShapeVisitor & visitor
	);

private:

	osg::Vec3f normalVector_;
	float d_;

}; // class

} // namespace

#endif // Layout_Shape_AbstractSphere_H
