//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// HistoryWnd.cpp : implementation file
//

#include "stdafx.h"
#include "EasyB.h"
#include "mainfrm.h"
#include "MainFrameOpts.h"
#include "HistoryWnd.h"
#include "progopts.h"
#include "Subclass.h"
#include "Help\HelpCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



// offsets into client area for various orientations
const RECT trOffsetFloating = { 2, 4, 2, 2 };
const RECT trOffsetHorizontal = { 18, 2, 2, 2};
const RECT trOffsetVertical = { 2, 19, 2, 2};


/////////////////////////////////////////////////////////////////////////////
// CHistoryWnd

//IMPLEMENT_DYNCREATE(CHistoryWnd, CCJControlBar)

CHistoryWnd::CHistoryWnd()
{
	//
	m_bShowBidding = TRUE;
	m_bShowPlay = TRUE;
	m_nCurrentChild = -1;
	m_bInitialized = FALSE;
}

CHistoryWnd::~CHistoryWnd()
{
}


BEGIN_MESSAGE_MAP(CHistoryWnd, CCJControlBar)
	//{{AFX_MSG_MAP(CHistoryWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_KEYDOWN()
	ON_WM_MOVE()
	ON_WM_HELPINFO()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(IDC_CHECK1, OnClickWindowShow)
	ON_BN_CLICKED(IDC_CHECK2, OnClickWindowShow)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_HIDE, OnHide)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CHistoryWnd message handlers



//
int CHistoryWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCJControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create the bidding history panel
	RECT rect = { 0, 0, 0, 0 };
	if (!m_editBidding.Create(ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY | WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE, rect, this, IDC_EDIT1))
		return -1;
	m_editBidding.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	// and the play history panel
	if (!m_editPlay.Create(ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY | WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE, rect, this, IDC_EDIT2))
		return -1;
	m_editPlay.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	// subclass the edit controls
	wpOrigRichEditProc = (WNDPROC) SetWindowLong(m_editBidding.m_hWnd, GWL_WNDPROC, (LONG) RichEditSubclassProc);
	SetWindowLong(m_editPlay.m_hWnd, GWL_WNDPROC, (LONG) RichEditSubclassProc);

	// set font
	CFont* pFont = (CFont*) pMAINFRAME->GetValuePV(tpFontHistory);
	m_editBidding.SendMessage(WM_SETFONT, (LONG)pFont->m_hObject, FALSE);
	m_editPlay.SendMessage(WM_SETFONT, (LONG)pFont->m_hObject, FALSE);

	// get text metrics
	CWindowDC textDC(&m_editBidding);
	TEXTMETRIC tm;
	textDC.GetTextMetrics(&tm);
	m_nFontHeight = tm.tmHeight;

	// create static labels
	// first adjust rect then create
	CWindowDC dc(this);
	pFont = (CFont*) pMAINFRAME->GetValuePV(tpFontSmall);
	CFont* pOldFont = (CFont*) dc.SelectObject(pFont);
	CString strLabel = _T("Bidding History");
	CSize size = dc.GetTextExtent(strLabel, strLabel.GetLength());
	rect.right = size.cx;
	rect.bottom = size.cy;
	m_lblBidding.Create(strLabel, WS_CHILD | WS_VISIBLE, rect, this, IDC_STATIC1);

	// adjust rect again and create
	strLabel = _T("Play History");
	size = dc.GetTextExtent(strLabel, strLabel.GetLength());
	rect.right = size.cx;
	m_lblPlay.Create(strLabel, WS_CHILD | WS_VISIBLE, rect, this, IDC_STATIC2);

	// create the buttons
	m_btnBidding.Create(_T("Bidding"), WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE, rect, this, IDC_CHECK1);
	m_btnPlay.Create(_T("Play"), WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE, rect, this, IDC_CHECK2);

	// set check state
	m_btnBidding.SetCheck(m_bShowBidding);
	m_btnPlay.SetCheck(m_bShowPlay);

	// and set fonts
	m_btnBidding.SendMessage(WM_SETFONT, (UINT)GetStockObject(ANSI_VAR_FONT), 0L);
	m_btnPlay.SendMessage(WM_SETFONT, (UINT)GetStockObject(ANSI_VAR_FONT), 0L);

	// restore old font
	dc.SelectObject(pOldFont);

	// set fonts
	m_lblBidding.SendMessage(WM_SETFONT, (LONG)pFont->m_hObject, FALSE);
	m_lblPlay.SendMessage(WM_SETFONT, (LONG)pFont->m_hObject, FALSE);

	// create the popup menu
	m_menuPopup.LoadMenu(IDR_HISTORY_POPUP);

	// done
	m_bInitialized = TRUE;
	return 0;
}


