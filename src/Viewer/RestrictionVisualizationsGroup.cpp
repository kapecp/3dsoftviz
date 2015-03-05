#include "Viewer/RestrictionVisualizationsGroup.h"
//-----------------------------------------------------------------------------


namespace Vwr {

RestrictionVisualizationsGroup::RestrictionVisualizationsGroup( void )
	: group_( new osg::Group() )
{
}

osg::ref_ptr<osg::Group> RestrictionVisualizationsGroup::getGroup()
{
	return group_;
}

void RestrictionVisualizationsGroup::restrictionAdded(
	QSharedPointer<Layout::ShapeGetter> shapeGetter
)
{
	osg::Group* visualizerWrapper = new osg::Group;
	group_->addChild( visualizerWrapper );
	restrictionsToVisualizers_[shapeGetter] = visualizerWrapper;
}

void RestrictionVisualizationsGroup::shapeChanged(
	QSharedPointer<Layout::ShapeGetter> shapeGetter,
	QSharedPointer<Layout::Shape> shape
)
{
	shape->accept( visualizerCreator_ );
	osg::ref_ptr<osg::Node> visualizer = visualizerCreator_.getCreatedVisualizer();

	osg::ref_ptr<osg::Group> visualizerWrapper = restrictionsToVisualizers_[shapeGetter];
	if ( visualizerWrapper->getNumChildren() > 0 ) {
		visualizerWrapper->setChild( 0, visualizer );
	}
	else {
		visualizerWrapper->addChild( visualizer );
	}
}

void RestrictionVisualizationsGroup::restrictionRemoved(
	QSharedPointer<Layout::ShapeGetter> shapeGetter
)
{
	group_->removeChild( restrictionsToVisualizers_[shapeGetter] );
	restrictionsToVisualizers_.remove( shapeGetter );
}

osg::Group* RestrictionVisualizationsGroup::getVisualizer( QSharedPointer<Layout::ShapeGetter> shapeGetter )
{
	return restrictionsToVisualizers_[shapeGetter];
}



} // namespace
