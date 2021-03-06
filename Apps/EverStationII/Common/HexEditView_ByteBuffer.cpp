// HexEditView_ByteBuffer.cpp: 实现文件
//

#include "stdafx.h"
#include "HexEditView_ByteBuffer.h"

// CHexEditView_ByteBuffer

IMPLEMENT_DYNCREATE(CHexEditView_ByteBuffer, CEditView)

char hextable[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
#define TOHEX(a, b)	{*b++ = hextable[a >> 4];*b++ = hextable[a&0xf];}

#define MAXLEN 0x200

CHexEditView_ByteBuffer::CHexEditView_ByteBuffer()
{
#if 1
	m_pData = NULL;		// pointer to data
	m_length = 0;		// length of data
#else
	m_pData = (LPBYTE)malloc(MAXLEN);
	for (int i = 0; i < MAXLEN; i++)
	{	
		m_pData[i] = i;
	}

	m_length = MAXLEN;
#endif

	m_topindex = 0;
	m_bpr = 8;		// byte per row 
	m_lpp = 1;

	m_bShowHex = TRUE;
	m_bShowAscii = TRUE;
	m_bShowAddress = TRUE;
	m_bAddressIsWide = TRUE;		// 4/8 byte address

	m_offAddress = 0;
	m_offHex = 0;
	m_offAscii = 0;

	m_bUpdate = TRUE;			// update font info
	m_bNoAddressChange = FALSE;
	m_currentMode = EDIT_NONE;

	m_editPos.x = m_editPos.y = 0;
	m_currentAddress = 0;
	m_bHalfPage = TRUE;

	m_selStart = 0xffffffff;
	m_selEnd = 0xffffffff;

	m_AddOff    = 0x0;
	m_Font.CreateFont(-14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Courier New");
}

CHexEditView_ByteBuffer::~CHexEditView_ByteBuffer()
{
	if (m_pData != NULL)
	{
		free(m_pData);
		m_pData = NULL;
	}
}

BEGIN_MESSAGE_MAP(CHexEditView_ByteBuffer, CEditView)
	ON_WM_CONTEXTMENU()
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_GETDLGCODE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CHexEditView_ByteBuffer 诊断

#ifdef _DEBUG
void CHexEditView_ByteBuffer::AssertValid() const
{
	CEditView::AssertValid();
}

#ifndef _WIN32_WCE
void CHexEditView_ByteBuffer::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif
#endif //_DEBUG


// CHexEditView_ByteBuffer 消息处理程序
void CHexEditView_ByteBuffer::OnPaint()
{
	CPaintDC pdc(this); // device context for painting
						// TODO: 在此处添加消息处理程序代码
						// 不为绘图消息调用 CEditView::OnPaint()
	CRect rc;
	GetClientRect(rc);

	CDC	dc;
	dc.CreateCompatibleDC(CDC::FromHandle(pdc.m_ps.hdc));
	CBitmap bm;

	bm.CreateCompatibleBitmap(CDC::FromHandle(pdc.m_ps.hdc), rc.Width(), rc.Height());
	dc.SelectObject(bm);

	CBrush b;
	b.CreateSolidBrush(RGB(0xff, 0xff, 0xff));
	dc.FillRect(rc, &b);

	ASSERT(m_currentAddress >= 0);
	ASSERT(m_topindex >= 0);

	dc.SelectObject(m_Font);
	int		height = 0;
	int		x, y;
	char	buf[256];

	int	 n=0;
	x = rc.TopLeft().x;
	y = rc.TopLeft().y;

	dc.SetBoundsRect(&rc, DCB_DISABLE);

	if (m_pData)
	{
		//
		// get char dimensions
		//
		if (m_bUpdate)
		{
			dc.GetCharWidth('0', '0', &m_nullWidth);
			CSize sz = dc.GetTextExtent("0", 1);
			m_lineHeight = sz.cy;

			m_offHex = m_bShowAddress ? (m_bAddressIsWide ? m_nullWidth * 9 : m_nullWidth * 5) : 0;
			m_offAscii = m_bShowAddress ? (m_bAddressIsWide ? m_nullWidth * 9 : m_nullWidth * 5) : 0;
			m_offAscii += m_bShowHex ? (m_bpr * 3 * m_nullWidth) : 0;

			m_lpp = rc.Height() / m_lineHeight;
			m_bHalfPage = FALSE;
			if (m_lpp * m_bpr > m_length)
			{
				m_lpp = (m_length + (m_bpr / 2)) / m_bpr;
				if (m_length % m_bpr != 0)
				{
					m_bHalfPage = TRUE;
					m_lpp++;
				}
			}
			m_bUpdate = FALSE;
			UpdateScrollbars();
		}

		TRACE("%i %i\n", m_topindex, m_selStart);

		height = rc.Height() / m_lineHeight;
		height *= m_lineHeight;

		if (m_bShowAddress)
		{
			char fmt[8] = { '%','0','8','l','X' };
			fmt[2] = m_bAddressIsWide ? '8' : '4';
			int w = m_bAddressIsWide ? 8 : 4;
			y = 0;
			CRect rcd = rc;
			rcd.TopLeft().x = m_offAddress;
			for(int	 i = m_topindex + m_AddOff; (i < m_length+ m_AddOff) && (rcd.TopLeft().y < height); i+= m_bpr)
			{
				sprintf_s(buf, sizeof(buf), fmt, i);
				dc.DrawText(buf, w, rcd, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
				rcd.TopLeft().y += m_lineHeight;
			}
		}
		if (m_bShowHex)
		{
			y = 0;
			CRect rcd = rc;
			rcd.TopLeft().x = x = m_offHex;

			if (m_selStart != 0xffffffff && (m_currentMode == EDIT_HIGH || m_currentMode == EDIT_LOW))
			{
				int	 i;
				int	 n = 0;
				int	 selStart = m_selStart, selEnd = m_selEnd;
				if (selStart > selEnd)
					selStart ^= selEnd ^= selStart ^= selEnd;

				for (i = m_topindex; (i < selStart) && (y < height); i++)
				{
					char* p = &buf[0];
					TOHEX(m_pData[i], p);
					*p++ = ' ';
					dc.TextOut(x, y, buf, 3);
					x += m_nullWidth * 3;
					n++;
					if (n == m_bpr)
					{
						n = 0;
						x = m_offHex;
						y += m_lineHeight;
					}
				}
				dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
				dc.SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
				for (; (i < selEnd) && (i < m_length) && (y < height); i++)
				{
					char* p = &buf[0];
					TOHEX(m_pData[i], p);
					*p++ = ' ';
					dc.TextOut(x, y, buf, 3);
					x += m_nullWidth * 3;
					n++;
					if (n == m_bpr)
					{
						n = 0;
						x = m_offHex;
						y += m_lineHeight;
					}
				}
				dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
				dc.SetBkColor(GetSysColor(COLOR_WINDOW));
				for (; (i < m_length) && (y < height); i++)
				{
					char* p = &buf[0];
					TOHEX(m_pData[i], p);
					*p++ = ' ';
					dc.TextOut(x, y, buf, 3);
					x += m_nullWidth * 3;
					n++;
					if (n == m_bpr)
					{
						n = 0;
						x = m_offHex;
						y += m_lineHeight;
					}
				}
			}
			else
			{
				for (int i = m_topindex; (i < m_length) && (rcd.TopLeft().y < height);)
				{
					char* p = &buf[0];
					int n;
					for (n = 0; (n < m_bpr) && (i < m_length); n++)
					{
						TOHEX(m_pData[i], p);
						*p++ = ' ';
						i++;
					}
					while (n < m_bpr)
					{
						*p++ = ' ';	*p++ = ' ';	*p++ = ' ';
						n++;
					}

					dc.DrawText(buf, m_bpr * 3, rcd, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
					rcd.TopLeft().y += m_lineHeight;
				}
			}
		}
		if (m_bShowAscii)
		{
			y = 0;
			CRect rcd = rc;
			rcd.TopLeft().x = x = m_offAscii;
			if (m_selStart != 0xffffffff && m_currentMode == EDIT_ASCII)
			{
				int	 i;
				int	 n = 0;
				int	 selStart = m_selStart, selEnd = m_selEnd;
				if (selStart > selEnd)
					selStart ^= selEnd ^= selStart ^= selEnd;

				for (i = m_topindex; (i < selStart) && (y < height); i++)
				{
					buf[0] = isprint(m_pData[i]) ? m_pData[i] : '.';
					dc.TextOut(x, y, buf, 1);
					x += m_nullWidth;
					n++;
					if (n == m_bpr)
					{
						n = 0;
						x = m_offAscii;
						y += m_lineHeight;
					}
				}
				dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
				dc.SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
				for (; (i < selEnd) && (y < height); i++)
				{
					buf[0] = isprint(m_pData[i]) ? m_pData[i] : '.';
					dc.TextOut(x, y, buf, 1);
					x += m_nullWidth;
					n++;
					if (n == m_bpr)
					{
						n = 0;
						x = m_offAscii;
						y += m_lineHeight;
					}
				}
				dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
				dc.SetBkColor(GetSysColor(COLOR_WINDOW));
				for (; (i < m_length) && y < height; i++)
				{
					buf[0] = isprint(m_pData[i]) ? m_pData[i] : '.';
					dc.TextOut(x, y, buf, 1);
					x += m_nullWidth;
					n++;
					if (n == m_bpr)
					{
						n = 0;
						x = m_offAscii;
						y += m_lineHeight;
					}
				}
			}
			else
			{
				for (int i = m_topindex; (i < m_length) && (rcd.TopLeft().y < height);)
				{
					char* p = &buf[0];
					for(	 n = 0; (n < m_bpr) && (i < m_length); n++)
					{
						*p++ = isprint(m_pData[i]) ? m_pData[i] : '.';
						i++;
					}
					dc.DrawText(buf, n, rcd, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
					rcd.TopLeft().y += m_lineHeight;
				}
			}
		}
	}
	pdc.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, 0, 0, SRCCOPY);
}


void CHexEditView_ByteBuffer::OnSetFocus(CWnd* pOldWnd)
{
	// TODO: 在此处添加消息处理程序代码
	if(m_pData && !IsSelected())
	{
		//if(m_editPos.x == 0 && m_bShowAddress)
		//	CreateAddressCaret();
		//else
		if (m_editPos.x != 0)
			CreateEditCaret();
		SetCaretPos(m_editPos);
		ShowCaret();
	}
	CEditView::OnSetFocus(pOldWnd);
}


void CHexEditView_ByteBuffer::OnKillFocus(CWnd* pNewWnd)
{
	// TODO: 在此处添加消息处理程序代码
	DestroyCaret();
	CEditView::OnKillFocus(pNewWnd);
}

void CHexEditView_ByteBuffer::OnSize(UINT nType, int cx, int cy)
{
	CEditView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}

void CHexEditView_ByteBuffer::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//pScrollBar;
	if(!m_pData)
		return;

	int oa = m_topindex;
	
	switch(nSBCode)
	{
		case SB_LINEDOWN:
			if(m_topindex < m_length - m_lpp*m_bpr)
			{
				//ScrollWindow(0, -m_lineHeight, rc);
				m_topindex += m_bpr;
				RedrawWindow();
			}
			break;
		
		case SB_LINEUP:
			if(m_topindex > 0)
			{
				//ScrollWindow(0, m_lineHeight, rc);
				m_topindex -= m_bpr;
				RedrawWindow();
			}
			break;
		
		case SB_PAGEDOWN:
			if(m_topindex < m_length - m_lpp*m_bpr)
			{
				m_topindex += m_bpr * m_lpp;
				if(m_topindex > m_length - m_lpp*m_bpr)
					m_topindex = m_length - m_lpp*m_bpr;
				RedrawWindow();
			}
			break;

		case SB_PAGEUP:
			if(m_topindex > 0)
			{
				m_topindex -= m_bpr * m_lpp;
				if(m_topindex < 0)
					m_topindex = 0;
				RedrawWindow();
			}
			break;

		case SB_THUMBTRACK:
			m_topindex = nPos * m_bpr;
			RedrawWindow();
			break;
	}
	::SetScrollPos(this->m_hWnd, SB_VERT, m_topindex / m_bpr, TRUE);
	if(!m_bNoAddressChange)
		m_currentAddress += (m_topindex - oa);
	TRACE("!!%d %d %d",m_topindex,oa,m_currentAddress);
	RepositionCaret(m_currentAddress);

//	CEditView::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CHexEditView_ByteBuffer::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

//	CEditView::OnHScroll(nSBCode, nPos, pScrollBar);
}



UINT CHexEditView_ByteBuffer::OnGetDlgCode()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return DLGC_WANTALLKEYS;

//	return CEditView::OnGetDlgCode();
}

BOOL CHexEditView_ByteBuffer::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 在此添加专用代码和/或调用基类

	//dwStyle |= WS_HSCROLL | WS_VSCROLL | ES_READONLY;
	dwStyle |= WS_HSCROLL | WS_VSCROLL;

	BOOL bRet = CEditView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	if(bRet)
		SetFont(&m_Font);
	return bRet;
}

BOOL CHexEditView_ByteBuffer::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
//	return CEditView::OnEraseBkgnd(pDC);
}

