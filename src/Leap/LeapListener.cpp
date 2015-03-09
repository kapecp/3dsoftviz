#include "Leap/LeapListener.h"

void Leap::LeapListener::onInit(const Controller& controller)
{

}

void Leap::LeapListener::onConnect(const Controller& controller)
{
  // we put our gestures here to initialize them
  controller.enableGesture(Gesture::TYPE_CIRCLE);
  controller.enableGesture(Gesture::TYPE_KEY_TAP);
  controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
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
    //qDebug() << "[Leap::LeapController::onFrame()]";
    Frame frame = controller.frame();
    //HandList hands = frame.hands();
    //std::cout << "id: " << frame.id();

    const GestureList gestures = frame.gestures();
      for (int g = 0; g < gestures.count(); ++g) {
        Gesture gesture = gestures[g];

        switch (gesture.type()) {
          case Gesture::TYPE_CIRCLE:
          {
            CircleGesture circle = gesture;

            if (circle.pointable().direction().angleTo(circle.normal()) <= PI/2) {
              qDebug() << "[onFrame()::CircleGesture - clockwise]";
            } else {
                qDebug() << "[onFrame()::CircleGesture - counterclockwise]";
            }
            break;
          }
          case Gesture::TYPE_SWIPE:
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
            break;
          }
          case Gesture::TYPE_KEY_TAP:
          {
            // key tap gesture
            qDebug() << "KeyTapGesture";
            break;
          }
          case Gesture::TYPE_SCREEN_TAP:
          {
            // screen tap gesture
            qDebug() << "ScreenTapGesture";
            break;
          }
          default:
            qDebug() << "Unknown gesture type.";
            break;
        }
      }
}

void Leap::LeapListener::onFocusGained(const Controller& controller)
{

}

void Leap::LeapListener::onFocusLost(const Controller& controller)
{

}

void Leap::LeapListener::onDeviceChange(const Controller& controller)
{
    const DeviceList devices = controller.devices();

    for (int i = 0; i < devices.count(); ++i) {
        std::cout << "id: " << devices[i].toString() << std::endl;
        std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
    }

}

void Leap::LeapListener::onServiceConnect(const Controller& controller)
{

}

void Leap::LeapListener::onServiceDisconnect(const Controller& controller)
{

}
