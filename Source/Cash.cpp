//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Cash.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "Deck.h"
#include "Card.h"
#include "Cash.h"
#include "Player.h"
#include "PlayEngine.h"
#include "CardList.h"
#include "DeclarerPlayEngine.h"
#include "CombinedHoldings.h"
#include "CombinedSuitHoldings.h"
#include "CardLocation.h"
#include "GuessedHandHoldings.h"
#include "PlayerStatusDialog.h"


//
//==================================================================
// constructon & destruction

CCash::CCash(int nTargetHand, int nStartingHand, CCardList* pRequiredPlayedCards, int nSuit, int nCardVal, PlayProspect nProspect, BOOL bOpportunistic) :
		CPlay(CPlay::CASH, nTargetHand, nSuit, nProspect, bOpportunistic),
		m_nCardVal(nCardVal),
		m_Properties(ORDINARY) // NCR-707
{
	Init();
	m_nStartingHand = nStartingHand;
	m_pRequiredPlayedCardsList = pRequiredPlayedCards;
}

CCash::CCash(int nTargetHand, int nStartingHand, CCardList* pRequiredPlayedCards, CCard* pCard, PlayProspect nProspect, BOOL bOpportunistic) :
		CPlay(CPlay::CASH, nTargetHand, NONE, nProspect, bOpportunistic),
		m_Properties(ORDINARY) // NCR-707
{
	VERIFY(pCard);
	m_nStartingHand = nStartingHand;
	m_nSuit = pCard->GetSuit();
	m_nSuit2 = NONE; // NCR-411 clear Suit to underplay
	m_nCardVal = pCard->GetFaceValue();
	Init();
	m_pRequiredPlayedCardsList = pRequiredPlayedCards;
}

CCash::CCash(CCash& srcPlay) :
		CPlay(srcPlay.m_nPlayType)
{
	m_nTargetHand = srcPlay.m_nTargetHand;
	m_nSuit = srcPlay.m_nSuit;
	m_nCardVal = srcPlay.m_nCardVal;
}

CCash::~CCash() 
{
}


//
void CCash::Clear()
{
	CPlay::Clear();
	//
	m_nCardVal = NONE;
}


//
void CCash::Init()
{
	CPlay::Init();
	//
	m_pConsumedCard = deck.GetCard(m_nSuit, m_nCardVal);
	// form name & description
	m_strName.Format("%s Cash", m_pConsumedCard->GetName());
	m_strDescription.Format("Cash the %s", m_pConsumedCard->GetName());
}


//
CString CCash::GetFullDescription()
{
	return FormString("Cash the %s in %s.",
					   m_pConsumedCard->GetFullName(), 
					   ((m_nTargetHand == IN_HAND)? "hand" : "dummy"));
}



//
int	CCash::UsesUpEntry()
{
	// this play does use up an entry
	return (m_nTargetHand == IN_DUMMY)? ENTRY_DUMMY: ENTRY_HAND;
}




