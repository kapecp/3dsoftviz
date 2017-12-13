#include "LeapLib/DirectionDetector.h" // IWYU pragma: associated
// IWYU pragma: no_include <Leap.h>
// IWYU pragma: no_include <LeapMath.h>
// IWYU pragma: no_include <cmath>


float LeapLib::DirectionDetector::getPitch( Leap::Hand hand )
{
	return hand.direction().pitch() * Leap::RAD_TO_DEG;
}

float LeapLib::DirectionDetector::getRoll( Leap::Hand hand )
{
	return hand.palmNormal().roll() * Leap::RAD_TO_DEG;
}

LeapLib::DirectionDetector::Direction LeapLib::DirectionDetector::getPalmDirection( Leap::Hand hand )
{
	float pitch = getPitch( hand );
	float roll = getRoll( hand );

	float absRoll = std::abs( roll );

	if ( absRoll < LeapLib::DirectionDetector::ANGLE_LIMIT ) {
		if ( pitch > LeapLib::DirectionDetector::ANGLE_LIMIT ) {
			return LeapLib::DirectionDetector::Direction::UP;
		}
		if ( -pitch > LeapLib::DirectionDetector::ANGLE_LIMIT ) {
			return LeapLib::DirectionDetector::Direction::DOWN;
		}
	}

	float absPitch = std::abs( pitch );

	if ( absPitch < LeapLib::DirectionDetector::ANGLE_LIMIT ) {
		if ( roll > LeapLib::DirectionDetector::ANGLE_LIMIT ) {
			return LeapLib::DirectionDetector::Direction::RIGHT;
		}
		if ( -roll > LeapLib::DirectionDetector::ANGLE_LIMIT ) {
			return LeapLib::DirectionDetector::Direction::LEFT;
		}
	}

	return LeapLib::DirectionDetector::Direction::STEADY;
}
