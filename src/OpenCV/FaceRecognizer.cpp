
#include "OpenCV/FaceRecognizer.h"

using namespace cv;

OpenCV::FaceRecognizer::FaceRecognizer()
{
	this->haar_cascade.load(file);
}

OpenCV::FaceRecognizer::~FaceRecognizer()
{
	this->faces.clear();
}

void OpenCV::FaceRecognizer::detectFaces(Mat gray)
{
	this->haar_cascade.detectMultiScale(gray, this->faces);
}

void OpenCV::FaceRecognizer::annotateFaces(Mat frame)
{
	if (this->faces.size()>0)
	{
		Rect face_i = this->faces[0];
		rectangle(frame, face_i, CV_RGB(0, 255,0), 1);
		computeEyesCoordinations(this->faces[0],frame.size());
		detected=true;
	}
	else
	{
		detected=false;
	}
}

cv::CascadeClassifier OpenCV::FaceRecognizer::getCascadeClassifier()
{
	return this->haar_cascade;
}

void OpenCV::FaceRecognizer::computeEyesCoordinations(Rect face, Size size)
{
	this->eyesCoord.x=(((float)(face.x+face.width/2)/(float)size.width-0.5)/0.5)*100;
	this->eyesCoord.y=(((float)(face.y+face.height/3)/(float)size.height-0.5)/0.5)*100;
}

cv::Point2i OpenCV::FaceRecognizer::getEyesCoords()
{
	return this->eyesCoord;
}
