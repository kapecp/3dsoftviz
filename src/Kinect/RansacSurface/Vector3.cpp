#include "Kinect/RansacSurface/Vector3.h"


Kinect::Vector3::Vector3()
{
}

Kinect::Vector3::Vector3(int x, int y, int z)
{
	this->mX=x;
	this->mY=y;
	this->mZ=z;
}

Kinect::Vector3::~Vector3()
{
}


Kinect::Vector3 Kinect::Vector3::subtract(Vector3 first, Vector3 second)
{
	return Vector3(first.mX-second.mX,first.mY-second.mY,first.mZ-second.mZ);
}
