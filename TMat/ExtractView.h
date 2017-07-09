#pragma once
#include "OGLWnd.h"
#include "ZPageObject.h"
#include "Extractor.h"


class CExtractView :
	public COGLWnd
{
public:
	CExtractView();
	~CExtractView();

	void InitGLview(int _nWidth, int _nHeight);
	void MouseWheel(short zDelta);
	void Render();
	void Render2D();

	//===========================//
	void SetExtractImage(CZPageObject* pImg, RECT2D cutRect);// { m_pImg = pImg; }
	void ProcExtractTextBoundary();

	void ChangeXExpand(int _d);// { m_Extractor.ChangeXExpand(_d); }
	void ChangeYExpand(int _d);// { m_Extractor.ChangeYExpand(_d); }


private:
	POINT3D m_lookAt;
	unsigned short m_rectWidth, m_rectHeight;

	CZPageObject* m_pImg;
	GLuint m_eTexId;
	RECT2D m_cutRect;

	cv::Mat m_MatImg;
	CExtractor m_Extractor;
	CString m_strImgPath;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

