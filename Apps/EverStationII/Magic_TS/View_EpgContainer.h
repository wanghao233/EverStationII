#pragma once

#include <afxwin.h>

// CView_EpgContainer 视图
#include "..\MFCExt\SplitWnd\SplitWnd.h"
#include "ListView_EpgSchedule.h"
#include "ListView_ServiceInfo.h"

class CView_EpgContainer : public CView
{
	DECLARE_DYNCREATE(CView_EpgContainer)

protected:
	CxSplitterWnd				m_wndSplitter2;
	CListView_ServiceInfo*		m_pServiceDescription;
	CListView_EpgSchedule*		m_pEpgScheduleList;

public:
	void Reset(void);
	void Update(uint16_t network_id, uint16_t transport_stream_id, uint16_t service_id);
protected:
	CView_EpgContainer();           // 动态创建所使用的受保护的构造函数
	virtual ~CView_EpgContainer();

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


