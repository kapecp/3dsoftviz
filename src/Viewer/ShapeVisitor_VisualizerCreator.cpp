#include "Viewer/ShapeVisitor_VisualizerCreator.h"
//-----------------------------------------------------------------------------
#include <osg/Geode>
#include <osg/ShapeDrawable>
//-----------------------------------------------------------------------------

namespace Vwr {

osg::Node * ShapeVisitor_VisualizerCreator::getCreatedVisualizer (void) {
	return createdVisualizer_;
}

void ShapeVisitor_VisualizerCreator::visit (Layout::Shape_Composite & shape) {

}

void ShapeVisitor_VisualizerCreator::visit (Layout::Shape_Plane & shape) {

}

void ShapeVisitor_VisualizerCreator::visit (Layout::Shape_Sphere & shape) {

}

void ShapeVisitor_VisualizerCreator::visit (Layout::Shape_SphereSurface & shape) {
	osg::Geode * geode = new osg::Geode;
	osg::ShapeDrawable * sd = new osg::ShapeDrawable;
	osg::Sphere * sphere = new osg::Sphere;
	sphere->setRadius (20);
	sd->setShape (sphere);
	sd->setColor (osg::Vec4 (0, 0, 1.0, 0.2));
	geode->addDrawable (sd);
	//osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform;
	//at->setPosition(node->getTargetPosition() * graphScale);

	createdVisualizer_ = geode;
}

} // namespace