//
void CHistoryWnd::OnClose() 
{
	// notify parent that we're closing
//	pMAINFRAME->InformChildFrameClosed(m_nChildType);
	//
	m_menuPopup.DestroyMenu();
	//
	CCJControlBar::OnClose();
}



//
BOOL CHistoryWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch (wParam)
	{
		case WMS_SETTEXT:
			if (lParam == 0)
				SetBiddingHistory((LPCTSTR) lParam);
			else
				SetPlayHistory((LPCTSTR) lParam);
			return TRUE;
	}

	//
	return CCJControlBar::OnCommand(wParam, lParam);
}



//
// OnRButtonDown()
//
void CHistoryWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// see which child this is
	CRect rect0, rect1;
	m_editBidding.GetWindowRect(&rect0);
	m_editPlay.GetWindowRect(&rect1);
	ScreenToClient(&rect0);
	ScreenToClient(&rect1);
	//
	if (rect0.PtInRect(point))
		m_nCurrentChild = 0;
	else if (rect1.PtInRect(point))
		m_nCurrentChild = 1;
	else
		m_nCurrentChild = -1;

	//
	ClientToScreen(&point);
	if (m_nCurrentChild >= 0)
	{
		CMenu* pPopup = m_menuPopup.GetSubMenu(0);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
							   point.x, 
							   point.y,
							   this, NULL);
	}
	else
	{
		CCJControlBar::OnContextMenu(this, point);
	}
}


//
void CHistoryWnd::OnUpdateMenuItem(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}


//
void CHistoryWnd::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	if ((m_nCurrentChild == 0) || (m_nCurrentChild == 1))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

//
// OnEditCopy() 
// 
void CHistoryWnd::OnEditCopy() 
{
	if (m_nCurrentChild == 0)
	{
		m_editBidding.SetSel(0, -1);
		m_editBidding.Copy();
		m_editBidding.SetSel(-1, -1);
	}
	else if (m_nCurrentChild == 1)
	{
		m_editPlay.SetSel(0, -1);
		m_editPlay.Copy();
		m_editPlay.SetSel(-1, -1);
	}
}



