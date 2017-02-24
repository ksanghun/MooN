
// TMatView.cpp : implementation of the CTMatView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "TMat.h"
#endif

#include "TMatDoc.h"
#include "TMatView.h"

#include "ZDataManager.h"
#include "ZPageObject.h"
#include "DragDropTreeCtrl.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTMatView
enum SEARCH_TIMER { _TIMER_SEARCH_PAGE = 1000 };

IMPLEMENT_DYNCREATE(CTMatView, CView)

BEGIN_MESSAGE_MAP(CTMatView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CTMatView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CTMatView construction/destruction
CTMatView* pView = nullptr;
CTMatView::CTMatView()
{
	// TODO: add construction code here
	pView = this;

	m_pViewImage = NULL;
	m_pViewResult = NULL;
	m_pMatchingProcessor = NULL;
	m_searchCnt = 0;
	// Init Data Manager //
	SINGLETON_TMat::GetInstance()->InitData();
}

CTMatView::~CTMatView()
{
	SINGLETON_TMat::Destory();
	if (m_pViewImage != NULL){
		delete m_pViewImage;
	}
	if (m_pViewResult != NULL)	{
		delete m_pViewResult;
	}
	if (m_pMatchingProcessor != NULL){
		delete m_pMatchingProcessor;
	}
}

BOOL CTMatView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CTMatView drawing

void CTMatView::OnDraw(CDC* /*pDC*/)
{
	//CTMatDoc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);
	//if (!pDoc)
	//	return;

	// TODO: add draw code for native data here
}


// CTMatView printing


void CTMatView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CTMatView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTMatView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTMatView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CTMatView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CTMatView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
//#ifndef SHARED_HANDLERS
//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
//#endif
}


// CTMatView diagnostics

#ifdef _DEBUG
void CTMatView::AssertValid() const
{
	CView::AssertValid();
}

void CTMatView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

//CTMatDoc* CTMatView::GetDocument() const // non-debug version is inline
//{
//	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTMatDoc)));
//	return (CTMatDoc*)m_pDocument;
//}
#endif //_DEBUG


// CTMatView message handlers


int CTMatView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_ctrlTab.Create(CMFCTabCtrl::STYLE_3D, rectDummy, this, 0x00))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	CRect cRect;
	GetWindowRect(&cRect);
	if (m_pViewImage == NULL){
		m_pViewImage = new CZViewImage;
		//	m_pImageView->Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, cRect, this, 0x01);
		m_pViewImage->Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, cRect, &m_ctrlTab, 0x01);
		m_pViewImage->InitGLview(0, 0);
	}

	if (m_pViewResult == NULL){
		m_pViewResult = new CZViewResult;
		//	m_pImageView->Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, cRect, this, 0x01);
		m_pViewResult->Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, cRect, &m_ctrlTab, 0x02);
		m_pViewResult->InitGLview(0, 0);
	}

	m_ctrlTab.AddTab(m_pViewImage, L"Image View", (UINT)0);
	m_ctrlTab.AddTab(m_pViewResult, L"Result View", (UINT)1);


	m_pMatchingProcessor = new CZMatching;

	return 0;
}


void CTMatView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	m_ctrlTab.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	m_ctrlTab.SetLocation(CMFCTabCtrl::LOCATION_TOP);
}


void CTMatView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
}


void CTMatView::SetTreeDragItem(CImageList* pImage, HTREEITEM hItem, CDragDropTreeCtrl* pCtrl)
{
//	SINGLETON_TMat::GetInstance()->TestThread();
	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();	
	int addcnt = 0;
	AddImageData(hItem, pCtrl, addcnt);
	

	CString str;
	str.Format(L"%d image(s) added", addcnt);
	pM->AddOutputString(str, false);

	// Generate Thumbnail Image //
	if (m_pViewImage){
		m_pViewImage->ProcGenerateThumbnail();
	}

}

