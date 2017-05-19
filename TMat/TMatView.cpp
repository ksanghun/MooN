
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
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

// CTMatView construction/destruction
CTMatView* pView = nullptr;
CTMatView::CTMatView()
{
	// TODO: add construction code here
	pView = this;

	m_pViewImage = NULL;
	m_pViewResult = NULL;
	m_pViewLog = NULL;
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
	if (m_pViewLog != NULL){
		delete m_pViewLog;
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

	if (m_pViewLog == NULL){
		m_pViewLog = new CZViewLog;
		//	m_pImageView->Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, cRect, this, 0x01);
		m_pViewLog->Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, cRect, &m_ctrlTab, 0x02);
		m_pViewLog->InitView(0,0);
	}

	if (m_pViewResult == NULL){
		m_pViewResult = new CZViewResult;
		//	m_pImageView->Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, cRect, this, 0x01);
		m_pViewResult->Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, cRect, &m_ctrlTab, 0x03);
		m_pViewResult->InitGLview(0, 0);
	}

	m_ctrlTab.AddTab(m_pViewImage, L"Image View", (UINT)0);
	m_ctrlTab.AddTab(m_pViewLog, L"Log View", (UINT)1);
	m_ctrlTab.AddTab(m_pViewResult, L"Result View", (UINT)2);



	return 0;
}


void CTMatView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	m_ctrlTab.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	m_ctrlTab.SetLocation(CMFCTabCtrl::LOCATION_TOP);

	if (m_pViewLog){
		m_pViewLog->ResizeView(cx, cy);
	}
}


void CTMatView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	DragAcceptFiles(true);


	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
}


void CTMatView::SetTreeDragItem(HTREEITEM hItem, CDragDropTreeCtrl* pCtrl)
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

void CTMatView::InitCamera(bool bmovexy)
{
	if (m_pViewImage){
		m_pViewImage->InitCamera(bmovexy);
	}
}

