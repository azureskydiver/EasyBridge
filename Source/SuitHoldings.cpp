//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------
/*  NCR Changes:
  08/29/31 - NCR-51 Uncommented computing m_numShortPoints
*/

//===========================================================
//
// CSuitHoldings
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "Deck.h"
#include "Card.h"
#include "SuitHoldings.h"
#include "ConventionSet.h"
#include "GuessedHandHoldings.h"
#include "progopts.h"
#include "Player.h"
#ifdef _DEBUG
#include "PlayerStatusDialog.h"  // NCR-587
#endif


//
//--------------------------------------------------------------
//
// Construction/Destruction
//

// constructor
CSuitHoldings::CSuitHoldings()
{
	// clear all variables
	Clear();
}

// destructor
CSuitHoldings::~CSuitHoldings()
{
	Clear();
/*
	// delete the sequence list and its contents
	for(int i=0;i<m_sequenceList.GetSize();i++)
		delete m_sequenceList[i];
	m_sequenceList.RemoveAll();
	for(i=0;i<m_missingSequenceList.GetSize();i++)
		delete m_missingSequenceList[i];
	m_missingSequenceList.RemoveAll();
	//
	m_honors.Clear();
	m_bodyCards.Clear();
	m_likelyWinners.Clear();
	m_likelyLosers.Clear();
*/
}



//
//--------------------------------------------------------------
//
// Misc routines
//

//
void CSuitHoldings::Init()
{
}


//
void CSuitHoldings::Clear()
{
	CCardHoldings::Clear();

	//
	m_bEnableSuitSorting = TRUE;
//	m_nSuit = -1;
	m_nRank = -1;
	m_nStrength = SS_NONE;
	m_numLengthPoints = 0;
	m_numShortPoints = 0;
	m_numBonusPoints = 0;
	m_numPenaltyPoints = 0;
	m_numTotalPoints = 0;
	//
	m_numTopCards = 0;
	m_numHonors = 0;
	m_numPseudoHonors = 0;
	m_numBodyCards = 0;
	m_nHighestHonor = NONE;
	m_nLowestHonor = NONE;
	m_nLowestPseudoHonor = NONE;
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<5;i++)
		m_nMissingHonors[i] = NONE;
	m_numMissingHonors = 0;
	m_numMissingTopHonors = 0;
	m_numMissingInternalHonors = 0;
	m_numMissingBottomHonors = 0;
	m_numTopHonors = 0;
	m_numSecondaryHonors = 0;

	// clear card lists
	m_honors.Clear();
	m_bodyCards.Clear();
	m_likelyWinners.Clear();
	m_likelyLosers.Clear();

	// clear the sequence list
	m_numSequences = 0;
	for(i=0;i<m_sequenceList.GetSize();i++)
		delete m_sequenceList[i];
	m_sequenceList.RemoveAll();
	// NCR-32 ditto for sequence2
	m_numSequence2s = 0;
	for(i=0;i<m_sequence2List.GetSize();i++)
		delete m_sequence2List[i];
	m_sequence2List.RemoveAll();
	//
	m_numMissingSequences = 0;
	for(i=0;i<m_missingSequenceList.GetSize();i++)
		delete m_missingSequenceList[i];
	m_missingSequenceList.RemoveAll();

	//
	m_bHaveAce = FALSE;
	m_bHaveKing = FALSE;
	//
	m_numQuickTricks = 0.0f;
	m_numStoppers = 0;
	m_numLikelyWinners = 0;
	m_numLikelyLosers = 0;
	m_numWinners = 0;
	m_numLosers = 0;
	//
	m_bSolid = FALSE;
	m_bPreferred = FALSE;
	m_bRebiddable = FALSE;
	m_bSelfSupporting = FALSE;
	m_bSuitStopped = FALSE;
	m_bSuitProbablyStopped = FALSE;
}


//
void CSuitHoldings::FormatHoldingsString()
{
	// format the holdings string
	// don't use the base class implementation
	m_strHoldings.Empty();
	if (m_numCards > 0) 
	{
		for(int i=0;i<m_numCards;i++) 
			m_strHoldings += m_cards[i]->GetCardLetter();
	} 
	else 
	{
		m_strHoldings = "---";
	}

	// and format the compressed holdings string
	// same as above for now, but may change later
	m_strCompressedHoldings.Empty();
	if (m_numCards == 0) 
	{
		m_strCompressedHoldings = "void";
	} 
	else 
	{
		for(int i=0;i<m_numCards;i++) 
			m_strCompressedHoldings += m_cards[i]->GetCardLetter();
	}

	//
	FormatGIBHoldingsString();
}



