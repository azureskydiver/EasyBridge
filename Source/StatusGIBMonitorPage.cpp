//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// StatusGIBMonitorPage.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "mainfrm.h"
#include "mainframeopts.h"
#include "progopts.h"
#include "StatusGIBMonitorPage.h"
#include "subclass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusGIBMonitorPage property page

IMPLEMENT_DYNCREATE(CStatusGIBMonitorPage, CPropertyPage)

CStatusGIBMonitorPage::CStatusGIBMonitorPage() : CPropertyPage(CStatusGIBMonitorPage::IDD)
{
	//{{AFX_DATA_INIT(CStatusGIBMonitorPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bInitialized = FALSE;
}

CStatusGIBMonitorPage::~CStatusGIBMonitorPage()
{
}

void CStatusGIBMonitorPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatusGIBMonitorPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatusGIBMonitorPage, CPropertyPage)
	//{{AFX_MSG_MAP(CStatusGIBMonitorPage)
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusGIBMonitorPage message handlers



//
BOOL CStatusGIBMonitorPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// create the RTF edit window
	CRect rect;
	GetClientRect(&rect);
	if (!m_edit.Create(ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL,
						 rect, this, IDC_EDIT))
		return -1;	// oops!
	m_edit.SendMessage(WM_SETFONT, (UINT)GetStockObject(ANSI_VAR_FONT), 0L);
	m_edit.HideSelection(TRUE, TRUE);
	// and subclass it
	wpOrigRichEditProc = (WNDPROC) SetWindowLong(m_edit.GetSafeHwnd(), GWL_WNDPROC, (LONG) RichEditSubclassProc);

	// set flags
	m_bInitialized = TRUE;
	Update();

	// done	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



//
BOOL CStatusGIBMonitorPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch(wParam) 
	{
		case WMS_SET_GIB_TEXT:
			// do nuttin' if not initialized
			if (!m_bInitialized)
				return TRUE;
			m_edit.SetWindowText((LPCTSTR)lParam);
			m_edit.UpdateWindow();
			return TRUE;
	}
	//			
	return CPropertyPage::OnCommand(wParam, lParam);
}


//
void CStatusGIBMonitorPage::Update()
{
	if (!m_bInitialized)
		return;
	if (theApp.GetValue(tbEnableGIBForDeclarer) || theApp.GetValue(tbEnableGIBForDefender))
		m_edit.SetWindowText(pMAINFRAME->GetValueString(tszGIBMonitorText));	
	else
		m_edit.SetWindowText("GIB is not enabled.");	
	UpdateData(FALSE);
	//
	ScrollToEnd();
}


//
void CStatusGIBMonitorPage::ScrollToEnd()
{
	RECT rect;
	m_edit.GetRect(&rect);
	int nSize = rect.bottom - rect.top;
	//
	CWindowDC editDC(&m_edit);
	TEXTMETRIC tm;
	editDC.GetTextMetrics(&tm);

	// scroll to bottom
//	int nHeight = tm.tmHeight;
	int nHeight = tm.tmAscent;
	int nVisibleRows = nSize / nHeight;
	int numLines = m_edit.GetLineCount();
	int nFirstLine = m_edit.GetFirstVisibleLine();
	if (numLines > nVisibleRows) 
	{
		int nScrollCount = numLines - nVisibleRows - nFirstLine;
		m_edit.LineScroll(nScrollCount + 1);
	}
	// and set selection at the end of the text
	int nTextLength = m_edit.GetTextLength();
	m_edit.SetSel(nTextLength,nTextLength);
	m_edit.UpdateWindow();
}


//
void CStatusGIBMonitorPage::Clear()
{
	if (!m_bInitialized)
		return;
//	m_edit.SetWindowText(NULL);
	m_edit.SetWindowText("GIB is not enabled.");	
	m_edit.UpdateWindow();
}


//
void CStatusGIBMonitorPage::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if (!m_bInitialized)
		return;
	// resize the RTF edit control
	CRect textRect;
	GetClientRect(&textRect);
	m_edit.SetWindowPos(&wndTop, textRect.left, textRect.top,
					     textRect.Width(), textRect.Height(),SWP_NOZORDER);
	m_edit.ShowWindow(SW_SHOW);
}


//
// OnKeyDown()
//
// pass along to the parent
//
void CStatusGIBMonitorPage::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//	CPropertyPage::OnKeyDown(nChar, nRepCnt, nFlags);
//	GetParent()->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
	pMAINFRAME->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
}


//
void CStatusGIBMonitorPage::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// we don't do anything special here, so send it up to the parent
	ClientToScreen(&point);
	CWnd* pParent = GetParent();
	pParent->ScreenToClient(&point);
	pParent->SendMessage(WM_RBUTTONDOWN, nFlags, MAKELONG(point.x, point.y));
}
