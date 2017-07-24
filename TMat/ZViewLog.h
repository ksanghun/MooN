#pragma once

#include "ZListCtrlLog.h"
#include "DlgAddCol.h"
// CZViewLog

class CZViewLog : public CWnd
{
	DECLARE_DYNAMIC(CZViewLog)

public:
	CZViewLog();
	virtual ~CZViewLog();

	void InitView(int width, int height);
	void ResizeView(int width, int height);
	void AddNewColumn();
	void SaveLogFile();
	void ResetLogList();

	void AddRecord();

private:
	CImageList m_imgList;
	CZListCtrlLog	m_List;	
	int m_nRecordNum;
	char* m_writeBuffer;
	

protected:

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


