#ifndef FACERECOGNIZER_H
#define FACERECOGNIZER_H

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Util/SizedQueue.h"

#include <string>
#include <vector>

namespace OpenCV {
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
class FaceRecognizer
{

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
	void detectFaces( cv::Mat gray );

	/**
		 * @author Autor: Marek Jakab
		 * @brief annotateFaces Draws rectangle around detected faces
		 * @param frame cv::Mat image to use for face annotation
		 */
	void annotateFaces( cv::Mat frame );

	/**
		 * @author Autor: Marek Jakab
		 * @brief computeEyesCoordinations Compute eyes position
		 * @param face Rectangle of detected face
		 */
	void computeEyesCoordinations( cv::Rect face, cv::Size size );

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
	float getHeadDistance( double screenWidth );

	/**
		 * @author Autor: Viktor Kostan
		 * @brief OpenCV::FaceRecognizer::getHeadDistanceFocal
		 * @param realFaceWidth - Width of user's face in meters (real width)
		 * @return float head distance
		 */
	float getHeadDistanceFocal( double realFaceWidth );
	/**
		 * @author Autor: Marek Jakab
		 * @brief detected Find out the occurence of face in the image
		 * @return bool true/false if face is detected in the image
		 */
	bool detected;
	/**
		 * @author Autor: Marek Jakab
		 * @brief isMovement Claim the position of the new face as movement
		 *	if the face move above selected threshold
		 * @return bool true/false if face have moved
		 */
	bool isMovement;

private:

	/**
		 * @brief rect Rectangle around the detected face. Changes with every new face detected
		 */
	cv::Rect rect;
	/**
		 * @brief drawrect wider rectangle around face which changes after threshold movement
		 */
	cv::Rect drawrect;
	/**
		 * @brief firstdetection first face detected (after we have lost the face and need
		 * to look up the entire frame)
		 */
	bool firstdetection;
	/**
		 * @brief queue SizedQueue used to compute average width and height position of eyes
		 */
	Util::SizedQueue* queue;
	/**
		 * @brief queueDistance SizedQueue used to compute average depth distance of face
		 */
	Util::SizedQueue* queueDistance;

	/**
		 * @author Autor: Marek Jakab
		 * @brief startRecognition Function to begin face detection
		 */
	void startRecognition();

	/**
		 * @brief eyesCoord Eyes location in the frame according to the face detected
		 */
	cv::Point2f eyesCoord;
	/**
		 * @brief haar_cascade Classifier used to decide if the face is in the frame or not
		 */
	cv::CascadeClassifier haar_cascade;
	/**
		 * @brief faces Vector holding detected faces locations
		 */
	std::vector< cv::Rect_<int> > faces;
};
}

#endif // FACERECOGNIZER_H
