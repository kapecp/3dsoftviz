#ifndef Layout_Shape_H
#define Layout_Shape_H
//-----------------------------------------------------------------------------

namespace Layout {

class ShapeVisitor;

/**
 * Base class of all shapes.
 */
class Shape {

public:

	/***/
	virtual ~Shape (void) {};

	/**
	 * [visitor pattern]
	 */
	virtual void accept (
		ShapeVisitor & visitor
	) = 0;

}; // class

} // namespace

#endif // Layout_Shape_H
