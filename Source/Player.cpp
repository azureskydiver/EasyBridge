//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//===========================================================
//
// CPlayer
//
//
#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "EasyBvw.h"
#include "mainfrm.h"
#include "Card.h"
#include "progopts.h"
#include "playeropts.h"
#include "bidopts.h"
#include "bidparams.h"
#include "HandHoldings.h"
#include "CombinedHoldings.h"
#include "DeclarerPlayEngine.h"
#include "DummyPlayEngine.h"
#include "DefenderPlayEngine.h"
#include "BidEngine.h"
#include "PlayEngine.h"
#include "CardLocation.h"
#include "GuessedHandHoldings.h"
#include "player.h"
#include "handopts.h"
#include "MainFrameOpts.h"
#include "PlayerStatusDialog.h"
#include "AutoHintDialog.h"


extern int nSuitDisplaySequence[];


// constructor
CPlayer::CPlayer()
{
	// allocate memory for member objects
	m_pHand = new CHandHoldings;
	m_pBidder = new CBidEngine;
	m_pDeclarer = new CDeclarerPlayEngine;
	m_pDummy = new CDummyPlayEngine;
	m_pDefender = new CDefenderPlayEngine;
	m_pCardLocation = new CCardLocation;
	m_pStatusDlg = new CPlayerStatusDialog;
	// 
	for(int i=0;i<4;i++)
		m_pGuessedHands[i] = new CGuessedHandHoldings;
}

// destructor
CPlayer::~CPlayer()
{
	// deallocate memory
	delete m_pHand;
	delete m_pBidder;
	delete m_pDeclarer;
	delete m_pDummy;
	delete m_pDefender;
	delete m_pCardLocation;
	delete m_pStatusDlg;
	// 
	for(int i=0;i<4;i++)
	{
		m_pGuessedHands[i]->Clear(TRUE);
		delete m_pGuessedHands[i];
	}
}


//
//=========================================================================
//
// simple routines (would be inlined if not for...)
//

//
// Hand-related routines
// (mostly delegated to the m_pHand object)
//
void CPlayer::RestoreInitialHand() 
	{ m_pHand->RestoreInitialHand(); }

void CPlayer::AddCardToHand(CCard* pCard, BOOL bSort) 
	{ m_pHand->Add(pCard, bSort); }

void CPlayer::AddCardToInitialHand(CCard* pCard, BOOL bSort) 
	{ m_pHand->AddToInitialHand(pCard, bSort); }

void CPlayer::RemoveCardFromHand(CCard* pCard) 
	{ 
		m_pHand->Remove(pCard); 
	}

CCard* CPlayer::RemoveCardFromHand(int nIndex) 
	{ 
		CCard* pCard = m_pHand->RemoveByIndex(nIndex); 
		return pCard;
	}

void CPlayer::RemoveCardFromInitialHand(CCard* pCard) 
	{ 
		m_pHand->GetInitialHand().Remove(pCard); 
	}

CCard* CPlayer::RemoveCardFromInitialHand(int nIndex) 
	{ 
		CCard* pCard = m_pHand->GetInitialHand().RemoveByIndex(nIndex); 
		return pCard;
	}

// remove all cards withtout clearing the player's internal state
void CPlayer::RemoveAllCardsFromHand() 
	{ m_pHand->ClearHand(FALSE); }

void CPlayer::SetCard(int nIndex, CCard* pCard) 
	{ m_pHand->SetCard(nIndex, pCard); }

void CPlayer::SetInitialHandCard(int nIndex, CCard* pCard) 
	{ m_pHand->SetInitialHandCard(nIndex, pCard); }

void CPlayer::SortHand() 
	{ m_pHand->Sort(); }

void CPlayer::InitialEvaluateHand() 
	{ m_pHand->EvaluateHoldings(); }

void CPlayer::CountCards() 
	{ m_pHand->CountCards(); }

double CPlayer::CountCardPoints(BOOL bForceCount) 
	{ return m_pHand->CountPoints(bForceCount); }

int	CPlayer::GetNumCards() const 
	{ return m_pHand->GetNumCards(); }

int CPlayer::GetNumCardsInSuit(int nSuit) const 
	{ return m_pHand->GetSuitLength(nSuit); }

int CPlayer::GetSuitLength(int nSuit) const 
	{ return GetNumCardsInSuit(nSuit); }

BOOL CPlayer::HasCard(const CCard* pCard) const
	{ return m_pHand->HasCard(pCard); }

BOOL CPlayer::HasCard(int nDeckValue) const
	{ return m_pHand->HasCard(nDeckValue); }

CCard* CPlayer::GetCardByValue(int nVal) const 
	{ return m_pHand->GetCardByValue(nVal); }

