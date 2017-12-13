#ifndef DIRECTIONDETECTOR_H
#define DIRECTIONDETECTOR_H
#include "LeapLib/LeapExport.h"

#include "Leap.h"

namespace LeapLib {

/**
 * @brief The DirectionDetector class
 * Detector containing methods that define the direction the palm is facing.
 */
class LEAPLIB_EXPORT DirectionDetector
{
public:
	/**
	 * @brief The Direction enum
	 * 5 possible states of palm position.
	 */
	enum Direction {UP,DOWN,LEFT,RIGHT,STEADY};
	/**
	 * @brief getDirection
	 * @param hand - hand object from leap sensor that we want to get direction of.
	 * @return one member of the direction enum
	 */
	static Direction getPalmDirection( Leap::Hand hand );
private:
	/**
	 * @brief ANGLE_LIMIT
	 * Helps to identify when the palm is in a specific position.
	 */
	static const int ANGLE_LIMIT = 15;
	/**
	 * @brief getPitch
	 * @param hand - hand object from leap sensor that we want to get pitch of.
	 * @return returns the pitch value in degrees, can be either positive or negative
	 */
	static float getPitch( Leap::Hand hand );
	/**
	 * @brief getRoll
	 * @param hand - hand object from leap sensor that we want to get roll of.
	 * @return returns the roll value in degrees, can be either positive or negative
	 */
	static float getRoll( Leap::Hand hand );
};

}

#endif // DIRECTIONDETECTOR_H
