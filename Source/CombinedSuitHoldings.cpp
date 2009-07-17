//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//===========================================================
//
// CCombinedSuitHoldings
//
// this subclass of CSuitHoldinss is used to combines uit holdings 
// of the declarer and dummy
//
//===========================================================

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "CombinedSuitHoldings.h"
#include "GuessedCardHoldings.h"
#include "GuessedSuitHoldings.h"
#include "GuessedHandHoldings.h"
#ifdef _DEBUG
#include "PlayerStatusDialog.h"  // NCR-587
#endif


//
//--------------------------------------------------------------
//
// Construction/Destruction
//

// constructor
CCombinedSuitHoldings::CCombinedSuitHoldings()
{
}

// destructor
CCombinedSuitHoldings::~CCombinedSuitHoldings()
{
}



//
//--------------------------------------------------------------
//
// Misc routines
//

//
void CCombinedSuitHoldings::Init()
{
}


//
void CCombinedSuitHoldings::Clear()
{
	CSuitHoldings::Clear();
	//
	m_declarerCards.Clear();
	m_dummyCards.Clear();

	// clear variables
	m_nDeclarerLength = 0;
	m_nDummyLength = 0;
	m_nMinLength = 0;
	m_nMaxLength = 0;
	m_nLongHand = -1;
	m_pLongHand = NULL;
	m_numMaxWinners = 0;
	m_numMaxLikelyWinners = 0;
	m_numMaxTopCards = 0;
	m_numDeclarerWinners = 0;
	m_numDummyWinners = 0;
	m_numExcessWinners = 0;
	m_numDeclarerExcessWinners = 0;
	m_numDummyExcessWinners = 0;
	m_numDeclarerLosers = 0;
	m_numDummyLosers = 0;
	m_numDeclarerTopCards = 0;
	m_numDummyTopCards = 0;
	m_numOutstandingCards = 0;
	//
	m_winners.Clear();
	m_losers.Clear();
	m_declarerWinners.Clear();
	m_declarerLosers.Clear();
	m_dummyWinners.Clear();
	m_dummyLosers.Clear();

	// set base class flag
	m_bEnableSuitSorting = FALSE;

	//
	m_numDeclarerEntries = 0;
	m_numDummyEntries = 0;
	//
	for(int i=0;i<13;i++)
		m_nMissingCards[i] = NONE;
}



//
void CCombinedSuitHoldings::SetSuit(int nSuit) 
{ 
	CSuitHoldings::SetSuit(nSuit);
	m_declarerCards.SetSuit(nSuit); 
	m_dummyCards.SetSuit(nSuit);
}



//
void CCombinedSuitHoldings::FormatHoldingsString()
{
	// format the holdings string
	int i;
	TCHAR sz1[2];
	if (m_numCards > 0) 
	{
		m_strHoldings = "";
		// first list the cominbed holdings
		for(i=0;i<m_numCards;i++) 
		{
			wsprintf(sz1,"%c",m_cards[i]->GetCardLetter());
			m_strHoldings += sz1;
		}
		// then the dummy's & player's respective holdings
		m_strHoldings += " (";
		for(/* NCR-FFS int*/ i=0;i<m_dummyCards.GetLength();i++) 
		{
			wsprintf(sz1,"%c",m_dummyCards[i]->GetCardLetter());
			m_strHoldings += sz1;
		}
		m_strHoldings += "/";
		for(i=0;i<m_declarerCards.GetLength();i++) 
		{
			wsprintf(sz1,"%c",m_declarerCards[i]->GetCardLetter());
			m_strHoldings += sz1;
		}
		m_strHoldings += ")";
	} 
	else 
	{
		m_strHoldings = "---";
	}

	//
	FormatGIBHoldingsString();
}


//
void CCombinedSuitHoldings::Add(CCard* pCard, const BOOL bSort)
{
	// shouldn't be using this version of the add operation
	// with this class!!!
	ASSERT(FALSE);
}


