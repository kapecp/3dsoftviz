#ifndef Layout_RestrictionsManager_H
#define Layout_RestrictionsManager_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeGetter.h"
#include "Layout/ShapeVisitor_RestrictedPositionGetter.h"
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

	/***/
	virtual ~RestrictionsManager (void) {};

private:

	typedef QMap<Data::Node *, QSharedPointer<ShapeGetter> > RestrictionsMapType;

	RestrictionsMapType restrictions_;

	QSharedPointer<ShapeGetter> getShapeGetter (
		Data::Node &node
	);

	ShapeVisitor_RestrictedPositionGetter restrictedPositionGetter_;

}; // class

} // namespace

#endif // Layout_RestrictionsManager_H
