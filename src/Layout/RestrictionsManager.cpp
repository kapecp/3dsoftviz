#include "Layout/RestrictionsManager.h"

#include "Data/Node.h"

void Layout::RestrictionsManager::setRestrictions (
        QSet<Data::Node *> nodes,
        QSharedPointer<Layout::ShapeGetter> shapeGetter
) {
        QList<QSharedPointer<Layout::RestrictionRemovalHandler> > removalHandlersToRun;

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
                )
                {
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

        for (QList<QSharedPointer<Layout::RestrictionRemovalHandler> >::iterator it = removalHandlersToRun.begin (); it != removalHandlersToRun.end (); ++it) {
		(*it)->afterRestrictionRemoved ();
	}




}

void Layout::RestrictionsManager::setRestrictionToShape(
    QLinkedList<osg::ref_ptr<Data::Node> > * nodesOfShapeGettersToRestrict,
    QSharedPointer<Layout::ShapeGetter> shapeGetter
){
    QLinkedList<osg::ref_ptr<Data::Node> >::iterator nodeIt;
    for (nodeIt=nodesOfShapeGettersToRestrict->begin(); nodeIt!=nodesOfShapeGettersToRestrict->end(); nodeIt++){
        QSharedPointer<Layout::ShapeGetter> shapeGetterToRestrict = getShapeGetter(*nodeIt);
            shapeGetterToRestrict->allowRestriction();
            shapeGetter->setInvisible(true);
            setRestrictions( shapeGetterToRestrict->getNodesOfShape(),shapeGetter);

    }
}

osg::Vec3f Layout::RestrictionsManager::applyRestriction (
        osg::ref_ptr<Data::Node> node,
        osg::Vec3f originalPosition
) {
	osg::Vec3f position;

	mutex_.lock ();
        QSharedPointer<Layout::ShapeGetter> shapeGetter = getShapeGetter (node);
	if (!shapeGetter.isNull ()) {


                QSharedPointer<Layout::Shape> shape = lastShapes_[shapeGetter];
		restrictedPositionGetter_.setOriginalPosition (originalPosition);
		shape->accept (restrictedPositionGetter_);
		position = restrictedPositionGetter_.getRestrictedPosition ();
                refreshShape (shapeGetter);
	} else {
		position = originalPosition;
	}



	mutex_.unlock ();

	return position;
}

bool Layout::RestrictionsManager::trySetRestrictionRemovalHandler (
        QSharedPointer<Layout::ShapeGetter> shapeGetter,
        QSharedPointer<Layout::RestrictionRemovalHandler> handler
) {
	bool result;

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

void Layout::RestrictionsManager::setOrRunRestrictionRemovalHandler (
        QSharedPointer<Layout::ShapeGetter> shapeGetter,
        QSharedPointer<Layout::RestrictionRemovalHandler> handler
) {
	mutex_.lock ();
	if (shapeGetterUsages_.find (shapeGetter) != shapeGetterUsages_.end ()) {
		removalHandlers_[shapeGetter] = handler;
	} else {
		handler->afterRestrictionRemoved ();
	}
	mutex_.unlock ();
}

void Layout::RestrictionsManager::setObservers (
        QSharedPointer<Vwr::RestrictionVisualizationsGroup> v_observer,
        QSharedPointer<Vwr::RestrictionManipulatorsGroup> m_observer
) {
	mutex_.lock ();
        visualizationObserver = v_observer;
        manipulationObserver = m_observer;

	// send notifications reflecting the current state to the new observer
	for (LastShapesMapType::iterator it = lastShapes_.begin (); it != lastShapes_.end (); ++it) {
		notifyRestrictionAdded (it.key ());
		notifyShapeChanged (it.key (), it.value ());
	}

	mutex_.unlock ();
}

void Layout::RestrictionsManager::resetObserver () {
	mutex_.lock ();
        visualizationObserver.clear ();
        manipulationObserver.clear();
	mutex_.unlock ();
}

QSharedPointer<Layout::ShapeGetter> Layout::RestrictionsManager::getShapeGetter (
        osg::ref_ptr<Data::Node> node
) {
        RestrictionsMapType::iterator it = restrictions_.find (node);
	if (it != restrictions_.end ()) {
		return it.value();
	} else {
                return QSharedPointer<Layout::ShapeGetter> ();
	}
}

osg::Group* Layout::RestrictionsManager::getNodes(QSharedPointer<ShapeGetter> shapeGetter){
    osg::Group* result = new osg::Group;
    RestrictionsMapType::iterator it;
    for (it = restrictions_.begin(); it != restrictions_.end(); ++it){
        if (it.value() == shapeGetter)  result->addChild(it.key());
    }
    return result;

}

void Layout::RestrictionsManager::refreshShape (
        QSharedPointer<Layout::ShapeGetter> shapeGetter
) {
        QSharedPointer<Layout::Shape> shape = shapeGetter->getShape ();

	shapeComparator_.setOtherShape (lastShapes_[shapeGetter]);
	shape->accept (shapeComparator_);
	if (!shapeComparator_.getComparisonResult ()) {
		lastShapes_[shapeGetter] = shape;
                notifyShapeChanged (shapeGetter, shape);
	}
}

void Layout::RestrictionsManager::notifyRestrictionAdded (
        QSharedPointer<Layout::ShapeGetter> shapeGetter
) {
        if (!visualizationObserver.isNull ()) {
                visualizationObserver->restrictionAdded (shapeGetter);
	}
        if (!manipulationObserver.isNull ()) {
                manipulationObserver->restrictionAdded (shapeGetter);
        }

}

void Layout::RestrictionsManager::notifyShapeChanged (
        QSharedPointer<Layout::ShapeGetter> shapeGetter,
        QSharedPointer<Layout::Shape> shape
) {
        if (!visualizationObserver.isNull ()) {
                visualizationObserver->shapeChanged (shapeGetter, shape);
	}
        if (!manipulationObserver.isNull ()) {
                manipulationObserver->shapeChanged (shapeGetter, shape);

        }
}

void Layout::RestrictionsManager::notifyRestrictionRemoved (
        QSharedPointer<Layout::ShapeGetter> shapeGetter
) {
        if (!visualizationObserver.isNull ()) {
                visualizationObserver->restrictionRemoved (shapeGetter);
	}
        if (!manipulationObserver.isNull ()) {
                manipulationObserver->restrictionRemoved (shapeGetter);
        }
}

