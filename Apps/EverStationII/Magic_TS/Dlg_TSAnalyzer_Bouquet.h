#if !defined(AFX_DLG_BOUQUET_H__92D96E66_1A02_4004_AA09_F47A40C01017__INCLUDED_)
#define AFX_DLG_BOUQUET_H__92D96E66_1A02_4004_AA09_F47A40C01017__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_Bouquet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_TS_Analyzer_Bouquets dialog
#include "..\MFCExt\SplitWnd\SplitWnd.h"
#include "../Common/TreeView_XMLBrowser.h"
#include "NaviTree_Bouquets.h"
#include "..\resource.h"

class CDlg_TS_Analyzer_Bouquets : public CDialog
{
// Construction
public:
	CDlg_TS_Analyzer_Bouquets(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_TS_Analyzer_Bouquets)
	enum { IDD = IDD_TS_ANALYZER_BOUQUETS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_TS_Analyzer_Bouquets)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CxSplitterWnd				m_wndSplitter;

	CNaviTree_Bouquets*	m_pPane;
	//CTreeView_BouquetInfo*		m_pTree;
	CTreeView_PacketSyntax*		m_pInfoTree;

	void Reset(void);
	void Set(int offline);

	//void UpdateDSMCC(CDSMCC* pDSMCC);
	//void UpdateSDT(CSDT* pSDT);

protected:

	// Generated message map functions
	//{{AFX_MSG(CDlg_TS_Analyzer_Bouquets)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_BOUQUET_H__92D96E66_1A02_4004_AA09_F47A40C01017__INCLUDED_)