//
void CSuitHoldings::Add(CCard* pCard, const BOOL bSort)
{
	CCardHoldings::Add(pCard, bSort);
	if ((bSort) || (m_bAutoSort))
	{
		// reassign cards' suit positions
		if (m_bEnableSuitSorting)
		{
			for(int i=0;i<m_numCards;i++) 
				m_cards[i]->SetSuitPosition(i);
		}
	}
//	FormatHoldingsString();
}


//
CCard* CSuitHoldings::RemoveByIndex(const int nIndex)
{
	// call base class operation
	CCard* pCard = CCardHoldings::RemoveByIndex(nIndex);
	// then reset suit indices
	if (m_bEnableSuitSorting)
	{
		for(int i=0;i<m_numCards;i++) 
			m_cards[i]->SetSuitPosition(i);
	}
	FormatHoldingsString();
	//
	return pCard;
}



//
void CSuitHoldings::Sort()
{
	// first call base class
	CCardHoldings::Sort();

	// now reassign cards' suit positions
	if (m_bEnableSuitSorting)
	{
		for(int i=0;i<m_numCards;i++) 
			m_cards[i]->SetSuitPosition(i);
	}
}





/////////////////////////////////////////////////////////////////////
//
// Utilities
//
/////////////////////////////////////////////////////////////////////



//
// GetLowestCardAbove()
//
// returns the lowest card in the suit above the specified card
//
CCard* CSuitHoldings::GetLowestCardAbove(const int nFaceValue) const
{
	if (GetNumCardsAbove(nFaceValue) == 0)
		return NULL;
	for(int i=m_numCards-1;i>=0;i--)
	{
		if (m_cards[i]->GetFaceValue() > nFaceValue)
			return m_cards[i];
	}
	// error!
	ASSERT(FALSE);
	return NULL;
}



//
// GetLowestCardAbove()
//
CCard* CSuitHoldings::GetLowestCardAbove(CCard* pCard) const
{ 
	ASSERT(pCard); 
	ASSERT(pCard->GetSuit() == m_nSuit);
	return GetLowestCardAbove(pCard->GetFaceValue()); 
}

// 
// NCR Get count of cards in missing list vs given card
//

int CSuitHoldings::GetNumMissingBelow(const CCard* pCard) const
{
	return GetNumMissingBelow(pCard->GetFaceValue());
}

int CSuitHoldings::GetNumMissingBelow(const int nFaceValue) const
{
	int nbr = 0;
	for(int i=0; i < m_numMissingSequences; i++) {
		CCardList& nxtSeq = *m_missingSequenceList[i];
		// Check that top card is Below given card value
		if(nxtSeq.GetTopCard()->GetFaceValue() < nFaceValue)
			nbr += nxtSeq.GetNumCards();  // add in nbr cards in this 
	}
	return nbr;
}


int CSuitHoldings::GetNumMissingAbove(const CCard* pCard) const
{
	return GetNumMissingAbove(pCard->GetFaceValue());
}

int CSuitHoldings::GetNumMissingAbove(const int nFaceValue) const
{
	int nbr = 0;
	for(int i=0; i < m_numMissingSequences; i++) {
		CCardList& nxtSeq = *m_missingSequenceList[i];
		// Check that top card is above given card value 
		if(nxtSeq.GetTopCard()->GetFaceValue() > nFaceValue)
			nbr += nxtSeq.GetNumCards();  // add in nbr cards in this 
	}
	return nbr;
}
// NCR end of getting counts of missing cards



//
//=================================================================
//=================================================================
//
// Main routines
//
//=================================================================
//=================================================================
//




//
// CountCards()
//
// called to count basic items in the suit
// separated out from the point count cuz the card count can be done many times 
// during the course of a hand, while the point count is only done once, for bidding
//
void CSuitHoldings::CountCards()
{
	// do a basic honors check 
	m_bHaveAce = FALSE;
	m_bHaveKing = FALSE;
	m_numHonors = 0;
	m_numPseudoHonors = 0;
	//
	for(int i=0;i<m_numCards;i++) 
	{
		// check honors
		if (m_cards[i]->GetFaceValue() == ACE)
			m_bHaveAce = TRUE;
		else if (m_cards[i]->GetFaceValue() == KING)
			m_bHaveKing = TRUE;
		//
		if (m_cards[i]->GetFaceValue() >= JACK)
			m_numHonors++;
		if (m_cards[i]->GetFaceValue() >= TEN)
			m_numPseudoHonors++;
	}
}



