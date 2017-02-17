
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
END_MESSAGE_MAP()

// CTMatView construction/destruction
CTMatView* pView = nullptr;
CTMatView::CTMatView()
{
	// TODO: add construction code here
	pView = this;

	m_pViewImage = NULL;
	m_pViewResult = NULL;
	
	// Init Data Manager //
	SINGLETON_TMat::GetInstance()->InitData();
}

CTMatView::~CTMatView()
{
	SINGLETON_TMat::Destory();
	if (m_pViewImage != NULL){
		delete m_pViewImage;
	}
	if (m_pViewResult != NULL)
	{
		delete m_pViewResult;
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
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
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

void CTMatView::AddImageData(HTREEITEM _item, CDragDropTreeCtrl* pCtrl, int& cnt)
{
	

	USES_CONVERSION;	char* sz = 0;

	CString strPath, strPName, strName;
	unsigned long pCode = 0, cCode = 0;
	HTREEITEM hChildItem = pCtrl->GetChildItem(_item);

	if (hChildItem == NULL){  // No Child!! File
		CZPageObject* pimg = new CZPageObject;
		HTREEITEM pItem = pCtrl->GetParentItem(_item);

		strName = pCtrl->GetItemText(_item);
		strPName = pCtrl->GetItemText(pItem);
		strPath = pCtrl->GetItemFullPath(_item);

		sz = T2A(strPName);		pCode = getHashCode(sz);
		sz = T2A(strPath);		cCode = getHashCode(sz);

		// Add Image Data //
		pimg->SetName(strPath, strPName, strName, pCode, cCode);
		SINGLETON_TMat::GetInstance()->PushImageDataSet(cCode, pCode, pimg);
		cnt++;

	}
	else{		// Has Child : Folder //
		strPName = pCtrl->GetItemText(_item);
		sz = T2A(strPName);		pCode = getHashCode(sz);

		while (hChildItem){
			HTREEITEM cItem = pCtrl->GetChildItem(hChildItem);

			if (cItem == NULL){ // File //
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