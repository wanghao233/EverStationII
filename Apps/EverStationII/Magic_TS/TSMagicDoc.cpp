// DocumentForTS.cpp: 实现文件
//

#include "stdafx.h"
#include "../EverStationII.h"
#include "TSMagicDoc.h"


// CTSMagicDoc

IMPLEMENT_DYNCREATE(CTSMagicDoc, CDocument)

CTSMagicDoc::CTSMagicDoc()
{
	m_strDebug = "天王盖地虎";
}

BOOL CTSMagicDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CTSMagicDoc::~CTSMagicDoc()
{
}


BEGIN_MESSAGE_MAP(CTSMagicDoc, CDocument)
END_MESSAGE_MAP()


// CTSMagicDoc 诊断

#ifdef _DEBUG
void CTSMagicDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CTSMagicDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CTSMagicDoc 序列化

void CTSMagicDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  在此添加存储代码
	}
	else
	{
		// TODO:  在此添加加载代码
	}
}
#endif


// CTSMagicDoc 命令