CCard* CPlayer::GetCardByPosition(int nIndex) const 
	{ return m_pHand->GetAt(nIndex); }

CCard* CPlayer::GetCardInSuit(int nSuit, int nIndex) const 
	{ return m_pHand->GetCardInSuit(nSuit,nIndex); }

CCard* CPlayer::GetDisplayedCard(int nIndex) const 
	{ return m_pHand->GetDisplayedCard(nIndex); }

CCard* CPlayer::GetInitialHandCard(int nIndex) const 
	{ return m_pHand->GetInitialHandCard(nIndex); }

BOOL CPlayer::AreCardsExposed() const 
	{ return m_pHand->AreCardsExposed(); }

int CPlayer::GetNumCardsOf(int nValue) const 
	{	if (nValue == ACE)
			return m_pHand->GetNumAces();
		else if (nValue == KING)
			return m_pHand->GetNumKings();
		else 
			return 0; 
	}

BOOL CPlayer::SuitHasCard(int nSuit, int nCardFaceValue) const 
	{ return m_pHand->SuitHasCard(nSuit, nCardFaceValue); }

double CPlayer::GetHCPoints() const 
	{ return m_pHand->GetHCPoints(); }

double CPlayer::GetTotalPoints() const 
	{ return m_pHand->GetTotalPoints(); }

int CPlayer::GetBalanceValue() const 
	{ return m_pHand->GetBalanceValue(); }

double CPlayer::GetNumQuickTricks() const 
	{ return m_pHand->GetNumQuickTricks(); }

int CPlayer::GetNumSuitsStopped() const 
	{ return m_pHand->GetNumSuitsStopped(); }

//
// Bidding-related routines
// (mostly delegated to the m_bidder object)
//
int CPlayer::GetBiddingHint(BOOL bAutoHintMode) 
{ 
//	m_pStatusDlg->SetAutoHintMode(bAutoHintMode);
	if (!theApp.GetValue(tbEnableAnalysisDuringHints) && !bAutoHintMode)
		SuspendTrace();
	//
	pVIEW->SetCurrentModeTemp(CEasyBView::MODE_THINKING);
	m_pStatusDlg->ClearHints();
	m_pStatusDlg->BeginHintBlock();
	int nBiddingHint = m_pBidder->GetBiddingHint(); 
	m_pStatusDlg->EndHintBlock();
	pVIEW->RestoreMode();
	//
	if (!theApp.GetValue(tbEnableAnalysisDuringHints) && !bAutoHintMode)
		ResumeTrace();
//	if (bAutoHintMode)
//		m_pStatusDlg->SetAutoHintMode(FALSE);
	//
	return nBiddingHint;
}

BOOL CPlayer::IsGameBid(int nBid) 
	{ return m_pBidder->IsGameBid(nBid); };

int CPlayer::GetGameBid(int nSuit) 
	{ return m_pBidder->GetGameBid(nSuit); };

void CPlayer::RecordBid(int nPos, int nBid) 
	{ m_pBidder->RecordBid(nPos, nBid); }

int CPlayer::InquireLastBid() 
	{ return m_pBidder->m_nBid; }

int CPlayer::GetNumBidsMade() const  // NCR added const
	{ return m_pBidder->GetNumBidsMade(); }

int CPlayer::GetNumBidTurns() 
	{ return m_pBidder->GetNumBidTurns(); }

BOOL CPlayer::OpenedBiddingForTeam() 
	{ return m_pBidder->OpenedBiddingForTeam(); }

int CPlayer::GetPriorSuit(int nIndex) 
	{ return BID_SUIT(GetPriorBid(nIndex)); }

void CPlayer::ClearConventionStatus(CConvention* pConvention)
	{ m_pBidder->ClearConventionStatus(pConvention); }





//
// play-related routines
// (mostly delegated to the m_playEngine object)
//
void CPlayer::SetDummyFlag(BOOL bValue) 
	{ 
	m_bDummy = bValue; 
	m_pHand->SetDummyFlag(bValue);
	}

CCard* CPlayer::PlayCard() 
	{ 
#ifndef _DEBUG
		 return m_pPlayEngine->PlayCard(); 
#else
		// do a sanity check
		CCard* pPlayCard = m_pPlayEngine->PlayCard();
		if (pDOC->GetNumCardsPlayedInRound() > 0)
		{
			CCard* pLeadCard = pDOC->GetCurrentTrickCardLed();
			if (pPlayCard->GetSuit() != pLeadCard->GetSuit())
			{
				if (m_pHand->GetNumCardsInSuit(pLeadCard->GetSuit()) > 0)
				{
					ASSERT(FALSE);
				}
			}
		}
		//
		return pPlayCard;
#endif
	}