//
// AddFromSource()
//
// - this is called upon a card/trick undo to add the card to both
//   the combined suit and the proper declarer/dummy card lists
//
void CCombinedSuitHoldings::AddFromSource(CCard* pCard, const BOOL bPlayerCard, const BOOL bSort)
{
	CSuitHoldings::Add(pCard, bSort);
	if (bPlayerCard)
	{
		m_declarerCards.Add(pCard, bSort);
		m_nDeclarerLength = m_declarerCards.GetLength();
	}
	else
	{
		m_dummyCards.Add(pCard, bSort);
		m_nDummyLength = m_dummyCards.GetLength();
	}
	//
	CompareHands();
	FormatHoldingsString();
}


//
// RemoveFromSource()
//
// - this is called when a card is pulled form the combined hand for a play
//
void CCombinedSuitHoldings::RemoveFromSource(CCard* pCard, const BOOL bPlayerCard)
{
	CSuitHoldings::Remove(pCard);
	if (bPlayerCard)
	{
		m_declarerCards.Remove(pCard);
		m_nDeclarerLength = m_declarerCards.GetLength();
	}
	else
	{
		m_dummyCards.Remove(pCard);
		m_nDummyLength = m_dummyCards.GetLength();
	}
	//
	CompareHands();
	FormatHoldingsString();
}



//
CCard* CCombinedSuitHoldings::RemoveByIndex(int nIndex)
{
	// call base class operation
	CCard* pCard = CSuitHoldings::RemoveByIndex(nIndex);

	// perform additional processing
	if (m_declarerCards.HasCard(pCard))
	{
		m_declarerCards.Remove(pCard);
		m_nDeclarerLength = m_declarerCards.GetLength();
	}
	else 
	{
		m_dummyCards.Remove(pCard);
		m_nDummyLength = m_dummyCards.GetLength();
	}
	//
	CompareHands();
	FormatHoldingsString();
	//
	return pCard;
}


//
void CCombinedSuitHoldings::CompareHands()
{
	m_nMaxLength = MAX(m_nDeclarerLength, m_nDummyLength);
	m_nMinLength = MIN(m_nDeclarerLength, m_nDummyLength);
	if (m_nDeclarerLength >= m_nDummyLength)
	{
		m_nLongHand = 0;
		m_pLongHand = &m_declarerCards;
	}
	else
	{
		m_nLongHand = 1;
		m_pLongHand = &m_dummyCards;
	}
}


//
void CCombinedSuitHoldings::Sort()
{
	CSuitHoldings::Sort();
	m_declarerCards.Sort();
	m_dummyCards.Sort();
}






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
//----------------------------------------------------------
//
// CountPoints()
//
// called right after the deal to count points in the hand
//
double CCombinedSuitHoldings::CountPoints(const BOOL bForceCount)
{
	//
	// first call the base class
	//
	CSuitHoldings::CountPoints(bForceCount);
	m_declarerCards.CountPoints(bForceCount);
	m_dummyCards.CountPoints(bForceCount);

	// now erase certain inappropriate variables
	m_nStrength = SS_NONE;

/*  NCR-416 remove here. Missing cards are set in CheckKeyHoldings below 
	// and form the list of missing cards
	int i,nIndex,nVal;
	for(i=0,nIndex=0,nVal=ACE;nVal>=2;nVal--)
	{
		if ((i >= m_numCards) || (m_cards[i]->GetFaceValue() != nVal))
			m_nMissingCards[nIndex++] = nVal;
		else
			i++;
	}
	m_numOutstandingCards = 13 - m_numCards;
	VERIFY(nIndex == m_numOutstandingCards);
*/
	// done
	return m_numTotalPoints; // NCR where is this computed ???
}





