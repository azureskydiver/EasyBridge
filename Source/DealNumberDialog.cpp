//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// DealNumberDialog.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "easybDoc.h"
#include "DealNumberDialog.h"
#include "help\helpcode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDealNumberDialog dialog


CDealNumberDialog::CDealNumberDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDealNumberDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDealNumberDialog)
	m_strDealNumber = _T("");
	//}}AFX_DATA_INIT
}


void CDealNumberDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDealNumberDialog)
	DDX_Text(pDX, IDC_DEAL_NUMBER, m_strDealNumber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDealNumberDialog, CDialog)
	//{{AFX_MSG_MAP(CDealNumberDialog)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDealNumberDialog message handlers

void CDealNumberDialog::OnOk() 
{
	UpdateData(TRUE);

	//
	if (m_strDealNumber.GetLength() < 8)
	{
		AfxMessageBox("The deal string is incomplete");
		return;
	}

	// parse the deal number
	m_nSpecialDealCode = 0;
	int nCode = 0;
	int numParams = _stscanf((LPCTSTR)m_strDealNumber,_T("%8lX%1X%2X"), &m_nDealNumber, &nCode, &m_nSpecialDealCode);

	// parse the dealer & vulnerability bits
	if (numParams >= 2)
	{
		// extract settings
		m_nDealer = nCode & 0x03;
		m_nVulnerability = (nCode & 0x0C) >> 2;
	}
	else
	{
		// set defaults
		m_nDealer = SOUTH;
		m_nVulnerability = NEITHER;
	}

	//
	EndDialog(TRUE);
}

void CDealNumberDialog::OnCancel() 
{
	EndDialog(FALSE);
}


//
BOOL CDealNumberDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	WinHelp(DIALOG_DEAL_NUMBER);
	return TRUE;
}
