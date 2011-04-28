#ifndef Layout_ShapeVisitor_RestrictedPositionGetter_H
#define Layout_ShapeVisitor_RestrictedPositionGetter_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeVisitor.h"
//-----------------------------------------------------------------------------
#include <osg/Vec3f>
//-----------------------------------------------------------------------------

namespace Layout {

/**
 * \brief Can compute restricted position (a position conforming the restriction).
 * [visitor pattern]
 * Usage:
 * 1. Call setOriginalPosition to set the original position (the restricted position will be computed
 * using this position - e.g. restricting using some shape definitions will result in finding of the nearest
 * point to the original point which conform the restriction).
 * 2. Call visit with the restriction shape (get from shape getter) as an argument.
 * 3. Call getRestrictedPosition to get the result.
 */
class ShapeVisitor_RestrictedPositionGetter : public ShapeVisitor {

public:

	/***/
	virtual ~ShapeVisitor_RestrictedPositionGetter (void) {};

	void setOriginalPosition (osg::Vec3f originalPosition);
	osg::Vec3f getRestrictedPosition (void);

	/**
	 * \brief Does not change the position.
	 */
	virtual void visit (Shape_Null & shape);

	/**
	 * \brief Finds the restricted position using each of the shapes contained in the composite shape
	 * and uses the nearest restricted position to the original position.
	 */
	virtual void visit (Shape_Composite & shape);

	/**
	 * \brief Finds the nearest point on the plane.
	 */
	virtual void visit (Shape_Plane & shape);

	/**
	 * \brief Finds the nearest point on the sphere (point on the sphere surface - if SURFACE restriction
	 * policy is defined by the shape) or a point with a random distance from the center (if RANDOM_DISTANCE_FROM_CENTER
	 * policy is used). Does not change the point if it already conforms the restriction.
	 */
	virtual void visit (Shape_Sphere & shape);

	/**
	 * \brief Finds the nearest point on the sphere surface.
	 */
	virtual void visit (Shape_SphereSurface & shape);

private: // private methods

	/**
	 * \brief Returns a point in the sphere which has the random distance from center constrained by radiusMin (min.
	 * distance from the center) and radiusMax (max. distance from the center).
	 * Set radiusMin=radiusMax=radius to get the nearest point on the sphere surface.
	 */
	osg::Vec3f toSphere (
		const osg::Vec3f center,
		const float radiusMin,
		const float radiusMax,
		const osg::Vec3f &point
	);

private: // visitor context

	osg::Vec3f originalPosition_;
	osg::Vec3f restrictedPosition_;

}; // class

} // namespace

#endif // Layout_ShapeVisitor_RestrictedPositionGetter_H
