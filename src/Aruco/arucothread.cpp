#include "Aruco/arucothread.h"

using namespace ArucoModul;

ArucoThread::ArucoThread(void)
{
	cancel=false;
}

ArucoThread::~ArucoThread(void)
{
}


void ArucoThread::run()
{
	while(!cancel) {
		// doing aruco work in loop

	}
}


void ArucoThread::setCancel(bool set){
	this->cancel=set;
}

void ArucoThread::pause()
{
	this->cancel=true;
}
