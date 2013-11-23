#ifndef CAPVIDEO_H
#define CAPVIDEO_H

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace OpenCV
{
    /**
    *@brief Class CapVideo
    *@author Autor: Marek Jakab
    *@date 18.11.2013
    */
    class CapVideo{

    public:
        /**
         * @brief CapVideo constructor
         * @param device_id Camera device ID
         * @param width Camera device width
         * @param height Camera device height
         */
        CapVideo(int device_id, int width, int height);
        ~CapVideo();

        /**
         * @author Autor: Marek Jakab
         * @brief setCaptureProperties Sets camera resolution
         * @param width Camera device width
         * @param height Camera device height
         */
        void setCaptureProperties(int width, int height);

        /**
         * @author Autor: Marek Jakab
         * @brief showFrame Show captured image in new frame
         */
        void showFrame();

        /**
         * @author Autor: Marek Jakab
         * @brief queryFrame Get image from camera device
         * @return cv::Mat
         */
        cv::Mat queryFrame();

        /**
         * @author Autor: Marek Jakab
         * @brief createGray Create grayscale image from captured image
         */
        void createGray();

        /**
         * @author Autor: Marek Jakab
         * @brief getGrayframe Returns grayscale image
         * @return cv::Mat
         */
        cv::Mat getGrayframe();

        /**
         * @author Autor: Marek Jakab
         * @brief getFrame Returns image
         * @return cv::Mat
         */
        cv::Mat getFrame();

    private:
        int device_id;
        cv::Mat frame;
        cv::Mat grayframe;
        CvCapture *capture;
        cv::Rect face;

    };
}

#endif // CAPVIDEO_H
