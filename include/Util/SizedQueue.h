#ifndef SIZEDQUEUE_H
#define SIZEDQUEUE_H

#include <deque>

class SizedQueue{

public:

	SizedQueue(int size);
	~SizedQueue();

private:
	std::deque<std::pair<float,float>> stack;

};

#endif // SIZEDQUEUE_H
