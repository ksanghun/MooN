#pragma once
#include "opencv2/opencv.hpp"

class CExtractor
{
public:
	CExtractor();
	~CExtractor();

	void detectLetters(cv::Mat img, std::vector<cv::Rect>& boundRect);


};