//
// OnHide() 
//
void CHistoryWnd::OnHide() 
{
	// simulate a button click
	if (m_nCurrentChild == 0)
	{
		((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(0);
		OnClickWindowShow();
	}
	else if (m_nCurrentChild == 1)
	{
		((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(0);
		OnClickWindowShow();
	}
}



//
// OnKeyDown()
//
void CHistoryWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//	CCJControlBar::OnKeyDown(nChar, nRepCnt, nFlags);
//	GetParent()->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
	if ((nChar == VK_INSERT) || (nChar == 'C'))
	{
		int nCode = GetAsyncKeyState(VK_CONTROL);
		if (GetAsyncKeyState(VK_CONTROL) & 0xFFFFFF00)
		{
//			m_pEdit->Copy();
			return;
		}
	}
	pMAINFRAME->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
}


//
void CHistoryWnd::OnMove(int x, int y) 
{
	CCJControlBar::OnMove(x, y);
	ResizeControls();

/*
	// update mainframe on our position 
	if (m_bInitialized && (x > 0) && (y > 0))
	{
		CRect rect;
		GetWindowRect(&rect);
		//
		const int nBarWidth = 3;
		if (m_dwStyle & CBRS_ORIENT_HORZ) 
			pMAINFRAME->SetValue(tnHistoryDialogWidth, rect.Width() + nBarWidth);
		else if (m_dwStyle & CBRS_ORIENT_VERT) 
			pMAINFRAME->SetValue(tnHistoryDialogHeight, rect.Height() + nBarWidth);
	}
*/
}



//
void CHistoryWnd::OnSize(UINT nType, int cx, int cy) 
{
//	CCJControlBar::OnSize(nType, cx, cy);
	CControlBar::OnSize(nType, cx, cy);

	//
	CRect rect;
	GetWindowRect(&rect);
	int nWidth = rect.Width();
	int nHeight = rect.Height();

	//	
	ResizeControls();

	// update mainframe on our position 
	if (m_bInitialized && (cx > 0) && (cy > 0))
	{
		const int nBarWidth = 4;
		const int nBarHeight = 6;
		if (m_dwStyle & CBRS_ORIENT_HORZ) 
//			pMAINFRAME->SetValue(tnHistoryDialogHeight, cy + nBarHeight);
			pMAINFRAME->SetValue(tnDockingWndHeight, cy + nBarHeight);
		else if (m_dwStyle & CBRS_ORIENT_VERT) 
//			pMAINFRAME->SetValue(tnHistoryDialogWidth, cx + nBarWidth);
			pMAINFRAME->SetValue(tnDockingWndWidth, cx + nBarWidth);
	}
}



//
void CHistoryWnd::ResizeControls() 
{
	CRect rectClient, rect;
	GetClientRect(&rectClient);

	// return if size is uninitialized
	if (rectClient.left == 0 && rectClient.top == 0 && rectClient.right == 0 && rectClient.bottom == 0)
		return;
	if ((rectClient.right <= rectClient.left) || (rectClient.bottom <= rectClient.top))
		return;

	//
	if (m_dwStyle & CBRS_FLOATING)
		rectClient.DeflateRect(trOffsetFloating.left, trOffsetFloating.top, trOffsetFloating.right, trOffsetFloating.bottom);
	else if (m_dwStyle & CBRS_ORIENT_HORZ) 
		rectClient.DeflateRect(trOffsetHorizontal.left, trOffsetHorizontal.top, trOffsetHorizontal.right, trOffsetHorizontal.bottom);
	else	// vertical
		rectClient.DeflateRect(trOffsetVertical.left, trOffsetVertical.top, trOffsetVertical.right, trOffsetVertical.bottom);

	// calc label window height
	CRect labelRect;
	m_lblBidding.GetWindowRect(&labelRect);

	// adjust rects
	rect = rectClient;
	int nLabelHeight = labelRect.Height();

	// set spacing between label & panel & between panels
	const int tnPanelSpacing = 6;
	const int tnTextSpacing = 4;

	//
	int numPanels = 0;
	if (m_bShowBidding)
		numPanels++;
	if (m_bShowPlay)
		numPanels++;

	// code depends on orientation
	if ((m_dwStyle & CBRS_ORIENT_VERT) || (m_dwStyle & CBRS_FLOATING))
	{
		// vertically docked or floating
		// set button height & spacing above or below button
		const int tnButtonHeight = 20;
		const int tnBelowSpacing = 4;

		//
		int nExtra = (nLabelHeight+tnTextSpacing)*numPanels + ((numPanels > 1)? tnPanelSpacing : 0) + tnButtonHeight + tnBelowSpacing;
		int nClientHeight = rectClient.Height() - nExtra;
		int nPanelHeight1 = 0, nPanelHeight2 = 0;
		if (m_bShowBidding)
			nPanelHeight1 = m_bShowPlay? (nClientHeight / 3) : nClientHeight;
		if (m_bShowPlay)
			nPanelHeight2 = m_bShowBidding? (nClientHeight - nPanelHeight1) : nClientHeight;

/*
		// show buttons at top
		int nBtnWidth = rect.Width() / 2;
		m_btnBidding.MoveWindow(rect.left, rect.top, nBtnWidth, tnButtonHeight);
		m_btnPlay.MoveWindow(rect.left+nBtnWidth, rect.top, nBtnWidth, tnButtonHeight);
		rect.top += tnButtonHeight + tnBelowSpacing;
*/

		// show bidding history
		if (m_bShowBidding)
		{
			// position bidding history label 
			m_lblBidding.MoveWindow(rect.left, rect.top, rect.Width(), nLabelHeight);
			m_lblBidding.ShowWindow(SW_SHOW);

			// size and position the bidding history panel
			rect.top += nLabelHeight + tnTextSpacing;
			rect.bottom = rect.top + nPanelHeight1;
			m_editBidding.MoveWindow(&rect);
			m_editBidding.ShowWindow(SW_SHOW);
			m_editBidding.UpdateWindow();

			// save metrics
			int nSize = rect.bottom - rect.top;
			m_numVisibleBiddingRows = nSize / m_nFontHeight;
		}

		// show play history
		if (m_bShowPlay)
		{
			// position play history label 
			if (m_bShowBidding)
				rect.top = rect.bottom + tnPanelSpacing;
			m_lblPlay.MoveWindow(rect.left, rect.top, rect.Width(), nLabelHeight);
			m_lblPlay.ShowWindow(SW_SHOW);

			// size and position the play history panel
			rect.top += nLabelHeight + tnTextSpacing;
			rect.bottom = rect.top + nPanelHeight2;
			m_editPlay.MoveWindow(&rect);
			m_editPlay.ShowWindow(SW_SHOW);
			m_editPlay.UpdateWindow();

			// save metrics
			m_numVisiblePlayRows = rect.Height() / m_nFontHeight;
		}

		// show buttons at bottom
		if (numPanels > 0)
			rect.top = rect.bottom + tnBelowSpacing;
		else
			rect.top = rect.bottom - tnButtonHeight;
		int nBtnWidth = rect.Width() / 2;
		m_btnBidding.MoveWindow(rect.left, rect.top, nBtnWidth, tnButtonHeight);
		m_btnPlay.MoveWindow(rect.left+nBtnWidth, rect.top, nBtnWidth, tnButtonHeight);
		rect.top += tnButtonHeight + tnBelowSpacing;
	}
	else
	{
		// horizontally docked
		// set button height & spacing beside button
		const int tnButtonWidth = 60;
		const int tnButtonHeight = 20;
		const int tnBesideSpacing = 4;

		//
		int nExtra = ((numPanels > 1)? tnPanelSpacing : 0) + tnButtonWidth + tnBesideSpacing;
		int nClientWidth = rectClient.Width() - nExtra;
		int nPanelWidth1 = 0, nPanelWidth2 = 0;
		if (m_bShowBidding)
			nPanelWidth1 = m_bShowPlay? (nClientWidth / 2) : nClientWidth;
		if (m_bShowPlay)
			nPanelWidth2 = m_bShowBidding? (nClientWidth - nPanelWidth1) : nClientWidth;
		//
		int nPanelHeight = rect.Height() - nLabelHeight - tnTextSpacing;

		// show buttons at left
		int nBtnHeight = tnButtonHeight;
		if (nPanelHeight < nBtnHeight*2)
			nBtnHeight = nPanelHeight / 2;
		//
		int nButtonTop = rect.top + nLabelHeight + tnTextSpacing;
		m_btnBidding.MoveWindow(rect.left, nButtonTop, tnButtonWidth, tnButtonHeight);
		m_btnPlay.MoveWindow(rect.left, nButtonTop+nBtnHeight, tnButtonWidth, tnButtonHeight);
		rect.left += tnButtonWidth + tnBesideSpacing;

		// adjust rect for labels
		int nLabelY = rect.top;
		rect.top = nButtonTop;	// align with buttons, below the labels + space

		// show bidding history
		if (m_bShowBidding)
		{
			// position bidding history label 
			m_lblBidding.MoveWindow(rect.left, nLabelY, nPanelWidth1, nLabelHeight);
			m_lblBidding.ShowWindow(SW_SHOW);

			// size and position the bidding history panel
			rect.right = rect.left + nPanelWidth1;
			m_editBidding.MoveWindow(&rect);
			m_editBidding.ShowWindow(SW_SHOW);
			m_editBidding.UpdateWindow();

			// save metrics
			m_numVisibleBiddingRows = rect.Height() / m_nFontHeight;
		}

		// show play history
		if (m_bShowPlay)
		{
			// position play history label 
			if (m_bShowBidding)
				rect.left = rect.right + tnPanelSpacing;
			m_lblPlay.MoveWindow(rect.left, nLabelY, nPanelWidth2, nLabelHeight);
			m_lblPlay.ShowWindow(SW_SHOW);

			// size and position the play history panel
			rect.right = rect.left + nPanelWidth2;
			m_editPlay.MoveWindow(&rect);
			m_editPlay.ShowWindow(SW_SHOW);
			m_editPlay.UpdateWindow();

			// save metrics
			m_numVisiblePlayRows = rect.Height() / m_nFontHeight;
		}
	}

	// this line seems necessary because of a bug in CJ60Lib
	m_lblPlay.Invalidate();
	m_lblPlay.UpdateWindow();
}




//
void CHistoryWnd::OnClickWindowShow() 
{
	// get checked status
	m_bShowBidding = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();
	m_bShowPlay = ((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck();

	// then enable each window
	m_editBidding.ShowWindow(m_bShowBidding? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_STATIC1)->ShowWindow(m_bShowBidding? SW_SHOW : SW_HIDE);
	//
	m_editPlay.ShowWindow(m_bShowPlay? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_STATIC2)->ShowWindow(m_bShowPlay? SW_SHOW : SW_HIDE);
	
	// and resize
	CRect rect;
	GetClientRect(&rect);
	OnSize(0, rect.Width(), rect.Height());
}



//
void CHistoryWnd::ShowBiddingHistory(BOOL bShow)
{
	// simulate a button click
	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(bShow);
	OnClickWindowShow();
}



//
void CHistoryWnd::ShowPlayHistory(BOOL bShow)
{
	((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(bShow);
	OnClickWindowShow();
}



//
BOOL CHistoryWnd::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// temp
	WinHelp(DIALOG_HISTORY);
	return TRUE;
}


// 
void CHistoryWnd::SetBiddingHistory(LPCTSTR szText) 
{
	BOOL bUseSuitSymbols = theApp.GetValue(tbUseSuitSymbols);
	if (bUseSuitSymbols)
		m_editBidding.LockWindowUpdate();
	//
	m_editBidding.SetWindowText(szText);
	//
	if (bUseSuitSymbols)
		ReplaceSuitSymbols(m_editBidding);
	//
	if (m_bShowBidding)
	{
		ScrollToEnd(m_editBidding, m_numVisibleBiddingRows);
		if (bUseSuitSymbols)
			m_editBidding.UnlockWindowUpdate();
		m_editBidding.UpdateWindow();
	}
}


// 
void CHistoryWnd::SetPlayHistory(LPCTSTR szText) 
{
	BOOL bUseSuitSymbols = theApp.GetValue(tbUseSuitSymbols);
	if (bUseSuitSymbols)
		m_editPlay.LockWindowUpdate();
	//
	m_editPlay.SetWindowText(szText);
	//
	if (bUseSuitSymbols)
		ReplaceSuitSymbols(m_editPlay);
	//
	if (m_bShowPlay)
	{
		ScrollToEnd(m_editPlay, m_numVisiblePlayRows);
		if (bUseSuitSymbols)
			m_editPlay.UnlockWindowUpdate();
		m_editPlay.UpdateWindow();
	}
}




//
void CHistoryWnd::ScrollToEnd(CRichEditCtrl& edit, int numVisibleRows)
{
	int numLines = edit.GetLineCount();
	int nFirstLine = edit.GetFirstVisibleLine();
	if (numLines > numVisibleRows) 
	{
		int nScrollCount = numLines - numVisibleRows - nFirstLine - 1;
//		int nScrollCount = numLines - numVisibleRows - nFirstLine;
		if (nScrollCount > 0)
			edit.LineScroll(nScrollCount);
	}
}



//
void CHistoryWnd::ReplaceSuitSymbols(CRichEditCtrl& edit)
{
	// scan the text
	CString strText;
	edit.GetWindowText(strText);
	int nLen = strText.GetLength();

	// prep format info
	CHARFORMAT format;
	format.cbSize = sizeof(format);
	format.dwMask = CFM_COLOR | CFM_FACE;
	strcpy(format.szFaceName, _T("Symbol"));
	format.bPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

	//
	for(int i=0;i<nLen;i++)
	{
		unsigned char letter = strText[i];
		if ((letter >= tSuitLetter) && (letter <= tSuitLetter+3))
		{
			// format the character
			edit.SetSel(i, i+1);
			format.crTextColor = (letter == (tSuitLetter+1) || letter == (tSuitLetter+2))? RGB(255, 0, 0) : RGB(0, 0, 0);
			edit.SetSelectionCharFormat(format);
		}
	}
	edit.SetSel(i,i);
}


