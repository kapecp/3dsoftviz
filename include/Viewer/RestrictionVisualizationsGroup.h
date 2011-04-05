#ifndef Viewer_RestrictionVisualizationsGroup_H
#define Viewer_RestrictionVisualizationsGroup_H
//-----------------------------------------------------------------------------
#include "Layout/RestrictionsObserver.h"
//-----------------------------------------------------------------------------
#include <osg/ref_ptr>
#include <osg/Group>
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

}; // class

} // namespace

#endif // Viewer_RestrictionVisualizationsGroup_H
