//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// BidDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "mainfrm.h"
#include "mainframeopts.h"
#include "EasyBvw.h"
#include "progopts.h"
#include "viewopts.h"
#include "Player.h"
#include "BidEngine.h"
#include "BidDialog.h"
#include "Deck.h"
#include "subclass.h"
#include "PassedHandDialog.h"
#include "PassedHandSimpleDialog.h"
#include "NNetAutotrainStatusDlg.h"
#include <mmsystem.h>


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const int BD_MODE_NONE			= 0;
const int BD_MODE_WAITFORBID	= 1;
const int BD_MODE_EVALUATE		= 2;
const int BD_MODE_DONE			= 3;



/////////////////////////////////////////////////////////////////////////////
// CBidDialog dialog


CBidDialog::CBidDialog(int nDialogID, CWnd* pParent /*=NULL*/)
	: CDialog(nDialogID, pParent)
{
	//{{AFX_DATA_INIT(CBidDialog)
	m_bManualBidding = FALSE;
	//}}AFX_DATA_INIT
	//
	m_nCurrMode = BD_MODE_NONE;
//	m_pSubclassButtons = NULL;
	//
	m_nComputerBid = BID_NONE;  // NCR changed NONE to BID_NONE
	m_bSpeechEnabled = FALSE;
	m_bTrainingMode = FALSE;
}


void CBidDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBidDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBidDialog, CDialog)
	//{{AFX_MSG_MAP(CBidDialog)
	ON_BN_CLICKED(IDC_BID_HINT, OnBidHint)
	ON_BN_CLICKED(IDC_BID_BACK_UP, OnBidBackUp)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_SHOWWINDOW()
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_BID_REDEAL, OnBidRedeal)
	ON_BN_CLICKED(IDC_BID_RESTART, OnBidRestart)
	ON_BN_CLICKED(ID_REDEAL, OnBidRedeal)
	//}}AFX_MSG_MAP
//	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
//
// CBidDialog message handlers
//
/////////////////////////////////////////////////////////////////////////////

// Array of button ID's to be subclassed
static int nBtnID[] = {
	IDCANCEL, IDHELP, 
	IDC_BID_PASS, IDC_BID_DOUBLE, IDC_BID_REDOUBLE,
	IDC_BID_RESTART, IDC_BID_REDEAL, IDC_BID_HINT, IDC_BID_BACK_UP, IDC_AUTO_TRAIN,
	-1
};

// tooltip information
const DialogToolTipInfo  tToolTipInfo[] = {
	{ 	IDCANCEL,			"Cancel bidding"				},
	{ 	IDHELP,				"Help on using this dialog"		},
	{ 	IDC_BID_PASS,		"Pass "							},
	{	IDC_BID_DOUBLE,		"Double the opponents' bid",	},
	{	IDC_BID_REDOUBLE,	"Redouble the opponents' bid",	},
	{	IDC_BID_RESTART,	"Restart the bidding",			},
	{	IDC_BID_REDEAL,		"Deal a new hand",				},
	{	IDC_BID_HINT,		"Get a bidding hint",			},
	{	IDC_BID_BACK_UP,	"Back up one bid",				},
	{	IDC_AUTO_TRAIN,		"Auto train the neural net",	},
};

const int tnumToolTips = sizeof(tToolTipInfo) / sizeof(DialogToolTipInfo);



//
int CBidDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	

	// load button icons
	for(int i=0;i<5;i++)
	{
		for(int j=0;j<7;j++)
		{
			int nIndex = (j * 5) + i;
			m_buttonIcons[i][j] = (HICON) LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDI_CONTRACT_1CLUB)+nIndex,
													IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
		}
	}

	// load other icons
	m_hIconPlayerPrompt = (HICON) LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDI_PLAYER_BIDPROMPT), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	m_hIconComputerPrompt = (HICON) LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDI_COMPUTER_BIDPROMPT), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);

	// 
	return 0;
}


//
void CBidDialog::OnDestroy() 
{
	CDialog::OnDestroy();

	// delete flat buttons
	delete[] m_pFlatBaseButtons;

	// delete the button icons
/*
	for(int i=0;i<5;i++)
		for(int j=0;j<7;j++)
			m_buttonIcons[i][j].DeleteObject();
*/
}


