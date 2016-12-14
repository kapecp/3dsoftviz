#include "Util/SizedQueue.h"

#include <math.h>
#include <utility>

namespace Util {

SizedQueue::SizedQueue( unsigned int size, float threshold )
{
	this->size = size;
	this->threshold = threshold;
	this->sum.first = 0.0f;
	this->sum.second = 0.0f;
	this->presum.first = 0.0f;
	this->presum.second = 0.0f;

	positive = 0;
}

SizedQueue::~SizedQueue()
{
	this->stack.clear();
	this->boolStack.clear();
}

int SizedQueue::getAvgBasedOnValue( float& x, float& y )
{
	// push/pop, LIFO stack
	if ( this->stack.size() >= this->size ) {

		std::pair<float,float> p;
		p = this->stack.front();
		this->sum.first -= p.first;
		this->sum.second -= p.second;
		this->stack.pop_front();
	}

	this->stack.push_back( std::pair<float,float>( x,y ) );
	this->sum.first += x;
	this->sum.second += y;

	// threshold implementation
	int retval=0;

	if ( fabsf( ( this->presum.first-this->sum.first ) /  static_cast<float>( this->stack.size() ) ) > this->threshold ) {
		this->presum.first=this->sum.first;
		retval=1;
	}
	if ( fabsf( ( this->presum.second-this->sum.second ) / static_cast<float>( this->stack.size() ) ) > this->threshold ) {
		this->presum.second=this->sum.second;
		retval=1;
	}
	x = this->presum.first / static_cast<float>( this->stack.size() );
	y = this->presum.second / static_cast<float>( this->stack.size() );

	return retval;
}

int SizedQueue::getAvgBasedOnValue( float& x )
{
	// push/pop, LIFO stack
	if ( this->stack.size() >= this->size ) {

		std::pair<float,float> p;
		p = this->stack.front();
		this->sum.first -= p.first;
		this->stack.pop_front();
	}

	this->stack.push_back( std::pair<float,float>( x,0.0f ) );
	this->sum.first += x;

	// threshold implementation
	int retval=0;

	if ( fabsf( ( this->presum.first-this->sum.first ) / static_cast<float>( this->stack.size() ) ) > this->threshold ) {
		this->presum.first=this->sum.first;
		retval=1;
	}
	x = this->presum.first / static_cast<float>( this->stack.size() );

	return retval;
}

bool SizedQueue::getAvgBool( bool y )
{
	// push/pop, LIFO stack
	if ( this->boolStack.size() >= this->size ) {
		bool front = boolStack.front();
		boolStack.pop_front();
		if ( front == true ) {
			positive--;
		}
	}
	boolStack.push_back( y );
	if ( y ) {
		positive++;
	}

	return positive*2 > static_cast<int>( boolStack.size() );

}

} // namespace Util
