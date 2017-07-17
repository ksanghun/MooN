// DlgExtractTool.cpp : implementation file
//

#include "stdafx.h"
#include "TMat.h"
#include "DlgExtractTool.h"
#include "afxdialogex.h"


// CDlgExtractTool dialog

IMPLEMENT_DYNAMIC(CDlgExtractTool, CDialog)

enum EXTRACTOR_TIMER_EVT{ X_INC, XDEC, Y_INC, Y_DEC };

CDlgExtractTool::CDlgExtractTool(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExtractTool::IDD, pParent)
{
	m_pExtView = NULL;
	m_sWidth = 0;
	m_sHeight = 0;
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
	DDX_Control(pDX, IDC_SLIDER_WIDTH, m_sliderWidth);
	DDX_Control(pDX, IDC_SLIDER_HEIGHT, m_sliderHeight);
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
	ON_WM_TIMER()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_WIDTH, &CDlgExtractTool::OnNMCustomdrawSliderWidth)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_WIDTH, &CDlgExtractTool::OnNMReleasedcaptureSliderWidth)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_HEIGHT, &CDlgExtractTool::OnNMCustomdrawSliderHeight)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_HEIGHT, &CDlgExtractTool::OnNMReleasedcaptureSliderHeight)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CDlgExtractTool message handlers


BOOL CDlgExtractTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_sliderWidth.SetRange(-100, 100, TRUE);
	m_sliderWidth.SetPos(0);
	m_sliderWidth.SetTicFreq(100);


	m_sliderHeight.SetRange(-100, 100, TRUE);
	m_sliderHeight.SetPos(0);
	m_sliderHeight.SetTicFreq(100);

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

		//m_nTimer = 200;
		//SetTimer(X_INC, m_nTimer, NULL);
	}
}


void CDlgExtractTool::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CDlgExtractTool::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == X_INC){
		m_pExtView->ChangeXExpand(1);
		m_pExtView->Render();

		KillTimer(X_INC);

		if (m_nTimer > 60)
			m_nTimer -= 50;
		
		SetTimer(X_INC, m_nTimer, NULL);
	}

	CDialog::OnTimer(nIDEvent);
}


void CDlgExtractTool::OnNMCustomdrawSliderWidth(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int pos = m_sliderWidth.GetPos();
	if (pos != 0){
		if (m_sWidth != pos){
			int diff = pos - m_sWidth;
			m_pExtView->ChangeXExpand(diff);
			m_pExtView->Render();
			m_sWidth = pos;
		}
	}

	*pResult = 0;
}


void CDlgExtractTool::OnNMReleasedcaptureSliderWidth(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	m_pExtView->ProcExtractTextBoundary();
	m_pExtView->Render();

	m_sliderWidth.SetPos(0);
	m_sWidth = 0;

	*pResult = 0;
}


void CDlgExtractTool::OnNMCustomdrawSliderHeight(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int pos = m_sliderHeight.GetPos();
	if (pos != 0){
		if (m_sHeight != pos){
			int diff = pos - m_sHeight;
			m_pExtView->ChangeYExpand(diff);
			m_pExtView->Render();
			m_sHeight = pos;
		}
	}
	*pResult = 0;
}


void CDlgExtractTool::OnNMReleasedcaptureSliderHeight(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	m_pExtView->ProcExtractTextBoundary();
	m_pExtView->Render();

	m_sliderHeight.SetPos(0);
	m_sHeight = 0;
	*pResult = 0;
}


BOOL CDlgExtractTool::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pExtView){
		m_pExtView->MouseWheel(zDelta);
	}

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}
