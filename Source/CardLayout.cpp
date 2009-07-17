//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// cdlayout.cpp : implementation file
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "mainfrm.h"
#include "EasyBvw.h"
#include "player.h"
#include "subclass.h"
#include "card.h"
#include "CardLayout.h"
#include "progopts.h"
#include "Help/HelpCode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern char* szLayoutDonePrompt;


/////////////////////////////////////////////////////////////////////////////
// CCardLayoutDialog dialog


CCardLayoutDialog::CCardLayoutDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCardLayoutDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCardLayoutDialog)
	m_nPlayer = -1;
	m_bUseSuitDisplayOrder = FALSE;
	//}}AFX_DATA_INIT
	m_nCardLimit = 0;
	m_nPlayer = SOUTH;
	m_bUseSuitDisplayOrder = theApp.GetValue(tbLayoutFollowsDisplayOrder);
}


void CCardLayoutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardLayoutDialog)
	DDX_Radio(pDX, IDC_RADIO1, m_nPlayer);
	DDX_Check(pDX, IDC_DISPLAY_ORDER, m_bUseSuitDisplayOrder);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardLayoutDialog, CDialog)
	//{{AFX_MSG_MAP(CCardLayoutDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_DISPLAY_ORDER, OnDisplayOrder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCardLayoutDialog message handlers




//
BOOL CCardLayoutDialog::OnInitDialog() 
{
	// gotta subclass all the controls
	int i;
 	CWnd* pWnd;

	// first the checkboxes
	for(i=IDC_CHECK1;i<=IDC_CHECK52;i++) 
	{
	 	pWnd = GetDlgItem(i);
		ASSERT(pWnd != NULL);
		wpOrigCheckBoxProc = (WNDPROC) SetWindowLong(pWnd->m_hWnd, GWL_WNDPROC, (LONG) CheckBoxSubclassProc);
	}

	// then the radiobuttons
	for(i=IDC_RADIO1;i<=IDC_RADIO4;i++) 
	{
	 	pWnd = GetDlgItem(i);
		ASSERT(pWnd != NULL);
		wpOrigRadioProc = (WNDPROC) SetWindowLong(pWnd->m_hWnd, GWL_WNDPROC, (LONG) RadioSubclassProc);
	}

	// then the pushbuttons
 	pWnd = GetDlgItem(IDOK);
	ASSERT(pWnd != NULL);
	wpOrigButtonProc = (WNDPROC) SetWindowLong(pWnd->m_hWnd, GWL_WNDPROC, (LONG) ButtonSubclassProc);
 	pWnd = GetDlgItem(IDC_CLEAR);
	ASSERT(pWnd != NULL);
	SetWindowLong(pWnd->m_hWnd, GWL_WNDPROC, (LONG) ButtonSubclassProc);

	//
	// housekeeping
	//
	if ((m_nCardLimit <= 0) || (m_nCardLimit > 13))
		m_nCardLimit = 13;
	//
	// done
	//
	CDialog::OnInitDialog();
	//
	if (pDOC != NULL)
		Reset();
	// 
//	CenterWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CCardLayoutDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	if (bShow)
	{
		CenterWindow();
		Reset();	
	}
	//
	CDialog::OnShowWindow(bShow, nStatus);
}


//
BOOL CCardLayoutDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int nID = LOWORD(wParam);
	int nCode = HIWORD(wParam);
	int nMessageType = wParam;
	CCard* pCard;
	int nScreenSuit,nSuit,nValue,nIndex;

	// process command
	if (wParam == WMS_SET_CARD_LIMIT)
	{
		m_nCardLimit = lParam;
		return TRUE;
	}
	//
	if ((nCode == BN_CLICKED) && (nID >= IDC_CHECK1) && (nID <= IDC_CHECK52)) 
	{
		int nOldCount = PLAYER(m_nPlayer).GetNumCards();
		//		
		nScreenSuit = (nID - IDC_CHECK1) / 13;
		if (m_bUseSuitDisplayOrder)
			nSuit = theApp.GetSuitSequence(nScreenSuit);
		else
			nSuit = SPADES - nScreenSuit;
		nValue = 12 - ((nID - IDC_CHECK1) % 13);
		nIndex = nSuit*13 + nValue;
		if (IsDlgButtonChecked(nID)) 
		{
			// take card from deck and place in player's hand
			pCard = pVIEW->m_layoutDeck[nIndex];
			if (pCard == NULL)
				return TRUE;
			if (pCard->IsCardAssigned())
				return TRUE;
			// assign card
			PLAYER(m_nPlayer).AddCardToHand(pCard);
			pVIEW->m_layoutDeck[nIndex] = NULL;			
			pVIEW->m_numCardsUnassigned--;
			pVIEW->DisplayHand((Position)m_nPlayer);
			EnableOrDisable();
			return TRUE;
		} 
		else 
		{
			// put player's card back in the deck
			pCard = PLAYER(m_nPlayer).GetCardByValue(nIndex);
			if (pCard == NULL)
				return TRUE;
			if (!pCard->IsCardAssigned())
				return TRUE;
			PLAYER(m_nPlayer).RemoveCardFromHand(pCard);
			pVIEW->m_layoutDeck[nIndex] = pCard;
			pVIEW->m_numCardsUnassigned++;
			pCard->SetOwner(UNKNOWN);   // NCR-OWN2POS
			pVIEW->DisplayHand((Position)m_nPlayer);
			EnableOrDisable();
			return TRUE;
		}
	} 
	else if ((nCode == BN_CLICKED) && (nID >= IDC_RADIO1) && (nID <= IDC_RADIO4)) 
	{
		m_nPlayer = nID - IDC_RADIO1;
		Reset();
	} 
	else if (nMessageType == WMS_RESET_DISPLAY) 
	{
		Reset();
	}
	//	
	return CDialog::OnCommand(wParam, lParam);
}