CCard* CPlayer::PlayForDummy() 
	{ return m_pDeclarer->PlayForDummy(); }

void CPlayer::RecordCardPlay(int nPos, CCard* pCard) 
	{ m_pPlayEngine->RecordCardPlay(nPos, pCard); }

void CPlayer::RecordCardUndo(int nPos, CCard* pCard) 
	{ m_pPlayEngine->RecordCardUndo(nPos, pCard); }

void CPlayer::RecordTrickUndo() 
	{ m_pPlayEngine->RecordTrickUndo(); }

void CPlayer::RecordRoundComplete(int nPos, CCard* pCard) 
	{ m_pPlayEngine->RecordRoundComplete(nPos, pCard); }

void CPlayer::RecordHandComplete(int nResult)
	{ m_pPlayEngine->RecordHandComplete(nResult); }

void CPlayer::RecordSpecialEvent(int nCode, int nParam1, int nParam2, int nParam3)
	{ m_pPlayEngine->RecordSpecialEvent(nCode, nParam1, nParam2, nParam3); }

int CPlayer::GetNumClaimableTricks()
	{ return m_pPlayEngine->GetNumClaimableTricks(); }


//
// feeback routines (using m_pStatusDlg)
//
void CPlayer::SuspendTrace() 
	{ m_pStatusDlg->EnableTrace(FALSE); }

void CPlayer::ResumeTrace() 
	{ m_pStatusDlg->EnableTrace(TRUE); }

void CPlayer::Trace(const CString& strText) 
	{ *m_pStatusDlg << strText & "\n"; }

void CPlayer::AddTrace(CString& strMessage, CString& strTemp) 
	{ *m_pStatusDlg << strMessage << strTemp; }

void CPlayer::AddTrace(CString& strMessage, LPCTSTR szTemp) 
	{ *m_pStatusDlg << strMessage << szTemp; }

void CPlayer::ClearAnalysis() 
	{ m_pStatusDlg->Clear(); }

void CPlayer::ShowAnalysis(BOOL bShow) 
	{ m_pStatusDlg->Show(bShow); }

void CPlayer::SetTestBiddingMode(bool bMode) 
	{ m_pBidder->SetTestBiddingMode(bMode); }

bool CPlayer::GetTestBiddingMode() 
	{ return m_pBidder->GetTestBiddingMode(); }

Position CPlayer::GetOpeningPosition() 
	{ return (Position) m_pBidder->GetOpeningPosition(); }




//
//==========================================================
//
//
// Main Routines
//
//
//==========================================================
//




//
void CPlayer::InitializePlayer(Position nPos, CPlayer* pPartner, CPlayer* pLHOpponent, CPlayer* pRHOpponent)
{
	// init settings
	m_nPosition = nPos;
	m_pPartner = pPartner;
	m_pLHOpponent = pLHOpponent;
	m_pRHOpponent = pRHOpponent;
	m_pPlayEngine = NULL;
	//
	m_nSkillLevel = 0;
	m_bDeclarer = FALSE;
	m_bDummy = FALSE;
	m_bTeamIsVulnerable = FALSE;

	// init member objects
	m_pStatusDlg->SetPosition(nPos);
	m_pHand->Initialize(this, m_pStatusDlg);
//	m_combinedHand.Initialize(this, m_pStatusDlg, m_pHand, m_pPartner->GetHand());
}



//
void CPlayer::InitializeEngines()
{
	// init player engines
	m_pBidder->Initialize(this, m_pPartner, m_pLHOpponent, m_pRHOpponent, m_pHand, m_pCardLocation, &m_pGuessedHands[0], m_pStatusDlg);
	m_pDeclarer->Initialize(this, m_pPartner, m_pLHOpponent, m_pRHOpponent, m_pHand, m_pCardLocation, &m_pGuessedHands[0], m_pBidder, m_pStatusDlg);
	m_pDummy->Initialize(this, m_pPartner, m_pLHOpponent, m_pRHOpponent, m_pHand, m_pCardLocation, &m_pGuessedHands[0], m_pBidder, m_pStatusDlg);
	m_pDefender->Initialize(this, m_pPartner, m_pLHOpponent, m_pRHOpponent, m_pHand, m_pCardLocation, &m_pGuessedHands[0], m_pBidder, m_pStatusDlg);

	// clear hand at this point, after the engines have been initialized
	ClearHand();
}


//
// InitializeHand()
//
// Called after the player has been dealt a hand
//
void CPlayer::InitializeHand()
{
	m_bDeclarer = FALSE;
	m_bDummy = FALSE;
	//
	m_pHand->InitNewHand();
	for(int i=0;i<4;i++)
	{
		m_pGuessedHands[i]->Clear();
		m_pGuessedHands[i]->InitNewHand(*m_pHand, m_nPosition);
	}
	// and clear the analysis dialog
	ClearAnalysis();
}




