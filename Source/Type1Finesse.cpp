//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Type1Finesse.cpp
//
// Type I Finesse 
// - lead of a low card towards a higher card in the opposite hand, 
//	 which holds a higher cover card and a commanding top card.
//   e.g., AQ3 (dummy) / 4 (hand) -- lead the 4, then finesse the Q
//
#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "Player.h"
#include "Card.h"
#include "CardList.h"
#include "Type1Finesse.h"
#include "PlayEngine.h"
#include "CombinedHoldings.h"
#include "CardLocation.h"
#include "GuessedHandHoldings.h"
#include "PlayerStatusDialog.h"



//
//==================================================================
// constructon & destruction

CType1Finesse::CType1Finesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, CCardList* pCoverCards, CCardList* pLeadCards, int nSuit, int nCardVal) :
		CFinesse(CFinesse::TYPE_I, nTargetHand, nPlayerPosition, pGapCards, nSuit, nCardVal),
		m_pCoverCards(pCoverCards),
		m_pLeadCards(pLeadCards)
{
	Init();
}

CType1Finesse::CType1Finesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, CCardList* pCoverCards, CCardList* pLeadCards, CCard* pCard) :
		CFinesse(CFinesse::TYPE_I, nTargetHand, nPlayerPosition, pGapCards, pCard),
		m_pCoverCards(pCoverCards),
		m_pLeadCards(pLeadCards)
{
	VERIFY(pCard);
	Init();
}

CType1Finesse::~CType1Finesse() 
{
}


//
void CType1Finesse::Clear()
{
	CFinesse::Clear();
}


//
void CType1Finesse::Init()
{
	// call the base class
//	CFinesse::Init();

	// need one or more cover cards, as well as one or more low cards
	m_pOrKeyCardsList = m_pCoverCards;
	m_pOrKeyCardsList2 = m_pLeadCards;
}



//
CString CType1Finesse::GetFullDescription()
{
	CString strText;
	int nTrumpSuit = pDOC->GetTrumpSuit();
	if (m_pCoverCards->GetNumCards() > 1)
		return FormString("Lead a low %s from %s to finesse the %s in %s against %s, with the { %s } as possible cover cards.",
						   ((m_nSuit == nTrumpSuit)? "trump" : STSS(m_nSuit)),
						   ((m_nTargetHand == IN_HAND)? "dummy" : "hand"),
						   m_pConsumedCard->GetFaceName(),
//						   STS(m_nSuit),
						   ((m_nTargetHand == 0)? "hand" : "dummy"),
						   PositionToString(m_nTargetPos),
						   m_pCoverCards->GetHoldingsString());
	else
		return FormString("Lead a low %s from %s to finesse the %s in %s against %s, with the %s as cover.",
						   ((m_nSuit == nTrumpSuit)? "trump" : STSS(m_nSuit)),
						   ((m_nTargetHand == IN_HAND)? "dummy" : "hand"),
						   m_pConsumedCard->GetFaceName(),
//						   STS(m_nSuit),
						   ((m_nTargetHand == IN_HAND)? "hand" : "dummy"),
						   PositionToString(m_nTargetPos),
						   m_pCoverCards->GetAt(0)->GetFaceName());
}





