#include "Viewer/ShapeVisitor_VisualizerCreator.h"
//-----------------------------------------------------------------------------
#include "Util/ApplicationConfig.h"
//-----------------------------------------------------------------------------
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/BlendFunc>
//volovar pridanie wireframe modu pre radial layout
#include <osg/PolygonMode>
//volovar koniec
//-----------------------------------------------------------------------------

namespace Vwr {

osg::ref_ptr<osg::Node> ShapeVisitor_VisualizerCreator::getCreatedVisualizer (void) {
	return createdVisualizer_;
}

void ShapeVisitor_VisualizerCreator::visit (Layout::Shape_Null /*& shape*/) {
	createdVisualizer_ = new osg::Group;
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


void ShapeVisitor_VisualizerCreator::visit (Layout::Shape_Intersection & shape) {
	float radius;
	osg::Vec3 center;
	//osg::Vec3 normalVector;

	if (shape.getCompositeType() == Layout::Shape_Composite::CompositeType::CIRCLE){
		Layout::Shape_Composite::ShapesListType & shapes = shape.getShapes();
		Layout::Shape_Composite::ShapesListType::iterator it = shapes.begin ();

		if(QSharedPointer<Layout::Shape_Sphere> sphere = qSharedPointerCast<Layout::Shape_Sphere>(*it)){
			radius = sphere->getRadius();
			center = sphere->getCenter();
		}
		it++;

		//if(QSharedPointer<Layout::Shape_Plane> plane = qSharedPointerCast<Layout::Shape_Plane>(*it)){
		//   normalVector = plane->getNormalVector();
		//}

		osg::Cylinder* cylinder = new osg::Cylinder;
		cylinder->setCenter (getScaledPosition (center));
		cylinder->setRadius(getScaledDistance(radius));
		cylinder->setHeight(1);

		osg::ShapeDrawable * sd = new osg::ShapeDrawable;
		sd->setShape (cylinder);

        //volovar zmena apha nie je konstanta 0.06f
        sd->setColor (osg::Vec4 (0.0f, 0.f, 1.0f, shape.getAlpha()));

		sd->getOrCreateStateSet()->setMode (GL_BLEND, osg::StateAttribute::ON);
		sd->getStateSet()->setRenderingHint (osg::StateSet::TRANSPARENT_BIN);
		sd->getStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
		sd->getStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		sd->getStateSet()->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON);
		sd->getStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		sd->getStateSet()->setRenderBinDetails( 11, "RenderBin");

		osg::Geode * geode = new osg::Geode;
		geode->addDrawable (sd);

		createdVisualizer_ = geode;

	}

}

void ShapeVisitor_VisualizerCreator::visit (Layout::Shape_Plane & shape) {
	// OSG does not support InfinitePlane drawing at this time, maybe try to use this piece of code with the new version of OSG
	/*
	osg::InfinitePlane * plane = new osg::InfinitePlane;
	plane->set (shape.getNormalVector (), getScaledDistance (shape.getD ()));

	osg::ShapeDrawable * sd = new osg::ShapeDrawable;
	sd->setShape (plane);
	sd->setColor (osg::Vec4 (0, 0.5, 0.0, 0.5));
	*/

	// now we use a temporary code instead - drawing a plane using box:
	if(shape.isInvisible()) {
		createdVisualizer_ = new osg::Group;
		return;
	}

	// center
	osg::Vec3 center (0, 0, (- shape.getD ()) / shape.getNormalVector ().z () ); // some point on the plane

	// rotation
	osg::Quat quat;
	quat.makeRotate (osg::Z_AXIS, shape.getNormalVector ());

	osg::Box * box = new osg::Box;
	box->setCenter (getScaledPosition (center));
	box->setRotation (quat);
	box->setHalfLengths (osg::Vec3 (1000, 1000, 1));

	osg::ShapeDrawable * sd = new osg::ShapeDrawable;
	sd->setShape (box);

    sd->setColor (osg::Vec4 (0.f, 0.f, 1.0f, 0.06f));
	sd->getOrCreateStateSet()->setMode (GL_BLEND, osg::StateAttribute::ON);
	sd->getStateSet()->setRenderingHint (osg::StateSet::TRANSPARENT_BIN);

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

void ShapeVisitor_VisualizerCreator::visit(Layout::Shape_CylinderSurface &shape)
{
	visualizeCylinder(shape);
}

void ShapeVisitor_VisualizerCreator::visit(Layout::Shape_ConeSurface &shape)
{
	visualizeCone(shape);
}

void ShapeVisitor_VisualizerCreator::visit(Layout::Shape_Cube &shape)
{
    visualizeCube(shape);
}

void ShapeVisitor_VisualizerCreator::visualizeSphere (Layout::Shape_AbstractSphere & abstractSphere) {
    osg::Sphere * sphere = new osg::Sphere;
	sphere->setRadius (getScaledDistance (abstractSphere.getRadius ()));
	sphere->setCenter (getScaledPosition (abstractSphere.getCenter ()));

	osg::ShapeDrawable * sd = new osg::ShapeDrawable;
	sd->setShape (sphere);
    //volovar zmena
    osg::PolygonMode * polygonMode = new osg::PolygonMode; //podla http://snipplr.com/view/30978/osg-wireframe-display/

    sd->getOrCreateStateSet()->setMode (GL_BLEND, osg::StateAttribute::ON);
    switch(abstractSphere.getRenderType()) //prepinanie medzi vykreslovacimi modmi shapu
    {
    case Layout::Shape_AbstractSphere::SOLID:

        sd->setColor (osg::Vec4 (0.f, 0.f, 1.0f, abstractSphere.getAlpha()));
        sd->getStateSet()->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON);
        sd->getStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        break;
    case Layout::Shape_AbstractSphere::WIREFRAME:
        polygonMode->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
        sd->setColor (osg::Vec4 (0.f, 0.f, 1.0f, abstractSphere.getAlpha()));
        sd->getStateSet()->setAttributeAndModes(polygonMode,
                                                osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
                                                sd->getStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        break;
    case Layout::Shape_AbstractSphere::CIRCLE:
        break;
        default:
            return;
    }
    //volovar koniec zmeny


	//transparency of sphere

	sd->getStateSet()->setRenderingHint (osg::StateSet::TRANSPARENT_BIN);
	sd->getStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	sd->getStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	sd->getStateSet()->setRenderBinDetails( 11, "RenderBin");
	//

	osg::Geode * geode = new osg::Geode;
	geode->addDrawable (sd);

	createdVisualizer_ = geode;
}

void ShapeVisitor_VisualizerCreator::visualizeCylinder(Layout::Shape_CylinderSurface &cylinder)
{
	osg::Vec3f center = cylinder.secondBaseCenter();

	osg::Cylinder *osgCylinder = new osg::Cylinder;
	osgCylinder->setRadius(getScaledDistance(100));
	osgCylinder->setCenter(getScaledPosition(center));

	osg::ShapeDrawable *sd = new osg::ShapeDrawable;
	sd->setShape(osgCylinder);
	sd->setColor(osg::Vec4 (0, 0, 1.0, /*0.06*/0.0));

	//transparency of cylinder
	sd->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	sd->getStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	sd->getStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	sd->getStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	sd->getStateSet()->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON);
	sd->getStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	sd->getStateSet()->setRenderBinDetails(11, "RenderBin");
	//

	osg::Geode *geode = new osg::Geode;
	geode->addDrawable(sd);

	createdVisualizer_ = geode;
}

void ShapeVisitor_VisualizerCreator::visualizeCone(Layout::Shape_ConeSurface &cone)
{
	osg::Vec3f center = cone.spike();

	osg::Cone *osgCone = new osg::Cone;
	osgCone->setRadius(getScaledDistance(100));
	osgCone->setCenter(getScaledPosition(center));

	osg::ShapeDrawable *sd = new osg::ShapeDrawable;
	sd->setShape(osgCone);
	sd->setColor(osg::Vec4 (0, 0, 1.0, /*0.06*/0.0));

	//transparency of cone
	sd->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	sd->getStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	sd->getStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	sd->getStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	sd->getStateSet()->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON);
	sd->getStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	sd->getStateSet()->setRenderBinDetails(11, "RenderBin");
	//

	osg::Geode *geode = new osg::Geode;
	geode->addDrawable(sd);

	createdVisualizer_ = geode;
}

void ShapeVisitor_VisualizerCreator::visualizeCube(Layout::Shape_AbstractCube &cube)
{
//    if(cube.isInvisible()) {
        createdVisualizer_ = new osg::Group;
//        return;
//    }
//    osg::Box *osgBox = new osg::Box(getScaledPosition(cube.getCenter()), getScaledDistance(cube.getRadius() * 2));

//    osg::ShapeDrawable *sd = new osg::ShapeDrawable;
//    sd->setShape(osgBox);
//    sd->setColor(osg::Vec4 (0, 0, 1.0, 0.06));

//    //transparency of cube
//    sd->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
//    sd->getStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
//    sd->getStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
//    sd->getStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
//    sd->getStateSet()->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON);
//    sd->getStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
//    sd->getStateSet()->setRenderBinDetails(11, "RenderBin");
//    //

//    osg::Geode *geode = new osg::Geode;
//    geode->addDrawable(sd);

//    createdVisualizer_ = geode;
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
