//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// RoundFinishedDialog.cpp : implementation file
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "EasyBVw.h"
#include "viewopts.h"
#include "deck.h"
#include "RoundFinishedDialog.h"
#include "Help/HelpCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
CString CRoundFinishedDialog::m_strOldText = _T("");


/////////////////////////////////////////////////////////////////////////////
// CRoundFinishedDialog dialog


CRoundFinishedDialog::CRoundFinishedDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CRoundFinishedDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRoundFinishedDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nCode = 0;
	m_bReplayMode = FALSE;
	m_bDisableCancel = FALSE;
	m_bCollapsed = FALSE;
	m_nFullHeight = -1;
	m_nCollapsedHeight = -1;
}


void CRoundFinishedDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRoundFinishedDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRoundFinishedDialog, CDialog)
	//{{AFX_MSG_MAP(CRoundFinishedDialog)
	ON_BN_CLICKED(ID_REPLAY_HAND, OnReplayHand)
	ON_BN_CLICKED(ID_REBID_HAND, OnRebidHand)
	ON_BN_CLICKED(ID_SAVE_GAME_RECORD, OnSaveGameRecord)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(ID_MORE, OnMore)
	ON_BN_CLICKED(IDC_COMPUTER_REPLAY, OnComputerReplay)
	ON_BN_CLICKED(ID_CANCEL, OnCancel)
	ON_BN_CLICKED(IDC_COMPUTER_REPLAY_AUTO, OnComputerReplayAuto)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoundFinishedDialog message handlers


BOOL CRoundFinishedDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//
	SetDlgItemText(IDC_MESSAGE, m_strText);
	if (m_bReplayMode)
		SetDlgItemText(IDC_MESSAGE2, m_strOldText);
	else
		SetDlgItemText(IDC_MESSAGE2, _T(""));

	// center the window, taking the docking dialogs into account
//	CenterWindow();
	CRect rect,parentRect;
	GetWindowRect(&rect);
	CEasyBView* pView = pVIEW;
	pView->GetWindowRect(&parentRect);

	// position dialog just above south's cards, centered horizontally
	int nYPos, nXPos = parentRect.left + (parentRect.Width() - rect.Width()) / 2;
	POINT ptNorth = { 0, 0 }, ptSouth = { 0, 0 };
	ptNorth.y = pVIEW->GetValue(tpDrawPoint, NORTH, 1) + deck.GetCardHeight();
	ptSouth.y = pVIEW->GetValue(tpDrawPoint, SOUTH, 1);
	pView->ClientToScreen(&ptNorth);
	pView->ClientToScreen(&ptSouth);

	//
	int nVertGap = ptSouth.y - ptNorth.y;
	if (nVertGap >= rect.Height())
		nYPos = ptSouth.y - (nVertGap - rect.Height())/2;
	else
		nYPos = ptSouth.y;
	//
	if ((rect.bottom != nYPos) || (rect.left != nXPos))
	{
		int dy = rect.bottom - nYPos;
		MoveWindow(nXPos, rect.top-dy, rect.Width(), rect.Height(), TRUE);
	}

	// disable controls if appropriate
	if (m_bDisableCancel)
		GetDlgItem(ID_CANCEL)->EnableWindow(FALSE);
	if (m_bReplayMode)
	{
		GetDlgItem(IDC_COMPUTER_REPLAY)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMPUTER_REPLAY_AUTO)->EnableWindow(FALSE);
	}

	// record measurements
	CRect winRect, lineRect;
	GetWindowRect(&winRect);
	GetDlgItem(IDC_SEPARATOR)->GetWindowRect(&lineRect);
//	ScreenToClient(&lineRect);
	m_nFullHeight = winRect.Height();
	m_nCollapsedHeight = lineRect.top - winRect.top;

	// collapse if desired
	if (m_bCollapsed)
		MoveWindow(winRect.left, winRect.top, winRect.Width(), m_nCollapsedHeight);

	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CRoundFinishedDialog::OnOK() 
{
	EndDialog(TRUE);
//	CDialog::OnOK();
}

void CRoundFinishedDialog::OnCancel() 
{
	m_nCode = RF_NONE;
	EndDialog(FALSE);	
}

void CRoundFinishedDialog::OnReplayHand() 
{
	m_nCode = RF_REPLAY;
	EndDialog(FALSE);
}

void CRoundFinishedDialog::OnRebidHand() 
{
	m_nCode = RF_REBID;
	EndDialog(FALSE);
}

//
BOOL CRoundFinishedDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	WinHelp(DIALOG_HAND_FINISHED);
	return TRUE;
}


//
void CRoundFinishedDialog::OnMore() 
{
	CRect winRect;
	GetWindowRect(&winRect);
	//
	m_bCollapsed = !m_bCollapsed;
	if (m_bCollapsed)
	{
		MoveWindow(winRect.left, winRect.top, winRect.Width(), m_nCollapsedHeight);
		GetDlgItem(ID_MORE)->SetWindowText(_T("More >>"));
	}
	else
	{
		MoveWindow(winRect.left, winRect.top, winRect.Width(), m_nFullHeight);
		GetDlgItem(ID_MORE)->SetWindowText(_T("More <<"));
	}
}


//
void CRoundFinishedDialog::OnSaveGameRecord() 
{
	pDOC->SaveFileAs();
}

//
void CRoundFinishedDialog::OnComputerReplay() 
{
	m_nCode = RF_AUTOPLAY;
	EndDialog(FALSE);
}

//
void CRoundFinishedDialog::OnComputerReplayAuto() 
{
	m_nCode = RF_AUTOPLAY_FULL;
	EndDialog(FALSE);
}
