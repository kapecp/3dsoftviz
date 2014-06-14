#ifndef RANSAC_H
#define RANSAC_H

namespace Kinect {

class PlaneAlgorithm;

/**
 * @author Martina
 * @brief The Ransac class
 * @brief class for calculate Ransac
 */
class Ransac{


public:
	Ransac();
	~Ransac();

	/**
	 * @brief read data from file and them findPoints();
	 */
	void calculate();

	/**
	 * @brief findPoints
	 * @brief find Points from vector of depths
	 * @param vectorDepths
	 */
	void findPoints(int *vectorDepths);

private:
	// algoritm for calculate
	PlaneAlgorithm *calc;


};
}
#endif // RANSAC_H
