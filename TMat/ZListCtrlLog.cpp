// ZListCtrlLog.cpp : implementation file
//

#include "stdafx.h"
#include "TMat.h"
#include "ZListCtrlLog.h"
#include "resource.h"

// CZListCtrlLog
#include "ZDataManager.h"


IMPLEMENT_DYNAMIC(CZListCtrlLog, CListCtrl)

CZListCtrlLog::CZListCtrlLog()
{
	m_nCulNum = m_nRecordNum = 0;
	m_SelectionFlag = FALSE;
}

CZListCtrlLog::~CZListCtrlLog()
{
}


BEGIN_MESSAGE_MAP(CZListCtrlLog, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, &CZListCtrlLog::OnNMClick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CZListCtrlLog::OnNMCustomdraw)
	ON_MESSAGE(MSG_POST_SUBCLASS_LISTVIEW, OnPostSubclassListview)

	ON_NOTIFY_REFLECT(NM_DBLCLK, &CZListCtrlLog::OnNMDblclk)
	ON_WM_LBUTTONDOWN()
	ON_WM_CONTEXTMENU()
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
	//m_Edit.Create(WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL , CRect(0, 0, 800, 500), this, NULL);
	//m_Edit.ShowWindow(SW_HIDE);

	
	
}


void CZListCtrlLog::ResetListCtrl()
{
	DeleteAllItems();
	m_nRecordNum = 0;

}



void CZListCtrlLog::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
//	Invalidate();
//	HWND hWnd1 = GetSafeHwnd();
//	LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE)pNMHDR;
//	RECT rect;
//	//get the row number
//	nItem = temp->iItem;
//	//get the column number
//	nSubItem = temp->iSubItem;
//	if (nSubItem == 0 || nSubItem == -1 || nItem == -1){
//		*pResult = 0;
////		m_Edit.ShowWindow(SW_HIDE);
//		return;
//	}
//	//Retrieve the text of the selected subItem from the list
//	CString str = GetItemText(nItem, nSubItem);
//
//	RECT rect1, rect2;
//	// this macro is used to retrieve the Rectanle of the selected SubItem
//	ListView_GetSubItemRect(hWnd1, temp->iItem, temp->iSubItem, LVIR_BOUNDS, &rect);
//	//Get the Rectange of the listControl
//	::GetWindowRect(temp->hdr.hwndFrom, &rect1);
//	//Get the Rectange of the Dialog
//	::GetWindowRect(m_hWnd, &rect2);
//
//	int x = rect1.left - rect2.left;
//	int y = rect1.top - rect2.top;

	//if (nItem != -1)
	//	m_Edit.SetWindowPos(NULL, rect.left + x + 1, rect.top, rect.right - rect.left - 1, rect.bottom - rect.top - 1, NULL);
	//m_Edit.ShowWindow(SW_SHOW);
	//m_Edit.SetFocus();
	//::Rectangle(::GetDC(temp->hdr.hwndFrom), rect.left, rect.top - 1, rect.right, rect.bottom);
	//m_Edit.SetWindowTextW(str);

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
//				m_Edit.GetWindowText(str);
				//SetItem(nItem, nSubItem, LVIF_TEXT, str, 0, 0, 0, NULL);
				//m_Edit.ShowWindow(SW_HIDE);
			}
		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}


void CZListCtrlLog::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW  pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = CDRF_DODEFAULT;
	//obtain row and column of item


	//Remove standard highlighting of selected (sub)item.
	pNMCD->nmcd.uItemState = CDIS_DEFAULT;

	switch (pNMCD->nmcd.dwDrawStage)
	{
	case  CDDS_PREPAINT:  // First stage (for the whole control)
	{

		*pResult = CDRF_NOTIFYITEMDRAW;

	}
		break;

	case CDDS_ITEMPREPAINT:
	{

		*pResult = CDRF_NOTIFYSUBITEMDRAW;

	}
		break;

	case  CDDS_ITEMPREPAINT | CDDS_SUBITEM: // Stage three 
	{

		//if (sub)item is of interest, set custom text/background color
		int itemid = (pNMCD->nmcd).dwItemSpec; //this is item id
		int sid = _wtoi(GetItemText(itemid, 0));

		if (sid % 2 == 0){
			pNMCD->clrText = RGB(0, 0, 0);
			pNMCD->clrTextBk = RGB(210, 210, 210);
		}
		else{
			pNMCD->clrText = RGB(0, 0, 0);
			pNMCD->clrTextBk = RGB(250, 250, 250);
		}




		*pResult = CDRF_NOTIFYPOSTPAINT;

	}
		break;

	case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM: // Stage four (called for each subitem of the focused item)
	{

	}
		break;

	default:// it wasn't a notification that was interesting to us.
	{
		*pResult = CDRF_DODEFAULT;
	}
		break;
	}

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

