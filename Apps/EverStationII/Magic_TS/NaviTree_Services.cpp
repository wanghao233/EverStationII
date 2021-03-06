// Pane_EpgInfoTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "NaviTree_Services.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiTables.h"

#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/Mpeg2_table_id.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/DVB_table_id.h"
#include "libs_MPEG&DVB\MPEG_DVB_Section\Include\DVB_SI_Utilities.h"

#include "TSMagicView.h"
/////////////////////////////////////////////////////////////////////////////
// CPane_EpgInfoTreeView

#define TREEITEM_STYLE_NETWORK	 0x1
#define TREEITEM_STYLE_STREAM	 0x2
#define TREEITEM_STYLE_SERVICE	 0x3
#define TREEITEM_STYLE_NULL		 0xf

//+------+----------------------+--------------------------+
//|   4  |          12          |            16            |
//+------+----------------------+--------------------------+
//  类型         reserved                    ID

IMPLEMENT_DYNCREATE(CNaviTree_Services, CTreeView)

CNaviTree_Services::CNaviTree_Services()
{
	m_hRootItem = NULL;
}

CNaviTree_Services::~CNaviTree_Services()
{
	m_hRootItem = NULL;
}


BEGIN_MESSAGE_MAP(CNaviTree_Services, CTreeView)
	//{{AFX_MSG_MAP(CPane_EpgInfoTreeView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	//}}AFX_MSG_MAP
//	ON_MESSAGE(WM_UPDATE_SDT, OnUpdateSDT)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPane_EpgInfoTreeView drawing

void CNaviTree_Services::OnDraw(CDC* pDC)
{
//	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CPane_EpgInfoTreeView diagnostics

#ifdef _DEBUG
void CNaviTree_Services::AssertValid() const
{
	CTreeView::AssertValid();
}

void CNaviTree_Services::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPane_EpgInfoTreeView message handlers

BOOL CNaviTree_Services::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	dwStyle = dwStyle | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_FULLROWSELECT;
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

int CNaviTree_Services::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	TV_INSERTSTRUCT		TreeCtrlItem;
	char				pszText[64];
	int					sItemHeight;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	m_ImageList.Create(IDB_EPG, 20, 1, RGB(255, 255, 255));
	treeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);

	sItemHeight = treeCtrl.GetItemHeight();
	treeCtrl.SetItemHeight(sItemHeight + 5);

	TreeCtrlItem.hParent = TVI_ROOT;

#if CHINESE_VERSION
	strcpy_s(pszText, sizeof(pszText), "全网络业务EPG");
#else
	strcpy_s(pszText, sizeof(pszText), "EPG Information");
#endif
	m_hRootItem = treeCtrl.InsertItem(&TreeCtrlItem);
	treeCtrl.SetItemData(m_hRootItem, 0xffffffff);
	treeCtrl.SetItemImage(m_hRootItem, 0, 0);

	return 0;
}

//void CPane_EpgInfoTreeView::Set(int offline)
//{
//}

void CNaviTree_Services::Reset(void)
{
	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (treeCtrl.ItemHasChildren(m_hRootItem))
	{
		DeleteChildItems(m_hRootItem);
	}

	m_pViewEpg->Reset();
}

