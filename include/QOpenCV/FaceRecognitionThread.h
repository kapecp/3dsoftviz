#ifndef FACERECOGNIZERTHREAD_H
#define FACERECOGNIZERTHREAD_H

#include <QThread>

#include "opencv2/highgui/highgui.hpp"

namespace OpenCV {
class CapVideo;
class FaceRecognizer;
}

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

	FaceRecognitionThread( QObject* parent = 0 );
	~FaceRecognitionThread( void );

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
	void pushImage( cv::Mat Image );

	void pushBackgrImage( cv::Mat image, bool trackHands );

	/**
			 * @author Autor: Marek Jakab
			 * @brief sendEyesCoords
			 * @param x % distance from middle on X axis
			 * @param y % distance from middle on Y axis
			 */
	void sendEyesCoords( float x, float y, float distance );

	/**
	         * @author Autor: Viktor Kostan
	         * @brief sendEyesRealCoords - Sends coords for monocular viewing. Sended real-world coords are in meters relative to camera ( [0,0,0] is in focal ).
	         * @param x % distance from middle on X axis
	         * @param y % distance from middle on Y axis
	         * @param y % distance from middle on Y axis
	         */
	void sendEyesRealCoords( float x, float y, float z );

public slots:
	/**
			 * @author Autor: Marek Jakab
			 * @brief setCancel Sets cancel=true
			 */
	void setCancel( bool );
	/**
			 * @author Autor: Marek Jakab
			 * @brief pauseWindow Pauses recognition window
			 */
	void pauseWindow();
	/**
			* @author Dávid Durčák
			* @brief setSendImgEnabling Set emiting of actual frame.
			* @param sendImgEnabled
	*/
	void setSendImgEnabled( bool sendImgEnabled );

	void setSendBackgrImgEnabled( bool sendBackgrImgEnabled );

	/**
			* @author Dávid Durčák
			* @brief setCapVideo Set member mCapVideo.
			* @param capVideo
	*/
	void setCapVideo( OpenCV::CapVideo* capVideo );

private:
	/**
		 * @brief mCapVideo CapVideo object representing camera
		 */
	OpenCV::CapVideo*		mCapVideo;
	/**
		 * @brief mCancel if the thread was canceled
		 */
	bool					mCancel;
	bool					mSendImgEnabled;
	bool					mSendBackgrImgEnabled;

};
}

#endif //FACERECOGNIZERTHREAD_H