void CZListCtrlLog::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO:  Add your code to draw the specified item
	/*
	TCHAR  lpBuffer[256];

	LV_ITEM lvi;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.iItem = lpDrawItemStruct->itemID;
	lvi.iSubItem = 0;
	lvi.pszText = lpBuffer;
	lvi.cchTextMax = sizeof(lpBuffer);
	VERIFY(GetItem(&lvi));

	LV_COLUMN lvc, lvcprev;
	::ZeroMemory(&lvc, sizeof(lvc));
	::ZeroMemory(&lvcprev, sizeof(lvcprev));
	lvc.mask = LVCF_WIDTH | LVCF_FMT;
	lvcprev.mask = LVCF_WIDTH | LVCF_FMT;


	CDC* pDC;
	pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	int nCol;
	if (!m_SelectionFlag)
	{
		for (nCol = 0; GetColumn(nCol, &lvc); nCol++)
		{
			if (nCol>0)
			{
				GetSubItemRect(lpDrawItemStruct->itemID, nCol, LVIR_BOUNDS, m_SelectionRect);
			}
			else
			{
				GetItemRect(lpDrawItemStruct->itemID, m_SelectionRect, LVIR_BOUNDS);
				m_SelectionRect.right = GetColumnWidth(0);
				m_SelectionRect.left = 0;
			}
			if (m_SelectionRect.PtInRect(m_Point))
			{
				m_SelectionFlag = TRUE;
				//				TRACE("\n***%d,%d",lpDrawItemStruct->itemID,nCol);
				break;
			}
			else
				m_SelectionFlag = FALSE;

		}



		if ((lpDrawItemStruct->itemState & ODS_SELECTED) && m_SelectionFlag)
		{
			CRect rc = lpDrawItemStruct->rcItem;

			rc.left = m_SelectionRect.left;
			rc.right = m_SelectionRect.right;

			pDC->FillSolidRect(&rc, RGB(255, 0, 0));//GetSysColor(COLOR_HIGHLIGHT)) ; 
			//pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT)) ;
		}
		else
		{
			pDC->FillSolidRect(&lpDrawItemStruct->rcItem, GetSysColor(COLOR_WINDOW));
			pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		}

	}

	for (nCol = 0; GetColumn(nCol, &lvc); nCol++)
	{
		if (nCol > 0)
		{
			// Get Previous Column Width in order to move the next display item
			GetColumn(nCol - 1, &lvcprev);
			lpDrawItemStruct->rcItem.left += lvcprev.cx;
			lpDrawItemStruct->rcItem.right += lpDrawItemStruct->rcItem.left;
		}

		// Get the text 
		::ZeroMemory(&lvi, sizeof(lvi));
		lvi.iItem = lpDrawItemStruct->itemID;
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iSubItem = nCol;
		lvi.pszText = lpBuffer;
		lvi.cchTextMax = sizeof(lpBuffer);
		VERIFY(GetItem(&lvi));

		pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));

		UINT		uFormat = DT_LEFT;

		::DrawText(lpDrawItemStruct->hDC, lpBuffer, wcslen(lpBuffer), &lpDrawItemStruct->rcItem, uFormat);

		pDC->SelectStockObject(SYSTEM_FONT);
	}
	*/
}



BOOL CZListCtrlLog::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	//cs.style |= LVS_REPORT | LVS_OWNERDRAWFIXED | LVS_SINGLESEL | WS_BORDER;

	return CListCtrl::PreCreateWindow(cs);
}




