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

private:
	IplImage *m_pCut;
	bool m_IsReadyToSearch;
	RECT2D m_cutRect;

	float m_Threshold;
	float m_colorAccScale;
	COLORf m_resColor;

};

