//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//===========================================================
//
// CGuessedHandHoldings
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "HandHoldings.h"
#include "GuessedHandHoldings.h"



//
//--------------------------------------------------------------
//
// Construction/Destruction
//

// constructor
CGuessedHandHoldings::CGuessedHandHoldings()
{
	// do one-time init of the suits
	for(int i=0;i<4;i++)
		m_suit[i].SetSuit(i);
	// and perform other inits
	Init();
}

// destructor
CGuessedHandHoldings::~CGuessedHandHoldings()
{
}


//
//--------------------------------------------------------------
//


//
// InitNewHand()
//
// called after being dealt a hand
//
void CGuessedHandHoldings::InitNewHand(CHandHoldings& hand, int nPosition)
{
	// fill our own hand with known cards
	for(int i=0;i<m_numTotalCards;i++)
	{
		CGuessedCard* pCard = new CGuessedCard(hand[i], TRUE, nPosition, 1.0);
		Add(pCard);
	}
}



//
// Init()
//
void CGuessedHandHoldings::Init()
{
	// clear analysis variables
	CGuessedCardHoldings::Init();
	for(int i=0;i<4;i++)
		m_suit[i].Init();
	m_bAllCardsIdentified = FALSE;
}



//
// Clear()
//
void CGuessedHandHoldings::Clear(BOOL bDelete)
{
	CGuessedCardHoldings::Clear(bDelete);
	// be sure NOT to delete the guessed cards again in the suits
	for(int i=0;i<4;i++)
		m_suit[i].Clear(FALSE);
}


//
void CGuessedHandHoldings::FormatHoldingsString()
{
	m_strHoldings.Format("S:%s  H:%s  D:%s  C:%s",
							m_suit[3].GetHoldingsString(),
							m_suit[2].GetHoldingsString(),
							m_suit[1].GetHoldingsString(),
							m_suit[0].GetHoldingsString());
}


//
void CGuessedHandHoldings::Add(CGuessedCard* pCard, BOOL bSort)
{
	CGuessedCardHoldings::Add(pCard, bSort);
	m_suit[pCard->GetSuit()].Add(pCard, bSort);
	FormatHoldingsString();
}



//
CGuessedCard* CGuessedHandHoldings::RemoveByIndex(int nIndex)
{
	CGuessedCard* pCard = CGuessedCardHoldings::RemoveByIndex(nIndex);

	//
	m_suit[pCard->GetSuit()].Remove(pCard);
	FormatHoldingsString();
	return pCard;
}




//
void CGuessedHandHoldings::Sort()
{
	// first call base class to sort the cards in S-H-D-C order
	CGuessedCardHoldings::Sort();

	// sort each of the suits individually
	for(int i=0;i<4;i++) 
		m_suit[i].Sort();

	// and then set the literal hand description
	FormatHoldingsString();
}



///////////////////////////////////////////////////////////////////////
//
// Utils
//
////////////////////////////////////////////////////////////////////////



//
int CGuessedHandHoldings::GetNumSuitsFullyIdentified()
{
	int numSuitsIdentified = 0;
	for(int i=0;i<4;i++)
	{
		if (m_suit[i].AreAllCardsIdentified())
			numSuitsIdentified++;
	}
	return numSuitsIdentified;
}


