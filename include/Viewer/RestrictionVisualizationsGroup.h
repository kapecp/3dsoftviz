#ifndef Viewer_RestrictionVisualizationsGroup_H
#define Viewer_RestrictionVisualizationsGroup_H
//-----------------------------------------------------------------------------
#include "Layout/RestrictionsObserver.h"
#include "Layout/ShapeGetter.h"
#include "Viewer/ShapeVisitor_VisualizerCreator.h"
//-----------------------------------------------------------------------------
#include <osg/ref_ptr>
#include <osg/Group>
#include <QMap>
#include <QSharedPointer>
//-----------------------------------------------------------------------------

namespace Vwr {

/**
 * \brief Encapsulates part of the OSG including restriction visualizations.
 */
class RestrictionVisualizationsGroup: public Layout::RestrictionsObserver
{

public:

	RestrictionVisualizationsGroup (void);

	/***/
	virtual ~RestrictionVisualizationsGroup (void) {};

	/**
	 * [getter].
	 */
	osg::ref_ptr<osg::Group> getGroup();

	/**
	 * [observer pattern]
	 */
	virtual void restrictionAdded (
		QSharedPointer<Layout::ShapeGetter> shapeGetter
	);

	/**
	 * [observer pattern]
	 */
	virtual void shapeChanged (
		QSharedPointer<Layout::ShapeGetter> shapeGetter,
		QSharedPointer<Layout::Shape> shape
	);

	/**
	 * [observer pattern]
	 */
	virtual void restrictionRemoved (
		QSharedPointer<Layout::ShapeGetter> shapeGetter
	);


        osg::Group* getVisualizer(QSharedPointer<Layout::ShapeGetter> shapeGetter);
private:

	/**
	 * \brief Part of OSG with restriction visualizations.
	 */
	osg::ref_ptr<osg::Group> group_;

        typedef QMap<QSharedPointer<Layout::ShapeGetter>, osg::ref_ptr<osg::Group> > RestrictionsToVisualizersMapType;


	/**
	 * \brief Holds associations between restrictions and visualizations.
	 */
	RestrictionsToVisualizersMapType restrictionsToVisualizers_;

	/**
	 * \brief Creates visualizations (OSG objects) of provided restrictions.
	 */
	ShapeVisitor_VisualizerCreator visualizerCreator_;



}; // class

} // namespace

#endif // Viewer_RestrictionVisualizationsGroup_H
