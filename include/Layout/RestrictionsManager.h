#ifndef Layout_RestrictionsManager_H
#define Layout_RestrictionsManager_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeGetter.h"
#include "Layout/ShapeVisitor_RestrictedPositionGetter.h"
#include "Layout/ShapeVisitor_Comparator.h"
#include "Layout/RestrictionsObserver.h"
#include "Layout/RestrictionRemovalHandler.h"
//-----------------------------------------------------------------------------
#include <QMap>
#include <QSet>
#include <QSharedPointer>
//-----------------------------------------------------------------------------

namespace Data {
	class Node;
}

namespace Layout {

/**
 * \brief Stores restrictions and provides main interface to work with the restrictions in a graph.
 * A graph should own one instance of this object and manage the restrictions though its methods.
 * Responsibilities:
 * - setting restrictions to nodes
 * - storing restrictions
 * - managing usages count of restrictions
 * - removing restriction after the last usage has been removed
 */
class RestrictionsManager {

public:

	/**
	 * \brief Sets the restriction represented by the shapeGetter to each node in the nodes set.
	 * If some node in the set already has a restriction attached, this restriction usage is removed,
	 * so the node will have only the new restriction represented by shapeGetter.
	 * If the shapeGetter is NULL, restriction (if any attached) is removed from each node in the nodes set.
	 */
	void setRestrictions (
		QSet<Data::Node *> nodes,
		QSharedPointer<ShapeGetter> shapeGetter
	);

	/**
	 * \brief Computes the restricted position for the node.
	 * \param[in] node Node to compute the position for.
	 * \param[in] originalPosition Original position of the node as computed by the layout algorithm.
	 * The returned restricted position is mostly the nearest point to the originalPosition which conforms
	 * the restriction.
	 */
	osg::Vec3f applyRestriction (
		Data::Node &node,
		osg::Vec3f originalPosition
	);

	/**
	 * \brief Tries to set the removal handler, which defines operations which need to be made when
	 * the restriction is removed.
	 * If the restriction represented by shapeGetter does not exist (e.g. its last usage has been removed and
	 * the restriction has been already destroyed) it does nothing and returns false.
	 */
	bool trySetRestrictionRemovalHandler (
		QSharedPointer<ShapeGetter> shapeGetter,
		QSharedPointer<RestrictionRemovalHandler> handler
	);

	/**
	 * \brief Sets the removal handler, which defines operations which need to be made when
	 * the restriction is removed.
	 * If the restriction represented by shapeGetter does not exist (e.g. its last usage has been removed and
	 * the restriction has been already destroyed), the handler code is run.
	 */
	void setOrRunRestrictionRemovalHandler (
		QSharedPointer<ShapeGetter> shapeGetter,
		QSharedPointer<RestrictionRemovalHandler> handler
	);

	/**
	 * \brief Sets the observer which is notified when a restriction is added, removed or its shape has been changed.
	 * [observer pattern]
	 */
	void setObserver (
		QSharedPointer<RestrictionsObserver> observer
	);

	/**
	 * \brief Removes the observer.
	 * [observer pattern]
	 */
	void resetObserver ();

	/***/
	virtual ~RestrictionsManager (void) {};

private:

	/**
	 * \brief Gets restriction (shape getter) associated with the specified node.
	 * Returns NULL if no restriction is associated.
	 */
	QSharedPointer<ShapeGetter> getShapeGetter (
		Data::Node &node
	);

	/**
	 * \brief Gets current shape from the restriction, notifies observer if the shape has been changed and
	 * stores it as last shape.
	 */
	void refreshShape (
		QSharedPointer<ShapeGetter> shapeGetter
	);

private: // observer notification

	/**
	 * [observer pattern]
	 */
	void notifyRestrictionAdded (
		QSharedPointer<ShapeGetter> shapeGetter
	);

	/**
	 * [observer pattern]
	 */
	void notifyShapeChanged (
		QSharedPointer<ShapeGetter> shapeGetter,
		QSharedPointer<Shape> shape
	);

	/**
	 * [observer pattern]
	 */
	void notifyRestrictionRemoved (
		QSharedPointer<ShapeGetter> shapeGetter
	);

private:

	typedef QMap<Data::Node *, QSharedPointer<ShapeGetter> > RestrictionsMapType;
	typedef QMap<QSharedPointer<ShapeGetter>, long> ShapeGetterUsagesMapType;
	typedef QMap<QSharedPointer<ShapeGetter>, QSharedPointer<Shape> > LastShapesMapType;
	typedef QMap<QSharedPointer<ShapeGetter>, QSharedPointer<RestrictionRemovalHandler> > RemovalHandlersMapType;

	/**
	 * \brief Nodes and associated restrictions.
	 */
	RestrictionsMapType restrictions_;

	/**
	 * \brief Usages count of shape getters.
	 * Used to determine when to destroy the shape getter (and run the removal handler) after last usage has been removed.
	 */
	ShapeGetterUsagesMapType shapeGetterUsages_;

	/**
	 * \brief Last shapes returned by the shape getters.
	 * Used to determine if the shape has been changed.
	 */
	LastShapesMapType lastShapes_;

	/**
	 * \brief Removal handlers defining code to be called when the shape getter is removed (after its last usage has been removed).
	 */
	RemovalHandlersMapType removalHandlers_;

	/**
	 * \brief Computes restricted positions for all restriction shapes.
	 */
	ShapeVisitor_RestrictedPositionGetter restrictedPositionGetter_;

	/**
	 * \brief Compares two shapes for equality.
	 */
	ShapeVisitor_Comparator shapeComparator_;

	/**
	 * \brief Observer currently set.
	 * NULL if no observer is set.
	 */
	QSharedPointer<RestrictionsObserver> observer_;

}; // class

} // namespace

#endif // Layout_RestrictionsManager_H
