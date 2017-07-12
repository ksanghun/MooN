#pragma once
#include "ExtractView.h"

// CDlgExtractTool dialog

class CDlgExtractTool : public CDialog
{
	DECLARE_DYNAMIC(CDlgExtractTool)

public:
	CDlgExtractTool(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgExtractTool();


	void SetExtractImage(CZPageObject* pImg, RECT2D rect);

// Dialog Data
	enum { IDD = IDD_DLG_EXTRACT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();


private:
	CExtractView* m_pExtView;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNcDestroy();
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnBnClickedBnExtract();
	afx_msg void OnBnClickedBnYdec();
	afx_msg void OnBnClickedBnYincre();
	afx_msg void OnBnClickedBnXdec();
	afx_msg void OnBnClickedBnXincre();
};
