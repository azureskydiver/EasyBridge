//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//=====================================================================
//
// CDefenderPlayEngine
//
// - encapsulates the defender's play engine and its status variables
//
//=====================================================================
/*  NCR changes:
    16 Apr 2009 - Opening lead changes
	19 Apr 2009 - Added lead of top of doubleton; lead top of sequence
	20 Apr 2009 - Do NOT discard A,K or Q
	21 Aug 2009 - Added ->GetFaceValue() to pointers returned by from GetTopCard() NCR-28
*/


#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "PlayerStatusDialog.h"
#include "Card.h"
#include "HandHoldings.h"
#include "BidEngine.h"
#include "GIB.h"
#include "progopts.h"
#include "DefenderPlayEngine.h"
#include "deck.h"  //NCR for NoCard
#include "handopts.h"  // NCR-508 for tn... variable definitions




//
//==========================================================================
//==========================================================================
//
// Initialization routines
//
//
CDefenderPlayEngine::CDefenderPlayEngine()
{
}

CDefenderPlayEngine::~CDefenderPlayEngine()
{
	Clear();
}



// 
//-----------------------------------------------------------------------
//
// Initialize()
//
// one-time initialization
//
void CDefenderPlayEngine::Initialize(CPlayer* pPlayer, CPlayer* pPartner, CPlayer* pLHOpponent, CPlayer* pRHOpponent, CHandHoldings* pHoldings, CCardLocation* pCardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine* pBidder, CPlayerStatusDialog* pStatusDlg)
{ 
	// call base class first
	CPlayEngine::Initialize(pPlayer, pPartner, pLHOpponent, pRHOpponent, pHoldings, pCardLocation, ppGuessedHands, pBidder, pStatusDlg);

}



//
//-----------------------------------------------------------------------
//
// Clear()
//
void CDefenderPlayEngine::Clear()
{ 
	// call base class first
	CPlayEngine::Clear();

	// clear class members
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<4;i++)
		m_nPartnerSuitPreference[i] = 0;
	//
	m_nSignalledSuit = NONE;
	m_nEchoedSuit = NONE;
	for(i=0;i<4;i++)
	{
		m_bSuitEchoed[i] = FALSE;
		m_bSuitLed[i] = FALSE;
		m_nSuitSignalStatus[i] = 0;
	}
}



	

//
//-----------------------------------------------------------------------
//
// InitNewHand()
//
// called when the hand is dealt (or play is restarted)
//
void CDefenderPlayEngine::InitNewHand()
{ 
	// make sure we're not declarer
	VERIFY(!m_pPlayer->IsDeclarer());

	// call base class first
	CPlayEngine::InitNewHand();

	// m_numRequiredTricks = # requried to defeat the contract
	m_numRequiredTricks = 13 - (BID_LEVEL(m_nContract) + 6) + 1;
}




//
// RecordCardPlay()
//
// note that a card has been played
//
void CDefenderPlayEngine::RecordCardPlay(int nPos, CCard* pCard) 
{ 
	// first call the base class
	CPlayEngine::RecordCardPlay(nPos, pCard);

	// skip the rest if reviewing a game
	if (pDOC->IsReviewingGame())
		return;

	// then re-evaluate holdings
	m_pHand->ReevaluateHoldings(pCard);

	// if this suit is being led for the first time, mark it
	if (pDOC->GetNumCardsPlayedInRound() == 1)
	{
		int nSuitLed = pCard->GetSuit();
		if (!m_bSuitLed[nSuitLed])
			m_bSuitLed[nSuitLed] = TRUE;
	}

	// see if partner is trying to signal with his discard
	CPlayerStatusDialog& status = *m_pStatusDlg;
	int nRoundLead = pDOC->GetRoundLead();
	CCard* pCardLed = pDOC->GetCurrentTrickCardByOrder(0);
	CCard* pTopCard = pDOC->GetCurrentTrickHighCard(0);
	int nSuitLed = pCardLed->GetSuit();
	int nTrumpSuit = pDOC->GetTrumpSuit();
	int nCurrPlayer = pCard->GetOwner();
	int nPlayerPos = GetPlayerPosition();
	int nPartnerPos = GetPartnerPosition();
	int nRound = pDOC->GetNumTricksPlayed();

	// we only check for hi/lo signals on rounds 1 & 2
	// if we led this round and partner just discarded, check for a signal
	if ( (nRoundLead == nPlayerPos) && (nCurrPlayer == nPartnerPos) &&
		 (pCard != pTopCard) && (pCard->GetSuit() == pCardLed->GetSuit()) )
	{
		// is this a signal?
		if (m_nPartnerSuitPreference[nSuitLed] == 0)
		{
			// check if partner played high (above a six) - NCR change to at least 1 lower out
			// this is the first indication of a suit preference
			CCardList outstandingCards; 
			int numOutstandingCards = GetOutstandingCards(nSuitLed, outstandingCards, true); 
			if ((pCard->GetFaceValue() > 6)  
				    // NCR at least one card lower outstanding ??? how to test this?
				 || ((numOutstandingCards >= 3) && (pCard > outstandingCards.GetBottomCard())
				     && (outstandingCards.GetNumCardsBelow(pCard) > 1))  // NCR-526 require 2 O.S.
			   )
			{
				// NCR-99 What if Dummy has most of the cards/winners in this suit???
				// NCR-109 Also consider number of cards in our hand to see if pard has future winners
				// NCR what does following logic test for ???
//				CPlayer* pDummy = pDOC->GetDummyPlayer();
//				CHandHoldings& dummyHand = pDummy->GetHand();
			    if((pDOC->GetDummyPlayer()->GetHand().GetNumCardsInSuit(nSuitLed)
					+ m_pHand->GetSuit(nSuitLed).GetNumCards()) < numOutstandingCards-2)
				{
					status << "2PLYSIG0! Partner discarded high with the " & pCard->GetFaceName() &
							  ", so consider it a suit preference signal.\n";
					m_nPartnerSuitPreference[nSuitLed] = 1;
				}
				// NCR-292 Flag as possible start of high-lo signal
				else
				{
					m_nPartnerSuitPreference[nSuitLed] = -5;  // NCR-292 set for below
				}
			}
			else
			{
				status << "2PLYSIG1! Partner discarded low with the " & pCard->GetFaceName() &
						  ", so consider it a signal that he dislikes the suit.\n";
				m_nPartnerSuitPreference[nSuitLed] = -1;
			}
		}
		else if ((nRound >= 1) && ((m_nPartnerSuitPreference[nSuitLed] == 1) 
			     || (m_nPartnerSuitPreference[nSuitLed] == -1) || (m_nPartnerSuitPreference[nSuitLed] == -5)) ) // NCR-292 
		{
			// see if we led the previous trick in the same suit, and 
			// partner discarded high or low
			int nPrevRound = nRound - 1;
			CCard* nPrevCardLed = pDOC->GetGameTrickCard(nPrevRound, nPlayerPos);
			if ( (pDOC->GetGameTrickLead(nPrevRound) == nPlayerPos) &&
				 (pDOC->GetGameTrickWinner(nPrevRound) != nPartnerPos) &&
				 (nPrevCardLed->GetSuit() == nSuitLed) )
			{
				// then check partner's discards from last round
				CCard* pPrevCard = pDOC->GetGameTrickCard(nPrevRound, nPartnerPos);
				int nPrevCardVal = pPrevCard->GetFaceValue();      // NCR-292 Q&D for high-lo
				if ( ((m_nPartnerSuitPreference[nSuitLed] == 1) ||(m_nPartnerSuitPreference[nSuitLed] == -5)) 
					 &&	(pCard->GetFaceValue() < nPrevCardVal) )
				{
					// high-lo
					status << "2PLYSIG6! Partner discarded low with the " & pCard->GetFaceName() &
							  " after discarding the " & pPrevCard->GetFaceName() & 
							  " last round, thus marking a high-low suit preference signal.\n";
					m_nPartnerSuitPreference[nSuitLed] = 2;
				}
				else if ( (m_nPartnerSuitPreference[nSuitLed] == -1) &&
										(pCard->GetFaceValue() > nPrevCardVal) )
				{
					// lo-high
					status << "2PLYSIG8! Partner discarded high with the " & pCard->GetFaceName() &
							  " after discarding the " & pPrevCard->GetFaceName() & 
							  " last round, thus marking a low-high signal for a poor suit.\n";
					m_nPartnerSuitPreference[nSuitLed] = -2;
				}
			}
		}
	}

	//
	// if this is notrump, record partner's opening lead suit
	//
	if ( (nRound == 0) && !ISSUIT(nTrumpSuit) && (nRoundLead == nPartnerPos) && 
		 (nCurrPlayer == nPartnerPos) )
	{
		m_nPartnersPrioritySuit = pCardLed->GetSuit();
	}


	//
	// check for an unusual discard / suit preference signal
	// rules:
	// ------
	// 1. partner must not have led
	// 2. partner must have discarded in a side suit (not trumps)
	// 3. this must be his first play of a card in this suit,
	// 3. the discarded card face value must be greater than a six
	// 4. this must be no later than the 4th round
	//
	int nPartnerSuit = pCard->GetSuit();
	if ( (nRoundLead != nPartnerPos) && (nCurrPlayer == nPartnerPos) &&
		 (nPartnerSuit != pCardLed->GetSuit()) &&     //  NCR-665 changed NumCardsPlayed test to 1 vs 0
		 (m_ppGuessedHands[m_nPartnerPosition]->GetSuit(nPartnerSuit).GetNumCardsPlayed() == 1) &&
		 (nPartnerSuit != nTrumpSuit) && (pCard->GetFaceValue() > 6) && (nRound <= 6))
	{
		// this is s suit preference signal
		status << "2PLYSIG20! Partner discarded high in the " & STSS(pCard->GetSuit()) &
				  " suit, indicating a preference for that suit.\n";
		m_nPartnerSuitPreference[pCard->GetSuit()] = 1;
	}
}




//
// RecordCardUndo()
//
// note that a card has been undone
//
void CDefenderPlayEngine::RecordCardUndo(int nPos, CCard* pCard)
{
	// call the base class
	CPlayEngine::RecordCardUndo(nPos, pCard);
}



//
// RecordTrickUndo()
//
// note that the current trick has been undone
// (currently not used)
//
void CDefenderPlayEngine::RecordTrickUndo()
{
	// call base class
	CPlayEngine::RecordTrickUndo();
}



//
// RecordRoundComplete()
//
void CDefenderPlayEngine::RecordRoundComplete(int nPos, CCard* pCard) 
{ 
	// call the base class
	CPlayEngine::RecordRoundComplete(nPos, pCard);
}




//
// RestartPlay()
//
void CDefenderPlayEngine::RestartPlay()
{
	// call base class
	CPlayEngine::RestartPlay();
}





/////////////////////////////////////////////////////////////////////////////
//
//
// UItilities
//
//
//
/////////////////////////////////////////////////////////////////////////////





//
// IsPartnerVoidInSuit()
//
BOOL CDefenderPlayEngine::IsPartnerVoidInSuit(int nSuit)
{
	CGuessedHandHoldings* pPartnersHand = m_pPlayer->GetGuessedHand(GetPartnerPosition());

	// see if partner has shown out
	return pPartnersHand->IsSuitShownOut(nSuit);
}



//
// GetNumClaimableTricks() 
//
int CDefenderPlayEngine::GetNumClaimableTricks() 
{ 
	int numWinners = 0;

	// special handling for suit contracts
	int nTrumpSuit = pDOC->GetTrumpSuit();
	if (ISSUIT(nTrumpSuit))
	{
		// first check trumps
		CSuitHoldings& trumpSuit = m_pHand->GetSuit(nTrumpSuit);
		numWinners = trumpSuit.GetNumWinners();
		// add the winners in the other suits only if we hold all winners in trumps
		// and at least as many as the outstanding trumps
		int numOutstandingTrumps = GetNumOutstandingCards(nTrumpSuit);
		if ( (numWinners >= trumpSuit.GetNumCards()) & (numWinners >= numOutstandingTrumps) )
		{
			for(int i=0;i<4;i++)
			{
				if (i != nTrumpSuit)
					numWinners += m_pHand->GetSuit(i).GetNumWinners();
			}
		}
	}
	else
	{
		// NT contract is straightforward
		numWinners = m_pHand->GetNumWinners();
	}

	//
	return numWinners;
}







/////////////////////////////////////////////////////////////////////////////
//
//
// Analysis routines
//
//
/////////////////////////////////////////////////////////////////////////////




//
// AssessPosition()
//
// called once on each round of play
//
void CDefenderPlayEngine::AssessPosition()
{
	// call base class first
	CPlayEngine::AssessPosition();

	// then perform class-specific operations
	m_bUsingGIB = theApp.GetValue(tbEnableGIBForDefender);
}