void CHexEditView_ByteBuffer::SetOptions(BOOL a, BOOL h, BOOL c, BOOL w)
{
	m_bShowHex		= h;
	m_bShowAscii	= c;
	m_bShowAddress	= a;
	m_bAddressIsWide= w;		// 4/8 byte address
	m_bUpdate		= TRUE;
}

void CHexEditView_ByteBuffer::SetBPR(int bpr)
{
	m_bpr = bpr;
	m_bUpdate		= TRUE;
}


void CHexEditView_ByteBuffer::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetFocus();
	if(!m_pData)
		return;

	if(nFlags & MK_SHIFT)
	{
		m_selStart = m_currentAddress;
	}
	CPoint pt = CalcPos(point.x, point.y);
	if(pt.x > -1)
	{
		m_editPos = pt;
		pt.x *= m_nullWidth;
		pt.y *= m_lineHeight;
		
		//if(pt.x == 0 && m_bShowAddress)
		//	CreateAddressCaret();
		//else
		if (pt.x != 0)
			CreateEditCaret();

		SetCaretPos(pt);
		if(nFlags & MK_SHIFT)
		{
			m_selEnd = m_currentAddress;
			if(m_currentMode == EDIT_HIGH || m_currentMode == EDIT_LOW)
				m_selEnd++;
			RedrawWindow();
		}
	}
	if(!(nFlags & MK_SHIFT))
	{
		if(::DragDetect(this->m_hWnd, point))
		{
			m_selStart = m_currentAddress;
			m_selEnd   = m_selStart;
			SetCapture();
		}
		else
		{
			BOOL bsel = m_selStart != 0xffffffff;

			m_selStart = 0xffffffff;
			m_selEnd   = 0xffffffff;
			if(bsel)
				RedrawWindow();
		}
	}
	if(!IsSelected())
	{
		ShowCaret();
	}

