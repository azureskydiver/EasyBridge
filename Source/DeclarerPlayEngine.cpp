//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------
/* Program changes by NCR
    7/23/09 - All unreferenced variables (warned at level 4) were commented out
         Possible problem here if there were side effects where functions modify an arg
		  and the return value is ignored. See GetOutstandingCards() for example 
	8/24/09 - NCR-38 Don't finesse (early?) if unstopped suit(s); try them if we need tricks
    8/25/09 - Also dont' use force if unstopped suits that could be run by opponents

*/

//=====================================================================
//
// CDeclarerPlayEngine
//
// - encapsulates the declarer play engine and its status variables
//
//=====================================================================


#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "EasyBVw.h"
#include "docopts.h"
#include "Mainfrm.h"
#include "HandHoldings.h"
#include "BidEngine.h"
#include "Player.h"
#include "Card.h"
#include "Deck.h"
#include "Play.h"
#include "Cash.h"
#include "TrumpPull.h"
#include "Force.h"
#include "Type1Finesse.h"
#include "Type2Finesse.h"
#include "Type3Finesse.h"
#include "TypeAFinesse.h"
#include "TypeBFinesse.h"
#include "Ruff.h"
#include "HoldUp.h"
#include "Drop.h"
#include "Discard.h"
#include "ExitPlay.h"
#include "gib.h"
#include "progopts.h"
#include "DeclarerPlayEngine.h"
#include "DummyPlayEngine.h"
#include "CombinedHoldings.h"
#include "CombinedSuitHoldings.h"
#include "StatusWnd.h"
#include "PlayerStatusDialog.h"
#include "handopts.h"  // NCR-38 for tn... variable definitions


//
//==========================================================================
//==========================================================================
//
// Initialization routines
//
//
CDeclarerPlayEngine::CDeclarerPlayEngine()
{
	m_pCombinedHand = new CCombinedHoldings;
	m_pInitialCombinedHand = new CCombinedHoldings;
	m_declarerPriorityDiscards.ReverseSortOrder();
	m_dummyPriorityDiscards.ReverseSortOrder();
}

CDeclarerPlayEngine::~CDeclarerPlayEngine()
{
	//
	Clear();
	delete m_pCombinedHand;
	delete m_pInitialCombinedHand;
}



// 
//-----------------------------------------------------------------------
//
// Initialize()
//
// one-time initialization
//
void CDeclarerPlayEngine::Initialize(CPlayer* pPlayer, CPlayer* pPartner, CPlayer* pLHOpponent, CPlayer* pRHOpponent, CHandHoldings* pHoldings, CCardLocation* pCardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine* pBidder, CPlayerStatusDialog* pStatusDlg)
{ 
	// call base class first
	CPlayEngine::Initialize(pPlayer, pPartner, pLHOpponent, pRHOpponent, pHoldings, pCardLocation, ppGuessedHands, pBidder, pStatusDlg);

	// init partner's hand ptr and the combined hand
	m_pPartnersHand = &m_pPartner->GetHand();
	m_pDummy = m_pPartner->GetDummyEngine();
	m_pCombinedHand->Initialize(pPlayer, m_pStatusDlg, m_pHand, m_pPartnersHand);
	m_pInitialCombinedHand->Initialize(pPlayer, m_pStatusDlg, m_pHand, m_pPartnersHand);

	// init play info
	m_pCurrentPlay = NULL;
}



//
//-----------------------------------------------------------------------
//
// Clear()
//
void CDeclarerPlayEngine::Clear()
{ 
	// call base class first
	CPlayEngine::Clear();

	// clear analysis variables
	m_numSureTricks = 0;
	m_numTricksShort = 0;
	m_numTricksMade = 0;
	m_numDeclarerTrumps = m_numDummyTrumps = m_numTotalTrumps = m_numOutstandingTrumps = 0;
	m_numDeclarerEntries = 0;
	m_numDummyEntries = 0;
	m_numDeclarerRuffingEntries = 0;
	m_numDummyRuffingEntries = 0;
	//
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<4;i++)
	{
		m_numDeclarerEntriesInSuit[i] = 0;
		m_numDummyEntriesInSuit[i] = 0;
	}
	//
	m_numRequiredTricks = 0;
	m_numTricksLeftToBeMade = 0;
	m_numPlannedHoldUps = 0;
	m_numPlannedCashingTricks = 0;
	m_numPlannedRuffingTricks = 0;
	m_numPlannedDummyRuffingTricks = 0;
	m_numPlannedDeclarerRuffingTricks = 0;
	m_numPlannedDropPlays = 0;
//	m_numPlannedFinesseTricks = 0;
	m_numPlannedFinesses = 0;
//	m_numPlannedSpeculativeTricks = 0;
	m_numPlannedForcePlays = 0 ;
	m_numPlannedSecondaryCashPlays = 0;
	m_numPlannedSluffTricks = 0;
	m_numPlannedTrumpPulls = 0;
	m_numTotalPlannedTricks = 0;
	m_numPlannedRounds = 0;
	for(i=0;i<4;i++)
		m_nSuitPriorityList[i] = NONE;
	//
	for(i=0;i<4;i++)
	{
		m_numPlannedDummyRuffsInSuit[i] = 0;
		m_numPlannedDeclarerRuffsInSuit[i] = 0;
		m_numPlannedTotalRuffsInSuit[i] = 0;
		m_numPlannedDropPlaysInSuit[i] = 0;
		m_numPlannedFinessesInSuit[i] = 0;
		m_numPlannedForcePlaysInSuit[i] = 0;
		m_numPlannedSecondaryCashPlaysInSuit[i] = 0;
	}
	//
	m_pCombinedHand->ClearHand();
	m_pInitialCombinedHand->ClearHand();
	m_playPlan.Clear();
	m_usedPlays.Clear();
	m_discardedPlays.Clear();
	m_declarerPriorityDiscards.Clear();
	m_dummyPriorityDiscards.Clear();
	m_declarerDiscardedPriorityDiscards.Clear();
	m_dummyDiscardedPriorityDiscards.Clear();
	m_declarerEntries.Clear();
	m_dummyEntries.Clear();
	m_declarerRuffingEntries.Clear();
	m_dummyRuffingEntries.Clear();
	m_pCurrentPlay = NULL;
	m_strFailedPlay = _T("");
	m_bClaimDisabled = FALSE;
	m_bCrossedOnLastPlay = FALSE;
	m_bInHintMode = false;
}

//--------------------------------------------------
// GetIndexOfBestQualPlay  NCR-761
// return index in playlist of best plan, or -1 if all have same quality(0)
//
int GetIndexOfBestQualPlay(CPlayList& playList) {
	int maxFnd = -999;
	int maxIdx = -1;
	for(int i=0; i < playList.GetSize(); i++) {
		if(playList[i]->GetQuality() > maxFnd) {
			maxFnd = playList[i]->GetQuality();  // save quality and index
			maxIdx = i;
		}
	}  // end for(i) through list
	if(maxFnd == 0 && maxIdx == 0)
		return -1;   // nothing found
	return maxIdx;   // return the idex
}  // end NCR-761

//
//-----------------------------------------------------------------------
//
// AddCardToCombinedHand()
//
// called when a card play is "undone" to update the combined hand info
//
void CDeclarerPlayEngine::AddCardToCombinedHand(CCard* pCard, BOOL bPlayerCard, BOOL bSort)
{
	m_pCombinedHand->AddFromSource(pCard, bPlayerCard, TRUE);
	m_pCombinedHand->ReevaluateHoldings();
}


//
//-----------------------------------------------------------------------
//
// RemoveCardFromCombinedHand()
//
// simialr to the above
//
void CDeclarerPlayEngine::RemoveCardFromCombinedHand(CCard* pCard)
{
	int nPos = pCard->GetOwner();
	ASSERT(ISPLAYER(nPos));
	if (nPos == m_pPlayer->GetPosition()) 
	{
		m_pCombinedHand->RemoveFromSource(pCard, TRUE);
		m_pCombinedHand->ReevaluateHoldings();
	}
	else if (nPos == m_pPartner->GetPosition())
	{
		m_pCombinedHand->RemoveFromSource(pCard, FALSE);
		m_pCombinedHand->ReevaluateHoldings();
	}
}




//
//-----------------------------------------------------------------------
//
// InitNewHand()
//
// called when the hand is dealt (or play is restarted)
//
void CDeclarerPlayEngine::InitNewHand()
{ 
	// make sure we're declarer
	VERIFY(m_pPlayer->IsDeclarer());

	// call base class first
	CPlayEngine::InitNewHand();
	m_numRequiredTricks = 6 + BID_LEVEL(m_nContract);
	m_numTricksLeftToBeMade = m_numRequiredTricks;
}





//
//-----------------------------------------------------------------------
//
// InitSwappedHand()
//
// called when the hand has been swapped with another player's in midgame
//
void CDeclarerPlayEngine::InitSwappedHand()
{ 
	InitNewHand();
}




//
//-----------------------------------------------------------------------
//
// InitCombinedHand()
//
// called when dummy first lays down his hand
//
void CDeclarerPlayEngine::InitCombinedHand()
{ 
	// init
	m_pCombinedHand->InitNewHand();
	m_pCombinedHand->EvaluateHoldings();
	m_declarerPriorityDiscards.Clear();
	m_declarerDiscardedPriorityDiscards.Clear();
	m_dummyDiscardedPriorityDiscards.Clear();
	m_dummyPriorityDiscards.Clear();
	m_declarerEntries.Clear();
	m_dummyEntries.Clear();
	m_declarerRuffingEntries.Clear();
	m_dummyRuffingEntries.Clear();

	// set other variables
	if (ISSUIT(m_nTrumpSuit))
	{
		m_numDeclarerTrumps = m_pHand->GetNumCardsInSuit(m_nTrumpSuit);
		m_numDummyTrumps = m_pPartnersHand->GetNumCardsInSuit(m_nTrumpSuit);
		m_numTotalTrumps = m_numDeclarerTrumps + m_numDummyTrumps;
		m_numOutstandingTrumps = 13 - m_numTotalTrumps;
	}
	else
	{
		m_numDeclarerTrumps = m_numDummyTrumps = m_numTotalTrumps = m_numOutstandingTrumps = 0;
	}
}




//
// RestartPlay()
//
void CDeclarerPlayEngine::RestartPlay()
{
	// call base class
	CPlayEngine::RestartPlay();
	// and update status display (play plan page)
	pMAINFRAME->UpdateStatusWindow(CStatusWnd::SP_PLAY_PLAN | CStatusWnd::SP_SUIT_STATUS);
}



//
// GetNumClaimableTricks() 
//
int CDeclarerPlayEngine::GetNumClaimableTricks() 
{ 
	int numWinners = 0;

	// special handling for suit contracts
	int nTrumpSuit = pDOC->GetTrumpSuit();
	if (ISSUIT(nTrumpSuit))
	{
		// first check trumps
		CCombinedSuitHoldings& trumpSuit = m_pCombinedHand->GetSuit(nTrumpSuit);
		numWinners = Min(trumpSuit.GetNumTopCards(), trumpSuit.GetMaxLength());
		// add the winners in the other suits only if we hold all winners in trumps
		// and at least as many as the outstanding trumps
		int numOutstandingTrumps = GetNumOutstandingCards(nTrumpSuit);
		if ( (numWinners >= trumpSuit.GetMaxLength()) & (numWinners >= numOutstandingTrumps) )
		{
			for(int i=0;i<4;i++)
			{
				if (i != nTrumpSuit)
					numWinners += m_pCombinedHand->GetSuit(i).GetNumMaxWinners();
			}
		}
	}
	else
	{
		// NT contract is straightforward
		numWinners = m_pCombinedHand->GetNumMaxWinners();
	}

	//
	return numWinners;
}




//
// RecordCardPlay()
//
// note that a card has been played
//
void CDeclarerPlayEngine::RecordCardPlay(int nPos, CCard* pCard) 
{ 
	// first call the base class
	CPlayEngine::RecordCardPlay(nPos, pCard);

	// see if we are currently restoring a position, and this is dummy's 
	// first card played
	// OR: we are in manual mode, watching the human play for us
	if ( ((pVIEW->GetCurrentMode() == CEasyBView::MODE_GAMERESTORE) || 
		   (theApp.GetValue(tnCardPlayMode) == CEasyBApp::PLAY_MANUAL)) && 
		 (nPos == m_pPartner->GetPosition()) && (pDOC->GetNumCardsPlayedInGame() == 2) )
	{
		// if so, we have to init the combined hand at this point, since
		// it won;t get done through the normal course of events
		InitCombinedHand();

		// also, we have to add the card back to the combined hand, as 
		// it will get removed in the very next line below!
		m_pCombinedHand->AddFromSource(pCard, FALSE, TRUE);
	}

	// then update our combined hand info
	if ((nPos == m_pPlayer->GetPosition()) || (nPos == m_pPartner->GetPosition()))
		m_pCombinedHand->Remove(pCard);

	// if this is an idle declarer engine (i.e., for North/South), init
	// the combined hand anyway 
	CEasyBDoc* pDoc = pDOC;
	if ( ((pDoc->GetDeclarerPosition() == NORTH) || (pDoc->GetDeclarerPosition() == SOUTH)) &&
		  (pDoc->GetNumTricksPlayed() == 0) && (pDoc->GetNumCardsPlayedInRound() == 2))
	{
		InitCombinedHand();
	}

	// now re-evaluate holdings
	m_pCombinedHand->ReevaluateHoldings(pCard);
	// and entries
	EvaluateEntries();

	// then update status display
	pMAINFRAME->UpdateStatusWindow(CStatusWnd::SP_SUIT_STATUS);
}



//
// RecordCardUndo()
//
// note that a card has been undone
//
void CDeclarerPlayEngine::RecordCardUndo(int nPos, CCard* pCard)
{
	// first call the base class
	CPlayEngine::RecordCardUndo(nPos, pCard);

	// then update our combined hand info
	if (nPos == m_pPlayer->GetPosition()) 
		AddCardToCombinedHand(pCard, TRUE, TRUE);
	else if (nPos == m_pPartner->GetPosition()) 
		AddCardToCombinedHand(pCard, FALSE, TRUE);

	//
	// try to undo certain information
	//

	// see if this was one of our priority discards
	if (m_declarerDiscardedPriorityDiscards.HasCard(pCard))
	{
		m_declarerDiscardedPriorityDiscards.Remove(pCard);
		m_declarerPriorityDiscards.Add(pCard);
	} 
	else if (m_dummyDiscardedPriorityDiscards.HasCard(pCard))
	{
		m_dummyDiscardedPriorityDiscards.Remove(pCard);
		m_dummyPriorityDiscards.Add(pCard);
	}
}



//
// RecordTrickUndo()
//
// note that the current trick has been undone
// (currently not used)
//
void CDeclarerPlayEngine::RecordTrickUndo()
{
	// first call base class
	CPlayEngine::RecordTrickUndo();

	// see if a play is currently in progres
	CPlay* pPlay = m_pCurrentPlay;
	if ((pPlay) && (pPlay->GetStatus() == PLAY_IN_PROGRESS))
	{
		// reset it
		pPlay->Reset();
	}
	else
	{
		// no play in progress -- restore the previous play from 
		// the used play queue
		// but check if there's nothing in the queue (e.g., N/S playing)
		if (m_usedPlays.GetSize() > 0)
		{
			CPlay* pPlay = m_usedPlays.PopTail();
			if (pPlay)
			{
				// reset the play and stick it back in the queue
				pPlay->Reset();
				m_playPlan.PushPlay(pPlay);
				pMAINFRAME->UpdateStatusWindow(CStatusWnd::SP_PLAY_PLAN | CStatusWnd::SP_SUIT_STATUS);
			}
		}
	}

	//
	// try to "forget certain things fomr the last trick, if possible
	//
	m_bCrossedOnLastPlay = FALSE;
}



//
// RecordRoundComplete()
//
// nPos = winner
// pCard = winning card
//
void CDeclarerPlayEngine::RecordRoundComplete(int nPos, CCard* pCard) 
{ 
	// first call the base class
	CPlayEngine::RecordRoundComplete(nPos, pCard);

	// remove the "current" play, if there is one, from the play list
	// and then save it in the used plays list
	if (m_pCurrentPlay)
	{
		int nIndex = m_playPlan.FindPlay(m_pCurrentPlay);
		ASSERT(nIndex >= 0);
		m_playPlan.RemovePlay(nIndex, FALSE);	// pull out but don't delete
	    m_usedPlays.AppendPlay(m_pCurrentPlay);
		m_pCurrentPlay = NULL;
	}
}





//
//==========================================================================
//==========================================================================
//
// Analysis routines
//
//




//
//-----------------------------------------------------------------------
//
// AssessPosition()
//
// called once on each round of play
//
void CDeclarerPlayEngine::AssessPosition()
{
	// call base class first
	CPlayEngine::AssessPosition();

	// then perform class-specific operations
	m_bUsingGIB = theApp.GetValue(tbEnableGIBForDeclarer);
}



//
//-----------------------------------------------------------------------
//
// AdjustCardCountFromPlay()
//
// adjust card count and analysis after a card is played
//
void CDeclarerPlayEngine::AdjustCardCountFromPlay(int nPos, CCard* pCard)
{
	if ((nPos != m_pPlayer->GetPosition()) && (nPos != m_pPartner->GetPosition()))
		CPlayEngine::AdjustCardCountFromPlay(nPos, pCard);
}



//
//-----------------------------------------------------------------------
//
// AdjustCardCountFromUndo()
//
// adjust card count and analysis after a card played is undone
//
void CDeclarerPlayEngine::AdjustCardCountFromUndo(int nPos, CCard* pCard)
{
	if ((nPos != m_pPlayer->GetPosition()) && (nPos != m_pPartner->GetPosition()))
		CPlayEngine::AdjustCardCountFromUndo(nPos, pCard);
}



//
//-----------------------------------------------------------------------
//
// AdjustHoldingsCount()
//
// called to adjust analysis of holdings after a round of play
//
void CDeclarerPlayEngine::AdjustHoldingsCount(CCard* pCard)
{
	CPlayEngine::AdjustHoldingsCount(pCard);
}




//
// GetOutstandingCards()
//
// determine the list of outstanding cards in this suit
// outstanding means all the cards that are not in our own hand
// or dummy, _and_ which have not been played
//
int	CDeclarerPlayEngine::GetOutstandingCards(int nSuit, CCardList& cardList) const
{
	// call the base class
	CPlayEngine::GetOutstandingCards(nSuit, cardList);

	// done 
	return cardList.GetNumCards();
}




//
// EvaluateEntries()
//
// counts the # of entries into each hand
//
void CDeclarerPlayEngine::EvaluateEntries()
{
	// and entry is a low card in one hand used to either cash a high card
	// or ruff in the other hand
	m_declarerEntries.Clear();
	m_dummyEntries.Clear();
	m_declarerRuffingEntries.Clear();
	m_dummyRuffingEntries.Clear();
	m_numDeclarerEntries = 0;
	m_numDummyEntries = 0;
	m_numDeclarerPossibleEntries = 0;  // NCR-563
	m_numDummyPossibleEntries = 0;     // NCR-563
	m_numDeclarerRuffingEntries = 0;
	m_numDummyRuffingEntries = 0;
	//
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<4;i++)
	{
		m_numDeclarerEntriesInSuit[i] = 0;
		m_numDummyEntriesInSuit[i] = 0;
	}

	// - - - - - - - - - - - - - - - - - - -
	// first count # of entries into declarer
	//
	// check # of possible ruffs
	if (ISSUIT(m_nTrumpSuit))
	{
		CCombinedSuitHoldings& trumpSuit = m_pCombinedHand->GetSuit(m_nTrumpSuit);
		int numDeclarerTrumps = trumpSuit.GetNumDeclarerCards();
//		int numDeclarerExcessTrumps = numDeclarerTrumps - trumpSuit.GetNumDeclarerWinners();

		// iterate over other suits
		int numSluffs = 0;
		int nSuit = GetNextSuit(m_nTrumpSuit);
		int i; // NCR-FFS added here, removed below
		for(/*int*/ i=0;i<3;i++)
		{	
			CCombinedSuitHoldings& suit = m_pCombinedHand->GetSuit(nSuit);
//			numSluffs += suit.GetNumDummyLosers();
			if (suit.GetNumDummyCards() > suit.GetNumDeclarerCards())
				numSluffs += (suit.GetNumDummyCards() - suit.GetNumDeclarerCards());
			nSuit = GetNextSuit(nSuit);
		}
//		int numRuffs = Min(numDeclarerExcessTrumps, numSluffs);
		int numRuffs = Min(numDeclarerTrumps, numSluffs);
		m_numDeclarerEntries += numRuffs;
		m_numDeclarerEntriesInSuit[m_nTrumpSuit] = numRuffs;
		m_numDeclarerRuffingEntries = m_numDeclarerEntries;
		//
		CSuitHoldings& trumps = m_pHand->GetSuit(m_nTrumpSuit);
		for(int j=0,n=trumps.GetNumCards()-1;j<numRuffs;j++,n--)
			m_declarerRuffingEntries << trumps.GetAt(n);
	}
	
	// check cashing entries in each suit
	// NCR-422 Changed search loop to move one index until > condition found, then move both
	for(i=0;i<4;i++)
	{
		CCombinedSuitHoldings& combinedSuit = m_pCombinedHand->GetSuit(i);
		if (combinedSuit.GetNumDummyCards() == 0)
			continue;

		// a card in hand is an "entry" if it's a winner and is higher than 
		// a corresponding card in the dummy suit
		CSuitHoldings& declarer = combinedSuit.GetDeclarerSuit();
		CSuitHoldings& dummy = combinedSuit.GetDummySuit();
		//
		// NCR-410 Compute Number of entries from Sure winners
		// If none out, then all are winners
		int numDeclarerWinners = (combinedSuit.GetNumOutstandingCards() <= 0) ? declarer.GetNumCards()
								: declarer.GetNumCardsAbove(combinedSuit.GetMissingCardVal(0));  // NCR-410
//		int numDeclarerWinners = combinedSuit.GetNumDeclarerWinners();
		int numDummyCards = dummy.GetNumCards();
		int numEntries = 0;

		// compare cards up the line
		for(int j=numDeclarerWinners-1,k=numDummyCards-1;j>=0 && k>=0;/*j--,k--*/)
		{
			if (*declarer[j] > *dummy[k])
			{
				numEntries++;
				m_declarerEntries << declarer[j];
				// NCR-422 move both indexes to next higher card
				j--;
				k--;
			}else{ // declarer's not greater
				j--; // only move declarers's
			}
		}  // end for(j,k) comparing cards

		// tally 
		m_numDeclarerEntries += numEntries;
		m_numDeclarerEntriesInSuit[i] += numEntries;

		// NCR-563 Now check for possible entries if a high card is forced out.
		// Say there is an Ace out and we have KQ then we can get one entry

  		   // Do declarer and dummy have at least 2 cards each
		if(((declarer.GetNumCards() > 1) && (numDummyCards > 1)) 
			&& (combinedSuit.GetNumOutstandingCards() > 0) 
			&& (combinedSuit.GetMissingCardVal(0) > combinedSuit.GetTopCard()->GetFaceValue()) 
			//  Is there only one higher card out? Only one O.S. or the second one is lower
			&& ((combinedSuit.GetNumOutstandingCards() == 1) 
			    || (combinedSuit.GetTopCard()->GetFaceValue() > combinedSuit.GetMissingCardVal(1)))
            // Does declarer have 2 sequential high cards
		    && (declarer.GetSequence2(0).GetNumCards() > 1) 
			// Finally are declarer's cards > dummy's cards
			&& (declarer.GetSequence2(0).GetBottomCard()->GetFaceValue()
			    > dummy.GetBottomCard()->GetFaceValue()) )
		{
			m_numDeclarerPossibleEntries++; // count this suit as a possible entry
		}  // NCR-563 - end checking for possible entries

	}  // end for(i) thru declarer's suits

	
	// - - - - - - - - - - - - - - - -
	// now do the same for dummy
	//
	// check # of possible ruffs
	if (ISSUIT(m_nTrumpSuit))
	{
		CCombinedSuitHoldings& trumpSuit = m_pCombinedHand->GetSuit(m_nTrumpSuit);
		int numDummyTrumps = trumpSuit.GetNumDummyCards();
//		int numDummyExcessTrumps = numDummyTrumps - trumpSuit.GetNumDummyWinners();
		// iterate over other suits
		int numSluffs = 0;
		int nSuit = GetNextSuit(m_nTrumpSuit);
		for(int i=0;i<3;i++)
		{	
			CCombinedSuitHoldings& suit = m_pCombinedHand->GetSuit(nSuit);
//			numSluffs += m_pCombinedHand->GetSuit(nSuit).GetNumDummyLosers();
			if (suit.GetNumDeclarerCards() > suit.GetNumDummyCards())
				numSluffs += (suit.GetNumDeclarerCards() - suit.GetNumDummyCards());
			nSuit = GetNextSuit(nSuit);
		}
//		int numRuffs = Min(numDummyExcessTrumps, numSluffs);
		int numRuffs = Min(numDummyTrumps, numSluffs);
		m_numDummyEntries += numRuffs;
		m_numDummyEntriesInSuit[m_nTrumpSuit] = numRuffs;
		m_numDummyRuffingEntries = m_numDummyEntries;
		//
		CSuitHoldings& trumps = m_pPartnersHand->GetSuit(m_nTrumpSuit);
		for(int j=0,n=trumps.GetNumCards()-1;j<numRuffs;j++,n--)
			m_dummyRuffingEntries << trumps.GetAt(n);
	}
	
	// check cashing entries in each suit
	for(i=0;i<4;i++)
	{
		CCombinedSuitHoldings& combinedSuit = m_pCombinedHand->GetSuit(i);
		if (combinedSuit.GetNumDummyCards() == 0)
			continue;

		// a card in dummy is an "entry" if it's a winner and is higher than 
		// a corresponding card in the declarer suit
		CSuitHoldings& declarer = combinedSuit.GetDeclarerSuit();
		CSuitHoldings& dummy = combinedSuit.GetDummySuit();
		//
		// NCR-410 Compute Number of entries from Sure winners
		// If none out, then all are winners
		int numDummyWinners = (combinedSuit.GetNumOutstandingCards() <= 0) ? dummy.GetNumCards()
								: dummy.GetNumCardsAbove(combinedSuit.GetMissingCardVal(0));  // NCR-410
//		int numDummyWinners = combinedSuit.GetNumDummyWinners();
		int numDeclarerCards = declarer.GetNumCards();
		int numEntries = 0;
		// compare cards up the line
		for(int j=numDummyWinners-1,k=numDeclarerCards-1;j>=0 && k>=0;/*j--,k--*/)
		{
			if (*dummy[j] > *declarer[k])
			{
				numEntries++;
				m_dummyEntries << dummy[j];
				// NCR-422 move both indexes to next higher card
				j--;
				k--;
			}else{ // dummy's not greater
				j--; // only move dummy's
			}
		} // end for(j,k) comparing cards

		// tally 
		m_numDummyEntries += numEntries;
		m_numDummyEntriesInSuit[i] += numEntries;

		// NCR-563 Now check for possible entries if a high card is forced out.
		// Say there is an Ace out and we have KQ then we can get one entry

  		   // Do declarer and dummy have at least 2 cards each
		if(((dummy.GetNumCards() > 1) && (numDeclarerCards > 1)) 
			&& (combinedSuit.GetNumOutstandingCards() > 0) 
			&& (combinedSuit.GetMissingCardVal(0) > combinedSuit.GetTopCard()->GetFaceValue()) 
			//  Is there only one higher card out? Only one O.S. or the second one is lower
			&& ((combinedSuit.GetNumOutstandingCards() == 1) 
			    || (combinedSuit.GetTopCard()->GetFaceValue() > combinedSuit.GetMissingCardVal(1)))
            // Does dummy have 2 sequential high cards
		    && (dummy.GetSequence2(0).GetNumCards() > 1) 
			// Finally are dummy's cards > declarer's cards
			&& (dummy.GetSequence2(0).GetBottomCard()->GetFaceValue()
			    > declarer.GetBottomCard()->GetFaceValue()) )
		{
			m_numDummyPossibleEntries++; // count this suit as a possible entry
		}  // NCR-563 - end checking for possible entries

	
	} // end for(i) thru dummy's suits

}






//
//==========================================================================
//==========================================================================
//
// Main operations 
//
//




//
// GetLeadCard()
//
CCard* CDeclarerPlayEngine::GetLeadCard()
{
	// shouldn't call this routine!
//	ASSERT(FALSE);

	// actually, we might have to call this routine if declarer 
	// is left with nothing but losers
	return CPlayEngine::GetLeadCard();
}



//
// PlayCard()
//
// select a card to play
// 
CCard* CDeclarerPlayEngine::PlayCard()
{ 
	CPlayerStatusDialog& status = *m_pStatusDlg;

	// analyze the combined hand
	AssessPosition();

	// check to see if we can claim
	TestForClaim();

	// see if we're using GIB
	CCard* pCard = NULL;
	if (m_bUsingGIB)
	{
		// call GIB
		CGIB& gib = *(theApp.GetGIB());
		status << "3GIB! Invoking GIB for the play...\n";
		int nPlay = gib.Invoke(m_pPlayer, m_pHand, m_pPartnersHand, m_pStatusDlg);
		// find and play the card
		if (ISDECKVAL(nPlay))
			pCard = m_pHand->FindCardByDeckValue(nPlay);
		if (pCard)
		{
			status << "3GIB! GIB plays the " & pCard->GetName() & ".\n";
			VERIFY(pCard->IsValid());
			return pCard;
		}
		else
		{
			status << "3GIBX! GIB failed to return a valid play; defaulting to standard play engine.\n";
		}
	}

	//
	// else we're not using GIB
	//

	// as we're playing declarer, we must have formed a play plan already
	// but if this is round 2 or later, look for any newly emergent plays
	if (pDOC->GetPlayRound() > 0) 
	{	
		// plan if we're in pos 1 or 2, or if we're in hint mode and 
		// we haven't planned any plays yet
		if (pDOC->GetNumCardsPlayedInRound() < 2)
			ReviewPlayPlan();
	}
	
	// now try the plays
	pCard = TrySelectedPlays();

	// did the play list fail?
	if (pCard == NULL)
		pCard = CPlayEngine::PlayCard();	

	// do some sanity checks here
	ASSERT(pCard->IsValid());
	ASSERT(m_pHand->HasCard(pCard));

	// update status display (play plan page)
	pMAINFRAME->UpdateStatusWindow(CStatusWnd::SP_PLAY_PLAN);
	//
	return pCard;
}



//
// PlayForDummy()
//
//
CCard* CDeclarerPlayEngine::PlayForDummy()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;

	// analyze the combined hand
	AssessPosition();

	// check to see if we can claim
	TestForClaim();

	//
	status << "2PLDM! Playing card from dummy.\n";


	//
	// call GIB if using it
	//
	// see if we're using GIB
	CCard* pCard = NULL;
	m_bUsingGIB = theApp.GetValue(tbEnableGIBForDeclarer);
	if (m_bUsingGIB)
	{
		// call gib
		CGIB& gib = *(theApp.GetGIB());
		status << "3GIB! Invoking GIB for the play...\n";
		int nPlay = gib.Invoke(m_pPlayer, m_pHand, m_pPartnersHand, m_pStatusDlg);
		// find and play the card
		if (ISDECKVAL(nPlay))
			pCard = m_pPartnersHand->FindCardByDeckValue(nPlay);
		if (pCard)
		{
			status << "3GIB! GIB plays the " & pCard->GetName() & ".\n";
			return pCard;	
		}
		else
		{
			status << "3GIBX! GIB failed to return a valid play; defaulting to standard play engine.\n";
		}
	}

	//
	// else we're not using GIB
	//

	// 
	if (m_nRound == 0)
	{
		InitCombinedHand();
		PlanPlay();
	}

	// if this is round 2 or later, look for any newly emergent plays
	if ((pDOC->GetPlayRound() > 0) && (pDOC->GetNumCardsPlayedInRound() < 2))
		ReviewPlayPlan();

	// then try the plays we have selected
	pCard = TrySelectedPlays();

	// did the play list fail?
	if (pCard == NULL)
//		pCard = m_pDummy->PlayForSelf();	
		pCard = CPlayEngine::PlayCard();	

	//
	ASSERT(pCard->IsValid());
	ASSERT(m_pPartnersHand->HasCard(pCard));

	// update status display (play plan page)
	pMAINFRAME->UpdateStatusWindow(CStatusWnd::SP_PLAY_PLAN);
	//
	return pCard;
}



//
// TestForClaim()
//
void CDeclarerPlayEngine::TestForClaim()
{
	//
	if (m_bClaimDisabled)
		return;

	//
	if (theApp.InExpressAutoPlay())
		return;

	// see if we can claim the hand (or should concede)
	if (pDOC->GetNumCardsPlayedInRound() == 0) 
	{
		// see how many tricks we have left -- if it's just one, don't bother
		int numRemainingTricks = 13 - pDOC->GetNumTricksPlayed();
		if (numRemainingTricks == 1)
			return;

		// add up top cards
		int numWinners = GetNumClaimableTricks();

		// required conditions:
		// 1 - we must have at least as many winners as remaining tricks
		// 2 - if we have winners in both hands, we must have entries into both,
		//     in DIFFERENT suits

		// test condition #1
		if ((numWinners < numRemainingTricks) || !theApp.GetValue(tbComputerCanClaim))
			return;

		// check entries
		EvaluateEntries();
		BOOL bPlayingInHand = (pDOC->GetCurrentPlayer() == m_pPlayer);

		// test condition #2 -- see if we need winners from both hands
		if ( (m_pCombinedHand->GetNumDeclarerTopCards() < numRemainingTricks) &&
			 (m_pCombinedHand->GetNumDummyTopCards() < numRemainingTricks) )
		{
			// if so, make sure we have entries into both hands
			if ((m_numDeclarerEntries == 0) || (m_numDummyEntries == 0))
				return;

			// also make sure the entries are in different suits
			int i; // NCR-FFS added here, removed below
			for(/*int*/ i=0;i<4;i++)
			{
				if (m_numDeclarerEntriesInSuit[i] != m_numDeclarerEntriesInSuit[i])
					break;
			}
			if (i == 4)
				return;
		}

		// see if we have the entries to cash winners in both hands
		// here, see if we're in dummy and need to cash winners from hand
		if (!bPlayingInHand && (m_pCombinedHand->GetNumDeclarerTopCards() >= 0) &&
			 (m_pCombinedHand->GetNumDummyTopCards() < numRemainingTricks) && (m_numDeclarerEntries == 0))
			return;
	
		// or vice versa
		if (bPlayingInHand && (m_pCombinedHand->GetNumDummyTopCards() >= 0) &&
			(m_pCombinedHand->GetNumDeclarerTopCards() < numRemainingTricks) && (m_numDummyEntries == 0))
			return;
		
		// try to claim the hand -- but first show the hands
		BOOL bHandsWereExposed = m_pPlayer->AreCardsExposed() || theApp.AreCardsFaceUp();
		m_pPlayer->ExposeCards(TRUE, !bHandsWereExposed);
		int nCode = AfxMessageBox(FormString("Declarer (%s) claims the remaining tricks.  Do you agree?", PositionToString(m_pPlayer->GetPosition())), MB_ICONQUESTION | MB_YESNO);
		m_pPlayer->ExposeCards(bHandsWereExposed, !bHandsWereExposed);
		if (nCode == IDYES)
		{
			// player concedes
			pDOC->ClaimTricks(m_pPlayer->GetPosition());
			throw 1;	// escape
		}
		else
		{
			// player rejects the claim -- continue
			pVIEW->Notify(WM_COMMAND, WMS_REFRESH_DISPLAY);	// clear the dialog
			m_bClaimDisabled = TRUE;	// no more claiming
		}

/*
		else if (m_pCombinedHand->GetNumWinners() == 0)
		{
			// concede the hand
			int nCode = AfxMessageBox(FormString("%s concedes the remaining %d tricks.  Do you accept?", PositionToString(m_pPlayer->GetPosition()), numRemainingTricks), MB_ICONQUESTION | MB_YESNO);
			if (nCode == IDYES)
			{
				pDOC->ConcedeTricks(m_pPlayer->GetPosition());
				throw 1;	// escape
			}
		}
*/
	}
}




//
// ScreenIneligiblePlays()
//
// go through the play list & remove those that are now invalid
//
int CDeclarerPlayEngine::ScreenIneligiblePlays()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
//	status << "PLSCN! Screening out ineligible plays based on current information...\n";
	status << "3PLYSCN! Reviewing available plays...\n";
		
	// cycle through plays 
//	int nCount = m_playPlan.GetSize();
	int numPlaysDeleted = 0;
	int numPlays = m_playPlan.GetSize(); 
	for(int i=0;i<numPlays;i++)
	{
		// grab the next play
		CPlay* pPlay = m_playPlan.PopPlay();
		VERIFY(pPlay != NULL);

		// if this play is currently active, skip the tests
		if (pPlay->GetStatus() == PLAY_IN_PROGRESS)
		{
			m_playPlan.AppendPlay(pPlay);
			continue;
		}

		// check if we still have the required consumable card
		CCard* pConsumedCard = pPlay->GetConsumedCard();
		if ((pConsumedCard) && (!m_pCombinedHand->HasCard(pConsumedCard)))
		{
			status << "5PLYSCN1! The play <" & pPlay->GetName() & 
					  "> is no longer viable as the " & pConsumedCard->GetName() &
					  " has already been played.\n";
			// adjust count if necessary
			AdjustPlayCountForDeletedPlay(pPlay);
			//
			delete pPlay;
			numPlaysDeleted++;
			continue;
		}

		// check if the target card is still outstanding
		CCard* pTargetCard = pPlay->GetTargetCard();
		if ((pTargetCard) && (!IsCardOutstanding(pTargetCard)))
		{
			status << "5PLYSCN2! The play <" & pPlay->GetName() & 
					  "> is no longer viable as the target card [" & pTargetCard->GetName() &
					  "] has already been played.\n";
			// adjust count if necessary
			AdjustPlayCountForDeletedPlay(pPlay);
			//
			delete pPlay;
			numPlaysDeleted++;
			continue;
		}

		// check if we still have the required key cards
		CCardList* pKeyCards = pPlay->GetKeyCardsList();
		if ((pKeyCards) && (!m_pCombinedHand->ContainsCards(*pKeyCards)))
		{
			status << "5PLYSCN4! The play <" & pPlay->GetName() & 
					  "> is no longer viable as the partnership no longer holds all the required cards of [" & 
					  pKeyCards->GetHoldingsString() & "].\n";
			// adjust count if necessary
			AdjustPlayCountForDeletedPlay(pPlay);
			//
			delete pPlay;
			numPlaysDeleted++;
			continue;
		}

		// check if we have at least one OR-Key cards
		CCardList* pOrKeyCards = pPlay->GetOrKeyCardsList();
		if ((pOrKeyCards) && (!m_pCombinedHand->ContainsAtLeastOneOf(*pOrKeyCards)))
		{
			status << "5PLYSCN6! The play <" & pPlay->GetName() & 
					  "> is no longer viable as the partnership no longer holds any of the alternative required cards of [" & 
					  pOrKeyCards->GetHoldingsString() & "].\n";
			// adjust count if necessary
			AdjustPlayCountForDeletedPlay(pPlay);
			//
			delete pPlay;
			numPlaysDeleted++;
			continue;	// nope
		}

		// check the second OR-Key cards list
		CCardList* pOrKeyCards2 = pPlay->GetOrKeyCardsList2();
		if ((pOrKeyCards2) && (!m_pCombinedHand->ContainsAtLeastOneOf(*pOrKeyCards2)))
		{
			status << "5PLYSCN7! The play <" & pPlay->GetName() & 
					  "> is no longer viable as the partnership no longer holds any of the second set of alternative required cards of [" & 
					  pOrKeyCards2->GetHoldingsString() & "].\n";
			// adjust count if necessary
			AdjustPlayCountForDeletedPlay(pPlay);
			//
			delete pPlay;
			numPlaysDeleted++;
			continue;	// nope
		}

		// check the enemy key cards list
		CCardList* pEnemyKeyCards = pPlay->GetEnemyKeyCardsList();
		BOOL bBreak = FALSE;
		if (pEnemyKeyCards)
		{
			// check to be sure each of the cards is still outstanding
			// that is, held by the opponents and not yet played
			for(int j=0;j<pEnemyKeyCards->GetNumCards();j++)
			{
				if (!IsCardOutstanding((*pEnemyKeyCards)[j]))
				{
					status << "5PLYSCN8! The play <" & pPlay->GetName() & 
							  "> is no longer viable as the opponents no longer hold the " & 
							  (*pEnemyKeyCards)[j]->GetName() & 
							  ", which is one of the cards required for the play.\n";
					// adjust count if necessary
					AdjustPlayCountForDeletedPlay(pPlay);
					//
					delete pPlay;
					numPlaysDeleted++;
					bBreak = TRUE;
					break;
				}
			}
			if (bBreak)
				continue;
		}

		// check the enemy OR-Key cards list
		CCardList* pEnemyOrKeyCards = pPlay->GetEnemyOrKeyCardsList();
		BOOL bCardFound = FALSE;
		if (pEnemyOrKeyCards)
		{
			// check if any of the cards are still outstanding
			for(int j=0;j<pEnemyOrKeyCards->GetNumCards();j++)
			{
				if (IsCardOutstanding((*pEnemyOrKeyCards)[j]))
				{
					bCardFound = TRUE;
					break;
				}
			}
			if (!bCardFound)
			{
				status << "5PLYSCN9! The play <" & pPlay->GetName() & 
						  "> is no longer viable as the opponents no longer hold any of the required alternative cards of [" & 
						  pEnemyOrKeyCards->GetHoldingsString() & "].\n";
				// adjust count if necessary
				AdjustPlayCountForDeletedPlay(pPlay);
				//
				delete pPlay;
				numPlaysDeleted++;
				continue;
			}
		}

/*
 * for now, don't delete these plays since they're needed to calculate tricks
 *
		// check the cards that must've been played 
		CCardList* pRequiredPlayedCards = pPlay->GetRequiredPlayedCardsList();
		if (pRequiredPlayedCards)
		{
			// check if any of the cards that should have benen played 
			// are still outstanding
			for(int j=0;j<pRequiredPlayedCards->GetNumCards();j++)
			{
				CCard* pCard = (*pRequiredPlayedCards)[j];
				if (IsCardOutstanding(pCard))
				{
					status << "5PLYSCN10! The play <" & pPlay->GetName() & 
							  "> is not yet viable as the card [" & 
							  pCard->GetFaceName() & "] is still outstanding.\n";
					// adjust count if necessary
					AdjustPlayCountForDeletedPlay(pPlay);
					//
					delete pPlay;
					numPlaysDeleted++;
					bBreak = TRUE;
					break;
				}
			}
		}
		if (bBreak)
			continue;
 */

		// else the play is still viable, so replace it at the back of the list
		m_playPlan.AppendPlay(pPlay);
#ifdef _DEBUG_XXX   // NCR Debugging
    	status << "5PLYSCN11! The play <" & pPlay->GetName() & "> still viable. Placed at end\n";
#endif
	}

	// 
	if (numPlaysDeleted > 0)
		status << "4PLYSCNX! " & numPlaysDeleted & " play(s) have been deleted as they are no longer viable.\n";

	// update the play plan
	pMAINFRAME->UpdateStatusWindow(CStatusWnd::SP_PLAY_PLAN);

	// return the number of valid plays left
	return m_playPlan.GetSize();
}



//
// AdjustPlayCountForDeletedPlay
//
void CDeclarerPlayEngine::AdjustPlayCountForDeletedPlay(CPlay* pPlay)
{
	//
	if (pPlay->IsWinner())
		m_numTotalPlannedTricks--;

	//
	switch(pPlay->GetPlayType())
	{
		case CPlay::TRUMP_PULL:
			m_numPlannedTrumpPulls--;
			break;

		case CPlay::CASH:
			m_numPlannedCashingTricks--;
			break;

		case CPlay::RUFF:
			m_numPlannedRuffingTricks--;
			break;

		case CPlay::DROP:
			m_numPlannedDropPlays--;
			break;

		case CPlay::FORCE:
			m_numPlannedForcePlays--;
			break;

		case CPlay::FINESSE:
			m_numPlannedFinesses--;
			break;

		case CPlay::HOLDUP:  case CPlay::DEVELOP:  
		case CPlay::DISCARD:  case CPlay::EXIT:
			break;
	}
}



//
// IsPlayUsable()
//
BOOL CDeclarerPlayEngine::IsPlayUsable(CPlay& play)
{
	CCard* pCard;
#ifndef _DEBUG
	// NCR Leave trace on when debugging 
	m_pStatusDlg->EnableTrace(FALSE);
#endif
	int nCode = play.Perform(*this, *m_pCombinedHand, *m_pCardLocation,
							  m_ppGuessedHands, *m_pStatusDlg, pCard);
	m_pStatusDlg->EnableTrace(TRUE);

	// check return code 
	if ((nCode == PLAY_IN_PROGRESS) || (nCode == PLAY_COMPLETE))
		return TRUE;

	//
	return FALSE;
}




//
// TrySelectedPlays()
//
CCard* CDeclarerPlayEngine::TrySelectedPlays()
{
	// see if a play plan even exists
	if ((!m_playPlan.IsActive()) || (m_playPlan.IsEmpty()))
		return NULL;

	//
	CPlayerStatusDialog& status = *m_pStatusDlg;

	// now sift through the play list
//	int nCurrPos = pDOC->GetCurrentPlayerPosition();

	// and search through the remaining plays for a usable one
	int nPlayIndex = 0;
	int numAvailablePlays = m_playPlan.GetSize();  // NCR moved before usage
	BOOL bWrap = FALSE;
	int numDeferrals = 0;
	if (!m_strFailedPlay.IsEmpty())
		m_strFailedPlay = _T("");

	// start with the "current" play in the play list, if there is one
	if (m_pCurrentPlay)
	{
		nPlayIndex = m_playPlan.FindPlay(m_pCurrentPlay);
		bWrap = (nPlayIndex > 0 ? TRUE : FALSE);  // NCR no need to wrap if starting at 0
	}
	// NCR-348 Problem with bWrap if only 1 plan - it skips it
	else
		bWrap = (numAvailablePlays == 1) ? TRUE : FALSE; // NCR-348 ensure processing one

	// and then inspect available plays
	CCard* pCard = NULL;
	CPlay* pPlay = NULL;
	int numRemovedPlans = 0;  // NCR-168
	PlayResult nCode = PLAY_ERROR; // NCR-421 debug

	// NCR Variables used in loop:
	//   numAvailablePlays - set to total plans available; decr when plan removed
	//   numPlaysExamined - count of plans examined: incr from 0; decr when play deferred 
	//                      incr at end of loop    
	//   i - loop count; only incremented
	//   nPlayIndex - indexes playPlan array; starts at 0 or current play; 
    //                incr at end of loop; decr when plan Removed or deferred

	// Stop looping when plan found or all plans examined
                                                        // NCR-168 added numRemovedPlans below
	for(int i=0,numPlaysExamined=0; numPlaysExamined < (numAvailablePlays+numRemovedPlans); 
	                  i++,numPlaysExamined++,nPlayIndex++)
	{
        // NCR-168 moved following to front because of continue
		// check to see if we're at the end -- and wrap if appropriate
		if (nPlayIndex > m_playPlan.GetSize()-1) // NCR-399 changed >= to >
		{
			if (bWrap)
			{
				nPlayIndex = 0;  // continue at first plan in array
				bWrap = FALSE;
			} // if not wrapping, we're done
			else
				break;  // NCR-168 exit if going past end
		}

		// grab the next play
		pPlay = m_playPlan[nPlayIndex];
		ASSERT(pPlay != NULL);
		m_pCurrentPlay = pPlay;
		ASSERT(pPlay != NULL);

		// check to be sure we're not using up valuable entries
		// i.e., if we're using up the last entry into a hand while the other hand has
		// more than one entry left, then defer this play
		BOOL bUsingUpLastEntry = FALSE;
		int nEntryCode = CPlay::ENTRY_NONE;
//		CCard* pConsumedCard = pPlay->GetConsumedCard();
		int numDeclarerWinners = m_pCombinedHand->GetNumDeclarerWinners();
		int numDummyWinners = m_pCombinedHand->GetNumDummyWinners();
		//
/*
		if (m_declarerEntries.HasCard(pConsumedCard))
		{
			// using up last entry in hand
			if ((m_numDeclarerEntries == 1) && (m_numDummyEntries > 1) &&
								(numDeclarerWinners > 1))
			{
				bUsingUpLastEntry = TRUE;
				nEntryCode = CPlay::ENTRY_HAND;
			}
		}
		else if (m_dummyEntries.HasCard(pConsumedCard))
		{
			// using up last entry in dummy
			if ((m_numDummyEntries == 1) && (m_numDeclarerEntries > 1) &&
								(numDummyWinners > 1))
			{
				bUsingUpLastEntry = TRUE;
				nEntryCode = CPlay::ENTRY_DUMMY;
			}
		}
*/

		// also check if the play uses the last card in a suit which is needed for
		// and entry into the other hand
		// e.g., playing the last winner in a hand when the other hand has several winners left
		int nSuit = pPlay->GetSuit();
		if (ISSUIT(nSuit) && (m_numDeclarerEntries == 1) && (m_numDummyEntries > 1) && 
				(numDeclarerWinners > 1) && (pPlay->GetEndingHand() == CPlay::IN_DUMMY) &&
				(m_numDeclarerEntriesInSuit[nSuit] == 1))
		{
			bUsingUpLastEntry = TRUE;
			nEntryCode = CPlay::ENTRY_HAND;
		}
		else if (ISSUIT(nSuit) && (m_numDummyEntries == 1) && (m_numDeclarerEntries > 1) && 
				(numDummyWinners > 1) && (pPlay->GetEndingHand() == CPlay::IN_HAND) &&
				(m_numDummyEntriesInSuit[nSuit] == 1))
		{
			bUsingUpLastEntry = TRUE;
			nEntryCode = CPlay::ENTRY_DUMMY;
		}

		// NCR-41 Don't cash last Trump if unstopped suits
		bool bSaveStopper = false;  
		if((nSuit == pDOC->GetTrumpSuit()) && (m_pCombinedHand->GetNumTrumps() == 1)
			&& (pPlay->GetPlayType() == CPlay::CASH) )
		{
			bSaveStopper = (m_pCombinedHand->GetNumSuitsStopped() != 4);
		} // NCR-41 end checking on last trump as stopper

		//
		if ((bUsingUpLastEntry || bSaveStopper) && (i < numAvailablePlays-1)) // NCR-41 added bSaveStopper
		{
			// push the play back
			// don't spout a message if a play is in progress
			if (m_pCurrentPlay == NULL)
				status << "4PLYTRY2! We want to avoid using up the last entry into " &
						   ((nEntryCode == CPlay::ENTRY_HAND)? "hand" : "dummy") & 
						   ", so defer the [" & pPlay->GetDescription() & "] play until later.\n";
//			m_playPlan.DeferPlay(pPlay);
			m_playPlan.MovePlayToEnd(pPlay);
			numDeferrals++;
			numPlaysExamined--;
			nPlayIndex--;
			continue;
		}

		// see if the play is usable here
		/*PlayResult*/ nCode = pPlay->Perform(*this, *m_pCombinedHand, *m_pCardLocation,  // NCR changed int to PlayResult
								           m_ppGuessedHands, *m_pStatusDlg, pCard);

		// see if the play failed
		if (nCode == PLAY_POSTPONE)
		{
			// the play could not be executed, so skip it
			// NCR added message for more info when debugging
			status << "5PLYTRY3! Postponed plan <" & pPlay->GetName() & ">\n"; // NCR DEBUG CODE

		}
		else if (nCode == PLAY_NOT_VIABLE)
		{
			// the play is no longer usable under the circumstances
			// so delete it from the list
			m_strFailedPlay = CString(pPlay->GetName()) + _T("> (abandoned)");
			m_playPlan.RemovePlay(nPlayIndex, FALSE);
			status << "4PLYTRY4! Removed plan <" & m_strFailedPlay & "\n"; // NCR-28 DEBUG CODE
			m_discardedPlays.AppendPlay(pPlay);				
			nPlayIndex--;
			numRemovedPlans++;  // NCR-168
			numAvailablePlays--;
			pCard = NULL;
		}
		else if (nCode == PLAY_FAILED)
		{
			// the play failed -- delete the play
			m_strFailedPlay = CString(pPlay->GetName()) + _T("> (failed)");
			m_playPlan.RemovePlay(nPlayIndex, FALSE);
			status << "4PLYTRY5! Removed plan <" & m_strFailedPlay & "\n"; // NCR-28 DEBUG CODE
			m_discardedPlays.AppendPlay(pPlay);				
			nPlayIndex--;
			numRemovedPlans++;  // NCR-168
			numAvailablePlays--;
			pCard = NULL;	// insurance just in case the play forgets to clear the card
		}
		else if (nCode == PLAY_IN_PROGRESS)
		{
			// the first part of the play succeded
			ASSERT(pCard->IsValid());
			ASSERT(m_pHand->HasCard(pCard) || m_pPartnersHand->HasCard(pCard));
			m_strFailedPlay = _T("");
			break;
		}
		else if (nCode == PLAY_COMPLETE)
		{
			// the play is complete
			ASSERT(pCard->IsValid());
			ASSERT(m_pHand->HasCard(pCard) || m_pPartnersHand->HasCard(pCard));
			break;
		}
		else if (nCode == PLAY_INACTIVE)
		{
			// mixup
			pCard = NULL;
		}
		else if (nCode <= 0)
		{
			// some other error ocurred -- delete the play
			m_strFailedPlay = CString(pPlay->GetName()) + _T("> (unusable)");
			m_playPlan.RemovePlay(nPlayIndex, FALSE);
			status << "4PLYTRY6! Removed plan <" & m_strFailedPlay & "\n"; // NCR-28 DEBUG CODE
			m_discardedPlays.AppendPlay(pPlay);				
			nPlayIndex--;
			numRemovedPlans++;  // NCR-168
			numAvailablePlays--;
			pCard = NULL;
		}

	} // end for() loop thru plays

	//  Test if plan & card to play found above
	if (pCard)
	{
		// make sure we play a valid card!!!
		if (pDOC->GetCurrentPlayerPosition() == m_pPlayer->GetPosition())
			ASSERT(m_pPlayer->HasCard(pCard));
		else
			ASSERT(m_pPartner->HasCard(pCard));
		return pCard;    // return card to play
	}

	// else no usable plays -- insert a blank space in the list of used plays
	status << "3PLYTRY! No usable plays found for the current situation.\n";
	m_pCurrentPlay = NULL;
	m_usedPlays.AppendPlay(NULL);
	return NULL;
}




//
// PlayFirst()
//
// default code for playing in first position
//
CCard* CDeclarerPlayEngine::PlayFirst()
{
	// see if we're playing in hand or in dummy
	BOOL bPlayingInHand = (pDOC->GetCurrentPlayer() == m_pPlayer);

	// call default code in base class
	if (bPlayingInHand)
		return CPlayEngine::PlayFirst();
	else
		return m_pDummy->PlayFirst();
}





//
// PlaySecond()
//
// - default declarer/dummy code for playing in second position
//
CCard* CDeclarerPlayEngine::PlaySecond()
{
	// see if we're playing in hand or in dummy
	BOOL bPlayingInHand = (pDOC->GetCurrentPlayer() == m_pPlayer);

	//
	CPlayerStatusDialog& status = *m_pStatusDlg;
	status << "3PLAYDC2! Playing second in " & (bPlayingInHand? "hand" : "dummy") &
			  ", using default player logic.\n";

/*
	// call default code in base class
	if (bPlayingInHand)
		return CPlayEngine::PlaySecond();
	else
		return m_pDummy->PlaySecond();
*/

	// get play info
	CCard* pCardLed = pDOC->GetCurrentTrickCardLed();
	int nSuitLed = pCardLed->GetSuit();
//	int nCardLedFaceValue = pCardLed->GetFaceValue();
	int nTrumpSuit = pDOC->GetTrumpSuit();
	CCard* pCard = NULL;

	// get hand info
	CCombinedHoldings& combinedHand = *m_pCombinedHand;
	CHandHoldings& playerHand = *(m_pCombinedHand->GetPlayerHand());
	CHandHoldings& dummyHand = *(m_pCombinedHand->GetPartnerHand());
	CCombinedSuitHoldings& combinedSuit = combinedHand.GetSuit(nSuitLed);
	CSuitHoldings& playerSuit = playerHand.GetSuit(nSuitLed);
	CSuitHoldings& dummySuit = dummyHand.GetSuit(nSuitLed);
	//
//	CGuessedHandHoldings& LHOHand = *(m_ppGuessedHands[m_pLHOpponent->GetPosition()]);
//	CGuessedHandHoldings& RHOHand = *(m_ppGuessedHands[m_pRHOpponent->GetPosition()]);
//	CGuessedSuitHoldings& LHOSuit = LHOHand.GetSuit(nSuitLed);
//	CGuessedSuitHoldings& RHOSuit = RHOHand.GetSuit(nSuitLed);


	//
	// get the hand & suit, either dummy's or declarer's
	//
	CHandHoldings& hand = bPlayingInHand? playerHand : dummyHand;
	CSuitHoldings& suit = bPlayingInHand? playerSuit : dummySuit;
//	CSuitHoldings& oppositeSuit = bPlayingInHand? dummySuit : playerSuit;


	// see if we can follow suit
	int numCardsInSuit = suit.GetNumCards();
	if (numCardsInSuit == 1)
	{
		// play the only card we have in the suit
		pCard = suit[0];
		status << "PLDC2A0! Play our only card in the suit, the " & pCard->GetFaceName() & ".\n";
	}
	else if (numCardsInSuit >= 2)
	{
		// we have multipl cards in the suit led
		// play high if we can win the trick, whether in dummy or hand
		// but need to check first if a winner is in the proper hand
		CCardList winnersList, oppositeWinnersList;
		BOOL bHaveHighCard = FALSE, bHaveOppositeHighCard = FALSE;
		if (bPlayingInHand)
		{
			// playing in hand
			if (combinedSuit.GetNumDeclarerWinners() > 0)
			{
				bHaveHighCard = TRUE;
				winnersList << combinedSuit.GetDeclarerWinners();
			}
			if ((combinedSuit.GetNumDummyWinners() > 0)
				// NCR-570 Make sure dummy really has a top card
				&& (combinedSuit.GetSequence2(0).HasCard(dummySuit.GetTopCard())) )
			{
				bHaveOppositeHighCard = TRUE;
				oppositeWinnersList << combinedSuit.GetDummyWinners();
			}
		}
		else
		{
			// playing in dummy
			if (combinedSuit.GetNumDummyWinners() > 0)
			{
				// do we have winners?
				bHaveHighCard = TRUE;
				winnersList << combinedSuit.GetDummyWinners();
			}
			if (combinedSuit.GetNumDeclarerWinners() > 0)
			{
				bHaveOppositeHighCard = TRUE;
				oppositeWinnersList << combinedSuit.GetDeclarerWinners();
			}
		}

		// check if the winners are valid only after the current round
		// i.e., the winners rank just below the lead card
		if (bHaveHighCard && (winnersList[0]->GetFaceValue() < pCardLed->GetFaceValue())) // NCR added () & GetFaceValue()
			bHaveHighCard = FALSE;
		// 
		if (bHaveOppositeHighCard && (oppositeWinnersList[0]->GetFaceValue() < pCardLed->GetFaceValue())) // NCR added ()
			bHaveOppositeHighCard = FALSE;

		//
		// NCR-283 Problem here: Dummy has singleton winner that we're overtaking
		// NCR-297 Play winner in hand if dummy has singleton non-winner
		if (bHaveHighCard && !(bHaveOppositeHighCard && dummySuit.IsSingleton()))
		{
			// TODO: check if we have a winner in the opposite hand, 
			// _AND_ and want to move into that hand

			// NCR-595 check if player (opposite hand) has singleton winner or card same value as ours
			if(playerSuit.IsSingleton() && (combinedSuit.GetSequence2(0).HasCard(playerSuit.GetTopCard())))
			{
				pCard = dummySuit.GetBottomCard(); // NCR-595 play low
			}else {
			// for now, play the bottom winner
			pCard = winnersList.GetLowestCardAbove(pCardLed);   //NCR-226 vs GetBottomCard();
			}
			status << "PLDC2A1! We have a winner in the suit in " &
					  (bPlayingInHand? "hand" : "dummy") & ", so play the " &
					  pCard->GetFaceName() & ".\n";
		}
		// NCR-653 Test if doubleton honor and play if it can lose next trick
		else if(!bPlayingInHand && dummySuit.IsDoubleton()  // && false  //<<<<<<<<<< DEBUG
			&& (dummySuit.GetNumMissingAbove(dummySuit.GetTopCard()) == 1) 
			// NCR-746 Don't play under a higher card
			&& (dummySuit.GetTopCard()->GetFaceValue() > pCardLed->GetFaceValue()) ) 
		{
			pCard = dummySuit.GetTopCard();  
			status << "PLDC2A2! We don't have a winner in the suit in " &
					  (bPlayingInHand? "hand" : "dummy") & ", play the " &
					  pCard->GetFaceName() & " and hope to win.\n";
		}   // end NCR-653 
		else
		{
			// we have no winner in this hand, so discard
			pCard = GetDiscard();
			status << "PLDC2A8! We don't have a winner in the suit in " &
					  (bPlayingInHand? "hand" : "dummy") & ", so discard the " &
					  pCard->GetFaceName() & ".\n";
		}

	}
	else
	{

		// we don't have any cards in the suit led
		// see if we're playing in a suit contract and can trump
		if (hand.GetNumTrumps() > 0)
		{
			// determine whether we should trump
			// we should trump if we don't have a winner in the opposite hand
			// TODO: if we can trump in both hands, see which hand we want to end up in
			
			// first see whether the opposite hand has a winner
			// NCR-83 problem here - numWinners does NOT consider what's been played already ???
			BOOL bHaveOppositeHighCard = FALSE;
			CCard* pOppositeWinner = NULL;
			if (bPlayingInHand)
			{
				// playing in hand; check dummy
				if ((combinedSuit.GetNumDummyWinners() > 0)
					 // NCR-83 check current top card less than dummy's
					&& (pDOC->GetCurrentTrickHighCard()->GetFaceValue() < dummySuit.GetTopCard()->GetFaceValue()) 
					// NCR-111 Also check if there is a higher card out than dummy's top card?
					&& ((dummySuit.GetNumMissingSequences() == 0) 
					    || (dummySuit.GetTopMissingSequence().GetTopCard()->GetFaceValue() 
						     < dummySuit.GetTopCard()->GetFaceValue())) )
				{
					bHaveOppositeHighCard = TRUE;
					pOppositeWinner = combinedSuit.GetDummyWinners().GetBottomCard();
				}
			}
			else
			{
				// playing in dummy; check hand
				if ((combinedSuit.GetNumDeclarerWinners() > 0)
				    // NCR-83 check current top card less than declarer's
					&& (pDOC->GetCurrentTrickHighCard()->GetFaceValue() < playerSuit.GetTopCard()->GetFaceValue()) )
{
					bHaveOppositeHighCard = TRUE;
					pOppositeWinner = combinedSuit.GetDeclarerWinners().GetBottomCard();
				}
			}
			//
			if (bHaveOppositeHighCard)
			{
				// can win in the opposite hand, so discard
				pCard = GetDiscard();
				status << "PLDC2B2! We can win the trick in " & (bPlayingInHand? "dummy" : "hand") & 
						  " with the " & pOppositeWinner->GetFaceName() &
						  ", so discard the " & pCard->GetName() & " here.\n"; // NCR changed GetFaceName to GetName
			}
			else
			{
				// can't win in the opposite hand, so trump here
				// TODO! See if we should wait to trump in the fourth hand
				CSuitHoldings& trumpSuit = bPlayingInHand? playerHand.GetSuit(nTrumpSuit) : dummyHand.GetSuit(nTrumpSuit);
				pCard = trumpSuit.GetBottomCard();
				status << "PLDC2B4! We don't have a winner in ths suit in " & (bPlayingInHand? "dummy" : "hand") & 
						  ", so trump here with the " & pCard->GetFaceName() & ".\n";
			}


		}
		else
		{
			// else we're playing in NT with no cards in suit led, 
			// or playing in a suit but have no trumps left, so simply discard
			pCard = GetDiscard();
			if (ISSUIT(nTrumpSuit))
				status << "PLDC2C1! We don't have any more " & STS(nSuitLed) & 
					      " or trumps in " & (bPlayingInHand? "hand" : "dummy") & 
						  ", so discard the " & pCard->GetFaceName() & ".\n";
			else
				status << "PLDC2C2! We have no more cards in the suit in " & (bPlayingInHand? "hand" : "dummy") & 
						  ", so discard the " & pCard->GetName() & ".\n";
		}
	}

	// done
	ASSERT(pCard->IsValid());
	return pCard;
}



//
// PlayThird()
//
// default code for playing in third position as declarer/dummy
//
CCard* CDeclarerPlayEngine::PlayThird()
{
	// see if we're playing in hand or in dummy
	BOOL bPlayingInHand = (pDOC->GetCurrentPlayer() == m_pPlayer);
	//
	CPlayerStatusDialog& status = *m_pStatusDlg;

	// get play info
	CCard* pLeadCard = pDOC->GetCurrentTrickCardLed();
	CCard* pRHOCard = pDOC->GetCurrentTrickCardByOrder(1);
	int nSuitLed = pLeadCard->GetSuit();
	int nTopPos;
	CCard* pCurrTopCard = pDOC->GetCurrentTrickHighCard(&nTopPos);
	CString strTopCardPos = PositionToString(nTopPos);
//	int nCurrentRound = pDOC->GetPlayRound();
//	int nCurrentSeat = pDOC->GetNumCardsPlayedInRound() + 1;
	CCard* pDeclarersCard = pDOC->GetCurrentTrickCard(GetPlayerPosition());
	CCard* pDummysCard = pDOC->GetCurrentTrickCard(GetPartnerPosition());
	CCard* pPartnersCard = bPlayingInHand? pDummysCard : pDeclarersCard;
	BOOL bPartnerHigh = (pCurrTopCard == pPartnersCard);
	//
	CHandHoldings& hand = bPlayingInHand? *m_pHand : *m_pPartnersHand;
	CCombinedSuitHoldings& combinedSuit = m_pCombinedHand->GetSuit(nSuitLed);
	CSuitHoldings& declarerSuit = combinedSuit.GetDeclarerSuit();
	CSuitHoldings& dummySuit = combinedSuit.GetDummySuit();
	CSuitHoldings& suit = bPlayingInHand? declarerSuit : dummySuit;
	// 
	int nTrumpSuit = pDOC->GetTrumpSuit();
	int numCardsInSuitLed = suit.GetNumCards();
	int numOutstandingTrumps = 0;
	if (ISSUIT(nTrumpSuit))
		numOutstandingTrumps = GetNumOutstandingCards(nTrumpSuit);

	// card to play
	CCard* pCard = NULL;

	// verify that partner is indeed high
	if (bPartnerHigh)
	{
		// see if partner's card will carry the day
		CCardList outstandingCards;
		GetOutstandingCards(nSuitLed, outstandingCards);
		if (outstandingCards.GetNumCardsAbove(pPartnersCard->GetFaceValue()) > 0)
			bPartnerHigh = FALSE;
	}


	// 
	// first see if RHO trumped in this hand
	//
	if ((pRHOCard->GetSuit() == nTrumpSuit) && (nTrumpSuit != nSuitLed))
	{
		// RHO trumped, so see if we can overtrump
		CSuitHoldings& trumpSuit = hand.GetSuit(nTrumpSuit);
		CCard* pTopTrump = NULL;
		if (trumpSuit.GetNumCards() > 0)
			pTopTrump = trumpSuit.GetTopCard();
		if ((numCardsInSuitLed == 0) && pTopTrump && (*pTopTrump > *pCurrTopCard))
		{
			// if we have enough trump winners, play the bottom card of the top sequence
			if (trumpSuit.GetNumTopCards() > numOutstandingTrumps)
			{
				pCard = trumpSuit.GetTopSequence().GetBottomCard();;
				status << "PLYD2A20! We can overtrump " & strTopCardPos & "'s " & pCurrTopCard->GetName() &
						  " with the " & pCard->GetFaceName() & ".\n";
			}
			else
			{
				// get the lowest trump that wil top the current top trump
				int numTopCards = trumpSuit.GetNumCardsAbove(pCurrTopCard);
				pCard = trumpSuit[numTopCards-1];
				status << "PLYD2A21! We can overtrump " & strTopCardPos & "'s " & pCurrTopCard->GetName() &
						  " with the " & pCard->GetFaceName() & ".\n";
			}
		}
		else
		{
			// no chance to win, so discard
			pCard = GetDiscard();
			if ((numCardsInSuitLed == 0) && (trumpSuit.GetNumCards() > 0))
				status << "PLYD2A22! We can't overtrump " & strTopCardPos & "'s " & 
						  pCurrTopCard->GetFaceName() & ", so discard the " & pCard->GetName() & ".\n";
			else
				status << "PLYD2A3! We can't beat the opponent's " & pCurrTopCard->GetFaceName() & 
						  " of trumps, so discard the " & pCard->GetName() & ".\n";
		}
	}
	else
	{
		// else nobody has played a trump this round, _or_ a trump was led
		// see if we can play trumps
		if (numCardsInSuitLed == 1)
		{
			// nope, gotta follow the suit that was led, trumps or otherwise
			pCard = suit[0];
			status << "PLYD2A20! Play our only card in the suit, " & pCard->GetFaceName() & ".\n";
		}
		else if (numCardsInSuitLed >= 2)
		{
			// still gotta follow the suit that was led, trumps or otherwise
			// if we can beat the current top card, do so with the cheapest card
			if (*suit[0] > *pCurrTopCard)
			{
				// beat the opponents' card w/ highest card affordable
				// but check if player 2 has showed out of the suit, 
				// and thereare higher cards outstanding
				CCardList outstandingCards;
				int numOutstandingCards = GetOutstandingCards(nSuitLed, outstandingCards);
				CCard* pTopOutstandingCard = (numOutstandingCards > 0) ? outstandingCards[0] : NULL;
				CGuessedSuitHoldings& Player2Suit = bPlayingInHand? m_ppGuessedHands[m_pRHOpponent->GetPosition()]->GetSuit(nSuitLed) : m_ppGuessedHands[m_pLHOpponent->GetPosition()]->GetSuit(nSuitLed);
				CGuessedSuitHoldings& Player4Suit = bPlayingInHand? m_ppGuessedHands[m_pLHOpponent->GetPosition()]->GetSuit(nSuitLed) : m_ppGuessedHands[m_pRHOpponent->GetPosition()]->GetSuit(nSuitLed);
				CString strPlayer2 = bPlayingInHand? szRHO : szLHO;
				CString strPlayer4 = bPlayingInHand? szLHO : szRHO;
				if (Player2Suit.IsSuitShownOut() && (numOutstandingCards > 0) && (*pTopOutstandingCard > *suit[0]))
				{
					// the 4th player holds a higher card, so try to knock it out
					pCard = suit.GetTopSequence().GetBottomCard();
					status << "PLYD2A30! Since " & strPlayer2 & " has shown out, we know " & strPlayer4 & 
							  " holds a winner in the suit, so play our second highest card (the " & 
							  pCard->GetFaceName() & ".\n";
				}
				else if (Player4Suit.IsSuitShownOut())
				{
					// Player 4 has shown out of the suit
					if (bPartnerHigh)
					{
						// partner's card is high, so discard
						pCard = GetDiscard();
						status << "PLYD2A32! Since " & strPlayer4 & " has shown out, partner's " & pCurrTopCard->GetFaceName() &
								  " is high, so discard the " & pCard->GetFaceName() & ".\n";
					}
					else
					{
						// Player 2 is high, so beat his top card
						pCard = suit.GetLowestCardAbove(pCurrTopCard);
						status << "PLYD2A34! Since " & strPlayer2 & " has shown out, play high to top the " & pCurrTopCard->GetFaceName() &
								  " with the " & pCard->GetFaceName() & ".\n";
					}
				}
				else
				{
					// here, player 2 has not shown out
					// see if partner is high (not literally, of course)
					if (bPartnerHigh)
					{
						// let partner's card ride
						pCard = GetDiscard();
						status << "PLYD2A40! Let the " & pLeadCard->GetName() & " ride and discard the " & pCard->GetName() & ".\n";
					}
					else
					{
						// partner's card is not high -- are there outstanding cards?
						if (numOutstandingCards == 0)
						{
							// no more outstanding cards in suit, so top the opponents' card
							pCard = suit.GetLowestCardAbove(pCurrTopCard);
							status << "PLYD2A42! Play high to win with the " & pCard->GetFaceName() & ".\n";
						}
						else
						{
							// player 4 may or may not have the top outstanding card -- 
							// so play the lowest card that beats the outstanding card, or the highest card otherwise
							CCard* pCardToBeat = (*pTopOutstandingCard > *pCurrTopCard)? pTopOutstandingCard : pCurrTopCard;
							if (suit.GetNumCardsAbove(pCardToBeat) > 0)
								pCard = suit.GetLowestCardAbove(pCardToBeat);
							else
								pCard = suit[0];
							status << "PLYD2A44! Play high to win with the " & pCard->GetFaceName() & ".\n";
						}
					}
				}
			}
			else
			{
				// else we have nothing higher, so discard
				pCard = GetDiscard();
				status << "PLYD2A50! With nothing higher in " & 
						  (bPlayingInHand? "hand" : "dummy") & ", discard the " &
						  pCard->GetFaceName() & ".\n";
			}
		}
		else if (ISSUIT(nTrumpSuit) && (nSuitLed != nTrumpSuit) &&
							(hand.GetNumCardsInSuit(nTrumpSuit) > 0))
		{
			// here, we have zero cards in the suit led, & can trump
			if (bPartnerHigh)
			{
				// partner's card is high, so discard
				pCard = GetDiscard();
				status << "PLYD2A51! Partner's " & pPartnersCard->GetName() & " is high, so discard the " & 
						   pCard->GetName() & ".\n";
			}
			else
			{
				// opponents have the high card (non-trump) -- so slam 'em
				// we can play a trump here, so do so if appropriate 
				// if we have enough trump winners, play bottom card of the top sequence
				CSuitHoldings& trumpSuit = hand.GetSuit(nTrumpSuit);
				if (trumpSuit.GetNumTopCards() > numOutstandingTrumps)
				{
					pCard = trumpSuit.GetTopSequence().GetBottomCard();;
					status << "PLYD2A52! With no cards in the " & SuitToString(nSuitLed) & 
							  " suit, trump with the " & pCard->GetFaceName() & ".\n";
				}
				else
				{
					pCard = hand.GetSuit(nTrumpSuit).GetBottomCard();
					status << "PLYD2A55! With no cards in " & SuitToString(nSuitLed) & 
							  ", trump with the " & pCard->GetName() & ".\n";
				}
			}
		}
		else
		{
			// here we have zero cards in the suit and in trumps, so we're hosed
			pCard = GetDiscard();
			status << "PLYD2A62! With no cards in the suit led " &
			 		  (ISSUIT(nTrumpSuit)? "and no trumps" : "") &
					  ", we discard the " & pCard->GetName() & ".\n";
		}
	}

	//
	ASSERT(pCard->IsValid());
	if (bPlayingInHand)
		ASSERT(m_pHand->HasCard(pCard));
	else
		ASSERT(m_pPartnersHand->HasCard(pCard));
	//
	return pCard;
}



//
// PlayFourth()
//
// default code for playing in fourth position
//
CCard* CDeclarerPlayEngine::PlayFourth()
{
	// see if we're playing in hand or in dummy
	BOOL bPlayingInHand = (pDOC->GetCurrentPlayer() == m_pPlayer);

	// call default code in base class
	if (bPlayingInHand)
		return CPlayEngine::PlayFourth();
	else
		return m_pDummy->PlayFourth();
}





//
// PlayBestCard()
//
// called on the third and fourth hand plays to try to win the trick
//
CCard* CDeclarerPlayEngine::PlayBestCard(int nPosition)
{
	// see if we're playing in hand or in dummy
	BOOL bPlayingInHand = (pDOC->GetCurrentPlayer() == m_pPlayer);

	//
	CPlayerStatusDialog& status = *m_pStatusDlg;
//	status << "PLYE3! Playing best card.\n";

	// get play info
	CCard* pCurrentCard = pDOC->GetCurrentTrickCardLed();
	int nSuitLed = pCurrentCard->GetSuit();
	int nTopPos;
	CCard* pCurrTopCard = pDOC->GetCurrentTrickHighCard(&nTopPos);
	CString strTopCardPos = PositionToString(nTopPos);
	int nCurrentRound = pDOC->GetPlayRound();
	int nCurrentSeat = pDOC->GetNumCardsPlayedInRound() + 1;
	// see what the top card in the round is
	CCard* pTopCard = pDOC->GetCurrentTrickHighCard();
	CCard* pDeclarerCard = pDOC->GetCurrentTrickCard(GetPlayerPosition());
	CCard* pDummysCard = pDOC->GetCurrentTrickCard(GetPartnerPosition());
	CCard* pPartnersCard = bPlayingInHand? pDummysCard: pDeclarerCard;
	BOOL bPartnerHigh = (pTopCard == pPartnersCard);
	// 
	int nTrumpSuit = pDOC->GetTrumpSuit();
	int numCardsInSuitLed = m_pHand->GetNumCardsInSuit(nSuitLed);
	// card to play
	CCard* pCard = NULL;

	// 
	// first see if somebody trumped in this hand
	//
	if ((pDOC->WasTrumpPlayed()) && (nTrumpSuit != nSuitLed))
	{
		// a trump has been played 
		// see whether it was played by partner or by an opponent
		if (bPartnerHigh)
		{
			// partner trumped -- leave it alone for now
			pCard = GetDiscard();
			status << "PLYE10! We let partner's " & pCurrTopCard->GetName() & " trump ride and discard the " &
					  pCard->GetName() & ".\n";
		}
		else
		{
			// it was an opponent that did the trumping
			// see if we can overtrump
			CSuitHoldings& trumpSuit = m_pHand->GetSuit(nTrumpSuit);
			CCard* pTopTrump = NULL;
			if (trumpSuit.GetNumCards() > 0)
				pTopTrump = trumpSuit.GetTopCard();
			if ((numCardsInSuitLed == 0) && (pTopTrump) && (*pTopTrump > *pCurrTopCard))
			{
				// get the lowest trump that wil top the current top trump
				int numTopCards = trumpSuit.GetNumCardsAbove(pCurrTopCard);
				pCard = trumpSuit[numTopCards-1];
				status << "PLYE20! We can overtrump " & strTopCardPos & "'s " & pCurrTopCard->GetName() &
						  " with the " & pCard->GetFaceName() & ".\n";
			}
			else
			{
				// no chance to win, so discard
				pCard = GetDiscard();
				if ((numCardsInSuitLed == 0) && (trumpSuit.GetNumCards() > 0))
					status << "PLYE22! We can't overtrump " & strTopCardPos & "'s " & 
							  pCurrTopCard->GetFaceName() & ", so discard the " & pCard->GetName() & ".\n";
				else
					status << "PLYE23! We can't beat the opponent's " & pCurrTopCard->GetFaceName() & 
							  " of trumps, so discard the " & pCard->GetName() & ".\n";
			}							
		}
	}
	else
	{
		// else nobody has played a trump this round, _or_ a trump was led
		// see if we can play trumps
		if (numCardsInSuitLed > 0)
		{
			// nope, gotta follow the suit that was led, trumps or otherwise
			// if we can beat the current top card, do so with the cheapest card
			CSuitHoldings& suit = m_pHand->GetSuit(nSuitLed);
			if (*(suit.GetTopCard()) > *pCurrTopCard)
			{
				// but see if the top card is partner's
				if (bPartnerHigh) 
				{
					// see if we should unblock here
					if (ISSUIT(nTrumpSuit) && (nCurrentRound == 0) &&
						(suit.GetNumHonors() == 1) && (suit.GetNumCards() <= 2)) // NCR don't need to yet
					{
						// first round in an NT contract, with one honor 
						// in the suit -- unblock
						pCard = suit.GetTopCard();
						if (suit.GetNumCards() > 1)
							status << "PLYE30! Drop the " & pCard->GetFaceName() & 
									  " here to unblock the suit for partner.\n";
					}
					else
					{
						// else this is not an unblocking situation
						if (nCurrentSeat == 4)
						{
							// playing in 4th seat, high card is partner, so discard
							pCard = GetDiscard();
							status << "PLYE34! Partner's " & pCurrTopCard->GetFaceName() & 
									  " is high, so discard the " & pCard->GetName() & ".\n";
						}
						else
						{
							// playing in third position -- decide whether to 
							// let partner's card ride
							// do so if if partner's card beats all outstanding cards
							CCard* pTopOutstandingCard = GetHighestOutstandingCard(nSuitLed);
							if ((pTopOutstandingCard == NULL) || (*pCurrTopCard > *pTopOutstandingCard))
							{
								// let partner's card ride
								pCard = GetDiscard();
								status << "PLYE36! Partner's " & pCurrTopCard->GetFaceName() & 
										  " is higher than any outstanding card, so discard the " & 
										  pCard->GetName() & ".\n";
							}
							else
							{
								// partner's card is not necessarily highest, so top it
								pCard = suit.GetTopSequence().GetBottomCard();
								status << "PLYE37! Partner's " & pCurrTopCard->GetFaceName() & 
										  " might not win the round, so top it with the " & pCard->GetFaceName() & ".\n";
							}
						}
					}
				}
				else
				{
					// else high card is opponent's, so beat it w/ highest card affordable
					// although, if playing in 3rd pos ahead of dummy, just play
					// high enuff to beat dummy
					if ((nCurrentSeat == 3) && (m_bLHDefender))
					{
						CSuitHoldings& dummySuit = GetDummySuit(nSuitLed);
						int nDummyTopCard = 0;
						if (dummySuit.GetNumCards() > 0)
							nDummyTopCard = dummySuit[0]->GetFaceValue();
						int nTopVal = Max(nDummyTopCard, pCurrTopCard->GetFaceValue());
						pCard = suit.GetLowestCardAbove(nTopVal);
						// see if we can beat the top card or dummy's top card
						if (pCard)
						{
							if (nTopVal == nDummyTopCard)
							{
								// dummy has the top card and we can beat it
								status << "PLYE38A! Playing third ahead of dummy, need to beat dummy's " & 
										  CardValToString(nDummyTopCard) & ".\n";
							}
							else
							{
								// the top card is declarer's
								status << "PLYE38B! Play high to win with the " & pCard->GetFaceName() & ".\n";
							}
						}
						else
						{
							// else we can't beat dummy's top card, but play
							// high anyway to force out his winner
							pCard = suit.GetLowestCardAbove(pCurrTopCard);
							status << "PLYE38C! We top declarer's " & pCurrTopCard->GetFaceName() &
								      " to force a winner from dummy.\n";
						}
					}
					else if (nCurrentSeat == 3) 
					{
						// else we're playing 3rd, so play the lowest card from the top sequence
						pCard = suit.GetTopSequence().GetBottomCard();
						status << "PLYE40! Play high to win with the " & pCard->GetFaceName() & ".\n";
					}
					else
					{
						// else we're playing last (4th)
						// play the cheapest card that will beat the top card
						pCard = suit.GetLowestCardAbove(pCurrTopCard);
						// NCR-211 Problem: Low card can be an entry to other hand
						status << "PLYE41! Play the " & pCard->GetFaceName() & " to win the trick.\n";
					}
				}
			}
			else
			{
				// we don't have a card to top the current high card
				if (bPartnerHigh)
				{
					// but partner's card is high, so we're OK
					pCard = GetDiscard();
					status << "PLYE47! Partner's " & pCurrTopCard->GetFaceName() & 
							  " can win the trick, so discard the " & pCard->GetName() & ".\n";
				}
				else
				{
					// else we're screwed
					pCard = GetDiscard();
					status << "PLYE48! We can't beat " & strTopCardPos & "'s " & pCurrTopCard->GetFaceName() & 
							  ", so discard the " & pCard->GetName() & ".\n";
				}
			}
		}
		else if (ISSUIT(nTrumpSuit) && (nSuitLed != nTrumpSuit) &&
							(m_pHand->GetNumCardsInSuit(nTrumpSuit) > 0))
		{
			// here, we can play a trump, so do so if appropriate 
			// see who has the top card in this round
			if (bPartnerHigh)
			{
				// let partner's card ride
				pCard = GetDiscard();
				status << "PLYE52! Although we could trump this hand, partner's " & pCurrTopCard->GetName() & 
						  " is high, so discard the " & pCard->GetName() & ".\n";
			}
			else
			{
				// opponents have the high card (non-trump) -- so slam 'em
				pCard = m_pHand->GetSuit(nTrumpSuit).GetBottomCard();
				status << "PLYE55! With no cards in " & SuitToString(nSuitLed) & 
						  ", trump with the " & pCard->GetName() & ".\n";
			}
		}
		else
		{
			// here we have zero cards in the suit and in trumps, so we're hosed
			pCard = GetDiscard();
			if (ISSUIT(nTrumpSuit))
				status << "PLYE52! With no cards in the suit led and no trumps, we discard the " & pCard->GetName() & ".\n";
			else
				status << "PLYE52a! With no cards in the suit led, we discard the " & pCard->GetName() & ".\n";
		}
	}
	//
	ASSERT(pCard->IsValid());
	ASSERT(m_pHand->HasCard(pCard));
	//
	return pCard;
}





//
// GetDiscard()
//
CCard* CDeclarerPlayEngine::GetDiscard()
{
	//
	CPlayerStatusDialog& status = *m_pStatusDlg;
	//
	CCard* pLeadCard = pDOC->GetCurrentTrickCardLed();
	int nSuitLed = pLeadCard->GetSuit();
	int nTrumpSuit = pDOC->GetTrumpSuit();
	bool bPlayingInHand = (pDOC->GetCurrentPlayer() == m_pPlayer);
	CCard* pCard = NULL;


	//
	// first see if we have priority discards
	//
 	if (bPlayingInHand && (m_declarerPriorityDiscards.GetNumCards() > 0))
	{
		// see if we're void in the suit led
		if (m_pHand->GetNumCardsInSuit(nSuitLed) == 0)
		{
			// return the first card in the discard list
			CCard* pCard = m_declarerPriorityDiscards.RemoveByIndex(0);
			status << "3PLYDCS0! Use this opportunity to discard the " & pCard->GetName() & ".\n";
			m_declarerDiscardedPriorityDiscards << pCard;
			return pCard;
		}
		else
		{
			// else see if there's a card of the suit led that we want to discard
			int numCards = m_declarerPriorityDiscards.GetNumCards();
			for(int i=0;i<numCards;i++)
			{
				// still not sure why the second part is necessary, but it prevents crashes
				if (m_declarerPriorityDiscards[i]->GetSuit() == nSuitLed)
				{
					// check to see if the priority discard hasn't been discarded already
					CCard* pCard = m_declarerPriorityDiscards[i];
					if (!m_declarerDiscardedPriorityDiscards.HasCard(pCard))
					{
						// pull out and return the card
						m_declarerPriorityDiscards.RemoveByIndex(i);
						CSuitHoldings& suit = m_pHand->GetSuit(pCard->GetSuit());
						if (suit.GetNumCards() > 1)
							status << "3PLYDCS1! Use this opportunity to discard the " & pCard->GetName() & ".\n";
						m_declarerDiscardedPriorityDiscards << pCard;
						return pCard;
					}
				}
			}
		}
	}
	else if (!bPlayingInHand && (m_dummyPriorityDiscards.GetNumCards() > 0))
	{
		// playing in dummy -- see if we're void in the suit led
		if (m_pPartnersHand->GetNumCardsInSuit(nSuitLed) == 0)
		{
			// return the first card in the discard list
			CCard* pCard = m_dummyPriorityDiscards.RemoveByIndex(0);
			status << "3PLYDCS5! Use this opportunity to discard the " & pCard->GetName() & ".\n";
			m_dummyDiscardedPriorityDiscards << pCard;
			return pCard;
		}
		else
		{
			// else see if there's a card of the suit led that we want to discard
			int numCards = m_dummyPriorityDiscards.GetNumCards();
			for(int i=0;i<numCards;i++)
			{
				if (m_dummyPriorityDiscards[i]->GetSuit() == nSuitLed)
				{
					// return the found card in the discard list
					CCard* pCard = m_dummyPriorityDiscards.RemoveByIndex(i);
					if (!m_dummyDiscardedPriorityDiscards.HasCard(pCard))
					{
						CSuitHoldings& suit = m_pHand->GetSuit(pCard->GetSuit());
						if (suit.GetNumCards() > 1)
							status << "3PLYDCS6! Use this opportunity to discard the " & pCard->GetName() & ".\n";
						m_dummyDiscardedPriorityDiscards << pCard;
						return pCard;
					}
				}
			}
		}
	}


	//
	// else we have no priority discards, so see what we should discard
	//
//	CSuitHoldings* pDiscardSuit = NULL;

	// see if we have cards in the suit led
	CCombinedHoldings& combinedHand = *m_pCombinedHand;
	CHandHoldings& declarerHand = *(combinedHand.GetPlayerHand());
	CHandHoldings& dummyHand = *(combinedHand.GetPartnerHand());
	CHandHoldings& hand = bPlayingInHand? declarerHand : dummyHand;
	//
	CCombinedSuitHoldings& combinedSuit = m_pCombinedHand->GetSuit(nSuitLed);
	CSuitHoldings& declarerSuit = declarerHand.GetSuit(nSuitLed);
	CSuitHoldings& dummySuit = dummyHand.GetSuit(nSuitLed);
	CSuitHoldings& suit = bPlayingInHand? declarerSuit : dummySuit;
	CSuitHoldings& oppositeSuit = bPlayingInHand? dummySuit : declarerSuit;
	int numOppositeEntries = bPlayingInHand ? m_numDummyEntries : m_numDeclarerEntries; // NCR-745

	// see if we have cards in the suit led
	if (suit.GetNumCards() > 0)
	{
		// we have to follow suit
		// in certain cases, we may want to unblock and not discard the lowest card
		// specifically, if we have no losers in the combined suit, and are 
		// discarding from the shorter hand, and 
		if ((combinedSuit.GetNumLosers() == 0) 
			&& (oppositeSuit.GetNumCards() >= suit.GetNumCards()) )
		{
			// unblock here
			if (suit.GetNumCardsBelow(pLeadCard) > 0)
			{
				pCard = suit.GetHighestCardBelow(pLeadCard);
				// NCR-482 Test that this card is not THE winner
				if((combinedSuit.GetNumOutstandingCards() == 0) 
					|| (pCard->GetFaceValue() < combinedSuit.GetMissingCardVal(0)) )
				{
					status << "3PLYDCS2! Discard the " & pCard->GetName() & " to unblock the suit.\n";
					return pCard;
				}
				else
					pCard = NULL; // NCR-482 Don't want to use this card
			}
		}

		// NCR-421 Unblock if suit has two cards, one is the top card in suit and opposite has more cards
		if ((suit.GetNumCards() == 2) && !oppositeSuit.IsVoid() 
			&& (suit.GetTopCardVal() > oppositeSuit.GetTopCardVal())
//			&& (combinedSuit.GetTopSequence().GetNumCards() == 2) <<<< Need to consider what has been played ???
			&& (oppositeSuit.GetNumCards() >= suit.GetNumCards()) 
			// NCR-478 Test if other hand has a winner before discarding a possible winner
			&& (combinedSuit.GetSequence2(0).GetNumCards() > 1) 
			// NCR-745 don't worry about blocking if opposite hand has entries
			&& (numOppositeEntries < 2) )
		{
			pCard = suit.GetTopCard();
			status << "3PLYDCS3! Discard the " & pCard->GetName() & " to unblock the suit.\n";
			return pCard;
		}  // NCR-421 end unblocking

		// NCR-475 Beat if playing second and our cards bracket lead card
		if(pDOC->GetNumCardsPlayedInRound() == 1)
			pCard = suit.BeatIfBracketed(pLeadCard);  // NCR-475 Beat if our cards bracket card lead

		// else just return the lowest card 
		if(pCard == NULL)
			pCard = suit.GetBottomCard();
		return pCard;
	}

	// here we have no more cards in the suit, so discard from another suit
	// but first see whether we're playing with a trump suit (and have trumps left)
	if ((ISSUIT(nTrumpSuit)) && (hand.GetNumTrumps() > 0))
	{
		// playing with a trump suit
		// see if we have any trumps left
		if (hand.GetNumCardsInSuit(nTrumpSuit) > 0)
		{
			// yes indeed, we do have at least one trump left (but not trumping here)
			pCard = GetFinalDiscard();
		}
		else
		{
			// just use the same code for now
			pCard = GetFinalDiscard();
		}
	}
	else
	{
		// else we're playing notrumps, OR we're out of trump cards
		pCard = GetFinalDiscard();
	}

	// verify the selected card is OK
	ASSERT(pCard->IsValid());
	if (bPlayingInHand)
		ASSERT(declarerHand.HasCard(pCard));
	else
		ASSERT(dummyHand.HasCard(pCard));
	return pCard;
		
}



// 
// GetFinalDiscard()
//
CCard* CDeclarerPlayEngine::GetFinalDiscard()
{

	//
	// standard discard routine -- declarer/dummy discards from the worst suits
	// playing no trumps -- generally return the bottom card of the worst suit
	//
	bool bPlayingInHand = (pDOC->GetCurrentPlayer() == m_pPlayer);
	CCombinedHoldings& combinedHand = *m_pCombinedHand;
	CHandHoldings& declarerHand = *(combinedHand.GetPlayerHand());
	CHandHoldings& dummyHand = *(combinedHand.GetPartnerHand());
	CHandHoldings& hand = bPlayingInHand? declarerHand : dummyHand;
	int nTrumpSuit = pDOC->GetTrumpSuit();
	CSuitHoldings* pDiscardSuit = NULL;

	// NCR-33 Special case- dummy is leading, his card will win and he will lead the next card
	// We should NOT discard the winning card in that suit
	// ??? can this be generalized?  
	// ??? do we need this also for when playing in Dummy?
	if(bPlayingInHand && (dummyHand.GetNumCards() == 1)) {
		int nSuit = dummyHand.GetLongestSuit();  // this is also the only suit
//		CCardList outstandingCards;
//		GetOutstandingCards(nSuit, outstandingCards);
        CSuitHoldings& rSuit = declarerHand.GetSuit(nSuit);
		// Test if our top card in suit is top card outstanding
		if(rSuit.GetNumWinners() == 1) {  // rely on other code to tell us this
			// if so, discard from another suit
			for(int xSuit = CLUBS; xSuit <= SPADES; xSuit++){
				if((xSuit == nSuit) || (xSuit == nTrumpSuit)) // NCr-465 skip trumps also
					continue;		// skip suit with winner
				if(declarerHand.GetSuit(xSuit).GetNumCards() > 0)
					return declarerHand.GetSuit(xSuit).GetBottomCard();  // this will do
			} // end for(xSuit)	
		}
	} // end NCR-33 special case - testing for sure winner

	//
	// first sort the suits by losers
	//
	int nSuitsByLosers[4] = { CLUBS, DIAMONDS, HEARTS, SPADES, };
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<4;i++)
	{
		for(int j=0;j<3;j++)
		{
			BOOL bSwap = FALSE;
			CCombinedSuitHoldings& combinedSuit1 = combinedHand.GetSuit(nSuitsByLosers[j]);
			CCombinedSuitHoldings& combinedSuit2 = combinedHand.GetSuit(nSuitsByLosers[j+1]);
			int numWinners1 = bPlayingInHand? combinedSuit1.GetNumDeclarerWinners() : combinedSuit1.GetNumDummyWinners();
			int numWinners2 = bPlayingInHand? combinedSuit2.GetNumDeclarerWinners() : combinedSuit2.GetNumDummyWinners();
			int numLosers1 = bPlayingInHand? combinedSuit1.GetNumDeclarerLosers() : combinedSuit1.GetNumDummyLosers();
			int numLosers2 = bPlayingInHand? combinedSuit2.GetNumDeclarerLosers() : combinedSuit2.GetNumDummyLosers();
			int numCards1 = bPlayingInHand? combinedSuit1.GetNumDeclarerCards() : combinedSuit1.GetNumDummyCards();
			int numCards2 = bPlayingInHand? combinedSuit2.GetNumDeclarerCards() : combinedSuit2.GetNumDummyCards();
			int numOppositeCards1 = bPlayingInHand? combinedSuit1.GetNumDummyCards() : combinedSuit1.GetNumDeclarerCards();
			int numOppositeCards2 = bPlayingInHand? combinedSuit2.GetNumDummyCards() : combinedSuit2.GetNumDeclarerCards();

			// comparisons used for selecting a discard suit
			// 1: if a suit has more losers than another suit, with the same or fewer winners
			// 2: if both have the same # of losers, but a suit has fewer winners
			// 3: if a suit has no winners and the other one does
			if ( ((numLosers2 > numLosers1) && (numWinners2 <= numWinners1)) ||
				 ((numLosers1 == numLosers2) && (numWinners1 > numWinners2)) ||
				 ((numWinners1 > 0) && (numWinners2 == 0)) )
				 bSwap = TRUE;	// discard suit 2 before suit 1

			// new code -- if we have a suit in this hand that has losers, and
			// with no suit cards in the oposite hand, try to discard from it first
			// this is because we don't need it for entries
			if ( (numLosers2 > 0) && (numOppositeCards2 == 0) && 
//					 ((numOppositeCards1 >= 1) || (Losers2 > numLosers1)) )
				 ((numOppositeCards1 >= 1) && (numWinners1 >= 1)) )
				 bSwap = TRUE;

			// also, if two suits both have zero losers, discard from the
			// longer hand first
			if ((numLosers1 == 0) && (numLosers2 == 0) && (numCards1 > numCards2))
				bSwap = TRUE;

			//
			if (bSwap)
			{
				// move the first suit back by one (i.e., swap the suits)
				// and favor the second suit when discarding
				int nTemp = nSuitsByLosers[j];
				nSuitsByLosers[j] = nSuitsByLosers[j+1];
				nSuitsByLosers[j+1] = nTemp;
			}
		}
	}

	//
	int nDiscardSuits[4];
	int numDiscardSuits = 0;

	// then sift through the suits to find one that is not a priority suit and 
	// has losers
	for(i=0;i<4;i++)
	{
		// check out the suit
		int nSuit = nSuitsByLosers[i];
		if (hand.GetNumCardsInSuit(nSuit) == 0)
			continue;

		// see if this is a trump suit
		if (nSuit == nTrumpSuit)
			continue;

		// but see if this is the priority suit, or the suit has zero losers
		CCombinedSuitHoldings& combinedSuit = combinedHand.GetSuit(nSuit);
		int numLosers = bPlayingInHand? combinedSuit.GetNumDeclarerLosers() : combinedSuit.GetNumDummyLosers();
		if ( (ISSUIT(nSuit) && ((nSuit == m_nPrioritySuit) || (nSuit == m_nPartnersPrioritySuit))) || 
			                 
		 	 (numLosers == 0))
		{
			// don't discard from this suit if possible;
			continue;
		}

		// see if this suit has only one card, which is the only entry in the 
		// suit into the opposite hand, which has winners
		CSuitHoldings& suit = bPlayingInHand? combinedSuit.GetDeclarerSuit() : combinedSuit.GetDummySuit();
		CSuitHoldings& oppositeSuit = bPlayingInHand? combinedSuit.GetDummySuit() : combinedSuit.GetDeclarerSuit();
		int numEntries = bPlayingInHand? m_numDummyEntriesInSuit[nSuit] : m_numDeclarerEntriesInSuit[nSuit];
		if ((suit.GetNumCards() == 1) && (oppositeSuit.GetNumWinners() > 0) && (numEntries == 1))
			continue;

		// NCR-414 Don't discard if this hand is longest of a long suit
		if( (combinedSuit.GetNumTopCards() >= combinedSuit.GetNumOutstandingCards())
			&& (suit.GetNumCards() > oppositeSuit.GetNumCards()) )
			continue; // NCR-414 don't discard from longest side

		// NCR-174 Don't discard from if will unguard an honor
//        if ((suit.GetTopCard()->GetFaceValue() > TEN)  // NCR-400 only if topped by an honor
		if(suit.GetNumMissingAbove(suit.GetTopCard()->GetFaceValue()) >= (suit.GetNumCards()-1) // need one extra card to guard
			// NCR-419 Ok to discard if combined suit has stopper 
			&& !((combinedSuit.GetNumOutstandingCards() == 0) 
					// NCR-419 or if they are ALL losers
//NCR-629			     || (combinedSuit.GetMaxLength() < combinedSuit.GetNumOutstandingCards())
			     || (combinedSuit.GetTopCardVal() > combinedSuit.GetMissingCardVal(0))) ) 
			continue;

		// test the suit for winners
//		CCombinedSuitHoldings& testSuit = combinedHand.GetSuit(nSuit);  // NCR changed testSuit to combinedSuit
//		numLosers = bPlayingInHand? combinedSuit.GetNumDeclarerLosers() : combinedSuit.GetNumDummyLosers(); // NCR done above
		if (numLosers > 0)
		{
			nDiscardSuits[numDiscardSuits] = nSuit; // save as possible
			numDiscardSuits++;
		}
	} // end for(i) thru suits

	// look for the first suit in the eligible list that doesn't have plays
	// and is not a trump suit
	if (numDiscardSuits > 1)
	{
		for(i=0;i<numDiscardSuits;i++)
		{
			int nSuit = nDiscardSuits[i];
			int nNumRuffs = 0;  // NCR-400 Only so many Ruffs are winners

			// see if there are plays in this suit
			int numPlays = m_playPlan.GetSize();

			int j; // NCR-FFS added here, removed below
			for(/*int*/ j=0;j<numPlays;j++)
			{
				// search until we find a winning play in this suit
				CPlay* pPlay = m_playPlan[j];
				if ((pPlay->GetSuit() == nSuit) && pPlay->IsWinner()) 
				{
					// NCR-400 Ruffs of suit only for number of trumps After pulling
					if(pPlay->GetPlayType() == CPlay::RUFF) {
						nNumRuffs++; // count ruffs
						continue;  // NCR-400 not too many, need trumps to pull???
					}
					break;
				}
			} // end for(j) thru plays

			// did we go through the whole list without finding a play in this suit?
			// NCR-400 NEED TO Consider number of trumps out vs number that can be used to ruff
			// and the number that will be left for ruffing
			if ((j == numPlays) && (nNumRuffs != 1))  // NCR-400 allow some(??) ruffs
			{
				// found a suit!
				break;
			}
		} // end for(i) thru discard suits

		// did we find a suit?
		if (i < numDiscardSuits)
		{
			pDiscardSuit = &hand.GetSuit(nDiscardSuits[i]);
		}
		else
		{
			// NCR chose longest or one without possible winner or ???
			// NCR Q&D for only 2 suits
			CSuitHoldings* dscrdSuit0 = &hand.GetSuit(nDiscardSuits[0]); 
			CSuitHoldings* dscrdSuit1 = &hand.GetSuit(nDiscardSuits[1]); 
			if(dscrdSuit0->GetNumCards() > dscrdSuit1->GetNumCards())
			{
				pDiscardSuit = dscrdSuit0;
			}
			else if(dscrdSuit1->GetNumCards() > dscrdSuit0->GetNumCards())
			{
				pDiscardSuit = dscrdSuit1;
			}
			else
			{  // NCR same length - need code here to chose ???
				// just give up and take the first suit
				pDiscardSuit = &hand.GetSuit(nDiscardSuits[0]);
			}
		}
	}
	else if (numDiscardSuits == 1)
	{
		pDiscardSuit = &hand.GetSuit(nDiscardSuits[0]);
	}

	// if we still found nothing, call the base class
	if (pDiscardSuit == NULL)
		pDiscardSuit = &hand.GetSuit(PickFinalDiscardSuit(hand));
	ASSERT(pDiscardSuit != NULL);

	// and get the card
	return pDiscardSuit->GetBottomCard();
}





////////////////////////////////////////////////////////////////////////////////
//
//
// Planning routines
//
//
////////////////////////////////////////////////////////////////////////////////






//
//-----------------------------------------------------------------------
//
// PlanPlay()
//
void CDeclarerPlayEngine::PlanPlay()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;

	// starting as declarer
	status << "2PLN00! Dummy's hand has been laid down; now planning the play with a contract of " & 
			  ContractToFullString(m_nContract, m_nContractModifier) & ".\n---------------------------\nPlanning the play...\n";

	// first assemble the combined hand
	// ok to have access to partner's hand cuz he's dummy
	m_pCombinedHand->InitNewHand();
	m_pCombinedHand->EvaluateHoldings();
	m_declarerPriorityDiscards.Clear();
	m_dummyPriorityDiscards.Clear();
	m_declarerDiscardedPriorityDiscards.Clear();
	m_dummyDiscardedPriorityDiscards.Clear();
	m_declarerEntries.Clear();
	m_dummyEntries.Clear();
	m_declarerRuffingEntries.Clear();
	m_dummyRuffingEntries.Clear();
	m_playPlan.Clear();
	m_pCurrentPlay = NULL;

	//
	m_numSureTricks = m_pCombinedHand->GetNumMaxWinners();
	status << "2PLN10! We have" & 
			  ((m_numSureTricks < m_numRequiredTricks)? " only " : " ") &
			  m_numSureTricks & " sure " & 
			  ((m_numSureTricks > 1)? "tricks" : "trick") &
			  " against a requirement of " & m_numRequiredTricks & 
			  " for the " & ContractToString(m_nContract, m_nContractModifier) & " contract.\n";

	// plan the play
	if (m_nTrumpSuit != NOTRUMP)
	{
		// planning in a suit contract
		CreateSuitContractPlayPlan();
	}
	else
	{
		// planning in a Notrump contract
		CreateNoTrumpPlayPlan();
	}

	// see which suit we should prioritize
	PickPrioritySuit();

	// then resequence the plays if necessary
	SequencePlays(TRUE);

	// now interleave the plays

	InterleavePlays();  // NCR moved here from end of SequencePlays()


	// done planning
	m_playPlan.Activate();
	status.SetStreamOutputLevel(3);
	status << "3PLN90! The tentative list of available plays is as follows:\n------>\n";
	ShowPlayList(m_playPlan); // NCR-760
/*
	CString strDescription;
	m_numPlannedRounds = m_playPlan.GetSize();
	for(int i=0;i<m_numPlannedRounds;i++)
	{
		strDescription = m_playPlan[i]->GetFullDescription();
		status << " Play " & i+1 & ": " & strDescription & "\n";
	}
*/
	//	status << "PLN92! The plan provides for " & m_numTotalPlannedTricks &
//			  " or more tricks versus the required " & m_numRequiredTricks & ".\n";
//	if (m_numTotalPlannedTricks < m_numRequiredTricks)
//		status << "PLN93! The remaining " & m_numTricksShort & 
//			      " trick" & ((m_numTricksShort > 1)? "s " : " ") &
//				  " will have to be developed during play.\n";
	status << "3PLN95! The planning is complete.\n";
	status << "---------------------------\n";
	status.ClearStreamOutputLevel();

	// done 
}




//
// ReviewPlayPlan()
//
void CDeclarerPlayEngine::ReviewPlayPlan()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	int numTricksLeftToPlay = (13-pDOC->GetNumTricksPlayed()); // NCR-429

	// starting as declarer
	status << "2PLNADJ! Reviewing play plan for round " & (pDOC->GetNumTricksPlayed()+1) & "...\n";
	m_declarerPriorityDiscards.Clear();
	m_dummyPriorityDiscards.Clear();
	m_declarerDiscardedPriorityDiscards.Clear();
	m_dummyDiscardedPriorityDiscards.Clear();
	m_playPlan.Clear();
	m_pCurrentPlay = NULL;


	//
	int nTeam = ::GetPlayerTeam(m_pPlayer->GetPosition());
	m_numTricksMade = pDOC->GetValue(tnumTricksWon, nTeam);
	m_numTricksLeftToBeMade =  m_numRequiredTricks - m_numTricksMade;
	// NCR-429 Note: We're down if m_numTricksLeftToBeMade > tricks left to be played 
//	ASSERT(m_numTricksLeftToBeMade <= numTricksLeftToPlay); // NCR verify trick logic

	// NCR-444 Which of the following two to use? see NCR-190 and NCR-240
//	m_numSureTricks = m_pCombinedHand->GetNumMaxWinners();
	m_numSureTricks = m_pCombinedHand->GetNumMaxTopCards();  // Gives better results ???
	m_numSureTricks = m_pCombinedHand->GetNumSureWinners() ; // NCR-587 new improved count
	m_numTricksShort = m_numTricksLeftToBeMade - m_numSureTricks;

#ifdef _DEBUG   // NCR-587 Need to get number of sure tricks
	status << "DEBUG1! NumMaxWinners=" & m_pCombinedHand->GetNumMaxWinners() 
		      & " vs SureTricks=" & m_numSureTricks 
			  & " vs NumWinners=" & m_pCombinedHand->GetNumWinners() 
			  & ", m_numTricksShort=" & m_numTricksShort & "\n";
#endif

	if (m_numTricksLeftToBeMade > 0)
	{
		// still tricks we need to make
		if (m_numSureTricks < m_numTricksLeftToBeMade)
			status << "2PLRV10! We have only " & m_numSureTricks & " reasonably sure " & 
					  ((m_numSureTricks > 1)? "tricks" : "trick") &
					  " left vs. " & m_numTricksLeftToBeMade & " more trick(s) required for the contract.\n";
		else 
			status << "2PLRV11! We have " & m_numSureTricks & " reasonably sure " & 
					  ((m_numSureTricks > 1)? "tricks" : "trick") &
					  " left vs. " & m_numTricksLeftToBeMade & " more trick(s) required for the contract.\n";
	}
	else
	{
		// contract has already been fulfilled
		if (m_numSureTricks > 0)
			status << "2PLRV12! We have " & m_numSureTricks & " sure " & 
					  ((m_numSureTricks > 1)? "tricks" : "trick") &
					  " left, though the contract has been fulfilled.\n";
		else
			status << "2PLRV13! We have no more sure tricks left, but the contract has been fulfilled.\n";
	}

	//
	if (m_nTrumpSuit != NOTRUMP)
	{
		// planning in a suit contract
		CreateSuitContractPlayPlan();

	}
	else
	{
		// planning in a Notrump contract
		CreateNoTrumpPlayPlan();
	}

	// NCR-404 Should m_nPrioritySuit be reset???
	if(m_nPrioritySuit != NONE) {
		PickPrioritySuit();  // NCR-404 go get new one
	}


#ifdef _DEBUG  // NCR-38  following for debug
	if((m_playPlan.GetSize() > 0) && (theApp.GetValue(tnAnalysisTraceLevel) > 4)) 
	{
		status << "5PLRV50! The unsorted list of available plays is as follows:\n------>\n";
		ShowPlayList(m_playPlan);
/*
		CString strDesc, reqStr;
		for(int ix=0;ix<m_playPlan.GetSize();ix++)
		{
			CCardList* pReqList = m_playPlan[ix]->GetRequiredPlayedCardsList(); 
			bool bHasReqPlay = pReqList != NULL;
			strDesc = m_playPlan[ix]->GetFullDescription(); 
			if(bHasReqPlay) {  // Build string showing number required
				reqStr.Format(" <Requires: %d card(s)>",pReqList->GetNumCards());
				strDesc += reqStr;
			}
			status << " Play " & ix+1 & ": " & strDesc &  "\n";
		}
*/
	}
#endif  // NCR-38 end 

	// resequence the plays if necessary
	SequencePlays(FALSE);

	// now interleave the plays
	InterleavePlays();  // NCR moved here from end of SequencePlays()

	// done planning
	m_playPlan.Activate();
	status.SetStreamOutputLevel(4);
	status << "PLRV90! The revised list of available plays is as follows:\n------>\n";
	ShowPlayList(m_playPlan); // NCR-760
/*
	CString strDescription;
	m_numPlannedRounds = m_playPlan.GetSize();
	for(int i=0;i<m_numPlannedRounds;i++)
	{
		strDescription = m_playPlan[i]->GetFullDescription();
		status << " Play " & i+1 & ": " & strDescription & "\n";
	}
*/
	//
	status << "PLRV95! The play plan review is complete.\n";
	status << "---------------------------\n";
	status.ClearStreamOutputLevel();

	// done 
}





//
//-----------------------------------------------------------------------
//
// CreateNoTrumpPlayPlan()
//
// create a no trump play plan
//
int CDeclarerPlayEngine::CreateNoTrumpPlayPlan()
{
	// our choices, in order if preference, are:
	// 1) Hold up, if necessary,
	// 2) Finesse, if necessary,
	// 3) Play for a drop (if the odds are in our favor),
	// 4) Develop a suit, and 
	// 5) Cash high cards
	//
	CPlayerStatusDialog& status = *m_pStatusDlg;
	// set verbosity level
	if (pDOC->GetNumTricksPlayed() == 0)
		status.SetStreamOutputLevel(4);
	else
		status.SetStreamOutputLevel(5);

	m_numTotalPlannedTricks = 0;


	//
	//---------------------------------------------------------
	// first hold up, if appropriate
	//
	CPlayList holdUpPlays;
	FindHoldUpPlays(holdUpPlays);
	FilterPlays(holdUpPlays);
	//
	int numHoldUpPlays = holdUpPlays.GetSize();
	m_playPlan << holdUpPlays;
	m_numPlannedHoldUps += numHoldUpPlays;

	
	//
	//---------------------------------------------------------
	// then look at playing for a drop
	//
	CPlayList dropPlays;
	FindDropPlays(dropPlays);
	FilterPlays(dropPlays);
	//
	int numPotentialDropTricks = dropPlays.GetSize();
	m_playPlan << dropPlays;
	m_numPlannedDropPlays = numPotentialDropTricks;
	m_numTotalPlannedTricks += numPotentialDropTricks;


	//
	//---------------------------------------------------------
	// then look at possible finesses
	//
	CPlayList finessePlays;
	FindFinessePlays(finessePlays);
	FilterPlays(finessePlays);
	int numPotentialFinesses = finessePlays.GetSize();
	//
	// NCR-38 Hold off finessing if unstopped suits that we're short in
	// We don't dare lose control if we have a long, unstopped suit out against us
	bool bOkToFinesse = true;
	int numStopped = m_pCombinedHand->GetNumSuitsStopped();   // NCR-700 Added test of num Unstopped
	if((numStopped < 4) && (m_numTricksShort <= 1) && (m_pCombinedHand->GetValue(tnumSuitsUnstopped) > 0)) 
	{
		// NCR-587 Add tests for >1 possible suits
		int nbrToTest = 4 - numStopped;
		int nbrStopped = 0; // count number we sort of have stopped
		for(int i=0; i < nbrToTest; i++)
		{
			int nUSS = m_pCombinedHand->GetValue(tnUnstoppedSuits, i);  // get next one, could be NONE
			if(ISSUIT(nUSS))
			{
				// Don't finesse if more than 4 outstanding cards
				if((m_pCombinedHand->GetSuit(nUSS).GetNumOutstandingCards() 
								 - m_pCombinedHand->GetSuit(nUSS).GetNumCards()) < 4)
					nbrStopped++; // count
			}
		} // end for(i)
		bOkToFinesse = (nbrStopped == nbrToTest); // NCR-587 Ok if all sort of stopped 

		// NCR-704 Another test looking at each suit
		if(!bOkToFinesse) {
			bOkToFinesse = true; // assume ok else
			for(int nxSuit=0; nxSuit < 4; nxSuit++) {
				CCombinedSuitHoldings& theSuit = m_pCombinedHand->GetSuit(nxSuit);
				if(!theSuit.IsSuitStopped() || ! theSuit.IsSuitProbablyStopped()) {
					if((theSuit.GetNumOutstandingCards() - theSuit.GetNumCards()) > 3) {
						bOkToFinesse = false; // failed tests
					}
				}
			} // end for(nxSuit)
		}  // NCR-704 end test
	}
/*   NCR-703 moved down so to add finesses later instead of having to remove them
	if(bOkToFinesse)  // NCR-38 finesse if all 4 stopped or we have enough
	{
		m_playPlan << finessePlays;
		m_numPlannedFinesses += numPotentialFinesses;
	}
	else {
		status << "3PLNCNT0! Skipping using finesses because of possiblity of losing control.\n";
	} // NCR-38 end Q&D to not finesse if there are unstopped suits
*/
	// NCR-779 Don't Force if too many losers which would set the contract
	// if #OS cards > top of unstopped suit is >= # cards in thatsuit
	// func(#OS cards) > max losers allowed  where func() = (#OS/2)+1
	bool bOkToForce = true;  // assume the best
	if((numStopped < 4) && ((m_nTrumpSuit == NOTRUMP) || (m_numTotalTrumps == 0)) ) 
	{
		int nbrToTest = 4 - numStopped;
		for(int i=0; i < nbrToTest; i++)
		{
			int nUSS = m_pCombinedHand->GetValue(tnUnstoppedSuits, i);  // get next one, could be NONE
			if(ISSUIT(nUSS))
			{
				CCombinedSuitHoldings& aSuit = m_pCombinedHand->GetSuit(nUSS);
				int numOS = aSuit.GetNumOutstandingCards();
				int numAboveTop = (aSuit.GetNumCards() > 0) ? aSuit.GetNumMissingAbove(aSuit.GetTopCard()->GetFaceValue())
															: numOS;
				// Check if too many higher cards
				if((numAboveTop >= aSuit.GetNumCards() && (numOS > 0)) 
					&& ((m_numTricksLeftToBeMade - m_numSureTricks) < (numOS/2)+1)) {
					bOkToForce = false;   // too many to try a force
					status << "5PLNCNT1! Skipping using Forces because of possiblity of losing too many tricks."
						      & " NumOS=" & numOS & ", numAboveTop=" & numAboveTop 
							  & ", gap=" & (m_numTricksLeftToBeMade - m_numSureTricks) 
							  & "\n";
				}
			}
		} // end for(i)

	}  // end have unstopped suits NCR-779

	//
	//---------------------------------------------------------
	// then look at developing a suit 
	//
	CPlayList forcePlays, secondaryCashPlays;
	FindSuitDevelopmentPlays(forcePlays, secondaryCashPlays);
	//
	// NCR-44 Skip force and secondary cashes if we don't dare lose control
	if(bOkToFinesse) {
	   if(bOkToForce) {  // NCR-779 check if Force allowed
		FilterPlays(forcePlays);
		int numForcePlays = forcePlays.GetSize();
		m_playPlan << forcePlays;
		m_numPlannedForcePlays = numForcePlays;
		m_numTotalPlannedTricks += numForcePlays;
	   } // NCR-779 end skipping Force
		//
		FilterPlays(secondaryCashPlays);
		m_numPlannedSecondaryCashPlays = secondaryCashPlays.GetSize();
		m_playPlan << secondaryCashPlays;
		m_numTotalPlannedTricks += m_numPlannedSecondaryCashPlays;
	} // NCR-44 end not daring to lose control


	// NCR-82 Don't bother finessing if one Force play will create enough(4+?) cash plays
	if(bOkToFinesse && (forcePlays.GetSize() == 1) && (secondaryCashPlays.GetSize() > 3)
		&& (finessePlays.GetSize() > 0)){
//		int nbrRemoved = m_playPlan.RemoveSameCardPlays(&finessePlays);
//		status << "3PLNCNT2! Removed " & nbrRemoved & " finesses because Force and Cash generate more than 3 tricks\n";
		status << "3PLNCNT2! Skipping adding finesses because Force and Cash generate more than 3 tricks\n";
	} // NCR-82 end
	// NCR-703 Moved down here because of problem removing finesses above
	else if(bOkToFinesse)  // NCR-38 finesse if all 4 stopped or we have enough
	{
		m_playPlan << finessePlays;
		m_numPlannedFinesses += numPotentialFinesses;
	}
	else {
		status << "3PLNCNT3! Skipping using finesses because of possiblity of losing control.\n";
	} // NCR-38 end Q&D to not finesse if there are unstopped suits


	//
	//---------------------------------------------------------
	// now look at the possible top-card cashing tricks
	//
	CPlayList cashingPlays;
	int numReqdCards = 0;  // NCR-449 get number of required cards
	m_numPlannedCashingTricks = FindCashingPlays(cashingPlays, numReqdCards, FALSE);
//	FilterPlays(cashingPlays);
//	int numPotentialCashingTricks = cashingPlays.GetSize();
	m_playPlan << cashingPlays;

#ifdef _DEBUG  // NCR DEBUGGING
	// Show parts of the list  
//	CPlay * pPlayXX = m_playPlan.GetAt(0);  // NEED To test if list is empty first
//	pPlayXX = cashingPlays.GetAt(0);
//	CPlay * pPlayXX = forcePlays.GetAt(0);   // What is on the list?
//	int pps = m_playPlan.GetSize();  //<<<< Are we losing plays?
#endif
	// adjust count for drops
	m_numPlannedCashingTricks -= numPotentialDropTricks;
	m_numTotalPlannedTricks += m_numPlannedCashingTricks;

	// NCR-44 Check if we suppressed finesses and if maybe we should try them if we dont'have enough
	// tricks. This could put finesses after cashes which could squeeze opponents some
	if(!bOkToFinesse && (m_numTricksShort > 1)) {
		m_playPlan << finessePlays;
		m_numPlannedFinesses += numPotentialFinesses;
	}

	// at this point, screen out any obviously ineligible plays
	ScreenIneligiblePlays();

	// adjust play list based on bidding history
	TestPlaysAgainstBiddingHistory();

	// remove any plays that will likely fail, based on current information
	// (i.e., finesses that go opposite a finesse that succeeded earlier)
	TestPlaysAgainstPlayHistory();

	//
	// tally up the totals
	//
	m_numPlannedRounds = m_playPlan.GetSize();
	m_numTricksShort = m_numRequiredTricks - m_numTotalPlannedTricks;
	status << "4PLNF1! Done examining plays...\n>>>>>>>>>>>>>>>>>>>>\n";

	//
	// done planning -- so return the max number of possible tricks
	int numRemainingTricks = 13 - pDOC->GetNumTricksPlayed();
	if (m_numTotalPlannedTricks < 13)
		status << "2PLNNT90A! We have " & m_playPlan.GetSize() & " plays and approx. " & 
				  m_numTotalPlannedTricks & "+ possible tricks including " & m_numSureTricks & 
				  " sure tricks (vs. " & m_numRequiredTricks & " required) accounted for in the plan.\n";
	else
		status << "2PLNNT90B! We have " & m_playPlan.GetSize() & " plays and up to " &
				  numRemainingTricks & " possible tricks including " & m_numSureTricks & 
				  " sure tricks (vs. " & m_numRequiredTricks & " required) accounted for in the plan.\n";
	status.ClearStreamOutputLevel();

	//
	return m_numTotalPlannedTricks;
}





//
//-----------------------------------------------------------------------
//
// CreateSuitContractPlayPlan()
//
// create a suit contract play plan
//
int CDeclarerPlayEngine::CreateSuitContractPlayPlan()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;

	// set verbosity level
	if (pDOC->GetNumTricksPlayed() == 0)
		status.SetStreamOutputLevel(4);
	else
		status.SetStreamOutputLevel(5);

	int nTrumpSuit = pDOC->GetTrumpSuit();
//	int nTricksCount = 0;
	m_numTotalPlannedTricks = 0;


	//
	// We know that the number of sure tricks is less than the number
	// of available tricks.  So our choices are (in order of preference)
	// 1) Draw trumps,
	// 2) Play for a drop (if the odds are in our favor),
	// 3) Finesse, if necessary,
	// 4) Develop a suit,
	// 5) Ruff losers in dummy (or declarer, perhaps), and finally
	// 6) Cash high cards
	//

	//
	// look at playing for a drop
	//
	CPlayList dropPlays;
	FindDropPlays(dropPlays);

	//
	// then look at possible finesses
	//
	CPlayList finessePlays;
	FindFinessePlays(finessePlays);

	//
	// then look at developing a suit 
	//
	CPlayList forcePlays, secondaryCashPlays;
	FindSuitDevelopmentPlays(forcePlays, secondaryCashPlays);

	// have to empty out the secondary cashing plays here
	// cuz they are superseded by the cashing plays at the end
	secondaryCashPlays.Clear();

	//
	// now look at the possible ruffing tricks
	//
	CPlayList ruffingPlays;
	FindRuffingPlays(ruffingPlays);
	

	//
	//---------------------------------------------------------
	// now sequence the tricks in proper order
	//

	//
	// add the drop plays
	//
	FilterPlays(dropPlays);
	m_playPlan << dropPlays;
	int numPotentialDropPlays = dropPlays.GetSize();
	m_numPlannedDropPlays = numPotentialDropPlays;
	m_numTotalPlannedTricks += m_numPlannedDropPlays;

	//
	// add the ruffing plays, if appropriate
	//
//	if (bRuffFirst)
	if (TRUE)
	{
		FilterPlays(ruffingPlays);
		m_playPlan << ruffingPlays;
		int numPotentialRuffingTricks = ruffingPlays.GetSize();
		m_numPlannedRuffingTricks = numPotentialRuffingTricks;
		m_numTotalPlannedTricks += m_numPlannedRuffingTricks;
	}

	//
	// add the finesse plays
	//
	FilterPlays(finessePlays);
	m_playPlan << finessePlays;
	int numPotentialFinesses = finessePlays.GetSize();
	m_numPlannedFinesses = numPotentialFinesses;

	//
	// now add the force plays
	//
	FilterPlays(forcePlays);
	m_playPlan << forcePlays;
//	int numForcePlays = forcePlays.GetSize();
//	m_numTotalPlannedTricks += numForcePlays;

	//
	// then look at the possible cashing tricks
	//

	// NCR-123 Don't cash Trumps if we're the only one holding them - save for stoppers
	CCombinedSuitHoldings& trumpSuit = m_pCombinedHand->GetSuit(nTrumpSuit);
	// NCR-123 & NCR-133 Don't cash trumps if we have all of them
    BOOL bExcludeTrump = (trumpSuit.GetNumMissingSequences() == 0);
	CPlayList cashingPlays;
	int numReqdCards = 0;  // NCR-449 get number of required cards to have been played
	m_numPlannedCashingTricks = FindCashingPlays(cashingPlays, numReqdCards, bExcludeTrump); // NCR-123

	// and add them to the play list
//	FilterPlays(cashingPlays);
	m_playPlan << cashingPlays;
	// adjust for drop plays
	m_numPlannedCashingTricks -= m_numPlannedDropPlays;
	m_numTotalPlannedTricks += m_numPlannedCashingTricks;

	// adjust our count of "sure tricks" to use a more accurate count
	// NCR-449 Reduce cashing card tricks by number of required tricks first
	m_numSureTricks = MAX(m_numSureTricks, (m_numPlannedCashingTricks - numReqdCards));
	


	//
	//-----------------------------------------------------------------
	//
	// at this point, see how many trumps we can safely draw,
	// taking ruffs into account
	//
	CPlayList trumpPullingPlays;
	int nNbrPlannedTP = FormTrumpPullingPlan(m_playPlan, trumpPullingPlays);	

	// then add the trumps at the head of the play list
	m_numPlannedTrumpPulls = trumpPullingPlays.GetSize();
	// NCR NB: This logic reverses the order of the plays ????
	int i, nIndex; // NCR-FFS added here, removed below
	for(/*int*/ i=0,nIndex=m_numPlannedTrumpPulls-1; i<m_numPlannedTrumpPulls; i++,nIndex--) {
		m_playPlan.AddPlay(i, trumpPullingPlays[i]); // NCR-135 Do NOT reverse order
//		m_playPlan.AddPlay(i, trumpPullingPlays[nIndex]);
	}
	// adjust trump play count for winners
//NCR-123	CCombinedSuitHoldings& trumpSuit = m_pCombinedHand->GetSuit(nTrumpSuit);
	m_numPlannedTrumpPulls = Min(m_numPlannedTrumpPulls, trumpSuit.GetNumMaxWinners());
	m_numTotalPlannedTricks += m_numPlannedTrumpPulls;

	//
	// see if we can finesse the opponents' trumps
	//
	bool bSaveOnly = false; // NCR-425 what logic do I need here???
	CPlayList trumpFinessePlays;
	FormTrumpFinessePlan(m_playPlan, trumpFinessePlays, bSaveOnly);	

	// then add them _before_ the trump pulls
	int numTrumpFinesses = trumpFinessePlays.GetSize();
	for(i=0;i<numTrumpFinesses;i++)
		m_playPlan.AddPlay(i, trumpFinessePlays[i]);

	// at this point, screen out any obviously ineligible plays
	ScreenIneligiblePlays();

	// then remove any plays that will likely fail, based on current information
	// (i.e., finesses that go opposite a finesse that succeeded earlier)
	TestPlaysAgainstPlayHistory();

	//
	//-------------------------------------------------------------------
	// tally up the totals
	//
	m_numPlannedRounds = m_playPlan.GetSize();
	m_numTricksShort = m_numRequiredTricks - m_numTotalPlannedTricks;
	status << "4PLNF1! Done examining plays...\n>>>>>>>>>>>>>>>>>>>>\n";

	//
	// done planning -- so return the max number of possible tricks
	int numRemainingTricks = 13 - pDOC->GetNumTricksPlayed();
	if (m_numTotalPlannedTricks < 13)
		status << "2PLNST90A! We have " & m_playPlan.GetSize() & " possible plays and approx. " & 
				  m_numTotalPlannedTricks & "+ possible tricks including " & m_numSureTricks & 
				  " sure tricks (vs. " & m_numRequiredTricks & " required) accounted for in the plan.\n";
	else
		status << "2PLNST90B! We have " & m_playPlan.GetSize() & " possible plays and up to " &
				  numRemainingTricks & " possible tricks including " & m_numSureTricks & 
				  " sure tricks (vs. " & m_numRequiredTricks & " required) accounted for in the plan.\n";
	status.ClearStreamOutputLevel();

	//
	return m_numTotalPlannedTricks;
}





//
// PickPrioritySuit()
//
// look at the list of plays and see which suit we should prioritize for development
//
void CDeclarerPlayEngine::PickPrioritySuit()
{	
	CPlayerStatusDialog& status = *m_pStatusDlg;

	//
	// first list the suits in which there are drop, finesse, ruff, and force plays
	//
	CArray<int,int> nSuitsList;
	BOOL bSuitHasPlays[4] = { FALSE, FALSE, FALSE, FALSE };
	int numPlaysInSuit[4] = { 0, 0, 0, 0 };
	double fSuitMerit[4] = { 0, 0, 0, 0 };
	BOOL bSuitConsidered[4] = { FALSE, FALSE, FALSE, FALSE };
	//
	int numPlays = m_playPlan.GetSize();
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<numPlays;i++)
	{
		CPlay* pPlay = m_playPlan[i];
//		int nSuit = pPlay->GetSuit();
		//
		if ((pPlay->GetPlayType() == CPlay::DROP) || (pPlay->GetPlayType() == CPlay::FINESSE) ||
			(pPlay->GetPlayType() == CPlay::FORCE) || (pPlay->GetPlayType() == CPlay::RUFF)
			// NCR-707 Add to merit for Promotional cash
			|| (pPlay->GetPlayType() == CPlay::CASH && ((CCash*)pPlay)->GetProperties() == CCash::FOR_PROMOTION) )
		{
			int nSuit = pPlay->GetSuit();
			if (!bSuitHasPlays[nSuit])
			{
				bSuitHasPlays[nSuit] = TRUE;  //NCR-NOTE: One time switch for this suit
				numPlaysInSuit[nSuit]++;
				nSuitsList.Add(nSuit);
				int nVal;

				// determine the "merit" of each suit
				if (!bSuitConsidered[nSuit])
				{
					switch(pPlay->GetPlayType())
					{
						// drop/force -- long hand + 0.5 * short hand
						case CPlay::DROP: 
						case CPlay::FORCE:
						case CPlay::CASH:      // NCR-707 For promotion
							nVal = m_pCombinedHand->GetSuit(nSuit).GetMaxLength() + (m_pCombinedHand->GetSuit(nSuit).GetMinLength() / 2);
							fSuitMerit[nSuit] = nVal;      //NCR-NOTE: Assign here, add to below ???
							bSuitConsidered[nSuit] = TRUE; //NCR-NOTE: Only 1 DROP/FORCE for this suit 
							break;

						// 3 pts for each finesse (less 1 pt for depth of finesse)
						case CPlay::FINESSE:
							nVal = 3 - ((CFinesse*)pPlay)->GetDepth();
							fSuitMerit[nSuit] += Max(nVal, 0);	
							break;

						// 4 pts for each ruff, less # of required discards
						case CPlay::RUFF:			
							nVal = 4 - ((CRuff*)pPlay)->GetNumDiscardsRequired()*2;  // NCR-701 added *2
							fSuitMerit[nSuit] += Max(nVal, 0);
							break;
					}
				}
			}
		}
	}

	// also add points for internal honors and body cards in the suit
	// and deduct pts for missing top honors
	for(i=0;i<4;i++)
	{
		if (i != m_nTrumpSuit)
		{
			fSuitMerit[i] += m_pCombinedHand->GetSuit(i).GetNumSecondaryHonors() +
								m_pCombinedHand->GetSuit(i).GetNumBodyCards();
			fSuitMerit[i] -= m_pCombinedHand->GetSuit(i).GetNumMissingTopHonors();
		}
	}



	//
	// if no such play were found, sort suits by length
	//
	if (nSuitsList.GetSize() == 0)
	{
		// start with the longest suit unless it's the trump suit
		int nIndex = 0;
		if (m_pCombinedHand->GetSuitsByLength(nIndex) == pDOC->GetTrumpSuit())
			nIndex++;
		// and add the suits to the list
		for(;nIndex<4;nIndex++)
		{
			nSuitsList.Add(m_pHand->GetSuitsByLength(nIndex));
			fSuitMerit[nIndex] = m_pCombinedHand->GetSuit(nIndex).GetMaxLength() + m_pCombinedHand->GetSuit(nIndex).GetMinLength()/2;
		}
	}

	// sanity check
	int numSuits = nSuitsList.GetSize();
	if (numSuits == 0)
	{
		m_nPrioritySuit = NONE;
		return;
	}

	// now that we have the list of suits in the playlist, prioritize
	// pick the strongest suit, in terms of merit calculated above
	double fTopMerit = -1;
	int oldPrioritySuit = m_nPrioritySuit; // NCR save for msg when changed
	//
	for(i=0;i<numSuits;i++)
	{
		int nSuit = nSuitsList[i];
		if ((fSuitMerit[nSuit] > fTopMerit) && (nSuit != m_nTrumpSuit))
		{
			fTopMerit = fSuitMerit[nSuit];
			m_nPrioritySuit = nSuit;
		}
	}
	// NCR-701 show msg if changed
	if(ISSUIT(oldPrioritySuit) && oldPrioritySuit != m_nPrioritySuit) { 
		status << "5PLNPRIx! Priority suit changed from " & STSS(oldPrioritySuit) & ". Selection merit was " 
			      & fSuitMerit[oldPrioritySuit] & " vs " & fSuitMerit[m_nPrioritySuit] & ".\n";
	}  // end NCR-701
	//
	status << "2PLNPRI! The " & STSS(m_nPrioritySuit) & " suit looks the most promising for further development.\n";
}





//
// TestPlaysAgainstBiddingHistory()
//
// look at the play history to remove plays that are unlikely to succeed
// e.g., see if a finesse goes against a player who bid the suit
//
void CDeclarerPlayEngine::TestPlaysAgainstBiddingHistory()
{
	// 
	CPlayerStatusDialog& status = *m_pStatusDlg;
	status << "3PLTSTB! Checking logic of selected plays against bidding history...\n";
	//

	// look through the list of plays, and see if the current play is a finesse
	for(int i=0,nIndex=0;i<m_playPlan.GetSize();i++,nIndex++)
	{
		CPlay* pPlay = m_playPlan[nIndex];
		if (pPlay->GetPlayType() != CPlay::FINESSE)
			continue;

		// for a finesse, see if an opponent had bid this suit
		CFinesse* pFinesse = (CFinesse*) pPlay;
		int nSuit = pPlay->GetSuit();
		int numBids = pDOC->GetNumBidsMade();
		int numRounds = numBids / 4;
		if ((numBids % 4) > 0)
			numRounds++;

		//
		BOOL bLHOBidSuit = FALSE, bRHOBidSuit = FALSE;

		//
		// problem -- how to handle artificial bids???
		//
		for(int j=0;j<numRounds;j++)
		{
			// check LHO
			int nBid = pDOC->GetBidByPlayer(m_pLHOpponent, j);
			if (ISBID(nBid) && (BID_SUIT(nBid) == nSuit))
			{
				bLHOBidSuit = TRUE;
				break;
			}
			// check RHO
			nBid = pDOC->GetBidByPlayer(m_pRHOpponent, j);
			if (ISBID(nBid) && (BID_SUIT(nBid) == nSuit))
			{
				bRHOBidSuit = TRUE;
				break;
			}
		}

		//
		if ( ((pFinesse->GetTargetPos() == m_pLHOpponent->GetPosition()) && (bLHOBidSuit)) ||
			 ((pFinesse->GetTargetPos() == m_pRHOpponent->GetPosition()) && (bRHOBidSuit)) )
		{
			// remark
			status << "3PLTSTB! The " & pPlay->GetConsumedCard()->GetName() & 
					  " finesse is targeted against " & PositionToString(pFinesse->GetTargetPos()) &
					  ", who had earlier bid the " & STSS(pPlay->GetSuit()) & ", so postpone the play.\n";
			// and push the play to the end of the play list
			m_playPlan.MovePlayToEnd(i);
		}
	}
}





//
// TestPlaysAgainstPlayHistory()
//
// look at the play history to remove plays that are unlikely to succeed
// e.g., see if a finesse goes opposite a finesse that succeeded earlier
//
void CDeclarerPlayEngine::TestPlaysAgainstPlayHistory()
{
	// 
	CPlayerStatusDialog& status = *m_pStatusDlg;
	status << "3PLTSTP! Checking logic of selected plays against play history...\n";
	//

	// look through the list of plays, and see if the current play is a finesse
	for(int i=0,nIndex=0;i<m_playPlan.GetSize();i++,nIndex++)
	{
		bool bSkip = false;
		CPlay* pPlay = m_playPlan[nIndex];
		if (pPlay->GetPlayType() != CPlay::FINESSE)
			continue;

		// for a finesse, see if there was an earlier finesse that succeeded
		// with the same opponent gap cards
		for(int j=0;j<m_usedPlays.GetSize();j++)
		{
			CPlay* pUsedPlay = m_usedPlays[j];
			if (pUsedPlay == NULL)
				continue;

			//
			if (pUsedPlay->GetPlayType() != CPlay::FINESSE)
				continue;

			// compare the two plays for the suit 
//			CFinesse* pNewFinesse = dynamic_cast<CFinesse*> (pPlay);
//			CFinesse* pOldFinesse = dynamic_cast<CFinesse*> (pUsedPlay);
			CFinesse* pNewFinesse = (CFinesse*) pPlay;
			CFinesse* pOldFinesse = (CFinesse*) pUsedPlay;
			if (pNewFinesse->GetSuit() != pOldFinesse->GetSuit())
				continue;

			// compare the gap cards
			CCardList* pNewGapCards = pNewFinesse->GetGapCards();
			CCardList* pOldGapCards = pOldFinesse->GetGapCards();
			// check for overlap
			if ((!pOldGapCards->ContainsAtLeastOneOf(*pNewGapCards)) &&
						(!pNewGapCards->ContainsAtLeastOneOf(*pOldGapCards)))
				continue;

			// here, the two plays have the same suit and gap cards
			// so see if the earlier play succeeded, and its target
			int nTrickWinner = pDOC->GetGameTrickWinner(j);
			if ((nTrickWinner == GetPlayerPosition()) || (nTrickWinner == GetPartnerPosition()))
			{
				// the play succeeded -- so see if the new play's target is 
				// different from it
				int nNewTarget = pNewFinesse->GetTargetPos();
				int nOldTarget = pOldFinesse->GetTargetPos();
				if (nNewTarget != nOldTarget)
				{
					status << "!5PLTST1! The [" & pNewFinesse->GetName() & "] targets " & PositionToString(nNewTarget) & 
							  ", while we successfully finessed against " & PositionToString(nOldTarget) &
							  " earlier, so delete this play.\n";
					AdjustPlayCountForDeletedPlay(m_playPlan[nIndex]);
					m_playPlan.RemovePlay(nIndex, FALSE);
					m_discardedPlays.AppendPlay(pPlay);				
					nIndex--;
					bSkip = true;
					break;
				}
			}
		}

		// see if we should skip the rest of the loop
		if (bSkip)
			continue;

		// if the first opponent has shown out of the suit, the finesse is pointless
		// e.g., if finessing in dummy (north) against east (leading from south), and west has already
		// shown out, then the finesse against east will fail
		CFinesse* pFinesse = (CFinesse*) (pPlay);
//		ASSERT(pFinesse);
/*
		int nOpponent = NONE;
		if (pFinesse->GetTargetPos() == m_pPartner->GetPosition()) // NCR-461 was CPlay::IN_DUMMY)
			nOpponent = GetNextPlayer(m_pPlayer->GetPosition());	// LHO
		else	// finessing in hand
			nOpponent = GetPrevPlayer(m_pPlayer->GetPosition());	// RHO
*/
		int nOpponent = ::GetPartner(pFinesse->GetTargetPos()); // NCR-461 test partner of target
		CGuessedHandHoldings* pOpponentHand = m_ppGuessedHands[nOpponent];
		if (pOpponentHand->IsSuitShownOut(pFinesse->GetSuit()))
		{
			status << "!5PLTST4! The [" & pFinesse->GetName()  & " " & pFinesse->GetSubType()
				       & "] will not suceed because " & 
					   PositionToString(nOpponent) & " has shown out of the suit, so delete the play.\n";
			AdjustPlayCountForDeletedPlay(m_playPlan[nIndex]);
			m_playPlan.RemovePlay(nIndex, FALSE);
			m_discardedPlays.AppendPlay(pPlay);				
			nIndex--;
		}
	}  // end for(i) thru playPlans
}





//
// SequencePlays()
//
// - this is where we take the list of potential plays and sequence 
//   them into something resembling a coherent plan
//                                     NB: bInitialPlan NOT used ???  
void CDeclarerPlayEngine::SequencePlays(BOOL bInitialPlan)
{
	// 
	CPlayerStatusDialog& status = *m_pStatusDlg;
	status << "!3PLSEQ! Checking play sequencing...\n";
	BOOL bSequenced = FALSE;
	BOOL bSkipOpportunisticPlays = FALSE;

	//
	int numPlays = m_playPlan.GetSize();
	if (numPlays == 0)
		return;
//	int numPlaysMoved = 0;
	BOOL bPlayingInHand = (pDOC->GetCurrentPlayer() == m_pPlayer);
//	int nCurrentHand = bPlayingInHand? CPlay::IN_HAND : CPlay::IN_DUMMY;
	//
	int nTrumpSuit = pDOC->GetTrumpSuit();
	int numOutstandingTrumps = ISSUIT(nTrumpSuit)? GetNumOutstandingCards(nTrumpSuit) : 0;
	int numTricksLeftToPlay = (13-pDOC->GetNumTricksPlayed()); // NCR-429


	//
	//------------------------------------------------------------------------
	//
	//  *** Straight Cashing ***
	//
	//
	// if there are enough cashing tricks to fulfill the contract
	// move them to the front
	//

	//
	// but we need to check entries first!
	//
	EvaluateEntries();

	BOOL bSufficientEntries = TRUE;
	int numRemainingTricks = m_numTricksLeftToBeMade;
	// 
	if ( (m_pCombinedHand->GetNumDeclarerWinners() < numRemainingTricks) &&
		 (m_pCombinedHand->GetNumDummyWinners() < numRemainingTricks) )
	{
		// we need entries into both hands
		if ((m_numDeclarerEntries == 0) || (m_numDummyEntries == 0))
			bSufficientEntries = FALSE;
	}

	// see if we have the entries to cash winners in both hands
	// here, see if we're in dummy and need to cash winners from hand
	if (!bPlayingInHand && (m_pCombinedHand->GetNumDeclarerTopCards() >= 0) &&
		 (m_pCombinedHand->GetNumDummyTopCards() < numRemainingTricks) && (m_numDeclarerEntries == 0))
		bSufficientEntries = FALSE;

	// or vice versa
	if (bPlayingInHand && (m_pCombinedHand->GetNumDummyTopCards() >= 0) &&
		(m_pCombinedHand->GetNumDeclarerTopCards() < numRemainingTricks) && (m_numDummyEntries == 0))
		bSufficientEntries = FALSE;
	
	// adjust cash count for declarer/dummy splits
	int numTopCards = m_pCombinedHand->GetNumMaxTopCards();
	int numWinners = m_pCombinedHand->GetNumWinners(); // NCR-568

	BOOL bEnoughCashes = FALSE;

	// new -- if we have enough winners in trumps to make the contract, go
	// ahead and try for a few extra tricks (why not?)
	int numTrumpWinners = ISSUIT(nTrumpSuit)? m_pCombinedHand->GetSuit(nTrumpSuit).GetNumMaxWinners() : 0;
	if (!bSequenced)
	{
		if ( ISSUIT(nTrumpSuit) &&  
				((numTrumpWinners - numOutstandingTrumps) >= m_numTricksLeftToBeMade) )
		{
			// don't mark that we have enough cashes
			// this will encourage other plays to be used first
//			status << "4PLSEQ0! Since we have enough cashing tricks (" & numCashes &
//					  ") to fulfill the contract, proceed with the cashing.\n";
		}
//		else if ( (numCashes >= m_numTricksLeftToBeMade) && bSufficientEntries &&
//		else if ( (m_numPlannedCashingTricks >= m_numTricksLeftToBeMade) && bSufficientEntries &&
		else if ( (numTopCards >= m_numTricksLeftToBeMade) && bSufficientEntries &&
					 (!ISSUIT(nTrumpSuit) || (numOutstandingTrumps == 0)) )
		{
			bEnoughCashes = TRUE;
			//
			if (m_numPlannedFinesses || m_numPlannedForcePlays || 
						m_numPlannedHoldUps || m_numPlannedDropPlays)
				status << "!2PLSEQ1! Since we have enough cashing tricks (" & m_numPlannedCashingTricks &
						  ") to fulfill the contract, we can just proceed with the cashing.\n";
			//
			int numMovedCashes = 0; // NCR-421
			int numPlays = m_playPlan.GetSize();
			int nPlayLocation = bPlayingInHand ? CPlay::IN_HAND : CPlay::IN_DUMMY;  // NCR-476
			for(int i=1;i<numPlays;i++)
			{
				// see if this is a cashing play, and if so, move it up
				CPlay* pPlay = m_playPlan[i];              // NCR-422 Must not be any required played cards
				if ((pPlay->GetPlayType() == CPlay::CASH) && (pPlay->GetRequiredPlayedCardsList() == NULL) 
					// NCR-476 Only local plays  ??? TESTING ???        // NCR-479 ???
					&& ((pPlay->GetStartingHand() == nPlayLocation) || (pPlay->GetStartingHand() == CPlay::IN_EITHER)) )
				{
					// advance it forward, ahead of all other non-cash plays
					for(int j=0;j<i;j++)
					{
						if (m_playPlan[j]->GetPlayType() != CPlay::CASH)
						{
							m_playPlan.MovePlay(i, j);
							numMovedCashes++; // NCR-421 count
							break;
						}
					}  // end for(j) looking for non cash play
				}
				// NCR-481 Gather discards in case needed - use Force cards since they are not needed now
				// NCR-505 Also don't need to finesse
				else if((pPlay->GetPlayType() == CPlay::FORCE) || (pPlay->GetPlayType() == CPlay::FINESSE))
				{  
					CCard* pCard = pPlay->GetConsumedCard();
					Position cardOwner = pCard->GetOwner();
					Position currPos = m_pPlayer->GetPosition();
					if(cardOwner == currPos) {
						int numCards = m_pHand->GetNumCardsInSuit(pCard->GetSuit());
						pCard = m_pHand->GetCardByPosition(pCard->GetSuit(), numCards-1);  // get lowest card
						ASSERT(m_pPlayer->HasCard(pCard));
						if(!m_declarerPriorityDiscards.HasCard(pCard))  // test if already there
							m_declarerPriorityDiscards << pCard;  // NCR-481
					}else{
						int numCards = m_pPartnersHand->GetNumCardsInSuit(pCard->GetSuit());
						pCard = m_pPartnersHand->GetCardByPosition(pCard->GetSuit(), numCards-1);  // get lowest card
						ASSERT(m_pPartner->HasCard(pCard));
						if(!m_dummyPriorityDiscards.HasCard(pCard))  // test if already there
							m_dummyPriorityDiscards << pCard;  // NCR-481
					}
				}  // NCR-481 end saving discards
				// should play be removed now?
			} // end for(i) thru plays

			if(numMovedCashes > 0)  // NCR-422 show what we've done
				status << "!5PLSEQ3! Since we have enough sure tricks (" & m_numSureTricks
							& ") to fulfill the contract, we moved " & numMovedCashes & " cash plays forward\n";

			// sequencing complete
			bSequenced = TRUE;
		}
	}

	//------------------------------------------------------------------------
	//
	// Check for other problems in the Cash plays
	//

	for(int i = 0; i < m_playPlan.GetSize()-1; i++)  // -1 because we don't need to look at last one
	{
		CPlay* pPlay = m_playPlan[i];
		if (pPlay->GetPlayType() == CPlay::CASH)
		{
			// NCR-654 check for singleton that can block suit
			CCard* pCard = pPlay->GetConsumedCard();
			int nSuit = pCard->GetSuit();
			Position cardOwner = pCard->GetOwner();
			CPlayer* pCardHolder = pDOC->GetPlayer(cardOwner);
			CPlayer* pOtherHand = pCardHolder->GetPartner();

			// Is cash card a singleton and does otherhand have > 1 cards
			if(pCardHolder->GetHand().GetSuit(nSuit).IsSingleton()
				&& (pOtherHand->GetHand().GetSuit(nSuit).GetNumCards() > 1)
				// Is singleton < top card otherhand
				&& (pCard->GetFaceValue() < pOtherHand->GetHand().GetSuit(nSuit).GetTopCard()->GetFaceValue()) 
				// And greater than smallest card >>> can block
				&& (pCard->GetFaceValue() > pOtherHand->GetHand().GetSuit(nSuit).GetBottomCard()->GetFaceValue()) )
			{
				// Now check if there is only one entry to the other hand
				if((pCardHolder->IsDummy() && (GetNumDeclarerEntries() < 2))
					|| (pCardHolder->IsDeclarer() && (GetNumDummyEntries() < 2 )) )
				{
					// If only one entry, then playing this card will block the suit
					m_playPlan.MovePlayToEnd(i);  // get this play out of the way
					status << "!5PLSEQ3a! Moved " & pPlay->GetName() 
							 & " to keep from blocking suit.\n"; 
				}
			}
			// end NCR-654
		} // end looking at Cash plays
	} // end for(i) through plans



	//
	//------------------------------------------------------------------------
	//
	// *** Cashes + Ruffs + Trump Pulls ***
	// 
	// If we can fulfill the contract using just cashes, ruffs, and trump pulls,
	// we should go ahead and do so, and avoid the more speculative plays like 
	// finesses and such
	//
//	int numSolidPlays = m_numPlannedCashingTricks + m_numPlannedTrumpPulls;

	// remove overlapping cashes and trump pulls
	// i.e., remove the cash plays that use cards used by trump pulls
	CPlayList* pCashes = m_playPlan.GetPlaysOfType(CPlay::CASH);
	CPlayList* pTrumpPulls = m_playPlan.GetPlaysOfType(CPlay::TRUMP_PULL);
	int numRemoved = 0;
	int numUniqueCashes = 0;
	if (pCashes)
	{
		int nbrRemoved = pCashes->RemoveSameCardPlays(pTrumpPulls);  // NCR added nbrRemoved

		// NCR-587 Don't move any if first one is good
		bool bHaveCashFirst = false;
		if(m_playPlan.GetAt(0)->GetPlayType() == CPlay::CASH) {
			// If no required play list leave list alone
			bHaveCashFirst = m_playPlan.GetAt(0)->GetRequiredPlayedCardsList() == NULL;
		}

		// NCR-417 If we have enough sure winners for contract, move them up
		// NCR-422 added !bSequenced below     NCR-429 use MIN vs only one
		if(!bSequenced && (m_numSureTricks >= MIN(m_numTricksLeftToBeMade, numTricksLeftToPlay))
			    // Leave trump pulls to be done first !!
			&& (m_numPlannedTrumpPulls == 0) && !bHaveCashFirst)  // NCR-587 only if not first
		{
			int nMoved = 0;
        	int nCurrentHand = bPlayingInHand? CPlay::IN_HAND : CPlay::IN_DUMMY;
			bool bMovedForOtherHand = false;  // NCR-429 keep track ???

			// Go thru list bottomup to preserve order
			for(int nCash = pCashes->GetSize() - 1; nCash >=0; nCash--) 
			{
				CPlay* pCash = pCashes->GetAt(nCash);
				// Sure cashes do NOT have required played cards list
				if(pCash->GetRequiredPlayedCardsList() == NULL) 
				{
					// Only move those for the current hand ???
					if ((pCash->GetStartingHand() == nCurrentHand)
						|| (pCash->GetQuality() > 50) )  // NCR-762 move if high quality
					{
						m_playPlan.MovePlayToFront(pCashes->GetAt(nCash));  // NCR-417 Move it to front
						nMoved++;  // count those moved
					}else {
						// If cards in other hand, do we have entries?
						if(!bMovedForOtherHand) {
							bMovedForOtherHand = true;  // only move one?
							m_playPlan.MovePlayToFront(pCashes->GetAt(nCash));  // NCR-417 Move it to front
							nMoved++;  // count those moved
						}
					}
				} // end looking at cashes without RequirePlayedCards
			} // end for(nCash) thru cashing plays

			if(nMoved > 0) {
				status << "!5PLSEQ5! Since we have enough sure tricks (" & m_numSureTricks
							& ") to fulfill the contract, we moved " & nMoved & " cash plays forward\n";
				bEnoughCashes = TRUE; // NCR-420 Prevent moving priority suit plays forward
			}
		} // end NCR-417 moving cashes forward

		numUniqueCashes = pCashes->GetSize() - numRemoved;
		numUniqueCashes = Min(numUniqueCashes, m_pCombinedHand->GetNumMaxTopCards());

	}  // end handling cashes


	int numSolidPlays = numUniqueCashes + m_numPlannedTrumpPulls;
	int numUsableRuffs = 0;

	if (ISSUIT(nTrumpSuit))
	{
		// the solid plays are trump pulls, cashes, and safe ruffs
		// the # of safe ruffs is equal to the # of excess trumps in the hand 
		// with the fewer trumps
		CCombinedSuitHoldings& trumps = m_pCombinedHand->GetSuit(nTrumpSuit);
		int numShortTrumps = trumps.GetMinLength() - m_numPlannedTrumpPulls;
		if (trumps.GetLengthDiff() > 0)
		{
			// declarer has more trumps
			// count the # of ruffs in dummy
			int numDummyRuffs = m_playPlan.GetNumPlaysOfType(CPlay::RUFF, CPlay::IN_DUMMY);
			numUsableRuffs = Min(numShortTrumps, numDummyRuffs);
		}
		else
		{
			// dummy has more trumps
			int numDeclarerRuffs = m_playPlan.GetNumPlaysOfType(CPlay::RUFF, CPlay::IN_HAND);
			numUsableRuffs = Min(numShortTrumps, numDeclarerRuffs);
		}
		// decrease # of ruffs for each trump cash play in the list
		if (pCashes)
		{
			for(int i=0;i<pCashes->GetSize();i++)
			{
				if (pCashes->GetAt(i)->GetSuit() == nTrumpSuit)
					numUsableRuffs--;
			}
			if (numUsableRuffs < 0)
				numUsableRuffs = 0;
		}
		//				
		numSolidPlays += numUsableRuffs;
	} 
	                                     // NCR-429 ToBeMade will be > if we're down     
	if (!bSequenced && (numSolidPlays >= MIN(m_numTricksLeftToBeMade, numTricksLeftToPlay))) // ?????
	{
		// format plan
		CString strPlan;
		if (m_numPlannedTrumpPulls > 0)
			strPlan.Format("pulling %d round(s) of trumps", m_numPlannedTrumpPulls);
		
		// list the anticipated cashes
		if (numUniqueCashes > 0)
		{
			if (m_numPlannedTrumpPulls > 0)
				strPlan += ", #";
			strPlan += "cashing the ";

			// don't list more from each suit than is possible
			int	numCashesInSuit[4] = { 0, 0, 0, 0 };
			int numCashes = pCashes->GetSize();
			for(int i=0,nIndex=0;(i<numUniqueCashes)&&(nIndex<numCashes);i++,nIndex++)
			{
				// check if we've exceed the limit
				CCard* pCard = pCashes->GetAt(nIndex)->GetConsumedCard();
				int nSuit = pCard->GetSuit();
				numCashesInSuit[nSuit]++;
				if (numCashesInSuit[nSuit] > m_pCombinedHand->GetSuit(nSuit).GetMaxLength())
				{
					// don't list this play cuz it can't be used anyway
					i--;
					continue;
				}
				else if ((nSuit == nTrumpSuit) && 
						 ((numCashesInSuit[nSuit] + m_numPlannedTrumpPulls) > m_pCombinedHand->GetSuit(nSuit).GetMaxLength()))
				{
					// likewise
					i--;
					continue;
				}
				//
				strPlan += pCashes->GetAt(nIndex)->GetConsumedCard()->GetName();
				if (numUniqueCashes > 2)
				{
					if (i < numUniqueCashes-2)
						strPlan += ", ";
					else if (i == numUniqueCashes-2)
						strPlan += " and ";
				}
				else
				{
					if (i == 0)
						strPlan += " and ";
				}
			}
		}
		//
		if (numUsableRuffs > 0)
		{
			// strip any previous "and" markers
			int nPos = strPlan.ReverseFind(_T('#'));
			if (nPos > 0)
				strPlan = strPlan.Left(nPos) + strPlan.Mid(nPos+1);
			//
			if ((m_numPlannedTrumpPulls > 0) || (m_numPlannedCashingTricks > 0))
				strPlan += ", #";
			CPlayList* pRuffs = m_playPlan.GetPlaysOfType(CPlay::RUFF);
			int numRuffPlays = pRuffs->GetSize();
			if (numRuffPlays == numUsableRuffs)	// there may be more available
			{
				strPlan += "ruffing a";
				int nSize = pRuffs->GetSize();
				for(int i=0;i<nSize;i++)
				{
					strPlan += (char)pRuffs->GetAt(i)->GetSuit();
					if (nSize > 2)
					{
						if (i < nSize-2)
							strPlan += ", ";
						else if (i == nSize-2)
							strPlan += " and ";
					}
					else
					{
						if (i == 0)
							strPlan += " and ";
					}
				}
			}
			else
			{
				// more ruffs than we can use
				strPlan += FormString("ruffing %d card(s)", numUsableRuffs);
			}
			delete pRuffs;
		}

		// push the non-cash/trump/ruff plays to the end
		int numPlays = m_playPlan.GetSize();
		for(int i=0,nIndex=0;i<numPlays;i++)
		{
			int nPlayType = m_playPlan.GetAt(nIndex)->GetPlayType();
			if ((nPlayType != CPlay::TRUMP_PULL) && (nPlayType != CPlay::CASH) &&
									(nPlayType != CPlay::RUFF))
				m_playPlan.MovePlayToEnd(nIndex);
			else
				nIndex++;
		}

		// insert a final "and" in the plan
		int nPos = strPlan.ReverseFind(_T('#'));
		if (nPos > 0)
			strPlan = strPlan.Left(nPos) + "and " + strPlan.Mid(nPos+1);

		//
		status << "2PLSEQ20! We can fulfill the contract by " & strPlan & ".\n";

		// sequencing is complete
		bSequenced = TRUE;
		m_nPrioritySuit = NONE; // NCR-596 Turn this off if we have all tricks needed
	}  // end having enough solid plays

	// clean up
	delete pCashes;
	delete pTrumpPulls;






	//
	//------------------------------------------------------------------------
	//
	// *** Throwing opponents into the lead ***
	//
	// in certain cases, we want to throw the opponents into the lead
	// usually, this occurs when we want to force the opponents to lead
	// cards we can ruff or break a suit (i.e., lead into a finesse)
	// this will happen when we have finesses in one or two suits and 
	// can empty out the other side suit(s) with cashes
	//
	int numDeclarerTrumps = m_pHand->GetNumTrumps();
//	int numDummyTrumps = m_pPartnersHand->GetNumTrumps();
	if (!bSequenced && ISSUIT(nTrumpSuit) && (numDeclarerTrumps > 0)
		// NCR-447 Don't do it if too many trumps out
		&& (numOutstandingTrumps < 2) )
	{
		// requirements for the throwout play: 
		// - we're playing in a trump contract, && we have at least one trump (as above)
		// - we have at least 2 suit(s) that can be cleared, or can be cleared with a sluff
		// - we have finesses in the remaining suits
		//
		// by sluffing the last card(s) in a suit in this manner, we force
		// the opponents to give us a ruff or lead into a finesse
		//
		// determine the number of suits where we have finesse opportunities
		int numFinesseSuits = 0;
		BOOL bSuitHasFinesses[4] = { FALSE, FALSE, FALSE, FALSE };
		CArray<int,int> arrayFinesseSuits;
		int i; // NCR-FFS added here, removed below
		for(/*int*/ i=0;i<4;i++)
		{
			if (i == nTrumpSuit)
				continue;
			// search through the play list
			for(int j=0;j<numPlays;j++)
			{
				if ((m_playPlan[j]->GetPlayType() == CPlay::FINESSE) &&
							(m_playPlan[j]->GetSuit() == i))
				{
					bSuitHasFinesses[i] = TRUE;
					arrayFinesseSuits.Add(i);
					numFinesseSuits++;
					break;
				}
			}
		}

		// then check if the suits that don't have finesses are empty
		// or have one card
		// first see if we have any "excess" winners
		int numExcessWinners = 0;
		for(i=0;i<4;i++)
		{
			if (i == nTrumpSuit)
				continue;
			if (!bSuitHasFinesses[i])
			{
				numExcessWinners += m_pCombinedHand->GetSuit(i).GetNumExcessWinners();
			}
		}	

		// now continue
		int numClearSuits = 0, numExitSuits = 0;
		BOOL bSuitIsClear[4] = { FALSE, FALSE, FALSE, FALSE };
		BOOL bExitSuit[4] = { FALSE, FALSE, FALSE, FALSE };
		CArray<int,int> arrayClearSuits;
		CArray<int,int> arrayExitSuits;
		for(i=0;i<4;i++)
		{
			if (i == nTrumpSuit)
				continue;

			// a suit is "Clear" if we have zero losers in the suit
			int numLosers = m_pCombinedHand->GetSuit(i).GetNumLosers();
			if ((numLosers == 0) && (m_pCombinedHand->GetNumCardsInSuit(i) > 0))
			{
				// we can clean up in this suit
				bSuitIsClear[i] = TRUE;
				arrayClearSuits.Add(i);
				numClearSuits++;
			}
			else if (numLosers == 1)
			{
				// with one loser, this is classified as an exit suit
				bExitSuit[i] = TRUE;
				arrayExitSuits.Add(i);
				numExitSuits++;
				if (bSuitHasFinesses[i])
				{
					// remove this suit from the finesse suits list
					bSuitHasFinesses[i] = FALSE;
					for(int j=0;j<arrayFinesseSuits.GetSize();j++)
					{
						if (arrayFinesseSuits[j] == i)
						{
							arrayFinesseSuits.RemoveAt(j);
							break;
						}
					}
					numFinesseSuits--;
				}
			}
			else if (numLosers == 2)
			{
				// see if we can discard losers here on an excess winner
				if (numExcessWinners > 0)
				{
					bExitSuit[i] = TRUE;
					arrayExitSuits.Add(i);
					numExitSuits++;
					//
					if (bSuitHasFinesses[i])
					{
						// remove this suit from the finesse suits list
						bSuitHasFinesses[i] = FALSE;
						for(int j=0;j<arrayFinesseSuits.GetSize();j++)
						{
							if (arrayFinesseSuits[j] == i)
							{
								arrayFinesseSuits.RemoveAt(j);
								break;
							}
						}
						numFinesseSuits--;
					}
				}
			}
		}

		// now, if we can exit one (or two) suits with a sluff, while the
		// remaining non-trump suit(s) have finesses, we're set!
		// but if we have two suits with one card each, they must be in opp hands
		BOOL bExitOK = FALSE;
		if ((numFinesseSuits + numClearSuits + numExitSuits) == 3)
		{
			if (numExitSuits == 1)
			{
				// 1 exit card -- simple enough
				bExitOK = TRUE;
			}
			else if (numExitSuits == 2)
			{
				// 2 exit cards -- check that they're in opposite hands
				int nFirstHand = -1;	// 0 = declarer, 1=dummy
				for(i=0;i<numExitSuits;i++)
				{
					int nSuit = arrayExitSuits[i];
					if (nFirstHand == -1)
					{
						// mark the location of the first exit card
						if (m_pCombinedHand->GetSuit(nSuit).GetNumDeclarerCards() > 0)
							nFirstHand = 0;
						else
							nFirstHand = 1;
					}
					else
					{
						// compare the location of the second exit card
						if ((nFirstHand == 0) && (m_pCombinedHand->GetSuit(nSuit).GetNumDummyCards() > 0))
							bExitOK = TRUE;
						else if ((nFirstHand == 1) && (m_pCombinedHand->GetSuit(nSuit).GetNumDeclarerCards() > 0))
							bExitOK = TRUE;
					}
				}
			}
		}
		//
		if (bExitOK)
		{
			// at this point, we need to sequence the plays properly for this
			// line of play to work
			// - gotta put the cashes in the clear suits first, right after
			//   any trump pulls, then insert an exit play before the finesses
			int numPlays = m_playPlan.GetSize();
			for(i=0;i<m_playPlan.GetSize();i++)
			{
				if (m_playPlan[i]->GetPlayType() != CPlay::TRUMP_PULL)
					break;
			}
			int nPos = i, numPlaysMoved = 0;

			// move up the cashes in the clear suits
			CPlayList* pPrerequisites = new CPlayList;
			for(int j=numPlays-1;j>nPos;j--)
			{
				// is this play a cash in the clear suit?
//				BOOL bCash = FALSE;
				CPlay* pPlay = m_playPlan[j];
				if (pPlay->GetPlayType() == CPlay::CASH)
				{
					// compare the play's suit against all clear suits
					BOOL bMoved = FALSE;
					for(int k=0;k<numClearSuits;k++)
					{
						if (pPlay->GetSuit() == arrayClearSuits[k])
						{
							pPrerequisites->AppendPlay(pPlay);
							m_playPlan.MovePlay(j, nPos);
							numPlaysMoved++;
							j++;	// keep the comparison where it is
							nPos++;	// and update the destination slot
							bMoved = TRUE;
							break;
						}
					}
					// and do the same for exit suit
					if (!bMoved)
					{
						for(int k=0;k<numExitSuits;k++)
						{
							if (pPlay->GetSuit() == arrayExitSuits[k])
							{
								pPrerequisites->AppendPlay(pPlay);
								m_playPlan.MovePlay(j, nPos);
								numPlaysMoved++;
								j++;	// keep the comparison where it is
								nPos++;	// and update the destination slot
								break;
							}
						}
					}
				} // end handling Cash plays
			} // end for(j) thru play plans

			// NCR-446 Report number of plans moved
			if(numPlaysMoved > 0)
				status << "5PLSEQ40! Moved " & numPlaysMoved & " plays forward for Exit play.\n";


			// then insert an exit play
			CExitPlay* pPlay = new CExitPlay(pPrerequisites,
											 arrayExitSuits[0],
											 ((numExitSuits == 2)? arrayExitSuits[1] : ANY));
			m_playPlan.AddPlay(nPos, pPlay);

			// then the exit play
			if (numExitSuits == 1)
			{
				status << "3PLSEQ44! We want to eventually exit with a " & STSS(arrayExitSuits[0]) & ".\n";
			}
			else
			{
				status << "3PLSEQ45! We want to eventually exit with a " & STSS(arrayExitSuits[0]) & 
						  " and a " & STSS(arrayExitSuits[1]) & ".\n";
			}

			// sequencing complete
			bSequenced = TRUE;

			// don't throw this off with an opportunistic play either!
			bSkipOpportunisticPlays = TRUE;
		}  // end bExitOK
	}  // end checking if we want to put opponents in lead





	//
	//------------------------------------------------------------------------
	//
	//  *** Opportunistic Play Handling ***
	//
	//
	// see if there is a play that have to be played _now_ to be effective
	//

	// opportunistic plays are generally speculative, so we cannot 
	// play them if we have to win every trick
	if (!bSkipOpportunisticPlays && 
		pDOC->GetNumTricksRemaining() > m_numTricksLeftToBeMade)
	{
		int numPlays = m_playPlan.GetSize();
		int i; // NCR-FFS added here, removed below
		for(/*int*/ i=0;i<numPlays;i++) {
			if (m_playPlan[i]->IsOpportunistic())
				break;
		}
		// see if the play is usable now
		if ((i < numPlays) && (m_playPlan[i]->IsPlayUsable(*m_pCombinedHand, *this)))
		{
			// if so, move it to the head of the pack
			m_playPlan.MovePlayToFront(i);
			status << "4PLSEQ50! Move the opportunistic " & m_playPlan[i]->GetName() & 
					  " to the front.\n";
			bSequenced = TRUE;
		}
	}






	//
	//------------------------------------------------------------------------
	//
	//  *** Priority Suit Handling ***
	//
	//
	// see if we have a priority suit we want to develop; if so, promote the 
	// suit plays to the front
	//
	if (!bSequenced && ISSUIT(m_nPrioritySuit) && !bEnoughCashes)
	{
		//
		status << "3PLSEQ6! We want to work on developing the " & STSS(m_nPrioritySuit) &
				  " suit first, so move its plays up to the front if possible.\n";
		int numPlaysMoved = 0;
		for(int i=1;i<numPlays;i++)
		{
			// see if this is a cash in this hand, and if so, move it up
			CPlay* pPlay = m_playPlan[i];
			if ((pPlay->GetSuit() == m_nPrioritySuit))
			{
				// advance it forward ahead of all plays in other suits
				// except for trump plays
				for(int j=0;j<i;j++)
				{
					CPlay* pExistingPlay = m_playPlan[j];
					if ((pExistingPlay->GetSuit() != m_nPrioritySuit) &&
						(pExistingPlay->GetSuit() != nTrumpSuit))
					{
						// NCR-707 skip moving play if existing play has high quality
						if((pExistingPlay->GetQuality() >= 1.0)  // NCR-707
							&& (pExistingPlay->GetQuality() > pPlay->GetQuality()) )
						{
#ifdef _DEBUG  // NCR DEBUGGING
//							theApp.SetValue(tnFileProgramBuildNumber, 1234); // pass flag to cause saving of board			
#endif
							continue; // NCR-707 leave in place
						}
						m_playPlan.MovePlay(i, j);  // NCR-NOTE: Move i to before j 
						numPlaysMoved++;
						break;
					}
				}  // end for(j)
			}
		}  // end for(i)

		// sequencing complete
		bSequenced = TRUE;

#ifdef _DEBUG  // NCR  following for debug
		if (numPlaysMoved > 0)
			status << "5PLSEQ6a! moved up " & numPlaysMoved & " plays using the " 
			          & STSS(m_nPrioritySuit) & " suit.\n";
#endif
	}



	//
	if (nTrumpSuit != NOTRUMP)
	{
		// suit contract play
		// check if the # of winners 
	}

	//
	// NCR-407 Don't do Type3 finesse if could go down
	CPlayList* pType3Fin = m_playPlan.GetPlaysOfType(CPlay:: FINESSE);
	if(pType3Fin != NULL) 
	{
		for(int i=0; i < pType3Fin->GetSize(); i++) 
		{
			CFinesse * aFinesse = (CFinesse*)pType3Fin->GetAt(i);
			if(aFinesse->GetSubType() == CFinesse::TYPE_III) {
				// Check how many tricks we can loss before using this
				int nSuit = aFinesse->GetGapCards()->GetAt(0)->GetSuit();
				CCombinedSuitHoldings & suit = m_pCombinedHand->GetSuit(nSuit);

				// Test if there is outstanding winner in this suit 
				if(!suit.IsVoid() && suit.GetNumMissingSequences() > 0 
					&& (suit.GetMissingCardVal(0) < suit.GetTopCardVal()) 
				     // If not, don't risk losing a trick here if ...
					&& ((13 - pDOC->GetNumTricksPlayed() - m_numTricksLeftToBeMade) < 2 ) ) 
				{
					int retVal = m_playPlan.MovePlayToEnd(aFinesse); // Move to end
					status << "4PLSEQ55! Move the Type 3 Finesse (" & aFinesse->GetName()
							  & ") to the end.\n";
				}
			} // end looking at type3 finesse
		} // end for(i) thru finesses
	} // NCR-407 end testing if Type3 Finesse OK
	
	delete pType3Fin;

	// NCR-421 Don't ruff if need to Force or Finesse (ie could lose control)
	// And dont' have more than 1 trump left
	if(ISSUIT(nTrumpSuit) && ((m_numDummyTrumps+m_numDeclarerTrumps) < 2) 
		&& (m_pCombinedHand->GetValuePV(tnumSuitsUnstopped) > 0))
	{
		CPlayList* pRuffs = m_playPlan.GetPlaysOfType(CPlay::RUFF);
		if(pRuffs != NULL) 
		{
			for(int i=0; i < pRuffs->GetSize(); i++) 
			{
				CPlay * pRuff = pRuffs->GetAt(i);
				int retVal = m_playPlan.MovePlayToEnd(pRuff); // Move to end ????
				status << "4PLSEQ57! Move the Ruff (" & pRuff->GetName()
				        & ") to the end.\n";
			}
		}
		delete pRuffs;
	}  // NCR-421 end moving Ruff to end if short of trumps as stoppers


	//
	// done; now interleave the plays
	//
//NCR moved	InterleavePlays();
} // end SequencePlays()






//
// InterleavePlays()
//
// interleave the play plan to avoid stranding winners
//
void CDeclarerPlayEngine::InterleavePlays()
{
	// 
	CPlayerStatusDialog& status = *m_pStatusDlg;
	status << "!3PINTLV! Interleaving plays...\n";

	//
	int numPlays = m_playPlan.GetSize();
	if (numPlays == 0)
		return;
//	int numPlaysMoved = 0;
	BOOL bPlayingInHand = (pDOC->GetCurrentPlayer() == m_pPlayer);
	int nCurrentHand = bPlayingInHand? CPlay::IN_HAND : CPlay::IN_DUMMY;
	//
	int nTrumpSuit = pDOC->GetTrumpSuit();
//	int numOutstandingTrumps = ISSUIT(nTrumpSuit)? GetNumOutstandingCards(nTrumpSuit) : 0;


	//
	// but we need to check entries first!
	//
	EvaluateEntries();
	BOOL bSufficientEntries = TRUE;
	int numRemainingTricks = m_numTricksLeftToBeMade;
	// 
	if ( (m_pCombinedHand->GetNumDeclarerWinners() < numRemainingTricks) &&
		 (m_pCombinedHand->GetNumDummyWinners() < numRemainingTricks) )
	{
		// we need entries into both hands
		if ((m_numDeclarerEntries == 0) || (m_numDummyEntries == 0))
			bSufficientEntries = FALSE;
	}


	//
	//------------------------------------------------------------------------
	//
	//  *** Long/Short Hand Handling ***
	//
	//
	// if we have a longer hand and a shorter hand, with winners in both hands,
	// cash the winners from the shorter hand first, UNLESS the shorter hand
	// only has one card which is a winner that's lower than a winner in the other hand
	// NCR-462 or the longer hand has more top cards than there are cards outstanding
	// and also more than the short hand
	//
	// examples:
	// ---------
	//     KQx/Ax -- play the Ace first, then lead back to the K
	//     Axx/QJ -- play the Q first, then lead the J back to the Ace
	//     KJ/Q   -- exception: play the King first, we strand it if we play the J
	//     QJx/AK -- doesn't matter; we end up stranding the Q or J either way
	//
	// examine each suit in turn
	for(Suit nSuit=CLUBS; nSuit <= SPADES; GETNEXTSUIT(nSuit))
	{
		// see if the suit has winners in both hands, and one hand is longer
		CCombinedSuitHoldings& suit = m_pCombinedHand->GetSuit(nSuit);
		CSuitHoldings& declarerSuit = suit.GetDeclarerSuit();
		CSuitHoldings& dummySuit = suit.GetDummySuit();
		//
		if ((suit.GetNumDeclarerWinners() > 0) && (suit.GetNumDummyWinners() > 0) &&
				(declarerSuit.GetNumCards() != dummySuit.GetNumCards()))
		{
			// check for the exception to avoid stranding, as in KJ/Q
			if ( ((declarerSuit.GetNumCards() >= 2) && (dummySuit.GetNumCards() == 1) 
				  && (*declarerSuit[0] > *dummySuit[0]))
				|| ((dummySuit.GetNumCards() >= 2) && (declarerSuit.GetNumCards() == 1) 
				    && (*dummySuit[0] > *declarerSuit[0])) )
				continue;	// the higher cash is first in the natural order

			//
			BOOL bDeclarerLong = (declarerSuit.GetNumCards() > dummySuit.GetNumCards())? TRUE : FALSE;

			// NCR-462 Don't need to move plans if longest hand has >= number of outstanding cards
			if(bDeclarerLong) 
			{
				if((declarerSuit.GetSequence2(0).GetNumCards() >= m_pCombinedHand->GetSuit(nSuit).GetNumOutstandingCards())
					&& (declarerSuit.GetSequence2(0).GetNumCards() >= dummySuit.GetNumCards()))
					continue; // skip
			}else{
				if((dummySuit.GetSequence2(0).GetNumCards() >= m_pCombinedHand->GetSuit(nSuit).GetNumOutstandingCards())
					&& (dummySuit.GetSequence2(0).GetNumCards() >= declarerSuit.GetNumCards()))
					continue; // skip
			}  // NCR-462 end testing if suit long enough to not worry

			// NCR-430 If we have long suit with many winners, move the short-side cash forward
			bool bMoveShortCashForward = ((nTrumpSuit == NOTRUMP) 
											// NCR Same as NoTrump if no trumps out
				                          || (m_pCombinedHand->GetSuit(nTrumpSuit).GetNumOutstandingCards() == 0))
										 && (suit.GetLength() >= 8)
				                         && (suit.GetMaxLength() >= 6) && (suit.GetNumTopHonors() >= 4);  
				
			// search for cashing plays in this suit
			int numPlaysToExamine = numPlays - 1;
			for(int j=0;j<numPlaysToExamine;j++)
			{
				CPlay* pPlay = m_playPlan[j];
				if(pPlay->GetSuit() != nSuit)
					continue; // NCR-524 skip plays not in current suit
				// NCR-524 look for both CASHs and TRUMP_PULLs
				if ((pPlay->GetPlayType() == CPlay::CASH) || (pPlay->GetPlayType() == CPlay::TRUMP_PULL))  
// NCR-524				if ((pPlay->GetPlayType() == CPlay::CASH) && (pPlay->GetSuit() == nSuit))
				{
					// found a play -- see if we need to move it
					int nOppHand = CCash::IN_EITHER;  // NCR-524 give initial value not used below
					if ((pPlay->GetTargetHand() == CPlay::IN_HAND) && bDeclarerLong)
						nOppHand = CCash::IN_DUMMY;	// plays to move behind
					else if ((pPlay->GetTargetHand() == CPlay::IN_DUMMY) && !bDeclarerLong)
						nOppHand = CCash::IN_HAND;
					else
					{
						// NCR-430 Move this cash forward
						if(bMoveShortCashForward) 
						{
							m_playPlan.MovePlayToFront(pPlay);
							status << "4PINTLV5! Advance the [" & pPlay->GetName() & 
								  "] so the suit winners in " & (bDeclarerLong? "dummy" : "hand") &
								  " are cashed first, to avoid possible stranding.\n";
						}  // NCR-430 end moving cash of short forward
						continue;	// no need to move the play
					}

					// move the play behind the last suit cash (or trump pull)
					int nPos = -1;
					for(int k=j;k<numPlays;k++)
					{
						CPlay* pSecondPlay = m_playPlan[k];
						if ((pPlay->GetPlayType() == CPlay::TRUMP_PULL) 
							 && (pSecondPlay->GetPlayType() == CPlay::TRUMP_PULL) 
							 &&	(pSecondPlay->GetTargetHand() == nOppHand))
							nPos = k;
						// NCR-524 Added following else and split conditions to one per line
						else if ((pPlay->GetPlayType() == CPlay::CASH) 
							    && (pSecondPlay->GetPlayType() == CPlay::CASH)
								&& (pSecondPlay->GetSuit() == nSuit) 
								&& (pSecondPlay->GetTargetHand() == nOppHand))
							nPos = k;
					} // end for(k) thru rest of plays

					if (nPos > j)
					{
						m_playPlan.MovePlay(j, nPos);
						j--;
						numPlaysToExamine--;
						status << "4PINTLV10! Postpone the [" & pPlay->GetName() & 
								  "] so the suit winners in " & (bDeclarerLong? "dummy" : "hand") &
								  " are cashed first, to avoid possible stranding.\n";
					}
				} // end looking at cash or trumppull plays
			} // end for(j) thru numPlaysToExamine
		}
	} // end for(nSuit) thru the suits
	

	//
	//------------------------------------------------------------------------
	//
	//  *** Crossover Sequencing ***
	//
	//
 	BOOL bCrossForNextPlay = FALSE, bStrandedWinners = FALSE;

	// Test #1:
	// -------
	//   if we need to cross to execute the next play, promote the next crossing
	//   play to the front
	//   BUT, try to cross with a different suit if possible, so as not
	//   to eliminate key cards for a finesse
	CPlay* pPlay = m_playPlan[0];
	int nStartingHand = pPlay->GetStartingHand();
	if (pDOC->GetCurrentPlayerPosition() == GetPlayerPosition())
		nCurrentHand = CPlay::IN_HAND;
	else
		nCurrentHand = CPlay::IN_DUMMY;
	//
	if ((nStartingHand != CPlay::IN_EITHER) && (nStartingHand != nCurrentHand))
	{
		// but for now, don't try to cross for a ruff that requires discards
//		CRuff* pRuff = dynamic_cast<CRuff*> (pPlay);
//		if (pRuff && pRuff->GetNumDiscardsRequired() > 0))
		if ((pPlay->GetPlayType() == CPlay::RUFF) && 
					(((CRuff*)pPlay)->GetNumDiscardsRequired() > 0))
		{
			// hold off on this one for now
		}    // NCR-421 Make sure not cash with required played cards
		else if ((pPlay->GetPlayType() == CPlay::CASH) // && (pPlay->GetSuit() != nTrumpSuit)
			      && (pPlay->GetRequiredPlayedCardsList() == NULL))
		{
			bCrossForNextPlay = TRUE;
		}
	}

	// Test #2:
	// ------
	// see if the opposite hand has stranded winners (for whatever reason),
	// and if so, try to cross over there
	// "stranded" means that the opposite hand is void (ie can't lead to the winners)
	// Ques: Can trumps be stranded???
	//
	EvaluateEntries();

	int numStrandedWinners = 0, 
		numOppositeStrandedWinners = 0;
	int nStrandedSuit = NONE;  // NCR Save stranded suit for message
	//
	for(int nSuit2 = CLUBS; nSuit2 <= SPADES; nSuit2++)
	{
		CCombinedSuitHoldings& combinedSuit = m_pCombinedHand->GetSuit(nSuit2);
		int numWinners = bPlayingInHand? combinedSuit.GetNumDeclarerWinners() : combinedSuit.GetNumDummyWinners();
		BOOL bNoSuitEntries = bPlayingInHand? (combinedSuit.GetNumDummyCards() == 0) : (combinedSuit.GetNumDeclarerCards() == 0);

		// NCR-457 Test if "winners" are first trick or if there are higher cards out
		// NO sense cossing if our "winners" are not immediate
		// Need to add tests for extra number of entries if we need to force out higher cards
		BOOL bHaveTopCard = ((combinedSuit.GetNumOutstandingCards() <= 0)
			                || (!combinedSuit.IsVoid() && (combinedSuit.GetTopCardVal() > combinedSuit.GetMissingCardVal(0))) );
		if ((numWinners > 0) && bNoSuitEntries  && bHaveTopCard)  // NCR-457 added test for having top card
//		if ((numWinners > 0) && (bNoSuitEntries) && (i != nTrumpSuit))
		{
			numStrandedWinners++;
			nStrandedSuit = nSuit2; // NCR save for msg later
		}
/*  NCR-457 Merge into one loop
	}
	//
	for(i=0;i<4;i++)
	{
		CCombinedSuitHoldings& combinedSuit = m_pCombinedHand->GetSuit(i);
*/ 
		int numOppWinners = bPlayingInHand? combinedSuit.GetNumDummyWinners() : combinedSuit.GetNumDeclarerWinners();
		BOOL bNoOppSuitEntries = bPlayingInHand? (combinedSuit.GetNumDeclarerCards() == 0) : (combinedSuit.GetNumDummyCards() == 0);
		if ((numOppWinners > 0) && bNoOppSuitEntries && bHaveTopCard)  // NCR-457 added test for having top card
//		if ((numWinners > 0) && (bNoSuitEntries) && (i != nTrumpSuit))
		{
			numOppositeStrandedWinners++;
			nStrandedSuit = nSuit2; // NCR save for msg later
		}
	}  // end for(nSuit2) thru 4 suits looking for "stranded" winners

	// an entry back is not guaranteed!!!  but it's a useful guide nonetheless
	BOOL bHaveEntryBack = bPlayingInHand? m_numDeclarerEntries : m_numDummyEntries;

	// if we have stranded winners in the opposite hand, try to find a crossing play
	if ( ((numStrandedWinners == 0) && (numOppositeStrandedWinners > 0)) ||
		 ((numOppositeStrandedWinners > numStrandedWinners) && bHaveEntryBack) )
		bStrandedWinners = TRUE;

	// but if we have stranded winners in each hand, don't cross unless we're 
	// down to our last entry 
	if (numOppositeStrandedWinners > numStrandedWinners)
	{
		if (nCurrentHand == CPlay::IN_HAND)  // NCR changed = to ==
		{
			// crossing into dummy? make sure we're done to our last dummy entry
			if (m_numDummyEntries > 1)
				bStrandedWinners = FALSE;
		}
		else
		{
			// crossing into hand? make sure we're down to our last dummy entry
			if (m_numDeclarerEntries > 1)
				bStrandedWinners = FALSE;
		}
	}

	// NCR-463 Postpone going for stranded winners if we have a Drop play on top
	//
	// Test if top play is a Drop
	if(bStrandedWinners && (pPlay->GetPlayType() == CPlay::DROP) 
	   && (pPlay->GetConsumedCard()->GetSuit() != nStrandedSuit) )
	{
		bStrandedWinners = FALSE; // turn off
	}  //  NCR-463 end turning off flag to cross over for stranded winner

	//
	// now try to facilitate the crossover
	//
	// new code -- don't cross twice in a row!!!
	//
	if ((bCrossForNextPlay || bStrandedWinners) && !m_bCrossedOnLastPlay)
	{

		// we're not in the correct hand for the next play, so move
		// find the first play (cash, trump pull, or ruff) of a different suit 
		// that ends up in the target hand
		int numPlays = m_playPlan.GetSize();
		int nOppositeHand = bPlayingInHand? CPlay::IN_DUMMY : CPlay::IN_HAND;
		CPlay* pCurrPlay;
		// first look for a trump pull or ruff (the cheapest crossing plays)
		int i; // NCR-FFS added here, removed below
		for(/*int*/ i=0;i<numPlays;i++)
		{
			pCurrPlay = m_playPlan[i];
			int nPlayType = pCurrPlay->GetPlayType();
			if ( ((nPlayType == CPlay::TRUMP_PULL) || (nPlayType == CPlay::RUFF)) &&
				  (pCurrPlay->GetTargetHand() == nOppositeHand) && pCurrPlay->IsWinner() &&
				  (pCurrPlay->GetSuit() != pPlay->GetSuit()) && IsPlayUsable(*pCurrPlay))
				break;	// OK, found a trump pull or ruffing  crossing play
		}
		if (i == numPlays)
		{
			// failing that, look for a cash
			for(i=0;i<numPlays;i++)
			{
				pCurrPlay = m_playPlan[i];
				int nPlayType = pCurrPlay->GetPlayType();
				if ( (nPlayType == CPlay::CASH) && (pCurrPlay->GetSuit() != nTrumpSuit) &&
					 (pCurrPlay->GetTargetHand() == nOppositeHand) && pCurrPlay->IsWinner() &&
					 (pCurrPlay->GetSuit() != pPlay->GetSuit()) && IsPlayUsable(*pCurrPlay))
					break;	// OK, found a cashing crossing play
			}
		}


		// TODO: provide code that allows a play of the same suit to be used
		// for crossing, if it doesn't prevent the play from being used
		//
		if (i < numPlays) 
		{
			// found an appropriate play; advance it to the front 
			// if this play is a cash, place it ahead of all non-trump pull plays
			if (m_playPlan[i]->GetPlayType() == CPlay::CASH)
			{
				int j; // NCR-FFS added here, removed below
				for(/*int*/ j=0;j<i;j++)
				{
					if (m_playPlan[j]->GetPlayType() != CPlay::TRUMP_PULL)
						break;
				}
				if (j < i)
				{
					if (bCrossForNextPlay)
						status << "3PINTLV70! Try to cross over to " & (bPlayingInHand? "dummy" : "hand") & 
								  " with the [" & m_playPlan[i]->GetName() & "] when possible to use the [" 
								  & pPlay->GetName() & "].\n";
					else
						status << "3PINTLV70A! Try to cross over to " & (bPlayingInHand? "dummy" : "hand") & 
								  " with the [" & m_playPlan[i]->GetName() 
								  & "] when possible to cash the stranded winners in " 
								  & STSS(nStrandedSuit) & "s.\n";   // NCR
					// NCR ??? NB:  This undoes move done at PINTLV10 to prevent stranding
//PINTLV10: Postpone the CK Cash so the suit winners in hand are cashed first, to avoid possible stranding.
//PINTLV70: Try to cross over to dummy with the [CK Cash] when possible to use the [SK Finesse].
					m_playPlan.MovePlay(i, j);
					m_bCrossedOnLastPlay = TRUE;
				}
			}
			else
			{
				// else it's a trump pull or a ruff, so place it at the front
				if (bCrossForNextPlay)
					status << "3PINTLV30! Use the [" & m_playPlan[i]->GetName() &"] to cross over to " 
					           & (bPlayingInHand? "dummy" : "hand") & 
								  " to use the [" & pPlay->GetName() & "].\n";
				else
					status << "3PINTLV31! Use the [" & m_playPlan[i]->GetName() &"] to cross over to " 
					           & (bPlayingInHand? "dummy" : "hand") & 
								  " to cash the stranded winners.\n";
				m_playPlan.MovePlayToFront(i);
				m_bCrossedOnLastPlay = TRUE;
			}
		}
		// NCR-438 No crossover plans found. See if we can create one using hand's entries
		else 
		{
			CCard* pCard = NULL;
			if(bPlayingInHand) {
				// Need entry to dummy
				if(m_dummyEntries.GetNumCards() > 0)
					pCard = m_dummyEntries.GetTopCard();
			} else {
				// Need entry to hand
				if(m_declarerEntries.GetNumCards() > 0)
					pCard = m_declarerEntries.GetTopCard();
			}
			int maxQualIdx = GetIndexOfBestQualPlay(m_playPlan);  // NCR-761
			int maxQual = maxQualIdx > 0 ? m_playPlan[maxQualIdx]->GetQuality() : 0;
                        // NCR-700 Don't add Cash if finesse in same suit
			if(pCard && !((pPlay->GetPlayType() == CPlay::FINESSE) && (pPlay->GetSuit() == pCard->GetSuit())) 
				&& (maxQual < 50))  // NCR-761 don't insert if have hi pri plans 
			{  // Flag as Opportunistic for the Cash class Not to skip when equivalent
				CCash* pCash = new CCash(GetCardOwner(pCard), CPlay::IN_EITHER, NULL, pCard, CPlay::PP_LIKELY_WINNER, TRUE);
				m_playPlan.AddPlay(0, pCash);  // Insert play at front of list
				m_bCrossedOnLastPlay = TRUE;
				status << "5PINTLV35! Add plan to cross over to " & (bPlayingInHand ? "dummy" : "hand") & " by playing "
						  & pCard->GetFullName() & ".\n";
			}

		} // end NCR-438
	}
	else
	{
		m_bCrossedOnLastPlay = FALSE;
	}


	//
	// -----------------------------------------------------------------------
	//
	//  *** Cash Sequencing ***
	//
	// - check if a suit in this hand has winners with no entries into the suit 
	//   in the particular hand and if so, cash them sooner rather than later
	//
//	for(int i=0;i<numPlays;i++)
	int numIters = numPlays-1;
	for(int i=numPlays-1;(i>0) && (numIters>0);i--,numIters--)	// go in reverse to preserve the order
	{
		//
//		BOOL bMoveUp = FALSE;
		CPlay* pPlay = m_playPlan[i];
		if ((pPlay->GetPlayType() == CPlay::CASH) && (pPlay->GetSuit() != nTrumpSuit)
			// NCR-421 Make sure there are NOT any required played cards
			&& (pPlay->GetRequiredPlayedCardsList() == NULL))
		{
			const int nCurrentHand = (pDOC->GetCurrentPlayer() == m_pPlayer) ? CPlay::IN_HAND : CPlay::IN_DUMMY;
			int nTargetHand = pPlay->GetTargetHand();
//			int nSuit = dynamic_cast<CCash*>(pPlay)->GetSuit();
			int nSuit = ((CCash*) pPlay)->GetSuit();
			CCombinedSuitHoldings& suit = m_pCombinedHand->GetSuit(nSuit);
			// NCR-277 Can we think of cashing if there are cards in the RequiredPlayedCardsList?
		    CCardList* pRequiredPlayedCards = pPlay->GetRequiredPlayedCardsList();

			// NCR-430 Cash winners in this suit that won't make any difference if played now
			// The cards are all in one hand, there will not be any outstanding after they are played
			// The suit is not needed for transportation.  The tricks can't be ruffed
			bool bOkToCashSuit = ((nTrumpSuit == NOTRUMP) 
				                  || (m_pCombinedHand->GetSuit(nTrumpSuit).GetNumOutstandingCards() == 0))
								 && (suit.GetNumWinners() >= suit.GetNumOutstandingCards())
								 && (nTargetHand == nCurrentHand)
								 && ((nTargetHand == CPlay::IN_HAND 
								       ? suit.GetNumDummyCards() : suit.GetNumDeclarerCards()) == 0);	

			// NCR-396 Use entries to hands to determine if we need to cash now
			// Don't cash if we have entries. In case there are promotions or finesses to do
			// Cashing can Unstop a suit. Don't want to cash before forcing an Ace or finessing
			int dclrEnt = GetNumDeclarerEntries();
			// NCR-563 add on possible entries if no trump
			int dmyEnt = GetNumDummyEntries();
			if ((nTrumpSuit == NOTRUMP) || (m_pCombinedHand->GetSuit(nTrumpSuit).GetNumOutstandingCards() == 0)) {
				dclrEnt += GetNumDeclarerPossibleEntries();  //NCR-563 add on possible ones also
				dmyEnt += GetNumDummyPossibleEntries();
            }
			// see if we have winners in one hand with no suit cards opposite
			if ( ((nTargetHand == CPlay::IN_HAND) && (nCurrentHand == CPlay::IN_HAND) 
				   && (suit.GetNumDummyCards() == 0) && (dclrEnt < 1)) // NCR-396 
				 || ((nTargetHand == CPlay::IN_DUMMY) && (nCurrentHand == CPlay::IN_DUMMY) 
				   && (suit.GetNumDeclarerCards() == 0) && (dmyEnt < 1)) // NCR-396 
				 // NCR-277 test no required cards. >>>> Need to test if card played yet???
//				 && (pRequiredPlayedCards == NULL)
				 || bOkToCashSuit  // NCR-430 
			   )
				 // NCR-264 Need to check if this card is stopper and if there is any unstopped suit that needs to be promoted
				 // NCR-264 Tests: no Forces needed; No outstanding cards; This hand has entries to be able to play this later
			{
				// NCR-775 Make sure this card is not a needed stopper
				CCardList outstandingCards;
				int numOutstandingCards = GetOutstandingCards(nSuit, outstandingCards);
				CCard* pSecCard = suit.GetSecondHighestCard();
				int numAboveSecCard = (pSecCard == NULL) ? 0 : outstandingCards.GetNumCardsAbove(pSecCard); 
				if ((numAboveSecCard > 1) && (numOutstandingCards > 3) )
					continue;  /// NCR-775 Skip cash if this card a stopper

				CString msg = "";  // NCR-277 Add message re
				if(pRequiredPlayedCards != NULL){ // NCR-277
					msg.Format(" >Note: %d required cards: %s", pRequiredPlayedCards->GetNumCards(),
						       pRequiredPlayedCards->GetTopCard()->GetFaceName());
				}
				status << "4PINTLV50! The cash of the " & pPlay->GetConsumedCard()->GetName() &
						  " can only be used here in " & ((nCurrentHand == CPlay::IN_HAND)? "the hand" : "dummy") &
						  " since there are no lead cards in the opposite hand -- so move up the play."
						  & msg & "\n"; // NCR-277 added stuff to message
				// move it ahead of all other plays, except for trump plays
				// i.e., put it just behind the last trump play if one exists
				BOOL bBehindTrump = FALSE;
				int j; // NCR-FFS added here, removed below
				for(/*int*/ j=i;j>=0;j--)
				{
//					if ((m_playPlan[j]->GetPlayType() == CPlay::TRUMP_PULL) &&
//						(m_playPlan[j]->GetTargetHand() == pPlay->GetTargetHand()))
					if (m_playPlan[j]->GetSuit() == nTrumpSuit)
					{
						bBehindTrump = TRUE;
						break;
					}
				}
				// move the play
				if (j < 0)
					j = 0;
				if ((j >= 0) && (j < i))
				{
					if (bBehindTrump)
					{
						// move the play to just after the trump play
						if (j < i-1)
							m_playPlan.MovePlay(i, j+1);
					}
					else
					{
						// move the play to the designated slot
						m_playPlan.MovePlay(i, j);
					}
					i++;
				}
			}  // end selecting play to move forward
		} // end testing Cash plays
	} // end for(i) thru play list


	//
	if (nTrumpSuit != NOTRUMP)
	{
	}

	// NCR-441 Check if the the first play is for the other hand - i.e. need to cross over for it!!
	CPlay* pFirstPlay = m_playPlan[0];
	if((pFirstPlay->GetPlayType() == CPlay::FINESSE) && (nCurrentHand == pFirstPlay->GetTargetHand())
		&& ((((CFinesse*)pFirstPlay)->GetSubType() == CFinesse::TYPE_I) 
		    || (((CFinesse*)pFirstPlay)->GetSubType() != CFinesse::TYPE_II)) )
	{
		status << "5PINTLV55! The play: [" & pPlay->GetDescription() & "] needs to be started from "
			       & ((nCurrentHand == CPlay::IN_HAND)? "dummy" : "the hand") & ".\n";

		// TODO: - Find play that will move us to the other hand

	} // end NCR-441

} // end InterleavePlays()





//
//-----------------------------------------------------------------------
//
// GetCurrentPlay()
//
// - returns the current play, if one is in progress
//   in the hand
//
const CPlay* CDeclarerPlayEngine::GetCurrentPlay()
{
	return m_pCurrentPlay;
}





//
//-----------------------------------------------------------------------
//
// FilterPlays()
//
// - filters out plays that require key cards that will be played earlier
//   in the hand
//
int CDeclarerPlayEngine::FilterPlays(CPlayList& playList)
{
	// skip this code for now, as it limits our options
	return 0;

	// look through the earlier plays in the sequence and see if
	// any of the key cards for this play are consumed earlier in the 
	// play list
	CPlayerStatusDialog& status = *m_pStatusDlg;

	// step through the specified play list
	int nIndex = 0;
	int nPlayListSize = playList.GetSize();
	for(int i=0;i<nPlayListSize;i++)
	{
		// grab the proposed play
		CPlay* pProposedPlay = playList[nIndex];
		ASSERT(pProposedPlay->IsValid());
		BOOL bSkipRemainingChecks = FALSE;

		//
		// now check the play's eligibility with regard to the other plays
		//

		// first get its list of OR key cards
		CCardList* pOrKeyCardsList = pProposedPlay->GetOrKeyCardsList();
		BOOL* pOrCardPresentList = NULL;
		int numOrKeyCards = pProposedPlay->GetNumOrKeyCards();
		if (numOrKeyCards > 0)
		{
			pOrCardPresentList = new BOOL[numOrKeyCards];
			for(int x=0;x<numOrKeyCards;x++)
				pOrCardPresentList[x] = TRUE;
		}
		// and the second set of or-cards as well
//		CCardList* pOrKeyCardsList2 = pProposedPlay->GetOrKeyCardsList2();
		BOOL* pOrCardPresentList2 = NULL;
		int numOrKeyCards2 = pProposedPlay->GetNumOrKeyCards2();
		if (numOrKeyCards2 > 0)
		{
			pOrCardPresentList2 = new BOOL[numOrKeyCards2];
			for(int x=0;x<numOrKeyCards2;x++)
				pOrCardPresentList2[x] = TRUE;
		}

		// then step through the existing play plan
		int numPlays = m_playPlan.GetSize();
		int j; // NCR-FFS added here, removed below
		for(/*int*/ j=0;j<numPlays;j++)
		{
			// first see if the consumed card for any pre-existing play
			// is required by the current proposed play
			CPlay* pExistingPlay = m_playPlan[j];
			if (pExistingPlay == pProposedPlay)
				continue;
//			CString strDescription = pExistingPlay->GetDescription();
			CCard* pPreviouslyConsumedCard = pExistingPlay->GetConsumedCard();
			if (pProposedPlay->RequiresCard(pPreviouslyConsumedCard))
			{
				// oops! can't use this play -- so delete it
				status << "5PLNFLT30! The <" & pProposedPlay->GetDescription() & 
						  "> play will be removed as it requires the " & pPreviouslyConsumedCard->GetName() &
						  ", which is used in the earlier " & pExistingPlay->GetDescription() & " play.\n";
				AdjustPlayCountForDeletedPlay(playList[nIndex]);
				m_playPlan.RemovePlay(nIndex, FALSE);
				m_discardedPlays.AppendPlay(pProposedPlay);				
				// decrement the counter to keep it where it is
				nIndex--;
				bSkipRemainingChecks = TRUE;	// to skip remaining viability tests
				break;
			}
			
			// also see if the existing play consumes one of the proposed play's 
			// OR key cards, and if so, mark it as such
			int nIndex = pProposedPlay->LookupORCard(pPreviouslyConsumedCard);
			if (nIndex >= 0)
				pOrCardPresentList[nIndex] = FALSE;
			// and repeat for the OR-2 cards
			nIndex = pProposedPlay->LookupORCard2(pPreviouslyConsumedCard);
			if (nIndex >= 0)
				pOrCardPresentList2[nIndex] = FALSE;
		}

		// now see if the play plan consumes all of the cards in the
		// proposed play's list of OR key cards
		if ((!bSkipRemainingChecks) && (numOrKeyCards > 0))
		{
			for(j=0;j<numOrKeyCards;j++)
				if (pOrCardPresentList[j])
					break;
			if (j == numOrKeyCards)
			{
				// oops! can't use this play -- so delete it
				if (numOrKeyCards > 1)
					status << "5PLNFLT40! The <" & pProposedPlay->GetDescription() &
							  "> play will be removed as it requires a card from among [" &
							  pOrKeyCardsList->GetHoldingsString() & 
							  "], which are all consumed earlier in the play plan.\n";
				else
					status << "5PLNFLT40! The <" & pProposedPlay->GetDescription() &
							  "> play will be removed as it requires the " &
							  (*pOrKeyCardsList)[0]->GetName() & 
							  ", which is consumed earlier in the play plan.\n";
				AdjustPlayCountForDeletedPlay(playList[nIndex]);
				m_playPlan.RemovePlay(nIndex, FALSE);
				m_discardedPlays.AppendPlay(pProposedPlay);				
				// decrement the counter to keep it where it is
				nIndex--;
				bSkipRemainingChecks = TRUE;
			}
		}

		// and do the same for the second set of OR key cards
		if ((!bSkipRemainingChecks) && (numOrKeyCards2 > 0))
		{
			for(j=0;j<numOrKeyCards2;j++)
				if (pOrCardPresentList2[j])
					break;
			if (j == numOrKeyCards2)
			{
				// oops! can't use this play -- so delete it
				if (numOrKeyCards2 > 1)
					status << "5PLNFLT41! The <" & pProposedPlay->GetDescription() &
							  "> play will be removed as it requires a card from among [" &
							  pOrKeyCardsList->GetHoldingsString() & 
							  "], which are all consumed earlier in the play plan.\n";
				else
					status << "5PLNFLT41! The <" & pProposedPlay->GetDescription() &
							  "> play will be removed as it requires the " &
							  (*pOrKeyCardsList)[0]->GetName() & 
							  ", which is consumed earlier in the play plan.\n";
				AdjustPlayCountForDeletedPlay(playList[nIndex]);
				m_playPlan.RemovePlay(nIndex, FALSE);
				m_discardedPlays.AppendPlay(pProposedPlay);				
				// decrement the counter to keep it where it is
				nIndex--;
				bSkipRemainingChecks = TRUE;
			}
		}

		// clean up
		delete[] pOrCardPresentList;
		delete[] pOrCardPresentList2;

		// and proceed to the next play
		nIndex++;
	}

	// done
	return 0;
}





//
// FindHoldUpPlays()
//
// - look to see if we shoulkd hold up
//
int CDeclarerPlayEngine::FindHoldUpPlays(CPlayList& playList, BOOL bExcludeTrumpSuit)
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	int nRound = pDOC->GetNumTricksPlayed();

	// no holdups in a suit contract, OR after the second round
	if (ISSUIT(pDOC->GetTrumpSuit()) || (nRound >= 2))
		return 0;

	// see if this is the second round
	// if so, must've held up the last round
	int nPreviousHoldUp = 0;
	if (nRound == 1)
	{
		if (m_usedPlays.GetSize() == 0)
			return 0;	// nope, didn't hold up last time
		CPlay* pPrevPlay = m_usedPlays.GetAt(0);
		if (!pPrevPlay || (pPrevPlay->GetPlayType() != CPlay::HOLDUP))
			return 0;   // didn't hold up
		nPreviousHoldUp = 1;
	}

	// check if we won the last round and hence are leading --
	// this could conceivably have happened even if we held up (???)
	if (pDOC->GetNumCardsPlayedInRound() == 0)
		return 0;

	// see if a holdup is appropriate
	// check the suit that was led
	CCard* pLeadCard = pDOC->GetCurrentTrickCardByOrder(0);
	int nSuitLed = pLeadCard->GetSuit();
	CCombinedSuitHoldings& suit = m_pCombinedHand->GetSuit(nSuitLed);

	// only hold up if we have an Ace or King in the suit
	if (suit.GetNumCards() == 0)
		return 0;
	if ((suit[0]->GetFaceValue() != ACE) && (suit[0]->GetFaceValue() != KING))
		return 0;

	// no need to hold up if we have more than 1 top winner in the suit
	// (or zero winners, of course)
	if ((suit.GetNumMaxTopCards() > 1) || (suit.GetNumWinners() == 0))
		return 0;

	// don't hold up if we have less than 6 cards in the suit total in round 1,
	// or 4 cards in round 2
	if ( ((nRound == 0) && (suit.GetNumCards() < 6)) ||
		 ((nRound == 1) && (suit.GetNumCards() < 4)) )
		return 0;

	// we have 1 winner in the suit, so hold up up to 2 times
	int numHoldUps = 2 - nRound;

	// don't hold up more than once if trying for a slam
	numHoldUps = Min(numHoldUps, 7 - pDOC->GetContractLevel());

	// NCR-709 Don't holdup if can only lose one trick
	int nTeam = ::GetPlayerTeam(m_pPlayer->GetPosition());
	int nTricksMade = pDOC->GetValue(tnumTricksWon, nTeam);
	int nTricksLeftToBeMade =  m_numRequiredTricks - nTricksMade;
	if(((13 - nRound) - nTricksLeftToBeMade) < 2)
		return 0;  // NCR-709 no holdup if only 1


	// adjust for previous holdup, if we played one
	if (numHoldUps <= 0)
		return 0;

	// and add the plays
	for(int i=0;i<numHoldUps;i++)
	{
		CHoldUp* pPlay = new CHoldUp(nSuitLed);
		playList << pPlay;
	}

	// spout a message 
	status << "4PLNHLD5! " & numHoldUps & (nPreviousHoldUp? " more" : "") & " hold-up" & ((numHoldUps > 1)? "s" : "") &
			  " may be appropriate.\n";

	// done
	return numHoldUps;
}





//
// FindCashingPlays()
//
// - look for easy cashing plays
// - returns the # of maximum usable cashes (as opposed to the total possible)
// NCR-449 also returns number of cashes that require other cards to have been played
//
int CDeclarerPlayEngine::FindCashingPlays(CPlayList& playList, int& numReqdCards, BOOL bExcludeTrumpSuit)
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	int i,nSuit,numSuitTopCards;
	int nTricksCount = 0;
//	int numTotalTopCards = m_pCombinedHand->GetNumTopCards();
	int numTotalTopCards = m_pCombinedHand->GetNumWinners();
	int numMaxCashes = 0;

	if (numTotalTopCards == 0)
	{
		status << "4PLNCSH0! The hand has no top cards that can be cashed.\n";
		return 0;
	}

	// pick the starting suit -- make the trump suit the last 
	if (ISSUIT(m_nTrumpSuit))
		nSuit = GetNextSuit(m_nTrumpSuit);
	else
		nSuit = SPADES;

	//
	status << "4PLNCSH1! Looking at the hand for cashes...\n";
	CPlayList localPlayList;

	// is the play guaranteed?
	int numOutstandingTrumps = ISSUIT(m_nTrumpSuit)? GetNumOutstandingCards(m_nTrumpSuit) : 0;
	CPlay::PlayProspect nPlayProspect = (numOutstandingTrumps == 0)? CPlay::PP_GUARANTEED_WINNER : CPlay::PP_LIKELY_WINNER;

	//
	int numMaxSuitCashes[4] = { 0, 0, 0, 0};
	int numRequiredCardsBePlayed = 0; // NCR-449  This should be zero for sure tricks

	// Scan the suits for winners for cashing
	for(i=0;i<4;i++)
	{

		// skip if not pulling honors from trumps
		if ((nSuit == m_nTrumpSuit) && (bExcludeTrumpSuit))
		{
			nSuit = GetNextSuit(nSuit);
			continue;
		}

		// see if we have top winners in this suit
		CCombinedSuitHoldings& suit = m_pCombinedHand->GetSuit(nSuit);
		numSuitTopCards = suit.GetNumWinners();  // NCR-240 this better for drawing trumps
		numSuitTopCards = suit.GetNumSureWinners(); // NCR-587 
#ifdef _DEBUG_XXX   // NCR-587
		status << "DEBUG5! " & SuitToString(nSuit) & " numWinners=" & suit.GetNumWinners() 
			    & ", numSureWinners=" & suit.GetNumSureWinners() & "\n";
#endif
//		numSuitTopCards = suit.GetNumTopCards(); // NCR-190 Which of these 2 to use???
		ASSERT(numSuitTopCards <= suit.GetNumCards());
		numMaxSuitCashes[nSuit] = Min(numSuitTopCards, suit.GetMaxLength());
		numMaxCashes += numMaxSuitCashes[nSuit];

		// note that "top cards" may not be instant winners; i.e., they may
		// require that previous cashes / force plays remove enemy top cards
		CCardList outstandingCards;
		GetOutstandingCards(nSuit, outstandingCards);

		bool bMoveSuitToFront = false;   // NCR-683 Remember if this suit should go first

		int nInsertIdx = 0;  // NCR-456 insert point for plans moved forward
		// so add the plays and their requirements
		int j; // NCR-FFS added here, removed below
		for(/*int*/ j=0;j<numSuitTopCards;j++)
		{
			// get the list of higher outstanding enemy cards
			int numHigherCards = outstandingCards.GetNumCardsAbove(suit[j]);
			CCardList* pRequiredPlayedList = NULL;
			if (numHigherCards > 0)
			{
				// NCR-488 Test if we could be lucky and get a drop without have ALL the top cards
				// Only test on the first round ie when all cards are still out
				if((outstandingCards.GetNumCards() + suit.GetNumCards()) == 13) {
					int x = 99; // debug stop point
				}
				pRequiredPlayedList = new CCardList;
				for(int k=0;k<numHigherCards;k++) {
					*pRequiredPlayedList << outstandingCards[k];
				}
				// NCR-449 Accumulate number of cashes with required cards lists
				numRequiredCardsBePlayed++;  // NCR-449 count
			}
/*  Logic for 654 moved to SequencePlays
			else // numHigherCards = 0
			{  // NCR-654 Don't cash a singleton that could block the suit
				// Safe to ignore singleton if its FV < FV top card long hand 
				//  And there are enough winners
				// Should this test be here or later in the analysis and plan moving stage???
//				ASSERT(suit.GetNumDummyCards() == suit.GetDummyLength());  //<<<<<<<<<<< DEBUG always true
//				ASSERT(suit.GetNumDeclarerCards() == suit.GetDeclarerLength());  //<<<<<<<<<<< DEBUG
				if(((GetCardOwner(suit[j]) == CPlay::IN_DUMMY) 
					&& (suit.GetDummyLength() == 1) && (suit.GetDeclarerLength() > 1)
					&& (suit.GetDummyCard(0)->GetFaceValue() < suit.GetDeclarerCard(0)->GetFaceValue()))
				  || ((GetCardOwner(suit[j]) == CPlay::IN_HAND) 
					&& (suit.GetDeclarerLength() == 1) && (suit.GetDummyLength() > 1)
				    && (suit.GetDeclarerCard(0)->GetFaceValue() < suit.GetDummyCard(0)->GetFaceValue())) ) 
				{
//REMOVED				   continue;            // skip cashing this card
				}
			}  // end NCR-654
*/
			// see if the starting hand matters
			int nStartingHand = CPlay::IN_EITHER;
			if ((suit.GetNumDeclarerCards() > 0) && (suit.GetNumDummyCards() == 0))
				nStartingHand = CPlay::IN_HAND;
			else if ((suit.GetNumDummyCards() > 0) && (suit.GetNumDeclarerCards() == 0))
				nStartingHand = CPlay::IN_DUMMY;
			// and add
			CCash* pPlay = new CCash(GetCardOwner(suit[j]), nStartingHand,
				                     pRequiredPlayedList, suit[j], nPlayProspect);

			// NCR-456 What if one hand has singleton that is a winner 
			// that could be overtaken?
			bool bMoveToFront = false;       // NCR-623 only worry if > 1 OS card 
			if((suit.GetMinLength() == 1) && (outstandingCards.GetNumCards() > 1))
			{
				if(suit.GetDummyLength() < suit.GetDeclarerLength()){
					// Dummy has the singleton, Is it a winner?
					if((suit.GetNumDummyWinners() == 1) && (GetCardOwner(suit[j]) == CPlay::IN_DUMMY)
						// and not all of declarer's cards are winners
						&& (suit.GetNumDeclarerWinners() < suit.GetNumDeclarerCards()))
						bMoveToFront = true;
				}
				else {  
					// Must be declarer that has the singleton
					if((suit.GetNumDeclarerWinners() == 1) && (GetCardOwner(suit[j]) == CPlay::IN_HAND)
						&& (suit.GetNumDummyWinners() < suit.GetNumDummyCards()))
						bMoveToFront = true;
				}
			}  // end NCR-456 testing for singleton

			// NCR-683 Also test if winners can be cut off. If so, move this suit's plays in front
			if(  ((nStartingHand == CPlay::IN_HAND) &&(suit.GetDummyLength() == 0))
			  || ((nStartingHand == CPlay::IN_DUMMY) &&(suit.GetDeclarerLength() == 0)) ) 
			{
				bMoveSuitToFront = true;
			} // end NCR-683

			// NCR-761 Move play if it could block suit
			// EG Dummy=KJ653 & Hand=QT need to cash Q first then lead T to K
			CCard* pCardToPlay = suit[j];
			bool bCardInHand = GetCardOwner(pCardToPlay) == CPlay::IN_HAND;
			CSuitHoldings& handWithCard = bCardInHand ? suit.GetDeclarerSuit() : suit.GetDummySuit();
			CSuitHoldings& otherHand = bCardInHand ? suit.GetDummySuit() : suit.GetDeclarerSuit();
		
			// NCR-761 Move cash to front if in doubleton and this card > 2nd card
			//  and the other hand has more cards
			if(handWithCard.IsDoubleton() && (otherHand.GetSecondHighestCard() < pCardToPlay) 
				&& (otherHand.GetNumCards() > handWithCard.GetNumCards()) ) {
				bMoveToFront = true;    // NOTE: Not any use if the list is empty!!!
				pPlay->SetQuality(77);  //????
			}  // end NCR-761


			if(bMoveToFront)
				localPlayList.AddPlay(nInsertIdx++, pPlay);  // To front and incr index 
			else
				localPlayList << pPlay;

			nTricksCount++;
		} // end for(j) thru this suit's top cards


		// spout a message 
		if (numSuitTopCards > 0)
		{
			if (numMaxSuitCashes[nSuit] == nTricksCount)
				status << "5PLNCSH3! We can cash " & numMaxSuitCashes[nSuit] & " " &
						   ((numSuitTopCards > 1)? "cards" : "card") &
					      " in " & SuitToString(nSuit);
			else
				status << " " & numMaxSuitCashes[nSuit] & " in " & SuitToString(nSuit);
			if (bExcludeTrumpSuit)
			{
				// are there more top cards aside from those in the trump suit?
				if (nTricksCount < (numTotalTopCards - m_pCombinedHand->GetSuit(m_nTrumpSuit).GetNumTopCards()))
					status < ", plus";
			}
			else
			{
				if (nTricksCount < numTotalTopCards)
					status < ", plus";
			}
		}


		// now copy plays to the master list
		int numCashes = localPlayList.GetSize();
		int nInsertIdx2 = 0;  // NCR-683  for moving first
		for(j=0;j<numCashes;j++) {
			CPlay * pNextPlay = localPlayList.PopPlay();
			if(bMoveSuitToFront)   // NCR-683 should this suit's plays go first?
				playList.AddPlay(nInsertIdx2++, pNextPlay);
			else if(pNextPlay->GetQuality() > 75.0)
				playList.AddPlay(0, pNextPlay);  // NCR-761 add to front ???
			else
				playList << pNextPlay;
		}

		// and proceed to the next suit
		nSuit = GetNextSuit(nSuit);
	}  // end for(i) thru suits



	if (nTricksCount > 0)
		status < ".\n";


	// NCR-411 Find and save discard suit for Cashes
	if((nPlayProspect == CPlay::PP_GUARANTEED_WINNER) && (nTricksCount >= m_numTricksLeftToBeMade))
	{
		// Find the suit in the numMaxSuitCashes[] list with no cashes
		int idx = 0;
		for(idx=0; idx < 4; idx++) {
			if((numMaxSuitCashes[idx] == 0) && (m_pCombinedHand->GetSuit(idx).GetNumCards() > 0)
				&& (idx != m_nTrumpSuit))
				break;  // exit with idx set
		} // end for(idx) thru suits looking for suit with no cashes

		// If we found one, save it in the CCash object
		if(idx < 4) {
			int numPlays = playList.GetSize();
			for(int i=0;i<numPlays;i++) 
			{
				// Check only cashing plays
				CPlay* pPlay = playList[i];
				CCombinedSuitHoldings& suit = m_pCombinedHand->GetSuit(idx);
 				if (pPlay->GetPlayType() == CPlay::CASH) {
					if( ((pPlay->GetStartingHand() == CPlay::IN_HAND) && (suit.GetNumDummyCards() > 0) )
						|| ((pPlay->GetStartingHand() == CPlay::IN_DUMMY) && (suit.GetNumDeclarerCards() > 0)) )
					pPlay->SetSecondSuit(idx); // save
				}
			}
		}
	} // NCR-411 end saving suit to discard

	//
	// now look for a delayed cashing trick
	//
	int nDelayedTricksCount = 0;
	if (pDOC->GetNumCardsPlayedInRound() == 1)
	
	{
		// a delayed cash is a card that we can cash now or later, depending on
		// what gets played by the opponents (specifically, RHO)
		// e.g., a Kx in hand is normally not a cash, but it becomes worth 1 
		// delayed cash if LHO plays a low card of the suit
		CCard* pCardLed = pDOC->GetCurrentTrickCardByOrder(0);
		nSuit = pCardLed->GetSuit();
		CCombinedSuitHoldings& suit = m_pCombinedHand->GetSuit(nSuit);
		CSuitHoldings& dummy = suit.GetDummySuit();
		CSuitHoldings& declarer = suit.GetDeclarerSuit();
		//
		if (suit.GetNumMissingSequences() > 0)
		{
			CCardList& missingTopCards = suit.GetTopMissingSequence();
			int numTricksLeft = 13 - pDOC->GetNumTricksPlayed();
			//
			if ((pDOC->GetCurrentPlayer() == m_pPartner) &&
				(pCardLed->GetSuit() == nSuit) && (dummy.GetNumWinners() == 0) && 
				(missingTopCards.GetNumCards() == 1) && (declarer.GetNumCards() >= 2) && 
				((*(missingTopCards[0]) - *(declarer[0])) == 1) && (numTricksLeft > m_numTricksLeftToBeMade))
			{
				// create a delayed cash of the top card in hand
				// but see if this cash is already in the play list
				CCard* pCashCard = missingTopCards[0];
				int numExistingPlays = playList.GetSize();
				for(i=0;i<numExistingPlays;i++)    // NCR removed int 
					if (playList[i]->GetConsumedCard() == pCashCard)
						break;
				if (i == numExistingPlays)	// card is not in the play list
				{
					CCardList* pRequiredPlayList = NULL;
					// NCR-188 test if outstanding card > than our card
					if(missingTopCards.GetTopCard()->GetFaceValue() > declarer.GetTopCard()->GetFaceValue())
					{
						// NCR-188 following 2 statements uncommented to recognize that declarer's card may not win
						pRequiredPlayList = new CCardList;
						pRequiredPlayList->Add(missingTopCards[0]);
					}
					CCash* pPlay = new CCash(CPlay::IN_HAND, CPlay::IN_DUMMY, pRequiredPlayList, declarer[0], CPlay::PP_LIKELY_WINNER, TRUE);
					playList << pPlay;
					// advance the count
					nDelayedTricksCount++;
				}
			}

			// try the same for dummy in fourth position
			if ((pDOC->GetCurrentPlayer() == m_pPlayer) &&
				(pCardLed->GetSuit() == nSuit) && (declarer.GetNumWinners() == 0) && 
				(missingTopCards.GetNumCards() == 1) && (dummy.GetNumCards() >= 2) && 
				((*(missingTopCards[0]) - *(dummy[0])) == 1) && (numTricksLeft > m_numTricksLeftToBeMade))
			{
				// see if this cash is already in the play list
				CCard* pCashCard = missingTopCards[0];
				int numExistingPlays = playList.GetSize();
				int i; // NCR-FFS added here, removed below
				for(/*int*/ i=0;i<numExistingPlays;i++)
					if (playList[i]->GetConsumedCard() == pCashCard)
						break;
				if (i == numExistingPlays)	// card is not in the play list
				{
					// create a delayed cash of the top card in dummy
					CCardList* pRequiredPlayList = NULL;
					// NCR-188 test if outstanding card > than our card (added 6/09/13)
					if(pCashCard->GetFaceValue() > dummy.GetTopCard()->GetFaceValue())
					{						
						// NCR-188 following 2 statements uncommented to recognize that dummy's card may not win
						pRequiredPlayList = new CCardList;
						pRequiredPlayList->Add(pCashCard);
					}
					CCash* pPlay = new CCash(CPlay::IN_DUMMY, CPlay::IN_HAND, pRequiredPlayList, dummy[0], CPlay::PP_LIKELY_WINNER, TRUE);
					playList << pPlay;
					// advance the count
					nDelayedTricksCount++;
				}
			}
		}
	}

	//
	if (numMaxCashes > 1)
		status << "4PLNCSH30! A total of " & numMaxCashes 
		          & " maximum cashing tricks are immediately available. "
				  & numRequiredCardsBePlayed & " require cards to have been played\n"; // NCR-449
	else if (numMaxCashes == 0)
		status << "4PLNCSH31! Only one cashing trick is immediately available.\n";

	// don't forget delayed cash(es)
	if (nDelayedTricksCount == 1)
	{
		if (numMaxCashes > 0)
			status << "4PLNCSH32! In addition, one delayed cash is also available with the " &  
					  playList.GetTail()->GetConsumedCard()->GetName() & ".\n";
		else if (numMaxCashes == 0)
			status << "4PLNCSH22! One delayed cashing trick is available with the " &
					  playList.GetTail()->GetConsumedCard()->GetName() & ".\n";
	}
	numMaxCashes += nDelayedTricksCount;

	// done
	numReqdCards = numRequiredCardsBePlayed; // NCR-449 return count
//	return nTricksCount;
	return numMaxCashes;	// return the # of actually usable cashes
}






//
// FormTrumpPullingPlan()
//
// - form a plan to pull trumps
//
int CDeclarerPlayEngine::FormTrumpPullingPlan(CPlayList& mainPlayList, CPlayList& trumpPlayList)
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	status << "4PLNTRMP! Evaluating the trumps situation...\n";

	// get the formal list of outstanding trumps
	CCardList missingTrumps;
	GetOutstandingCards(m_nTrumpSuit, missingTrumps);
	m_numOutstandingTrumps = missingTrumps.GetNumCards();

	// get the # of ruffs required
	CCombinedSuitHoldings& trumpSuit = m_pCombinedHand->GetSuit(m_nTrumpSuit);
	int numTrumpWinners = trumpSuit.GetNumWinners();
//	int numDeclarerTrumps = trumpSuit.GetNumDeclarerCards();
//	int numDummyTrumps = trumpSuit.GetNumDeclarerCards();

	// then try to distribute the ruffs between declarer and dummy
	const int numDummyRuffs = m_numPlannedDummyRuffingTricks;
	const int numDeclarerRuffs = m_numPlannedDeclarerRuffingTricks;
	const int numTotalRuffs = m_numPlannedRuffingTricks;

	// check how many ruffs are really required
	// formula is the # of tricks required less the # of top cards (sure winners)
	// but if the "top cards" include trumps, each ruff takes away a trump!
	int numRequiredRuffs = m_numTricksLeftToBeMade - m_numSureTricks;
	numRequiredRuffs = Min(numRequiredRuffs, numTotalRuffs);
	int numPreviousRequiredRuffs = numRequiredRuffs;
	int numRequiredDummyRuffs = 0, numRequiredDeclarerRuffs = 0;
	int numSureTricks = m_numSureTricks; 
	int numTricksLeftToBeMade = m_numTricksLeftToBeMade;

	// do this iteratively
	while (numRequiredRuffs > 0)
	{
		numRequiredRuffs = Min(numRequiredRuffs, numTotalRuffs);
		int nDiff = trumpSuit.GetLengthDiff();
		if (nDiff >= 0)
		{
			// declarer's trumps are longer, so use dummy ruffs first,
			numRequiredDummyRuffs = Min(numDummyRuffs, numRequiredRuffs);
			numRequiredRuffs -= numRequiredDummyRuffs;
			// then use declarer ruffs, 
			numRequiredDeclarerRuffs = Min(numDeclarerRuffs, numRequiredRuffs);
			// adjust the # of "sure" tricks to reflect shortening of the
			// long trump hand
//			int nOutsideWinners = numSureTricks - trumpSuit.GetNumDeclarerWinners();
			if (numRequiredDeclarerRuffs <= nDiff)
				numSureTricks = m_numSureTricks - numRequiredDeclarerRuffs;
		}
		else
		{
			// else dummy's trumps are longer, so use declarer ruffs first, then dummy
			numRequiredDeclarerRuffs = Min(numDeclarerRuffs, numRequiredRuffs);
			numRequiredRuffs -= numRequiredDeclarerRuffs;
			numRequiredDummyRuffs = Min(numDummyRuffs, numRequiredRuffs);
			// adjust the # of "sure" tricks
//			int nOutsideWinners = numSureTricks - trumpSuit.GetNumDummyWinners();
			if (numRequiredDummyRuffs <= Abs(nDiff))
				numSureTricks = m_numSureTricks - numRequiredDummyRuffs;
		}

		// see if the ruff requirements changed
		numRequiredRuffs = numTricksLeftToBeMade - numSureTricks;
		if (numRequiredRuffs == numPreviousRequiredRuffs)
			break;	// nope, so break out
		numPreviousRequiredRuffs = numRequiredRuffs;
	}

	// figure out how many trumps we'll have left after the ruffs
	int numDummyExcessTrumps = m_numDummyTrumps - numRequiredDummyRuffs;
	int numDeclarerExcessTrumps = m_numDeclarerTrumps - numRequiredDeclarerRuffs;

	// now determine the number of trumps rounds that may be pulled safely
	int numSafeTrumpRounds = 0;

	// if the number of sure winners in the hand is >= number of tricks required,
	// _AND_ we have enough entries, we can go ahead and cash all the trumps we want
	if (numSureTricks >= m_numTricksLeftToBeMade)
	{
		// max out the number of safe trumps
		numSafeTrumpRounds = MAX(m_numDeclarerTrumps, m_numDummyTrumps);
	}
	else
	{
		// else we need to save trumps either to ruff or as entries
		if ((numDummyRuffs > 0) && (numDeclarerRuffs > 0))
			numSafeTrumpRounds = MIN(numDummyExcessTrumps, numDeclarerExcessTrumps);
		else if (numDummyRuffs > 0)
			numSafeTrumpRounds = numDummyExcessTrumps;
		else if (numDeclarerRuffs > 0)
			numSafeTrumpRounds = numDeclarerExcessTrumps;
		else
		{
			// NCR-201 draw some trumps if declarer and dummy have same length suits
			numSafeTrumpRounds = 1;   // NCR-201 set for all suits equal
			for(int aSuit = CLUBS; aSuit <= SPADES; aSuit++)
			{
				if(aSuit == m_nTrumpSuit)
					continue;  // skip trumps
				CSuitHoldings& declarerSuit = m_pHand->GetSuit(aSuit);
				CSuitHoldings& dummySuit = m_pPartnersHand->GetSuit(aSuit);
				int nbrDummyCards = dummySuit.GetNumCards();
				int nbrDeclarerCards = declarerSuit.GetNumCards();
				if((nbrDeclarerCards == nbrDummyCards)
    				// NCR-397 also OK to play trumps if no need to ruff (ie longer suit is winners)
					|| ((nbrDeclarerCards > nbrDummyCards) 
					    && (nbrDeclarerCards == declarerSuit.GetNumWinners()))
					|| ((nbrDeclarerCards < nbrDummyCards) 
					    && (nbrDummyCards == dummySuit.GetNumWinners())) 
					// NCR-565 Can't ruff if number OS cards <= shortest suit length
					|| ((m_pCombinedHand->GetSuit(aSuit).GetNumOutstandingCards() + 1)/2)
					    <= MIN(nbrDummyCards, nbrDeclarerCards))
					continue;  // No ruffs possible if same length

				numSafeTrumpRounds = 0;  // Possible to ruff if not equal
			}
		} // NCR-201 end

		// restrict trump pulls to the trump suit winners
		numSafeTrumpRounds = MIN(numSafeTrumpRounds, numTrumpWinners);
	}

	// pull no more trumps than necessary
	int numTrumpPulls = MIN(numSafeTrumpRounds, m_numOutstandingTrumps);	

	// NCR-452 Make sure that we draw trumps even if the above says no
	if((numTrumpPulls == 0) && (trumpSuit.GetMaxLength() >= m_numOutstandingTrumps)
		&& (trumpSuit.GetNumCards() > 0) )
	{
		// Check if the short hand has NO void or singleton where there could be a ruff
		// Other considerations ??? > 2 trumps OS or if <=2 do we have the top trump
		if(trumpSuit.GetDeclarerLength() < trumpSuit.GetDummyLength())
		{
			if((m_pCombinedHand->GetPlayerHand()->GetNumVoids() == 0)
				&& ((m_pCombinedHand->GetPlayerHand()->GetNumSingletons() == 0) 
				         // Check that its the trump suit that is a singleton
				     || (trumpSuit.GetDeclarerLength() == 1)) ) 
			{
				numTrumpPulls = 1;  // Do at least one
			}
		}
		else if(trumpSuit.GetDeclarerLength() > trumpSuit.GetDummyLength())
		{
			if((m_pCombinedHand->GetPartnerHand()->GetNumVoids() == 0)
				&& ((m_pCombinedHand->GetPartnerHand()->GetNumSingletons() == 0) 
				         // Check that its the trump suit that is a singleton
				     || (trumpSuit.GetDummyLength() == 1)) ) 
			{
				numTrumpPulls = 1;  // Do at least one
			}
		}
		// NCR-488 Make another test to see if we should draw trumps
		if(numTrumpPulls < 1) 
		{
			// Sum up all the possible cashing tricks by comparing number of top cards
			// against the outstanding cards. Assume split at worst x-1 for >=4 cards out (90%)
			int topWinnersCount = 0;
			CCardList dmyDiscards;
			CCardList dclrDiscards;
			for(Suit theSuit = CLUBS; theSuit <= SPADES; GETNEXTSUIT(theSuit))
			{
				CCombinedSuitHoldings& theSuitHoldings = m_pCombinedHand->GetSuit(theSuit); 
				if(theSuitHoldings.IsVoid())
					continue; // skip 
				// Assume X-1 trump split (#OScards-1)
				if(theSuitHoldings.GetSequence2(0).GetNumCards() >= (theSuitHoldings.GetNumOutstandingCards()-1))
					topWinnersCount += theSuitHoldings.GetMaxLength();  // All of these will be winners
				else {
					// Check if this suit could be good for discards
					if(theSuitHoldings.GetTopCardVal() < theSuitHoldings.GetMissingCardVal(0)) {
						if((theSuitHoldings.GetNumDeclarerCards() > 0) 
							// NCR-598 don't discard if other hand is void and can ruff
							&& ((theSuitHoldings.GetNumDummyCards() > 0)
							 || (trumpSuit.GetNumDummyCards() == 0)) ) {
							dclrDiscards << theSuitHoldings.GetDeclarerCard(theSuitHoldings.GetNumDeclarerCards()-1);
						}
						if(theSuitHoldings.GetNumDummyCards() > 0) {
							dmyDiscards << theSuitHoldings.GetDummyCard(theSuitHoldings.GetNumDummyCards()-1);
						}
					}
				}
			} // end for(theSuit) checking each suit for winners

			if(topWinnersCount >= m_numTricksLeftToBeMade) {
				numTrumpPulls = 1; // Go draw trumps
				status << "3PLNTRMP3! We have " & topWinnersCount & " probable winners from long suits so will try "
					      & "one more trump pull.\n";
				// Merge temp discard lists into global lists
				m_dummyPriorityDiscards.Merge(dmyDiscards, true);
				m_declarerPriorityDiscards.Merge(dclrDiscards, true);
			}
		}  // end NCR-488 another test to
	} // end NCR-452 making further tests about drawing trump

	//
	if (numSafeTrumpRounds <= 0)
	{
		status << "3PLNTRMP4! We cannot safely play any trumps without endangering the contract.\n";
	}
	else
	{
		status << "3PLNTRMP5! We can safely play up to " & numSafeTrumpRounds & 
				  ((numSafeTrumpRounds > 1)? " rounds" : " round") &
				  " of trumps without endangering the contract.\n";
//				  " of trumps while maintaining ruffing ability.\n";
		if (numSafeTrumpRounds > m_numOutstandingTrumps)
		{
			if (m_numOutstandingTrumps == 1)
				status << "3PLNTRMP6! However, we only need to pull one round to collect the single outstanding trump.\n";
			else
				status << "3PLNTRMP6! However, we only need to pull a maximum of " & 
						   m_numOutstandingTrumps & 
						   " rounds to collect all the outstanding trumps.\n";
		}
		else if (numSafeTrumpRounds == m_numOutstandingTrumps)
		{
			if (m_numOutstandingTrumps == 1)
				status << "3PLNTRMP7! This will pull in the single outstanding trump.\n";
			else
				status << "3PLNTRMP7! This will pull in all " & m_numOutstandingTrumps & 
						   " outstanding trumps.\n";
		}
		else
		{
			status << "3PLNTRMP8! However, this may not pull in all " & 
					   m_numOutstandingTrumps & " outstanding trump(s).\n";
		}
	}

	//
//	int numSideCashingTricks = m_numPlannedCashingTricks - numTrumpWinners;
	int numSideCashingTricks = m_numPlannedCashingTricks - trumpSuit.GetNumMaxWinners();
	status << "3PLNTRMP9! This will result in " & 
			  ((numTrumpPulls == 1)? " " : "(up to) ") & numTrumpPulls & 
			  " round" & ((numTrumpPulls == 1)? " " : "s ") & "of trumps";
	if (numSideCashingTricks > 0)
		status < ", " & numSideCashingTricks &
				  " cashing trick" & ((numSideCashingTricks > 1)? "s " : " ") &
				  "in the side suits,";
	status < " and up to " & numTotalRuffs & " ruffing trick" &
			  ((numTotalRuffs == 1)? ".\n" : "s.\n");

	// determine min # of trumps rounds req'd
	int minReqdTrumpRounds = m_numOutstandingTrumps / 2;
	if ((m_numOutstandingTrumps % 2) != 0)
		minReqdTrumpRounds++;

	// NCR-472 & NCR-421 Created order list of trumps to use below
	CCardList ourTrumps;
	ourTrumps.EnableAutoSort(FALSE);  // Don't sort

	if(numTrumpPulls > 0) 
	{
		CPlayer *pCurrPlayer = pDOC->GetCurrentPlayer();
		Position nOurPosition = pCurrPlayer->GetPosition();
		CCardList& topSequence = trumpSuit.GetSequence2(0);
		// Do this in two passes. First for top cards in our position
		int ix; // NCR-FFS added here, removed below
		for(/*int*/ ix = 0; ix < trumpSuit.GetNumCards(); ix++) 
		{
			CCard* pCard = trumpSuit.GetAt(ix);
			if((pCard->GetOwner() == nOurPosition) && (topSequence.HasCard(pCard)) ){
				if(!ourTrumps.HasCard(pCard))   // NCR-569
					ourTrumps.Add(pCard);  // add to list
			} else if(pCard->GetOwner() != nOurPosition) {  
				// NCR-569 check if the suit could be blocked: 
				// Other hand has Ax and we have KQJxx
				// Need to lead low to A
				int numTrumpsInHand = m_pHand->GetSuit(m_nTrumpSuit).GetNumCards(); // NCR-619
				if((numTrumpsInHand == 2)
					||(numTrumpsInHand == 1)) // NCR-619 ?? also test if singleton is higher 
				{
					pCard = m_pPartnersHand->GetSuit(m_nTrumpSuit).GetBottomCard(); 
					if(pCard && !ourTrumps.HasCard(pCard))
						ourTrumps.Add(pCard);  // add to list
				}
			}
		}  // end for(ix) thru trumps in our hand
		// Now get rest of them
		for(ix = 0; ix < trumpSuit.GetNumCards(); ix++) 
		{
			CCard* pCard = trumpSuit.GetAt(ix);
			if(pCard->GetOwner() != nOurPosition && !ourTrumps.HasCard(pCard))  // NCR-569 test HasCard()
				ourTrumps.Add(pCard);  // add to list
		}  // end for(ix) thru trumps in other hand

		// Adjust count of pulls
		numTrumpPulls = Min(numTrumpPulls, ourTrumps.GetNumCards());
	}// NCR-472 end creating list of trumps in order to be played

	// note that our top trump winners may not be instant winners; i.e., they may
	// require that previous cashes / force plays remove enemy top cards

	// and create the trump pull play
	for(int i=0;i<numTrumpPulls;i++)
	{
		// mark the card to pull trumps with 
		CCard* pCard = ourTrumps.GetAt(i);
		int numHigherTrumps = missingTrumps.GetNumCardsAbove(pCard);
		// NCR skip the required play list to FORCE drawing trump
		// if we have the KQJ or more trumps than them
		bool bSkipRequiredPlayList = false;
		if(((i == 0) && (trumpSuit.GetNumCardsAbove(TEN) == 3))    // NCR-40 >= 2 vs 3 ???
		    || ((i < 3) && ((trumpSuit.GetNumCards() - ((missingTrumps.GetNumCards()+1)/2)) >= 2)))
		{
			bSkipRequiredPlayList = true;  // NCR leave off RequirePlay list
		}

		// get the list of higher outstanding enemy cards 
		// that must have been played already
		CCardList* pRequiredPlayedList = NULL;
		if (numHigherTrumps > 0 && !bSkipRequiredPlayList) // NCR
		{
			pRequiredPlayedList = new CCardList;
			for(int k=0;k<numHigherTrumps ;k++)
				*pRequiredPlayedList << missingTrumps[k];
		}
		// NCR-702 Set target hand to the one with the highest card
		CCard* pPlayCard = trumpSuit.GetTopCard();
		int nTargetHand = m_pHand->GetSuit(pCard->GetSuit()).HasCard(pPlayCard) ? CPlay::IN_HAND : CPlay::IN_DUMMY;  //NCR-702
//		ASSERT(nTargetHand == GetCardOwner(pCard)); // DEBUG to catch when

		// add the list of trumps that must be outstanding for a 
		// trump pull to be used
		CCardList* pNewMissingTrumps = new CCardList(missingTrumps);
		CTrumpPull* pPlay = new CTrumpPull(nTargetHand, //GetCardOwner(pCard), 
										   pCard, 
										   pRequiredPlayedList,
										   pNewMissingTrumps,
										   (i < numTrumpWinners)? TRUE : FALSE,
										   (i >= minReqdTrumpRounds)? TRUE : FALSE);
		trumpPlayList << pPlay;
	} // end for(i) thru numTrumpPulls
	//
	return numTrumpPulls;
}





//
// FormTrumpFinessePlan()
//
// - form a plan to finesse trumps from the opponents' hand
//
int CDeclarerPlayEngine::FormTrumpFinessePlan(CPlayList& mainPlayList, CPlayList& trumpPlayList,
											  bool bSafeOnly)
{
	int nTrumpSuit = pDOC->GetTrumpSuit();
	if (!ISSUIT(nTrumpSuit))
		return 0;

	CPlayerStatusDialog& status = *m_pStatusDlg;  // NCR added output for debug

	// call the standard finesse routine
	CCombinedSuitHoldings& suit = m_pCombinedHand->GetSuit(nTrumpSuit);
	int numTrumpFinesses = FindFinessesInSuit(suit, trumpPlayList);

	// filter out inappropriate finesses
	// leave in only type 1 and 2 finesses
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<numTrumpFinesses;i++)
	{
		CFinesse* pPlay = (CFinesse*) trumpPlayList[i];
		if ((pPlay->GetSubType() != CFinesse::TYPE_I) && (pPlay->GetSubType() != CFinesse::TYPE_II))
		{
			status << "5PLFTF1! Removed plan <" & pPlay->GetName()  // NCR for debug
				         & ">, only types 1 & 2 for trump.\n";
			trumpPlayList.RemovePlay(i);
			i--;
			numTrumpFinesses--;
		}
	}

	if(bSafeOnly)  // NCR-425 Remove "unsafe" finesses if following hand is not out
	{
		// do a second layer of filtering -- allow a finesse only if the
		// target opponent has shown out of the suit
		for(i=0;i<numTrumpFinesses;i++)
		{
			CFinesse* pPlay = (CFinesse*) trumpPlayList[i];
			int nTargetPos = pPlay->GetTargetPos();

			// suit needs to be shown out
			CGuessedSuitHoldings& suit = m_ppGuessedHands[nTargetPos]->GetSuit(nTrumpSuit);
			if (!suit.IsSuitShownOut())
			{
				status << "5PLFTF3! Removed plan <" & pPlay->GetName() 
						  & "> following hand not shown out.\n";    // NCR for debug
				trumpPlayList.RemovePlay(i);
				i--;
				numTrumpFinesses--;
			}
		} // end for(i) thru finesses
	} // end NCR-425 remove if SafeOnly desired

	// done
	return numTrumpFinesses;
}







//-----------------------------------------------------------------------
//
// internal structure
//
struct RuffInfo {
	int numRuffs;
	int numDiscardsRequired;
	int nSuit;
	int nDiscardSuit;
	int nDirection;
	CCardList* pPrerequisites;
};

//
// FindRuffingPlays()
//
// - look for potential ruffs & prioritize them
//
int CDeclarerPlayEngine::FindRuffingPlays(CPlayList& playList)
{
	RuffInfo ruffCandidates[8];
	int numRuffCandidates = 0;
	int numTotalPossibleRuffs = 0;
	CPlayerStatusDialog& status = *m_pStatusDlg;
	m_numDeclarerTrumps = m_pHand->GetNumCardsInSuit(m_nTrumpSuit);
	m_numDummyTrumps = m_pPartnersHand->GetNumCardsInSuit(m_nTrumpSuit);

	//
	status << "4PLNRF0! Looking for ruffing tricks...\n";

	// first look to see if the current opponent's lead card can be ruffed
	// this check is ONLY done if we're planning in second hand, so this is OK
	CCard* pCardLed = pDOC->GetCurrentTrickCardByOrder(0);
	if (pCardLed != NULL)
	{
		int nSuitLed = pCardLed->GetSuit();
		if (nSuitLed != m_nTrumpSuit)
		{
			// check both hands
			CSuitHoldings& declarerSuit = m_pHand->GetSuit(nSuitLed);
			CSuitHoldings& dummySuit = m_pPartnersHand->GetSuit(nSuitLed);
			// check if we can ruff in dummy
			if ((dummySuit.GetNumCards() == 0) && (m_numDummyTrumps > 0)
				// NCR-468 Don't ruff if dummy has winning card
				&& (declarerSuit.GetNumLosers() > 0))
			{
				ruffCandidates[numRuffCandidates].numRuffs = 1;
				ruffCandidates[numRuffCandidates].numDiscardsRequired = 0;
				ruffCandidates[numRuffCandidates].nSuit = nSuitLed;
				ruffCandidates[numRuffCandidates].nDiscardSuit = nSuitLed;
				ruffCandidates[numRuffCandidates].nDirection = CRuff::IN_DUMMY;
				numRuffCandidates++;
				numTotalPossibleRuffs++;
			}
			// check if we can ruff in hand
			if ((declarerSuit.GetNumCards() == 0) && (m_numDeclarerTrumps > 0)
				// NCR-468 Don't ruff if dummy has winning card
				&& (dummySuit.GetNumLosers() > 0))
			{
				ruffCandidates[numRuffCandidates].numRuffs = 1;
				ruffCandidates[numRuffCandidates].numDiscardsRequired = 0;
				ruffCandidates[numRuffCandidates].nSuit = nSuitLed;
				ruffCandidates[numRuffCandidates].nDiscardSuit = nSuitLed;
				ruffCandidates[numRuffCandidates].nDirection = CRuff::IN_HAND;
				numRuffCandidates++;
				numTotalPossibleRuffs++;
			}
		}
	}

	// now look for any potential ruffs fron the combined hand
	int i,j,nSuit = CLUBS;   // NCR CLUBS vs 0
 	for(i=0;i<4;i++)
	{
		if (nSuit == m_nTrumpSuit)
		{
			nSuit = GetPrevSuit(nSuit);
			continue;
		}

		//
		CCombinedSuitHoldings& suit = m_pCombinedHand->GetSuit(nSuit);
		int nLengthDiff = suit.GetLengthDiff();
		int numLosers = suit.GetNumLosers();
		if (numLosers > 0) 
		{
			// the ruff-able cards in a suit are the excess losers
			// in one hand relative to the other
			int numRuffsFromHand = 0, numRuffsFromDummy = 0;
			int numPossibleRuffs = Min(Abs(nLengthDiff), suit.GetNumLosers());
			// NCR-472 Num possible ruffs should consider number of outstanding cards vs
			// the number in our hand. Our cards could be winners after all the cards in
			// the short hand are played. IE there may not be any ruffs needed.
			// The suit can have losers without there being a chance of ruffing
			// Assume some split of outstanding cards (at least one in other hand)
			// See also NCR-469
			if(suit.GetMinLength() != 0) {  // NCR-701  Ignore if one side void
				numPossibleRuffs = Max(0, Min(numPossibleRuffs, 
				                   ((suit.GetNumOutstandingCards() - 1) - suit.GetMinLength()))); // NCR-472
			}

			if (numPossibleRuffs > 0)
			{
				// also limit the max # of ruffs in a suit to the # of trumps in other hand
				if (nLengthDiff > 0)
					numPossibleRuffs = Min(numPossibleRuffs, m_numDummyTrumps);
				else
					numPossibleRuffs = Min(numPossibleRuffs, m_numDeclarerTrumps);

				//
				ruffCandidates[numRuffCandidates].numRuffs = numPossibleRuffs;
				ruffCandidates[numRuffCandidates].nSuit = nSuit;
				ruffCandidates[numRuffCandidates].nDiscardSuit = nSuit;
				if (nLengthDiff > 0)
				{
					// hand is longer than dummy -- record ruff location
					ruffCandidates[numRuffCandidates].nDirection = CRuff::IN_DUMMY;

					// record # of sluffs needed in the suit to perform the ruff
					int numDiscardsRequired = suit.GetDummyLength() - suit.GetNumSureWinners();  // NCR-701 use Sure winners
					if (numDiscardsRequired < 0)
						numDiscardsRequired = 0;
					ruffCandidates[numRuffCandidates].numDiscardsRequired = numDiscardsRequired;
					numRuffsFromHand = numPossibleRuffs; // ruffing a _declarer_ cards
				}
				else
				{
					// dummy is longer than hand
					ruffCandidates[numRuffCandidates].nDirection = CRuff::IN_HAND;
					int numDiscardsRequired = suit.GetDeclarerLength() - suit.GetNumSureWinners(); // NCR-701 use Sure winners
					if (numDiscardsRequired < 0)
						numDiscardsRequired = 0;
					ruffCandidates[numRuffCandidates].numDiscardsRequired = numDiscardsRequired;
					numRuffsFromDummy = numPossibleRuffs;	// ruffing _dummy_ cards
				}
				//
				numRuffCandidates++;
				numTotalPossibleRuffs += numPossibleRuffs;
			}

			// additional ruffs can be developed by discarding losers in 
			// one hand on winners in the other hand to set up a ruff
			// there has to be at least one card left in a hand after the ruffs above,
			// and each hand has to have at least one loser
			int numLeft = suit.GetMaxLength() - numPossibleRuffs;
			int numDummyLosers = suit.GetNumDummyLosers() - numRuffsFromDummy;
			int numDeclarerLosers = suit.GetNumDeclarerLosers() - numRuffsFromHand;
			if ((numLeft > 0) && (numDummyLosers > 0) && (numDeclarerLosers > 0)
				// NCR-472 and -469 Long holdings will not need ruffing
				&& ((suit.GetNumOutstandingCards() - 1) > suit.GetMinLength()) )
			{
				// see which way we want to ruff
				BOOL bRuffFromHand = TRUE;	// meaning ruff a card _from_ hand
				if ((nLengthDiff < 0) || (m_pCombinedHand->GetNumDummyTrumps() > m_pCombinedHand->GetNumDeclarerTrumps()))
					bRuffFromHand = FALSE;

				// so look for "excess" winners
				for(int j=CLUBS;j<=SPADES;j++)
				{
					if (j == m_nTrumpSuit)
						continue;

					// info about helper suit
					CCombinedSuitHoldings& testSuit = m_pCombinedHand->GetSuit(j);
					int nOutsideLengthDiff = testSuit.GetLengthDiff();
					int nOutsideWinnersDiff = testSuit.GetWinnersDiff();
//					int nOutsideDummyLength = testSuit.GetNumDummyCards();
//					int nOutsideDeclarerLength = testSuit.GetNumDeclarerCards();

					// see if we can create a shortage in dummy
					if ((nOutsideLengthDiff > 0) && (nOutsideWinnersDiff > 0) && 
						(numDeclarerLosers > 0) && (numDummyLosers > 0) && bRuffFromHand)
					{
						// can discard up to nLengthDiff cards in dummy outside the suit
						ruffCandidates[numRuffCandidates].nSuit = nSuit;
						ruffCandidates[numRuffCandidates].nDiscardSuit = nSuit;
						int numMaxRuffs = Min(numDeclarerLosers, m_numDummyTrumps);
						ruffCandidates[numRuffCandidates].numRuffs = numMaxRuffs;
						ruffCandidates[numRuffCandidates].nDirection = CRuff::IN_DUMMY;
						ruffCandidates[numRuffCandidates].numDiscardsRequired = numDummyLosers;
						// the play will remove a card from both hand & dummy
						numDummyLosers -= ruffCandidates[numRuffCandidates].numRuffs;
						numDeclarerLosers -= ruffCandidates[numRuffCandidates].numRuffs;
						numRuffCandidates++;
						numTotalPossibleRuffs += numMaxRuffs;
					}
					else if ((nOutsideLengthDiff < 0) && (nOutsideWinnersDiff < 0) && 
							 (numDeclarerLosers > 0) && (numDummyLosers > 0) && !bRuffFromHand)
					{
						// here we can create a shortage in hand
						ruffCandidates[numRuffCandidates].nSuit = nSuit;
						ruffCandidates[numRuffCandidates].nDiscardSuit = nSuit;
						int numMaxRuffs = Min(numDummyLosers, m_numDeclarerTrumps);
						ruffCandidates[numRuffCandidates].numRuffs = Min(numDummyLosers, m_numDeclarerTrumps);
						ruffCandidates[numRuffCandidates].nDirection = CRuff::IN_HAND;
						ruffCandidates[numRuffCandidates].numDiscardsRequired = numDeclarerLosers;
						//
						numDeclarerLosers -= ruffCandidates[numRuffCandidates].numRuffs;
						numDummyLosers -= ruffCandidates[numRuffCandidates].numRuffs;
						numRuffCandidates++;
						numTotalPossibleRuffs += numMaxRuffs;
					}
				}
			}
		}

		// and move on to the next suit
		nSuit = GetPrevSuit(nSuit);
	} // end for(i) thru suits

	// first remove redundant ruff candidates
	for(i=0;i<numRuffCandidates;i++)
	{
		RuffInfo currRuffCandidate = ruffCandidates[i];
		for(int j=i+1;j<numRuffCandidates;j++)
		{
			if ((currRuffCandidate.nSuit == ruffCandidates[j].nSuit) &&
				(currRuffCandidate.nDirection == ruffCandidates[j].nDirection))
			{
				// remove the second ruff
				for(int k=j;k<numRuffCandidates-1;k++)
					ruffCandidates[k] = ruffCandidates[k+1];
				i--;
				numRuffCandidates--;
				break;
			}
		}
	}

	
	//
	// now sort and prioritize the list of potential ruffs
	//
	// first sort the suit ruffs by the # of sluffs required (fewer is better)
	// bubble sort is okay since we have 3 elements at most
	//
	RuffInfo tempRuff;
	for(i=0;i<numRuffCandidates;i++)
	{
		for(j=0;j<numRuffCandidates-1;j++)
		{
			// swap if two adjoining ruffs have different sluff rqmts
			if (ruffCandidates[j].numDiscardsRequired > ruffCandidates[j+1].numDiscardsRequired)
			{
				tempRuff = ruffCandidates[j];
				ruffCandidates[j] = ruffCandidates[j+1];
				ruffCandidates[j+1] = tempRuff;
			}
		}
	}

	//
	// now insert as many ruffs into the given play list as we can perform 
	// given our trump holdings
	//
	// but try to avoid ruffing using trump winners if possible
	// (actually, this is not true -- we may want to use ruffs for crossing)
	//
//	CCombinedSuitHoldings& trumpSuit = m_pCombinedHand->GetSuit(m_nTrumpSuit);
//	int numDummyTrumpsLeft = m_numDummyTrumps - trumpSuit.GetNumDummyWinners();
	int numDummyTrumpsLeft = m_numDummyTrumps;
	if (numDummyTrumpsLeft < 0)
		numDummyTrumpsLeft = 0;
//	int numDeclarerTrumpsLeft = m_numDeclarerTrumps - trumpSuit.GetNumDeclarerWinners();
	int numDeclarerTrumpsLeft = m_numDeclarerTrumps;
	if (numDeclarerTrumpsLeft < 0)
		numDeclarerTrumpsLeft = 0;
	int nMaxRuffs = 0;
	int numDummyRuffs = 0;
	int numDeclarerRuffs = 0;
	int numTotalRuffs = 0;
	int numTotalSluffs = 0;

	// is the ruff guaranteed?
	int numOutstandingTrumps = ISSUIT(m_nTrumpSuit)? GetNumOutstandingCards(m_nTrumpSuit) : 0;
	CPlay::PlayProspect nPlayProspect = (numOutstandingTrumps == 0)? CPlay::PP_GUARANTEED_WINNER : CPlay::PP_LIKELY_WINNER;
	//
	for(i=0;i<numRuffCandidates;i++)
	{
		if (ruffCandidates[i].nDirection == CRuff::IN_DUMMY)
		{
			// ruffing in dummy
			// adjust available ruff count for trumps
			nMaxRuffs = MIN(ruffCandidates[i].numRuffs, numDummyTrumpsLeft);
			ruffCandidates[i].numRuffs = nMaxRuffs;
			numDummyTrumpsLeft -= nMaxRuffs;
			numDummyRuffs += nMaxRuffs;
			m_numPlannedDummyRuffsInSuit[ruffCandidates[i].nSuit]++;
			m_numPlannedTotalRuffsInSuit[ruffCandidates[i].nSuit]++;

			// record the cards we need to get rid of in dummy for the ruff
			CSuitHoldings& suit = m_pPartnersHand->GetSuit(ruffCandidates[i].nDiscardSuit);
			for(int j=0;j<ruffCandidates[i].numDiscardsRequired;j++)
			{
				// pick discards starting from the bottom
				int nIndex = suit.GetNumCards() - 1 - j;
				CCard* pCard = suit[nIndex];
				if (!m_dummyPriorityDiscards.HasCard(pCard) &&
					!m_dummyDiscardedPriorityDiscards.HasCard(pCard))	// why is this necessary?????
					m_dummyPriorityDiscards << pCard;
			}
		}
		else
		{
			// ruffing in hand
			// adjust available ruff count for trumps
			nMaxRuffs = MIN(ruffCandidates[i].numRuffs, numDeclarerTrumpsLeft);
			ruffCandidates[i].numRuffs = nMaxRuffs;
			numDeclarerTrumpsLeft -= nMaxRuffs;
			numDeclarerRuffs += nMaxRuffs;
			m_numPlannedDummyRuffsInSuit[ruffCandidates[i].nSuit]++;
			m_numPlannedTotalRuffsInSuit[ruffCandidates[i].nSuit]++;

			// record the cards we need to get rid of in hand for the ruff
			CSuitHoldings& suit = m_pHand->GetSuit(ruffCandidates[i].nDiscardSuit);
			for(int j=0;j<ruffCandidates[i].numDiscardsRequired;j++)
			{
				int nIndex = suit.GetNumCards() - 1 - j;
				CCard* pCard = suit[nIndex];
				if (!m_declarerPriorityDiscards.HasCard(pCard) &&
					!m_declarerDiscardedPriorityDiscards.HasCard(pCard))	// why is this necessary?????
					m_declarerPriorityDiscards << pCard;
			}
		}
		// insert the ruffs in this suit into the provided play list
		for(j=0;j<nMaxRuffs;j++)
		{
			CRuff* pRuff = new CRuff(ruffCandidates[i].nDirection, ruffCandidates[i].numDiscardsRequired, ruffCandidates[i].nSuit, nPlayProspect);
			playList << pRuff;
		}
		//
		status << "4PLNRF1! We can potentially ruff up to " & ruffCandidates[i].numRuffs & " " & 
			((ruffCandidates[i].numRuffs > 1)? SuitToString(ruffCandidates[i].nSuit) : SuitToSingularString(ruffCandidates[i].nSuit));
		status < ((ruffCandidates[i].nDirection == CRuff::IN_DUMMY)? " in dummy" : " in hand");
		if (ruffCandidates[i].numDiscardsRequired > 0)
			status < " after discarding the " & ruffCandidates[i].numDiscardsRequired & 
					  ((ruffCandidates[i].numDiscardsRequired > 1)? " losers" : " loser") &
					  " from " & ((ruffCandidates[i].nDirection == CRuff::IN_DUMMY)? "dummy" : "hand") & ".\n";
		else
			status < ".\n";
		numTotalRuffs += nMaxRuffs;
		numTotalSluffs += ruffCandidates[i].numDiscardsRequired;
	}   // end for(i) thru ruff candidates

	//
	// All done
	//
	m_numPlannedDeclarerRuffingTricks = numDeclarerRuffs;
	m_numPlannedDummyRuffingTricks = numDummyRuffs;

	if (numTotalRuffs == 0)
	{
		status << "5PLNRF4! No immediate ruffing tricks are available.\n";
	}
	else if (numTotalRuffs != nMaxRuffs)
	{
		status << "4PLNRF5! Up to " & numTotalRuffs & " ruffs can be used (" &
				  numDummyRuffs & " in dummy/" & numDeclarerRuffs & " in hand) out of a total of " &
				  numTotalPossibleRuffs & " available";
		if (numTotalSluffs == 0)
		{
			status < ".\n";
		}
		else
		{
			status < ", but require " & numTotalSluffs & " discard(s) first.\n";
			status << "5PLNRF8! This consists of the { ";
			if (m_dummyPriorityDiscards.GetNumCards() > 0)
				status < m_dummyPriorityDiscards.GetHoldingsString() & "} from dummy";
			//
			if (m_declarerPriorityDiscards.GetNumCards() > 0)
			{
				if (m_dummyPriorityDiscards.GetNumCards() > 0)
					status < " and the { ";
				status < m_declarerPriorityDiscards.GetHoldingsString() & "} from hand.\n";
			}
			else
			{
				status < ".\n";
			}
		}
	}
	return numTotalRuffs;
}






// NCR_NOTE Define struct to hold data about possible suit(s) to develope
typedef struct {
	int nSuit, numForces, numWinners, numResiduals, nLongHand;
} DevOpportunity;

//
//-----------------------------------------------------------------------
//
//  ()
//
// - try to develop a suit
//
int CDeclarerPlayEngine::FindSuitDevelopmentPlays(CPlayList& forcePlayList, CPlayList& secondaryCashList)
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	int numTotalForces = 0;
	int numSecondaryCashes = 0;
	DevOpportunity opportunities[4];
	int numDevOpportunities = 0;
	int possiblePromoteSuit = NONE;  // NCR-706  Hold possible suit with promotion possibilities

	//
	status << "4PLNDEV0! Looking for ways to force out enemy top cards and promote our own......\n";

	// examine each suit for possible forces
	int nSuit = SPADES;
	for(int z=0;z<4;z++)
	{
		nSuit = GetNextSuit(nSuit);

		// skip trumps
		if (nSuit == m_nTrumpSuit)
			continue;

		// see if this suit is appropriate for a forcing play
		CCombinedSuitHoldings& suit = m_pCombinedHand->GetSuit(nSuit);
		if (suit.GetNumCards() == 0)
			continue;

		//
		// get some stats -- the "extra" cards are cards we can use for forcing, 
		// while still having cards left over to cash
		//
//		int numCards = suit.GetNumCards();
		const int numWinners = suit.GetNumTopCards();
		const int nLongHand = Max(suit.GetDeclarerLength(), suit.GetDummyLength());

		// "nExtra" is the # of cards left in the long hand after cashing winners
		int nExtra = nLongHand - numWinners;

		// for a force, we need to be able to drive out the opponents' cards
		// we need to have more "extra" cards than outstanding high cards
		// in order to drive out all the opponents' winners
		CCardList outstandingCards;
		int numOutstandingCards = GetOutstandingCards(nSuit, outstandingCards);
		const int numOutstandingCardsOrig = numOutstandingCards; // NCR-707 save for below

		// first adjust the oppoenents' cards for the cashes we can perform here
		for(int i=0;i<numWinners;i++)
		{
			// assume the opponents will discard the lowest cards possible
			if (numOutstandingCards > 0)
			{
				outstandingCards.Remove(outstandingCards.GetBottomCard());
				numOutstandingCards--;
			}
			else
			{
				// no more enemy winners! yay!
				break;
			}
		}

		//
		if (numOutstandingCards == 0)
			continue;

		// a simpler and more valid calculation of the opponents' winners
		// count is the cards we expect to lose
		int numOpponentWinners = suit.GetMaxLength() - suit.GetNumMaxWinners();
		if (numOpponentWinners > numOutstandingCards)
			numOpponentWinners = numOutstandingCards;

		// see if we can force out all the opponents' high cards
		int numForces = 0;
		if (nExtra > numOpponentWinners)
			numForces = numOpponentWinners;
		else
			numForces = 0;

		// then what will we have left?
		// NCR-463 use long hand cards vs combined hand cards
		int numResiduals = /*suit.GetNumCards()*/ nLongHand - numWinners - numForces;

		// NCR-705 Check that neither opponent has shown out
		CGuessedSuitHoldings& LHO_Suit = m_ppGuessedHands[m_pRHOpponent->GetPosition()]->GetSuit(nSuit);
		CGuessedSuitHoldings& RHO_Suit = m_ppGuessedHands[m_pLHOpponent->GetPosition()]->GetSuit(nSuit);
		bool bOpponentShownOut = LHO_Suit.IsSuitShownOut() || RHO_Suit.IsSuitShownOut();
//		if(bOpponentShownOut) {
//			int x = 1;    // DEBUG Break point for testing
//		}

		// but the cards left have to be able to win!
		int nFirstRemainingCardPos = numWinners + numForces;
		if ((numForces > 0) && 
				(numOutstandingCards > numForces) &&
				(*suit[nFirstRemainingCardPos] < *outstandingCards[numForces])
				// NCR-705  Ok to try if we could have 3-4 extra cards
				//  And Not if one opponent is void
				&& (((nLongHand - (numOutstandingCardsOrig+1)/2) < 3) && !bOpponentShownOut) )
			numForces = 0;	// forget it

		// see if we have a singleton winner in either hand, in which case 
		// we shouldn't attempt a force (at least not right away)
		BOOL bSingletonWinner = FALSE;
		CSuitHoldings& declarerSuit = suit.GetDeclarerSuit();
		CSuitHoldings& dummySuit = suit.GetDummySuit();
		if (suit.GetNumTopCards() > 0)
		{
			CCardList& topSeq = suit.GetTopSequence();
			if ( ((declarerSuit.GetNumCards() == 1) && (topSeq.HasCard(declarerSuit[0]))) ||
			     ((dummySuit.GetNumCards() == 1) && (topSeq.HasCard(dummySuit[0]))) )
				bSingletonWinner = TRUE;
		}

		// skip singleton winner suits for now
		if (bSingletonWinner)
			continue;

		// see if we have a singleton in one hand which is equivalent to the top
		// card in the other hand

		// store this
		if (numForces > 0)
		{
			opportunities[numDevOpportunities].nSuit = nSuit;
			opportunities[numDevOpportunities].numForces = numForces;
			opportunities[numDevOpportunities].numWinners = numWinners;
			opportunities[numDevOpportunities].numResiduals = numResiduals;
			opportunities[numDevOpportunities].nLongHand = nLongHand;
			numDevOpportunities++;
		}

		//  NCR-707 If no forces, there may be cashes needed to promote a long suit
		else if((numWinners > 0) && ((nLongHand - (numOutstandingCardsOrig+1)/2) > 1)   // 2 or 3 extra cards ???
			    && (m_nTrumpSuit == NOTRUMP) && ! bOpponentShownOut
			    && (suit.GetNumMaxWinners() < suit.GetMaxLength())) 
		{
			if(possiblePromoteSuit != NONE)
				int x = 0;  // NCR DEBUG break point
			possiblePromoteSuit = nSuit; // Save

			bool bFoundCash = false;
			// Search playlist for Cashes of this suit to mark as desired
			for(int i=0; i < m_playPlan.GetSize(); i++) {
				CPlay* pPlay = m_playPlan[i];
				if(pPlay->GetPlayType() == CPlay::CASH) {
					int pSuit = pPlay->GetConsumedCard()->GetSuit();
					if(pSuit == nSuit) {  // is this the right suit
						((CCash*)pPlay)->SetProperites(CCash::FOR_PROMOTION);
						bFoundCash = true;   // let em know
						break; // one is enough ???
					}
				}
			} // end for(i) through play plans
			if(!bFoundCash) {  // If we didn't find one, add one
				CCard* topCard = suit.GetTopCard();
				int owner = GetCardOwner(topCard);
				CCash* pCash = new CCash(owner, CPlay::IN_EITHER, NULL, topCard, CPlay::PP_GUARANTEED_WINNER);
				pCash->SetProperites(CCash::FOR_PROMOTION);
 				m_playPlan << pCash;   //<<<< Note: Add directly to play plan list vs conditional for this method
			    status << "5PLNDEV28! One cash play has been found, which will prepare to promote "
							& STSS(nSuit) &  "s.\n";
			}
		}  // NCR-707 end

	}  // end for(z) thru suits

	//
	// now sort the suit development opportunities
	//
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<numDevOpportunities;i++)
	{
		for(int j=0;j<numDevOpportunities-1;j++)
		{
			// one suit is preferable to develop if it has fewer forces & same or more residual cards,
			// OR if it has the same forces and more residual cards
			if ( ((opportunities[j+1].numForces < opportunities[j].numForces) && 
				  (opportunities[j+1].numResiduals >= opportunities[j].numResiduals)) ||
				 ((opportunities[j+1].numForces == opportunities[j].numForces) && 
				  (opportunities[j+1].numResiduals > opportunities[j].numResiduals)) )
			{
				DevOpportunity opp = opportunities[j];
				opportunities[j] = opportunities[j+1];
				opportunities[j+1] = opp;
			}
		}
	}


	//
	// now create the plays
	//

	// new -- for now, try to develop only one suit at a time
	if (numDevOpportunities > 1)
		numDevOpportunities = 1;

	for(i=0;i<numDevOpportunities ;i++)
	{
		// pull out data
		nSuit = opportunities[i].nSuit;
		const int numForces = opportunities[i].numForces;
		const int numWinners = opportunities[i].numWinners;
		const int nLongHand = opportunities[i].nLongHand;
		const int numResiduals = opportunities[i].numResiduals; // NCR-498
		CCombinedSuitHoldings& suit = m_pCombinedHand->GetSuit(nSuit);

		// see if the secondary cash plays are guaranteed
		int numOutstandingTrumps = ISSUIT(m_nTrumpSuit)? GetNumOutstandingCards(m_nTrumpSuit) : 0;
		CPlay::PlayProspect nPlayProspect = (numOutstandingTrumps == 0)? CPlay::PP_GUARANTEED_WINNER : CPlay::PP_LIKELY_WINNER;

		CCardList outstandingCards;   // NCR-706 Moved outside
		const int numOutstandingCards = GetOutstandingCards(nSuit, outstandingCards); 

		//
		// NCR-498 Require there be more promotions that needed forces(losses)
		int numPossibleWinners = nLongHand - (numOutstandingCards+1)/2; // NCR-707 assume even split
		// NCR-707 Don't Force if number of losers > max allowed to make contract
		int numTricksLeft = 13 - pDOC->GetNumTricksPlayed();
		if((numTricksLeft - m_numTricksLeftToBeMade) < numForces)
			numPossibleWinners = 0; // NCR-707 too late if we don't make contract

		if ((numForces > 0) && ((numResiduals > numForces)
			                      // NCR-707 Simpler test: will our long suit give us tricks?
			                     || (numPossibleWinners > 1)) )
		{		
//			CCardList outstandingCards;   
//			int numOutstandingCards = GetOutstandingCards(nSuit, outstandingCards); 

			// NCR only do Force if our second card has a chance to win soon
			// NCR-290 also don't force if we can pull
			// NCR-428 Allow forces when 2 or more tricks will be created
			int maxNumHigherCards = (suit.GetMaxLength() >= 4) ? 2 : 1; // NCR-428
			if (((suit.GetNumCards() > 1) 
				 && (outstandingCards.GetNumCardsAbove(suit.GetSecondHighestCard()) > maxNumHigherCards)  // NCR-428
				 && (numPossibleWinners < 2) )  // NCR-707 try for at least 2 winners
				|| ((suit.GetSequence2(0).GetNumCards() >= (numOutstandingCards+1)/2)   // NCR-290 test against what's out
                    && (numWinners > 0)) ) // NCR-317 No force if we have winners
				continue;  // NCR skip Force if we don't win soon or don't need it

			// NCR-382 Don't try force if current top card > our top card
			int nTopPos;
			CCard* pCurrTopCard = pDOC->GetCurrentTrickHighCard(&nTopPos);
			if (pCurrTopCard && (pCurrTopCard->GetFaceValue() > suit.GetTopCard()->GetFaceValue()))
				continue;  // NCR-382 no force with small card

 			// start with the first non-winner card
			int nIndex = numWinners;

			// and create
			CCard* pLastCard = suit.GetHighestCardBelow(outstandingCards[0]);
			// play the next highest card from our hand
			// NCR-449 Get ForceCard here vs inside loop (changed name)
			// NCR-498 Why not use the top card??? remove getting second below
			CCard* pForceCard = pLastCard; // NCR-498 suit.GetHighestCardBelow(pLastCard);
			// NCR-449 Check if either side is a singleton
			// One case: KJT98 vs Q and want to continue leading???
			// NCR-473 Also check for doubleton because of blocking problem
			// One case: T765 vs J8 with 9xxx out. J and T on same trick leaves 9
			if((suit.GetNumDeclarerCards() == 1) 
				|| ((suit.GetNumDeclarerCards() == 2) && (suit.GetSequence2(0).GetNumCards() <= 2))) // NCR-473 
			{
				CCard* dclrCard = suit.GetDeclarerCard(0);  // get card
				// Check if the singleton is in the top bunch
				if(suit.GetSequence2(0).HasCard(dclrCard)) {
					pForceCard = dclrCard; // use the singleton
				}
			}
			else if((suit.GetNumDummyCards() == 1) 
				     || ((suit.GetNumDummyCards() == 2) && (suit.GetSequence2(0).GetNumCards() <= 2))) // NCR-473
			{
				CCard* dmyCard = suit.GetDummyCard(0);  // get card
				// Check if the singleton is in the top bunch
				if(suit.GetSequence2(0).HasCard(dmyCard)
					// NCR-753 can't force with the high card
					&& (dmyCard->GetFaceValue() < outstandingCards.GetTopCard()->GetFaceValue())
					) {
					pForceCard = dmyCard; // use the singleton
				}
			}
			else if(numOutstandingCards == 1)
			{   // NCR-463 use top card if only one to get correct promoted cards
				pForceCard = pLastCard;
			}
			ASSERT(pForceCard != NULL);

			// NCR-763 can't force if a high card on trick already
			if(pCurrTopCard && (pCurrTopCard->GetFaceValue() > pForceCard->GetFaceValue()))
				continue; // skip trying a force

			int ix, j; // NCR-FFS added here, removed below
			for(/*int*/ ix=nIndex,j=0; ix<nIndex+numForces; ix++,j++)
			{
				//
				CCard* pTargetCard = outstandingCards[j];
				// NCR-763 Can't "Force" a lower valued card
				if(pForceCard->GetFaceValue() > pTargetCard->GetFaceValue())
					break; //  DONE, exit loop
				// play the next highest card from our hand
//NCR-449		CCard* pCard = suit.GetHighestCardBelow(pLastCard);
//				ASSERT(pCard != NULL);
				CForce* pForce = new CForce(GetCardOwner(pForceCard), 
										    pTargetCard->GetFaceValue(), 
//											pReqPlayedCardsList,
											NULL,
											pForceCard);
				// NCR-707 Compute play's Quality - number of probable tricks after Force played
				double quality = (nLongHand - numOutstandingCards) + 0.5*(nLongHand - (numOutstandingCards+1)/2);
				pForce->SetQuality(quality);  //  NCR-707 save the quality

				forcePlayList << pForce;
				status << "5PLNDEV30! We can play the " & pForceCard->GetFullName() &
						  " to force out the opponents' " & pTargetCard->GetFaceName() & ".\n";
//				pLastCard = pCard; // NCR-162 set next test card ???
			}  // end for(ix) creating Force plays

			// record the force play
			numTotalForces += numForces;
			m_numPlannedForcePlaysInSuit[nSuit] = numForces;

			// then cash the newly promoted cards
			// NCR The following output needs to be built in a String vs using status 
//			status << "5PLNDEV31! This will promote the ";
			CString statusMsg = "5PLNDEV31! This will promote the ";

			// but note that the remaining cashes should not take effect until the
			// offending enemy top cards are eliminated
			nIndex = numWinners + numForces;
			int numLeftOver = nLongHand - nIndex;

			for(j=nIndex; j<nLongHand; j++)
			{
				// get the list of higher outstanding enemy cards
				int numHigherCards = outstandingCards.GetNumCardsAbove(suit[j]);
				CCardList* pRequiredPlayedList = NULL;
				if (numHigherCards > 0)
				{
					pRequiredPlayedList = new CCardList;
					for(int k=0;k<numHigherCards;k++)
						*pRequiredPlayedList << outstandingCards[k];
				}
				// check starting hand
				// fill this code in later

				// and create
				CCard* pCard = suit[j];
				// NCR-449 Skip cashing same card as being used in Force
				if(pForceCard == pCard) {
					numLeftOver--;  // NCR-162 decr as used
					continue;   // skip this card if its the force card
				}
				CCash* pCash = new CCash(GetCardOwner(pCard), CPlay::IN_EITHER, 
					                      pRequiredPlayedList, pCard, nPlayProspect);
				secondaryCashList << pCash;
				numSecondaryCashes++;
				m_numPlannedSecondaryCashPlays++;
				m_numPlannedSecondaryCashPlaysInSuit[nSuit]++;

				statusMsg += pCard->GetFaceName();
				//  NCR-451 Changed following logic
				if (numLeftOver > 2) {
					statusMsg += ", ";		// a, b, c, ...
				}else if (numLeftOver == 2) {
					statusMsg += " and ";	// a and b
				}else if((nLongHand-nIndex) == 1){ 
					statusMsg += " to a winner.\n";  // Only one
				}else if (numLeftOver == 1) {
					statusMsg += " to winners.\n";
				}
				numLeftOver--;  // NCR-162 decr as used???
			} // end for(j) thru building Cash plans

			if(numSecondaryCashes > 0)  {
				status << statusMsg; // NCR output built msg
				// NCR-701 Testing setting PrioritySuit here so PLSEQ6 will move it forward
				int oldPrioritySuit = m_nPrioritySuit; // save for msg
				m_nPrioritySuit = nSuit; // NCR-701 this is now 
				if(oldPrioritySuit != m_nPrioritySuit)
				   status << "PLDEV35! Changed priority suit to " & STSS(m_nPrioritySuit) & " from " 
					                 & STSS(oldPrioritySuit) & ".\n";
			}
		} // end numForces > 0

		// NCR-706 - Are there cashes needed before Force available?
		else if((numWinners > 0) && (numWinners < 3) && (numResiduals > 0) 
			     && (m_nTrumpSuit == NOTRUMP)
			     && ((nLongHand -(numOutstandingCards+1)/2) > 1)) 
		{
			CCard* topCard = suit.GetTopCard();
			int owner = GetCardOwner(topCard);
			CCash* pCash = new CCash(owner, CPlay::IN_EITHER, NULL, topCard, nPlayProspect);
 			m_playPlan << pCash;   //<<<< Note: Add directly to play plan list vs conditional for this method
#ifdef _DEBUG  // NCR DEBUGGING
//			theApp.SetValue(tnFileProgramBuildNumber, 1234); // pass flag to cause saving of board			
#endif
			status << "4PLNDEV80! One cash play has been found, which will prepare to promote "
							& STSS(nSuit) &  "s.\n";
		}  // NCR-706 end making extra cashes

	} // end for(i=0;i<numDevOpportunities ;i++)


	// done
	if (numTotalForces > 1)
		status << "4PLNDEV90! A total of " & numTotalForces & " forcing plays have been found, which will promote " &
				  numSecondaryCashes & " lower card" &
				  ((numSecondaryCashes > 1)? "s.\n" : ".\n");
	else if ((numTotalForces == 1) && (numSecondaryCashes > 0))
		status << "4PLNDEV91! One forcing play has been found, which will promote " &
			      numSecondaryCashes & " lower card" &
				  ((numSecondaryCashes > 1)? "s.\n" : ".\n");
	else
		status << "4PLNDEV95! No suit development plays have been found.\n";
	return numTotalForces;
}






//
//-----------------------------------------------------------------------
//
// FindDropPlays()
//
// - look for suits where a drop is preferable to a finesses
//
int CDeclarerPlayEngine::FindDropPlays(CPlayList& playList)
{
	// generally, we play for a drop when one or two honors are
	// missing out of up to 3 missing cards, and we hold the top 
	// honor in the suit
	CPlayerStatusDialog& status = *m_pStatusDlg;
	int nSuit, nTricksCount = 0;

	// pick the starting suit, skipping the trump suit the last 
	if (ISSUIT(m_nTrumpSuit))
		nSuit = GetNextSuit(m_nTrumpSuit);
	else
		nSuit = SPADES;
	status << "5PLNDRP1! Looking at the hand for drop plays...\n";
	CPlayList localPlayList;

	//
	// and check each suit
	//
	for(int z=0;z<4;z++,nSuit=GetNextSuit(nSuit))
	{
		// skip the trump suit
		if (nSuit == m_nTrumpSuit)
			continue;

		CCombinedSuitHoldings& suit = m_pCombinedHand->GetSuit(nSuit);
		// don't bother if we've already played two rounds in the suit
		// NCR-568 Why this test and exclusion??? 
		if (GetNumCardsPlayedInSuit(m_pPlayer->GetPosition(), nSuit) >= 2) {
			if(suit.GetNumMaxTopCards() < suit.GetNumOutstandingCards())  // NCR-568
				continue;
		}


		//
//		CCombinedSuitHoldings& suit = m_pCombinedHand->GetSuit(nSuit);
		CSuitHoldings& declarer = suit.GetDeclarerSuit();
		CSuitHoldings& dummy = suit.GetDummySuit();

		// see if we have top winners in this suit
//		int numSuitTopCards = suit.GetNumWinners();
		int numSuitTopCards = suit.GetNumTopCards();
		ASSERT(numSuitTopCards <= suit.GetNumCards());

		// no point in trying to drop if we have no top cards, or if we 
		// have 3 or more
		if ((numSuitTopCards == 0) // NCR-464 || (numSuitTopCards > 3))  // NCR changed 2 to 3
			|| (numSuitTopCards >= suit.GetMaxLength()) ) // NCR-463 need more cards to be promoted
			continue;

		// check that we have missing honors
		int numMissingHonors = suit.GetNumMissingHonors();
		if (numMissingHonors == 0)
			continue;

		// check limits on the missing cards
		CCardList missingCards;
		int numOutstandingCards = GetOutstandingCards(nSuit, missingCards);
		// there should be no more than 1/3 or 2/5 cards held/outstanding 
		// X = the number of top cards we hold in the suit
		// e.g., 1/2 -- holding AQJxxx, trying to drop the K from Kxx, or
		//       1/3                    hoping for a K/xx distribution
		//       2/4 -- holding AHTxxx, trying to drop the QJ from QJxxx, or
		//       2/5                    hoping for a QJ/xxx distribution
		// but realistically, we shouldn't work with anything other than
		// a 1/2 or 2/4 distribution -- i.e., X*2, where X = # of top cards held
// NCR-464		if ((numOutstandingCards > (numSuitTopCards*2)) || (numOutstandingCards > 5)) // NCR changed 4 to 5
		// NCR-464 Try drop if there is a chance we can get the top outstanding card
		// alternately, don't try drop if we don't expect to get the top card
		if(numSuitTopCards < (numOutstandingCards+1)/2)  // NCR-464
			continue;		// too many cards outstanding

		// add plays equiv. to the minimum of the top cards held or honors outstanding
		int numPlays = Min(numSuitTopCards, numMissingHonors);
		int nBottomTopCard = suit[numSuitTopCards-1]->GetDeckValue();
		for(int j=0;j<numSuitTopCards && numPlays > 0;j++)  // NCR-464 change ending logic
		{
			// form the enemy Or-cards list
			CCardList* pEnemyOrCards = new CCardList;
			for(int k=0;k<numMissingHonors;k++)
			{
//				int nIndex = nBottomTopCard - 1 - k;
				*pEnemyOrCards << missingCards.GetAt(k); // NCR get here vs: deck.GetSortedCard(nIndex);
			}

			// and create the play
			CCard* pConsumedCard = suit[j];   // get the card to play
			int nTargetHand = declarer.HasCard(pConsumedCard)? CPlay::IN_HAND : CPlay::IN_DUMMY;
			// NCR-148 don't play a high card if other hand has a higher singleton
			if (((nTargetHand == CPlay::IN_HAND) && dummy.IsSingleton() 
			    && ((dummy.GetTopCard()->GetFaceValue() > pConsumedCard->GetFaceValue())
				     // NCR-593 Don't overtake card > top OS card
				    || (dummy.GetTopCard()->GetFaceValue() > missingCards.GetTopCard()->GetFaceValue()) ))
				// NCR-164 this time declarer has 2 cards eg: A6                   // NCR-527 Singleton test  
				|| ((nTargetHand == CPlay::IN_DUMMY) && (declarer.IsDoubleton() || declarer.IsSingleton()) 
// NCR-464				    && (declarer.GetTopCard()->GetFaceValue() >= KING)) 
					&& (suit.GetTopSequence().RangeCovers(declarer.GetTopCard())))  // NCR-464
				// NCR-284 problem if dummy's cards can block suit. Eg Axxx in hand and Dummy has Kx
				|| ((nTargetHand == CPlay::IN_HAND) && dummy.IsDoubleton() && (declarer.GetNumCards() > 2)
				    && (dummy.GetTopCard()->GetFaceValue() > declarer.GetSecondHighestCard()->GetFaceValue())) )
			{
				continue;   // NCR-148 skip this card
			}
			CDrop* pDrop = new CDrop(nTargetHand, 
									 CDrop::AGAINST_EITHER,
									 pEnemyOrCards,
									 pConsumedCard);

			// NCR-707 Compute play's Quality - number of probable tricks after Drop played
			int numOSCardsAfter = numOutstandingCards - numSuitTopCards;
			int numLongCardsAfter = suit.GetMaxLength() - numSuitTopCards;
			double quality = (numLongCardsAfter - numOSCardsAfter) + 0.5*(numLongCardsAfter - (numOSCardsAfter+1)/2);
			pDrop->SetQuality(quality);  //  NCR-707 save the quality

			// NCR-760 See if there could be a blocking problem
			if((nTargetHand == CPlay::IN_DUMMY) && (m_numDummyEntries == 1)
				&& (declarer.GetNumCards() > 1) && (dummy.GetNumCards() > 2)) {
				// If Dummy = Axxxx and Hand = YYZ with Y > x and x > Z
				// you need to save Z as an entry to dummy 
				// lead Y to A instead of Z to preserve entry
				if(dummy.GetSecondHighestCard() > declarer.GetBottomCard()) {
					CCard* pCardToLead = declarer.GetSecondLowestCard();
					pDrop->SetAuxCard(pCardToLead);
				}
			}  // end NCR-760

			// and add
			playList << pDrop;
			nTricksCount++;
			numPlays--; // NCR-464 decrement for plays made

			// then spout message
			if (numMissingHonors == 1)
				status << "5PLNDRP3! Can play the " & pConsumedCard->GetFullName() & " in " &
						  ((nTargetHand == CPlay::IN_HAND)? "hand" : "dummy") & 
						  " to try and drop the opponents' " & pEnemyOrCards->GetAt(0)->GetFaceName() & ".\n";
			else
				status << "5PLNDRP3a! Can play the " & pConsumedCard->GetFullName() & " in " &
						  ((nTargetHand == CPlay::IN_HAND)? "hand" : "dummy") & 
						  " to try and drop one or more of the opponents' { " &
						  pEnemyOrCards->GetHoldingsString() & " }.\n";
		}  // end for(j) thru numPlays

		// update count
		m_numPlannedDropPlaysInSuit[nSuit] = numPlays;

		// and proceed to the next suit
	}  // end for(z) thru the four suits looking for drop possibilities
//	status << ".\n";

	//
	if (nTricksCount > 1)
		status << "4PLNDRP5! A total of " & nTricksCount & " possible drop plays are available.\n";
	else if (nTricksCount == 1)
		status << "4PLNDRP5! One drop play is available.\n";
	else if (nTricksCount == 0)
		status << "4PLNDRP5! No drop plays are available.\n";

	return nTricksCount;
}






//
//-----------------------------------------------------------------------
//
// FindFinessePlays()
//
// - look for possible finesses
//
int CDeclarerPlayEngine::FindFinessePlays(CPlayList& playList)
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	int numTotalFinesses = 0;

	//
	status << "4PLNFNS0! Looking for possible finesses...\n";
//	if ((m_pCombinedHand->GetNumHonors() == 0) ||
	if (m_pCombinedHand->GetNumMissingHonors() == 0)
	{
		status << "4PLNFNS1! The hand offers no apparent finessing opportunities.\n";
		return 0;
	}

	// examine each suit for possible finesses
	int nSuit = 0;
	for(int z=0;z<4;z++)
	{
		if (nSuit == m_nTrumpSuit)
		{
			nSuit = GetPrevSuit(nSuit);
			continue;
		}
		CCombinedSuitHoldings& suit = m_pCombinedHand->GetSuit(nSuit);
		CPlayList suitPlayList;
		int nSuitFinesses = FindFinessesInSuit(suit, suitPlayList);
		if (nSuitFinesses > 0)
		{
			numTotalFinesses += nSuitFinesses;
			playList << suitPlayList;
		}
		//
		nSuit = GetPrevSuit(nSuit);
	}

	// evaluate and sort the finesse possibilities


	// done
	if (numTotalFinesses > 1)
		status << "4PLNFNS90! A total of " & numTotalFinesses & " possible finesses exist.\n";
	else if (numTotalFinesses == 1)
		status << "4PLNFNS91! One possible finesse has been found.\n";
	else
		status << "4PLNFNS95! No immediate finesses have been found.\n";
	return numTotalFinesses;
}






//
//-----------------------------------------------------------------------
//
// FindFinessesInSuit()
//
int CDeclarerPlayEngine::FindFinessesInSuit(CCombinedSuitHoldings& suit, CPlayList& playList)
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	CSuitHoldings& dummy = suit.GetDummySuit();
	CSuitHoldings& declarer = suit.GetDeclarerSuit();

	// see if this is the trump suit
	int nSuit = suit.GetSuit();
	if (nSuit == m_nTrumpSuit)
	{
	}

	// a finesse is possible only if there are missing honors
	if (suit.GetNumMissingHonors() == 0)
		return 0;

	// and at least one hand has at least one card in the suit
//	if ((dummy.GetLength() == 0) || (declarer.GetLength() == 0))
	if (suit.IsVoid())
		return 0;

	//
	int i,numFinesses = 0;

	// NCR-454 Test if 4th is out of this suit - guaranteed finesse!
	const int leaderPos = pDOC->GetRoundLead();
	const int nLastPos = GetPrevPlayer(leaderPos);  // Player to leader's right

	CGuessedHandHoldings* pLastHand = m_ppGuessedHands[nLastPos];
	BOOL bLastOutOfCards = pLastHand->IsSuitShownOut(nSuit);

	// NCR-571 check if we're playing second and our pard has a high card
	int nPlayPos = pDOC->GetNumCardsPlayedInRound();  //NCR-571
	bool bLookForFinesse = false;
    if(nPlayPos == 1) {
		CPlayer* pLastPlayer = pDOC->GetPlayer(nLastPos);
		CHandHoldings& lastPlayersHand = pLastPlayer->GetHand();
		CSuitHoldings& lastPlayersSuit = lastPlayersHand.GetSuit(nSuit);
		// Check if last player has card in the top sequence
		if(!lastPlayersSuit.IsVoid() 
			&& suit.GetSequence2(0).HasCard(lastPlayersSuit.GetTopCard()))
		   bLookForFinesse = true;  //go for it
	} // end NCR-571 

	// check for "commanding" cards
	CCardList& topSequence = suit.GetTopSequence();
	if(!bLastOutOfCards   // NCR-454 OK to finesse if RHO is out
		&& !bLookForFinesse  // NCR-571 set if we want to skip this test
     	// NCR-162 Don't finesse if have 3 cards in sequence and only 1 out greater
		&& ((topSequence.GetNumCards() >= 3) && (suit.GetNumMissingAbove(topSequence.GetTopCard()) == 1)
		   // NCR-409 Don't finesse if we could drop the gap card 
		   || ((topSequence.GetNumCards() >= (suit.GetNumOutstandingCards()+1)/2)
		      && (suit.GetNumOutstandingCards() < 6)   // NCR up to 5 cards out Hope for 3:2 split
			  )) )  
		return 0;    // NCR Exit don't try a finesse with this suit

	CCardList* pDummyCoverCards = NULL;
	CCardList* pDeclarerCoverCards = NULL;
	BOOL bDummyHasCommandingCard = FALSE;
	BOOL bDeclarerHasCommandingCard = FALSE;;

	// check if a hand has a commanding sequence
	CCard* pCard = GetHighestOutstandingCard(suit.GetSuit());
	if ((pCard) && (*topSequence[0] > *pCard))
	{
		if ((dummy.GetNumCards() > 0) && (topSequence.RangeCovers(dummy.GetTopCard())))
		{
			bDummyHasCommandingCard = TRUE;
			pDummyCoverCards = &dummy.GetTopSequence();
			// NCR don't finesse if we can win all tricks in suit
			if((pDummyCoverCards->GetNumCards() >= GetNumOutstandingCards(suit.GetSuit()))
			   // NCR-11 or if declarer has singleton and some trump ??? Player or Partner here ???
			   // NCR-11 Player works for the case: Dummy has Axx and Declarer has singleton
			   || (declarer.IsSingleton() && (m_nTrumpSuit != NOTRUMP) 
			       && (GetPlayer()->GetNumCardsInSuit(m_nTrumpSuit) > 1)
			      // NCR-11 allow finesse if too many losers - How to test this???
			       && (m_pCombinedHand->GetNumLikelyLosers() <= 2)) // NCR-11 allow a few losers
			  )
				return 0;  // don't finesse
		}
		if ((declarer.GetNumCards() > 0) && (topSequence.RangeCovers(declarer.GetTopCard())))
		{
			bDeclarerHasCommandingCard = TRUE;
			pDeclarerCoverCards = &declarer.GetTopSequence();
			// NCR don't finesse if we can win all tricks in suit
			if((pDeclarerCoverCards->GetNumCards() >= GetNumOutstandingCards(suit.GetSuit()))
			   // NCR-11 or if dummy has singleton and some trump	
			   || (dummy.IsSingleton() && (m_nTrumpSuit != NOTRUMP) 
			       && (GetNumDummyEntriesInSuit(m_nTrumpSuit) > 1) 
			       && (m_pCombinedHand->GetNumLikelyLosers() <= 2))  // see above
			  )
				return 0; // don't finesse
		}
	}

	//
	// see what kind of finesses are possible (4 types presently defined)
	// NCR the "against" player is the one to play last
	//

	// prepare basic info
	CCard* pFinesseCard;
	int nGap;
	int numDeclarerLosers = suit.GetNumDeclarerLosers();
	int numDummyLosers = suit.GetNumDummyLosers();

	// make sure finesses are not tried when the suit has all winners
	int numCombinedWinners = suit.GetNumWinners();
	int nMaxLength = Max(suit.GetDeclarerLength(), suit.GetDummyLength());
	bool bHaveLosers = (numCombinedWinners < nMaxLength)? true : false;
//	bool bRejectFinesse = false;

	// now test
	if (bHaveLosers && (suit.GetNumMissingInternalHonors() > 0))
	{
		//
		//---------------------------------------------------------------------
		//
		// Type I Finesse 
		// - lead a low card towards a higher card in the opposite hand, 
		//	 which holds a higher cover card and a commanding top card.
		//   e.g., AQ3 (dummy) / 4 (hand) -- lead the 4, then finesse the Q
		//                      against East, or play low if W plays the K
		//
		// - target hand needs to have a top card (as well as a gap and
		//   a lower ranking card, preferably an honor, and source hand needs 
		//   to have a body card
		// - also, there need to be fewer winners in the combined suit than the 
		//   # of cards in the longer hand
		if ((bDummyHasCommandingCard) && (dummy.GetNumMissingInternalHonors() > 0) &&
						(declarer.GetNumBodyCards() > 0))
		{
			// if LHO has shown out of the suit, the finesse is pointless
			int nLHO = GetNextPlayer(m_pPlayer->GetPosition());
			CGuessedHandHoldings* pLHOHand = m_ppGuessedHands[nLHO];
			if (!pLHOHand->IsSuitShownOut(nSuit))
			{
				// add as many finesses as there cards in dummy's hand below the
				// top sequence, _AND_ low cards in declarer's hand
				CCardList seq;
				int numCandidates = dummy.GetAllCardsBelow(dummy.GetTopSequence().GetBottomCard(), seq);
				int numMaxFinesses = MIN(numCandidates, numDeclarerLosers);
				for(i=numMaxFinesses-1;i>=0;i--)
				{
					// the finesse card in the example above is the Queen in dummy
					pFinesseCard = seq[i];

					// the gap in this case is 1, for the missing King
					nGap = (*(pDummyCoverCards->GetBottomCard()) - *pFinesseCard) - 1;

					// form the list of OR-cards needed by the defenders for this
					// play to have any meaning
					CCardList* pGapCards = new CCardList;
					int nIndex = pFinesseCard->GetDeckValue() + 1;
					for(int j=0;j<nGap;j++)
					{
						CCard* pGapCard = deck.GetSortedCard(nIndex++);
						*pGapCards << pGapCard;
					}

					// check if one or more gap cards are outstanding
					// and break out of this loop if not
					// the confirm procedure will delete the gap cards if needed
					int numGapCards = GetNumGapCardsOutstanding(nSuit, pGapCards);
					if (numGapCards == 0)
						break;

					// don't bother with really deep finesses
					if (numGapCards > 2)
					{
						delete pGapCards;
						continue;
					}

					// init cover cards to the dummy's list of cover cards
					CCardList* pCoverCards = new CCardList(*pDummyCoverCards);

					// form the list of possible lead cards (low cards) from hand
					CCardList* pLeadCards = new CCardList;
					nIndex = declarer.GetNumCards() - numDeclarerLosers;
					for(int k=0;k<numDeclarerLosers;k++)
					{
						// but dont' bother if the lead card is equivalent to the finesse card
						if (suit.AreEquivalentCards(pFinesseCard, declarer[nIndex]))
						{
							nIndex++;
							continue;
						}
						*pLeadCards << declarer[nIndex++];
					}
					//
					if (pLeadCards->IsEmpty())
					{
						// we have no viable cards to lead
						delete pGapCards;
						delete pCoverCards;
						delete pLeadCards;
						break;
					}

					// and create the finesse play object
					CType1Finesse* pFinesse = new CType1Finesse(m_nPosition,
																CFinesse::IN_DUMMY, 
																pGapCards,
																pCoverCards,
																pLeadCards,
																pFinesseCard);
					playList << pFinesse;
					m_numPlannedFinessesInSuit[nSuit]++;
					if (pDummyCoverCards->GetNumCards() == 1)
						status << "5PSFNS10! Can finesse the " & pFinesseCard->GetFullName() & " in dummy with the " &
								  pDummyCoverCards->GetAt(0)->GetFaceName() & " as the potential cover card.\n";
					else
						status << "5PSFNS10! Can finesse the " & pFinesseCard->GetFullName() & " in dummy with the { " &
								  pDummyCoverCards->GetHoldingsString() & " } as the potential cover cards.\n";
					// advance the count
					numFinesses++;
				}
			}
		}

		// or in hand
		if ((bDeclarerHasCommandingCard) && (declarer.GetNumMissingInternalHonors() > 0) &&
						(dummy.GetNumBodyCards() > 0))
		{
			// add as many finesses as there cards in declarer hand below the 
			// top sequence, _AND_ low cards in dummy's hand
			CCardList seq;
			int numCandidates = declarer.GetAllCardsBelow(declarer.GetTopSequence().GetBottomCard(), seq);
			int numMaxFinesses = MIN(numCandidates, numDummyLosers);
			for(i=numMaxFinesses-1;i>=0;i--)
			{
				pFinesseCard = seq[i];

				// determine the gap size
				nGap = (*(pDeclarerCoverCards->GetBottomCard()) - *pFinesseCard) - 1;

				// form the enemy OR-cards list
				CCardList* pGapCards = new CCardList;
				int nIndex = pFinesseCard->GetDeckValue() + 1;
				for(int j=0;j<nGap;j++)
				{
					CCard* pGapCard = deck.GetSortedCard(nIndex++);
					*pGapCards << pGapCard;
				}

				// check if one or more gap cards are outstanding
				int numGapCards = GetNumGapCardsOutstanding(nSuit, pGapCards);
				if (numGapCards == 0)
					break;

				// don't bother with really deep finesses
				if (numGapCards > 2)
				{
					delete pGapCards;
					continue;
				}

				// get the cover cards list
				CCardList* pCoverCards = new CCardList(*pDeclarerCoverCards);

				// form the list of possible lead cards (low cards) from dummy
				CCardList* pLeadCards = new CCardList;
				nIndex = dummy.GetNumCards() - numDummyLosers;
				for(int k=0;k<numDummyLosers;k++)
				{
					// but dont' bother if the lead card is equivalent to the finesse card
					if (suit.AreEquivalentCards(pFinesseCard, dummy[nIndex]))
					{
						nIndex++;
						continue;
					}
					*pLeadCards << dummy[nIndex++];
				}
				//
				if (pLeadCards->IsEmpty())
				{
					// we have no viable cards to lead
					delete pGapCards;
					delete pCoverCards;
					delete pLeadCards;
					break;
				}
				
				// and create the finesse
				CType1Finesse* pFinesse = new CType1Finesse(m_nPosition,
															CFinesse::IN_HAND, 
															pGapCards,
															pCoverCards,
															pLeadCards,
															pFinesseCard);
				playList << pFinesse;
				m_numPlannedFinessesInSuit[nSuit]++;
				if (pDeclarerCoverCards->GetNumCards() == 1)
					status << "5PSFNS11! Can finesse the " & pFinesseCard->GetFullName() & " in hand with the " &
							  pDeclarerCoverCards->GetAt(0)->GetFaceName() & " as the potential cover card.\n";
				else
					status << "5PSFNS11! Can finesse the " & pFinesseCard->GetFullName() & " in hand with the { " &
							  pDeclarerCoverCards->GetHoldingsString() & " } as the potential cover cards.\n";
				// advance the count
				numFinesses++;
			}
		}


		//
		//---------------------------------------------------------------------
		//
		// Type II Finesse
		// - lead the finesse card, when a lower card is held in the same hand, 
		//   towards the opposite hand, which holds a cover card 
		//   e.g., K42 / AJT -- lead the J, cover if West plays Q; the T is 
		//                      then good; else, finesse the J against East
		//
		// target hand needs to have a top card, and the source hand
		// needs an honor with a lower ranking card
		// look first for a finesse in dummy
		BOOL bEligible = FALSE;
		CCardList* pSourceSequence = NULL;
		if (bDummyHasCommandingCard && (declarer.GetNumCards() > 0)) 
		{
			int nFirstIndex = -1;  // NCR-461 Allow index to be set depending on ...
			// look for a card in declarer's hand that is not a commanding card
			// and has a lower adjoining card
			if (topSequence.RangeCovers(declarer.GetTopCard()))
			{
				// top card is in commanding sequence, so check the 
				// top card of the second sequence
				if ((declarer.GetNumSequences() > 1) &&
					(declarer.GetSecondSequence().GetNumCards() > 1))
				{
					bEligible = TRUE;
					pSourceSequence = &declarer.GetSecondSequence();
					nFirstIndex = pSourceSequence->GetNumCards()-2;  // NCR-461
				}
			}
			else if (declarer.GetTopSequence().GetNumCards() > 1)
			{
				// else here, the top card is not part of the commanding sequence,
				// and tops one or more cards
				bEligible = TRUE;
				pSourceSequence = &declarer.GetTopSequence();
				nFirstIndex = pSourceSequence->GetNumCards()-2;  // NCR-461
			}
			// NCR-461 Allow adjoining card to be located in dummy
			else if ((suit.GetSecondSequence().RangeCovers(declarer.GetTopCard())) 
					     && (suit.GetSecondSequence().GetNumCards() > 1))
			{
				// Here the adjoining cards are split between declarer and dummy
				bEligible = TRUE;
				pSourceSequence = &declarer.GetTopSequence();
				nFirstIndex = 0;  // Only one here, so start with the first
			}

			//
			if (bEligible)
			{
				// add as many finesses as there are cards in declarer's eligible 
				// sequence, less one
				for(i=nFirstIndex/*pSourceSequence->GetNumCards()-2*/;i>=0;i--)
				{
					// the gap here is between the bottom cover card and the 
					// highest possible finesse card
					pFinesseCard = (*pSourceSequence)[i];
					CCard* pTopEquivCard = suit.GetHighestEquivalentCard(pFinesseCard);
					nGap = (*(pDummyCoverCards->GetBottomCard()) - *pTopEquivCard) - 1;
					// form the enemy OR-cards list
					CCardList* pGapCards = new CCardList;
					int nIndex = pTopEquivCard->GetDeckValue() + 1;
					for(int j=0;j<nGap;j++)
					{
						CCard* pGapCard = deck.GetSortedCard(nIndex++);
						*pGapCards << pGapCard;
					}
					// check if one or more gap cards are outstanding
					// NCR-404 Skip finesse if no gap cards or too many gap cards
					int nGapCards = GetNumGapCardsOutstanding(nSuit, pGapCards); // NCR-404
					int nMaxGapCards = (pSourceSequence->GetNumCards() > 2) ? 3 : 2;  // NCR-404 set max
					if ((nGapCards == 0) || (nGapCards > nMaxGapCards)  ) // NCR-404 up to 3 gap cards out?
						break;
					// form the list of cover cards
					CCardList* pCoverCards = new CCardList(*pDummyCoverCards);
					// and create the play object
					CType2Finesse* pFinesse = new CType2Finesse(m_nPosition,
																CFinesse::IN_DUMMY, // NCR-421 ???  was HAND, 
																pGapCards,
																pCoverCards,
																pFinesseCard);
					playList << pFinesse;
					m_numPlannedFinessesInSuit[nSuit]++;
					if (pDummyCoverCards->GetNumCards() == 1)
						status << "5PSFNS20! Can finesse the " & pFinesseCard->GetFullName() & " by leading it from hand, with the " &
								  pDummyCoverCards->GetAt(0)->GetFaceName() & " as the potential cover card in dummy.\n";
					else
						status << "5PSFNS20! Can finesse the " & pFinesseCard->GetFullName() & " by leading it from hand, with the { " &
								  pDummyCoverCards->GetHoldingsString() & " } as the potential cover cards in dummy.\n";
					// advance the count
					numFinesses++;
				} // end for(i) thru list of cards to finesse
			}
		}  // end dummy has commmanding card

		// or see if the dummy's hand is eligible for the type II finesse
		if (bDeclarerHasCommandingCard && (dummy.GetNumCards() > 0)) 
		{
			//
			bEligible = FALSE;
			pSourceSequence = NULL;
			// look for a card in dummy's hand that is not a commanding card
			// and has a lower adjoining card
			if (topSequence.RangeCovers(dummy.GetTopCard()))
			{
				// top card is in commanind sequence, so check the 
				// top card of the second sequence
				if ((dummy.GetNumSequences() > 1) &&
					(dummy.GetSecondSequence().GetNumCards() > 1))
				{
					bEligible = TRUE;
					pSourceSequence = &dummy.GetSecondSequence();
				}
			}
			else if (dummy.GetTopSequence().GetNumCards() > 1)
			{
				// else the top card of this sequence will do
				bEligible = TRUE;
				pSourceSequence = &dummy.GetTopSequence();
			}
			//
			if (bEligible)
			{
				// add as many finesses as there are in declarer's eligible 
				// sequence, less one
				for(i=pSourceSequence->GetNumCards()-2;i>=0;i--)
				{
					// the gap here is between the bottom cover card and the 
					// highest possible finesse card
					pFinesseCard = (*pSourceSequence)[i];
					CCard* pTopEquivCard = suit.GetHighestEquivalentCard(pFinesseCard);
					nGap = (*(pDeclarerCoverCards->GetBottomCard()) - *pTopEquivCard) - 1;
					// form the enemy OR-cards list
					CCardList* pGapCards = new CCardList;
					int nIndex = pTopEquivCard->GetDeckValue() + 1;
					for(int j=0;j<nGap;j++)
					{
						CCard* pGapCard = deck.GetSortedCard(nIndex++);
						*pGapCards << pGapCard;
					}
					// check if one or more gap cards are outstanding
//					if (GetNumGapCardsOutstanding(nSuit, pGapCards) == 0)
					// NCR-444 Skip finesse if no gap cards or too many gap cards
					int nGapCards = GetNumGapCardsOutstanding(nSuit, pGapCards); // NCR-404
					int nMaxGapCards = (pSourceSequence->GetNumCards() > 2) ? 3 : 2;  // NCR-444 set max
					if ((nGapCards == 0) || (nGapCards > nMaxGapCards)  ) // NCR-444 up to 3 gap cards out?
						break;
					// form the list of cover cards
					CCardList* pCoverCards = new CCardList(*pDeclarerCoverCards);
					// and create the finesse play object
					CType2Finesse* pFinesse = new CType2Finesse(m_nPosition,
																CFinesse::IN_HAND, // NCR-421  was DUMMY, 
																pGapCards,
																pCoverCards,
																pFinesseCard);
					playList << pFinesse;
					m_numPlannedFinessesInSuit[nSuit]++;
					if (pDeclarerCoverCards->GetNumCards() == 1)
						status << "5PSFNS21! Can finesse the " & pFinesseCard->GetFullName()  
						          & " by leading it from dummy, with the " &
								  pDeclarerCoverCards->GetAt(0)->GetFaceName() & " as the potential cover card in hand.\n";
					else
						status << "5PSFNS21! Can finesse the " & pFinesseCard->GetFullName() 
						          & " by leading it from dummy, with the { " &
								  pDeclarerCoverCards->GetHoldingsString() & " } as the potential cover cards in hand.\n";
					// advance the count
					numFinesses++;
				}
			}
		} // end declarer has commanding card
	}  // end have losers and there is an internal missing honor


	//
	//---------------------------------------------------------------
	//
	// Type III Finesse
	// - lead a low card towards a higher card in the opposite hand, which 
	//   does not have a commanding card.  
	//   e.g., Q52 / J53 -- lead the 3 and finesse the King against East, 
	//                      or play low if West plays the Ace.
	//
	if ((!bDummyHasCommandingCard) && bHaveLosers && (dummy.GetNumHonors() >= 1))
	{
		// add as many finesses as there honors in dummy's top sequence
		// _AND_ low cards in declarer's hand
		CCardList& seq = dummy.GetTopSequence();
		int numMaxFinesses = MIN(seq.GetNumCards(), numDeclarerLosers);
		//
		for(i=numMaxFinesses-1;i>=0;i--)
		{
			// get the finesse card
			pFinesseCard = seq[i];

			// form the enemy OR-cards list
			// the gap card may not be the one immediately above this one
			CCardList* pGapCards = new CCardList;
			CCard* pTopEqivCard = suit.GetHighestEquivalentCard(pFinesseCard);
			int nIndex = pTopEqivCard->GetDeckValue() + 1;
			int nGap = 0;
			for(int ix=pTopEqivCard->GetFaceValue()+1;ix<=ACE;ix++)
			{
				if (suit.HasCardOfFaceValue(ix))
					break;
				nGap++;
			}
			//
			for(int j=0;j<nGap;j++)
			{
				CCard* pGapCard = deck.GetSortedCard(nIndex++);
				// confirm that the gap card is higher (test is required here)
				if (*pGapCard > *pFinesseCard)
					*pGapCards << pGapCard;
			}

			// check if one or more gap cards are outstanding
			if (GetNumGapCardsOutstanding(nSuit, pGapCards) == 0)
				break;

			// form the list of possible lead cards (low cards) from hand
			CCardList* pLeadCards = new CCardList;
			nIndex = declarer.GetNumCards() - numDeclarerLosers;
			for(int k=0;k<numDeclarerLosers;k++)
			{
				// but dont' bother if the lead card is equivalent to the finesse card
				if (suit.AreEquivalentCards(pFinesseCard, declarer[nIndex])
   				       // NCR-624 or if lead card is greater then finesse card
   					|| (declarer[nIndex]->GetFaceValue() > pFinesseCard->GetFaceValue()) )
				{
					nIndex++;
					continue;
				}
				*pLeadCards << declarer[nIndex++];
			}
			//
			if (pLeadCards->IsEmpty())
			{
				// we have no viable cards to lead
				delete pGapCards;
				delete pLeadCards;
				break;
			}

			//NCR-396 Don't finesse if dummy's suit doesn't have more cards than gap cards
			// The gap cards will just eat dummy's cards
			if(dummy.GetNumCards() <= pGapCards->GetNumCards())
				continue;  // NCR-396 skip this finesse 

			// and create the play object
			CType3Finesse* pFinesse = new CType3Finesse(m_nPosition,
														CFinesse::IN_DUMMY, 
														pGapCards,
														pLeadCards,
														pFinesseCard);
			playList << pFinesse;
			m_numPlannedFinessesInSuit[nSuit]++;
			status << "5PSFNS30! Can finesse the " & pFinesseCard->GetFullName() 
// NCR				       & " by leading it from dummy against " &
				       & " by leading to it from hand against " &
					   PositionToString(GetNextPlayer(m_nPosition)) & ".\n";
// NCR				   PositionToString(GetNextPlayer(m_pPartner->GetPosition())) & ".\n";
			// advance the count
			numFinesses++;
		}
	}

	// or in hand
	if ((!bDeclarerHasCommandingCard) && bHaveLosers && (declarer.GetNumHonors() >= 1))
	{
		// add as many finesses as there honors in declarer's top sequence
		// _AND_ low cards in dummy's hand
		CCardList& seq = declarer.GetTopSequence();
		// NCR-464 require adjoining top cards in combined hand => max finesses = number -1
		int nNumTopCards = suit.GetTopSequence().GetNumCards(); // NCR-464  
		int numMaxFinesses = MIN(MIN(nNumTopCards-1, seq.GetNumCards()), numDummyLosers);
		for(i=numMaxFinesses-1;i>=0;i--)
		{
			// get the finesse card
			pFinesseCard = seq[i];

			// form the enemy OR-cards list
			// the gap card may not be the one immediately above this one
			CCardList* pGapCards = new CCardList;
			CCard* pTopEqivCard = suit.GetHighestEquivalentCard(pFinesseCard);
			int nIndex = pTopEqivCard->GetDeckValue() + 1;
			int nGap = 0;
			for(int ix=pTopEqivCard->GetFaceValue()+1;ix<=ACE;ix++)
			{
				if (suit.HasCardOfFaceValue(ix))
					break;
				nGap++;
			}
			for(int j=0;j<nGap;j++)
			{
				CCard* pGapCard = deck.GetSortedCard(nIndex++);
				// confirm that the gap card is higher (test is required here)
				if (*pGapCard > *pFinesseCard)
					*pGapCards << pGapCard;
			}

			// check if one or more gap cards are outstanding
			// NCR-27 Also don't finesse if more than 2 gap cards out
			int numGapCardsOut = GetNumGapCardsOutstanding(nSuit, pGapCards); // NCR-27
			if ((numGapCardsOut == 0) || (numGapCardsOut > 2)) // NCR-27
				break;

			// form the list of possible lead cards (low cards) from dummy
			CCardList* pLeadCards = new CCardList;
			nIndex = dummy.GetNumCards() - numDummyLosers;
			for(int k=0;k<numDummyLosers;k++)
			{
				// but dont' bother if the lead card is equivalent to the finesse card
				if (suit.AreEquivalentCards(pFinesseCard, dummy[nIndex]))
				{
					nIndex++;
					continue;
				}
				*pLeadCards << dummy[nIndex++];
			}
			//
			if (pLeadCards->IsEmpty())
			{
				// we have no viable cards to lead
				delete pGapCards;
				delete pLeadCards;
				break;
			}
					
			// NCR-396 Don't finesse if declarer's suit doesn't have more cards than gap cards
			// The gap cards will just eat declarer's cards
			// Since this hand is hidden, perhaps it would be OK ???
			if(declarer.GetNumCards() <= pGapCards->GetNumCards())
				continue;  // NCR-396 skip this finesse 


			// and create the play object
			CType3Finesse* pFinesse = new CType3Finesse(m_nPosition,
														CFinesse::IN_HAND, 
														pGapCards,
														pLeadCards,
														pFinesseCard);
			playList << pFinesse;
			m_numPlannedFinessesInSuit[nSuit]++;
			status << "5PSFNS31! Can finesse the " & pFinesseCard->GetFullName() 
// NCR				       & " by leading it from hand against " &
				       & " by leading to it from dummy against " &
//wrong against here						   PositionToString(GetNextPlayer(m_pPartner->GetPosition())) & ".\n";
					   PositionToString(GetNextPlayer(m_nPosition)) & ".\n";
			// advance the count
			numFinesses++;
		}
	}



	//
	//---------------------------------------------------------------
	//
	// Type A Finesse
	// - an opportunistic finesse in second or fourth position when a low card 
	//   is led by the opponents, with NO cover cards in the opposite hand
	//   and NO higher cards in the opposite hand
	//
	// check if we have a gap in the honors, located in dummy, and no cover in hand but one in dummy
	CCard* pCardLed = pDOC->GetCurrentTrickCardByOrder(0);
	if ((pDOC->GetNumCardsPlayedInRound() == 1) && (pDOC->GetCurrentPlayer() == m_pPartner) &&
		(pCardLed->GetSuit() == nSuit) && (suit.GetNumMissingHonors() > 0) && (dummy.GetNumCards() > 0) && 
		(dummy.GetNumMissingHonors() > 0) && // NCR-241 !bDeclarerHasCommandingCard &&
		(dummy.GetNumCardsAbove(pCardLed) > 0) && bHaveLosers)
	{
		// get the first non-winning sequence
		CCard *pFinesseCard = NULL, *pTopEquivCard = NULL;
		if (suit.GetNumWinners() > 0)
		{
			// the combined hand has some top cards, so get the top sequence 
			CCardList& seq = suit.GetTopSequence();
			// then get the highest card in dummy that's not in the sequence
			for(int i=0;i<dummy.GetNumCards();i++)
			{
				if (!seq.HasCard(dummy[i]))
				{
					pFinesseCard = dummy[i];
					break;
				}
			}
			// did we find a suitable finesse card?
			if (pFinesseCard)
			{
				// use the lowest equivalent card instead
				pTopEquivCard = pFinesseCard;
				pFinesseCard = dummy.GetLowestEquivalentCard(pFinesseCard);
			}
		}
		else
		{
			// the combined hand has no immediate winners, so 
			// just get the lowest card from dummy's top sequence
			pFinesseCard = dummy.GetTopSequence().GetBottomCard();
			pTopEquivCard = dummy.GetTopCard();
		}

		// see if we have a higher card in hand, ie., with Kx/J
		// there's no point in finessing the J
		BOOL bOppositeHigherCard = FALSE;
		if (pFinesseCard)
		{
			if (declarer.GetNumCardsAbove(pFinesseCard) > 0)
				// NCR-241 play low if have card in hand to win
				// We still want to have TypeA finesse play - is Opportunistic!
//				bOppositeHigherCard = TRUE;
				pFinesseCard = dummy.GetBottomCard(); 
		}

		// see if we have an equivalent singleton card in hand, 
		// and multiple cards in dummy -- then there's no point in finessing from dummy
		BOOL bOppositeEquivSingleton = FALSE;
		if ( pFinesseCard &&
			 (dummy.GetNumCards() > 1) && (declarer.GetNumCards() == 1) && 
			 (suit.AreEquivalentCards(pFinesseCard, declarer[0])) )
			 bOppositeEquivSingleton = TRUE;

		// make sure we have a card we can finesse (& it's higher then the card led)
		CCard* pCardLed = pDOC->GetCurrentTrickCardLed();
		if (pFinesseCard && (*pFinesseCard > *pCardLed) && !bOppositeHigherCard && !bOppositeEquivSingleton)
		{
			// form the enemy OR-cards list
			CCardList* pGapCards = new CCardList;
			int nIndex = pTopEquivCard->GetDeckValue() + 1;
			int nMax = ACE - pTopEquivCard->GetFaceValue();
			for(int j=0;j<nMax;j++)
			{
				if (!suit.HasCard(nIndex))
					*pGapCards << deck.GetSortedCard(nIndex++);
				else
					break;
			}

			// check to be sure one or more gap cards are outstanding
			// (the test routine will delete the gap cards if the test fails)
			int numGapCards = GetNumGapCardsOutstanding(nSuit, pGapCards); // NCR-370
			if ((numGapCards > 0) && (numGapCards <= 2))  // NCR-370 don't finesse if > 2 O.S.
			{
				// confirm that LHO has not shown out of the suit
				CGuessedHandHoldings* pLHOHand = m_ppGuessedHands[m_pLHOpponent->GetPosition()];
				if (pLHOHand->IsSuitShownOut(nSuit))
				{
					// oops!
					status << "5PSFNS42! We'd like to finesse the " & pFinesseCard->GetFullName() & " in dummy against " &
							   PositionToString(GetNextPlayer(m_nPosition)) & ", but he's shown out of " & STS(nSuit) & ".\n";
					delete pGapCards;
				}
				else
				{
					// proceed and create the play object
					CTypeAFinesse* pFinesse = new CTypeAFinesse(m_nPosition,
																CFinesse::IN_DUMMY, 
																pGapCards,
																pFinesseCard);
					playList << pFinesse;
					m_numPlannedFinessesInSuit[nSuit]++;
					status << "5PSFNS44! Can opportunistically finesse the " & pFinesseCard->GetFullName() & " in dummy against " &
							   PositionToString(GetNextPlayer(m_pPartner->GetPosition())) & ".\n";
					// advance the count
					numFinesses++;
				}
			}
		}
	}

	// or in hand
	if ((pDOC->GetNumCardsPlayedInRound() == 1) && (pDOC->GetCurrentPlayer() == m_pPlayer) &&
		(suit.GetNumMissingHonors() > 0) && (declarer.GetNumCards() > 0) &&
		(declarer.GetNumMissingHonors() > 0) && !bDummyHasCommandingCard &&
		(declarer.GetNumCardsAbove(pCardLed) > 0) && bHaveLosers)
	{
		// get the first non-winning sequence
		CCard *pFinesseCard = NULL, *pTopEquivCard = NULL;
		if (suit.GetNumWinners() > 0)
		{
			// the combined hand has some top cards, so get the top sequence 
			CCardList& seq = suit.GetTopSequence();
			// then get the highest card in hand that's not in the sequence
			for(int i=0;i<declarer.GetNumCards();i++)
			{
				if (!seq.HasCard(declarer[i]))
				{
					pFinesseCard = declarer[i];
					break;
				}
			}
			// did we find a suitable finesse card?
			if (pFinesseCard)
			{
				// use the lowest equivalent card instead
				pTopEquivCard = pFinesseCard;
				pFinesseCard = declarer.GetLowestEquivalentCard(pFinesseCard);
			}
		}
		else
		{
			// the combined hand has no winners, so 
			// just get the lowest card from declarer hand's top sequence
			pFinesseCard = declarer.GetTopSequence().GetBottomCard();
			pTopEquivCard = declarer.GetTopCard();
		}

		// see if we have a higher card in dummy, ie., with Kx/J
		// there's no point in finessing the J
		BOOL bOppositeHigherCard = FALSE;
		if (pFinesseCard)
		{
			if (dummy.GetNumCardsAbove(pFinesseCard) > 0)
				bOppositeHigherCard = TRUE;
		}

		// see if we have an equivalent singleton card in hand, 
		// and multiple cards in dummy -- then there's no point in finessing from dummy
		BOOL bOppositeEquivSingleton = FALSE;
		if ( pFinesseCard && 
			 (declarer.GetNumCards() > 1) && (dummy.GetNumCards() == 1) && 
			 (suit.AreEquivalentCards(pFinesseCard, dummy[0])) )
			 bOppositeEquivSingleton = TRUE;

		// make sure we have a card we can finesse (& it's higher then the card led)
		CCard* pCardLed = pDOC->GetCurrentTrickCardLed();
		if (pFinesseCard && pFinesseCard->SameSuit(*pCardLed) 
			&& (*pFinesseCard > *pCardLed) && !bOppositeHigherCard && !bOppositeEquivSingleton)
		{
			// form the enemy OR-cards list
			CCardList* pGapCards = new CCardList;
			int nIndex = pTopEquivCard->GetDeckValue() + 1;
			int nMax = ACE - pTopEquivCard->GetFaceValue();
			for(int j=0;j<nMax;j++)
			{
				if (!suit.HasCard(nIndex))
					*pGapCards << deck.GetSortedCard(nIndex++);
				else
					break;
			}
			// check to be sure one or more gap cards are outstanding
			int numGapCards = GetNumGapCardsOutstanding(nSuit, pGapCards); // NCR-370
			if ((numGapCards > 0) && (numGapCards < 3))  // NCR-370 Not too deep
			{
				// confirm that LHO has not shown out of the suit
				CGuessedHandHoldings* pLHOHand = m_ppGuessedHands[m_pLHOpponent->GetPosition()];
				if (pLHOHand->IsSuitShownOut(nSuit))
				{
					// oops!
					status << "5PSFNS46! We'd like to finesse the " & pFinesseCard->GetFullName() 
						       & " in hand against " &
							   PositionToString(GetNextPlayer(m_nPosition)) & ", but he's shown out of " 
							   & STS(nSuit) & ".\n";
					delete pGapCards;
				}
				else
				{
					// proceed and create the play object
					CTypeAFinesse* pFinesse = new CTypeAFinesse(m_nPosition,
																CFinesse::IN_HAND, 
																pGapCards,
																pFinesseCard);
					playList << pFinesse;
					m_numPlannedFinessesInSuit[nSuit]++;
					status << "5PSFNS47! Can opportunistically finesse the " & pFinesseCard->GetFullName()  
						       & " in hand against " &
							   PositionToString(GetNextPlayer(m_pPartner->GetPosition())) & ".\n";
					// advance the count
					numFinesses++;
				}
			}
		}
	
	}


	// NCR-423 Type B Finesses
	// - an opportunistic finesse in fourth position when a low card 
	//   is led by the opponents, with cover cards in the fourth hand
	//
	// Are we playing second and do we have a winning card
	if ((pDOC->GetNumCardsPlayedInRound() == 1) && (suit.GetNumCardsAbove(pCardLed) > 1)
		&& 	(pCardLed->GetSuit() == nSuit) 
		// Also can't finesse if same number of cards and ???
		&& !((dummy.GetNumCards() == declarer.GetNumCards()) && (dummy.GetNumCards() < 3)) )
	{

		// Determine who is playing 4th
		// If dummy is next, declarer (playing 4th) has the cards to look at
		if((pDOC->GetCurrentPlayer() == m_pPartner) && (declarer.HasTenAce())
			    // We need 2 cards if we're to finesse
			&& (declarer.GetNumCardsAbove(pCardLed) >= 2)) 
		{
			pFinesseCard = declarer.GetLowestCardAbove(pCardLed); // for beginning ???
			CCard* pTopEquivCard = declarer.GetSequence(1).GetTopCard(); // Q&D

			// form the enemy OR-cards list
			CCardList* pGapCards = new CCardList;
			int nIndex = pTopEquivCard->GetDeckValue() + 1;
			int nMax = ACE - pTopEquivCard->GetFaceValue();
			for(int j=0;j<nMax;j++)
			{
				if (!suit.HasCard(nIndex))
					*pGapCards << deck.GetSortedCard(nIndex++);
				else
					break;
			}
			// check to be sure one or more gap cards are outstanding
			int numGapCards = GetNumGapCardsOutstanding(nSuit, pGapCards);
			if ((numGapCards > 0) && (numGapCards < 3))
			{
					// proceed and create the play object
					CTypeBFinesse* pFinesse = new CTypeBFinesse(m_nPosition,
																CFinesse::IN_HAND, 
																pGapCards,
																pFinesseCard);
					playList << pFinesse;
					m_numPlannedFinessesInSuit[nSuit]++;
					status << "5PSFNS48! Can opportunistically finesse the " & pFinesseCard->GetFullName()  
						       & " in hand against " &
							   PositionToString(GetNextPlayer(m_pPartner->GetPosition())) & ".\n";
					// advance the count
			 		numFinesses++;
			}
		} 
		
		// If declarer playing next, then check dummy's cards
		else if((pDOC->GetCurrentPlayer() == m_pPlayer) && (dummy.HasTenAce())
			&& (dummy.GetNumCardsAbove(pCardLed) >= 2))
		{
			pFinesseCard = dummy.GetLowestCardAbove(pCardLed); // for beginning ???
			CCard* pTopEquivCard = dummy.GetSequence(1).GetTopCard(); // Q&D

			// form the enemy OR-cards list
			CCardList* pGapCards = new CCardList;
			int nIndex = pTopEquivCard->GetDeckValue() + 1;
			int nMax = ACE - pTopEquivCard->GetFaceValue();
			for(int j=0;j<nMax;j++)
			{
				if (!suit.HasCard(nIndex))
					*pGapCards << deck.GetSortedCard(nIndex++);
				else
					break;
			}
			// check to be sure one or more gap cards are outstanding
			int numGapCards = GetNumGapCardsOutstanding(nSuit, pGapCards);
			if ((numGapCards > 0) && (numGapCards < 3))
			{
					// proceed and create the play object
					CTypeBFinesse* pFinesse = new CTypeBFinesse(m_nPosition,
																CFinesse::IN_DUMMY, 
																pGapCards,
																pFinesseCard);
					playList << pFinesse;
					m_numPlannedFinessesInSuit[nSuit]++;
					status << "5PSFNS48! Can opportunistically finesse the " & pFinesseCard->GetFullName()  
						       & " in hand against " &
							   PositionToString(GetNextPlayer(m_pPartner->GetPosition())) & ".\n";
					// advance the count
			 		numFinesses++;
			}
		}
	} // NCR-423 end checking for Type B finesse


	//
	//---------------------------------------------------------------------------
	//
	// Type IV Finesse
	// - lead a finesse card that is not part of a sequence, covering the 
	//   opponents' cover and hoping to establish another finesse opportunity 
	//	 e.g., K52 / AJ9 - lead the 9 and if West covers with the Q, 
	//   cover with the K; this sets up a return finesse of the J against
	//   East.  Only works if the Q and T are placed in West/East.

	return numFinesses;
} // end FindFinessesInSuit()





//
// GetNumGapCardsOutstanding()
//
// - checks to see at least one of the the "gap" cards required in a finesse 
//   are still outstanding
//
int CDeclarerPlayEngine::GetNumGapCardsOutstanding(int nSuit, CCardList*& pGapCards) const
{
	// get the list of outstanding cards
	CCardList outstandingCards;
	GetOutstandingCards(nSuit, outstandingCards);
	for(int i=0,nIndex=0;i<pGapCards->GetNumCards();i++)
	{
		// check if the required gap card is not in the outstanding list
		if (!outstandingCards.HasCard(pGapCards->GetAt(nIndex)))
		{
			// if so, remove
			pGapCards->RemoveByIndex(nIndex);
		}
		else
		{
			nIndex++;
		}
	}

	// check count
	if (pGapCards->GetNumCards() == 0)
	{
		// clean up and return
		delete pGapCards;
		pGapCards = NULL;
		return 0;
	}

	// else all's ok
	return pGapCards->GetNumCards();
}






//
// GetPlayHint()
//
CCard* CDeclarerPlayEngine::GetPlayHint()
{
	return PlayCard();
}




//
// GetPlayHintForDummy()
//
CCard* CDeclarerPlayEngine::GetPlayHintForDummy()
{
	return PlayForDummy();
}



////////////////////////////////////////////////////////////////////////////
//
// Misc utils
//
////////////////////////////////////////////////////////////////////////////





//
// GetCardOwner()
//
// returns the card's owner (IN_DECLARER=0=declarer, IN_DUMMY=1=dummy)
//
int CDeclarerPlayEngine::GetCardOwner(CCard* pCard) const
{
	if (pCard->GetOwner() == m_pPlayer->GetPosition())
	{
		return CPlay::IN_HAND;
	}
	else if (pCard->GetOwner() == m_pPartner->GetPosition())
	{
		return CPlay::IN_DUMMY;
	}
	else
	{
		ASSERT(FALSE);
		return 0;
	}
}







//
//==========================================================================
//==========================================================================
//
// Observation routines
//
//
//
//  Show plays in list  NCR-760
//
void CDeclarerPlayEngine::ShowPlayList(CPlayList& playList) {
	CString strDesc, reqStr;
	CPlayerStatusDialog& status = *m_pStatusDlg;

	int m_numPlannedRounds = playList.GetSize();
	for(int i=0;i<m_numPlannedRounds;i++)
	{
		CCardList* pReqList = playList[i]->GetRequiredPlayedCardsList(); 
		bool bHasReqPlay = pReqList != NULL;
		CString qualMsg = "";
		int qualVal = playList[i]->GetQuality();
		if(qualVal != 0) {
			qualMsg.Format(" > Qual=%d", qualVal);
		}
		strDesc = playList[i]->GetFullDescription(); 
		if(bHasReqPlay) {  // Build string showing number required
			reqStr.Format(" <Requires: %d card(s)>", pReqList->GetNumCards());
			strDesc += reqStr;
		}
		status << " Play " & i+1 & ": " & strDesc & qualMsg & "\n";
	}  // end for(i) through play list

}



