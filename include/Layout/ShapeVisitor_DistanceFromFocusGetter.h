#ifndef LAYOUT_SHAPEVISITOR_DISTANCEFROMFOCUSGETTER_H
#define LAYOUT_SHAPEVISITOR_DISTANCEFROMFOCUSGETTER_H

#include "Layout/ShapeVisitor.h"

#include <osg/Vec3f>

namespace Layout {

/**
 * \brief Can compute distance of a position from focus.
 * [visitor pattern]
 * Usage:
 * 1. Call setPosition to set the position (the distance from focus will be
 * computed using this position).
 * 2. Accept the visitor by the shape (get from shape getter).
 * 3. Call distanceFromFocus to get the result.
 */
class ShapeVisitor_DistanceFromFocusGetter : public ShapeVisitor
{
public:
//    ShapeVisitor_DistanceFromFocusGetter( osg::Vec3f mPosition, float mDistanceFromFocus ) {
//        this->mPosition = mPosition;
//        this->mDistanceFromFocus = mDistanceFromFocus;
//    }

	virtual ~ShapeVisitor_DistanceFromFocusGetter() {}

	void setPosition( const osg::Vec3f& position );
	float distanceFromFocus();

	/**
	 * \brief Does not change the position.
	 */

	virtual void visit( Shape_Null /*&shape*/ );

	/**
	 * \brief Finds the restricted position using each of the shapes contained
	 * in the composite shape and uses the nearest restricted position to the
	 * original position.
	 */
	virtual void visit( Shape_Composite& shape );

	/**
	 * \brief Finds the nearest point on the plane.
	 */
	virtual void visit( Shape_Plane& shape );



	virtual void visit( Shape_Intersection& shape );
	/**
	 * \brief Finds the nearest point on the sphere (point on the sphere
	 * surface - if SURFACE restriction policy is defined by the shape) or a
	 * point with a random distance from the center (if
	 * RANDOM_DISTANCE_FROM_CENTER policy is used). Does not change the point
	 * if it already conforms the restriction.
	 */
	virtual void visit( Shape_Sphere& shape );

	/**
	 * \brief Finds the nearest point on the sphere surface.
	 */
	virtual void visit( Shape_SphereSurface& shape );

	/**
	 * \brief Finds the nearest point on the cylinder surface.
	 */
	virtual void visit( Shape_CylinderSurface& shape );

	/**
	 * \brief Finds the nearest point on the cone surface.
	 */
	virtual void visit( Shape_ConeSurface& shape );

	virtual void visit( Shape_Cube& shape );

private: // visitor context
	osg::Vec3f mPosition;
	float mDistanceFromFocus;
};

}

#endif // LAYOUT_SHAPEVISITOR_DISTANCEFROMFOCUSGETTER_H