//
// CountPoints()
//
// called right after the deal to count points in the hand
//
double CSuitHoldings::CountPoints(const BOOL bForceCount)
{
	// avoid repeating the count unless forced to do so
	if ((m_bCountPerformed) && (!bForceCount))
		return m_numTotalPoints;

	//
	// first call the base class
	//
	CCardHoldings::CountPoints(bForceCount);

	// then sort the suit
	if (!m_bSorted)
		Sort();

	// check key holdings and form sequences
	CheckKeyHoldings();


	//
	// then check distribution
	//

	// count short suit points
    // NCR-51 uncommented the following to set m_numShortPoints value
	if (m_numCards <= 2)
		m_numShortPoints = (3 - m_numCards);  // NCR doubleton=1, singleton=2, void=3


	// count long points if the suit is good 
	// i.e., 2+ honors of KJ or better (i.e., 4+ HCPs)
	if (pCurrConvSet->IsConventionEnabled(tid5CardMajors))
	{
		// if playing 5-card majors, add 1 pt for the sixth card + 2 per extra card
		if ((m_numCards > 5) && (m_numHonors >= 2) && (m_numHCPoints >= 4))
			m_numLengthPoints = 1 + 2*(m_numCards - 6);
	}
	else
	{
		// if playing 4-card majors, 5th card = 1 pts and each add'l = 2 pts
		if ((m_numCards > 4) && (m_numHonors >= 2) && (m_numHCPoints >= 4))
			m_numLengthPoints = 1 + 2*(m_numCards - 5);
	}

	// and set the total points tally
	if (theApp.GetValue(tbCountShortSuits))
		m_numTotalPoints = m_numHCPoints + m_numLengthPoints + m_numShortPoints;
	else
		m_numTotalPoints = m_numHCPoints + m_numLengthPoints;

	// check for unguarded/poorly guarded honors
	m_numPenaltyPoints = 0;
	if (theApp.GetValue(tbPenalizeUGHonors)) 
	{
		//
		if (m_numCards == 1) 
		{
			// singleton King = -3 pts
			if (m_cards[0]->GetFaceValue() == KING)
//				m_numPenaltyPoints += 3;
				m_numPenaltyPoints += 1.5;
			// singletone Queen = -2 pts
			if (m_cards[0]->GetFaceValue() == QUEEN)
//				m_numPenaltyPoints += 2;
				m_numPenaltyPoints += 1;
			// singletone Jack = -1 pts
			if (m_cards[0]->GetFaceValue() == JACK)
//				m_numPenaltyPoints += 1;
				m_numPenaltyPoints += 0.5;
		}
		//
		if ((m_numCards == 2) && (m_cards[1]->GetFaceValue() < TEN))
		{
			// Qx = -2 pts
			if (m_cards[0]->GetFaceValue() == QUEEN)
//				m_numPenaltyPoints += 2;
				m_numPenaltyPoints += 1;
			// Jx = -1 pts
			if (m_cards[0]->GetFaceValue() == JACK)
//				m_numPenaltyPoints += 1;
				m_numPenaltyPoints += 0.5;
		}
		//
		if ((m_numCards == 3) && 
						(m_cards[0]->GetFaceValue() == JACK) &&
						(m_cards[1]->GetFaceValue() < TEN)) 
		{
			// Jxx = -1 pts
//			m_numPenaltyPoints += 1;
			m_numPenaltyPoints += 0.5;
		}
	}

	//
	// now check the suit for QTs' and stoppers
	//
	m_bSuitStopped = FALSE;
	m_bSuitProbablyStopped = FALSE;
	m_numStoppers = 0;
	m_numQuickTricks = 0.0f;
	
	// count stoppers & quick tricks

	// first count quick tricks
	if ((m_numCards >= 2) && 
		(m_cards[0]->GetFaceValue() == ACE) && (m_cards[1]->GetFaceValue() == KING)) 
	{
		// AK == 2 quick tricks & a stopper
		m_numQuickTricks = 2;
		m_numStoppers = 2;
		m_bSuitStopped = TRUE;
		m_bSuitProbablyStopped = TRUE;
	} 
	else if ((m_numCards >= 2) && 
			 (m_cards[0]->GetFaceValue() == ACE) && (m_cards[1]->GetFaceValue() == QUEEN)) 
	{
		// AQ == 1-1/2 quick tricks & a stopper
		m_numQuickTricks = 1.5;
		m_numStoppers = 2;
		m_bSuitStopped = TRUE;
		m_bSuitProbablyStopped = TRUE;
	} 
	else if ((m_numCards >= 1) && (m_cards[0]->GetFaceValue() == ACE)) 
	{
		// A == 1 quick trick & a stopper
		m_numQuickTricks = 1;
		m_numStoppers = 1;
		m_bSuitStopped = TRUE;
		m_bSuitProbablyStopped = TRUE;
	} 
	else if ((m_numCards >= 2) && 
			   (m_cards[0]->GetFaceValue() == KING) && (m_cards[1]->GetFaceValue() == QUEEN)) 
	{
		// KQ == 1 quick trick & a stopper
		m_numQuickTricks = 1;
		m_numStoppers = 2;
		m_bSuitStopped = TRUE;
		m_bSuitProbablyStopped = TRUE;
	} 
	else if ((m_numCards >= 2) && (m_cards[0]->GetFaceValue() == KING)) 
	{
		// Kx == 1/2 quick trick & a stopper
		m_numQuickTricks = 0.5;
		m_numStoppers = 1;
		m_bSuitStopped = TRUE;
		m_bSuitProbablyStopped = TRUE;
	} 
	else if ((m_numCards >= 3) && 
			 (m_cards[0]->GetFaceValue() == QUEEN) && (m_cards[1]->GetFaceValue() == JACK) 
			 && (m_cards[2]->GetFaceValue() == TEN)) 
	{
		// QJT == no quick tricks, but a stopper
		m_numStoppers = 1;  // NCR changed 3 to 1
		m_bSuitStopped = TRUE;
		m_bSuitProbablyStopped = TRUE;
	} 
	else if ((m_numCards >= 3) && (m_cards[0]->GetFaceValue() == QUEEN)) 
	{
		// Qxx = no QT's, but a stopper
		m_numStoppers = 1;
		m_bSuitStopped = TRUE;
		m_bSuitProbablyStopped = TRUE;
	} 
	else if ((m_numCards >= 4) && 
			   (m_cards[0]->GetFaceValue() == JACK) && (m_cards[1]->GetFaceValue() == TEN) &&
			   (m_cards[2]->GetFaceValue() == 9) && (m_cards[3]->GetFaceValue() == 8)) 
	{
		// JT98 = no QTs, but a stopper
		m_numStoppers = 1;   // NCR changed 4 to 1 
		m_bSuitStopped = TRUE;
		m_bSuitProbablyStopped = TRUE;
	} 
	else if ((m_numCards >= 4) && (m_cards[0]->GetFaceValue() == JACK)) 
	{
		// Jxxx = no QTs', but a probable stopper
		m_bSuitProbablyStopped = TRUE;
		m_numStoppers = 1;
	}

	// done
	return m_numTotalPoints;
}




