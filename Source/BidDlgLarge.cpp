//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// BidDlgLarge.cpp : implementation file
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "mainfrm.h"
#include "EasyBvw.h"
#include "progopts.h"
#include "viewopts.h"
#include "Player.h"
#include "BidDlgLarge.h"
#include "Deck.h"
#include "subclass.h"
#include "Help\Helpcode.h"



#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CBidDialogLarge dialog


CBidDialogLarge::CBidDialogLarge(CWnd* pParent /*=NULL*/)
	: CBidDialog(CBidDialogLarge::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBidDialogLarge)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBidDialogLarge::DoDataExchange(CDataExchange* pDX)
{
	CBidDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBidDialogLarge)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBidDialogLarge, CBidDialog)
	//{{AFX_MSG_MAP(CBidDialogLarge)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_AUTO_TRAIN, OnAutotrain)
	//}}AFX_MSG_MAP
//	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
//
// CBidDialogLarge message handlers
//
/////////////////////////////////////////////////////////////////////////////


//
int CBidDialogLarge::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBidDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	// 
	return 0;
}


//
void CBidDialogLarge::OnDestroy() 
{
	CBidDialog::OnDestroy();
}


//
BOOL CBidDialogLarge::OnInitDialog() 
{
	CBidDialog::OnInitDialog();

/*
	// subclass the bid buttons
	for(int i=IDC_BID_1C;i<=IDC_BID_7NT;i++) 
	{
	 	CWnd* pWnd = GetDlgItem(i);
		ASSERT(pWnd != NULL);
		SetWindowLong(pWnd->m_hWnd, GWL_WNDPROC, (LONG) ButtonSubclassProc);
	}
*/

	// subclass and set icons
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<35;i++)
	{
		m_flatButtons[i].SubclassDlgItem(IDC_BID_1C + i, this);
		m_flatButtons[i].SetIcon(IDI_CONTRACT_1CLUB + i);
		m_flatButtons[i].SetShowText(FALSE);
	}

	// need this to prevent bid icon flickering the first time
	for(i=IDCI_BID_SOUTH;i<IDCI_BID_SOUTH+4;i++)
	{
		GetDlgItem(i)->ShowWindow(FALSE);
		GetDlgItem(i)->UpdateWindow();
	}

	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}





//
void CBidDialogLarge::OnHelp() 
{
	WinHelp(DIALOG_BIDDING);
}


//
BOOL CBidDialogLarge::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}


//
void CBidDialogLarge::EnableButtonImages(BOOL bEnable)
{
	if (bEnable)
	{
		// set button icons
		for(int i=0;i<5;i++)
		{
/*
			for(j=0;j<7;j++)
			{
				CButton* pButton = (CButton*) GetDlgItem(IDC_BID_1C + (j*5) + i);
				pButton->ModifyStyle(NULL, BS_ICON);
				pButton->SetIcon(m_buttonIcons[i][j]);
			}
*/
		}
	}
	else
	{
/*		
		for(int i=0;i<5;i++)
			for(int j=0;j<7;j++)
				((CButton*) GetDlgItem(IDC_BID_1C + (j*5) + i))->ModifyStyle(BS_ICON, NULL);
*/
	}
	//
	Invalidate();
}



//
//
//
BOOL CBidDialogLarge::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int nID = LOWORD(wParam);
	int nCode = HIWORD(wParam);
	int nMessageType = wParam;
//	int nBid;
/*
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
		if ((m_nCurrMode != BD_MODE_WAITFORBID) && (m_nCurrMode != BD_MODE_TRAIN))
			return TRUE;
		//
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
*/
	//
	return CBidDialog::OnCommand(wParam, lParam);
}



//
// RegisterBid()
//
// called from OnCommand() above or from the outside to force a bid
//
void CBidDialogLarge::RegisterBid(int nBid, BOOL bShowButtonPress)
{
	// simulate button press if desired
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
		nStatus = GetComputerBids(pDOC->GetCurrentPlayerPosition());
	}

	// set focus on the "Pass" button
	if (nStatus == 0)
		GetDlgItem(IDC_BID_PASS)->SetFocus();
}




//
void CBidDialogLarge::EnableControls()
{
	int i;
	for(i=IDC_BID_1C;i<=IDC_BID_7NT;i++)
		GetDlgItem(i)->EnableWindow(TRUE);
}


//
void CBidDialogLarge::DisableControls()
{
	int nLastValidBid = pDOC->GetLastValidBid();
	if (nLastValidBid > BID_PASS)
	{
		int nStart = IDC_BID_1C;
		int nEnd = nStart + nLastValidBid - 1;
		if (nEnd < nStart)
			nEnd = nStart;
		if (nEnd > IDC_BID_7NT)
			nEnd = IDC_BID_7NT;
		for(int i=nStart;i<=nEnd;i++)
			GetDlgItem(i)->EnableWindow(FALSE);
	}

	// set defaults for double/redouble buttons
	BOOL bEnableDouble = TRUE;
	BOOL bEnableReDouble = FALSE;

	// can't double if no valid bids yet
	if (nLastValidBid <= BID_PASS)
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
void CBidDialogLarge::PressBidButton(int nBid)
{
	//
	CButton* pButton = NULL;
	//
	if (ISBID(nBid))
		pButton = (CButton*) GetDlgItem(IDC_BID_1C + nBid - 1);
	else if (nBid == BID_PASS)
		pButton = (CButton*) GetDlgItem(IDC_BID_PASS);
	else if (nBid == BID_DOUBLE)
		pButton = (CButton*) GetDlgItem(IDC_BID_DOUBLE);
	else if (nBid == BID_REDOUBLE)
		pButton = (CButton*) GetDlgItem(IDC_BID_REDOUBLE);
	//
	if (pButton == NULL)
		return;

	//
	pButton->SetState(TRUE);
	Sleep(300);
	pButton->SetState(FALSE);

	// done
	return;
}



//
// FlashButton()
//
// used to temporarily flash a button's border
//
void CBidDialogLarge::FlashButton(int nBid)
{
	//
	CWnd* pWnd = NULL;
	//
	if (ISBID(nBid))
		pWnd = GetDlgItem(IDC_BID_1C + nBid - 1);
	else if (nBid == BID_PASS)
		pWnd = GetDlgItem(IDC_BID_PASS);
	else if (nBid == BID_DOUBLE)
		pWnd = GetDlgItem(IDC_BID_DOUBLE);
	else if (nBid == BID_REDOUBLE)
		pWnd = GetDlgItem(IDC_BID_REDOUBLE);
	//
	FlashWindow(pWnd);
}





//
// UpdateBidDisplay
//
void CBidDialogLarge::UpdateBidDisplay(int nPos, int nBid, BOOL bPrompt, BOOL bTentative)
{
	CBidDialog::UpdateBidDisplay(nPos, nBid, bPrompt, bTentative);
}