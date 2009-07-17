//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//===========================================================
//
// CPlayEngine
//
// - encapsulates the play engine and its status variables
//
//=========================================================


#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "HandHoldings.h"
#include "BidEngine.h"
#include "Player.h"
#include "PlayerStatusDialog.h"
#include "Card.h"
#include "deck.h"
/*
#include "Play.h"
#include "Cash.h"
#include "Type1Finesse.h"
#include "Type2Finesse.h"
#include "Type3Finesse.h"
#include "Ruff.h"
#include "Sluff.h"
#include "Drop.h"
#include "Discard.h"
*/
#include "PlayEngine.h"
//#include "PlayList.h"





//
//==========================================================================
//==========================================================================
//
// Initialization routines
//
//

CPlayEngine::CPlayEngine()
{
}

CPlayEngine::~CPlayEngine()
{
	Clear();
}



// 
//-----------------------------------------------------------------------
//
// Initialize()
//
// one-time initialization at program start
//
void CPlayEngine::Initialize(CPlayer* pPlayer, CPlayer* pPartner, CPlayer* pLHOpponent, CPlayer* pRHOpponent, CHandHoldings* pHoldings, CCardLocation* pCardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine* pBidder, CPlayerStatusDialog* pStatusDlg)
{ 
	//
	m_pPlayer = pPlayer; 
	m_pPartner = pPartner; 
	m_pLHOpponent = pLHOpponent;
	m_pRHOpponent = pRHOpponent;
	m_nPosition = pPlayer->GetPosition();
	m_nPartnerPosition = m_pPartner->GetPosition();
	m_pPartnersHand = &m_pPartner->GetHand();
	//
	ASSERT(ISPOSITION(m_nPartnerPosition));
	strcpy(szLHO, PositionToString(m_pLHOpponent->GetPosition()));
	strcpy(szRHO, PositionToString(m_pRHOpponent->GetPosition()));
	//
	m_pHand = pHoldings; 
	m_pCardLocation = pCardLocation; 
	m_ppGuessedHands = ppGuessedHands;
	m_pBidder = pBidder;
	m_pStatusDlg = pStatusDlg;
}


//
//-----------------------------------------------------------------------
//
// Clear()
//
void CPlayEngine::Clear()
{ 
	// clear analysis variables
	for(int i=0;i<4;i++)
		m_ppGuessedHands[i]->Clear(TRUE);
	//
	m_nPrioritySuit = NONE;
	m_nPartnersPrioritySuit = NONE;
	for(int j=0;j<4;j++)
		m_nSuitPriorityList[j] = NONE;  // NCR changed [4] to [j]

}

	

//
//-----------------------------------------------------------------------
//
// InitNewHand()
//
// called when the hand is dealt (or play is restarted)
//
void CPlayEngine::InitNewHand()
{ 
	// set general status info
	CEasyBDoc* pDoc = pDOC;
	m_nContract = pDoc->GetContract();
	m_nContractModifier = pDoc->GetContractModifier();
	m_nTrumpSuit = pDoc->GetContractSuit();
	//
	m_bVulnerable = pDOC->IsTeamVulnerable(m_pPlayer->GetTeam());
	m_bOpponentsVulnerable = pDoc->IsTeamVulnerable(m_pLHOpponent->GetTeam());
	if (m_pRHOpponent->IsDeclarer())
		m_bLHDefender = TRUE;
	else
		m_bLHDefender = FALSE;
	if (m_pLHOpponent->IsDeclarer())
		m_bRHDefender = TRUE;
	else
		m_bRHDefender = FALSE;

	// clear analysis variables
	Clear();
}



	

//
//-----------------------------------------------------------------------
//
// InitSwappedHand()
//
// called when the hand is swapped in midgame
//
void CPlayEngine::InitSwappedHand()
{ 
	// set general status info
	CEasyBDoc* pDoc = pDOC;
	m_nContract = pDoc->GetContract();
	m_nContractModifier = pDoc->GetContractModifier();
	m_nTrumpSuit = pDoc->GetContractSuit();
	//
	m_bVulnerable = pDOC->IsTeamVulnerable(m_pPlayer->GetTeam());
	m_bOpponentsVulnerable = pDoc->IsTeamVulnerable(m_pLHOpponent->GetTeam());
	if (m_pRHOpponent->IsDeclarer())
		m_bLHDefender = TRUE;
	else
		m_bLHDefender = FALSE;
	if (m_pLHOpponent->IsDeclarer())
		m_bRHDefender = TRUE;
	else
		m_bRHDefender = FALSE;

	// clear analysis variables
	Clear();
}



//
// RestartPlay()
//
void CPlayEngine::RestartPlay()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	status << "3PLAY! Play restarted.\n====================\n";
	Clear();
	InitNewHand();
}




//
// GetDummyHand()
//
CHandHoldings& CPlayEngine::GetDummyHand()
{
	ASSERT(pDOC->IsDummyExposed());
	CHandHoldings& dummyHand = pDOC->GetDummyPlayer()->GetHand();
	return dummyHand;
}



//
// GetDummySuit()
//
CSuitHoldings& CPlayEngine::GetDummySuit(int nSuit)
{
	ASSERT(pDOC->IsDummyExposed());
	ASSERT(ISSUIT(nSuit));
	CHandHoldings& dummyHand = pDOC->GetDummyPlayer()->GetHand();
	return dummyHand.GetSuit(nSuit);
}





//
//==========================================================================
//==========================================================================
//
// Observation routines
//
//



//
// RecordCardPlay()
//
// record that a card has been played
//
void CPlayEngine::RecordCardPlay(int nPos, CCard* pCard) 
{ 
	CPlayerStatusDialog& status = *m_pStatusDlg;
/*
	if (pDOC->GetNumCardsPlayedInRound() == 1)
	{
		status << "3==============================================\n";
		status << "3Beginning round " & pDOC->GetPlayRound() + 1 & ".\n";
	}
*/
	if (nPos == m_nPosition)
		status << "4PLAY! We" & 
				  ((pDOC->GetNumCardsPlayedInRound() == 1)? " lead" : " play") &
				  " the " & pCard->GetName() & ".\n";
	else
		status << "4PLAY! " & PositionToString(nPos) & 
				  ((pDOC->GetNumCardsPlayedInRound() == 1)? " leads" : " plays") &
				  " the " & pCard->GetName() & ".\n";

	// record the card played by the specified player
	CGuessedHandHoldings* pHand = m_ppGuessedHands[nPos];
	CGuessedCard* pGuessedCard = new CGuessedCard(pCard, // card
												  FALSE, // not outstanding
												  nPos,	 // location
												  1.0);	 // known with certainty
	*pHand << pGuessedCard;
	pHand->Sort();

	// then re-evaluate holdings in light of the new information
	AdjustHoldingsCount(pCard);
	AdjustCardCountFromPlay(nPos, pCard);
}


//
// RecordCardUndo()
//
// note that a card has been undone
//
void CPlayEngine::RecordCardUndo(int nPos, CCard* pCard)
{
	// remove the card from the observed list (i.e., "forget" that it was played)
	VERIFY(ISPLAYER(nPos) && (pCard != NULL));
	CPlayerStatusDialog& status = *m_pStatusDlg;
	status << "4>> Card taken back.\n";

	// remove the card from the guessed list
	CGuessedHandHoldings* pHand = m_ppGuessedHands[nPos];
	CGuessedCard* pGuessedCard = pHand->RemoveByValue(pCard->GetDeckValue());
	delete pGuessedCard;

	// re-adjust hand values
	AdjustHoldingsCount(pCard);
	AdjustCardCountFromUndo(nPos, pCard);
}


//
// RecordTrickUndo()
//
// note that the trick has been undone
// (currently not used)
//
void CPlayEngine::RecordTrickUndo()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	status << "4- - - - - - - -\n";
	status << "4>> Trick taken back.\n";
}



//
// RecordRoundComplete()
//
// nPos = winner
// pCard = winning card
//
void CPlayEngine::RecordRoundComplete(int nPos, CCard* pCard) 
{ 
	CPlayerStatusDialog& status = *m_pStatusDlg;
	status << "4PLAY! " & PositionToString(nPos) & 
			  " wins the round.\n--------\n";
	//
//	AdjustHoldingsCount(pCard);
//	RecordCardsPlayed();
}