//	CEditView::OnLButtonDown(nFlags, point);
}


void CHexEditView_ByteBuffer::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

//	CEditView::OnLButtonDblClk(nFlags, point);
}


CPoint CHexEditView_ByteBuffer::CalcPos(int x, int y)
{
	y /= m_lineHeight;
	if(y < 0 || y > m_lpp)
		return CPoint(-1, -1);

	if(y * m_bpr > m_length)
		return CPoint(-1, -1);

	x += m_nullWidth;
	x /= m_nullWidth;

	int xp;

	if(m_bShowAddress && x <= (m_bAddressIsWide ? 8 : 4))
	{
		m_currentAddress = m_topindex + (m_bpr * y);
		m_currentMode = EDIT_NONE;
		return CPoint(0, y);
	}

	xp = (m_offHex  / m_nullWidth) + m_bpr * 3;
	if(m_bShowHex && x < xp)
	{
		if(x%3)
			x--;
		m_currentAddress = m_topindex + (m_bpr * y) + (x - (m_offHex  / m_nullWidth)) / 3;
		m_currentMode =  ((x%3) & 0x01) ? EDIT_LOW : EDIT_HIGH;
		return CPoint(x, y);
	}

	xp = (m_offAscii  / m_nullWidth) + m_bpr;
	if(m_bShowAscii && x <= xp)
	{
		m_currentAddress = m_topindex + (m_bpr * y) + (x - (m_offAscii  / m_nullWidth));
		m_currentMode = EDIT_ASCII;
		return CPoint(x, y);
	}

	return CPoint(-1,-1);
}

