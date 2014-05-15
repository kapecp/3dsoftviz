#ifndef PLANEALGORITHM_H
#define PLANEALGORITHM_H

#include <fstream>

namespace Kinect {

struct Vector3
{
	int mX, mY, mZ;
	Vector3(){}
	Vector3(int mX,int mY,int mZ){
		this->mX=mX;
		this->mY=mY;
		this->mZ=mZ;
	}
};

class PlaneAlgorithm{

public:
	PlaneAlgorithm();
	~PlaneAlgorithm();

	int setRandomPoint(int i,int x, int y, int z);
	int findIntersection(int x, int y, int z);
	int calculatePlane();
	std::ofstream logfile;

private:

	Vector3 subtract(Vector3 first, Vector3 second);

	Vector3 points[3];
	Vector3 w;
	int d;
};
}
#endif // PLANEALGORITHM_H
