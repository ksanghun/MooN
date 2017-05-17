// ZListCtrlLog.cpp : implementation file
//

#include "stdafx.h"
#include "TMat.h"
#include "ZListCtrlLog.h"


// CZListCtrlLog

IMPLEMENT_DYNAMIC(CZListCtrlLog, CListCtrl)

CZListCtrlLog::CZListCtrlLog()
{

}

CZListCtrlLog::~CZListCtrlLog()
{
}


BEGIN_MESSAGE_MAP(CZListCtrlLog, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, &CZListCtrlLog::OnNMClick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CZListCtrlLog::OnNMCustomdraw)
	ON_MESSAGE(MSG_POST_SUBCLASS_LISTVIEW, OnPostSubclassListview)
END_MESSAGE_MAP()

void CZListCtrlLog::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();

	DWORD dwStyles = GetExtendedStyle();
	dwStyles |= LVS_EX_FULLROWSELECT;
	SetExtendedStyle(dwStyles);

	PostMessage(MSG_POST_SUBCLASS_LISTVIEW, 0L, 0L);
}



LRESULT CZListCtrlLog::OnPostSubclassListview(WPARAM wParam, LPARAM lParam)
{
	m_ctlHeader.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());
	m_ctlHeader.Invalidate(TRUE);

	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CCustomDrawHeaderCtrl

CZListCtrlLog::CCustomDrawHeaderCtrl::CCustomDrawHeaderCtrl()
{
	m_brBackground.CreateSolidBrush(RGB(200, 100, 50));
}

CZListCtrlLog::CCustomDrawHeaderCtrl::~CCustomDrawHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CZListCtrlLog::CCustomDrawHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CListCtrlColoredHeader::CCustomDrawHeaderCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, CZListCtrlLog::CCustomDrawHeaderCtrl::OnCustomdraw)
	ON_MESSAGE(WM_ERASEBKGND, CZListCtrlLog::CCustomDrawHeaderCtrl::OnEraseBkgndOverride)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomDrawHeaderCtrl message handlers


// CZListCtrlLog message handlers
void CZListCtrlLog::InitListCtrl()
{
	m_Edit.Create(WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL , CRect(0, 0, 800, 500), this, NULL);
	m_Edit.ShowWindow(SW_HIDE);
}




void CZListCtrlLog::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here

	Invalidate();
	HWND hWnd1 = GetSafeHwnd();
	LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE)pNMHDR;
	RECT rect;
	//get the row number
	nItem = temp->iItem;
	//get the column number
	nSubItem = temp->iSubItem;
	if (nSubItem == 0 || nSubItem == -1 || nItem == -1)
		return;
	//Retrieve the text of the selected subItem from the list
	CString str = GetItemText(nItem, nSubItem);

	RECT rect1, rect2;
	// this macro is used to retrieve the Rectanle of the selected SubItem
	ListView_GetSubItemRect(hWnd1, temp->iItem, temp->iSubItem, LVIR_BOUNDS, &rect);
	//Get the Rectange of the listControl
	::GetWindowRect(temp->hdr.hwndFrom, &rect1);
	//Get the Rectange of the Dialog
	::GetWindowRect(m_hWnd, &rect2);

	int x = rect1.left - rect2.left;
	int y = rect1.top - rect2.top;

	if (nItem != -1)
		m_Edit.SetWindowPos(NULL, rect.left + x + 1, rect.top, rect.right - rect.left - 1, rect.bottom - rect.top - 1, NULL);
	m_Edit.ShowWindow(SW_SHOW);
	m_Edit.SetFocus();
	::Rectangle(::GetDC(temp->hdr.hwndFrom), rect.left, rect.top - 1, rect.right, rect.bottom);
	m_Edit.SetWindowTextW(str);

	*pResult = 0;
}


BOOL CZListCtrlLog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			CWnd* pwndCtrl = GetFocus();
			if (pwndCtrl->IsKindOf(RUNTIME_CLASS(CEdit))){
				CString str;
				m_Edit.GetWindowText(str);
				SetItem(nItem, nSubItem, LVIF_TEXT, str, 0, 0, 0, NULL);
				m_Edit.ShowWindow(SW_HIDE);
			}
		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}


void CZListCtrlLog::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;


}


void CZListCtrlLog::CCustomDrawHeaderCtrl::OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pCDraw = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	// Take the default processing unless we set this to something else below

	*pResult = CDRF_DODEFAULT;

	switch (pCDraw->dwDrawStage)
	{
	case CDDS_PREPAINT:
	{
		// the item's prepaint stage: return CDRF_NOTIFYITEMDRAW so that we 
		// get future WM_CUSTOMDRAW messages
		*pResult = CDRF_NOTIFYITEMDRAW;

		break;
	}

	case CDDS_ITEMPREPAINT:
	{
		// the item's pre-paint stage: change the DC settings to get needed text colors
		CDC dc;
		dc.Attach(pCDraw->hdc);		

		dc.SetTextColor(RGB(0, 0, 255));

		dc.Detach();

		*pResult = CDRF_DODEFAULT;

		break;
	}

	case CDDS_ITEMPOSTPAINT:
	{
		// the item's postpaint stage: draw on top of whatever is there

		*pResult = CDRF_DODEFAULT;

		break;
	}
	}

}

LRESULT CZListCtrlLog::CCustomDrawHeaderCtrl::OnEraseBkgndOverride(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}