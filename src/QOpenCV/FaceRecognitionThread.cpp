#include "QOpenCV/FaceRecognitionThread.h"

using namespace QOpenCV;

QOpenCV::FaceRecognitionThread::FaceRecognitionThread(OpenCV::FaceRecognizer* alg)
{
    this->mFaceRecognizer = alg;
    this->mCapVideo = new OpenCV::CapVideo(0,640,480);
    this->cancel=false;
    qRegisterMetaType<cv::Mat>("Mat");
}

QOpenCV::FaceRecognitionThread::~FaceRecognitionThread(void)
{

}

void QOpenCV::FaceRecognitionThread::setWindow(FaceRecognitionWindow *mFaceRecognitionWindow)
{
    this->mFaceRecognitionWindow = mFaceRecognitionWindow;
}

void QOpenCV::FaceRecognitionThread::run()
{
    connect(this->mFaceRecognitionWindow,SIGNAL(cancelLoop()),this,SLOT(setCancel()));
    cv::Mat image;
    for(;;) {
        image=this->mCapVideo->queryFrame();
        this->mCapVideo->createGray();

        this->mFaceRecognizer->detectFaces(this->mCapVideo->getGrayframe());
        this->mFaceRecognizer->annotateFaces(image);
        emit this->pushImage(image);

        //cv::imshow("test",image);
        char key = (char) waitKey(20);
        if(key == 27) break;

        if (cancel) break;
    }
    this->mCapVideo->CapVideo::~CapVideo();
    this->mFaceRecognizer->FaceRecognizer::~FaceRecognizer();
    this->quit();
}

void QOpenCV::FaceRecognitionThread::pauseWindow()
{

}

void QOpenCV::FaceRecognitionThread::setCancel(){
    this->cancel=true;
}
