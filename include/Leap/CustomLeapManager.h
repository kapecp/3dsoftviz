#ifndef CUSTOMLEAPMANAGER_H
#define CUSTOMLEAPMANAGER_H

#include "LeapLib/LeapManager.h"
#include "Viewer/CameraManipulator.h"
#include "Viewer/CoreGraph.h"
#include "Layout/LayoutThread.h"
#include "Layout/FRAlgorithm.h"
#include <osg/ref_ptr>
#include <osg/Transform>
#include <osg/Matrix>
#include <osg/MatrixTransform>

namespace Leap {

class CustomLeapManager : public LeapManager
{
public:
	CustomLeapManager( Vwr::CameraManipulator* cameraManipulator,Layout::LayoutThread* layout,
					   Vwr::CoreGraph* coreGraph, osg::ref_ptr<osg::Group> handsGroup = NULL );
	~CustomLeapManager( void );

	void enableCameraMovement( Movement direction );
	void disableCameraMovement();
	void rotateCamera( float py0, float px0, double throwScale, float py1, float px1 );
	void updateHands( float lx,float ly, float lz, float rx, float ry, float rz );

	//jurik
	void graphRotateSwipe( int swipeDirection );
	void scaleNodes( bool scaleUp );
	void rotateArucoLeft();
	void rotateArucoRight();
	void scaleEdgesUp();
	void scaleEdgesDown();
	//*****


	Vwr::CameraManipulator* cameraManipulator;
	Layout::LayoutThread* layout;
	Vwr::CoreGraph* coreGraph;

	osg::ref_ptr<osg::Group> handsGroup;

	osg::ref_ptr<osg::MatrixTransform> transformLeft;
	osg::ref_ptr<osg::MatrixTransform> transformRight;
};
}
#endif
