//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// TypeAFinesse.cpp
//
// Type A Finesse
// - opportunistic play of a non-top card in second position to
//   finesse against LHO
//
#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "Player.h"
#include "Card.h"
#include "TypeAFinesse.h"
#include "PlayEngine.h"
#include "CombinedHoldings.h"
#include "CardLocation.h"
#include "GuessedHandHoldings.h"
#include "PlayerStatusDialog.h"



//
//==================================================================
// constructon & destruction

CTypeAFinesse::CTypeAFinesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, int nSuit, int nCardVal) :
		CFinesse(CFinesse::TYPE_A, nTargetHand, nPlayerPosition, pGapCards, nSuit, nCardVal, TRUE)
{
	Init();
}

CTypeAFinesse::CTypeAFinesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, CCard* pCard) :
		CFinesse(CFinesse::TYPE_A, nTargetHand, nPlayerPosition, pGapCards, pCard, TRUE)
{
	VERIFY(pCard);
	Init();
}

CTypeAFinesse::~CTypeAFinesse() 
{
}


//
void CTypeAFinesse::Clear()
{
	CFinesse::Clear();
}



//
void CTypeAFinesse::Init()
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
CString CTypeAFinesse::GetFullDescription()
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
PlayResult CTypeAFinesse::Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
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
	CCard* pOppCard = NULL;

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
			// can't use in first seat
			m_nStatusCode = PLAY_NOT_VIABLE;
			return m_nStatusCode;


		case 1:
			// playing second -- this is the key to the finesse
			// see if the wrong suit was led 
			if (nSuitLed != m_nSuit)
			{
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			// check which hand we're playing in
			if (bPlayingInHand) 
			{
				// playing second in our own hand (declarer)
				// see if it's time to finesse
				if (m_nTargetHand == IN_HAND)
				{
					// play the finesse card
					pPlayCard = m_pConsumedCard;
					status << "PLAFN20! Opportunistically finesse the " & pPlayCard->GetName() & " from hand in second position against " &
							  PositionToString(playEngine.GetLHOpponent()->GetPosition()) & ".\n";
				}
				else
				{
					// finessing in hand, but this is dummy? messed up
					status << "4PLAFN30! We intended to finesse in hand, but ended up here in dummy in third position -- so skip this play.\n";
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
					// finesse the card from dummy 
					pPlayCard = m_pConsumedCard;
					status << "PLAFN54! Opportunistically finesse the " & pPlayCard->GetName() & " from dummy in second position against " &
							  PositionToString(playEngine.GetRHOpponent()->GetPosition()) & ".\n";
				}
				else
				{
					// messed up
					status << "4PLAFN60! We intended to finesse in hand, but ended up here in dummy in third position -- so skip this play.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_POSTPONE;
				}
			}
			// all went OK
			m_nStatusCode = PLAY_IN_PROGRESS;
			break;


		case 2:
			// can't use this play in third posiiton
			m_nStatusCode = PLAY_NOT_VIABLE;
			return m_nStatusCode;


		case 3:
			// in 4th position, simply discard 
			if (m_nStatusCode == PLAY_IN_PROGRESS)
			{
				// discard here, but see if we won
				if (pTopCard == m_pConsumedCard)
				{
					pPlayCard = playEngine.GetDiscard();
					status << "PLAFN76! The finesse worked; finish the play by discarding the " & pPlayCard->GetName() & ".\n";
					m_nStatusCode = PLAY_COMPLETE;
				}
				else
				{
					status << "PLAFN77! The finesse failed, so abandon the play\n";
					m_nStatusCode = PLAY_NOT_VIABLE;
				}
				return m_nStatusCode;
			}
			else
			{
				// play is not active
				m_nStatusCode = PLAY_INACTIVE;
				return m_nStatusCode;
			}
	}

	// done
	ASSERT(pPlayCard->IsValid());
	return m_nStatusCode;
}