//
// CheckKeyHoldings()
//
// check key cards and form sequences
//
int CCombinedSuitHoldings::CheckKeyHoldings()
{
	// first call base class
	CSuitHoldings::CheckKeyHoldings();

	// then for the members
	m_declarerCards.CheckKeyHoldings();
	m_dummyCards.CheckKeyHoldings();

	// get the list of cards played in this suit
	CGuessedCardHoldings playedCardsList;
	int numCardsPlayed = GetCardsPlayedInSuit(playedCardsList);

	// reset count of outstanding cards
	m_numOutstandingCards = 0;
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<13;i++)
		m_nMissingCards[i] = NONE;
	//
	for(i=ACE;i>=2;i--)
	{
		// a card is considered "outstanding" if it's not in our (combined)
		// hand and has not been played yet
		if ((!HasCardOfFaceValue(i)) && (!playedCardsList.HasCardOfFaceValue(i)))
		{
			m_nMissingCards[m_numOutstandingCards] = i;
			m_numOutstandingCards++;
		}
	}
	ASSERT(m_numOutstandingCards == (13 - m_numCards - numCardsPlayed));

	//
	// and re-determine winners & losers
	//
	m_numLosers = 0;
	m_numLikelyLosers = 0;
//NCR-416	m_numOutstandingCards = 13 - m_numCards;   // NCR why change the value here ???

	// use a different algorithm for finding winners here
	// deduct one winner for each outstanding higher card
	int numWinners = m_nMaxLength;
	int nTheirOffset = 0, nOurOffset = 0;
	int numComparisons = m_nMaxLength;
	// NCR-587 If we lead, opponent will play his lowest if he can not win
	// We'll always move to the next lower card
	// Opponent will move (uses high card) only if he wins, otherwise plays low
	for(i=0;i<numComparisons;i++)
	{
		// see if there are outstanding cards matching up against this one
		if (m_numOutstandingCards > i) 
		{
			// see if their next card is higher than our next card
			if (m_nMissingCards[nTheirOffset] > m_cards[nOurOffset]->GetFaceValue())
			{
				// if so, then deduct a winner and compare their next card 
				// against our current  card
				numWinners--;
				nTheirOffset++;  // Opponent only moves if he wins
			}
			else  // NCR-587 If else is commented out then the count is for our leading (a lower value)
			{
				// else ours is higher, so compare their card against our next card
				// NCR-587 We always move to next card
				nOurOffset++;	// else compare against our next lower card
			}
		}
	}

	int numSureWinners = 0;  // NCR-587 Count winners that can be taken from the top
	// NCR-240 All are winners if we have more top cards than are out
	// Below 'Auto Test'ed at 54%
	if((m_nMaxLength > 0) && ((m_numOutstandingCards < 1) 
		 || ((m_numOutstandingCards <= m_nMaxLength) 
		     && (m_cards[m_numOutstandingCards-1]->GetFaceValue() > m_nMissingCards[0]))) ) 
	{
		numSureWinners = m_nMaxLength; // NCR-240 set to max length
	}
	else
	{
		for(int i=0; i < m_nMaxLength; i++) {
			// Compare our next card to opp's top card
			if(m_cards[i]->GetFaceValue() < m_nMissingCards[0])
				break; // exit loop if our card less
			numSureWinners++; // count our top card as a winner
		} // end for(i)
	} // NCR-587 end getting numSureWinners
	m_numSureWinners = numSureWinners;   // NCR-587 save the count

	m_numWinners = m_numLikelyWinners = numWinners;

	// NCR-321 numLosers can't be greater than number of outstanding cards
//NCR-469	int tempNumLosers =  m_numCards - m_numWinners;
	// NCR-469 Can't have more losers than the longest hand minus number of winners
	int tempNumLosers =  Max(0, m_nMaxLength - m_numWinners);  // NCR-469 use maxLength vs numCards
	if(tempNumLosers > m_numOutstandingCards)
		tempNumLosers = m_numOutstandingCards;  // NCR-321 set to lower
	m_numLosers = m_numLikelyLosers = tempNumLosers; //m_numCards - m_numWinners;
	m_numLosers = Min(m_numLosers, m_nMaxLength); // NCR-469 Can't have more losers than cards

