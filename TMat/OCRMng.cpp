#include "stdafx.h"
#include "OCRMng.h"



COCRMng::COCRMng()
{
}


COCRMng::~COCRMng()
{
}


bool COCRMng::InitOCRMng()
{
	if (m_tessEng.Init("./tessdata/", "eng")){
		return false;
	}
	if (m_tessChi.Init("./tessdata/", "chi_sim")){
		return false;
	}

	m_tessEng.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
//	m_tessEng.SetVariable("tessedit_char_whitelist", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmonpqrstuvwxyz~!@#$%^&*()_-+={}[]:;'<>?,./\|");


	m_tessChi.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
//	m_tessChi.SetVariable("tessedit_char_blacklist", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
	return true;
}


void COCRMng::SetOCRDetectMode(_DETECT_MODE mode)
{
	//if (mode == _DET_WORD){
	//	m_tessEng.SetPageSegMode(tesseract::PSM_SINGLE_WORD);
	//	m_tessChi.SetPageSegMode(tesseract::PSM_SINGLE_WORD);
	//}
	//else if (mode == _DET_CHAR){
	//	m_tessEng.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
	//	m_tessChi.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
	//}
}


float COCRMng::extractWithOCR(cv::Mat image, std::vector<_OCR_RES>& boundRect, tesseract::TessBaseAPI& tess, tesseract::PageIteratorLevel level)
{	
	//======================//
//	float fScale = 96.0f / (float)image.rows;
	float fScale = 1.2f;
	int nW = image.cols*fScale;
	int nH = image.rows*fScale;
	cv::Size size(nW, nH);
	cv::resize(image, image, size);

	tess.SetImage((uchar*)image.data, image.size().width, image.size().height, image.channels(), image.step1());
	const char* out = tess.GetUTF8Text();

	//tesseract::PageIteratorLevel level;
	//level = tesseract::RIL_SYMBOL;

	float averConf = 0.0f;
	int cnt = 0;

		tesseract::ResultIterator* ri = tess.GetIterator();
		if (ri != 0) {
			do {
				char* word = ri->GetUTF8Text(level);				

				float conf = ri->Confidence(level);
				if (conf > 94.99f) continue;
					int x1, y1, x2, y2, w, h;
					ri->BoundingBox(level, &x1, &y1, &x2, &y2);
					x1 /= fScale;
					x2 /= fScale;
					y1 /= fScale;
					y2 /= fScale;

					w = x2 - x1;
					h = y2 - y1;

					if ((word) && (w>2) && (h>2)){
						_OCR_RES res;
						res.rect = cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2));


						res.fConfidence = conf;
						wchar_t* tword = Utf8ToUnicode(word);
						res.strCode = tword;
						boundRect.push_back(res);

						delete [] tword;
					}

					

			} while (ri->Next(level));

	}

	if (cnt > 0)
		averConf /= cnt;
	return averConf;
}


wchar_t * COCRMng::Utf8ToUnicode(char* szU8)
{
	int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), NULL, 0);
	wchar_t* wszString = new wchar_t[wcsLen + 1];
	::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), wszString, wcsLen);
	wszString[wcsLen] = '\0';
	return wszString;
}