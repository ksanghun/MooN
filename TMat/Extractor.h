#pragma once
#include "opencv2/opencv.hpp"

class CExtractor
{
public:
	CExtractor();
	~CExtractor();

	void detectLetters(cv::Mat img, std::vector<cv::Rect>& boundRect);
	void DeSkewImg();
	void extractContours(cv::Mat& image, std::vector< std::vector<cv::Point> > contours_poly);
	void getContours(cv::Mat img);
	void TestFunc();


};