//
// GetCardsPlayedInSuit()
//
// get the list of cards played in this suit
// NCR-83 This list does NOT include cards on the current trick???
//
int CSuitHoldings::GetCardsPlayedInSuit(CGuessedCardHoldings& playedCardsList)
{
	// use SOUTH's knowledge of the cards that have been played (this is kosher)
	// can't use any arbitrary player's info since that may not have been cleared
	// yet by the time this routines is called (a player's guessed hands are cleared,
	// then his hands are initialized using this info, before the next player is processed)
	CGuessedCardHoldings  tempList;
	CPlayer* pPlayer = pDOC->GetPlayer(SOUTH);
	if (pPlayer)
	{
		for(int i=0;i<4;i++)
		{
			CGuessedSuitHoldings& suit = pPlayer->GetGuessedHand(i)->GetSuit(m_nSuit);
			suit.GetIdentifiedCardsList(tempList);
			playedCardsList << &tempList;
			tempList.Clear(FALSE);
		}
	}
	// done
	return playedCardsList.GetNumDefiniteCards();
}




//
// CheckKeyHoldings()
//
// check key cards and form sequences
//
int CSuitHoldings::CheckKeyHoldings()
{
	//
	CountCards();

	// clear existing sequences
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<m_sequenceList.GetSize();i++)
		delete m_sequenceList[i];
	m_sequenceList.RemoveAll();
	m_numSequences = 0;
	// NCR-32 ditto for sequence2
	for(i=0;i<m_sequence2List.GetSize();i++)
		delete m_sequence2List[i];
	m_sequence2List.RemoveAll();
	m_numSequence2s = 0;
	// and missing sequences as well
	for(i=0;i<m_missingSequenceList.GetSize();i++)
		delete m_missingSequenceList[i];
	m_missingSequenceList.RemoveAll();
	m_numMissingSequences = 0;
	//
	m_honors.Clear();
	m_bodyCards.Clear();
	m_likelyWinners.Clear();
	m_likelyLosers.Clear();

	// clear values
	m_numBodyCards = 0;
	m_nHighestHonor = m_nLowestHonor = m_nLowestPseudoHonor = NONE;
	m_numMissingHonors = m_numMissingTopHonors = m_numMissingBottomHonors = m_numMissingInternalHonors = 0;
	m_numTopCards = m_numSecondaryHonors = 0;
	m_numLikelyWinners = 0;
	m_numLikelyLosers = 0;
	m_numWinners = 0;
	m_numLosers = 0;
	m_numTopHonors = 0; // NCR-404 ??? This gets double values some times

	// sort honors & body cards
