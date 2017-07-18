#pragma once
#include "opencv2/opencv.hpp"
#include "data_type.h"

// tessract library //
//#include <baseapi.h>
//#include <allheaders.h>

enum _TEXT_ORDER{ V_ORDER, H_ORDER};

typedef struct{
	cv::Rect textbox;
	cv::Rect textboxForCheck;
	cv::Rect textboxSmall;

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

	void setExtendBox(int w, int h)
	{
		textboxForCheck = textbox;
		textboxSmall = textbox;

	//	textboxForCheck.x -= w;
	//	if (textboxForCheck.x < 0)	textboxForCheck.x = 0;
		textboxForCheck.width += w;

	//	textboxForCheck.y -= h;
	//	if (textboxForCheck.y < 0)	textboxForCheck.y = 0;
		textboxForCheck.height += h;


		//textboxSmall.x += 2;
		//textboxForCheck.width -= w * 2;

		//textboxSmall.y +=2;
		//textboxForCheck.height -= h * 2;
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
	void getContours(cv::Mat& img, cv::Mat& oImg, std::vector<cv::Rect>& boundRect);
	void contractCharacters(cv::Mat img, std::vector<std::vector<cv::Point> >& contours);
	void TestFunc();

	void ProcDeskewing(cv::Mat img);
	void DeSkew(cv::Mat& srcimg);


	void extractWithOCR(cv::Mat image, std::vector<cv::Rect>& boundRect);
	std::vector<_EXTRACT_BOX>& GetTextBoxes() { return m_exTextBox; }
	std::vector<_EXTRACT_BOX>& GetLineBoxes() { return vecLines; }

	
	void ChangeXExpand(int _d);// { m_xExpand += _d; }
	void ChangeYExpand(int _d);// { m_yExpand += _d; }

	RECT2D GetSearchingRect();
	RECT2D GetAverRect() { return m_averTextSize; }
	void InitExtractor();


	// Extraction Functions //
	void ShrinkCharacter(cv::Mat& img);
	void ProcExtraction(cv::Mat& img, _TEXT_ORDER _torder);
	void ExtractLines(std::vector<std::vector<cv::Point> >& contour, std::vector<_EXTRACT_BOX>& veclineBox, int maxWidth, int maxHeight, int extX, int extY);
	bool RcvMeargingBoundingBox(int maxwidth, int maxheight, std::vector<_EXTRACT_BOX>& veclineBox, int& depth, int extX, int extY);

private:
	std::vector<_EXTRACT_BOX> m_exTextBox;
	std::vector<_EXTRACT_BOX> vecLines;



	int m_xExpand;
	int m_yExpand;
	int m_maxWidth;
	int m_maxHeight;

	RECT2D m_averTextSize;

	void ProcExtractTextBox(std::vector<std::vector<cv::Point> >& contour, int maxWidth, int maxHeight);
	bool RcvMergeTextBox(int width, int height, float aRatio, int& depth);
	bool IsBoxToBoxIntersect(cv::Rect b1, cv::Rect b2);

	

};

