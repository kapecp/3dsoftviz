#include "OpenCV/OpenCVCore.h"

using namespace OpenCV;

OpenCV::OpenCVCore * OpenCV::OpenCVCore::mOpenCVCore;

OpenCV::OpenCVCore::OpenCVCore( QApplication* app)
{
    mOpenCVCore = this;
    this->app=app;
}

void OpenCV::OpenCVCore::faceRecognition()
{
    OpenCV::FaceRecognizer *mFaceRecognizer = new OpenCV::FaceRecognizer();

    QOpenCV::FaceRecognitionThread *thr = new QOpenCV::FaceRecognitionThread(mFaceRecognizer);
    QOpenCV::FaceRecognitionWindow *cvw = new QOpenCV::FaceRecognitionWindow(AppCore::Core::getInstance(this->app)->getCoreWindow(),this->app,thr);
    cvw->show();
    thr->setWindow(cvw);
    thr->start();
}

OpenCV::OpenCVCore * OpenCV::OpenCVCore::getInstance( QApplication* app)
{
    // if OpenCV exists
    if(mOpenCVCore == NULL)
    {
        mOpenCVCore = new OpenCV::OpenCVCore(app);
    }
    return mOpenCVCore;
}
