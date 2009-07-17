//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Sluff.cpp
//
// - a play of throwing away losing cards
//
#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "card.h"
#include "HoldUp.h"
#include "Player.h"
#include "PlayEngine.h"
#include "CombinedHoldings.h"
#include "CombinedSuitHoldings.h"  // NCR-5
#include "CardLocation.h"
#include "GuessedHandHoldings.h"
#include "PlayerStatusDialog.h"



//
//==================================================================
// constructon & destruction

CHoldUp::CHoldUp(int nSuit) :
			CPlay(CPlay::HOLDUP, IN_EITHER, nSuit, PP_LOSER)
{
	Init();
}

CHoldUp::CHoldUp(CHoldUp& srcPlay) :
			CPlay(srcPlay.m_nPlayType, IN_EITHER, NONE, PP_LOSER)
{
	m_nSuit = srcPlay.m_nSuit;
}

CHoldUp::~CHoldUp() 
{
}


//
void CHoldUp::Clear()
{
	CPlay::Clear();
}


//
void CHoldUp::Init()
{
	CPlay::Init();
	// form name & description
	m_strName.Format("%s Hold up", STSS(m_nSuit));
	m_strDescription.Format("Hold up a round of %s", STS(m_nSuit));
}



//
CString CHoldUp::GetFullDescription()
{
	return FormString("Hold up a round of %s.",STS(m_nSuit));
}




//
// Perform()
//
PlayResult CHoldUp::Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
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
//	CCard* pOppCard = NULL;
	// 
	CCard* pRoundTopCard = pDOC->GetCurrentTrickHighCard();
	CCard* pDeclarerCard = pDOC->GetCurrentTrickCard(playEngine.GetPlayerPosition());
	CCard* pDummysCard = pDOC->GetCurrentTrickCard(playEngine.GetPartnerPosition());
	CCard* pPartnersCard = bPlayingInHand? pDummysCard : pDeclarerCard;
//	BOOL bPartnerHigh = (pRoundTopCard == pPartnersCard);
	//
//	BOOL bValid = FALSE;

	// test preconditions
	if (!CPlay::IsPlayUsable(combinedHand, playEngine))
	{
		m_nStatusCode = PLAY_INACTIVE;
		return PLAY_POSTPONE;
	}

	// NCR don't holdup if our top card covers opponent's top card and dummy's top card will be left tops
	if(bPlayingInHand){
		// Skip if partner already high or we're not in 3rd hand
		if(((pRoundTopCard != pPartnersCard) && (nOrdinal == 3) 
			  // NCR-25 added ->GetFaceValue() to following:
			  && (!playerSuit.IsVoid() && playerSuit.GetTopCard()->GetFaceValue() > pRoundTopCard->GetFaceValue()))
			     // NCR is players top card same value as dummy's?
		    && (((!dummySuit.IsVoid() && (pDOC->AreCardsInSequence(dummySuit.GetTopCard()->GetDeckValue(), 
			                             playerSuit.GetTopCard()->GetDeckValue()))))
                // NCR-5 cover if we some better cards than the top card 
		        || (combinedHand.GetSuit(m_nSuit).GetNumCardsAbove(pRoundTopCard) >= 3)
		        // NCR-5 or we can beat top card and have some winners after that
		        || (combinedHand.GetSuit(m_nSuit).GetNumCardsAbove(playEngine.GetHighestOutstandingCard(m_nSuit)) >= 3))
		  )
		{
			pPlayCard = playerSuit.GetLowestCardAbove(pRoundTopCard);
			status << "PLHLD01! Finish the hold-up and play the " & 
					  pPlayCard->GetFaceName() & " from " & (bPlayingInHand? "hand" : "dummy") & ".\n";
			m_nStatusCode = PLAY_COMPLETE;
			return m_nStatusCode;
		}
	} // NCR end 


	// a holdup is simple -- discard instead of winning
	switch(nOrdinal)
	{
		case 0:
			// can't hold up here
			m_nStatusCode = PLAY_INACTIVE;
			return PLAY_POSTPONE;

		case 1:
			if (bPlayingInHand) 
				pPlayCard = playerHand.GetDiscard();
			else
				pPlayCard = dummyHand.GetDiscard();
			status << "PLHLD04! Hold up a round of " & STS(m_nSuit) & 
					  " and discard the " & pPlayCard->GetFaceName() & " from " &
					  (bPlayingInHand? "hand" : "dummy") & ".\n";
			m_nStatusCode = PLAY_IN_PROGRESS;
			break;

		case 2:
			// can't hold up here
			m_nStatusCode = PLAY_INACTIVE;
			return PLAY_POSTPONE;

		case 3:
			// complete the hold-up
			if (m_nStatusCode != PLAY_IN_PROGRESS)
				return PLAY_INACTIVE;
			if (bPlayingInHand) 
				pPlayCard = playerHand.GetDiscard();
			else
				pPlayCard = dummyHand.GetDiscard();
			status << "PLHLD08! Finish the hold-up and discard the " & 
					  pPlayCard->GetFaceName() & " from " & (bPlayingInHand? "hand" : "dummy") & ".\n";
			m_nStatusCode = PLAY_COMPLETE;
			break;
	}

	// done
	ASSERT(pPlayCard->IsValid());
	return m_nStatusCode;
}

