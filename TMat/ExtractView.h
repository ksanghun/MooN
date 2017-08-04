#pragma once
#include "OGLWnd.h"
#include "ZPageObject.h"
#include "Extractor.h"
#include "ZDataManager.h"
#include "GuideLine.h"
#include "OCRMng.h"


enum _MOVE_TYPE {_CAM_MOVE, _ARROW_MOVE, _OBJ_MOVE };
enum _USER_EDIT_MODE{ __EDIT_NONE=0, __EDIT_SELECTION, __EDIT_ADDCHAR, __EDIT_ADDLINE};

class CExtractView :
	public COGLWnd
{
public:
	CExtractView();
	~CExtractView();

	void InitGLview(int _nWidth, int _nHeight);
	void InitCamera();
	void MouseWheel(short zDelta);
	void Render();
	void Render2D();
	void DrawGuideLines();
	void DrawExtractions();
	void DrawExtractionsForPick();
	void DrawExtractedLine();
	void DrawExtractedLineForPick();

	//===========================//
	void SetExtractImage(CZPageObject* _pImg, RECT2D cutRect);// { m_pImg = pImg; }
	void ProcExtractTextBoundary();
	void ContractImage(cv::Mat& img);

	void ChangeXExpand(int _d);// { m_Extractor.ChangeXExpand(_d); }
	void ChangeYExpand(int _d);// { m_Extractor.ChangeYExpand(_d); }

	void DrawSearchRect();
	void DrawCNSRect(float r, float g, float b, float a);

	void IDragMap(int x, int y, short sFlag);

	void DoFineExtractionText(_TEXT_ORDER order);
	void DoExtractionWord(_TEXT_ORDER order);
	void DoExtractionLine(_TEXT_ORDER order);
	void DoExtractionText(_TEXT_ORDER order);
	
	float MatchingCutImgs(IplImage* pCut, IplImage* dst);
	void InsertExtrationIntoDB();

	void SetLanguageType(_LANGUAGE_TYPE ltype);
	void SetVertiCharSpace(int _v);
	void SetHoriCharSpze(int _h);



	void GroupingExtractions();
	void CutNSearchExtractions();
	GLuint Load4ChannelImage(char* sz);
	int SelectObject3D(int x, int y, int rect_width, int rect_height, int selmode);

	bool IsSelectedRect(int _id);
	void SetUserEditMode(_USER_EDIT_MODE _bMode) { m_bUserEditMode = _bMode; }
	void DeleteExtSelections();
	cv::Rect GetNomalizedSize(int averArea, cv::Rect rect);
	cv::Rect GetNomalizedWordSize(cv::Rect rect);
private:
	POINT3D m_lookAt;
	unsigned short m_rectWidth, m_rectHeight;
	VA_DRAG_OPER m_dragOper;
	_MOVE_TYPE m_mouseMode;
	CPoint m_stratPnt, m_moveVec;

	CZPageObject* m_pImg;
	CZPageObject* m_pSelectcImg;
	GLuint m_eTexId;
	RECT2D m_cutRect;

	cv::Mat m_MatImg;
	cv::Mat m_MatOImg;

//===OCR Manager=============//
	COCRMng m_OCRMng;

	CExtractor m_Extractor;
	CString m_strImgPath;

	POINT3D m_posSearchBox;


	std::map<unsigned long, WORD_RECORD> m_mapExtractWord;

	CGuideLine m_guideLine[4];// 0: hori_01 1: hori_02: 2: verti_01, 3:verti_02
	int m_selGuideLineId;
	bool m_bCameraMove;
	POINT3D m_startArrowPos;
	_USER_EDIT_MODE m_bUserEditMode;

	POINT3D m_PN, m_PO, m_CNSRectStart, m_CNSRectEnd;


	LOGFONT		m_LogFont;
	BITMAPINFO* m_pBmpInfo;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};

