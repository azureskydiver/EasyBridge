//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// BidDlgSmall.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "easybDoc.h"
#include "BidDlgSmall.h"
#include "progopts.h"
#include "Player.h"
#include "subclass.h"
#include "Help\Helpcode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBidDialogSmall dialog


CBidDialogSmall::CBidDialogSmall(CWnd* pParent /*=NULL*/)
	: CBidDialog(CBidDialogSmall::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBidDialogSmall)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBidDialogSmall::DoDataExchange(CDataExchange* pDX)
{
	CBidDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBidDialogSmall)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBidDialogSmall, CBidDialog)
	//{{AFX_MSG_MAP(CBidDialogSmall)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_SHOWWINDOW()
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBidDialogSmall message handlers


//
int CBidDialogSmall::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBidDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// load level button icons
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<7;i++)
		m_buttonLevelIcons[i] = (HICON) LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDI_LEVEL_1)+i, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	
	// load suit button icons
	for(i=0;i<5;i++)
		m_buttonSuitIcons[i] = (HICON) LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDI_SUIT_CLUB)+i, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	
	// done
	return 0;
}


//
void CBidDialogSmall::OnDestroy() 
{
	CBidDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}


//
BOOL CBidDialogSmall::OnInitDialog() 
{
	CBidDialog::OnInitDialog();

/*
	// subclass the bid level buttons
	for(int i=IDC_BID_LEVEL_1;i<=IDC_BID_LEVEL_7;i++) 
	{
	 	CWnd* pWnd = GetDlgItem(i);
		ASSERT(pWnd != NULL);
		SetWindowLong(pWnd->m_hWnd, GWL_WNDPROC, (LONG) ButtonSubclassProc);
	}
	// subclass the suit buttons
	for(i=IDC_SUIT_CLUBS;i<=IDC_SUIT_NOTRUMP;i++) 
	{
	 	CWnd* pWnd = GetDlgItem(i);
		ASSERT(pWnd != NULL);
		SetWindowLong(pWnd->m_hWnd, GWL_WNDPROC, (LONG) ButtonSubclassProc);
	}
*/

	// subclass the bid level buttons
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<7;i++)
	{
		m_flatLevelButtons[i].SubclassDlgItem(IDC_BID_LEVEL_1 + i, this);
		m_flatLevelButtons[i].SetIcon(IDI_LEVEL_1 + i);
		m_flatLevelButtons[i].SetShowText(FALSE);
	}

	// subclass the bid suit buttons
	for(i=0;i<5;i++)
	{
		m_flatSuitButtons[i].SubclassDlgItem(IDC_SUIT_CLUBS + i, this);
		m_flatSuitButtons[i].SetIcon(IDI_SUIT_CLUB + i);
		m_flatSuitButtons[i].SetShowText(FALSE);
	}

	// done
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



//
void CBidDialogSmall::OnHelp() 
{
	WinHelp(DIALOG_BIDDING_SMALL);
}


//
BOOL CBidDialogSmall::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}


//
void CBidDialogSmall::InitBiddingSequence()
{
	m_nSelectedLevel = 0;
	// 
	CBidDialog::InitBiddingSequence();
}


//
void CBidDialogSmall::EnableButtonImages(BOOL bEnable)
{
	if (bEnable)
	{
/*
		// set level button icons
		for(int i=0;i<7;i++)
		{
			CButton* pButton = (CButton*) GetDlgItem(IDC_BID_LEVEL_1+i);
			pButton->ModifyStyle(NULL, BS_ICON);
			pButton->SetIcon(m_buttonLevelIcons[i]);
		}
		// set suit button icons
		for(i=0;i<5;i++)
		{
			CButton* pButton = (CButton*) GetDlgItem(IDC_SUIT_CLUBS+i);
			pButton->ModifyStyle(NULL, BS_ICON);
			pButton->SetIcon(m_buttonSuitIcons[i]);
		}
*/
	}
	//
	Invalidate();
}



