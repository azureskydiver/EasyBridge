//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//===========================================================
//
// CGuessedCardHoldings
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "GuessedCardHoldings.h"
#include "Card.h"
#include "CardList.h"


//
//--------------------------------------------------------------
//
// Construction/Destruction
//

// constructor
CGuessedCardHoldings::CGuessedCardHoldings()
{
	// clear all variables
	for(int i=0;i<MAXHOLDING;i++)
		m_cards[i] = NULL;
	m_bAutoSort = TRUE;
	m_bReverseSort = FALSE;
	Init();
}

// destructor
CGuessedCardHoldings::~CGuessedCardHoldings()
{
	Clear();
}


//
//--------------------------------------------------------------
//

//
void CGuessedCardHoldings::Init()
{
	// total cards = definite cards + likely cards
	m_numDefiniteCards = 0;
	m_numLikelyCards = 0;
	m_numTotalCards = 0;
	//
	m_numPlayedCards = 0;
	m_numRemainingCards = 0;
	m_numMinRemainingCards = 0;
	m_numMaxRemainingCards = 0;
	m_numOriginalCards = 0;
	m_numMinOriginalCards = 0;
	m_numMaxOriginalCards = 0;
	//
	m_bSorted = FALSE;
}

//
void CGuessedCardHoldings::Clear(BOOL bDelete)
{
	for(int i=0;i<m_numTotalCards;i++)
	{
		if (bDelete)
			delete m_cards[i];
		m_cards[i] = NULL;
	}
	Init();
}


//
void CGuessedCardHoldings::FormatHoldingsString()
{
	// format the holdings string
	if (m_numTotalCards > 0) 
	{
		m_strHoldings = "";
		for(int i=0;i<m_numTotalCards;i++) 
		{
			m_strHoldings += m_cards[i]->GetName();
			m_strHoldings += " ";
		}
	} 
	else 
	{
		m_strHoldings = "---";
	}
}


//
void CGuessedCardHoldings::Add(CGuessedCard* pCard, BOOL bSort)
{
	if (m_numTotalCards >= MAXHOLDING)
		return;

	// see if we have a duplicate card value
	if (HasCard(pCard->GetDeckValue()))
	{
		// remove and delete the old card
		DeleteByValue(pCard->GetDeckValue());
	}

	// now add
	if ((bSort) || (m_bAutoSort))
	{
		// insert the card in its proper place
		int nValue = pCard->GetDeckValue();
		for (int i=0;i<m_numTotalCards;i++)
		{
			if (!m_bReverseSort)
			{
				if (nValue > m_cards[i]->GetDeckValue())
					break;
			}
			else
			{
				if (m_cards[i]->GetDeckValue() > nValue)
					break;
			}
		}
		// shift rest over
		for(int j=m_numTotalCards;j>i;j--)
			m_cards[j] = m_cards[j-1];
		// and insert
		m_cards[i] = pCard;
		m_bSorted = TRUE;
		// update the count here
		// gotta do this before the holdings string formatting
		m_numTotalCards++;
		FormatHoldingsString();
	}
	else
	{
		// add without sorting
		m_cards[m_numTotalCards] = pCard;
		// update the count here
		m_numTotalCards++;
	}

	// update specific counts
	if (pCard->LocationIdentified())
		m_numDefiniteCards++;
	else
		m_numLikelyCards++;

	// if this is a record of a card play,
	// update the count of cards remaining
	if (pCard->WasPlayed())
	{
		m_numPlayedCards++;
		m_numRemainingCards--;
		m_numMinRemainingCards--;
		m_numMaxRemainingCards--;
		m_numOriginalCards++;
	}
	else
	{
		// the card wasn't played, but we think the player has it
		m_numMinRemainingCards++;
		m_numMaxRemainingCards++;
	}

	// adjust min & max original card count
	m_numMinOriginalCards++;
	m_numMaxOriginalCards++;

	// check limits
	if (m_numMinOriginalCards < 0) 
		m_numMinOriginalCards = 0;
//	if (m_numMinOriginalCards > 13) 
//		m_numMinOriginalCards = 13;
	if (m_numMaxOriginalCards < 0) 
		m_numMaxOriginalCards = 0;
//	if (m_numMaxOriginalCards > 13) 
//		m_numMaxOriginalCards = 13;
	//
	if (m_numMinRemainingCards < 0) 
		m_numMinRemainingCards = 0;
//	if (m_numMinRemainingCards > 13) 
//		m_numMinRemainingCards = 13;
	if (m_numMaxRemainingCards < 0) 
		m_numMaxRemainingCards = 0;
//	if (m_numMaxRemainingCards > 13) 
//		m_numMaxRemainingCards = 13;
}



