//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// TrumpPull.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "Deck.h"
#include "Card.h"
#include "TrumpPull.h"
#include "Player.h"
#include "PlayEngine.h"
#include "CombinedHoldings.h"
#include "combinedSuitHoldings.h"
#include "CardLocation.h"
#include "GuessedHandHoldings.h"
#include "DeclarerPlayEngine.h"
#include "PlayerStatusDialog.h"



//
//==================================================================
// constructon & destruction

CTrumpPull::CTrumpPull(int nTargetHand, int nSuit, int nCardVal, CCardList* pRequiredPlayedCards, CCardList* pOutstandingCards, BOOL bWinner, BOOL bOptional) :
CPlay(CPlay::TRUMP_PULL, nTargetHand, nSuit, bWinner? PP_GUARANTEED_WINNER : PP_LOSER),
			m_nCardVal(nCardVal),
			m_pOutstandingCards(pOutstandingCards),
			m_bOptional(bOptional)
{
	Init();
	m_pRequiredPlayedCardsList = pRequiredPlayedCards;
}

CTrumpPull::CTrumpPull(int nTargetHand, CCard* pCard, CCardList* pRequiredPlayedCards, CCardList* pOutstandingCards, BOOL bWinner, BOOL bOptional) :
			CPlay(CPlay::TRUMP_PULL, nTargetHand, NONE, bWinner? PP_GUARANTEED_WINNER : PP_LOSER),
			m_pOutstandingCards(pOutstandingCards),
			m_bOptional(bOptional)
{
	VERIFY(pCard);
	m_nSuit = pCard->GetSuit(); 
	m_nCardVal = pCard->GetFaceValue();
	Init();
	m_pRequiredPlayedCardsList = pRequiredPlayedCards;
}

CTrumpPull::~CTrumpPull() 
{
}


//
void CTrumpPull::Clear()
{
	CPlay::Clear();
	//
	m_nCardVal = NONE;
}


//
void CTrumpPull::Init()
{
	CPlay::Init();
	//
	m_pConsumedCard = deck.GetCard(m_nSuit, m_nCardVal);
	m_pEnemyOrKeyCardsList = m_pOutstandingCards;	// will be deleted by base class
	// form name & description
	m_strName.Format("%s Trump Pull", m_pConsumedCard->GetName());
	m_strDescription.Format("Pull trumps with the %c", m_pConsumedCard->GetCardLetter());
}


//
CString CTrumpPull::GetFullDescription()
{
	CString strText;
	strText.Format("Pull a round of trumps with the %s in %s%s.",
				   m_pConsumedCard->GetFaceName(),
				   ((m_nTargetHand == IN_HAND)? "hand" : "dummy"),
				   (m_bOptional? " if necessary" : ""));
	// done
	return strText;
}



//
int	CTrumpPull::UsesUpEntry()
{
	// this play does use up an entry
	return (m_nTargetHand == IN_DUMMY)? ENTRY_DUMMY: ENTRY_HAND;
}




