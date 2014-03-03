#ifndef CAMSELECTCORE_H
#define CAMSELECTCORE_H

#include "opencv2/highgui/highgui.hpp"
#include "QOpenCV/CamSelectWindow.h"
#include "Core/Core.h"

namespace OpenCVCam
{
/**
	*@brief Class CamSelectCore
	*@author Autor: Marek Jakab
	*@date 2.3.2014
	*/
class CamSelectCore
{

public:
	CamSelectCore( QApplication* app);
	~CamSelectCore(void);
	/**
		 * @author Autor: Marek Jakab
		 * @brief getInstance Return instance of CamSelectCore class
		 * @param app QApplication
		 * @return CamSelectCore instance
		 */
	static CamSelectCore *getInstance( QApplication* app);
	/**
		 * @author Autor: Marek Jakab
		 */
	// cv::VideoCapture
	cv::VideoCapture selectCamera();
	int countCameras();

	static CamSelectCore *mCamSelectCore;
	void setCam(int dev_id, int width, int height);
private:
	QApplication *app;

};
}

#endif // CAMSELECTCORE_H