//
// RemoveByIndex()
//
// this is the main removal routine, called by all others
// derived classes should only redefine this version of the operation
//
CGuessedCard* CGuessedCardHoldings::RemoveByIndex(int nIndex)
{
	if ((nIndex < 0) || (nIndex >= m_numTotalCards))
		return NULL;
	CGuessedCard* pCard = m_cards[nIndex];

	// update count
	if (pCard->LocationIdentified())
		m_numDefiniteCards--;
	else
		m_numLikelyCards--;
	m_numTotalCards--;
	//
	m_numMinOriginalCards--;
	m_numMaxOriginalCards--;
	//
	if (pCard->WasPlayed())
	{
		m_numPlayedCards--;
		m_numRemainingCards++;
		m_numMinRemainingCards++;
		m_numMaxRemainingCards++;
		m_numOriginalCards--;
	}
	else
	{
		m_numMinRemainingCards--;
		m_numMaxRemainingCards--;
	}
	//
	if (m_numDefiniteCards < 0) 
		m_numDefiniteCards = 0;
	if (m_numLikelyCards < 0) 
		m_numLikelyCards = 0;
	if (m_numTotalCards < 0) 
		m_numTotalCards = 0;
	if (m_numPlayedCards < 0) 
		m_numPlayedCards = 0;
	if (m_numMinOriginalCards < 0) 
		m_numMinOriginalCards = 0;
	if (m_numMaxOriginalCards < 0) 
		m_numMaxOriginalCards = 0;
	if (m_numMinRemainingCards < 0) 
		m_numMinRemainingCards = 0;
	if (m_numMaxRemainingCards < 0) 
		m_numMaxRemainingCards = 0;
/*
	ASSERT(m_numDefiniteCards >= 0);
	ASSERT(m_numLikelyCards >= 0);
	ASSERT(m_numTotalCards >= 0);
	ASSERT(m_numPlayedCards >= 0);
	ASSERT(m_numMinOriginalCards >= 0);
	ASSERT(m_numMaxOriginalCards >= 0);
	ASSERT(m_numMinRemainingCards >= 0);
	ASSERT(m_numMaxRemainingCards >= 0);
*/

	// move other cards over
	for(int i=nIndex;i<m_numTotalCards;i++) 
		m_cards[i] = m_cards[i+1];
	for(;i<MAXHOLDING;i++) 
		m_cards[i] = NULL;

	// reform holdings string
	FormatHoldingsString();
	//
	return pCard;
}


//
CGuessedCard* CGuessedCardHoldings::RemoveByValue(int nDeckValue)
{
	VERIFY((nDeckValue >= 0) && (nDeckValue <= 51));
	if (m_numTotalCards <= 0)
		return NULL;
	int nIndex = GetCardIndex(nDeckValue);
	return RemoveByIndex(nIndex);
}



//
void CGuessedCardHoldings::Remove(CGuessedCard* pCard)
{
	VERIFY(pCard != NULL);
	if (m_numTotalCards <= 0)
		return;
	int nIndex = GetCardIndex(pCard);
	RemoveByIndex(nIndex);
}



//
CGuessedCard* CGuessedCardHoldings::Remove(CCard* pCard)
{
	VERIFY(pCard != NULL);
	if (m_numTotalCards <= 0)
		return NULL;
	int nIndex = GetCardIndex(pCard);
	CGuessedCard* pGuessedCard = NULL;
	if (nIndex >= 0)
	{
		pGuessedCard = RemoveByIndex(nIndex);
		return pGuessedCard;
	}
	else
	{
		// not found
		return NULL;
	}
}



//
void CGuessedCardHoldings::DeleteByIndex(int nIndex)
{
	CGuessedCard* pCard = RemoveByIndex(nIndex);
	delete pCard;
}


//
void CGuessedCardHoldings::DeleteByValue(int nDeckValue)
{
	CGuessedCard* pCard = RemoveByValue(nDeckValue);
	delete pCard;
}


//
void CGuessedCardHoldings::Delete(CGuessedCard* pCard)
{
	Remove(pCard);
	delete pCard;
}