//
// InitializeRestoredHand()
//
// Called after the player's hand has been loaded from a file
//
void CPlayer::InitializeRestoredHand()
{
	// set internal variables
	if (pDOC->GetDeclarerPosition() == m_nPosition)
		m_bDeclarer = TRUE;
	else
		m_bDeclarer = FALSE;
	//
	if (pDOC->GetDummyPosition() == m_nPosition)
		m_bDummy = TRUE;
	else
		m_bDummy = FALSE;

	// init the newly received hand
//	m_pHand->InitNewHand();

	// clear the analysis dialog
	ClearAnalysis();
	*m_pStatusDlg << "3Starting play from restored game.\n------------------------\n";

	// determine the proper player algorithm to use
	if (m_bDeclarer)
		m_pPlayEngine = m_pDeclarer;
	else if (m_bDummy)
		m_pPlayEngine = m_pDummy;
	else
		m_pPlayEngine = m_pDefender;

	// and init
	m_pHand->InitNewHand(); 
	m_pHand->EvaluateHoldings();
	m_pPlayEngine->InitNewHand(); 

	// mark game position
	int nContract = pDOC->GetContract();
	int nModifier = pDOC->GetContractModifier();
	if (ISBID(nContract))
	{
		*m_pStatusDlg << "4The contract is " & ContractToFullString(nContract,nModifier) & 
					   " by " & PositionToString(pDOC->GetDeclarerPosition()) & ".\n";
		*m_pStatusDlg << "4" & PositionToString(pDOC->GetRoundLead()) & " leads.\n";
		*m_pStatusDlg << "4====================\n";
	}

	// finally, expose cards if necessary
	if ( (m_nPosition == SOUTH) ||
		 ((pDOC->GetDeclarerPosition() == NORTH) && (m_nPosition == NORTH)) )
		ExposeCards(TRUE, FALSE);
	else
		ExposeCards(FALSE, FALSE);
}



//
// InitializeSwappedHand()
//
// Called after the player's hand has been swapped with another player's in midgame
//
void CPlayer::InitializeSwappedHand()
{
	// set internal variables
	if (pDOC->GetDeclarerPosition() == m_nPosition)
		m_bDeclarer = TRUE;
	else
		m_bDeclarer = FALSE;
	//
	if (pDOC->GetDummyPosition() == m_nPosition)
		m_bDummy = TRUE;
	else
		m_bDummy = FALSE;

	// clear the analysis dialog
	ClearAnalysis();
	*m_pStatusDlg << "3Swapped into a new hand.\n------------------------\n";

	// determine the proper player algorithm to use
	if (m_bDeclarer)
		m_pPlayEngine = m_pDeclarer;
	else if (m_bDummy)
		m_pPlayEngine = m_pDummy;
	else
		m_pPlayEngine = m_pDefender;

	// and init
	m_pHand->InitSwappedHand(); 
	m_pHand->EvaluateHoldings();
	m_pPlayEngine->InitSwappedHand(); 

	// finally, expose cards if necessary
	if ( (m_nPosition == SOUTH) ||
		 ((pDOC->GetDeclarerPosition() == NORTH) && (m_nPosition == NORTH)) )
		ExposeCards(TRUE, FALSE);
	else if (m_bDummy && (pDOC->GetNumCardsPlayedInGame() > 0))
		ExposeCards(TRUE, FALSE);
	else
		ExposeCards(FALSE, FALSE);
}



//
// ClearHand()
//
// - called to clear and empty out the hand
//
void CPlayer::ClearHand()
{
	m_pHand->ClearHand(); 
//	if (m_pPlayEngine)
//		m_pPlayEngine->Clear(); 
	m_pDeclarer->Clear(); 
	m_pDummy->Clear(); 
	m_pDefender->Clear(); 
	m_pCardLocation->Clear();
	for(int i=0;i<4;i++)
		m_pGuessedHands[i]->Clear(TRUE); 
	//
	m_bDeclarer = FALSE;
	m_bDummy = FALSE;
}



//
// ExposeCards()
//
// turns the cards face up (or down)
//
void CPlayer::ExposeCards(BOOL bCode, BOOL bRedraw)
{
	m_pHand->ExposeCards(bCode);
	if (bRedraw)
		RedrawHand();
}



//
// RedrawHand()
//
void CPlayer::RedrawHand() const
{
	pVIEW->DisplayHand(m_nPosition);
}




//
// Call the bidding logic to make a bid
//
int CPlayer::Bid()
{
	return m_pBidder->Bid();

	// do any additional stuff here
}



//
// Call the bidding logic just to set internal variables
//
int CPlayer::BidInternal()
{
	SuspendTrace();
	int nBid = GetBiddingHint();
	ResumeTrace();
	return nBid;
}


