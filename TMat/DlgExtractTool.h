#pragma once
#include "ExtractView.h"
#include "afxcmn.h"

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
	UINT m_nTimer;

	//CSliderCtrl m_sliderWidth;
	//CSliderCtrl m_sliderHeight;

	int m_sWidth, m_sHeight;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL Create(CWnd* pParentWnd = NULL);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNcDestroy();
	afx_msg void OnPaint();
	afx_msg void OnClose();
	//afx_msg void OnBnClickedBnExtract();
	//afx_msg void OnBnClickedBnYdec();
	//afx_msg void OnBnClickedBnYincre();
	//afx_msg void OnBnClickedBnXdec();
	//afx_msg void OnBnClickedBnXincre();

//	afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	//afx_msg void OnNMCustomdrawSliderWidth(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnNMReleasedcaptureSliderWidth(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnNMCustomdrawSliderHeight(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnNMReleasedcaptureSliderHeight(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBnClickedBnDetectLines();
	int m_textOrder;
	BOOL m_bVertical;
	BOOL m_bHorizontal;
	afx_msg void OnBnClickedCheckVorder();
	afx_msg void OnBnClickedCheckHorder();
	afx_msg void OnBnClickedBnExtractText();
	afx_msg void OnBnClickedBnFineExtract();


//	CSliderCtrl m_sliserSpaceChar;
	BOOL m_bAlphabetic;
	BOOL m_bNonAlphabetic;
	afx_msg void OnBnClickedCheckAlphabet();
	afx_msg void OnBnClickedCheckNonAlphabet();
//	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderCharSpace(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderCharSpace(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_sliderVertiSpace;
	CSliderCtrl m_sliderHoriSpace;
	afx_msg void OnNMCustomdrawSliderCharSpaceH(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderCharSpaceH(NMHDR *pNMHDR, LRESULT *pResult);
	int m_nVertCharSpace;
	int m_nHoriCharSpace;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
