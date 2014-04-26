#ifndef FACERECOGNIZER_H
#define FACERECOGNIZER_H

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include "Util/SizedQueue.h"

namespace OpenCV
{
/**
	 * @brief file Path to file with haar like features for face detection
	 */
static const std::string file = "../share/3dsoftviz/haarcascade_frontalface_alt.xml";

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
	cv::CascadeClassifier getCascadeClassifier();

	/**
		 * @author Autor: Marek Jakab
		 * @brief detectFaces Detect faces on selected image
		 * @param gray cv::Mat image to detect on
		 */
	void detectFaces(cv::Mat gray);

	/**
		 * @author Autor: Marek Jakab
		 * @brief annotateFaces Draws rectangle around detected faces
		 * @param frame cv::Mat image to use for face annotation
		 */
	void annotateFaces(cv::Mat frame);

	/**
		 * @author Autor: Marek Jakab
		 * @brief computeEyesCoordinations Compute eyes position
		 * @param face Rectangle of detected face
		 */
	void computeEyesCoordinations(cv::Rect face, cv::Size size);

	/**
		 * @author Autor: Marek Jakab
		 * @brief OpenCV::FaceRecognizer::getEyesCoords
		 * @return cv::Point2i Eyes coordinations: %/100 distance from middle of the frame
		 */
	cv::Point2i getEyesCoords();

	/**
		 * @author Autor: Marek Jakab
		 * @brief OpenCV::FaceRecognizer::getHeadDistance
		 * @param screenWidth screen resolution - x axis
		 * @return float head distance
		 */
	float getHeadDistance(double screenWidth);

	bool detected;
	bool isMovement;

private:

	cv::Rect rect;
	cv::Rect drawrect;
	bool firstdetection;
	SizedQueue *queue;
	SizedQueue *queueDistance;

	/**
		 * @author Autor: Marek Jakab
		 * @brief startRecognition Function to begin face detection
		 */
	void startRecognition();

	cv::Point2f eyesCoord;
	cv::Point2f previousEyesCoord;
	cv::CascadeClassifier haar_cascade;
	std::vector< cv::Rect_<int> > faces;
};
}

#endif // FACERECOGNIZER_H
