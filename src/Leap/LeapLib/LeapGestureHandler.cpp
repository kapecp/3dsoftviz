#include "LeapLib//LeapGestureHandler.h"
#include "LeapLib/DirectionDetector.h"
#include "LeapLib/LeapActions.h"
#include "osg/Image"
#include "CustomLeapManager.h"

//#include "osg/Image"
#include "osgDB/WriteFile"
#include <stdio.h>

Leap::LeapGestureHandler::LeapGestureHandler( LeapManager* leapManager )
{
    leapActions = new Leap::LeapActions( leapManager );
    //this->arMode = leapManager->arMode;
    LOG( INFO ) << "Leap/LeapLib/LeapGestureHandler Constructor";
}

Leap::LeapGestureHandler::~LeapGestureHandler( void )
{
    if ( leapActions != NULL ) {
        delete( leapActions );
    }
}

void Leap::LeapGestureHandler::processGestures(Frame frame){
    LOG(INFO) << "MELISKO TO JE";
}