//
BOOL CBidDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//
//	pMAINFRAME->SetNonBoldDialogFont(this);
	//
//	CenterWindow();
	InitBiddingSequence();

/*
	// subclass the buttons
	// first the misc controls
	for(int i=0;i<tnumToolTips;i++) 
	{
	 	CWnd* pWnd = GetDlgItem(tToolTipInfo[i].nControlID);
		ASSERT(pWnd != NULL);
		wpOrigButtonProc = (WNDPROC) SetWindowLong(pWnd->m_hWnd, GWL_WNDPROC, (LONG) ButtonSubclassProc);
	}
*/
	
	// get button bitmap mode 
	EnableButtonImages(TRUE);

	// populate the control ID -> index map
	EnableToolTips(TRUE);
	int i; // NCR-FFS added here and removed below
	for(/*int*/ i=0;i<tnumToolTips;i++)
		m_mapIDtoIndex.SetAt(tToolTipInfo[i].nControlID, i);

	// subclass flat buttons
	m_pFlatBaseButtons = new CFlatButton[tnumToolTips];
	for(i=0;i<tnumToolTips;i++)
		m_pFlatBaseButtons[i].SubclassDlgItem(tToolTipInfo[i].nControlID, this);
	
	// clear bid displays
	for(i=0;i<4;i++)
	{
		SetDlgItemText(IDC_BID_SOUTH+i,"");
		((CStatic*)GetDlgItem(IDCI_BID_SOUTH+i))->SetIcon(NULL);
	}

	// all done
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CBidDialog::OnHelp() 
{
	// should use derived routines
	ASSERT(FALSE);
}


//
BOOL CBidDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}



//
void CBidDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
	pMAINFRAME->PostMessage(WM_KEYDOWN, nChar, MAKELONG(nRepCnt, nFlags));
//	pMAINFRAME->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
}




//
//
//
BOOL CBidDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int nID = LOWORD(wParam);
	int nCode = HIWORD(wParam);
	int nMessageType = wParam;
	int nBid;

	//
	// Respond to a button press
	//
	if (wParam == WMS_BIDDING_RESTART) 
	{
		OnBidRestart();
		return TRUE;
	}
	//
	if (wParam == WMS_FLASH_BUTTON)
	{
		FlashButton(lParam);
		return TRUE;
	}
	//
	if ((nCode == BN_CLICKED) &&
		( ((nID >= IDC_BID_PASS) && (nID <= IDC_BID_7NT)) ||
	      (nID == IDC_BID_DOUBLE) || (nID == IDC_BID_REDOUBLE) ) ) 
	{
		// check current mode
		if ((m_nCurrMode != BD_MODE_WAITFORBID) && (m_nCurrMode != BD_MODE_TRAIN) && 
							(m_nCurrMode != BD_MODE_MANUAL_BID))
			return TRUE;
		//
		if (nID == IDC_BID_DOUBLE)
			nBid = BID_DOUBLE;
		else if (nID == IDC_BID_REDOUBLE)
			nBid = BID_REDOUBLE;
		else
			nBid = nID - IDC_BID_PASS;

		// see if we're in training mode
		CPlayer* pPlayer = pDOC->GetCurrentPlayer();
		int nPos = pPlayer->GetPosition();
/*
		if (m_nCurrMode == BD_MODE_TRAIN)
		{
			// train the neural net

			// correct the conputer player's bid if necesary
			if (nBid != m_nComputerBid)
			{
				// train the net with the correct bid
				CBidEngine* pBidEngine = pPlayer->GetBidEngine();
				pBidEngine->SetNeuralNetBid(nBid);
			}
		}
*/
		if (m_nCurrMode == BD_MODE_MANUAL_BID)
		{
			// do any special processing here
		}
		
		// record the bid
		if (nPos == SOUTH)
			pPlayer->EnterHumanBid(nBid);
		int nCode = pDOC->EnterBid(nPos, nBid);
		UpdateBidDisplay(nPos, nBid);	

		// check the result of the bid
		ProcessBidReturnCode(nCode);
		if (nCode == 0)
		{
			// bid entered OK, move on to the next player
			int nPos = pDOC->GetCurrentPlayerPosition();
			GetComputerBids(nPos);
		}
		return TRUE;
	}
	//
	return CDialog::OnCommand(wParam, lParam);
}



