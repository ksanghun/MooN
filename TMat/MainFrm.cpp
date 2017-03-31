
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "TMat.h"
#include "TMatView.h"
#include "MainFrm.h"
#include "ZViewImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_FILE_CONFIGURATION, &CMainFrame::OnFileConfiguration)
	ON_COMMAND(ID_FILE_OPENFILE, &CMainFrame::OnFileOpenfile)
	ON_COMMAND(ID_VIEW_LOGVIEW, &CMainFrame::OnViewLogview)
	ON_COMMAND(ID_VIEW_2DVIEW, &CMainFrame::OnView2dview)
	ON_COMMAND(ID_VIEW_3DVIEW, &CMainFrame::OnView3dview)
	ON_COMMAND(ID_VIEW_INITIALIZE, &CMainFrame::OnViewInitialize)
	ON_COMMAND(ID_PROJECT_ADDIMAGE, &CMainFrame::OnProjectAddimage)
	ON_COMMAND(ID_PROJECT_REMOVEIMAGE, &CMainFrame::OnProjectRemoveimage)
	ON_COMMAND(ID_PROJECT_SEARCH, &CMainFrame::OnProjectSearch)
	ON_COMMAND(ID_PROJECT_KEYWORDSEARCH, &CMainFrame::OnProjectKeywordsearch)
	ON_COMMAND(ID_PROJECT_CLEARRESULT, &CMainFrame::OnProjectClearresult)
	ON_COMMAND(ID_PROJECT_CONFIGURATION, &CMainFrame::OnProjectConfiguration)
	ON_COMMAND(ID_ANALYZE_EXTRACTTEXT, &CMainFrame::OnAnalyzeExtracttext)
	ON_COMMAND(ID_ANALYZE_UPLOADRESULT, &CMainFrame::OnAnalyzeUploadresult)
	ON_COMMAND(ID_ANALYZE_DOWNLOADRESULT, &CMainFrame::OnAnalyzeDownloadresult)
	ON_COMMAND(ID_HELP_ABOUT, &CMainFrame::OnHelpAbout)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
	int a = 0;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (Authorization() == false){		
		exit(0);
	}


	BOOL bNameValid;

	//if (!m_wndMenuBar.Create(this))
	//{
	//	TRACE0("Failed to create menubar\n");
	//	return -1;      // fail to create
	//}

	//m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// prevent the menu bar from taking the focus on activation
	//CMFCPopupMenu::SetForceMenuFocus(FALSE);

	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME_256))
	//{
	//	TRACE0("Failed to create toolbar\n");
	//	return -1;      // fail to create 
	//}

	DWORD dwCtrlStyle = TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CBRS_SIZE_DYNAMIC;
	DWORD dwStyle = AFX_DEFAULT_TOOLBAR_STYLE;
	CMFCToolBarInfo tbi, tbiA;
	const CRect r1(1, 1, 1, 1);
	if (!m_wndToolBar.CreateEx(this, dwCtrlStyle, dwStyle, r1, IDR_MAINFRAME) ||
		!m_wndToolBar.LoadToolBarEx(IDR_MAINFRAME_256, tbi, TRUE))
	{
		return -1;      // fail to create
	}
	
	CSize szImage, szButton;
	szImage = m_wndToolBar.GetImageSize();
	szButton.cx = szImage.cx + 6; // button size must be at least image size + 6
	szButton.cy = szImage.cy + 6;
	m_wndToolBar.SetMenuSizes(szButton, szImage);
	




	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	//{
	//	TRACE0("Failed to create toolbar\n");
	//	return -1;      // fail to create
	//}

	//CString strToolBarName;
	//bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	//ASSERT(bNameValid);
	//m_wndToolBar.SetWindowText(strToolBarName);

	//CString strCustomize;
	//bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	//ASSERT(bNameValid);
	//m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// Allow user-defined toolbars operations:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
//	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
//	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Load menu item image (not placed on any standard toolbars):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
//	m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndFileView);
	CDockablePane* pTabbedBar = NULL;
//	m_wndFileView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);


	// set the visual manager used to draw all user interface elements
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));

	// set the visual style to be used the by the visual manager
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);

	// Enable toolbar and docking window menu replacement
