#ifndef VECTOR3_H
#define VECTOR3_H

namespace Kinect{

class Vector3{

public:

	Vector3();
	Vector3(int mX,int mY,int mZ);
	~Vector3();

	Vector3 subtract(Vector3 first,Vector3 second);

	int mX,mY,mZ;

};
}

#endif // VECTOR3_H
