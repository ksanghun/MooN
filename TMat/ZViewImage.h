#pragma once
#include "OGLWnd.h"
#include "ZPageObject.h"


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
	void DrawCNSRect(float r, float g, float b, float a);
	
	void IDragMap(int x, int y, short sFlag);
	void InitCamera(bool movexy=true);
	void DrawImageByOrderForPicking();
	int SelectObject3D(int x, int y, int rect_width, int rect_height, int selmode);
	void EnableCutSearchMode(bool IsEnable);


	// key event //
	void MovePrePage();
	void MoveNextPage();
	void MoveNextDown();
	void MoveNextUp();
	void SetAnimation(bool _isAni) { m_isAnimation = _isAni; }
	void SetDBSearch(bool _isDB) { m_IsDBSearch = _isDB; }
	void ResetAllPages();

	CZPageObject* GetSelectedPageForCNS() { return m_pSelectPageForCNS; }
	RECT2D GetSelectedAreaForCNS();
	
	bool IsReadyForSearch();

private:

	

	POINT3D m_lookAt;
	unsigned short m_rectWidth, m_rectHeight;
	VA_DRAG_OPER m_dragOper;
	LOGFONT		m_LogFont;
//	BITMAPINFO* m_pBmpInfo;
	unsigned int m_addImgCnt;

	bool m_isAnimation;
	unsigned short m_mouseMode;
	CPoint m_stratPnt, m_moveVec;

	bool m_bIsCutNSearchMode;


	short m_nAniCnt;
	POINT3D  m_AniMoveVec;
	float  m_fAniMoveSca;
	CZPageObject* m_pSelectPageForCNS;

	

	// For Cut & Search  //
	POINT3D m_PN, m_PO, m_CNSRectStart, m_CNSRectEnd;

	bool m_IsDBSearch;
	int m_selWordId;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};

