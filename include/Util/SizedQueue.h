#ifndef SIZEDQUEUE_H
#define SIZEDQUEUE_H

#include <deque>
#include <utility>

namespace Util {

/**
	*@brief Class FaceRecognizer
	*@author Autor: Marek Jakab
	*@date 20.5.2014
	*/

class SizedQueue
{

public:

	SizedQueue( unsigned int size, float threshold );
	~SizedQueue();

	/**
	 * @brief getAvgBasedOnValue Compute average value from N previous values
	 * @author Autor: Marek Jakab
	 * @param x float value on current x axis (will be rewriten)
	 * @param y float value on current y axis (will be rewriten)
	 * @return 0/1 if the computed value changed over by set threshold
	 */
	int getAvgBasedOnValue( float& x, float& y );

	/**
	 * @brief getAvgBasedOnValue Compute average value from N previous values
	 * @author Autor: Marek Jakab
	 * @param x float current value (will be rewriten)
	 * @return 0/1 if te computed value changed over by set threshold
	 */
	int getAvgBasedOnValue( float& x );

	/**
	 * @brief getAvgBool add y to stack, and return true if there are nore true values then false values
	 * @author Autor: David Durcak
	 * @param y value that will be added to stack
	 * @return true if there are nore true values then false values in stack
	 */
	bool getAvgBool( bool y );

private:
	std::deque< std::pair<float,float> > stack;
	// size of queue
	unsigned int size;
	// threshold <0.0,1.0>
	float threshold;
	// current sum for values in queue
	std::pair<float,float> sum;
	// sum considering threshold
	std::pair<float,float> presum;

	// bool queue
	std::deque< bool > boolStack;
	int positive;
};
}
#endif // SIZEDQUEUE_H