void CNaviTree_Services::UpdateNIT(CNIT* pNIT)
{
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszNetworkName[64];
	char			pszText[MAX_TXT_CHARS];
	HTREEITEM		hNetworkItem;
	HTREEITEM		hStreamItem;
	//char*			pstr_dst;
	int				find_network_item;
	int				find_stream_item;
	STREAM_INFO_t   stStreamInfo;

	uint32_t		item_code;
	int				item_style;
	int				item_param_id;

	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CDB_PsiSiTables* pDB_PsiSiTables = pTSMagicView->GetPsiSiTablesDBase();

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pNIT != NULL)
	{
		uint16_t usNetworkID = pNIT->GetNetworkID();

		pNIT->GetNetworkName(pszNetworkName, sizeof(pszNetworkName));
		if (pNIT->GetTableID() == TABLE_ID_NIT_ACTUAL)
		{
			sprintf_s(pszText, sizeof(pszText), "当前网络: %s [network_id=0x%04X(%d)]", pszNetworkName, usNetworkID, usNetworkID);
		}
		else
		{
			sprintf_s(pszText, sizeof(pszText), "其他网络: %s [network_id=0x%04X(%d)]", pszNetworkName, usNetworkID, usNetworkID);
		}

		find_network_item = 0;
		if (treeCtrl.ItemHasChildren(m_hRootItem))
		{
			hNetworkItem = treeCtrl.GetChildItem(m_hRootItem);
			while (hNetworkItem != NULL)
			{
				//通过network_id查找
				item_code = (DWORD)treeCtrl.GetItemData(hNetworkItem);

				item_style = (item_code & 0xf0000000) >> 28;
				assert(item_style == TREEITEM_STYLE_NETWORK);

				item_param_id = (item_code & 0x0000ffff);

				if (item_param_id == usNetworkID)
				{
					find_network_item = 1;

					//更新network序号和名称
					//item_code &= 0xf000ffff;
					//item_code |= (network_index & 0x0fff) << 16;

					//treeCtrl.SetItemData(hNetworkItem, item_code);
					treeCtrl.SetItemText(hNetworkItem, pszText);
					treeCtrl.SetItemState(hNetworkItem, TVIS_BOLD, TVIS_BOLD);

					break;
				}
				else
				{
					hNetworkItem = treeCtrl.GetNextItem(hNetworkItem, TVGN_NEXT);
				}
			}
		}

		if (find_network_item == 0)
		{
			TreeCtrlItem.hParent = m_hRootItem;

			hNetworkItem = treeCtrl.InsertItem(&TreeCtrlItem);

			item_code = (TREEITEM_STYLE_NETWORK << 28);
			item_code |= 0x0fff0000;
			item_code |= usNetworkID;

			treeCtrl.SetItemData(hNetworkItem, item_code);
			treeCtrl.SetItemText(hNetworkItem, pszText);
			treeCtrl.SetItemImage(hNetworkItem, 0, 0);

			if (pNIT->GetTableID() == TABLE_ID_NIT_ACTUAL)
			{
				treeCtrl.SetItemState(hNetworkItem, TVIS_BOLD, TVIS_BOLD);
			}
		}

		if (hNetworkItem != NULL)
		{
			for (int i = 0; i < pNIT->GetStreamCount(); i++)
			{
				pNIT->GetStreamByIndex(i, &stStreamInfo);

				find_stream_item = 0;
				if (treeCtrl.ItemHasChildren(hNetworkItem))
				{
					hStreamItem = treeCtrl.GetChildItem(hNetworkItem);
					//按stream_id查找
					while (hStreamItem != NULL)
					{
						item_code = (DWORD)treeCtrl.GetItemData(hStreamItem);
						item_style = (item_code & 0xf0000000) >> 28;
						//item_param_index = (item_code & 0x0fff0000) >> 16;
						item_param_id = (item_code & 0x0000ffff);

						assert(item_style == TREEITEM_STYLE_STREAM);

						if (item_param_id == stStreamInfo.transport_stream_id)
						{
							sprintf_s(pszText, sizeof(pszText), "传送流[ONetID=0x%04x(%d)-TSID=0x%04X(%d)]", stStreamInfo.original_network_id, stStreamInfo.original_network_id, stStreamInfo.transport_stream_id, stStreamInfo.transport_stream_id);
							treeCtrl.SetItemText(hStreamItem, pszText);

							find_stream_item = 1;
							break;
						}
						else
						{
							hStreamItem = treeCtrl.GetNextItem(hStreamItem, TVGN_NEXT);
						}
					}
				}

				if (find_stream_item == 0)
				{
					sprintf_s(pszText, sizeof(pszText), "传送流[ONetID=0x%04x(%d)-TSID=0x%04X(%d)] <only refered in NIT>", stStreamInfo.original_network_id, stStreamInfo.original_network_id, stStreamInfo.transport_stream_id, stStreamInfo.transport_stream_id);

					TreeCtrlItem.hParent = hNetworkItem;
					hStreamItem = treeCtrl.InsertItem(&TreeCtrlItem);

					item_code = (TREEITEM_STYLE_STREAM << 28);
					item_code |= stStreamInfo.transport_stream_id;

					treeCtrl.SetItemData(hStreamItem, item_code);
					treeCtrl.SetItemImage(hStreamItem, 0, 0);
				}
			}

			treeCtrl.SortChildren(m_hRootItem);
			treeCtrl.Expand(m_hRootItem, TVE_EXPAND);

			treeCtrl.SortChildren(hNetworkItem);
			treeCtrl.Expand(hNetworkItem, TVE_EXPAND);

			//if (pNIT->m_ucTableID == TABLE_ID_NIT_ACTUAL)
			//{
			//	treeCtrl.Expand(hNetworkItem, TVE_EXPAND);
			//}
		}
	}
}

