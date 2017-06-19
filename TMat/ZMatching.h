#pragma once

#include "ZDataManager.h"

class CZMatching
{
public:
	CZMatching();
	~CZMatching();

	void PrepareCutNSearch(CZPageObject* pSelPage, RECT2D selRect);
	void SetCutImage(IplImage* pCut);
	bool DoSearch(unsigned int& sCnt);
	void SetThreshold(float _th);
	void SetResColor(POINT3D _color);// { m_resColor.r = _color.x; m_resColor.g = _color.y; m_resColor.b = _color.z;  m_resColor.a = 1.0f; }

	unsigned int GetCutId(){ return m_iCutId; }
	unsigned int GetFileId() { return m_iFileId; }
	unsigned int GetPosId() { return m_iPosId; }
	float GetThreshold(){ return m_Threshold; }

private:
	IplImage *m_pCut;
	bool m_IsReadyToSearch;
	RECT2D m_cutRect;

	float m_Threshold;
	float m_colorAccScale;
	COLORf m_resColor;


	unsigned int m_iCutId, m_iFileId, m_iPosId;

	void NomalizeCutImage(IplImage* pSrc, IplImage* pCut, RECT2D cutRect, unsigned short norSize);
	RECT2D FitCutImageRect(IplImage* pSrc,  RECT2D& cutRect);
	bool FindHorizonEage(IplImage* pSrc, RECT2D& cutRect, int type, int direction);
	bool FindVerticalEage(IplImage* pSrc, RECT2D& cutRect, int type, int direction);

};

