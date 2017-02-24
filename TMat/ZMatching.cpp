#include "stdafx.h"
#include "ZMatching.h"


CZMatching::CZMatching()
{
	m_pCut = NULL;
	m_IsReadyToSearch = false;

	m_Threshold = 0.75f;
	m_colorAccScale = 1.0f;
}




CZMatching::~CZMatching()
{
	if (m_pCut != NULL){
		cvReleaseImage(&m_pCut);
	}
}

void CZMatching::SetThreshold(float _th)
{
	if (_th == 1.0)
		_th = 0.9999f;

	m_Threshold = _th;
	m_colorAccScale = 1.0f / (1.0f - m_Threshold);
}

void CZMatching::PrepareCutNSearch(CZPageObject* pSelPage, RECT2D selRect)
{
	m_cutRect = selRect;
	m_IsReadyToSearch = false;
	if (pSelPage == NULL){
		return;
	}

	if (m_pCut != NULL){
		cvReleaseImage(&m_pCut);
		m_pCut = NULL;
	}

	USES_CONVERSION;
	char* sz = T2A(pSelPage->GetPath());
	IplImage *pSrc = cvLoadImage(sz);
	if (pSrc != NULL){
		IplImage *gray = cvCreateImage(cvSize(pSrc->width, pSrc->height), 8, 1);
		if (pSrc->nChannels == 3){
			cvCvtColor(pSrc, gray, CV_RGB2GRAY);//Change from RGB to GrayScale
		}
		else{
			cvCopy(pSrc, gray);
		}	
		
		m_pCut = cvCreateImage(cvSize(selRect.width, selRect.height), gray->depth, gray->nChannels);
		cvSetImageROI(gray, cvRect(selRect.x1, selRect.y1, selRect.width, selRect.height));		// posx, posy = left - top
		cvCopy(gray, m_pCut);
	//	cvShowImage("crop", m_pCut);
		m_IsReadyToSearch = true;
	}
}

bool CZMatching::DoSearch(unsigned int& sCnt)
{
	bool IsFinish = true;
	int search_size = m_pCut->width;
	if (search_size > m_pCut->height)		search_size = m_pCut->height;


	_vecPageObj pImgVec = SINGLETON_TMat::GetInstance()->GetImgVec();
	int cnt = 0;
	for (; sCnt < pImgVec.size(); sCnt++){
		if (pImgVec[sCnt]->GetThTex() == 0)
			continue;

		USES_CONVERSION;
		char* sz = T2A(pImgVec[sCnt]->GetPath());
		IplImage *gray = cvLoadImage(sz, CV_LOAD_IMAGE_GRAYSCALE);
		IplImage *result_img = cvCreateImage(cvSize(gray->width - m_pCut->width + 1, gray->height - m_pCut->height + 1),IPL_DEPTH_32F, 1);
		cvMatchTemplate(gray, m_pCut, result_img, CV_TM_CCOEFF_NORMED);

		float* d = (float*)result_img->imageData;		
		for (int y = 0; y<result_img->height; y++){
			for (int x = 0; x<result_img->width; x++){
				float fD = *(d + y*result_img->width + x);
				if (fD>m_Threshold)	{
					//POINT3D left_top;
					_MATCHInfo mInfo;
					mInfo.pos.x = x + m_pCut->width*0.5f;
					mInfo.pos.y = y + m_pCut->height*0.5f;
					mInfo.pos.z = 0;
					mInfo.accuracy = fD;
					mInfo.rect.set(x, x + m_pCut->width, y, y + m_pCut->height);

				//	mInfo.color = SINGLETON_TMat::GetInstance()->GetColor((fD)*1.1f);
					m_resColor.a = ((fD - m_Threshold)*m_colorAccScale) + 0.2f;
					mInfo.color = m_resColor;
					pImgVec[sCnt]->AddMatchedPoint(std::move(mInfo), search_size);
				}
			}
		}

		cvReleaseImage(&result_img);
		cvReleaseImage(&gray);

		cnt++;
		if (cnt>10){
			IsFinish = false;
			break;
		}
	}	
	return IsFinish;
}