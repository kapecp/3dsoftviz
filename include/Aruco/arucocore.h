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
	ArucoCore();

	/**
		 * @author Autor: Martina Tregerova
		 * @brief cameraParameters Function that reads the camera information from yml file.
		 * @note May not be needed anymore. Also the file will not be found.
		 */
	bool setCameraParameters(const QString markerDesFile);

	/**
		 * @author Autor:Dávid Durčák
		 * @brief getDetectedMatrix method that encapsule all necessary funtionality for detecting marker on input image and than return result model view matrix
		 * @param inputImage Image from cammera, where marker should be detected
		 * @param[out] result model view matrix
		 */
	const QMatrix4x4 getDetectedMatrix(cv::Mat inputImage);

	/**
		 * @author Autor:Dávid Durčák
		 * @brief getDetectedPosAndQuat method that encapsule all necessary funtionality for detecting marker on input image and fill position and qauternion vectors with that informations
		 * @param inputImage Image from cammera, where marker should be detected
		 * @param position[3] output array that will describe positio of marker by vector (x,y,z)
		 * @param quaternion[4] output array that will describe orientation of marker by quaternion (w,x,y,z)
		 * @param[out] true if least one marker was detected
		 */
	bool getDetectedPosAndQuat(cv::Mat inputImage, double position[3], double quaternion[4]);

	/**
		 * @author Autor:Dávid Durčák
		 * @brief detect method that encapsule all necessary funtionality for detecting marker on input image
		 * @param inputImage Image from cammera, where markers should be detected
		 * @param[out] number of detected markers
		 */
	long detect(cv::Mat inputImage);

	/**
			 * @author Autor:Dávid Durcák
			 * @brief getPosAndQuat method thad fill position and qauternion vectors with that informations of marker with number markerNum, if least markerNum of markers were found
			 * @param markerNum  number of marker, that we want position and orientation for. MarkerNum is counted from 0
			 * @param position[3] output array that will describe positio of marker by vector (x,y,z)
			 * @param quaternion[4] output array that will describe orientation of marker by quaternion (w,x,y,z)
			 * @param[out] true if least markerNum of markers were found
			 */
		bool getPosAndQuat(unsigned int markerNum, double position[3], double quaternion[4]);

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



	cv::Mat					mCamImage;		// frame
	aruco::CameraParameters mCamParam;		// camera parameters
	aruco::MarkerDetector	mMDetector;		// detector
	vector<aruco::Marker>	mMarkers;		// vector of detected markers
	float					mMarkerSize;	// real marker size

	};
} // end ArucoModul namespace

#endif // ARUCOCORE_H
