#pragma once
#include "opencv2/opencv.hpp"

// tessract library //
//#include <baseapi.h>
//#include <allheaders.h>

class CExtractor
{
public:
	CExtractor();
	~CExtractor();

	void extractContours(cv::Mat img, std::vector<cv::Rect>& boundRect);
	void DeSkewImg();
	void detectLetters(cv::Mat& image, std::vector< std::vector<cv::Point> > contours_poly, std::vector<cv::Rect>& boundRect);
	void getContours(cv::Mat img, std::vector<cv::Rect>& boundRect, cv::Mat& cropped2);
	void TestFunc();

	void ProcDeskewing(cv::Mat img);
	void DeSkew(cv::Mat& srcimg);


	void extractWithOCR(cv::Mat image, std::vector<cv::Rect>& boundRect);
		

};

