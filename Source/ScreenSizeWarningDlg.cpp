//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// ScreenSizeWarningDlg.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "ScreenSizeWarningDlg.h"
#include "progopts.h"
#include "help\helpcode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScreenSizeWarningDlg dialog


CScreenSizeWarningDlg::CScreenSizeWarningDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScreenSizeWarningDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScreenSizeWarningDlg)
	m_bDontShowDialog = FALSE;
	//}}AFX_DATA_INIT
	m_bDontShowDialog = !theApp.GetValue(tbShowScreenSizeWarning);
}


void CScreenSizeWarningDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScreenSizeWarningDlg)
	DDX_Check(pDX, IDC_DONT_SHOW, m_bDontShowDialog);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScreenSizeWarningDlg, CDialog)
	//{{AFX_MSG_MAP(CScreenSizeWarningDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(ID_TELLMEMORE, OnTellMeMore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScreenSizeWarningDlg message handlers

void CScreenSizeWarningDlg::OnClose() 
{
	// see if we need to prevent thsi dialog from appearing in the future
	UpdateData(TRUE);
	theApp.SetValue(tbShowScreenSizeWarning, !m_bDontShowDialog);
	CDialog::OnClose();
}

void CScreenSizeWarningDlg::OnTellMeMore() 
{
	WinHelp(HIDT_FAQ_CARDS_CROWDING);
}

void CScreenSizeWarningDlg::OnOK() 
{
	UpdateData(TRUE);
	theApp.SetValue(tbShowScreenSizeWarning, !m_bDontShowDialog);
	EndDialog(0);
}
