#ifndef PLANEALGORITHM_H
#define PLANEALGORITHM_H

#include <fstream>

namespace Kinect {

/**
 * @brief The Vector3 struct
 * @brief data structure for 3D space in Int
 */
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

/**
 * @author Martina
 * @brief The PlaneAlgorithm class
 * @brief algoritm class for calculate
 */
class PlaneAlgorithm{

public:
	PlaneAlgorithm();
	~PlaneAlgorithm();

	/**
	 * @brief setRandomPoint
	 * @param i position in vector
	 * @param x x coordinate
	 * @param y y coordinate
	 * @param z z coordinate
	 * @return 0 if set, else 1
	 */
	int setRandomPoint(int i,int x, int y, int z);

	/**
	 * @brief findIntersection
	 * @param x coordinate
	 * @param y coordinate
	 * @param z coordinate
	 * @return if find intersection 0, else 1
	 */
	int findIntersection(int x, int y, int z);

	/**
	 * @brief calculate Plane equation
	 * @return 0 if find, else 1
	 */
	int calculatePlane();

	/**
	 * @brief logfile for equation
	 */
	std::ofstream logfile;

private:

	/**
	 * @brief subtract Vector3 - Vector3
	 * @param first Vector3
	 * @param second Vector3
	 * @return Vector3
	 */
	Vector3 subtract(Vector3 first, Vector3 second);

	Vector3 points[3];
	Vector3 w;
	int d;
};
}
#endif // PLANEALGORITHM_H
