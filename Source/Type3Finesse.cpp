//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Type3Finesse.cpp
//
// Type III Finesse
// - lead of a low card towards a higher card in the opposite hand, which 
//   does not have a commanding card.  
//   e.g., K52 / 853 -- lead the 3 and finesse the King against East, 
//                      or play low if West plays the Ace.
//
#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "Player.h"
#include "Card.h"
#include "Type3Finesse.h"
#include "PlayEngine.h"
#include "CombinedHoldings.h"
#include "CardLocation.h"
#include "GuessedHandHoldings.h"
#include "PlayerStatusDialog.h"



//
//==================================================================
// constructon & destruction

CType3Finesse::CType3Finesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, CCardList* pLeadCards, int nSuit, int nCardVal) :
		CFinesse(CFinesse::TYPE_III, nTargetHand, nPlayerPosition, pGapCards, nSuit, nCardVal),
		m_pLeadCards(pLeadCards)
{
	Init();
}

CType3Finesse::CType3Finesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, CCardList* pLeadCards, CCard* pCard) :
		CFinesse(CFinesse::TYPE_III, nTargetHand, nPlayerPosition, pGapCards, pCard),
		m_pLeadCards(pLeadCards)
{
	VERIFY(pCard);
	Init();
}

CType3Finesse::~CType3Finesse() 
{
}


//
void CType3Finesse::Clear()
{
	CFinesse::Clear();
}



//
void CType3Finesse::Init()
{
	// call base class
//	CFinesse::Init();

	// the ending hand is the target hand (opposite of default)
	m_nEndingHand = m_nTargetHand;
	m_nStartingHand = (m_nEndingHand == IN_HAND)? IN_DUMMY : IN_HAND;

	// need one or more of the lead cards
	m_pOrKeyCardsList = m_pLeadCards;
}



//
CString CType3Finesse::GetFullDescription()
{
	CString strText;
	int nTrumpSuit = pDOC->GetTrumpSuit();
	return FormString("Lead a low %s from %s and finesse the %s in %s against %s.",
					   ((m_nSuit == nTrumpSuit)? "trump" : STSS(m_nSuit)),
					   (m_nTargetHand == IN_HAND)? "dummy" : "hand",
					   m_pConsumedCard->GetFaceName(),
//					   STS(m_nSuit),
					   (m_nTargetHand == IN_HAND)? "hand" : "dummy",
					   PositionToString(m_nTargetPos));
	// done
	return strText;
}




