#include "Layout/RestrictionsManager.h"
//-----------------------------------------------------------------------------
#include "Data/Node.h"
#include "Layout/Shape_Null.h"
//-----------------------------------------------------------------------------

namespace Layout {

void RestrictionsManager::setRestrictions (
	QSet<Data::Node *> nodes,
	QSharedPointer<ShapeGetter> shapeGetter
) {
	QList<QSharedPointer<RestrictionRemovalHandler> > removalHandlersToRun;

	mutex_.lock ();
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
					removalHandlersToRun.push_back (*removalHandlerIt);
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

				lastShapes_[shapeGetter] = QSharedPointer<Shape> (new Shape_Null);
				removalHandlers_[shapeGetter];

				notifyRestrictionAdded (shapeGetter);
				refreshShape (shapeGetter);
			}
		}
	}
	mutex_.unlock ();

	for (QList<QSharedPointer<RestrictionRemovalHandler> >::iterator it = removalHandlersToRun.begin (); it != removalHandlersToRun.end (); ++it) {
		(*it)->afterRestrictionRemoved ();
	}
}

osg::Vec3f RestrictionsManager::applyRestriction (
	Data::Node &node,
	osg::Vec3f originalPosition
) {
	osg::Vec3f position;

	mutex_.lock ();
	QSharedPointer<ShapeGetter> shapeGetter = getShapeGetter (node);
	
	//nastavenie obmedzovaca podla zvoleneho tvaru
	if (!shapeGetter.isNull ()) {
		refreshShape (shapeGetter);

		QSharedPointer<Shape> shape = lastShapes_[shapeGetter];
		restrictedPositionGetter_.setOriginalPosition (originalPosition);
		shape->accept (restrictedPositionGetter_);
		position = restrictedPositionGetter_.getRestrictedPosition ();
	} else {
		position = originalPosition;
	}
	mutex_.unlock ();

	return position;
}

bool RestrictionsManager::trySetRestrictionRemovalHandler (
	QSharedPointer<ShapeGetter> shapeGetter,
	QSharedPointer<RestrictionRemovalHandler> handler
) {
	bool result;

	//odstranenie obmedzovaca
	mutex_.lock ();
	if (shapeGetterUsages_.find (shapeGetter) != shapeGetterUsages_.end ()) {
		removalHandlers_[shapeGetter] = handler;
		result = true;
	} else {
		result = false;
	}
	mutex_.unlock ();

	return result;
}

void RestrictionsManager::setOrRunRestrictionRemovalHandler (
	QSharedPointer<ShapeGetter> shapeGetter,
	QSharedPointer<RestrictionRemovalHandler> handler
) {
	//odstranenie obmedzovaca
	mutex_.lock ();
	if (shapeGetterUsages_.find (shapeGetter) != shapeGetterUsages_.end ()) {
		removalHandlers_[shapeGetter] = handler;
	} else {
		handler->afterRestrictionRemoved ();
	}
	mutex_.unlock ();
}

void RestrictionsManager::setObserver (
	QSharedPointer<RestrictionsObserver> observer
) {
	mutex_.lock ();
	observer_ = observer;

	// send notifications reflecting the current state to the new observer
	for (LastShapesMapType::iterator it = lastShapes_.begin (); it != lastShapes_.end (); ++it) {
		notifyRestrictionAdded (it.key ());
		notifyShapeChanged (it.key (), it.value ());
	}
	mutex_.unlock ();
}

void RestrictionsManager::resetObserver () {
	mutex_.lock ();
	observer_.clear ();
	mutex_.unlock ();
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

	shapeComparator_.setOtherShape (lastShapes_[shapeGetter]);
	shape->accept (shapeComparator_);
	if (!shapeComparator_.getComparisonResult ()) {
		lastShapes_[shapeGetter] = shape;
		notifyShapeChanged (shapeGetter, shape);
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