//
void CCardLayoutDialog::OnOK() 
{
	OnClose();
//	EndDialog(TRUE);
//	CDialog::OnOK();
}


//
void CCardLayoutDialog::Reset(BOOL bForceRefresh) 
{
	int i,nIndex;
	
	// first set labels
	for(i=0;i<4;i++) 
	{
		if (m_bUseSuitDisplayOrder)
			nIndex = theApp.GetSuitSequence(i);
		else
			nIndex = SPADES - i;
		SetDlgItemText(i+IDC_STATIC1,theApp.GetSuitName(nIndex));
	}

	// then clear all checks
	for(i=IDC_CHECK1;i<(IDC_CHECK1+52);i++)
		CheckDlgButton(i,0);
	
	// then set the player code
	CheckRadioButton(IDC_RADIO1,IDC_RADIO4,IDC_RADIO1+m_nPlayer);
	
	// and fill in the checkboxes
	int nCards = PLAYER(m_nPlayer).GetNumCards();
	int nSuit,nValue,nItem;
	CCard* pCard;
	for(i=0;i<nCards;i++) 
	{
		pCard = PLAYER(m_nPlayer).GetCardByPosition(i);	
		if (m_bUseSuitDisplayOrder)
			nSuit = pVIEW->m_nSuitToScreenIndex[pCard->GetSuit()];
		else
			nSuit = SPADES - pCard->GetSuit();
		nValue = 14 - pCard->GetFaceValue();
		nItem = IDC_CHECK1 + (nSuit*13) + nValue;
		CheckDlgButton(nItem, 1);
	}
	//
	EnableOrDisable(bForceRefresh);
}



//
void CCardLayoutDialog::ForceRefresh()
{
	// force a refresh
//	SendMessage(WM_COMMAND, IDC_RADIO1, 0L);
	Reset(TRUE);
	EnableOrDisable(TRUE);
}


//
void CCardLayoutDialog::OnDisplayOrder() 
{
	UpdateData(TRUE);
	theApp.SetValue(tbLayoutFollowsDisplayOrder, m_bUseSuitDisplayOrder);
	ForceRefresh();
}


//
void CCardLayoutDialog::EnableOrDisable(BOOL bForceReset) 
{
	int i,nSuit,nValue,nIndex;
	//
	if (!IsWindowVisible() && (!bForceReset))
		return;
	//
	int numCards = PLAYER(m_nPlayer).GetNumCards();
	CString strText;
	strText.Format("%d",numCards);
	SetDlgItemText(IDC_NUMCARDS,strText);
	PLAYER(m_nPlayer).CountCardPoints();
	strText.Format("%d",PLAYER(m_nPlayer).GetHCPoints());
	SetDlgItemText(IDC_CARD_POINTS,strText);
	strText.Format("%d",PLAYER(m_nPlayer).GetTotalPoints());
	SetDlgItemText(IDC_TOTAL_POINTS,strText);
/*
	if (PLAYER[m_nPlayer].m_numCards < m_nCardLimit) 
	{
		SetDlgItemText(IDC_TOTAL_POINTS,"");
	} 
	else 
	{
		strText.Format("%d",PLAYER[m_nPlayer].m_numTotalPoints);
		SetDlgItemText(IDC_TOTAL_POINTS,strText);
	}
*/
	if (numCards < m_nCardLimit) 
	{
		for(i=0;i<52;i++) 
		{
			if (m_bUseSuitDisplayOrder)
				nSuit = theApp.GetSuitSequence(i/13);
			else
				nSuit = SPADES - (i/13);
			//
			nValue = 12 - (i % 13);
			nIndex = nSuit*13 + nValue;
			// disable the assigned cards
			// i.e., those that are not checked and not available
			if ((!IsDlgButtonChecked(IDC_CHECK1+i)) && (pVIEW->m_layoutDeck[nIndex] == NULL))
				GetDlgItem(IDC_CHECK1+i)->EnableWindow(FALSE);
			else
				GetDlgItem(IDC_CHECK1+i)->EnableWindow(TRUE);
		}
	} 
	else 
	{
		// disable all unchecked boxes
		for(i=0;i<52;i++) 
		{
			if (!IsDlgButtonChecked(IDC_CHECK1+i))
				GetDlgItem(IDC_CHECK1+i)->EnableWindow(FALSE);
			else
				GetDlgItem(IDC_CHECK1+i)->EnableWindow(TRUE);
		}
	}
}