//
// Perform()
//
PlayResult CType3Finesse::Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
						   CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, 
						   CPlayerStatusDialog& status, CCard*& pPlayCard)
{
	// Type III Finesse
	// - lead of a low card towards a higher card in the opposite hand, which 
	//   does not have a commanding card.  
	//   e.g., K52 / 853 -- lead the 3 and finesse the King against East, 
	//                      or play low if West plays the Ace.

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
	CCard* pTopCard = pDOC->GetCurrentTrickHighCard();
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
					status << "4PL3FNS10! Can't use this (Type III) finesse leading from hand, as the finesse card (" &
							   m_pConsumedCard->GetName() & ") is in our own hand.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_POSTPONE;
				}
				else
				{
					// finessing from dummy so lead low card of the suit from hand
					if (playerHand.GetNumCardsInSuit(m_nSuit) > 0)
					{
						pPlayCard = playerHand.GetSuit(m_nSuit).GetBottomCard();
						status << "PL3FN12! Lead a low " & STSS(m_nSuit) & 
								  " (the " & pPlayCard->GetFaceName() &
								  ") from hand to finesse the " & 
								  m_pConsumedCard->GetFaceName() & " in dummy.\n";
					}
					else
					{
						// oops, no card in the suit to lead!
						status << "4PL3FN14! Oops, we wanted to finesse a " & STSS(m_nSuit) & 
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
					status << "4PL3FNS20! Can't use this (Type III) finesse leading from dummy, as the finesse card (" &
							  m_pConsumedCard->GetName() & ") is in dummy.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_POSTPONE;
				}
				else 
				{
					// leading from dummy and finessing in hand, so do it
					if (dummyHand.GetNumCardsInSuit(m_nSuit) > 0)
					{
						pPlayCard = dummyHand.GetSuit(m_nSuit).GetBottomCard();
						status << "PL3FN22! Lead a low " & STSS(m_nSuit) & 
								  " (the " & pPlayCard->GetFaceName() &
								  ") from dummy to finesse the " & 
								  m_pConsumedCard->GetFaceName() & " in hand.\n";
					}
					else
					{
						// oops, no card in the suit to lead!
						status << "4PL3FN24! Oops, we wanted to finesse a " & STSS(m_nSuit) & 
								  " in hand, but we have no " & STS(m_nSuit) & 
								  " in dummy to lead, so we have to abandon the play.\n";
						m_nStatusCode = PLAY_NOT_VIABLE;
						return m_nStatusCode;
					}
				}
			}
			// all went OK
			m_nStatusCode = PLAY_IN_PROGRESS;
			break;


		case 1:
			// playing second -- can try the type III finesse opportunistically
			// but only if it's in the opposite hand
			if ((nSuitLed != m_nSuit) || (*pCardLed > *m_pConsumedCard) ||
				(bPlayingInHand && (m_nTargetHand == IN_HAND)) ||
				(!bPlayingInHand && (m_nTargetHand == IN_DUMMY)) )
			{
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			//
			if (bPlayingInHand) 
			{
				// see where the finese card is located
				if (m_nTargetHand == IN_HAND)
				{
					// play the card here!
					pPlayCard = m_pConsumedCard;
					status << "PL3FNS30! Opportunistically finesse the " & m_pConsumedCard->GetName() & 
							  ") from hand in second position.\n";
				}
				else
				{
					// finessing from dummy, so play low in second hand
					if (playerHand.GetNumCardsInSuit(m_nSuit) > 0)
					{
						pPlayCard = playerHand.GetSuit(m_nSuit).GetBottomCard();
						status << "PL3FN32! Play low " & STSS(m_nSuit) & 
								  " (the " & pPlayCard->GetFaceName() & ") from hand to opportunistically finesse the " & 
								  m_pConsumedCard->GetFaceName() & " in dummy.\n";
					}
					else
					{
						// oops, no cards in the suit to play, so discard
						pPlayCard = playerHand.GetDiscard();
						status << "PL3FN33! We want to opportunistically finesse a " & STSS(m_nSuit) & 
							      " in dummy, but have no " & STS(m_nSuit) & " in hand, so discard the " & pPlayCard->GetName() & ".\n";
					}
				}
			}
			else
			{
				// playing second in dummy
				if (m_nTargetHand == IN_DUMMY)
				{
					// play the card here!
					pPlayCard = m_pConsumedCard;
					status << "PL3FNS35! Opportunistically finesse the " & m_pConsumedCard->GetName() & 
							  ") from dummy in second position.\n";
				}
				else 
				{
					// finessing from hand, so play low in second hand
					if (dummyHand.GetNumCardsInSuit(m_nSuit) > 0)
					{
						pPlayCard = dummyHand.GetSuit(m_nSuit).GetBottomCard();
						status << "PL3FN36! Play low " & STSS(m_nSuit) & 
								  " (the " & pPlayCard->GetFaceName() & ") from dummy to opportunistically finesse the " & 
								  m_pConsumedCard->GetFaceName() & " in hand.\n";
					}
					else
					{
						// oops, no cards in the suit to play, so discard
						pPlayCard = dummyHand.GetDiscard();
						status << "PL3FN37! We want to opportunistically finesse a " & STSS(m_nSuit) & 
							      " in hand, but have no " & STS(m_nSuit) & " in dummy, so discard the " & pPlayCard->GetName() & ".\n";
					}
				}
			}
			// all went OK
			m_nStatusCode = PLAY_IN_PROGRESS;
			break;


		case 2:
			// playing third -- this is the key to the finesse
			// make sure play is in progress
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
				status << "3PL3FN50! the opponent has trumped, so abandon the finesse for this round.\n";
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			// check the intervening opponents's card
			pOppCard = pDOC->GetCurrentTrickCardByOrder(1);
			// check if RHO showed out
			if (pOppCard->GetSuit() != nSuitLed)
			{
				// RHO has showed out, so skip the finesse and play low
				status << "3PL3FN55! Oops -- RHO (" & strRHO &
						  ") showed out of " & STS(nSuitLed) & ", meaning that LHO holds the " & 
						  m_pGapCards->GetAt(0)->GetFaceName() & ", so the finesse cannot succeed -- so skip it.\n";
				m_nStatusCode = PLAY_NOT_VIABLE;
				return m_nStatusCode;
			}
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
						// if so, play low from hand
						pPlayCard = playEngine.GetDiscard();
						status << "PL3FN62! RHO has played a higher card (the " & 
								  pOppCard->GetFaceName() & ") than our intended finesse (the " &
								  m_pConsumedCard->GetFaceName() & "), so play low with the " &
								  pPlayCard->GetFaceName() & ".\n";
					}
					else
					{
						// OK, finesse the card as intended
						pPlayCard = m_pConsumedCard;
						status << "PL3FN64! Finesse the " & pPlayCard->GetName() & " from hand.\n";
					}
				}
				else
				{
					// finessing in hand, but this is dummy? messed up
					status << "4PL3FN66! We intended to finesse in hand, but ended up here in dummy in third position -- so skip this play.\n";
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
						// if so, play low from hand
						pPlayCard = playEngine.GetDiscard();
						status << "PL3FN72! LHO has played a higher card (the " & 
								  pOppCard->GetFaceName() & ") than our intended finesse (the " &
								  m_pConsumedCard->GetFaceName() & "), so play low with the " &
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
					status << "4PL3FN76! We intended to finesse in hand, but ended up here in dummy in third position -- so skip this play.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_POSTPONE;
				}
			}
			// all went OK
			m_nStatusCode = PLAY_COMPLETE;
			break;


		case 3:
			// finish up opportunistic finesse
			if (m_nStatusCode != PLAY_IN_PROGRESS)
				return PLAY_INACTIVE;
			//
			// see if the wrong suit was led 
			if (nSuitLed != m_nSuit)
			{
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			// see if LHO/RHO trumped
			if (bTrumped)
			{
				status << "3PL3FN70! the opponent has trumped, so abandon the finesse for this round.\n";
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			// check RHO's card
			pOppCard = pDOC->GetCurrentTrickCardByOrder(2);
			// check if RHO showed out
			if (pOppCard->GetSuit() != nSuitLed)
			{
				// RHO has showed out, so skip the finesse and play just high enough to win
				status << "3PL3FN75! RHO (" & strRHO & ") showed out of " & STS(nSuitLed) & 
						  ", so we may not necessarily need to play the finesse card.\n";
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			// else check which hand we're playing in
			if ((bPlayingInHand) && (m_nTargetHand == IN_HAND))
			{
				// playing fourth in our own hand (declarer)
				// see if RHO's card is higher than the intended finesse
				if (*pOppCard > *m_pConsumedCard)
				{
					// if so, ditch the finesse
					status << "3PL3FN92! RHO has played a higher card (the " & 
							  pOppCard->GetFaceName() & ") than our intended finesse (the " &
							  m_pConsumedCard->GetFaceName() & "), so skip the play.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_POSTPONE;
				}
				else if (playerSuit.GetNumCardsAbove(pTopCard) > 1)
				{
					// we can beat the top card with less than the finesse card
					status << "3PL3FN93! We can beat the current top card (the " & 
							  pTopCard->GetFaceName() & ") with multiple cards in hand, so the finesse is no longer necessary.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_POSTPONE;
				}
				else
				{
					// OK, finesse the card as intended
					pPlayCard = m_pConsumedCard;
					status << "PL3FN94! Finesse the " & pPlayCard->GetName() & " from hand.\n";
				}
			}
			else if ((!bPlayingInHand) && (m_nTargetHand == IN_DUMMY))
			{
				// playing fourth in dummy
				// see if RHO's card is higher than the intended finesse
				if (*pOppCard > *m_pConsumedCard)
				{
					// if so, ditch the finesse
					status << "3PL3FN96! RHO has played a higher card (the " & 
							  pOppCard->GetFaceName() & ") than our intended finesse (the " &
							  m_pConsumedCard->GetFaceName() & "), so skip the play.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_POSTPONE;
				}
				else if (dummySuit.GetNumCardsAbove(pTopCard) > 1)
				{
					// we can beat the top card with less than the finesse card
					status << "3PL3FN97! We can beat the current top card (the " & 
							  pTopCard->GetFaceName() & ") with multiple cards in dummy, so the finesse is no longer necessary.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_POSTPONE;
				}
				else
				{
					// finesse the card from dummy 
					pPlayCard = m_pConsumedCard;
					status << "PLCSH98! Finesse the " & pPlayCard->GetName() & " from dummy.\n";
				}
			}
			else
			{
				// else it's an error
				m_nStatusCode = PLAY_ERROR;
				return m_nStatusCode;
			}
			// all went OK
			m_nStatusCode = PLAY_COMPLETE;
			break;

	}

	// done
	ASSERT(pPlayCard->IsValid());
	return m_nStatusCode;
}