//
// Perform()
//
PlayResult CType1Finesse::Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
						   CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, 
						   CPlayerStatusDialog& status, CCard*& pPlayCard)
{
	// Type I Finesse 
	// - lead of a low card towards a higher card in the opposite hand, 
	//	 which holds a higher cover card and a commanding top card.
	//   e.g., AQ3 (dummy) / 4 (hand) -- lead the 4, then finesse the Q

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
//	BOOL bLeading = TRUE;
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
		case 0:
			// we're leading, player #0
			if (bPlayingInHand) 
			{
				// playing from our own hand (declarer)
				// see where the finese card is located
				if (m_nTargetHand == IN_HAND)
				{
					// can't finesse here
					status << "4PL1FNS10! Can't use this (Type I) finesse leading from hand, as the finesse card (" &
							   m_pConsumedCard->GetName() & ") and its covers are in our own hand.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_POSTPONE;
				}
				else
				{
					// finessing from dummy so lead low card of the suit from hand
					if (playerHand.GetNumCardsInSuit(m_nSuit) > 0)
					{
						pPlayCard = playerHand.GetSuit(m_nSuit).GetBottomCard();
						status << "PL1FN12! Leading a low " & STSS(m_nSuit) & 
								  " (the " & pPlayCard->GetFaceName() &
								  ") from hand to finesse the " & 
								  m_pConsumedCard->GetFaceName() & " in dummy.\n";
					}
					else
					{
						// oops, no card in the suit to lead!
						status << "4PL1FN14! Oops, we wanted to finesse a " & STSS(m_nSuit) & 
								  " in dummy, but we have no " & STS(m_nSuit) & 
								  " in hand to lead, so we have to abandon the play.\n";
						m_nStatusCode = PLAY_NOT_VIABLE;
						return m_nStatusCode;
					}
				}
			}
			else
			{
				// leading from dummy
				if (m_nTargetHand == IN_DUMMY)
				{
					// leading from dummy & finessing in dummy? no can do
					status << "4PL1FNS20! Can't use this (Type I) finesse leading from dummy, as the finesse card (" &
							  m_pConsumedCard->GetName() & ") and its covers are in dummy.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_POSTPONE;
				}
				else 
				{
					// leading from dummy and finessing in hand, so do it
					if (dummyHand.GetNumCardsInSuit(m_nSuit) > 0)
					{
						pPlayCard = dummyHand.GetSuit(m_nSuit).GetBottomCard();
						status << "PL1FN22! Lead a low " & STSS(m_nSuit) & 
								  " (the " & pPlayCard->GetFaceName() &
								  ") from dummy to finesse the " & 
								  m_pConsumedCard->GetFaceName() & " in hand.\n";
					}
					else
					{
						// oops, no card in the suit to lead!
						status << "4PL1FN24! Oops, we wanted to finesse a " & STSS(m_nSuit) & 
								  " in hand, but we have no " & STS(m_nSuit) & 
								  " in dummy to lead, so we have to abandon the play.\n";
						m_nStatusCode = PLAY_NOT_VIABLE;
						return m_nStatusCode;
					}
				}
			}
			// at this point, all went OK
			m_nStatusCode = PLAY_IN_PROGRESS;
			break;

		case 1:
			// playing second -- can't really use the type I finesse here
			m_nStatusCode = PLAY_INACTIVE;
			return PLAY_POSTPONE;

		case 2:
			// playing third -- this is the key to the finesse
			// make sure the play is still on
			{
			if (m_nStatusCode != PLAY_IN_PROGRESS)
				return PLAY_INACTIVE;
			// see if the wrong suit was led 
			if (nSuitLed != m_nSuit)
			{
				m_nStatusCode = PLAY_NOT_VIABLE;
				return m_nStatusCode;
			}
			// see if LHO trumped
			if (bTrumped)
			{
				status << "3PL1FN50! the opponent has trumped, so abandon the finesse for this round.\n";
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			// see if RHO showed out
			CCard* pLHOCard = pDOC->GetCurrentTrickCardByOrder(1);
			if (pLHOCard->GetSuit() != nSuitLed)
			{
				// oops! RHO showed out! the finesse can't win!
				status << "3PL1FN55! Oops -- RHO (" & strRHO &
						  ") showed out of " & STS(nSuitLed) & ", meaning that LHO holds the " & 
						  m_pGapCards->GetAt(0)->GetFaceName() & ", so the finesse cannot succeed -- so skip it.\n";
				m_nStatusCode = PLAY_NOT_VIABLE;
				return m_nStatusCode;
/*
				// play the cover card
				pPlayCard = m_pCoverCards->GetBottomCard();
				ASSERT(pPlayCard->IsValid());
				status << "3PL1FN55! Oops -- " & playEngine.szLHO & " showed out of " & STS(nSuitLed) & 
						  ", meaning that RHO holds the " & m_pGapCards[0]->GetFaceName() & 
						  ", so the finesse cannot succeed -- so play the cover card (the " &
						  pCoverCard->GetFaceName() & ").\n";
				m_nStatusCode = PLAY_COMPLETE;
				return m_nStatusCode;
*/
			}
			// check the intervening opponents's card
			pOppCard = pDOC->GetCurrentTrickCardByOrder(1);
			// else check which hand we're playing in
			if (bPlayingInHand) 
			{
				// playing third from our own hand (declarer)
				// see if it's time to finesse
				if (m_nTargetHand == IN_HAND)
				{
					// see if RHO's card is higher than the intended finesse
					if (*pOppCard > *m_pConsumedCard)
					{
						// if so, play a cover card if possible
// ???					pPlayCard = dummySuit.GetLowestCardAbove(pOppCard);
						pPlayCard = playerSuit.GetTopSequence().GetBottomCard();
						status << "PL1FN62! RHO has played a higher card (the " & 
								  pOppCard->GetFaceName() & ") than our intended finesse (the " &
								  m_pConsumedCard->GetFaceName() & "), so cover with the " &
								  pPlayCard->GetFaceName() & ".\n";
					}
					else
					{
						// OK, finesse the card as intended
						pPlayCard = m_pConsumedCard;
						status << "PL1FN64! Finesse the " & pPlayCard->GetName() & " from hand.\n";
					}
				}
				else
				{
					// finessing in hand, but this is dummy? messed up
					status << "4PL1FN66! We intended to finesse in hand, but ended up here in dummy in third position -- so skip this play.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_POSTPONE;
				}
			}
			else
			{
				// playing third in dummy
				// see if it's time to finnesse here
				if (m_nTargetHand == IN_DUMMY)
				{
					// see if LHO's card is higher than the intended finesse
					if (*pOppCard > *m_pConsumedCard)
					{
						// if so, play a cover card
// ???					pPlayCard = dummySuit.GetLowestCardAbove(pOppCard);
						pPlayCard = dummySuit.GetTopSequence().GetBottomCard();
						status << "PL1FN72! LHO has played a higher card (the " & 
								  pOppCard->GetFaceName() & ") than our intended finesse (the " &
								  m_pConsumedCard->GetFaceName() & "), so cover with the " &
								  pPlayCard->GetFaceName() & ".\n";
					}
					else
					{
						// finesse the card from dummy 
						pPlayCard = m_pConsumedCard;
						status << "PLCSH74! Finesse the " & pPlayCard->GetName() & " from dummy.\n";
					}
				}
				else
				{
					// messed up
					status << "4PL1FN76! We intended to finesse in hand, but ended up here in dummy in third position -- so skip this play.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_POSTPONE;
				}
			}
			// else all went OK
			m_nStatusCode = PLAY_COMPLETE;
			break;
			}

		case 3:
			//  can't use the type I finesse in 4th position!
			m_nStatusCode = PLAY_INACTIVE;
			return PLAY_POSTPONE;
	}

	// done
	ASSERT(pPlayCard->IsValid());
	return m_nStatusCode;
}