//
// GetLeadCard()
//
// - returns a card to lead
//
CCard* CDefenderPlayEngine::GetLeadCard()
{
	// get basic info
	CPlayerStatusDialog& status = *m_pStatusDlg;
	// NCR added const to most of following variables
	const int nRound = pDOC->GetPlayRound();
	const int nTrumpSuit = pDOC->GetTrumpSuit();
	const int nContractLevel = pDOC->GetContractLevel();
	CCard* pLeadCard = NULL;
	const int nPlayerPos = GetPlayerPosition();
	const int nPartnerPos = GetPartnerPosition();
	int nPartnersSuit = NONE;
	const int	nDeclarer = pDOC->GetDeclarerPosition();
//	int nDummy = pDOC->GetDummyPosition();
	CPlayer* const pDummy = pDOC->GetDummyPlayer(); //NCR get reference to Dummy


	// see if this is the first round
	if (nRound == 0)
	{
		// this is the opening lead
		status << "2PLYLEAD! Making opening lead as " & PositionToString(nPlayerPos) &
				  " against a contract of " & ContractToFullString(pDOC->GetContract()) & ".\n";
		// review partner's bidding history
		nPartnersSuit = ReviewBiddingHistory();

		// NCR if nothing in history, see if we've set an Agreed suit
		if(!ISSUIT(nPartnersSuit))
		{
//			nPartnersSuit = m_nAgreedSuit; // NCR ???
		}

		// special code -- see if we can beat a slam contract in our hand
		if (nContractLevel == 7)
		{
			// do we have an Ace?
			if (m_pHand->GetNumAces() >= 1)
			{
				int nSuit; // NCR-FFS added here, removed below
				for(/*int*/ nSuit=CLUBS;nSuit<=SPADES;nSuit++)
				{
					if (m_pHand->GetSuit(nSuit).HasAce())
						break;
				}
				pLeadCard = m_pHand->GetSuit(nSuit).GetTopCard();
				status << "PLYLDS1! We can " & (ISSUIT(nTrumpSuit)? "most likely" : "") & 
						  "defeat the contract by playing the " & pLeadCard->GetName() & " here, so lead it now.\n";
				return pLeadCard;
			}
		}
		else if (nContractLevel == 6)
		{
			// do we have two Aces?
			if (m_pHand->GetNumAces() >= 2)
			{
				// lead Aces from the shortest suit first
				int nSuit = ANY, nIndex = 3;
				while(nIndex >= 0)
				{
					nSuit = m_pHand->GetSuitsByLength(nIndex);
					if (m_pHand->GetSuit(nSuit).HasAce())
						break;
					nSuit = m_pHand->GetSuitsByLength(nIndex);
					nIndex--;
				}
				pLeadCard = m_pHand->GetSuit(nSuit).GetTopCard();
				status << "PLYLDS2! We can " & (ISSUIT(nTrumpSuit)? "most likely" : "") & 
						  "defeat the contract by playing two Aces, so lead them starting with the shortest suit -- lead the " & pLeadCard->GetName() & " first.\n";
				return pLeadCard;
			}
			// else see if we have an Ace-King combo
			int nSuit = ANY, nIndex = 3;
			while(nIndex >= 0)
			{
				nSuit = m_pHand->GetSuitsByLength(nIndex);
				if (m_pHand->GetSuit(nSuit).HasAce() && m_pHand->GetSuit(nSuit).HasKing())
					break;
				nSuit = m_pHand->GetSuitsByLength(nIndex);
				nIndex--;
			}
			if (nIndex >= 0)
			{
				pLeadCard = m_pHand->GetSuit(nSuit).GetTopCard();
				status << "PLYLDS3! We " & (ISSUIT(nTrumpSuit)? "can" : "may be able to") & 
						  " defeat the contract by playing two Aces, so lead them starting with the shortest suit -- lead the " & pLeadCard->GetName() & " first.\n";
				return pLeadCard;
			}
		}

		// see if we're playing in a suit contract
		if (ISSUIT(nTrumpSuit))
		{
			// leading in the first round against a suit contract
			// either lead from a singleton, from partner's suit, from a sequence, 
			// an Ace, a trump, or simply 4th best

			// see if we have a singleton and some trumps
			if ((m_pHand->GetNumSingletons() >= 1) && (m_pHand->GetNumTrumps() > 0))
			{
				// but don't lead the trump suit, or a singleton honor
				// NCR allow lead of singleton Ace
				// NCR-686 Singleton Jack OK 
				for(int i=0;i<4;i++)
				{
					CSuitHoldings& suit = m_pHand->GetSuit(i);
					if ((suit.GetNumCards() == 1) 
						&& (suit[0]->GetFaceValue() <= JACK || suit[0]->GetFaceValue() == ACE) //NCR
						&& (i != nTrumpSuit))
					{
						pLeadCard = suit[0];
						status << "PLYLD4! Lead the singleton " & pLeadCard->GetName() & ".\n";
						ASSERT(m_pHand->HasCard(pLeadCard));
						return pLeadCard;
					}
				}
			}

			// see if we can lead partner's suit
			if (ISSUIT(nPartnersSuit))
			{
				CSuitHoldings& partnersSuit = m_pHand->GetSuit(nPartnersSuit);
				if (partnersSuit.GetNumCards() >= 3)
				{
					//NCR Lead Ace in suit contract with 2 or more cards
					if(partnersSuit.HasAce()) {
						pLeadCard = partnersSuit[0]; //NCR Is this Ace or need to get seq???
						status << "PLYLD4! Lead the Ace of partner's " & STSS(nPartnersSuit) & ".\n"; 
					}else {	
						//NCR  Lead top of sequence, if any
						CCardList& seq = partnersSuit.GetTopSequence();
						if (seq.GetNumCards() > 1)
						{
							// we have a sequence in partner's suit, so lead from the top
							pLeadCard = seq[0];
							status << "PLYLD15! Lead the " & pLeadCard->GetName() &  
									  " from the top of the { " & seq.GetHoldingsString() & 
							          "} sequence.\n";
						}else { // NCR end of added code

							// lead LOW with 3+ cards
							pLeadCard = partnersSuit.GetBottomCard();
							status << "PLYLD4! Lead the bottom card of partner's " & STSS(nPartnersSuit) & 
								  " suit (the " & pLeadCard->GetFaceName() & ".\n";
						}
					}
					ASSERT(m_pHand->HasCard(pLeadCard));
					return pLeadCard;
				}
				else if (partnersSuit.GetNumCards() > 0)
				{
					pLeadCard = partnersSuit[0];
					status << "PLYLD4! Lead the top card of partner's " & STSS(nPartnersSuit) & 
							  " suit (the " & pLeadCard->GetFaceName() & ".\n";
					ASSERT(m_pHand->HasCard(pLeadCard));
					return pLeadCard;
				}
			}

			// else can't lead partner's suit, so check each of our suits
			for(int i=0;i<4;i++)
			{
				int nSuit = m_pHand->GetSuitsByPreference(i);
				if (nSuit == nTrumpSuit)
					continue;	// but don't lead a trump
				CSuitHoldings& suit = m_pHand->GetSuit(nSuit);
				if (suit.GetNumCards() == 0)
					continue;
				CCardList& seq = suit.GetTopSequence();
				if (seq.GetNumCards() > 1)
				{
					// we have a sequence in our favorite suit, so lead from the top
					if(suit.HasAce())  // NCR King from Ace King
						pLeadCard = seq[1];  // the King if top is Ace
					else
						pLeadCard = seq[0];
					status << "PLYLD15! Lead the " & pLeadCard->GetName() &  
							  " from the top of the { " & seq.GetHoldingsString() & 
							  "} sequence.\n";
					ASSERT(m_pHand->HasCard(pLeadCard));
					return pLeadCard;
				}
			}
/*   NCR removed leading an ace
			// lead out an ace if available
			if (m_pHand->GetNumAces() > 0)
			{
				// but not the trump ace
				for(int i=0;i<4;i++)
				{
					CSuitHoldings& suit = m_pHand->GetSuit(i);
					if (suit.HasAce() && (i != nTrumpSuit))
					{
						pLeadCard = suit[0];
						status << "PLYLD18! Lead the " & pLeadCard->GetName() & " to grab a quick trick.\n";
						ASSERT(m_pHand->HasCard(pLeadCard));
						return pLeadCard;
					}
				}
			}
*/ // NCR end removing ace lead

            // NCR lead top of a doubleton
			if(m_pHand->GetNumDoubletons() > 0) {
//				for(int i=0; i < m_pHand->GetNumDoubletons(); i++) 
//					CSuitHoldings& dblton = m_pHand->GetValuePV(tnDoubletonSuits, i); //???
				for(int i=0; i<4; i++) {
					if (i == nTrumpSuit)
						continue;	// but don't lead a trump

					CSuitHoldings& dblton = m_pHand->GetSuit(i);
					if (dblton.IsDoubleton()) { // a doubleton?
						pLeadCard = dblton.GetTopCard();
						// NCR don't lead a King or Queen
						if(pLeadCard->GetFaceValue() < QUEEN) 
						{
							status << "PLYLD17! Lead the " & pLeadCard->GetName() 
									  & " Top of doubleton.\n";
							ASSERT(m_pHand->HasCard(pLeadCard));
  							return pLeadCard;
						}
					}
				}
			} // NCR end getting a doubleton to lead

			// lead a trump if appropriate (add code here later)
			//NCR Lead "top of nothing" in trump
			CSuitHoldings& trumps = m_pHand->GetSuit(nTrumpSuit);
			if(trumps.GetNumCards() > 0) 
			{	
				if (trumps[0]->GetFaceValue() < NOTHING)
				{
					pLeadCard = trumps.GetTopCard();
					status << "PLYLD18! Lead the " & pLeadCard->GetName() & " Top of nothing.\n";
					ASSERT(m_pHand->HasCard(pLeadCard));
					return pLeadCard;
				}
				// NCR-97 lead top trump if we have a sequence
				else if(trumps.GetSequence2(0).GetNumCards() > 2) // NCR-310 this failed ???
				{
					pLeadCard = trumps.GetTopCard();
					status << "PLYLD18a! Lead the " & pLeadCard->GetName() & " Top of sequence.\n";
					ASSERT(m_pHand->HasCard(pLeadCard));
					return pLeadCard;
				}
			}  // NCR end leading trump

			// here, no other lead was found, so simply lead 4th best
			// NCR-252 problem here if chosen card unguards Jack in 4 card suit ???
			return Get4thBestLeadCard();

		}
		else
		{
			// else here we're playing in a notrump contract
			// lead the the top card from a 3-card honor sequence, or
			// otherwise 4th card of the best suit
			int nSuit = m_pHand->GetSuitsByPreference(0);
			CSuitHoldings& suit = m_pHand->GetSuit(nSuit);
			CCardList& seq = suit.GetTopSequence();
			if ((seq.GetNumCards() >= 3) && (IsHonor(seq[0]->GetFaceValue())))
			{
				// we have a 3-card honor sequence in out favorite suit, so lead from the top
				pLeadCard = seq[0];
				m_nPrioritySuit = nSuit;
				status << "PLYLD22! Lead the " & pLeadCard->GetName() & 
						  " from the top of the " & seq.GetNumCards() & 
						  "-card sequence {" & seq.GetHoldingsString() & "}.\n";
			}
			// NCR If partner bid, lead his suit
			else if((pLeadCard = CheckLeadPartnersSuit(nPartnersSuit)) != NULL) {  // NCR added explict test
				return pLeadCard;  // NCR ok do it
			}
			else
			{
				// lead fourth-best from this suit OR our longest suit
				CCard* pLeadCard = Get4thBestLeadCard();
				int theSuit = pLeadCard->GetSuit();
				// NCR-664 Don't make it the priority suit unless ...
				CSuitHoldings& suitH = m_pHand->GetSuit(theSuit);
				if((suitH.GetNumCards() > 4)  || (suitH.GetSequence2(0).GetNumCards() > 1)
					|| ((suitH.GetNumSequence2s() > 1) && (suitH.GetSequence2(1)[0]->GetFaceValue() > TEN)) )
				{
					m_nPrioritySuit = theSuit;   // Save it
				}  // end NCR-664
				return pLeadCard;
			}
		}
	}
	else
	{
		//
		// playing in the second or later round
		//

		// on the second trick, see if we can defeat a slam contract 
		// by playing a winner
		int nDefendingTeam = m_pPlayer->GetTeam();
		CHandHoldings& dummyHand = pDOC->GetDummyPlayer()->GetHand();
		//
		if ((nRound == 1) && (nContractLevel == 6) && (m_pHand->GetNumWinners() > 0) &&
			(pDOC->GetNumTricksWonByTeam(nDefendingTeam) == 1))
		{
			// lead an Ace if we have one
			if (m_pHand->GetNumAces() >= 1) 
			{
				// lead Aces from the shortest suit first
				int nSuit = ANY, nIndex = 3;
				while(nIndex >= 0)
				{
					nSuit = m_pHand->GetSuitsByLength(nIndex);
					if (m_pHand->GetSuit(nSuit).HasAce())
						break;
					nSuit = m_pHand->GetSuitsByLength(nIndex);
					nIndex--;
				}
				pLeadCard = m_pHand->GetSuit(nSuit).GetTopCard();
				if (ISSUIT(nTrumpSuit))
					status << "PLYLDS6a! Try to defeat the contract now by playing the " & pLeadCard->GetName() & ".\n";
				else
					status << "PLYLDS6b! Try to defeat the contract now by playing the " & pLeadCard->GetName() & ".\n";
				return pLeadCard;
			}
			// else play a king
			if (m_pHand->GetNumKings() > 1)  // NCR require 2 Kings???
			{
				// lead Aces from the shortest suit first
				int nSuit = ANY, 
					nIndex = 3;
				while(nIndex >= 0)
				{
					nSuit = m_pHand->GetSuitsByLength(nIndex);
					// NCR-113 Use first suit that has a winner.   NOTE: Problem with GetNumWinners() ???
					if(m_pHand->GetSuit(nSuit).GetTopCard() > m_pHand->GetSuit(nSuit).GetMissingSequence(0).GetTopCard())
						break;
					// NCR-113 Leading a King seems bad. Maybe OK if have Queen also -> Added HasQueen()
					if (m_pHand->GetSuit(nSuit).HasKing() && m_pHand->GetSuit(nSuit).HasQueen())
						break;
					nSuit = m_pHand->GetSuitsByLength(nIndex);
					nIndex--;
				}
				// NCR What if no KQ combos found???
				pLeadCard = m_pHand->GetSuit(nSuit).GetTopCard();
				if (ISSUIT(nTrumpSuit))
					status << "PLYLDS6c! Try to defeat the contract now by playing the " & pLeadCard->GetName() & ".\n";
				else
					status << "PLYLDS6d! Defeat the contract now by playing the " & pLeadCard->GetName() & ".\n";
				return pLeadCard;
			}
		} // end second round for small slam

		// NCR-266 test if we can set contract by leading a winner
		if (m_numRequiredTricks <= pDOC->GetNumTricksWonByTeam(m_pPlayer->GetTeam())+1) // booked?
		{
			pLeadCard = CashWinners();
			if(pLeadCard != NULL) {
				status << "PLYLDS6e! Try to defeat the contract now by playing the " & pLeadCard->GetName() & ".\n";
				return pLeadCard;
			}
		} // end NCR-266 

		// if we're leading in the second or later round after winning 
		// the prior round which partner led, return partner's suit
		CCard* pPartnersLastCard = pDOC->GetGameTrickCard(nRound-1, nPartnerPos);
		int nSuit = pPartnersLastCard->GetSuit();

		// NCR-21 If pard's last card played was trump, find what he last lead
		bool bReturnPardsSuit = false;
		if((nSuit == nTrumpSuit) && (nRound > 1)) // NCR added nRound test
		{
			int nRnd = nRound-2;
			int nLeader = pDOC->GetGameTrickLead(nRnd);
			if(nLeader == nPartnerPos) {
				CCard* pPardsLastLead = pDOC->GetGameTrickCard(nRnd, nPartnerPos);
				if(pPardsLastLead != NULL) {  // Possible none found
					nSuit = pPardsLastLead->GetSuit();
					bReturnPardsSuit = true;
				}
			}
		} //  NCR-21 end getting pard's last lead

//		int nLead = pDOC->GetGameTrickLead(nRound-1);
		if ( (((pDOC->GetGameTrickWinner(nRound-1) == nPlayerPos)
			    && (pDOC->GetGameTrickLead(nRound-1) == nPartnerPos))
			   || bReturnPardsSuit) // NCR-21
			  && (m_pHand->GetNumCardsInSuit(nSuit) > 0)
			  // NCR don't lead trump suit???
			  && (nSuit != nTrumpSuit) )
		{
			// but don't bother returning the suit if this is a suit contract 
			// and dummy is void in the suit with trumps available
			BOOL bDummyCanTrump = FALSE;
			CHandHoldings& dummyHand = pDOC->GetDummyPlayer()->GetHand();
			if ((ISSUIT(nTrumpSuit)) && (dummyHand.GetNumCardsInSuit(nSuit) == 0) &&
										(dummyHand.GetNumTrumps() > 0))
			{
				status << "3PLYLDRT1! We'd like to return partner's " & STS(nSuit) &
					      " suit, but dummy might ruff.\n";
				bDummyCanTrump = TRUE;
			}

			// also don't bother returning the suit if this is a suit contract (NCR why suit???) 
			// and dummy has the top cards outstanding -- i.e., if dummy's 
			// top card is higher than any outstanding cards
			// NCR-21 need test here if pard could be void and have trump???
			CCardList outstandingCards;
			BOOL bDummyHasHighCard = FALSE; // NCR-564
			const int numOutstandingCards = GetOutstandingCards(nSuit, outstandingCards); 
			if (/*(ISSUIT(nTrumpSuit)) &&*/ (dummyHand.GetNumCardsInSuit(nSuit) > 0) && // NCR removed test
					(numOutstandingCards > 0) &&
					(dummyHand.GetSuit(nSuit).GetTopCardVal() > outstandingCards[0]->GetFaceValue())
					// NCR-267 Test if dummy's top card beats ours
					&& (dummyHand.GetSuit(nSuit).GetTopCardVal() > m_pHand->GetSuit(nSuit).GetTopCardVal()) )
			{
				status << "3PLYLDRT2! We'd like to return partner's " & STS(nSuit) &
					      " suit, but dummy will most likely win.\n";
				bDummyHasHighCard = TRUE;   // NCR-564 vs DummCanTrump
			}

			// also see if declarer might trump
			// that's if there are trumps outstanding, but none in dummy and
			// partner has shown out
			BOOL bDeclarerCanTrump = FALSE;
			if ( (ISSUIT(nTrumpSuit)) && (GetNumOutstandingCards(nTrumpSuit) > 0) &&
							 (IsPartnerVoidInSuit(nTrumpSuit)) &&
							 (dummyHand.GetNumTrumps() == 0) )
			{
				status << "3PLYLDRT4! We'd like to return partner's " & STS(nSuit) &
					      " suit, but declarer might ruff.\n";
				bDeclarerCanTrump = TRUE;
			}

			// NCR-180 return the suit if partner played high-low
			BOOL bPardPlayedHiLow = FALSE;
			CCard* aCard = GetPrevCardPlayedThisSuit(nPartnerPos, // NCR-561
				                   pDOC->GetGameTrickCard(nRound-1, nPartnerPos)->GetSuit(),
								   nRound-2);
			if((nRound >= 2) && ISSUIT(nTrumpSuit) && (aCard != NULL)
    			// NCR-556 Compare last card pard played in this suit against previous card
                && (aCard->GetFaceValue()  // NCR-561
				    > pDOC->GetGameTrickCard(nRound-1, nPartnerPos)->GetFaceValue())
                // NCR-564 and pard lead the last card
				&& (pDOC->GetGameTrickLead(nRound-1) == nPartnerPos) ) 
			{
				bPardPlayedHiLow = TRUE; // NCR-180 allow lead of this suit		
			}	

			// NCR-663 Check if pard could be void 
			if(!bReturnPardsSuit) {
				bReturnPardsSuit = (numOutstandingCards < 3);
			} // end NCR-663

			// return a winner in this suit if we have one; else return a low card
			CSuitHoldings& suit = m_pHand->GetSuit(nSuit);   // NCR-561 changed below to >= vs >
			if ((suit.GetNumCards() > 0) && (numOutstandingCards >= 2) // NCR-228 require some??? cards out
				&& (!bDummyCanTrump && !bDeclarerCanTrump)    // NCR-564 Not if can be ruffed 
				&&  (bPardPlayedHiLow || bReturnPardsSuit) ) // NCR-180 pard could be out & NCR-663
			{
				// mark this as the priority suit
				m_nPrioritySuit = nSuit;
				// NCR-79 Play low if Dummy has singleton Top card
				//        Play high if only 1 card out > our top sequence of 2 cards
//				CCardList & mis_seq = suit.GetMissingSequence(0);  // NCR DEBUG - showed bug in CheckKeyHoldings()

				bool bPlayHigh = (suit.GetNumMissingSequences() > 0)
					              && (suit.GetMissingSequence(0).GetNumCards() < suit.GetSequence2(0).GetNumCards()); 
				if(!dummyHand.GetSuit(nSuit).IsVoid() && dummyHand.GetSuit(nSuit).IsSingleton() 
					&& dummyHand.GetSuit(nSuit).GetTopCard()->GetFaceValue() > suit.GetTopCard()->GetFaceValue())
					bPlayHigh = false;  // Don't play high if dummy's singleton is higher
				if ((suit.GetNumWinners() > 0) || bPlayHigh)
					pLeadCard = suit.GetTopSequence().GetBottomCard();
				else
					pLeadCard = suit.GetBottomCard();
				status << "PLYLDRT8! Return partner's " & STSS(nSuit) & " suit with the " &
						  pLeadCard->GetFaceName() & ".\n";
				ASSERT(m_pHand->HasCard(pLeadCard));
				return pLeadCard;
			}
		}

		// see if we're playing in a suit contract
		if (ISSUIT(nTrumpSuit))
		{
			// check if partner has shown out of a suit
			int numPartnerVoidSuits = 0;
			int nPartnerVoidSuits[4] = { NONE, NONE, NONE, NONE };
			for(int i=0;i<4;i++)
			{
				if (IsPartnerVoidInSuit(i) && (i != nTrumpSuit))
					nPartnerVoidSuits[numPartnerVoidSuits++] = i;
			}

			// see if there are trumps outstanding, partner has not shown out 
			// of trumps, and _has_ shown out of a suit, _and_ we have no 
			// winners in that suit (whew!)
			// NCR-455 If only one trump out it will win anytime!!
			int numOutstandingTrumps = GetNumOutstandingCards(nTrumpSuit, TRUE);  // NCR-682 Ck Dummy also
			   // NCR-455 change test to 1 vs 0 // NCR skip if dummy has all trumps
			if ( (numOutstandingTrumps > 1) && (dummyHand.GetNumTrumps() != numOutstandingTrumps)
				 && !IsPartnerVoidInSuit(nTrumpSuit) && (numPartnerVoidSuits > 0))
			{
				// see if we can lead the suit
				for(int j=0;j<numPartnerVoidSuits;j++)
				{
					int theSuit = nPartnerVoidSuits[j]; // NCR-603
					CSuitHoldings& suit = m_pHand->GetSuit(theSuit);
					int nbrDummyCards = dummyHand.GetSuit(theSuit).GetNumCards(); // NCR-603
					int nbrOutstandingCards = GetNumOutstandingCards(theSuit, true); // NCR-603
//					if (suit.GetNumLosers() > 0)
					if ((suit.GetNumCards() > 0)  // NCR-603 && (suit.GetNumWinners() == 0))
						// NCR-603 lead if there are cards out and void hand plays first
						&& (nbrOutstandingCards > 0) 
						&& ((m_pLHOpponent == pDummy)  // Don't care if void here
						     || ((m_pRHOpponent == pDummy) && (nbrDummyCards != 0))) )
					{
						// by all means, lead the suit
						pLeadCard = suit.GetBottomCard();
						// NCR-178 don't give up cheap trick if have a long sequence
						if((suit.GetSequence2(0).GetNumCards() >= 3)
							// NCR-614 Play top of sequence if only have a sequence
							|| (suit.GetSequence2(0).GetNumCards() == suit.GetNumCards())
							 // NCR-606 Declarer playing last is void
							|| ((m_pLHOpponent == pDummy)
							    && (nbrDummyCards == nbrOutstandingCards)) )
							pLeadCard = suit.GetSequence2(0).GetTopCard();  // NCR-178  high card
						status << "PLYLD32! Partner is void in " & STS(nPartnerVoidSuits[j]) &
								  " and may still have some trumps, so lead the " & 
								  pLeadCard->GetName() & ".\n";
						ASSERT(m_pHand->HasCard(pLeadCard));
						return pLeadCard;
					}
				}
			}

			// here, we had no luck in finding a suit for partner to ruff, so 
			// see if he expressed a suit preference
			pLeadCard = FindLeadCardFromPartnerPreference();
			if (pLeadCard)
			{
				ASSERT(m_pHand->HasCard(pLeadCard));
				return pLeadCard;
			}

			// NCR-101 Did partner bid any suit?
			nPartnersSuit = ReviewBiddingHistory();
			if(ISSUIT(nPartnersSuit) && (m_pHand->GetSuit(nPartnersSuit).GetNumCards() > 0)
				// NCR-182 Don't lead if dummy void and has trump
				&& !(dummyHand.GetSuit(nPartnersSuit).IsVoid() && !dummyHand.GetSuit(nTrumpSuit).IsVoid())
				)
			{
				pLeadCard = m_pHand->GetSuit(nPartnersSuit).GetBottomCard();
				// NCR-542 One last check before we make a stupid play
				if(dummyHand.GetSuit(nPartnersSuit).IsSingleton() 
				   && (dummyHand.GetSuit(nPartnersSuit).GetTopCard()->GetFaceValue() < pLeadCard->GetFaceValue())) 
				{
				  status << "PLYLD33! Partner has bid the " & STS(nPartnersSuit) &
						  " suit, so lead the " & pLeadCard->GetName() & ".\n";
				  return pLeadCard;
				} // end NCR-542
			}  // NCR-101 end


			// NCR-432 Pull trump if we have winners
			if( (m_pHand->GetSuit(nTrumpSuit).GetNumCards() > 0)
				&& (m_pHand->GetSuit(nTrumpSuit).GetSequence2(0).GetNumCards() >= numOutstandingTrumps)
				&& (numOutstandingTrumps > 0) )
			{
				CCardList outstandingCards;                                 // NCR-677 added LookAtDummy
				const int numOutstandingCards = GetOutstandingCards(nTrumpSuit, outstandingCards, true);
				if(m_pHand->GetSuit(nTrumpSuit).GetTopCardVal() > outstandingCards.GetTopCardVal())
				{
					pLeadCard = m_pHand->GetSuit(nTrumpSuit).GetTopCard(); // NCR-432 pull the trump
					status << "PLYLD35! We have the boss trumps, so pull declarer's trumps by"
						       & " leading the " & pLeadCard->GetName() & ".\n";
					return pLeadCard;
				}
			}  // end NCR-432 pulling declarers trumps

			// no luck in finding suit for partner, so pick our own suit to lead
			// if all trumps have been accounted for, cash winners
			// but not if it's a trump winner -- save it for later
			pLeadCard = CashWinners();

			// NCR don't lead an Ace if ...
			//    King NOT played
			//    dummy does NOT have singleton (we have trumps)
			//    > 6 cards outstanding  (How many ???)
			if(pLeadCard && (pLeadCard->GetFaceValue() == ACE)) 
			{
				int nSuit = pLeadCard->GetSuit();
				CCardList outstandingCards;                 // NCR-581 false don't count dummy's cards
				int numOutstandingCards = GetOutstandingCards(nSuit, outstandingCards, false); 
				// NCR-581 save Ace if 4 or more cards out
				if((numOutstandingCards > 3) && !(dummyHand.GetSuit(nSuit).IsSingleton()) 
					&& (outstandingCards.GetTopCard()->GetFaceValue() == KING)) // NCR what if King in dummy?
				{
					pLeadCard = NULL;  // don't lead the Ace
				}

			}

			if (pLeadCard && (pLeadCard->GetSuit() != nTrumpSuit))
			{
				ASSERT(m_pHand->HasCard(pLeadCard));
				return pLeadCard;
			}
			else{
				pLeadCard = NULL;  //NCR don't want this one
			}

			// NCR-555 Return partner's opening lead if we have some and dummy not void
			// Only worry about it if in the early rounds
			if((nRound < 4) && (pDOC->GetGameTrickLead(0) == nPartnerPos))
			{
				CCard* pPartnersFirstCard = pDOC->GetGameTrickCard(0, nPartnerPos);
				int nSuit = pPartnersFirstCard->GetSuit();
				if((dummyHand.GetSuit(nSuit).GetNumCards() > 1) && (m_pHand->GetSuit(nSuit).GetNumCards() > 0)) 
				{
					pLeadCard = m_pHand->GetSuit(nSuit).GetTopCard();   // Which one, top or ???
					status << "PLYLD38! Return partner's openning lead by"
						       & " leading the " & pLeadCard->GetName() & ".\n";
					return pLeadCard;				
				}
			} // end NCR-555

			// else we've got nothing to lead, so try to lead something safe
			// lead a suit that the opponents won't ruff
/*
			CGuessedSuitHoldings* pDeclarerTrumps = m_pPlayer->GetGuessedHand(nDeclarer)->GetSuit(nTrumpSuit);
			CSuitHoldings& dummyTrumps = dummyHand.Suit(nTrumpSuit);
			BOOL bDeclarerMayTrump = FALSE, bDummyMayTrump = FALSE;
			if ((numOutstandingTrumps > 0) && !pDeclarerTrumps->IsSuitShownOut())
				bDeclarerMayTrump = TRUE;
			if (dummyTrumps.GetnumCards() > 0)
				bDummyMayTrump = TRUE;
*/
			bool bMayLeadTrump = false;  //NCR could lead trump if all else fails

			// check each suit
			int j; // NCR-FFS added here, removed below
			for(/*int*/ j=0;j<4;j++)
			{
				CSuitHoldings& suit = m_pHand->GetSuit(j);
				if (j == nTrumpSuit){
					// NCR might lead trump if few and small
					if((!suit.IsVoid() && suit.GetNumCards() <= 3) && (suit.GetTopCard()->GetFaceValue() < 7))
						bMayLeadTrump = true; // remember
					continue;
				}
				if (suit.GetNumCards() == 0)
					continue;

				CGuessedSuitHoldings& declarerSuit = m_pPlayer->GetGuessedHand(nDeclarer)->GetSuit(j);
				CSuitHoldings& dummySuit = dummyHand.GetSuit(j);
				const int numOutstandingCards = GetNumOutstandingCards(j);
				// lead a suit if declarer/dummy has not shown out of the suit
				//NCR don't lead an A or Q if dummy has K
				if (!declarerSuit.IsSuitShownOut() && (dummySuit.GetNumCards() > 0) &&
									(numOutstandingCards >= 1) )
				{
					//NCR check for top of sequence first
					if(suit.GetNumSequences() > 0) 
					{
						// NCR Need more logic here - this is quick fix for now
						// NCR does a sequence have more than 1 card??? and honors?
						if((suit.GetTopSequence().GetNumCards() > 1) 
							&& (suit.GetTopCard()->GetFaceValue() >= TEN))
						{
							pLeadCard = suit.GetTopCard();	//NCR which one??	
						}
						else
						{
							// NCR if dummy on left and it has TenAce in suit
							if((m_pLHOpponent == pDummy) && dummySuit.HasTenAce() 
								&& (suit.GetNumCardsAbove(JACK) < 1)) // NCR-100 and we're not finessing ourself
							{
								pLeadCard = suit.GetBottomCard(); // lead thru TenAce
							}
							else if((m_pRHOpponent == pDummy) && dummySuit.HasTenAce())
							{
								continue; // don't lead to TenAce
							}
							else 
							// NCR what to do here???
							continue; //NCR skip this suit???
						}
					}else { //NCR end 
						pLeadCard = suit.GetBottomCard();
					}
					status << "PLYLD42! With no good card to lead, lead the " & pLeadCard->GetName() &
							  " since the opponents have not shown out of the " & STSS(pLeadCard->GetSuit()) &
							  " suit.\n";
					ASSERT(m_pHand->HasCard(pLeadCard));
					return pLeadCard;
				}
			} // end for(j) thru suits

			// NCR-304 Check if we can draw trumps and have winners left after that

			//NCR is trump a possibility?
			if(bMayLeadTrump){
				pLeadCard = m_pHand->GetSuit(nTrumpSuit).GetBottomCard();  // lead low trump
				status << "PLYLD44! With no other obvious plays, lead a low trump " & pLeadCard->GetName() & ".\n";
				ASSERT(m_pHand->HasCard(pLeadCard));
				return pLeadCard;
			}  // NCR end leading trump

			// else lead a suit that dummy won't ruff
			for(j=0;j<4;j++)
			{
				CSuitHoldings& suit = m_pHand->GetSuit(j);
				if ((suit.GetNumCards() == 0)
					|| m_ppGuessedHands[pDOC->GetDeclarerPosition()]->IsSuitShownOut(j)) //NCR declarer out?
					continue; 
				CSuitHoldings& dummySuit = dummyHand.GetSuit(j);
				const int numOutstandingCards = GetNumOutstandingCards(j);
				if ((dummySuit.GetNumCards() > 0) && (numOutstandingCards >= 1)
					// NCR don't lead singleton into winning dummy cards
					&& (suit.IsSingleton() && (suit.GetTopCard()->GetFaceValue() > dummySuit.GetTopCard()->GetFaceValue())
					    && (nTrumpSuit != j)) //NCR  save high trump???
					)
				{
					pLeadCard = suit.GetBottomCard();
					status << "PLYLD43! With no good card to lead, lead the " & pLeadCard->GetName() &
							  " since dummy is not void in the " & STSS(pLeadCard->GetSuit()) &
							  " suit (we don't know about declarer, though).\n";
					ASSERT(m_pHand->HasCard(pLeadCard));
					return pLeadCard;
				}
			}  // end for(j) thru suits

		}
		else
		{
			// playing in notrump
			//NCR see if we can defeat the contract by leading a winner
			if(pDOC->GetNumTricksWonByTeam(nDefendingTeam) == (7 - nContractLevel)) 
			{
				pLeadCard = CashWinners();
				if (pLeadCard)
				{
					status << "PLYLD45! Defeat contract by cashing a winner with the " & pLeadCard->GetName() & ".\n";
					ASSERT(m_pHand->HasCard(pLeadCard));
					return pLeadCard;
				}
			} // NCR end defeating contract by cashing a winner

			// see if partner has expressed a suit preference and has not shown out
			// of that suit
			pLeadCard = FindLeadCardFromPartnerPreference();
			if ((!ISSUIT(m_nPrioritySuit) || (m_pHand->GetSuit(m_nPrioritySuit).GetNumCards() < 3)) // NCR-553 need some cards
				&& pLeadCard && !IsPartnerVoidInSuit(pLeadCard->GetSuit()))
			{
				// if we like this suit also, mark it as our priority suit
				int nPartnerSuit = pLeadCard->GetSuit();
//				if (!ISSUIT(m_nPrioritySuit) && (m_pHand->GetSuitStrength(nPartnerSuit) >= SS_GOOD_SUPPORT))
				// mark this as the suit to develop if we like it
				if (m_pHand->GetSuitStrength(nPartnerSuit) >= SS_GOOD_SUPPORT)
					m_nPrioritySuit = nPartnerSuit;
				//
				ASSERT(m_pHand->HasCard(pLeadCard));
				return pLeadCard;
			}

			// see if we're currently trying to develop a suit
			if (ISSUIT(m_nPrioritySuit))
			{
				// see if we should keep trying to develop the suit
				if ((m_pHand->GetSuit(m_nPrioritySuit).GetNumCards() <= 1))
				{
					 status << "4PLYLD50! With " &
							   ((m_pHand->GetSuit(m_nPrioritySuit).GetNumCards() == 1)? "only one card" : "no cards") &
							   " left in " &  STS(m_nPrioritySuit) & 
							   ", there's no point in continuing to try and develop the suit.\n";
					 m_nPrioritySuit = NONE;
				}
				else
				{   
					// NCR have at least 2 cards. Check if dummy has more and better cards than we do
					CSuitHoldings& dummySuit = dummyHand.GetSuit(m_nPrioritySuit);
					CSuitHoldings& mySuit = m_pHand->GetSuit(m_nPrioritySuit);
					// NCR a Q&D test needs work???
					if ( ((dummySuit.GetNumCards() > 2) && (mySuit.GetNumCards() <= 3)
						 && (dummySuit.GetSecondHighestCard() > mySuit.GetTopCard()))
						   // NCR-538 more Q&D tests
						|| ((dummySuit.GetNumCards() >= mySuit.GetNumCards()) 
						    && (dummySuit.GetTopCard() > mySuit.GetTopCard())) )
					{
						status << "4PLYLD52! Dummy has a better " & STS(m_nPrioritySuit) & " suit. "
							   "There's no point in continuing to try and develop the suit.\n";
						m_nPrioritySuit = NONE;
					}
				}
				

				//
				if (ISSUIT(m_nPrioritySuit))
				{
					 // also stop developing the suit if partner and either N/S 
					 // show out (i.e., the opponents have no entries)
					 BOOL bPartnerShownOut = m_ppGuessedHands[m_nPartnerPosition]->IsSuitShownOut(m_nPrioritySuit);
					 BOOL bLHOShowOut = m_ppGuessedHands[GetNextPlayer(m_nPosition)]->IsSuitShownOut(m_nPrioritySuit);
					 BOOL bRHOShowOut = m_ppGuessedHands[GetPrevPlayer(m_nPosition)]->IsSuitShownOut(m_nPrioritySuit);
					 if ( (bPartnerShownOut && bLHOShowOut && !bRHOShowOut) ||
							(bPartnerShownOut && bLHOShowOut && !bRHOShowOut) )
					 {
						 status << "4PLYLD54! Since both partner and " & (bLHOShowOut? "LHO" : "RHO") &
								   " have shown out, there is no point in trying to keep developing the " &
								   STSS(m_nPrioritySuit) & " suit, as we'll only provide entries for the oponents.\n";
						 m_nPrioritySuit = NONE;
					 }
				}
			}

			// NCR-508 Test if we have a suit with winners that we might not get another chance to play
			// One suit stopped and no other cards of that suit are out
			if(m_pHand->GetNumSuitsStopped() == 1)
			{
				int stoppedSuitIx = (int)m_pHand->GetValuePV(tnStoppedSuits, 0); // get stopped suit
				// Check if any cards outstanding
				CSuitHoldings& stoppedSuit = m_pHand->GetSuit(stoppedSuitIx);
				if((stoppedSuit.GetNumCards() > 0) && (stoppedSuit.GetNumMissingSequences() == 0))
				{
					pLeadCard = stoppedSuit.GetTopCard();
					status << "PLYLD58! With no other obvious plays, cash a winner " & pLeadCard->GetName() & ".\n";
					return pLeadCard;
				}
			}  // end NCR-508 testing for possible stranded winners

			// if we're still trying to develop or work a suit, 
			// keep playing the suit
			int nSuit = NONE;
			if (ISSUIT(m_nPrioritySuit)){
				nSuit = m_nPrioritySuit;
			}
			else if (ISSUIT(m_nPartnersPrioritySuit) 
				      // NCR make sure partner has some of them
				      && !m_ppGuessedHands[m_nPartnerPosition]->IsSuitShownOut(m_nPartnersPrioritySuit)
				      && (m_pHand->GetSuit(m_nPartnersPrioritySuit).GetNumCards() >= 1))
			{
				// NCR-666 Don't use Pard's suit if not enough outstanding
				CCardList outstandingCards;
				const int numOutstandingCards = GetOutstandingCards(m_nPartnersPrioritySuit, outstandingCards);
				CSuitHoldings& dummySuit = dummyHand.GetSuit(m_nPartnersPrioritySuit);
				if((numOutstandingCards > dummySuit.GetNumCards())  
					&& ((dummySuit.GetNumCards() == 0)
					     || (outstandingCards[0]->GetFaceValue() > dummySuit.GetTopCard()->GetFaceValue())) )
				{
					nSuit = m_nPartnersPrioritySuit;
				} // end NCR-666
			}
			// NCR check that we're NOT leading to dummy's TenAce on RH
			if (ISSUIT(nSuit) && (m_pRHOpponent == pDummy)) 
			{
				CSuitHoldings& dummySuit = dummyHand.GetSuit(nSuit);
				// does dummy have at least 3 cards with a TenAce/
				if((dummySuit.GetNumCards() >= 3) && dummySuit.HasTenAce()) {
					nSuit = NONE;  // NCR don't lead to dummy's TenAce
				}
			}
			// NCR don't lead a suit if dummy has more of them and also has the top ones
			if(ISSUIT(nSuit) && (nSuit != m_nPartnersPrioritySuit)) // NCR-389 Not if pard's suit
			{
				CSuitHoldings& dummySuit = dummyHand.GetSuit(nSuit);
				CSuitHoldings& suit = m_pHand->GetSuit(nSuit);
				if((dummySuit.GetNumCards() > suit.GetNumCards())
					&& (dummySuit.GetTopCard()->GetFaceValue() > suit.GetTopCard()->GetFaceValue())) // NCR-28
				{
					nSuit = NONE;		// NCR don't lead if dummy's suit is better
				}
			}

			if (ISSUIT(nSuit))
			{
				CSuitHoldings& suit = m_pHand->GetSuit(nSuit);
				pLeadCard = suit[0];
				// NCR if contract > 5 don't lead an honor unless there are two equal ones
				if((nContractLevel > 5) && (suit.GetTopSequence().GetNumCards() < 2) 
					&& (suit.GetNumSequences() > 1))
					pLeadCard = suit.GetSecondSequence().GetTopCard();  // NCR get a middle card
				status << "PLYLD76! Continue developing " &
						  ((nSuit == m_nPrioritySuit)? "our " : "partner's ") & STSS(nSuit) & 
						  " suit by playing the " & pLeadCard->GetName() & ".\n";
				ASSERT(m_pHand->HasCard(pLeadCard));
				// check for suit depletion
				if (suit.GetNumCards() == 1)
				{
					// mark the the priority suit is no longer a priority!
					if (nSuit == m_nPrioritySuit)
						m_nPrioritySuit = NONE;
					else if (nSuit == m_nPartnersPrioritySuit)
						m_nPartnersPrioritySuit = NONE;
				}
				return pLeadCard;
			}

			// if we have no suit we're trying to develop yet, but have a good suit 
			// with an honor sequence, lead the top card of the sequence to 
			// try and promote the cards below
			nSuit = m_pHand->GetPreferredSuit();
			CSuitHoldings& suit = m_pHand->GetSuit(nSuit);
			if (!ISSUIT(m_nPrioritySuit) && (suit.GetStrength() >= SS_OPENABLE) && 
				(suit.GetNumHonors() >= 2) && (nRound <= 6) && !suit.HasTenAce())  //NCR don't lead from TenAce
			{
//				pLeadCard = suit[0];
				// mark this as the suit to develop
				m_nPrioritySuit = nSuit;
				pLeadCard = suit.GetLowestCardAbove(9);
				status << "PLYLD80! Try to develop our " & STSS(nSuit) & 
						  " suit by playing the " & pLeadCard->GetName() & ".\n";
				ASSERT(m_pHand->HasCard(pLeadCard));
				return pLeadCard;
			}

			// NCR-258 Is this suit strong enough to pull all of outstanding cards?
			if((suit.GetNumCards() > 2) && (suit.GetNumMissingSequences() > 0)  // first test that we have the top card
				 && (suit.GetTopCard()->GetFaceValue() > suit.GetMissingSequence(0).GetTopCard()->GetFaceValue()) )
			{
				// Now see if we have enough cards to pull all the rest
				int numOSCards = suit.GetNumMissingBelow(ACE);  // Note want all missing cards. We have top card
				int numOurTopCards = suit.GetSequence2(0).GetNumCards();
				CSuitHoldings& dummySuit = dummyHand.GetSuit(nSuit);
				if((numOurTopCards > numOSCards) 
					|| ((numOurTopCards >= numOSCards-2)   // -2 assumes cards NOT all in one hand
					    && (dummySuit.GetNumCards() > 0) && (dummySuit.GetNumCards() <= numOurTopCards)) )
				{
					pLeadCard = suit.GetTopCard();
					status << "PLYLD83! With no other obvious plays, play top card the " & pLeadCard->GetName() 
						       & " and hope for drop.\n";
					return pLeadCard;
				}
			} // NCR-258 end

			// NCR get 4th best if we have a loooong suit (at least 6)
			pLeadCard = Get4thBestLeadCard(6);  
			if (pLeadCard)
			{
				status << "PLYLD81! With no other obvious plays, fourth best with the " & pLeadCard->GetName() & ".\n";
				ASSERT(m_pHand->HasCard(pLeadCard));
				return pLeadCard;
			}
			// no luck in finding suit for partner, so pick our own suit to lead
			// try cashing winners
			pLeadCard = CashWinners();
			if (pLeadCard)
			{
				status << "PLYLD84! With no other obvious plays, cash a winner with the " & pLeadCard->GetName() & ".\n";
				ASSERT(m_pHand->HasCard(pLeadCard));
				return pLeadCard;
			}

			// else we're SOL
		} // NCR end finding lead for NoTrump 
	} // NCR end playing in second and following rounds


	// if all else fails, give up and call the base class
	// TODO: specialize the code from the base class to pick the suit that dummy
	// is weakest in
	if (pLeadCard == NULL) 
	{
		pLeadCard = CPlayEngine::GetLeadCard();
		// NCR-4 One last test to keep from making a stupid play
		// ??? Where should this code go???
		// Does dummy have a singleton, greater card than this that we can beat without giving up a trick?
		int bidSuit = pLeadCard->GetSuit();
		if(pDummy->GetHand().GetSuit(bidSuit).IsSingleton()
    		// NCR-401 Leave card to play if it beats dummy's card
			&& (pDummy->GetHand().GetSuit(bidSuit).GetTopCard()->GetFaceValue() > pLeadCard->GetFaceValue()) ) 
		{
			CCard * pBetterCard = m_pHand->GetSuit(bidSuit).GetLowestCardAbove(pDummy->GetHand().GetSuit(bidSuit).GetTopCard());
			if(pBetterCard != NULL)  {
				pLeadCard = pBetterCard;
				status << "PLYLD86! Whoops, don't let dummy win this, change lead to " & pLeadCard->GetName() & ".\n";
			}
		}
	} // NCR-4 end Q&D to not play J from KJ when dummy has singleton Q

	// done
	return pLeadCard;
} // end GetLeadCard()

