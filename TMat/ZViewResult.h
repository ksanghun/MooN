#pragma once
#include "OGLWnd.h"
class CZViewResult :
	public COGLWnd
{
public:
	CZViewResult();
	~CZViewResult();

	void InitGLview(int _nWidth, int _nHeight);
	void MouseWheel(short zDelta);
	void Render();
	void Render2D();

private:
	POINT3D m_lookAt;
	VA_DRAG_OPER m_dragOper;
	LOGFONT		m_LogFont;
	BITMAPINFO* m_pBmpInfo;
};

