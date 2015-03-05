#ifndef Layout_Shape_SphereSurface_H
#define Layout_Shape_SphereSurface_H
//-----------------------------------------------------------------------------
#include "Layout/Shape_AbstractSphere.h"
//-----------------------------------------------------------------------------
#include <osg/Vec3f>
//-----------------------------------------------------------------------------

namespace Layout {

/**
 * [immutable]
 */
class Shape_SphereSurface : public Shape_AbstractSphere
{

public:

	Shape_SphereSurface(
		const osg::Vec3f center,
		const float radius
	);

	/***/
	virtual ~Shape_SphereSurface( void ) {}

	/**
	 * [visitor pattern]
	 */
	virtual void accept(
		ShapeVisitor& visitor
	);

}; // class

} // namespace

#endif // Layout_Shape_SphereSurface_H
