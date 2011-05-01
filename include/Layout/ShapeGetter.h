#ifndef Layout_ShapeGetter_H
#define Layout_ShapeGetter_H
//-----------------------------------------------------------------------------
#include "Layout/Shape.h"
//-----------------------------------------------------------------------------
#include <QSharedPointer>
//-----------------------------------------------------------------------------

namespace Layout {

/**
 * \brief Implementations return current restriction shape.
 * [interface]
 * The purpose was to create the possibility to define dynamic shapes - the implementation
 * can return different shape after each call of getShape.
 */
class ShapeGetter {

public:

	virtual QSharedPointer<Shape> getShape (void) = 0;

	/***/
	virtual ~ShapeGetter (void) {};

}; // class

} // namespace

#endif // Layout_ShapeGetter_H
