#pragma once

#include "ZListCtrlLog.h"
// CZViewLog

class CZViewLog : public CWnd
{
	DECLARE_DYNAMIC(CZViewLog)

public:
	CZViewLog();
	virtual ~CZViewLog();

	void InitView();

private:
	CZListCtrlLog	m_List;

	void InsertItems();
	void SetCell(HWND hWnd1, CString value, int nRow, int nCol);
	

protected:

	DECLARE_MESSAGE_MAP()

};