void CNaviTree_Services::UpdateSDT(CSDT* pSDT)
{
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[MAX_TXT_CHARS];

	int				service_index;
	
	int				find_network_item;
	int				find_stream_item;
	
	HTREEITEM		hNetworkItem;
	HTREEITEM		hStreamItem;
	HTREEITEM		hServiceItem;
	CString			strTemp;

	uint8_t			item_style;
	uint16_t		item_param_index;
	uint16_t		item_param_id;
	uint32_t		item_code;
	//uint16_t		network_id;

	SERVICE_INFO_t	stServiceInfo;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pSDT != NULL)
	{
		uint16_t usOriginalNetworkID = pSDT->GetOriginalNetworkID();
		uint16_t usTransportStreamID = pSDT->GetTransportStreamID();

		find_network_item = 0;
		if (treeCtrl.ItemHasChildren(m_hRootItem))
		{
			hNetworkItem = treeCtrl.GetChildItem(m_hRootItem);
			while (hNetworkItem != NULL)
			{
				item_code = (DWORD)treeCtrl.GetItemData(hNetworkItem);
				item_style = (item_code & 0xf0000000) >> 28;
				item_param_index = (item_code & 0x0fff0000) >> 16;
				item_param_id = (item_code & 0x0000ffff);

				assert(item_style == TREEITEM_STYLE_NETWORK);

				if (item_param_id == usOriginalNetworkID)
				{
					find_network_item = 1;
					break;
				}
				else
				{
					hNetworkItem = treeCtrl.GetNextItem(hNetworkItem, TVGN_NEXT);
				}
			}
		}

		if (find_network_item == 0)
		{
			sprintf_s(pszText, sizeof(pszText), "未知网络 [network_id=0x%04X(%d)] <only refered in SDT>", usOriginalNetworkID, usOriginalNetworkID);

			TreeCtrlItem.hParent = m_hRootItem;

			hNetworkItem = treeCtrl.InsertItem(&TreeCtrlItem);

			item_code = (TREEITEM_STYLE_NETWORK << 28);
			item_code |= 0x0fff0000;
			item_code |= usOriginalNetworkID;

			treeCtrl.SetItemData(hNetworkItem, item_code);
			treeCtrl.SetItemImage(hNetworkItem, 0, 0);
		}

		find_stream_item = 0;
		if (treeCtrl.ItemHasChildren(hNetworkItem))
		{
			hStreamItem = treeCtrl.GetChildItem(hNetworkItem);
			//按stream_id查找
			while (hStreamItem != NULL)
			{
				item_code = (DWORD)treeCtrl.GetItemData(hStreamItem);
				item_style = (item_code & 0xf0000000) >> 28;
				item_param_index = (item_code & 0x0fff0000) >> 16;
				item_param_id = (item_code & 0x0000ffff);

				assert(item_style == TREEITEM_STYLE_STREAM);

				if (item_param_id == usTransportStreamID)
				{
#if CHINESE_VERSION
					sprintf_s(pszText, sizeof(pszText), "传送流[ONetID=0x%04X(%d)-TSID=0x%04X(%d)]", usOriginalNetworkID, usOriginalNetworkID, usTransportStreamID, usTransportStreamID);
#else
					sprintf_s(pszText, sizeof(pszText), "STREAM[ONetID=0x%04X(%d)-TSID=0x%04X(%d)]", usOriginalNetworkID, usOriginalNetworkID, usTransportStreamID, usTransportStreamID);
#endif

					treeCtrl.SetItemText(hStreamItem, pszText);
					find_stream_item = 1;
					break;
				}
				else
				{
					hStreamItem = treeCtrl.GetNextItem(hStreamItem, TVGN_NEXT);
				}
			}
		}

		//新的流
		if (find_stream_item == 0)
		{
#if CHINESE_VERSION
			sprintf_s(pszText, sizeof(pszText), "传送流[ONetID=0x%04X(%d)-TSID=0x%04X(%d)] <only refered in SDT>", usOriginalNetworkID, usOriginalNetworkID, usTransportStreamID, usTransportStreamID);
#else
			sprintf_s(pszTemp, sizeof(pszTemp), "STREAM[ONetID=0x%04X(%d)-TSID=0x%04X(%d)] - only refered in SDT", usOriginalNetworkID, usOriginalNetworkID, usTransportStreamID, usTransportStreamID);
#endif

			//strcpy_s(pszText, sizeof(pszText), pszTemp);
			TreeCtrlItem.hParent = hNetworkItem;

			hStreamItem = treeCtrl.InsertItem(&TreeCtrlItem);

			item_code = (TREEITEM_STYLE_STREAM << 28);
			item_code |= 0x0fff0000;
			item_code |= usTransportStreamID;

			treeCtrl.SetItemData(hStreamItem, item_code);
			treeCtrl.SetItemImage(hStreamItem, 0, 0);

			if (pSDT->GetTableID() == TABLE_ID_SDT_ACTUAL)
			{
				treeCtrl.SetItemState(hStreamItem, TVIS_BOLD, TVIS_BOLD);
			}
		}

		if (hStreamItem != NULL)
		{
			TreeCtrlItem.hParent = hStreamItem;

			for (service_index = 0; service_index < pSDT->GetServiceCount(); service_index ++)
			{
				pSDT->GetServiceInfoByIndex(service_index, &stServiceInfo);

#if CHINESE_VERSION
				sprintf_s(pszText, sizeof(pszText), "业务 0x%04X(%d) [%s]", stServiceInfo.service_id, stServiceInfo.service_id, stServiceInfo.pszServiceName);
#else
				sprintf_s(pszText, sizeof(pszText), "SERVICE 0x%04X(%d) [%s]", stServiceInfo.service_id, stServiceInfo.service_id, stServiceInfo.pszServiceName);
#endif
				hServiceItem = treeCtrl.InsertItem(&TreeCtrlItem);

				item_code = (TREEITEM_STYLE_SERVICE << 28);
				item_code |= 0x0fff0000;
				item_code |= stServiceInfo.service_id;

				treeCtrl.SetItemData(hServiceItem, item_code);
				treeCtrl.SetItemImage(hServiceItem, 2, 2);
			}

			treeCtrl.SortChildren(m_hRootItem);
			treeCtrl.Expand(m_hRootItem, TVE_EXPAND);

			treeCtrl.SortChildren(hNetworkItem);
			treeCtrl.Expand(hNetworkItem, TVE_EXPAND);
		}
	}
}