//
// ProcessBidReturnCode()
//
void CBidDialog::ProcessBidReturnCode(int nCode)
{
	if (nCode == -99)
	{
		// passed out the hand
		DealNewHands();		
	}
	else if (nCode == 1) 
	{
		// bidding done
		BiddingDone();		
	}
}




//
// RegisterBid()
//
// called from OnCommand() above or from the outside to force a bid
//
void CBidDialog::RegisterBid(int nBid, BOOL bShowButtonPress)
{
	// simulate button press if desired
	if (bShowButtonPress)
		PressBidButton(nBid);

	// enter the bid
	int nStatus = EnterPlayerBid(nBid);

	// see what the result of the bid was
	if (nStatus == 1)
	{
		// bidding is finished
		BiddingDone();
	}
	else if (nStatus == 0)
	{
		// received player's bid; get computer players' bids
		if (!m_bManualBidding)
			nStatus = GetComputerBids(WEST);
		else
			GetComputerBids(WEST);
	}
}



//
// OnBidBackUp()
// 
void CBidDialog::OnBidBackUp()
{
	//
	int nPos = pDOC->GetCurrentPlayerPosition();
	UpdateBidDisplay(nPos, -1);	
	pDOC->UndoBid();
	GetComputerBids(nPos);
}



//
int CBidDialog::EnterPlayerBid(int nBid)
{
	if (!pDOC->IsBidValid(nBid))
		return 0;

	// update records ( this calls ValidateBid() )
	int nPlayer = pDOC->GetCurrentPlayerPosition();	// not necessarily South
	UpdateBidDisplay(nPlayer, nBid);	
	PLAYER(nPlayer).EnterHumanBid(nBid);
	int nCode = pDOC->EnterBid(nPlayer, nBid);
	if (nPlayer == SOUTH)
		m_nComputerBid = BID_NONE;  // NCR changed NONE to BID_NONE

	//
	ProcessBidReturnCode(nCode);
	//
	if (nCode == -99)
		return -1;
	else 
		return nCode;
}



//
// GetComputerBids()
//
// gets the computer players' bids, starting with the specified 
// player position
//
// return code:
// 0  = OK
// 1  = bidding complete
// -1 = passed out or error
//
int CBidDialog::GetComputerBids(int nStart)
{
	int i,nBid;
	m_nCurrMode = BD_MODE_EVALUATE;

	//
	UpdateWindow();
	if (m_bTrainingMode)
	{
		// get the current computer's bid
		nBid = PLAYER(nStart).Bid();
		m_nComputerBid = nBid;
		UpdateBidDisplay(nStart, nBid, FALSE, TRUE);	
		DisableControls();
		m_nCurrMode = BD_MODE_TRAIN;
		return 0;
	}
	else if (m_bManualBidding)
	{
		// get the current computer's bid
		UpdateBidDisplay(nStart, -2, TRUE, FALSE);	
		DisableControls();
		m_nCurrMode = BD_MODE_MANUAL_BID;
		return 0;
	}
	else
	{
		// get computer bids through East
		for(i=nStart;i<=EAST;i++) 
		{
			// insert cadence delay here
			if (m_bInsertBiddingPause)
			{
				UpdateBidDisplay(i, -2);	
				::Sleep(m_nBiddingPauseLength);
			}
			else if (m_bSpeechEnabled)
			{
				// also insert a short pause if speaking
				UpdateBidDisplay(i, -2);	
				::Sleep(100);
			}

			//
			nBid = PLAYER(i).Bid();
			UpdateBidDisplay(i,nBid);	
			int nCode = pDOC->EnterBid(i,nBid);

			// then proceed
			if (nCode == -99)
			{
				DealNewHands();
				return -1;
			}
//			else if (nCode >= 0)
//			{
//				UpdateBidDisplay(i,nBid);	
//			}
			else if (nCode == 1) 
			{
				// 3 passes, and bidding is complete
				if (BiddingDone())
					return 1;
				else
					return 0;
			}
			else if (nCode == -1) 
			{
				AfxMessageBox("Error in Bidding Dialog!");
				return -1;
			}
		}

		// else all went OK
		UpdateBidDisplay(SOUTH,0,TRUE);	
		DisableControls();
		m_nCurrMode = BD_MODE_WAITFORBID;
		pDOC->ShowAutoHint();
		return 0;
	}
}


