
#pragma once

#include "Common\ListView_RunningLog.h"
/////////////////////////////////////////////////////////////////////////////

class CDockPane_RunningLog : public CDockablePane
{
// 构造
public:
	CDockPane_RunningLog();

	void UpdateFonts();

// 特性
protected:

	CMFCTabCtrl	m_wndTabs;

	CListCtrl_RunningLog m_wndUserActionDebug;
	CListCtrl_RunningLog m_wndLogTSDebug;
	CListCtrl_RunningLog m_wndLogPcrDebug;

protected:
	//void AdjustHorzScroll(CListBox& wndListBox);

// 实现
public:
	void Open(void);
	void Close(void);

	void AppendLogForUserAction(int type, char* pszItem);
	void AppendLogForTSWarning(int type, char* pszItem);
	void AppendLogForPcrRecord(int type, char* pszItem);
public:
	virtual ~CDockPane_RunningLog();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

