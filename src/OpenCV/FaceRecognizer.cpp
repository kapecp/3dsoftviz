
#include "OpenCV/FaceRecognizer.h"
#include <QDebug>

#include "opencv2/core/core_c.h"

using namespace cv;

OpenCV::FaceRecognizer::FaceRecognizer()
{
	if (!this->haar_cascade.load(file))
	{
		qDebug() << "Cannot load file: " << QString::fromStdString(file);
	}
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
	this->eyesCoord.x = (((float)(face.x+face.width/2) / (float)size.width-0.5f)/0.5f)*100;
	this->eyesCoord.y = (((float)(face.y+face.height/3) / (float)size.height-0.5f)/0.5f)*100;
}

cv::Point2i OpenCV::FaceRecognizer::getEyesCoords()
{
	return this->eyesCoord;
}

float OpenCV::FaceRecognizer::getHeadDistance(double screenWidth)
{
	if (this->faces.size()>0)
	{
		Rect face = this->faces[0];
		// 1.0 when the face width is 3.5 times smaller than frame
		return ((float)screenWidth/(float)face.width)/3.5f;
	}
	return 0.0f;
}