//	for(i=0;i<m_numHonors;i++)
//		m_honors.Add(m_cards[i]);
	for(i=0;i<m_numPseudoHonors;i++)
		m_honors.Add(m_cards[i]);
	for(;i<m_numCards;i++)
		m_bodyCards.Add(m_cards[i]);
//	m_numBodyCards = m_numCards - m_numHonors;
	m_numBodyCards = m_numCards - m_numPseudoHonors;
	VERIFY(m_numBodyCards == m_bodyCards.GetNumCards());
	//
	if (m_numHonors > 0)
	{
		m_nHighestHonor = m_cards[0]->GetFaceValue();
		m_nLowestHonor = m_cards[m_numHonors-1]->GetFaceValue();
		m_nLowestPseudoHonor = m_cards[m_numPseudoHonors-1]->GetFaceValue();
	}

	// get the list of cards played in this suit
	CGuessedCardHoldings playedCardsList;
	GetCardsPlayedInSuit(playedCardsList);

	// check for cards missing from an AKQJT sequence 
	for(i=0;i<5;i++)
	{
		int nCardVal = ACE - i;
		if (!m_honors.HasCardOfFaceValue(nCardVal))
		{
			// see if the card has been played
			if (!playedCardsList.HasCardOfFaceValue(nCardVal))
			{
				// card is still outstanding
				m_nMissingHonors[m_numMissingHonors] = nCardVal;
				m_numMissingHonors++;
			}
		}
	}
	//
	if (m_numCards > 0) {
		// NCR-454 Use Missing honors vs assuming ACE is out
		if((m_numMissingHonors == 0) || (m_cards[0]->GetFaceValue() > m_nMissingHonors[0]))
			m_numMissingTopHonors = 0;
		else
			m_numMissingTopHonors =  // NCR-454 use top missing card vs ACE  
//			                        Min(5, (m_nMissingHonors[0] - m_cards[0]->GetFaceValue()));
									Min(m_numMissingHonors, m_nMissingHonors[0] - Max(10, m_cards[0]->GetFaceValue())); // NCR-701
	}
	// NCR-454 test against missingHonors list vs TEN
	if (m_nLowestPseudoHonor > m_nMissingHonors[m_numMissingHonors-1])
		m_numMissingBottomHonors = Min(4, m_nLowestPseudoHonor - TEN);

	m_numMissingInternalHonors = m_numMissingHonors - m_numMissingTopHonors - m_numMissingBottomHonors;
	if (m_numMissingInternalHonors < 0)
		m_numMissingInternalHonors = 0;
	//
	int nPrevCardVal = ACE;
	for(i=0;i<m_numCards;i++)
	{
		// consecutive high cards = sure winners
		// e.g., in AKTxx, AK are the top cards
		if (playedCardsList.HasCardOfFaceValue(nPrevCardVal)) 
		{
			// skip cards that have been played
			nPrevCardVal--;
			i--;
			continue;
		}
		if (m_cards[i]->GetFaceValue() == nPrevCardVal)
		{
			m_numTopCards++;
			if (m_cards[i]->GetFaceValue() >= TEN)
				m_numTopHonors++;
			nPrevCardVal--;
		}
	}
	ASSERT(m_numTopCards <= m_numCards);
	m_numSecondaryHonors = m_numPseudoHonors - m_numTopHonors;
	// NCR debug code
	if(m_numPseudoHonors < m_numTopHonors) {
		int x = 0;  // NCR A place to break
	}

	// build the sequence list
	for(i=0;i<m_numCards;)
	{
		CCardList* pList = new CCardList;
		m_sequenceList.Add(pList);
		m_numSequences++;
		do {
			*pList << m_cards[i++];
		} while ((i<m_numCards) && (m_cards[i]->GetFaceValue() == (m_cards[i-1]->GetFaceValue() - 1)));
#ifdef _DEBUG
		pList->FormatHoldingsString();
#endif
	}

	// NCR-32 build the sequence2 list - cards of equivalent values including whats been played
	// Assumes that m_cards[] sorted in descending FV order
	for(i=0;i<m_numCards;)
	{
		CCardList* pList = new CCardList;
		m_sequence2List.Add(pList);
		m_numSequence2s++;
		bool bSearching = true;
		int nxtFV = m_cards[i]->GetFaceValue(); // prime the control
		do {
			*pList << m_cards[i++];  // save this card
			if(i >= m_numCards) 
				break;			// done
			do{
				nxtFV--;   // move to previous card
				if(m_cards[i]->GetFaceValue() == nxtFV)  // in sequence?
					break;  // yes, go save
				if(!playedCardsList.HasCardOfFaceValue(nxtFV))  // has card been played?
					bSearching = false;  // stop search scan if NOT
			} while(bSearching);
		} while (bSearching);
#ifdef _DEBUG
		pList->FormatHoldingsString();
#endif
	} // NCR-32 end for(i) thru cards

	// and form the list of missing card sequences
	for(i=ACE;i>=2;i--)
	{
		if ( (!HasCardOfFaceValue(i)) && 
			 (!playedCardsList.HasCardOfFaceValue(i)) )
		{
			CCardList* pList = new CCardList;
			m_missingSequenceList.Add(pList);
			m_numMissingSequences++;                // NCR-79 added below test for played card 
			while ((i>=2) && !HasCardOfFaceValue(i) && !playedCardsList.HasCardOfFaceValue(i))
			{
				*pList << deck.GetCard(m_nSuit, i);
				i--;
			}
#ifdef _DEBUG
		pList->FormatHoldingsString();
#endif
		}
	}

	// adjust winners & losers counts
	m_numWinners = m_numTopCards;
	int numCardsOutstanding = 13 - m_numCards - playedCardsList.GetNumDefiniteCards();
	if (m_numCards > m_numTopCards)
	{
		// see if we have more cards than those left outstanding
		if (m_numTopCards >= numCardsOutstanding)
			m_numWinners += (m_numCards - m_numTopCards);
	}
	m_numLosers = m_numCards - m_numWinners;

	// adjust likely winners & losers
	m_numLikelyWinners = m_numWinners;
	m_numLikelyLosers = m_numLosers;

	// adjust winner & loser lists
	for(i=0;i<m_numLikelyWinners;i++)
		m_likelyWinners << m_cards[i];
	for(;i<m_numCards;i++)
		m_likelyLosers << m_cards[i];

	// done
	playedCardsList.Clear(FALSE);
	return 0;
}





