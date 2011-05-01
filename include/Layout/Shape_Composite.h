#ifndef Layout_Shape_Composite_H
#define Layout_Shape_Composite_H
//-----------------------------------------------------------------------------
#include "Layout/Shape.h"
//-----------------------------------------------------------------------------
#include <list>
#include <QSharedPointer>
//-----------------------------------------------------------------------------

namespace Layout {

/**
 * Shape which can be composed using more shapes of various types.
 */
class Shape_Composite : public Shape {

public:

	typedef std::list<QSharedPointer<Shape> > ShapesListType;

	/***/
	virtual ~Shape_Composite (void) {};

	void addShape (
		QSharedPointer<Shape> shape
	);

	ShapesListType & getShapes (void);

	/**
	 * [visitor pattern]
	 */
	virtual void accept (
		ShapeVisitor & visitor
	);

private:

	ShapesListType shapes_;

}; // class

} // namespace

#endif // Layout_Shape_Composite_H