//
void CBidDialog::EnableManualBidding(BOOL bEnable) 
{
	m_bManualBidding = bEnable;
	int nPos = pDOC->GetCurrentPlayerPosition();
	GetComputerBids(nPos);
}


//
void CBidDialog::OnAutotrain() 
{
	CNNetAutotrainStatusDlg	statusDlg;
	statusDlg.Create(CNNetAutotrainStatusDlg::IDD, this);
	statusDlg.ShowWindow(SW_SHOW);
	statusDlg.UpdateWindow();
	// 
	int numHands=0, numBids=0, numNonPassBids=0, numCorrections=0, numNonPassCorrections=0, numCycles=0;
	double fError = 0;

	//
	m_nCurrMode = BD_MODE_AUTO_TRAIN;
	BOOL bBreak = FALSE;
	SetWindowText(_T("Auto Training in progress"));

	// suppress updates
	pDOC->SuppressBidHistoryUpdate(TRUE);
	pDOC->SuppressPlayHistoryUpdate(TRUE);
	pVIEW->SuppressRefresh();
	pVIEW->ClearDisplay();
	BOOL bOutputVisible = pMAINFRAME->IsDialogVisible(twNNetOutputDialog);
//	pMAINFRAME->HideDialog(twNNetOutputDialog);

	// loop 
	do
	{
		// get computer bids continuously
		int nCode = 0;
		do
		{
			// deal a new hand
			pDOC->DealHands();
			numHands++;

			// and get bids
			do 
			{
				// get the computer's bid
				CBidEngine* pBidEngine = pDOC->GetCurrentPlayer()->GetBidEngine();
				theApp.SetValue(tnBiddingEngine, 1);
				int nNetBid = pBidEngine->Bid();
				theApp.SetValue(tnBiddingEngine, 0);
				int nRuleBid = pBidEngine->Bid();
				theApp.SetValue(tnBiddingEngine, 1);
				numBids++;
				if (nNetBid != BID_PASS)
					numNonPassBids++;
				//
				if (nRuleBid != nNetBid)
				{
					pBidEngine->SetNeuralNetBid(nRuleBid);
					fError = pBidEngine->GetNeuralNetError();
					numCorrections++;
					if (nNetBid != BID_PASS)
						numNonPassCorrections++;
					numCycles = pBidEngine->GetNeuralNetTrainingCycles();
				}

				// record the bid and process
				int nCode = pDOC->EnterBid(pDOC->GetCurrentPlayerPosition(), nRuleBid);
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

				// update display
				if (!statusDlg.Update(numHands, numBids, numCorrections, numNonPassBids, numNonPassCorrections, numCycles, fError))
				{
					bBreak = TRUE;
					break;
				}

			} while (!bBreak);
			//
			if (bBreak)
				break;

		} while (!bBreak);
		//
		if (bBreak)
			break;

	} while (!bBreak);

	// done -- reset
	statusDlg.ShowWindow(SW_SHOW);
	pDOC->SuppressBidHistoryUpdate(FALSE);
	pDOC->SuppressPlayHistoryUpdate(FALSE);
	pVIEW->EnableRefresh();
	pVIEW->Refresh(TRUE);
//	if (bOutputVisible)
//		pMAINFRAME->MakeDialogVisible(twNNetOutputDialog);
	OnBidRestart();
}



