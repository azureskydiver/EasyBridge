//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// TestPlayDialog.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "EasyBDoc.h"
#include "EasyBVw.h"
#include "Mainfrm.h"
#include "Player.h"
#include "TestPlayDialog.h"
#include "subclass.h"
#include "progopts.h"
#include "MainFrameopts.h"
#include "docopts.h"
#include "mmsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//#define _TESTING


/////////////////////////////////////////////////////////////////////////////
// CTestPlayDialog dialog


CTestPlayDialog::CTestPlayDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTestPlayDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestPlayDialog)
	m_strStatus = _T("");
	m_strPercentMade = _T("");
	//}}AFX_DATA_INIT

	//
	m_bPlayActive = FALSE;
	m_bStopFlag = FALSE;
}


void CTestPlayDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestPlayDialog)
	DDX_Control(pDX, IDC_LIST_RESULTS, m_listResults);
	DDX_Text(pDX, IDC_STATUS_TEXT, m_strStatus);
	DDX_Text(pDX, IDC_PERCENT_MADE, m_strPercentMade);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestPlayDialog, CDialog)
	//{{AFX_MSG_MAP(CTestPlayDialog)
	ON_BN_CLICKED(ID_STOP, OnStop)
	ON_WM_CLOSE()
	ON_BN_CLICKED(ID_START, OnStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CTestPlayDialog message handlers


LPCTSTR tszRowTitle[] = {
	"1-Level", "2-Level", "3-Level", "4-Level", 
	"5-Level", "6-Level", "7-Level", "Totals",
};
LPCTSTR tszColumnName[] = {
	"Contract", "Clubs  ", "Diams  ", "Hearts ", "Spades", "Notrump", "Doubled", "Redbld", "Totals        "
};
const int tnumColumns = sizeof(tszColumnName) / sizeof(LPCTSTR);
int nDoubledColumn = 5;
int nRedoubledColumn = 6;
int nTotalsColumn = 7;
int nTotalsRow = 7;


//
// OnInitDialog() 
//
BOOL CTestPlayDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// 
	CDialog::OnInitDialog();
	const int tnColSpacer = 14;

	// init the list control
	for(int i=0;i<tnumColumns;i++)
		m_listResults.InsertColumn(i, tszColumnName[i], LVCFMT_LEFT, m_listResults.GetStringWidth(tszColumnName[i]) + tnColSpacer, i);

	// subclass the list control
//	wpOrigListCtrlProc = (WNDPROC) SetWindowLong(m_listResults.GetSafeHwnd(), GWL_WNDPROC, (LONG) ListCtrlSubclassProc);

	// clear data
	for(i=0;i<8;i++)
	{
		for(int j=0;j<8;j++)
		{
			m_numContracts[i][j] = 0;
			m_numMade[i][j] = 0;
		}
	}

	GetDlgItem(ID_STOP)->EnableWindow(FALSE);

/*
	// record initial window characterisitcs
	CRect winRect, listRect;
	GetClientRect(&winRect);
	m_listPlayPlan.GetWindowRect(&listRect);
	ScreenToClient(&listRect);
	m_nInitialX = listRect.left;
	m_nInitialY = listRect.top;
	m_nInitialXMargin = listRect.left - winRect.left;
	m_nInitialYMargin = winRect.bottom - listRect.bottom;
*/

	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CTestPlayDialog::OnOK() 
{
	// 
	EndDialog(IDOK);	
//	CDialog::OnOK();
}


//
void CTestPlayDialog::OnClose() 
{
	// 
	OnStop();	
	CDialog::OnClose();
}


//
void CTestPlayDialog::OnStop() 
{
	// set the flag
	m_bStopFlag = TRUE;
	m_strStatus = "Stopping...";
	UpdateData(FALSE);
	GetDlgItem(IDC_STATUS_TEXT)->UpdateWindow();
}


//
void CTestPlayDialog::Update() 
{
	// display each row
	m_listResults.DeleteAllItems();

	// get the latest results from the document
	int nTrumpSuit = pDOC->GetTrumpSuit();
	int nContractLevel = pDOC->GetContractLevel();
	int nDeclaringTeam = pDOC->GetDeclaringTeam();
	int numTricksMade = pDOC->GetNumTricksWonByTeam(nDeclaringTeam);
	BOOL bDoubled = pDOC->IsContractDoubled();
	BOOL bReDoubled = pDOC->IsContractRedoubled();

	// update counts
	m_numContracts[nContractLevel-1][nTrumpSuit]++;
	m_numContracts[nContractLevel-1][nTotalsColumn]++;
	m_numContracts[nTotalsRow][nTrumpSuit]++;
	m_numContracts[nTotalsRow][nTotalsColumn]++;
	if (bDoubled)
	{
		m_numContracts[nContractLevel-1][nDoubledColumn]++;
		m_numContracts[7][nDoubledColumn]++;
	}
	else if (bReDoubled)
	{
		m_numContracts[nContractLevel-1][nRedoubledColumn]++;
		m_numContracts[7][nRedoubledColumn]++;
	}

	// also update # contracts made
	if (numTricksMade >= (nContractLevel+6))
	{
		m_numMade[nContractLevel-1][nTrumpSuit]++;
		m_numMade[nContractLevel-1][nTotalsColumn]++;
		m_numMade[nTotalsRow][nTrumpSuit]++;
		m_numMade[nTotalsRow][nTotalsColumn]++;
		if (bDoubled)
		{
			m_numMade[nContractLevel-1][nDoubledColumn]++;
			m_numMade[7][nDoubledColumn]++;
		}
		else if (bReDoubled)
		{
			m_numMade[nContractLevel-1][nRedoubledColumn]++;
			m_numMade[7][nRedoubledColumn]++;
		}
	}

	// and display
	for(int nRow=0;nRow<8;nRow++)
	{
		m_listResults.InsertItem(nRow, tszRowTitle[nRow]);
		for(int nCol=0;nCol<8;nCol++)
			m_listResults.SetItem(nRow, nCol+1, LVIF_TEXT, FormString("%d / %d", m_numMade[nRow][nCol], m_numContracts[nRow][nCol]), 0, 0, 0, 0L);
	}

	// show % made
	CStatic* pLabel = (CStatic*) GetDlgItem(IDC_GAMES_MADE_LABEL);
	if (!pLabel->IsWindowVisible())
	{
		pLabel->ShowWindow(SW_SHOW);
		pLabel->UpdateWindow();
	}
	double fPercent = m_numMade[nTotalsRow][nTotalsColumn] / (double) m_numContracts[nTotalsRow][nTotalsColumn];
	m_strPercentMade.Format(_T("%.1f%%"), fPercent * 100);
	UpdateData(FALSE);

	//
	m_listResults.UpdateWindow();
}



//
void CTestPlayDialog::OnStart() 
{
	//
	GetDlgItem(ID_START)->EnableWindow(FALSE);
	GetDlgItem(ID_STOP)->EnableWindow(TRUE);
	m_bPlayActive = TRUE;

	// results array is 8 x 8
	//          Clubs  Diamonds  Hearts  Spades  NT  Doubled  Redoubled  Total
	// -----------------------------------------------------------------------
	// 1-level   xx/yy
	// 2-level
	// ...
	// 7-level
	// Totals
	//
	int numHands = 0;
	BOOL bBreak = FALSE;

	//
	// suppress updates
	//
	pDOC->SuppressBidHistoryUpdate(TRUE);
	pDOC->SuppressPlayHistoryUpdate(TRUE);
	pMAINFRAME->HideDialog(twBidDialog);
	pVIEW->ClearDisplay();
	pVIEW->SuppressRefresh();
	pDOC->ClearAllInfo();

	// save settings
	BOOL bOldClaimSetting = theApp.GetValue(tbComputerCanClaim);
	theApp.SetValue(tbComputerCanClaim, FALSE);
	BOOL bOldAnalysisSetting = theApp.GetValue(tbEnableAnalysisTracing);
#ifndef _TESTING
	theApp.SetValue(tbEnableAnalysisTracing, FALSE);
#endif

	// 
	theApp.SetValue(tbAutoTestMode, TRUE);
	m_strStatus = "Auto play in progress...";
	UpdateData(FALSE);

	//
	int nCode = 0;
	double lfTotalTime = 0;
	long lNumHands = 0;
	CString strAvgTime;

	// loop 
	do
	{
		// play continuously

		// deal a new hand
		m_strStatus = "Dealing...";
		UpdateData(FALSE);
		pDOC->DealHands();
		numHands++;

		// and get bids
		m_strStatus = "Bidding...";
		UpdateData(FALSE);
		do 
		{
			// get the computer's bids
			int nPos = pDOC->GetCurrentPlayerPosition();
			int nBid = pDOC->GetCurrentPlayer()->Bid();
			nCode = pDOC->EnterBid(nPos, nBid);
			if ((nCode == -99) || (nCode == 1))
			{
				// passed out, or 3 passes, and bidding is complete
				break;
			}
			else if (nCode == -1) 
			{
				AfxMessageBox("Error in Bidding Dialog!");
				bBreak = TRUE;
				break;
			}
		} while (!bBreak);

		// bidding is complete; see if we reached a contract
		if (nCode == -99)
			continue;	// passed out, so redeal

		// start timeing
		long lStartTime = timeGetTime();

		// now play out the hand -- play on full auto
		theApp.SetValue(tnCardPlayMode, CEasyBApp::PLAY_FULL_AUTO_EXPRESS);
		pDOC->SetValue(tbExpressPlayMode, TRUE);
		pDOC->InvokeNextPlayer();

		// pump the mesage loop while the hand is being played out
		m_strStatus = "Playing hand...";
		UpdateData(FALSE);
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) 
		{ 
			if (!AfxGetApp()->PumpMessage()) 
			{ 
				::PostQuitMessage(0); 
				return; 
			} 
		} 

		// reset flags
		pDOC->SetValue(tbExpressPlayMode, FALSE);

		// end timer 
		lNumHands++;
		long lEndTime = timeGetTime();
		lfTotalTime += (lEndTime - lStartTime);
		strAvgTime.Format(_T("%.1f secs"), lfTotalTime / (lNumHands*1000));
		//
		CStatic* pText = (CStatic*) GetDlgItem(IDC_AVG_TIME);
		pText->SetWindowText(strAvgTime);
		pText->UpdateWindow();
		//
		CStatic* pLabel = (CStatic*) GetDlgItem(IDC_LABEL_AVGTIME);
		if (!pLabel->IsWindowVisible())
		{
			pLabel->ShowWindow(SW_SHOW);
			pLabel->UpdateWindow();
		}

		//
		if (m_bStopFlag)
			break;

		// save results and update the display
		Update();

	} while (!bBreak);


	//
	// done -- reset
	//
	pDOC->SuppressBidHistoryUpdate(FALSE);
	pDOC->SuppressPlayHistoryUpdate(FALSE);
	pVIEW->EnableRefresh();
	pVIEW->Refresh(TRUE);
	pDOC->ClearAllInfo();

	// restore settings
	theApp.SetValue(tbComputerCanClaim, bOldClaimSetting);
	theApp.SetValue(tbEnableAnalysisTracing, bOldAnalysisSetting);

	//
	theApp.SetValue(tbAutoTestMode, FALSE);
	GetDlgItem(ID_START)->EnableWindow(TRUE);
	GetDlgItem(ID_STOP)->EnableWindow(FALSE);
	m_bPlayActive = FALSE;
	m_bStopFlag = FALSE;
	
	//
	m_strStatus = "Ready.";
	UpdateData(FALSE);
}




