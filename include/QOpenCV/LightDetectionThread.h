#ifndef LIGHTDETECTIONTHREAD_H
#define LIGHTDETECTIONTHREAD_H

#include <QThread>

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
                 * @author Autor: Marek Jakab
                 * @brief pushImage Send image to FaceRecognitionWindow
                 * @param Image cv::Mat
                 */
        void pushImage( cv::Mat Image );
        void pushBackgrImage( cv::Mat image );

    public slots:
        /**
                 * @author Autor: Marek Jakab
                 * @brief setCancel Sets cancel=true
                 */
        void setCancel( bool );

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

#endif //LIGHTDETECTIONTHREAD_H