void CTMatView::InitCamera()
{
	if (m_pViewImage){
		m_pViewImage->InitCamera();
	}
}

short CTMatView::ProcSetSelectedItem(HTREEITEM hItem, CDragDropTreeCtrl* pCtrl)
{
	//if (m_pViewImage){
	//	m_pViewImage->InitCamera();
	//}
	float offset = 0.0f;
	return SetSelectedItem(hItem, pCtrl, offset);
}
short CTMatView::SetSelectedItem(HTREEITEM hItem, CDragDropTreeCtrl* pCtrl, float& offset)
{
	short res = false;
	USES_CONVERSION;	char* sz = 0;
	CString strPath, strPName, strName;
	unsigned long pCode = 0, cCode = 0;
	HTREEITEM hChildItem = pCtrl->GetChildItem(hItem);

	if (hChildItem != NULL){  // Has Child!! Folder  --" change image for cut&search mode
		strName = pCtrl->GetItemText(hItem);
		strPath = pCtrl->GetItemFullPath(hItem);

	//	sz = T2A(strPName);		
	//	pCode = getHashCode(sz);
		sz = T2A(strPath);		
		cCode = getHashCode(sz);
		// Set Page Selected //
		res = SINGLETON_TMat::GetInstance()->SelectPages(cCode);
	}

	else{		// File - Select a image //
	//	strPName = pCtrl->GetItemFullPath(hItem);
		strPath = pCtrl->GetItemFullPath(hItem);
		sz = T2A(strPath);
		cCode = getHashCode(sz);
//		SINGLETON_TMat::GetInstance()->SelectPages(cCode);
		//while (hChildItem){
		//	HTREEITEM cItem = pCtrl->GetChildItem(hChildItem);

		//	if (cItem == NULL){ // File //
		//		strName = pCtrl->GetItemText(hChildItem);
		//		strPath = pCtrl->GetItemFullPath(hChildItem);
		//		//==================================//
		//		char* sz = T2A(strPath);			
		//		cCode = getHashCode(sz);
		//		// Set Page Selected //
		//		SINGLETON_TMat::GetInstance()->SelectPages(cCode, pCode);
		//	}
		//	else{
		//		SetSelectedItem(hChildItem, pCtrl, offset);
		//	}
		//	hChildItem = pCtrl->GetNextItem(hChildItem, TVGN_NEXT);
		//}
	}
	return res;
}

