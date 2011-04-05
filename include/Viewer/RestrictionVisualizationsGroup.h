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

class RestrictionVisualizationsGroup
	: public Layout::RestrictionsObserver
{

public:

	RestrictionVisualizationsGroup (void);

	/***/
	virtual ~RestrictionVisualizationsGroup (void) {};

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

private:

	osg::ref_ptr<osg::Group> group_;

	typedef QMap<QSharedPointer<Layout::ShapeGetter>, osg::Group *> RestrictionsToVisualizersMapType;

	RestrictionsToVisualizersMapType restrictionsToVisualizers_;

	ShapeVisitor_VisualizerCreator visualizerCreator_;

}; // class

} // namespace

#endif // Viewer_RestrictionVisualizationsGroup_H