//
// BiddingFinished() 
//
// bidding is finished, so prepare to play the hand
//
void CPlayer::BiddingFinished() 
{
	m_pBidder->BiddingFinished(); 
	// determine the proper player
	if (m_bDeclarer)
		m_pPlayEngine = m_pDeclarer;
	else if (m_bDummy)
		m_pPlayEngine = m_pDummy;
	else
		m_pPlayEngine = m_pDefender;
	m_pPlayEngine->InitNewHand(); 
}


//
// Call this to record a bid made by a player
// calls the bid validation routines
//
BOOL CPlayer::EnterHumanBid(int nBid)
{
	// this bid is provided by the human, the old fashioned way
	m_pBidder->SetBid(nBid);
	return m_pBidder->ValidateBid(nBid);
}

//
// Call the bidding logic to cancel bidding
//
void CPlayer::CancelBidding()
{
	m_pBidder->CancelBidding();

	// do any additional stuff here
}


//
// ClearBiddingInfo() 
// 
void CPlayer::ClearBiddingInfo() 
{ 
	// 
//	m_pHand->Clear();
	m_pBidder->Clear();
}


//
// Call the bidding logic to cancel bidding
//
void CPlayer::RestartBidding()
{ 
	m_pHand->RestartBidding(); 
	m_pHand->EvaluateHoldings();
	m_pBidder->RestartBidding(); 
}



//
// RestartPlay()
// 
// restart play of the current hand
//
void CPlayer::RestartPlay()
{
	// reset guessed hands
	for(int i=0;i<4;i++)
	{
		m_pGuessedHands[i]->Clear();
		m_pGuessedHands[i]->InitNewHand(*m_pHand, m_nPosition);
	}
	// reset hand
	RestoreInitialHand();
	m_pPlayEngine->RestartPlay();
	//
	if (m_nPosition != SOUTH)
		m_pHand->ExposeCards(FALSE);
	InitialEvaluateHand();
}



//
// GetPriorBid()
//
// if index = 0, returns the most recently made bid
// if index = 1, returns the bid before the most recent made bid
// if index = 2, returns the bid two turns before the most recent bid
// etc...
//
int CPlayer::GetPriorBid(int nIndex)
{
	if (nIndex < 0)
		return 0;
	int numBidTurns = m_pBidder->GetNumBidTurns();
	int nEntry = numBidTurns - nIndex - 1;
	if (nEntry < 0)
		return BID_NONE;
	return pDOC->GetBidByPlayer(m_nPosition,nEntry);
}


//
// GetPlayHint() 
//
CCard* CPlayer::GetPlayHint(BOOL bAutoHintMode) 
{ 
//	m_pStatusDlg->SetAutoHintMode(bAutoHintMode); 
	if (!theApp.GetValue(tbEnableAnalysisDuringHints) && !bAutoHintMode)
		SuspendTrace();
	CCard* pCard = NULL;
	//
	pVIEW->SetCurrentModeTemp(CEasyBView::MODE_THINKING);
	m_pStatusDlg->ClearHints();
	m_pStatusDlg->BeginHintBlock();
	try
	{
		if (m_bDummy)
		{
			m_pDummy->SetHintMode(true);
			pCard = m_pDummy->GetPlayHint(); 
		}
		else
		{
			m_pPlayEngine->SetHintMode(true);
			pCard = m_pPlayEngine->GetPlayHint(); 
		}
	}
	catch(int)
	{
		// computer player claimed
	}
	//
	if (m_bDummy)
		m_pDummy->SetHintMode(false);
	else
		m_pPlayEngine->SetHintMode(false);
	m_pStatusDlg->EndHintBlock();
	pVIEW->RestoreMode();
	//
	if (!theApp.GetValue(tbEnableAnalysisDuringHints) && !bAutoHintMode)
		ResumeTrace();
//	if (bAutoHintMode)
//		m_pStatusDlg->SetAutoHintMode(FALSE); 
	return pCard;
}


//
// GetPlayHintForDummy()
//
CCard* CPlayer::GetPlayHintForDummy()
{ 
	CCard* pCard = m_pDeclarer->GetPlayHintForDummy(); 
	return pCard;
}