//
//
void CBidDialog::InitBiddingSequence()
{
	// check for autotrain mode
	if (m_nCurrMode == BD_MODE_AUTO_TRAIN)
		return;

	// get display mode
	m_bUseSuitSymbols = theApp.GetValue(tbUseSuitSymbols);

	// get manual bid option
	m_bManualBidding = theApp.GetValue(tbManualBidding);

	// get pause option
	m_bInsertBiddingPause = theApp.GetValue(tbInsertBiddingPause);
	if (m_bInsertBiddingPause)
		m_nBiddingPauseLength = theApp.GetValue(tnBiddingPauseLength) * 100;
	else
		m_nBiddingPauseLength = 0;

	// get speech option
	m_bSpeechEnabled = theApp.GetValue(tbEnableSpokenBids);

	// get bidding engine info
//	m_nBiddingEngine = theApp.GetValue(tnBiddingEngine);
//	m_bTrainingMode = theApp.GetValue(tbNNetTrainingMode);
	m_nBiddingEngine = 0;
	m_bTrainingMode = FALSE;

	// start the bidding process
	CEasyBDoc* pDoc = CEasyBDoc::GetDoc();
	if (!pDoc || !pDoc->IsInitialized())
		return;
	m_nComputerBid = BID_NONE;  // NCR changed NONE to BID_NONE
	EnableControls();
	DisableControls();
	
	// clear bid displays
	for(int i=0;i<4;i++)
	{
		SetDlgItemText(IDC_BID_SOUTH+i,"");
		((CStatic*)GetDlgItem(IDCI_BID_SOUTH+i))->SetIcon(NULL);
	}

	// and start bidding, if appropriate
	// but return if just initializing
	if (!theApp.IsBiddingInProgress())
		return;
	int nDealer = pDOC->GetDealer();
	if ((nDealer != SOUTH) || m_bTrainingMode)
	{
		GetComputerBids(nDealer);
	} 
	else 
	{
		UpdateBidDisplay(SOUTH,0,TRUE);	
		m_nCurrMode = BD_MODE_WAITFORBID;
		pDOC->ShowAutoHint();
	}

	// show dealer
	int nPos = pDOC->GetDealer();
	CString strTitle = FormString("Bid  (Dealer = %s)", PositionToString(nPos));
//	if (m_bTrainingMode)
//		strTitle += _T(" (Training Mode)");
	SetWindowText(strTitle);
}


//
void CBidDialog::CloseSecondaryWindows()
{
	if ((m_bidFinishedDialog.m_hWnd) &&
		(m_bidFinishedDialog.IsWindowVisible()))
		m_bidFinishedDialog.EndDialog(FALSE);
//	if (m_nBiddingEngine == 1)
//		pMAINFRAME->HideDialog(twNNetOutputDialog);
}



//
///////////////////////////////////////////////////////////
//
void CBidDialog::DealNewHands()
{
	// hand got passed out, so deal again
	// but first show the original hands if desired
	BOOL bRebid = FALSE;
	BOOL bEnableRebidOption = theApp.GetValue(tbAllowRebidPassedHands);
	BOOL bShowAndHide = FALSE;
	BOOL bFaceUpState = theApp.AreCardsFaceUp();
	if (theApp.GetValue(tbShowPassedHands) && bEnableRebidOption && !bFaceUpState)
		bShowAndHide = TRUE;
	//
	if (bShowAndHide)
	{
		theApp.SetValue(tbShowCardsFaceUp, TRUE);
		pVIEW->Notify(WM_COMMAND, WMS_RESET_DISPLAY, TRUE);
	}

	// allow rebid of passed hand if appropriate
	if (bEnableRebidOption)
	{
		// show dialog w/ option to rebid
		CPassedHandDialog passedHands;
		bRebid = passedHands.DoModal();
	}
	else
	{
		// no rebid option provided
		CPassedHandSimpleDialog passedHands(this);
		passedHands.DoModal();
	}

	// restore cards face-down status
	if (bShowAndHide)
		theApp.SetValue(tbShowCardsFaceUp, bFaceUpState);

	// see if we should redeal or rebid
	if (bRebid)
	{
		if (bShowAndHide)
			pVIEW->PostMessage(WM_COMMAND, WMS_REFRESH_DISPLAY);
		pMAINFRAME->PostMessage(WM_COMMAND, ID_BID_CURRENT_HAND);
	}
	else
	{
		// go on to the next hand
		pMAINFRAME->PostMessage(WM_COMMAND, ID_DEAL_NEW_HAND);
	}
}



//
///////////////////////////////////////////////////////////
//

