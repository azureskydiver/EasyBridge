//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

/=====================================================================
//
// CDefenderPlayEngine
//
// - encapsulates the defender's play engine and its status variables
//
//=====================================================================


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
	for(int i=0;i<4;i++)
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
			// check if partner played high (above a six)
			// this is the first indication of a suit preference
			if (pCard->GetFaceValue() > 6)
			{
				status << "2PLYSIG0! Partner discarded high with the " & pCard->GetFaceName() &
						  ", so consider it a suit preference signal.\n";
				m_nPartnerSuitPreference[nSuitLed] = 1;
			}
			else
			{
				status << "2PLYSIG1! Partner discarded low with the " & pCard->GetFaceName() &
						  ", so consider it a signal that he dislikes the suit.\n";
				m_nPartnerSuitPreference[nSuitLed] = -1;
			}
		}
		else if ((nRound >= 1) && ((m_nPartnerSuitPreference[nSuitLed] == 1) || (m_nPartnerSuitPreference[nSuitLed] == -1)) )
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
				int nPrevCardVal = pPrevCard->GetFaceValue();
				if ( (m_nPartnerSuitPreference[nSuitLed] == 1) &&
								(pCard->GetFaceValue() < nPrevCardVal) )
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
		 (nPartnerSuit != pCardLed->GetSuit()) &&
		 (m_ppGuessedHands[m_nPartnerPosition]->GetSuit(nPartnerSuit).GetNumCardsPlayed() == 0) &&
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
	int nRound = pDOC->GetPlayRound();
	int nTrumpSuit = pDOC->GetTrumpSuit();
	int nContractLevel = pDOC->GetContractLevel();
	CCard* pLeadCard = NULL;
	int nPlayerPos = GetPlayerPosition();
	int nPartnerPos = GetPartnerPosition();
	int nPartnersSuit = NONE;
	int	nDeclarer = pDOC->GetDeclarerPosition();
	int nDummy = pDOC->GetDummyPosition();

	// see if this is the first round
	if (nRound == 0)
	{
		// this is the opening lead
		status << "2PLYLEAD! Making opening lead as " & PositionToString(nPlayerPos) &
				  " against a contract of " & ContractToFullString(pDOC->GetContract()) & ".\n";
		// review partner's bidding history
		nPartnersSuit = ReviewBiddingHistory();

		// special code -- see if we can beat a slam contract in our hand
		if (nContractLevel == 7)
		{
			// do we have an Ace?
			if (m_pHand->GetNumAces() >= 1)
			{
				for(int nSuit=CLUBS;nSuit<=SPADES;nSuit++)
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
				int nSuit, nIndex = 3;
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
			int nSuit, nIndex = 3;
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
				for(int i=0;i<4;i++)
				{
					CSuitHoldings& suit = m_pHand->GetSuit(i);
					if ((suit.GetNumCards() == 1) && (suit[0]->GetFaceValue() < JACK) && (i != nTrumpSuit))
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
					// lead LOW with 3+ cards
					pLeadCard = partnersSuit.GetBottomCard();
					status << "PLYLD4! Lead the bottom card of partner's " & STSS(nPartnersSuit) & 
							  " suit (the " & pLeadCard->GetFaceName() & ".\n";
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
					pLeadCard = seq[0];
					status << "PLYLD15! Lead the " & pLeadCard->GetName() &  
							  " from the top of the { " & seq.GetHoldingsString() & 
							  "} sequence.\n";
					ASSERT(m_pHand->HasCard(pLeadCard));
					return pLeadCard;
				}
			}

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

			// lead a trump if appropriate (add code here later)

			// here, no other lead was found, so simply lead 4th best
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
			else
			{
				// lead fourth-best from this suit OR our longest suit
				CCard* pLeadCard = Get4thBestLeadCard();
				m_nPrioritySuit = pLeadCard->GetSuit();
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
				int nSuit, nIndex = 3;
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
			if (m_pHand->GetNumKings() >= 1)
			{
				// lead Aces from the shortest suit first
				int nSuit, nIndex = 3;
				while(nIndex >= 0)
				{
					nSuit = m_pHand->GetSuitsByLength(nIndex);
					if (m_pHand->GetSuit(nSuit).HasKing())
						break;
					nSuit = m_pHand->GetSuitsByLength(nIndex);
					nIndex--;
				}
				pLeadCard = m_pHand->GetSuit(nSuit).GetTopCard();
				if (ISSUIT(nTrumpSuit))
					status << "PLYLDS6a! Try to defeat the contract now by playing the " & pLeadCard->GetName() & ".\n";
				else
					status << "PLYLDS6b! Defeat the contract now by playing the " & pLeadCard->GetName() & ".\n";
				return pLeadCard;
			}
		}

		// if we're leading in the second or later round after winning 
		// the prior round which partner led, return partner's suit
		CCard* pPartnersLastCard = pDOC->GetGameTrickCard(nRound-1, nPartnerPos);
		int nSuit = pPartnersLastCard->GetSuit();
		int nLead = pDOC->GetGameTrickLead(nRound-1);
		if ( (pDOC->GetGameTrickWinner(nRound-1) == nPlayerPos) &&
						  (pDOC->GetGameTrickLead(nRound-1) == nPartnerPos) &&
						  (m_pHand->GetNumCardsInSuit(nSuit) > 0) )
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

			// also don't bother returning the suit if this is a suit contract 
			// and dummy has the top cards outstanding -- i.e., if dummy's 
			// top card is higher than any outstanding cards
			CCardList outstandingCards;
			int numOutstandingCards = GetOutstandingCards(nSuit, outstandingCards); 
			if ((ISSUIT(nTrumpSuit)) && (dummyHand.GetNumCardsInSuit(nSuit) > 0) &&
					(numOutstandingCards > 0) &&
					(dummyHand.GetSuit(nSuit).GetTopCardVal() > outstandingCards[0]->GetFaceValue()))
			{
				status << "3PLYLDRT2! We'd like to return partner's " & STS(nSuit) &
					      " suit, but dummy will most likely win.\n";
				bDummyCanTrump = TRUE;
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

			// return a winner in this suit if we have one; else return a low card
			CSuitHoldings& suit = m_pHand->GetSuit(nSuit);
			if ((suit.GetNumCards() > 0) && (!bDummyCanTrump) && (!bDeclarerCanTrump))
			{
				// mark this as the priority suit
				m_nPrioritySuit = nSuit;
				if (suit.GetNumWinners() > 0)
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
			int numOutstandingTrumps = GetNumOutstandingCards(nTrumpSuit);
			if ( (numOutstandingTrumps > 0) && 
				 (!IsPartnerVoidInSuit(nTrumpSuit)) && (numPartnerVoidSuits > 0))
			{
				// see if we can lead the suit
				for(int j=0;j<numPartnerVoidSuits;j++)
				{
					CSuitHoldings& suit = m_pHand->GetSuit(nPartnerVoidSuits[j]);
//					if (suit.GetNumLosers() > 0)
					if ((suit.GetNumCards() > 0) && (suit.GetNumWinners() == 0))
					{
						// by all means, lead the suit
						pLeadCard = suit.GetBottomCard();
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

			// no luck in finding suit for partner, so pick our own suit to lead
			// if all trumps have been accounted for, cash winners
			// but not if it's a trump winner -- save it for later
			pLeadCard = CashWinners();
			if (pLeadCard && pLeadCard->GetSuit() != nTrumpSuit)
			{
				ASSERT(m_pHand->HasCard(pLeadCard));
				return pLeadCard;
			}

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

			// check each suit
			for(int j=0;j<4;j++)
			{
				if (j == nTrumpSuit)
					continue;
				CSuitHoldings& suit = m_pHand->GetSuit(j);
				if (suit.GetNumCards() == 0)
					continue;
				CGuessedSuitHoldings& declarerSuit = m_pPlayer->GetGuessedHand(nDeclarer)->GetSuit(j);
				CSuitHoldings& dummySuit = dummyHand.GetSuit(j);
				int numOutstandingCards = GetNumOutstandingCards(j);
				// lead a suit if declarer/dummy has not shown out of the suit
				if (!declarerSuit.IsSuitShownOut() && (dummySuit.GetNumCards() > 0) &&
									(numOutstandingCards >= 1) )
				{
					pLeadCard = suit.GetBottomCard();
					status << "PLYLD42! With no good card to lead, lead the " & pLeadCard->GetName() &
							  " since the opponents have not shown out of the " & STSS(pLeadCard->GetSuit()) &
							  " suit.\n";
					ASSERT(m_pHand->HasCard(pLeadCard));
					return pLeadCard;
				}
			}

			// else lead a suit that dummy won't ruff
			for(j=0;j<4;j++)
			{
				CSuitHoldings& suit = m_pHand->GetSuit(j);
				if (suit.GetNumCards() == 0)
					continue;
				CSuitHoldings& dummySuit = dummyHand.GetSuit(j);
				int numOutstandingCards = GetNumOutstandingCards(j);
				if ((dummySuit.GetNumCards() > 0) && (numOutstandingCards >= 1))
				{
					pLeadCard = suit.GetBottomCard();
					status << "PLYLD43! With no good card to lead, lead the " & pLeadCard->GetName() &
							  " since dummy is not void in the " & STSS(pLeadCard->GetSuit()) &
							  " suit (we don't know about declarer, though).\n";
					ASSERT(m_pHand->HasCard(pLeadCard));
					return pLeadCard;
				}
			}

		}
		else
		{
			// playing in notrump
			// see if partner has expressed a suit preference and has not shown out
			// of that suit
			pLeadCard = FindLeadCardFromPartnerPreference();
			if (!ISSUIT(m_nPrioritySuit) && pLeadCard && !IsPartnerVoidInSuit(pLeadCard->GetSuit()))
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
						 status << "4PLYLD50! Since both partner and " & (bLHOShowOut? "LHO" : "RHO") &
								   " have shown out, there is no point in trying to keep developing the " &
								   STSS(m_nPrioritySuit) & " suit, as we'll only provide entries for the oponents.\n";
						 m_nPrioritySuit = NONE;
					 }
				}
			}

			// if we're still trying to develop or work a suit, 
			// keep playing the suit
			int nSuit = NONE;
			if (ISSUIT(m_nPrioritySuit))
				nSuit = m_nPrioritySuit;
			else if (ISSUIT(m_nPartnersPrioritySuit) && (m_pHand->GetSuit(m_nPartnersPrioritySuit).GetNumCards() >= 1))
				nSuit = m_nPartnersPrioritySuit;
			if (ISSUIT(nSuit))
			{
				CSuitHoldings& suit = m_pHand->GetSuit(nSuit);
				pLeadCard = suit[0];
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
				(suit.GetNumHonors() >= 2) && (nRound <= 6))
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
		}
	}


	// if all else fails, give up and call the base class
	// TODO: specialize the code from the base class to pick the suit that dummy
	// is weakest in
	if (pLeadCard == NULL)
		pLeadCard = CPlayEngine::GetLeadCard();

	// done
	return pLeadCard;
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
	int numBiddingRounds = numBidsMade / 4;

	// we have the # of bidding rounds (rounded up)
	// check partner's bids for each round
	int nPartner = GetPartnerPosition();
	int nTopSuit = NONE;
	int nTopSuitVal = 0;
	for(int i=0;i<4;i++)
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
	int nDummy = pDOC->GetDummyPosition();
	CCard* pLeadCard = NULL;

	//
	for(int i=0;i<4;i++)
	{
		// check the suit -- partner must've signalled favorably, must not have
		// shown out of the suit, and _we_ must have cards in the suit
		CSuitHoldings& suit = m_pHand->GetSuit(i);
		if ( (m_nPartnerSuitPreference[i] > 0) && (suit.GetNumCards() > 0) &&
			 (!IsPartnerVoidInSuit(i)) )
		{
			// check if we're in a suit contract
			if (ISSUIT(nTrumpSuit))
			{
				// don't bother if dummy is void in the suit and has trumps
				CGuessedHandHoldings* pDummyHand = m_pPlayer->GetGuessedHand(nDummy);
				if (pDummyHand->IsSuitShownOut(i) && 
					(pDummyHand->GetSuit(nTrumpSuit).GetNumRemainingCards() > 0) )
				{
					// dummy will ruff, so don't bother with this suit
					status << "3PLYLDPRF1! Partner likes the " & STSS(i) & 
							  " suit, but dummy can ruff, so skip this suit for now.\n";
					continue;
				}

				// see if we think declarer has trumps in the suit
				int nDeclarer = pDOC->GetDeclarerPosition();
				CGuessedHandHoldings* pDeclarerHand = m_pPlayer->GetGuessedHand(nDeclarer);
				if ( pDeclarerHand->IsSuitShownOut(i) && 
					((pDeclarerHand->GetSuit(nTrumpSuit).GetNumRemainingCards() > 0) ||
					 (GetNumOutstandingCards(nTrumpSuit) > 0)) )
				{
					// declarer may ruff, so don't bother with this suit
					status << "3PLYLDPRF2! Partner likes the " & STSS(i) & 
							  " suit, but declarer has shown out of the suit and _may_ be able to ruff, so avoid leading the suit if possible.\n";
					continue;
				}

				// else we can lead the suit
				// play a winner if we have one, else lead low
				if (suit.GetNumWinners() > 0)
					pLeadCard = suit[0];
				else
					pLeadCard = suit.GetBottomCard();
				status << "PLYLDPRF4! Partner has indicated a preference for the " & STS(i) &
						  " suit, so lead the " & pLeadCard->GetName() & ".\n";
				return pLeadCard;
			}
			else
			{
				// notrump contract
				// see if dummy holds the top two cards in the suit
				ASSERT(pDOC->GetDummyPlayer()->AreCardsExposed());
				CHandHoldings& dummyHand = pDOC->GetDummyPlayer()->GetHand();
				CSuitHoldings& dummySuit = dummyHand.GetSuit(i);
				// get the # of outstanding cards in the suit
				// (in partner's or declarer's hands)
				CCardList outstandingCards;
				int numOutstandingCards = GetOutstandingCards(i, outstandingCards);
				if (numOutstandingCards == 0)
					continue;	// partner has no cards in this suit

				// add our own cards to the list of cards to consider
				int numOwnCards = m_pHand->GetSuit(i).GetNumCards();
				for(int j=0;j<numOwnCards;j++)
				{
					outstandingCards << m_pHand->GetSuit(i).GetAt(j);
					numOutstandingCards++;
				}

				// see if dummy has the top two cards in the suit
				if ((dummySuit.GetNumCards() >= 2) && 
					(*dummySuit[0] > *outstandingCards[0]) && (*dummySuit[1] > *outstandingCards[1]))
				{
					status << "3PLYLDPRF12! Partner likes the " & STSS(i) & 
							  " suit, but dummy holds the top two cards in the suit, so prefer to skip it.\n";
					continue;
				}
				
				// play a winner if we have one in the suit, else lead low
				if (suit.GetNumWinners() > 0)
					pLeadCard = suit[0];
				else
					pLeadCard = suit.GetBottomCard();
				status << "PLYLDPRF8! Partner has indicated a preference for the " & STS(i) &
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
CCard* CDefenderPlayEngine::Get4thBestLeadCard()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	CSuitHoldings& suit = m_pHand->GetSuit(m_pHand->GetSuitsByPreference(0));
	int nTrumpSuit = pDOC->GetTrumpSuit();
	CCard* pLeadCard = NULL;

	// see if we have 4+ cards in our best suit 
	if (suit.GetNumCards() >= 4)
	{
		// make sure this is not the trump suit
		if (suit.GetSuit() != nTrumpSuit)
		{
			// lead 4th best of the suit
			pLeadCard = suit[3];
			status << "PLYLD4A! Lead the fourth card from our best suit (" & STS(suit.GetSuit()) &
					   & ") -- play the " & pLeadCard->GetName() & ".\n";
		}
		else
		{
			// hmm, our best suit is the trump suit -- see if we have another
			// 4-card suit
			int nOtherSuit = m_pHand->GetSuitsByPreference(1);
			if (m_pHand->GetNumCardsInSuit(nOtherSuit) >= 4)
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
		ASSERT(longSuit.GetNumCards() >= 4);
		pLeadCard = longSuit[3];
		status << "PLYLD4N! Lead the fourth card from our longest suit (" & STS(nLongSuit) &
				  ") -- play the " & pLeadCard->GetName() & ".\n";
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
				continue;

			// here, we found a suit with top card winners
			// but check dummy's hand (OK since it's exposed by now)
			int nDummy = pDOC->GetDummyPosition();
			CPlayer* pPlayer = pDOC->GetDummyPlayer();
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
			}
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
	for(int i=0;i<4;i++)
	{
		// find the first non-trump suit with winners
		if (( bSuitIsSafe[i]) && (m_pHand->GetSuit(nSuit).GetNumTopCards() > 0) )
			break;
		// else advance to the next suit
		nSuit = GetNextSuit(nSuit);
	}
	
	// make sure we found a suit!
	// if we didn't then we dont't have any top card winners
	if (i == 4)
		return NULL;

	// new code -- don't cash trump winners
	if (nSuit == nTrumpSuit)
		return NULL;

	// so play the card
	CSuitHoldings& suit = m_pHand->GetSuit(nSuit);
	pLeadCard = suit.GetTopSequence().GetBottomCard();
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
	int nOrdinal = pDOC->GetNumCardsPlayedInRound();
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
				if (suit.GetNumCards() >= 2)
					status << "PLDIS8! Begin the high-low signal with the " & pDiscard->GetFaceName() & ".\n";
				m_nSignalledSuit = nSuitLed;
			}
			else
			{
				// the suit is not preferred, so discard low
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
		if (prefSuit.GetNumWinners() > 0)
		{
			if (prefSuit.GetNumSequences() > 1)
			{
				// see if the top card of the second sequence is higher than a 6
				if (prefSuit.GetSecondSequence().GetTopCard()->GetFaceValue() > 6)
				{
					// if so, return the top card of the second sequence
					pDiscard = prefSuit.GetSecondSequence().GetTopCard();
				}
				else
				{
					// else just return the bottom card of the top sequence
					pDiscard = prefSuit.GetTopSequence().GetBottomCard();
				}
			}
			else
			{
				// there's only one sequence in the suit, so return the bottom card
				pDiscard = prefSuit.GetBottomCard();
			}
			//
			status << "PLDIS40! Indicate a preference for the " & STSS(nPrefSuit) &
					  " by discarding high with the " & pDiscard->GetName() & ".\n";
		}
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
		for(int i=3;i>=0;i--)
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
					if ((hand.GetNumCardsInSuit(nSuit) > 0) && (nSuit != nTrumpSuit))
					{
						pDiscardSuit = &hand.GetSuit(nSuit);
						break;
					}
				}
			}
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
		for(int i=0;i<4;i++)
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
		for(i=0;i<4;i++)
		{
			// check out the suit
			int nSuit = nSuitsByLosers[i];
			if (hand.GetNumCardsInSuit(nSuit) == 0)
				continue;

			// but see if this is the priority suit, or the suit has zero losers
			CSuitHoldings& suit = hand.GetSuit(nSuit);
			int numLosers = suit.GetNumLosers();
			if ( (ISSUIT(nSuit) && ((nSuit == m_nPrioritySuit)) || (numLosers == 0)) )
			{
				// don't discard from this suit if possible;
				continue;
			}

			// test this next suit for winners
			CSuitHoldings& testSuit = hand.GetSuit(nSuit);
			numLosers = testSuit.GetNumLosers();
			if (numLosers > 0)
			{
				pDiscardSuit = &hand.GetSuit(nSuit);
				break;	   // found one
			}
		}
		
		// if we still found nothing, call the base class
		if (pDiscardSuit == NULL)
			pDiscardSuit = &hand.GetSuit(PickFinalDiscardSuit(hand));

		// and get the card
		pDiscard = pDiscardSuit->GetBottomCard();
	}

	//
	// verify the selected card is OK
	//
	ASSERT(pDiscard->IsValid());
	ASSERT(hand.HasCard(pDiscard));
	return pDiscard;
}




//
// GetHighDiscard()
//
// select a discard that's high but not too high
// 
CCard* CDefenderPlayEngine::GetHighDiscard(CSuitHoldings& suit)
{ 
	// first see if the suit has a lot of high cards
	CCard* pHighCard = NULL;
	if (suit.GetNumCardsAbove(6) > 1)
	{
		// multiple high cards -- see if we have one that's not a winner
		int numHighCards = suit.GetNumCardsAbove(6);
		int numLikelyWinners = suit.GetNumLikelyWinners();
		if (numHighCards > numLikelyWinners)
		{
			// we have excess high cards
			pHighCard = suit.GetLowestCardAbove(6);
		}
		else
		{
			// nothing but likely winners! so return something lower if possible
			pHighCard = suit.GetHighestCardBelow(10);
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
	int nDeclarerPos = pDOC->GetDeclarerPosition();
	int nDummyPos = pDOC->GetDummyPosition();
	CPlayer* pDummy = pDOC->GetDummyPlayer();
	CCard* pCardLed = pDOC->GetCurrentTrickCardLed();
	int nSuitLed = pCardLed->GetSuit();
	int nFaceValue = pCardLed->GetFaceValue();
	CCard* pCurrTopCard = pDOC->GetCurrentTrickHighCard();
	int nTrumpSuit = pDOC->GetTrumpSuit();
	CSuitHoldings& suit = m_pHand->GetSuit(nSuitLed);
	// card to play
	CCard* pCard = NULL;

	// determine the top card -- it's either the one led by declarer 
	// or dummy's top holding in the suit, if dummy is to our left
	CCard* pTopCard;
	CHandHoldings& dummyHand = GetDummyHand();
	CSuitHoldings& dummySuit = dummyHand.GetSuit(nSuitLed);
	int numDummySuitCards = dummySuit.GetNumCards();
	if ((m_pLHOpponent == pDummy) && (numDummySuitCards > 0) && (*dummySuit[0] > *pCardLed))
		pTopCard = dummySuit[0];
	else
		pTopCard = pCardLed;

	//
	// we play second hand low, usually but not always
	// see how many cards we have in the suit
	//
	int numCardsInSuitLed = suit.GetNumCards();
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
		if ((pCardLed->GetFaceValue() >= 10) && (*suit[0] > *pCardLed))
		{
			pCard = suit.GetTopSequence().GetBottomCard();
			status << "PLDF2E! " & szRHO & " led an honor (the " & pCardLed->GetFaceName() & 
					  "), so cover it with our " & pCard->GetFaceName() & ".\n";
			ASSERT(pCard->IsValid());
			return pCard;
		}

		// play the bottom card of an honor sequence to promote the other honors
		// the top card of our top sequence must be higher than the card led
		// (but lower than the top card in dummy)
		CCardList& topSeq = suit.GetTopSequence();
		if (m_pLHOpponent == pDummy)
		{
			// playing ahead of dummy -- play the bottom honor of the sequence
			if ((topSeq.GetNumCards() > 1) && IsHonor(topSeq[0]->GetFaceValue()) && (*topSeq[0] > *pCardLed) &&
				(*topSeq[0] < *pTopCard))
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

		// if playing ahead of dummy, and dummy has only a single card in the 
		// suit led and we have an Ace (plus other cards), play the Ace now 
		// lest it be trumped later (any exceptions???)
		if (ISSUIT(nTrumpSuit) && (numCardsInSuitLed > 1) && (suit[0]->GetFaceValue() == ACE) &&
				(m_pLHOpponent == pDummy) && (numDummySuitCards == 1))
		{
			// see if the opponents can indeed trump the Ace
			CGuessedHandHoldings* pDeclarerHand = m_ppGuessedHands[nDeclarerPos];
			if ((GetNumOutstandingCards(nTrumpSuit) > 0) &&
					((dummyHand.GetNumTrumps() > 0) || !pDeclarerHand->IsSuitShownOut(nTrumpSuit)) )
			{
				// declarer/dummy may have some trumps left, so take the Ace now
				pCard = suit[0];
				status << "PLDF2G1! Since dummy has only a single " & STSS(nSuitLed) &
						  " left, play our Ace now lest it be trumped later.\n";
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
		CCardList outstandingCards;
		// get the outstanding cards (also count dummy)
		int numOutstandingCards = GetOutstandingCards(nSuitLed, outstandingCards, true);
//		CHandHoldings& dummyHand = GetDummyHand();
//		CSuitHoldings& dummySuit = dummyHand.GetSuit(nSuitLed);
//		int numDummySuitCards = dummySuit.GetNumCards();
		if ((numOutstandingCards == 0) && (*suit[0] > *pCardLed))
		{
			pCard = suit.GetLowestCardAbove(pCardLed);
			status << "PLDF2G1! Since there are no more outstanding cards in the suit, win with the " &
					  pCard->GetFaceName() & ".\n";
			ASSERT(pCard->IsValid());
			return pCard;
		}

		// also if there's one other card outstanding in the suit, and we beat it also
		if ( ((numOutstandingCards == 1) && (*suit[0] > *pCardLed) && (*suit[0] > *outstandingCards[0])) )
		{
			pCard = suit.GetLowestCardAbove(outstandingCards[0]);
			status << "PLDF2G1! There's only one more card outstanding card in the suit, so win here with the " &
					  pCard->GetFaceName() & ".\n";
			ASSERT(pCard->IsValid());
			return pCard;
		}

		// else no special conditions apply -- just play low
		pCard = GetDiscard();
		if (numCardsInSuitLed > 1)
		{
			if (*pCard < *pCurrTopCard)
				status << "PLDF2H1! Play second hand low with the " & pCard->GetFaceName() & ".\n";
			else if (pCard == suit.GetBottomCard())
				status << "PLDF2H2! As second hand, play the lowest card we have in the suit, the " & pCard->GetFaceName() & ".\n";
		}
		else
		{
			status << "PLDF2C4! Play our only " & STSS(nSuitLed) & ", the " & pCard->GetFaceName() & ".\n";
		}
	}
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
			int i;
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
		}
		else
		{
			// discard
			pCard = GetDiscard();
			if (ISSUIT(nTrumpSuit))
				status << "PLDF2Y! We have no " & SuitToString(nSuitLed) & " and no trumps, so discard the " & pCard->GetName() & ".\n";
			else
				status << "PLDF2Ya! We have no " & SuitToString(nSuitLed) & ", so discard the " & pCard->GetName() & ".\n";
		}
	}
	//
	ASSERT(pCard->IsValid());
	return pCard;
}




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
	int nSuitLed = pCurrentCard->GetSuit();
	CPlayer* pDummy = pDOC->GetDummyPlayer();
	int nTopPos;
	CCard* pCurrTopCard = pDOC->GetCurrentTrickHighCard(&nTopPos);
	CString strTopCardPos = PositionToString(nTopPos);
	BOOL bPartnerHigh = FALSE;
	int nCurrentRound = pDOC->GetPlayRound();
	int nCurrentSeat = pDOC->GetNumCardsPlayedInRound() + 1;
	int nTrumpSuit = pDOC->GetTrumpSuit();
	int numCardsInSuitLed = m_pHand->GetNumCardsInSuit(nSuitLed);
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
	int numDummySuitCards = dummySuit.GetNumCards();
	// if dummy plays last (on our left), see if he holds the top card
	if ((m_pLHOpponent == pDummy) && (numDummySuitCards > 0) &&
		(pCurrTopCard->GetSuit() == nSuitLed) && (*dummySuit[0] > *pCurrTopCard))
	{
		// dummy does indeed hold the top card
		pActualTopCard = dummySuit[0];
		nTopPos = pDOC->GetDummyPosition();
		bPartnerHigh = FALSE;	// partner's no longer high (he's sober?)
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
			if ((m_pLHOpponent == pDummy) && (numDummySuitCards == 0) &&
					(dummyHand.GetNumTrumps() > 0) && 
					(*(dummyHand.GetSuit(nTrumpSuit).GetTopCard()) > *pTopTrump))
			{
				// dummy can overtrump, so discard
				pCard = GetDiscard();
				status << "PLDF10! We'd like to trump here, but dummy can overtrump, so discard the " & pCard->GetName() & ".\n";
			}
			else
			{
				// get the lowest trump that wil top the current top trump
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
			if (*(suit.GetTopCard()) > *pCurrTopCard)
			{
				// but see if the top card is partner's
				if (bPartnerHigh) 
				{
					// see if we should unblock here
					if (ISSUIT(nTrumpSuit) && (nCurrentRound == 0) &&
						(suit.GetNumHonors() == 1) && (suit.GetNumCards() == 2))
					{
						// first round in an NT contract, with one honor 
						// in the suit -- unblock
						pCard = suit.GetTopCard();
						if (suit.GetNumCards() > 1)
							status << "PLDF30! Drop the " & pCard->GetFaceName() & 
									  " here to unblock the suit for partner.\n";
					}
					else if (!ISSUIT(nTrumpSuit) && (suit.GetNumWinners() == suit.GetNumCards()))
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
							CCard* pTopOutstandingCard = GetHighestOutstandingCard(nSuitLed);
							CCard* pOurCard = suit.GetTopSequence().GetBottomCard();
							if ( (pTopOutstandingCard == NULL) || (*pCurrTopCard > *pTopOutstandingCard) ||
								 (pOurCard->GetFaceValue() <= pCurrTopCard->GetFaceValue() + 1) )
							{
								// partner's card is higher than any outstanding, or
								// else we don't have anything much higher, so let it ride
								pCard = GetDiscard();
								status << "PLDF36! Partner's " & pCurrTopCard->GetFaceName() & 
										  " is higher than any outstanding card, so discard the " & 
										  pCard->GetName() & ".\n";
							}
							else
							{
								// partner's card is not necessarily highest, so top it
								// if we can
								pCard = pOurCard;
								status << "PLDF37! Partner's " & pCurrTopCard->GetFaceName() & 
										  " might not win the round, so top it with the " & pCard->GetFaceName() & ".\n";
							}
						}
					}
				}
				else
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
								pCard = suit.GetLowestCardAbove(pCurrTopCard);
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
					else if (nCurrentSeat == 3)
					{
						// else we're not playing ahead of dummy, 
						// so play the lowest card from our top sequence
						pCard = suit.GetTopSequence().GetBottomCard();
						status << "PLDF40! Play high with the " & pCard->GetFaceName() & ".\n";
					}
				}
			}
			else
			{
				// here we don't have a card to top the current high card
				if (bPartnerHigh)
				{
					// but partner's card is high, so we're OK
					pCard = GetDiscard();
					status << "PLDF47! Partner's " & pCurrTopCard->GetFaceName() & 
							  " can win the trick, so discard the " & pCard->GetName() & ".\n";
				}
				else
				{
					// else we're screwed
					pCard = GetDiscard();
					status << "PLDF48! We can't beat " & strTopCardPos & "'s " & pCurrTopCard->GetFaceName() & 
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
				if (bPartnerHigh)
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
					pCard = m_pHand->GetSuit(nTrumpSuit).GetBottomCard();
					status << "PLDF55! With no cards in " & SuitToString(nSuitLed) & 
							  ", trump with the " & pCard->GetName() & ".\n";
				}
			}
			else
			{
				// here we have zero cards in the suit and in trumps, so we're hosed
				pCard = GetDiscard();
				status << "PLDF52! With no cards in the suit led " & 
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