//
// Check if we can/should lead partner's suit

CCard* CDefenderPlayEngine::CheckLeadPartnersSuit(int nPartnersSuit) 
{
	CCard* pLeadCard = NULL;
	CPlayerStatusDialog& status = *m_pStatusDlg;

	// see if we can lead partner's suit
	if (ISSUIT(nPartnersSuit))
	{
		CSuitHoldings& partnersSuit = m_pHand->GetSuit(nPartnersSuit);
		if (partnersSuit.GetNumCards() >= 3)
		{
			//NCR Lead Ace in suit contract with 2 or more cards
			if(partnersSuit.HasAce()) {
				pLeadCard = partnersSuit[0]; //NCR Is this Ace or need to get seq???
				status << "PLYLD4! Lead the Ace of partner's " & STSS(nPartnersSuit) & ".\n"; 
			}else {	
				//NCR  Lead top of sequence, if any
				CCardList& seq = partnersSuit.GetTopSequence();
				if (seq.GetNumCards() > 1)
				{
					// we have a sequence in partner's suit, so lead from the top
					pLeadCard = seq[0];
					status << "PLYLD15! Lead the " & pLeadCard->GetName() &  
							  " from the top of the { " & seq.GetHoldingsString() & 
							  "} sequence.\n";
				}else { // NCR end of added code

					// lead LOW with 3+ cards
					pLeadCard = partnersSuit.GetBottomCard();
					status << "PLYLD4! Lead the bottom card of partner's " & STSS(nPartnersSuit) & 
						  " suit (the " & pLeadCard->GetFaceName() & ".\n";
				}
			}
			ASSERT(m_pHand->HasCard(pLeadCard));
			return pLeadCard;
		}
		else if (partnersSuit.GetNumCards() > 0)
		{
			pLeadCard = partnersSuit[0];
			status << "PLYLD4! Lead the top card of partner's " & STSS(nPartnersSuit) & 
					  " suit (the " & pLeadCard->GetFaceName() & ".\n";
			ASSERT(m_pHand->HasCard(pLeadCard));
			return pLeadCard;
		}
	}
	return NULL;
}


//
// GetPrevCardPlayedThisSuit()
//
// Look thru played cards for previous card played of this suit by this player
//
CCard* CDefenderPlayEngine::GetPrevCardPlayedThisSuit(const int nPlayer, const int nSuit, const int nRound) const // NCR-561
{
	VERIFY((nPlayer >= 0) && (nPlayer <= 3) && ISSUIT(nSuit));

	for(int i = nRound; i >= 0; i--) {
		CCard* aCard = pDOC->GetGameTrickCard(i, nPlayer);
		if(aCard->GetSuit() == nSuit)
			return aCard;
	} // end for(i) through rounds played
	return NULL;
}



//
// FindLeadCardFromPartnerPreference()
//
// - see if partner expressed a suit preference, and if so, see whether
//   we can lead a card in the suit
//
// - returns a suit partner bid, if any
//
int CDefenderPlayEngine::ReviewBiddingHistory()
{
	int numBidsMade = pDOC->GetNumBidsMade();
	int numBiddingRounds = (numBidsMade +3) / 4;  // NCR added +3

	// we have the # of bidding rounds (rounded up)
	// check partner's bids for each round
	int nPartner = GetPartnerPosition();
	int nTopSuit = NONE;
	int nTopSuitVal = 0;
	for(int i=0;i<numBiddingRounds;i++) // NCR changed from 4 to numBiddingRounds
	{
		int nBid = pDOC->GetBidByPlayer(nPartner, i);
		if (ISBID(nBid))
		{
			int nSuit = BID_SUIT(nBid);
			m_nPartnerSuitPreference[nSuit]++;
			if (m_nPartnerSuitPreference[nSuit] > nTopSuitVal)
			{
				// this suit is the "favorite"
				nTopSuit = nSuit;
				nTopSuitVal = m_nPartnerSuitPreference[nSuit];
			}
		}
	}

	// return the suit bid most by partner, if any
	return nTopSuit;
}





