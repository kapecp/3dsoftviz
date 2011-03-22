#ifndef Layout_ShapeGetter_Const_H
#define Layout_ShapeGetter_Const_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeGetter.h"
#include "Layout/Shape.h"
//-----------------------------------------------------------------------------

namespace Layout {

class ShapeGetter_Const : public ShapeGetter {

public:

	ShapeGetter_Const (
		QSharedPointer<Shape> shape
	);

	/***/
	virtual ~ShapeGetter_Const (void) {};

	virtual QSharedPointer<Shape> getShape (void);

private:

	QSharedPointer<Shape> shape_;

}; // class

} // namespace

#endif // Layout_ShapeGetter_Const_H
