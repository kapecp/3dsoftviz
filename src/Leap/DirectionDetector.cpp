#include "Leap/DirectionDetector.h"
#include "LeapMath.h"

float Leap::DirectionDetector::getPitch( Hand hand )
{
	return hand.direction().pitch() * RAD_TO_DEG;
}

float Leap::DirectionDetector::getRoll( Hand hand )
{
	return hand.palmNormal().roll() * RAD_TO_DEG;
}

bool Leap::DirectionDetector::isSteady( float pitch, float roll )
{

	if ( abs( pitch ) < Leap::DirectionDetector::ANGLE_LIMIT && abs( roll ) < Leap::DirectionDetector::ANGLE_LIMIT ) {
		return true;
	}

	return false;
}

bool Leap::DirectionDetector::isUp( float pitch, float roll )
{

	if ( pitch > Leap::DirectionDetector::ANGLE_LIMIT && abs( roll ) < Leap::DirectionDetector::ANGLE_LIMIT ) {
		return true;
	}

	return false;
}

bool Leap::DirectionDetector::isDown( float pitch, float roll )
{

	if ( pitch < -Leap::DirectionDetector::ANGLE_LIMIT && abs( roll ) < Leap::DirectionDetector::ANGLE_LIMIT ) {
		return true;
	}

	return false;
}

bool Leap::DirectionDetector::isRight( float pitch, float roll )
{

	if ( abs( pitch ) < Leap::DirectionDetector::ANGLE_LIMIT && roll > Leap::DirectionDetector::ANGLE_LIMIT ) {
		return true;
	}

	return false;
}

bool Leap::DirectionDetector::isLeft( float pitch, float roll )
{

	if ( abs( pitch ) < Leap::DirectionDetector::ANGLE_LIMIT && roll < -Leap::DirectionDetector::ANGLE_LIMIT ) {
		return true;
	}

	return false;
}

Leap::DirectionDetector::Direction Leap::DirectionDetector::getPalmDirection( Hand hand )
{

	float pitch = getPitch( hand );
	float roll = getRoll( hand );

	if ( isUp( pitch,roll ) ) {
		return Leap::DirectionDetector::Direction::UP;
	}
	else if ( isDown( pitch,roll ) ) {
		return Leap::DirectionDetector::Direction::DOWN;
	}
	else if ( isRight( pitch,roll ) ) {
		return Leap::DirectionDetector::Direction::RIGHT;
	}
	else if ( isLeft( pitch,roll ) ) {
		return Leap::DirectionDetector::Direction::LEFT;
	}
	else {
		return Leap::DirectionDetector::Direction::STEADY;
	}
}
