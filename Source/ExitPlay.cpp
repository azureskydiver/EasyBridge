//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// ExitPlay.cpp
//
#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "Deck.h"
#include "Card.h"
#include "ExitPlay.h"
#include "PlayEngine.h"
#include "CombinedHoldings.h"
#include "CardLocation.h"
#include "GuessedHandHoldings.h"
#include "CombinedSuitHoldings.h"
#include "PlayerStatusDialog.h"
#include "DeclarerPlayEngine.h"


//
//==================================================================
// constructon & destruction

CExitPlay::CExitPlay(CPlayList* pPrerequisites, int nSuit1, int nSuit2) :
CPlay(CPlay::EXIT, CPlay::IN_EITHER, nSuit1, CPlay::PP_LOSER)
{
	m_nSuit2 = nSuit2;
	m_pPrerequisiteList = pPrerequisites;
}

CExitPlay::~CExitPlay() 
{
}


//
void CExitPlay::Clear()
{
	CPlay::Clear();
	//
	m_nSuit2 = NONE;
}


//
void CExitPlay::Init()
{
	CPlay::Init();
	//
	m_strName = "Exit Play";
	// form description
	if (ISSUIT(m_nSuit2))
		m_strDescription.Format("Exit with a %s or %s.", STSS(m_nSuit), STS(m_nSuit2));
	else
		m_strDescription.Format("Exit with a %s.", STSS(m_nSuit));
}


//
CString CExitPlay::GetFullDescription()
{
	CString strText;
	if (ISSUIT(m_nSuit2))
		strText.Format("Exit with a %s or %s and throw the opponents into the lead.", STSS(m_nSuit), STSS(m_nSuit2));
	else
		strText.Format("Exit with a %s and throw the opponents into the lead.", STSS(m_nSuit));

	// done
	return strText;
}




