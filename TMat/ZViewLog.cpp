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
	m_List.AddUserColumn(L"SEARCH ID", 50);
	m_List.AddUserColumn(L"CUT ID", 100);
	m_List.AddUserColumn(L"CUT FILE ID", 100);
	m_List.AddUserColumn(L"CUT POSITION", 100);
	m_List.AddUserColumn(L"MATCH ID", 100);
	m_List.AddUserColumn(L"MATCH FILE ID", 100);
	m_List.AddUserColumn(L"MATCH POSITION", 100);
	m_List.AddUserColumn(L"Threshold", 100);
	m_List.AddUserColumn(L"Accuracy", 100);


	//CString strItem[10];
	//strItem[0] = L"1";
	//strItem[1] = L"90.0";
	//strItem[2] = L"100";
	//strItem[3] = L"280";
	//strItem[4] = L"90";
	//strItem[5] = L"0.75";

	//m_List.AddRecode(strItem, 6);
	//m_List.AddRecode(strItem, 6);
	//m_List.AddRecode(strItem, 6);
	//m_List.AddRecode(strItem, 6);
	//m_List.AddRecode(strItem, 6);
}

void CZViewLog::ResetLogList()
{
	m_List.ResetListCtrl();
}
void CZViewLog::AddRecord()
{
	m_List.AddRecode();
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


void CZViewLog::SaveLogFile()
{
	CFileDialog dlg(FALSE, L"*.csv", NULL, OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT, L"CSV Files(*.csv)|*.csv|");
	if (dlg.DoModal() == IDOK)
	{
		CString m_strPath = dlg.GetPathName();

		FILE* fp = 0;
		char wBuff[1024];
		CString strRecord;
		fopen_s(&fp, (CStringA)m_strPath, "w");
		if (fp){
			strRecord = L"CUT_ID,CUT CUT_FILE_ID,CUT_POSITION,MATCH_ID,MATCH_FILE_ID,MATCH_POSITION,Threshold,Accuracy";
			int len = strRecord.GetLength() + 1;
			sprintf_s(wBuff, len, (CStringA)strRecord, fp);
			// Write Column //
			fwrite(wBuff, len, 1, fp);
			fwrite("\r\n", 1, 1, fp);

			for (int i = 0; i < m_List.GetItemCount(); i++){
				strRecord = m_List.GetItemText(i, 0);	strRecord += L",";
				strRecord += m_List.GetItemText(i, 1);	strRecord += L",";
				strRecord += m_List.GetItemText(i, 2);	strRecord += L",";
				strRecord += m_List.GetItemText(i, 3);	strRecord += L",";
				strRecord += m_List.GetItemText(i, 4);	strRecord += L",";
				strRecord += m_List.GetItemText(i, 5);	strRecord += L",";
				strRecord += m_List.GetItemText(i, 6);	strRecord += L",";
				strRecord += m_List.GetItemText(i, 7);	

				len = strRecord.GetLength() + 1;
				sprintf_s(wBuff, len, (CStringA)strRecord, fp);
				// Write Items //
				fwrite(wBuff, len, 1, fp);
				fwrite("\r\n", 1, 1, fp);
			}

			fclose(fp);
		}

		int a = 0;
	}
}



//m_List.AddUserColumn(L"CUT_ID", 100);
//m_List.AddUserColumn(L"CUT_FILE_ID", 100);
//m_List.AddUserColumn(L"CUT_POSITION", 100);
//m_List.AddUserColumn(L"MATCH_ID", 100);
//m_List.AddUserColumn(L"MATCH_FILE_ID", 100);
//m_List.AddUserColumn(L"MATCH_POSITION", 100);
//m_List.AddUserColumn(L"Threshold", 100);
//m_List.AddUserColumn(L"Accuracy", 100);


