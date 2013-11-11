#ifndef FACERECOGNIZER_H
#define FACERECOGNIZER_H

#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

using namespace cv;

namespace OpenCV
{
	static const std::string file = "haarcascade_frontalface_alt.xml";

    class FaceRecognizer;

    class FaceRecognizer{

	public:

        FaceRecognizer();
        ~FaceRecognizer();

		cv::CascadeClassifier getCascadeClassifier() { return haar_cascade; }
		void detectFaces(Mat gray);
		void annotateFaces(Mat frame);

	private:

        void OpenCV::FaceRecognizer::startRecognition();
		cv::CascadeClassifier haar_cascade;
		vector< Rect_<int> > faces;
	};
}

#endif // FACERECOGNIZER_H
