//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Drop.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "Deck.h"
#include "Card.h"
#include "Drop.h"
#include "Player.h"
#include "PlayEngine.h"
#include "CombinedHoldings.h"
#include "CardLocation.h"
#include "GuessedHandHoldings.h"
#include "PlayerStatusDialog.h"



//
//==================================================================
// constructon & destruction

CDrop::CDrop(int nTargetHand, int nTargetOpponent, CCardList* pEnemyCards, int nSuit, int nCardVal) :
			CPlay(CPlay::DROP, nTargetHand, nSuit, PP_LIKELY_WINNER),
			m_nTargetOpponent(nTargetOpponent),
			m_nCardVal(nCardVal)
{
	Init();
	m_pEnemyOrKeyCardsList = pEnemyCards;
}

CDrop::CDrop(int nTargetHand, int nTargetOpponent, CCardList* pEnemyCards, CCard* pCard) :
			CPlay(CPlay::DROP, nTargetHand, NONE, PP_LIKELY_WINNER),
			m_nTargetOpponent(nTargetOpponent)
{
	VERIFY(pCard);
	m_nSuit = pCard->GetSuit(); 
	m_nCardVal = pCard->GetFaceValue();
	m_pEnemyOrKeyCardsList = pEnemyCards;
	Init();
}

CDrop::CDrop(CDrop& srcPlay) :
		CPlay(srcPlay.m_nPlayType, srcPlay.m_nTargetHand)
{
	m_nTargetHand = srcPlay.m_nTargetHand;
	m_nTargetOpponent = AGAINST_EITHER;
	m_nSuit = srcPlay.m_nSuit; 
	m_nCardVal = srcPlay.m_nCardVal;
	m_pEnemyOrKeyCardsList = srcPlay.m_pEnemyOrKeyCardsList;
}

CDrop::~CDrop() 
{
}


//
void CDrop::Clear()
{
	CPlay::Clear();
	//
	m_nCardVal = NONE;
	m_nTargetOpponent = AGAINST_EITHER;
	m_numTargetCards = 0;
	m_strDropCards.Empty();
}


//
void CDrop::Init()
{
	CPlay::Init();

	//
	m_pConsumedCard = deck.GetCard(m_nSuit, m_nCardVal);
	m_numTargetCards = m_pEnemyOrKeyCardsList->GetNumCards();
	if (m_numTargetCards == 1)
		m_strDropCards.Format("the %s", m_pEnemyOrKeyCardsList->GetAt(0)->GetName());
	else
		m_strDropCards.Format("one or more of the { %s }", m_pEnemyOrKeyCardsList->GetHoldingsString());

	// form name & description
	m_strDropMessage = " to try and drop " + m_strDropCards;
	m_strName.Format("Drop %s", m_strDropCards);
	m_strDescription.Format("Drop %s w/ the %s.", m_strDropCards, m_pConsumedCard->GetFaceName());
}



//
CString CDrop::GetFullDescription()
{
	// form description
	return FormString("Play the %s from %s%s.", m_pConsumedCard->GetName(), ((m_nTargetHand == IN_HAND)? "hand" : "Dummy"), m_strDropMessage);
}




