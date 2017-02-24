#include "Opencv/MarkerlessTracker.h"

//#include "stdafx.h" nvm k comu to je

#include <QDebug>

using namespace cv; // OpenCV namespace
using namespace std;

OpenCV::MarkerlessTracker::MarkerlessTracker()
{
}

OpenCV::MarkerlessTracker::~MarkerlessTracker()
{
}

RNG rng(12345);

void OpenCV::MarkerlessTracker::track( cv::Mat& img) {

//	Mat img = imread("img_test.jpg");
	resize(img, img, Size(img.cols / 4, img.rows / 4));
	Mat img1;
	cvtColor(img, img1, CV_BGR2GRAY);
	Mat dst_gaus;
	GaussianBlur(img1, dst_gaus, Size(7, 7), 1.5, 0.0, 4);
	Mat dst_bil;
	int kernel_size = 15;
	bilateralFilter(img1, dst_bil, kernel_size, kernel_size * 2, kernel_size / 2);

//	imshow("img1", img1);
//	imshow("img1_bilateral", dst_bil);
//	imshow("img1_gaussian", dst_gaus);

	vector<Vec3f> circles;
	vector<Vec3f> circlesReal;

	/// Hough Circles
	HoughCircles(dst_bil, circles, CV_HOUGH_GRADIENT, 1, dst_bil.rows / 8, 25, 35, 0, 0);

	std::vector<cv::Mat> mask_vector;

	Mat img_circles;
	img.copyTo(img_circles);
	bool do_continue = true;

	/// Draw the circles
	for (size_t i = 0; i < circles.size(); i++)
	{
		do_continue = true;
		cv::Mat mask(dst_bil.size(), dst_bil.type());
		mask.setTo(cv::Scalar(0, 0, 0));

		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));

		//Test stredu kruhu pod dalsim kruhom - odstranenie z vectora
		for (int j = 0; j < mask_vector.size(); j++)
		{
			uchar color = mask_vector[j].at<uchar>(center.y, center.x);
			if (color > 0)
			{
				do_continue = false;
			}
		}
		if (!do_continue)
			continue;

		int radius = cvRound(circles[i][2]);
		// circle center
		circle(img_circles, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		// circle outline
		circle(img_circles, center, radius, Scalar(0, 0, 255), 3, 8, 0);

		circle(mask, center, radius, cv::Scalar(255, 255, 255), -1, 8, 0);
		mask_vector.push_back(mask);
		circlesReal.push_back(circles[i]);
	}

	/// Show your results
//	namedWindow("Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE);
//	imshow("Hough Circle Transform Demo", img_circles);
	img = img_circles;

	// /Masks and histogram
/*
 *
	/// Separate the image in 3 places ( B, G and R )
	vector<Mat> bgr_planes;
	split(img, bgr_planes);

	/// Establish the number of bins
	int histSize = 256;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;
//	imshow("color_test", img);

	for (size_t i = 0; i < mask_vector.size(); i++)
	{
		Mat masked_image;
		img.copyTo(masked_image, mask_vector[i]);
//		imshow("mask_" + std::to_string(i), masked_image);
		Mat b_hist, g_hist, r_hist, gray_hist;

		/// Compute the histograms:
		calcHist(&bgr_planes[0], 1, 0, mask_vector[i], b_hist, 1, &histSize, &histRange, uniform, accumulate);
		calcHist(&bgr_planes[1], 1, 0, mask_vector[i], g_hist, 1, &histSize, &histRange, uniform, accumulate);
		calcHist(&bgr_planes[2], 1, 0, mask_vector[i], r_hist, 1, &histSize, &histRange, uniform, accumulate);
		calcHist(&dst_bil, 1, 0, mask_vector[i], gray_hist, 1, &histSize, &histRange, uniform, accumulate);

		// Draw the histograms for B, G and R
		int hist_w = 512; int hist_h = 400;
		int bin_w = cvRound((double)hist_w / histSize);

		Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
		Mat histImageGray(hist_h, hist_w, CV_8UC1, Scalar(0, 0, 0));

		/// Normalize the result to [ 0, histImage.rows ]
		normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
		normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
		normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
		normalize(gray_hist, gray_hist, 0, histImageGray.rows, NORM_MINMAX, -1, Mat());

		/// Draw for each channel
		for (int i = 1; i < histSize; i++)
		{
			line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
				Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
				Scalar(255, 0, 0), 2, 8, 0);
			line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
				Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
				Scalar(0, 255, 0), 2, 8, 0);
			line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
				Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
				Scalar(0, 0, 255), 2, 8, 0);
			line(histImageGray, Point(bin_w*(i - 1), hist_h - cvRound(gray_hist.at<float>(i - 1))),
				Point(bin_w*(i), hist_h - cvRound(gray_hist.at<float>(i))),
				Scalar(255, 255, 255), 2, 8, 0);
		}

		/// Display
//		imshow("Histogram_" + std::to_string(i), histImage);
//		imshow("Histogram_gray_" + std::to_string(i), histImageGray);
//		cvWaitKey(0);

	}

*/

	//// CIRCLES ////


//	cvWaitKey(0);
	return;

}