void CNaviTree_Services::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	uint32_t	item_code;
	uint8_t		item_style;
	uint16_t	item_param_id;

	uint16_t	network_id;
	uint16_t	transport_stream_id;
	uint16_t	service_id;

	m_pViewEpg->Reset();

	CTreeCtrl& treeCtrl = GetTreeCtrl();
	HTREEITEM hSelItem = treeCtrl.GetSelectedItem();

	item_code = (DWORD)treeCtrl.GetItemData(hSelItem);
	item_style = (item_code & 0xf0000000) >> 28;
	item_param_id = (item_code & 0x0000ffff);

	if (item_style == TREEITEM_STYLE_SERVICE)
	{
		service_id = item_param_id;
		HTREEITEM hParentItem = treeCtrl.GetParentItem(hSelItem);

		uint32_t parent_item_code = (DWORD)treeCtrl.GetItemData(hParentItem);
		uint8_t parent_item_style = (parent_item_code & 0xf0000000) >> 28;
		uint16_t parent_item_param_id = (parent_item_code & 0x0000ffff);

		assert(parent_item_style == TREEITEM_STYLE_STREAM);
		transport_stream_id = parent_item_param_id;

		HTREEITEM hGrandParentItem = treeCtrl.GetParentItem(hParentItem);

		uint32_t grandparent_item_code = (DWORD)treeCtrl.GetItemData(hGrandParentItem);
		uint8_t grandparent_item_style = (grandparent_item_code & 0xf0000000) >> 28;
		uint16_t grandparent_item_param_id = (grandparent_item_code & 0x0000ffff);

		assert(grandparent_item_style == TREEITEM_STYLE_NETWORK);
		network_id = grandparent_item_param_id;

		m_pViewEpg->Update(network_id, transport_stream_id, service_id);
	}
	else if (item_style == TREEITEM_STYLE_STREAM)
	{
		transport_stream_id = item_param_id;
		HTREEITEM hParentItem = treeCtrl.GetParentItem(hSelItem);

		uint32_t parent_item_code = (DWORD)treeCtrl.GetItemData(hParentItem);
		uint8_t parent_item_style = (parent_item_code & 0xf0000000) >> 28;
		uint16_t parent_item_param_id = (parent_item_code & 0x0000ffff);

		assert(parent_item_style == TREEITEM_STYLE_NETWORK);
		network_id = parent_item_param_id;

		m_pViewEpg->Update(network_id, transport_stream_id, 0xFFFF);
	}
	else if (item_style == TREEITEM_STYLE_NETWORK)
	{
		network_id = item_param_id;
		m_pViewEpg->Update(network_id, 0xFFFF, 0xFFFF);
	}

	*pResult = 0;
}

void CNaviTree_Services::DeleteChildItems(HTREEITEM hParentItem)
{
	HTREEITEM hChildItem;
	HTREEITEM hOldItem;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (treeCtrl.ItemHasChildren(hParentItem))
	{
		hChildItem = treeCtrl.GetChildItem(hParentItem);

		while (1)
		{
			hOldItem = hChildItem;
			hChildItem = treeCtrl.GetNextItem(hChildItem, TVGN_NEXT);

			treeCtrl.DeleteItem(hOldItem);

			if (hChildItem == NULL)
			{
				break;
			}
		}
	}
}