//
// FindLeadCardFromPartnerPreference()
//
// - see if partner expressed a suit preference, and if so, see whether
//   we can lead a card in the suit
//
CCard* CDefenderPlayEngine::FindLeadCardFromPartnerPreference()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	int nTrumpSuit = pDOC->GetTrumpSuit();
//	int nDummy = pDOC->GetDummyPosition();
	CCard* pLeadCard = NULL;

	//
	for(Suit nSuit=CLUBS;nSuit<SPADES; GETNEXTSUIT(nSuit))  // NCR changed i to nSuit
	{
		// check the suit -- partner must've signalled favorably, must not have
		// shown out of the suit, and _we_ must have cards in the suit
		CSuitHoldings& suit = m_pHand->GetSuit(nSuit);
		if ( (m_nPartnerSuitPreference[nSuit] > 0) && (suit.GetNumCards() > 0) &&
			 (!IsPartnerVoidInSuit(nSuit)) )
		{
			// check if we're in a suit contract
			if (ISSUIT(nTrumpSuit))
			{
				// don't bother if dummy is void in the suit and has trumps
//				CGuessedHandHoldings* pDummyHand = m_pPlayer->GetGuessedHand(nDummy);
//				if (pDummyHand->IsSuitShownOut(nSuit) && 
//					(pDummyHand->GetSuit(nTrumpSuit).GetNumRemainingCards() > 0) )
				// NCR used following to test if dummy could trump this suit
				CHandHoldings& dummyHand = pDOC->GetDummyPlayer()->GetHand();
				if ((dummyHand.IsSuitVoid(nSuit)) && (dummyHand.GetNumTrumps() > 0))
				{
					// dummy will ruff, so don't bother with this suit
					status << "3PLYLDPRF1! Partner likes the " & STSS(nSuit) & 
							  " suit, but dummy can ruff, so skip this suit for now.\n";
					continue;
				}

				// see if we think declarer has trumps in the suit
				int nDeclarer = pDOC->GetDeclarerPosition();
				CGuessedHandHoldings* pDeclarerHand = m_pPlayer->GetGuessedHand(nDeclarer);
				if ( pDeclarerHand->IsSuitShownOut(nSuit) && 
					((pDeclarerHand->GetSuit(nTrumpSuit).GetNumRemainingCards() > 0) ||
					 (GetNumOutstandingCards(nTrumpSuit) > 0)) )
				{
					// declarer may ruff, so don't bother with this suit
					status << "3PLYLDPRF2! Partner likes the " & STSS(nSuit) & 
							  " suit, but declarer has shown out of the suit and _may_ be able to ruff, so avoid leading the suit if possible.\n";
					continue;
				}

				// NCR don't lead trump from short suit with honor
				if((nSuit == nTrumpSuit) && (suit.GetNumCards() <= 2) 
					&& (suit.GetTopCard()->GetFaceValue() > JACK) ) {
					continue;
				}
				// NCR-20 Check that opponents doesn't have all the winners
				ASSERT(pDOC->GetDummyPlayer()->AreCardsExposed());
//NCR				CSuitHoldings& dummySuit = dummyHand.GetSuit(nSuit);
				// get the # of outstanding cards in the suit
				// (in partner's or declarer's hands)
				CCardList outstandingCards;
				int numOutstandingCards = GetOutstandingCards(nSuit, outstandingCards);
				if (numOutstandingCards == 0)
					continue;	// NCR-20 partner has no cards in this suit

				// else we can lead the suit
				// play a winner if we have one, else lead low
				if (suit.GetNumWinners() > 0)
					pLeadCard = suit[0];
				else 
				{
					// NCR check that we don't lead low into dummy with better cards ???
					if((dummyHand.GetSuit(nSuit).GetNumCards() > 0)
						&& (suit.GetNumCardsAbove(dummyHand.GetSuit(nSuit).GetTopCard()) > 2)) //NCR have 3 better?
						pLeadCard = suit.GetLowestCardAbove(dummyHand.GetSuit(nSuit).GetTopCard());
					else
						pLeadCard = suit.GetBottomCard();
				}
				status << "PLYLDPRF4! Partner has indicated a preference for the " & STS(nSuit) &
						  " suit, so lead the " & pLeadCard->GetName() & ".\n";
				return pLeadCard;
			}
			else
			{
				// notrump contract
				// see if dummy holds the top two cards in the suit
				ASSERT(pDOC->GetDummyPlayer()->AreCardsExposed());
				CHandHoldings& dummyHand = pDOC->GetDummyPlayer()->GetHand();
				CSuitHoldings& dummySuit = dummyHand.GetSuit(nSuit);
				// get the # of outstanding cards in the suit
				// (in partner's or declarer's hands)
				CCardList outstandingCards;
				int numOutstandingCards = GetOutstandingCards(nSuit, outstandingCards);
				if (numOutstandingCards == 0)
					continue;	// partner has no cards in this suit

				// add our own cards to the list of cards to consider
				int numOwnCards = m_pHand->GetSuit(nSuit).GetNumCards();
				for(int j=0;j<numOwnCards;j++)
				{
					outstandingCards << m_pHand->GetSuit(nSuit).GetAt(j);
					numOutstandingCards++;
				}

				// see if dummy has the top two cards in the suit
				if ((dummySuit.GetNumCards() >= 2) && 
					(*dummySuit[0] > *outstandingCards[0]) 
					 && (*dummySuit[1] > *outstandingCards[0]))  // NCR test against osC[0] vs [1]
				{
					status << "3PLYLDPRF12! Partner likes the " & STSS(nSuit) & 
							  " suit, but dummy holds the top two cards in the suit, so prefer to skip it.\n";
					continue;
				}
				
				// play a winner if we have one in the suit, else lead low
				if (suit.GetNumWinners() > 0)
					pLeadCard = suit[0];
				else
					pLeadCard = suit.GetBottomCard();
				status << "PLYLDPRF8! Partner has indicated a preference for the " & STS(nSuit) &
						  " suit, so lead the " & pLeadCard->GetName() & ".\n";
				return pLeadCard;
			}
		}
	}

	// nothing found
	return NULL;
}





//
// Get4thBestLeadCard()
//
CCard* CDefenderPlayEngine::Get4thBestLeadCard(int nMinLen)
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	CSuitHoldings& suit = m_pHand->GetSuit(m_pHand->GetSuitsByPreference(0));
	int nTrumpSuit = pDOC->GetTrumpSuit();
	CCard* pLeadCard = NULL;

	// see if we have 4+ cards in our best suit 
	if (suit.GetNumCards() >= nMinLen)
	{
		// make sure this is not the trump suit
		if (suit.GetSuit() != nTrumpSuit)
		{
			// lead 4th best of the suit
			pLeadCard = suit[3];
			status << "PLYLD4A! Lead the fourth card from our best suit (" & STS(suit.GetSuit())
					   & ") -- play the " & pLeadCard->GetName() & ".\n";
		}
		else
		{
			// hmm, our best suit is the trump suit -- see if we have another
			// 4-card suit
			int nOtherSuit = m_pHand->GetSuitsByPreference(1);
			if (m_pHand->GetNumCardsInSuit(nOtherSuit) >= nMinLen)
			{
				// lead from this suit
				pLeadCard = m_pHand->GetSuit(nOtherSuit).GetAt(3);
				status << "PLYLD4E! We don't want to lead from the trump suit, so lead the 4th best from the " &
						   STSS(nOtherSuit) & " suit -- the " & pLeadCard->GetName() & ".\n";
			}
			else
			{
				// no choice but to lead from the trump suit
				pLeadCard = suit[3];
				status << "PLYLD4G! We have no 4-card suits other than the trump suit, so lead the 4th best trump -- the " &
						   pLeadCard->GetName() & ".\n";
			}
		}
	}
	else
	{
		// lead 4th best of our longest suit
		int nLongSuit = m_pHand->GetLongestSuit();
		CSuitHoldings& longSuit = m_pHand->GetSuit(nLongSuit);
		if(longSuit.GetNumCards() >= nMinLen) {  // NCR test OK length
			ASSERT(longSuit.GetNumCards() >= 4);
			pLeadCard = longSuit[3];
			status << "PLYLD4N! Lead the fourth card from our longest suit (" & STS(nLongSuit) &
					  ") -- play the " & pLeadCard->GetName() & ".\n";
		}
	}
	//
	return pLeadCard;
}




//
// CashWinners()
//
// - lead a winner card if we have one
// - called _after_ the first round
//
CCard* CDefenderPlayEngine::CashWinners()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	int nTrumpSuit = pDOC->GetTrumpSuit();
	CCard* pLeadCard = NULL;

	// check to see if we even have any winners
	if (m_pHand->GetNumWinners() == 0)
		return NULL;

	// OK, we do have some winners -- check if we can cash winners
	BOOL bSuitIsSafe[4] = { TRUE, TRUE, TRUE, TRUE };
	//
	if (ISSUIT(nTrumpSuit))
	{
		// playing in a suit contract
		// check each suit
		for(int i=0;i<4;i++)
		{
			// skip if this suit is a loser, or it's the trump suit
			if ((m_pHand->GetSuit(i).GetNumTopCards() == 0) || (i == nTrumpSuit))
			{
				bSuitIsSafe[i] = FALSE;  // NCR added
				continue;
			}
			// here, we found a suit with top card winners
			// but check dummy's hand (OK since it's exposed by now)
//			int nDummy = pDOC->GetDummyPosition();
//			CPlayer* pPlayer = pDOC->GetDummyPlayer();
			ASSERT(pDOC->GetDummyPlayer()->AreCardsExposed());
			CHandHoldings& dummyHand = pDOC->GetDummyPlayer()->GetHand();
			if ((dummyHand.IsSuitVoid(i)) && (dummyHand.GetNumTrumps() > 0))
			{
				status << "3PLYLDWN2! We'd like to lead the " & STS(i) &
						  " suit, but dummy might ruff.\n";
				// may get ruffed in dummy
				bSuitIsSafe[i] = FALSE;
				continue;	// may get ruffed in dummy
			}

			// then see if there are still trumps outstanding, and
			// partner has shown out of trumps -- meaning that declarer 
			// still has some left, and declarer has shown out of the suit
			if ( (GetNumOutstandingCards(nTrumpSuit) > 0) &&
					(IsPartnerVoidInSuit(nTrumpSuit)) &&
						(m_ppGuessedHands[pDOC->GetDeclarerPosition()]->IsSuitShownOut(i)) )
			{
				status << "3PLYLDWN4! We'd like to lead the " & STS(i) &
						  " suit, but declarer might ruff.\n";
				// may get ruffed by declarer
				bSuitIsSafe[i] = FALSE;
				continue;  //NCR skip it
			}
			//NCR if we made it here, the suit is safe
		}
	}

	// here, we're determined which suits are safe to cash
	// so play the winners
	int nSuit;
	if (ISSUIT(nTrumpSuit))
		nSuit = GetNextSuit(nTrumpSuit);
	else
		nSuit = CLUBS;
	//
	int nMaybeSuit = NOSUIT; // NCR-267 to allow testing more than one suit
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<4;i++)  // thru 4 suits in order determined above
	{
		// find the first non-trump suit with winners
		// NCR ??? should indexes (i & nSuit) be the same. ie nSuit ???
		// NCR changed first index [i] to [nSuit]
		if (( bSuitIsSafe[nSuit]) && (m_pHand->GetSuit(nSuit).GetNumTopCards() > 0) ) 
		{
			if(nMaybeSuit == NOSUIT)
				nMaybeSuit = nSuit; // save
			else {
				if(m_pHand->GetSuit(nMaybeSuit).GetTopCardVal() == ACE)
					nMaybeSuit = nSuit;  // replace suit with Ace with this suit
			}
//NCR			break;  // exit loop, we've found a winner
		}
		// else advance to the next suit
		nSuit = GetNextSuit(nSuit);
	} // end for(i) thru suits
	
	// NCR-267 check if we found a suit
	if(nMaybeSuit != NOSUIT) {
		nSuit = nMaybeSuit;  // Set the suit
		i = 3; // reset for below
		// This could use the else to return vs following test
	}

	// make sure we found a suit!
	// if we didn't then we dont't have any top card winners
	if (i == 4)
		return NULL;

	// new code -- don't cash trump winners
	if (nSuit == nTrumpSuit)
		return NULL;

	//NCR make sure dummy can't trump this suit (has to be on LH)
	CHandHoldings& dummyHand = pDOC->GetDummyPlayer()->GetHand();
	if(ISSUIT(nTrumpSuit) && dummyHand.IsSuitVoid(nSuit) 
	   && (dummyHand.GetNumTrumps() > 0) && (m_pLHOpponent == pDOC->GetDummyPlayer()) )
	{
	   return NULL;   //NCR dummy can trump it
	}	

	//NCR make sure declarer can't trump this suit - check if declarer is void
	// declarer is void if there are no outstanding cards left
	if(ISSUIT(nTrumpSuit) && (GetNumOutstandingCards(nTrumpSuit) > 0) // NCR-597 any trump out?
	   && ((GetNumOutstandingCards(nSuit) == 0) 
	       || m_ppGuessedHands[pDOC->GetDeclarerPosition()]->IsSuitShownOut(nSuit)) )
	{
		return NULL;
	}

	// so play the card
	CSuitHoldings& suit = m_pHand->GetSuit(nSuit);
	// NCR-19 problem with leading bottom card - partner doesn't know who has top card!
	pLeadCard = suit.GetTopSequence().GetTopCard(); // NCR-19 .GetBottomCard();

	// NCR-217 Don't cash Ace if Dmy as RHO has King
//	if(dummyHand.GetSuit(nSuit).GetTopCard()->GetFaceValue() == KING)
//		return NULL;

	ASSERT(m_pHand->HasCard(pLeadCard));
	status << "PLCSW9! With no other good options, cash a winner in the " &
			  STSS(nSuit) & " suit, the " & pLeadCard->GetFaceName() & "\n";
	return pLeadCard;
}






//
// GetDiscard()
//
CCard* CDefenderPlayEngine::GetDiscard()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	CCard* pDiscard = NULL;

	//
	int nTrumpSuit = pDOC->GetTrumpSuit();
	CCard* pCardLed = pDOC->GetCurrentTrickCardByOrder(0);
	int nSuitLed = pCardLed->GetSuit();
	CHandHoldings& hand = *m_pHand;
	CSuitHoldings& suit = hand.GetSuit(nSuitLed);

	// try to use hi-lo signalling if playing third
	// i.e., indicate whether we like partner's lead
//	int nOrdinal = pDOC->GetNumCardsPlayedInRound();
	int nRound = pDOC->GetPlayRound();

	//
	if ((pDOC->GetRoundLead() == GetPartnerPosition()) && 
			(nSuitLed != nTrumpSuit) && 
					(m_pHand->GetNumCardsInSuit(nSuitLed) > 0) && 
								(m_nSuitSignalStatus[nSuitLed] <= 1))
	{

		// see if we like the suit
		if (m_nSuitSignalStatus[nSuitLed] == 0)
		{
			// give an encouraging sign if the suit is good (start the hi-lo)
			BOOL bSuitPreferred = FALSE;

			// a suit is good if it has a high honor (K+) with 2+ cards, 
			// OR has 1 card in a suit countract (i.e., can trump the 3rd card)
			if ((suit.GetNumCards() >= 3) && (suit[0]->GetFaceValue() >= KING))
			{
				status << "PLDIS2! Since we can win the next round of " & STS(nSuitLed) & 
	 					  ", we want to signal by discarding high in the suit.\n";
				bSuitPreferred = TRUE;
			}
/*
 * huh? with only 1 card in the suit, we have no choices
			if ((ISSUIT(nTrumpSuit)) && (m_pHand->GetNumCardsInSuit(nSuitLed) == 1) &&
										(m_pHand->GetNumTrumps() > 0))
			{
				status << "PLDIS4! Since we can ruff the next round of " & STS(nSuitLed) & 
	 					  ", we want to signal by discarding high in the suit.\n";
				bSuitPreferred = TRUE;
			}
*/

			// signal our preference for the suit
			if (bSuitPreferred)
			{
				// discard high (as high as possible) and set this as our signaled suit
				pDiscard = GetHighDiscard(suit);
				// NCR-194 test if discard > top card                ??? card led may NOT be top card ???
				if(pDiscard->GetFaceValue() > pCardLed->GetFaceValue())
				{
					if(suit.GetNumCardsBelow(pCardLed) > 2) // NCR-194 are there more than one
						pDiscard = suit.GetHighestCardBelow(pCardLed); // NCR-194 get highest to allow hi-lo
				}
				if (suit.GetNumCards() >= 2)
					status << "PLDIS8! Begin the high-low signal with the " & pDiscard->GetFaceName() & ".\n";
				m_nSignalledSuit = nSuitLed;
			}
			else
			{
				// the suit is not preferred, so discard low
				// NCR if the suit is a doubleton, signal hi-lo to indicate it
				// NCR does this need extra logic ???                                 // NCR-363 only hi-low first time
				if(suit.IsDoubleton() && (suit.GetTopCard()->GetFaceValue() < QUEEN)  && (GetNumOutstandingCards(nSuitLed, true) > 10)
					// NCR-213 Also don't want to discard a winner or stopper
					&& ((suit.GetNumMissingSequences() == 0) 
					    // NCR-363 Problem here if Dummy on left(plays next) has a singleton and it is > my top card
					    || (suit.GetTopCard()->GetFaceValue() 
						     < suit.GetTopMissingSequence().GetTopCard()->GetFaceValue()) ) )
					pDiscard = suit.GetTopCard();  // NCR top of doubleton
				// NCR-374 Play middle card if ...  Nullified by code for NCR-2 ???
				else if ((suit.GetLength() > 2) 
					     && (suit.GetTopCard() > pDOC->GetCurrentTrickHighCard()) ) //NCR-537  Only force if ... 
				{
					pDiscard = suit.GetSecondHighestCard();  // NCR-374 Q&D for now (Prevent cheap trick) 
				}
				else
					pDiscard = suit.GetBottomCard();
				status << "3PLDIS12! We do not have a winner in the " & STSS(nSuitLed) &
						  " suit, so discard low with the " & pDiscard->GetFaceName() & ".\n";
			}
			// set suit signalled status
			m_nSuitSignalStatus[nSuitLed] = 1;
		}
		else
		{
			// if this is the same suit as last time, continue the signal
			// with a low discard
			if ((nSuitLed == m_nSignalledSuit) && (m_nSuitSignalStatus[nSuitLed] == 1) &&
				(m_pHand->GetNumCardsInSuit(nSuitLed) >= 1))
			{
				pDiscard = suit.GetBottomCard();
				status << "PLDIS16! Complete the hi-lo signal by discarding the " & 
						  pDiscard->GetFaceName() & ".\n";
				// mark the suit signal complete
				m_nSuitSignalStatus[nSuitLed] = 2;
			}
		}

		// done
		if (pDiscard)
		{
			ASSERT(pDiscard->IsValid());
			ASSERT(hand.HasCard(pDiscard));
			return pDiscard;
		}
	}


	// if the opponents lead a suit for the first time & we're following suit, 
	// use distribution echo to signal odd/even number of cards in the suit
	CCardHoldings& origCards = m_pHand->GetInitialHand();
	int numOrigSuitCards = origCards.GetCardsOfSuit(nSuitLed);
	int numCards = suit.GetNumCards();
	if ( ((pDOC->GetRoundLeadPlayer() == m_pLHOpponent) || (pDOC->GetRoundLeadPlayer() == m_pRHOpponent)) &&
			(numOrigSuitCards >= 2) && (numCards > numOrigSuitCards-2) && !m_bSuitLed[nSuitLed])
	{
		// echo hi/lo with an even # of cards, or lo/hi with an odd number
		// exception: with trumps, echo hi/lo with odd, lo/hi with even
		BOOL bEvenCount = ((numOrigSuitCards %2 ) == 0);
		// find a decent high card to drop
		CCard* pHighCard = GetHighDiscard(suit);
		//
		CCard* pLowCard = suit[numCards-1];
		//
		if ((m_nEchoedSuit == NONE) && !m_bSuitEchoed[nSuitLed])
		{
			// start the echo
			if (bEvenCount)
			{
				// even count
				if (nSuitLed != nTrumpSuit)
				{
					// hi/lo if not trumps
					pDiscard = pHighCard;	// echo high w/ 2nd lowest card
					status << "PLDIS20! The opponents are attacking the " & STS(nSuitLed) &
							  "; start the hi/lo echo with the " & pDiscard->GetFaceName() & 
							  " to tell partner we have an even number of cards in the suit.\n";
				}
				else
				{
					// trumps, so lo/hi
					pDiscard = pLowCard;	// echo low w/ lowest card
					status << "PLDIS21! The opponents are pulling trumps, so start a lo/hi echo with the " &
							  pDiscard->GetFaceName() & " to tell partner we have an even number of trumps.\n";
				}
			}
			else
			{
				// odd count
				if (nSuitLed != nTrumpSuit)
				{
					// lo/hi if not trump
					pDiscard = pLowCard;	// echo high w/ 2nd lowest card
					status << "PLDIS22! The opponents are attacking the " & STS(nSuitLed) &
							  "; start the lo/hi echo with the " & pDiscard->GetFaceName() & 
							  " to tell partner we have an odd number of cards in the suit.\n";
				}
				else
				{
					// hi/lo if trump
					pDiscard = pHighCard;	// echo low w/ lowest card
					status << "PLDIS23! The opponents are pulling trumps, so start a hi/lo echo with the " &
							  pDiscard->GetFaceName() & " to tell partner we have an odd number of trumps.\n";
				}
			}
			//
			m_nEchoedSuit = nSuitLed;
			m_bSuitEchoed[nSuitLed] = TRUE;
		}
		else if (nSuitLed == m_nEchoedSuit)
		{
			// we've echoed once in the suit, so echo again and clear state
			// start the echo
			if (bEvenCount)
			{
				// even count
				if (nSuitLed != nTrumpSuit)
				{
					// finish with the lo card if not trump
					pDiscard = pLowCard;	// complete the hi/low echo
					status << "PLDIS25! Complete the hi/lo distributional echo with the " & pDiscard->GetFaceName() & ".\n";
				}
				else
				{
					// finish with the hi card if not trump
					pDiscard = pHighCard;	// complete the low/hi echo
					status << "PLDIS26! Complete the lo/hi distributional echo with the " & pDiscard->GetFaceName() & ".\n";
				}
			}
			else
			{
				// odd count
				if (nSuitLed != nTrumpSuit)
				{
					// finish with the hi card if not trump
					pDiscard = pHighCard;	// complete the lo/hi echo
					status << "PLDIS27! Complete the lo/hi distributional echo with the " & pDiscard->GetFaceName() & ".\n";
				}
				else
				{
					// finish with the low card if trump
					pDiscard = pLowCard;	// complete the hi/lo echo
					status << "PLDIS28! Complete the hi/lo distributional echo with the " & pDiscard->GetFaceName() & ".\n";
				}
			}
			// clear state
			m_nEchoedSuit = NONE;
		}
		else
		{
			// opponents changed suits, so clear the echo state
			m_nEchoedSuit = NONE;
		}

		// done
		if (pDiscard)
		{
			ASSERT(pDiscard->IsValid());
			ASSERT(hand.HasCard(pDiscard));
			return pDiscard;
		}
	}



	// if we can't follow suit and are not trumping, indicate suit preference
	// with an "unusual discard" (but only in the first 7 tricks)
	if ( (m_pHand->GetNumCardsInSuit(nSuitLed) == 0) && 
				(m_pHand->GetNumTrumps() == 0) && (nRound <= 6))
	{
		// throw a high card from our favorite suit if it has winners
		int nPrefSuit = m_pHand->GetPreferredSuit();
		CSuitHoldings& prefSuit = m_pHand->GetSuit(nPrefSuit);
		// NCR be sure honors are protected after discard: King needs +1 and Queen needs +2
		int nNumInSuit = prefSuit.GetNumCards();
		int nTopCardFV = prefSuit.GetTopCard()->GetFaceValue();

		if ((prefSuit.GetNumWinners() > 0) 
			// NCR if top of suit is honor, be sure it is protected
			// NCR ??? need test for second highest card vs just King as Ace could have been played
			&& ((nTopCardFV < QUEEN) || (((nTopCardFV == QUEEN) && (nNumInSuit >= 4)) // discard will leave +2
										 || ((nTopCardFV == KING) && (nNumInSuit >=3))
										 || ((nTopCardFV == ACE)))) )
		{
			if (prefSuit.GetNumSequences() > 1)
			{
				// see if the top card of the second sequence is higher than a 6 and less than 10 NCR
				if ((prefSuit.GetSecondSequence().GetTopCard()->GetFaceValue() > 6)
					&& (prefSuit.GetSecondSequence().GetTopCard()->GetFaceValue() < 10)) //NCR 
				{
					// if so, return the top card of the second sequence
					pDiscard = prefSuit.GetSecondSequence().GetTopCard();
				}
				else
				{
					// else just return the bottom card of the top sequence
					//NCR Test if the sequence has more than 1 card
					if(prefSuit.GetSecondSequence().GetNumCards() > 1) {  //NCR
					   pDiscard = prefSuit.GetSecondSequence().GetBottomCard(); // NCR which Sequence???
					}else{
						//NCR If only 1 card, return second lowest in suit
						pDiscard = prefSuit.GetSecondLowestCard();//NCR
					}
				}
			}
			else
			{
				// there's only one sequence in the suit, so return the bottom card
				pDiscard = prefSuit.GetBottomCard();
			}
			//NCR Do NOT discard an Ace, King or Queen
			if (pDiscard && pDiscard->GetFaceValue() < QUEEN) 
			{  
				// NCR may not have found a discard yet!!!
				ASSERT(pDiscard->IsValid());  //NCR
				ASSERT(hand.HasCard(pDiscard)); //NCR
				status << "PLDIS40! Indicate a preference for the " & STSS(nPrefSuit) &
					  " by discarding high with the " & pDiscard->GetName() & ".\n";
				return pDiscard; // NCR return the chosen card
			}
		} // end number of winners > 0
	}



	//
	// else use default behavior
	//
	//
