//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// StatusCardLocationsPage.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "easybDoc.h"
#include "mainfrm.h"
#include "Player.h"
#include "StatusCardLocationsPage.h"
#include "GuessedHandHoldings.h"
#include "PlayEngine.h"
#include "HandHoldings.h"
#include "SuitHoldings.h"
#include "StatusSheet.h"
#include "subclass.h"
#include "docopts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusCardLocationsPage property page

IMPLEMENT_DYNCREATE(CStatusCardLocationsPage, CPropertyPage)

CStatusCardLocationsPage::CStatusCardLocationsPage() : CPropertyPage(CStatusCardLocationsPage::IDD)
{
	//{{AFX_DATA_INIT(CStatusCardLocationsPage)
	//}}AFX_DATA_INIT
	m_bInitialized = FALSE;
}

CStatusCardLocationsPage::~CStatusCardLocationsPage()
{
}

void CStatusCardLocationsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatusCardLocationsPage)
	DDX_Control(pDX, IDC_COMBO_TARGET, m_cbTarget);
	DDX_Control(pDX, IDC_COMBO_OBSERVER, m_cbObserver);
	DDX_Control(pDX, IDC_HOLDINGS, m_listHoldings);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatusCardLocationsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CStatusCardLocationsPage)
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_COMBO_OBSERVER, OnSelchangeComboObserver)
	ON_CBN_SELCHANGE(IDC_COMBO_TARGET, OnSelchangeComboTarget)
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusCardLocationsPage message handlers


//
static LPCTSTR tszColumnName[] = {
	"Cards Played", 
	"Cards left", 
	"Min left", 
	"Max left", 
	"Starting Cards", 
};


//
BOOL CStatusCardLocationsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	const int tnColSpacer = 14;    // NCR added int

	// init selections
	m_cbObserver.SetCurSel(0);
	m_cbTarget.SetCurSel(1);

	// init the list control
	m_listHoldings.SetImageList(&CStatusSheet::m_imageList, LVSIL_SMALL);
	m_listHoldings.InsertColumn(0, "", LVCFMT_LEFT, 16, 0);	// icon column

	// add columns
	int numCols = sizeof(tszColumnName) / sizeof(LPCTSTR);
	for(int i=0;i<numCols;i++)
		m_listHoldings.InsertColumn(i+1, tszColumnName[i], LVCFMT_LEFT, m_listHoldings.GetStringWidth(tszColumnName[i]) + tnColSpacer, i+1);

	// subclass controls
	wpOrigListCtrlProc = (WNDPROC) SetWindowLong(m_listHoldings.GetSafeHwnd(), GWL_WNDPROC, (LONG) ListCtrlSubclassProc);
	wpOrigRadioProc = (WNDPROC) SetWindowLong(GetDlgItem(IDC_OBSERVER_SOUTH)->GetSafeHwnd(), GWL_WNDPROC, (LONG) RadioSubclassProc);
	wpOrigRadioProc = (WNDPROC) SetWindowLong(GetDlgItem(IDC_OBSERVER_WEST)->GetSafeHwnd(), GWL_WNDPROC, (LONG) RadioSubclassProc);
	wpOrigRadioProc = (WNDPROC) SetWindowLong(GetDlgItem(IDC_OBSERVER_NORTH)->GetSafeHwnd(), GWL_WNDPROC, (LONG) RadioSubclassProc);
	wpOrigRadioProc = (WNDPROC) SetWindowLong(GetDlgItem(IDC_OBSERVER_EAST)->GetSafeHwnd(), GWL_WNDPROC, (LONG) RadioSubclassProc);
	wpOrigRadioProc = (WNDPROC) SetWindowLong(GetDlgItem(IDC_TARGET_SOUTH)->GetSafeHwnd(), GWL_WNDPROC, (LONG) RadioSubclassProc);
	wpOrigRadioProc = (WNDPROC) SetWindowLong(GetDlgItem(IDC_TARGET_WEST)->GetSafeHwnd(), GWL_WNDPROC, (LONG) RadioSubclassProc);
	wpOrigRadioProc = (WNDPROC) SetWindowLong(GetDlgItem(IDC_TARGET_NORTH)->GetSafeHwnd(), GWL_WNDPROC, (LONG) RadioSubclassProc);
	wpOrigRadioProc = (WNDPROC) SetWindowLong(GetDlgItem(IDC_TARGET_EAST)->GetSafeHwnd(), GWL_WNDPROC, (LONG) RadioSubclassProc);

	// record initial window characterisitcs
	CRect winRect, listRect;
	GetClientRect(&winRect);
	m_listHoldings.GetWindowRect(&listRect);
	ScreenToClient(&listRect);
	m_nInitialX = listRect.left;
	m_nInitialY = listRect.top;
	m_nInitialXMargin = listRect.left - winRect.left;
	m_nInitialYMargin = winRect.bottom - listRect.bottom;

	// done with the inits, so show data
	m_bInitialized = TRUE;
	Update();

	// all done
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
BOOL CStatusCardLocationsPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch(wParam)
	{
		case WMS_UPDATE_TOOLTIP_TEXT:
			return TRUE;
	}
	// call default
	return CPropertyPage::OnCommand(wParam, lParam);
}