//
// RecordHandComplete()
//
void CPlayEngine::RecordHandComplete(int nResult)
{ 
	CPlayerStatusDialog& status = *m_pStatusDlg;
	int nTeam = pDOC->GetDeclaringTeam();
	if (nResult > 0)
	{
		status << "3RESULT! " & TeamToString(nTeam) & " made " & 
				   nResult & " overtrick" & ((nResult > 1)? "s" : "") & ".\n";
	}
	else if (nResult == 0) // nCR changed < to ==
	{
		status << "3RESULT! " & TeamToString(nTeam) & " made the contract.\n";
	}
	else
	{
		status << "3RESULT! " & TeamToString(nTeam) & " were set " & 
				  -nResult & " trick" & ((nResult < -1)? "s" : "") & ".\n";
	}
}



//
// RecordSpecialEvent()
//
void CPlayEngine::RecordSpecialEvent(int nCode, int nParam1, int nParam2, int nParam3)
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	int nPlayer = nParam1;
	int nTeam = GetPlayerTeam(nPlayer);
	int numTricks = nParam2;
	//
	switch(nCode)
	{
		case CEasyBDoc::EVENT_CLAIMED:
//			status << "3PLAYCL! " & TeamToString(nTeam) & " has claimed the remaining " &
			status << "3PLAYCL! " & PositionToString(nPlayer) & " has claimed the remaining " &
				      numTricks & " tricks.\n";
			break;

		case CEasyBDoc::EVENT_CONCEDED:
//			status << "3PLAYCN! " & TeamToString(nTeam) & " has conceded the remaining " &
			status << "3PLAYCN! " & PositionToString(nPlayer) & " has conceded the remaining " &
				      numTricks & " tricks.\n";
			break;
	}
}



//
// RecordCardsPlayed()
//
// - records card played from a full round
// (currently not used)
//
void CPlayEngine::RecordCardsPlayed()
{
	// record the cards that have been played
	for(int i=0;i<4;i++)
	{
		// record the card played by this player
		CCard* pPlayedCard = pDOC->GetCurrentTrickCard(i);
		CGuessedHandHoldings* pHand = m_ppGuessedHands[i];
		CGuessedCard* pCard = new CGuessedCard(pPlayedCard,	// card
											   FALSE,		// not outstanding
											   i,			// location
											   1.0);		// known with certainty
		*pHand << pCard;
		pHand->Sort();
	}
}



//
// AdjustCardCountFromPlay()
//
// adjust card count and analysis after a card is played
//
void CPlayEngine::AdjustCardCountFromPlay(int nPos, CCard* pCard)
{
	// default code
//	if (nPos != m_pPlayer->GetPosition())
//	{
		// note the card that was played
		CGuessedHandHoldings* pPlayerHoldings = m_ppGuessedHands[nPos];
		CGuessedCard* pGuessedCard = new CGuessedCard(pCard,	// card
													  FALSE,	// no longer outstanding
													  nPos,		// location
													  1.0);		// known with certainty
		*pPlayerHoldings << pGuessedCard;

		// see if the player showed out
		CPlayerStatusDialog& status = *m_pStatusDlg;
		CCard* pCardLed = pDOC->GetCurrentTrickCardByOrder(0);
		if (pCard)
		{
			int nSuitLed = pCardLed->GetSuit();
			ASSERT(nSuitLed != NONE);
			CGuessedSuitHoldings& suit = pPlayerHoldings->GetSuit(nSuitLed);
			if ((pCard->GetSuit() != nSuitLed) && (!suit.IsSuitShownOut()))
			{
				status << "4RCP10! " & PositionToString(nPos) & 
						  " shows out of " & STS(nSuitLed) & ".\n";
				suit.MarkSuitShownOut();
				if (pPlayerHoldings->GetNumSuitsFullyIdentified() > 1)
				{
					// multiple suits identified
					status << "4RCP1A! " & PositionToString(nPos) & 
							  " is now known to have started with ";
					int numTotalIdentifiedSuits = pPlayerHoldings->GetNumSuitsFullyIdentified();
					int numIdentifiedSuits = 0;
					int numTotalIdentifiedCards = 0;
					int numTotalOriginalCards = 0;
					int nIdentifiedSuits[4];
					for(int i=0;i<4;i++)
					{
						if (pPlayerHoldings->GetSuit(i).AreAllCardsIdentified())
						{
							CGuessedSuitHoldings& currSuit = pPlayerHoldings->GetSuit(i);
							status < ((numIdentifiedSuits > 0)? " and " : " ") &
									  currSuit.GetNumOriginalCards() & " " & 
									  ((suit.GetNumOriginalCards() > 1)? STS(i) : STSS(i));
							nIdentifiedSuits[numIdentifiedSuits] = i;
							numIdentifiedSuits++;
							numTotalIdentifiedCards += currSuit.GetNumDefiniteCards();
							numTotalOriginalCards += currSuit.GetNumOriginalCards();
						}
					}
					status < ".\n";
					// if the number of cards is known in 3 suits, the orignal 
					// and current length of the 4th suit is also known
					if (numTotalIdentifiedSuits == 3)
					{
						// first identify the fourth suit
						int nFourthSuit = NONE;
						for(int i=0;i<4;i++)
						{
							// test each suit to see if it's in the list of know suits
							int j; // NCR-FFS added here, removed below
							for(/*int*/ j=0;j<3;j++)
							{
								if (nIdentifiedSuits[j] == i)
									break;
							}
							if (j == 3)
								nFourthSuit = i;
						}
						//
						VERIFY(nFourthSuit != NONE);
						CGuessedSuitHoldings& fourthSuit = pPlayerHoldings->GetSuit(nFourthSuit);
						int numOriginalCards = 13 - numTotalOriginalCards;
						VERIFY(numOriginalCards >= 0);
						fourthSuit.SetNumOriginalCards(numOriginalCards);
						int numRemainingCards = numOriginalCards - fourthSuit.GetNumCardsPlayed();
						VERIFY(numRemainingCards >= 0);
						// mark the # of remaining cards
						// revisit this later
	//					fourthSuit.SetNumLikelyCards(numRemainingCards);
						fourthSuit.SetNumRemainingCards(numRemainingCards);
						//
						status << "4RCP1B! Therefore, " & PositionToString(nPos) &
								  " started with " & numOriginalCards & " " & STS(nFourthSuit) &
								  " and has " & numRemainingCards & " left.\n";
					}
				}
				else
				{
					status < "RCP5A! " & PositionToString(nPos) & 
							  " is now known to have started with " &
							  suit.GetNumOriginalCards() & " " & 
							  ((suit.GetNumOriginalCards() == 1)? STSS(nSuitLed) : STS(nSuitLed)) & ".\n";
				}
			}

			// also, if this is the dummy, and the dummy plays his last card 
			// in the suit, he has effectively shown out
			CPlayer* pDummy = pDOC->GetDummyPlayer();
			if ((nPos == pDummy->GetPosition()) && (pDummy->AreCardsExposed()))
			{
				if ((pCard->GetSuit() == nSuitLed) && 
					(pDummy->GetHand().GetNumCardsInSuit(nSuitLed) == 0) &&
					(!pPlayerHoldings->IsSuitShownOut(nSuitLed)))
				{
					status << "3RCP20! Dummy is now out of " & STS(nSuitLed) & ".\n";
					pPlayerHoldings->MarkSuitShownOut(nSuitLed);
				}
			}
		}
//	}

	// special code -- if dummy has just been laid down, mark a suit as shown
	// out if dummy is void in the suit
	CPlayer* pDummy = pDOC->GetDummyPlayer();
	int nRound = pDOC->GetPlayRound();
	if ((nRound == 0) && (nPos == pDummy->GetPosition()) && (pDummy->AreCardsExposed()))
	{
		CHandHoldings& dummy = pDummy->GetHand();
		for(int i=0;i<4;i++)
		{
			if (dummy.GetNumCardsInSuit(i) == 0) 
			{
				status << "3RCP25! Dummy is shown to be void in " & STS(i) & ".\n";
				pPlayerHoldings->MarkSuitShownOut(i);
			}
		}
	}
}



//
// AdjustCardCountFromUndo()
//
// adjust card count and analysis after a card play is undone
//
void CPlayEngine::AdjustCardCountFromUndo(int nPos, CCard* pCard)
{
	// default code
//	if (nPos != m_pPlayer->GetPosition())
	{
		// find the card in the guessed cards list and delete it
		m_ppGuessedHands[nPos]->Delete(pCard);
		
		// later put in code to "forget" analysis based on the last card
	}
}



