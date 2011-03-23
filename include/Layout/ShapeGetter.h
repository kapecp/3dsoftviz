#ifndef Layout_ShapeGetter_H
#define Layout_ShapeGetter_H
//-----------------------------------------------------------------------------
#include "Layout/Shape.h"
//-----------------------------------------------------------------------------
#include <QSharedPointer>
//-----------------------------------------------------------------------------

namespace Layout {

class ShapeGetter {

public:

	virtual QSharedPointer<Shape> getShape (void) = 0;

	/***/
	virtual ~ShapeGetter (void) {};

}; // class

} // namespace

#endif // Layout_ShapeGetter_H
