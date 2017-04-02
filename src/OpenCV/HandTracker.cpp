#include "OpenCV/HandTracker.h"
#include <easylogging++.h>
#include <vector>
#include<iostream>


OpenCV::HandTracker::HandTracker()
{
}

OpenCV::HandTracker::~HandTracker()
{

}
double OpenCV::HandTracker::dist(cv::Point x,cv::Point y)
{
    return (x.x-y.x)*(x.x-y.x)+(x.y-y.y)*(x.y-y.y);
}


//This function returns the radius and the center of the circle given 3 points
//If a circle cannot be formed , it returns a zero radius circle centered at (0,0)
std::pair<cv::Point,double> OpenCV::HandTracker::circleFromPoints(cv::Point p1, cv::Point p2, cv::Point p3)
{
    double offset = pow(p2.x,2) +pow(p2.y,2);
    double bc =   ( pow(p1.x,2) + pow(p1.y,2) - offset )/2.0;
    double cd =   (offset - pow(p3.x, 2) - pow(p3.y, 2))/2.0;
    double det =  (p1.x - p2.x) * (p2.y - p3.y) - (p2.x - p3.x)* (p1.y - p2.y);
    double TOL = 0.0000001;
//    if (abs(det) < TOL) { std::cout<<"POINTS TOO CLOSE"<<endl;return std::make_pair(cv::Point(0,0),0); }

    double idet = 1/det;
    double centerx =  (bc * (p2.y - p3.y) - cd * (p1.y - p2.y)) * idet;
    double centery =  (cd * (p1.x - p2.x) - bc * (p2.x - p3.x)) * idet;
    double radius = sqrt( pow(p2.x - centerx,2) + pow(p2.y-centery,2));

    return std::make_pair(cv::Point(centerx,centery),radius);
}

void OpenCV::HandTracker::getParameterValues(int *threshold, int *areaSize,
                                             float brightness, float depth){
    if(depth > 380){
        *threshold = 50;
    }
    else{
        *threshold = 160 - (depth/4);
    }
}