void CCardLayoutDialog::OnClear() 
{
/*
	if (AfxMessageBox("Clear all hands?",MB_ICONQUESTION|MB_OKCANCEL) == IDCANCEL)
		return;
	pVIEW->PrepareCardLayout();
	pVIEW->PostMessage(WM_COMMAND,WMS_UPDATE);
	Reset();
*/
	// clear the current hand
	if (AfxMessageBox(FormString("Clear %s's hand?", PositionToString(m_nPlayer)),
					  MB_ICONQUESTION|MB_OKCANCEL) == IDCANCEL)
		return;
	//
	int i,nIndex;
	CCard* pCard;
	int nCount = PLAYER(m_nPlayer).GetNumCards();
	for(i=0;i<nCount;i++) 
	{
		// return card to deck
		pCard = PLAYER(m_nPlayer).GetCardByPosition(0);
		if (pCard == NULL)
			return;
		PLAYER(m_nPlayer).RemoveCardFromHand(pCard);
		nIndex = pCard->GetSortedDeckIndex();
		pVIEW->m_layoutDeck[nIndex] = pCard;
		pVIEW->m_numCardsUnassigned++;
		pCard->SetOwner(UNKNOWN);  // NCR-OWN2POS
 		pCard->ClearAssignment();
		// clear checkbox
		// get screen order from suit
		int nSuit = pVIEW->GetSuitToScreenIndex(pCard->GetSuit());
		int nCheckIndex = nSuit*13 + (14 - pCard->GetFaceValue());
		CheckDlgButton(IDC_CHECK1+nCheckIndex,0);
	}
	pVIEW->DisplayHand((Position)m_nPlayer);
	EnableOrDisable();
	return;
}

void CCardLayoutDialog::OnClose() 
{
	CRect rect;
	GetWindowRect(&rect);
	//
	ShowWindow(SW_HIDE);
	UpdateWindow();
	if (pVIEW->GetCurrentMode() == CEasyBView::MODE_CARDLAYOUT)
	{
		// redraw the window
		pVIEW->UpdateWindow();
	}
	else
	{
		// just clear the area under the dialog
		pVIEW->ScreenToClient(&rect);
		pVIEW->ClearDisplayArea(&rect);
	}

	// supress flashing
	pVIEW->SuppressRefresh();
	pMAINFRAME->UpdateWindow();
	pVIEW->EnableRefresh();
	UpdateWindow();
	pVIEW->Notify(WM_COMMAND, WMS_LAYOUT_DONE);
/*
	pVIEW->SuppressRefresh();
	pMAINFRAME->UpdateWindow();
	pVIEW->Notify(WM_COMMAND, WMS_LAYOUT_DONE);
	pVIEW->EnableRefresh();
	UpdateWindow();
*/

	//
	pMAINFRAME->SetStatusMessage("Assign cards or press F9 to end Edit mode");
//	pMAINFRAME->SetStatusMessage("Drag and release cards to assign them to players");
//	EndDialog(TRUE);
//	CDialog::OnClose();

	//
	pVIEW->PostMessage(WM_COMMAND, WMS_LAYOUT_DONE);
}

void CCardLayoutDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	//	
/*
	if (IsIconic()) 
	{
		dc.SetBkMode(TRANSPARENT);
//		dc.DrawIcon(0, 0, m_hIcon);
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	} 
	else 
	{
		SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
*/
}



//
BOOL CCardLayoutDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	WinHelp(DIALOG_CARD_LAYOUT);
	return TRUE;
}


