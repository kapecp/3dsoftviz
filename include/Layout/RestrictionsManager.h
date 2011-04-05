#ifndef Layout_RestrictionsManager_H
#define Layout_RestrictionsManager_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeGetter.h"
#include "Layout/ShapeVisitor_RestrictedPositionGetter.h"
#include "Layout/ShapeVisitor_Comparator.h"
#include "Layout/RestrictionsObserver.h"
//-----------------------------------------------------------------------------
#include <QMap>
#include <QSet>
#include <QSharedPointer>
//-----------------------------------------------------------------------------

namespace Data {
	class Node;
}

namespace Layout {

class RestrictionsManager {

public:

	void setRestrictions (
		QSet<Data::Node *> nodes,
		QSharedPointer<ShapeGetter> shapeGetter
	);

	osg::Vec3f applyRestriction (
		Data::Node &node,
		osg::Vec3f originalPosition
	);

	void setObserver (
		QSharedPointer<RestrictionsObserver> observer
	);

	void resetObserver ();

	/***/
	virtual ~RestrictionsManager (void) {};

private:

	QSharedPointer<ShapeGetter> getShapeGetter (
		Data::Node &node
	);

	void compareAndNotifyShapeChanged (
		QSharedPointer<ShapeGetter> shapeGetter,
		QSharedPointer<Shape> shape
	);

private: // observer notification

	void notifyRestrictionAdded (
		QSharedPointer<ShapeGetter> shapeGetter
	);

	void notifyShapeChanged (
		QSharedPointer<ShapeGetter> shapeGetter,
		QSharedPointer<Shape> shape
	);

	void notifyRestrictionRemoved (
		QSharedPointer<ShapeGetter> shapeGetter
	);

private:

	typedef QMap<Data::Node *, QSharedPointer<ShapeGetter> > RestrictionsMapType;
	typedef QMap<QSharedPointer<ShapeGetter>, long> ShapeGetterUsagesMapType;
	typedef QMap<QSharedPointer<ShapeGetter>, QSharedPointer<Shape> > LastShapesMapType;

	RestrictionsMapType restrictions_;
	ShapeGetterUsagesMapType shapeGetterUsages_;
	LastShapesMapType lastShapes_;

	ShapeVisitor_RestrictedPositionGetter restrictedPositionGetter_;
	ShapeVisitor_Comparator shapeComparator_;

	QSharedPointer<RestrictionsObserver> observer_;

}; // class

} // namespace

#endif // Layout_RestrictionsManager_H