//
// Perform()
//
PlayResult CExitPlay::Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
						      CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, 
						      CPlayerStatusDialog& status, CCard*& pPlayCard)
{
	// exit with a card of the specified suit, or with one of the two
	// specified suits
	// check which hand this is
	CPlayer* pPlayer = playEngine.GetPlayer();
	CDeclarerPlayEngine& declarerEngine = (CDeclarerPlayEngine&) playEngine;
	BOOL bPlayingInHand = (pDOC->GetCurrentPlayer() == pPlayer);
	CHandHoldings& playerHand = *(combinedHand.GetPlayerHand());
	CHandHoldings& dummyHand = *(combinedHand.GetPartnerHand());
	CString strRHO = PositionToString(GetPrevPlayer(playEngine.GetPlayerPosition()));
	CCard* pCardLed = pDOC->GetCurrentTrickCardByOrder(0);
	int nSuitLed = pCardLed? pCardLed->GetSuit() : NONE;
	//
	BOOL bPlayViable = FALSE;

	// perform basic tests
	if (!CPlay::IsPlayUsable(combinedHand, playEngine))
	{
		m_nStatusCode = PLAY_INACTIVE;
		return PLAY_POSTPONE;
	}

	//
	int nOrdinal = pDOC->GetNumCardsPlayedInRound();
	switch(nOrdinal)
	{
		case 1:
		case 3:
			// play cannot be used in the second or fourth positions 
			m_nStatusCode = PLAY_INACTIVE;
			return PLAY_NOT_VIABLE;

		case 0:
			// leading this round
			if (bPlayingInHand) 
			{
				// leading from declarer's hand
				CCombinedSuitHoldings& suit = combinedHand.GetSuit(m_nSuit);
				if (suit.GetNumDeclarerLosers() >= 1)
				{
					// see if we have two suits to discard in
					if (ISSUIT(m_nSuit2))
					{
						// OK, so if dummy has losers in the second suit, we' reset
						CCombinedSuitHoldings& suit2 = combinedHand.GetSuit(m_nSuit2);
						if (suit2.GetNumDummyLosers() >= 1)
							bPlayViable = TRUE;
					}
					else
					{
						// only one suit specified, so it's OK
						bPlayViable = TRUE;
					}
					// pull the bottom card fom the suit
					if (bPlayViable)
						pPlayCard = suit.GetDeclarerSuit().GetBottomCard();
				}
				else
				{
					// oops, no losers in suit #1; can we lead from suit #2?
					if (ISSUIT(m_nSuit2))
					{
						// this only works if we have a loser in suit #2
						// and dummy has a losers in suit #1
						CCombinedSuitHoldings& suit2 = combinedHand.GetSuit(m_nSuit2);
						if ((suit2.GetNumDeclarerLosers() >= 1) &&
											(suit.GetNumDummyLosers() >= 1))
						{
							bPlayViable = TRUE;
							pPlayCard = suit2.GetDeclarerSuit().GetBottomCard();
						}
					}
				}
				//
				if (bPlayViable)
				{
					status << "PLEXT1! Start the exit play by leading  the " & 
							  pPlayCard->GetName() & " from hand.\n";
					m_nStatusCode = PLAY_IN_PROGRESS;
					return m_nStatusCode;
				}
				else
				{
					// we can't lead from hand 
					status << "4PLEXT5! Oops, want to exit with a " & STS(m_nSuit) &
							  " from hand, but we have no losers in the suit, so we have to skip this play.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_NOT_VIABLE;
				}
			}
			else
			{
				// leading from dummy's hand
				CCombinedSuitHoldings& suit = combinedHand.GetSuit(m_nSuit);
				if (suit.GetNumDummyLosers() >= 1)
				{
					// see if we have two suits to discard in
					if (ISSUIT(m_nSuit2))
					{
						// OK, so if declarer has losers in the second suit, we' reset
						CCombinedSuitHoldings& suit2 = combinedHand.GetSuit(m_nSuit2);
						if (suit2.GetNumDeclarerLosers() >= 1)
							bPlayViable = TRUE;
					}
					else
					{
						// only one suit specified, so it's OK
						bPlayViable = TRUE;
					}
					// pull the bottom card fom the suit
					if (bPlayViable)
						pPlayCard = suit.GetDummySuit().GetBottomCard();
				}
				else
				{
					// oops, no losers in suit #1; can we lead from suit #2?
					if (ISSUIT(m_nSuit2))
					{
						// this only works if we have a loser in suit #2
						// and declarer has a losers in suit #1
						CCombinedSuitHoldings& suit2 = combinedHand.GetSuit(m_nSuit2);
						if ((suit2.GetNumDummyLosers() >= 1) &&
											(suit.GetNumDeclarerLosers() >= 1))
						{
							bPlayViable = TRUE;
							pPlayCard = suit2.GetDummySuit().GetBottomCard();
						}
					}
				}
				//
				if (bPlayViable)
				{
					status << "PLEXT7! Start the exit play by leading  the " & 
							  pPlayCard->GetName() & " from dummy.\n";
					m_nStatusCode = PLAY_IN_PROGRESS;
					return m_nStatusCode;
				}
				else
				{
					// we can't lead from hand 
					status << "4PLEXT9! Oops, want to exit with a " & STS(m_nSuit) &
							  " from dummy, but we have no losers in the suit, so we have to skip this play.\n";
					m_nStatusCode = PLAY_INACTIVE;
					return PLAY_NOT_VIABLE;
				}
			}
			break;


		case 2 : 
			// playing 3rd -- discard a card form the second suit, if specified, 
			// else discard any loser
			if (bPlayingInHand) 
			{
				// playing in hand
				// first see if we have to follow suit
				if (playerHand.GetNumCardsInSuit(nSuitLed) > 0)
				{
					pPlayCard = playerHand.GetSuit(nSuitLed).GetBottomCard();
				}
				else if (ISSUIT(m_nSuit2))
				{
					// two exit suits were specified, and we don't need to 
					// immediately follow suit; so see which suit was led
					if ((nSuitLed == m_nSuit) || (playerHand.GetNumCardsInSuit(m_nSuit) == 0))
					{
						// first suit was led; discard a card of the second suit now
						pPlayCard = playerHand.GetSuit(m_nSuit2).GetBottomCard();
					}
					else
					{
						// second suit was led; discard a card of the first suit now
						pPlayCard = playerHand.GetSuit(m_nSuit).GetBottomCard();
					}
				}
				else
				{
					// else only one suit was specified, and we're void in it 
					// (see test #1), so else discard generically
					pPlayCard = playerHand.GetDiscard();
				}
				//
				status << "PLEXT20! Complete the exit play by discarding the " & pPlayCard->GetName() &
						  " from hand.\n";
			}
			else
			{
				// playing in dummy
				// first see if we have to follow suit
				if (dummyHand.GetNumCardsInSuit(nSuitLed) > 0)
				{
					pPlayCard = dummyHand.GetSuit(nSuitLed).GetBottomCard();
				}
				else if (ISSUIT(m_nSuit2))
				{
					// two exit suits were specified, and we don't need to 
					// immediately follow suit; so see which suit was led
					if ((nSuitLed == m_nSuit) || (dummyHand.GetNumCardsInSuit(m_nSuit) == 0))
					{
						// first suit was led; discard a card of the second suit now
						pPlayCard = dummyHand.GetSuit(m_nSuit2).GetBottomCard();
					}
					else
					{
						// second suit was led; discard a card of the first suit now
						pPlayCard = dummyHand.GetSuit(m_nSuit).GetBottomCard();
					}
				}
				else
				{
					// else only one suit was specified, and we're void in it 
					// (see test #1), so else discard generically
					pPlayCard = dummyHand.GetDiscard();
				}
				//
				status << "PLEXT21! Complete the exit play by discarding the " & pPlayCard->GetName() &
						  " from dummy.\n";
			}
			// mark play status
			m_nStatusCode = PLAY_COMPLETE;
			break;
	}

	// done
	ASSERT(pPlayCard->IsValid());
	return m_nStatusCode;

}

