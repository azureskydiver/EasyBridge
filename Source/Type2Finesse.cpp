//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Type2Finesse.cpp
//
// Type II Finesse
// - lead of the finesse card, when a lower card is held in the same hand, 
//   towards the opposite hand, which holds a cover card 
//   e.g., K42 / AJT -- lead the J, cover if West plays Q; the T is 
//                      then good; else, finesse the J against East
//
#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "Player.h"
#include "Card.h"
#include "CardList.h"
#include "Type2Finesse.h"
#include "PlayEngine.h"
#include "CombinedHoldings.h"
#include "CardLocation.h"
#include "GuessedHandHoldings.h"
#include "PlayerStatusDialog.h"



//
//==================================================================
// constructon & destruction

CType2Finesse::CType2Finesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, CCardList* pCoverCards, int nSuit, int nCardVal) :
		CFinesse(CFinesse::TYPE_II, nTargetHand, nPlayerPosition, pGapCards, nSuit, nCardVal),
		m_pCoverCards(pCoverCards)

{
	m_pOrKeyCardsList = pCoverCards;
	Init();
}

CType2Finesse::CType2Finesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, CCardList* pCoverCards, CCard* pCard) :
		CFinesse(CFinesse::TYPE_II, nTargetHand, nPlayerPosition, pGapCards, pCard),
		m_pCoverCards(pCoverCards)

{
	VERIFY(pCard);
	Init();
}

CType2Finesse::~CType2Finesse() 
{
}


//
void CType2Finesse::Clear()
{
	CFinesse::Clear();
}


//
void CType2Finesse::Init()
{
	// call base class
//	CFinesse::Init();

	// check the enemy and location
/*
	m_nTarget = (m_nEndingHand == CFinesse::IN_DUMMY)? AGAINST_RHO: AGAINST_LHO;
	if (m_nTarget == AGAINST_LHO)
		m_nTargetPos = GetNextPlayer(m_nPlayerPosition);
	else
		m_nTargetPos = GetPrevPlayer(m_nPlayerPosition);
*/
	m_nTarget = AGAINST_BOTH;

	//
	m_pOrKeyCardsList = m_pCoverCards;
}



//
CString CType2Finesse::GetFullDescription()
{
	CString strText;
	int nTrumpSuit = pDOC->GetTrumpSuit();
	if (m_pCoverCards->GetNumCards() > 1)
		return FormString("Lead the %s %s from %s and finesses it, with the { %s } in %s as possible cover cards.",
						   ((m_nSuit == nTrumpSuit)? "trump" : STSS(m_nSuit)),
						   m_pConsumedCard->GetFaceName(),
						   ((m_nTargetHand == IN_HAND)? "dummy" : "hand"), // NCR-421 "hand" : "dummy"),
						   m_pCoverCards->GetHoldingsString(),
						   ((m_nTargetHand == IN_HAND)? "hand" : "dummy") /* NCR-421 "dummy" : "hand")*/);
	// done
	else
		return FormString("Lead the %s %s from %s and finesse it, with the %s in %s as cover.",
						   ((m_nSuit == nTrumpSuit)? "trump" : STSS(m_nSuit)),
						   m_pConsumedCard->GetFaceName(),
						   ((m_nTargetHand == IN_HAND)? "dummy" : "hand"), // NCR-421 "hand" : "dummy"),
						   m_pCoverCards->GetAt(0)->GetFaceName(),
						   ((m_nTargetHand == IN_HAND)? "hand" : "dummy") /* NCR-421 "dummy" : "hand")*/);
	// done
	return strText;
}





