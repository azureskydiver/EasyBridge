//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//===========================================================
//
// CGuessedSuitHoldings
//

#include "stdafx.h"
#include "EasyB.h"
//#include "EasyBdoc.h"
#include "GuessedSuitHoldings.h"



//
//--------------------------------------------------------------
//
// Construction/Destruction
//

// constructor
CGuessedSuitHoldings::CGuessedSuitHoldings()
{
	// clear all variables
	m_nSuit = NONE;
	Init();
}

// destructor
CGuessedSuitHoldings::~CGuessedSuitHoldings()
{
	Clear();
}


//
//--------------------------------------------------------------
//

//
void CGuessedSuitHoldings::Init()
{
	CGuessedCardHoldings::Init();
	//
	m_bShownOut = FALSE;
	m_bSuitWasVoid = FALSE;
	m_bAllCardsIdentified = FALSE;
	// 
	m_numMaxRemainingCards = 13;
	m_numMaxOriginalCards = 13;
}


//
void CGuessedSuitHoldings::Clear(BOOL bDelete)
{
	CGuessedCardHoldings::Clear(bDelete);
}


//
void CGuessedSuitHoldings::FormatHoldingsString()
{
	// format the holdings string
	m_strHoldings.Empty();
	if (m_numTotalCards > 0) 
	{
		for(int i=0;i<m_numTotalCards;i++) 
			m_strHoldings += m_cards[i]->GetCardLetter();
	} 
	else 
	{
		m_strHoldings = "---";
	}
}



//
// RemoveByIndex()
//
CGuessedCard* CGuessedSuitHoldings::RemoveByIndex(int nIndex)
{
	// first call base class
	CGuessedCard* pCard = CGuessedCardHoldings::RemoveByIndex(nIndex);

	// then clean up items in this class
	if (pCard->WasPlayed() && (m_bShownOut))
	{
		// "forget" that the suit was shown out
		m_bShownOut = FALSE;
	}

	// done
	return pCard;
}





///////////////////////////////////////////////////////////////
//
// Utils
//
///////////////////////////////////////////////////////////////


//
// MarkSuitShownOut()
// 
// a player has shown out in this suit
//
void CGuessedSuitHoldings::MarkSuitShownOut(BOOL bCode) 
{ 
	if (m_bShownOut == bCode)
		return;
	//
	m_bShownOut = bCode; 

	// see if the player turned out to have been void
	if (GetNumCardsPlayed() == 0)
	{
		// mark the suit as void
		m_bSuitWasVoid = TRUE;
	}

	// remove any speculative cards from this list
	for(int i=0,nIndex=0;i<m_numTotalCards;i++)
	{
		if (!m_cards[nIndex]->WasPlayed())
		{
			Delete(m_cards[nIndex]);
		}
		else
		{
			nIndex++;
		}
	}

	// adjust counts
	m_numTotalCards = m_numDefiniteCards = m_numPlayedCards;
	m_numLikelyCards = 0;

	// remaining cards are now zero
	m_numRemainingCards = 0;
	m_numMinRemainingCards = m_numMaxRemainingCards = 0;

	// original (strating) cards = cards played
	m_numOriginalCards = m_numPlayedCards;
	m_numMinOriginalCards = m_numMaxOriginalCards = m_numPlayedCards;

	// mark status
	m_bAllCardsIdentified = TRUE;
}



//
// GetNumRemainingCards()
//
int CGuessedSuitHoldings::GetNumRemainingCards()
{
	// see if all cardshave been indentified
	if (m_bAllCardsIdentified)
	{
		return m_numOriginalCards - m_numPlayedCards;
	}
	else
	{
		// don't know yet
		return -1;
	}
}

