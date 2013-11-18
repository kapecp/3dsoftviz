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
     /**
     * @brief file Path to file with haar like features for face detection
     */
	static const std::string file = "haarcascade_frontalface_alt.xml";

    class FaceRecognizer;

    /**
    *@brief Class FaceRecognizer
    *@author Autor: Marek Jakab
    *@date 18.11.2013
    */
    class FaceRecognizer{

	public:

        FaceRecognizer();
        ~FaceRecognizer();

        /**
         * @author Autor: Marek Jakab
         * @brief getCascadeClassifier Returns Cascade Classifier
         * @return cv::CascadeClassifier
         */
        cv::CascadeClassifier getCascadeClassifier() { return haar_cascade; }

        /**
         * @author Autor: Marek Jakab
         * @brief detectFaces Detect faces on selected image
         * @param gray cv::Mat image to detect on
         */
		void detectFaces(Mat gray);

        /**
         * @author Autor: Marek Jakab
         * @brief annotateFaces Draws rectangle around detected faces
         * @param frame cv::Mat image to use for face annotation
         */
		void annotateFaces(Mat frame);

	private:

        cv::Rect rect;
        bool detected;

        /**
         * @author Autor: Marek Jakab
         * @brief startRecognition Function to begin face detection
         */
        void startRecognition();
		cv::CascadeClassifier haar_cascade;
		vector< Rect_<int> > faces;
	};
}

#endif // FACERECOGNIZER_H