//
// TestForAutoPlayLastCard()
//
BOOL CPlayer::TestForAutoPlayLastCard()
{
	int nSuitLed = NONE;
	CCard* pLeadCard = pDOC->GetCurrentTrickCardLed();
	if (pLeadCard)
		nSuitLed = pLeadCard->GetSuit();
	if (ISSUIT(nSuitLed) && (GetNumCardsInSuit(nSuitLed) == 1))
	{
		// sleep if necessary
		if (theApp.GetValue(tbInsertBiddingPause))
			Sleep(theApp.GetValue(tnPlayPauseLength)*100);

		// auto play last card in suit
		CCard* pPlayCard = GetCardInSuit(nSuitLed, 0);
		ASSERT(pPlayCard);
		ASSERT((Position)pPlayCard->GetOwner() == m_nPosition);
		pVIEW->PostMessage(WM_COMMAND, WMS_CARD_PLAY+1002, (int)pPlayCard);

		// note the play in the hint window
		if (theApp.GetValue(tnAutoHintMode) > 0)
		{
			m_pStatusDlg->ClearHints();
			m_pStatusDlg->BeginHintBlock();
			*m_pStatusDlg << "Follow with our only " & STSS(nSuitLed) & ", the " & pPlayCard->GetFaceName() & ".\n";
			m_pStatusDlg->EndHintBlock();
		}
		// done
		return TRUE;
	}
	else if (GetNumCards() == 1)
	{
		// sleep if necessary
		if (theApp.GetValue(tbInsertBiddingPause))
			Sleep(theApp.GetValue(tnPlayPauseLength)*100);

		// auto play last card in hand
		CCard* pPlayCard = GetCardByPosition(0);
		ASSERT(pPlayCard);
		ASSERT((Position)pPlayCard->GetOwner() == m_nPosition);
		pVIEW->PostMessage(WM_COMMAND, WMS_CARD_PLAY+1003, (int)pPlayCard);
		
		// note the play in the hint window
		if (theApp.GetValue(tnAutoHintMode) > 0)
		{
			m_pStatusDlg->ClearHints();
			m_pStatusDlg->BeginHintBlock();
			*m_pStatusDlg << "Follow with our last card, the " & pPlayCard->GetFaceName() & ".\n";
			m_pStatusDlg->EndHintBlock();
		}
		// done
		return TRUE;
	}

	// nothing to autoplay here
	return FALSE;
}



//
// IsHumanPlayer()
//
// determines whether this is a human player or not
//
BOOL CPlayer::IsHumanPlayer() const
{
	// might change this code later to allow the human player
	// to play something other than South

	// a player is human if it's south, or if it's north AND
	// declarer is north or south
	int nDeclarer = pDOC->GetDeclarerPosition();
	if ((m_nPosition == SOUTH) || 
		( (m_nPosition == NORTH) && 
		  ((nDeclarer == NORTH) || (nDeclarer == SOUTH)) ) )
		return TRUE;
	else
		return FALSE;
}






///////////////////////////////////////////////////////////////
//
// Data get/set functions
//
///////////////////////////////////////////////////////////////




