#ifndef PLANEALGORITHM_H
#define PLANEALGORITHM_H

#include <fstream>
#include "Kinect/RansacSurface/Vector3.h"

namespace Kinect {

//class Vector3;

class PlaneAlgorithm{

public:
	PlaneAlgorithm();
	~PlaneAlgorithm();

	int setRandomPoint(int i,int x, int y, int z);
	int findIntersection(int x, int y, int z);
	int calculatePlane();
	std::ofstream logfile;

private:

	Vector3 points[3];
	Vector3 w;
	int d;
};
}
#endif // PLANEALGORITHM_H
