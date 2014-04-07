
#include "OpenCV/FaceRecognizer.h"
#include <QDebug>
#include <deque>

#define LIFOSIZE 10

using namespace cv;

OpenCV::FaceRecognizer::FaceRecognizer()
{
	if (!this->haar_cascade.load(file))
	{
		qDebug() << "Cannot load file: " << QString::fromStdString(file);
	}
	this->detected=false;
	this->isMovement=false;
	this->firstdetection=true;
	this->sumx=0;
	this->sumy=0;
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
			this->haar_cascade.detectMultiScale(subImg, this->faces,1.15, 5,
												0|CV_HAAR_SCALE_IMAGE|
												CV_HAAR_FIND_BIGGEST_OBJECT|
												CV_HAAR_DO_ROUGH_SEARCH);
	}
	else
	{
		this->haar_cascade.detectMultiScale(gray, this->faces,1.15, 5,
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
		if (this->firstdetection){
			this->drawrect=this->faces[0];
			this->firstdetection=false;
		}
		if (detected)
		{
			if ((1.15f > abs((float)this->drawrect.x/(float)(this->rect.x+face_i.x)))&&
					(0.85f < abs((float)this->drawrect.x/(float)(this->rect.x+face_i.x))) &&
					(1.15f > abs((float)this->drawrect.y/(float)(this->rect.y+face_i.y))) &&
					(0.85f < abs((float)this->drawrect.y/(float)(this->rect.y+face_i.y))) &&
					(1.15f > abs((float)this->drawrect.width/(float)(face_i.width))) &&
					(0.85f < abs((float)this->drawrect.width/(float)(face_i.width))) &&
					(1.15f > abs((float)this->drawrect.height/(float)(face_i.height))) &&
					(0.85f < abs((float)this->drawrect.height/(float)(face_i.height)))){
				face_i.x=(face_i.x-face_i.width*0.4+this->rect.x);
				if (face_i.x<0) face_i.x=0;
				if (face_i.x>frame.cols-1) face_i.x=frame.cols-1;
				face_i.width=face_i.width*1.8;
				if (face_i.x+face_i.width>frame.cols-1) face_i.width=frame.cols-1-face_i.x;
				face_i.y=(face_i.y-face_i.height*0.4+this->rect.y);
				if (face_i.y<0) face_i.y=0;
				if (face_i.y>frame.rows-1) face_i.y=frame.rows-1;
				face_i.height=face_i.height*1.8;
				if (face_i.y+face_i.height>frame.rows-1) face_i.height=frame.rows-1-face_i.y;

				this->rect=face_i;
				isMovement=false;
			} else {
				//TODO conversion to INT, no effective
				this->drawrect.x=(face_i.x+this->rect.x); //-face_i.width*0.1
				if (this->drawrect.x<0) this->drawrect.x=0;
				if (this->drawrect.x>frame.cols-1) this->drawrect.x=frame.cols-1;
				this->drawrect.width=face_i.width;//*1.2;
				if (this->drawrect.x+this->drawrect.width>frame.cols-1) this->drawrect.width=frame.cols-1-this->drawrect.x;
				this->drawrect.y=(face_i.y+this->rect.y); //-face_i.height*0.1
				if (this->drawrect.y<0) this->drawrect.y=0;
				if (this->drawrect.y>frame.rows-1) this->drawrect.y=frame.rows-1;
				this->drawrect.height=face_i.height;//*1.2;
				if (this->drawrect.y+this->drawrect.height>frame.rows-1) this->drawrect.height=frame.rows-1-this->drawrect.y;

				face_i.x=(face_i.x-face_i.width*0.4+this->rect.x);
				if (face_i.x<0) face_i.x=0;
				if (face_i.x>frame.cols-1) face_i.x=frame.cols-1;
				face_i.width=face_i.width*1.8;
				if (face_i.x+face_i.width>frame.cols-1) face_i.width=frame.cols-1-face_i.x;
				face_i.y=(face_i.y-face_i.height*0.4+this->rect.y);
				if (face_i.y<0) face_i.y=0;
				if (face_i.y>frame.rows-1) face_i.y=frame.rows-1;
				face_i.height=face_i.height*1.8;
				if (face_i.y+face_i.height>frame.rows-1) face_i.height=frame.rows-1-face_i.y;

				this->rect=face_i;
				isMovement=true;
			}
		}
		if (!detected)
		{
			detected=true;
			isMovement=true;
			this->drawrect=face_i;
			face_i.x=(face_i.x-face_i.width*0.4);
			if (face_i.x<0) face_i.x=0;
			if (face_i.x>frame.cols-1) face_i.x=frame.cols-1;
			face_i.width=face_i.width*1.8;
			if (face_i.x+face_i.width>frame.cols-1) face_i.width=frame.cols-1-face_i.x;
			face_i.y=(face_i.y-face_i.height*0.4);
			if (face_i.y<0) face_i.y=0;
			if (face_i.y>frame.rows-1) face_i.y=frame.rows-1;
			face_i.height=face_i.height*1.8;
			if (face_i.y+face_i.height>frame.rows-1) face_i.height=frame.rows-1-face_i.y;
			this->rect=face_i;
		}

		rectangle(frame, drawrect, CV_RGB(0, 255,0), 1);
		rectangle(frame, face_i, CV_RGB(255, 0,0), 1);
		computeEyesCoordinations(this->drawrect,frame.size()); //face_i
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
	if (this->lifo.size()<LIFOSIZE){
		this->lifo.push_back(Point2f(((((float)(face.x+face.width/2) / (float)size.width-0.5f)/0.5f)*100),
								((((float)(face.y+face.height/3) / (float)size.height-0.5f)/0.5f)*100)));
		this->sumx+=((((float)(face.x+face.width/2) / (float)size.width-0.5f)/0.5f)*100);
		this->sumy+=((((float)(face.y+face.height/3) / (float)size.height-0.5f)/0.5f)*100);
	} else {
		Point2f p = lifo.front();
		this->sumx-= p.x;
		this->sumy-= p.y;
		lifo.pop_front();
		this->lifo.push_back(Point2f(((((float)(face.x+face.width/2) / (float)size.width-0.5f)/0.5f)*100),
								((((float)(face.y+face.height/3) / (float)size.height-0.5f)/0.5f)*100)));
		this->sumx+=((((float)(face.x+face.width/2) / (float)size.width-0.5f)/0.5f)*100);
		this->sumy+=((((float)(face.y+face.height/3) / (float)size.height-0.5f)/0.5f)*100);
	}
	this->eyesCoord.x = this->sumx/(float)lifo.size();
	this->eyesCoord.y = this->sumy/(float)lifo.size();
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

