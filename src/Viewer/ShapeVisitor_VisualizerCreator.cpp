#include "Viewer/ShapeVisitor_VisualizerCreator.h"
//-----------------------------------------------------------------------------
#include "Util/ApplicationConfig.h"
//-----------------------------------------------------------------------------
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/AutoTransform>
//-----------------------------------------------------------------------------

namespace Vwr {

osg::Node * ShapeVisitor_VisualizerCreator::getCreatedVisualizer (void) {
	return createdVisualizer_;
}

void ShapeVisitor_VisualizerCreator::visit (Layout::Shape_Composite & shape) {

}

void ShapeVisitor_VisualizerCreator::visit (Layout::Shape_Plane & shape) {
	osg::InfinitePlane * plane = new osg::InfinitePlane;
	plane->set (shape.getNormalVector (), -getScaledDistance (shape.getD ()));

	osg::ShapeDrawable * sd = new osg::ShapeDrawable;
	sd->setShape (plane);
	sd->setColor (osg::Vec4 (0, 0.5, 0.0, 0.5));

	osg::Geode * geode = new osg::Geode;
	geode->addDrawable (sd);

	createdVisualizer_ = geode;
}

void ShapeVisitor_VisualizerCreator::visit (Layout::Shape_Sphere & shape) {

}

void ShapeVisitor_VisualizerCreator::visit (Layout::Shape_SphereSurface & shape) {
	osg::Sphere * sphere = new osg::Sphere;
	sphere->setRadius (getScaledDistance (shape.getRadius ()));

	osg::ShapeDrawable * sd = new osg::ShapeDrawable;
	sd->setShape (sphere);
	sd->setColor (osg::Vec4 (0, 0, 1.0, 0.2));

	osg::Geode * geode = new osg::Geode;
	geode->addDrawable (sd);

	createdVisualizer_ = wrapByAutoTransform (geode, getScaledPosition (shape.getCenter ()));
}

osg::Node * ShapeVisitor_VisualizerCreator::wrapByAutoTransform (
	osg::Node * node,
	const osg::Vec3f & position
) {
	osg::AutoTransform * at = new osg::AutoTransform;
	at->addChild (node);
	at->setPosition (position);

	return at;
}

float ShapeVisitor_VisualizerCreator::getScaledDistance (
	const float & distance
) {
	return distance * getScale ();
}

osg::Vec3f ShapeVisitor_VisualizerCreator::getScaledPosition (
	const osg::Vec3f & position
) {
	return position * getScale ();
}

float ShapeVisitor_VisualizerCreator::getScale () {
	return Util::ApplicationConfig::get()->getValue("Viewer.Display.NodeDistanceScale").toFloat();
}

} // namespace
