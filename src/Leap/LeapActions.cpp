#include "Leap/LeapActions.h"

Leap::LeapActions::LeapActions(){
    mouse = new Vwr::MouseControl();
    zoomCounter = 0;
    qDebug() << "LeapActions() Constructor";
}

void Leap::LeapActions::onSwipe(Gesture gesture)
{
    // swipe gesture
    SwipeGesture swipe = gesture;
    Vector direction = swipe.direction();
    // najprv porovnam direction[0] a direction[1], aby som zistil o aky pohyb sa jedna

    if(abs(direction[0]) > abs(direction[1])){ // ak plati toto jedna sa o horizontalny pohyb
        if(direction[0] > 0)
            qDebug() << "SwipeGesture - right";
        else if(direction[0] < 0)
            qDebug() << "SwipeGesture - left";
    }else{ // inak sa jedna o vertikalny pohyb
        if(direction[1] > 0)
            qDebug() << "SwipeGesture - up";
        else if(direction[1] < 0)
            qDebug() << "SwipeGesture - down";
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
    // key tap gesture
    qDebug() << "KeyTapGesture";
}

void Leap::LeapActions::onScreenTap(Gesture gesture)
{
    // screen tap gesture
    qDebug() << "ScreenTapGesture";
}