//	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// enable quick (Alt+drag) toolbar customization
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// load user-defined toolbar images
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// enable menu personalization (most-recently used commands)
	// TODO: define your own basic commands, ensuring that each pulldown menu has at least one basic command.
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	InitConfituration();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	//cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
	//	 | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;

	cs.style &= ~(LONG)FWS_ADDTOTITLE;

	cs.lpszName = L"MoÙþoN";
	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// Create class view
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	//if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	//{
	//	TRACE0("Failed to create Class View window\n");
	//	return FALSE; // failed to create
	//}

	// Create file view
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create File View window\n");
		return FALSE; // failed to create
	}

	// Create output window
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}

	// Create properties window
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);

//	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
//	m_wndClassView.SetIcon(hClassViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}


BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// base class does the real work

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// enable customization button for all user toolbars
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}


bool CMainFrame::checkMacAddr()
{
	// Known MAC Address ====================//
	CString authorized = L"00:0D:3A:A2:E7:C2";	// VM in Azure
	//========================================


	PIP_ADAPTER_INFO AdapterInfo;
	DWORD dwBufLen = sizeof(AdapterInfo);
	//	char *mac_addr = (char*)malloc(17);
	CString strMacAddr = L"";

	AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	if (AdapterInfo == NULL){
		return false;
	}

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {
		AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
		if (AdapterInfo == NULL) {
			return false;
		}
	}

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
		do {
			strMacAddr.Format(L"%02X:%02X:%02X:%02X:%02X:%02X",
				pAdapterInfo->Address[0], pAdapterInfo->Address[1],
				pAdapterInfo->Address[2], pAdapterInfo->Address[3],
				pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
			pAdapterInfo = pAdapterInfo->Next;
		} while (pAdapterInfo);
	}
	free(AdapterInfo);


	if (strMacAddr == authorized){
		return true;
	}

	return false;

}

bool CMainFrame::checkCurrTime()
{
	//WORD year = 2017;
	//WORD month = 3;
	//WORD day = 1;


	WORD eYear = 2017;
	WORD eMonth = 4;
	WORD eDay = 30;

	SYSTEMTIME st;
	GetSystemTime(&st);


	if ((st.wYear <= eYear)){

		if (st.wMonth == eMonth){
			if ((st.wDay <= eDay)){
				return true;
			}
		}
		else{
			if (st.wMonth < eMonth){
				return true;
			}
		}		
	}

	return false;
}


bool CMainFrame::Authorization()
{
	//if (checkMacAddr() == false){
	//	AfxMessageBox(L"Authorization failed");
	//	return false;
	//}

	if (checkCurrTime() == false){
		AfxMessageBox(L"Authentication has expired");
		return false;
	}
	return true;
}

void CMainFrame::InitConfituration()
{

//	pView->initImageData();	
	CString sPath;
	GetModuleFileName(nullptr, sPath.GetBuffer(_MAX_PATH + 1), _MAX_PATH);
	sPath.ReleaseBuffer();
	CString path = sPath.Left(sPath.ReverseFind(_T('\\')));
	CString strFle = path + "\\userdata\\conf.bin";



	// Folder Check================ //
	CString strfolder = path + "\\userdata";
	if (PathFileExists(strfolder) == 0){
		CreateDirectory(strfolder, NULL);
	}
	CString strLog = path + "\\logdata";
	if (PathFileExists(strLog) == 0){
		CreateDirectory(strLog, NULL);
	}
	SINGLETON_TMat::GetInstance()->SetLogPath(strLog);
	////======================================//



	// Load Config
	FILE* fp = 0;
	fopen_s(&fp, (CStringA)strFle, "rb");

	const int pathSize = 256;
	char srcPath[pathSize];
	//	memset(srcPath, 0x00, sizeof(srcPath));
	if (fp){
		SYSTEMTIME st;
		fread(&st, sizeof(SYSTEMTIME), 1, fp);
		fread(srcPath, sizeof(srcPath), 1, fp);
		m_strSrcPath = (CStringA)srcPath;
		m_strLogPath = (CStringA)srcPath;
		fclose(fp);

	}
	else{
		CDlgConfig dlg;
		if (dlg.DoModal() == IDOK)
		{
			m_strSrcPath = dlg.GetSrcPath();

			// Save Config //
			fopen_s(&fp, (CStringA)strFle, "wb");
			SYSTEMTIME st;
			GetSystemTime(&st);
			if (fp){
				sprintf_s(srcPath, sizeof(srcPath), (CStringA)m_strSrcPath, fp);
				fwrite(&st, sizeof(SYSTEMTIME), 1, fp);
				fwrite(srcPath, pathSize, 1, fp);
				fclose(fp);
			}
		}
	}

	m_wndFileView.FillFileView(m_strSrcPath);
	GetImgFilePath(m_strSrcPath);

	

}

