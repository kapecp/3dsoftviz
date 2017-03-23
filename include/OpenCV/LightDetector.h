#ifndef LIGHTDETECTOR_H
#define LIGHTDETECTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>


namespace OpenCV {

    class LightDetector;

/**
	*@brief Class LightDetector
	*@author Autor: Marek Karas
	*@date 18.11.2013
	*/
    class LightDetector
    {

    public:

        LightDetector();
        ~LightDetector();

    private:
        int fisheye_x;
        int fisheye_y;
        int fisheye_r;

        void Threshold(int, void*);

        cv::Mat CircleMask(cv::Mat src, cv::Point center, int radius);

    };
}

#endif // LIGHTDETECTOR_H
