#pragma once
#include "opencv2/opencv.hpp"
#include "data_type.h"

// tessract library //
//#include <baseapi.h>
//#include <allheaders.h>

enum _TEXT_ORDER{ V_ORDER, H_ORDER};
enum _MERGE_TYPE{ _MERGE_LINE, _MERGE_TEXT };
enum _LANGUAGE_TYPE{ _ALPHABETIC, _NONALPHABETIC };

typedef struct{
	cv::Point center;
	int fRadious;


	void setbyRect(cv::Rect rect)
	{
		center.x = (rect.x + rect.x + rect.width)*0.5f;
		center.y = (rect.y + rect.y + rect.height)*0.5f;

		fRadious = rect.width*0.5f;
		if (fRadious < rect.height)
			fRadious = rect.height*0.5f;

	};

}_MOONCircle;

typedef struct _EXTRACT_BOX{
	cv::Rect textbox;
	cv::Rect textboxForCheck;
//	cv::Rect textboxSmall;
	IplImage* pcutImg;

	_MOONCircle textSphere;

	int refCnt;
	bool IsOk;
	bool IsMerged;
	bool IsAmbig;
	bool IsMatched;
	bool IsBig;
	bool IsSmall;

	int lineId;
//	int detectArea;
	int detectWidth;
	int detectHeight;

	_EXTRACT_BOX* pNextBox;

	bool IsSelected;

	void init()
	{
		refCnt = 0;
		IsOk = false;
		IsMerged = false;
		IsAmbig = false;
		IsMatched = false; 
		IsBig = false;
		IsSmall = false;
		
		pcutImg = NULL;
		lineId = 0;
	//	detectArea = 0;
		detectWidth = 0;
		detectHeight = 0;

		pNextBox = NULL;

		IsSelected = false;
	};

	void setExtendBox(int w, int h)
	{
		textboxForCheck = textbox;
//		textboxSmall = textbox;

		textboxForCheck.x -= w;
		if (textboxForCheck.x < 0)	textboxForCheck.x = 0;
		textboxForCheck.width += w*2;

		textboxForCheck.y -= h;
		if (textboxForCheck.y < 0)	textboxForCheck.y = 0;
		textboxForCheck.height += h*2;


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
	std::vector<_EXTRACT_BOX>* GetTextBoxes() { return &m_exTextBox; }
	std::vector<_EXTRACT_BOX>* GetLineBoxes() { return &m_vecLines; }

	
	void ChangeXExpand(int _d);// { m_xExpand += _d; }
	void ChangeYExpand(int _d);// { m_yExpand += _d; }

	RECT2D GetSearchingRect();
	RECT2D GetAverRect() { return m_averTextSize; }
	void InitExtractor();

	//void InitSelectedRect();
	//void PushSelectredRect(int id);

	void InitSelectionOfExtractBoxs();
	void InitSelectionOfExtractLines();
	void AddSelectionOfExtractBoxs(int _id);
	void AddSelectionOfExtractLines(int _id);
	void DelSelectionOfExtractBoxs();
	void DelSelectionOfExtractLines();

	void AddExtLine(cv::Rect rect);
	void AddExtBox(cv::Rect rect);

	// Extraction Functions //
	void CheckAmbiguous();
	void ContractImage(cv::Mat& img);
	cv::Rect GetBoundingBox(cv::Rect r1, cv::Rect r2);
	void ShrinkCharacter(cv::Mat& img);
	void ProcExtractionLine(cv::Mat& img, _TEXT_ORDER _torder, int _w, int _h);
	void ProcExtractionText(cv::Mat& img, _TEXT_ORDER _torder, int _w, int _h);
	void ProcFineExtraction(cv::Mat& img, _TEXT_ORDER _torder, int _w, int _h);
//	void Extraction(std::vector<std::vector<cv::Point> >& contour, std::vector<_EXTRACT_BOX>& veclineBox, int maxWidth, int maxHeight, int extX, int extY, _EXTRACTION_TYPE extType);
	void DetectLines(std::vector<std::vector<cv::Point> >& contour, std::vector<_EXTRACT_BOX>& veclineBox, int maxWidth, int maxHeight, int extX, int extY);
	void DetectChars(std::vector<std::vector<cv::Point> >& contour, std::vector<_EXTRACT_BOX>& veclineBox, int minsize, int maxSize, int extX, int extY);
	void ExtractTexts(cv::Mat& img, cv::Rect lineBox, std::vector<_EXTRACT_BOX>& vecBox, _TEXT_ORDER _torder);
	void FineExtractTexts(cv::Mat& img, cv::Rect lineBox, std::vector<_EXTRACT_BOX>& vecBox, _TEXT_ORDER _torder, int _w, int _h, int _incre, bool IsContract);
	bool RcvMeargingBoundingBox(int maxwidth, int maxheight, std::vector<_EXTRACT_BOX>& veclineBox, int& depth, int extX, int extY, _MERGE_TYPE mergeType);
	bool RcvMeargingBoundingCircle(int minsize, int maxSize, std::vector<_EXTRACT_BOX>& veclineBox, int& depth, int extX, int extY);
	int FindOptimalBox(std::vector<_EXTRACT_BOX>& tmp, int cid, int maxwidth, int maxheight, _EXTRACT_BOX& resBox);
	int FindOptimalCircle(std::vector<_EXTRACT_BOX>& tmp, int i, int minSize, int maxSize, _EXTRACT_BOX& resBox);

	void SetLanguageType(_LANGUAGE_TYPE _type) { m_languageType = _type; }
	void SetVertiCharSpace(int _v) { m_vSpace = _v; }
	void SetHoriCharSpze(int _h) { m_hSpace = _h; }

	void verifyCutSize(cv::Rect& rect, int imgwidth, int imgheight);


private:
//	std::vector<int> m_idSelectedRect;
	std::vector<_EXTRACT_BOX> m_exTextBox;
	std::vector<_EXTRACT_BOX> m_vecLines;

	_LANGUAGE_TYPE m_languageType;


	int m_xExpand;
	int m_yExpand;
	int m_maxWidth;
	int m_maxHeight;

	int m_fineExtractCoff;

	RECT2D m_averTextSize;

	void ProcExtractTextBox(std::vector<std::vector<cv::Point> >& contour, int maxWidth, int maxHeight);
	bool RcvMergeTextBox(int width, int height, float aRatio, int& depth);
	int IsBoxToBoxIntersect(cv::Rect b1, cv::Rect b2);
	bool IsSphereToSphereIntersect(_MOONCircle c1, _MOONCircle c2);


	int m_vSpace;
	int m_hSpace;
	

};