/*
	//
	// first see if we have priority discards
	//
	if (m_priorityDiscards.GetNumCards() > 0)
	{
		// see if we're void in the suit led
		if (m_pHand->GetNumCardsInSuit(nSuitLed) == 0)
		{
			// return the first card in the discard list
			CCard* pCard = m_priorityDiscards[0];
			status << "PLDIS50! Use this opportunity to discard the " & pCard->GetName() & ".\n";
			return pCard;
		}
		else
		{
			// else see if there's a card of the suit led that we want to discard
			int numCards = m_priorityDiscards.GetNumCards();
			for(int i=0;i<numCards;i++)
			{
				if (m_priorityDiscards[i]->GetSuit() == nSuitLed)
				{
					// return the first card in the discard list
					CCard* pCard = m_priorityDiscards[i];
					CSuitHoldings& suit = m_pHand->GetSuit(pCard->GetSuit());
					if (suit.GetNumCards() > 1)
						status << "PLDIS51! Use this opportunity to discard the " & pCard->GetName() & ".\n";
					return pCard;
				}
			}
		}
	}
*/

	// NCR-154 Special case where 2 cards left, dummy leading and we have card in
	if((nRound == 11) && (pDOC->GetDummyPlayer() == pDOC->GetRoundLeadPlayer())
		&& (hand.GetNumCardsInSuit(nSuitLed) == 0))
	{
		// If dummy is leading, keep what he has
		CHandHoldings& testHand = GetDummyHand();
		// NEED LOGIC here to find who is leader ???
		Suit suitD; // NCR-FFS added here, removed below
		for(/*int*/ suitD = CLUBS; suitD <= SPADES; GETNEXTSUIT(suitD)) 
		{
			if(!testHand.GetSuit(suitD).IsVoid())
				break;  // found suit of dummy's card 
		}
		// Discard other than what dummy has
		for(Suit suitT = CLUBS; suitT < SPADES; GETNEXTSUIT(suitT))
		{
			if(suitT == nTrumpSuit)
				continue; // NCR-510 Skip if trump suit
			CSuitHoldings& theSuit = hand.GetSuit(suitT); 
			if(theSuit.IsVoid())
				continue;
			// have cards in this suit
			if(theSuit.GetNumCards() == 2)
				break;  // no choice, have to play one of these
			if(suitT == suitD) 
				continue;  // don't discard if dummy has
			pDiscard = theSuit.GetBottomCard();
			return pDiscard;

		} // end for(suitT)
	} // NCR-154 end last two cards

	//
	// else we have no priority discards, so see what we should discard
	//
	CSuitHoldings* pDiscardSuit = NULL;

	// see if we have cards in the suit led
	if (suit.GetNumCards() > 0)
	{
		// we have no choice but to follow suit, so return the lowest card 
		pDiscard = suit.GetBottomCard();
		return pDiscard;
	}

	// here we have no more cards in the suit, so discard from another suit
	// but first see whether we're playing with a trump suit (and have trumps left)
	if (ISSUIT(nTrumpSuit) && (hand.GetNumCardsInSuit(nTrumpSuit) > 0))
	{
		// yes indeed, we do have at least one trump left (but not trumping here)
		// so return the shortest suit with losers in it
		// first find the shortest suit with no winners, if there is one
		int i; // NCR-FFS added here, removed below
		for(/*int*/ i=3;i>=0;i--)
		{
			int nSuit = hand.GetSuitsByLength(i);
			CSuitHoldings& testSuit = hand.GetSuit(nSuit);
			// special code -- try to protect high honors, even if not sure winners
			if ( (testSuit.HasKing()) || (testSuit.HasQueen() && (testSuit.GetNumCards() >= 2)) )
				 continue;
			// else discard from the suit if it has no winners
			if ((nSuit != nTrumpSuit) && (testSuit.GetNumCards() > 0) && 
								(testSuit.GetNumWinners() == 0))
			{
				pDiscardSuit = &testSuit;
				break;
			}
		}

		// see if we found one
		if (pDiscardSuit == NULL)
		{
			// here, each of the remaining suits has at least one winner, 
			// or else have no cards in them
			// so just pick the shortest non-void, non-trump suit that has 
			// at least one loser in it
			for(i=3;i>=0;i--)
			{
				int nSuit = hand.GetSuitsByLength(i);
				int nSuitLength = hand.GetNumCardsInSuit(nSuit);
				int numLosers = hand.GetNumLosersInSuit(nSuit);
    			CSuitHoldings& testSuit = hand.GetSuit(nSuit);
				// NCR ??? Need test if second highest card vs King ie if Ace played, K doesn't need guard
				if ( (testSuit.HasKing() && testSuit.GetNumCards() <= 2) 
					|| (testSuit.HasQueen() && (testSuit.GetNumCards() <= 3)) )
						continue;  // NCR skip this suit

				if ((nSuitLength > 0) && (numLosers > 0) && (nSuit != nTrumpSuit))
				{
					pDiscardSuit = &hand.GetSuit(nSuit);
					break;
				}
			}
		}

		// see if we still failed to find a suit
		if (pDiscardSuit == NULL)
		{
			// no suit found -- meaning that there are no suits with losers,
			// or that the only suit left is the trump suit
			if (hand.GetNumCards() == (hand.GetNumTrumps()))
			{
				// no cards outside the trump suit? then we have no
				// choice -- "discard" a trump
				pDiscardSuit = &hand.GetSuit(nTrumpSuit);
			}
			else
			{
				// all suits have only winners, so pick the shortest non-trump suit
				for(int i=3;i>=0;i--)
				{
					int nSuit = hand.GetSuitsByLength(i);
					CSuitHoldings& testSuit = hand.GetSuit(nSuit);
					// special code -- try to protect high honors, even if not sure winners
					// NCR ??? Need test if second highest card vs King ie if Ace played, K doesn't need guard
					if ( (testSuit.HasKing() && testSuit.GetNumCards() <= 2) 
						|| (testSuit.HasQueen() && (testSuit.GetNumCards() <= 3)) )
							continue;  // NCR skip this suit

					if ((hand.GetNumCardsInSuit(nSuit) > 0) && (nSuit != nTrumpSuit))
					{
						pDiscardSuit = &hand.GetSuit(nSuit);
						break;
					}
				}
			}
		}
		// NCR Final test - use longest suit
		
		if(pDiscardSuit == NULL)          // NCR-336 Not a trump please
		{
			int theSuit = hand.GetLongestSuit(SUIT_NOTTRUMP);
			if (theSuit == NOSUIT)  // NCR-336 if no suit found
				theSuit = nTrumpSuit; // then must use trump
			pDiscardSuit = &hand.GetSuit(theSuit);  // Have to do something
		}

		// return the bottom card of the discard suit
		pDiscard = pDiscardSuit->GetBottomCard();

	}
	else
	{
		// playing in a NT contract, OR we're out of trumps
		// new discard routine -- discard from the worst suits
		// playing no trumps -- generally return the bottom card of the worst suit

		// sort the suits by losers in the partcular hand
		int nSuitsByLosers[4] = { CLUBS, DIAMONDS, HEARTS, SPADES, };
		int i; // NCR-FFS added here, removed below
		for(/*int*/ i=0;i<4;i++)
		{
			for(int j=0;j<3;j++)
			{
				CSuitHoldings& suit1 = hand.GetSuit(nSuitsByLosers[j]);
				CSuitHoldings& suit2 = hand.GetSuit(nSuitsByLosers[j+1]);
				int numWinners1 = suit1.GetNumWinners();
				int numWinners2 = suit2.GetNumWinners();
				int numLosers1 = suit1.GetNumLosers();
				int numLosers2 = suit2.GetNumLosers();

				// comparisons used for selecting a discard suit
				// 1: if a suit has more losers than another suit, with the same or fewer winners
				// 2: if both have the same # of losers, but a suit has fewer winners
				// 3: if a suit has no winners and the other one does
				if ( ((numLosers2 > numLosers1) && (numWinners2 <= numWinners1)) ||
					 ((numLosers1 == numLosers2) && (numWinners1 > numWinners2)) ||
					 ((numWinners1 > 0) && (numWinners2 == 0)) )
				{
					// move the first suit back by one (i.e., swap the suits)
					// and favor the second suit when discarding
					int nTemp = nSuitsByLosers[j];
					nSuitsByLosers[j] = nSuitsByLosers[j+1];
					nSuitsByLosers[j+1] = nTemp;
				}
			}
		}

		// look through the sorted list of suits
		CSuitHoldings* pMaybeSuit = NULL;  // NCR save possible suit
		for(i=0;i<4;i++)
		{
			// check out the suit
			const int nSuit = nSuitsByLosers[i];
			if (hand.GetNumCardsInSuit(nSuit) == 0)
				continue;

			CSuitHoldings& suit = hand.GetSuit(nSuit);
			int numLosers = suit.GetNumLosers();
			CCard* pTopOutstandingCard = GetHighestOutstandingCard(nSuit, true);
			if(pTopOutstandingCard == NULL) 
			{
				// NCR if no outstanding cards in this suit, then is this suit an orphan?
				if(hand.GetNumCards() < 5)  // NCR arbitrary test for nmbr cards left in hand
					pDiscardSuit = &hand.GetSuit(nSuit);  // NCR this eligible for discard
				pTopOutstandingCard = deck.m_pNoCard;  // NCR vs NULL tests???
			}
			// NCR skip suit if singleton and our card is top card
			if(suit.IsSingleton() && (suit.GetTopCard()->GetFaceValue() > pTopOutstandingCard->GetFaceValue()) // NCR-28
				&& (GetNumOutstandingCards(nSuit, true) > 1))  // NCR how many outstanding??? include dummy
				continue;				// Skip this suit
			// NCR skip suit if there is an honor that needs protection
			const int numCards = suit.GetNumCards();
			const int topCardFV = suit.GetTopCard()->GetFaceValue();
			if(((topCardFV == KING) && (*pTopOutstandingCard > KING) 
				  && (numCards == 2)) // King needs 1
				|| ((topCardFV == QUEEN) && (*pTopOutstandingCard > QUEEN) 
				    && (numCards <= 3))// Queen needs 2
                || (suit.GetNumMissingAbove(topCardFV) < (numCards+1)) // need one extra card to guard
			     // but see if this is the priority suit, or the suit has zero losers
				 // NCR Should prioritySuit have more cards than losers???
				|| (ISSUIT(nSuit) && ((nSuit == m_nPrioritySuit) && (numCards > numLosers)))
				|| (numLosers == 0)  //NCR what if hand has no entries???
				|| (topCardFV <= TEN) // NCR-105 all small cards (or no cards lower???)
				|| (numCards > 4) // NCR long suit is a maybe
			   )
			{
//???				if((numLosers == 0) && (suit.GetNumWinners() > 1)) 
				// NCR-81 discard this singleton if its a loser
				// NCR-233 Don't discard a singleton King or Queen
				if((numLosers == numCards) && (((numCards == 1) && (topCardFV < QUEEN)) || (topCardFV <= TEN)
					                              // NCR-238 Discard if lots of cards
					                           || (numCards > 4)) )  
				{
					pDiscardSuit = &suit;  // NCR use this if lots of losers
					break;				// use this one
				}
				else
				{
					// NCR-107 Save longer suit if a choice
					if((pMaybeSuit != NULL) 
					   && ((pMaybeSuit->GetNumCards() < numCards)
					          // NCR-141 Change maybeSuit if this suit's top cards are guarded
					       || ((suit.GetNumMissingAbove(topCardFV) < (numCards-1))
						       // NCR-156 not if suit is all winners???
							   && (suit.GetNumWinners() != numCards))
					           // NCR-118 if equal length, use the stopped one
							   // NCR-280 problem if suit stopped now, discarding a guard will unstop it
					       || ((pMaybeSuit->GetNumCards() == numCards) 
						       && (suit.GetNumMissingAbove(topCardFV) < MAX(1,numCards-2)) /*suit.IsSuitStopped()*/)) )
					{
						pMaybeSuit = &suit;  // NCR save as possible, if nothing better
					}
					else if (pMaybeSuit == NULL) 
					{
						pMaybeSuit = &suit; // NCR-118 save this one ???
					}
					// don't discard from this suit if possible;
					continue;
				}
			}

			// test this next suit for winners
//			CSuitHoldings& testSuit = hand.GetSuit(nSuit); // ??? Is this the same as suit???
//			ASSERT(suit == testSuit); // ???
//			numLosers = testSuit.GetNumLosers();
			// NCR don't discard a singleton K ???
			if (((numLosers > 0) && !(suit.IsSingleton() && (topCardFV == KING))
				// NCR-26 Don't discard if has honors and there is a maybe suit
				 && ((suit.GetNumSecondaryHonors() < 2) && (pMaybeSuit == NULL)))
                // NCR-35 discard if all losers and low valued 
                || ((numLosers == numCards) && ((numCards == 1) // NCR-81 discard if only card is a loser
				                               || (suit.GetTopMissingSequence().GetNumCardsAbove(suit.GetTopCard()) > 2)))  
                // NCR-41 discard lower cards under a top card
                || ((suit.GetNumSequence2s() > 1) 
				    && (suit.GetSequence2(1).GetTopCard()->GetFaceValue() < TEN)) // NCR-41 Hardcoded 10?
			   )
			{
				pDiscardSuit = &suit; //hand.GetSuit(nSuit);
				break;	   // found one - exit loop
			}
		}  // end for(i) thru suits by losers
		

		if(pDiscardSuit == NULL)
			pDiscardSuit = pMaybeSuit;  // NCR just in case something else found

		// if we still found nothing, call the base class
		if (pDiscardSuit == NULL)
			pDiscardSuit = &hand.GetSuit(PickFinalDiscardSuit(hand));

		// and get the card
		pDiscard = pDiscardSuit->GetBottomCard();
		// NCR don't discard a loser that is protecting an honor???
	} // end No Trump or out of trumps

	//
	// verify the selected card is OK
	//
	ASSERT(pDiscard->IsValid());
	ASSERT(hand.HasCard(pDiscard));
	return pDiscard;
} // end GetDiscard()




//
//  ()
//
// select a discard that's high but not too high
// 
CCard* CDefenderPlayEngine::GetHighDiscard(CSuitHoldings& suit)
{ 
	// first see if the suit has a lot of high cards
	CCard* pHighCard = NULL;
	int numHighCards = suit.GetNumCardsAbove(6);  // NCR moved outside if
	if (numHighCards > 1)
	{
		// multiple high cards -- see if we have one that's not a winner
		int numLikelyWinners = suit.GetNumLikelyWinners();
		if (numHighCards > numLikelyWinners)
		{
			// we have excess high cards
			pHighCard = suit.GetLowestCardAbove(6);
			// NCR-398 Don't discard a JACK and up
			if(pHighCard->GetFaceValue() >= JACK) 
			{
				CCard* pCard = suit.GetHighestCardBelow(pHighCard);
				if(pCard != NULL) pHighCard = pCard;  // NCR-398 get a lower one
			}
		}
		else
		{
			// nothing but likely winners! so return something lower if possible
			pHighCard = suit.GetHighestCardBelow(TEN);
			if (pHighCard == NULL)
				pHighCard = suit.GetBottomCard();
		}
	}
	else
	{
		// no high cards, so return the highest of the low face cards
		pHighCard = suit.GetHighestCardBelow(7);
	}

	// 
	return pHighCard;
}





//
// PlayCard()
//
// select a card to play
// 
CCard* CDefenderPlayEngine::PlayCard()
{ 
	CPlayerStatusDialog& status = *m_pStatusDlg;
	AssessPosition();

	// see if we're using GIB
	CCard* pCard;
/*
	if (m_bUsingGIB)
	{
		// call GIB
		CGIB& gib = *(theApp.GetGIB());
		status << "GIB! Invoking GIB for the play...\n";
		int nPlay = gib.Invoke(m_pPlayer, m_pHand, m_pPartnersHand, m_pStatusDlg);
		// find and play the card
		if (ISDECKVAL(nPlay))
			pCard = m_pHand->FindCardByDeckValue(nPlay);
		if (pCard)
		{
			status << "GIB! GIB plays the " & pCard->GetName() & ".\n";
			VERIFY(pCard->IsValid());
			return pCard;
		}
		else
		{
			status << "GIBX! GIB failed to return a valid play; defaulting to standard play engine.\n";
		}
	}
*/

	// else we're not using GIB for defender play
	// see if this is the first round,
	if (m_nRound == 0)
	{
		if (m_bLHDefender)
		{
			status << "2PL02! Starting play as left-hand defender.\n";
			// default routine
			return CPlayEngine::PlayCard();	
		}
		else
		{
			status << "2PL03! Starting play as right-hand defender.\n";
			// default routine
			return CPlayEngine::PlayCard();	
		}
	}

	//
	// else it's not the first round
	// default routine
	pCard = CPlayEngine::PlayCard();	
	// do some sanity checks
	ASSERT(pCard->IsValid());
	ASSERT(m_pHand->HasCard(pCard));
	//
	return pCard;
}





//
// PlayFirst()
//
CCard* CDefenderPlayEngine::PlayFirst()
{
	return CPlayEngine::PlayFirst();
/*
	CPlayerStatusDialog& status = *m_pStatusDlg;
	status << "PLAY1! Leading card...\n";

	// lead a random card
	return (*m_pHand)[0];	
*/
}



