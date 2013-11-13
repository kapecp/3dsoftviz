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
        static OpenCVCore * getInstance( QApplication* app);
        void faceRecognition();
        ~OpenCVCore(void);

    private:
        QApplication *app;

    private slots:


    };
}

#endif // OPENCVCORE_H
