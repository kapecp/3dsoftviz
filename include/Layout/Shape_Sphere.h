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

	Shape_Sphere (
		const osg::Vec3f center_,
		const float radius_
	);

	/***/
	virtual ~Shape_Sphere (void) {};

	virtual void accept (
		ShapeVisitor & visitor
	);

}; // class

} // namespace

#endif // Layout_Shape_Sphere_H