//
// AdjustHoldingsCount()
//
// called to adjust analysis of holdings after a round of play
//
void CPlayEngine::AdjustHoldingsCount(CCard* pCard)
{
	// adjust our own holdings
	m_pHand->ReevaluateHoldings(pCard);
}




//
//================================================================
//
// Play routines
//
//================================================================
//

  


//
// PlayCard()
//
// default implementation
//
CCard* CPlayEngine::PlayCard()
{
	int nOrdinal = pDOC->GetNumCardsPlayedInRound();
	CCard* pCard = NULL;
	switch(nOrdinal)
	{
		case 0:
			pCard =  PlayFirst();
			break;
		case 1:
			pCard =  PlaySecond();
			break;
		case 2:
			pCard =  PlayThird();
			break;
		case 3:
			pCard =  PlayFourth();
			break;
		default:
			pCard = NULL;
	}
	// do some sanity checks
	ASSERT(pCard->IsValid());
	if (!m_pPlayer->IsDeclarer())
	{
		// playing as defender (or dummy, potentially)
		ASSERT(m_pHand->HasCard(pCard));
	}
	else
	{
		// declarer  -- may be playing for dummy or self
		if (pDOC->GetCurrentPlayer() == m_pPartner)
			ASSERT(m_pPartnersHand->HasCard(pCard));
		else
			ASSERT(m_pHand->HasCard(pCard));
	}
	//
	return pCard;
}





//
// PlayFirst()
//
// default implementation
//
CCard* CPlayEngine::PlayFirst()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	status << "5PLAY1! Leading card, using default player logic.\n";

	// lead a card
	CCard* pLeadCard = GetLeadCard();
	ASSERT(m_pHand->HasCard(pLeadCard));
	return pLeadCard;
}




//
// PlaySecond()
//
// default implementation, generally should be overridden in derived classes
//
CCard* CPlayEngine::PlaySecond()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	status << "5PLAY2! Playing second, using default player logic.\n";

	// get play info
	int nDummyPos = pDOC->GetDummyPosition();
	CCard* pCardLed = pDOC->GetCurrentTrickCardLed();
	int nSuitLed = pCardLed->GetSuit();
	int nFaceValue = pCardLed->GetFaceValue();
	CCard* pCurrTopCard = pDOC->GetCurrentTrickHighCard();
	int nTrumpSuit = pDOC->GetTrumpSuit();
	CSuitHoldings& suit = m_pHand->GetSuit(nSuitLed);
	// card to play
	CCard* pCard = NULL;

	// second hand low
	int numCardsInSuit = suit.GetNumCards();
	if (numCardsInSuit > 0)
	{
		// default behavior -- just play the low card
		pCard = m_pHand->GetSuit(nSuitLed).GetBottomCard();
		if (numCardsInSuit > 1)
		{
			if (*pCard < *pCurrTopCard)
				status << "PLY2C1! Play second hand low with the " & pCard->GetFaceName() & ".\n";
			else
				status << "PLY2C2! As second hand, we play the lowest card we have in the suit, the " & pCard->GetFaceName() & ".\n";
		}
		else
		{
			status << "PLY2C4! Play our only " & STSS(nSuitLed) & ", the " & pCard->GetFaceName() & ".\n";
		}
	}
	else
	{
		// no cards in the suit led
		// trump here if possible
		if (m_pHand->GetNumTrumps() > 0)
		{
			//
			CSuitHoldings& trumpSuit = m_pHand->GetSuit(nTrumpSuit);

			// see if partner would win the suit otherwise
			CGuessedSuitHoldings& partnerSuit = m_pPlayer->GetGuessedHand(m_nPartnerPosition)->GetSuit(nSuitLed);
			CCardList outstandingCards;
			GetOutstandingCards(nSuitLed, outstandingCards);
			if (partnerSuit.AreAllCardsIdentified() && partnerSuit.HasCard(outstandingCards[0]->GetFaceValue()))
			{
				// partner may win the trick, so discard
				status << "PLY2K1! We could trump here, but we know partner holds the " & outstandingCards[0]->GetName() &
						  " and can win the trick, so discard the " & pCard->GetName() & ".\n";
			}
			else
			{
				// trump here if possible
				// but first see if we're playing ahead of dummy (dummy is to our left), 
				// and dummy is also void in the suit
				CGuessedHandHoldings* pDummyHand = m_pPlayer->GetGuessedHand(nDummyPos);
				if ((m_pLHOpponent == pDOC->GetDummyPlayer()) &&
					(pDummyHand->GetSuit(nSuitLed).GetNumRemainingCards() == 0) &&
					(pDummyHand->GetSuit(nTrumpSuit).GetNumRemainingCards() > 0))
				{
					// dummy is also void, so trump if we have a trump higher than dummy's
					CGuessedCard* pDummyTopTrump = pDummyHand->GetSuit(nTrumpSuit).GetAt(0);
					if (trumpSuit.GetNumCardsAbove(pDummyTopTrump->GetFaceValue()) > 0)
					{
						// go ahead and ruff
						pCard = trumpSuit.GetLowestCardAbove(pDummyTopTrump->GetFaceValue());
						status << "PLY2M1! Trump here, making sure to thwart dummy's " & pDummyTopTrump->GetFaceName() &
								  " of trumps by playing the " & pCard->GetFaceName() & ".\n";
					}
					else
					{
						// dummy would overtrump
						pCard = GetDiscard();
						status << "PLY2M2! We'd like to trump here, but Dummy may overruff, so discard the " & pCard->GetName() & ".\n";
					}
				}
				else
				{
					// safe to trump -- play the lowest trump
					pCard = trumpSuit.GetBottomCard();
					status << "PLY2P1! Trump with the " & pCard->GetFaceName() & ".\n";
				}
			}
		}
		else
		{
			// discard
			pCard = GetDiscard();
			status << "PLY2Y! We have no " & SuitToString(nSuitLed) & ", so discard the " & pCard->GetName() & ".\n";
		}
	}
	//
	ASSERT(pCard->IsValid());
	return pCard;
}



//
// PlayThird()
//
// default implementation
//
CCard* CPlayEngine::PlayThird()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	status << "5PLAY3! Playing third, using default player logic.\n";

	// use common code
	CCard* pCard = PlayBestCard(3);
	//
	ASSERT(pCard->IsValid());
	return pCard;
}



//
// PlayFourth()
//
// default implementation
//
CCard* CPlayEngine::PlayFourth()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	status << "5PLAY4! Playing fourth, using default player logic.\n";

	// use common code
	CCard* pCard = PlayBestCard(4);
	//
	ASSERT(pCard->IsValid());
	return pCard;
}




