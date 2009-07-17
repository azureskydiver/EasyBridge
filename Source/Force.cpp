//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Finesse.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "Deck.h"
#include "Card.h"
#include "Force.h"
#include "Player.h"
#include "PlayEngine.h"
#include "CombinedHoldings.h"
#include "CombinedSuitHoldings.h"
#include "CardLocation.h"
#include "GuessedHandHoldings.h"
#include "PlayerStatusDialog.h"



//
//==================================================================
// constructon & destruction

CForce::CForce(int nTargetHand, int nTargetCardVal, CCardList* pReqPlayedList, int nSuit, int nCardVal) :
			CPlay(CPlay::FORCE, nTargetHand, nSuit, PP_LOSER),
			m_nTargetCardVal(nTargetCardVal),
			m_nCardVal(nCardVal)
{
	m_pRequiredPlayedCardsList = pReqPlayedList;
	Init();
}

CForce::CForce(int nTargetHand, int nTargetCardVal, CCardList* pReqPlayedList, CCard* pCard) :
			CPlay(CPlay::FORCE, nTargetHand, NONE, PP_LOSER),
			m_nTargetCardVal(nTargetCardVal)
{
	VERIFY(pCard);
	m_pRequiredPlayedCardsList = pReqPlayedList;
	m_nSuit = pCard->GetSuit();
	m_nCardVal = pCard->GetFaceValue();
	Init();
}

CForce::~CForce() 
{
}


//
void CForce::Clear()
{
	CPlay::Clear();
	//
	m_nTargetCardVal = NONE;
	m_nSuit = NONE;
	m_nCardVal = NONE;
}


//
void CForce::Init()
{
	CPlay::Init();
	//
	m_pTargetCard = deck.GetSortedCard(MAKEDECKVALUE(m_nSuit, m_nTargetCardVal));
	m_pConsumedCard = deck.GetSortedCard(MAKEDECKVALUE(m_nSuit, m_nCardVal));
	// form name & description
	m_strName.Format("Force out the %s", m_pTargetCard->GetName());
	m_strDescription.Format("Force out the %s with the %s",
							m_pTargetCard->GetName(), m_pConsumedCard->GetName());
}




//
CString CForce::GetFullDescription()
{
	return FormString("Play the %s from %s to force out the opponents' %s.",
					   CardToString(MAKEDECKVALUE(m_nSuit,m_nCardVal)),
					   (m_nTargetHand == IN_HAND)? "hand" : "dummy",
					   CardValToString(m_nTargetCardVal));
}




