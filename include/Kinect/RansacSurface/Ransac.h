#ifndef RANSAC_H
#define RANSAC_H

namespace Kinect {

class PlaneAlgorithm;

class Ransac{


public:
	Ransac();
	~Ransac();

	void calculate();
	void findPoints(int *vectorDepths);

private:
	PlaneAlgorithm *calc;


};
}
#endif // RANSAC_H
