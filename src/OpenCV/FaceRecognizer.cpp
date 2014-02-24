
#include "OpenCV/FaceRecognizer.h"
#include <QDebug>

using namespace cv;

OpenCV::FaceRecognizer::FaceRecognizer()
{
	if (!this->haar_cascade.load(file))
	{
		qDebug() << "Cannot load file: " << QString::fromStdString(file);
	}
	this->detected=false;
	this->isMovement=false;
}

OpenCV::FaceRecognizer::~FaceRecognizer()
{
	this->faces.clear();
}

void OpenCV::FaceRecognizer::detectFaces(Mat gray)
{
	if (this->detected)
	{
			cv::Mat subImg = gray(cv::Rect(this->rect.x,this->rect.y,
										   this->rect.width,this->rect.height));
			this->haar_cascade.detectMultiScale(subImg, this->faces,1.05, 5,
												0|CV_HAAR_SCALE_IMAGE|
												CV_HAAR_FIND_BIGGEST_OBJECT|
												CV_HAAR_DO_ROUGH_SEARCH);
	}
	else
	{
		this->haar_cascade.detectMultiScale(gray, this->faces,1.05, 5,
											0|CV_HAAR_SCALE_IMAGE|
											CV_HAAR_FIND_BIGGEST_OBJECT|
											CV_HAAR_DO_ROUGH_SEARCH);
	}
}

void OpenCV::FaceRecognizer::annotateFaces(Mat frame)
{
	if (this->faces.size()>0)
	{
		Rect face_i = this->faces[0];
		if (detected)
		{
			if ((abs(1.0f-((float)(face_i.x-face_i.width*0.1+this->rect.x)/(float)(this->rect.x)))<0.07)
				 && (abs(1.0f-((float)(face_i.y-face_i.height*0.1+this->rect.y)/(float)(this->rect.y)))<0.07)){
				face_i=this->rect;
				isMovement=false;
			} else {
				face_i.x=(face_i.x-face_i.width*0.1+this->rect.x);
				if (face_i.x<0) face_i.x=0;
				if (face_i.x>frame.cols-1) face_i.x=frame.cols-1;
				face_i.width=face_i.width*1.2;
				face_i.y=(face_i.y-face_i.height*0.1+this->rect.y);
				if (face_i.y<0) face_i.y=0;
				if (face_i.y>frame.rows-1) face_i.y=frame.rows-1;
				face_i.height=face_i.height*1.2;
				this->rect=face_i;
				isMovement=true;
			}
		}
		rectangle(frame, face_i, CV_RGB(0, 255,0), 1);
		computeEyesCoordinations(face_i,frame.size());
		if (!detected)
		{
			detected=true;
			isMovement=true;
			this->rect=face_i;
		}
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
	this->eyesCoord.x = (((float)(face.x+face.width/2) / (float)size.width-0.5)/0.5)*100;
	this->eyesCoord.y = (((float)(face.y+face.height/3) / (float)size.height-0.5)/0.5)*100;
}

cv::Point2i OpenCV::FaceRecognizer::getEyesCoords()
{
	return this->eyesCoord;
}

float OpenCV::FaceRecognizer::getHeadDistance(int screenWidth)
{
	if (this->faces.size()>0)
	{
		Rect face = this->faces[0];
		// 1.0 when the face width is 3.5 times smaller than frame
		return ((float)screenWidth/(float)face.width)/3.5f;
	}
	return 0.0f;
}

