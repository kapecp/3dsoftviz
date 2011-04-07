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
	osg::Group * group = new osg::Group;
	Layout::Shape_Composite::ShapesListType & shapes = shape.getShapes ();
	for (Layout::Shape_Composite::ShapesListType::iterator it = shapes.begin (); it != shapes.end (); ++it) {
		(*it)->accept (*this);
		group->addChild (createdVisualizer_);
	}

	createdVisualizer_ = group;
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
	visualizeSphere (shape);
}

void ShapeVisitor_VisualizerCreator::visit (Layout::Shape_SphereSurface & shape) {
	visualizeSphere (shape);
}

void ShapeVisitor_VisualizerCreator::visualizeSphere (Layout::Shape_AbstractSphere & abstractSphere) {
	osg::Sphere * sphere = new osg::Sphere;
	sphere->setRadius (getScaledDistance (abstractSphere.getRadius ()));

	osg::ShapeDrawable * sd = new osg::ShapeDrawable;
	sd->setShape (sphere);
	sd->setColor (osg::Vec4 (0, 0, 1.0, 0.2));

	osg::Geode * geode = new osg::Geode;
	geode->addDrawable (sd);

	createdVisualizer_ = wrapByAutoTransform (geode, getScaledPosition (abstractSphere.getCenter ()));
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
