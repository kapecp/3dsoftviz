#ifndef LIGHTDETECTIONTHREAD_H
#define LIGHTDETECTIONTHREAD_H

#include <QThread>
#include <osg/Vec4d>
#include "opencv2/highgui/highgui.hpp"

namespace OpenCV {
class CapVideo;
class LightDetector;
}

namespace QOpenCV

{
/**
    * @brief Class Light Detection Thread
    * @author Autor: Marek Karas
    * @date 22.03.2017
    */
class LightDetectionThread : public QThread
{
	Q_OBJECT

public:

	LightDetectionThread( QObject* parent = 0 );
	~LightDetectionThread( void );

	/**
	     * @author Autor: Marek Jakab
	     * @brief run Starts thread
	     */
	void run();

signals:
	/**
	 * @author Autor: Marek Karas
			 * @brief pushImage Send image to OpenCVWindow
	 * @param Image cv::Mat
	 */
	void pushImage( cv::Mat Image );
	void pushBackgrImage( cv::Mat image );
	/**
			 * @author Autor: Marek Karas
			 * @brief sendLightCoords asdfghjkl
			 * @param Image cv::Mat
			 */
	void sendLightCoords( osg::Vec4d );

public slots:
	/**
	 * @author Autor: Marek Karas
	 * @brief setCancel Sets cancel=true
	 */
	void setCancel( bool );

	/**
	* @author Marek Karas
	* @brief setSendImgEnabling Set emiting of actual frame.
	* @param sendImgEnabled
	*/
	void setSendImgEnabled( bool sendImgEnabled );

	void setSendBackgrImgEnabled( bool sendBackgrImgEnabled );


	/**
	* @author Marek Karas
	* @brief setCapVideo Set member mCapVideo.
	* @param capVideo
	*/
	void setCapVideo( OpenCV::CapVideo* capVideo );

	/**
	* @author Autor: Marek Karas
	* @brief setOffsetX Sets offset for mFisheyeX
	*/
	void setFishEyeCenterX( int );

	/**
	* @author Autor: Marek Karas
	* @brief setOffsetY Sets offset for mFisheyeY
	*/
	void setFishEyeCenterY( int );

	/**
	* @author Autor: Marek Karas
	* @brief setRadius Sets value for mFisheyeR
	*/
	void setFishEyeRadius( int );


	void setShowProcessing( bool );

private:
	/**
	     * @brief mCapVideo CapVideo object representing camera
	     */
	OpenCV::CapVideo*		mCapVideo;

	OpenCV::LightDetector*	mLightDetector;
	/**
	     * @brief mCancel if the thread was canceled
	     */
	bool					mCancel;
	bool					mSendImgEnabled;
	bool					mSendBackgrImgEnabled;
	bool					mShowProcessing;

	int						mFisheyeX;
	int						mFisheyeY;
	int						mFisheyeR;
};
}

#endif //LIGHTDETECTIONTHREAD_H