//
// Perform()
//
PlayResult CForce::Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
				    CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, 
				    CPlayerStatusDialog& status, CCard*& pPlayCard)
{
	// a "force" is a play of the lowest possible card that will force out
	// a key enemy card
	// check which hand this is
	int nOrdinal = pDOC->GetNumCardsPlayedInRound();
	CPlayer* pPlayer = playEngine.GetPlayer();
	BOOL bPlayingInHand = (pDOC->GetCurrentPlayer() == pPlayer);
	CHandHoldings& playerHand = *(combinedHand.GetPlayerHand());
	CHandHoldings& dummyHand = *(combinedHand.GetPartnerHand());
	CCombinedSuitHoldings& combinedSuit = combinedHand.GetSuit(m_nSuit);
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

	// test preconditions
	if (!CPlay::IsPlayUsable(combinedHand, playEngine))
	{
		m_nStatusCode = PLAY_INACTIVE;
		return PLAY_POSTPONE;
	}

	//
	// test to make sure that the required played cards have indeed been played
	//
	if (m_pRequiredPlayedCardsList)
	{
		// check if any of the cards that should have benen played 
		// are still outstanding
		for(int i=0;i<m_pRequiredPlayedCardsList->GetNumCards();i++)
		{
			CCard* pCard = (*m_pRequiredPlayedCardsList)[i];
			if (playEngine.IsCardOutstanding(pCard))
			{
				status << "5PLFRCA! The force play of the " & m_pConsumedCard->GetFaceName() &
						  " to force out the " & m_nTargetCardVal & 
						  " is not yet viable as the card [" & pCard->GetFaceName() & 
						  "] is still outstanding.\n";
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
		}
	}


	//
	// check our position in the play
	//
	switch(nOrdinal)
	{
		case 0:
			// we're leading, player #0
			if (bPlayingInHand) 
			{
				// playing from our own hand (declarer)
				if (m_nTargetHand == IN_HAND)
				{
					// play the card necessary to force the opponent
					pPlayCard = playerSuit.GetHighestCardBelow(m_nTargetCardVal);
					// NCR check that dummy does not have a singleton honor that's same value as our lead
					if(dummySuit.IsSingleton() 
						&& ((dummySuit.GetTopCard()->GetFaceValue() > pPlayCard->GetFaceValue()) // NCR-28
						    || (combinedHand.AreEquivalentCards(pPlayCard, dummySuit.GetTopCard()))))
						pPlayCard = playerSuit.GetBottomCard(); // NCR get lowest card

					if (pPlayCard == NULL)
					{
						status << "4PLFRC02! Error in force play -- no cards in declarer hand usable in a force play.\n";
						m_nStatusCode = PLAY_ERROR;
						return m_nStatusCode;
					}
					status << "PLFRC04! Play the " & pPlayCard->GetName() &
							  " from hand to force out the opponents' " & CardValToString(m_nTargetCardVal) & ".\n";
				}
				else
				{
					// forcing from dummy, so lead a low card 
					if (playerSuit.GetNumCards() > 0)
					{
						pPlayCard = playerSuit.GetBottomCard();
						status << "PLFRC06! Lead a low " & STSS(m_nSuit) & 
								  " (" & pPlayCard->GetFaceName() &
								  ") from hand to trigger a force play in dummy.\n";
					}
					else
					{
						// oops, no card in the suit to lead!
						status << "4PLFRC08! Oops, we can't start a force play in the " & STSS(m_nSuit) & 
								  " suit from declarer, since we have no cards in the suit.\n";
						m_nStatusCode = PLAY_POSTPONE;
						return m_nStatusCode;
					}
				}
			}
			else
			{
				// leading from dummy
				if (m_nTargetHand == IN_DUMMY)
				{
					// forcing from dummy, so do it
					pPlayCard = dummySuit.GetHighestCardBelow(m_nTargetCardVal);
					if (pPlayCard == NULL)
					{
						status << "4PLFRC10! Error in force play -- no cards in dummy usable in a force play.\n";
						m_nStatusCode = PLAY_ERROR;
						return m_nStatusCode;
					}
					status << "PLFRC12! Play the " & pPlayCard->GetName() &
							  " from dummy to force out the opponents' " & CardValToString(m_nTargetCardVal) & ".\n";
				}
				else
				{
					// forcing from hand, so lead a low card from dummy 
					if (dummySuit.GetNumCards() > 0)
					{
						pPlayCard = dummySuit.GetBottomCard();
						status << "PLFRC14! Lead a low " & STSS(m_nSuit) & 
								  " (" & pPlayCard->GetFaceName() &
								  ") from dummy to trigger a force play in hand.\n";
					}
					else
					{
						// oops, no card in the suit to lead!
						status << "4PLFRC16! Oops, we can't start a force play in the " & STSS(m_nSuit) & 
								  " suit from dummy, since we have no cards in the suit.\n";
						m_nStatusCode = PLAY_POSTPONE;
						return m_nStatusCode;
					}
				}
			}
			// all went OK
			m_nStatusCode = PLAY_IN_PROGRESS;
//			m_nStatusCode = PLAY_COMPLETE;
			break;

		case 1:
			// playing second -- see if we can still force
			// (i.e., the right suit was led, and the card led < target card)
			if ((nSuitLed == m_nSuit) && (pCardLed->GetFaceValue() < m_nTargetCardVal))
			{
				// opponent led the suit, so we still try to force
				if ( ((bPlayingInHand) && (m_nTargetHand == IN_HAND)) ||
				     ((!bPlayingInHand) && (m_nTargetHand == IN_DUMMY)) )
				{
					// this is the right time to play the forcing card
					if (bPlayingInHand)
						pPlayCard = playerSuit.GetHighestCardBelow(m_nTargetCardVal);
					else
						pPlayCard = dummySuit.GetHighestCardBelow(m_nTargetCardVal);
					// check for error
					if (pPlayCard == NULL)
					{
						status << "4PLFRC20! Error in force play -- no cards in " &
								  (bPlayingInHand? "hand" : "dummy") & " usable in a force play.\n";
						m_nStatusCode = PLAY_ERROR;
						return m_nStatusCode;
					}
					// got the forcing card
					status << "PLFRC22! The opponents led a " & STSS(m_nSuit) & 
							  ", so play the " & pPlayCard->GetName() & " from " &
							  (bPlayingInHand? "hand" : "dummy") & " to try and force out the " & 
							  CardValToString(m_nTargetCardVal) & " now.\n";
//					m_nStatusCode = PLAY_IN_PROGRESS;
					m_nStatusCode = PLAY_COMPLETE;
				}
				else
				{
					// not the right hand. so discard
					pPlayCard = playEngine.GetDiscard();
					status << "4PLFRC24! We're not in the right position to play a forcing card, so discard the " &
							   pPlayCard->GetName() & ".\n";
					m_nStatusCode = PLAY_POSTPONE;
				}
			}
			else
			{
				// wrong suit led, so no point here
				m_nStatusCode = PLAY_POSTPONE;
			}
			// done
			return m_nStatusCode;
			break;

		case 2:
			// playing third
			// sanity check
			if ((m_nStatusCode != PLAY_IN_PROGRESS) || (nSuitLed != m_nSuit))
			{
				m_nStatusCode = PLAY_ERROR;
				return m_nStatusCode;
			}
			// see if RHO trumped
			if (bTrumped)
			{
				status << "5PLFR30! RHO trumped, rendering the force play of the " & m_pConsumedCard->GetFaceName() &
						  " ineffective, so skip the play.\n";
				m_nStatusCode = PLAY_POSTPONE;
				return m_nStatusCode;
			}

			// see if we're in the correct hand
			if ( (bPlayingInHand && (m_nTargetHand == IN_HAND)) ||
				 (!bPlayingInHand && (m_nTargetHand == IN_DUMMY)) )
			{
				// see if RHO played the target card
				CCard* pRHOCard = pDOC->GetCurrentTrickCardByOrder(1);
				if (pRHOCard->GetFaceValue() >= m_nTargetCardVal)
				{
					status << "5PLFR50! RHO played the " & CardValToString(m_nTargetCardVal) &
							  ", so skip this force play.\n";
					m_nStatusCode = PLAY_NOT_VIABLE;
					return m_nStatusCode;
				}

				// the target card has not been played yet, so proceed
				if (bPlayingInHand)
					pPlayCard = playerSuit.GetHighestCardBelow(m_nTargetCardVal);
				else
					pPlayCard = dummySuit.GetHighestCardBelow(m_nTargetCardVal);

				// NCR check if our card is less than the top card already played
				if(pPlayCard < pDOC->GetCurrentTrickHighCard())
				{
					m_nStatusCode = PLAY_POSTPONE; // or NOT_VIABLE ???
					return m_nStatusCode;
				}


				// see if we played a card from the other hand that's equivalent 
				// to this card; if so, discard low
				CSuitHoldings testSuit;
				testSuit << combinedSuit;
				testSuit << pCardLed;	// needed for valid test
				//
				if (testSuit.AreEquivalentCards(pCardLed, pPlayCard))
				{
					// the opposite card can do the trick
					if (bPlayingInHand && (playerSuit.GetNumCards() > 1))
					{
						pPlayCard = playerSuit.GetBottomCard();
						status << "PLFRC55! The " & pCardLed->GetName() & " is sufficient for the force, so discard the " &
								  pPlayCard->GetFaceName() & " from hand.\n";
					}
					else if (!bPlayingInHand && (dummySuit.GetNumCards() > 1))
					{
						pPlayCard = dummySuit.GetBottomCard();
						status << "PLFRC56! The " & pCardLed->GetName() & " is sufficient for the force, so discard the " &
								  pPlayCard->GetFaceName() & " from dummy.\n";
					}
				}
				else 
				{
					// check for error
					if (pPlayCard == NULL)
					{
						status << "4PLFRC61! Error in force play -- no cards in " &
								  (bPlayingInHand? "hand" : "dummy") & " usable in a force play.\n";
						m_nStatusCode = PLAY_ERROR;
						return m_nStatusCode;
					}
					// got the forcing card
					status << "PLFRC62! Play the " & pPlayCard->GetName() & " from " &
							  (bPlayingInHand? "hand" : "dummy") & " to try and force out the " & 
							  CardValToString(m_nTargetCardVal) & " now.\n";
					m_nStatusCode = PLAY_COMPLETE;
				}
			}
			else
			{
				// we're in the opposite (discard) hand. so discard
				pPlayCard = playEngine.GetDiscard();
				status << "PLFRC64! We're in the opposite hand of a force play, so discard the " &
						   pPlayCard->GetName() & ".\n";
				m_nStatusCode = PLAY_COMPLETE;
			}
			break;

		case 3:
			// playing fourth -- can't use a force play here
			return PLAY_POSTPONE;
			break;
	}

	// done
	ASSERT(pPlayCard->IsValid());
	return m_nStatusCode;
}