//
// BiddingDone()
//
// handles cleanup after it's been established that the 
// bidding is finished
//
BOOL CBidDialog::BiddingDone()
{
	// this may be called twice due to delayed <space> key rollover
	if (!theApp.GetValue(tbBiddingInProgress))
		return FALSE;
	//
	m_nCurrMode = BD_MODE_DONE;
	CString strTemp;
	strTemp.Format("Contract is %s.\nDeclarer is %s;\n%s leads.",
			pDOC->GetFullContractString(),
			PositionToString(pDOC->GetDeclarerPosition()),
			PositionToString(pDOC->GetRoundLead()));
//	AfxMessageBox(strTemp, MB_ICONINFORMATION);
//	CBiddingFinishedDialog bidFinishedDialog;
	
	// mark the bidding finished here so that if the user presses <space>,
	// we don't try to provide another bidding hint!
	theApp.SetValue(tbBiddingInProgress, FALSE);
	m_bidFinishedDialog.SetText(strTemp);
	if (m_bidFinishedDialog.DoModal())
	{
		// bidding is complete
		// the bidding-in-progress flag has to be cleared here to keep the
		// view from popping the bid dialog up again when it's activated
//		EndDialog(TRUE);
		ShowWindow(SW_HIDE);
		pVIEW->PostMessage(WM_COMMAND, WMS_BIDDING_DONE);
		return TRUE;
	}
	else
	{
		// rebid or redeal
		theApp.SetValue(tbBiddingInProgress, TRUE);
		if (m_bidFinishedDialog.m_nResultCode == 0)
		{
			PostMessage(WM_COMMAND, IDC_BID_RESTART);
			return FALSE;
		}
		else
		{
			PostMessage(WM_COMMAND, ID_DEAL_NEW_HAND);
			return FALSE;
		}
	}
}


void CBidDialog::OnCancel() 
{
/*
	if (AfxMessageBox("Cancel Bidding?",MB_ICONQUESTION | MB_YESNO) == IDYES) {
		pVIEW->PostMessage(WM_COMMAND, WMS_BIDDING_CANCELLED);
		EndDialog(FALSE);
	}
*/
	// clear out bidding info
	pDOC->ClearBiddingInfo();
	theApp.SetValue(tbBiddingInProgress, FALSE);
	pVIEW->Notify(WM_COMMAND, WMS_BIDDING_CANCELLED);
//	EndDialog(FALSE);
	ShowWindow(SW_HIDE);
//	CDialog::OnCancel();
}

void CBidDialog::CancelImmediate() 
{
	// cancels immediately and uses SendMessage() insetad of Post()
	pDOC->ClearBiddingInfo();
	theApp.SetValue(tbBiddingInProgress, FALSE);
	pVIEW->Notify(WM_COMMAND, WMS_BIDDING_CANCELLED);
//	EndDialog(FALSE);
}

//
void CBidDialog::OnBidHint() 
{
	pDOC->GetGameHint();
}


//
void CBidDialog::OnBidRestart() 
{
	pDOC->RestartBidding();
	InitBiddingSequence();	
}

//
void CBidDialog::OnBidRedeal() 
{
	pMAINFRAME->PostMessage(WM_COMMAND, ID_DEAL_NEW_HAND);
}



//
void CBidDialog::DisableControls()
{
	if (pDOC->GetLastValidBid() == BID_PASS)
		return;

	// set defaults for double/redouble buttons
	BOOL bEnableDouble = TRUE;
	BOOL bEnableReDouble = FALSE;

	// can't double own team's bid
	int nBidTeam = pDOC->GetLastValidBidTeam();
	if (nBidTeam == NORTH_SOUTH && !m_bTrainingMode)
		bEnableDouble = FALSE;

	// or if already doubled
	if (pDOC->IsContractDoubled())
		bEnableDouble = FALSE;

	// can redouble only if already doubled && is team contract (or is training)
	if (pDOC->IsContractDoubled() && ((nBidTeam == NORTH_SOUTH) || m_bTrainingMode))
		bEnableReDouble = TRUE;

	// and set
	GetDlgItem(IDC_BID_DOUBLE)->EnableWindow(bEnableDouble);
	GetDlgItem(IDC_BID_REDOUBLE)->EnableWindow(bEnableReDouble);
}