//	ASSERT(m_numLosers <= m_nMaxLength); // NCR-469 Can't have more losers than cards?
	// NCR-469 num losers should be <= num losers in longest hand?
	/* NCR-469 Num winners should be >= top sequence - unless top cards are missing!!!
	if(m_numWinners > 0) { 
		// Following fails for seq2={QJT96} having 2 winners and AK are outstanding
		ASSERT(GetSequence2(0).GetNumCards() <= m_numWinners); // NCR-469
	}  */

	// adjust winner counts for declarer/dummy length
	m_numMaxTopCards = Min(m_numTopCards, m_nMaxLength);
	m_numMaxWinners = Min(m_numWinners, m_nMaxLength);
	m_numMaxLikelyWinners = Min(m_numLikelyWinners, m_nMaxLength);

#ifdef _DEBUG_XXX   // NCR-587 Need to get number of sure tricks  ??? Which player???
	// NOTE need import for this class def
	CPlayerStatusDialog& status = pDOC->GetPlayer(0)->GetStatusDialog();
	CString strText;
	strText.Format("DEBUG3! Suit=%d, m_numWinners=%d,  numO_S_Cards=%d, m_nMaxLength=%d, numSureWinners=%d\n",
		                       m_nSuit, m_numWinners, m_numOutstandingCards, m_nMaxLength, numSureWinners);
	status << strText;
#endif


	// clear lists
	m_winners.Clear();
	m_losers.Clear();
	m_declarerWinners.Clear();
	m_declarerLosers.Clear();
	m_dummyWinners.Clear();
	m_dummyLosers.Clear();

	// determine declarer's & dummy's winners & losers
	m_numDeclarerWinners = 0;
	m_numDummyWinners = 0;
	for(i=0;i<m_numSureWinners;i++)    // NCR-613 Use only sure winners
	{
		m_winners << m_cards[i];
		if (m_declarerCards.HasCard(m_cards[i]))
		{
			m_numDeclarerWinners++;
			m_declarerWinners << m_cards[i];
		}
		else
		{
			m_numDummyWinners++;
			m_dummyWinners << m_cards[i];
		}
	}
	m_numDeclarerLosers = m_nDeclarerLength - m_numDeclarerWinners;
	m_numDummyLosers = m_nDummyLength - m_numDummyWinners;
	ASSERT((m_numDeclarerLosers >= 0) && (m_numDummyLosers >= 0));
//	ASSERT((m_numLosers <= m_numDeclarerLosers) && (m_numLosers <= m_numDummyLosers)); // NCR-469

	// determine excess winners & losers
	// "excess" winners are winners in one hand where there are no cards
	// to discard in the suit in the opposite hand
	m_numExcessWinners = 0;
	m_numDeclarerExcessWinners = 0;
	m_numDummyExcessWinners = 0;
	if (m_numTopCards > m_nMinLength)
	{
		m_numExcessWinners = m_numTopCards - m_nMinLength;
		if (m_nDeclarerLength > m_nDummyLength)
			m_numDeclarerExcessWinners = m_numExcessWinners;
		else
			m_numDummyExcessWinners = m_numExcessWinners;
	}

	//
	m_numDeclarerTopCards = 0;
	m_numDummyTopCards = 0;
	for(i=0;i<m_numTopCards;i++)
	{
		if (m_declarerCards.HasCard(m_cards[i]))
			m_numDeclarerTopCards++;
		else
			m_numDummyTopCards++;
	}

	// fill the losers lists
	int nIndex = m_numWinners;
	for(i=0;i<m_numLosers;i++,nIndex++)
	{
		m_losers << m_cards[nIndex];
		if (m_declarerCards.HasCard(m_cards[nIndex]))
			m_declarerLosers << m_cards[nIndex];
		else
			m_dummyLosers << m_cards[nIndex];
	}

	// check entries
	// improve this code later
	m_numDeclarerEntries = m_declarerCards.GetNumWinners();
	m_numDummyEntries = m_dummyCards.GetNumWinners();

	// done
	playedCardsList.Clear(FALSE);
	return 0;
}





