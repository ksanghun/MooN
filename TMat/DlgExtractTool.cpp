// DlgExtractTool.cpp : implementation file
//

#include "stdafx.h"
#include "TMat.h"
#include "DlgExtractTool.h"
#include "afxdialogex.h"


// CDlgExtractTool dialog

IMPLEMENT_DYNAMIC(CDlgExtractTool, CDialog)

CDlgExtractTool::CDlgExtractTool(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExtractTool::IDD, pParent)
{
	m_pExtView = NULL;
}

CDlgExtractTool::~CDlgExtractTool()
{
	//if (m_pExtView){
	////	delete m_pExtView;
	//}
}

void CDlgExtractTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgExtractTool, CDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDOK, &CDlgExtractTool::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgExtractTool::OnBnClickedCancel)
	ON_WM_NCDESTROY()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BN_EXTRACT, &CDlgExtractTool::OnBnClickedBnExtract)
	ON_BN_CLICKED(IDC_BN_YDEC, &CDlgExtractTool::OnBnClickedBnYdec)
	ON_BN_CLICKED(IDC_BN_YINCRE, &CDlgExtractTool::OnBnClickedBnYincre)
	ON_BN_CLICKED(IDC_BN_XDEC, &CDlgExtractTool::OnBnClickedBnXdec)
	ON_BN_CLICKED(IDC_BN_XINCRE, &CDlgExtractTool::OnBnClickedBnXincre)
END_MESSAGE_MAP()


// CDlgExtractTool message handlers


BOOL CDlgExtractTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgExtractTool::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (m_pExtView){

		m_pExtView->MoveWindow(0, 0, cx, cy - 100);

	}
}


BOOL CDlgExtractTool::Create(CWnd* pParentWnd)
{
	// TODO: Add your specialized code here and/or call the base class
	

	return CDialog::Create(IDD, pParentWnd);
}


int CDlgExtractTool::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect cRect;
	GetWindowRect(&cRect);
	if (m_pExtView == NULL){
		m_pExtView = new CExtractView;
		m_pExtView->Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, cRect, this, 0x05);
		m_pExtView->InitGLview(0, 0);
	}

	return 0;
}


void CDlgExtractTool::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}


void CDlgExtractTool::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}


void CDlgExtractTool::OnNcDestroy()
{
	CDialog::OnNcDestroy();

	// TODO: Add your message handler code here
	if (m_pExtView){
		delete m_pExtView;
	}
}


void CDlgExtractTool::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	if (m_pExtView){
		m_pExtView->Render();
	}
}


void CDlgExtractTool::SetExtractImage(CZPageObject* pImg, RECT2D rect)
{
	if (m_pExtView){
		m_pExtView->SetExtractImage(pImg, rect);
	}
}

void CDlgExtractTool::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnClose();
}


void CDlgExtractTool::OnBnClickedBnExtract()
{
	// TODO: Add your control notification handler code here
	if (m_pExtView){
		m_pExtView->ProcExtractTextBoundary();
		m_pExtView->Render();
	}
}


void CDlgExtractTool::OnBnClickedBnYdec()
{
	// TODO: Add your control notification handler code here
	if (m_pExtView){
		m_pExtView->ChangeYExpand(-1);
	//	m_pExtView->ProcExtractTextBoundary();
		m_pExtView->Render();
	}
}


void CDlgExtractTool::OnBnClickedBnYincre()
{
	// TODO: Add your control notification handler code here
	if (m_pExtView){
		m_pExtView->ChangeYExpand(1);
	//	m_pExtView->ProcExtractTextBoundary();
		m_pExtView->Render();
	}
}


void CDlgExtractTool::OnBnClickedBnXdec()
{
	// TODO: Add your control notification handler code here
	if (m_pExtView){
		m_pExtView->ChangeXExpand(-1);
	//	m_pExtView->ProcExtractTextBoundary();
		m_pExtView->Render();
	}
}


void CDlgExtractTool::OnBnClickedBnXincre()
{
	// TODO: Add your control notification handler code here
	if (m_pExtView){
		m_pExtView->ChangeXExpand(1);
	//	m_pExtView->ProcExtractTextBoundary();
		m_pExtView->Render();
	}
}