//
void CBidDialog::UpdateBidDisplay(int nPos, int nBid, BOOL bPrompt, BOOL bTentative)
{
	int nTextID = IDC_BID_SOUTH + nPos;
	int nIconID = IDCI_BID_SOUTH + nPos;

	// see if we're using text for bids
	if (!UsingSuitSymbols() || !ISBID(nBid) || bTentative)
	{
		// show bid text
		GetDlgItem(nIconID)->ShowWindow(FALSE);
		GetDlgItem(nTextID)->ShowWindow(TRUE);
		CString strTemp;

		// show text
		if ((nPos == SOUTH) && (bPrompt))
		{
			SetDlgItemText(nTextID, "??");
		} 
		else if (nBid == -2)
		{
			SetDlgItemText(nTextID, "?");
		} 
		else if (nBid == -1) 
		{
			SetDlgItemText(nTextID, "");
		} 
		else if (bTentative)
		{
			strTemp = BidToShortString(nBid) + _T('?');
			SetDlgItemText(nTextID, (LPCTSTR)strTemp);
		}
		else 
		{
			// real bid
			strTemp = BidToShortString(nBid);
			SetDlgItemText(nTextID, (LPCTSTR)strTemp);
		}
		GetDlgItem(nTextID)->UpdateWindow();
	}
	else
	{
		// show bid icon
		GetDlgItem(nTextID)->ShowWindow(FALSE);
		GetDlgItem(nIconID)->ShowWindow(TRUE);
		CStatic* pStatic = (CStatic*) GetDlgItem(nIconID);
		if (ISBID(nBid))
		{
			int nSuit = (nBid-1) % 5;
			int nLevel = (nBid-1) / 5;
			pStatic->SetIcon(m_buttonIcons[nSuit][nLevel]);
		}
		else
		{
			pStatic->SetIcon(NULL);
		}
		//
		pStatic->UpdateWindow();
	}
/*
	CString strTemp;

	// show text
	if ((nPos == SOUTH) && (bPrompt))
	{
		SetDlgItemText(IDC_BID_SOUTH, "??");
	} 
	else if (nBid == -2)
	{
		SetDlgItemText(IDC_BID_SOUTH+nPos, "?");
	} 
	else if (nBid == -1) 
	{
		SetDlgItemText(IDC_BID_SOUTH+nPos, "");
	} 
	else if (bTentative)
	{
		strTemp = BidToShortString(nBid) + _T('?');
		SetDlgItemText(IDC_BID_SOUTH+nPos, (LPCTSTR)strTemp);
	}
	else 
	{
		// real bid
		strTemp = BidToShortString(nBid);

		// play sound if appropriate
		if (m_bSpeechEnabled)
		{
			CString strDir = theApp.GetValueString(tszProgramDirectory);
//			CString strPath = strDir +  _T("\\Sounds\\") + PositionToString(nPos) + strTemp + _T(".wav");
			CString strPath = strDir +  _T("\\Sounds\\") + strTemp + _T(".wav");
			CFile file;
			if (file.Open(strPath, CFile::modeRead))
			{
				file.Close();
				PlaySound(strPath, NULL, SND_SYNC | SND_NOSTOP | SND_NOWAIT);
			}
		}

		// show bid
		SetDlgItemText(IDC_BID_SOUTH+nPos, (LPCTSTR)strTemp);
	}
*/
}



//
void CBidDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CRect rect,parentRect;
	//
	if (bShow) 
	{
		// showing
		RepositionWindow();

		// if the neural net bidding engine is active, show the dialog
//		if (m_nBiddingEngine == 1)
//			pMAINFRAME->MakeDialogVisible(twNNetOutputDialog);
	}
	else
	{
		// if the neural net bidding engine is active, hide the dialog
//		if (m_nBiddingEngine == 1)
//			pMAINFRAME->HideDialog(twNNetOutputDialog);
	}

	//
	CDialog::OnShowWindow(bShow, nStatus);
}




