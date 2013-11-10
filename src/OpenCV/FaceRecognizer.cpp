
#include "OpenCV/FaceRecognizer.h"

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
	for(unsigned int i=0; i < this->faces.size(); i++){
		Rect face_i = this->faces[i];
		rectangle(frame, face_i, CV_RGB(0, 255,0), 1);
	}
}
