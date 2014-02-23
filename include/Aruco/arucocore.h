#ifndef ARUCOCORE_H
#define ARUCOCORE_H

#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>
#include <QMatrix4x4>

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
	ArucoCore(const QString markerDesFile);

	/**
		 * @author Autor:Dávid Durčák
		 * @brief method that encapsule all necessary funtionality for detecting marker on input image and than return result model view matrix
		 * @param modelviewmatrix double* the field that needs to be filled
		 * @param[out] result model view matrix
		 */
	const QMatrix4x4 getDetectedMatrix(cv::Mat inputImage);

	/**
		 * @author Autor:Dávid Durčák
		 * @brief  getDetImage return image with drawed boundaries and 3D cube on detected markers, if any are
		 */
	cv::Mat getDetImage();

private:
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
	void updateImage(cv::Mat inputImage);
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
	void cameraParameters(const QString markerDesFile);


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
