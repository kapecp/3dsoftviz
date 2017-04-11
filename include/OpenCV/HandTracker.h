#ifndef HANDTRACKER_H
#define HANDTRACKER_H
#include "opencv2/core/types_c.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Core/Core.h"

#define NSAMPLES 7

namespace OpenCV {

class HandTracker
{

public:

    HandTracker();

    ~HandTracker();
    double dist(cv::Point x,cv::Point y);
    std::pair<cv::Point,double> circleFromPoints(cv::Point p1, cv::Point p2, cv::Point p3);
    void getParameterValues(int *threshold, int *areaSize, float brightness, float depth);
    void initTrackbars();
    void normalizeColors();
    cv::Mat produceBinaries(cv::Mat m);
    cv::Mat findHand( cv::Mat mask, float depth);


    int avgColor[NSAMPLES][3];
    int c_lower[NSAMPLES][3];
    int c_upper[NSAMPLES][3];
    std::vector<cv::Mat> bwList;
};
}

#endif // HANDTRACKER_H