//
// Perform()
//
PlayResult CType2Finesse::Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
						   CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, 
	 					   CPlayerStatusDialog& status, CCard*& pPlayCard)
{
	// Type II Finesse
	// - lead of the finesse card, when a lower card is held in the same hand, 
	//   towards the opposite hand, which holds a cover card 
	//   e.g., K42 / AJT -- lead the J, cover if West plays Q; the T is 
	//                      then good; else, finesse the J against East

	// check which hand this is
	int nOrdinal = pDOC->GetNumCardsPlayedInRound();
	CPlayer* pPlayer = playEngine.GetPlayer();
	BOOL bPlayingInHand = (pDOC->GetCurrentPlayer() == pPlayer);
	CHandHoldings& playerHand = *(combinedHand.GetPlayerHand());
	CHandHoldings& dummyHand = *(combinedHand.GetPartnerHand());
	CSuitHoldings& playerSuit = playerHand.GetSuit(m_nSuit);
	CSuitHoldings& dummySuit = dummyHand.GetSuit(m_nSuit);
	CCard* pCardLed = pDOC->GetCurrentTrickCardByOrder(0);
	int nSuitLed = NONE;
	if (pCardLed)
		nSuitLed = pCardLed->GetSuit();
	// see if a trump was played in this round
	BOOL bTrumped = FALSE;
	if ((nSuitLed != pDOC->GetTrumpSuit()) && (pDOC->WasTrumpPlayed()))
		bTrumped = TRUE;
	pPlayCard = NULL;
	CCard* pOppCard = NULL;
	BOOL bLeading = TRUE;
	CString strRHO = bPlayingInHand? playEngine.szRHO : playEngine.szLHO;

	// test preconditions
	if (!CPlay::IsPlayUsable(combinedHand, playEngine))
	{
		m_nStatusCode = PLAY_INACTIVE;
		return PLAY_POSTPONE;
	}

	// check our position in the play
	switch(nOrdinal)
	{
		case 1:
			// playing second -- see if we can still finesse
			// OK if the card led is lower than our finesse card
			if ((nSuitLed == m_nSuit) && (*pCardLed < *m_pConsumedCard))
			{
				status << "3PL2FN30! LHO has led the " & pCardLed->GetName() & 
						  ", which is the same suit as our " & 
						  m_pConsumedCard->GetName() & 
						  " finesse card and lower in value, so we can still try the finesse.\n";
				bLeading = FALSE;
			}
			else
			{
				// no can do
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			// else drop into the case below

		case 0:
			// we're leading, player #0
			if (bPlayingInHand) 
			{
				// leading form our own hand (declarer)
				// see where the finese card is located
				// NCR-421 Lead comes from starting hand towards target hand
				if (m_nTargetHand == IN_HAND)  // NCR-421 was DUMMY) Lead is from starting hand
				{
					// can't finesse here
					status << "4PL2FNS10! Can't use this (Type II) finesse " &
							  (bLeading? "leading" : "playing") & 
							  " from hand, as the finesse card (" &
							  m_pConsumedCard->GetName() & ") has to be led from dummy.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_POSTPONE;
				}
				else
				{
					// finessing from hand, so lead the finesse card
					pPlayCard = m_pConsumedCard;
					status << "PL2FN12! " & (bLeading? "Lead" : "Play") & 
							  " the finesse card (the " &
							  pPlayCard->GetFaceName() & ") from hand.\n";
				}
			}
			else
			{
				// leading from dummy
				if (m_nTargetHand == IN_DUMMY) // NCR-421 was HAND) Lead is from starting hand
				{
					// leading from dummy & finessing in hand? no can do
					status << "4PL2FNS20! Can't use this (Type II) finesse " &
							  (bLeading? "leading" : "playing") & 
							  " from dummy, as the finesse card (" &
							  m_pConsumedCard->GetName() & ") has to be led from hand.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_POSTPONE;
				}
				else 
				{
					// finessing from dummy, so OK
					pPlayCard = m_pConsumedCard;
					status << "PL2FN22! " & (bLeading? "Lead" : "Play") & 
							  " the finesse card (the " &
							  pPlayCard->GetFaceName() & ") from dummy.\n";
				}
			}
			// all went OK
			m_nStatusCode = PLAY_IN_PROGRESS;
			break;

		case 2:
			// playing third -- this is the key to the finesse
			// make sure the play is in progress
			if (m_nStatusCode != PLAY_IN_PROGRESS)
				return PLAY_INACTIVE;
			// see if the wrong suit was led 
			if (nSuitLed != m_nSuit)
			{
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			// see if LHO/RHO trumped
			if (bTrumped)
			{
				status << "3PL2FN50! the opponent has trumped, so abandon the finesse.\n";
				m_nStatusCode = PLAY_NOT_VIABLE;
				return PLAY_NOT_VIABLE;
			}
			// check the intervening opponents's card
			pOppCard = pDOC->GetCurrentTrickCardByOrder(1);
			// check if RHO showed out
			if (pOppCard->GetSuit() != nSuitLed)
			{
				// RHO has showed out, so skip the finesse play
				status << "3PL2FN55! Oops -- RHO (" & strRHO &
						  ") showed out of " & STS(nSuitLed) & ", meaning that LHO holds the " & 
						  m_pGapCards->GetAt(0)->GetFaceName() & ", so the finesse cannot succeed -- so skip it.\n";
				m_nStatusCode = PLAY_NOT_VIABLE;
				return m_nStatusCode;
			}
			// check which hand we're playing in
			if (bPlayingInHand) 
			{
				// playing third from our own hand (declarer)
				// make sure the finessee card was led from dummy
				if (m_nTargetHand == IN_HAND) // NCR-421 was IN_DUMMY)
				{
					// see if RHO's card is higher than the intended finesse
					if (*pOppCard > *m_pConsumedCard)
					{
						// if so, play a cover card
//						pPlayCard = playerSuit.GetLowestCardAbove(pOppCard);
						pPlayCard = playerSuit.GetTopSequence().GetBottomCard();
						status << "PL2FN62! RHO has played a higher card (the " & 
								  pOppCard->GetFaceName() & ") than our finesse card (the " &
								  m_pConsumedCard->GetFaceName() & "), so cover with the " &
								  pPlayCard->GetFaceName() & ".\n";
					}
					else
					{
						// else discard
// NCR-458						pPlayCard = playEngine.GetDiscard();
						pPlayCard = playerSuit.GetBottomCard();  // NCR-458  Discard lowest card vs ???
						status << "PL2FN64! RHO has played low, so let the finesse card ride and discard the " 
							      & pPlayCard->GetName() & " from hand.\n";
					}
				}
				else
				{
					// ended up in the wrong hand
					status << "4PL2FN66! We intended to finesse in hand, but ended up here in dummy in third position -- so skip this play.\n";
					m_nStatusCode = PLAY_NOT_VIABLE;
					return PLAY_NOT_VIABLE;
				}
			}
			else
			{
				// playing third in dummy
				// make sure our finesse was from hand
				if (m_nTargetHand == IN_DUMMY) // NCR-421 wasIN_HAND)
				{
					// see if LHO's card is higher than the intended finesse
					if (*pOppCard > *m_pConsumedCard)
					{
						// if so, play a cover card
//						pPlayCard = dummySuit.GetLowestCardAbove(pOppCard);
						pPlayCard = dummySuit.GetTopSequence().GetBottomCard();
						status << "PL2FN72! LHO has played a higher card (the " & 
								  pOppCard->GetFaceName() & ") than our finesse card (the " &
								  m_pConsumedCard->GetFaceName() & "), so cover with the " &
								  pPlayCard->GetFaceName() & ".\n";
					}
					else
					{
						// else discard
// NCR-458						pPlayCard = playEngine.GetDiscard();
						pPlayCard = dummySuit.GetBottomCard();  // NCR-458  Discard lowest card vs ???
						status << "PL2FN64! LHO has played low, so let the finesse card ride and discard the " 
							      & pPlayCard->GetName() & " from hand.\n";
					}
				}
				else
				{
					// messed up
					status << "4PL2FN76! We intended to finesse from dummy, but ended up here in dummy in third position -- so skip this play.\n";
					m_nStatusCode = PLAY_NOT_VIABLE;
					return PLAY_NOT_VIABLE;
				}
			}
			// all went OK
			m_nStatusCode = PLAY_COMPLETE;
			break;

		case 3:
			// make sure the play is in progress
			if (m_nStatusCode != PLAY_IN_PROGRESS)
				return PLAY_INACTIVE;
			// we may have played the finesse card in second position
			// and ended up in the opposite hand in 4th position
			if ( (pDOC->GetCurrentTrickCardByOrder(1) != m_pConsumedCard) ||
				 ((bPlayingInHand) && (m_nTargetHand == IN_HAND)) ||
				 ((!bPlayingInHand) && (m_nTargetHand != IN_HAND)) )
			{
				// no go
				m_nStatusCode = PLAY_NOT_VIABLE;
				return PLAY_NOT_VIABLE;
			}
			//
			status << "3PL2FN80! We played the finesse card in second position, so check the results.\n";
			// check the intervening opponents's card
			pOppCard = pDOC->GetCurrentTrickCardByOrder(2);
			// and cover if necessary
			if (bPlayingInHand) 
			{
				// ended up in hand in 4th spot -- cover if 3rd hand played high
				if ((pOppCard->GetSuit() == nSuitLed) && (*pOppCard > *m_pConsumedCard))
				{
					// cover RHO
					pPlayCard = playerSuit.GetLowestCardAbove(pOppCard);
					status << "PL2FN80! RHO topped our finesse card, so cover it with the " &
							  pPlayCard->GetFaceName() & ".\n";
				}
				else
				{
					// else discard
					pPlayCard = playEngine.GetDiscard();
					status << "PL2FN82! Our finesse card (the " & 
							  m_pConsumedCard->GetFaceName() & " has held, so discard the " &
							  pPlayCard->GetName() & ".\n";
				}
			}
			else
			{
				// ending up in dummy in 4th spot -- cover if 3rd hand played high
				if ((pOppCard->GetSuit() == nSuitLed) && (*pOppCard > *m_pConsumedCard))
				{
					// cover RHO
					pPlayCard = dummySuit.GetLowestCardAbove(pOppCard);
					status << "PL2FN90! LHO topped our finesse card, so cover it with the " &
							  pPlayCard->GetFaceName() & ".\n";
				}
				else
				{
					// else discard
					pPlayCard = playEngine.GetDiscard();
					status << "PL2FN92! Our finesse card (the " & 
							  m_pConsumedCard->GetFaceName() & " has held, so discard the " &
							  pPlayCard->GetName() & ".\n";
				}
			}
			// all went OK
			m_nStatusCode = PLAY_COMPLETE;
			break;
		
	}

	// done
	ASSERT(pPlayCard->IsValid());
	return m_nStatusCode;
}
