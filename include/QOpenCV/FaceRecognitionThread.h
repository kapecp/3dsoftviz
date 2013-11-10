#ifndef QOPENCV_LAYOUT_THREAD_OPENCV_DEF
#define QOPENCV_LAYOUT_THREAD_OPENCV_DEF 1

#include <QThread>

#include "QOpenCV/FaceRecognitionWindow.h"
#include "OpenCV/FaceRecognizer.h"
#include "OpenCV/CapVideo.h"

Q_DECLARE_METATYPE(cv::Mat);

namespace QOpenCV
{
    class FaceRecognitionWindow;
    class FaceRecognitionThread : public QThread
	{
        Q_OBJECT

	public:
        FaceRecognitionThread(OpenCV::FaceRecognizer *alg);
        ~FaceRecognitionThread(void);
        OpenCV::FaceRecognizer *mFaceRecognizer;
        OpenCV::CapVideo *mCapVideo;
        QOpenCV::FaceRecognitionWindow *mFaceRecognitionWindow;
        void setWindow(QOpenCV::FaceRecognitionWindow *mFaceRecognitionWindow);
        void run();
        signals:
            void pushImage(cv::Mat Image);
	private:
            bool cancel;
    private slots:
            void setCancel();
            void pauseWindow();
	};
}

#endif
