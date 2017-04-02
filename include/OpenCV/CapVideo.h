#ifndef CAPVIDEO_H
#define CAPVIDEO_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace OpenCV {
/**
	*@brief Class CapVideo
	*@author Autor: Marek Jakab
	*@date 18.11.2013
	*/
class CapVideo
{

public:
	/**
		 * @brief CapVideo constructor
		 * @param device_id Camera device ID
		 * @param width Camera device width
		 * @param height Camera device height
		 */
	CapVideo( int device_id, int width, int height );
	~CapVideo();
	/**
		 * @author Autor: Marek Jakab
		 * @brief setCaptureProperties Sets camera resolution
		 * @param width Camera device width
		 * @param height Camera device height
		 */
	void setCaptureProperties( int width, int height );


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
	/**
		 * @author Autor: Marek Jakab
		 * @brief startCamera Set camera parameters and open it for use
		 * @param width Camera width
		 * @param height Camera height
		 */
	void startCamera( int width, int height );
	/**
		 * @author Autor: Marek Jakab
		 * @brief getWidth Return width of the camera used by this object
		 * @return Camera width
		 */
	int getWidth();
	/**
		 * @author Autor: Marek Jakab
		 * @brief getHeight Return height of the camera used by this object
		 * @return Camera height
		 */
	int getHeight();
	/**
		 * @author Autor: Marek Jakab
		 * @brief getDeviceId Return ID of the camera used by this object
		 * @return Camera ID
		 */
	int getDeviceId();
	/**
		 * @author Autor: Marek Jakab
		 * @brief isOpened Check if camera is opened for capture
		 * @return true/false if camera is ready
		 */
	bool isOpened();
	/**
		 * @author Autor: Marek Jakab
		 * @brief release Release the camera, making it available by another object to use
		 */
	void release();

	void setAutoExposure( double value );

private:
	/**
		 * @brief device_id Camera ID
		 */
	int device_id;
	/**
		 * @brief connections Number of active connections using the camera
		 */
	int connections;
	/**
		 * @brief width Camera width
		 */
	int width;
	/**
		 * @brief height Camera height
		 */
	int height;
	/**
		 * @brief frame Camera frame (RGB)
		 */
	cv::Mat frame;
	/**
		 * @brief grayframe Camera frame (GRAYSCALE)
		 */
	cv::Mat grayframe;
	/**
		 * @brief capture VideoCapture object used to manipulate with camera (OpenCV)
		 */
	cv::VideoCapture capture;

};
}

#endif // CAPVIDEO_H
