//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// PassedHandDialog.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "PassedHandDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPassedHandDialog dialog


CPassedHandDialog::CPassedHandDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPassedHandDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPassedHandDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPassedHandDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPassedHandDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPassedHandDialog, CDialog)
	//{{AFX_MSG_MAP(CPassedHandDialog)
	ON_BN_CLICKED(ID_REBID_HAND, OnRebidHand)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPassedHandDialog message handlers

void CPassedHandDialog::OnOK() 
{
	EndDialog(FALSE);
}

void CPassedHandDialog::OnRebidHand() 
{
	EndDialog(TRUE);
}