//
//----------------------------------------------------------
//
// EvaluateHoldings()
//
// called to evaluate the strength of this suit and estimate
// the numbers of winners and losers
//
void CSuitHoldings::EvaluateHoldings()
{
	//
	// first clear variables
	//
	m_bSolid = FALSE;
	m_bRebiddable = FALSE;
	m_bSelfSupporting = FALSE;
	m_bPreferred = FALSE;
	m_numLikelyWinners = 0;
	m_numLikelyLosers = 0;
	m_numWinners = 0;
	m_numLosers = 0;
	m_nRank = -1;

	//
	// first check length of suit
	//
	if (m_numCards == 0) 
	{
		m_nStrength = SS_VOID;		
	} 
	else if (m_numCards < 2) 
	{
		// 2 cards == suit is unusable except opposite 
		// a great holding by partner
		m_nStrength = SS_UNUSABLE;			
	} 
	else if (m_numCards == 2) 
	{
		// 2 cards == poor holding; may be usable opposite a rebid suit
		m_nStrength = SS_POOR;			
	} 
	else if (m_numCards == 3) 
	{
		// if playing 5-card majors, suit is weak to moderate support 
		// with 3 cards
		if (pCurrConvSet->IsConventionEnabled(tid5CardMajors))
		{
			if (m_numHCPoints >= 2)		// Qxx is moderate support
				m_nStrength = SS_MODERATE_SUPPORT;			
			else
				m_nStrength = SS_WEAK_SUPPORT;			
		}
		else
		{
			// else with 4-card majors, the suit is nominally 
			// unusable in the support role unless it has 
			// >= 2 HCPs (a Queen or better)
			if (m_numHCPoints >= QUEEN_VALUE)  // NCR replaced 2 with QUEEN_VALUE
				m_nStrength = SS_WEAK_SUPPORT;
			else if(m_numHCPoints >= JACK_VALUE) // NCR-383 points for 3 cards to Jack
				m_nStrength = SS_POOR; // NCR-383
			else
				m_nStrength = SS_UNUSABLE;			
		}
	} 
	else if (m_numCards == 4) 
	{
		// if all small cards, it's a moderate support suit;
		// else, with 1-3 honor pts, a good support suit
		// & with 4+ honor points, is an openable suit
		// (i.e., it can be opened when playing 4-card majors, 
		//  or bid as a second suit playing 5-card majors)
		if (m_numHCPoints == 0)
			m_nStrength = SS_MODERATE_SUPPORT;			
		else if (m_numHCPoints <= 3)
			m_nStrength = SS_GOOD_SUPPORT;	// == SS_MARGINAL_OPENER
		else
			m_nStrength = SS_OPENABLE;		// == SS_STRONG_SUPPORT
	} 
	else if (m_numCards == 5) 
	{
		// if all small cards, it's a moderate support suit
		// (or a marginal opener);
		// if it has 0-2 pts, it's a good support suit
		// if it has 3-7 pts, it's an openable suit
		// else with 8+ HCPs, it's a strong suit
//		if (m_numHCPoints == 0)
//			m_nStrength = SS_MODERATE_SUPPORT;			
		if ((m_numHCPoints >= 0) && (m_numHCPoints <= 2))
			m_nStrength = SS_GOOD_SUPPORT;			
		else if ((m_numHCPoints >= 3) && (m_numHCPoints <= 7))
			m_nStrength = SS_OPENABLE;			
		else
			m_nStrength = SS_STRONG;			
	} 
	else if (m_numCards == 6) 
	{
		// with 0-4 honor pts, it's an opener
		// with 5-7 honor points, it's a strong suit
		// else with 8+ pts, it's an absolute suit
		if (m_numHCPoints <= 4)
			m_nStrength = SS_OPENABLE;			
		else if ((m_numHCPoints >= 5) && (m_numHCPoints <= 7))
			m_nStrength = SS_STRONG;			
		else
			m_nStrength = SS_ABSOLUTE;			
	} 
	else if (m_numCards >= 7) 
	{
		// with 0-4 honor pts, it's an openable suit
		// with 5-6 honor points, it's a strong suit
		// else with 7+ honor points, it's an absolute suit
		if (m_numHCPoints <= 4)
			m_nStrength = SS_OPENABLE;			
		else if ((m_numHCPoints >= 5) && (m_numHCPoints <= 6))
			m_nStrength = SS_STRONG;			
		else
			m_nStrength = SS_ABSOLUTE;			
	}


	//
	//-------------------------------------------------------------
	//
	// perform some quality test on the suit
	//
	
	// see if the suit is "Solid"
	// need 3 honors with a minumum of AKQ
	// NCR Allow fewer for Agressiveness
	int nReqHCPs = ((theApp.GetBiddingAgressiveness() > 1) ? 7 : 9); // NCR adj for agression
	if ((m_numCards >= 5) && (m_numHCPoints >= nReqHCPs) &&
									(m_numHonors >= 3))
		m_bSolid = TRUE;

	// see if the suit is self-supporting
	// for a suit to be self-supporting, it need either
	// 6+ cards with 4 honors or 
	// 7+ cards with 3 honors
	if ( ((m_numCards >= 6) && (m_numHonors == 4)) ||
		 ((m_numCards >= 7) && (m_numHonors == 3))) 
		m_bSelfSupporting = TRUE;

	// see if the suit is rebiddable
	// any six-card suit or a 5-card suit with 2 honors is rebiddable
	if ((m_numCards >= 6) || 
		((m_numCards == 5) && (m_numHonors >= 2))) 
		m_bRebiddable = TRUE;


	//
	// analyze suit for winners & losers
	//

/*
	//
	// count the "sure"(?) winners -- by counting high cards
	// 
	if (HasCardOfFaceValue(ACE))
		m_numWinners++;
	// holding a King is a "sure" winner if we have 2+ cards in a hand
	if ((HasCardOfFaceValue(KING)) && (m_numCards >= 2))
		m_numWinners++;
	// holding a King is a "sure" winner if we have 3+ cards in a hand
	if ((HasCardOfFaceValue(QUEEN)) && (m_numCards >= 3))
		m_numWinners++;
	// holding a King is a "sure" winner if we have 4+ cards in a hand
	if ((HasCardOfFaceValue(JACK)) && (m_numCards >= 4))
		m_numWinners++;
	m_numLosers = m_numCards - m_numWinners;
*/

	//
	// now count the likely winners & losers by subtracting 
	// for missing key cards
	//
	if (m_numCards == 0) 
	{
		// no winners or losers here
	} 
	else if (m_numCards == 1) 
	{
		// if the card is not an ace, 1 loser
		if (m_cards[0]->GetFaceValue() != ACE) 
			m_numLikelyLosers++;
	} 
	else if (m_numCards == 2) 
	{
		// need AK to have 2 winners
		if (m_cards[0]->GetFaceValue() != ACE) 
		{
			// 1 loser already; check if the top card is a king
			if (m_cards[0]->GetFaceValue() != KING) 
			{
				// the first card is not an Ace, and not a King = 2 losers
				m_numLikelyLosers += 2;
			} 
			else 
			{
				// Kx = 1 loser
				m_numLikelyLosers++;
			}
		} 
		else 
		{
			// first card is an Ace; check the second card
			if (m_cards[1]->GetFaceValue() == KING) 
			{
				// AK = 2 winners
			} 
			else 
			{
				// 2nd card is not a King; 1 loser
				m_numLikelyLosers++;
			}
		}
	} 
	else if (m_numCards < 10) 
	{
		// here, we have 3-13 cards 
		// we may have a lot of cards in the suit if we're looking at a
		// combined holding or a freakish hand
		if (m_numCards == 13) 
		{
			// no losers in the suit
		}
		else if (m_numCards == 12) 
		{
			// no losers in the suit, unless we're missing the ace
			if (m_cards[0]->GetFaceValue() != ACE) 
				m_numLikelyLosers = 1;
		}
		else if (m_numCards == 11) 
		{
			// missing ace and/or king represents loser
			if (!HasCardOfFaceValue(ACE))
				m_numLikelyLosers++;
			if (!HasCardOfFaceValue(KING))
				m_numLikelyLosers++;
		}
		else
		{
			// with 10 or fewer cards, a
			// missing ace, king, or queen each represents a loser
			if (!HasCardOfFaceValue(ACE))
				m_numLikelyLosers++;
			if (!HasCardOfFaceValue(KING))
				m_numLikelyLosers++;
			if (!HasCardOfFaceValue(QUEEN))
				m_numLikelyLosers++;

			// the remaining cards are all winners if we hold 
			// at least one honor; else add 1 loser
			if ((m_numCards >= 4) && (m_numHCPoints == 0)) 
			{
				m_numLikelyLosers++;
			}
			// NCR-260 this seems too liberal
			// Must be more losers if ???
			if((m_numCards < 7) && (GetNumCardsAbove(JACK) <= 2))
			{
				if (!HasCardOfFaceValue(JACK))
					m_numLikelyLosers++;
			} // NCR-260 end
		}
	}

	// and set the winners/losers count
	m_numLikelyWinners = m_numCards - m_numLikelyLosers;
	m_numLosers = m_numLikelyLosers;
	m_numWinners = m_numCards - m_numLosers;

#ifdef _DEBUG_XXX   // NCR-587 Need to get number of sure tricks  ??? Which player???
	// NOTE need import for this class def
	CPlayerStatusDialog& status = pDOC->GetPlayer(0)->GetStatusDialog();
	CString strText;
	strText.Format("DEBUG2! Suit=%d, m_numWinners=%d = m_numCards=%d - m_numLosers %d\n",
		                       m_nSuit, m_numWinners, m_numCards, m_numLosers);
	status << strText;
#endif

	if(m_numWinners < 0) {
		m_numWinners = 0;  //NCR-311 How can this go negative???
    }

	// and for the winners and losers sequences
	m_likelyWinners.Clear();
	m_likelyLosers.Clear();
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<m_numLikelyWinners;i++)
		m_likelyWinners << m_cards[i];
	for(;i<m_numCards;i++)
		m_likelyLosers << m_cards[i];

}