//
void CBidDialog::RepositionWindow() 
{
	CRect rect,parentRect;
	GetWindowRect(&rect);
	CEasyBView* pView = pVIEW;
	if (!pView)	 // could fail for any number of reasons
		return;
	pVIEW->GetWindowRect(&parentRect);
	// position dialog just above south's cards, centered horizontally
	int nYPos, nXPos = parentRect.left + (parentRect.Width() - rect.Width()) / 2;
	POINT ptNorth = { 0, 0 }, ptSouth = { 0, 0 };
	ptNorth.y = pVIEW->GetValue(tpDrawPoint, NORTH, 1) + deck.GetCardHeight();
	ptSouth.y = pVIEW->GetValue(tpDrawPoint, SOUTH, 1);
	pVIEW->ClientToScreen(&ptNorth);
	pVIEW->ClientToScreen(&ptSouth);
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
		int newY = rect.top - dy;  // NCR make sure not less than 0
		if(newY < 0) newY = 0;
		MoveWindow(nXPos, /*rect.top-dy*/ newY, rect.Width(), rect.Height(), TRUE);
	}
}



//
BOOL CBidDialog::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
    UINT nID = pNMHDR->idFrom;
	int nIndex;
	//
    if (pTTT->uFlags & TTF_IDISHWND)
    {
        // idFrom is actually the HWND of the tool
        nID = ::GetDlgCtrlID((HWND)nID);
        if(nID)
        {
			// look up the internal ordinal of the control
			if (!m_mapIDtoIndex.Lookup(nID, nIndex))
				return FALSE;
            pTTT->lpszText = tToolTipInfo[nIndex].szToolTipText;
            pTTT->hinst = AfxGetResourceHandle();
            return TRUE;
        }
    }
    return FALSE;
}




//
// FlashWindow()
//
// used to temporarily flash a window's border
//
void CBidDialog::FlashWindow(CWnd* pWnd1, CWnd* pWnd2)
{
	if (pWnd1 == NULL)
		return;
	//
	CDC *pDC1 = pWnd1->GetDC(), *pDC2;
	CRect rect1, rect2;
    pWnd1->GetWindowRect(&rect1);
    rect1.OffsetRect(-rect1.left, -rect1.top);
	//
	BOOL bWindow2Enabled = TRUE;
	if (pWnd2)
	{
		pDC2 = pWnd2->GetDC();
		bWindow2Enabled = pWnd2->IsWindowEnabled();
	    pWnd2->GetWindowRect(&rect2);
	    rect2.OffsetRect(-rect2.left, -rect2.top);
	}

	//
	const int tnBorder = 2;
    if (!rect1.IsRectEmpty())
    {
		const int numTimes = 2;
		for(int i=0;i<numTimes;i++)
		{
			// highlight rect 1
			HighlightRect(pDC1, rect1);
			UpdateWindow();
			// highlight rect 2
			if (pWnd2)
			{
				pWnd2->EnableWindow(TRUE);
				HighlightRect(pDC2, rect2);
				UpdateWindow();
			}
	
			// wait
			Sleep(250);

			// de-hilight rect 1
			HighlightRect(pDC1, rect1);
			UpdateWindow();
			// de-hilight rect 2
			if (pWnd2)
			{
				HighlightRect(pDC2, rect2);
				pWnd2->EnableWindow(bWindow2Enabled);
				UpdateWindow();
			}

			// sleep again
			if (i < numTimes-1)
				Sleep(250);
		}
    }

	// done
	pWnd1->ReleaseDC(pDC1);
	if (pWnd2)
		pWnd2->ReleaseDC(pDC2);
}


//
void CBidDialog::HighlightRect(CDC* pDC, const CRect& rect)
{
	const int DINV = 4;
	const int tnBorder = 2;
	//
    pDC->PatBlt(rect.left, rect.top, rect.right - rect.left, DINV,  DSTINVERT);
    pDC->PatBlt(rect.left, rect.bottom - DINV, DINV, -(rect.bottom - rect.top - tnBorder * DINV), DSTINVERT);
    pDC->PatBlt(rect.right - DINV, rect.top + DINV, DINV, rect.bottom - rect.top - tnBorder * DINV, DSTINVERT);
    pDC->PatBlt(rect.right, rect.bottom - DINV, -(rect.right - rect.left), DINV, DSTINVERT);
}