// 
// Perform()
//
// called to do the deed
//
PlayResult CCash::Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
				   CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, 
				   CPlayerStatusDialog& status, CCard*& pPlayCard)
{
	// check which hand this is
	int nOrdinal = pDOC->GetNumCardsPlayedInRound();
	CPlayer* pPlayer = playEngine.GetPlayer();
	BOOL bPlayingInHand = (pDOC->GetCurrentPlayer() == pPlayer);
	CHandHoldings& playerHand = *(combinedHand.GetPlayerHand());
	CHandHoldings& dummyHand = *(combinedHand.GetPartnerHand());
	CSuitHoldings& declarerSuit = playerHand.GetSuit(m_nSuit);
	CSuitHoldings& dummySuit = dummyHand.GetSuit(m_nSuit);
	CCombinedSuitHoldings& combinedSuit = combinedHand.GetSuit(m_nSuit);
	CCard* pCardLed = pDOC->GetCurrentTrickCardByOrder(0);
	int nSuitLed = pCardLed? pCardLed->GetSuit() : NONE;
	CDeclarerPlayEngine& declarerEngine = (CDeclarerPlayEngine&) playEngine;
	
	// see if a trump was played in this round
	BOOL bTrumped = FALSE;
	int nTrumpSuit = pDOC->GetTrumpSuit();
	if ((nSuitLed != nTrumpSuit) && (pDOC->WasTrumpPlayed()))
		bTrumped = TRUE;
	pPlayCard = NULL;

	// see what the top card in the round is
	CCard* pTopCard = pDOC->GetCurrentTrickHighCard();
	CCard* pDeclarerCard = pDOC->GetCurrentTrickCard(playEngine.GetPlayerPosition());
	CCard* pDummysCard = pDOC->GetCurrentTrickCard(playEngine.GetPartnerPosition());
	CCard* pPartnersCard = bPlayingInHand? pDummysCard : pDeclarerCard;
	BOOL bPartnerHigh = (pTopCard == pPartnersCard);

	//
	if (!CPlay::IsPlayUsable(combinedHand, playEngine))
	{
		m_nStatusCode = PLAY_INACTIVE;
		return PLAY_POSTPONE;
	}

	// see if one or more opponents are void in the suit AND have not shown out
	// of trumps
	if (ISSUIT(nTrumpSuit))
	{
		int numOutstandingTrumps = playEngine.GetNumOutstandingCards(nTrumpSuit);
		BOOL bSkipPlay = FALSE;
		// if leading, check LHO
		CGuessedHandHoldings* pLHOHand = ppGuessedHands[playEngine.GetLHOpponent()->GetPosition()];
		if ((nOrdinal == 0) && (numOutstandingTrumps > 0) && pLHOHand->IsSuitShownOut(m_nSuit) && !pLHOHand->IsSuitShownOut(nTrumpSuit))
		{
			bSkipPlay = TRUE;
			status << "5PLCSHZ1! The play <" & m_strName & "> is not yet safe as LHO has shown out of the suit and may ruff.\n";
		}
		// also check RHO
		CGuessedHandHoldings* pRHOHand = ppGuessedHands[playEngine.GetRHOpponent()->GetPosition()];
		if ((numOutstandingTrumps > 0) && pRHOHand->IsSuitShownOut(m_nSuit) && !pRHOHand->IsSuitShownOut(nTrumpSuit))
		{
			bSkipPlay = TRUE;
			status << "5PLCSHZ2! The play <" & m_strName & "> is not yet safe as RHO has shown out of the suit and may ruff.\n";
		}
		//
		if (bSkipPlay)
		{
			// opponents might ruff
			m_nStatusCode = PLAY_INACTIVE;
			return PLAY_POSTPONE;
		}
	}

	// else proceed
	// check our position in the play
	switch(nOrdinal)
	{
		case 0:
			// we're leading, player #0
			if (bPlayingInHand) 
			{
				// playing from our own hand (declarer) and cashing in hand
				if (m_nTargetHand == IN_HAND)
				{
					// cashing from hand -- check to be sure the other hand has losers
					// i.e., don't discard a winner on a winner UNLESS both hands 
					// have nothing but winners, OR the other hand has only one card, 
					// a winner that's lower than the top card in this hand, 
					// AND this hand has only winners
					// this is because we don't want to end up stranded in the wrong hand
					// examples: 
					// ---------
					//   from Kx/A   -- don't lead the King!
					//        Kxx/AQ -- again, don't lead the King
					//        KJ/Q   -- we _can_ lead the K (in fact, we should)
					//        KQJ/T9 -- doesn't matter
					//        Qx/AKJ -- doesn't matter
					if ((dummySuit.GetNumCards() == 1) && (combinedSuit.GetNumDummyLosers() == 0) && 
								(combinedSuit.GetNumDeclarerLosers() == 0) && (*dummySuit[0] < *m_pConsumedCard))
					{
						// this is a special case, so it's OK
					}
					else if ((dummySuit.GetNumCards() > 0) && (combinedSuit.GetNumDummyLosers() == 0) && 
								(combinedSuit.GetNumDeclarerLosers() > 0) &&
								(combinedSuit.GetNumDummyWinners() < declarerSuit.GetNumCards()) &&
								(*dummySuit[0] > *declarerSuit[0]) &&
								(declarerEngine.GetNumDeclarerEntries() == 1) 
							  // NCR Special case  if dummy has a singleton honor same value as lead card 	
							  || (dummySuit.IsSingleton() && combinedSuit.AreEquivalentCards(dummySuit[0], m_pConsumedCard)
							      // NCR-134 Only get stranded if Dummy card > pConsumedCard
							      && (dummySuit.GetTopCard()->GetFaceValue() > m_pConsumedCard->GetFaceValue()))	)
					{
						// here, nothing but higher winners in dummy, which is shorter than hand
						status << "5PLCSH02! We could cash the " & m_pConsumedCard->GetName() &
								  " from hand, but dummy has no losers to discard in the suit and could get stranded there.\n";
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
					// proceed 
					pPlayCard = m_pConsumedCard;
					status << "PLCSH10! Cash the " & pPlayCard->GetName() &
							  " from hand.\n";
				}
				else
				{
					// playing from hand. but cashing from dummy,
					// lead a low card from hand
					if (declarerSuit.GetNumCards() > 0)
					{
						// we have cards in the suit
//						if (combinedSuit.GetNumDeclarerLosers() > 0)
						if (declarerSuit.GetNumCardsBelow(m_pConsumedCard) > 0)
						{
							// lead a low card, but test first
							pPlayCard = declarerSuit.GetBottomCard();
							if (combinedSuit.AreEquivalentCards(pPlayCard, m_pConsumedCard)
//								(declarerSuit.GetNumCards() > 1) && (ombinedSuit.GetNumDummyLosers() > 0) )
								&& (combinedSuit.GetNumDummyLosers() > 0)
								// NCR-438 Ignore if flagged as Opportunistic
								&& !IsOpportunistic() 
								// NCR-754 Ok to overtake a singleton when dummy has more winners
								&& !(declarerSuit.IsSingleton() 
								     && (dummyHand.GetNumWinners() > playerHand.GetNumWinners()) )
								)
							{
								// oops, the "low" card is equivalent to the cash card!
								status << "4PLCSH15! Oops, the lowest card we can lead from hand for the cash is the " & 
										  pPlayCard->GetFaceName() & ", which is equivalent to the " &
										  m_pConsumedCard->GetFaceName() & ", so skip the cash play.\n";
								m_nStatusCode = PLAY_INACTIVE;
								return PLAY_POSTPONE;
							}
							else
							{
								status << "PLCSH20! Lead a low " & STSS(m_nSuit) & 
										  " (the " & pPlayCard->GetFaceName() &
										  ") from hand in order to cash the " & 
										  m_pConsumedCard->GetFaceName() & " in dummy.\n";
							}
						}
						else 
						{
							// oops, we have no low cards in hand to lead!
							status << "4PLCSH21! Oops, we wanted to cash a " & STSS(m_nSuit) & 
									  " in dummy, but we have no low " & STS(m_nSuit) & 
									  " in hand to lead, so we have to abandon that play.\n";
							m_nStatusCode = PLAY_INACTIVE;
							return PLAY_POSTPONE;
						}
					}
					else
					{
						// oops, no card in the suit to lead!
						status << "4PLCSH22! Oops, we wanted to cash a " & STSS(m_nSuit) & 
								  " in dummy, but we have no " & STS(m_nSuit) & 
								  " in hand to lead, so we have to abandon that play.\n";
						m_nStatusCode = PLAY_NOT_VIABLE;
						return m_nStatusCode;
					}
				}
			}
			else
			{
				// playing in dummy
				if (m_nTargetHand == IN_DUMMY)
				{
					// we're leading from dummy and also cashing in dummy 
					// also check to be sure the player's hand has losers
					if ((declarerSuit.GetNumCards() == 1) && (combinedSuit.GetNumDeclarerLosers() == 0) && 
								(combinedSuit.GetNumDummyLosers() == 0) && (*declarerSuit[0] < *m_pConsumedCard))
					{
						// this is a special case (e.g., Kx/J), so it's OK
					}
					else if ((declarerSuit.GetNumCards() > 0) && (combinedSuit.GetNumDeclarerLosers() == 0) && 
								(combinedSuit.GetNumDummyLosers() > 0) &&
								(combinedSuit.GetNumDeclarerWinners() < dummySuit.GetNumCards()) &&
								(*declarerSuit[0] > *dummySuit[0]) &&
								(declarerEngine.GetNumDummyEntries() == 1) )
					{
						// here, nothing but higher winners in hand, which is shorter than dummy
						status << "5PLCSH12! We could cash the " & m_pConsumedCard->GetName() &
								  " from dummy, but we have no losers in hand to discard in the suit and could get stranded there.\n";
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}

					// proceed
					pPlayCard = m_pConsumedCard;
					status << "PLCSH30! Cash the " & pPlayCard->GetName() & " from dummy.\n";
				}
				else
				{
					// leading from dummy, but cashing from hand
					// lead a low card of the suit from dummy
					if (dummySuit.GetNumCards() > 0)
					{
//						if (combinedSuit.GetNumDummyLosers() > 0)
						if (dummySuit.GetNumCardsBelow(m_pConsumedCard) > 0)
						{
							pPlayCard = dummySuit.GetBottomCard();
							if (combinedSuit.AreEquivalentCards(pPlayCard, m_pConsumedCard)
//								 (dummySuit.GetNumCards() > 1) && (combinedSuit.GetNumDeclarerLosers() > 0) )
								&& (combinedSuit.GetNumDeclarerLosers() > 0) 
								// NCR-438 Ignore if flagged as Opportunistic
								&& !IsOpportunistic() )
							{
								// oops, the "low" card is equivalent to the cash card!
								status << "4PLCSH35! Oops, the lowest card we can lead from dummy for the cash is the " & 
										  pPlayCard->GetFaceName() & ", which is equivalent to the " &
										  m_pConsumedCard->GetFaceName() & ", so skip the cash play.\n";
								m_nStatusCode = PLAY_INACTIVE;
								return PLAY_POSTPONE;
							}
							else
							{
								status << "PLCSH40! Lead a low " & STSS(m_nSuit) & 
										  " (the " & pPlayCard->GetFaceName() &
										  ") from dummy in order to cash the " & 
										  m_pConsumedCard->GetFaceName() & " in hand.\n";
							}
						}
						else
						{
							// oops, we have no low cards in dummy to lead!
							status << "4PLCSH41! Oops, we wanted to cash a " & STSS(m_nSuit) & 
									  " in hand, but we have no low " & STS(m_nSuit) & 
									  " in dummy to lead, so we have to abandon that play.\n";
							m_nStatusCode = PLAY_INACTIVE;
							return PLAY_POSTPONE;
						}
					}
					else
					{
						// oops, no card in the suit to lead!
						status << "4PLCSH42! Oops, we wanted to cash a " & STSS(m_nSuit) & 
								  " in hand, but we have no " & STS(m_nSuit) & 
								  " in dummy to lead, so we have to abandon that play.\n";
						m_nStatusCode = PLAY_NOT_VIABLE;
						return m_nStatusCode;
					}
				}
			}
			// all went OK
			m_nStatusCode = PLAY_IN_PROGRESS;
			break;

		case 1:
			// playing second
			if (nSuitLed == m_nSuit)
			{
				// see if the card led is higher than ours
				if (*pCardLed > *m_pConsumedCard)
				{
					status << "3PLCSH53! the card led (" & pCardLed->GetName() & ") " & 
							  " beats the " & m_pConsumedCard->GetName() & 
							  " we were going to play, so skip the cash for now.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_POSTPONE;
				}
				// card led is not higher than our cash card
				// see if this is the correct hand for the cash
				if ( ((bPlayingInHand) && (m_nTargetHand == IN_HAND)) ||
				     ((!bPlayingInHand) && (m_nTargetHand == IN_DUMMY)) )
				{
					// this is the right time to play the cash card
					// but do a special test here -- see if partner 
					// has a singleton which is higher than this card
					if ( (bPlayingInHand && (dummySuit.GetNumCards() == 1) &&
					     (*(dummySuit[0]) > *m_pConsumedCard)) ||
						 (!bPlayingInHand && (declarerSuit.GetNumCards() == 1) &&
					     (*(declarerSuit[0]) > *m_pConsumedCard)) )
					{
						status << "3PLCSH54! Partner has a singleton which is higher than the " & 
								  m_pConsumedCard->GetName() & " we were going to cash, so skip the cash play for now.\n";
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
					
					// also check if partner has nothing but winners in his hand
					if ( (bPlayingInHand && (dummySuit.GetNumCards() > 0) && (combinedSuit.GetNumDummyLosers() == 0) 
						  && (combinedSuit.GetNumDeclarerLosers() > 0))
						|| (!bPlayingInHand && (declarerSuit.GetNumCards() > 0) && (combinedSuit.GetNumDeclarerLosers() == 0) 
						                                                  // NCR-622 Check really have winner
						     && (combinedSuit.GetNumDummyLosers() > 0) && (*(declarerSuit[0]) > *pCardLed)) )
					{
/*
 * ???
 */
						status << "5PLCSH54a! We could cash the " & m_pConsumedCard->GetName() & 
							(bPlayingInHand? " in hand" : " in dummy") & ", but we have no losers in " &
							(bPlayingInHand? " in dummy" : " in hand") & " in the suit, so skip this play.\n";
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
					// NCR-620 Check if LHO is void. Only need to cover lead card if so
					CPlayer* pLHOPlayer = bPlayingInHand ? playEngine.GetLHOpponent() : playEngine.GetRHOpponent();
	 	            CGuessedHandHoldings* pLHOHand = ppGuessedHands[pLHOPlayer->GetPosition()];
					if( pLHOHand->IsSuitShownOut(m_nSuit))
					{
						CHandHoldings& theHand = bPlayingInHand ? playerHand : dummyHand; 
						pPlayCard = theHand.GetSuit(m_nSuit).GetLowestCardAbove(pCardLed);
					}  // end NCR_620 getting lowest card needed to win
					else
						pPlayCard = m_pConsumedCard;
					status << "PLCSH55! The opponents led a " & STSS(m_nSuit) & 
							  ", so cash the " & pPlayCard->GetFaceName() & " now.\n";
				}
				else
				{
					// not the right hand, so discard from this hand
					// but skip if we have no losers in this hand but do in the other
					if ( (m_nTargetHand == IN_HAND) &&
								 (dummySuit.GetNumCards() > 0) && 
								 (combinedSuit.GetNumDummyLosers() == 0) &&
								 (combinedSuit.GetNumDeclarerLosers() > 0))
					{
						// no losers in dummy to discard, but some in hand!
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
					if ( (m_nTargetHand == IN_DUMMY) &&
								 (declarerSuit.GetNumCards() > 0) && 
								 (combinedSuit.GetNumDeclarerLosers() == 0) &&
								 (combinedSuit.GetNumDummyLosers() > 0))
					{
						// no losers in hand to discard, but some in dummy!
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}

					// NCR-586 Use discard chosen by analysis (See NCR-411)
					if(ISSUIT(m_nSuit2) && (dummyHand.GetSuit(m_nSuit2).GetNumCards() > 0) )
					{
						CHandHoldings& theHand = bPlayingInHand ? playerHand : dummyHand;  
						pPlayCard = theHand.GetSuit(m_nSuit2).GetBottomCard();
					}  // end NCR-586 using chosen suit
					else {
						// else go ahead and discard
						pPlayCard = playEngine.GetDiscard();   // NCR use GetSuit()
					}
					status << "PLCSH56! Discard a " & STSS(pPlayCard->GetSuit()) & " from " &
							  (bPlayingInHand? "hand" : "dummy") &
							  " in anticipation of cashing the " & 
							  m_pConsumedCard->GetFaceName() & " in " &
							  (bPlayingInHand? "dummy" : "hand") & ".\n";
				}
			}
			else
			{
				// wrong suit led, so no point here
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			// else we're ok
			m_nStatusCode = PLAY_IN_PROGRESS;
			break;


		case 2:
			// playing third -- this play may or may not be active
			// i.e., may have started one cash, then switched to another
			// see if the wrong suit was led 
			if (nSuitLed != m_nSuit)
			{
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			// see if RHO has trumped
			if (bTrumped)
			{
				status << "2PLCSH64! RHO has trumped, so abandon the cashing play for this round.\n";
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			// see if the current top card in the trick is higher than ours
			// NCR-62 How can TopCard and ConsumedCard be in the same hand???
			if (*pTopCard > *m_pConsumedCard)
			{
				if(pCardLed != m_pConsumedCard) // NCR this happens
				   status << "2PLCSH66! the " & m_pConsumedCard->GetName() & " can't beat the " &
						   pTopCard->GetName() & ", so skip the cash.\n";
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			// else check which hand we're playing in
			if (bPlayingInHand) 
			{
				// playing from our own hand (declarer)
				// see if it's time to cash
				if (m_nTargetHand == IN_HAND)
				{
					// proceed
					pPlayCard = m_pConsumedCard;
					status << "PLCSH68! Cash the " & pPlayCard->GetName() & " from hand.\n";
				}
				else
				{
					// cash is/was in dummy
					if (bTrumped)
					{
						pPlayCard = playEngine.GetDiscard();
						status << "2PLCSH70! RHO's trump negates our cash; discard the " &
								  pPlayCard->GetName() & ".\n";
					}
					else
					{
						// NCR-275 are we winning trick?  If not try to win
						if((pCardLed->GetFaceValue() < pTopCard->GetFaceValue())
							&& !declarerSuit.IsVoid())
						{
							pPlayCard = declarerSuit.GetLowestCardAbove(pTopCard);
							// NCR-ErrFix above can return NULL ???
							if(pPlayCard == NULL) // NCR-ErrFix
								pPlayCard = declarerSuit.GetTopCard(); // NCR ??? is this better
							status << "PLCSH70a! Our cash of the " &
									  pCardLed->GetName() & " from hand is not holding, so cover with the " &
									  pPlayCard->GetName() & ".\n";
}
						else  // NCR-275 tie into next if
						// see if we need to win in this hand to cash the remaining winnners
						if ((dummySuit.GetNumCards() == 0) && (declarerSuit.GetNumCards() > 1) && 
								(declarerSuit.GetNumLosers() == 0) && (*declarerSuit[0] > *pCardLed))
						{
							pPlayCard = declarerSuit.GetLowestCardAbove(pCardLed);
							status << "PLCSH71! Our cash of the " & pCardLed->GetName() & 
									  " from dummy is holding, but we need to win in the hand to cash the remaining winners, so play the " &
									  pPlayCard->GetName() & " here.\n";
						}
						else
						{
							// NCR-411 Test and use suit chosen earlier
							if(ISSUIT(m_nSuit2) && (playerHand.GetSuit(m_nSuit2).GetNumCards() > 0)
								    // Can't use if player has card of suit led
								&& (playerHand.GetSuit(pCardLed->GetSuit()).GetNumCards() == 0) )
								pPlayCard = playerHand.GetSuit(m_nSuit2).GetBottomCard();  // NCR-411 Discard from this one
							else
								pPlayCard = playEngine.GetDiscard();
							status << "PLCSH72! Our cash of the " & pCardLed->GetName() & 
									  " from dummy is holding, so discard the " & pPlayCard->GetName() & ".\n";
						}
					}
				}
			}
			else
			{
				// playing in dummy
				// see if it's time to cash
				if (m_nTargetHand == IN_DUMMY)
				{
					// proceed
					pPlayCard = m_pConsumedCard;
					status << "PLCSH74! Cash the " & pPlayCard->GetName() &
							  " from dummy.\n";
				}
				else
				{
					// cashed from hand, so discard
					if (bTrumped)
					{
						pPlayCard = playEngine.GetDiscard();
						status << "2PLCSH73! RHO's trump negates our cash; discard the " &
								  pPlayCard->GetName() & ".\n";
					}
					else
					{
						// see if we need to win here in dummy to cash the remaining winnners
						if ((declarerSuit.GetNumCards() == 0) && (dummySuit.GetNumCards() > 1) && 
								(dummySuit.GetNumLosers() == 0) && (*dummySuit[0] > *pCardLed))
						{
							pPlayCard = dummySuit.GetLowestCardAbove(pCardLed);
							status << "PLCSH77! Our cash of the " & pCardLed->GetName() & 
									  " from hand is holding, but we need to win in dummy to cash the remaining winners, so play the " &
									  pPlayCard->GetName() & " here.\n";
						}
						// NCR-84 need to worry about blocking - if NT and # cards in declarer > # cards in dummy
						// and there are no other cards out and ...
						// Play high in short hand unless ...
						else if((declarerSuit.GetNumCards() >= dummySuit.GetNumCards()) // = because declarer has lead
							     && ((combinedSuit.GetNumMissingSequences() == 0) 
								     || (combinedSuit.GetMissingSequence(0).GetTopCard() < dummySuit.GetTopCard())) 
								 && !combinedSuit.IsVoid() // NCR-604 make sure some
							     && (combinedSuit.GetSequence2(0).GetNumCards() 
								      > playEngine.GetNumOutstandingCards(nSuitLed)) )  // NCR-604 Make sure we have enough
						{
							pPlayCard = dummySuit.GetTopCard();
							if(pPlayCard == NULL) {  // How does this happen???
								m_nStatusCode = PLAY_INACTIVE;
								return PLAY_POSTPONE;
							}
							status << "PLCSH75! Our cash of the " & pCardLed->GetName() 
								      & " from hand is holding, so unblock the " 
									  & pPlayCard->GetName() & ".\n";
						} // NCR-84 end unblocking
						// NCR-139 Don't give up cheap trick. If RHO covered lead, cover if we can
						else if((pCardLed->GetFaceValue() < pTopCard->GetFaceValue())
								&& ((dummySuit.GetNumCardsAbove(pTopCard->GetFaceValue()) > 1)
								       // NCR-205 Cover if no Outstanding cards greater
								    || ((combinedSuit.GetNumMissingSequences() > 0) 
									   && (combinedSuit.GetMissingSequence(0).GetTopCard() < dummySuit.GetTopCard()))) )
						{
							pPlayCard = dummySuit.GetLowestCardAbove(pTopCard);
							// NCR-ErrFix above can return NULL ???
							if(pPlayCard == NULL) // NCR-ErrFix NCR-604 If no higher card, go low
								pPlayCard = dummySuit.GetBottomCard(); // NCR ??? is this better
							status << "PLCSH76! Our cash of the " &
									  pCardLed->GetName() & " from hand is not holding, so cover with the " &
									  pPlayCard->GetName() & ".\n";
						}
						// NCR-157 Play high to prevent cheap trick if have 2 equal value cards w/only one out greater
						else if(((dummySuit.GetNumSequence2s() > 0) 
							      && (dummySuit.GetSequence2(0).GetNumCards() > 1)) 
							    && ((dummySuit.GetNumMissingSequences() > 0)
								    && (dummySuit.GetMissingSequence(0).GetNumCards() == 1))
								&& ((combinedSuit.GetNumMissingSequences() > 0)
								    && (combinedSuit.GetNumMissingAbove(pCardLed) > 2)) )
						{
							pPlayCard = dummySuit.GetSecondHighestCard();
							status << "PLCSH79! Our cash of the " & pCardLed->GetName() 
								      & " from hand won't hold, so play the "
									  & pPlayCard->GetName() & ".\n";
						}
						else
						{
							// NCR-411 Test and use suit chosen earlier (this done on 6/7/2013)
							if(ISSUIT(m_nSuit2) && (dummyHand.GetSuit(m_nSuit2).GetNumCards() > 0)
								    // Can't use if player has card of suit led
								&& (dummyHand.GetSuit(pCardLed->GetSuit()).GetNumCards() == 0) )
								pPlayCard = dummyHand.GetSuit(m_nSuit2).GetBottomCard();  // NCR-411 Discard from this one
							else
								pPlayCard = playEngine.GetDiscard();
							status << "PLCSH78! Our cash of the " &
									  pCardLed->GetName() & " from hand is holding, so discard the " &
									  pPlayCard->GetName() & ".\n";
						}
					}
				}
			}
			// all's OK
			m_nStatusCode = PLAY_COMPLETE;
			break;


		case 3:
			// playing fourth
			// make sure the play is in progress
			if (m_nStatusCode != PLAY_IN_PROGRESS)
				return PLAY_INACTIVE;
			// since opponent led the suit, we are still trying to cash
			status << "3PLCSH80! the opponents led a " & STSS(nSuitLed) & 
					  ", so see if we can finish up the cash play here.\n";
			// see if RHO (or partner) has trumped
			if (bTrumped)
			{
				status << "2PLCSH82! The suit was trumped, so abandon the cashing play for this round.\n";
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			// see if the current (opponents') top card in the trick is higher than ours
			if ((!bPartnerHigh) && (*pTopCard > *m_pConsumedCard))
			{
				status << "2PLCSH83! the " & m_pConsumedCard->GetName() & " we intended to cash can't beat the " &
						   pTopCard->GetName() & ", so skip the cash.\n";
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			// else see if partner played a top card (which wasn't part of the cash)
			if ((bPartnerHigh) && (m_pConsumedCard != pPartnersCard))
			{
				status << "2PLCSH84! Partner's " & pTopCard->GetName() & " is high, so skip the cash of the " &
						  m_pConsumedCard->GetFaceName() & ".\n";
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			// else check which hand we're playing in
			if (bPlayingInHand) 
			{
				// playing from our own hand (declarer)
				// see if it's time to cash
				if (m_nTargetHand == IN_HAND)
				{
					// cashing from hand, so do it
					// BUT see if we have a lower card which would win!
					if (declarerSuit.GetNumCardsAbove(pTopCard) > 1)
					{
						CCard* pCard = declarerSuit.GetLowestCardAbove(pTopCard);
						status << "3PLCSH85! We can actually play the " & pCard->GetFaceName() &
								  " to win this trick, so postpone the cash of the " &
								  m_pConsumedCard->GetFaceName() & ".\n";
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
					pPlayCard = m_pConsumedCard;
					status << "PLCSH86! Cash the " & pPlayCard->GetName() & " from hand.\n";
				}
				else
				{
					// cash is/was in dummy, so discard
					if (bTrumped)
					{
						pPlayCard = playEngine.GetDiscard();
						status << "2PLCSH87! RHO's trump negates our cash; discard the " &
								  pPlayCard->GetName() & ".\n";
					}
					else
					{
						// see if we need to win in this hand to cash the remaining winnners
						if ((dummySuit.GetNumCards() == 0) && (declarerSuit.GetNumCards() > 1) && 
								(declarerSuit.GetNumLosers() == 0) && (*declarerSuit[0] > *pCardLed))
						{
							pPlayCard = declarerSuit.GetLowestCardAbove(pCardLed);
							status << "PLCSH88a! Our cash of the " & m_pConsumedCard->GetName() & 
									  " from dummy is holding, but we need to win in the hand to cash the remaining winners, so play the " &
									  pPlayCard->GetName() & " here.\n";
						}
						else
						{
							pPlayCard = playEngine.GetDiscard();
							status << "PLCSH88b! Our cash of the " &
									  m_pConsumedCard->GetName() & " from dummy is holding, so discard the " &
									  pPlayCard->GetName() & ".\n";
						}
					}
				}
			}
			else
			{
				// playing in dummy
				// see if it's time to cash
				if (m_nTargetHand == IN_DUMMY)
				{
					// cashing from dummy, so do it
					// BUT see if we have a lower card which would win!
					if (dummySuit.GetNumCardsAbove(pTopCard) > 1)
					{
						CCard* pCard = dummySuit.GetLowestCardAbove(pTopCard);
						status << "3PLCSH89! We can actually play the " & pCard->GetFaceName() &
								  " to win this trick, so postpone the cash of the " &
								  m_pConsumedCard->GetFaceName() & ".\n";
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
					pPlayCard = m_pConsumedCard;
					status << "PLCSH90! Cash the " & pPlayCard->GetName() &
							  " from dummy.\n";
				}
				else
				{
					// cashed from hand, so discard
					if (bTrumped)
					{
						pPlayCard = playEngine.GetDiscard();
						status << "2PLCSH91! RHO's trump negates our cash; discard the " &
								  pPlayCard->GetName() & ".\n";
					}
					else
					{
						// see if we need to win here in dummy to cash the remaining winnners
						if ((declarerSuit.GetNumCards() == 0) && (dummySuit.GetNumCards() > 1) && 
								(dummySuit.GetNumLosers() == 0) && (*dummySuit[0] > *pCardLed))
						{
							pPlayCard = dummySuit.GetLowestCardAbove(pCardLed);
							status << "PLCSH92a! Our cash of the " & m_pConsumedCard->GetName() & 
									  " from hand is holding, but we need to win in dummy to cash the remaining winners, so play the " &
									  pPlayCard->GetName() & " here.\n";
						}
						else
						{
							pPlayCard = playEngine.GetDiscard();
							status << "PLCSH92b! Our cash of the " &
									  m_pConsumedCard->GetName() & " from hand is holding, so discard the " &
									  pPlayCard->GetName() & ".\n";
						}
					}
				}
			}
			// all's OK
			m_nStatusCode = PLAY_COMPLETE;
			break;
	}

	// ### TEMP ###
	if (pDOC->GetCurrentPlayerPosition() == playEngine.GetPlayerPosition())
		ASSERT(playEngine.GetPlayer()->HasCard(pPlayCard));
	else
		ASSERT(playEngine.GetPartner()->HasCard(pPlayCard));

	// done
	ASSERT(pPlayCard->IsValid());
	return m_nStatusCode;
}

