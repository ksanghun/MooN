// ZViewLog.cpp : implementation file
//

#include "stdafx.h"
#include "TMat.h"
#include "ZViewLog.h"
#include "ZDataManager.h"


// CZViewLog

IMPLEMENT_DYNAMIC(CZViewLog, CWnd)

CZViewLog::CZViewLog()
{
	m_writeBuffer = new char[1024 * 100];
}

CZViewLog::~CZViewLog()
{
	delete[] m_writeBuffer;
}


BEGIN_MESSAGE_MAP(CZViewLog, CWnd)
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CZViewLog message handlers
void CZViewLog::InitView(int width, int height)
{	
	m_List.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT , CRect(0, 0, width, height), this, NULL);
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);


	m_List.EnableScrollBarCtrl(SB_HORZ);
	m_List.ShowScrollBar(SB_HORZ);
	m_List.SetScrollRange(SB_HORZ, 0, 2000);

	m_List.InitListCtrl();
	m_List.AddUserColumn(L"CUT", _NORMALIZE_SIZE_W);
	m_List.AddUserColumn(L"CODE", 100);
	m_List.AddUserColumn(L"SEARCH ID", 120);
	m_List.AddUserColumn(L"CUT ID", 100);
	m_List.AddUserColumn(L"CUT FILE ID", 100);
	m_List.AddUserColumn(L"CUT POSITION", 100);
	m_List.AddUserColumn(L"MATCH ID", 100);
	m_List.AddUserColumn(L"MATCH FILE ID", 100);
	m_List.AddUserColumn(L"MATCH POSITION", 100);
	m_List.AddUserColumn(L"Threshold", 100);
	m_List.AddUserColumn(L"Accuracy", 100);	
	m_List.AddUserColumn(L"BASE64", 1000);


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


	UINT nFlags = ILC_MASK;
	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_imgList.Create(_NORMALIZE_SIZE_W, _NORMALIZE_SIZE_H, nFlags, 0, 0);
	m_List.SetImageList(&m_imgList, LVSIL_SMALL);
}

