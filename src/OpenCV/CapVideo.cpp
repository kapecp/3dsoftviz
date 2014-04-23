#include "OpenCV/CapVideo.h"

#include <opencv2/imgproc/imgproc.hpp>
#include "OpenCV/CamSelectCore.h"
#include <QDebug>



using namespace OpenCV;

OpenCV::CapVideo::CapVideo( int device_id, int width, int height)
{
	this->device_id		= device_id;
	this->connections	= 0;
	this->width			= width;
	this->height		= height;
}

void OpenCV::CapVideo::startCamera(int width,int height){
	this->connections++;
	if ( !capture.isOpened() ){
		cv::VideoCapture cap( this->device_id);
		this->capture = cap;
		this->capture.set( CV_CAP_PROP_FRAME_WIDTH, width );
		this->capture.set( CV_CAP_PROP_FRAME_HEIGHT, height );
		this->width  = (int) this->capture.get( CV_CAP_PROP_FRAME_WIDTH );
		this->height = (int) this->capture.get( CV_CAP_PROP_FRAME_HEIGHT );
		return;
	}
}

void OpenCV::CapVideo::release(){
	this->connections--;
	if (this->connections == 0){
		this->capture.release();
		this->width  = 0;
		this->height = 0;
	}
}

OpenCV::CapVideo::~CapVideo()
{
	this->release();
	//qDebug() << "CapVideo: dectructor: release capture";

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
	return this->capture.isOpened();
}


cv::Mat OpenCV::CapVideo::queryFrame()
{
	capture >> this->frame;
	return this->frame;
}

cv::Mat CapVideo::getGrayframe()
{
	if(!frame.empty())
	{
		cvtColor( this->frame, this->grayframe, CV_BGR2GRAY);
	}return this->grayframe;
}

cv::Mat CapVideo::getFrame()
{
	return this->frame;
}

cv::VideoCapture OpenCV::CapVideo::getCapture()
{
	return this->capture;
}
