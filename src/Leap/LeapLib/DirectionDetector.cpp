#include "LeapLib/DirectionDetector.h" // IWYU pragma: associated
// IWYU pragma: no_include <Leap.h>
// IWYU pragma: no_include <LeapMath.h>
// IWYU pragma: no_include <cmath>


float Leap::DirectionDetector::getPitch( Hand hand )
{
	return hand.direction().pitch() * RAD_TO_DEG;
}

float Leap::DirectionDetector::getRoll( Hand hand )
{
	return hand.palmNormal().roll() * RAD_TO_DEG;
}

Leap::DirectionDetector::Direction Leap::DirectionDetector::getPalmDirection( Hand hand )
{
	float pitch = getPitch( hand );
	float roll = getRoll( hand );

	float absRoll = std::abs( roll );

	if ( absRoll < Leap::DirectionDetector::ANGLE_LIMIT ) {
		if ( pitch > Leap::DirectionDetector::ANGLE_LIMIT ) {
			return Leap::DirectionDetector::Direction::UP;
		}
		if ( -pitch > Leap::DirectionDetector::ANGLE_LIMIT ) {
			return Leap::DirectionDetector::Direction::DOWN;
		}
	}

	float absPitch = std::abs( pitch );

	if ( absPitch < Leap::DirectionDetector::ANGLE_LIMIT ) {
		if ( roll > Leap::DirectionDetector::ANGLE_LIMIT ) {
			return Leap::DirectionDetector::Direction::RIGHT;
		}
		if ( -roll > Leap::DirectionDetector::ANGLE_LIMIT ) {
			return Leap::DirectionDetector::Direction::LEFT;
		}
	}

	return Leap::DirectionDetector::Direction::STEADY;
}