void CZViewLog::ResetLogList()
{	
	m_imgList.DeleteImageList();

	UINT nFlags = ILC_MASK;
	m_imgList.Create(_NORMALIZE_SIZE_W, _NORMALIZE_SIZE_H, nFlags, 0, 0);

	m_List.ResetListCtrl();
}
void CZViewLog::AddRecord()
{
//	m_List.ResetListCtrl();
	m_nRecordNum = 0;
	int imgId = 0;

	CString strItem;
	std::map<unsigned long, MATCHGROUP>& matches = SINGLETON_TMat::GetInstance()->GetMatchResults();
	std::map<unsigned long, MATCHGROUP>::iterator iter_gr = matches.begin();
	for (; iter_gr != matches.end(); iter_gr++){

		for (int i = 0; i < iter_gr->second.matche.size(); i++){
			CBitmap* pbmp = SINGLETON_TMat::GetInstance()->GetLogCBitmap(iter_gr->second.matche[i].pImgCut);

			if (pbmp == NULL) continue;

			BITMAP bmpObj;
			pbmp->GetBitmap(&bmpObj);

			m_imgList.Add(pbmp, RGB(255, 0, 0));
			delete pbmp;


			m_List.InsertItem(m_nRecordNum, L"", imgId);

			//strItem.Format(L"%d", iter_gr->second.searchId);
			//m_List.InsertItem(m_nRecordNum, strItem, imgId);

			m_List.SetItem(m_nRecordNum, 1, LVIF_TEXT, iter_gr->second.matche[i].strCode, imgId, 0, 0, NULL);	// CODE //

			strItem.Format(L"%u", iter_gr->second.matche[i].searchId);
			m_List.SetItem(m_nRecordNum, 2, LVIF_TEXT, strItem, imgId, 0, 0, NULL);


			strItem.Format(L"%u", iter_gr->second.matche[i].cutId);
			m_List.SetItem(m_nRecordNum, 3, LVIF_TEXT, strItem, imgId, 0, 0, NULL);

			strItem.Format(L"%u", iter_gr->second.matche[i].fileId);
			m_List.SetItem(m_nRecordNum, 4, LVIF_TEXT, strItem, imgId, 0, 0, NULL);

			strItem.Format(L"%u", iter_gr->second.matche[i].posId);
			m_List.SetItem(m_nRecordNum, 5, LVIF_TEXT, strItem, imgId, 0, 0, NULL);

			strItem.Format(L"%d%u", (int)(iter_gr->second.matche[i].accuracy), iter_gr->second.matche[i].matchId);
			m_List.SetItem(m_nRecordNum, 6, LVIF_TEXT, strItem, imgId, 0, 0, NULL);

			strItem.Format(L"%u", iter_gr->second.matche[i].matchFile);
			m_List.SetItem(m_nRecordNum, 7, LVIF_TEXT, strItem, imgId, 0, 0, NULL);

			strItem.Format(L"%u", iter_gr->second.matche[i].matchPos);
			m_List.SetItem(m_nRecordNum, 8, LVIF_TEXT, strItem, imgId, 0, 0, NULL);

			strItem.Format(L"%3.2f", iter_gr->second.matche[i].fTh);
			m_List.SetItem(m_nRecordNum, 9, LVIF_TEXT, strItem, imgId, 0, 0, NULL);

			strItem.Format(L"%3.2f", iter_gr->second.matche[i].accuracy);
			m_List.SetItem(m_nRecordNum, 10, LVIF_TEXT, strItem, imgId, 0, 0, NULL);

//			m_List.SetItem(m_nRecordNum, 9, LVIF_TEXT, L"-", imgId, 0, 0, NULL);

			m_List.SetItem(m_nRecordNum, 11, LVIF_TEXT, iter_gr->second.matche[i].strBase64, imgId, 0, 0, NULL);

			m_nRecordNum++;
			imgId++;



		}
	}


//	m_List.AddRecode();
}

void CZViewLog::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}

void CZViewLog::ResizeView(int width, int height)
{
	if (m_List){
		m_List.MoveWindow(0, 0, width, height-20);
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
	//	char wBuff[102400];
		memset(m_writeBuffer, 0, sizeof(m_writeBuffer));
		CString strRecord;
		fopen_s(&fp, (CStringA)m_strPath, "w");
		if (fp){
			strRecord = L"SEARCH_ID, CUT_ID,CUT CUT_FILE_ID,CUT_POSITION,MATCH_ID,MATCH_FILE_ID,MATCH_POSITION,Threshold,Accuracy, CODE, BASE64";
			int len = strRecord.GetLength() + 1;
			sprintf_s(m_writeBuffer, len, (CStringA)strRecord, fp);
			// Write Column //
			fwrite(m_writeBuffer, len, 1, fp);
			fwrite("\r\n", 1, 1, fp);

			for (int i = 0; i < m_List.GetItemCount(); i++){
				strRecord = m_List.GetItemText(i, 0);	strRecord += L",";
				strRecord += m_List.GetItemText(i, 1);	strRecord += L",";
				strRecord += m_List.GetItemText(i, 2);	strRecord += L",";
				strRecord += m_List.GetItemText(i, 3);	strRecord += L",";
				strRecord += m_List.GetItemText(i, 4);	strRecord += L",";
				strRecord += m_List.GetItemText(i, 5);	strRecord += L",";
				strRecord += m_List.GetItemText(i, 6);	strRecord += L",";
				strRecord += m_List.GetItemText(i, 7);	strRecord += L",";
				strRecord += m_List.GetItemText(i, 8);	strRecord += L",";
				strRecord += m_List.GetItemText(i, 9);	strRecord += L",";
				strRecord += m_List.GetItemText(i, 10);

				len = strRecord.GetLength() + 1;
				sprintf_s(m_writeBuffer, len, (CStringA)strRecord, fp);
				// Write Items //
				fwrite(m_writeBuffer, len, 1, fp);
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