//
// PlaySecond()
//
// - play second hand as defender
//
CCard* CDefenderPlayEngine::PlaySecond()
{
//	return CPlayEngine::PlaySecond();

	CPlayerStatusDialog& status = *m_pStatusDlg;
	status << "3PLAYDF2! Playing second as defender, using default player logic.\n";

	// get play info
	const int nDeclarerPos = pDOC->GetDeclarerPosition();
//	int nDummyPos = pDOC->GetDummyPosition();
	CPlayer* const pDummy = pDOC->GetDummyPlayer();
	CCard* const pCardLed = pDOC->GetCurrentTrickCardLed();
	const int nSuitLed = pCardLed->GetSuit();
//	int nFaceValue = pCardLed->GetFaceValue();
	CCard* const pCurrTopCard = pDOC->GetCurrentTrickHighCard();
	const int nTrumpSuit = pDOC->GetTrumpSuit();
	CSuitHoldings& suit = m_pHand->GetSuit(nSuitLed);
	// card to play
	CCard* pCard = NULL;

	// determine the top card -- it's either the one led by declarer 
	// or dummy's top holding in the suit, if dummy is to our left
	CCard* pTopCard;
	CHandHoldings& dummyHand = GetDummyHand();
	CSuitHoldings& dummySuit = dummyHand.GetSuit(nSuitLed);
	const int numDummySuitCards = dummySuit.GetNumCards();
	if ((m_pLHOpponent == pDummy) && (numDummySuitCards > 0) && (*dummySuit[0] > *pCardLed))
		pTopCard = dummySuit[0];
	else
		pTopCard = pCardLed;

	//
	// we play second hand low, usually but not always
	// see how many cards we have in the suit
	//
	const int numCardsInSuitLed = suit.GetNumCards();
	if (numCardsInSuitLed == 1)
	{
		// play the only card we have in the suit
		pCard = suit[0];
		status << "PLDF2A! Play our only card in the suit, the " & pCard->GetFaceName() & ".\n";
		ASSERT(pCard->IsValid());
		return pCard;
	}
	else if (numCardsInSuitLed >= 2)
	{
		// NCR check if Slam bid or if contract booked (ie need one trick to set them)
		if ((pDOC->GetContractLevel() >= 6) 
                // NCR-49 changed below from == to <= to allow for more tricks?   
			|| (m_numRequiredTricks <= pDOC->GetNumTricksWonByTeam(m_pPlayer->GetTeam())+1)) // NCR or booked
		{
			// if we have the top outstanding card, play it; or if we can beat dummy on the left
			bool bCkDmy = (m_pLHOpponent == pDummy); // NCR-302 Only consider dummy if on left
			CCard* pTopOutstandingCard = GetHighestOutstandingCard(nSuitLed, bCkDmy);
			if(pTopOutstandingCard == NULL) pTopOutstandingCard = deck.m_pNoCard; //NCR vs NULL
			  // NCR don't play our top card if it we know it doesn't beat current top card
			if((suit.GetTopCard()->GetFaceValue() > pCurrTopCard->GetFaceValue())
				&& ((suit.GetTopCard()->GetFaceValue() > pTopOutstandingCard->GetFaceValue()) // NCR-28
				    // NCR or if dummy plays next and our top card beats his top card
			        || ((m_pLHOpponent == pDummy) && (numDummySuitCards > 0) 
				        && (suit.GetTopCard()->GetFaceValue() > dummySuit[0]->GetFaceValue())) 
				    // NCR or if dummy on right and has only cards that beat ours; ie we have Q and he has AK
			        || ((m_pRHOpponent == pDummy) && (numDummySuitCards > 0) 
			            && (dummySuit.GetTopSequence().GetBottomCard()->GetFaceValue() 
				            == suit.GetTopCard()->GetFaceValue()+1))) ) 
			{
				CCard* pCardToBeat = pCurrTopCard;  // NCR-117 Save ptr to card to beat
				// NCR-49 If Dummy plays next, beat his top card vs using our top card
				if((m_pLHOpponent == pDummy) && (numDummySuitCards > 0)
				   // NCR-117 Also need to beat current top card
				   && (dummySuit[0]->GetFaceValue() > pCurrTopCard->GetFaceValue()) )
				{
				    pCardToBeat = dummySuit[0];  // change card to beat
				} // NCR-163 play to win if declarer next
				else  if((m_pRHOpponent == pDummy)  // Declarer next player
					     && (pTopOutstandingCard != deck.m_pNoCard) // test NOT null card
						 && (pTopOutstandingCard->GetFaceValue() > pCardToBeat->GetFaceValue())) // NCR-602
				{
					pCardToBeat = pTopOutstandingCard; // NCR-163 declarer could have top card
				}
				pCard = suit.GetLowestCardAbove(pCardToBeat); // NCR-49 & NCR-117 
				if(pCard == NULL)
					pCard = suit.GetTopCard();
		        status << "PLDF2A1! Play a high card in the suit, the " & pCard->GetFaceName() & ".\n";
 				return pCard;
			}
			// NCR test if we can promote a winner - if our top sequence has 2+ cards and there is only
			// one card out greater, play bottom card of sequence
			CCardList outstandingCards;
			GetOutstandingCards(nSuitLed, outstandingCards, true); // include dummy's cards
			if((suit.GetTopSequence().GetNumCards() >= 2) 
				&& (outstandingCards.GetNumCardsAbove(suit.GetTopCard()) == 1))
			{
				pCard = suit.GetTopSequence().GetBottomCard();
		        status << "PLDF2A2! Play high card in the suit, the " & pCard->GetFaceName() 
					     & " to promote neighbor.\n";
 				return pCard;
			} // NCR end promoting
		} // end special code for booked contract

		// we have multiple cards in the suit led
		// try to play low, unless there are mitigating circumstances

		// one such circumstance is in Notrump, or in a suit contract 
		// playing ahead of dummy, where dummy has no trumps, and partner 
		// has no more cards in the suit and we have a winner in our hand
		CGuessedSuitHoldings& partnerSuit = m_ppGuessedHands[m_pPartner->GetPosition()]->GetSuit(nSuitLed);
		if (partnerSuit.IsSuitShownOut() && (m_pLHOpponent == pDummy) &&
			(!ISSUIT(nTrumpSuit) || (dummyHand.GetNumTrumps() == 0)) )
		{
			// in this case, play high if possible
			// see if we have cards higher than the top card
			if (suit.GetNumCardsAbove(pTopCard) > 0)
			{
				CCard* pCard = suit.GetLowestCardAbove(pTopCard);
				if ((nTrumpSuit != NOTRUMP) && (dummyHand.GetNumTrumps() == 0))
				{
					// suit play, and dummy is out of trumps
					status << "PLDF2B! Partner is out of " & STS(nSuitLed) & 
							  ", and dummy on our left is out of trumps, so we can play high in second position with the " & pCard->GetFaceName() & ".\n";
				}
				else
				{
					// NT play
					status << "PLDF2B1! Partner is out of " & STS(nSuitLed) & 
							  ", so play high in second position to win with the " & pCard->GetFaceName() & ".\n";
				}
				ASSERT(pCard->IsValid());
				return pCard;
			}
		}

		// if the opponents lead an honor, cover it if possible
		if ((pCardLed->GetFaceValue() >= TEN) && (*suit[0] > *pCardLed) )
		{
			// NCR test if our cards "bracket" the card led - if so cover it
			if((pCard = suit.BeatIfBracketed(pCardLed)) != NULL) 
			{
				status << "PLDF2E2! " & szRHO & " led an honor (the " & pCardLed->GetFaceName() & 
						  "), our cards surround it, so play " & pCard->GetFaceName() & ".\n";
				return pCard;
			} // NCR end bracketing cards test

			// NCR-75 Don't cover if our card might win later. We covered above if the card was bracketed. 
			// Don't cover if: Dmy on RH now void AND (trumps lead OR dummy void in trumps) - guard against dummy ruffing next lead
			//                 AND our top card is guarded (eg AK out and we have Qxxx - need extra to be played here)
			CCardList outstandingCards;
			GetOutstandingCards(nSuitLed, outstandingCards, true); // include dummy's cards

			if((m_pRHOpponent == pDummy) && dummySuit.IsVoid() && ((nSuitLed == nTrumpSuit) ||(dummyHand.GetNumTrumps() == 0))
				&& (outstandingCards.GetNumCardsAbove(suit.GetTopCard()) < (suit.GetNumCards()-1)) )  // Qxx is guarded against AK
			{
				pCard = suit.GetBottomCard();
				status << "PLDF2E3! " & szRHO & " led an honor (the " & pCardLed->GetFaceName() & 
						  "), we want to save the " &suit.GetTopCard()->GetFaceName() & ", so play the " 
						  & pCard->GetFaceName() & ".\n";
				return pCard;
			} // NCR-75 end not covering with a possible later winner

			//NCR don't always cover if: dummy on left and dummy has bigger card
			if ((m_pLHOpponent == pDummy) 
				&& (!dummySuit.IsVoid() && (dummySuit.GetTopCard()->GetFaceValue() > suit.GetTopSequence().GetBottomCard()->GetFaceValue()) // NCR-28
					|| (dummySuit.IsVoid() && ISSUIT(nTrumpSuit) && dummyHand.GetNumTrumps() > 0)
				   )
				&& ((suit.GetNumCards() > dummySuit.GetNumCards())
					|| (suit.GetNumCards() >= 3 && !OneIn3())
					|| (suit.GetNumCards() == 2 && !OneIn2())
				   )
                // NCR-48 Don't play low if partner is out and dummy is out
				&& !(partnerSuit.IsSuitShownOut() && dummySuit.IsVoid())
				// NCR-651 Cover if have 2 in sequence
				&& (suit.GetTopSequence().GetNumCards() < 2)
			   )
			{
				// follow with a low card
				pCard = suit.GetBottomCard();
				ASSERT(pCard->IsValid());
				status << "PLDF2E1! " & szRHO & " led an honor (the " & pCardLed->GetFaceName() & 
						  "), we may be being finessed, so play " & pCard->GetFaceName() & ".\n";
				return pCard;
			}  // NCR end changing covering of Honor
			else
			{
				//NCR if dummy plays next and he has top card, beat it we can
				if((m_pLHOpponent == pDummy)  && !dummySuit.IsVoid() && (pTopCard == dummySuit[0])
					// NCR-652 beat top card if we can
					&& (suit.GetTopCard()->GetFaceValue() > pTopCard->GetFaceValue())) // NCR-652
				{
					pCard = suit.GetLowestCardAbove(pTopCard);
				} 
				else 
				{
					pCard = suit.GetLowestCardAbove(pCardLed); // NCR beat it
				}
				if(pCard == NULL)  // should not happen
					pCard = suit.GetBottomCard();
				status << "PLDF2E! " & szRHO & " led an honor (the " & pCardLed->GetFaceName() & 
						  "), so cover it with our " & pCard->GetFaceName() & ".\n";
				ASSERT(pCard->IsValid());
				return pCard;
			}
		}
		//NCR if playing after dummy, have 2 in suit with sure winner if played now
		// but if left as singleton, it could be captured next lead
		// Is dummy's cards the only ones higher than our card?
		if ((numCardsInSuitLed == 2) && (m_pRHOpponent == pDummy)) 
		{
			if(!dummySuit.IsVoid() && (dummySuit.GetTopCard()->GetFaceValue() > suit.GetTopCard()->GetFaceValue()) // NCR-28 here and below
				&& (suit.GetTopCard()->GetFaceValue() > pCurrTopCard->GetFaceValue())) // NCR make sure we'll win
			{
				// Do it randomly 2 out of 3 times ???
				if(!OneIn3()) 
				{
					pCard = suit.GetTopCard();
					status << "PLDF2F1! Play the " & pCard->GetFaceName() & 
					          " to prevent it from being captured next lead.\n";
					ASSERT(pCard->IsValid());
					return pCard;
				}
			}
        }  // NCR end test if winner now

		// play the bottom card of an honor sequence to promote the other honors
		// the top card of our top sequence must be higher than the card led
		// (but lower than the top card in dummy)
		CCardList& topSeq = suit.GetTopSequence();
		if (m_pLHOpponent == pDummy)
		{
			// playing ahead of dummy -- play the bottom honor of the sequence
			if ((topSeq.GetNumCards() > 1) && IsHonor(topSeq[0]->GetFaceValue()) && (*topSeq[0] > *pCardLed)
				&& (*topSeq[0] < *pTopCard) && !dummySuit.IsSingleton()) // NCR-384 can't force against singleton
			{
				pCard = topSeq.GetBottomCard();
				status << "PLDF2F! Play the " & pCard->GetFaceName() & 
						  " to force a higher card from dummy and promote the other top cards in our hand.\n";
				ASSERT(pCard->IsValid());
				return pCard;
			}
		}
		else
		{
/*
			// not playing ahead of dummy -- does this strategy still apply???
			if ((topSeq.GetNumCards() > 1) && IsHonor(topSeq[0]) && (*topSeq[0] > *pCardLed))
			{
				pCard = topSeq.GetBottomCard();
				status << "PLDF2F! Play the " & pCard->GetFaceName() & 
						  " to promote the other top cards in our hand.\n";
			}
*/
		}
		// NCR-487 Get Outstanding cards earlier for next test
		CCardList outstandingCards;
		// get the outstanding cards (also count dummy)
		bool bDummyOnLeft = (m_pLHOpponent == pDummy); // NCR to count dummy's cards if he plays next
		const int numOutstandingCards = GetOutstandingCards(nSuitLed, outstandingCards, bDummyOnLeft);


		// if playing ahead of dummy, and dummy has only a single card in the 
		// suit led and we have an Ace (plus other cards), play the Ace now 
		// lest it be trumped later (any exceptions???)
		if (ISSUIT(nTrumpSuit) && (numCardsInSuitLed > 1) 
			&& (m_pLHOpponent == pDummy) && (numDummySuitCards == 1)
			&& (suit[0]->GetFaceValue() > outstandingCards.GetTopCardVal()) ) // NCR-487 check against top OScard
		{
			// see if the opponents can indeed trump the Ace
			CGuessedHandHoldings* pDeclarerHand = m_ppGuessedHands[nDeclarerPos];
			if ((GetNumOutstandingCards(nTrumpSuit) > 0) &&
					((dummyHand.GetNumTrumps() > 0) || !pDeclarerHand->IsSuitShownOut(nTrumpSuit)) )
			{
				// declarer/dummy may have some trumps left, so take the Ace now
				// NCR don't play Ace necessarily, just make sure we take the trick
				pCard = suit.GetLowestCardAbove(dummySuit.GetTopCard());  // NCR  was suit[0];
				// NCR-47 Make sure we beat both top card played and dummy's top card
				if((pCard == NULL) || (pCard->GetFaceValue() < pCurrTopCard->GetFaceValue())) 
				{
					pCard = suit.GetLowestCardAbove(pCurrTopCard);
					if(pCard == NULL) pCard = suit.GetBottomCard();  // give up if none found
				} // NCR-47 end making sure we win
				status << "PLDF2G0! Since dummy has only a single " & STSS(nSuitLed) & //NCR
						  " left, play our " & pCard->GetFaceName() & " now lest it be trumped later.\n";
				ASSERT(pCard->IsValid());
				return pCard;
			}
			else
			{
				// declarer/dummy have no trumps left
				// hold off on winning the Ace for now
//				status << "PLDF2G1! Since dummy has only a single " & STSS(nSuitLed) &
//						  " left, play our Ace now lest it be trumped later.\n";
			}
		}

		// if there are no more outstanding cards in the suit and we can play high,
		// do so here
/*  NCR-487 Moved these lines of code above previous if test
		CCardList outstandingCards;
		// get the outstanding cards (also count dummy)
		bool bDummyOnLeft = (m_pLHOpponent == pDummy); // NCR to count dummy's cards if he plays next
		const int numOutstandingCards = GetOutstandingCards(nSuitLed, outstandingCards, bDummyOnLeft);
*/
//		CHandHoldings& dummyHand = GetDummyHand();
//		CSuitHoldings& dummySuit = dummyHand.GetSuit(nSuitLed);
//		int numDummySuitCards = dummySuit.GetNumCards();
		if ((numOutstandingCards == 0) && (suit[0]->GetFaceValue() > pCardLed->GetFaceValue()))
		{
			pCard = suit.GetLowestCardAbove(pCardLed);
			status << "PLDF2G1! Since there are no more outstanding cards in the suit, win with the " &
					  pCard->GetFaceName() & ".\n";
			ASSERT(pCard->IsValid());
			return pCard;
		}

		// also if there's one other card outstanding in the suit, and we beat it also
		// NCR-192 Added ->GetFaceValue() on next two lines
		if ( ((numOutstandingCards == 1) && (suit[0]->GetFaceValue() > pCardLed->GetFaceValue()) 
			   && (suit[0]->GetFaceValue() > outstandingCards[0]->GetFaceValue())) )
		{
			// NCR-192 test for card to beat: Led or outstanding
			CCard * pTestCard = outstandingCards[0];
			if(pTestCard->GetFaceValue() < pCardLed->GetFaceValue())
				pTestCard = pCardLed;   // NCR-192 set to higher
			pCard = suit.GetLowestCardAbove(pTestCard);  // NCR-192 use pTestCard
			status << "PLDF2G1! There's only one more card outstanding card in the suit, so win here with the " &
					  pCard->GetFaceName() & ".\n";
			ASSERT(pCard->IsValid());
			return pCard;
		}
		//NCR If we have 2 cards left and the highest is not highest left in the suit, but it's higher than dummy
		// if we don't play it now, it may lose next time to the higher card. Do it 2 out of 3 times or if trump 
		//the dummy is on the left 
		if((numCardsInSuitLed == 2) && (m_pLHOpponent == pDummy) && !dummySuit.IsVoid()
			// NCR-8 added ->GetFaceValue() to all of following
			&& suit.GetTopCard()->GetFaceValue() < outstandingCards.GetTopCard()->GetFaceValue()
		    && suit.GetTopCard()->GetFaceValue() > pCurrTopCard->GetFaceValue() 
			&& suit.GetTopCard()->GetFaceValue() > dummySuit.GetTopCard()->GetFaceValue() 
//			&& ((suit.GetSuit() == nTrumpSuit) || !OneIn3())  // NCR if trump, better win it now
		  )
		{
			pCard = suit.GetTopCard();
			status << "PLDF2G2! There's a chance highcard will be picked up next trick, so win now with the " &
					  pCard->GetFaceName() & ".\n";
			ASSERT(pCard->IsValid());
			return pCard;
		}  //NCR end saving highcard in doubleton from being pickoff as singleton

		//NCR if the dummy(RH) has an honor and we can beat it, save it to cover dummy's.
		//NCR if we have a second high card that beats the top outstanding-> play it to keep declr from winning it
		if(m_pRHOpponent == pDummy && !dummySuit.IsVoid() // NCR what if dummy is void??? do NOT need to save
			&& (suit.GetTopCard()->GetFaceValue() > dummySuit.GetTopCard()->GetFaceValue())) // NCR-28
		{
			CCard* pTopOSCard = GetHighestOutstandingCard(nSuitLed);
			if(pTopOSCard != NULL){
				pCard = suit.GetLowestCardAbove(pTopOSCard->GetFaceValue());
				if((pCard != NULL) && (pCard != suit.GetTopCard())  //NCR check not top card
					&& (suit.GetTopCard()->GetFaceValue() > pCurrTopCard->GetFaceValue()) )  // NCR check if it'll be tops
				{
					status << "PLDF2G3! We can beat top outstanding card and still guard dummy so play " &
						  pCard->GetFaceName() & ".\n";
					ASSERT(pCard->IsValid());
					return pCard;
				} // end have card above top OS card
			} // end have top OS card
		}
		//NCR should we play high here if dummy to left has just 2 cards with an honor we can beat???
		// or if there are < three cards left out in suit
		CCardList outstandingCards2;  // NCR new object or Clear() this one???
		if((m_pLHOpponent == pDummy) && ((dummySuit.GetNumCards() <= 2) 
			                           || GetOutstandingCards(nSuitLed, outstandingCards2, false) < 3 ) 
			&& !dummySuit.IsVoid()
			&& IsHonor(dummySuit.GetTopCard()->GetFaceValue())  // is honor necessary
		    && suit.GetTopCard()->GetFaceValue() > pCurrTopCard->GetFaceValue() // NCR have to beat what's been played
			// NCR-24 added ->GetFaceValue() above and below
			&& suit.GetTopCard()->GetFaceValue() > dummySuit.GetTopCard()->GetFaceValue()) 
		{
			pCard = suit.GetLowestCardAbove(dummySuit.GetTopCard()); //NCR-204 Get min needed vs top
			status << "PLDF2G4! Keep declarer from finessing the " & dummySuit.GetTopCard()->GetFaceName()
				   & ", win now with the " & pCard->GetFaceName() & ".\n";
			ASSERT(pCard->IsValid());
			return pCard;
		}

		// NCR see if we bracket the top card, if so cover it
		pCard = suit.BeatIfBracketed(pCurrTopCard); //???pCardLed);


		// NCR We should play high here if:
		int ourTopCardFV = suit.GetTopCard()->GetFaceValue();
		if((( ourTopCardFV > pCurrTopCard->GetFaceValue()) // NCR first beat current
			// NCR-373 Don't Play high if we have move than 2 and first round of this suit
			&& ((numCardsInSuitLed <= 2) && ((numOutstandingCards + numCardsInSuitLed) < 12))
  		    // NCR special case - there is only one card outstanding and our top card beats it. 
			//dummy (RH) has played
		    && (((m_pRHOpponent == pDummy) && (outstandingCards.GetNumCards() == 1) 
			     && (ourTopCardFV > outstandingCards.GetTopCard()->GetFaceValue()) )
				// NCR also play high if we have the top 3 cards left in the suit, ie our 3rd card is greater 
				|| ((suit.GetNumCards() >= 3)
					&& (suit[2]->GetFaceValue() > outstandingCards.GetTopCard()->GetFaceValue()))
				// NCR or dummy is/will be void and has trumps (to ruff), so we should play to win 
				|| (((m_pRHOpponent == pDummy) && dummySuit.IsVoid())
					 || ((m_pLHOpponent == pDummy) && dummySuit.IsSingleton()) 
					&& (dummyHand.GetNumTrumps() > 0) // NCR dummy needs trumps to be a threat
					&& (nSuitLed != nTrumpSuit)    // NCR-56 no worries about ruffing trumps
					&& (ourTopCardFV > outstandingCards.GetTopCard()->GetFaceValue()))
                // NCR-158 Play winner if dummy can win and we have higher card on no other winners left
				|| ((m_pLHOpponent == pDummy) && !dummySuit.IsVoid()
				    && (ourTopCardFV > dummySuit.GetTopCard()->GetFaceValue())
				    && (suit.GetNumMissingAbove(dummySuit.GetTopCard()) < 2)) // NCR-158 ? 1 other winner
					) )
               // NCR-528 Play high if we have top cards
			|| ((suit.GetTopSequence().GetNumCards() > 1) && (numOutstandingCards > 0) 
			    && (suit.GetTopSequence().GetTopCard()->GetFaceValue() > outstandingCards.GetTopCard()->GetFaceValue()) )   
		  )
		{
			// NCR-56 Find card that we need to beat
			CCard* pCardToBeat = pCurrTopCard; // start with current
			int cardToBeatFV = pCardToBeat->GetFaceValue();
			// if dummy on right, need to beat top outstanding card
			if((pDummy == m_pRHOpponent) && (outstandingCards.GetTopCard()->GetFaceValue() > cardToBeatFV)) 
				pCardToBeat = outstandingCards.GetTopCard();
			// if dummy on left, need to beat his top card
			else if((pDummy == m_pLHOpponent) && !dummySuit.IsVoid() && (dummySuit.GetTopCard()->GetFaceValue() > cardToBeatFV))
				pCardToBeat = dummySuit.GetTopCard();

			pCard = suit.GetLowestCardAbove(pCardToBeat); // GetTopCard();  // NCR get the winning card
			if(pCard != NULL)  // NCR
				status << "PLDF2H5! Cover with the winner " & pCard->GetFaceName() & ".\n";
		}

		// NCR-554 Cover if playing after dummy and partner is out to prevent cheap trick
		if( (m_pRHOpponent == pDummy) && (partnerSuit.IsSuitShownOut())
			&& (suit.GetNumCardsAbove(pCurrTopCard) > 2) 
			&& (outstandingCards.GetNumCardsAbove(suit.GetLowestCardAbove(pCurrTopCard)) < 2) )
		{
			pCard = suit.GetLowestCardAbove(pCurrTopCard);
		}

		// NCR-562 See if we can prevent Dummy as LHO from getting a cheap trick
		if((pDummy == m_pLHOpponent) && !dummySuit.IsVoid() 
			&& (dummySuit.GetTopCard()->GetFaceValue() > suit.GetTopCard()->GetFaceValue()) 
			//  Can the outstanding high cards pull all my cards?
			&& (outstandingCards.GetNumCardsAbove(suit.GetTopCard()) <= suit.GetNumCards() - 1) 
			// If I force Dummy's high card(s) will I have a winner left?
			&& (dummySuit.GetNumCardsAbove(suit.GetTopCard()) < suit.GetSequence2(0).GetNumCards()) 
			// NCR-633 Don't bother if can't beat current top card
			&& (pCurrTopCard->GetFaceValue() < suit.GetTopCard()->GetFaceValue()) )
		{
			pCard = suit.GetTopCard();
		}

		// NCR-631 Will Dummy be void after this trick and have trump to ruff?
		if(ISSUIT(nTrumpSuit) 
			&& ((pDummy == m_pLHOpponent) && (dummySuit.GetNumCards() == 1)
				// NCR-657 and dummy's card < our top card
				&& (dummyHand.GetTopCard()->GetFaceValue() < suit.GetTopCard()->GetFaceValue()))
			   || ((pDummy == m_pRHOpponent) && dummySuit.IsVoid())
			&& (dummyHand.GetNumTrumps() > 0) ) 
		{
			CCard* pTopOutstandingCard = GetHighestOutstandingCard(nSuitLed);
			if(pTopOutstandingCard != NULL) {
				pCard = suit.GetLowestCardAbove(pTopOutstandingCard); // NCR-631 get higher card
				// Another test - higher card played already  (7/12/13)
				if((pCard != NULL) && (pCard->GetFaceValue() < pCurrTopCard->GetFaceValue()) )
					pCard = NULL;
				}
		}  // end NCR-631

		// else no special conditions apply -- just play low
		if(pCard == NULL)
			pCard = GetDiscard();
		if (numCardsInSuitLed > 1)  //NCR Can NEVER FAIL we're in block with numCardsInSuitLed >= 2 ???
		{
			if (*pCard < *pCurrTopCard)
				status << "PLDF2H1! Play second hand low with the " & pCard->GetFaceName() & ".\n";
			else if (pCard == suit.GetBottomCard())
				status << "PLDF2H2! As second hand, play the lowest card we have in the suit, the " & pCard->GetFaceName() & ".\n";
			else if(*pCard > *pCurrTopCard)
				status << "PLDF2H3! Cover with the " & pCard->GetFaceName() & ".\n";
			else
				status << "PLDF2H4! Play second hand ??? with the " & pCard->GetFaceName() & ".\n";
		}
		else
		{
			status << "PLDF2C4! Play our only " & STSS(nSuitLed) & ", the " & pCard->GetFaceName() & ".\n";
		}
	} // end numCardsInSuitLed >= 2
	else
	{
		// else we have no cards in the suit led
		// trump here if possible
		if (m_pHand->GetNumTrumps() > 0)
		{
			CSuitHoldings& trumpSuit = m_pHand->GetSuit(nTrumpSuit);

			// see if partner would win the suit otherwise
			CGuessedSuitHoldings& partnerSuit = m_pPlayer->GetGuessedHand(m_nPartnerPosition)->GetSuit(nSuitLed);
			CCardList outstandingCards;
			GetOutstandingCards(nSuitLed, outstandingCards);

			// partner can win the trick if he has an outstanding card higher than the top card
			BOOL bPartnerCanWin = FALSE;
			int i = NONE;
			if (partnerSuit.AreAllCardsIdentified())
			{
				int numTopOutstandingCards = outstandingCards.GetNumCardsAbove(pTopCard);
				for(i=0;i<numTopOutstandingCards;i++)
				{
					if (partnerSuit.HasCard(outstandingCards[i]->GetFaceValue()))
					{
						bPartnerCanWin = TRUE;
						break;
					}
				}
			}

			// partner MAY win if he is not void in the suit, 
			// and dummy does not have all the remaining top cards
			BOOL bPartnerMayWin = FALSE;
			if (!partnerSuit.AreAllCardsIdentified() && 
					!partnerSuit.IsSuitShownOut() && (outstandingCards.GetNumCardsAbove(pTopCard) > 0))
			{
				bPartnerMayWin = TRUE;
				status << "3PLDF2J2! Since there is at least one higher card outstanding in the " &
						   STSS(nSuitLed) & " suit than the one led (the " & pCardLed->GetFaceName() &
						   "), partner could still have it and win the round.\n";
			}

			//
			if (bPartnerCanWin)
			{
				// partner may win the trick, so discard
				pCard = GetDiscard();
				status << "PLDF2K1! We could trump here, but we know partner holds the " & outstandingCards[i]->GetName() &
						  " and can win the trick, so discard the " & pCard->GetName() & ".\n";
			}
			else
			{
				// we're not sure if partner can win, so trump here if possible
				// but first see if we're playing ahead of dummy (dummy is to our left), 
				// and dummy is also void in the suit
				CHandHoldings& dummyHand = GetDummyHand();
				if ((m_pLHOpponent == pDummy) && (dummyHand.IsSuitVoid(nSuitLed)) &&
							(dummyHand.GetNumCardsInSuit(nTrumpSuit) > 0))
				{
					// dummy is also void in the suit led, 
					// so trump only if we have a trump higher than dummy's
					CCard* pDummyTopTrump = dummyHand.GetSuit(nTrumpSuit).GetTopCard();
					if (trumpSuit.GetNumCardsAbove(pDummyTopTrump->GetFaceValue()) > 0)
					{
						// go ahead and ruff
						pCard = trumpSuit.GetLowestCardAbove(pDummyTopTrump->GetFaceValue());
						status << "PL2DFM1! Trump here, making sure to thwart dummy's " & pDummyTopTrump->GetFaceName() &
								  " of trumps by playing the " & pCard->GetFaceName() & ".\n";
					}
					else
					{
						// dummy would overtrump
						pCard = GetDiscard();
						status << "PL2DFM2! We'd like to trump here, but Dummy can overruff, so discard the " & pCard->GetName() & ".\n";
					}
				}
				else 
				{
					// dummy is not void in  the suit led, or is out of trumps
					// see if we'd be trumping with a top card while partner could stil win
					pCard = trumpSuit.GetBottomCard();
					if (trumpSuit.GetNumTopCards() == trumpSuit.GetNumCards() && bPartnerMayWin)
					{
						//
						pCard = GetDiscard();
						status << "PL2DFN2! We could ruff here, but there's no point in dropping a top trump now when partner might still win the round, so hold on to the trump and discard the " & pCard->GetName() & ".\n";
					}
					else
					{
						// safe to trump -- play the lowest trump
						pCard = trumpSuit.GetBottomCard();
						if (partnerSuit.IsSuitShownOut())
							status << "PLDF2N4! Partner is show out, and we won't be overtrumped, so trump here with the " & pCard->GetFaceName() & ".\n";
						else
							status << "PLDF2N4! We don't think partner will win the round, so trump with the " & pCard->GetFaceName() & ".\n";
					}
				}
			}
		}  // end have trumps
		else
		{
			//NCR-76 Special case when discarding on the next to last trick
			if(pDOC->GetPlayRound() == 11) 
			{
				// Save what leader(if he will win) has, discard what he does not have
				// HOW TO DETERMINE IF CURRENT LEADER WILL KEEP LEAD???
				if(pCardLed->GetSuit() == nTrumpSuit || (GetHighestOutstandingCard(nSuitLed) == NULL)) 
				{
					// NCR-504 If RHO is dummy, see what he has left
					if(m_pRHOpponent == pDummy) 
					{
						// See if we can beat dummy's last card
						CCard* dmysCard = dummyHand.GetDisplayedCard(0);
						// Check if we can beat dummy's last card
						CSuitHoldings& ourSuit = m_pHand->GetSuit(dmysCard->GetSuit());
						if((ourSuit.GetNumCards() > 0) && (*ourSuit[0] > *dmysCard))
						{
							// Save this card!  Discard from other suit
							CHandHoldings& ourHand = *m_pHand;
							for(int ix = 0; ix < 2; ix++) 
							{
								if(ourHand.GetDisplayedCard(ix)->GetSuit() != dmysCard->GetSuit()) 
								{
									pCard = ourHand.GetDisplayedCard(ix);
									break;
								}
							} // end for(ix) thru remaining 2 cards
						}
					}  // end NCR-504
					else
					{
						// Get data on RHO
						CGuessedHandHoldings* rhoHand = m_ppGuessedHands[m_pRHOpponent->GetPosition()];
						// Discard card in suit he is void in
						for(Suit suit = CLUBS; suit < SPADES; GETNEXTSUIT(suit)) 
						{
							if(rhoHand->GetSuit(suit).IsSuitShownOut() && (m_pHand->GetSuit(suit).GetNumCards() > 0)) {
								pCard = m_pHand->GetSuit(suit).GetBottomCard();
								break;
							}
						}
					}
				}
				if(pCard == NULL)
					pCard = GetDiscard(); 
			} // NCR-76 end discarding on last round			
			// discard
			else
				pCard = GetDiscard();

			if (ISSUIT(nTrumpSuit))
				status << "PLDF2Y! We have no " & SuitToString(nSuitLed) & " and no trumps, so discard the " & pCard->GetName() & ".\n";
			else
				status << "PLDF2Ya! We have no " & SuitToString(nSuitLed) & ", so discard the " & pCard->GetName() & ".\n";
		}
	} // end no cards left in suit led
	//
	ASSERT(pCard->IsValid());
	return pCard;
} // end PlaySecond()




