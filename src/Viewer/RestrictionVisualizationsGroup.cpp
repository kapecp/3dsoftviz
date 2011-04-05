#include "Viewer/RestrictionVisualizationsGroup.h"
//-----------------------------------------------------------------------------
#include <iostream>
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
	std::cout << "restriction added" << std::endl;

	osg::Group * visualizerWrapper = new osg::Group;
	group_->addChild (visualizerWrapper);
	restrictionsToVisualizers_[shapeGetter] = visualizerWrapper;
}

void RestrictionVisualizationsGroup::shapeChanged (
	QSharedPointer<Layout::ShapeGetter> shapeGetter,
	QSharedPointer<Layout::Shape> shape
) {
	std::cout << "shape changed" << std::endl;

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
	std::cout << "restriction removed" << std::endl;

	group_->removeChild (restrictionsToVisualizers_[shapeGetter]);
	restrictionsToVisualizers_.remove (shapeGetter);
}

} // namespace
