#pragma once


// CDlgExtractTool dialog

class CDlgExtractTool : public CDialog
{
	DECLARE_DYNAMIC(CDlgExtractTool)

public:
	CDlgExtractTool(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgExtractTool();

// Dialog Data
	enum { IDD = IDD_DLG_EXTRACT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
