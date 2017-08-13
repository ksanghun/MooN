
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "FileView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "DlgConfig.h"


#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

	void AddOutputString(CString str, bool IsReplace);
	CString GetLogPath() { return m_strLogPath; }
private:
	void InitConfituration();
	void GetImgFilePath(CString strPath);
	
// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
//	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCToolBarImages m_UserImages;
	CFileView         m_wndFileView;
//	CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;


	CString			m_strSrcPath;
	CString			m_strLogPath;
	


// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	bool Authorization();
	bool checkCurrTime();
	bool checkMacAddr();
public:
	afx_msg void OnFileConfiguration();
	afx_msg void OnFileOpenfile();
	afx_msg void OnViewLogview();
	afx_msg void OnView2dview();
	afx_msg void OnView3dview();
	afx_msg void OnViewInitialize();
	afx_msg void OnProjectAddimage();
	afx_msg void OnProjectRemoveimage();
	afx_msg void OnProjectSearch();
	afx_msg void OnProjectKeywordsearch();
	afx_msg void OnProjectClearresult();
	afx_msg void OnProjectConfiguration();
	afx_msg void OnAnalyzeExtracttext();
	afx_msg void OnAnalyzeUploadresult();
	afx_msg void OnAnalyzeDownloadresult();
	afx_msg void OnHelpAbout();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnViewFileviewMoon();
	afx_msg void OnViewPropertyviewMoon();
	afx_msg void OnViewOutputviewMoon();
	afx_msg void OnListAddcol();
	afx_msg void OnListSavetocsv();
	afx_msg void OnListSavetotext();
	afx_msg void OnToolsCutarea();
};


