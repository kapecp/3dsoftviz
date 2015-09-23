#ifndef Layout_Shape_AbstractCube_H
#define Layout_Shape_AbstractCube_H
//-----------------------------------------------------------------------------
#include "Layout/Shape.h"
//-----------------------------------------------------------------------------
#include <osg/Vec3f>
//-----------------------------------------------------------------------------

namespace Layout {

/**
 * \brief Base class for Sphere and SphereSurface (shapes which need to have
 * center and radius defined).
 * [immutable]
 */
class Shape_AbstractCube : public Shape
{

public:

	Shape_AbstractCube(
		const osg::Vec3f center_,
		const osg::Vec3f surfaceX_,
		const osg::Vec3f surfaceY_,
		const osg::Vec3f surfaceZ_
	);

	osg::Vec3f& getCenter();
	osg::Vec3f& getSurfaceX();
	osg::Vec3f& getSurfaceY();
	osg::Vec3f& getSurfaceZ();

	/**
	 * [visitor pattern]
	 */
	virtual void accept(
		ShapeVisitor& visitor
	) = 0;

protected:

	/***/
	virtual ~Shape_AbstractCube( void ) {}

private:

	osg::Vec3f center_;
	osg::Vec3f surfaceX_;
	osg::Vec3f surfaceY_;
	osg::Vec3f surfaceZ_;

}; // class

} // namespace

#endif // Layout_Shape_AbstractCube_H
