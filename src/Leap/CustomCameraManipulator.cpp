#include "Leap/CustomCameraManipulator.h"

Leap::CustomCameraManipulator::CustomCameraManipulator(Vwr::CameraManipulator* cameraManipulator, Layout::LayoutThread* layout,
		Vwr::CoreGraph* coreGraph , osg::ref_ptr<osg::Group> handsGroup)
{
	this->cameraManipulator = cameraManipulator;
	this->layout = layout;
	this->coreGraph = coreGraph;
	this->handsGroup = handsGroup;
	arMode = false;

	//init hand nodes here
	if (this->handsGroup != NULL){
		arMode = true;

		osg::ref_ptr<osg::Geode> leftHandGeode (new osg::Geode);
		osg::ref_ptr<osg::Sphere>  leftHandSphere = new osg::Sphere(osg::Vec3f(0,0,0),0.1);

		osg::ref_ptr<osg::ShapeDrawable> leftHandDrawable (new osg::ShapeDrawable(leftHandSphere.get()));
		leftHandGeode->addDrawable(leftHandDrawable.get());

		osg::ref_ptr<osg::Geode> rightHandGeode (new osg::Geode);
		osg::ref_ptr<osg::Sphere> rightHandSphere = new osg::Sphere(osg::Vec3f(0,0,0),0.1);
		osg::ref_ptr<osg::ShapeDrawable> rightHandDrawable (new osg::ShapeDrawable(rightHandSphere.get()));
		rightHandGeode->addDrawable(rightHandDrawable.get());

		transformLeft = new osg::MatrixTransform();
		transformRight = new osg::MatrixTransform();
		transformLeft->addChild(leftHandGeode.get());
		transformRight->addChild(rightHandGeode.get());

		transformLeft->setMatrix(osg::Matrix::translate(-0.5,0,-0.5));
		transformRight->setMatrix(osg::Matrix::translate(0.5,0,-0.5));

		handsGroup->insertChild(0,transformLeft.get());
		handsGroup->insertChild(1,transformRight.get());
	}
}

Leap::CustomCameraManipulator::~CustomCameraManipulator( void )
{
	//remove hands nodes here
	if (handsGroup){
		handsGroup->removeChildren(0,handsGroup->getNumChildren());
	}
}

void Leap::CustomCameraManipulator::enableCameraMovement( Movement direction )
{
	this->cameraManipulator->enableCameraMovement( static_cast<Vwr::CameraManipulator::Movement>( direction ) );
}

void Leap::CustomCameraManipulator::disableCameraMovement()
{
	this->cameraManipulator->disableCameraMovement();
}

void Leap::CustomCameraManipulator::rotateCamera( float py0, float px0, double throwScale, float py1, float px1 )
{
	this->cameraManipulator->rotateCamera( py0,px0,throwScale,py1, px1 );
}

//jurik
void Leap::CustomCameraManipulator::graphRotateSwipe( int swipeDirection )
{
	//direction -1 = LEFT, 1 = RIGHT
	switch ( swipeDirection ) {
		case -1: {
			coreGraph->rotateGraph( swipeDirection );
		}
		case 1: {
			coreGraph->rotateGraph( swipeDirection );
		}
	}
}

void Leap::CustomCameraManipulator::rotateArucoLeft()
{
	coreGraph->rotateGraph( 1 );
}

void Leap::CustomCameraManipulator::rotateArucoRight()
{
	coreGraph->rotateGraph( -1 );
}

void Leap::CustomCameraManipulator::scaleEdgesUp()
{

	float distance = layout->getAlg()->getMaxDistance();

	layout->pause();
	coreGraph->setNodesFreezed( true );
	layout->getAlg()->setMaxDistance( distance * 1.02 );
	coreGraph->scaleGraphToBase();
	coreGraph->setNodesFreezed( false );
	layout->play();
}

void Leap::CustomCameraManipulator::scaleEdgesDown()
{

	float distance = layout->getAlg()->getMaxDistance();

	layout->pause();
	coreGraph->setNodesFreezed( true );
	layout->getAlg()->setMaxDistance( distance * 0.98 );
	coreGraph->scaleGraphToBase();
	coreGraph->setNodesFreezed( false );
	layout->play();
}

void Leap::CustomCameraManipulator::scaleNodes( bool scaleUp )
{
	if ( scaleUp ) {
		coreGraph->scaleNodes( true );
	}
	else {
		coreGraph->scaleNodes( false );
	}
}

//*****
void Leap::CustomCameraManipulator::updateHands(float lx,float ly, float lz, float rx, float ry, float rz){
	if (this->handsGroup != NULL){
		LOG(INFO) << "updateHands vals left: "<< lx/200.0f<< " " << ly/200.0f<< " " << lz/200.0f;
		LOG(INFO) << "updateHands vals right: "<< rx/200.0f<< " " << ry/200.0f<< " " << rz/200.0f;
		transformLeft->setMatrix(osg::Matrix::translate(lx/100.0f,-lz/100.0f,ly/100.0f));
		transformRight->setMatrix(osg::Matrix::translate(rx/100.0f,-rz/100.0f,ry/100.0f));
	}
}
