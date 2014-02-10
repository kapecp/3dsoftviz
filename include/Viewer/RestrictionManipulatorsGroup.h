#ifndef RESTRICTIONMANIPULATORSGROUP_H
#define RESTRICTIONMANIPULATORSGROUP_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeGetter.h"
#include "Layout/RestrictionsObserver.h"
#include "Viewer/ShapeVisitor_ManipulatorCreator.h"
//-----------------------------------------------------------------------------
#include <osg/ref_ptr>
#include <osg/Group>
#include <QMap>
#include <QSharedPointer>

#include <osg/Switch>
//-----------------------------------------------------------------------------


namespace Vwr {


/**
 * \brief Encapsulates part of the OSG including restriction manipulations.
 */
class RestrictionManipulatorsGroup: public Layout::RestrictionsObserver
{


public:
	RestrictionManipulatorsGroup();

	/***/
	virtual ~RestrictionManipulatorsGroup (void) {};

	/**
	 * [getter].
	 */
	osg::ref_ptr<osg::Group> getGroup();

	virtual void restrictionAdded (
			QSharedPointer<Layout::ShapeGetter> shapeGetter
			);


	virtual void shapeChanged (
			QSharedPointer<Layout::ShapeGetter> shapeGetter,
			QSharedPointer<Layout::Shape> shape
			);


	virtual void restrictionRemoved (
			QSharedPointer<Layout::ShapeGetter> shapeGetter
			);

	void addNodeToSelection(QSharedPointer<Layout::ShapeGetter> shapeGetter, osg::Node* node);

	osg::Geode* test();

private:
	osg::ref_ptr<osg::Group> group_;

	typedef QMap<QSharedPointer<Layout::ShapeGetter>, osg::Switch*> ManipulatorMapType;

	/**
	 * \brief Holds associations between restrictions and manipulator.
	 */
	ManipulatorMapType manipulator;

	/**
	 * \brief Creates manipulators (OSG objects) of provided restrictions.
	 */
	ShapeVisitor_ManipulatorCreator manipulatorCreator;

	/**
	 * \brief True, if manipulator is visible;
	 */
	bool TURN_ON;
};


}

#endif // RESTRICTIONMANIPULATORSGROUP_H
