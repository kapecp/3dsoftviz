#ifndef CAPVIDEO_H
#define CAPVIDEO_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace OpenCV
{
/**
	*@brief Class CapVideo
	*@author Autor: Marek Jakab
	*@date 18.11.2013
	*/
class CapVideo{

public:
	/**
		 * @brief CapVideo constructor
		 * @param device_id Camera device ID
		 * @param width Camera device width
		 * @param height Camera device height
		 */
	CapVideo(int device_id, int width, int height);
	~CapVideo();
	/**
		 * @author Autor: Marek Jakab
		 * @brief setCaptureProperties Sets camera resolution
		 * @param width Camera device width
		 * @param height Camera device height
		 */
	void setCaptureProperties(int width, int height);


	/**
		 * @author Autor: Marek Jakab
		 * @brief queryFrame Get image from camera device
		 * @return cv::Mat
		 */
	cv::Mat queryFrame();

	/**
		 * @author Autor: Marek Jakab
		 * @brief getGrayframe Create grayscale image from captured image and return it
		 * @return cv::Mat
		 */
	cv::Mat getGrayframe();

	/**
		 * @author Autor: Marek Jakab
		 * @brief getFrame Returns image
		 * @return cv::Mat
		 */
	cv::Mat getFrame();

	/**
		 * @author Autor: Marek Jakab
		 * @brief getCapture Returns capture
		 * @return CvCapture
		 */
	cv::VideoCapture getCapture();

	void startCamera(int width, int height);
	int getWidth();
	int getHeight();
	int getDeviceId();
	bool isOpened();
	void release();

private:
	int device_id;
	int connections;
	int width;
	int height;
	cv::Mat frame;
	cv::Mat grayframe;
	cv::VideoCapture capture;

};
}

#endif // CAPVIDEO_H