//
// PlayBestCard()
//
// called on the third and fourth hand plays to try to win the trick
//
CCard* CPlayEngine::PlayBestCard(int nPosition)
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
//	status << "2PLAY3! Playing best card.\n";

	// get play info
	CCard* pCurrentCard = pDOC->GetCurrentTrickCardLed();
	int nSuitLed = pCurrentCard->GetSuit();
	int nTopPos;
	CCard* pCurrTopCard = pDOC->GetCurrentTrickHighCard(&nTopPos);
	CString strTopCardPos = PositionToString(nTopPos);
	BOOL bPartnerHigh = FALSE;
	int nCurrentRound = pDOC->GetPlayRound();
	int nCurrentSeat = pDOC->GetNumCardsPlayedInRound() + 1;
	CCard* pPartnersCard = pDOC->GetCurrentTrickCard(m_pPartner->GetPosition());
	if (pPartnersCard == pCurrTopCard)
		bPartnerHigh = TRUE;
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
			status << "PLAYB10! We let partner's " & pCurrTopCard->GetName() & " trump ride and discard the " &
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
				status << "PLAYB20! We can overtrump " & strTopCardPos & "'s " & pCurrTopCard->GetName() &
						  " with the " & pCard->GetFaceName() & ".\n";
			}
			else
			{
				// no chance to win, so discard
				pCard = GetDiscard();
				if ((numCardsInSuitLed == 0) && (trumpSuit.GetNumCards() > 0))
					status << "PLAYB22! We can't overtrump " & strTopCardPos & "'s " & 
							  pCurrTopCard->GetFaceName() & ", so discard the " & pCard->GetName() & ".\n";
				else
					status << "PLAYB23! We can't beat the opponent's " & pCurrTopCard->GetFaceName() & 
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
						(suit.GetNumHonors() == 1))
					{
						// first round in an NT contract, with one honor 
						// in the suit -- unblock
						pCard = suit.GetTopCard();
						if (suit.GetNumCards() > 1)
							status << "PLAYB30! Drop the " & pCard->GetFaceName() & 
									  " here to unblock the suit for partner.\n";
					}
					else
					{
						// else this is not an unblocking situation
						if (nCurrentSeat == 4)
						{
							// playing in 4th seat, high card is partner, so discard
							pCard = GetDiscard();
							status << "PLAYB34! Partner's " & pCurrTopCard->GetFaceName() & 
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
								status << "PLAYB36! Partner's " & pCurrTopCard->GetFaceName() & 
										  " is higher than any outstanding card, so discard the " & 
										  pCard->GetName() & ".\n";
							}
							else
							{
								// partner's card is not necessarily highest, so top it
								pCard = suit.GetTopSequence().GetBottomCard();
								status << "PLAYB37! Partner's " & pCurrTopCard->GetFaceName() & 
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
								status << "PLAYB38A! Playing third ahead of dummy, need to beat dummy's " & 
										  CardValToString(nDummyTopCard) & ".\n";
							}
							else
							{
								// the top card is declarer's
								status << "PLAYB38B! Play high to win with the " & pCard->GetFaceName() & ".\n";
							}
						}
						else
						{
							// else we can't beat dummy's top card, but play
							// high anyway to force out his winner
							pCard = suit.GetLowestCardAbove(pCurrTopCard);
							status << "PLAYB38C! We top declarer's " & pCurrTopCard->GetFaceName() &
								      " to force a winner from dummy.\n";
						}
					}
					else if (nCurrentSeat == 3) 
					{
						// else we're playing 3rd, so play the lowest card from the top sequence
						pCard = suit.GetTopSequence().GetBottomCard();
						status << "PLAYB40! Play high to win with the " & pCard->GetFaceName() & ".\n";
					}
					else
					{
						// else we're playing last (4th)
						// play the cheapest card that will beat the top card
						pCard = suit.GetLowestCardAbove(pCurrTopCard);
						status << "PLAYB41! Play the " & pCard->GetFaceName() & " to win the trick.\n";
					}
				}
			}
			else
			{
				// NCR-154 check if single card left
				if(numCardsInSuitLed == 1) {
					pCard = suit.GetTopCard(); // get only card
				} 
				// we don't have a card to top the current high card
				else if (bPartnerHigh)
				{
					// but partner's card is high, so we're OK
					pCard = GetDiscard();
					status << "PLAYB47! Partner's " & pCurrTopCard->GetFaceName() & 
							  " can win the trick, so discard the " & pCard->GetName() & ".\n";
				}
				else
				{
					// else we're screwed
					pCard = GetDiscard();
					status << "PLAYB48! We can't beat " & strTopCardPos & "'s " & pCurrTopCard->GetFaceName() & 
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
				status << "PLAYB52! Although we could trump this hand, partner's " & pCurrTopCard->GetName() & 
						  " is high, so discard the " & pCard->GetName() & ".\n";
			}
			else
			{
				// opponents have the high card (non-trump) -- so slam 'em
				pCard = m_pHand->GetSuit(nTrumpSuit).GetBottomCard();
				status << "PLAYB55! With no cards in " & SuitToString(nSuitLed) & 
						  ", trump with the " & pCard->GetName() & ".\n";
			}
		}
		else
		{
			// here we have zero cards in the suit and in trumps, so we're hosed
 			pCard = GetDiscard();
			status << "PLAYB56! With no cards in the suit led and no trumps, we discard the " & pCard->GetName() & ".\n";
		}
	}
	//
	ASSERT(pCard->IsValid());
	ASSERT(m_pHand->HasCard(pCard));
	//
	return pCard;
}




