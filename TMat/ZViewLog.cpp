// ZViewLog.cpp : implementation file
//

#include "stdafx.h"
#include "TMat.h"
#include "ZViewLog.h"


// CZViewLog

IMPLEMENT_DYNAMIC(CZViewLog, CWnd)

CZViewLog::CZViewLog()
{

}

CZViewLog::~CZViewLog()
{
}


BEGIN_MESSAGE_MAP(CZViewLog, CWnd)
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CZViewLog message handlers
void CZViewLog::InitView(int width, int height)
{
	m_List.Create(	WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_EDITLABELS,CRect(0, 0, width, height), this, NULL);	
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_List.InitListCtrl();
	m_List.AddUserColumn(L"Search No.", 100);
	m_List.AddUserColumn(L"Accuracy", 100);
	m_List.AddUserColumn(L"Pos_x", 100);
	m_List.AddUserColumn(L"Pos_y", 100);
	m_List.AddUserColumn(L"Page No.", 100);
	m_List.AddUserColumn(L"Threshold", 100);


	CString strItem[10];
	strItem[0] = L"1";
	strItem[1] = L"90.0";
	strItem[2] = L"100";
	strItem[3] = L"280";
	strItem[4] = L"90";
	strItem[5] = L"0.75";

	m_List.AddRecode(strItem, 6);
	m_List.AddRecode(strItem, 6);
	m_List.AddRecode(strItem, 6);
	m_List.AddRecode(strItem, 6);
	m_List.AddRecode(strItem, 6);
}

void CZViewLog::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}

void CZViewLog::ResizeView(int width, int height)
{
	if (m_List){
		m_List.MoveWindow(0, 0, width, height);
	}

}

void CZViewLog::AddNewColumn()
{
	CDlgAddCol dlg;
	if (dlg.DoModal() == IDOK){
		if (m_List){
			m_List.AddUserColumn(dlg.m_strColName, dlg.m_intColWidth);
		}
	}
}




