#ifndef DIRECTIONDETECTOR_H
#define DIRECTIONDETECTOR_H

#include "Leap.h"

namespace Leap {

/**
 * @brief The DirectionDetector class
 * Detector containing methods that define the direction the palm is facing.
 */
class DirectionDetector
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
	static Direction getPalmDirection( Hand hand );
private:
	/**
	 * @brief ANGLE_LIMIT
	 * Helps to identify when the palm is in a specific position.
	 */
	static const int ANGLE_LIMIT = 15;
	/**
	 * @brief getPitch
	 * @param hand - hand object from leap sensor that we want to get pitch of.
	 * @return returns the pitch value, can be either positive or negative
	 */
	static float getPitch( Hand hand );
	/**
	 * @brief getRoll
	 * @param hand - hand object from leap sensor that we want to get roll of.
	 * @return returns the roll value, can be either positive or negative
	 */
	static float getRoll( Hand hand );
	/**
	 * @brief isSteady
	 * @param pitch
	 * @param roll
	 * @return true if the hand is in a steady position
	 */
	static bool isSteady( float pitch, float roll );
	/**
	 * @brief isUp
	 * @param pitch
	 * @param roll
	 * @return true if the palm is facing up
	 */
	static bool isUp( float pitch, float roll );
	/**
	 * @brief isDown
	 * @param pitch
	 * @param roll
	 * @return true if the palm is facing down
	 */
	static bool isDown( float pitch, float roll );
	/**
	 * @brief isRight
	 * @param pitch
	 * @param roll
	 * @return true if the palm is facing to the right
	 */
	static bool isRight( float pitch, float roll );
	/**
	 * @brief isLeft
	 * @param pitch
	 * @param roll
	 * @return true if the palm is facing to the left
	 */
	static bool isLeft( float pitch, float roll );
};

}

#endif // DIRECTIONDETECTOR_H