//
// PlayThird()
//
CCard* CDefenderPlayEngine::PlayThird()
{
//	return CPlayEngine::PlayThird();
	CPlayerStatusDialog& status = *m_pStatusDlg;

	//
	// playing third as defender
	// we should generally play high, unless...
	//

	// get play info
	CCard* pCurrentCard = pDOC->GetCurrentTrickCardLed();
	const int nSuitLed = pCurrentCard->GetSuit();
	CPlayer* pDummy = pDOC->GetDummyPlayer();
	int nTopPos;
	CCard* pCurrTopCard = pDOC->GetCurrentTrickHighCard(&nTopPos);
	CString strTopCardPos = PositionToString(nTopPos);
	BOOL bPartnerHigh = FALSE;
	bool bDummyHighest = false;   // NCR-582
	const int nCurrentRound = pDOC->GetPlayRound();
	const int nCurrentSeat = pDOC->GetNumCardsPlayedInRound() + 1;
	const int nTrumpSuit = pDOC->GetTrumpSuit();
	const int numCardsInSuitLed = m_pHand->GetNumCardsInSuit(nSuitLed);
	// card to play
	CCard* pCard = NULL;
	//
	CCard* pPartnersCard = pDOC->GetCurrentTrickCard(m_pPartner->GetPosition());
	if (pPartnersCard == pCurrTopCard)
		bPartnerHigh = TRUE;

	// determine the actual top opponent card -- it's either the highest one played
	// or dummy's top holding in the suit, if dummy is to our left
	CCard* pActualTopCard = pCurrTopCard;
	CHandHoldings& dummyHand = GetDummyHand();
	CSuitHoldings& dummySuit = dummyHand.GetSuit(nSuitLed);
	const int numDummySuitCards = dummySuit.GetNumCards();
	// if dummy plays last (on our left), see if he holds the top card
	if ((m_pLHOpponent == pDummy) && (numDummySuitCards > 0) &&
		(pCurrTopCard->GetSuit() == nSuitLed) && (*dummySuit[0] > *pCurrTopCard))
	{
		// dummy does indeed hold the top card
		pActualTopCard = dummySuit[0];
		nTopPos = pDOC->GetDummyPosition();
//		bPartnerHigh = FALSE;	// partner's no longer high (he's sober?)
		bDummyHighest = true; // NCR-582 need this as well as knowing pard is current high
	}


	// 
	// first see if somebody trumped in this hand
	//
	if ((pDOC->WasTrumpPlayed()) && (nTrumpSuit != nSuitLed))
	{
		// a trump has been played by the opponent
		// (in third seat, partner can't have trumps)
		// see if we can overtrump
		CSuitHoldings& trumpSuit = m_pHand->GetSuit(nTrumpSuit);
		CCard* pTopTrump = NULL;
		if (trumpSuit.GetNumCards() > 0)
			pTopTrump = trumpSuit.GetTopCard();
		if ((numCardsInSuitLed == 0) && pTopTrump && (*pTopTrump > *pCurrTopCard))
		{
			// we have a higher trump we can play
			// but see if dummy is to our left & holds a higher trump
			if ((m_pLHOpponent == pDummy) && (numDummySuitCards == 0)
				 &&	(dummyHand.GetNumTrumps() > 0) 
				 // NCR-533 Check whether dummy boss over current top
				 && (*(dummyHand.GetSuit(nTrumpSuit).GetTopCard()) > *pCurrTopCard) )  
			{   
				// NCR-176 Check if dummy can overtrump
				if((*(dummyHand.GetSuit(nTrumpSuit).GetTopCard()) > *pTopTrump))
				{
					// dummy can overtrump, so discard
					pCard = GetDiscard();
					status << "PLDF10! We'd like to trump here, but dummy can overtrump, so discard the " 
						      & pCard->GetName() & ".\n";
				}
				else
				{
					pCard = trumpSuit.GetLowestCardAbove(dummyHand.GetSuit(nTrumpSuit).GetTopCard());
					status << "PLDF12! We'll trump high enough with " & pCard->GetName() & " to beat dummy.\n";
				}

			}
			else
			{
				// get the lowest trump that will top the current top trump
				int numTopCards = trumpSuit.GetNumCardsAbove(pCurrTopCard);
				pCard = trumpSuit[numTopCards-1];
				status << "PLDF20! We can overtrump " & strTopCardPos & "'s " & pCurrTopCard->GetName() &
						  " with the " & pCard->GetFaceName() & ".\n";
			}
		}
		else
		{
			// no chance to win here, so discard
			pCard = GetDiscard();
			if ((numCardsInSuitLed == 0) && (trumpSuit.GetNumCards() > 0))
				status << "PLDF22! We can't overtrump " & strTopCardPos & "'s " & 
						  pCurrTopCard->GetFaceName() & ", so discard the " & pCard->GetName() & ".\n";
			else
				status << "PLDF23! We can't beat the opponent's " & pCurrTopCard->GetFaceName() & 
						  " of trumps, so discard the " & pCard->GetName() & ".\n";
		}							
	}
	else
	{
		// else nobody has played a trump this round, _or_ a trump was led
		// (or we're playing NT)
		// see how many cards we have in the suit
		if (numCardsInSuitLed == 1)
		{
			// play the only card we have in the suit
			pCard = m_pHand->GetSuit(nSuitLed).GetTopCard();
			status << "PLDF25! Play our only card in the suit, the " & pCard->GetFaceName() & ".\n";
		}
		else if (numCardsInSuitLed >= 2)
		{
			// we have multiple cards in the suit
			// if we can beat the current top card, do so with the cheapest card
			CSuitHoldings& suit = m_pHand->GetSuit(nSuitLed);
			CCard* pTopOutstandingCard = GetHighestOutstandingCard(nSuitLed); // NCR for test below

			if (*(suit.GetTopCard()) > *pCurrTopCard)
			{
				// but see if the top card is partner's
				if (bPartnerHigh && !bDummyHighest)   // NCR-582 add test for dummy
				{
					// see if we should unblock here
					if (ISSUIT(nTrumpSuit) && (nCurrentRound == 0)
						&& (suit.GetNumHonors() == 1) && (suit.GetNumCards() == 2)
						// NCR don't drop possible winner. Save if greater than dummy's top
						&& ((m_pRHOpponent == pDummy) && (numDummySuitCards > 0) 
						    && (suit.GetTopCard()->GetFaceValue() < dummySuit.GetTopCard()->GetFaceValue())))
					{
						// first round in an NT contract, with one honor 
						// in the suit -- unblock
						pCard = suit.GetTopCard();
						if (suit.GetNumCards() > 1)
							status << "PLDF30! Drop the " & pCard->GetFaceName() & 
									  " here to unblock the suit for partner.\n";
					}
					else if (!ISSUIT(nTrumpSuit) && (suit.GetNumWinners() == suit.GetNumCards())
						     // NCR play highest card only if rest of cards will win
						     && ( (pTopOutstandingCard ==  NULL) 
							    || (suit.GetSecondHighestCard()->GetFaceValue() > pTopOutstandingCard->GetFaceValue())) )
					{
						// we're holding all the winners in this suit, so capture high
						pCard = suit.GetTopCard();
						if (suit.GetNumCards() > 1)
							status << "PLDF31! We're holding all the remaining winning " & STS(nSuitLed) &
									  " in our hand, so win with the " & pCard->GetFaceName() &
									  " in order to cash our winners.\n";
					}
					else
					{
						// else this is not an unblocking situation
						if (nCurrentSeat == 4)
						{
							// playing 4th seat, high card is partner, so discard
							pCard = GetDiscard();
							status << "PLDF34! Partner's " & pCurrTopCard->GetFaceName() & 
									  " is high, so discard the " & pCard->GetName() & ".\n";
						}
						else
						{
							// playing in third seat -- decide whether to 
							// let partner's card ride
							// do so if if partner's card beats all outstanding cards
							// NCR or if our top card is equivalent to pard's EG our Q vs pard's J
// NCR done above							CCard* pTopOutstandingCard = GetHighestOutstandingCard(nSuitLed);
							if(pTopOutstandingCard == NULL) pTopOutstandingCard = deck.m_pNoCard; //NCR vs NULL
							// NCR-577 change following to const and cast assigns as needed below
							const CCard* pOurCard = suit.GetTopSequence().GetBottomCard();
/* NCR replaced following
							if ( (pTopOutstandingCard == NULL) || (*pCurrTopCard > *pTopOutstandingCard)
								 || (pOurCard->GetFaceValue() <= pCurrTopCard->GetFaceValue() + 1)
							     //NCR test if dummy is to left and if partners card beats dummy
 								 || ((m_pLHOpponent == pDummy)   // is dummy to left, ie he plays AFTER us
								     && (dummySuit.IsVoid()  // dummy void in this suit
								         || (!dummySuit.IsVoid() && (*dummySuit[0] < *pCurrTopCard)))) 
                                  //NCR is card lead same value as what we'd play because dummy has intermediate? 
								  //NCR following is Q&D - Need method to check if cards are in sequence ???
							     || ((m_pRHOpponent == pDummy) && bPartnerHigh 
								      && (!dummySuit.IsVoid() && *dummySuit[0] > *pCurrTopCard)) )
end NCR removal*/
							if( ((m_pRHOpponent == pDummy) && ((pCurrTopCard->GetFaceValue() > pTopOutstandingCard->GetFaceValue())
								//NCR if top outstanding card is > our top card, don't play it unless we have >1
								                                || ((pTopOutstandingCard->GetFaceValue() > pOurCard->GetFaceValue()) 
																     && (suit.GetTopSequence().GetNumCards() == 1))
							                                    || pDOC->AreCardsInSequence(pOurCard->GetDeckValue(), pCurrTopCard->GetDeckValue()) 
														      )
                                   // NCR if have doubleton and top card is K or Q, play it
								   && !(suit.IsDoubleton() && ((*suit.GetTopCard() == KING) || (*suit.GetTopCard() == QUEEN)))
								   // NCR-115 Don't force if pard's card, dummy and our card are in Sequence
                                   && !((dummySuit.GetNumCards() > 0) && InSequence(suit[0], dummySuit[0], pCurrTopCard)) 
								) 

								// if dummy on left(to play) and dummy is void, discard under partner´s winner
								|| ( (m_pLHOpponent == pDummy) && (dummySuit.IsVoid()))
								// if dummy on left(to play) need to see if top card beats him
								|| ((m_pLHOpponent == pDummy) && (!dummySuit.IsVoid() && (pCurrTopCard->GetFaceValue() > dummySuit[0]->GetFaceValue())))
								// or if dummy has higher card than mine
								|| ((m_pLHOpponent == pDummy) && (!dummySuit.IsVoid() && (dummySuit[0]->GetFaceValue() > pCurrTopCard->GetFaceValue())
								    && (dummySuit[0]->GetFaceValue() > pOurCard->GetFaceValue()))) 	)  // NCR end replacement
							{

								//NCR one last check - do we have a second sequence that is useful to force higher card
								if((m_pRHOpponent == pDummy) 
									// NCR-52 pard's card is NOT top card in suit
									&& (pCurrTopCard->GetFaceValue() < pTopOutstandingCard->GetFaceValue())
									&& (suit.GetNumSequences() > 1) 
									&& (suit.GetSecondSequence().GetNumCards() > 1)) 
								{
									pCard = suit.GetSecondSequence().GetBottomCard();
									if(pDOC->AreSameValue(pPartnersCard, pCard)
									   // NCR-50 or our card is smaller than partners
									   || ((pCard->GetFaceValue() < pPartnersCard->GetFaceValue())
										   && !pDOC->AreSameValue(pPartnersCard, pOurCard))  // NCR-106 don't change if same  
									  )
										pCard = (CCard*)pOurCard;  // change to higher for force
									status << "PLDF35! Try to force a higher card by playing " & pCard->GetName() & ".\n";
								}
								else
								{
									// partner's card is higher than any outstanding, or
									// else we don't have anything much higher, so let it ride
									//NCR Problem here - GetDiscard returns higher than needed card???
									if(pDOC->AreCardsInSequence(pOurCard->GetDeckValue(), pCurrTopCard->GetDeckValue())
										// NCR play low if partner's card is high
										|| (bPartnerHigh && (pCurrTopCard->GetFaceValue() 
										                    > pTopOutstandingCard->GetFaceValue())) ) 
									{
										// NCR test if we should signal vs low card
										// NCR signal if 2+ honors and at least 4 cards
										if((suit.GetNumHonors() >= 2) && (suit.GetNumCards() >= 4)) 
										{
											pCard = GetHighDiscard(suit);
											// NCR don't beat partners card???
											if(pCard->GetFaceValue() > pCurrTopCard->GetFaceValue()) 
											{
												CCard * pCard2 = suit.GetHighestCardBelow(pCard);
											    if(pCard2 != NULL)  pCard = pCard2;  // NCR-2 make sure valid
											}
										} 
										else
											pCard = suit.GetBottomCard();
									}
									else if (nCurrentRound == 0) 
									{
										// // NCR play high card on partner's first lead
										int smallCardFV = 7; // NCR-159 set FaceValue for following tests
										if(suit.GetTopCard()->GetFaceValue() < JACK)  // NCR-579 Don't play high into declr
											pCard = suit.GetTopCard();
										// NCR-7 don't beat pard's card
										else if((suit.GetNumCards() > 1)
											     && (suit.GetSecondHighestCard()->GetFaceValue() < pCurrTopCard->GetFaceValue()) //NCR-7
												 // NCR-579 Save high card if it beats dummy's high card
												 && (!dummySuit.IsVoid() && (dummySuit.GetTopCard()->GetFaceValue()  > JACK) 
												     && (suit.GetTopCard()->GetFaceValue() > dummySuit.GetTopCard()->GetFaceValue())) )
										{
											pCard = suit.GetSecondHighestCard();
										}
										// NCR-323 Play high if dummy is void or does not have a high card to guard 
										else if((m_pRHOpponent == pDummy) && (dummySuit.IsVoid())) {
											pCard = suit.GetTopCard(); // NCR-323 Play high NB> Wrong status msg below 
										}
										// NCR-98 Play high if partner's card is a low one WEAK CODE ???
										else if ((pCurrTopCard->GetFaceValue() < smallCardFV) 
											     && (suit.GetNumCardsAbove(smallCardFV) >= 2))
										{
											pCard = suit.GetLowestCardAbove(smallCardFV);  // NCR-98 play higher
										} 
										else
											pCard = GetDiscard();

									}else {
										pCard = GetDiscard();
										// NCR-2 Check if discard is higher than pard's card and get lower
										// card if available
										// NCR-374 test number of cards out that will beat this card
										CCardList outstandingCards; 
								        int nCardsOut = GetOutstandingCards(nSuitLed, outstandingCards, false);
										if((pCard->GetFaceValue() > pCurrTopCard->GetFaceValue())
											&& ((m_pLHOpponent == pDummy) 
											       // NCR-374 2 above out of 5 = 40%  4 above out of 5 = 80%
											    || ((outstandingCards.GetNumCardsAbove(pCard->GetFaceValue())*100 / nCardsOut) > 50 ) ))
										{
											CCard * pCard2 = suit.GetHighestCardBelow(pCard); 
											if(pCard2 != NULL)  pCard = pCard2;  // NCR-2 make sure valid
										}
									}
									// NCR chose message
									if(pCard->GetFaceValue()  < pCurrTopCard->GetFaceValue()) {
										status << "PLDF36! Partner's " & pCurrTopCard->GetFaceName() & 
											  " is high or we can't beat the highest card, so discard the " & 
											  pCard->GetName() & ".\n";
									}else{
										status << "PLDF36A! Play third hand high with the " & pCard->GetName() & ".\n";
									}
								}
							}
							// NCR check if our pard's card, dummy's and our's are in sequence
							else if(((dummySuit.GetNumCards() > 0) 
								     // NCR-577 Changed suit[0] to pOurCard
								     && InSequence(pOurCard, dummySuit[0], pCurrTopCard)) // High to low
									// NCR-34 or if just pard's and ours
									|| (pDOC->AreSameValue(pPartnersCard, pOurCard)) ) // NCR-34
							{	
								pCard = suit.GetBottomCard();
								status << "PLDF36C! Partner's card is same as our's so play " & pCard->GetName() & ".\n";
							}
							else
							{
								// partner's card is not necessarily highest, so top it
								// if we can

								// NCR check if our top two cards bracket dummy's cards
								// THIS NEED WORK ??? eg topSequence vs [0]
								if ((m_pRHOpponent == pDummy) && (dummySuit.GetNumCards() > 0) 
									&& (numCardsInSuitLed < 4)  // only notice dummy if few cards left ???
									&& (suit.GetNumCards() > 1) && (suit.GetNumSequences() > 1)
									&& InSequence(suit.GetLowestCardAbove(dummySuit[0]), dummySuit[0],
												  suit.GetHighestCardBelow(dummySuit[0])) )
								{
									pCard = suit.GetHighestCardBelow(dummySuit[0]); // if inSeq this will do 
								}
								else
								{
									// NCR play lowest card that will win
									if(pTopOutstandingCard != deck.m_pNoCard) // dummy card???
										pCard = suit.GetLowestCardAbove(pTopOutstandingCard); //NCR our lowest card above top
									if(pCard == NULL)  pCard = (CCard*)pOurCard;  // NCR just in case???
								}
								status << "PLDF37! Partner's " & pCurrTopCard->GetFaceName() & 
										  " might not win the round, so top it with the " & pCard->GetFaceName() & ".\n";
							}
						}
					}
				}
				else  // partner's card is NOT high
				{
					// here the high card is opponent's, so beat it w/ highest card affordable
					// but see if we're playing ahead of dummy
					if (m_pLHOpponent == pDummy)
					{
						// playing in front of dummy; see if he holds the top card
						if (*pActualTopCard > *pCurrTopCard)
						{
							// dummy holds the top card, so see if we can beat it
							if (*(suit.GetTopCard()) > *pActualTopCard)
							{
								// we can beat it!
								pCard = suit.GetLowestCardAbove(pActualTopCard);
								status << "PLDF38A! Playing third ahead of dummy, we need to beat dummy's " & 
										  pActualTopCard->GetFaceName() & ", so play the " &
										  pCard->GetFaceName() & ".\n";
							}
							else
							{
								// else we can't beat dummy's top card, but play
								// high anyway to force out his winner
								// NCR do we have a sequence that beats dummy's second highest card?
								if((suit.GetTopSequence().GetNumCards() >= 2) && !dummySuit.IsSingleton()
									&& (suit.GetTopCard()->GetFaceValue() > dummySuit.GetSecondHighestCard()->GetFaceValue()) ) 
								{
									pCard = suit.GetSecondHighestCard(); // NCR 2nd card is end of top sequence ???
								}
								// NCR can we force dummy's high card
								else if(!dummySuit.IsSingleton() && (dummySuit.GetNumSequences() > 1)
									    // NCR-582 if partner is high, does his card beat dummy's second
									    && (!bPartnerHigh 
										    || (pPartnersCard->GetFaceValue() 
											    < dummySuit.GetSecondSequence().GetTopCard()->GetFaceValue())) )
								{
									pCard = suit.GetLowestCardAbove(dummySuit.GetSecondSequence().GetTopCard());
									// One last test, does pCard beat current top card?
 /*NCR-582 removed this ??*/		if((pCard == NULL) || (*pCard < *pCurrTopCard))
										pCard = suit.GetLowestCardAbove(pCurrTopCard);  // NCR-651 if not, beat it
								}
								else 
								{
									// NCR don't play high if our card same value as partners
									if(!bPartnerHigh) // NCR-582 don't cover partner
									    pCard = suit.GetLowestCardAbove(pCurrTopCard); // ???
								}

								if(pCard == NULL) // NCR what if we don't have a card above ???
									pCard = suit.GetBottomCard();

								//NCR following Q&D need better function that tests for played cards also???
								// NCR pCard was NULL below ??? so added code above
								if((pDOC->AreSameValue(pPartnersCard, pCard) // looks at currently played
									 && (pCurrTopCard == pPartnersCard)) //NCR test if our card same as partners
									//NCR also if dummy's lowest card is > than our highest
								    || (dummySuit.GetBottomCard()->GetFaceValue() > suit.GetTopCard()->GetFaceValue())
									// NCR and if we need to force dummy's high card (or will partner's card do it)
									|| (!dummySuit.IsSingleton() 
									    && (dummySuit.GetSecondHighestCard()->GetFaceValue() < pPartnersCard->GetFaceValue())
									    && (pPartnersCard == pCurrTopCard)))
								{
									pCard = suit.GetBottomCard();
								}
								if (suit.GetNumCardsBelow(pCard) > 0)
									status << "PLDF38C! We top declarer's " & pCurrTopCard->GetFaceName() &
											  " with the " & pCard->GetFaceName() & "  to force dummy to play a winner.\n";
								else
									status << "PLDF38D! We can't beat dummy in this hand, so discard the " &
											  pCard->GetFaceName() & " .\n";
							}
						}
						else
						{
							// else the top card is declarer's, which we can beat
							pCard = suit.GetLowestCardAbove(pCurrTopCard);
							status << "PLDF38B! Play high to win with the " & pCard->GetFaceName() & ".\n";
						}
					}
					else if (nCurrentSeat == 3)   // Dummy is RHO
					{
						// else we're not playing ahead of dummy 
						// NCR if first round, use Rule of 11 to chose play - assumes pard lead 4th
						if(nCurrentRound == 0)
						{
							int numBiggerCardsOut = 11 - pPartnersCard->GetFaceValue();
							int numInDummy = dummySuit.GetNumCardsAbove(pPartnersCard);
							int numInMine = suit.GetNumCardsAbove(pPartnersCard);
							int numInTrick = (pDOC->GetCurrentTrickCard(pDummy->GetPosition())->GetFaceValue() 
								                    > pPartnersCard->GetFaceValue()) ? 1 : 0;
							// See if all accounted for
							if(numBiggerCardsOut == (numInDummy + numInMine + numInTrick))
							{
								// NCR-45 What if pard led singleton and we have top winners?
								if(((numBiggerCardsOut == 9) && (numInMine >= 5))  // NCR-45 make some test?
									// NCR-269 play high if we have top cards
									|| (suit.GetTopSequence().GetNumCards() >= 3) )
									pCard = suit.GetTopSequence().GetBottomCard();  // NCR-45 play high
								else
									pCard = suit.GetLowestCardAbove(pActualTopCard); //???pPartnersCard);  // This is all we need
							}
							else   // NCR-499 Third hand should play high ???
							{
								pCard = suit.GetTopSequence().GetBottomCard(); // NCR-499 
							}
							// NCR-505 What if pard did NOT lead 4th card? Would he with suit contract?
							if(ISSUIT(nTrumpSuit))
								pCard = GetLowestCardNeededToWin(suit, dummySuit, bPartnerHigh); // NCR-505
						}
						else 
						{
							// NCR find lowest card needed to win
							pCard = GetLowestCardNeededToWin(suit, dummySuit, bPartnerHigh); // NCR
						}

						if(pCard == NULL) 
						{   
							// NCR check if our top two cards bracket dummy's cards
							// want to keep winner over dummy's top card
							// THIS NEED WORK ??? eg topSequence vs [0]
							if ((dummySuit.GetNumCards() > 0) && (suit.GetNumCards() > 1)
								&& InSequence(suit.GetLowestCardAbove(dummySuit[0]), dummySuit[0],
								              suit.GetHighestCardBelow(dummySuit[0])) 
				                //  and make sure our card will be a winner
							    && (suit.GetHighestCardBelow(dummySuit[0])->GetFaceValue()
								     > pCurrTopCard->GetFaceValue()) 
                                // NCR-91 Don't worry about dummy if: have 5 with winners that could be ruffed
								&& !((suit.GetNumCards() > 4) && (suit.GetTopSequence().GetNumCards() > 1)
								      && (suit.GetTopCard() > suit.GetTopMissingSequence().GetTopCard()) ) 
							   )
							{
								pCard = suit.GetHighestCardBelow(dummySuit[0]); // if inSeq this will do 
								// NCR-221 Play lowest that could win
								CCardList outstandingCards;
								int nCardsOut = GetOutstandingCards(nSuitLed, outstandingCards, false);
								if((outstandingCards.GetNumCardsAbove(pCard) < 2)
									&& (nCardsOut < 4))  // nbr winners vs nbr out???
									pCard = suit.GetLowestCardAbove(pCurrTopCard); // NCR-221 lowest that wins
							}
							else
							{
								// NCR-29 Also test if Dummy (RHO) has all the cards that beat the 
								// current trick's top card (fm Dummy) then play lowest card that 
								// beats the current top card
								CCardList outstandingCards;
								// NCR-398 Include dummy if he plays next (to the left)
								bool bIncludeDummy = (m_pLHOpponent == pDummy) ? true : false;
								int nCardsOut = GetOutstandingCards(nSuitLed, outstandingCards, bIncludeDummy); // NCR-398
								if((outstandingCards.GetNumCardsAbove(pCurrTopCard) == dummySuit.GetNumCardsAbove(pCurrTopCard))
									// NCR-29 Test no cards left
									|| ((m_pRHOpponent == pDummy) && (nCardsOut == 0)) )
								{
									pCard = suit.GetLowestCardAbove(pCurrTopCard); // NCR-29
								}
								// NCR-398 See if we can win 
								else if ((nCardsOut > 0) && (suit.GetNumCardsAbove(outstandingCards.GetTopCard()) > 0) )
								{
									// NCR-398 Win with cheapest that will wind
									CCard * pCardToBeat = pCurrTopCard > outstandingCards.GetTopCard() 
										                   ? pCurrTopCard : outstandingCards.GetTopCard();
									pCard = suit.GetLowestCardAbove(pCardToBeat);
								}
								else
								{
									// so play the lowest card from our top sequence
									pCard = suit.GetTopSequence().GetBottomCard();
								}
							}
							status << "PLDF40! Play high with the " & pCard->GetFaceName() & ".\n";
						}
						else
						{
							status << "PLDF40a! Play higher with the " & pCard->GetFaceName() & ".\n";
						}
					}
				} // NCR end partner NOT high
			}
			else
			{
				// here we don't have a card to top the current high card
				if (bPartnerHigh)
				{
					// NCR unblock if NT, have 2 cards and pard has lead this suit before
					// NCR Q&D needs work ???
					if(ISNOTRUMP(nTrumpSuit) && (numCardsInSuitLed == 2)
						// NCR-616 Don't unblock if dummy has high card
						&& !((dummySuit.GetNumCards() > 1) 
						     && pDOC->AreSameValue(dummySuit.GetTopCard(), suit.GetTopCard()))
							 // NCR-656 Don't unblock if dummy will have a winner
							 && !((dummySuit.GetNumCards() > 0)
							      && (suit.GetTopCard()->GetFaceValue() > dummySuit.GetTopCard()->GetFaceValue())	
							      && (suit.GetBottomCard)()->GetFaceValue()  < dummySuit.GetTopCard()->GetFaceValue()) )
					{
						// NCR need special logic at end of play - save entries if we have winners
						if ((nCurrentRound < 8) 
							|| ((nCurrentRound >= 8) && (m_pHand->GetNumWinners() < 3))) 
						{
							pCard = suit.GetTopCard();
							status << "PLDF41! Drop the " & pCard->GetName() & 
							         " here to unblock the suit for partner.\n";
						}
						else
						{
							// NCR Late play with 3 or more winners
							pCard = suit.GetBottomCard();
							status << "PLDF42! Discard the " & pCard->GetName() & 
							         " here to preserve our entries.\n";
						}
					}
					else
					{
						// but partner's card is high, so we're OK
						pCard = GetDiscard();
						status << "PLDF47! Partner's " & pCurrTopCard->GetName() & 
								  " can win the trick, so discard the " & pCard->GetName() & ".\n";
					}
				}
				else
				{
					// else we're screwed
					pCard = GetDiscard();
					status << "PLDF48! We can't beat " & strTopCardPos & "'s " & pCurrTopCard->GetName() & 
							  ", so discard the " & pCard->GetName() & ".\n";
				}
			}
		}
		else 
		{
			// here. we have no cards in the suit led
			// see if we have trumps
			if (m_pHand->GetNumTrumps() > 0)
			{
				// here, we can play a trump, so do so if appropriate 
				// see who has the top card in this round

				// NCR we'll discard if:  declarer (to left) is void 
				// or nbr of cards out < 3 -> greater chance declarer is void
				// or partner's card is higher than outstanding
				CCard* pTopOutstandingCard = GetHighestOutstandingCard(nSuitLed);
				if(pTopOutstandingCard == NULL) pTopOutstandingCard = deck.m_pNoCard; //NCR vs NULL
				int nDeclarer = pDOC->GetDeclarerPosition();
			    CGuessedSuitHoldings& pDeclarerCards = m_pPlayer->GetGuessedHand(nDeclarer)->GetSuit(nSuitLed);
    			BOOL bOkToDiscard = ( ((m_pLHOpponent != pDummy) // declarer is on left (plays next)
					                    && (pDeclarerCards.IsSuitShownOut()))  // and declarer is void
									 || ((m_pLHOpponent == pDummy) && bPartnerHigh)  // NCR pards card is winner	
									 ||	(GetNumOutstandingCards(nSuitLed) < 3) // OR there are at least 3 out
								     || (pPartnersCard->GetFaceValue() > pTopOutstandingCard->GetFaceValue())  // or pard's card is highest left
							        ); 
				// NCR also check if there are less than 2 outstanding cards greater than pard's card
				if(bOkToDiscard) {
					CCardList outstandingCards;
					int nCardsOut = GetOutstandingCards(nSuitLed, outstandingCards);
					if ((((nCardsOut > 1) && (outstandingCards[1]->GetFaceValue() > pPartnersCard->GetFaceValue())
						  && (m_pLHOpponent != pDummy))  // NCR ignore if dummy plays next
						//or only one greater and declarer is on the right
						 || ((m_pRHOpponent == pDummy) && (nCardsOut >= 1) 
						    && (outstandingCards[0]->GetFaceValue() > pPartnersCard->GetFaceValue())))
						// NCR check dummy on LH - higher card or void and trumps
						|| ((m_pLHOpponent == pDummy) && ((!dummySuit.IsVoid() 
						     && (dummySuit.GetTopCard()->GetFaceValue() > pPartnersCard->GetFaceValue()))
							|| (dummySuit.IsVoid() && (dummyHand.GetSuit(nTrumpSuit).GetNumCards() > 0))))  
					   )
					{
						bOkToDiscard = FALSE;  // better Ruff, not sure if pard's card will win
					}
				}
				if (bPartnerHigh && bOkToDiscard)
				{
/*
					// let partner's card ride, UNLESS playing in front of dummy
					// who is void in the suit and has trumps
					if (m_pLHOpponent == pDummy)
					{
						// playing ahead of dummy -- is dummy void in the suit & has trumps?
						// if so, he'll most likely trump -- so we should trump if
						// we can spare it
						if ((dummyHand.GetNumCardsInSuit(nSuitLed) == 0) && (dummyHand.GetNumTrumps() > 0))
						{
							// see if we should trump here???
							CCardList& dummyTrumps = dummyHand.GetSuit(nTrumpSuit);
						}
						else
						{
							pCard = GetDiscard();
							status << "PLDF52! Although we could trump this hand, partner's " & pCurrTopCard->GetName() & 
									  " is high and dummy can't trump, so discard the " & pCard->GetName() & ".\n";
						}
					}
					else
					{
						// not playing ahead of dummy, so discard and hope
						// partner's card wins
						pCard = GetDiscard();
						status << "PLDF54! Although we could trump this hand, partner's " & pCurrTopCard->GetName() & 
								  " is high, so discard the " & pCard->GetName() & ".\n";
					}
*/
					// since we're not sure when to trump (and possibly be overtrumped by dummy),
					// simply punt and discard here
					pCard = GetDiscard();
					status << "PLDF54! Although we could trump this hand, partner's " & pCurrTopCard->GetName() & 
							  " is high, so discard the " & pCard->GetName() & ".\n";
				}
				else
				{
					// opponents have the high card (non-trump) -- so slam 'em
					// NCR check if dummy plays next, is void and has trump
					CSuitHoldings& trumpSuit = m_pHand->GetSuit(nTrumpSuit);
					CSuitHoldings& dummyTrumps = dummyHand.GetSuit(nTrumpSuit);
					if((m_pLHOpponent == pDummy) && dummySuit.IsVoid() 
						&& (dummyTrumps.GetNumCards() > 0) // NCR we have trumps to be here
						&& (trumpSuit.GetTopCard()->GetFaceValue() > dummyTrumps.GetTopCard()->GetFaceValue()))
					{
						pCard = trumpSuit.GetLowestCardAbove(dummyTrumps.GetTopCard()); // NCR beat dummy
					}
					else
					{
						// NCR-122 Pick card to force higher card from dummy
						if(!dummyTrumps.IsVoid() && (m_pLHOpponent == pDummy) && dummySuit.IsVoid() 
							&& (((trumpSuit.GetBottomCard()->GetFaceValue() < dummyTrumps.GetBottomCard()->GetFaceValue()))
							       // NCR-378 Force to play higher card here if two equal cards
							    || (trumpSuit.GetSequence2(0).GetNumCards() > 1)) )
						{
							// NCR-122 Which card to get???
							pCard = trumpSuit.GetHighestCardBelow(dummyTrumps.GetTopCard()); // NCR-122
							if(dummyTrumps.GetNumSequences() > 1)  // Get card above 2nd lowest
							{
								CCard * pCard2 = trumpSuit.GetLowestCardAbove(dummyTrumps.GetSequence(1).GetTopCard());
								if(pCard2 != NULL)  pCard = pCard2;  // NCR-378 make sure valid
							}

						}
						else
							pCard = trumpSuit.GetBottomCard();
					}
					status << "PLDF55! With no cards in " & SuitToString(nSuitLed) & 
							  ", trump with the " & pCard->GetName() & ".\n";
				}
			}
			else
			{
				// here we have zero cards in the suit and in trumps, so we're hosed
				pCard = GetDiscard();
				status << "PLDF60! With no cards in the suit led " & 
						  (ISSUIT(nTrumpSuit)? "and no trumps" : "") &
						  ", we discard the " & pCard->GetName() & ".\n";
			}
		}
	}
	//
	ASSERT(pCard->IsValid());
	ASSERT(m_pHand->HasCard(pCard));
	//
	return pCard;
}





//
// PlayFourth()
//
CCard* CDefenderPlayEngine::PlayFourth()
{
	return CPlayEngine::PlayFourth();
}





//
// GetPlayHint()
//
CCard* CDefenderPlayEngine::GetPlayHint()
{
	return PlayCard();
}