void CHexEditView_ByteBuffer::CreateAddressCaret()
{
	DestroyCaret();
	CreateSolidCaret(m_nullWidth * (m_bAddressIsWide ? 8 : 4), m_lineHeight);
}

void CHexEditView_ByteBuffer::CreateEditCaret()
{
	DestroyCaret();
	CreateSolidCaret(m_nullWidth, m_lineHeight);
}


void CHexEditView_ByteBuffer::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(!m_pData)
		return;

	if(nFlags & MK_LBUTTON && m_selStart != 0xffffffff)
	{
		CRect rc;
		GetClientRect(&rc);
		if(!rc.PtInRect(point))
		{
			if(point.y < 0)
			{
				OnVScroll(SB_LINEUP, 0, NULL);
				point.y = 0;
			}
			else if(point.y > rc.Height())
			{
				OnVScroll(SB_LINEDOWN, 0, NULL);
				point.y = rc.Height() -1;
			}
		}

		//
		// we are selecting
		//
		int	 seo = m_selEnd;
		CPoint pt = CalcPos(point.x, point.y);
		if(pt.x > -1)
		{
			m_selEnd = m_currentAddress;
			if(m_currentMode == EDIT_HIGH || m_currentMode == EDIT_LOW)
				m_selEnd++;
		}
		if(IsSelected())
			DestroyCaret();

		if(seo != m_selEnd)
			RedrawWindow();
	}

//	CEditView::OnMouseMove(nFlags, point);
}


void CHexEditView_ByteBuffer::UpdateScrollbars(void)
{
	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = (m_length / m_bpr) - 1;
	// =============================================================
	// BugFix: 如果不能整除要加1
	// =============================================================
	if (m_length % m_bpr != 0)
	{
		si.nMax += 1;
	}
	si.nPage = m_lpp;
	si.nPos = m_topindex / m_bpr;

	::SetScrollInfo(this->m_hWnd, SB_VERT, &si, TRUE);
	if (si.nMax > (int)si.nPage)
		::EnableScrollBar(this->m_hWnd, SB_VERT, ESB_ENABLE_BOTH);

	CRect rc;
	GetClientRect(&rc);

	si.nMin = 0;
	si.nMax = ((m_bShowAddress ? (m_bAddressIsWide ? 8 : 4) : 0) +
		(m_bShowHex ? m_bpr * 3 : 0) +
		(m_bShowAscii ? m_bpr : 0)) * m_nullWidth;
	si.nPage = 1;
	si.nPos = 0;
	::SetScrollInfo(this->m_hWnd, SB_HORZ, &si, TRUE);
}


inline BOOL CHexEditView_ByteBuffer::IsSelected()
{
	return m_selStart != 0xffffffff;
}

void CHexEditView_ByteBuffer::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(IsSelected())
		ReleaseCapture();

	CEditView::OnLButtonUp(nFlags, point);
}