//
// Perform()
//
PlayResult CDrop::Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
				   CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, 
			       CPlayerStatusDialog& status, CCard*& pPlayCard)
{
	// this code is almost, but not quite, the same as a cash

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
	int nSuitLed = pCardLed? pCardLed->GetSuit() : NONE;
	// see if a trump was played in this round
	BOOL bTrumped = FALSE;
	if ((nSuitLed != pDOC->GetTrumpSuit()) && (pDOC->WasTrumpPlayed()))
		bTrumped = TRUE;
	pPlayCard = NULL;
	BOOL bDropSucceeded = FALSE;
	BOOL bLHODropped = FALSE;
	CCard *pRHOCard, *pLHOCard;
	//
	CString strLHO = bPlayingInHand? playEngine.szLHO : playEngine.szRHO;
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
			if ( ((bPlayingInHand) && (m_nTargetHand == IN_HAND)) ||
				 ((!bPlayingInHand) && (m_nTargetHand == IN_DUMMY)) )
			{
				// leading high from hand
				pPlayCard = m_pConsumedCard;
				status << "PLDRP10! Lead the " & pPlayCard->GetName() &
						  m_strDropMessage & ".\n";
			}
			else
			{
				// in the opposite hand, so lead 
				if ( (bPlayingInHand && playerSuit.IsVoid()) ||
						(!bPlayingInHand && dummySuit.IsVoid()) )
				{
					// oops, have no card to lead from this hand
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_POSTPONE;
				}
				//
				pPlayCard = bPlayingInHand? playerSuit.GetBottomCard(): dummySuit.GetBottomCard();
				status << "PLDRP11! Lead low with the " & pPlayCard->GetName() &
						  " from " & (bPlayingInHand? "hand" : "dummy") & " up to the " &
						  m_pConsumedCard->GetFaceName() & " in " &
						  (bPlayingInHand? "dummy" : "hand") & m_strDropMessage & ".\n";
			}
			// all went OK
			m_nStatusCode = PLAY_IN_PROGRESS;
			break;


		case 1:
			// playing second -- check if the correct suit was led
			if (nSuitLed != m_nSuit)
			{
				// wrong suit led, so no point here
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}

			// LHO at least led the correct suit, so try the drop
			// see if it's aimed at either RHO or both
			if ((m_nTargetOpponent != AGAINST_RHO) && (m_nTargetOpponent != AGAINST_EITHER))
			{
				// no can do
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}

			// see if LHO dropped a key card
			pLHOCard = pDOC->GetCurrentTrickCardByOrder(0);
			if (m_pEnemyOrKeyCardsList->HasCard(pLHOCard))
				bLHODropped = TRUE;

			// see if this is the correct hand for the cash
			if ( ((bPlayingInHand) && (m_nTargetHand == IN_HAND)) ||
				 ((!bPlayingInHand) && (m_nTargetHand == IN_DUMMY)) )
			{
				pPlayCard = m_pConsumedCard;
				if (bLHODropped)
					status << "PLDRP50! " & strLHO & " led the " & pCardLed->GetName() &
							  "; capture it with the " & pPlayCard->GetFaceName() &
							  " in " & (bPlayingInHand? "hand" : "dummy") & ".\n";
				else
					status << "PLDRP51! " & strLHO & " led the " & pCardLed->GetName() &
							  "; try to drop the " & m_strDropCards & " from " & strRHO &
							  "by playing the "& pPlayCard->GetFaceName() & " from " & (bPlayingInHand? "hand" : "dummy") & ".\n";
				m_nStatusCode = PLAY_IN_PROGRESS;
			}
			else
			{
				// in the opposite hand, so skip the play
				m_nStatusCode = PLAY_POSTPONE;
				return m_nStatusCode;
			}
			break;


		case 2:
			// playing third -- make sure play is in progress
			if (m_nStatusCode != PLAY_IN_PROGRESS)
				return PLAY_INACTIVE;

			// see if the wrong suit was led (which shouldn't happen, 
			// if the play is still active)
			if (nSuitLed != m_nSuit)
			{
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}

			// see if RHO has trumped
			if (bTrumped)
			{
				status << "4PLDRP64! " & strRHO & " has trumped, so abandon the drop play for this round.\n";
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}

			// see if RHO dropped a key card
			pRHOCard = pDOC->GetCurrentTrickCardByOrder(1);
			if (m_pEnemyOrKeyCardsList->HasCard(pRHOCard))
				bDropSucceeded = TRUE;

			// check which hand we're plaing in
			if ( ((bPlayingInHand) && (m_nTargetHand == IN_HAND)) ||
				 ((!bPlayingInHand) && (m_nTargetHand == IN_DUMMY)) )
			{
				// cashing from hand, so do it
				pPlayCard = m_pConsumedCard;
				if (bDropSucceeded)
					status << "PLDRP60! " & strRHO & " drped the " & pRHOCard->GetName() & 
							  " under our " & pCardLed->GetFaceName() & " as hoped; rein it in with the " & 
							  pPlayCard->GetName() & " in " & ((bPlayingInHand)? "hand" : "dummy") & ".\n";
				else
					status << "PLDRP61! " & strRHO & " did not drop a desired card (" & m_strDropCards & 
							  "), but perhaps " & strLHO & " might, so continue and play the " & 
							  pPlayCard->GetName() & " from " & ((bPlayingInHand)? "hand" : "dummy") & 
							  " and hope " & strLHO & " drops " & m_strDropCards & ".\n";
			}
			else
			{
				// cash is/was in the other hand, so discard
				pPlayCard = playEngine.GetDiscard();
				if (bDropSucceeded)
					status << "PLDRP64! " & strRHO & " dropped the " & pRHOCard->GetName() & 
							  " under our " & pCardLed->GetFaceName() & " as hoped; now discard the " & pPlayCard->GetName() & 
							  " from " & ((bPlayingInHand)? "hand" : "dummy") & ".\n";
				else
					status << "PLDRP65! " & strRHO & " did not drop a desired card (" & m_strDropCards & 
							  "), but perhaps " & strLHO & " might; continue the play by discarding the " &
							  pPlayCard->GetName() & " from " & ((bPlayingInHand)? "hand" : "dummy") & ".\n";
			}
			// all's OK
			m_nStatusCode = PLAY_COMPLETE;
			break;


		case 3:
			// playing fourth -- make sure the play is in progress
			// play was started by LHO, and we hoped to drop RHO's card
			if (m_nStatusCode != PLAY_IN_PROGRESS)
				return PLAY_INACTIVE;

			// see if we succeeded -- i.e., RHO dropped the desired card
			pRHOCard = pDOC->GetCurrentTrickCardByOrder(2);
			if (m_pEnemyOrKeyCardsList->HasCard(pRHOCard))
				bDropSucceeded = TRUE;

			// if the play ends here, cash if the drop worked
			if ( ((bPlayingInHand) && (m_nTargetHand == IN_HAND)) ||
				 ((!bPlayingInHand) && (m_nTargetHand == IN_DUMMY)) )
			{
/*
 * shouldn't be here!
				if (bDropSucceeded)
				{
					// yup, the card dropped, so rein it in
					pPlayCard = m_pConsumedCard;
					status << "PLDRP80! " & strRHO & " dropped the " & pRHOCard->GetName() &
							  ", so finish the drop play with the " & m_pConsumedCard->GetName() & ".\n";
					m_nStatusCode = PLAY_COMPLETE;
					return m_nStatusCode;
				}
				else
				{
					// the drop did not work
					status << "PLDRP81! " & strRHO & " did not drop the " & pRHOCard->GetName() &
							  " as hoped for, so forget the drop play.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_POSTPONE;
				}
*/			
				m_nStatusCode = PLAY_POSTPONE;
				return m_nStatusCode;
			}
			// else we cashed from the other hand, so discard here
			pPlayCard = playEngine.GetDiscard();
			if (bDropSucceeded)
			{
				// yup, the card dropped, so rein it in
				status << "PLDRP90! " & strRHO & " dropped the " & pRHOCard->GetName() &
						  ", so finish the drop play and discard the " & pPlayCard->GetName() & 
						  " from " & (bPlayingInHand? "hand" : "dummy") & ".\n";
			}
			else
			{
				// the drop did not work
				status << "4PLDRP81! The play failed, as " & strRHO & " did not drop the " & pRHOCard->GetName() &
						  " as hoped for -- discard the " & pPlayCard->GetName() & 
						  " from " & (bPlayingInHand? "hand" : "dummy") & ".\n";
				m_nStatusCode = PLAY_INACTIVE;
				return PLAY_POSTPONE;
			}
			//
			m_nStatusCode = PLAY_COMPLETE;
			break;

	}

	// done
	ASSERT(pPlayCard->IsValid());
	return m_nStatusCode;
}