//
// OnCommand()
//
BOOL CBidDialogSmall::OnCommand(WPARAM wParam, LPARAM lParam) 
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

	// check for bid level press
	if ((nCode == BN_CLICKED) && (nID >= IDC_BID_LEVEL_1) && (nID <= IDC_BID_LEVEL_7))
	{
		// check current mode
		if ((m_nCurrMode != BD_MODE_WAITFORBID) && (m_nCurrMode != BD_MODE_MANUAL_BID))
			return TRUE;
		//
		m_nSelectedLevel = nID - IDC_BID_LEVEL_1 + 1;

		// enable/disable buttons
		EnableControls();
		// highlight the level
		((CButton*)GetDlgItem(nID))->SetState(TRUE);
		//
		DisableControls();
		//
		return TRUE;
	}

	// check for bid suit press
	if ((nCode == BN_CLICKED) && (nID >= IDC_SUIT_CLUBS) && (nID <= IDC_SUIT_NOTRUMP) )
	{
		// check current mode
		if ((m_nCurrMode != BD_MODE_WAITFORBID) && (m_nCurrMode != BD_MODE_MANUAL_BID))
			return TRUE;
		//
		int nSelectedSuit = nID - IDC_SUIT_CLUBS;
		int nBid = MAKEBID(nSelectedSuit, m_nSelectedLevel);
		if (m_nCurrMode == BD_MODE_MANUAL_BID)
		{
			// record the bid
			int nPos = pDOC->GetCurrentPlayerPosition();
			if (nPos == SOUTH)
				pDOC->GetCurrentPlayer()->EnterHumanBid(nBid);
			int nCode = pDOC->EnterBid(nPos, nBid);
			UpdateBidDisplay(nPos, nBid);	

			// and check the result of the bid
			ProcessBidReturnCode(nCode);
			if (nCode == 0)
			{
				// bid entered OK, move on to the next player
				int nPos = pDOC->GetCurrentPlayerPosition();
				GetComputerBids(nPos);
			}
		}
		else
		{
			RegisterBid(nBid);
		}
		return TRUE;
	}

	// check for double/redouble
	if ( (nID == IDC_BID_DOUBLE) || (nID == IDC_BID_REDOUBLE) ) 
	{
		if (nID == IDC_BID_DOUBLE)
			nBid = BID_DOUBLE;
		else if (nID == IDC_BID_REDOUBLE)
			nBid = BID_REDOUBLE;
		else
			nBid = nID - IDC_BID_PASS;

		//
		RegisterBid(nBid);
		return TRUE;
	}
	//
	return CBidDialog::OnCommand(wParam, lParam);
}



//
// RegisterBid()
//
// called from OnCommand() above or from the outside to force a bid
//
void CBidDialogSmall::RegisterBid(int nBid, BOOL bShowButtonPress)
{
	// simualte button press if desired
	if (bShowButtonPress)
		PressBidButton(nBid);

	// enter the bid
	int nStatus = EnterPlayerBid(nBid);

	// see what the result of the bid was
/*
	if (nStatus == 1)
	{
		// bidding is finished
		BiddingDone();
	}
	else if (nStatus == 0)
*/
	if (nStatus == 0)
	{
		// received player's bid; get computer players' bids
		nStatus = GetComputerBids(WEST);
	}

	// reset suit selection
	m_nSelectedLevel = 0;
	EnableControls();

	// and set focus on the "Pass" button
	if (nStatus == 0)
		GetDlgItem(IDC_BID_PASS)->SetFocus();
}



//
void CBidDialogSmall::EnableControls()
{
	// enable all controls
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=IDC_BID_LEVEL_1;i<=IDC_BID_LEVEL_7;i++)
	{
		CButton* pButton = (CButton*) GetDlgItem(i);
		pButton->EnableWindow(TRUE);
		pButton->SetState(FALSE);
	}

	for(i=IDC_SUIT_CLUBS;i<=IDC_SUIT_NOTRUMP;i++)
	{
		CButton* pButton = (CButton*) GetDlgItem(i);
		pButton->EnableWindow(TRUE);
		pButton->SetState(FALSE);
	}
/*
	// enable the level & suit buttons
	int nLastBid = pDOC->GetLastValidBid();
	if (nLastBid == BID_PASS)
	{
	}
	//	
	int nLevel = BID_LEVEL(nLastBid);
	int nSuit = BID_SUIT(nLastBid);

	// enable all suits higher than the one bid
	for(int i=0;i<nSuit;i++)
		GetDlgItem(IDC_SUIT_CLUBS+i)->EnableWindow(TRUE);
*/
}


