//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//===========================================================
//
// CCardList
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "Card.h"
#include "CardList.h"



//
//--------------------------------------------------------------
//
// Construction/Destruction
//

// constructor
CCardList::CCardList()
{
	// clear all variables
	Clear();
	m_bAutoSort = TRUE;
	m_bReverseSort = FALSE;
}

// copy constructor
CCardList::CCardList(const CCardList& srcList)
{
	// clear all variables
	Clear();
	m_bAutoSort = TRUE;
	m_bReverseSort = FALSE;
	// and merge
	Merge(srcList);
}

// destructor
CCardList::~CCardList()
{
}



//
//================================================================
//
// Simple routines
//

//
CCard* CCardList::operator[](const int nIndex) const 
	{ VERIFY(nIndex < m_numCards); return m_cards[nIndex]; }

CCard* CCardList::GetAt(const int nIndex) const 
	{ VERIFY(nIndex < m_numCards); return m_cards[nIndex]; }

CCard* CCardList::GetTopCard() const 
//	{ VERIFY(m_numCards > 0); VERIFY(m_bSorted); return m_cards[0]; }
	{ if (m_numCards == 0) return NULL; VERIFY(m_bSorted); return m_cards[0]; }

CCard* CCardList::GetBottomCard() const 
//	{ VERIFY(m_numCards > 0); VERIFY(m_bSorted); return m_cards[m_numCards-1]; }
	{ if (m_numCards == 0) return NULL; VERIFY(m_bSorted); return m_cards[m_numCards-1]; }

CCard* CCardList::GetSecondHighestCard() const 
//	{ VERIFY(m_numCards > 1); VERIFY(m_bSorted); return m_cards[1]; }
	{ if (m_numCards <= 1) return NULL; VERIFY(m_bSorted); return m_cards[1]; }

CCard* CCardList::GetSecondLowestCard() const 
//	{ VERIFY(m_numCards > 1); VERIFY(m_bSorted); return m_cards[m_numCards-2]; }
	{ if (m_numCards <= 1) return NULL; VERIFY(m_bSorted); return m_cards[m_numCards-2]; }

int CCardList::GetTopCardVal() const 
	{ VERIFY(m_numCards > 0); VERIFY(m_bSorted); return m_cards[0]->GetFaceValue(); }

int CCardList::GetBottomCardVal() const 
	{ VERIFY(m_numCards > 0); VERIFY(m_bSorted); return m_cards[m_numCards-1]->GetFaceValue(); }

int CCardList::GetNumCardsAbove(const CCard* pCard) const 
	{ return GetNumCardsAbove(pCard->GetFaceValue()); }

int CCardList::GetNumCardsBelow(const CCard* pCard) const 
	{ return GetNumCardsBelow(pCard->GetFaceValue()); }
	

	
//
//================================================================
//
// Basic routines
//
//================================================================
//

//
void CCardList::Init()
{
}


//
// SetCard()
//
void CCardList::SetCard(const int nIndex, CCard* pCard) 
{ 
	m_cards[nIndex] = pCard; 
}


//
// Clear()
//
// clear the holding of all cards
//
void CCardList::Clear()
{
	m_numCards = 0;
	for(int i=0;i<MAXHOLDING;i++)
		m_cards[i] = NULL;
	m_bSorted = FALSE;
}




//
void CCardList::Add(CCard* pCard, const BOOL bSort)
{
	// check for duplicates
	ASSERT(!HasCard(pCard));
	//
	if (m_numCards >= MAXHOLDING)
		return;
	if ((bSort) || (m_bAutoSort))
	{
		// insert the card in its proper place
		int nValue = pCard->GetDeckValue();
		for (int i=0;i<m_numCards;i++)
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
		for(int j=m_numCards;j>i;j--)
			m_cards[j] = m_cards[j-1];
		// and insert
		m_cards[i] = pCard;
		m_numCards++;
		m_bSorted = TRUE;
		FormatHoldingsString();
	}
	else
	{
		m_cards[m_numCards] = pCard;
		m_numCards++;
	}
}



//
// Merge()
//
void CCardList::Merge(const CCardList& srcList) 
{ 
	//
	for(int i=0;i<srcList.GetNumCards();i++) 
		Add(srcList[i]); 
	//
	Sort(); 
}




