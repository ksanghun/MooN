#pragma once
#include "OGLWnd.h"
#define ANI_FRAME_CNT 10


class CZViewImage :	public COGLWnd
{
public:
	CZViewImage();
	virtual ~CZViewImage();

	void InitGLview(int _nWidth, int _nHeight);
	void MouseWheel(short zDelta);
	void GenerateThumbnail();
	void ProcGenerateThumbnail();
	void Render();
	void Render2D();

	void DrawBGPageAni();
	void DrawBGPage();
	
	void IDragMap(int x, int y, short sFlag);
	void InitCamera();
private:
	POINT3D m_lookAt;
	unsigned short m_rectWidth, m_rectHeight;
	VA_DRAG_OPER m_dragOper;
	LOGFONT		m_LogFont;
	BITMAPINFO* m_pBmpInfo;
	unsigned int m_addImgCnt;

	bool m_isAnimation;
	unsigned short m_mouseMode;
	CPoint m_stratPnt, m_moveVec;
	short m_nAniCnt;
	float m_fAniAcceration[ANI_FRAME_CNT];

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

