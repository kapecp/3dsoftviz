#ifndef CAMSELECTCORE_H
#define CAMSELECTCORE_H

#include <vector>

class QApplication;

namespace OpenCV {
class CapVideo;
}

namespace OpenCV {
/**
	*@brief Class CamSelectCore
	*@author Autor: Marek Jakab
	*@date 2.3.2014
	*/
class CamSelectCore
{

public:
	~CamSelectCore( void );
	/**
		 * @author Autor: Marek Jakab
		 * @brief getInstance Return instance of CamSelectCore class
		 * @param app QApplication
		 * @return CamSelectCore instance
		 */
	static CamSelectCore* getInstance();
	/**
		 * @author Autor: Marek Jakab
		 */
	OpenCV::CapVideo* selectCamera();
	/**
		 * @author Autor: Marek Jakab
		 * @brief countCameras Count the number of active cameras
		 * @return number of plugged in and active camera devices
		 */
	int countCameras();

	/**
		 * @author Autor: David Durcak, Marek Jakab
		 * @brief setCam Set camera parameters
		 * @param dev_id Camera ID
		 * @param width Camera width
		 * @param height Camera height
		 */
	void setCam( std::vector<OpenCV::CapVideo*>::size_type dev_id, int width, int height );

private:
	CamSelectCore();

	QApplication* app;
	/**
		 * @brief camlist Vector holding list of camera devices
		 */
	std::vector<OpenCV::CapVideo*> camlist;
	/**
		 * @brief device_id Specific camera ID for selection
		 */
	std::vector<OpenCV::CapVideo*>::size_type device_id;
	/**
		 * @brief mCamSelectCore Instance for Singleton
		 */
	static CamSelectCore* mCamSelectCore;

};
}

#endif // CAMSELECTCORE_H
