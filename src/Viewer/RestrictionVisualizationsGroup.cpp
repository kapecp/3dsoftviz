#include "Viewer/RestrictionVisualizationsGroup.h"
//-----------------------------------------------------------------------------
#include <iostream>
//-----------------------------------------------------------------------------

namespace Vwr {

RestrictionVisualizationsGroup::RestrictionVisualizationsGroup (void) {

}

osg::ref_ptr<osg::Group> RestrictionVisualizationsGroup::getGroup() {
	return osg::ref_ptr<osg::Group> ();
}

void RestrictionVisualizationsGroup::restrictionAdded (
	QSharedPointer<Layout::ShapeGetter> shapeGetter
) {
	std::cout << "restriction added" << std::endl;
}

void RestrictionVisualizationsGroup::shapeChanged (
	QSharedPointer<Layout::ShapeGetter> shapeGetter,
	QSharedPointer<Layout::Shape> shape
) {
	std::cout << "shape changed" << std::endl;
}

void RestrictionVisualizationsGroup::restrictionRemoved (
	QSharedPointer<Layout::ShapeGetter> shapeGetter
) {
	std::cout << "restriction removed" << std::endl;
}

} // namespace