void CZListCtrlLog::AddUserColumn(CString strLable, unsigned short colWidth)
{
	InsertColumn(m_nCulNum, strLable, LVCFMT_LEFT, colWidth, -1);
	m_nCulNum++;
}
void CZListCtrlLog::AddRecode()
{
	//InsertItem(m_nRecordNum, strItem[0]);
	//for (int i = 1; i < itemNum; i++){
	//	SetItem(m_nRecordNum, i, LVIF_TEXT, strItem[i], 0, 0, 0, NULL);
	//}

	ResetListCtrl();


	CString strItem;
	std::map<unsigned long, MATCHGROUP>& matches = SINGLETON_TMat::GetInstance()->GetMatchResults();
	std::map<unsigned long, MATCHGROUP>::iterator iter_gr = matches.begin();

	for (; iter_gr != matches.end(); iter_gr++){

		for (int i = 0; i < iter_gr->second.matche.size(); i++){

			strItem.Format(L"%d", iter_gr->second.searchId);
			InsertItem(m_nRecordNum, strItem);

			strItem.Format(L"%u", iter_gr->second.matche[i].cutId);
			SetItem(m_nRecordNum, 1, LVIF_TEXT, strItem, 0, 0, 0, NULL);

			strItem.Format(L"%u", iter_gr->second.matche[i].fileId);
			SetItem(m_nRecordNum, 2, LVIF_TEXT, strItem, 0, 0, 0, NULL);

			strItem.Format(L"%u", iter_gr->second.matche[i].posId);
			SetItem(m_nRecordNum, 3, LVIF_TEXT, strItem, 0, 0, 0, NULL);

			strItem.Format(L"%d%u", (int)(iter_gr->second.matche[i].accuracy * 100),iter_gr->second.matche[i].matchId);
			SetItem(m_nRecordNum, 4, LVIF_TEXT, strItem, 0, 0, 0, NULL);

			strItem.Format(L"%u", iter_gr->second.matche[i].matchFile);
			SetItem(m_nRecordNum, 5, LVIF_TEXT, strItem, 0, 0, 0, NULL);

			strItem.Format(L"%u", iter_gr->second.matche[i].matchPos);
			SetItem(m_nRecordNum, 6, LVIF_TEXT, strItem, 0, 0, 0, NULL);

			strItem.Format(L"%3.2f", iter_gr->second.matche[i].fTh);
			SetItem(m_nRecordNum, 7, LVIF_TEXT, strItem, 0, 0, 0, NULL);

			strItem.Format(L"%3.2f", iter_gr->second.matche[i].accuracy);
			SetItem(m_nRecordNum, 8, LVIF_TEXT, strItem, 0, 0, 0, NULL);

			SetItem(m_nRecordNum, 9, LVIF_TEXT, L"-", 0, 0, 0, NULL);

			SetItem(m_nRecordNum, 10, LVIF_TEXT, iter_gr->second.matche[i].strBase64, 0, 0, 0, NULL);

			m_nRecordNum++;
		}
		
	}

}

void CZListCtrlLog::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	//Invalidate();
	//HWND hWnd1 = GetSafeHwnd();
	//LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE)pNMHDR;
	//RECT rect;
	////get the row number
	//nItem = temp->iItem;
	////get the column number
	//nSubItem = temp->iSubItem;
	//if (nSubItem == 0 || nSubItem == -1 || nItem == -1){
	//	*pResult = 0;
	//	m_Edit.ShowWindow(SW_HIDE);
	//	return;
	//}
	////Retrieve the text of the selected subItem from the list
	//CString str = GetItemText(nItem, nSubItem);

	//RECT rect1, rect2;
	//// this macro is used to retrieve the Rectanle of the selected SubItem
	//ListView_GetSubItemRect(hWnd1, temp->iItem, temp->iSubItem, LVIR_BOUNDS, &rect);
	////Get the Rectange of the listControl
	//::GetWindowRect(temp->hdr.hwndFrom, &rect1);
	////Get the Rectange of the Dialog
	//::GetWindowRect(m_hWnd, &rect2);

	//int x = rect1.left - rect2.left;
	//int y = rect1.top - rect2.top;

	//if (nItem != -1)
	//	m_Edit.SetWindowPos(NULL, rect.left + x + 1, rect.top, rect.right - rect.left - 1, rect.bottom - rect.top - 1, NULL);
	//m_Edit.ShowWindow(SW_SHOW);
	//m_Edit.SetFocus();
	//::Rectangle(::GetDC(temp->hdr.hwndFrom), rect.left, rect.top - 1, rect.right, rect.bottom);
	//m_Edit.SetWindowTextW(str);

	*pResult = 0;
}


void CZListCtrlLog::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//m_Point = point;
	//m_SelectionFlag = FALSE;
	//Invalidate();

	//CString str;
	//m_Edit.GetWindowText(str);
	//SetItem(nItem, nSubItem, LVIF_TEXT, str, 0, 0, 0, NULL);
//	m_Edit.ShowWindow(SW_HIDE);

	CListCtrl::OnLButtonDown(nFlags, point);
}


void CZListCtrlLog::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	CMenu menu;
	menu.LoadMenuW(IDR_POPUP_MENU);
	CMenu* pMenu = menu.GetSubMenu(0);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
}
