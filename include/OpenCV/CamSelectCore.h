#ifndef CAMSELECTCORE_H
#define CAMSELECTCORE_H

#include <vector>

class QApplication;

namespace OpenCV{
class CapVideo;
}

namespace OpenCV
{
/**
	*@brief Class CamSelectCore
	*@author Autor: Marek Jakab
	*@date 2.3.2014
	*/
class CamSelectCore
{

public:
	~CamSelectCore(void);
	/**
		 * @author Autor: Marek Jakab
		 * @brief getInstance Return instance of CamSelectCore class
		 * @param app QApplication
		 * @return CamSelectCore instance
		 */
	static CamSelectCore *getInstance();
	/**
		 * @author Autor: Marek Jakab
		 */
	OpenCV::CapVideo *selectCamera();
	int countCameras();

	void setCam(int dev_id, int width, int height);

private:
	CamSelectCore();

	QApplication *app;
	std::vector<OpenCV::CapVideo*> camlist;
	int device_id;
	static CamSelectCore *mCamSelectCore;

};
}

#endif // CAMSELECTCORE_H
