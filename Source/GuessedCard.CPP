//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// GuessedCard.cpp
//
#include "stdafx.h"
#include "EasyB.h"
//#include "EasyBDoc.h"
#include "Card.h"
#include "Deck.h"
#include "GuessedCard.h"




// constructor
CGuessedCard::CGuessedCard(int nSuit, int nFaceValue, BOOL bOutstanding, int nLocation, float fFixProbability) :
			m_nSuit(nSuit),
			m_nFaceValue(nFaceValue),
			m_bOutstanding(bOutstanding), 
			m_nLocation(nLocation), 
			m_fFixProbability(fFixProbability)
{	
	int nDeckValue = MAKEDECKVALUE(m_nSuit,m_nFaceValue);
	VERIFY(ISDECKVAL(nDeckValue));
	m_pCard = deck.GetSortedCard(nDeckValue);
	strcpy(m_szName, m_pCard->GetName());
}

CGuessedCard::CGuessedCard(CCard* pCard, BOOL bOutstanding, int nLocation, float fFixProbability) :
			m_pCard(pCard),
			m_bOutstanding(bOutstanding), 
			m_nLocation(nLocation), 
			m_fFixProbability(fFixProbability)
{
	VERIFY(pCard);
	m_nSuit = pCard->GetSuit();
	m_nFaceValue = pCard->GetFaceValue();
	strcpy(m_szName, pCard->GetName());
}

CGuessedCard::CGuessedCard()
{
	Clear();
}

// destructor
CGuessedCard::~CGuessedCard()
{
}

//
// Init()
//
// call this if the default constructor is used
//
void CGuessedCard::Init(int nSuit, int nFaceValue)
{
	m_nSuit = nSuit;
	m_nFaceValue = nFaceValue;
	int nDeckValue = MAKEDECKVALUE(m_nSuit,m_nFaceValue);
	m_pCard = deck.GetSortedCard(nDeckValue);
	strcpy(m_szName, CardToShortString(nDeckValue));
	//
	Clear();
}


/////////////////////////////////////////////////////////////
//
// Main routines
//
/////////////////////////////////////////////////////////////



//
// Clear()
//
//
void CGuessedCard::Clear()
{
	m_nLocation = UNKNOWN;
	m_bOutstanding = TRUE;
	m_fFixProbability = 0.0f;
}


//
// ScaleFixProbability
//
// scale the fix probability by a factor, 0 to +n
// results are clamped to the range 0f, 1.0f
// e.g., 30% scaled by 0.5 is 15%
// e.g., 30% scaled by 1.5 is 45%
// e.g., 30% scaled by 0.0 is 30%	(0% adj has no effect)
// e.g.,  0% scaled by 1.5 is  0%   (0% scaled always stays at zero)
//
float CGuessedCard::ScaleFixProbability(float fAdj)
{
	if (m_fFixProbability == 0.0f)
		return 0.0f;
	// adjust only if scalar is > 0)
	if (fAdj > 0.0f)
		m_fFixProbability *= fAdj;
	// adjust the probability to make sense
	return ClampFixProbability();
}



//
// AdjustFixProbability
//
// input is in the range -1.0 to +n
// adds the input value to the existing probability
// results are clamped to the range 0f, 0.9999f
// e.g., 30% adjusted by  0.5 (+50%) is 80%
// e.g., 60% adjusted by -0.5 (-50%) is 10%
//
float CGuessedCard::AdjustFixProbability(float fAdj)
{
	if (fAdj < -1.0f)
		fAdj = -1.0f;
	//
	m_fFixProbability += fAdj;
	return ClampFixProbability();
}


//
// AdjustFixProbabilityIncremental
//
// input is in the range -1.0 to +1.0
// if the input is positive, adjusts the probability figure by 
// adding the equivalent of n% of the difference between the 
// current probability and 1.0%
// if input is negative, subtracts the equivalent of ABS(n%) from the
// existing probability
// e.g., 40% adjusted by  0.5 (+ 50% increment of 60% gap) is 70%
// e.g., 40% adjusted by  1.0 (+100% increment of 60% gap) is 100%
// e.g., 40% adjusted by -0.5 (- 50% of probability) is 20%
//
float CGuessedCard::AdjustFixProbabilityIncremental(float fAdj)
{
	if (fAdj > 1.0f)
		fAdj = 1.0f;
	if (fAdj < -1.0f)
		fAdj = -1.0f;
	//
	if (fAdj > 0)
		m_fFixProbability += (1.0f - m_fFixProbability) * fAdj;
	else if (fAdj < 0)
		m_fFixProbability -= m_fFixProbability * fAdj;
	return ClampFixProbability();
}