void CHexEditView_ByteBuffer::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	nFlags;nRepCnt;
//	if(!m_pData)
//		return;
//	if(nChar == '\t')
//		return;
//	if(GetKeyState(VK_CONTROL) & 0x80000000)
//	{
//		switch(nChar)
//		{
//			case 0x03:
//				if(IsSelected())
//					OnEditCopy();
//				return;
//			case 0x16:
//				OnEditPaste();
//				return;
//			case 0x18:
//				if(IsSelected())
//					OnEditCut();
//				return;
//			case 0x1a:
//				OnEditUndo();
//				return;
//		}
//	}
//
//	if(nChar == 0x08)
//	{
//		if(m_currentAddress > 0)
//		{
//			m_currentAddress--;
//			SelDelete(m_currentAddress, m_currentAddress+1);
//			RepositionCaret(m_currentAddress);
//			RedrawWindow();
//		}
//		return;
//	}
//
//	SetSel(-1, -1);
//	switch(m_currentMode)
//	{
//		case EDIT_NONE:
//			return;
//		case EDIT_HIGH:
//		case EDIT_LOW:
//			if((nChar >= '0' && nChar <= '9') || (nChar >= 'a' && nChar <= 'f'))
//			{
//				UINT b = nChar - '0';
//				if(b > 9) 
//					b = 10 + nChar - 'a';
//
//				if(m_currentMode == EDIT_HIGH)
//				{
//					m_pData[m_currentAddress] = (unsigned char)((m_pData[m_currentAddress] & 0x0f) | (b << 4));
//				}
//				else
//				{
//					m_pData[m_currentAddress] = (unsigned char)((m_pData[m_currentAddress] & 0xf0) | b);
//				}
//				Move(1,0);
//			}
//			break;
//		case EDIT_ASCII:
//			m_pData[m_currentAddress] = (unsigned char)nChar;
//			Move(1,0);
//			break;
//	}
//	RedrawWindow();

	//想实现的效果是禁止编辑输入，因此注释下面这行调用。  chendelin  2018.5.8
	//CEditView::OnChar(nChar, nRepCnt, nFlags);
}


//void CHexEditView_ByteBuffer::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	nFlags;	nRepCnt;
//
//	BOOL bShift = GetKeyState(VK_SHIFT) & 0x80000000;
//	BOOL bac = m_bNoAddressChange;
//	m_bNoAddressChange = TRUE;
//	switch(nChar)
//	{
//		case VK_DOWN:
//			if(bShift)
//			{
//				if(!IsSelected())
//				{
//					m_selStart = m_currentAddress;
//				}
//				Move(0,1);
//				m_selEnd   = m_currentAddress;
//				if(m_currentMode == EDIT_HIGH || m_currentMode == EDIT_LOW)
//					m_selEnd++;
//				RedrawWindow();
//				break;
//			}
//			else
//				SetSel(-1, -1);
//			Move(0,1);
//			break;
//		case VK_UP:
//			if(bShift)
//			{
//				if(!IsSelected())
//				{
//					m_selStart = m_currentAddress;
//				}
//				Move(0,-1);
//				m_selEnd   = m_currentAddress;
//				RedrawWindow();
//				break;
//			}
//			else
//				SetSel(-1, -1);
//			Move(0,-1);
//			break;
//		case VK_LEFT:
//			if(bShift)
//			{
//				if(!IsSelected())
//				{
//					m_selStart = m_currentAddress;
//				}
//				Move(-1,0);
//				m_selEnd   = m_currentAddress;
//				RedrawWindow();
//				break;
//			}
//			else
//				SetSel(-1, -1);
//			Move(-1,0);
//			break;
//		case VK_RIGHT:
//			if(bShift)
//			{
//				if(!IsSelected())
//				{
//					m_selStart = m_currentAddress;
//				}
//				Move(1,0);
//				m_selEnd   = m_currentAddress;
//				if(m_currentMode == EDIT_HIGH || m_currentMode == EDIT_LOW)
//					m_selEnd++;
//				RedrawWindow();
//				break;
//			}
//			else
//				SetSel(-1, -1);
//			Move(1,0);
//			break;
//		case VK_PRIOR:
//			if(bShift)
//			{
//				if(!IsSelected())
//				{
//					m_selStart = m_currentAddress;
//				}
//				OnVScroll(SB_PAGEUP, 0, NULL);
//				Move(0,0);
//				m_selEnd   = m_currentAddress;
//				RedrawWindow();
//				break;
//			}
//			else
//				SetSel(-1, -1);
//			OnVScroll(SB_PAGEUP, 0, NULL);
//			Move(0,0);
//			break;
//		case VK_NEXT:
//			if(bShift)
//			{
//				if(!IsSelected())
//				{
//					m_selStart = m_currentAddress;
//				}
//				OnVScroll(SB_PAGEDOWN, 0, NULL);
//				Move(0,0);
//				m_selEnd   = m_currentAddress;
//				RedrawWindow();
//				break;
//			}
//			else
//				SetSel(-1, -1);
//			OnVScroll(SB_PAGEDOWN, 0, NULL);
//			Move(0,0);
//			break;
//		case VK_HOME:
//			if(bShift)
//			{
//				if(!IsSelected())
//				{
//					m_selStart = m_currentAddress;
//				}
//				if(GetKeyState(VK_CONTROL) & 0x80000000)
//				{
//					OnVScroll(SB_THUMBTRACK, 0, NULL);
//					Move(0,0);
//				}
//				else
//				{
//					m_currentAddress /= m_bpr;
//					m_currentAddress *= m_bpr;
//					Move(0,0);
//				}
//				m_selEnd   = m_currentAddress;
//				RedrawWindow();
//				break;
//			}
//			else
//				SetSel(-1, -1);
//			if(GetKeyState(VK_CONTROL) & 0x80000000)
//			{
//				OnVScroll(SB_THUMBTRACK, 0, NULL);
//				m_currentAddress = 0;
//				Move(0,0);
//			}
//			else
//			{
//				m_currentAddress /= m_bpr;
//				m_currentAddress *= m_bpr;
//				Move(0,0);
//			}
//			break;
//		case VK_END:
//			if(bShift)
//			{
//				if(!IsSelected())
//				{
//					m_selStart = m_currentAddress;
//				}
//				if(GetKeyState(VK_CONTROL) & 0x80000000)
//				{
//					m_currentAddress = m_length-1;
//					OnVScroll(SB_THUMBTRACK, ((m_length+(m_bpr/2)) / m_bpr) - m_lpp, NULL);
//					Move(0,0);
//				}
//				else
//				{
//					m_currentAddress /= m_bpr;
//					m_currentAddress *= m_bpr;
//					m_currentAddress += m_bpr - 1;
//					if(m_currentAddress > m_length)
//						m_currentAddress = m_length-1;
//					Move(0,0);
//				}
//				m_selEnd   = m_currentAddress;
//				RedrawWindow();
//				break;
//			}
//			else
//				SetSel(-1, -1);
//			if(GetKeyState(VK_CONTROL) & 0x80000000)
//			{
//				m_currentAddress = m_length-1;
//				if(m_bHalfPage)
//					OnVScroll(SB_THUMBTRACK, 0, NULL);
//				else
//					OnVScroll(SB_THUMBTRACK, ((m_length+(m_bpr/2)) / m_bpr) - m_lpp, NULL);
//				Move(0,0);
//			}
//			else
//			{
//				m_currentAddress /= m_bpr;
//				m_currentAddress *= m_bpr;
//				m_currentAddress += m_bpr - 1;
//				if(m_currentAddress > m_length)
//					m_currentAddress = m_length-1;
//				Move(0,0);
//			}
//			break;
//		case VK_INSERT:
//			SelInsert(m_currentAddress, max(1, m_selEnd-m_selStart));
//			RedrawWindow();
//			break;
//		case VK_DELETE:
//			if(IsSelected())
//			{
//				OnEditClear();
//			}
//			else
//			{
//				SelDelete(m_currentAddress, m_currentAddress+1);
//				RedrawWindow();
//			}
//			break;
//		case '\t':
//			switch(m_currentMode)
//			{
//				case EDIT_NONE:
//					m_currentMode = EDIT_HIGH;
//					break;
//				case EDIT_HIGH:
//				case EDIT_LOW:
//					m_currentMode = EDIT_ASCII;
//					break;
//				case EDIT_ASCII:
//					m_currentMode = EDIT_HIGH;
//					break;
//			}
//			Move(0,0);
//			break;
//
//	}
//	m_bNoAddressChange = bac;

