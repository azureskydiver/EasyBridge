//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// SelectHandDialog.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "easybDoc.h"
#include "SelectHandDialog.h"
#include "Help\helpcode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectHandDialog dialog


CSelectHandDialog::CSelectHandDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectHandDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectHandDialog)
	m_nPosition = -1;
	m_nVulnerability = -1;
	//}}AFX_DATA_INIT
	//
	m_nMode = SH_MODE_DEALER;
	m_nVulnerability = 0;
	m_bCollapsed = TRUE;
}


void CSelectHandDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectHandDialog)
	DDX_Radio(pDX, IDC_RADIO_SOUTH, m_nPosition);
	DDX_Radio(pDX, IDC_VULNERABILITY0, m_nVulnerability);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectHandDialog, CDialog)
	//{{AFX_MSG_MAP(CSelectHandDialog)
	ON_BN_CLICKED(IDC_RADIO_SOUTH, OnSelectPosition)
	ON_BN_CLICKED(IDC_RADIO_WEST, OnSelectPosition)
	ON_BN_CLICKED(IDC_RADIO_NORTH, OnSelectPosition)
	ON_BN_CLICKED(IDC_RADIO_EAST, OnSelectPosition)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectHandDialog message handlers



BOOL CSelectHandDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText(m_strTitle);
	if (m_nMode == SH_MODE_DEALER)
		GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);

	// get the full height
	CRect clientRect, windowRect;
	GetClientRect(&clientRect);
	GetWindowRect(&windowRect);
	m_nFullHeight = windowRect.Height();

	// and the collapsed height
	CRect btnRect;
	GetDlgItem(IDC_RADIO_NORTH)->GetWindowRect(&btnRect);
	ScreenToClient(&btnRect);
	int nSpace = btnRect.top - clientRect.top;
	GetDlgItem(IDC_RADIO_SOUTH)->GetWindowRect(&btnRect);
	m_nCollapsedHeight = btnRect.bottom + nSpace - windowRect.top + 4;

	//
	CollapseWindow(m_bCollapsed);

	//	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CSelectHandDialog::CollapseWindow(BOOL bCollapse) 
{
	CRect rect;
	GetWindowRect(&rect);
	//
	if (bCollapse)
	{
		// collapsing window
		rect.bottom = rect.top + m_nCollapsedHeight; 
	}
	else
	{
		// expanding window
		rect.bottom = rect.top + m_nFullHeight;
	}
	//
	MoveWindow(&rect, TRUE);
	m_bCollapsed = bCollapse;
}



//
void CSelectHandDialog::OnSelectPosition() 
{
	UpdateData(TRUE);
}


//
void CSelectHandDialog::OnOK() 
{
	UpdateData(TRUE);
	EndDialog(TRUE);
}


//
void CSelectHandDialog::OnCancel() 
{
	EndDialog(FALSE);
}


//
BOOL CSelectHandDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (m_nMode == SH_MODE_HAND)
		WinHelp(DIALOG_SELECT_HAND);
	else
		WinHelp(DIALOG_SELECT_DEALER);
	return TRUE;	
//	return CDialog::OnHelpInfo(pHelpInfo);
}