//
//----------------------------------------------------------
//
// ReevaluateHoldings()
//
// called to re-evaluate the suit after a card has been played from our hand
//
void CSuitHoldings::ReevaluateHoldings(const CCard* pCard)
{
	// reset key holdings
	CheckKeyHoldings();

	// re-evaluate suit winners and losers
//	ReevalutateWinners();
}




//
// GetQualityTest()
//
// performs a basic quality test on the suit
//

int CSuitHoldings::GetQualityTest()
{
	return m_numCards + m_numHonors;
}


//
// HasTenAce - test if suit has two honors with gap -  NCR
//
BOOL CSuitHoldings::HasTenAce() const {
	return (GetNumSequence2s() > 1) 
		    // if second sequence starts with an honor, there is a gap
		    && IsHonor(GetSequence2(1).GetTopCard()->GetFaceValue());
}

// GetNumGuardsRequired() - NCR-75
//
// Return number higher outstanding cards
//
int  CSuitHoldings::GetNumGuardsRequired(const CCard* pCard, const CCardList& cardList) const {
	// Is there a way to have an optional cardList?
	return cardList.GetNumCardsAbove(pCard);
}

/*
//
// test whether a suit is "self-supporting"
//
BOOL CSuitHoldings::IsSuitSelfSupporting(int nSuit)
{
	// for a suit to be self-supporting, it need either
	// 6+ cards with 4 honors or 
	// 7+ cards with 3 honors
	if ( ((m_numInSuit[nSuit] >= 6) &&
				(m_numHonorsInSuit[nSuit] == 4)) ||
		 ((m_numInSuit[nSuit] >= 7) &&
				   (m_numHonorsInSuit[nSuit] == 3))) 
		return TRUE;
	else
		return FALSE;
}
*/