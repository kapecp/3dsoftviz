#include "Viewer/RestrictionVisualizationsGroup.h"
//-----------------------------------------------------------------------------

namespace Vwr {

RestrictionVisualizationsGroup::RestrictionVisualizationsGroup (void)
: group_ (new osg::Group())
{
}

osg::ref_ptr<osg::Group> RestrictionVisualizationsGroup::getGroup() {
	return group_;
}

void RestrictionVisualizationsGroup::restrictionAdded (
	QSharedPointer<Layout::ShapeGetter> shapeGetter
) {
	osg::Group * visualizerWrapper = new osg::Group;
	group_->addChild (visualizerWrapper);
	restrictionsToVisualizers_[shapeGetter] = visualizerWrapper;
}

void RestrictionVisualizationsGroup::shapeChanged (
	QSharedPointer<Layout::ShapeGetter> shapeGetter,
	QSharedPointer<Layout::Shape> shape
) {
	shape->accept (visualizerCreator_);
	osg::Node * visualizer = visualizerCreator_.getCreatedVisualizer ();

	osg::Group * visualizerWrapper = restrictionsToVisualizers_[shapeGetter];
	if (visualizerWrapper->getNumChildren () == 1) {
		visualizerWrapper->setChild (0, visualizer);
	} else {
		visualizerWrapper->addChild (visualizer);
	}
}

void RestrictionVisualizationsGroup::restrictionRemoved (
	QSharedPointer<Layout::ShapeGetter> shapeGetter
) {
	group_->removeChild (restrictionsToVisualizers_[shapeGetter]);
	restrictionsToVisualizers_.remove (shapeGetter);
}

} // namespace