//
LPVOID CPlayer::GetValuePV(int nItem, int nIndex1, int nIndex2, int nIndex3) const // NCR
{
	switch (nItem)
	{
		case tnPosition:
			return (LPVOID) m_nPosition;
		case tnSkillLevel:
			return (LPVOID) m_nSkillLevel;
		case tbDummy:
			return (LPVOID) m_bDummy;
		case tbTeamIsVulnerable:
			return (LPVOID) m_bTeamIsVulnerable;
		case tpPartner:
			return (LPVOID) m_pPartner;
		// game info
		case tszAnalysis:
			return (LPVOID) m_pStatusDlg->GetText();

		//
		// the following get passed into m_pHand
		//

		// card info
		case tInitialHand:
		case tHand:		
		case tDisplayHand:
		case tstrHoldings:		
		case tstrScreenHoldings:	
		case tbHandSorted:
		case tbExposeCards:
		case tnumCardsHeld:
		case tSuitHoldings:
		case tnumCardsInSuit:
		// basic hand statistics
		case tnumAcesHeld:	
		case tnumKingsHeld:	
		case tnumHighCardPoints:
		case tnumSuitPoints:
		case tnumShortPoints:
		case tnumLongPoints:
		case tnumBonusPoints:
		case tnumPenaltyPoints:
		case tnumTotalPoints:
		case tnumAdjustedPoints:
		case tnBalanceValue:
		// more detailed hand analysis
		case tnumSuitHonors:
		case tbSuitStopped:	
		case tbSuitProbStopped:
		case tnumSuitsStopped:		
		case tnumSuitsProbStopped:	
		case tnumStoppersInSuit:
		case tnumSuitsUnstopped:
		case tnStoppedSuits:
		case tnProbStoppedSuits:
		case tnUnstoppedSuits:
		case tstrSuitsStopped:
		case tstrSuitsUnstopped:
		case tnumRebiddableSuits:
		case tbSuitIsRebiddable:
		case tnSuitStrength:	
		case tbSuitIsMarginal:
		case tbSuitIsOpenable:
		case tbSuitIsStrong:
		case tbSuitIsPreferred:
		case tbSuitIsAbsolute:
		case tbSuitIsSolid:	
		case tnumMarginalSuits:
		case tnumOpenableSuits:
		case tnumStrongSuits:
		case tnumPreferredSuits:
		case tnumAbsoluteSuits:
		case tnumSolidSuits:
		case tnLowestMarginalSuit:
		case tnLowestOpenableSuit:	
		case tnLowestStrongSuit:
		case tnLowestPreferredSuit:
		case tnLowestAbsoluteSuit:
		case tnLowestSolidSuit:
		case tnMarginalSuitList:
		case tnOpenableSuitList:
		case tnStrongSuitList:	
		case tnPreferredSuitList:
		case tnAbsoluteSuitList:
		case tnSolidSuitList:	
		case tnHighestMarginalSuit:
		case tnHighestOpenableSuit:
		case tnHighestStrongSuit:
		case tnHighestPreferredSuit:
		case tnHighestAbsoluteSuit:
		case tnHighestSolidSuit:
		case tnPreferredSuit:	
		case tnSuitsByPreference:
		case tnSuitRank:		
		case tnumVoidSuits:
		case tnVoidSuits:		
		case tnumSingletonSuits:
		case tnSingletonSuits:
		case tnumDoubletonSuits:
		case tnDoubletonSuits:	
		case tnSuitsOfAtLeast:	
		case tnumWinners:
		case tnumLosers:
		case tnumSuitWinners:	
		case tnumSuitLosers:	
			return m_pHand->GetValuePV(nItem, nIndex1, nIndex2, nIndex3);

		//
		// the following get passed to m_bidStatus
		//
		// team pos
		case tnMinimumTPPoints:		
		case tnMinimumTPCPoints:	
		case tnMaximumTPPoints:	
		case tnMaximumTPCPoints:
		case tnPartnersMinimum:
		case tnPartnersMaximum:
		//
		case tnBid:
		case tnumBidTurns:
		case tnumBidsMadeByPlayer:
		case tnNextIntendedBid:
		case tnIntendedSuit:
		case tnIntendedContract:
		//
		case tnOpeningPosition:
		case tnPartnersSuit:
		case tnAgreedSuit:
		case tbRoundForceActive:
		case tbGameForceActive:
		// convention status
		case tnBlackwoodStatus:
		case tnGerberStatus:
		case tnJacobyTransferStatus:
		case tnStaymanStatus:
		// partner's hand info
		case tnumPartnersAces:
		case tnumPartnersKings:
			return m_pBidder->GetValuePV(nItem, nIndex1, nIndex2, nIndex3);

		//
		default:
			AfxMessageBox("Unhandled Call to CPlayer::GetFaceValue()");
			return NULL;
	}
	return NULL;
}

//
double CPlayer::GetValueDouble(int nItem, int nIndex1, int nIndex2, int nIndex3)
{
	switch (nItem)
	{
		case tfQuickTricks:	
			return m_pHand->GetNumQuickTricks();
		default:
			AfxMessageBox("Unhandled Call to CPlayer::GetValue");
			return 1;
	}
	return 0;
}

