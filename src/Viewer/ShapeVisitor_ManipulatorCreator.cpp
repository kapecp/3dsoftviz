#include "Viewer/ShapeVisitor_ManipulatorCreator.h"


namespace Vwr {


void ShapeVisitor_ManipulatorCreator::visit( Layout::Shape_Null /*& shape*/ )
{
	executeDefault();
}

void ShapeVisitor_ManipulatorCreator::visit( Layout::Shape_Composite& shape )
{
	executeDefault();
}

void ShapeVisitor_ManipulatorCreator::visit( Layout::Shape_Intersection& shape )
{
	executeDefault();
}


void ShapeVisitor_ManipulatorCreator::visit( Layout::Shape_Plane& shape )
{
	//if (type == ManipulatorType::TRACKBALLDRAGGER){
	//    osg::Vec3 center (0, 0, (- shape.getD ()) / shape.getNormalVector ().z () );
	//    createRotationManipulator(center,100,'a');
	// }else{
	executeDefault();
	// }
}

void ShapeVisitor_ManipulatorCreator::visit( Layout::Shape_Sphere& shape )
{
	executeDefault();
}

void ShapeVisitor_ManipulatorCreator::visit( Layout::Shape_SphereSurface& shape )
{
	executeDefault();
}

void ShapeVisitor_ManipulatorCreator::visit( Layout::Shape_ConeSurface& shape )
{
	executeDefault();
}

void ShapeVisitor_ManipulatorCreator::visit( Layout::Shape_CylinderSurface& shape )
{
	executeDefault();
}

void ShapeVisitor_ManipulatorCreator::visit( Layout::Shape_Cube& shape )
{
	executeDefault();
}

void ShapeVisitor_ManipulatorCreator::executeDefault()
{
	createdDragger = 0;
	createdSelection = 0;
}

void ShapeVisitor_ManipulatorCreator::createRotationManipulator( osg::Vec3 center,float trackBallRadius, int key )
{
	createdDragger = new osgManipulator::TrackballDragger();
	createdDragger->setupDefaultGeometry();

	double scale =static_cast<double>( trackBallRadius ) * 1.6;
	createdDragger->setMatrix( osg::Matrix::scale( scale, scale, scale ) *osg::Matrix::translate( center ) );
	createdDragger->setHandleEvents( true );
	createdDragger->setActivationModKeyMask( osgGA::GUIEventAdapter::MODKEY_CTRL );
	createdDragger->setActivationKeyEvent( key );

	createdSelection = new osg::MatrixTransform;
	createdDragger->addTransformUpdating( createdSelection );
}

}