//	CEditView::OnKeyDown(nChar, nRepCnt, nFlags);
//}

void CHexEditView_ByteBuffer::Move(int x, int y)
{
	switch(m_currentMode)
	{
		case EDIT_NONE:
			return;
		case EDIT_HIGH:
			if(x != 0)
				m_currentMode = EDIT_LOW;
			if(x == -1)
				m_currentAddress --;
			m_currentAddress += y* m_bpr;
			break;
		case EDIT_LOW:
			if(x != 0)
				m_currentMode = EDIT_HIGH;
			if(x == 1)
				m_currentAddress++;
			m_currentAddress += y* m_bpr;
			break;
		case EDIT_ASCII:
			{
				m_currentAddress += x;
				m_currentAddress += y*m_bpr;
			}
			break;
	}
	if(m_currentAddress < 0)
		m_currentAddress = 0;

	if(m_currentAddress >= m_length)
	{
		m_currentAddress -= x;
		m_currentAddress -= y*m_bpr;
	}
	m_bNoAddressChange = TRUE;
	if(m_currentAddress < m_topindex)
	{
		OnVScroll(SB_LINEUP, 0, NULL);
	}
	if(m_currentAddress >= m_topindex + m_lpp*m_bpr)
	{
		OnVScroll(SB_LINEDOWN, 0, NULL);
	}
	m_bNoAddressChange = FALSE;
	//ScrollIntoView(m_currentAddress);
	RepositionCaret(m_currentAddress);
}

void CHexEditView_ByteBuffer::SetSel(int s, int e)
{
	DestroyCaret();
	m_selStart = s;
	m_selEnd = e;
	RedrawWindow();
	//if(m_editPos.x == 0 && m_bShowAddress)
	//	CreateAddressCaret();
	//else
	if (m_editPos.x != 0)
		CreateEditCaret();
	SetCaretPos(m_editPos);
	ShowCaret();
}

