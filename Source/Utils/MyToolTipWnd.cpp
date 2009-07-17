//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// MyToolTipWnd.cpp : implementation file
//

#include "stdafx.h"
#include "MyToolTipWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyToolTipWnd

CMyToolTipWnd::CMyToolTipWnd()
{
	m_hFont = NULL;
	m_location.x = m_location.y = 0;
}

CMyToolTipWnd::~CMyToolTipWnd() 
{
}


BEGIN_MESSAGE_MAP(CMyToolTipWnd, CWnd)
	//{{AFX_MSG_MAP(CMyToolTipWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMyToolTipWnd message handlers


//
BOOL CMyToolTipWnd::Create(CWnd* pParentWnd)
{
	CString strTooltipClassName = AfxRegisterWndClass(CS_SAVEBITS); 
	//
	m_hFont = (HFONT) ::GetStockObject(ANSI_VAR_FONT);
	m_pParentWnd = pParentWnd;
	//
	CRect rect(0,0,0,0);
	return CreateEx(WS_EX_TOOLWINDOW | WS_EX_NOPARENTNOTIFY, 
					strTooltipClassName, 
					"", 
					WS_BORDER | WS_POPUP | WS_DISABLED, 
					0, 0, 0, 0, 
					pParentWnd->m_hWnd, 
					NULL);
}


//
BOOL CMyToolTipWnd::SetText(LPCTSTR szText, CPoint* pPoint)
{
	m_strText = szText;
	if (pPoint)
		m_location = *pPoint;

	// resize window
	CWindowDC dc(this);
	CFont* pFont = CFont::FromHandle(m_hFont);
	CFont* pOldFont = dc.SelectObject(pFont);
	CSize size = dc.GetTextExtent(m_strText);
	CRect rect(0,0,0,0);
	rect.right = size.cx;
	rect.bottom = size.cy;
	AdjustWindowRect(&rect, GetStyle(), FALSE);

	// position the rect to 0,0
	rect.OffsetRect(-rect.left, -rect.top);

	// and locate at the cursor
	rect.OffsetRect(m_location.x - rect.left, m_location.y - rect.top);

	// improve visibility
//	rect.OffsetRect(4, -(rect.Height()/2));
//	rect.OffsetRect(5, 0);

	// set timer
	SetTimer(1, 100, NULL);

	// and display
	MoveWindow(&rect, TRUE);
	Invalidate();
	return TRUE;
}


//
void CMyToolTipWnd::OnTimer(UINT nIDEvent) 
{
	// turn off if no longer in rect
	CPoint pt;
	GetCursorPos(&pt);
	CRect rcItem;
	GetWindowRect(rcItem);
	if (!rcItem.PtInRect(pt))
	{
		KillTimer (1);
		Hide();
	}
//	CWnd::OnTimer(nIDEvent);
}


//
void CMyToolTipWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	//
	CRect rect;
	GetClientRect(&rect);
	CBrush brush(RGB(0, 255,255));
	dc.FillRect(&rect, &brush);
	CFont* pFont = CFont::FromHandle(m_hFont);
	CFont* pOldFont = dc.SelectObject(pFont);
	dc.TextOut(0, 0, m_strText);
	dc.SelectObject(pOldFont);
}



//
// ListControlHitTest()
//
// - helper function for using the custom tooltip control with a list control
//
int CMyToolTipWnd::ListControlHitTest(CPoint point, CListCtrl& listCtl, int nColumn)
{
	// get client position
	listCtl.ScreenToClient(&point);

	// get the index of the line under the cursor
	int nIndex = -1;
	int nFirstVisible = listCtl.GetTopIndex();
	int numVisible = listCtl.GetCountPerPage();
	CRect itemRect;
	for(int i=nFirstVisible;i<nFirstVisible+numVisible;i++)
	{
		if (listCtl.GetItemRect(i, &itemRect, LVIR_BOUNDS))
		{
			if (itemRect.PtInRect(point))
			{
				nIndex = i;
				break;
			}
		}
	}

	// bail out if not on a line
	if (nIndex < 0)
		return -1;

	// else the line was found, so return if the column doesn't matter
	if (nColumn < 0)
		return nIndex;

	// else need to check if the cursor is on the right column
	BOOL bShow = FALSE;
	BOOL bInPosition = FALSE;

	// check if the cursor is within the specified col
	CRect targetRect;
//	listCtl.ClientToScreen(&itemRect);
	targetRect.left = itemRect.left;
	// NCR-FFS Changed i to j
	for(int j=0;j<nColumn;j++)
		targetRect.left += listCtl.GetColumnWidth(j);
	targetRect.right = targetRect.left + listCtl.GetColumnWidth(nColumn);
	if ((point.x >= targetRect.left) && (point.x <= targetRect.right))
	{
		// point is in the proper column, so save the rect
		CRect rect;
		listCtl.GetItemRect(nIndex, &rect, LVIR_LABEL);
		m_location.x = rect.right;
		m_location.y = rect.top;
		// account for column padding
		m_location.x += 5;
		m_location.y -= 1;
		// return the line index
		listCtl.ClientToScreen(&m_location);
		return nIndex;
	}
	else
	{
		// not within the proper column
		return -1;
	}
}