// find contours of segmented hand and count fingers
cv::Mat OpenCV::HandTracker::findHand( cv::Mat mask, float depth )
{
    cv::vector<cv::vector<cv::Point> > contours;
    cv::vector<cv::Vec4i> hierarchy;
    cv::Mat tempMask = mask.clone();

    cv::vector<std::pair<cv::Point,double> > palm_centers;

    cv::Scalar summ = sum(mask);


    float brightness = summ[0]/((pow(2,8)-1)*mask.rows * mask.cols) * 2;
//    LOG (INFO) << "brightness: " + std::to_string(brightness);
//    LOG (INFO) << "Depth: " + std::to_string(depth);

    int threshold_down = 50;
    int threshold_up = 150;

    int thresholdValue =0;
    int areaSize = 0;

    getParameterValues(&thresholdValue, &areaSize, brightness, depth);
//    LOG (INFO) << std::to_string(areaSize);

    threshold(tempMask, tempMask, thresholdValue, threshold_up, cv::THRESH_BINARY);
    GaussianBlur(tempMask, tempMask, cv::Size(3, 3), 2.5, 2.5);

    Canny(tempMask,tempMask,threshold_down, threshold_up,3);
    threshold(tempMask, tempMask, 30, threshold_up, cv::THRESH_BINARY);

    findContours( tempMask,contours, hierarchy, CV_RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point() );

    for(int i=0;i<contours.size();i++)
    {
                //Ignore all small insignificant areas
                if(contourArea(contours[i])>=1500)
                {
//                    LOG (INFO) << "Size of area: " + std::to_string(contourArea(contours[i]));
                    //Draw contour
                    cv::vector<cv::vector<cv::Point> > tcontours;
                    tcontours.push_back(contours[i]);
                    drawContours(mask,tcontours,-1,cv::Scalar(255,255,255),2);

                    //Detect Hull in current contour
                    cv::vector<cv::vector<cv::Point> > hulls(1);
                    cv::vector<cv::vector<int> > hullsI(1);
                    convexHull(cv::Mat(tcontours[0]),hulls[0],false);
                    convexHull(cv::Mat(tcontours[0]),hullsI[0],false);
//                    drawContours(mask,hulls,-1,cv::Scalar(0,255,0),2);

                    //Find minimum area rectangle to enclose hand
                    cv::RotatedRect rect=minAreaRect(cv::Mat(tcontours[0]));

                    //Find Convex Defects
                    cv::vector<cv::Vec4i> defects;
                    if(hullsI[0].size()>0)
                    {
                        cv::Point2f rect_points[4]; rect.points( rect_points );
//                        for( int j = 0; j < 4; j++ )
//                            line( mask, rect_points[j], rect_points[(j+1)%4], cv::Scalar(255,255,255), 1, 8 );
                        cv::Point rough_palm_center;
                        convexityDefects(tcontours[0], hullsI[0], defects);
                        if(defects.size()>=3)
                        {
                            cv::vector<cv::Point> palm_points;
                            for(int j=0;j<defects.size();j++)
                            {
                                int startidx=defects[j][0]; cv::Point ptStart( tcontours[0][startidx] );
                                int endidx=defects[j][1]; cv::Point ptEnd( tcontours[0][endidx] );
                                int faridx=defects[j][2]; cv::Point ptFar( tcontours[0][faridx] );
                                //Sum up all the hull and defect points to compute average
                                rough_palm_center+=ptFar+ptStart+ptEnd;
                                palm_points.push_back(ptFar);
                                palm_points.push_back(ptStart);
                                palm_points.push_back(ptEnd);
                            }

                            //Get palm center by 1st getting the average of all defect points, this is the rough palm center,
                            //Then U chose the closest 3 points ang get the circle radius and center formed from them which is the palm center.
                            rough_palm_center.x/=defects.size()*3;
                            rough_palm_center.y/=defects.size()*3;
                            cv::Point closest_pt=palm_points[0];
                            cv::vector<std::pair<double,int> > distvec;
                            for(int i=0;i<palm_points.size();i++)
                                distvec.push_back(std::make_pair(dist(rough_palm_center,palm_points[i]),i));
                            sort(distvec.begin(),distvec.end());

                            //Keep choosing 3 points till you find a circle with a valid radius
                            //As there is a high chance that the closes points might be in a linear line or too close that it forms a very large circle
                            std::pair<cv::Point,double> soln_circle;
                            for(int i=0;i+2<distvec.size();i++)
                            {
                                cv::Point p1=palm_points[distvec[i+0].second];
                                cv::Point p2=palm_points[distvec[i+1].second];
                                cv::Point p3=palm_points[distvec[i+2].second];
                                    soln_circle=circleFromPoints(p1,p2,p3);//Final palm center,radius
                                    if(soln_circle.second!=0 && soln_circle.second< 75 && soln_circle.second> 10)
                                        break;
                            }

                            //Find avg palm centers for the last few frames to stabilize its centers, also find the avg radius
                            palm_centers.push_back(soln_circle);
                            if(palm_centers.size()>10)
                                palm_centers.erase(palm_centers.begin());

                            cv::Point palm_center;
                            double radius=0;
                            for(int i=0;i<palm_centers.size();i++)
                            {
                                palm_center+=palm_centers[i].first;
                                radius+=palm_centers[i].second;
                            }
                            palm_center.x/=palm_centers.size();
                            palm_center.y/=palm_centers.size();
                            radius/=palm_centers.size();

                            //Draw the palm center and the palm circle
                            //The size of the palm gives the depth of the hand
                            if (radius > 0){
                                circle(mask,palm_center,5,cv::Scalar(144,144,255),3);
                                circle(mask,palm_center,radius,cv::Scalar(144,144,255),2);
                            }

                            //Detect fingers by finding points that form an almost isosceles triangle with certain thesholds
                            int no_of_fingers=0;
                            for(int j=0;j<defects.size();j++)
                            {
                                int startidx=defects[j][0]; cv::Point ptStart( tcontours[0][startidx] );
                                int endidx=defects[j][1]; cv::Point ptEnd( tcontours[0][endidx] );
                                int faridx=defects[j][2]; cv::Point ptFar( tcontours[0][faridx] );
                                //X o--------------------------o Y
                                double Xdist=sqrt(dist(palm_center,ptFar));
                                double Ydist=sqrt(dist(palm_center,ptStart));
                                double length=sqrt(dist(ptFar,ptStart));

                                double retLength=sqrt(dist(ptEnd,ptFar));

                                // fingertips can be just y-coordinates leeser than y-coordinate of palm
                                if(ptEnd.y > palm_center.y){
                                    continue;
                                }

                                //Play with these thresholds to improve performance
                                //zvlacsovanim cisla v - length<=7*radius&&Ydist // vznika viacej prestov
                                //retLength>=10 // vzavcsovanim vznika viacej prestov
                                if(length<=3*radius&&Ydist>=0.4*radius&&length>=10&&retLength>=10&&std::max(length,retLength)/std::min(length,retLength)>=0.8)
                                {
                                    if(std::min(Xdist,Ydist)/std::max(Xdist,Ydist)<=0.8)
                                    {
                                        if((Xdist>=0.1*radius&&Xdist<=1.3*radius&&Xdist<Ydist)||(Ydist>=0.1*radius&&Ydist<=1.3*radius&&Xdist>Ydist)){
                                            line( mask, ptEnd, ptFar, cv::Scalar(255,255,255), 1 );
                                            circle(mask,ptEnd,5,cv::Scalar(200,200,255),2);
                                            no_of_fingers++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
    return mask;
}
