#ifndef CAPVIDEO_H
#define CAPVIDEO_H

#endif // CAPVIDEO_H

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

namespace OpenCV
{
    class CapVideo{

    public:
        CapVideo(int device_id, int width, int height);
        ~CapVideo();

        void setCaptureProperties(int width, int height);
        void showFrame();
        cv::Mat queryFrame();
        void createGray();

        cv::Mat getGrayframe();
        cv::Mat getFrame();

    private:
        int device_id;
        cv::Mat frame;
        cv::Mat grayframe;
        CvCapture* capture;
        cv::Rect face;

    };
}
