
// MainFrm.h: CMainFrame 类的接口
//

#pragma once
#include "FileView.h"
#include "ClassView.h"
#include "DockPane_RunningLog.h"

#define SHOW_PROPERTIES_WND			0
#define SHOW_SYSDEFAULT_TOOLBAR		0

#if SHOW_PROPERTIES_WND
#include "PropertiesWnd.h"
#endif

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CDockPane_RunningLog        m_DockPane_RunningLog;

protected:  // 控件条嵌入成员
	CMFCMenuBar       m_wndMenuBar;

#if SHOW_SYSDEFAULT_TOOLBAR
	CMFCToolBar       m_wndToolBar_SysDefault;
#endif

	CMFCToolBar		  m_wndToolBar_EverStation;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_imgToolBar_EverStation;
	//CFileView         m_wndFileView;
	//CClassView        m_wndClassView;

#if SHOW_PROPERTIES_WND
	CPropertiesWnd    m_wndProperties;
#endif

public :
	CMFCStatusBar & GetStatusBar(void);


// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	//afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	afx_msg void OnClose();
};


