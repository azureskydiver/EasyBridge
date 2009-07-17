//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Ruff.cpp
//
#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "card.h"
#include "Ruff.h"
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

CRuff::CRuff(int nTargetHand, int numDiscardsRequired, int nSuit, PlayProspect nPlayProspect) :
		CPlay(CPlay::RUFF, nTargetHand, nSuit, nPlayProspect),
		m_numDiscardsRequired(numDiscardsRequired)
{
	Init();
}

CRuff::CRuff(int nTargetHand, int numDiscardsRequired, CCard* pCard, PlayProspect nPlayProspect) :
		CPlay(CPlay::RUFF, nTargetHand, NONE, nPlayProspect),
		m_numDiscardsRequired(numDiscardsRequired)
{
	VERIFY(pCard);
	m_nSuit = pCard->GetSuit(); 
	Init();
}

CRuff::CRuff(CRuff& srcPlay) :
		CPlay(srcPlay.m_nPlayType)
{
	m_nTargetHand = srcPlay.m_nTargetHand;
	m_nStartingHand = srcPlay.m_nStartingHand;
	m_nEndingHand = srcPlay.m_nEndingHand;
	m_numDiscardsRequired = srcPlay.m_numDiscardsRequired;
}

CRuff::~CRuff() 
{
}


//
void CRuff::Clear()
{
	CPlay::Clear();
	//
	m_nSuit = NONE;
}


//
void CRuff::Init()
{
	CPlay::Init();
	//
	m_nEndingHand = m_nTargetHand;
	m_nStartingHand = (m_nEndingHand == IN_HAND)? IN_DUMMY : IN_HAND;
	// form name & description
	m_strName.Format("%s Ruff", STSS(m_nSuit));
	m_strDescription.Format("Ruff a %s in %s", 
							SuitToSingularString(m_nSuit),
							((m_nTargetHand == IN_HAND)? "hand" : "dummy"));
}



//
CString CRuff::GetFullDescription()
{
	return FormString("Ruff a %s in %s.",
					   SuitToSingularString(m_nSuit),
					   (m_nTargetHand == IN_HAND)? "hand" : "dummy");
}


//
int	CRuff::UsesUpEntry()
{
	// this play does use up an entry
	return (m_nTargetHand == IN_DUMMY)? ENTRY_DUMMY: ENTRY_HAND;
}