void CHexEditView_ByteBuffer::RepositionCaret(int	 p)
{
	int x, y;

	y = (p - m_topindex) / m_bpr;
	x = (p - m_topindex) % m_bpr;

	switch (m_currentMode)
	{
	case EDIT_NONE:
		//CreateAddressCaret();
		x = 0;
		break;
	case EDIT_HIGH:
		CreateEditCaret();
		x *= m_nullWidth * 3;
		x += m_offHex;
		break;
	case EDIT_LOW:
		CreateEditCaret();
		x *= m_nullWidth * 3;
		x += m_nullWidth;
		x += m_offHex;
		break;
	case EDIT_ASCII:
		CreateEditCaret();
		x *= m_nullWidth;
		x += m_offAscii;
		break;
	}
	m_editPos.x = x;
	m_editPos.y = y * m_lineHeight;
	CRect rc;
	GetClientRect(&rc);
	if (rc.PtInRect(m_editPos))
	{
		SetCaretPos(m_editPos);
		ShowCaret();
	}
}

void CHexEditView_ByteBuffer::ScrollIntoView(int p)
{
	if (p < m_topindex || p > m_topindex + m_lpp * m_bpr)
	{
		m_topindex = (p / m_bpr) * m_bpr;
		m_topindex -= (m_lpp / 3) * m_bpr;
		if (m_topindex < 0)
			m_topindex = 0;

		UpdateScrollbars();
		RedrawWindow();
	}
}




void CHexEditView_ByteBuffer::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	// CG: This block was added by the Pop-up Menu component
	{
		//if (point.x == -1 && point.y == -1)
		//{
		//	//keystroke invocation
		//	CRect rect;
		//	GetClientRect(rect);
		//	ClientToScreen(rect);

		//	point = rect.TopLeft();
		//	point.Offset(5, 5);
		//}

		//CMenu menu;
		//VERIFY(menu.LoadMenu(CG_IDR_POPUP_HEX_EDIT));

		//CMenu* pPopup = menu.GetSubMenu(0);
		//ASSERT(pPopup != NULL);

		//pPopup->EnableMenuItem(ID_EDIT_UNDO, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		//if(!IsSelected())
		//{
		//	pPopup->EnableMenuItem(ID_EDIT_CLEAR, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		//	pPopup->EnableMenuItem(ID_EDIT_CUT, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		//	pPopup->EnableMenuItem(ID_EDIT_COPY, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		//}
		//{
		//	COleDataObject	obj;	
		//	if (obj.AttachClipboard()) 
		//	{
		//		if(!obj.IsDataAvailable(CF_TEXT) && !obj.IsDataAvailable(RegisterClipboardFormat("BinaryData")))
		//			pPopup->EnableMenuItem(ID_EDIT_PASTE, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		//	}
		//}

		//pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		//	this);
	}
}
//void CHexEditView_ByteBuffer::OnEditClear()
//{
//	m_currentAddress = m_selStart;
//	SelDelete(m_selStart, m_selEnd);
//	RepositionCaret(m_currentAddress);
//	RedrawWindow();
//}
//
//void CHexEditView_ByteBuffer::OnEditCopy()
//{
//	COleDataSource*		pSource = new COleDataSource();
//	EmptyClipboard();
//	if (m_currentMode != EDIT_ASCII)
//	{
//		int			dwLen = (m_selEnd - m_selStart);
//		HGLOBAL		hMemb = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT, m_selEnd - m_selStart);
//		HGLOBAL		hMema = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT, (dwLen) * 3);
//
//		if (!hMema)
//			return;
//
//		// copy binary
//		LPBYTE	p = (BYTE*)::GlobalLock(hMemb);
//		memcpy(p, m_pData + m_selStart, dwLen);
//		::GlobalUnlock(hMemb);
//
//		// copy ascii
//		p = (BYTE*)::GlobalLock(hMema);
//		for (int i = 0; i < dwLen;)
//		{
//			TOHEX(m_pData[m_selStart + i], p);
//			*p++ = ' ';
//			i++;
//		}
//
//		::GlobalUnlock(hMema);
//
//		pSource->CacheGlobalData(RegisterClipboardFormat("BinaryData"), hMemb);
//		pSource->CacheGlobalData(CF_TEXT, hMema);
//	}
//	else
//	{
//		HGLOBAL				hMemb = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT, m_selEnd - m_selStart);
//		HGLOBAL				hMema = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT, m_selEnd - m_selStart);
//		if (!hMemb || !hMema)
//			return;
//
//		// copy binary
//		LPBYTE	p = (BYTE*)::GlobalLock(hMemb);
//		int		dwLen = m_selEnd - m_selStart;
//
//		memcpy(p, m_pData + m_selStart, dwLen);
//		::GlobalUnlock(hMemb);
//
//		// copy ascii
//		p = (BYTE*)::GlobalLock(hMema);
//		memcpy(p, m_pData + m_selStart, dwLen);
//		for (int i = 0; i < dwLen; p++, i++)
//			if (!isprint(*p))
//				*p = '.';
//		::GlobalUnlock(hMema);
//
//		pSource->CacheGlobalData(RegisterClipboardFormat("BinaryData"), hMemb);
//		pSource->CacheGlobalData(CF_TEXT, hMema);
//	}
//	pSource->SetClipboard();
//}
//
//void CHexEditView_ByteBuffer::OnEditCut()
//{
//	OnEditCopy();
//	SelDelete(m_selStart, m_selEnd);
//	RedrawWindow();
//}
//
//void CHexEditView_ByteBuffer::OnEditPaste()
//{
//	COleDataObject	obj;
//	if (obj.AttachClipboard())
//	{
//		HGLOBAL hmem = NULL;
//		if (obj.IsDataAvailable(RegisterClipboardFormat("BinaryData")))
//		{
//			hmem = obj.GetGlobalData(RegisterClipboardFormat("BinaryData"));
//		}
//		else if (obj.IsDataAvailable(CF_TEXT))
//		{
//			hmem = obj.GetGlobalData(CF_TEXT);
//		}
//		if (hmem)
//		{
//			LPBYTE	p = (BYTE*)::GlobalLock(hmem);
//			DWORD	dwLen = ::GlobalSize(hmem);
//			int		insert;
//			int		oa = m_currentAddress;
//
//			NormalizeSel();
//			if (m_selStart == 0xffffffff)
//			{
//				if (m_currentMode == EDIT_LOW)
//					m_currentAddress++;
//				insert = m_currentAddress;
//				SelInsert(m_currentAddress, dwLen);
//			}
//			else
//			{
//				insert = m_selStart;
//				SelDelete(m_selStart, m_selEnd);
//				SelInsert(insert, dwLen);
//				SetSel(-1, -1);
//			}
//
//			memcpy(m_pData + insert, p, dwLen);
//
//			m_currentAddress = oa;
//			RedrawWindow();
//			::GlobalUnlock(hmem);
//		}
//	}
//}
//
//void CHexEditView_ByteBuffer::OnEditSelectAll()
//{
//	m_selStart = 0;
//	m_selEnd = m_length;
//	DestroyCaret();
//	RedrawWindow();
//}
//
//void CHexEditView_ByteBuffer::OnEditUndo()
//{
//	// TODO: Add your command handler code here
//
//}

