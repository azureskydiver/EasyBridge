//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CGuessedHandHoldings
//
//
#ifndef __CGUESSEDHANDHOLDINGS__
#define __CGUESSEDHANDHOLDINGS__

#include "GuessedSuitHoldings.h"
class CHandHoldings;

//
class CGuessedHandHoldings : public CGuessedCardHoldings {

// public routines
public:
	// overridden functions
	virtual void Init();
	virtual void InitNewHand(CHandHoldings& hand, int nPosition);
	virtual void Clear(BOOL bDelete=TRUE);
	virtual void Add(CGuessedCard* pCard, BOOL bSort=FALSE);
	virtual void operator<<(CGuessedCard* pCard) { Add(pCard); }
	virtual CGuessedCard* RemoveByIndex(int nIndex);
	virtual void Sort();
	virtual void FormatHoldingsString();
	//
	virtual void MarkSuitShownOut(int nSuit, BOOL bCode=TRUE) { VERIFY(ISSUIT(nSuit)); m_suit[nSuit].MarkSuitShownOut(bCode); }
	virtual BOOL IsSuitShownOut(int nSuit) { VERIFY(ISSUIT(nSuit)); return m_suit[nSuit].IsSuitShownOut(); }
	virtual BOOL WasSuitVoid(int nSuit) { VERIFY(ISSUIT(nSuit)); return m_suit[nSuit].WasSuitVoid(); }
	//
	virtual void MarkAllCardsIdentified(BOOL bCode=TRUE) { m_bAllCardsIdentified = bCode; }
	virtual BOOL AreAllCardsIdentified() { return m_bAllCardsIdentified; }
	virtual int  GetNumSuitsFullyIdentified();

	//
	CGuessedSuitHoldings& GetSuit(int nSuit) { VERIFY(ISSUIT(nSuit)); return m_suit[nSuit]; }

	//
	// inline functions
	//
	
// public data
public:

// private data
protected:
	CGuessedSuitHoldings m_suit[4];
	BOOL	m_bAllCardsIdentified;

// construction/destruction
public:
	CGuessedHandHoldings();
	~CGuessedHandHoldings();
};

#endif