//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//===========================================================
//
// CCardHoldings
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "Card.h"
#include "CardHoldings.h"
#include "BidParams.h"



//
//--------------------------------------------------------------
//
// Construction/Destruction
//

// constructor
CCardHoldings::CCardHoldings()
{
	// clear all variables
	Clear();
}

// destructor
CCardHoldings::~CCardHoldings()
{
}




	
//
//================================================================
//
// Basic routines
//
//================================================================
//

//
void CCardHoldings::Init()
{
	CCardList::Init();
}


//
// Clear()
//
// clear the holding of all cards
//
void CCardHoldings::Clear()
{
	CCardList::Clear();
	// always clear the point count when clearing the hand
	ClearPointCount();
}


//
// ClearPointCount()
//
void CCardHoldings::ClearPointCount()
{
	m_bCountPerformed = FALSE;
	m_numHCPoints = 0;
}


//
// Sort()
//


//
void CCardHoldings::Sort()
{
	// first call base class to sort the cards in S-H-D-C order
	CCardList::Sort();

	// and set the custom hand description for GIB
	FormatGIBHoldingsString();
}



//
// GetCardsOfSuit()
//
int CCardHoldings::GetCardsOfSuit(const int nSuit, CCardList* pSuitList) const
{
	int nCount = 0;
	for(int i=0;i<m_numCards;i++)
	{
		if (m_cards[i]->GetSuit() == nSuit)
		{
			nCount++;
			if (pSuitList)
				*pSuitList << m_cards[i];
		}
	}
	//
	if ((pSuitList) && (!pSuitList->IsSorted()))
		pSuitList->Sort();
	//
	return nCount;
}




//
// FormatHoldingsString()
//
void CCardHoldings::FormatHoldingsString()
{
	// first call the base class
	CCardList::FormatHoldingsString();

	// then format for GIB
	FormatGIBHoldingsString();
}




//
// FormatGIBHoldingsString()
//
void CCardHoldings::FormatGIBHoldingsString()
{
	// format the hand string in GIB style
	int nCurrentSuit = SPADES;
	int i,nIndex=0;
	m_strGIBFormatHoldings.Empty();
	for(i=0;i<4;i++)
	{
		while((nIndex < m_numCards) &&
				  (m_cards[nIndex]->GetSuit() == nCurrentSuit))
		{
			m_strGIBFormatHoldings += m_cards[nIndex]->GetCardLetter();
			nIndex++;
		}
		if (i < 3)
			m_strGIBFormatHoldings += '.';
		nCurrentSuit--;
	}
}



//
double CCardHoldings::CountPoints(const BOOL bForceCount)
{
	// avoid repeating the count unless forced to do so
	if ((m_bCountPerformed) && (!bForceCount))
		return m_numHCPoints;

	// else clear and start the count
	ClearPointCount();
	for(int i=0;i<m_numCards;i++)
	{
		int nFaceVal = m_cards[i]->GetFaceValue();
		if (nFaceVal >= 10)
			m_numHCPoints += HONOR_VALUE(nFaceVal);
	}

	// done
	m_bCountPerformed = TRUE;
	return m_numHCPoints;
}


//
void CCardHoldings::EvaluateHoldings()
{
}


