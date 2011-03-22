#include "Layout/RestrictionsManager.h"

namespace Layout {

void RestrictionsManager::setRestrictions (
	QSet<Data::Node *> nodes,
	QSharedPointer<ShapeGetter> shapeGetter
) {
	for (QSet<Data::Node *>::iterator it = nodes.begin (); it != nodes.end (); ++it) {
		if (shapeGetter.isNull()) {
			restrictions_.remove (*it);
		} else {
			restrictions_[*it] = shapeGetter;
		}
	}
}

osg::Vec3f RestrictionsManager::applyRestriction (
	Data::Node &node,
	osg::Vec3f originalPosition
) {
	QSharedPointer<ShapeGetter> shapeGetter = getShapeGetter (node);
	if (!shapeGetter.isNull ()) {
		QSharedPointer<Shape> shape = shapeGetter->getShape ();
		if (!shape.isNull ()) {
			restrictedPositionGetter_.setOriginalPosition (originalPosition);
			shape->accept (restrictedPositionGetter_);
			return restrictedPositionGetter_.getRestrictedPosition ();
		} else {
			return originalPosition;
		}
	} else {
		return originalPosition;
	}
}

QSharedPointer<ShapeGetter> RestrictionsManager::getShapeGetter (
	Data::Node &node
) {
	RestrictionsMapType::iterator it = restrictions_.find (&node);
	if (it != restrictions_.end ()) {
		return it.value();
	} else {
		return QSharedPointer<ShapeGetter> ();
	}
}

} // namespace