void CTMatView::ProcExtractTextBoundary()
{

	CMainFrame* pM = (CMainFrame*)AfxGetMainWnd();
	USES_CONVERSION;	

	CZPageObject* pPage = m_pViewImage->GetSelectedPageForCNS();
	if (pPage){
		IplImage *src = SINGLETON_TMat::GetInstance()->LoadIplImage(pPage->GetPath(), 1);
		cv::Mat img = cv::cvarrToMat(src);

		std::vector<cv::Rect> textbox;

		pM->AddOutputString(L"Start to extract words", false);

		//m_Extractor.extractWithOCR(img, textbox);

		

		/*
		//CString strFile;
		int addcnt = 0;
		for (int i = 0; i < textbox.size(); i++){

			cv::rectangle(img, textbox[i], cv::Scalar(0, 255, 0), 1, 8, 0);

			cv::Mat crop = img(textbox[i]);

			if (SINGLETON_TMat::GetInstance()->InsertIntoLogDB(crop, textbox[i].x, textbox[i].x + textbox[i].width,
				textbox[i].y, textbox[i].y + textbox[i].height, pPage->GetCode())==false){
				addcnt++;
			}

			//strFile.Format(L"C:/FGBD_project/Books/log/%d_%d.bmp", pPage->GetCode(), i);
			//char* sz = T2A(strFile);
			//cv::imwrite(sz, crop);	
			//	imshow("before deskew img ", crop);
		}
		cv::imshow("extractionWithOCR", img);
		CString strInfo;
		strInfo.Format(L"%d of %d are added", addcnt, textbox.size());
		pM->AddOutputString(L"Extraction is finished", false);
		pM->AddOutputString(strInfo, false);
		*/



		

	//	m_Extractor.ProcDeskewing(img);
		
//		std::vector<cv::Rect> textbox;
		cv::Mat cropimg;
	//	m_Extractor.extractContours(img, textbox);
	//	cv::resize(img, img, cv::Size(2 * img.cols, 2 * img.rows), 0, 0, CV_INTER_CUBIC);
		m_Extractor.getContours(img, textbox, cropimg);

	//	//Test Display
		src = SINGLETON_TMat::GetInstance()->LoadIplImage(pPage->GetPath(), 0);
		cv::Mat img2 = cv::cvarrToMat(src);
		
		CString strFile;
		int addcnt = 0;
		for (int i = 0; i < textbox.size(); i++){

			cv::rectangle(img2, textbox[i], cv::Scalar(0, 0, 255), 1, 8, 0);
			//cv::Mat crop = img2(textbox[i]);
			//if (SINGLETON_TMat::GetInstance()->InsertIntoLogDB(crop, textbox[i].x, textbox[i].x + textbox[i].width,
			//	textbox[i].y, textbox[i].y + textbox[i].height, pPage->GetCode()) == false){
			//	addcnt++;
			//}


			//cv::rectangle(img, textbox[i], cv::Scalar(0, 0, 255), 1, cv::LINE_8, 0);
			//cv::Mat crop = img(textbox[i]);
			//strFile.Format(L"C:/FGBD_project/Books/log/%d_%d.bmp", pPage->GetCode(), i);
			//char* sz = T2A(strFile);
			//cv::imwrite(sz, crop);
		//	imshow("before deskew img ", crop);

		}
////		cv::imwrite("imgOut1.jpg", img);
//
		cv::imshow("Extraction", img2);

		//CString strInfo;
		//strInfo.Format(L"%d of %d are added", addcnt, textbox.size());
		//pM->AddOutputString(L"Extraction is finished", false);
		//pM->AddOutputString(strInfo, false);


		cvReleaseImage(&src);
		img.release();		
//		pPage->UpdateTextBoundary();
	}

	else{
		AfxMessageBox(L"Page is not selected");
	}

}
short CTMatView::ProcSetSelectedItem(HTREEITEM hItem, CDragDropTreeCtrl* pCtrl)
{
	if (m_pViewImage){
		m_pViewImage->InitCamera();
	}
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


void CTMatView::RemoveImageData(HTREEITEM _item, CDragDropTreeCtrl* pCtrl)
{
	USES_CONVERSION;	char* sz = 0;
	CString strPath, strPName, strName;
	unsigned long pCode = 0;
	HTREEITEM hChildItem = pCtrl->GetChildItem(_item);

	if (hChildItem){
		pCtrl->SetItemImage(_item, 0, 0);

		strPName = pCtrl->GetItemFullPath(_item);
		sz = T2A(strPName);		
		pCode = getHashCode(sz);

		SINGLETON_TMat::GetInstance()->PopImageDataSet(pCode);
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

		pCtrl->SetItemImage(_item, 4, 4);
		strName = pCtrl->GetItemText(_item);
		strPName = pCtrl->GetItemFullPath(pItem);
		strPath = pCtrl->GetItemFullPath(_item);

		sz = T2A(strPName);		
		pCode = getHashCode(sz);
		sz = T2A(strPath);		
		cCode = getHashCode(sz);

		// Add Image Data //
		pimg->SetName(strPath, strPName, strName, pCode, cCode);
		SINGLETON_TMat::GetInstance()->PushImageDataSet(cCode, pCode, pimg);
		cnt++;

	}
	else{		// Has Child : Folder //
		strPName = pCtrl->GetItemFullPath(_item);
		sz = T2A(strPName);		
		pCode = getHashCode(sz);
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
				char* sz = T2A(strPath);				
				cCode = getHashCode(sz);
				// Add Image Data //
				pimg->SetName(strPath, strPName, strName, pCode, cCode);
			//	std::map<unsigned long, CZPageObject*>::iterator iter;
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



	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CTMatView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CTMatView::DoCurNSearch()
{
	if ((m_pViewImage)){

		pView->InitCamera(false);
		Sleep(150);

		m_pMatchingProcessor.PrepareCutNSearch(m_pViewImage->GetSelectedPageForCNS(), m_pViewImage->GetSelectedAreaForCNS());

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
		if (m_pMatchingProcessor.DoSearch(m_searchCnt) == false){
			

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

		m_pMatchingProcessor.SetThreshold(_th);

}
void CTMatView::SetResultColor(int R, int G, int B)
{

		POINT3D color;
		mtSetPoint3D(&color, (float)R / 255.0f, (float)G / 255.0f, (float)B / 255.0f);
		m_pMatchingProcessor.SetResColor(color);

}

BOOL CTMatView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	




	return CView::PreTranslateMessage(pMsg);
}




void CTMatView::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnDropFiles(hDropInfo);
}