//
// GetLeadCard()
//
CCard* CPlayEngine::GetLeadCard()
{
	// default implementation 
	CPlayerStatusDialog& status = *m_pStatusDlg;
	CCard* pLeadCard = NULL;
	int nTrumpSuit = pDOC->GetTrumpSuit();
	
	// look to see if we have any winners
	if (m_pHand->GetNumWinners() > 0)
	{
		// return the first winner found
		// but avoid the trump suit unless there are no other winners
		int nSuit = NONE;
		if (ISSUIT(nTrumpSuit))
			nSuit = GetNextSuit(nTrumpSuit);
		else
			nSuit= CLUBS;	// else start with the club suit

		// NCR  Searches for suit = nSuit (not i suit); i is counter
		for(int i=0;i<4;i++, nSuit = GetNextSuit(nSuit)) // NCR added nSuit = etc here
		{
			// NCR check if dummy is void and has trump
			CHandHoldings& dummyHand = pDOC->GetDummyPlayer()->GetHand();
			if (!m_pPlayer->IsDeclarer() &&  //NCR declarer doesn't worry about dummy
				(nSuit != nTrumpSuit) && (dummyHand.IsSuitVoid(nSuit)) && (dummyHand.GetNumTrumps() > 0)) 
			{
				continue;  // skip this suit
			}

			// NCR check if declarer could be void - and has trumps
			CGuessedHandHoldings* pDclrHand = m_ppGuessedHands[pDOC->GetDeclarerPosition()];
			if(ISSUIT(nTrumpSuit) && pDclrHand->IsSuitShownOut(nSuit)  // NCR check there are trump out
			   && !pDclrHand->IsSuitShownOut(nTrumpSuit) && (GetNumOutstandingCards(nTrumpSuit) != 0) )
			{
				continue; // skip suit if declarer could trump it
			}
			CSuitHoldings& suit = m_pHand->GetSuit(nSuit);
			if ((suit.GetNumTopCards() > 0) && ((nSuit != nTrumpSuit) ))
				                                // NCR-313 draw trump if we have all winners ie #winners = #cards
// NCR-432 replaces this				                               || (suit.GetNumTopCards() >= suit.GetNumWinners())) )
			{
				pLeadCard = suit.GetTopSequence().GetBottomCard();

				// NCR-217 Don't cash Ace if Dmy as RHO has King
				// Need code here to test if our card is only stopper for dummy's high card!
				if((dummyHand.GetSuit(nSuit).GetNumCards() > 1) // need 2 cards to worry
					&& (dummyHand.GetSuit(nSuit).GetTopCard()->GetFaceValue() == pLeadCard->GetFaceValue()-1))  // Q&D
					continue;  // skip this one
				// NCR-471 Don't lead Ace if there are some missing honors
				if((pLeadCard->GetFaceValue() == ACE) && (suit.GetNumMissingHonors() > 2)
					&& (suit.GetSequence2(0).GetNumCards() < 2)) // Use winner if 2 or more
					continue; // NCR-471 skip this suit

				status << "PLYLDA! With no other obvious plays, cash a winner with the " & pLeadCard->GetName() & ".\n";
				ASSERT(m_pHand->HasCard(pLeadCard));
				return pLeadCard;
			}
			// else look at the next suit
//NCR			nSuit = GetNextSuit(nSuit);
		}
	}

	// if we have a card in an unbid suit, lead from that suit.
	CArray<int,int> suitsUnbid;
	int numSuitsUnbid = pDOC->GetSuitsUnbid(suitsUnbid);
	for(int i=0;i<numSuitsUnbid;i++)
	{
		CSuitHoldings& suit = m_pHand->GetSuit(suitsUnbid[i]);
		if (suit.GetNumCards() > 0)
		{		
			// NCR don't lead a card that will give declarer Ruff and Sluff
			CCardList outstandingCards;
			int numOutstandingCards = GetOutstandingCards(suit.GetSuit(), outstandingCards, true);
			if (ISSUIT(nTrumpSuit) && (numOutstandingCards == 0))  // NCR assume some trump out!!!
				continue;  // NCR skip this suit if its an orphan

			//NCR Check if dummy has top cards in chosen suit. If so, skip it
			CSuitHoldings& dummySuit = GetDummySuit(suit.GetSuit()); //NCR get dummy's suit
			CCard* pTopOutstandingCard = GetHighestOutstandingCard(suit.GetSuit(), true); //NCR look at dummy also
			//NCR test dummy does NOT have top card and is NOT void
            if(pTopOutstandingCard == NULL || (!dummySuit.HasCard(pTopOutstandingCard) && !dummySuit.IsVoid()) ) 
			{ 
				// NCR following needs work???
				if(suit.IsDoubleton()) {
					pLeadCard = suit.GetTopCard();
					if(*pLeadCard >= QUEEN)
						continue;	// NCR don't lead a KING or QUEEN from doubleton
				}
				else
				{
					// NCR check for 3 card sequence and lead top
					if(suit.GetTopSequence().GetNumCards() > 2)
						pLeadCard = suit.GetTopSequence().GetTopCard();
					else if((suit.GetNumSequences() > 1) && (suit.GetSecondSequence().GetNumCards() > 2))
						pLeadCard = suit.GetSecondSequence().GetTopCard();
					// NCR If dummy has singleton, beat if if possible, unless its our partner
					else if(dummySuit.IsSingleton() && (pDOC->GetDummyPlayer() != m_pPartner)) 
					{
						pLeadCard = suit.GetLowestCardAbove(dummySuit.GetTopCard());
						if(pLeadCard == NULL) pLeadCard = suit.GetBottomCard();
					}
					// NCR don't unguard an honor
					else if(((suit.GetTopCard()->GetFaceValue() == QUEEN) && (suit.GetNumCards() <= 3))
						    || ((suit.GetTopCard()->GetFaceValue() == KING) && (suit.GetNumCards() <= 2)))
					{
						continue;  // don't unguard the honor
					}
					// NCR-32 lead top of seq if # outstanding winners < sequence length
					else if(suit.GetSequence2(0).GetNumCards() 
						    > outstandingCards.GetNumCardsAbove(suit.GetTopCard()))
					{
						pLeadCard = suit.GetTopCard();  // top card is top of top sequence
					}
					else
						pLeadCard = suit.GetBottomCard();
				}
				ASSERT(m_pHand->HasCard(pLeadCard));
				status << "PLYLDB! With no other clear plays available, lead a card from the unbid " &
					  STSS(pLeadCard->GetSuit()) & " suit.\n";
				return pLeadCard;
			} //NCR end checking if dummy has top card
		} // end have cards in suit
	} // end for() thru unbid suits

	// no winners in hand, so just lead anything
	// NCR No outstanding trumps same as NoTrump
	if (ISSUIT(nTrumpSuit) && (GetNumOutstandingCards(nTrumpSuit, true) != 0)) //NCR added nbr of trumps test
	{
		// playing in a suit contract
		// if we have any trumps left, _and_ have a singleton, then lead it
		if ((m_pHand->GetNumTrumps() > 0) && (m_pHand->GetNumSingletons() > 0))
		{
			// search for the singleton suit
			BOOL bSuitFound = FALSE;
			int nSuit;
			for(int i=3;i>=0;i--)
			{
				nSuit = m_pHand->GetSuitsByLength(3);
				if (m_pHand->GetNumCardsInSuit(nSuit) > 1)
					break;	// oops, no more singletons
				// check if this is a non-trump singleton suit
				// NCR check that dummy not void
				if ((m_pHand->GetNumCardsInSuit(nSuit) == 1) && (nSuit != nTrumpSuit)
					// NCR and dummy NOT void or is void in trump
					&& !(GetDummySuit(nSuit).IsVoid() && !GetDummySuit(nTrumpSuit).IsVoid()) )
				{
					bSuitFound = TRUE;
					break;
				}
			}
			// lead a card from the suit
			if (bSuitFound)
			{
				CSuitHoldings& suit = m_pHand->GetSuit(nSuit);
				pLeadCard = suit[0];
				status << "PLYLDC! Lead the singleton " & pLeadCard->GetName() 
					      & " in the hopes of setting up a ruff later.\n";
				ASSERT(m_pHand->HasCard(pLeadCard));
				return pLeadCard;
			}
		}

		// else we're stuck -- just lead a card from the worst suit
		// (i.e., keep the "good" suits alive)
		// NCR save suit that could be ruffed in case a better one
		int nMaybeSuit = NONE;
		for(int i=3;i>=0;i--)
		{
			int nSuit = m_pHand->GetSuitsByPreference(i);
			// avoid leading from the trump suit if we can help it
			if ((nSuit == nTrumpSuit) || (m_pHand->GetNumCardsInSuit(nSuit) == 0))
				continue;
			// NCR Skip suit if dummy is void and has trump
		    if((GetDummySuit(nSuit).IsVoid() && !GetDummySuit(nTrumpSuit).IsVoid())
				//  NCR-516 Don't lead into Dummy's TenAce
				|| ((pDOC->GetDummyPlayer() == m_pRHOpponent) && (GetDummySuit(nSuit).HasTenAce())) )
			{
				if(nMaybeSuit == NONE)
					nMaybeSuit = nSuit; // save in case no others found
				continue;
			}
			// else lead from this suit
//NCR			if (m_pHand->GetNumCardsInSuit(nSuit) > 0)
			{
				nMaybeSuit = nSuit;  // save
			}
		}
		// NCR test if we found something
		if (nMaybeSuit != NONE)
		{
			CSuitHoldings& maybeSuit = m_pHand->GetSuit(nMaybeSuit); // NCR-46 
			pLeadCard = maybeSuit.GetBottomCard();
			// NCR-46 don't lead a card below dummy's lowest card if possible
			CSuitHoldings& dummySuit = GetDummySuit(nMaybeSuit);
			if(!dummySuit.IsVoid()
			   && (dummySuit.GetBottomCard()->GetFaceValue() > pLeadCard->GetFaceValue())
			   && (maybeSuit.GetNumCards() > 2) )  // NCR-46 how many 3 or ???
			{
				CCard* testCard = maybeSuit.GetLowestCardAbove(dummySuit.GetBottomCard()); 
				// NCR-65 Don't lead an honor if sure to lose ??? how to determine if top card is winner?
				if((testCard != NULL) && (testCard->GetFaceValue() < QUEEN)) 
					pLeadCard = testCard; // NCR-46 change to a higher card 
			}  // NCR-574 Lead high if we have the top cards
			else if(maybeSuit.GetNumTopCards() == dummySuit.GetNumCards()
				     && (GetNumOutstandingCards(nMaybeSuit) <= maybeSuit.GetNumCards())) {
				pLeadCard = maybeSuit.GetTopCard(); // NCR-574 Cover dummy to prevent cheap trick
			}
			ASSERT(m_pHand->HasCard(pLeadCard));
			status << "PLYLDD! With no other obvious plays, play " & pLeadCard->GetName() & ".\n"; //NCR
			return pLeadCard;
		}

		// else we're stuck with leading a trump
		if (m_pHand->GetNumTrumps() > 0)
		{
			pLeadCard = m_pHand->GetSuit(nTrumpSuit).GetAt(0);
			status << "PLYLDDa! With no other obvious plays, play " & pLeadCard->GetName() & ".\n"; //NCR
			return pLeadCard;
		}
	}
	else
	{
		// playing in notrump
		// lead a card from the worst suit, keeping the good suits alive
		for(int i=3;i>=0;i--)
		{
			int nSuit = m_pHand->GetSuitsByPreference(i);
			if (m_pHand->GetNumCardsInSuit(nSuit) > 0)
			{
				CSuitHoldings& testSuit = m_pHand->GetSuit(nSuit);
				// NCR check if a sequence and use its top card
				// Problem: first find used vs testing all suits???
				if(testSuit.GetTopSequence().GetNumCards() >= 2) {
					pLeadCard = m_pHand->GetSuit(nSuit).GetTopCard();
					break;
				}
				pLeadCard = testSuit.GetBottomCard();
				if(*pLeadCard >= QUEEN) {
					continue;  // NCR leave pointer but hope for another one
				}
				else
				{
					// NCR-4 Check if lead will be sure loser - there are other higher cards
					// and partner doesn't have any
					CCardList cardList;
					bool bCntDmy = (pDOC->GetDummyPlayer() != GetPartner());
					CSuitHoldings& dmySuit = pDOC->GetDummyPlayer()->GetHand().GetSuit(nSuit);  //  NCR-699
					int numCardsOS = GetOutstandingCards(nSuit, cardList,  bCntDmy);
					if((numCardsOS > 0) 
					   && (m_ppGuessedHands[GetPartner()->GetPosition()]->IsSuitShownOut(nSuit)
					       // NCR-43 or does dummy have ALL of the cards outstanding? 
					       || (dmySuit.GetNumCards() ==  numCardsOS)
						   // NCR-699 or if dmy has enough winners
						   || (dmySuit.GetNumWinners() >= (numCardsOS + 1)/2) )
					   && (pLeadCard->GetFaceValue() < cardList.GetTopCard()->GetFaceValue()) )
						continue;  // NCR-4 skip this card if sure loser
					else
						break;  // if not an honor, use it
				}
//				ASSERT(m_pHand->HasCard(pLeadCard));
//				status << "PLYLDE! With no other obvious plays, play " & pLeadCard->GetName() & ".\n"; //NCR
//				return pLeadCard;
			}
		} // end for(i) thru Suits

		// NCR hope that above found something
		ASSERT(m_pHand->HasCard(pLeadCard));
		status << "PLYLDE! With no other obvious plays, play " & pLeadCard->GetName() & ".\n"; //NCR
		return pLeadCard;
	} // NCR end notrump selection


	// we should NEVER get here
	ASSERT(FALSE);
	return NULL;
} // end GetLeadCard()




