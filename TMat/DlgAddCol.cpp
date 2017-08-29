// DlgAddCol.cpp : implementation file
//

#include "stdafx.h"
#include "TMat.h"
#include "DlgAddCol.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDlgAddCol dialog

IMPLEMENT_DYNAMIC(CDlgAddCol, CDialog)

CDlgAddCol::CDlgAddCol(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddCol::IDD, pParent)
	, m_strColName(_T("Untitled"))
	, m_intColWidth(100)
{

}

CDlgAddCol::~CDlgAddCol()
{
}

void CDlgAddCol::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_COLNAME, m_strColName);
	DDX_Text(pDX, IDC_EDIT_COLW, m_intColWidth);
	DDV_MinMaxUInt(pDX, m_intColWidth, 0, 2000);
}


BEGIN_MESSAGE_MAP(CDlgAddCol, CDialog)
END_MESSAGE_MAP()


// CDlgAddCol message handlers