//
//----------------------------------------------------------
//
// EvaluateHoldings()
//
// called to evaluate the strength of this suit
//
void CCombinedSuitHoldings::EvaluateHoldings()
{
	//
	// first call the base class 
	//
//	CSuitHoldings::EvaluateHoldings();
	m_declarerCards.EvaluateHoldings();
	m_dummyCards.EvaluateHoldings();

	// first count the number of likely winners
	// adjusted for a combined holding
	m_numLosers = 0;
	m_numLikelyLosers = 0;
//NCR-416	m_numOutstandingCards = 13 - m_numCards; // NCR what about played cards ???


	// NCR-416 Check that #OS cards matches what is in the missing cards list
	ASSERT((m_numOutstandingCards == 0) || (m_nMissingCards[m_numOutstandingCards-1] != NONE)); // NCR-416
	//
	if (m_numCards == 0) 
	{
		// special case
		m_numWinners = m_numLikelyWinners = 0;
	}	
	else if (m_numCards == 13) 
	{
		// no losers in the suit
		m_numWinners = m_numLikelyWinners = 13;
	}
	else if (m_numCards == 12) 
	{
		// no losers in the suit, unless we're missing the ace
		if (m_cards[0]->GetFaceValue() != ACE) 
			m_numLosers = 1;
		m_numWinners = m_numLikelyWinners = 12 - m_numLosers;
	}
	else if (m_numCards == 11) 
	{
		// missing ace and/or king represents loser
		if (!HasCardOfFaceValue(ACE))
			m_numLosers++;
		if (!HasCardOfFaceValue(KING))
			m_numLosers++;
		m_numWinners = m_numLikelyWinners = 11 - m_numLosers;
	}
	else
	{
		// with 10 or fewer cards, use a more sophisticated winner
		// counting algorithm, counting sure winners & likely winners

		// the winners are: the top cards in the suit (automatic winners)
		//  less the missing top cards (if we don't have any)
		//  plus the number of secondary honors,
		//  less the number of missing internal honors in the tenace
		int numSecondaryWinners = m_numSecondaryHonors - m_numMissingInternalHonors;
		if (numSecondaryWinners < 0)
			numSecondaryWinners = 0;
		m_numWinners = m_numTopHonors - m_numMissingTopHonors + numSecondaryWinners;

		// 
		if (m_numWinners < 0)
			m_numWinners = 0;

		// if we have as many winners as outstanding cards, all
		// remaining cards in the long hand are winners
		// e.g., in AKQxxxxxx (AKxxx/Qxxx), there are _2_ "remaining" cards
		// also count rounds used to force out enemy high cards
		// and see if we have cards left after all that 
		// e.g., with AQJxxxxxx (AQxx/Jxxxx), there are 2 remaining
		// cards after the 2 winners and 1 force round
		int numForceRounds = m_numMissingTopHonors + m_numMissingInternalHonors;
		int numRemainingCards = m_nMaxLength - m_numWinners - numForceRounds;
		if (numRemainingCards < 0)
			numRemainingCards = 0;
		if (((m_numWinners + numForceRounds) >= m_numOutstandingCards) && 
				(numRemainingCards > 0))
		{
			m_numWinners += numRemainingCards;
			m_numLikelyWinners = m_numWinners;
		}
/*
		if ((m_numWinners >= m_numOutstandingCards) && (numRemainingCards > 0))
		{
			m_numWinners += numRemainingCards;
			m_numLikelyWinners = m_numWinners;
		}
*/
		else if ((m_nMaxLength >= 5) && (m_numCards >= 8) && 
					    (m_numWinners >= m_numOutstandingCards-1) && 
						(numRemainingCards > 0))
		{
			// if we have 5 or more cards in the long hand, 8 or more
			// cards combined, and enough winners, 
			// then the remaining cards are likely winners
			m_numLikelyWinners = m_numWinners + numRemainingCards;
		}
		else
		{
			// else the likely winners is the same as the sure winners
			m_numLikelyWinners = m_numWinners;
		}

		// and adjust again
		if (m_numWinners > m_nMaxLength)
			m_numWinners = m_nMaxLength;
		if (m_numLikelyWinners > m_nMaxLength)
			m_numLikelyWinners = m_nMaxLength;
		m_numLosers = m_nMaxLength - m_numWinners;
		if (m_numLosers < 0)
			m_numLosers = 0;
		m_numLikelyLosers = m_nMaxLength - m_numLikelyWinners;
	}

	// adjust winner counts for declarer/dummy length
	m_numMaxTopCards = Min(m_numTopCards, m_nMaxLength);
	m_numMaxWinners = Min(m_numWinners, m_nMaxLength);
	m_numMaxLikelyWinners = Min(m_numLikelyWinners, m_nMaxLength);

#ifdef _DEBUG_XXX   // NCR-587 Need to get number of sure tricks  ??? Which player???
	// NOTE need import for this class def
	CPlayerStatusDialog& status = pDOC->GetPlayer(0)->GetStatusDialog();
	CString strText;
	strText.Format("DEBUG4! Suit=%d, m_numWinners=%d,  numO_S_Cards=%d, m_nMaxLength=%d\n",
		                       m_nSuit, m_numWinners, m_numOutstandingCards, m_nMaxLength);
	status << strText;
#endif

	// clear lists
	m_winners.Clear();
	m_losers.Clear();
	m_declarerWinners.Clear();
	m_declarerLosers.Clear();
	m_dummyWinners.Clear();
	m_dummyLosers.Clear();

	// determine declarer's & dummy's winners & losers
	m_numDeclarerWinners = 0;
	m_numDummyWinners = 0;
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<m_numWinners;i++)
	{
		m_winners << m_cards[i];
		if (m_declarerCards.HasCard(m_cards[i]))
		{
			m_numDeclarerWinners++;
			m_declarerWinners << m_cards[i];
		}
		else
		{
			m_numDummyWinners++;
			m_dummyWinners << m_cards[i];
		}
	}
	m_numDeclarerLosers = m_nDeclarerLength - m_numDeclarerWinners;
	m_numDummyLosers = m_nDummyLength - m_numDummyWinners;
	ASSERT((m_numDeclarerLosers >= 0) && (m_numDummyLosers >= 0));

	// determine excess winners & losers
	// "excess" winners are winners in one hand where there are no cards
	// to discard in the suit in the opposite hand
	m_numExcessWinners = 0;
	m_numDeclarerExcessWinners = 0;
	m_numDummyExcessWinners = 0;
	if (m_numTopCards > m_nMinLength)
	{
		m_numExcessWinners = m_numTopCards - m_nMinLength;
		if (m_nDeclarerLength > m_nDummyLength)
			m_numDeclarerExcessWinners = m_numExcessWinners;
		else
			m_numDummyExcessWinners = m_numExcessWinners;
	}

	//
	m_numDeclarerTopCards = 0;
	m_numDummyTopCards = 0;
	for(i=0;i<m_numTopCards;i++)
	{
		if (m_declarerCards.HasCard(m_cards[i]))
			m_numDeclarerTopCards++;
		else
			m_numDummyTopCards++;
	}

	// fill the losers lists
	int nIndex = m_numWinners;
	for(i=0;i<m_numLosers;i++,nIndex++)
	{
		m_losers << m_cards[nIndex];
		if (m_declarerCards.HasCard(m_cards[nIndex]))
			m_declarerLosers << m_cards[nIndex];
		else
			m_dummyLosers << m_cards[nIndex];
	}

}






//
//----------------------------------------------------------
//
// ReevaluateHoldings()
//
// called to re-evaluate the suit after a card has been played
//
void CCombinedSuitHoldings::ReevaluateHoldings(const CCard* pCard)
{
	// first call base class
	CSuitHoldings::ReevaluateHoldings(pCard);

	// then for the members
	m_declarerCards.ReevaluateHoldings(pCard);
	m_dummyCards.ReevaluateHoldings(pCard);

	// and re-count
	CountPoints(TRUE);

}


