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
	InsertItems();

	/*
	m_List.DeleteAllItems();

	m_List.InsertColumn(0, _T("순번"), LVCFMT_LEFT, 40, -1);

	m_List.InsertColumn(1, _T("환자이름"), LVCFMT_LEFT, 70, -1);

	m_List.InsertColumn(2, _T("환자번호"), LVCFMT_LEFT, 70, -1);

	m_List.InsertColumn(3, _T("환자성별"), LVCFMT_LEFT, 70, -1);

	m_List.InsertColumn(4, _T("진료날짜"), LVCFMT_LEFT, 70, -1);

	m_List.InsertColumn(5, _T("사진파일"), LVCFMT_LEFT, 600, -1);

	//칼럼 추가 인덱스, 칼람명, 정렬방향, 칼럼길이, 서브아이템 갯수



	int seq = 0;

	CString seq_string;

	seq_string.Format(_T("%d"), seq);

	CString patientName = _T("홍길동");

	CString patientID = _T("365412");

	CString patientSex = _T("M");

	CString studyDate = _T("20120321");

	CString dcmFullPath = _T("D:\\DCM ROOT\\365412\\20123021\\홍길동.dcm");



	m_List.InsertItem(seq, seq_string);

	m_List.SetItem(seq, 1, LVIF_TEXT, patientName, 0, 0, 0, NULL);

	m_List.SetItem(seq, 2, LVIF_TEXT, patientID, 0, 0, 0, NULL);

	m_List.SetItem(seq, 3, LVIF_TEXT, patientSex, 0, 0, 0, NULL);

	m_List.SetItem(seq, 4, LVIF_TEXT, studyDate, 0, 0, 0, NULL);

	m_List.SetItem(seq, 5, LVIF_TEXT, dcmFullPath, 0, 0, 0, NULL);



	seq = 1;

	seq_string.Format(_T("%d"), seq);

	patientName = _T("박환자");

	patientID = _T("145632");

	patientSex = _T("F");

	studyDate = _T("20130215");

	dcmFullPath = _T("D:\\DCM ROOT\\145632\\20130215\\박환자.dcm");



	m_List.InsertItem(seq, seq_string);

	m_List.SetItem(seq, 1, LVIF_TEXT, patientName, 0, 0, 0, NULL);

	m_List.SetItem(seq, 2, LVIF_TEXT, patientID, 0, 0, 0, NULL);

	m_List.SetItem(seq, 3, LVIF_TEXT, patientSex, 0, 0, 0, NULL);

	m_List.SetItem(seq, 4, LVIF_TEXT, studyDate, 0, 0, 0, NULL);

	m_List.SetItem(seq, 5, LVIF_TEXT, dcmFullPath, 0, 0, 0, NULL);

	*/

}

void CZViewLog::InsertItems()
{
	HWND hWnd = m_List.GetSafeHwnd();

	// Set the LVCOLUMN structure with the required column information
	LVCOLUMN list;
	list.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT | LVCF_SUBITEM;
	list.fmt = LVCFMT_LEFT;
	list.cx = 50;
	list.pszText = L"S.No";
	list.iSubItem = 0;
	//Inserts the column
	::SendMessage(hWnd, LVM_INSERTCOLUMN, (WPARAM)0, (WPARAM)&list);

	list.cx = 100;
	list.pszText = L"Name";
	list.iSubItem = 1;
	::SendMessage(hWnd, LVM_INSERTCOLUMN, (WPARAM)1, (WPARAM)&list);

	list.cx = 100;
	list.pszText = L"Address";
	list.iSubItem = 2;
	::SendMessage(hWnd, LVM_INSERTCOLUMN, (WPARAM)1, (WPARAM)&list);

	list.cx = 100;
	list.pszText = L"Country";
	list.iSubItem = 2;
	::SendMessage(hWnd, LVM_INSERTCOLUMN, (WPARAM)1, (WPARAM)&list);

	// Inserts first Row with four column .
	SetCell(hWnd, L"1", 0, 0);
	SetCell(hWnd, L"Prabhakar", 0, 1);
	SetCell(hWnd, L"Hyderabad", 0, 2);
	SetCell(hWnd, L"India", 0, 3);

	// Inserts second Row with four columns .
	SetCell(hWnd, L"2", 1, 0);
	SetCell(hWnd, L"Uday", 1, 1);
	SetCell(hWnd, L"Chennai", 1, 2);
	SetCell(hWnd, L"India", 1, 3);

	// Inserts third Row with four column .
	SetCell(hWnd, L"3", 2, 0);
	SetCell(hWnd, L"Saradhi", 2, 1);
	SetCell(hWnd, L"Bangolore", 2, 2);
	SetCell(hWnd, L"India", 2, 3);

	// Inserts fourth Row with four column .
	SetCell(hWnd, L"4", 3, 0);
	SetCell(hWnd, L"Surya", 3, 1);
	SetCell(hWnd, L"Calcutta", 3, 2);
	SetCell(hWnd, L"India", 3, 3);


}


void CZViewLog::SetCell(HWND hWnd1, CString value, int nRow, int nCol)
{
	TCHAR     szString[256];
	wsprintf(szString, value, 0);

	//Fill the LVITEM structure with the values given as parameters.
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nRow;
	lvItem.pszText = szString;
	lvItem.iSubItem = nCol;
	if (nCol >0)
		//set the value of listItem
		::SendMessage(hWnd1, LVM_SETITEM, (WPARAM)0, (WPARAM)&lvItem);
	else
		//Insert the value into List
		ListView_InsertItem(hWnd1, &lvItem);

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