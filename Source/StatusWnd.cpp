//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

/
// CStatusWnd.cpp
//
#include "stdafx.h"
#include "easyb.h"
#include "mainfrm.h"
#include "MainFrameOpts.h"
#include "StatusWnd.h"
#include "StatusSheet.h"


//
CStatusWnd*	CStatusWnd::m_pWnd = NULL;


//////////////////////////////////////////////////////////////////////
CStatusWnd::CStatusWnd()
{
	m_pStatusSheet = NULL;
	m_bInitialized = FALSE;
}


///////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CStatusWnd, CCJControlBar)
	//{{AFX_MSG_MAP(CStatusWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_MOVE()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



//
int CStatusWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCJControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pStatusSheet = new CStatusSheet("Status",this,0);
	m_pStatusSheet->m_psh.dwFlags |= PSH_MODELESS;
//	if (!m_pStatusSheet->Create(this, WS_CHILD | WS_VISIBLE, 0))
	if (!m_pStatusSheet->Create(this, WS_CHILD, 0))
	{
		delete m_pStatusSheet;
		m_pStatusSheet = NULL;
		return -1;
	}
	//
	m_pStatusSheet->SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE);
	m_pStatusSheet->ShowWindow(SW_SHOW);
	m_pStatusSheet->UpdateWindow();
	m_bInitialized = TRUE;

	//
	m_pWnd = this;
	return 0;
}



//
void CStatusWnd::OnClose() 
{
	CCJControlBar::OnClose();
	// notify parent that we've closed
	pMAINFRAME->InformChildFrameClosed(twStatusDialog);
}


//
BOOL CStatusWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch(wParam)
	{
		case WMS_SETTEXT:
		case WMS_SET_FEEDBACK_TEXT:
		case WMS_SET_GIB_TEXT:
			return m_pStatusSheet->SendMessage(WM_COMMAND, wParam, lParam);
	}
	//
	return CCJControlBar::OnCommand(wParam, lParam);
}


void CStatusWnd::OnMove(int x, int y) 
{
	CCJControlBar::OnMove(x, y);
	// update main frame on our new position
	if (IsWindowVisible() && m_bInitialized)
	{
		RECT rect;
		GetWindowRect(&rect);
		pMAINFRAME->SetValue(tnStatusDialogRectLeft, rect.left);
		pMAINFRAME->SetValue(tnStatusDialogRectTop, rect.top);
		pMAINFRAME->SetValue(tnStatusDialogRectRight, rect.right);
		pMAINFRAME->SetValue(tnStatusDialogRectBottom, rect.bottom);
	}
}


//
void CStatusWnd::OnSize(UINT nType, int cx, int cy) 
{
	CCJControlBar::OnSize(nType, cx, cy);
	// resize the property sheet
	CRect clientRect;
	GetClientRect(&clientRect);

	// adjust the client rect info sent to the child window 
	if (m_dwStyle & CBRS_FLOATING)
		clientRect.DeflateRect(2, 2, 2, 2);
	else if (m_dwStyle & CBRS_ORIENT_HORZ) 
		clientRect.DeflateRect(14, 0, 0, 0);
	else
		clientRect.DeflateRect(1, 15, 1, 1);
	//
	m_pStatusSheet->MoveWindow(&clientRect);
	m_pStatusSheet->UpdateWindow();

	// update mainframe on our position 
	if (m_bInitialized && (cx > 0) && (cy > 0))
	{
		const int nBarWidth = 4;
		const int nBarHeight = 6;
		if (m_dwStyle & CBRS_ORIENT_HORZ) 
//			pMAINFRAME->SetValue(tnStatusDialogHeight, cy + nBarHeight);
			pMAINFRAME->SetValue(tnDockingWndHeight, cy + nBarHeight);
		else if (m_dwStyle & CBRS_ORIENT_VERT) 
//			pMAINFRAME->SetValue(tnStatusDialogWidth, cx + nBarWidth);
			pMAINFRAME->SetValue(tnDockingWndWidth, cx + nBarWidth);
	}
}


//
// OnKeyDown()
//
// pass along to the parent
//
void CStatusWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//	CCJControlBar::OnKeyDown(nChar, nRepCnt, nFlags);
//	GetParent()->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
	pMAINFRAME->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
}


//
void CStatusWnd::Update(int nCode)
{ 
	m_pStatusSheet->Update(nCode); 
}


//
void CStatusWnd::Clear() 
{ 
	m_pStatusSheet->Clear(); 
}


//
void CStatusWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	//
	ClientToScreen(&point);
	CCJControlBar::OnContextMenu(this, point);
}


//
void CStatusWnd::SetActiveTab(int nTab)
{
	if (!IsWindowVisible())
		return;
	//
	switch(nTab)
	{
		case SP_ANALYSES:
			m_pStatusSheet->SetActivePage(0);
			break;
		case SP_CARD_LOCATIONS:
			m_pStatusSheet->SetActivePage(1);
			break;
		case SP_PLAY_PLAN:
			m_pStatusSheet->SetActivePage(2);
			break;
		case SP_SUIT_STATUS:
			m_pStatusSheet->SetActivePage(3);
			break;
		case SP_GIB_MONITOR:
			m_pStatusSheet->SetActivePage(4);
			break;
/*
		case SP_FEEDBACK:
			m_pStatusSheet->SetActivePage(5);
			break;
*/
		default:
			ASSERT(FALSE);
	}
}

//
BOOL CStatusWnd::IsTabActive(int nTab)
{
	if (!IsWindowVisible())
		return FALSE;
	//
	switch(m_pStatusSheet->GetActiveIndex())
	{
		case 0:
			return (nTab == SP_ANALYSES);
		case 1:
			return (nTab == SP_CARD_LOCATIONS);
		case 2:
			return (nTab == SP_PLAY_PLAN);
		case 3:
			return (nTab == SP_SUIT_STATUS);
		case 4:
			return (nTab == SP_GIB_MONITOR);
/*
		case 5:
			return (nTab == SP_FEEDBACK);
*/
		default:
			return FALSE;
	}
}
