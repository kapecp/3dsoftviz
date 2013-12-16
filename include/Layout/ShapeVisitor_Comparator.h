#ifndef Layout_ShapeVisitor_Comparator_H
#define Layout_ShapeVisitor_Comparator_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeVisitor.h"
//-----------------------------------------------------------------------------
#include <QSharedPointer>
//-----------------------------------------------------------------------------

namespace Layout {

/**
 * \brief Can compare two shapes.
 * [visitor pattern]
 * Usage:
 * 1. Call setOtherShape to set first shape to compare.
 * 2. Accept the visitor by the second shape to compare.
 * 3. Call getComparison result to get TRUE if the shapes have the same type and properties.
 */
class ShapeVisitor_Comparator : public ShapeVisitor {

public:

	/***/
	virtual ~ShapeVisitor_Comparator (void) {};

	void setOtherShape (QSharedPointer<Shape> otherShape);
	bool getComparisonResult (void);

	virtual void visit(Shape_Null & shape);
	virtual void visit(Shape_Composite & shape);
	virtual void visit (Shape_Intersection & shape);
	virtual void visit(Shape_Plane & shape);
	virtual void visit(Shape_Sphere & shape);
	virtual void visit(Shape_SphereSurface & shape);
	virtual void visit(Shape_CylinderSurface &shape);
	virtual void visit(Shape_ConeSurface &shape);

private:

	void compareSpheres (Shape_AbstractSphere & shape, Shape_AbstractSphere & otherShape);
	void compareCylinders(Shape_CylinderSurface &shape, Shape_CylinderSurface &otherShape);
	void compareCones(Shape_ConeSurface &shape, Shape_ConeSurface &otherShape);

private: // visitor context

	QSharedPointer<Shape> otherShape_;
	bool comparisonResult_;

}; // class

} // namespace

#endif // Layout_ShapeVisitor_Comparator_H
