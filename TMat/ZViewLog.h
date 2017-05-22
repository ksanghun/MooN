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

private:
	CZListCtrlLog	m_List;

	void InsertItems();
	void SetCell(HWND hWnd1, CString value, int nRow, int nCol);
	

protected:

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


