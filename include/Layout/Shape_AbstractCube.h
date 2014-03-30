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
class Shape_AbstractCube : public Shape {

public:

    Shape_AbstractCube (
			const osg::Vec3f center_,
			const float radius_
			);

	osg::Vec3f & getCenter ();

	float getRadius ();

	/**
	 * [visitor pattern]
	 */
	virtual void accept (
			ShapeVisitor & visitor
			) = 0;

protected:

	/***/
    virtual ~Shape_AbstractCube (void) {}

private:

	osg::Vec3f center_;
	float radius_;

}; // class

} // namespace

#endif // Layout_Shape_AbstractCube_H