//
// SetValuePV()
//
int CPlayer::SetValuePV(int nItem, LPVOID value, int nIndex1, int nIndex2, int nIndex3)
{
	int nVal = (int) value;
	BOOL bVal = (BOOL) value;
	LPCTSTR sVal = (LPCTSTR) value;
	//
	switch (nItem)
	{
		case tnPosition:
			m_nPosition = (Position) nVal;
			break;
		case tnSkillLevel:
			m_nSkillLevel = nVal;
			break;
		case tbDummy:
			m_bDummy = bVal;
			break;
		case tbTeamIsVulnerable:
			m_bTeamIsVulnerable = bVal;
			break;
		case tpPartner:
			m_pPartner = (CPlayer*) value;
			break;
		// game info
		case tszAnalysis:
			m_pStatusDlg->Clear();
			*m_pStatusDlg << sVal;
			break;
			
		//
		// the following get passed to m_pHand
		//
		// card info
		case tInitialHand:
		case tHand:		
		case tDisplayHand:
		case tstrHoldings:		
		case tstrScreenHoldings:	
		case tbHandSorted:
		case tbExposeCards:
		case tnumCardsHeld:
		case tSuitHoldings:
		case tnumCardsInSuit:
		// basic hand statistics
		case tnumAcesHeld:	
		case tnumKingsHeld:	
		case tnumHighCardPoints:
		case tnumSuitPoints:
		case tnumShortPoints:
		case tnumLongPoints:
		case tnumBonusPoints:
		case tnumPenaltyPoints:
		case tnumTotalPoints:
		case tnumAdjustedPoints:
		case tnBalanceValue:
		// more detailed hand analysis
		case tnumSuitHonors:
		case tbSuitStopped:	
		case tbSuitProbStopped:
		case tnumSuitsStopped:		
		case tnumSuitsProbStopped:	
		case tnumStoppersInSuit:
		case tnumSuitsUnstopped:
		case tnStoppedSuits:
		case tnProbStoppedSuits:
		case tnUnstoppedSuits:
		case tstrSuitsStopped:
		case tstrSuitsUnstopped:
		case tnumRebiddableSuits:
		case tbSuitIsRebiddable:
		case tnSuitStrength:	
		case tbSuitIsMarginal:
		case tbSuitIsOpenable:
		case tbSuitIsStrong:
		case tbSuitIsPreferred:
		case tbSuitIsAbsolute:
		case tbSuitIsSolid:	
		case tnumMarginalSuits:
		case tnumOpenableSuits:
		case tnumStrongSuits:
		case tnumPreferredSuits:
		case tnumAbsoluteSuits:
		case tnumSolidSuits:
		case tnLowestMarginalSuit:
		case tnLowestOpenableSuit:	
		case tnLowestStrongSuit:
		case tnLowestPreferredSuit:
		case tnLowestAbsoluteSuit:
		case tnLowestSolidSuit:
		case tnMarginalSuitList:
		case tnOpenableSuitList:
		case tnStrongSuitList:	
		case tnPreferredSuitList:
		case tnAbsoluteSuitList:
		case tnSolidSuitList:	
		case tnHighestMarginalSuit:
		case tnHighestOpenableSuit:
		case tnHighestStrongSuit:
		case tnHighestPreferredSuit:
		case tnHighestAbsoluteSuit:
		case tnHighestSolidSuit:
		case tnPreferredSuit:	
		case tnSuitsByPreference:
		case tnSuitRank:		
		case tnumVoidSuits:
		case tnVoidSuits:		
		case tnumSingletonSuits:
		case tnSingletonSuits:
		case tnumDoubletonSuits:
		case tnDoubletonSuits:	
		case tnSuitsOfAtLeast:	
		case tnumWinners:
		case tnumLosers:
		case tnumSuitWinners:	
		case tnumSuitLosers:	
			m_pHand->SetValuePV(nItem, value, nIndex1, nIndex2, nIndex3);
			return 0;

		//
		// the following get passed to m_bidStatus
		//
		// team pos
		case tnMinimumTPPoints:		
		case tnMinimumTPCPoints:	
		case tnMaximumTPPoints:	
		case tnMaximumTPCPoints:
		case tnPartnersMinimum:
		case tnPartnersMaximum:
		//
		case tnBid:
		case tnumBidTurns:
		case tnumBidsMadeByPlayer:
		case tnNextIntendedBid:
		case tnIntendedSuit:
		case tnIntendedContract:
		//
		case tnOpeningPosition:
		case tnPartnersSuit:
		case tnAgreedSuit:
		case tbRoundForceActive:
		case tbGameForceActive:
		// convention status
		case tnBlackwoodStatus:
		case tnGerberStatus:
		case tnJacobyTransferStatus:
		case tnStaymanStatus:
		// partner's hand info
		case tnumPartnersAces:
		case tnumPartnersKings:
			m_pBidder->SetValuePV(nItem, value, nIndex1, nIndex2, nIndex3);
			return 0;

		//
		default:
			AfxMessageBox("Unhandled Call to CPlayer::SetValue()");
			return 1;
	}
	return 0;
}

// "double" version
int CPlayer::SetValue(int nItem, double fValue, int nIndex1, int nIndex2, int nIndex3)
{
//	switch (nItem)
//	{
//		default:
			AfxMessageBox("Unhandled Call to CPlayer::SetValue()");
			return 1;
//	}
//	return 0;
}

// conversion functions
LPCTSTR CPlayer::GetValueString(int nItem, int nIndex1, int nIndex2, int nIndex3)
{
	return (LPCTSTR) GetValuePV(nItem, nIndex1, nIndex2, nIndex3);
}

void CPlayer::SetValueString(int nItem, LPCTSTR szValue, int nIndex1, int nIndex2, int nIndex3)
{
	SetValuePV(nItem, (LPVOID) szValue, nIndex1, nIndex2, nIndex3);
}

int CPlayer::GetValue(int nItem, int nIndex1, int nIndex2, int nIndex3) const // NCR added const
{
	return (int) GetValuePV(nItem, nIndex1, nIndex2, nIndex3);
}

int CPlayer::SetValue(int nItem, int nValue, int nIndex1, int nIndex2, int nIndex3)
{
	return SetValuePV(nItem, (LPVOID) nValue, nIndex1, nIndex2, nIndex3);
}