//
BOOL CGuessedCardHoldings::Delete(CCard* pCard)
{
	CGuessedCard* pGuessedCard = Remove(pCard);
	if (pCard)
	{
		delete pGuessedCard;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


//
void CGuessedCardHoldings::Merge(CGuessedCardHoldings* pList)
{ 
//	if ((m_numTotalCards + pList->GetNumTotalCards()) > 13)
//		return;
	for(int i=0;i<pList->GetNumDefiniteCards();i++) 
		Add((*pList)[i]); 
/*
	if (m_numMinimumCards > 13)
		m_numMinimumCards = 13;
	if (m_numMaximumCards > 13)
		m_numMaximumCards = 13;
	if (m_numLikelyCards > 13)
		m_numLikelyCards = 13;
*/
	Sort(); 
	//
	FormatHoldingsString();
}



//
void CGuessedCardHoldings::Sort()
{
	if (m_bSorted)
		return;
	//
	CGuessedCard* pTemp;
	int i,j;
	for(i=0;i<m_numTotalCards-1;i++) 
	{
		for(j=0;j<m_numTotalCards-1;j++) 
		{
			if ( ((m_cards[j]->GetDeckValue() < m_cards[j+1]->GetDeckValue()) && (!m_bReverseSort)) ||
				 ((m_cards[j]->GetDeckValue() > m_cards[j+1]->GetDeckValue()) && (m_bReverseSort)) )
			{
				// simple bubble sort
				pTemp = m_cards[j];	
				m_cards[j] = m_cards[j+1];
				m_cards[j+1] = pTemp;	
			}
		}
	}
	//
	FormatHoldingsString();
}



//
//--------------------------------------------------------------
//
// Misc routines
//


//
BOOL CGuessedCardHoldings::HasCard(CGuessedCard* pCard) const
{
	VERIFY(pCard != NULL);
	return HasCard(pCard->GetDeckValue());
}

//
BOOL CGuessedCardHoldings::HasCard(int nDeckValue) const
{
	// searches for a card of a specific suit and face value
	if (m_numTotalCards == 0) return FALSE;
	if (m_bSorted)
	{
		for(int i=0;i<m_numTotalCards;i++)
		{
			// change this later to a binary search
			if (m_cards[i]->GetDeckValue() == nDeckValue)
				return TRUE;
			if (m_cards[i]->GetDeckValue() < nDeckValue)
				return FALSE;	// passed where it should have been
		}
	}
	else
	{
		// do a simple linear search
		for(int i=0;i<m_numTotalCards;i++)
			if (m_cards[i]->GetDeckValue() == nDeckValue)
				return TRUE;
	}
	//
	return FALSE;
}

//
BOOL CGuessedCardHoldings::HasCardOfFaceValue(int nFaceValue) const
{
	// searches for a card of the face value, any suit
	if (m_numTotalCards == 0) return FALSE;
	if (m_bSorted)
	{
		// do a linear search
		for(int i=0;i<m_numTotalCards;i++)
		{
			// maybe change this later to a binary search
			if (m_cards[i]->GetFaceValue() == nFaceValue)
				return TRUE;
		}
	}
	else
	{
		// do a simple linear search
		for(int i=0;i<m_numTotalCards;i++)
			if (m_cards[i]->GetFaceValue() == nFaceValue)
				return TRUE;
	}
	//
	return FALSE;
}


//
// return card index if it's in the holding, else return -1
//
int CGuessedCardHoldings::GetCardIndex(int nDeckValue) const
{
	if (m_numTotalCards == 0) return FALSE;
	// do a linear search
	for(int i=0;i<m_numTotalCards;i++)
		if (m_cards[i]->GetDeckValue() == nDeckValue)
			return i;
	return -1;
}

//
int CGuessedCardHoldings::GetCardIndex(CGuessedCard* pCard) const
{
	VERIFY(pCard != NULL);
	return GetCardIndex(pCard->GetDeckValue());
}

//
int CGuessedCardHoldings::GetCardIndex(CCard* pCard) const
{
	VERIFY(pCard != NULL);
	return GetCardIndex(pCard->GetDeckValue());
}



///////////////////////////////////////////////////////////////////
//
// Utils
//
///////////////////////////////////////////////////////////////////



//
// GetPlayedCardsList()
//
int CGuessedCardHoldings::GetPlayedCardsList(CGuessedCardHoldings& cardList)
{
	for(int i=0;i<m_numTotalCards;i++)
	{
		if (m_cards[i]->WasPlayed())
			cardList << m_cards[i];
	}
	cardList.Sort();
	int numCards = cardList.GetNumTotalCards();
	VERIFY(numCards == m_numPlayedCards);
	return numCards;
}


//
// GetIdentifiedCardsList()
//
int CGuessedCardHoldings::GetIdentifiedCardsList(CGuessedCardHoldings& cardList)
{
	for(int i=0;i<m_numTotalCards;i++)
	{
		if (m_cards[i]->LocationIdentified())
			cardList << m_cards[i];
	}
	cardList.Sort();
	int numCards = cardList.GetNumTotalCards();
	VERIFY(numCards == m_numDefiniteCards);
	return numCards;
}


//
// GetRemainingCardsList()
//
int CGuessedCardHoldings::GetRemainingCardsList(CGuessedCardHoldings& cardList)
{
	// remaining cards are identified cards that have not been played yet
	for(int i=0;i<m_numTotalCards;i++)
	{
		if (m_cards[i]->LocationIdentified() && !m_cards[i]->WasPlayed())
			cardList << m_cards[i];
	}
	cardList.Sort();
	int numCards = cardList.GetNumTotalCards();
	VERIFY(numCards == (m_numDefiniteCards - m_numPlayedCards));
	return numCards;
}

