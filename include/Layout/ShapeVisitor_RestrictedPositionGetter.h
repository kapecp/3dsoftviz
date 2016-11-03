#ifndef LAYOUT_SHAPEVISITOR_RESTRICTEDPOSITIONGETTER_H
#define LAYOUT_SHAPEVISITOR_RESTRICTEDPOSITIONGETTER_H

#include "Layout/ShapeVisitor.h"

#include <osg/Vec3f>
//-----------------------------------------------------------------------------

namespace Layout {

/**
 * \brief Can compute restricted position (a position conforming the
 * restriction).
 * [visitor pattern]
 * Usage:
 * 1. Call setOriginalPosition to set the original position (the restricted
 * position will be computed using this position - e.g. restricting using some
 * shape definitions will result in finding of the nearest point to the
 * original point which conform the restriction).
 * 2. Accept the visitor by the restriction shape (get from shape getter).
 * 3. Call restrictedPositionConstRef to get the result.
 */
class ShapeVisitor_RestrictedPositionGetter : public ShapeVisitor
{
public:
	virtual ~ShapeVisitor_RestrictedPositionGetter() {}

	void setOriginalPosition( const osg::Vec3f& position );
	const osg::Vec3f& restrictedPositionConstRef();

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
		 * \brief Finds the restricted position using intersection of the shapes contained in the intersection shape.
		 */
	virtual void visit( Shape_Intersection& shape );

	/**
	 * \brief Finds the nearest point on the plane.
	 */
	virtual void visit( Shape_Plane& shape );

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

private: // private methods

	/**
	 * \brief Returns a point in the sphere which has the random distance from
	 * center constrained by radiusMin (min. distance from the center) and
	 * radiusMax (max. distance from the center). Set
	 * radiusMin=radiusMax=radius to get the nearest point on the sphere
	 * surface.
	 */
	osg::Vec3f toSphere( const osg::Vec3f& center, float radiusMin, float radiusMax, const osg::Vec3f& point );

	osg::Vec3f toCube( const osg::Vec3f& center, const osg::Vec3f& surfaceX, const osg::Vec3f& surfaceY, const osg::Vec3f& surfaceZ, const osg::Vec3f& point );

	bool insideCube( const osg::Vec3f& center, const osg::Vec3f& surfaceX, const osg::Vec3f& surfaceY, const osg::Vec3f& surfaceZ, const osg::Vec3f& point );

private: // visitor context
	osg::Vec3f mOriginalPosition;
	osg::Vec3f mRestrictedPosition;
};

float median( float A, float B, float C );

}

#endif // LAYOUT_SHAPEVISITOR_RESTRICTEDPOSITIONGETTER_H
