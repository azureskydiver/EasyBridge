//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// BiddingFinishedDialog.cpp : implementation file
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "BiddingFinishedDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBiddingFinishedDialog dialog


CBiddingFinishedDialog::CBiddingFinishedDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CBiddingFinishedDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBiddingFinishedDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBiddingFinishedDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBiddingFinishedDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBiddingFinishedDialog, CDialog)
	//{{AFX_MSG_MAP(CBiddingFinishedDialog)
	ON_BN_CLICKED(ID_REBID_HAND, OnRebidHand)
	ON_BN_CLICKED(ID_REDEAL, OnRedeal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBiddingFinishedDialog message handlers


//
BOOL CBiddingFinishedDialog::OnInitDialog() 
{
	ASSERT(pDOC->GetNumBidsMade() >= 3);
	//
 	CDialog::OnInitDialog();
	//
	SetDlgItemText(IDC_MESSAGE, (LPCTSTR) m_strText);
	CenterWindow();
	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CBiddingFinishedDialog::OnOK() 
{
	EndDialog(TRUE);	
//	CDialog::OnOK();
}

void CBiddingFinishedDialog::OnRebidHand() 
{
	m_nResultCode = 0;
	EndDialog(FALSE);	
}


void CBiddingFinishedDialog::OnRedeal() 
{
	m_nResultCode = 1;
	EndDialog(FALSE);	
}
