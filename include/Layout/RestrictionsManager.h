#ifndef LAYOUT_RESTRICTIONSMANAGER_H
#define LAYOUT_RESTRICTIONSMANAGER_H

#include <QMap>
#include <QSet>
#include <QSharedPointer>
#include <QMutex>
#include <QtGlobal>
#include "Layout/ShapeGetter.h"
#include "Layout/ShapeVisitor_RestrictedPositionGetter.h"
#include "Layout/ShapeVisitor_DistanceFromFocusGetter.h"
#include "Layout/ShapeVisitor_Comparator.h"
#include "Layout/RestrictionsObserver.h"
#include "Layout/RestrictionRemovalHandler.h"
#include "Viewer/RestrictionManipulatorsGroup.h"
#include "Viewer/RestrictionVisualizationsGroup.h"

namespace Data {
class Node;
}

namespace Layout {
typedef QMap<Data::Node*, QSharedPointer<ShapeGetter> > RestrictionsMapType;
typedef QMap<QSharedPointer<ShapeGetter>, long> ShapeGetterUsagesMapType;
typedef QMap<QSharedPointer<ShapeGetter>, QSharedPointer<Shape> > LastShapesMapType;
typedef QMap<QSharedPointer<ShapeGetter>, QSharedPointer<RestrictionRemovalHandler> > RemovalHandlersMapType;


/**
 * \brief Stores restrictions and provides main interface to work with the
 * restrictions in a graph.
 * A graph should own one instance of this object and manage the restrictions
 * though its methods.
 * Responsibilities:
 * - setting restrictions to nodes
 * - storing restrictions
 * - managing usages count of restrictions
 * - removing restriction after the last usage has been removed
 */
class RestrictionsManager
{
public:
	//RestrictionsManager(){
	//}

	virtual ~RestrictionsManager() {}

	/**
	 * \brief Sets the restriction represented by the shapeGetter to each node
	 * in the nodes set.
	 * If some node in the set already has a restriction attached, this
	 * restriction usage is removed, so the node will have only the new
	 * restriction represented by shapeGetter. If the shapeGetter is NULL,
	 * restriction (if any attached) is removed from each node in the nodes
	 * set.
	 */
	void setRestrictions( QSet<Data::Node*>& nodes, const QSharedPointer<ShapeGetter>& shapeGetter );

	/**
		 * \brief Sets the restriction represented by the shapeGetter to nides that represented shape with
		 *  specific node.
		 */
	void setRestrictionToShape(
		QLinkedList<osg::ref_ptr<Data::Node> >* nodesOfShapeGettersToRestrict,
		QSharedPointer<ShapeGetter> shapeGetter
	);

	/**
	 * \brief Finds out the distance of a node from the focus point according
	 * to currently used restriction. If none restriction is used at the
	 * moment, this function will return -1 value.
	 * \param node the node for which a distance from the focus point will be
	 * computed
	 * \return the distance of the node from the focus point
	 */
	float distanceFromFocus( Data::Node& node );

	/**
	 * \brief Computes the restricted position for the node.
	 * \param[in] node Node to compute the position for.
	 * The computed restricted position is mostly the nearest point to the
	 * originalPosition which conforms the restriction.
	 * The process of getting the restricted position includes getting the
	 * latest shape from the shape getter and notifying the observer if the
	 * shape has been changed.
	 */
	void applyRestriction( Data::Node& node );

	osg::Group* getRestrictedNodes( Shape& shape );

	/**
	 * \brief Tries to set the removal handler, which defines operations which
	 * need to be made when the restriction is removed.
	 * If the restriction represented by shapeGetter does not exist (e.g. its
	 * last usage has been removed and the restriction has been already
	 * destroyed) it does nothing and returns false.
	 */
	bool trySetRestrictionRemovalHandler( const QSharedPointer<ShapeGetter>& shapeGetter,
										  const QSharedPointer<RestrictionRemovalHandler>& handler );

	/**
	 * \brief Sets the removal handler, which defines operations which need to
	 * be made when the restriction is removed.
	 * If the restriction represented by shapeGetter does not exist (e.g. its
	 * last usage has been removed and the restriction has been already
	 * destroyed), the handler code is run.
	 */
	void setOrRunRestrictionRemovalHandler( const QSharedPointer<ShapeGetter>& shapeGetter,
											const QSharedPointer<RestrictionRemovalHandler>& handler );

