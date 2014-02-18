#ifndef ARUCOCORE_H
#define ARUCOCORE_H

#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>

namespace ArucoModul {

class ArucoCore : public QObject
	{
	/**
		*@brief Class ArucoCore
		*@author Autor: Martina Tregerova
		*@date 16.12.2013
		*@note This is the first draft version of class. Most functions will be changed in next few weeks.
		*/
public:
	/**
		 * @author Autor:Martina Tregerova
		 * @brief Constructor
		 * @note Will be changed soon.
		 */
	ArucoCore();
	/**
		 * @author Autor: Martina Tregerova
		 * @brief getMatrix Fills the field with information about marker position given from aruco.
		 * @param modelviewmatrix double* the field that needs to be filled
		 * @note The need to send a field will be changed, too.
		 */
	int getMatrix(double* modelviewmatrix);
	/**
		 * @author Autor: Martina Tregerova
		 * @brief updateImage updates the camera matrix with each update.
		 * @param ImputImage cv::Mat the camera matrix
		 * @note The need to send the image will be changed too.
		 */
	void updateImage(cv::Mat InputImage);
	/**
		 * @author Autor: Martina Tregerova
		 * @brief detectMarkers The actual function that creates a position information based on camera information.
		 */
	void detectMarkers();
	/**
		 * @author Autor: Martina Tregerova
		 * @brief cameraParameters Function that reads the camera information from yml file.
		 * @note May not be needed anymore. Also the file will not be found.
		 */
	void cameraParameters();

private:
	cv::Mat mCamImage;
	aruco::CameraParameters mCamParam;
	aruco::MarkerDetector mMDetector;
	vector<aruco::Marker> mMarkers;
	float mMarkerSize;

	//signals:
	//detect();
	};
} // end ArucoModul namespace

#endif // ARUCOCORE_H
