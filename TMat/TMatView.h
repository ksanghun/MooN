
// TMatView.h : interface of the CTMatView class
//

#pragma once

#include "ZViewImage.h"
#include "ZViewResult.h"
#include "ZMatching.h"

class CDragDropTreeCtrl;
class CTMatView : public CView
{
protected: // create from serialization only
	CTMatView();
	DECLARE_DYNCREATE(CTMatView)

// Attributes
public:
//	CTMatDoc* GetDocument() const;

	void SetTreeDragItem(CImageList* pImage, HTREEITEM hItem, CDragDropTreeCtrl* pCtrl);
	short ProcSetSelectedItem(HTREEITEM hItem, CDragDropTreeCtrl* pCtrl);
	short SetSelectedItem(HTREEITEM hItem, CDragDropTreeCtrl* pCtrl, float& offset);
	void InitCamera(bool bmovexy);

	void DoCurNSearch();
	void SetThresholdValue(float _th);
	void SetResultColor(int R, int G, int B);

	CZViewImage* GetViewImage() { return m_pViewImage; }

// Operations
public:

private:
	CMFCTabCtrl m_ctrlTab;
	CZViewImage* m_pViewImage;
	CZViewResult* m_pViewResult;
	CZMatching* m_pMatchingProcessor;

	unsigned int m_searchCnt;


	void AddImageData(HTREEITEM _item, CDragDropTreeCtrl* pCtrl, int& cnt);
// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);


// Implementation
public:
	virtual ~CTMatView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#ifndef _DEBUG  // debug version in TMatView.cpp
//inline CTMatDoc* CTMatView::GetDocument() const
//   { return reinterpret_cast<CTMatDoc*>(m_pDocument); }
#endif

