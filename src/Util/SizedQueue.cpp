#include "Util/SizedQueue.h"

SizedQueue::SizedQueue(int size, float threshold)
{
	this->size=size;
	this->threshold=threshold;
	this->sum.first=0.0f;
	this->sum.second=0.0f;
	this->presum.first=0.0f;
	this->presum.second=0.0f;
}

SizedQueue::~SizedQueue()
{
	this->stack.clear();
}

int SizedQueue::getAvgBasedOnValue(std::pair<float, float> &value)
{
	// push/pop, LIFO stack
	if (this->stack.size()<this->size){
		this->stack.push_back(value);
		this->sum.first+=value.first;
		this->sum.second+=value.second;
	} else {
		pair<float,float> p;
		p=this->stack.front();
		this->sum.first-= p.first;
		this->sum.second-= p.second;
		this->stack.pop_front();
		this->stack.push_back(value);
		this->sum.first+=value.first;
		this->sum.second+=value.second;
	}
	// threshold implementation
	int retval=0;

	if (abs((this->presum.first-this->sum.first)/(float)this->stack.size())>this->threshold)
	{
		this->presum.first=this->sum.first;
		retval=1;
	}
	if (abs((this->presum.second-this->sum.second)/(float)this->stack.size())>this->threshold)
	{
		this->presum.second=this->sum.second;
		retval=1;
	}
	value.first=this->presum.first/(float)this->stack.size();
	value.second=this->presum.second/(float)this->stack.size();

	return retval;
}