void CMainFrame::GetImgFilePath(CString strPath)
{

	//	else{
	//int nCount = 0;
	CString strSubPath;
	CString strFileName;
	CFileFind file_find;
	BOOL bWorking;
	bWorking = file_find.FindFile(strPath + ("\\*"));

	BOOL isSubDir = false;
	while (bWorking)
	{
		bWorking = file_find.FindNextFile();
		if (!file_find.IsDots()){

			if (file_find.IsDirectory()){//directory 				
				strSubPath = strPath + ("\\") + file_find.GetFileName();
				GetImgFilePath(strSubPath);
				//nCount++;
			}
		}
	}

	// Save File //
	//	}
}

void CMainFrame::OnFileConfiguration()
{
	// TODO: Add your command handler code here
	CDlgConfig dlg(NULL, m_strSrcPath, m_strLogPath);


	CString sPath;
	GetModuleFileName(nullptr, sPath.GetBuffer(_MAX_PATH + 1), _MAX_PATH);
	sPath.ReleaseBuffer();
	CString path = sPath.Left(sPath.ReverseFind(_T('\\')));
	CString strFle = path + "\\userdata\\conf.bin";

	if (dlg.DoModal() == IDOK)
	{
		m_strSrcPath = dlg.GetSrcPath();

		const int pathSize = 256;
		char srcPath[pathSize];
		// Save Config //
		FILE* fp = 0;
		fopen_s(&fp, (CStringA)strFle, "wb");
		SYSTEMTIME st;
		GetSystemTime(&st);
		if (fp){
			sprintf_s(srcPath, sizeof(srcPath), (CStringA)m_strSrcPath, fp);
			fwrite(&st, sizeof(SYSTEMTIME), 1, fp);
			fwrite(srcPath, pathSize, 1, fp);
			fclose(fp);
		}

		m_wndFileView.FillFileView(m_strSrcPath);
		GetImgFilePath(m_strSrcPath);
	//	InitConfituration();
	}
}


void CMainFrame::AddOutputString(CString str, bool IsReplace)
{
	if (IsReplace == true){
		m_wndOutput.ReplaceString(str);
	}
	else{
		m_wndOutput.AddString(str);
	}
}


void CMainFrame::OnFileOpenfile()
{
	// TODO: Add your command handler code here
}


void CMainFrame::OnViewLogview()
{
	// TODO: Add your command handler code here
	AfxMessageBox(L"Log View is not available now");
}


void CMainFrame::OnView2dview()
{
	// TODO: Add your command handler code here
}


void CMainFrame::OnView3dview()
{
	// TODO: Add your command handler code here
}


void CMainFrame::OnViewInitialize()
{
	// TODO: Add your command handler code here
	pView->InitCamera(true);
}


void CMainFrame::OnProjectAddimage()
{
	// TODO: Add your command handler code here
	CDragDropTreeCtrl* pCtrl = m_wndFileView.GetTreeCtrl();
	for (unsigned int i = 0; i < pCtrl->GetSelItemList()->size(); i++){
		pView->SetTreeDragItem((*pCtrl->GetSelItemList())[i], pCtrl);
	}
}


void CMainFrame::OnProjectRemoveimage()
{
	// TODO: Add your command handler code here
	CDragDropTreeCtrl* pCtrl = m_wndFileView.GetTreeCtrl();
	for (unsigned int i = 0; i < pCtrl->GetSelItemList()->size(); i++){
		HTREEITEM selItem = (*pCtrl->GetSelItemList())[i];
		pView->RemoveImageData(selItem, pCtrl);
	}
}