//
// FormatHoldingsString()
//
void CCardList::FormatHoldingsString()
{
	// format the holdings string
	// first verify that the hand is sorted
	if (!m_bSorted)
		Sort();
	//
	if (m_numCards > 0) 
	{
		m_strHoldings.Empty();
		for(int i=0;i<m_numCards;i++) 
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
// GetHoldingsCommaDelimited()
//
// - format holdings as in "HA, HK, and H8"
//
CString CCardList::GetHoldingsCommaDelimited() const
{
	CString strCards;
	for(int i=0;i<m_numCards;i++)
	{
		strCards += m_cards[i]->GetName();
		if (i < m_numCards-1)
			strCards += ", ";
		else if (i == m_numCards-1)
			strCards += ", and ";
	}
	return strCards;
}



//
// TurnCardsFaceUp()
//
void CCardList::TurnCardsFaceUp(const BOOL bCode)
{
	for(int i=0;i<m_numCards;i++)
		m_cards[i]->SetFaceUp(bCode);
}



//
// RemoveByIndex()
//
// this is the main removal routine,c alled by all others
// derived classes should only redefine this version of the operation
//
CCard* CCardList::RemoveByIndex(const int nIndex)
{
	if ((nIndex < 0) || (nIndex >= m_numCards))
		return NULL;
	CCard* pCard = m_cards[nIndex];
	// update count
	m_numCards--;
	// move other cards over
	for(int i=nIndex;i<m_numCards;i++) 
		m_cards[i] = m_cards[i+1];
	for(;i<MAXHOLDING;i++) 
		m_cards[i] = NULL;
	//
	if (m_bAutoSort)
		FormatHoldingsString();
	//
	return pCard;
}


//
CCard* CCardList::RemoveByValue(const int nDeckValue)
{
	VERIFY((nDeckValue >= 0) && (nDeckValue <= 51));
	if (m_numCards <= 0)
		return NULL;
	int nIndex = GetCardIndex(nDeckValue);
	return RemoveByIndex(nIndex);
}



//
void CCardList::Remove(const CCard* pCard)
{
	VERIFY(pCard != NULL);
	if (m_numCards <= 0)
		return;
	int nIndex = GetCardIndex(pCard);
	ASSERT(nIndex >= 0);
	RemoveByIndex(nIndex);
}



//
void CCardList::Sort()
{
	if (m_bSorted)
		return;
	//
	CCard* pTemp;
	int i,j;
	for(i=0;i<m_numCards-1;i++) 
	{
		for(j=0;j<m_numCards-1;j++) 
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
	// done
	m_bSorted = TRUE;
	FormatHoldingsString();
}


//
BOOL CCardList::RangeCovers(const int nFaceValue) const 
{ 
	return ((nFaceValue >= m_cards[m_numCards-1]->GetFaceValue()) && (nFaceValue <= m_cards[0]->GetFaceValue()))? TRUE : FALSE; 
}


//
BOOL CCardList::RangeCovers(const CCard* pCard) const 
{ 
	return ((pCard->GetFaceValue() >= m_cards[m_numCards-1]->GetFaceValue()) && (pCard->GetFaceValue() <= m_cards[0]->GetFaceValue()))? TRUE : FALSE; 
}



//
//--------------------------------------------------------------
//
// Misc routines
//



//
// comparison operator
//
BOOL CCardList::operator==(const CCardList& srcList) const
{
	if (m_numCards != srcList.GetNumCards())
		return FALSE;
	//
	for(int i=0;i<m_numCards;i++)
	{
		if (m_cards[i] != srcList[i])
			return FALSE;
	}
	// everything checks out OK
	return TRUE;
}


//
BOOL CCardList::operator!=(const CCardList& srcList) const
{
	return !(operator==(srcList));
}



//
CCard* CCardList::FindCard(const int nSuit, const int nFaceValue) const
{
	//
	return FindCardByDeckValue(MAKEDECKVALUE(nSuit, nFaceValue));
}


//
CCard* CCardList::FindCardByDeckValue(const int nDeckValue) const
{
	// searches for a card of a specific suit and face value
	if (m_numCards == 0) return NULL;
	if (m_bSorted)
	{
		for(int i=0;i<m_numCards;i++)
		{
			// change this later to a binary search
			if (m_cards[i]->GetDeckValue() == nDeckValue)
				return m_cards[i];
			if (m_cards[i]->GetDeckValue() < nDeckValue)
				return NULL;	// passed where it should have been
		}
	}
	else
	{
		// do a simple linear search
		for(int i=0;i<m_numCards;i++)
			if (m_cards[i]->GetDeckValue() == nDeckValue)
				return m_cards[i];
	}
	//
	return NULL;
}

//
CCard* CCardList::FindCardByFaceValue(const int nFaceValue) const
{
	// searches for a card of the face value, any suit
	if (m_numCards == 0) return FALSE;
	if (m_bSorted)
	{
		// do a linear search
		for(int i=0;i<m_numCards;i++)
		{
			// maybe change this later to a binary search
			if (m_cards[i]->GetFaceValue() == nFaceValue)
				return m_cards[i];
//			if (m_cards[i]->GetFaceValue() < nFaceValue)
//				return NULL;	// passed where it should have been
		}
	}
	else
	{
		// do a simple linear search
		for(int i=0;i<m_numCards;i++)
			if (m_cards[i]->GetFaceValue() == nFaceValue)
				return m_cards[i];
	}
	//
	return NULL;
}


//
BOOL CCardList::HasCard(const CCard* pCard) const
{
	VERIFY(pCard != NULL);
	return HasCard(pCard->GetDeckValue());
}

//
BOOL CCardList::HasCard(const int nDeckValue) const
{
	// searches for a card of a specific suit and face value
	if (FindCardByDeckValue(nDeckValue))
		return TRUE;
	else
		return FALSE;
}

//
BOOL CCardList::HasCardOfFaceValue(const int nFaceValue) const
{
	// searches for a card of the face value, any suit
	if (FindCardByFaceValue(nFaceValue))
		return TRUE;
	else
		return FALSE;
}



//
// ContainsCards()
//
// tests whether the cards in the specified list are contained 
// within this list
//
BOOL CCardList::ContainsCards(const CCardList& cards) const
{
	int numCards = cards.GetNumCards();
	for(int i=0;i<numCards;i++)
	{
		// abort if any are missing
		if (!FindCardByDeckValue(cards[i]->GetDeckValue()))
			return FALSE;
	}
	// all there
	return TRUE;
}




//
// ContainsAtLeastOneOf()
//
// tests whether at least one card from the specified list 
// is contained within this list
//
BOOL CCardList::ContainsAtLeastOneOf(const CCardList& cards) const
{
	int numCards = cards.GetNumCards();
	for(int i=0;i<numCards;i++)
	{
		// return if a card is found
		if (FindCardByDeckValue(cards[i]->GetDeckValue()))
			return TRUE;
	}
	// nuttin' found
	return FALSE;
}




//
// return card index if it's in the holding, else return -1
//
int CCardList::GetCardIndex(const int nDeckValue) const
{
	if (m_numCards == 0) 
		return FALSE;
	// do a linear search
	for(int i=0;i<m_numCards;i++)
		if (m_cards[i]->GetDeckValue() == nDeckValue)
			return i;
	return -1;
}

//
int CCardList::GetCardIndex(const CCard* pCard) const
{
	VERIFY(pCard != NULL);
	return GetCardIndex(pCard->GetDeckValue());
}


//
// GetNumCardsAbove()
//
// return the number of cards held above a certain face value
//
int CCardList::GetNumCardsAbove(const int nFaceValue) const
{
	VERIFY((nFaceValue >= 2) && (nFaceValue <= ACE));
	if ((m_numCards == 0) || (nFaceValue >= m_cards[0]->GetFaceValue()))
		return 0;
	int nCount = 0;
	for(int i=0;i<m_numCards;i++)
	{
		if (m_cards[i]->GetFaceValue() <= nFaceValue)
			break;
		nCount++;
	}
	return nCount;
}


//
// GetNumCardsBelow()
//
// return the number of cards held below a certain face value
//
int CCardList::GetNumCardsBelow(const int nFaceValue) const
{
	VERIFY((nFaceValue >= 2) && (nFaceValue <= ACE));
	if ((m_numCards == 0) || (nFaceValue <= m_cards[m_numCards-1]->GetFaceValue()))
		return 0;
	int nCount = 0;
	for(int i=m_numCards-1;i>=0;i--)
	{
		if (m_cards[i]->GetFaceValue() >= nFaceValue)
			break;
		nCount++;
	}
	return nCount;
}



//
// GetLowestCardAbove()
//
CCard* CCardList::GetLowestCardAbove(const int nFaceValue) const
{
	int numCardsAbove = GetNumCardsAbove(nFaceValue);
	if (numCardsAbove == 0)
		return NULL;
	ASSERT((numCardsAbove >= 0) && (numCardsAbove <= m_numCards));
	return m_cards[numCardsAbove-1];
}



//
// GetHighestCardBelow()
//
CCard* CCardList::GetHighestCardBelow(const int nFaceValue) const
{
	int numCardsBelow = GetNumCardsBelow(nFaceValue);
	if (numCardsBelow == 0)
		return NULL;
	int nCardIndex = m_numCards - numCardsBelow;
	ASSERT((nCardIndex >= 0) && (nCardIndex < m_numCards));
	return m_cards[nCardIndex];
}



//
// GetLowestCardAbove()
//
CCard* CCardList::GetLowestCardAbove(const CCard* pCard) const
{
	ASSERT(pCard != NULL);
	return GetLowestCardAbove(pCard->GetFaceValue());
}



//
// GetHighestCardBelow()
//
CCard* CCardList::GetHighestCardBelow(const CCard* pCard) const
{
	ASSERT(pCard != NULL);
	return GetHighestCardBelow(pCard->GetFaceValue());
}



//
// GetAllCardsAbove()
//
int CCardList::GetAllCardsAbove(const int nFaceValue, CCardList& cards) const
{
	int numCardsAbove = GetNumCardsAbove(nFaceValue);
	if (numCardsAbove == 0)
		return NULL;
	ASSERT((numCardsAbove >= 0) && (numCardsAbove <= m_numCards));
	cards.Clear();
	for(int i=0;i<numCardsAbove;i++)
		cards << m_cards[i];
	//
	return numCardsAbove;
}



//
// GetAllCardsBelow()
//
int CCardList::GetAllCardsBelow(const int nFaceValue, CCardList& cards) const
{
	int numCardsBelow = GetNumCardsBelow(nFaceValue);
	if (numCardsBelow == 0)
		return NULL;
	int nCardIndex = m_numCards - numCardsBelow;
	ASSERT((nCardIndex >= 0) && (nCardIndex < m_numCards));
	cards.Clear();
	for(int i=nCardIndex;i<m_numCards;i++)
		cards << m_cards[i];
	//
	return numCardsBelow;
}


//
// GetAllCardsAbove()
//
int CCardList::GetAllCardsAbove(const CCard* pCard, CCardList& cards) const
{
	ASSERT(pCard != NULL);
	return GetAllCardsAbove(pCard->GetFaceValue(), cards);
}


//
// GetAllCardsBelow()
//
int CCardList::GetAllCardsBelow(const CCard* pCard, CCardList& cards) const
{
	ASSERT(pCard != NULL);
	return GetAllCardsBelow(pCard->GetFaceValue(), cards);
}


//
// GetEquivalentCards()
//
// - returns a list of touching cards
//
int CCardList::GetEquivalentCards(CCard* pCard, CCardList& cardList, const BOOL bIncludeCard)
{
	ASSERT(pCard != NULL);
	ASSERT(HasCard(pCard));
	if (!m_bSorted)
		Sort();
	cardList.Clear();

	// add card if desired
	if (bIncludeCard)
		cardList << pCard;

	// first add higher cards
	int nVal = pCard->GetFaceValue() + 1;
	int nIndex = GetCardIndex(pCard) - 1;
	for(int i=nIndex;i>=0;i--,nVal++)
	{
		if (m_cards[i]->GetFaceValue() == nVal)
			cardList << m_cards[i];
		else
			break;
	}

	// then add lower cards
	nVal = pCard->GetFaceValue() - 1;
	nIndex = GetCardIndex(pCard) + 1;
	for(i=nIndex;i<m_numCards;i++,nVal--)
	{
		if (m_cards[i]->GetFaceValue() == nVal)
			cardList << m_cards[i];
		else
			break;
	}

	// done
	return cardList.GetNumCards();
}




//
// AreEquivalentCards()
//
// determines whether two cards are more or less equivalent in rank
// i.e., if we have continuous sequence covering the two cards
// assume the cards are of the same suit
//
BOOL CCardList::AreEquivalentCards(CCard* pCard1, CCard* pCard2)
{
	int nIndex1 = GetCardIndex(pCard1);
	int nIndex2 = GetCardIndex(pCard2);
	if ((nIndex1 < 0) || (nIndex2 < 0))
		return FALSE;	// oops

	// check distance versus face value difference
	int nVal1 = pCard1->GetFaceValue();
	int nVal2 = pCard2->GetFaceValue();
	if ((nVal1 - nVal2) == (nIndex2 - nIndex1))
		return TRUE;
	else
		return FALSE;
}




//
// GetHighestEquivalentCard()
//
// - returns the highest touching card
//   assume the cards are of the same suit
//
CCard* CCardList::GetHighestEquivalentCard(CCard* pCard)
{
	ASSERT(pCard != NULL);
	ASSERT(HasCard(pCard));
	if (!m_bSorted)
		Sort();

	// 
	int nVal = pCard->GetFaceValue() + 1;
	int nIndex = GetCardIndex(pCard) - 1;
	for(int i=nIndex;i>=0;i--,nVal++)
	{
		if (m_cards[i]->GetFaceValue() != nVal)
			break;
	}
	// 
	return m_cards[i+1];
}




//
// GetLowestEquivalentCard()
//
// - returns the lowest card of the sequence this card belongs to
//
CCard* CCardList::GetLowestEquivalentCard(CCard* pCard)
{
	ASSERT(pCard != NULL);
	ASSERT(HasCard(pCard));
	if (!m_bSorted)
		Sort();

	//
	int nVal = pCard->GetFaceValue() - 1;
	int nIndex = GetCardIndex(pCard) + 1;
	for(int i=nIndex;i<m_numCards;i++,nVal--)
	{
		if (m_cards[i]->GetFaceValue() != nVal)
			break;
	}
	return m_cards[i-1];
}