//
// GetDiscard()
//
CCard* CPlayEngine::GetDiscard()
{
	return m_pHand->GetDiscard();
}




//
// PickFinalDiscardSuit()
//
int CPlayEngine::PickFinalDiscardSuit(CHandHoldings& hand)
{
	// this routine is called by the declarer & defender engined when
	// they absolutely can't find a card to discard
	int nDiscardSuit = NONE;

	// this means generally that we have no suits with losers, 
	// or that the only one with losers in the current hand is the priority suit
	// so look through suits by preference for one that has losers
	int nMaybeSuitLen = 0;  // NCR allow > 1 to be found vs first; we'll use the longest or weakest
	CSuitHoldings* maybeSuit = NULL;
    int nTrumpSuit = pDOC->GetTrumpSuit();

	for(int i=3;i>=0;i--)
	{
		int nSuit = hand.GetSuitsByPreference(i);
		if(nSuit == nTrumpSuit)
			continue;		// NCR skip trumps ???
		CSuitHoldings& testSuit = hand.GetSuit(nSuit);
		// avoid letting go of winners!
		if ((testSuit.GetNumCards() > 0) 
			// NCR Select a suit if it has > 2 winners
			// NCR or if all suits have winners, select one with most cards ???
			// NCR what good are winners if there are no entries??? Test if there are outstanding cards
			&& (((testSuit.GetNumCards() > testSuit.GetNumWinners()) && (GetNumOutstandingCards(nSuit) > 0))  
			    || testSuit.GetNumCards() >= 2) ) //NCR ??? added = here 5/28/09
		{
			// NCR save it, we'll use the longest one
			// try to protect an honor by keeping at least one card with it???
			if((testSuit.GetNumCards() > nMaybeSuitLen) 
				&& !(testSuit.GetNumCards() == 2 && testSuit.GetTopCard()->GetFaceValue() > JACK))
			{
				nDiscardSuit = nSuit;
				nMaybeSuitLen = testSuit.GetNumCards(); // save length to find longest
				maybeSuit = &testSuit;
			}
			// If the same length, chose weaker suit (or if same randomly choose one)
			else if((nMaybeSuitLen > 0) && (testSuit.GetNumCards() == nMaybeSuitLen)) 
			{
				// NCR what if one suit has sure winners and the other suit has a high
				// card that needs protection. EG SKQ and CK9 with SA out. SQ is better discard???
				// If suitA is winner & suitB is not winner and only n-1 cards greater
				//     discard from suitA  -  protect possible winner in suitB
				// If suitB is winner & suitA is not winner and only m-1 cards greater
				//     discard from suitB - protect possible winner in suitA
				// NCE end This all depends on what has been played!!!
				if(testSuit.GetTotalPoints() < maybeSuit->GetTotalPoints()) 
				{
					nDiscardSuit = nSuit;
					maybeSuit = &testSuit;
				}
				else if(testSuit.GetTotalPoints() == maybeSuit->GetTotalPoints())
				{
					nDiscardSuit = (OneIn2() ? nSuit : maybeSuit->GetSuit());
				}

			}
//NCR			break;
		}
	}			
	
	// if we failed above, we absolutely have no losers anywhere 
	// so search for a suit that has winners but no outstanding cards
	// i.e., suits that we may not be able to cash
	if (!ISSUIT(nDiscardSuit))
	{
		for(int j=3;j>=0;j--) // NCR-FFS change i to int j
		{
			int nSuit = hand.GetSuitsByPreference(j);
			if(nSuit == nTrumpSuit)
				continue;		// NCR skip trumps ???

			// see if we have cards in this suit, and no outstanding cards in the suit
			if ((hand.GetNumCardsInSuit(nSuit) > 0) && (GetNumOutstandingCards(nSuit) == 0))
			{
				nDiscardSuit = nSuit;
				break;
			}
				
		}
	}

	// NCR If we're here without any suit, pick the longest one
	if (!ISSUIT(nDiscardSuit))
 	{
		int nLongestSuit = NONE;
		int nLongestSuitLen = 0;
		for(Suit nSuit = CLUBS; nSuit <= SPADES; GETNEXTSUIT(nSuit))
		{
			if(nSuit == nTrumpSuit)
				continue;		// NCR skip trumps ???
			CSuitHoldings& testSuit = hand.GetSuit(nSuit);
			if (testSuit.GetNumCards() > nLongestSuitLen){
				nLongestSuitLen = testSuit.GetNumCards();
				nLongestSuit = nSuit;  // Save longest suit
			}
		} // end for(nSuit)
		if(nLongestSuit != NONE)
			nDiscardSuit = nLongestSuit;  // Set to longest
	}  // end looking for longest suit
	
	// finally, give up and get any old suit that has cards
	if (!ISSUIT(nDiscardSuit))
 	{
		//NCR don't discard the top card remaining outstanding
		for(int j=3;j>=0;j--)  // NCR-FFS changed i to int j
		{
			int nSuit = hand.GetSuitsByPreference(j);
			if (hand.GetNumCardsInSuit(nSuit) > 0)
			{
				nDiscardSuit = nSuit;
				break;
			}
		}
	}

	//
	ASSERT(ISSUIT(nDiscardSuit));
	return nDiscardSuit;

} // end PickFinalDiscardSuit()







//
//==========================================================================
//==========================================================================
//
// Analysis routines
//
//




//
// GetOutstandingCards()
//
// determine the list of outstanding cards in this suit
// outstanding means all the cards that are not in our own hand
// (or dummy), _and_ which have not been played
//
int	CPlayEngine::GetOutstandingCards(int nSuit, CCardList& cardList, bool bCountDummy) const
{
	VERIFY(ISSUIT(nSuit));

	// first fill the list with all the cards in the suit
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=2;i<=ACE;i++)
		cardList << deck.GetSortedCard(MAKEDECKVALUE(nSuit,i));
	cardList.Sort();

	// then remove the cards remaining in our own hand
	CSuitHoldings& suit = m_pHand->GetSuit(nSuit);
	for(i=0;i<suit.GetNumCards();i++)
		cardList.Remove(suit[i]);

	// then remove the cards in dummy from the list
	// but only if indicated, _and_ dummy's cards are visible
	if (!bCountDummy)		// i.e, don't count dummy's cards as outstanding
	{
		CPlayer* pDummy = pDOC->GetDummyPlayer();
		if (pDummy->AreCardsExposed() && (GetPlayerPosition() != pDOC->GetDummyPosition()))
		{
			CSuitHoldings& dummySuit = pDummy->GetHand().GetSuit(nSuit);
			for(int i=0;i<dummySuit.GetNumCards();i++)
				cardList.Remove(dummySuit[i]);
		}
	}

	// then search the list of guessed cards for cards in this suit 
	// that have already been played, and remove them from the list
	int nOurPos = m_pPlayer->GetPosition();
	for(int nPlayer=0;nPlayer<4;nPlayer++)
	{
		CGuessedSuitHoldings& suit = m_ppGuessedHands[nPlayer]->GetSuit(nSuit);
		for(int j=0;j<suit.GetNumDefiniteCards();j++)
		{
			if (suit[j]->WasPlayed())
				cardList.Remove(deck.GetSortedCard(suit[j]->GetDeckValue()));
		}
	}

	// now we have only the outstanding cards in the suit
	cardList.Sort();
	return cardList.GetNumCards();
}


