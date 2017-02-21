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

private:
	IplImage *m_pCut;
	bool m_IsReadyToSearch;
	RECT2D m_cutRect;

	float m_Threshold;
	float m_colorAccScale;

};

