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
#include "convcodes.h" // NCR-SCU

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
	m_bSaveDowns = FALSE;   // NCR-AT turn on/off saving of brds
	m_bSaveConvUsed = FALSE;  // NCR-SCU turn off saving of brds
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
	ON_BN_CLICKED(ID_START, OnStart)
	ON_BN_CLICKED(ID_STOP, OnStop)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SAVEDOWNS_CHECK, OnSavedownsCheck)
	ON_BN_CLICKED(IDC_SAVECONVUSED_CHECK, OnSaveConvUsedCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CTestPlayDialog message handlers


LPCTSTR tszRowTitle[] = {
	"1-Level", "2-Level", "3-Level", "4-Level", 
	"5-Level", "6-Level", "7-Level", "Totals",
};
LPCTSTR tszColumnName[] = {  // NCR-AT added space to: Clubs, Hearts, Spades and Totals
	"Contract", "Clubs   ", "Diams  ", "Hearts  ", "Spades ", "Notrump", "Doubled", "Redbld", "Totals         "
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
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<tnumColumns;i++)
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
	
#ifdef _DEBUG
	// NCR Output statistics to a file
	if(TRUE) {
		CFile file;
		CFileException fileException;
		CString strPath;
		strPath.Format("%s\\AutoTestStats_%s.txt", theApp.GetValue(tszProgramDirectory),
						pDOC->GetDealIDString());
		int nCode = file.Open((LPCTSTR)strPath, 
							  CFile::modeWrite | CFile::modeCreate | CFile::shareDenyWrite, 
							  &fileException);
		CArchive ar(&file, CArchive::store);
	    CFile* pFile = ar.GetFile();
		CString header = "Contract    Club         Diamonds     Heart        Spades       NoTrump      Doubled      ReDoubled    Totals\n";
		pFile->Write(header, header.GetLength());

  		for(int nRow=0; nRow<8; nRow++)
		{
			CString rowData;
			CString strLine;
			if(nRow < 7)   // 0 to 6 for contract levels 1->7
			   strLine.Format("Level %d:  ", nRow+1);
			else
				strLine =     "Totals:   "; // last line is totals	
			for(int nCol=0; nCol<8; nCol++) {
				rowData.Format("%4d /%4d   ",  m_numMade[nRow][nCol], m_numContracts[nRow][nCol]);
				strLine += rowData;
			}
			strLine += "\r\n";
			pFile->Write((LPCTSTR) strLine, strLine.GetLength());
		}
	    double fPercent = m_numMade[nTotalsRow][nTotalsColumn] / (double) m_numContracts[nTotalsRow][nTotalsColumn];
		CString percent;
	    percent.Format(_T("Percent made: %.1f%%\r\n"), fPercent * 100);
		pFile->Write((LPCTSTR) percent, percent.GetLength());
  		ar.Close();
		file.Close();
	} // NCR outputting statistics to file
#endif

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

	// NCR-706  WRite out boards that go down using Cash play
	BOOL bSaveThisBoard = FALSE;
	if(theApp.GetValue(tnFileProgramBuildNumber) == 1234) {
		bSaveThisBoard = TRUE;  // Ask that this board be written
		theApp.SetValue(tnFileProgramBuildNumber, 1);   // turn off
    }  // NCR-706 end
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
	// NCR-AT  Save contracts that went down
	else if (((nContractLevel >= 1) && m_bSaveDowns) || bSaveThisBoard)   //<<<<<<<< NCR changed to 1 vs 4 for NCR-706 testing
	{
		int nDownCnt = (nContractLevel+6) - numTricksMade; // show number of tricks down
		CFile file;
		CFileException fileException;
		CString strPath;
//		CTime time = CTime::GetCurrentTime();
		strPath.Format("%s\\LostContract_Down_%d_%s.brd",theApp.GetValue(tszProgramDirectory),
								nDownCnt, pDOC->GetDealIDString());
//				                (LPCTSTR)time.Format("%X"));
		int nCode = file.Open((LPCTSTR)strPath, 
							  CFile::modeWrite | CFile::modeCreate | CFile::shareDenyWrite, 
							  &fileException);
		CArchive ar(&file, CArchive::store);
		pDOC->WriteFile(ar);
		ar.Close();
		file.Close();
	} // NCR-AT end saving lost contracts
#ifdef _DEBUG
	// NCR-SCU Save hands that have used conventions
    //
	if(m_bSaveConvUsed &&  (pDOC->GetNumSCU() > 0))
	{
		CFile file;
		CFileException fileException;
		CString strPath;
		// Build filename from conventions used
		CString fileNm = "CU";  // Filename prefix
		const int nInitNameLen = fileNm.GetLength(); // save length for test below
		bool usedConv[] = {false,false,false,false,false,false,false,false,false,false,
			               false,false,false,false,false,false,false,false,false,false,
			               false,false,false,false,false,false,false,false,false,false,
						   false,false,false,false,false,false,false,false,false,false};

		for(int k = 0; k < pDOC->GetNumSCU(); k++) {
			int cix = pDOC->GetSCU(k); // get next convention
			if(cix == tidOvercalls)
				continue;  // skip saving overcalls
			if(usedConv[cix])
				continue;  // skip if seen before
			usedConv[cix] = true;  // remember that we've use this one
			fileNm += GetConvName(cix); // add on convention
		} // end for(k) thru used conventions

		if(fileNm.GetLength() > nInitNameLen) {  // Only write if a convention used
			// build filename from conventions used
			strPath.Format("%s\\%s_%s.brd",theApp.GetValue(tszProgramDirectory),
							fileNm,
							pDOC->GetDealIDString());
			int nCode = file.Open((LPCTSTR)strPath, 
								  CFile::modeWrite | CFile::modeCreate | CFile::shareDenyWrite, 
								  &fileException);
			CArchive ar(&file, CArchive::store);
			pDOC->WriteFile(ar);
			ar.Close();
			file.Close();
		}
	}  // NCR-SCU end saving hands that used a convention
#endif

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
#ifdef _DEBUG
// NCR-SCU Get name of convention. Have leading _ as separator
//
CString CTestPlayDialog::GetConvName(int id)
{
	CString convNm = "_";   // prefix with a _
	switch(id) {
	case tid5CardMajors:
		return convNm + "5CardMajor";
	case tidArtificial2ClubConvention:
		return convNm + "Strong2Club";
	case tidWeakTwoBids:
		return convNm + "WeakTwo";
	case tidWeakJumpOvercalls:
		return convNm + "WeakJumpOvercall";
	case tidStrongTwoBids:
		return convNm + "Strong2";
	case tidShutoutBids:
		return convNm + "Shutout";
	case tid4thSuitForcing:
		return convNm + "4thSuitForcing";
	case tidOgust:
		return convNm + "Ogust";
	case tidBlackwood:
		return convNm + "Blackwood";
	case tidRKCB:
		return convNm + "RoyalKeyCardBlackwood";
	case tidCueBids:
		return convNm + "Cuebid";
	case tidGerber:
		return convNm + "Gerber";
	case tidStayman:
		return convNm + "Stayman";
	case tidJacobyTransfers:
		return convNm + "JacobyTransfer";
	case tidLimitRaises:
		return convNm + "LimitRaise";
	case tidTakeoutDoubles:
		return convNm + "TakeoutDouble";
	case tidNegativeDoubles:
		return convNm + "NegativeDouble";
	case tidSplinterBids:
		return convNm + "SplinterBid";
	case tidMichaels:
		return convNm + "Michaels";
	case tidUnusualNT:
		return convNm + "UnusualNT";
	case tidJacoby2NT:
		return convNm + "Jacoby2NT";
	case tidGambling3NT:
		return convNm + "Gambling3NT";
	case tidDrury:
		return convNm + "Drury";
	case tidLebensohl:
		return convNm + "Lebensohl";
	case tidDONT:
		return convNm + "DONT";
	case tidOvercalls:
		return convNm + "Overcall";

	default:
		CString val;
		val.Format("_Unkn_%d",id);
		return val;
	}
}
#endif
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
//NCR	pVIEW->SuppressRefresh();   // Dialog box "smears" with this code ???
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




// NCR-AT  Toggle flag to save hands that went down
void CTestPlayDialog::OnSavedownsCheck() 
{
	m_bSaveDowns = !m_bSaveDowns; // toggle 
	
}
// NCR-AT  Toggle flag to save hands that went down
void CTestPlayDialog::OnSaveConvUsedCheck() 
{
	m_bSaveConvUsed = !m_bSaveConvUsed; // toggle 
	
}
