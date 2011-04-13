#include "Layout/RestrictionsManager.h"
//-----------------------------------------------------------------------------
#include "Data/Node.h"
//-----------------------------------------------------------------------------

namespace Layout {

void RestrictionsManager::setRestrictions (
	QSet<Data::Node *> nodes,
	QSharedPointer<ShapeGetter> shapeGetter
) {
	for (QSet<Data::Node *>::iterator it = nodes.begin (); it != nodes.end (); ++it) {
		// remove current shape getter if exists
		RestrictionsMapType::iterator it2 = restrictions_.find (*it);
		if (it2 != restrictions_.end ()) {
			shapeGetterUsages_[it2.value ()]--;
			if (shapeGetterUsages_[it2.value ()] == 0) {
				// last shape getter usage removed

				notifyRestrictionRemoved (it2.value ());

				shapeGetterUsages_.remove (it2.value ());
				lastShapes_.remove(it2.value ());

				RemovalHandlersMapType::iterator removalHandlerIt = removalHandlers_.find (it2.value ());
				if (! removalHandlerIt->isNull ()) {
					(*removalHandlerIt)->afterRestrictionRemoved ();
				}
				removalHandlers_.erase (removalHandlerIt);
			}
			restrictions_.remove (*it);
		}

		if (
			(! shapeGetter.isNull ())
			&&
			((*it)->getPositionCanBeRestricted ())
		) {
			// add shape getter
			restrictions_[*it] = shapeGetter;
			shapeGetterUsages_[shapeGetter]++;
			if (shapeGetterUsages_[shapeGetter] == 1) {
				// first shape getter usage added

				lastShapes_[shapeGetter];
				removalHandlers_[shapeGetter];

				notifyRestrictionAdded (shapeGetter);
				refreshShape (shapeGetter);
			}
		}
	}
}

osg::Vec3f RestrictionsManager::applyRestriction (
	Data::Node &node,
	osg::Vec3f originalPosition
) {
	QSharedPointer<ShapeGetter> shapeGetter = getShapeGetter (node);
	if (!shapeGetter.isNull ()) {
		refreshShape (shapeGetter);

		QSharedPointer<Shape> shape = lastShapes_[shapeGetter];
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

// TODO: maybe create setOrRunRestrictionRemovalHandler method
void RestrictionsManager::setRestrictionRemovalHandler (
	QSharedPointer<ShapeGetter> shapeGetter,
	QSharedPointer<RestrictionRemovalHandler> handler
) {
	removalHandlers_[shapeGetter] = handler;
}

bool RestrictionsManager::isRestrictionUsed (
	QSharedPointer<ShapeGetter> shapeGetter
) {
	return (shapeGetterUsages_.find (shapeGetter) != shapeGetterUsages_.end ());
}

void RestrictionsManager::setObserver (
	QSharedPointer<RestrictionsObserver> observer
) {
	observer_ = observer;

	// send notifications reflecting the current state to the new observer
	for (LastShapesMapType::iterator it = lastShapes_.begin (); it != lastShapes_.end (); ++it) {
		notifyRestrictionAdded (it.key ());
		notifyShapeChanged (it.key (), it.value ());
	}
}

void RestrictionsManager::resetObserver () {
	observer_.clear();
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

void RestrictionsManager::refreshShape (
	QSharedPointer<ShapeGetter> shapeGetter
) {
	QSharedPointer<Shape> shape = shapeGetter->getShape ();

	if (shape.isNull ()) {
		// handle special case, because we can not visit null shape
		// maybe use null object pattern

		if (!lastShapes_[shapeGetter].isNull ()) {
			lastShapes_[shapeGetter] = shape;
			notifyShapeChanged (shapeGetter, shape);
		} else {
			return;
		}
	} else {
		shapeComparator_.setOtherShape (lastShapes_[shapeGetter]);
		shape->accept (shapeComparator_);
		if (!shapeComparator_.getComparisonResult ()) {
			lastShapes_[shapeGetter] = shape;
			notifyShapeChanged (shapeGetter, shape);
		}
	}
}

void RestrictionsManager::notifyRestrictionAdded (
	QSharedPointer<ShapeGetter> shapeGetter
) {
	if (!observer_.isNull ()) {
		observer_->restrictionAdded (shapeGetter);
	}
}

void RestrictionsManager::notifyShapeChanged (
	QSharedPointer<ShapeGetter> shapeGetter,
	QSharedPointer<Shape> shape
) {
	if (!observer_.isNull ()) {
		observer_->shapeChanged (shapeGetter, shape);
	}
}

void RestrictionsManager::notifyRestrictionRemoved (
	QSharedPointer<ShapeGetter> shapeGetter
) {
	if (!observer_.isNull ()) {
		observer_->restrictionRemoved (shapeGetter);
	}
}

} // namespace
