#include "Leap/LeapActions.h"
#include "Core/Core.h"

Leap::LeapActions::LeapActions(){
    mouse = new Vwr::MouseControl();
    zoomCounter = 0;
    cmrManipulator = AppCore::Core::getInstance( NULL )->getCoreWindow()->getCameraManipulator();
    isCameraMoving = true;
    qDebug() << "LeapActions() Constructor";
}

void Leap::LeapActions::moveCamera(Gesture gesture)
{
    // swipe gesture
    SwipeGesture swipe = gesture;
    Vector direction = swipe.direction();

    float gestureDuration = gesture.duration()/1000;
    // compare direction[0] and direction[1] to find out what kind of movement are we working with
    if(abs(direction[0]) > abs(direction[1])){ // horizontal movement
        if(direction[0] > 0){
            cmrManipulator->enableCameraMovement(Vwr::CameraManipulator::Movement::RIGHT);
            Sleep(gestureDuration);
            cmrManipulator->disableCameraMovement();
            qDebug() << "SwipeGesture - right";
        }else if(direction[0] < 0){
            cmrManipulator->enableCameraMovement(Vwr::CameraManipulator::Movement::LEFT);
            Sleep(gestureDuration);
            cmrManipulator->disableCameraMovement();
            qDebug() << "SwipeGesture - left";
        }
    }else{ // vertical movement
        if(direction[1] > 0){
            cmrManipulator->enableCameraMovement(Vwr::CameraManipulator::UP);
            Sleep(gestureDuration);
            cmrManipulator->disableCameraMovement();
            qDebug() << "SwipeGesture - up";
        }
        else if(direction[1] < 0){
            cmrManipulator->enableCameraMovement(Vwr::CameraManipulator::DOWN);
            Sleep(gestureDuration);
            cmrManipulator->disableCameraMovement();
            qDebug() << "SwipeGesture - down";
        }
    }
}

void Leap::LeapActions::zoomGraph(Gesture gesture)
{
    CircleGesture circle = gesture;

    if(zoomCounter++ == ZOOM_READY){
        zoomCounter = 0;

        if (circle.pointable().direction().angleTo(circle.normal()) <= PI/2)
        {
            qDebug() << "[onFrame()::CircleGesture - clockwise]";
            mouse->scrollMouse(osgGA::GUIEventAdapter::SCROLL_DOWN);
        } else {
            qDebug() << "[onFrame()::CircleGesture - counterclockwise]";
            mouse->scrollMouse(osgGA::GUIEventAdapter::SCROLL_UP);
        }

    }
}

void Leap::LeapActions::onKeyTap(Gesture gesture)
{
    if(isCameraMoving)
        isCameraMoving = false;
    else
        isCameraMoving = true;
    qDebug() << "KeyTapGesture";
}

void Leap::LeapActions::onScreenTap(Gesture gesture)
{
    // screen tap gesture
    qDebug() << "ScreenTapGesture";
}

void Leap::LeapActions::rotateGraph(Gesture gesture){

    // swipe gesture
    SwipeGesture swipe = gesture;
    Vector direction = swipe.direction();

    if(abs(direction[0]) > abs(direction[1])){ // horizontal movement
        if(direction[0] > 0){
              cmrManipulator->rotateCamera(0, 0, 1, 0, 0.05);
        }else if(direction[0] < 0){
              cmrManipulator->rotateCamera(0, 0, 1, 0, -0.05);
        }
    }else{ // vertical movement
        if(direction[1] > 0){
               cmrManipulator->rotateCamera(0, 0 ,1, 0.05, 0);
        }
        else if(direction[1] < 0){
             cmrManipulator->rotateCamera(0, 0, 1, -0.05, 0);
        }
    }
}