void CHexEditView_ByteBuffer::NormalizeSel()
{
	if (m_selStart > m_selEnd)
		m_selStart ^= m_selEnd ^= m_selStart ^= m_selEnd;
}

void CHexEditView_ByteBuffer::SelDelete(int s, int e)
{
	LPBYTE p = (LPBYTE)malloc(m_length - (e - s) + 1);
	memcpy(p, m_pData, s);
	if (s < m_length - (e - s))
		memcpy(p + s, m_pData + e, (m_length - e));

	free(m_pData);
	SetSel(-1, -1);
	m_pData = p;
	m_length = m_length - (e - s);
	if (m_currentAddress > m_length)
	{
		m_currentAddress = m_length;
		RepositionCaret(m_currentAddress);
	}
	m_bUpdate = TRUE;
}

void CHexEditView_ByteBuffer::SelInsert(int s, int l)
{
	LPBYTE p = (LPBYTE)calloc(m_length + l, 1);
	memcpy(p, m_pData, s);
	memcpy(p + s + l, m_pData + s, (m_length - s));
	free(m_pData);
	SetSel(-1, -1);
	m_pData = p;
	m_length = m_length + l;
	m_bUpdate = TRUE;
}

CSize CHexEditView_ByteBuffer::GetSel()
{
	return CSize(m_selStart, m_selEnd);
}

void CHexEditView_ByteBuffer::SetData(LPBYTE p, int len)
{
	if (m_pData != NULL)
	{
		free(m_pData);
		m_pData = NULL;
		m_length = 0;
	}

	m_pData = (LPBYTE)malloc(len);
	memcpy(m_pData, p, len);

	SetSel(-1, -1);
	m_length = len;
	m_currentAddress = 0;
	m_editPos.x = m_editPos.y = 0;
	m_currentMode = EDIT_HIGH;
	m_topindex = 0;
	m_bUpdate = TRUE;

	RedrawWindow();
}

int CHexEditView_ByteBuffer::GetData(LPBYTE p, int len)
{
	memcpy(p, m_pData, min(len, m_length));
	return m_length;
}

void CHexEditView_ByteBuffer::Reset(void)
{
	if (m_pData != NULL)
	{
		free(m_pData);
		m_pData = NULL;
		m_length = 0;

		RedrawWindow();
	}
}

void CHexEditView_ByteBuffer::DisplayByteBuffer(unsigned char* buf, int length)
{
	if (buf != NULL)
	{
		if (length > 0)
		{
			SetData(buf, length);
		}
	}
}



BOOL CHexEditView_ByteBuffer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (zDelta < 0)
	{
		SendMessage(WM_VSCROLL, SB_LINEDOWN, NULL);
	}
	else if (zDelta > 0)
	{
		SendMessage(WM_VSCROLL, SB_LINEUP, NULL);
	}

	return CEditView::OnMouseWheel(nFlags, zDelta, pt);
}
