//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// editdlg.cpp : implementation file
//

#include "stdafx.h"
#include "EasyB.h"
#include "subclass.h"
#include "mainfrm.h"
#include "Editdlg.h"
#include "MainFrameOpts.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const int EDIT_MARGIN = 2;

/////////////////////////////////////////////////////////////////////////////
// CEditDialog dialog


CEditDialog::CEditDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CEditDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bShow = SW_HIDE;
}


void CEditDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditDialog, CDialog)
	//{{AFX_MSG_MAP(CEditDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditDialog message handlers

// 
BOOL CEditDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// subclass the edit control
	CWnd* pWnd = GetDlgItem(IDC_EDIT);
	ASSERT(pWnd != NULL);
	wpOrigEditProc = (WNDPROC) SetWindowLong(pWnd->m_hWnd, GWL_WNDPROC, (LONG) EditSubclassProc);
	// set the window title
//	SetWindowText((LPCTSTR)m_strTitle);
	// set the edit control's font
	CFont* pFont = (CFont*) pMAINFRAME->GetValuePV(tpFontFixed);
	SendDlgItemMessage(IDC_EDIT, 
					   WM_SETFONT, 
					   (LONG)pFont->m_hObject, 
					   FALSE);
	//
	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_EDIT);
	//pEdit->LimitText(30000);
	//
/*
	RECT rect;
	GetWindowRect(&rect);
	MoveWindow(m_windowPoint.x,m_windowPoint.y,
			   rect.right-rect.left,rect.bottom-rect.top);
*/
	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CEditDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
	GetParent()->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
}



//
void CEditDialog::SetWindowSize(int nRows, int nCols)
{
	CEdit *pEdit = (CEdit*) GetDlgItem(IDC_EDIT);
	if (pEdit == NULL)
		return;
	CDC* pDC = GetDC();

	// get width the simple way
	CString strFill('0',nCols);
//	CString strFill('0',nCols+1);	// need extra margin on win95
	CSize size = pDC->GetTextExtent(strFill);

	// but get height the more accurate way
	CFont* pFont = (CFont*) pMAINFRAME->GetValuePV(tpFontFixed);
	CFont* pOldFont = (CFont*) pDC->SelectObject(pFont);
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	pDC->SelectObject(pOldFont);
	int cx = size.cx;
	int cy = tm.tmHeight * nRows;
	ReleaseDC(pDC);

	// first get size of extra margins around formatting rectangle
	RECT winRect,newRect,formatRect;
	pEdit->GetRect(&formatRect);
	pEdit->GetClientRect(&winRect);

	// extra room around top & sides
	int nHorizMargin = 2;
	int nVertMargin = 3;

	// then size the edit box
	// first set the new formatting rect
	formatRect.left = 0;
	formatRect.top = 0;
	formatRect.right = formatRect.left + cx + 2;
	formatRect.bottom = formatRect.top + cy + 1;
	pEdit->SetRect(&formatRect);

	// and the edit window rect
	int nEditWidth = formatRect.right - formatRect.left;
	int nEditHeight = formatRect.bottom - formatRect.top;
	newRect.left = newRect.top = EDIT_MARGIN;
	newRect.right = newRect.left + nEditWidth + nHorizMargin*2;
	newRect.bottom = newRect.top + nEditHeight + nVertMargin*2;

	// size the edit control
	pEdit->SetWindowPos(&wndTop, 
						newRect.left, newRect.top,
						newRect.right - newRect.left,
						newRect.bottom - newRect.top,
					    SWP_NOZORDER);

	// and then the main window
	RECT rectWnd,rectClient;
	GetWindowRect(&rectWnd);
	GetClientRect(&rectClient);
	int nSideMargin = (rectWnd.right - rectWnd.left) - (rectClient.right - rectClient.left);
	int nTopMargin = (rectWnd.bottom - rectWnd.top) - (rectClient.bottom - rectClient.top);
	SetWindowPos(&wndTop, rectWnd.left, rectWnd.top, 
				  newRect.right - newRect.left + EDIT_MARGIN*2 + nSideMargin,
				  newRect.bottom - newRect.top + EDIT_MARGIN*2 + nTopMargin,
				  SWP_NOMOVE | SWP_NOZORDER);
	//
	m_nCols = nCols;
	m_nRows = nRows;
	return;
}




//
void CEditDialog::Clear()
{
	CEdit *pEdit = (CEdit*) GetDlgItem(IDC_EDIT);
	if (pEdit == NULL)
		return;
	pEdit->SetWindowText("");
}


// 
void CEditDialog::SetText(LPCTSTR szText) 
{
	CEdit *pEdit = (CEdit*) GetDlgItem(IDC_EDIT);
	if (pEdit == NULL)
		return;
	//
	pEdit->SetWindowText(szText);
	//
	ScrollToEnd();
	pEdit->UpdateWindow();
}


//
void CEditDialog::ScrollToTop()
{
	CEdit *pEdit = (CEdit*) GetDlgItem(IDC_EDIT);
	if (pEdit == NULL)
		return;
}


//
void CEditDialog::ScrollToEnd()
{
	CEdit *pEdit = (CEdit*) GetDlgItem(IDC_EDIT);
	if (pEdit == NULL)
		return;
	//
	RECT rect;
	pEdit->GetRect(&rect);
	int nSize = rect.bottom - rect.top;
	//
	CDC* pDC = GetDC();
	CFont* pFont = (CFont*) pMAINFRAME->GetValuePV(tpFontFixed);
	CFont* pOldFont = (CFont*) pDC->SelectObject(pFont);
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);
	//
	// scroll to bottom
//	int nHeight = tm.tmHeight - tm.tmInternalLeading;
//	int nVisibleRows = nSize / nHeight;
	int nVisibleRows = m_nRows;
	int numLines = pEdit->GetLineCount();
	int nFirstLine = pEdit->GetFirstVisibleLine();
	if (numLines > nVisibleRows) 
	{
//		int nScrollCount = numLines - nVisibleRows - nFirstLine - 1;
		int nScrollCount = numLines - nVisibleRows - nFirstLine;
		pEdit->LineScroll(nScrollCount);
	}
}


//
void CEditDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	m_bShow = bShow;
}


//
void CEditDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	//	
/*
	if (IsIconic()) 
	{
		// Erase the icon background when placed over other app window 
		DefWindowProc(WM_ICONERASEBKGND, (WORD)dc.m_hDC, 0L); 
		// Center the icon 
		CRect rc; 
		GetClientRect(&rc); 
		rc.left = (rc.right  - ::GetSystemMetrics(SM_CXICON)) >> 1; 
		rc.top  = (rc.bottom - ::GetSystemMetrics(SM_CYICON)) >> 1; 
		// Draw the icon 
		dc.DrawIcon(rc.left, rc.top, m_hIcon); 
	} 
	else 
	{
		RECT rect;
		GetClientRect(&rect);
		dc.Draw3dRect(&rect, RGB(255,255,255), RGB(128,128,128));
	}
*/
	GetParent()->SetFocus();
}



BOOL CEditDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch (wParam)
	{
		case WMS_SETTEXT:
			SetText((LPCTSTR) lParam);
			return TRUE;
	}
	//
	return CDialog::OnCommand(wParam, lParam);
}


//
void CEditDialog::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	delete this;
}