//
// Perform()
//
// this is similar to the cashing play, with a few exceptions
//
PlayResult CTrumpPull::Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
					    CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, 
					    CPlayerStatusDialog& status, CCard*& pPlayCard)
{
	// trumps can only be pulled if we're leading (???)
	int nOrdinal = pDOC->GetNumCardsPlayedInRound();
	if ((nOrdinal != 0) && (nOrdinal != 2))
	{
		m_nStatusCode = PLAY_INACTIVE;
		return PLAY_POSTPONE;
	}

	// check which hand this is
	CPlayer* pPlayer = playEngine.GetPlayer();
	CDeclarerPlayEngine& declarerEngine = (CDeclarerPlayEngine&) playEngine;
	BOOL bPlayingInHand = (pDOC->GetCurrentPlayer() == pPlayer);
	CHandHoldings& playerHand = *(combinedHand.GetPlayerHand());
	CHandHoldings& dummyHand = *(combinedHand.GetPartnerHand());
	CString strRHO = PositionToString(GetPrevPlayer(playEngine.GetPlayerPosition()));
	const int nTrumpSuit = pDOC->GetTrumpSuit();
	CSuitHoldings& playerTrumps = playerHand.GetSuit(nTrumpSuit);
	CSuitHoldings& dummyTrumps = dummyHand.GetSuit(nTrumpSuit);
	CCombinedSuitHoldings& combinedTrumps = combinedHand.GetSuit(nTrumpSuit);
	CCard* pCardLed = pDOC->GetCurrentTrickCardByOrder(0);
	// see what the top card in the round is
	CCard* pTopCard = pDOC->GetCurrentTrickHighCard();
	CCard* pDeclarerCard = pDOC->GetCurrentTrickCard(playEngine.GetPlayerPosition());
	CCard* pDummysCard = pDOC->GetCurrentTrickCard(playEngine.GetPartnerPosition());
	CCard* pPartnersCard = bPlayingInHand? pDummysCard : pDeclarerCard;
	BOOL bPartnerHigh = (pTopCard == pPartnersCard);
	//
	const int nSuitLed = pCardLed? pCardLed->GetSuit() : NONE;
	pPlayCard = NULL;
	CCard* pOutstandingCard = NULL;

	//
	if (!CPlay::IsPlayUsable(combinedHand, playEngine) 
		|| (combinedTrumps.GetNumCards() == 0)) // NCR-30 don't pull if no trumps
	{
		m_nStatusCode = PLAY_INACTIVE;
		return PLAY_POSTPONE;
	}


	// NCR-12 Don't pull last outstanding trump if its a winner
	// There is a special case where you want to run a long suit and don't want opponents to
	// trump in to stop the run.  How/where to test for this???
	CCardList outstandingTrumps;
	playEngine.GetOutstandingCards(nTrumpSuit, outstandingTrumps);
	if((outstandingTrumps.GetNumCards() == 1)
		// NCR-28 added ->GetFaceValue() to following CCard*
		&& (outstandingTrumps.GetTopCard()->GetFaceValue() > combinedTrumps.GetTopCard()->GetFaceValue()))
	{
		status << "4PLTRMP1! Oops, don't pull trump if the one outstanding is a winner.\n";
		m_nStatusCode = PLAY_INACTIVE;
		return PLAY_POSTPONE;
	} // NCR-12 end ignoring singleton top trump outstanding

	CGuessedHandHoldings* pLHOHand = NULL;  // NCR-413 Define here, used in case below
	//
	switch(nOrdinal)
	{
		case 0:
			// leading this round
			if (bPlayingInHand) 
			{
				// leading from hand
				// check if this is the correct hand for the play
				if (m_nTargetHand == IN_HAND)
				{
					// leading from declarer's hand
					// make sure declarer has at least one trump
					if (playerTrumps.GetNumCards() == 0)
					{
						// oops, no trumps in hand -- we're screwed!
						status << "4PLTRMP10! Oops, want to pull a trump, but the declarer's hand does not have any trumps to lead, so skip this play.\n";
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}

					// pullling from hand -- apply the same test used in cashes
					// e.g., with Kx/A, don't lead the King!
					if ((dummyTrumps.GetNumCards() == 1) && (combinedTrumps.GetNumDummyLosers() == 0) && 
								(combinedTrumps.GetNumDeclarerLosers() == 0) && (*dummyTrumps[0] < *m_pConsumedCard))
					{
						// this is a special case, so it's OK
					}
					else if ((dummyTrumps.GetNumCards() > 0) && (combinedTrumps.GetNumDummyLosers() == 0) && 
								(combinedTrumps.GetNumDeclarerLosers() > 0) &&
								(combinedTrumps.GetNumDummyWinners() < playerTrumps.GetNumCards()) &&
								(*dummyTrumps[0] > *playerTrumps[0]) &&
								(declarerEngine.GetNumDeclarerEntries() == 1) )
					{
						// here, nothing but higher winners in dummy, which is shorter than hand
						status << "5PLTRMP11! We could pull trumps with the " & m_pConsumedCard->GetName() &
								  " from hand, but dummy has no trump losers to discard while declarer does.\n";
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}

					// if there's a top trump in the declarer's hand, lead it
					if (combinedTrumps.GetTopSequence().HasCard(playerTrumps.GetTopCard())
						// NCR-667 Also allow lead if this hand has high card
						|| ((outstandingTrumps.GetNumCards() > 0)
						     && (playerTrumps.GetTopCard() > outstandingTrumps.GetTopCard())) )
					{
						// just lead the bottom card of the top sequence
						pPlayCard = playerTrumps.GetTopSequence().GetBottomCard();
						// NCR if dummy has a singleton of higher value, play low
						if(dummyTrumps.IsSingleton() 
						    // NCR-609 Don't play low (may block) if hand can pull all trumps
  							&& (outstandingTrumps.GetNumCards() > playerTrumps.GetSequence2(0).GetNumCards()) 
							&& (combinedTrumps.AreEquivalentCards(dummyTrumps[0], pPlayCard)))
						{
							pPlayCard = playerTrumps.GetBottomCard();
						}
						status << "PLTRMP12! Lead out the " & pPlayCard->GetName() &
								  " from hand to pull in a round of trumps.\n";
					}
					else
					{
						// oops, no high trumps left in hand to lead!
						status << "4PLTRMP13! Error - no high trump in hand to lead.\n";
						m_nStatusCode = PLAY_ERROR;
						return m_nStatusCode;
					}
				}
				else
				{
					// high trump is in dummy -- lead a low trump from hand if possible
					if (playerTrumps.GetNumCards() > 0)
					{
						// but test for equality
						pPlayCard = playerTrumps.GetBottomCard();
						if ( combinedTrumps .AreEquivalentCards(pPlayCard, m_pConsumedCard) &&
											(dummyTrumps.GetNumLosers() > 1) )
						{
							// oops, the "low" card is equivalent to the trump pull ash card!
							status << "3PLTMRP15! Oops, the lowest trump we can lead from hand is the " & 
									  pPlayCard->GetFaceName() & ", which is equivalent to the " &
									  m_pConsumedCard->GetFaceName() & ", so skip this play.\n";
							m_nStatusCode = PLAY_INACTIVE;
							return PLAY_POSTPONE;
						}
						else
						{
							status << "PLTRMP18! Lead the " & pPlayCard->GetName() &
									  " up to dummy to pull in a round of trumps.\n";
						}
					}
					else
					{
						// oops, no trumps to lead in hand!
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
				}
			}
			else
			{
				// pulling from dummy's hand
				if (m_nTargetHand == IN_DUMMY)
				{
					// make sure dummy has at least one trump
					if (dummyTrumps.GetNumCards() == 0)
					{
						// oops, no trumps in dummy -- we're screwed!
						status << "4PLTRMP20! Oops, want to pull a trump, but the dummy's hand does not have any trumps to lead, so skip this play.\n";
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}

					// also check to be sure the player's hand has losers
					if ((playerTrumps.GetNumCards() == 1) && (combinedTrumps.GetNumDeclarerLosers() == 0) && 
								(combinedTrumps.GetNumDummyLosers() == 0) && (*playerTrumps[0] < *m_pConsumedCard))
					{
						// this is a special case (e.g., Kx/J), so it's OK
					}
					else if ((playerTrumps.GetNumCards() > 0) && (combinedTrumps.GetNumDeclarerLosers() == 0) && 
								(combinedTrumps.GetNumDummyLosers() > 0) &&
								(combinedTrumps.GetNumDeclarerWinners() < dummyTrumps.GetNumCards()) &&
								(*playerTrumps[0] > *dummyTrumps[0]) &&
								(declarerEngine.GetNumDummyEntries() == 1) )
					{
						// here, nothing but higher winners in hand, which is shorter than dummy
						status << "5PLTRMP21! We could pull trumps with the " & m_pConsumedCard->GetName() &
								  " from dummy, but we have no trump losers in hand to discard.\n";
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
					
					// if there's a top trump in the dummy's hand, lead it
					// NCR-488 Use Sequence2 vs Sequence below
					if (combinedTrumps.GetSequence2(0).HasCard(dummyTrumps.GetTopCard()))
					{
						// just lead the bottom card of the top sequence
						pPlayCard = dummyTrumps.GetTopSequence().GetBottomCard();
						pPlayCard = m_pConsumedCard; // NCR-569
						status << "PLTRMP22! Lead out the " & pPlayCard->GetName() &
								  " from dummy to pull in a round of trumps.\n";
					}
					else
					{
						// oops, no high trumps left in dummy to lead!
						status << "4PLTRMP23! Error - no high trump in dummy to lead.\n";
						m_nStatusCode = PLAY_ERROR;
						return m_nStatusCode;
					}
				}
				else
				{
					// high trump is in declarer's hand -- lead a low trump from dummy if possible
					if (dummyTrumps.GetNumCards() > 0)
					{
						// NCR test if we could pull all outstanding trumps by playing high
						CCard* pTopOutstandingTrump = playEngine.GetHighestOutstandingCard(nTrumpSuit); // NCR-401
						if(((playEngine.GetNumOutstandingCards(nTrumpSuit) / 2) 
							  <= combinedTrumps.GetTopSequence().GetNumCards())
						   && (pTopOutstandingTrump < dummyTrumps.GetTopCard()) 
						   && (pTopOutstandingTrump > m_pConsumedCard) )  // NCR-401 don't play high if declarer can win	
						{
							pPlayCard = dummyTrumps.GetTopCard(); // NCR is this one of the high ones???
							status << "PLTRMP24! Try to pull trumps by playing high with " 
								    & pPlayCard->GetName() & ".\n";
						}
						else
						{
						// but test for equality
						pPlayCard = dummyTrumps.GetBottomCard();
						if (combinedTrumps.AreEquivalentCards(pPlayCard, m_pConsumedCard)
							                                          // NCR-321 combined vs player?
							 &&	/*(playerTrumps.GetNumLosers() > 1)  &&*/ (combinedTrumps.GetNumLosers() > 1) )
						{
							// oops, the "low" card is equivalent to the trump pull card!
							status << "3PLTMRP25! Oops, the lowest trump we can lead from dummy is the " & 
									  pPlayCard->GetFaceName() & ", which is equivalent to the " &
									  m_pConsumedCard->GetFaceName() & ", so skip this play.\n";
							m_nStatusCode = PLAY_INACTIVE;
							return PLAY_POSTPONE;
						}
						else
						{
							status << "PLTRMP28! Lead the " & pPlayCard->GetName() &
									  " up to own hand to pull in a round of trumps.\n";
						}
						}
					}
					else
					{
						// oops, no trumps to lead in hand!
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
				}
			}
			// mark play status
			m_nStatusCode = PLAY_IN_PROGRESS;
			break;


		case 1:
			// playing second -- can still use this play if trumps were led
			if (nSuitLed != nTrumpSuit)
			{
				// but no go here
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			//
			status << "3PLTRMP30! " & strRHO & 
					  " led a trump, so we may still be able pull in a round of trumps.\n";
			// check which hand we're in
			if (bPlayingInHand) 
			{
				// playing second in hand
				// check if this is the correct hand for the play
				if (m_nTargetHand == IN_HAND)
				{
					// playing from declarer's hand -- make sure we have at least one trump
					if (playerTrumps.GetNumCards() == 0)
					{
						// oops, no trumps in hand -- we're screwed!
						status << "4PLTRMP31! Oops, want to play a trump, but the declarer's hand is out of trumps, so skip this play.\n";
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
					// if there's a top trump in the declarer's hand, lead it
					if (combinedTrumps.GetTopSequence().HasCard(playerTrumps.GetTopCard()))
						pPlayCard = playerTrumps.GetTopSequence().GetBottomCard();
					if (*pPlayCard > *pCardLed)
					{
						// play this card
						status << "PLTRMP32! Play the " & pPlayCard->GetName() &
								  " from hand to top " & strRHO & "'s " & pCardLed->GetFaceName() & 
								  " and collect the round of trumps.\n";
					}
					else
					{
						// else lead a low trump from hand
/*
						pPlayCard = playerTrumps.GetBottomCard();
						status << "3PLTRMP34! We can't beat " & strRHO & "'s " & pCardLed->GetFaceName() & 
								  ", so discard the " & pPlayCard->GetFaceName() & ".\n";
*/
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
				}
				else
				{
					// play a low trump here
					if (playerTrumps.GetNumCards() > 0)
					{
						pPlayCard = playerTrumps.GetBottomCard();
						status << "PLTRMP38! Play a low trump (the " & pPlayCard->GetFaceName() & 
								  ") from hand in anticipation of playing the " & m_pConsumedCard->GetFaceName() &
								  " from dummy.\n";
					}
					else
					{
						// no trump to play
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
				}
			}
			else
			{
				// playing second in dummy
				// check if this is the correct hand for the play
				if (m_nTargetHand == IN_DUMMY)
				{
					// playing from dummy's hand -- make sure we have at least one trump
					if (dummyTrumps.GetNumCards() == 0)
					{
						// oops, no trumps in dummy -- we're screwed!
						status << "4PLTRMP41! Oops, want to play a trump, but dummy's hand is out of trumps,"
							       & " so skip this play.\n";
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
					// if there's a top trump in the dummy's hand, lead it
					if (combinedTrumps.GetTopSequence().HasCard(dummyTrumps.GetTopCard()))
						pPlayCard = dummyTrumps.GetTopSequence().GetBottomCard();
					if (*pPlayCard > *pCardLed)
					{
						// play this card
						status << "PLTRMP42! Play the " & pPlayCard->GetName() &
								  " from dummy to top " & strRHO & "'s " & pCardLed->GetFaceName() & 
								  " and collect the round of trumps.\n";
					}
					else
					{
						// else just discard a low trump from dummy
/*
						pPlayCard = dummyTrumps.GetBottomCard();
						status << "PLTRMP42! We can't beat " & strRHO & "'s " & pCardLed->GetFaceName() & 
								  ", so discard the " & pPlayCard->GetFaceName() & ".\n";
*/
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
				}
				else
				{
					// play a low trump here
					if (dummyTrumps.GetNumCards() > 0)
					{
						pPlayCard = dummyTrumps.GetBottomCard();
						status << "PLTRMP48! Play a low trump (the " & pPlayCard->GetFaceName() & 
								  ") from dummy in anticipation of playing the " & m_pConsumedCard->GetFaceName() &
								  " from hand.\n";
					}
					else
					{
						// no trump to play
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
				}
			}
			// mark play status
			m_nStatusCode = PLAY_IN_PROGRESS;
			break;


		case 2 : 
			// playing 3rd -- as with a cash, the play may or may not be active,
			// as a different trump pull may have been intended byt failed
			if (nSuitLed != nTrumpSuit)
			{
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}

			// NCR-413 Check if LHO is void
			pLHOHand = ppGuessedHands[playEngine.GetLHOpponent()->GetPosition()];

			// get the top outstanding trump
			pOutstandingCard = playEngine.GetHighestOutstandingCard(nTrumpSuit);
			int nTopTrump;
			if (pOutstandingCard != NULL)
				nTopTrump = Max(pTopCard->GetFaceValue(), pOutstandingCard->GetFaceValue());
			else
				nTopTrump = pTopCard->GetFaceValue();
			//
			if (bPlayingInHand) 
			{
				//
				// playing third in hand -- see if partner is high
				//
				if (bPartnerHigh) 
				{
					// see if there are no outstanding trumps
					if (pOutstandingCard == NULL)
					{
						// no outstanding higher trumps, so we've got the trick won
						pPlayCard = playerHand.GetDiscard();
						status << "PLTRMP82! Since there are no more outstanding trumps, " & 
								  " partner's " & pTopCard->GetFaceName() & 
								  " is high, so discard the " & pCardLed->GetName() & " from hand.\n";
					}
					else if (*pPartnersCard > *pOutstandingCard)
					{
						// partner's card is higher than any outstanding 
						// trump, so discard a low trump
						// NCR-609 Unless it blocks
						if ((dummyTrumps.GetNumCards() < 1) 
							&& (outstandingTrumps.GetNumCards() 
							    <= playerTrumps.GetSequence2(0).GetNumCards()) )  {
							pPlayCard = playerTrumps.GetTopCard();  // NCR-609 Overtake
							
						}
						else
						   pPlayCard = playerHand.GetDiscard();
						status << "PLTRMP82A! Discard the " & pPlayCard->GetName() & 
								  " on dummy's top " & pCardLed->GetFaceName() & " of trumps.\n";
					}
					else if (playerTrumps.GetNumCardsAbove(pOutstandingCard) > 0)
					{
						// partner's card is high, but there are still outstanding
						// trumps that are higher, so gotta play the highest card we have
						pPlayCard = playerTrumps.GetLowestCardAbove(pOutstandingCard);
						status << "PLTRMP82B! Since LHO may hold the trump " & pOutstandingCard->GetFaceName() &
								  ", we have to play the " & pPlayCard->GetFaceName() & ".\n";
					}
					else
					{
						// NCR play high enough to NOT let a low card from opponents win
						                                           // NCR-ErrFix  
						if((pPartnersCard->GetFaceValue() < TEN) && !playerTrumps.IsVoid()
							&& ((combinedTrumps.GetTopSequence().GetNumCards() >= 3)
							      // NCR-23 also check if have second sequence of 3 cards or more
							     || ((combinedTrumps.GetNumSequences() > 1) 
								     && (combinedTrumps.GetSecondSequence().GetNumCards() >= 3))) )
						{
							CCard *pTestCard = NULL;
							if(combinedTrumps.GetNumSequences() > 1) 
								pTestCard = combinedTrumps.GetSecondSequence().GetTopCard();
							else
								pTestCard = combinedTrumps.GetTopSequence().GetBottomCard();
							pPlayCard = playerTrumps.GetLowestCardAbove(pTestCard);
							if(pPlayCard == NULL) 
							{
								if(playerTrumps.HasCard(pTestCard))
									pPlayCard = pTestCard;
								else
									pPlayCard = playerTrumps.GetHighestCardBelow(pTestCard);
								if(pPlayCard == NULL)   // NCR above returns null ???
									pPlayCard = playerTrumps.GetTopCard(); // NCR which to get???
							}
							status << "PLTRMP82D! Play high enough trump with the " & pPlayCard->GetFaceName() 
								        & " to not allow cheap win.\n";
						}
						else
						{
						// else hope partner's card wins
						pPlayCard = playerHand.GetDiscard();
						status << "PLTRMP82C! Although LHO may hold the trump " & pOutstandingCard->GetFaceName() &
								  ", we have nothing to beat it, so hope partner's " & pTopCard->GetFaceName() & 
								  " wins and discard the " & pPlayCard->GetFaceName() & ".\n";
						}
					}
				}
				else 
				{
					// partner did NOT play the top trump, so try to beat 
					// RHO and/or the top outstanding card if possible
					if (playerTrumps.GetNumCardsAbove(nTopTrump) > 0)
					{
						// can top the highest trump the opponents have
						// NCR-413 Only need to beat top card if LHO has shown out
						int nValToBeat = pLHOHand->IsSuitShownOut(nTrumpSuit) ? pTopCard->GetFaceValue() : nTopTrump;
						pPlayCard = playerTrumps.GetLowestCardAbove(nValToBeat); // NCR-413 nTopTrump);
						status << "PLTRMP82E! Play high to take the round of trumps with the " & 
								  pPlayCard->GetFaceName() & ".\n";
					}
					else if (playerTrumps.GetNumCardsAbove(pTopCard))
					{
						// NCR check if we have a sequence of at least 3 cards - if so, play one
						if((combinedTrumps.GetTopSequence().GetNumCards() >= 3)
							|| ((playEngine.GetNumOutstandingCards(nTrumpSuit) <= 2) // NCR or if only 2 out
							    && (combinedTrumps.GetTopSequence().GetNumCards() >= 2)) ) 
						{
							// NCR ??? what if all cards in dummy
							pPlayCard = playerTrumps.GetTopSequence().GetBottomCard(); // NCR get a high card
						}
						else
						{
							// NCR play a higher card than one just above current top
							if(playerTrumps.GetNumSequences() > 1) 
							{
								pPlayCard = playerTrumps.GetSecondSequence().GetTopCard();
								// NCR-87 above will NOT always get winning card
								if(pPlayCard->GetFaceValue() < pTopCard->GetFaceValue())
									pPlayCard = playerTrumps.GetLowestCardAbove(pTopCard); // NCR-87 insure a winner
							}
							else
								// can top the highest trump the opponents _played_
								pPlayCard = playerTrumps.GetLowestCardAbove(pTopCard);
						}
						status << "PLTRMP82e! Top RHO's " & pTopCard->GetFaceName() &  // NCR changed from 83a
								  " of trumps with the " & pPlayCard->GetFaceName() & ".\n";
					}
					else
					{
						// can't do a trump pull here
//						status << "2PLTRMP83b! Since we can't beat opponents, skip the trump pull.\n";
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
				}
			}
			else
			{
				//
				// playing third in dummy -- see if partner is high
				//
				if (bPartnerHigh) 
				{
					// see if there are no outstanding trumps
					if (pOutstandingCard == NULL)
					{
						// no outstanding higher trumps, so we've got the trick won
						pPlayCard = dummyHand.GetDiscard();
						status << "PLTRMP82f! Since there are no more outstanding trumps, " & 
								  " partner's " & pTopCard->GetFaceName() & 
								  " is high, so discard the " & pCardLed->GetName() & " from dummy.\n";
					}
					else if (*pDeclarerCard > *pOutstandingCard)
					{
						// partner's card is higher than any outstanding 
						// trump, so discard a low trump
						pPlayCard = dummyHand.GetDiscard();
						// NCR-202 Problem here - discard is only entry to board
						status << "PLTRMP82a! Discard the " & pPlayCard->GetName() & 
								  " on declarer's top " & pCardLed->GetFaceName() & " of trumps.\n";
					}
					else if (dummyTrumps.GetNumCardsAbove(pOutstandingCard) > 0)
					{
						// partner's card is high, but there are still outstanding
						// trumps that are higher, so gotta play the highest card we have
						pPlayCard = dummyTrumps.GetLowestCardAbove(pOutstandingCard);
						status << "PLTRMP82b! Since LHO may hold the trump " & pOutstandingCard->GetFaceName() &
								  ", we have to play the " & pPlayCard->GetFaceName() & " from dummy.\n";
					}
					else
					{
						// NCR-31 play high enough to NOT let a low card from opponents win
						if((pPartnersCard->GetFaceValue() < TEN) && !dummyTrumps.IsVoid()   // NCR-86 must have trumps
							&& ((combinedTrumps.GetTopSequence().GetNumCards() >= 3)
							      // NCR-23 also check if have second sequence of 3 cards or more
							     || ((combinedTrumps.GetNumSequences() > 1) 
								     && (combinedTrumps.GetSecondSequence().GetNumCards() >= 3))     
                                 // NCR-318 Check if LHO can get cheap trick
							     || ((outstandingTrumps.GetNumCards() <= 2)	
								      && (outstandingTrumps.GetNumCardsAbove(dummyTrumps.GetTopCard()) <= 1))) 
						  ) 
						{
							CCard *pTestCard = NULL;
							if(combinedTrumps.GetNumSequences() > 1) 
								pTestCard = combinedTrumps.GetSecondSequence().GetTopCard();
							else
								pTestCard = combinedTrumps.GetTopSequence().GetBottomCard();
							pPlayCard = dummyTrumps.GetLowestCardAbove(pTestCard);
							if(pPlayCard == NULL) {
								if(dummyTrumps.HasCard(pTestCard))
									pPlayCard = pTestCard;
								else
									pPlayCard = dummyTrumps.GetHighestCardBelow(pTestCard);
							}
							status << "PLTRMP82d! Play high enough trump with the " & pPlayCard->GetFaceName() 
								        & " to not allow cheap win.\n";
						}
						else
						{
						// else hope partner's card wins
						pPlayCard = dummyHand.GetDiscard();
						status << "PLTRMP82c! although LHO may hold the trump " & pOutstandingCard->GetFaceName() &
								  ", we have nothing to beat it, so hope partner's " & pTopCard->GetFaceName() & 
								  " wins and discard the " & pPlayCard->GetFaceName() & ".\n";
						}
					}
				}
				else 
				{
					// partner did NOT play the top trump, so try to beat 
					// RHO and/or the top outstanding card if possible
					if (dummyTrumps.GetNumCardsAbove(nTopTrump) > 0)
					{
						// can top the highest trump the opponents have
						pPlayCard = dummyTrumps.GetLowestCardAbove(nTopTrump);
						status << "PLTRMP83! Play high to take the round of trumps with the " & 
								  pPlayCard->GetFaceName() & ".\n";
					}
					else if (dummyTrumps.GetNumCardsAbove(pTopCard))
					{
						// can top the highest trump the opponents _played_
						pPlayCard = dummyTrumps.GetLowestCardAbove(pTopCard);
						status << "PLTRMP83a! Top RHO's " & pTopCard->GetFaceName() & 
								  " of trumps with the " & pPlayCard->GetFaceName() & ".\n";
					}
					else
					{
						// can't do a trump pull here
//						status << "2PLTRMP83b! Since we can't beat opponents, skip the trump pull.\n";
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
				}
			}
			// mark play status
			m_nStatusCode = PLAY_COMPLETE;
			break;


		case 3:
			// playing 4th -- see if the play is active
			if (m_nStatusCode != PLAY_IN_PROGRESS)
				return PLAY_INACTIVE;
			//
			if (bPlayingInHand) 
			{
				// playing fourth in hand -- see if partner's card is high
				if (bPartnerHigh)
				{
					// simply discard
					pPlayCard = playerHand.GetDiscard();
					if (*pPlayCard < *pTopCard)
						status << "PLTRMP91! Since partner's " & pTopCard->GetName() & 
								  " is high, discard the " & pPlayCard->GetFaceName() & ".\n";
				}
				else
				{
					// else play a high trump from hand if we can
					if ((nSuitLed == nTrumpSuit) && (playerTrumps.GetNumCardsAbove(pTopCard) > 0))
					{
						pPlayCard = playerTrumps.GetLowestCardAbove(pTopCard);
						status << "PLTRMP94! Take the round of trumps with the " & 
								  pPlayCard->GetFaceName() & ".\n";
					}
					else
					{
						// have no high trump -- can't do a trump pull here
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
				}
			}
			else
			{
				// playing third or fourth in dummy
				// see if we played a high trump from dummy, or there are no
				// outstanding trumps
				if (bPartnerHigh)
				{
					pPlayCard = dummyHand.GetDiscard();
					if (*pPlayCard < *pTopCard)
						status << "PLTRMP96! Since partner's " & pTopCard->GetName() & 
								  " is high, discard the " & pPlayCard->GetFaceName() & ".\n";
				}
				else
				{
					// else play a high trump from dummy if we can
					if ((nSuitLed == nTrumpSuit) && (dummyTrumps.GetNumCardsAbove(pTopCard) > 0))
					{
						pPlayCard = dummyTrumps.GetLowestCardAbove(pTopCard);
						status << "PLTRMP98! Take the round of trumps with the " & 
								  pPlayCard->GetFaceName() & ".\n";
					}
					else
					{
						// got no high trump -- can't do a trump pull here
						m_nStatusCode = PLAY_INACTIVE;
						return PLAY_POSTPONE;
					}
				}
			}
			// mark play status
			m_nStatusCode = PLAY_COMPLETE;
			break;

	} // end switch(nOrdinal)

	// done
	ASSERT(pPlayCard->IsValid());
	return m_nStatusCode;
}
