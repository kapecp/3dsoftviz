#ifndef Layout_Shape_Null_H
#define Layout_Shape_Null_H
//-----------------------------------------------------------------------------
#include "Layout/Shape.h"
//-----------------------------------------------------------------------------

namespace Layout {

class Shape_Null : public Shape {

public:

	Shape_Null (void);

	/***/
	virtual ~Shape_Null (void) {};

	virtual void accept (
		ShapeVisitor & visitor
	);

}; // class

} // namespace

#endif // Layout_Shape_Null_H
