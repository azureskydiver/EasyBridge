//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// TypeBFinesse.cpp
//
// Type A Finesse
// - opportunistic play (or non-play) of a non-top card in fourth position
//   against both LHO and RHO
//
//   e.g., holding Kx in fourth position, playing low in second hand 
//
#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "Player.h"
#include "Card.h"
#include "TypeBFinesse.h"
#include "PlayEngine.h"
#include "CombinedHoldings.h"
#include "CardLocation.h"
#include "GuessedHandHoldings.h"
#include "PlayerStatusDialog.h"



//
//==================================================================
// constructon & destruction

CTypeBFinesse::CTypeBFinesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, int nSuit, int nCardVal) :
		CFinesse(CFinesse::TYPE_B, nTargetHand, nPlayerPosition, pGapCards, nSuit, nCardVal, TRUE)
{
	Init();
}

CTypeBFinesse::CTypeBFinesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, CCard* pCard) :
		CFinesse(CFinesse::TYPE_A, nTargetHand, nPlayerPosition, pGapCards, pCard, TRUE)
{
	VERIFY(pCard);
	Init();
}

CTypeBFinesse::~CTypeBFinesse() 
{
}


//
void CTypeBFinesse::Clear()
{
	CFinesse::Clear();
}



//
void CTypeBFinesse::Init()
{
	// call base class
//	CFinesse::Init();

	// check the enemy and location
	m_nTarget = (m_nEndingHand == CFinesse::IN_DUMMY)? AGAINST_LHO: AGAINST_RHO;
	if (m_nTarget == AGAINST_LHO)
		m_nTargetPos = GetNextPlayer(m_nPlayerPosition);
	else
		m_nTargetPos = GetPrevPlayer(m_nPlayerPosition);

	// the ending hand is the target hand (opposite of default)
	m_nStartingHand = m_nEndingHand = m_nTargetHand;
}



//
CString CTypeBFinesse::GetFullDescription()
{
	CString strText;
	return FormString("Opportunistically play the %s from %s to finesse against %s.",
					   m_pConsumedCard->GetFaceName(),
					   (m_nTargetHand == IN_HAND)? "hand" : "dummy",
					   PositionToString(m_nTargetPos));
	// done
	return strText;
}




//
// Perform()
//
PlayResult CTypeBFinesse::Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
						   CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, 
						   CPlayerStatusDialog& status, CCard*& pPlayCard)
{
	// Type A Finesse
	// - opportunistic play of a non-top card in second position to
	//   finesse against LHO

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
	CCard* pTopCard = pDOC->GetCurrentTrickHighCard();
	pPlayCard = NULL;

	// test preconditions
	if (!CPlay::IsPlayUsable(combinedHand, playEngine))
	{
		m_nStatusCode = PLAY_INACTIVE;
		return PLAY_POSTPONE;
	}

	// can't use this play if we can't afford to risk losing any more tricks
	// (???)

	// check our position in the play
	switch(nOrdinal)
	{
		case 0:
			// can't use in first seat
			m_nStatusCode = PLAY_NOT_VIABLE;
			return m_nStatusCode;


		case 1:
			// playing second -- sluff a card
			if (nSuitLed != m_nSuit)
			{
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			// 
			pPlayCard = playEngine.GetDiscard();
			status << "PLBFN20! Sluff the " & pPlayCard->GetName() & " from " &
					 (bPlayingInHand? "hand" : "dummy") & " in second position" 
					 " to prep the " & m_pConsumedCard->GetName() & " play.\n";
			m_nStatusCode = PLAY_IN_PROGRESS;
			break;


		case 2:
			// can't use this play in third posiiton
			m_nStatusCode = PLAY_NOT_VIABLE;
			return m_nStatusCode;


		case 3:
			// make sure the play is in progress
			if (m_nStatusCode != PLAY_IN_PROGRESS)
			{
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			// in 4th position, win or duck
			// see if we can top the high card
			if (*m_pConsumedCard > *pTopCard)
			{
				pPlayCard = m_pConsumedCard;
				status << "PLBFN76! The finesse worked; win with the " & 
									pPlayCard->GetFaceValue() & ".\n";
			}
			else
			{
				// discard low here
				pPlayCard = playEngine.GetDiscard();
				status << "PLBFN77! West played high, so discard the " &
					       pPlayCard->GetFaceValue() & ".\n";
			}
			m_nStatusCode = PLAY_COMPLETE;
	}

	// done
	ASSERT(pPlayCard->IsValid());
	return m_nStatusCode;
}