//
// NCR - GetLowestCardNeededToWin
//
// Assumes that dummy is RHOpp.
// Make list of cards in trick, what's been played, what's in my hand and what's in dummy
// Select lowest card from my hand needed to win the trick
// If not able to win the trick, return a low card
//  Returns NULL if no card found
//
CCard* CPlayEngine::GetLowestCardNeededToWin(const CSuitHoldings& mySuit, const CSuitHoldings& dmySuit,
										     BOOL bPartnerHigh) const
{
	if(mySuit.IsVoid())
		return NULL;          // I have no cards to return 
	int cardsSeen[ACE+1] = {0};  // hold flags for cards(2-Ace); index with FaceValues

	// Define values for flags
	const int Played = 11;
	const int Mine = 22;
	const int Dummy = 33;

	// Now fill in array with accounted for cards
	int nTopPos;
	CCard* pCurrTopCard = pDOC->GetCurrentTrickHighCard(&nTopPos);
	const int topCardFV = pCurrTopCard->GetFaceValue();
	cardsSeen[topCardFV] = Played;  // top card only one needed of what's been played

	for(int i=0; i < mySuit.GetNumCards(); i++) {
		cardsSeen[mySuit.GetAt(i)->GetFaceValue()] = Mine;
	}
	for(int j=0; j < dmySuit.GetNumCards(); j++) {
		cardsSeen[dmySuit.GetAt(j)->GetFaceValue()] = Dummy;
	}
	// Now get cards played in this suit???
	int dv = pCurrTopCard->GetDeckValue()+1;   // get first possible played card
	if(!ISCARD(dv) || topCardFV == ACE)
		return mySuit.GetBottomCard();		   // Topcard is Ace

	int fv = topCardFV+1;                      // keep this in sync with the above
	for(; fv <= ACE; fv++, dv++) {
   		if(pDOC->WasCardPlayed(dv)) {
			ASSERT(cardsSeen[fv] == 0);  // NCR the card should not be here
			cardsSeen[fv] = Played;
		}
	}

    // Now start at top look for gap (ie outstanding card)
	for(int k=ACE; k > topCardFV; k--){
		if(cardsSeen[k] == 0) {
			if(k == ACE)
				return NULL;  // Ace is missing, let caller resolve
			// k has FaceValue of missing card, return our winner over it
			return mySuit.GetLowestCardAbove(k);
		}
	}

	// if no gap(outstanding card) found then two cases:
	// If partner high, then play low
	// if partner not high, the play higher than top card
	if(bPartnerHigh)
		return mySuit.GetBottomCard();  //  let pard win it
	else
		return mySuit.GetLowestCardAbove(topCardFV);  // beat what's there now
}  // NCR end GetLowestCardedNeededToWin()

//
// NCR InSequence()
// Test if three cards are InSequence considering played cards
//
bool CPlayEngine::InSequence(const CCard* pCard1, const CCard* pCard2, const CCard* pCard3) const
{
	if(pCard1 == NULL || pCard2 == NULL || pCard3 == NULL)
		return false;  // all three must be good cards

	// Start at the highest card and work down check for gaps
	int thisDV = pCard1->GetDeckValue() - 1;
	const int card2DV = pCard2->GetDeckValue();
	const int card3DV = pCard3->GetDeckValue();
	if((thisDV < card2DV) || (card2DV < card3DV))
		return false;  // not in order

	while(thisDV > card2DV) {
		if(!pDOC->WasCardPlayed(thisDV))
			return false;  // gap if card not played
		thisDV--;  // go down another
	}
	thisDV--;  // go below card2
	while(thisDV > card3DV) {
		if(!pDOC->WasCardPlayed(thisDV))
			return false;  // gap if card not played
		thisDV--;  // go down another
	}
    // If we made it here, must NOT be any gaps between the 3 cards
	return (thisDV == card3DV); // should be true
}

//
// GetHighestOutstandingCard()
//
CCard* CPlayEngine::GetHighestOutstandingCard(int nSuit, bool bCountDummy) const  // NCR added bool etc
{
	CCardList cardList;
	int nCount = GetOutstandingCards(nSuit, cardList, bCountDummy);
	if (nCount > 0)
		return cardList[0];
	else
		return NULL;
}



//
// GetNumOutstandingCards()
//
int CPlayEngine::GetNumOutstandingCards(int nSuit, bool  bCountDummy) const  // NCR added bool
{
	CCardList cardList;
	return GetOutstandingCards(nSuit, cardList,  bCountDummy);  // NCR added bCountDummy
}



//
// IsCardOutstanding()
//
// determine whether a card is still outstanding
//
BOOL CPlayEngine::IsCardOutstanding(CCard* pCard) const
{
	VERIFY(pCard->IsValid());
	// get the list of outstanding cards in this suit
	CCardList cardList;
	/*return*/ GetOutstandingCards(pCard->GetSuit(), cardList);  // NCR removed return here ???
	// and see if the card is among them
	return cardList.HasCard(pCard);
}



//
// IsCardOutstanding()
//
BOOL CPlayEngine::IsCardOutstanding(int nSuit, int nFaceValue) const
{
	int nDeckValue = MAKEDECKVALUE(nSuit, nFaceValue);
	ASSERT(ISDECKVAL(nDeckValue));
	CCard* pCard = deck.GetSortedCard(nDeckValue);
	return IsCardOutstanding(pCard);
}



//
// GetNumCardsPlayedInSuit()
//
int	CPlayEngine::GetNumCardsPlayedInSuit(int nPlayer, int nSuit) const
{
	VERIFY((nPlayer >= 0) && (nPlayer <= 3) && ISSUIT(nSuit));
	return m_ppGuessedHands[nPlayer]->GetSuit(nSuit).GetNumCardsPlayed();
}



//
// GetMinCardsInSuit()
//
// returns the minimum number of cards a player currently holds in a suit
//
int CPlayEngine::GetMinCardsInSuit(int nPlayer, int nSuit) const
{
	VERIFY((nPlayer >= 0) && (nPlayer <= 3) && ISSUIT(nSuit));

	// see if the hand's cards have all been identified
	CGuessedSuitHoldings& suit = m_ppGuessedHands[nPlayer]->GetSuit(nSuit);
	if (suit.AreAllCardsIdentified())
		return suit.GetNumMinRemainingCards();

	// else see if the player is dummy & his cards are visible
	if (nPlayer == pDOC->GetDummyPosition() && pDOC->IsDummyExposed())
	{
		CHandHoldings& hand = pDOC->GetDummyPlayer()->GetHand();
		return hand.GetNumCardsInSuit(nSuit);
	}

	// the initial minimum is of course 0
	int nMin = 0;

	// adjust the minimum only if everyone but ourselves (and the player)
	// has shown out, and there are still cards outstanding
	BOOL bAllShownOut = TRUE;
	int nOurPos = m_pPlayer->GetPosition();
	for(int nPos=0;nPos<4;nPos++)
	{
		if ((nPos == nOurPos) || (nPos == nPlayer))
			continue;
		if (!m_ppGuessedHands[nPos]->IsSuitShownOut(nSuit))
		{
			// someone may still have cards left
			bAllShownOut = FALSE;
			break;
		}
	}

	// see if the two other players have shown out
	if (bAllShownOut)
	{
		// see if the number of suit cards played so far, plus the suit
		// cards we have remaining, add up to to less than 13
		int numCardsPlayed = 0;
		// count the cards that have been played in the suit
		for(int i=0;i<4;i++)
			numCardsPlayed += m_ppGuessedHands[i]->GetSuit(nSuit).GetNumCardsPlayed();
		// add the suit cards remaining in our own hand
		numCardsPlayed += m_pHand->GetSuit(nSuit).GetNumCards();
		if (numCardsPlayed < 13)
		{
			// the player must have the remaining cards
			nMin = 13 - numCardsPlayed;
			// take this opportunity to update the player's info
			// mark the player as having the missing cards
			CSuitHoldings  missingCards;
			int nCount = GetOutstandingCards(nSuit, missingCards);
			VERIFY(nCount == nMin);
			CGuessedHandHoldings* pHand = m_ppGuessedHands[nPlayer];
			for(int j=0;j<nCount;j++)
			{
				CGuessedCard* pGuessedCard = new CGuessedCard(missingCards[j],	// card
															  TRUE,				// still outstanding
															  nPlayer,			// location
															  1.0);				// known with certainty
				*pHand << pGuessedCard;
			}
			// update status
			suit.MarkAllCardsIdentified();
			CPlayerStatusDialog& status = *m_pStatusDlg;
			status << "4PLEMN5! " & PositionToString(nPlayer) &
					  " is now known to hold the remaining " & 
					  missingCards.GetNumCards() & " " & STS(nSuit) & 
					  " (" & missingCards.GetHoldingsString() & ").\n";
			// and update counts
			suit.SetNumOriginalCards(nMin + m_ppGuessedHands[nPlayer]->GetSuit(nSuit).GetNumCardsPlayed());
			suit.SetNumRemainingCards(nMin);
			suit.SetNumLikelyCards(nMin);
			suit.SetNumMinRemainingCards(nMin);
			suit.SetNumMaxRemainingCards(nMin);
		}
	}

	// done
	return nMin;
}