//
void CBidDialogSmall::DisableControls()
{
	int nLastBid = pDOC->GetLastValidBid();
	int nLastBidLevel = BID_LEVEL(nLastBid);
	int nLastBidSuit = BID_SUIT(nLastBid);

	// check if a bid level has been specified
	if (m_nSelectedLevel == 0)
	{
		// if no bid level has been selected yet, disable the suits
		for(int i=IDC_SUIT_CLUBS;i<=IDC_SUIT_NOTRUMP;i++)
			GetDlgItem(i)->EnableWindow(FALSE);

		// and disable all lower bid levels
		if (nLastBidSuit == NOTRUMP)
		{
			// disble up to & including current level
			for(int i=IDC_BID_LEVEL_1;i<IDC_BID_LEVEL_1+nLastBidLevel;i++)
				GetDlgItem(i)->EnableWindow(FALSE);
		}
		else
		{
			// disble up to prev level
			for(int i=IDC_BID_LEVEL_1;i<IDC_BID_LEVEL_1+nLastBidLevel-1;i++)
				GetDlgItem(i)->EnableWindow(FALSE);
		}
	}
	else
	{
		// a bid level has been specified, so disable certain suits
		if ((m_nSelectedLevel == nLastBidLevel) && (nLastBidSuit != NOTRUMP))
		{
			for(int i=IDC_SUIT_CLUBS;i<=IDC_SUIT_CLUBS+nLastBidSuit;i++)
				GetDlgItem(i)->EnableWindow(FALSE);
		}
	}

	// set defaults for double/redouble buttons
	BOOL bEnableDouble = TRUE;
	BOOL bEnableReDouble = FALSE;

	// can't double if no valid bids yet
	if (nLastBid <= BID_PASS)
		bEnableDouble = FALSE;

	// or if already doubled or redoubled
	if (pDOC->IsContractDoubled() || pDOC->IsContractRedoubled())
		bEnableDouble = FALSE;

	// can't double own team's bid
	int nBidTeam = pDOC->GetLastValidBidTeam();
	if ((nBidTeam == pDOC->GetCurrentPlayer()->GetTeam()) && !m_bTrainingMode)
		bEnableDouble = FALSE;

	// can redouble only if already doubled && is team contract (or is training)
	if (pDOC->IsContractDoubled() && ((nBidTeam == NORTH_SOUTH) || m_bTrainingMode))
		bEnableReDouble = TRUE;

	// and set
	GetDlgItem(IDC_BID_DOUBLE)->EnableWindow(bEnableDouble);
	GetDlgItem(IDC_BID_REDOUBLE)->EnableWindow(bEnableReDouble);
}



//
// PressBidButton()
//
// used to make a button it appear pressed
//
void CBidDialogSmall::PressBidButton(int nBid)
{
	//
	CButton *pButton1 = NULL, *pButton2 = NULL;
	//
	if (ISBID(nBid))
	{
		int nLevel = BID_LEVEL(nBid);
		int nSuit = BID_SUIT(nBid);
		pButton1 = (CButton*) GetDlgItem(IDC_BID_LEVEL_1 + nLevel - 1);
		pButton2 = (CButton*) GetDlgItem(IDC_SUIT_CLUBS + nSuit);
	}
	else if (nBid == BID_PASS)
		pButton1 = (CButton*) GetDlgItem(IDC_BID_PASS);
	else if (nBid == BID_DOUBLE)
		pButton1 = (CButton*) GetDlgItem(IDC_BID_DOUBLE);
	else if (nBid == BID_REDOUBLE)
		pButton1 = (CButton*) GetDlgItem(IDC_BID_REDOUBLE);
	//
	if (pButton1 == NULL)
		return;

	//
	BOOL bWindow2Enabled = pButton2? pButton2->IsWindowEnabled() : FALSE;
	//
	pButton1->SetState(TRUE);
	if (pButton2)
	{
		pButton2->EnableWindow(TRUE);
		pButton2->SetState(TRUE);
	}
	//
	Sleep(300);
	//
	pButton1->SetState(FALSE);
	if (pButton2)
	{
		pButton2->SetState(FALSE);
		pButton2->EnableWindow(bWindow2Enabled);
	}

	// done
	return;
}



//
// FlashButton()
//
// used to temporarily flash a button's border
//
void CBidDialogSmall::FlashButton(int nBid)
{
	//
	CWnd *pWnd1 = NULL, *pWnd2 = NULL;
	//
	if (ISBID(nBid))
	{
		int nLevel = BID_LEVEL(nBid);
		int nSuit = BID_SUIT(nBid);
		pWnd1 = GetDlgItem(IDC_BID_LEVEL_1 + nLevel - 1);
		pWnd2 = GetDlgItem(IDC_SUIT_CLUBS + nSuit);
	}
	else if (nBid == BID_PASS)
		pWnd1 = GetDlgItem(IDC_BID_PASS);
	else if (nBid == BID_DOUBLE)
		pWnd1 = GetDlgItem(IDC_BID_DOUBLE);
	else if (nBid == BID_REDOUBLE)
		pWnd1 = GetDlgItem(IDC_BID_REDOUBLE);
	//
	if (pWnd1 == NULL)
		return;

	//
	FlashWindow(pWnd1, pWnd2);
}




//
// UpdateBidDisplay
//
void CBidDialogSmall::UpdateBidDisplay(int nPos, int nBid, BOOL bPrompt, BOOL bTentative)
{
	CBidDialog::UpdateBidDisplay(nPos, nBid, bPrompt, bTentative);
}