void CTMatView::AddImageData(HTREEITEM _item, CDragDropTreeCtrl* pCtrl, int& cnt)
{
	USES_CONVERSION;	char* sz = 0;

	CString strPath, strPName, strName;
	unsigned long pCode = 0, cCode = 0;
	HTREEITEM hChildItem = pCtrl->GetChildItem(_item);

	if (hChildItem == NULL){  // No Child!! File
		CZPageObject* pimg = new CZPageObject;
		HTREEITEM pItem = pCtrl->GetParentItem(_item);

		pCtrl->SetItemImage(_item, 4, 4);
		strName = pCtrl->GetItemText(_item);
		strPName = pCtrl->GetItemFullPath(pItem);
		strPath = pCtrl->GetItemFullPath(_item);

		sz = T2A(strPName);		pCode = getHashCode(sz);
		sz = T2A(strPath);		cCode = getHashCode(sz);

		// Add Image Data //
		pimg->SetName(strPath, strPName, strName, pCode, cCode);
		SINGLETON_TMat::GetInstance()->PushImageDataSet(cCode, pCode, pimg);
		cnt++;

	}
	else{		// Has Child : Folder //
		strPName = pCtrl->GetItemFullPath(_item);
		sz = T2A(strPName);		pCode = getHashCode(sz);
		// Change Item Status //
		pCtrl->SetItemImage(_item, 1, 1);
		while (hChildItem){
			HTREEITEM cItem = pCtrl->GetChildItem(hChildItem);
			

			if (cItem == NULL){ // File //

				pCtrl->SetItemImage(hChildItem, 4, 4);
				CZPageObject* pimg = new CZPageObject;
				strName = pCtrl->GetItemText(hChildItem);
				strPath = pCtrl->GetItemFullPath(hChildItem);
				//==================================//
				char* sz = T2A(strPath);				cCode = getHashCode(sz);
				// Add Image Data //
				pimg->SetName(strPath, strPName, strName, pCode, cCode);
				std::map<unsigned long, CZPageObject*>::iterator iter;
				SINGLETON_TMat::GetInstance()->PushImageDataSet(cCode, pCode, pimg);
				cnt++;
			}

			else{
				
				AddImageData(hChildItem, pCtrl, cnt);
			}
			hChildItem = pCtrl->GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
}

BOOL CTMatView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pViewImage){
		m_pViewImage->MouseWheel(zDelta);
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CTMatView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
    if(nChar == 90){  // Ctrl key
		if (m_pViewImage){
			m_pViewImage->EnableCutSearchMode(true);
			m_pViewImage->SendMessage(WM_SETCURSOR);
		}
	}

	else if (nChar == 39){
		if (m_pViewImage){
			m_pViewImage->MoveNextPage();
		}
	}
	else if (nChar == 37){
		if (m_pViewImage){
			m_pViewImage->MovePrePage();
		}
	}

	else if (nChar == 38){
		if (m_pViewImage){
			m_pViewImage->MoveNextUp();
		}
	}

	else if (nChar == 40){
		if (m_pViewImage){
			m_pViewImage->MoveNextDown();
		}
	}

	else if (nChar == 88){	// excute search
		DoCurNSearch();
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CTMatView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (nChar == 90){  // Ctrl key
		if (m_pViewImage){
			m_pViewImage->EnableCutSearchMode(false);
			m_pViewImage->SendMessage(WM_SETCURSOR);
		}
	}
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CTMatView::DoCurNSearch()
{
	if ((m_pViewImage)&&(m_pMatchingProcessor)){
		m_pMatchingProcessor->PrepareCutNSearch(m_pViewImage->GetSelectedPageForCNS(), m_pViewImage->GetSelectedAreaForCNS());

		m_searchCnt = 0;
		SetTimer(_TIMER_SEARCH_PAGE, 10, NULL);
		CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
		pM->AddOutputString(_T("Start Search Process...."), true);
	}
}

void CTMatView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (nIDEvent == _TIMER_SEARCH_PAGE){
		CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();

		KillTimer(_TIMER_SEARCH_PAGE);
		if (m_pMatchingProcessor->DoSearch(m_searchCnt) == false){
			

			float complete = (float)m_searchCnt / (float)SINGLETON_TMat::GetInstance()->GetImgVec().size();
			CString str;
			str.Format(_T("Searching images.....%d"), int(complete * 100));
			str += _T("%");
			str += _T(" completed.");
			pM->AddOutputString(str, true);

			SetTimer(_TIMER_SEARCH_PAGE, 10, NULL);		// Continue //
		}
		else{			
			float complete = (float)m_searchCnt / (float)SINGLETON_TMat::GetInstance()->GetImgVec().size();
			CString str;
			str.Format(_T("Searching images.....%d"), int(complete * 100));
			str += _T("%");
			str += _T(" completed.");
			pM->AddOutputString(str, true);
		}
	}
	CView::OnTimer(nIDEvent);
}


void CTMatView::SetThresholdValue(float _th)
{
	if (m_pMatchingProcessor){
		m_pMatchingProcessor->SetThreshold(_th);
	}
}
void CTMatView::SetResultColor(int R, int G, int B)
{
	if (m_pMatchingProcessor){
		POINT3D color;
		mtSetPoint3D(&color, (float)R / 255.0f, (float)G / 255.0f, (float)B / 255.0f);
		m_pMatchingProcessor->SetResColor(color);
	}
}