#ifndef OPENCVCORE_H
#define OPENCVCORE_H

#include "QOpenCV/FaceRecognitionThread.h"
#include "QOpenCV/FaceRecognitionWindow.h"

namespace OpenCV
{
    class OpenCVCore
    {

    public:
        OpenCVCore( QApplication* app);
        static OpenCVCore * mOpenCVCore;
        static OpenCVCore * OpenCVCore::getInstance( QApplication* app);
        void OpenCV::OpenCVCore::faceRecognition();
        ~OpenCVCore(void);

    private:
        QApplication *app;

    private slots:


    };
}

#endif // OPENCVCORE_H
