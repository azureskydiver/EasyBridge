//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// PassedHandSimpleDialog.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "PassedHandSimpleDialog.h"
#include "progopts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPassedHandSimpleDialog dialog


CPassedHandSimpleDialog::CPassedHandSimpleDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPassedHandSimpleDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPassedHandSimpleDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPassedHandSimpleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPassedHandSimpleDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPassedHandSimpleDialog, CDialog)
	//{{AFX_MSG_MAP(CPassedHandSimpleDialog)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPassedHandSimpleDialog message handlers

BOOL CPassedHandSimpleDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//
	int nInterval = theApp.GetValue(tnPassedHandWaitInterval);
	SetTimer(0, nInterval, NULL);
	CenterWindow();

	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
void CPassedHandSimpleDialog::OnTimer(UINT nIDEvent) 
{
//	CDialog::OnTimer(nIDEvent);
	EndDialog(0);
}

//
void CPassedHandSimpleDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
//	CDialog::OnLButtonDown(nFlags, point);
	EndDialog(0);
}

//
void CPassedHandSimpleDialog::OnRButtonDown(UINT nFlags, CPoint point) 
{
//	CDialog::OnRButtonDown(nFlags, point);
	EndDialog(0);
}

//
void CPassedHandSimpleDialog::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//	CDialog::OnChar(nChar, nRepCnt, nFlags);
	EndDialog(0);
}

