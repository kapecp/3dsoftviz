#include "Layout/RestrictionsManager.h"

#include "Data/Node.h"
#include "Layout/Shape_Null.h"

namespace Layout {

void RestrictionsManager::setRestrictions(QSet<Data::Node *> &nodes, const QSharedPointer<ShapeGetter> &shapeGetter)
{
    QList<QSharedPointer<RestrictionRemovalHandler> > removalHandlersToRun;

    mMutex.lock();
    for (QSet<Data::Node *>::iterator it = nodes.begin (); it != nodes.end (); ++it)
    {
        // remove current shape getter if exists
        RestrictionsMapType::iterator it2 = mRestrictions.find(*it);
        if (it2 != mRestrictions.end())
        {
            --mShapeGetterUsages[it2.value()];
            if (mShapeGetterUsages[it2.value()] == 0)
            {
                // last shape getter usage removed

                notifyRestrictionRemoved (it2.value());

                mShapeGetterUsages.remove (it2.value());
                mLastShapes.remove(it2.value());

                RemovalHandlersMapType::iterator removalHandlerIt = mRemovalHandlers.find(it2.value());
                if (!removalHandlerIt->isNull())
                    removalHandlersToRun.push_back(*removalHandlerIt);

                mRemovalHandlers.erase(removalHandlerIt);
            }
            mRestrictions.remove(*it);
        }

        if ( (!shapeGetter.isNull()) && ((*it)->getPositionCanBeRestricted()) )
        {
            // add shape getter
            mRestrictions[*it] = shapeGetter;
            ++mShapeGetterUsages[shapeGetter];
            if (mShapeGetterUsages[shapeGetter] == 1)
            {
                // first shape getter usage added

                mLastShapes[shapeGetter] = QSharedPointer<Shape>(new Shape_Null);
                mRemovalHandlers[shapeGetter];

                notifyRestrictionAdded(shapeGetter);
                refreshShape(shapeGetter);
            }
        }
    }
    mMutex.unlock();

    for (QList< QSharedPointer<RestrictionRemovalHandler> >::iterator it = removalHandlersToRun.begin(); it != removalHandlersToRun.end(); ++it)
    {
        (*it)->afterRestrictionRemoved();
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
                  QSet<Data::Node *> nodes = shapeGetterToRestrict->getNodesOfShape();
            setRestrictions(nodes,shapeGetter);

    }
}

float RestrictionsManager::distanceFromFocus(Data::Node &node)
{
    mMutex.lock();

    float distance;

    QSharedPointer<ShapeGetter> shapeGetter = getShapeGetter(node);

    // Get distance from focus by chosen shape
    if (!shapeGetter.isNull())
    {
        refreshShape(shapeGetter);

        QSharedPointer<Shape> shape = mLastShapes[shapeGetter];
        mDistanceFromFocusGetter.setPosition( node.targetPositionConstRef() );
        shape->accept(mDistanceFromFocusGetter);
        distance = mDistanceFromFocusGetter.distanceFromFocus();
    }
    else
    {
        distance = -1;
    }

    mMutex.unlock();

    return distance;
}

void RestrictionsManager::applyRestriction(Data::Node &node)
{
    mMutex.lock();

    QSharedPointer<ShapeGetter> shapeGetter = getShapeGetter(node);

    // Set restriction by chosen shape
    if (!shapeGetter.isNull() && !node.isFocused())
    {
        QSharedPointer<Shape> shape = mLastShapes[shapeGetter];
        mRestrictedPositionGetter.setOriginalPosition( node.targetPositionConstRef() );
        shape->accept(mRestrictedPositionGetter);
        node.setRestrictedTargetPosition( mRestrictedPositionGetter.restrictedPositionConstRef() );
        refreshShape(shapeGetter);
    }
    else
    {
        node.setRestrictedTargetPosition( node.targetPositionConstRef() );
    }

    mMutex.unlock();
}

bool RestrictionsManager::trySetRestrictionRemovalHandler(const QSharedPointer<ShapeGetter> &shapeGetter,
                                                          const QSharedPointer<RestrictionRemovalHandler> &handler)
{
    bool result;

    // Remove restriction
    mMutex.lock();
    if (mShapeGetterUsages.find(shapeGetter) != mShapeGetterUsages.end())
    {
        mRemovalHandlers[shapeGetter] = handler;
        result = true;
    }
    else
    {
        result = false;
    }
    mMutex.unlock();

    return result;
}

void RestrictionsManager::setOrRunRestrictionRemovalHandler(const QSharedPointer<ShapeGetter> &shapeGetter,
                                                            const QSharedPointer<RestrictionRemovalHandler> &handler)
{
    // Remove restriction
    mMutex.lock();
    if (mShapeGetterUsages.find(shapeGetter) != mShapeGetterUsages.end())
    {
        mRemovalHandlers[shapeGetter] = handler;
    }
    else
    {
        handler->afterRestrictionRemoved();
    }
    mMutex.unlock();
}

void Layout::RestrictionsManager::setObservers (
        QSharedPointer<Vwr::RestrictionVisualizationsGroup> v_observer,
        QSharedPointer<Vwr::RestrictionManipulatorsGroup> m_observer
) {
    mMutex.lock();
        visualizationObserver = v_observer;
        manipulationObserver = m_observer;

    // send notifications reflecting the current state to the new observer
    for (LastShapesMapType::iterator it = mLastShapes.begin (); it != mLastShapes.end (); ++it) {
        notifyRestrictionAdded (it.key ());
        notifyShapeChanged (it.key (), it.value ());
    }
    mMutex.unlock();
}

void RestrictionsManager::resetObserver()
{
    mMutex.lock();
        visualizationObserver.clear ();
        manipulationObserver.clear();
    mMutex.unlock();
}

QSharedPointer<ShapeGetter> RestrictionsManager::getShapeGetter(Data::Node &node)
{
    RestrictionsMapType::iterator it = mRestrictions.find(&node);
    if (it != mRestrictions.end())
        return it.value();
    else
        return QSharedPointer<ShapeGetter>();
}
QSharedPointer<Layout::ShapeGetter> Layout::RestrictionsManager::getShapeGetter (
        osg::ref_ptr<Data::Node> node
) {
        RestrictionsMapType::iterator it = mRestrictions.find (node);
    if (it != mRestrictions.end ()) {
        return it.value();
    } else {
                return QSharedPointer<Layout::ShapeGetter> ();
    }
}
osg::Group* Layout::RestrictionsManager::getNodes(QSharedPointer<ShapeGetter> shapeGetter){
    osg::Group* result = new osg::Group;
    RestrictionsMapType::iterator it;
    for (it = mRestrictions.begin(); it != mRestrictions.end(); ++it){
        if (it.value() == shapeGetter)  result->addChild(it.key());
    }
    return result;

}

void RestrictionsManager::refreshShape(const QSharedPointer<ShapeGetter> &shapeGetter)
{
    QSharedPointer<Shape> shape = shapeGetter->getShape();

    mShapeComparator.setOtherShape(mLastShapes[shapeGetter]);
    shape->accept(mShapeComparator);
    if (!mShapeComparator.getComparisonResult())
    {
        mLastShapes[shapeGetter] = shape;
        notifyShapeChanged(shapeGetter, shape);
    }
}

void RestrictionsManager::notifyRestrictionAdded(const QSharedPointer<ShapeGetter> &shapeGetter)
{
        if (!visualizationObserver.isNull ()) {
                visualizationObserver->restrictionAdded (shapeGetter);
    }
        if (!manipulationObserver.isNull ()) {
                manipulationObserver->restrictionAdded (shapeGetter);
        }
}

void RestrictionsManager::notifyShapeChanged(const QSharedPointer<ShapeGetter> &shapeGetter,
                                             const QSharedPointer<Shape> &shape)
{
        if (!visualizationObserver.isNull ()) {
                visualizationObserver->shapeChanged (shapeGetter, shape);
    }
        if (!manipulationObserver.isNull ()) {
                manipulationObserver->shapeChanged (shapeGetter, shape);

        }
}

void RestrictionsManager::notifyRestrictionRemoved(const QSharedPointer<ShapeGetter> &shapeGetter)
{
        if (!visualizationObserver.isNull ()) {
                visualizationObserver->restrictionRemoved (shapeGetter);
    }
        if (!manipulationObserver.isNull ()) {
                manipulationObserver->restrictionRemoved (shapeGetter);
        }
}

} // namespace
