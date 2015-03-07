#include "Leap/LeapListener.h"

void Leap::LeapListener::onInit(const Controller& controller)
{

}

void Leap::LeapListener::onConnect(const Controller& controller)
{
  // we put our gestures here to initialize them
  controller.enableGesture(Gesture::TYPE_SWIPE);
}

void Leap::LeapListener::onDisconnect(const Controller& controller)
{

}

void Leap::LeapListener::onExit(const Controller& controller)
{

}

void Leap::LeapListener::onFrame(const Controller& controller)
{
    // here we need to put our code
        qDebug() << "[Leap::LeapController::onFrame()]";
}

void Leap::LeapListener::onFocusGained(const Controller& controller)
{

}

void Leap::LeapListener::onFocusLost(const Controller& controller)
{

}

void Leap::LeapListener::onDeviceChange(const Controller& controller)
{

}

void Leap::LeapListener::onServiceConnect(const Controller& controller)
{

}

void Leap::LeapListener::onServiceDisconnect(const Controller& controller)
{

}
