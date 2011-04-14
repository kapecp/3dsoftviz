#ifndef Layout_RestrictionsObserver_H
#define Layout_RestrictionsObserver_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeGetter.h"
#include "Layout/Shape.h"
//-----------------------------------------------------------------------------
#include <QSharedPointer>
//-----------------------------------------------------------------------------

namespace Layout {

class RestrictionsObserver {

public:

	/***/
	virtual ~RestrictionsObserver (void) {};

public:

	virtual void restrictionAdded (
		QSharedPointer<ShapeGetter> shapeGetter
	) = 0;

	virtual void shapeChanged (
		QSharedPointer<ShapeGetter> shapeGetter,
		QSharedPointer<Shape> shape
	) = 0;

	virtual void restrictionRemoved (
		QSharedPointer<ShapeGetter> shapeGetter
	) = 0;

}; // class

} // namespace

#endif // Layout_RestrictionsObserver_H
