//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// AutoHintDialog.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "easybDoc.h"
#include "Mainfrm.h"
#include "MainFrameOpts.h"
#include "AutoHintDialog.h"
#include "subclass.h"
#include "PlayerStatusDialog.h"
#include "progopts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
const COLORREF tclrWindow = RGB(230, 245, 255);
const COLORREF tclrBlack = RGB(0, 0, 255);

//


/////////////////////////////////////////////////////////////////////////////
// CAutoHintDialog dialog


CAutoHintDialog::CAutoHintDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoHintDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoHintDialog)
	m_nHintLevel = -1;
	//}}AFX_DATA_INIT
	//
	m_strText = _T("");
	m_bInitialized = FALSE;
}


void CAutoHintDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoHintDialog)
	DDX_Control(pDX, IDC_COMBO_LEVEL, m_cbLevel);
	DDX_CBIndex(pDX, IDC_COMBO_LEVEL, m_nHintLevel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoHintDialog, CDialog)
//	ON_WM_PAINT()
	//{{AFX_MSG_MAP(CAutoHintDialog)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_MOVE()
	ON_WM_KEYDOWN()
	ON_CBN_SELENDOK(IDC_COMBO_LEVEL, OnChangeHintLevel)
	ON_BN_CLICKED(IDC_ACCEPT, OnAcceptHint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAutoHintDialog message handlers


//
// OnInitDialog() 
//
BOOL CAutoHintDialog::OnInitDialog() 
{
	m_nHintLevel = CPlayerStatusDialog::GetHintTraceLevel() - 1;

	//
	CDialog::OnInitDialog();
	//
	m_flatButtons[0].SubclassDlgItem(IDC_ACCEPT, this);
	m_flatButtons[1].SubclassDlgItem(IDCANCEL, this);

	// subclass listbox
	CListBox* pList = (CListBox*) GetDlgItem(IDC_COMBO_LEVEL);
	wpOrigListBoxProc = (WNDPROC) SetWindowLong(pList->GetSafeHwnd(), GWL_WNDPROC, (LONG) ListBoxSubclassProc);

	// init the rich edit control
	m_edit.Create(ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL, CRect(0,0,0,0), this, IDC_EDIT);
	m_edit.HideSelection(TRUE, TRUE);
	wpOrigRichEditProc = (WNDPROC) SetWindowLong(m_edit.GetSafeHwnd(), GWL_WNDPROC, (LONG) RichEditSubclassProc);

	// set its font
	m_pFont = (CFont*) pMAINFRAME->GetValuePV(tpFontAutoHint);
	m_edit.SendMessage(WM_SETFONT, (LONG)m_pFont->m_hObject, FALSE);

	// and size window
	m_bInitialized = TRUE;
	SetWindowPos(&wndTop,
				 m_rect.left,
				 m_rect.top,
				 m_rect.Width(),
				 m_rect.Height(),
				 SWP_NOZORDER);

	// get metrics
	CWindowDC dc(&m_edit);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	int nFontHeight = tm.tmHeight;
	CRect rect;
	m_edit.GetWindowRect(&rect);
	m_numVisibleRows = rect.Height() / nFontHeight;

	// set the trace level
	m_nHintLevel = theApp.GetValue(tnAutoHintTraceLevel) - 1;
	UpdateData(FALSE);

	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CAutoHintDialog::OnAcceptHint() 
{
	pDOC->GetGameHint();
}


//
void CAutoHintDialog::OnClose() 
{
	// unless the app is shutting down, we simply close; don't destroy
	if (pMAINFRAME->IsClosing())
		CDialog::OnClose();
	else
		pMAINFRAME->InformChildFrameClosed(twAutoHintDialog);
//		ShowWindow(SW_HIDE);
}


//
void CAutoHintDialog::OnOK() 
{
	ShowWindow(SW_HIDE);
//	CDialog::OnOK();
}


//
void CAutoHintDialog::OnCancel() 
{
	// use the messaging mechanism
	pMAINFRAME->PostMessage(ID_GAME_AUTOHINT);
//	ShowWindow(SW_HIDE);
//	CDialog::OnCancel();
}


//
void CAutoHintDialog::SetHintText(LPCTSTR szText) 
{
	m_edit.SetWindowText(szText);
	m_edit.UpdateWindow();
	m_strText = szText;

	// scroll to the end
	int numLines = m_edit.GetLineCount();
	int nFirstLine = m_edit.GetFirstVisibleLine();
	//
	if (numLines > m_numVisibleRows) 
	{
		int nScrollCount = numLines - m_numVisibleRows - nFirstLine - 1;
		if (nScrollCount > 0)
			m_edit.LineScroll(nScrollCount);
	}
}


//
void CAutoHintDialog::Clear() 
{
	if (!m_bInitialized)
		return;
	//
	m_edit.SetWindowText(_T(""));
	m_strText = _T("");
//	m_nSuspendOutput = 0;
//	Invalidate();
}


//
void CAutoHintDialog::EnableHintAccept(BOOL bEnable)
{
	if (!m_bInitialized)
		return;
	//
	GetDlgItem(IDC_ACCEPT)->EnableWindow(bEnable);
}


//
void CAutoHintDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	ResizeWindows();
	//
	RECT rect;
	GetWindowRect(&rect);
	pMAINFRAME->SetValue(tnAutoHintDialogRectLeft, rect.left);
	pMAINFRAME->SetValue(tnAutoHintDialogRectTop, rect.top);
	pMAINFRAME->SetValue(tnAutoHintDialogRectRight, rect.right);
	pMAINFRAME->SetValue(tnAutoHintDialogRectBottom, rect.bottom);
}


//
void CAutoHintDialog::ResizeWindows() 
{
	if (!m_bInitialized)
		return;

	// move the buttons
	RECT acceptBtnRect, hideBtnRect, cbRect;
	CRect winRect;
	GetClientRect(&winRect);
	const int nComboWidth = 48;
	int nSizeAvailable = winRect.Width() - nComboWidth;
	int nAcceptBtnWidth = (int)(((double)nSizeAvailable / 3) * 2);
	int nHideBtnWidth = nSizeAvailable - nAcceptBtnWidth;
	CButton* pAcceptBtn = (CButton*) GetDlgItem(IDC_ACCEPT);
	CButton* pHideBtn = (CButton*) GetDlgItem(IDCANCEL);
	//
	acceptBtnRect.left = winRect.left;
	acceptBtnRect.top = winRect.bottom - 20;
	acceptBtnRect.bottom = winRect.bottom;
	acceptBtnRect.right = nAcceptBtnWidth;
	pAcceptBtn->MoveWindow(&acceptBtnRect);
	//
	hideBtnRect.left = acceptBtnRect.right;
	hideBtnRect.top = acceptBtnRect.top;
	hideBtnRect.bottom = acceptBtnRect.bottom;
	hideBtnRect.right = hideBtnRect.left + nHideBtnWidth;
	pHideBtn->MoveWindow(&hideBtnRect);
	//
	cbRect.left = hideBtnRect.right;
	cbRect.top = hideBtnRect.top;
	cbRect.bottom = hideBtnRect.bottom;
	cbRect.right = cbRect.left + nComboWidth;
	m_cbLevel.MoveWindow(&cbRect);


	// then size the edit control
	CRichEditCtrl* pEdit = (CRichEditCtrl*) GetDlgItem(IDC_EDIT);
	if (pEdit == NULL)
		return;
	RECT textRect;
	textRect.left = winRect.left;	
	textRect.top = winRect.top;
	textRect.right = winRect.right;	
	textRect.bottom = acceptBtnRect.top - 2;	
	m_edit.SetWindowPos(&wndTop, textRect.left, textRect.top,
					    textRect.right - textRect.left,
					    textRect.bottom - textRect.top,
						SWP_NOZORDER);
}



//
void CAutoHintDialog::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	//
	RECT rect;
	GetWindowRect(&rect);
	pMAINFRAME->SetValue(tnAutoHintDialogRectLeft, rect.left);
	pMAINFRAME->SetValue(tnAutoHintDialogRectTop, rect.top);
	pMAINFRAME->SetValue(tnAutoHintDialogRectRight, rect.right);
	pMAINFRAME->SetValue(tnAutoHintDialogRectBottom, rect.bottom);
}


//
void CAutoHintDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	pMAINFRAME->PostMessage(WM_KEYDOWN, nChar, MAKELONG(nRepCnt, nFlags));
//	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CAutoHintDialog::OnChangeHintLevel() 
{
	UpdateData(TRUE);	
	theApp.SetValue(tnAutoHintTraceLevel, m_nHintLevel+1);
	CPlayerStatusDialog::SetHintTraceLevel(m_nHintLevel + 1);
}
