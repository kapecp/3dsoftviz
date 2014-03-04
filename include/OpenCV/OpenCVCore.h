#ifndef OPENCVCORE_H
#define OPENCVCORE_H


class QApplication;

namespace ArucoModul{
	class ArucoThread;
}

namespace QOpenCV {
	class FaceRecognitionThread;
	class FaceRecognitionWindow;
}
namespace OpenCV
{
	class FaceRecognizer;
}

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
		 * @brief faceRecognition Starts window with face detection in another thread
		 */
	void faceRecognition();

	/**
		 * @author Autor: Marek Jakab
		 * @brief getInstance Return instance of OpenCVCore class
		 * @param app QApplication
		 * @return OpenCVCore instance
		 */
	static OpenCVCore	*getInstance( QApplication* app);
	static OpenCVCore	*mOpenCVCore;

private:

	void createConnection();

	QApplication					*mApp;
	ArucoModul::ArucoThread			*mThrAruco;
	QOpenCV::FaceRecognitionThread	*mThrFaceRec;
	QOpenCV::FaceRecognitionWindow	*mOpencvDialog;

	};
}

#endif // OPENCVCORE_H
