// View_EpgContainer.cpp: 实现文件
//

#include "stdafx.h"
#include "View_EpgContainer.h"


// CView_EpgContainer

IMPLEMENT_DYNCREATE(CView_EpgContainer, CView)

CView_EpgContainer::CView_EpgContainer()
{

}

CView_EpgContainer::~CView_EpgContainer()
{
}

BEGIN_MESSAGE_MAP(CView_EpgContainer, CView)
ON_WM_SIZE()
ON_WM_CREATE()
END_MESSAGE_MAP()


// CView_EpgContainer 绘图

void CView_EpgContainer::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  在此添加绘制代码
}


// CView_EpgContainer 诊断

#ifdef _DEBUG
void CView_EpgContainer::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CView_EpgContainer::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CView_EpgContainer 消息处理程序


void CView_EpgContainer::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	m_wndSplitter2.SetWindowPos(NULL, 0, 0, cx, cy, 0);
	m_wndSplitter2.SetRowInfo(0, 150, 100);		//设置行的高度，最小高度

	m_wndSplitter2.RecalcLayout();
}


int CView_EpgContainer::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_wndSplitter2.CreateStatic(this, 2, 1);
	m_wndSplitter2.CreateView(0, 0, RUNTIME_CLASS(CListView_ServiceInfo), CSize(0, 100), NULL);
	m_wndSplitter2.CreateView(1, 0, RUNTIME_CLASS(CListView_EpgSchedule), CSize(0, 0), NULL);

	m_pServiceDescription = (CListView_ServiceInfo*)m_wndSplitter2.GetPane(0, 0);
	m_pEpgScheduleList = (CListView_EpgSchedule*)m_wndSplitter2.GetPane(1, 0);

	return 0;
}

void CView_EpgContainer::Reset(void)
{
	m_pServiceDescription->Reset();
	m_pEpgScheduleList->Reset();
}

void CView_EpgContainer::Update(uint16_t network_id, uint16_t transport_stream_id, uint16_t service_id)
{
	m_pServiceDescription->Update(network_id, transport_stream_id, service_id);
	m_pEpgScheduleList->Update(network_id, transport_stream_id, service_id);
}
