#pragma once


// CZListCtrlLog
#define MSG_POST_SUBCLASS_LISTVIEW (WM_APP+50)

class CZListCtrlLog : public CListCtrl
{
	DECLARE_DYNAMIC(CZListCtrlLog)

public:
	CZListCtrlLog();
	virtual ~CZListCtrlLog();

	void InitListCtrl();


// ColumnHeader Class============================//
	class CCustomDrawHeaderCtrl : public CHeaderCtrl
	{
	public:
		CCustomDrawHeaderCtrl();
		virtual ~CCustomDrawHeaderCtrl();


	protected:
		CBrush m_brBackground;
		afx_msg LRESULT OnEraseBkgndOverride(WPARAM wParam, LPARAM lParam);
		afx_msg void OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
		DECLARE_MESSAGE_MAP()
	};
	CCustomDrawHeaderCtrl m_ctlHeader;
//===========================================//



protected:

	virtual void PreSubclassWindow();

	CEdit m_Edit;
	int nItem, nSubItem;

	afx_msg LRESULT OnPostSubclassListview(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};


