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
	if ((pSelPage == NULL) || (selRect.width==0)){
		return;
	}

	if (m_pCut != NULL){
		cvReleaseImage(&m_pCut);
		m_pCut = NULL;
	}

	CString strpath = pSelPage->GetPath();
	USES_CONVERSION;
	char* sz = T2A(strpath);
	IplImage *pSrc = SINGLETON_TMat::GetInstance()->LoadIplImage(strpath, 1);

	if (pSrc != NULL){
		m_pCut = cvCreateImage(cvSize(selRect.width, selRect.height), pSrc->depth, pSrc->nChannels);
		cvSetImageROI(pSrc, cvRect(selRect.x1, selRect.y1, selRect.width, selRect.height));		// posx, posy = left - top
		cvCopy(pSrc, m_pCut);
		m_IsReadyToSearch = true;


		m_iFileId = getHashCode((CStringA)pSelPage->GetPath());
		m_iPosId = (int)selRect.x1 * 10000 + (int)selRect.y1;
		

		CString strId;
		strId.Format(L"%u%u", m_iFileId, m_iPosId);
		m_iCutId = getHashCode((CStringA)strId);
						
	}
	cvReleaseImage(&pSrc);

	

}

bool CZMatching::DoSearch(unsigned int& sCnt)
{
	if (m_pCut == NULL)
		return false;

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
			IplImage *gray = SINGLETON_TMat::GetInstance()->LoadIplImage(pImgVec[sCnt]->GetPath(), 1);
			IplImage *result_img = cvCreateImage(cvSize(gray->width - m_pCut->width + 1, gray->height - m_pCut->height + 1), IPL_DEPTH_32F, 1);
			cvMatchTemplate(gray, m_pCut, result_img, CV_TM_CCOEFF_NORMED);

			float* d = (float*)result_img->imageData;
			for (int y = 0; y < result_img->height; y++){
				for (int x = 0; x < result_img->width; x++){
					float fD = *(d + y*result_img->width + x);
					if (fD > m_Threshold)	{
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
			if (cnt > 10){
				IsFinish = false;
				break;
			}
		}
		return IsFinish;

}

void CZMatching::SetResColor(POINT3D _color) 
{ 
	m_resColor.r = _color.x; m_resColor.g = _color.y; m_resColor.b = _color.z;  m_resColor.a = 1.0f; 
	SINGLETON_TMat::GetInstance()->SetResColor(m_resColor);
}