//
// GetMaxCardsInSuit()
//
// returns the maximum number of cards a player currently holds in a suit
//
int CPlayEngine::GetMaxCardsInSuit(int nPlayer, int nSuit) const
{
	VERIFY((nPlayer >= 0) && (nPlayer <= 3) && ISSUIT(nSuit));

	// see if the hand's cards have all been identified
	CGuessedSuitHoldings& suit = m_ppGuessedHands[nPlayer]->GetSuit(nSuit);
	if (suit.AreAllCardsIdentified())
		return suit.GetNumMaxRemainingCards();

	// else see if the player is dummy & his cards are visible
	if (nPlayer == pDOC->GetDummyPosition() && pDOC->IsDummyExposed())
	{
		CHandHoldings& hand = pDOC->GetDummyPlayer()->GetHand();
		return hand.GetNumCardsInSuit(nSuit);
	}

	// else calc
	// initial max is 13 cards in a suit
	int nMax = 13;

	// deduct the # of our original suit cards 
	nMax -= m_pHand->GetInitialHand().GetNumCardsOfSuit(nSuit);

	// deduct dummy's suit holdings, if visible
	CPlayer* pDummy = pDOC->GetDummyPlayer();
	if (pDummy->AreCardsExposed())
	{
		CCardHoldings& initialHand = pDummy->GetHand().GetInitialHand();
		nMax -= initialHand.GetNumCardsOfSuit(nSuit);
	}

	// and deduct the cards that have been played by the other two players
	int nDummyPos = pDummy->GetPosition();
	int nOurPos = m_pPlayer->GetPosition();
	for(int nPos=0;nPos<4;nPos++)
	{
		if ((nPos == nDummyPos) || (nPos == nOurPos))
			continue;
		nMax -= GetNumCardsPlayedInSuit(nPos, nSuit);
	}

	// if the max == 0, all the player's suit cards have been identified
	if (nMax == 0)
	{
		// update status
		suit.MarkAllCardsIdentified();
		// and update counts
		suit.SetNumOriginalCards(nMax + m_ppGuessedHands[nPlayer]->GetSuit(nSuit).GetNumCardsPlayed());
		suit.SetNumRemainingCards(nMax);
		suit.SetNumLikelyCards(nMax);
		suit.SetNumMinRemainingCards(nMax);
		suit.SetNumMaxRemainingCards(nMax);
	}

	// and we're done
	return nMax;
}




//
// GetMinStartingCardsInSuit()
//
// returns the minimum possible number of cards a player could have 
// started out with in a suit
//
int CPlayEngine::GetMinStartingCardsInSuit(int nPlayer, int nSuit) const
{
	VERIFY((nPlayer >= 0) && (nPlayer <= 3) && ISSUIT(nSuit));

	// see if the player is dummy & his cards are visible
	if (nPlayer == pDOC->GetDummyPosition() && pDOC->IsDummyExposed())
	{
		CHandHoldings& hand = pDOC->GetDummyPlayer()->GetHand();
		return hand.GetInitialHand().GetNumCardsOfSuit(nSuit);
	}

	// the minimum _starting_ count is the same as the current min count
	int nMin = GetMinCardsInSuit(nPlayer, nSuit);
	return nMin;
}




//
// GetMaxStartingCardsInSuit()
//
// returns the maximum possible number of cards a player could have 
// started out with in a suit
//
int CPlayEngine::GetMaxStartingCardsInSuit(int nPlayer, int nSuit) const
{
	VERIFY((nPlayer >= 0) && (nPlayer <= 3) && ISSUIT(nSuit));

	// see if the player is dummy & his cards are visible
	if (nPlayer == pDOC->GetDummyPosition() && pDOC->IsDummyExposed())
	{
		CHandHoldings& hand = pDOC->GetDummyPlayer()->GetHand();
		return hand.GetInitialHand().GetNumCardsOfSuit(nSuit);
	}

	// start with the current maximum holdings for that player in the suit
	int nMax = GetMaxCardsInSuit(nPlayer, nSuit);

	// add back the cards that player had played
	nMax += GetNumCardsPlayedInSuit(nPlayer, nSuit);

	// and we're done
	return nMax;
}





//
// AssessPosition()
//
// fill in some shortcut variables
//
void CPlayEngine::AssessPosition()
{
	CEasyBDoc* pDoc = pDOC;
	m_nRound = pDoc->GetNumTricksPlayed();
	m_numCardsPlayed = pDoc->GetNumCardsPlayedInRound();
	if (m_numCardsPlayed > 0)
	{
		m_nRoundLead = pDoc->GetRoundLead();
		m_pLeadCard = pDoc->GetCurrentTrickCard(m_nRoundLead);
		m_nLeadSuit = m_pLeadCard->GetSuit();
		m_nLeadFaceValue = m_pLeadCard->GetFaceValue();
	}
	else
	{
		m_nRoundLead = -1;
		m_pLeadCard = NULL;
		m_nLeadSuit = NONE;
		m_nLeadFaceValue = -1;
	}
	//

}










//
//================================================================
//
// Value Set/Retrieval
//

//
LPVOID CPlayEngine::GetValuePV(int nItem, int nIndex1, int nIndex2, int nIndex3) const
{
	switch (nItem)
	{
		case 0:
			break;
		//
		default:
			AfxMessageBox("Unhandled Call to CDeclarerPlayEngine::GetValue()");
			return NULL;
	}
	return NULL;
}

//
double CPlayEngine::GetValueDouble(int nItem, int nIndex1, int nIndex2, int nIndex3) const
{
/*	switch (nItem)
	{
		default:
			AfxMessageBox("Unhandled Call to CDeclarerPlayEngine::GetValue");
			return 1;
	}
*/
	return 0;
}

//
// SetValuePV()
//
int CPlayEngine::SetValuePV(int nItem, LPVOID value, int nIndex1, int nIndex2, int nIndex3)
{
	int nValue = (int) value;
	BOOL bValue = (BOOL) value;
	LPCTSTR sValue = (LPCTSTR) value;
	//
	switch (nItem)
	{
		case 0:
			break;
		// 
		default:
			AfxMessageBox("Unhandled Call to CDeclarerPlayEngine::SetValue()");
			return 1;
	}
	return 0;
}

// "double" version
int CPlayEngine::SetValue(int nItem, double fValue, int nIndex1, int nIndex2, int nIndex3)
{
//	switch (nItem)
//	{
//		default:
			AfxMessageBox("Unhandled Call to CDeclarerPlayEngine::SetValue()");
			return 1;
//	}
//	return 0;
}

//
int CPlayEngine::GetValue(int nItem, int nIndex1, int nIndex2, int nIndex3) const
{
	return (int) GetValuePV(nItem, nIndex1, nIndex2, nIndex3);
}

int CPlayEngine::SetValue(int nItem, int nValue, int nIndex1, int nIndex2, int nIndex3)
{
	return SetValuePV(nItem, (LPVOID) nValue, nIndex1, nIndex2, nIndex3);
}


//
int CPlayEngine::GetPlayerPosition() const
{
	return m_pPlayer->GetPosition();
}

//
int CPlayEngine::GetPartnerPosition() const
{
	return m_pPartner->GetPosition();
}


