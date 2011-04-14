#ifndef Layout_ShapeVisitor_Comparator_H
#define Layout_ShapeVisitor_Comparator_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeVisitor.h"
//-----------------------------------------------------------------------------
#include <QSharedPointer>
//-----------------------------------------------------------------------------

namespace Layout {

class ShapeVisitor_Comparator : public ShapeVisitor {

public:

	/***/
	virtual ~ShapeVisitor_Comparator (void) {};

	void setOtherShape (QSharedPointer<Shape> otherShape);
	bool getComparisonResult (void);

	virtual void visit (Shape_Composite & shape);
	virtual void visit (Shape_Plane & shape);
	virtual void visit (Shape_Sphere & shape);
	virtual void visit (Shape_SphereSurface & shape);

private:

	void compareSpheres (Shape_AbstractSphere & shape, Shape_AbstractSphere & otherShape);

private: // visitor context

	QSharedPointer<Shape> otherShape_;
	bool comparisonResult_;

}; // class

} // namespace

#endif // Layout_ShapeVisitor_Comparator_H