void CMainFrame::OnProjectSearch()
{
	// TODO: Add your command handler code here
	pView->DoCurNSearch();
}


void CMainFrame::OnProjectKeywordsearch()
{
	// TODO: Add your command handler code here
	AfxMessageBox(L"Keyword Search is not available now");
}


void CMainFrame::OnProjectClearresult()
{
	// TODO: Add your command handler code here
	SINGLETON_TMat::GetInstance()->ResetResult();
}


void CMainFrame::OnProjectConfiguration()
{
	// TODO: Add your command handler code here
	CDlgConfig dlg(NULL, m_strSrcPath, m_strLogPath);


	CString sPath;
	GetModuleFileName(nullptr, sPath.GetBuffer(_MAX_PATH + 1), _MAX_PATH);
	sPath.ReleaseBuffer();
	CString path = sPath.Left(sPath.ReverseFind(_T('\\')));
	CString strFle = path + "\\userdata\\conf.bin";

	if (dlg.DoModal() == IDOK)
	{
		m_strSrcPath = dlg.GetSrcPath();

		const int pathSize = 256;
		char srcPath[pathSize];
		// Save Config //
		FILE* fp = 0;
		fopen_s(&fp, (CStringA)strFle, "wb");
		SYSTEMTIME st;
		GetSystemTime(&st);
		if (fp){
			sprintf_s(srcPath, sizeof(srcPath), (CStringA)m_strSrcPath, fp);
			fwrite(&st, sizeof(SYSTEMTIME), 1, fp);
			fwrite(srcPath, pathSize, 1, fp);
			fclose(fp);
		}

		m_wndFileView.FillFileView(m_strSrcPath);
		GetImgFilePath(m_strSrcPath);
		//	InitConfituration();
	}
}


void CMainFrame::OnAnalyzeExtracttext()
{
	// TODO: Add your command handler code here
	pView->ProcExtractTextBoundary();
}


void CMainFrame::OnAnalyzeUploadresult()
{
	// TODO: Add your command handler code here
}


void CMainFrame::OnAnalyzeDownloadresult()
{
	// TODO: Add your command handler code here
}


void CMainFrame::OnHelpAbout()
{
	// TODO: Add your command handler code here
}


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	
	// TODO: Add your specialized code here and/or call the base class

	CZViewImage* pViewImage = pView->GetViewImage();

	if (pMsg->message == WM_KEYDOWN){
		int nChar = (int)pMsg->wParam;
		if (nChar == 90){  // Ctrl key
			if (pViewImage){
				pViewImage->EnableCutSearchMode(true);
				pViewImage->SendMessage(WM_SETCURSOR);
			}
		}

		else if (nChar == 39){
			if (pViewImage){
				pViewImage->MoveNextPage();
			}
		}
		else if (nChar == 37){
			if (pViewImage){
				pViewImage->MovePrePage();
			}
		}

		else if (nChar == 38){
			if (pViewImage){
				pViewImage->MoveNextUp();
			}
		}

		else if (nChar == 40){
			if (pViewImage){
				pViewImage->MoveNextDown();
			}
		}
		
		else if (nChar == 88){	// excute search
			pView->DoCurNSearch();
		}
		else if (nChar == 17){	// ctrl key
			if (pViewImage){
				pViewImage->SetAnimation(false);
			}
		}

		else if (nChar == 67){
			if (pViewImage){
				pViewImage->ResetAllPages();
			}
		}
		else if (nChar == 68){
			if (pViewImage){
				pViewImage->SetDBSearch(true);
			}
		}
	}


	if (pMsg->message == WM_KEYUP){
		int nChar = (int)pMsg->wParam;
		if (nChar == 90){  // z key
			if (pViewImage){
				pViewImage->EnableCutSearchMode(false);
				pViewImage->SendMessage(WM_SETCURSOR);
			}
		}
		else if (nChar == 17){	// ctrl key
			if (pViewImage){
				pViewImage->SetAnimation(true);
			}
			float offset = 0.0f;
		}
		else if (nChar == 68){	// "d"
			if (pViewImage){
				pViewImage->SetDBSearch(false);
			}
		}
	}

	return CFrameWndEx::PreTranslateMessage(pMsg);
}
