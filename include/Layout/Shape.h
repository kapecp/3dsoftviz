#ifndef Layout_Shape_H
#define Layout_Shape_H
//-----------------------------------------------------------------------------

namespace Layout {

class ShapeVisitor;

class Shape {

public:

	/***/
	virtual ~Shape (void) {};

	virtual void accept (
		ShapeVisitor & visitor
	) = 0;

}; // class

} // namespace

#endif // Layout_Shape_H
