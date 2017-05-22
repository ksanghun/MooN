#pragma once

#include "resource.h"
// CDlgAddCol dialog

class CDlgAddCol : public CDialog
{
	DECLARE_DYNAMIC(CDlgAddCol)

public:
	CDlgAddCol(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAddCol();

// Dialog Data
	enum { IDD = IDD_DIALOG_ADDLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strColName;
	UINT m_intColWidth;
};
