//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// AutotrainStatusDlg.cpp : implementation file
//
#include "stdafx.h"
#include "easyb.h"
#include "NNetAutotrainStatusDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNNetAutotrainStatusDlg  dialog


CNNetAutotrainStatusDlg ::CNNetAutotrainStatusDlg (CWnd* pParent /*=NULL*/)
	: CDialog(CNNetAutotrainStatusDlg ::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNNetAutotrainStatusDlg )
	m_strNumHands = _T("");
	m_strNumBids = _T("");
	m_strNumCorrections = _T("");
	m_strCorrectionRate = _T("");
	m_strNumNPBids = _T("");
	m_strNumNPCorrections = _T("");
	m_strNPCorrectionRate = _T("");
	m_strErrorValue = _T("");
	m_strNumTotalCycles = _T("");
	//}}AFX_DATA_INIT

	//
	m_bCancel = FALSE;
}


void CNNetAutotrainStatusDlg ::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNNetAutotrainStatusDlg )
	DDX_Text(pDX, IDC_LBL_HANDS, m_strNumHands);
	DDX_Text(pDX, IDC_LBL_BIDS, m_strNumBids);
	DDX_Text(pDX, IDC_LBL_CORRECTIONS, m_strNumCorrections);
	DDX_Text(pDX, IDC_LBL_CORRECTION_RATE, m_strCorrectionRate);
	DDX_Text(pDX, IDC_LBL_NP_BIDS, m_strNumNPBids);
	DDX_Text(pDX, IDC_LBL_NP_CORRECTIONS, m_strNumNPCorrections);
	DDX_Text(pDX, IDC_LBL_NP_CORRECTION_RATE, m_strNPCorrectionRate);
	DDX_Text(pDX, IDC_LBL_ERROR, m_strErrorValue);
	DDX_Text(pDX, IDC_LBL_CYCLES, m_strNumTotalCycles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNNetAutotrainStatusDlg , CDialog)
	//{{AFX_MSG_MAP(CNNetAutotrainStatusDlg )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNNetAutotrainStatusDlg  message handlers


//
BOOL CNNetAutotrainStatusDlg ::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// 
	CenterWindow();
	
	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
BOOL CNNetAutotrainStatusDlg ::Update(int numHands, int numBids, int numCorrections, int numNonPassBids, int numNonPassCorrections, int numTotalCycles, double fError) 
{
	// check flag
	if (m_bCancel)
		return FALSE;

	// update display
	m_strNumHands.Format(_T("%d"), numHands);
	m_strNumBids.Format(_T("%d"), numBids);
	m_strNumCorrections.Format(_T("%d"), numCorrections);
	m_strCorrectionRate.Format(_T("%.4f%%"), (numCorrections / (double)numBids) * 100);
	//
	m_strNumNPBids.Format(_T("%d"), numNonPassBids);
	m_strNumNPCorrections.Format(_T("%d"), numNonPassCorrections);
	m_strNPCorrectionRate.Format(_T("%.4f%%"), (numNonPassCorrections / (double)numNonPassBids) * 100);
	//
	m_strNumTotalCycles.Format(_T("%d"), numTotalCycles);
	m_strErrorValue.Format(_T("%.8f"), fError);
	UpdateData(FALSE);
	UpdateWindow();
	
	// and pump messages
	// check for messages
    MSG msg;
    while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) 
    { 
        if (!AfxGetApp()->PumpMessage()) 
        { 
            ::PostQuitMessage(0); 
            return FALSE; 
        } 
    } 

	//
	if (m_bCancel)
		return FALSE;
	else
		return TRUE;
}



//
void CNNetAutotrainStatusDlg ::OnCancel() 
{
	m_bCancel = TRUE;	
//	CDialog::OnCancel();
}
