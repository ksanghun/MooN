#pragma once
#include "opencv2/opencv.hpp"

// tessract library //
//#include <baseapi.h>
//#include <allheaders.h>

typedef struct{
	cv::Rect textbox;
	cv::Rect textboxForCheck;
	int refCnt;
	bool IsOk;
	bool IsMerged;
	bool IsAmbig;

	void init()
	{
		refCnt = 0;
		IsOk = false;
		IsMerged = false;
		IsAmbig = false;
	};

}_EXTRACT_BOX;

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
	std::vector<_EXTRACT_BOX>& GetTextBoxes() { return m_exTextBox; }
	
private:
	std::vector<_EXTRACT_BOX> m_exTextBox;

	void ProcExtractTextBox(std::vector<std::vector<cv::Point> >& contour);
	bool RcvMergeTextBox(int width, int height, float aRatio, int& depth);
	bool IsBoxToBoxIntersect(cv::Rect b1, cv::Rect b2);

};

