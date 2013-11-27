#ifndef OPENCVCORE_H
#define OPENCVCORE_H

#include "QOpenCV/FaceRecognitionThread.h"
#include "QOpenCV/FaceRecognitionWindow.h"
#include "Core/Core.h"

namespace OpenCV
{
/**
	*@brief Class OpenCVCore
	*@author Autor: Marek Jakab
	*@date 18.11.2013
	*/
class OpenCVCore
{

public:
	OpenCVCore( QApplication* app);
	~OpenCVCore(void);
	/**
		 * @author Autor: Marek Jakab
		 * @brief getInstance Return instance of OpenCVCore class
		 * @param app QApplication
		 * @return OpenCVCore instance
		 */
	static OpenCVCore *getInstance( QApplication* app);
	/**
		 * @author Autor: Marek Jakab
		 * @brief faceRecognition Starts window with face detection in another thread
		 */
	void faceRecognition();

	static OpenCVCore *mOpenCVCore;
private:
	QApplication *app;
};
}

#endif // OPENCVCORE_H
