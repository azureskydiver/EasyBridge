//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// filecmts.cpp : implementation file
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "docopts.h"
#include "mainfrm.h"
#include "MainFrameOpts.h"
#include "FileComments.h"
#include "Help\HelpCode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileCommentsDialog dialog


CFileCommentsDialog::CFileCommentsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFileCommentsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileCommentsDialog)
	m_bAutoShow = FALSE;
	//}}AFX_DATA_INIT
}


void CFileCommentsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileCommentsDialog)
	DDX_Check(pDX, IDC_AUTO_SHOW, m_bAutoShow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileCommentsDialog, CDialog)
	//{{AFX_MSG_MAP(CFileCommentsDialog)
	ON_BN_CLICKED(IDC_AUTO_SHOW, OnAutoShow)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_MOVE()
	ON_WM_HELPINFO()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFileCommentsDialog message handlers

void CFileCommentsDialog::OnAutoShow() 
{
	UpdateData(TRUE);
	pDOC->SetValue(tbShowCommentsUponOpen, m_bAutoShow);
}

BOOL CFileCommentsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//	
	// set the edit control's font
	SendDlgItemMessage(IDC_EDIT,
		 			   WM_SETFONT, 
					   (LONG)pMAINFRAME->m_standardFont.m_hObject,
					   FALSE);
	// and set window info
	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_EDIT);
	pEdit->LimitText(30000);
	// and size window
	SetWindowPos(&wndTop,m_rect.left,m_rect.top,
				 m_rect.right-m_rect.left,m_rect.bottom-m_rect.top,
				 SWP_NOZORDER);
	SizeTextBox();
	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



//
BOOL CFileCommentsDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch (wParam)
	{
		case WMS_UPDATE_TEXT:
			UpdateText((BOOL)lParam);
			return TRUE;
	}
	return CDialog::OnCommand(wParam, lParam);
}



//
void CFileCommentsDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	SizeTextBox();
}



//
void CFileCommentsDialog::SizeTextBox() 
{
/*
	CEdit *pEdit = (CEdit*) GetDlgItem(IDC_EDIT);
	if (pEdit == NULL)
		return;
	RECT rect,rect2,textRect;
	GetClientRect(&rect);
	GetDlgItem(IDC_CLOSE)->GetWindowRect(&rect2);
	ScreenToClient(&rect2);
	textRect.left = rect.left + 2;	
	textRect.top = rect2.bottom + 4;
	textRect.right = rect.right - 2;	
	textRect.bottom = rect.bottom - 2;	
	pEdit->SetWindowPos(&wndTop, textRect.left, textRect.top,
					    textRect.right - textRect.left,
					    textRect.bottom - textRect.top,
						SWP_NOZORDER);
*/
	CEdit *pEdit = (CEdit*) GetDlgItem(IDC_EDIT);
	if (pEdit == NULL)
		return;
	RECT rect,rect2,textRect;
	GetClientRect(&rect);
	GetDlgItem(IDC_AUTO_SHOW)->GetWindowRect(&rect2);
	ScreenToClient(&rect2);
	textRect.left = rect.left + 2;	
	textRect.top = rect2.bottom + 4;
	textRect.right = rect.right - 2;	
	textRect.bottom = rect.bottom - 2;	
	pEdit->SetWindowPos(&wndTop, textRect.left, textRect.top,
					    textRect.right - textRect.left,
					    textRect.bottom - textRect.top,
						SWP_NOZORDER);
}



//
void CFileCommentsDialog::UpdateText(BOOL bUpdateVariable) 
{
	CEdit *pEdit = (CEdit*) GetDlgItem(IDC_EDIT);
	if (pEdit == NULL)
		return;
	if (bUpdateVariable) 
	{
		// update main document string
		CString strText;
		pEdit->GetWindowText(strText);
		pDOC->SetValue(tstrFileComments,(LPCTSTR)strText);
		return;
	} 
	// else update window text display
	pEdit->SetWindowText(pDOC->GetValueString(tstrFileComments));
	//
	RECT rect;
	pEdit->GetClientRect(&rect);
	int nSize = rect.bottom - rect.top;
	//
	CDC* pDC = pEdit->GetDC();
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	ReleaseDC(pDC);
	// scroll to bottom
	int nHeight = tm.tmHeight - tm.tmInternalLeading;
	int nVisibleRows = (nSize-3) / nHeight;
	int numLines = pEdit->GetLineCount();
	int nFirstLine = pEdit->GetFirstVisibleLine();
	if (numLines > nVisibleRows) 
	{
		int nScrollCount = numLines - nVisibleRows - 1;
		pEdit->LineScroll(nScrollCount);
	}
	//
	m_bAutoShow = pDOC->GetValue(tbShowCommentsUponOpen);
	UpdateData(FALSE);
}


//
void CFileCommentsDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	// auto update text
	if (bShow)
		UpdateText(FALSE);

	// and show
	CDialog::OnShowWindow(bShow, nStatus);
}

//
void CFileCommentsDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	//	
//	GetParent()->SetFocus();
}


//
void CFileCommentsDialog::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	//
	RECT rect;
	GetWindowRect(&rect);
	pMAINFRAME->SetValue(tnCommentsDialogRectLeft, rect.left);
	pMAINFRAME->SetValue(tnCommentsDialogRectTop, rect.top);
	pMAINFRAME->SetValue(tnCommentsDialogRectRight, rect.right);
	pMAINFRAME->SetValue(tnCommentsDialogRectBottom, rect.bottom);
}


//
BOOL CFileCommentsDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	WinHelp(DIALOG_FILE_COMMENTS);
	return TRUE;
}
