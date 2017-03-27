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

        cv::Mat CircleMask( cv::Mat );

        void DrawBoundary( cv::Mat );

        void Threshold(int, void*);

        void setFisheyeCenter( cv::Point );

        void setFisheyeRadius( int );

    private:
        cv::Point mfisheyeCenter;
        int mFisheyeRadius;



    };
}

#endif // LIGHTDETECTOR_H