//
// Perform()
//
PlayResult CRuff::Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
				   CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, 
				   CPlayerStatusDialog& status, CCard*& pPlayCard)
{
	// check which hand this is
	int nOrdinal = pDOC->GetNumCardsPlayedInRound();
	CPlayer* pPlayer = playEngine.GetPlayer();
	BOOL bPlayingInHand = (pDOC->GetCurrentPlayer() == pPlayer);
	CHandHoldings& playerHand = *(combinedHand.GetPlayerHand());
	CHandHoldings& dummyHand = *(combinedHand.GetPartnerHand());
	CSuitHoldings& playerSuit = playerHand.GetSuit(m_nSuit);
	CSuitHoldings& dummySuit = dummyHand.GetSuit(m_nSuit);
	CCombinedSuitHoldings& combinedSuit = combinedHand.GetSuit(m_nSuit);
	CCard* pCardLed = pDOC->GetCurrentTrickCardByOrder(0);
	int nSuitLed = NONE;
	if (pCardLed)
		nSuitLed = pCardLed->GetSuit();
	// see if a trump was played in this round
	BOOL bTrumped = FALSE;
	int nTrumpSuit = pDOC->GetTrumpSuit();
	if ((nSuitLed != nTrumpSuit) && (pDOC->WasTrumpPlayed()))
		bTrumped = TRUE;
	pPlayCard = NULL;
	CCard* pOppCard = NULL;
	//
	CCard* pRoundTopCard = pDOC->GetCurrentTrickHighCard();
	CCard* pDeclarerCard = pDOC->GetCurrentTrickCard(playEngine.GetPlayerPosition());
	CCard* pDummysCard = pDOC->GetCurrentTrickCard(playEngine.GetPartnerPosition());
	CCard* pPartnersCard = bPlayingInHand? pDummysCard : pDeclarerCard;
	const BOOL bPartnerHigh = (pRoundTopCard == pPartnersCard);
	//
	BOOL bValid = FALSE;
	int nTopOSCardFV = 0; // NCR-17 FaceValue of top Outstanding card

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
				// playing from our own hand (declarer)?  can't do so!
				if (m_nTargetHand == IN_HAND)
				{
					// can't ruff here
					status << "4PLRUF02! Can't ruff dummy's " & STSS(m_nSuit) & " in hand when leading.\n";
					m_nStatusCode = PLAY_POSTPONE;
					return m_nStatusCode;
				}
				else
				{
					// ruffing in dummy -- first check eligibility
					if (dummyHand.GetNumCardsInSuit(m_nSuit) > 0)
					{
						// can't use this now
  						status << "4PLRUF03! Can't ruff " & STSS(m_nSuit)
							    & " in dummy, dummy is not void.\n"; // NCR added
						m_nStatusCode = PLAY_POSTPONE;
						return m_nStatusCode;
					}
					// now lead a low card of the suit from hand
					if (combinedSuit.GetNumDeclarerLosers() > 0)
					{
						pPlayCard = playerHand.GetSuit(m_nSuit).GetBottomCard();
						status << "PLRUF04! Lead a low " & STSS(m_nSuit) & 
								  " (the " & pPlayCard->GetFaceName() & ") from hand to ruff in dummy.\n";
					}
					else
					{
						// oops, no card in the suit to lead!
						status << "4PLRUF08! Oops, we wanted to ruff a " & STSS(m_nSuit) & 
								  " in dummy, but we have no " & STSS(m_nSuit) & 
								  " losers in hand to lead, so we have to abandon the play.\n";
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
					// leading from dummy & ruffing in dummy? no can do
					status << "4PLRUF12! Can't lead from dummy and ruff in dummy at the same time.\n";
					m_nStatusCode = PLAY_POSTPONE;
					return m_nStatusCode;
				}
				else 
				{
					// ruffing in hand -- first check eligibility
					if (playerHand.GetNumCardsInSuit(m_nSuit) > 0)
					{
						// can't use this now
  						status << "4PLRUF13! Can't ruff " & STSS(m_nSuit)
							       & " in hand, hand is not void.\n"; // NCR added
						m_nStatusCode = PLAY_POSTPONE;
						return m_nStatusCode;
					}
					// now lead a low card of the suit from dummy
					if (combinedSuit.GetNumDummyLosers() > 0)
					{
						pPlayCard = dummyHand.GetSuit(m_nSuit).GetBottomCard();
						status << "PLRUF14! Lead a low " & STSS(m_nSuit) & 
								  " (the " & pPlayCard->GetFaceName() & ") from dummy to ruff in hand.\n";
					}
					else
					{
						// oops, no card in the suit to lead!
						status << "4PLRUF18! Oops, we wanted to ruff a " & STSS(m_nSuit) & 
								  " in hand, but we have no " & STS(m_nSuit) & 
								  " losers in dummy to lead, so we have to abandon the play.\n";
						m_nStatusCode = PLAY_NOT_VIABLE;
						return m_nStatusCode;
					}
				}
			}
			// at this point, all went OK
			m_nStatusCode = PLAY_IN_PROGRESS;
			break;

		case 1:
			// playing second -- may be able to ruff here, intended or not
    		// NCR-468 Check if we're to ruff the suit led. Exit if another suit
			if ((nSuitLed == nTrumpSuit) || (m_nSuit != nSuitLed))
			{
				// but not if a trump was led
					m_nStatusCode = PLAY_POSTPONE;
					return m_nStatusCode;
			}

			// an unintended ruff is OK only if the card led is not a trump card, 
			// _and_ we have zero cards in the suit in the appropriate hand
			{  // NCR-17 Get top outstanding card to see if dummy has larger
			CCardList cardList;
			int nCnt = playEngine.GetOutstandingCards(nSuitLed, cardList, true);
			if(nCnt > 0) 
				nTopOSCardFV = cardList.GetTopCard()->GetFaceValue();
			}
			if ( ((m_nTargetHand == IN_HAND) && (playerHand.GetNumCardsInSuit(nSuitLed) == 0)
				 // NCR-17 Test that dummy doesn't have the top card - save ruff if so
				 // Also should check if ???
				 && !dummySuit.IsVoid() && (dummySuit.GetTopCard()->GetFaceValue() > nTopOSCardFV) )
				|| ((m_nTargetHand == IN_DUMMY) && (dummyHand.GetNumCardsInSuit(nSuitLed) == 0)) )
			{
				status << "3PLRUF20! We can ruff in the suit led by the opponent.\n";
			}
			else
			{
//				status << "3PLRUF21! A ruff here is not possible.\n";
				m_nStatusCode = PLAY_POSTPONE;
				return m_nStatusCode;
			}

			// see which hand this is
			if (bPlayingInHand) 
			{
				// playing second in hand -- see where the ruff is
				if (m_nTargetHand == IN_HAND)
				{
					// ruff here
					CSuitHoldings& playerTrumps = playerHand.GetSuit(nTrumpSuit);
					if (playerTrumps.GetNumCards() > 0)
					{
						pPlayCard = playerTrumps.GetBottomCard();
						// NCR-209 Ruff high enough if lots of trumps
						CCombinedSuitHoldings& combinedTrumps = combinedHand.GetSuit(nTrumpSuit);
						if(combinedTrumps.GetTopSequence().GetNumCards() > 5)
							// NCR-209 following might get too high a card ???
							pPlayCard = playerTrumps.GetTopSequence().GetBottomCard(); // NCR-209 a highcard
						status << "PLRUF30! Ruff in hand with the " & pPlayCard->GetName() & ".\n";
					}
					else
					{
						status << "4PLRUF32! Error - no trumps left in hand to use in a ruff.\n";
						m_nStatusCode = PLAY_ERROR;
						return m_nStatusCode;
					}
				}
				else
				{
					// we'll be ruffing in dummy later, so discard
					pPlayCard = playEngine.GetDiscard();
					status << "PLRUF34! We'll be ruffing in dummy, so discard the " &
							  pPlayCard->GetName() & " from hand.\n";
				}
			}
			else
			{
				// playing second in dummy -- see where the finese card is located
				if (m_nTargetHand == IN_DUMMY)
				{
					// ruff here
					CSuitHoldings& dummyTrumps = dummyHand.GetSuit(nTrumpSuit);
					if (dummyTrumps.GetNumCards() > 0)
					{
						pPlayCard = dummyTrumps.GetBottomCard();
						status << "PLRUF40! Ruff in dummy with the " & pPlayCard->GetName() & ".\n";
					}
					else
					{
						status << "4PLRUF42! Error - no trumps left in dummy to use in a ruff.\n";
						m_nStatusCode = PLAY_ERROR;
						return m_nStatusCode;
					}
				}
				else
				{
					// finessing in hand, so discard from dummy 
					pPlayCard = playEngine.GetDiscard();
					status << "PLRUF44! We'll be ruffing in hand, so discard the " &
							  pPlayCard->GetName() & " from dummy.\n";
				}
			}
			// done here
			m_nStatusCode = PLAY_IN_PROGRESS;
			break;


		case 2:  case 3:
			// playing third -- ruff if possible
//			if (m_nStatusCode != PLAY_IN_PROGRESS)
//				return PLAY_INACTIVE;
			// unintended ruff is OK only if the card led is not a trump card, 
			// _and_ we have zero cards in the suit
			if (bPlayingInHand)
			{
				if ( (m_nStatusCode == PLAY_IN_PROGRESS) ||
					 ((nSuitLed != nTrumpSuit) && (playerHand.GetNumCardsInSuit(nSuitLed) == 0)) )
					bValid = TRUE;
			}
			else
			{
				if ( (m_nStatusCode == PLAY_IN_PROGRESS) ||
					 ((nSuitLed != nTrumpSuit) && (dummyHand.GetNumCardsInSuit(nSuitLed) == 0)) )
					bValid = TRUE;
			}
			//
    		// NCR-468 Check if we're to ruff the suit led. Exit if another suit
			if (bValid &&  (m_nSuit == nSuitLed))
			{
				// bonanza
//				status << "3PLRUF50! We can ruff in the suit led by the opponent.\n";
//				status << "3PLRUF50! We can ruff here.\n";
			}
			else
			{
//				status << "3PLRUF51! A ruff here is not possible.\n";
				m_nStatusCode = PLAY_POSTPONE;
				return m_nStatusCode;
			}

			// NCR-472 Get RHO's card played
//			CCard* pRHOCard = pDOC->GetCurrentTrickCard(playEngine.GetRHOpponent()->GetPosition()); // NCR-472
			// at this point, we MUST be in the proper hand to ruff
			if (bPlayingInHand) 
			{
				// playing third/fourth in hand -- see where the ruff is
				if (m_nTargetHand == IN_HAND)
				{
					// ruff here
					CSuitHoldings& playerTrumps = playerHand.GetSuit(nTrumpSuit);
					if (playerTrumps.GetNumCards() > 0)
					{
						// see if RHO trumped
						if (bTrumped && (pPartnersCard != pRoundTopCard))
						{
							// see if we can beat it
							pPlayCard = playerTrumps.GetLowestCardAbove(pRoundTopCard);
							if (pPlayCard)
							{
								status << "PLRUF56! Overruff RHO's trump " & pRoundTopCard->GetFaceName() & 
										  " with the " & pPlayCard->GetFaceName() & ".\n";
							}
							else
							{
								status << "4PLRUF58! RHO ruffed, and we can't overruff.\n";
								m_nStatusCode = PLAY_POSTPONE;
								return m_nStatusCode;
							}
						}
						// NCR-171 don't ruff if pard's card is high and (playing last or no > cards out)
						else if ((pPartnersCard == pRoundTopCard) 
							     && ((nOrdinal == 3) || (combinedSuit.GetNumMissingAbove(pPartnersCard) == 0)))
							      
						{
							status << "4PLRUF59! Partner'" & pPartnersCard->GetName() & " is high, so don't ruff it.\n";
							m_nStatusCode = PLAY_POSTPONE;
							return m_nStatusCode;
						}
						else
						{
							// go ahead and ruff
							pPlayCard = playerTrumps.GetBottomCard();
							status << "PLRUF60! Ruff in hand with the " & pPlayCard->GetName() & ".\n";
						}
					}
					else
					{
						status << "4PLRUF62! Error - no trumps left in hand to use in a ruff.\n";
						m_nStatusCode = PLAY_ERROR;
						return m_nStatusCode;
					}
				}
				else
				{
					// if we're playing 4th, we can discard (ruffed opposite)
					if (nOrdinal == 3)
					{
						if (bPartnerHigh)
						{
							pPlayCard = playEngine.GetDiscard();
							status << "PLRUF63! Complete the ruff by discarding the " & pPlayCard->GetName() & " from hand.\n";
						}
						else
						{
							// oops, opponents overruffed
							status << "3PLRUF63a! Oops, the opponents overruffed, so skip the play.\n";
							m_nStatusCode = PLAY_FAILED;
							return m_nStatusCode;
						}
					}
					else
					{
						// can't use _this_ ruff here
//						status << "4PLRUF64! Oops, we ended up in hand opposite a ruff in dummy.\n";
//						m_nStatusCode = PLAY_ERROR;
						m_nStatusCode = PLAY_POSTPONE;
						return m_nStatusCode;
					}
				}
			}
			else
			{
				// NCR-472 Get card played by LHO to test if he's shown out
				CCard* pLHOCard = pDOC->GetCurrentTrickCard(playEngine.GetLHOpponent()->GetPosition()); // NCR-472
				bool bLHOShownOut = pLHOCard->GetSuit() != nSuitLed; // NCR-472
				// playing second in dummy -- see if we're ruffing here
				if (m_nTargetHand == IN_DUMMY)
				{
					// ruff here
					CSuitHoldings& dummyTrumps = dummyHand.GetSuit(nTrumpSuit);
					if (dummyTrumps.GetNumCards() > 0)
					{
						// see if RHO trumped
						if (bTrumped && (pPartnersCard != pRoundTopCard))
						{
							// see if we can beat it
							pPlayCard = dummyTrumps.GetLowestCardAbove(pRoundTopCard);
							if (pPlayCard)
							{
								status << "PLRUF70! Overruff RHO's trump " & pRoundTopCard->GetFaceName() & 
										  " with the " & pPlayCard->GetFaceName() & ".\n";
							}
							else
							{
								status << "3PLRUF72! RHO ruffed, and we can't overruff.\n";
								m_nStatusCode = PLAY_POSTPONE;
								return m_nStatusCode;
							}
						}
						// NCR ruff a low card (less than Jack) that will probably be too low
						// unless we're in the last position
						else if ((pPartnersCard == pRoundTopCard) 
							     && ((nOrdinal == 3) || (!bLHOShownOut  // NCR-472 ruff only if LHO shown out and higher card out 
								                         && (combinedSuit.GetMissingCardVal(0) < pPartnersCard->GetFaceValue()) )))
						{
							status << "4PLRUF73! Partner's " & pPartnersCard->GetName() & " is high, so don't ruff it.\n";
							m_nStatusCode = PLAY_POSTPONE;
							return m_nStatusCode;
						}
						else
						{
							// go ahead and ruff
							pPlayCard = dummyTrumps.GetBottomCard();
							status << "PLRUF74! Ruff in dummy with the " & pPlayCard->GetName() & ".\n";
						}
					}
					else
					{
						status << "4PLRUF76! Error - no trumps left in dummy to use in a ruff.\n";
						m_nStatusCode = PLAY_ERROR;
						return m_nStatusCode;
					}
				}
				else
				{
					// if we're playing 4th, we can discard (ruffed opposite)
					if (nOrdinal == 3)
					{
						if (bPartnerHigh)
						{
							pPlayCard = playEngine.GetDiscard();
							status << "PLRUF77! Complete the ruff by discarding the " & pPlayCard->GetName() & " from hand.\n";
						}
						else
						{
							// oops, opponents overruffed
							status << "3PLRUF77a! Oops, the opponents overruffed, so skip the play.\n";
							m_nStatusCode = PLAY_FAILED;
							return m_nStatusCode;
						}
					}
					else
					{
						// can't ruff here
//						status << "4PLRUF78! Oops, we ended up in dummy opposite a ruff in hand.\n";
//						m_nStatusCode = PLAY_ERROR;
						m_nStatusCode = PLAY_POSTPONE;
						return m_nStatusCode;
					}
				}
			} // end playing in dummy
			// else all went OK
			m_nStatusCode = PLAY_COMPLETE;
			break;
	}

	// done
	ASSERT(pPlayCard->IsValid());
	return m_nStatusCode;
}