//
void CStatusCardLocationsPage::OnSize(UINT nType, int cx, int cy) 
{
	if (!m_bInitialized)
		return;
	CPropertyPage::OnSize(nType, cx, cy);
	// resize the list control
	CRect rect;
	GetClientRect(&rect);
	//
	m_listHoldings.MoveWindow(m_nInitialX, m_nInitialY, 
							  rect.Width() - m_nInitialXMargin*2,
							  rect.Height() - m_nInitialY - m_nInitialYMargin);
}


//
void CStatusCardLocationsPage::OnDestroy() 
{
	// clean up
	m_listHoldings.DeleteAllItems();
	CPropertyPage::OnDestroy();
}


//
void CStatusCardLocationsPage::Clear()
{
	if (!m_bInitialized)
		return;
	m_listHoldings.DeleteAllItems();
	UpdateData(FALSE);
}


//
void CStatusCardLocationsPage::Update()
{
	// return if not all inits are complete
	if (!m_bInitialized)
		return;
	if ((pDOC == NULL) || !pDOC->GetValue(tbHandsDealt) || !theApp.IsGameInProgress())
		return;

	//
	UpdateData(TRUE);
	int nObserver = m_cbObserver.GetCurSel();
	int nTarget = m_cbTarget.GetCurSel();
	VERIFY(ISPLAYER(nObserver));
	VERIFY(ISPLAYER(nTarget));
	if ((pDOC->GetPlayer(nObserver) == NULL) ||
		(pDOC->GetPlayer(nTarget) == NULL))
		return;
	//
	m_listHoldings.DeleteAllItems();

	//
	CGuessedSuitHoldings playedCards, remainingCards, identifiedCards;
	CGuessedHandHoldings* pHand = pDOC->GetPlayer(nObserver)->GetGuessedHand(nTarget);
	CPlayEngine* pPlayEngine = pDOC->GetPlayer(nObserver)->GetPlayEngine();
	//
	if ((pHand == NULL) || (pPlayEngine == NULL))
		return;
	// see if this hand is dummy and exposed
	BOOL bDummy = FALSE;
	if ((nTarget == pDOC->GetDummyPosition()) && pDOC->IsDummyExposed())
		bDummy = TRUE;

	// and show the card info
	for(int nIndex=0;nIndex<4;nIndex++)
	{
		int nSuit = theApp.GetSuitSequence(nIndex);
		// first show the list of played cards
		m_listHoldings.InsertItem(nIndex, "", nSuit);
		// then set played cards field
		CGuessedSuitHoldings playedCards;
		int numPlayedCards = pHand->GetSuit(nSuit).GetPlayedCardsList(playedCards);
		if (numPlayedCards == 0)
			m_listHoldings.SetItem(nIndex, 1, LVIF_TEXT, 
								   FormString("(%d)",numPlayedCards),
								   0, 0, 0, 0L);
		else
			m_listHoldings.SetItem(nIndex, 1, LVIF_TEXT, 
								   FormString("(%d) %s", numPlayedCards, playedCards.GetHoldingsString()),
								   0, 0, 0, 0L);
		playedCards.Clear(FALSE);

		//
		// then show known information
		//
		if (!bDummy)
		{
			//
			// show the guessed card info
			//
			CGuessedSuitHoldings& suit = pHand->GetSuit(nSuit);

			// set cards left field
			if (suit.AreAllCardsIdentified())
			{
				int numRemainingCards = suit.GetRemainingCardsList(remainingCards);
				CString strText;
				if (numRemainingCards < 0)
					strText = "?";
				else if (numRemainingCards == 0)
					strText = FormString("(%d)", numRemainingCards);
				else
					strText = FormString("(%d) %s", numRemainingCards, remainingCards.GetHoldingsString());
				m_listHoldings.SetItem(nIndex, 2, LVIF_TEXT, strText, 0, 0, 0, 0L);
				remainingCards.Clear(FALSE);
			}
			else
			{
				m_listHoldings.SetItem(nIndex, 2, LVIF_TEXT, "?", 0, 0, 0, 0L);
			}

			// set min remaining cards field (deduced)
	//		int numMinimumCards = suit.GetNumMinimumCards();
			int numMinimumCards = pPlayEngine->GetMinCardsInSuit(nTarget, nSuit);
			m_listHoldings.SetItem(nIndex, 3, LVIF_TEXT, 
								   ((numMinimumCards >= 0)? (FormString("(%d)", numMinimumCards)) : "?"),
								   0, 0, 0, 0L);

			// set max remaining cards field (deduced)
	//		int numMaximumCards = suit.GetNumMaximumCards();
			int numMaximumCards = pPlayEngine->GetMaxCardsInSuit(nTarget, nSuit);
			m_listHoldings.SetItem(nIndex, 4, LVIF_TEXT, 
								   ((numMaximumCards >= 0)? (FormString("(%d)", numMaximumCards)) : "?"),
								   0, 0, 0, 0L);

			// set starting cards field, if known
			if (suit.AreAllCardsIdentified())
			{
				int numOriginalCards = suit.GetIdentifiedCardsList(identifiedCards);
				m_listHoldings.SetItem(nIndex, 5, LVIF_TEXT, 
									   (FormString("(%d) %s", numOriginalCards, identifiedCards.GetHoldingsString())),
									   0, 0, 0, 0L);
				identifiedCards.Clear(FALSE);
			}
			else
			{
				m_listHoldings.SetItem(nIndex, 5, LVIF_TEXT, "?", 0, 0, 0, 0L);
			}
		}
		else
		{
			//
			// this hand is dummy (and exposed), so the 
			// holdings are known with certainty
			//
			// set cards left field
			CHandHoldings& hand = pDOC->GetDummyPlayer()->GetHand();
			CSuitHoldings& suit = hand.GetSuit(nSuit);
			CString strText = FormString("(%d) %s", suit.GetNumCards(), suit.GetHoldingsString());
			m_listHoldings.SetItem(nIndex, 2, LVIF_TEXT, strText, 0, 0, 0, 0L);

			// set min & max cards field (both known)
			int numMinimumCards = suit.GetNumCards();
			m_listHoldings.SetItem(nIndex, 3, LVIF_TEXT, 
								   ((numMinimumCards >= 0)? (FormString("(%d)", numMinimumCards)) : "?"),
								   0, 0, 0, 0L);
			int numMaximumCards = suit.GetNumCards();
			m_listHoldings.SetItem(nIndex, 4, LVIF_TEXT, 
								   ((numMaximumCards >= 0)? (FormString("(%d)", numMaximumCards)) : "?"),
								   0, 0, 0, 0L);

			// set starting cards field (also known)
			CCardHoldings& initialHand = hand.GetInitialHand();
			CSuitHoldings initialSuit;
			initialHand.GetCardsOfSuit(nSuit, &initialSuit);
			int numOriginalCards = initialSuit.GetNumCards();
			m_listHoldings.SetItem(nIndex, 5, LVIF_TEXT, 
								   (FormString("(%d) %s", numOriginalCards, initialSuit.GetHoldingsString())),
								   0, 0, 0, 0L);
		}
	}
}



//
// OnKeyDown()
//
// pass along to the parent
//
void CStatusCardLocationsPage::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//	CPropertyPage::OnKeyDown(nChar, nRepCnt, nFlags);
//	GetParent()->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
	pMAINFRAME->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
}


//
void CStatusCardLocationsPage::OnSelchangeComboObserver() 
{
	Update();	
}

//
void CStatusCardLocationsPage::OnSelchangeComboTarget() 
{
	Update();	
}


//
void CStatusCardLocationsPage::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// we don't do anything special here, so send it up to the parent
	ClientToScreen(&point);
	CWnd* pParent = GetParent();
	pParent->ScreenToClient(&point);
	pParent->SendMessage(WM_RBUTTONDOWN, nFlags, MAKELONG(point.x, point.y));
}
