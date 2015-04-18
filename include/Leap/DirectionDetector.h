#ifndef DIRECTIONDETECTOR_H
#define DIRECTIONDETECTOR_H

#include "Leap.h"

namespace Leap {

class DirectionDetector
{
public:
    enum Direction {UP,DOWN,LEFT,RIGHT,STEADY};
    static Direction getDirection(Hand hand);
private:
    static const int ANGLE_LIMIT = 15;
    static float getPitch(Hand hand);
    static float getRoll(Hand hand);
    static bool isSteady(float pitch, float roll);
    static bool isUp(float pitch, float roll);
    static bool isDown(float pitch, float roll);
    static bool isRight(float pitch, float roll);
    static bool isLeft(float pitch, float roll);
};

}

#endif // DIRECTIONDETECTOR_H
