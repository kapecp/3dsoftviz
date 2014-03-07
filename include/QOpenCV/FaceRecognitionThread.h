#ifndef FACERECOGNIZERTHREAD_H
#define FACERECOGNIZERTHREAD_H

#include <QThread>


#include "OpenCV/FaceRecognizer.h"
#include "OpenCV/CapVideo.h"


namespace QOpenCV

{
class FaceRecognitionWindow;

/**
	 * @brief Class FaceRecognitionThread
	 * @author Autor: Marek Jakab
	 * @date 18.11.2013
	 */
class FaceRecognitionThread : public QThread
{
	Q_OBJECT

public:
	FaceRecognitionThread( OpenCV::FaceRecognizer *alg, QObject *parent = 0);
	~FaceRecognitionThread(void);
	OpenCV::FaceRecognizer *mFaceRecognizer;
	OpenCV::CapVideo *mCapVideo;
	FaceRecognitionWindow *mFaceRecognitionWindow;

	/**
		 * @author Autor: Marek Jakab
		 * @brief setWindow Pass FaceRecognitionWindow instance param to thread
		 * @param mFaceRecognitionWindow instance of window for recognition
		 */
	void setWindow(QOpenCV::FaceRecognitionWindow *mFaceRecognitionWindow);

	/**
		 * @author Autor: Marek Jakab
		 * @brief run Starts thread
		 */
	void run();
signals:
	/**
			 * @author Autor: Marek Jakab
			 * @brief pushImage Send image to FaceRecognitionWindow
			 * @param Image cv::Mat
			 */
	void pushImage(cv::Mat Image);

	/**
			 * @author Autor: Marek Jakab
			 * @brief sendEyesCoords
			 * @param x % distance from middle on X axis
			 * @param y % distance from middle on Y axis
			 */
	void sendEyesCoords(float x, float y, float distance);

public slots:
	/**
			 * @author Autor: Marek Jakab
			 * @brief setCancel Sets cancel=true
			 */
	void setCancel(bool);
	/**
			 * @author Autor: Marek Jakab
			 * @brief pauseWindow Pauses recognition window
			 */
	void pauseWindow();

	void setSendImgEnabled( bool sendImgEnabled );

private:
	bool cancel;
	bool mSendImgEnabled;
};
}

#endif //FACERECOGNIZERTHREAD_H