	/**
	 * \brief Sets the observer which is notified when a restriction is added,
	 * removed or its shape has been changed.
	 * [observer pattern]
	 */
	void setObservers(
		QSharedPointer<Vwr::RestrictionVisualizationsGroup> v_observer,
		QSharedPointer<Vwr::RestrictionManipulatorsGroup> m_observer
	);

	/**
	 * \brief Removes the observer.
	 * [observer pattern]
	 */
	void resetObserver();

private:

	/**
	 * \brief Gets restriction (shape getter) associated with the specified
	 * node.
	 * Returns NULL if no restriction is associated.
	 */
	QSharedPointer<ShapeGetter> getShapeGetter( Data::Node& node );
	QSharedPointer<ShapeGetter> getShapeGetter(
		osg::ref_ptr<Data::Node> node
	);
	/**
	 * \brief Gets current shape from the restriction, notifies observer if
	 * the shape has been changed and stores it as last shape.
	 */
	void refreshShape( const QSharedPointer<ShapeGetter>& shapeGetter );

	osg::Group* getNodes( QSharedPointer<ShapeGetter> shapeGetter );

private: // observer notification

	/**
	 * [observer pattern]
	 */
	void notifyRestrictionAdded( const QSharedPointer<ShapeGetter>& shapeGetter );

	/**
	 * [observer pattern]
	 */
	void notifyShapeChanged( const QSharedPointer<ShapeGetter>& shapeGetter, const QSharedPointer<Shape>& shape );

	/**
	 * [observer pattern]
	 */
	void notifyRestrictionRemoved( const QSharedPointer<ShapeGetter>& shapeGetter );

private:

	typedef QMap<osg::ref_ptr<Data::Node>, QSharedPointer<ShapeGetter> > RestrictionsMapType;
	//typedef QMap<Data::Node *, QSharedPointer<ShapeGetter> > RestrictionsMapType;
	typedef QMap<QSharedPointer<ShapeGetter>, long> ShapeGetterUsagesMapType;
	typedef QMap<QSharedPointer<ShapeGetter>, QSharedPointer<Shape> > LastShapesMapType;
	typedef QMap<QSharedPointer<ShapeGetter>, QSharedPointer<RestrictionRemovalHandler> > RemovalHandlersMapType;

	/**
	 * \brief Nodes and associated restrictions.
	 */
	RestrictionsMapType mRestrictions;

	/**
	 * \brief Usages count of shape getters.
	 * Used to determine when to destroy the shape getter (and run the removal
	 * handler) after last usage has been removed.
	 */
	ShapeGetterUsagesMapType mShapeGetterUsages;

	/**
	 * \brief Last shapes returned by the shape getters.
	 * Used to determine if the shape has been changed.
	 */
	LastShapesMapType mLastShapes;

	/**
	 * \brief Removal handlers defining code to be called when the shape
	 * getter is removed (after its last usage has been removed).
	 */
	RemovalHandlersMapType mRemovalHandlers;

	/**
	 * \brief Computes distance from focus for all shapes.
	 */
	ShapeVisitor_DistanceFromFocusGetter mDistanceFromFocusGetter;

	/**
	 * \brief Computes restricted positions for all restriction shapes.
	 */
	ShapeVisitor_RestrictedPositionGetter mRestrictedPositionGetter;

	/**
	 * \brief Compares two shapes for equality.
	 */
	ShapeVisitor_Comparator mShapeComparator;

	/**
	 * \brief Observer currently set.
	 * NULL if no observer is set.
	 */
	QSharedPointer<Vwr::RestrictionVisualizationsGroup> visualizationObserver;

	/**
		 * \brief Observer currently set.
		 * NULL if no observer is set.
		 */
	QSharedPointer<Vwr::RestrictionManipulatorsGroup> manipulationObserver;


	/**
	 * \brief usage: to synchronize public method calls (they change/get
	 * internal state and can be called from 2 threads: layout and gui)
	 */
	QMutex mMutex;
};

}

#endif // LAYOUT_RESTRICTIONSMANAGER_H
