#include "OpenCV/CapVideo.h"
#include "OpenCV/CamSelectCore.h"

using namespace OpenCV;

OpenCV::CapVideo::CapVideo(int device_id, int width, int height)
{
	this->device_id=device_id;
	this->connections=0;
	this->width=width;
	this->height=height;
}

void OpenCV::CapVideo::startCamera(int width,int height){
	if (!capture2.isOpened()){
		cv::VideoCapture cap(this->device_id);
		this->capture2=cap;
		this->width=width;
		this->height=height;
		this->capture2.set(CV_CAP_PROP_FRAME_WIDTH,width);
		this->capture2.set(CV_CAP_PROP_FRAME_HEIGHT,height);
		this->connections++;
		return;
	}
	this->connections++;
}

void OpenCV::CapVideo::release(){
	this->connections--;
	if (this->connections==0){
		this->capture2.release();
		this->width=0;
		this->height=0;
	}
}

OpenCV::CapVideo::~CapVideo()
{
	this->connections--;
	if (this->connections==0){
		this->capture2.release();
		this->width=0;
		this->height=0;
	}
}

int OpenCV::CapVideo::getWidth(){
	return this->width;
}
int OpenCV::CapVideo::getHeight(){
	return this->height;
}
int OpenCV::CapVideo::getDeviceId(){
	return this->device_id;
}
bool OpenCV::CapVideo::isOpened(){
	return this->capture2.isOpened();
}

cv::Mat OpenCV::CapVideo::queryFrame()
{
	capture2 >> this->frame;
	return this->frame;
}

cv::Mat CapVideo::getGrayframe()
{
	return this->grayframe;
}

cv::Mat CapVideo::getFrame()
{
	return this->frame;
}

void OpenCV::CapVideo::createGray()
{
	cvtColor(this->frame, this->grayframe, CV_BGR2GRAY);
}

cv::VideoCapture OpenCV::CapVideo::getCapture()
{
	return this->capture2;
}
