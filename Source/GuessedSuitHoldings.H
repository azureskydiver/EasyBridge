//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CGuessedCardHoldings
//
//
#ifndef __CGUESSEDSUITHOLDINGS__
#define __CGUESSEDSUITHOLDINGS__

#include "GuessedCardHoldings.h"


//
class CGuessedSuitHoldings : public CGuessedCardHoldings {

// public routines
public:
	virtual void Init();
	virtual void Clear(BOOL bDelete =TRUE);
	virtual void FormatHoldingsString();
	virtual CGuessedCard* RemoveByIndex(int nIndex);
	//
	virtual void MarkSuitShownOut(BOOL bCode=TRUE);
	virtual BOOL IsSuitShownOut() { return m_bShownOut; }
	virtual void MarkSuitVoid(BOOL bCode=TRUE) { m_bSuitWasVoid = bCode; }
	virtual BOOL WasSuitVoid() { return m_bSuitWasVoid; }
	virtual void MarkAllCardsIdentified(BOOL bCode=TRUE) { m_bAllCardsIdentified = bCode; }
	virtual BOOL AreAllCardsIdentified() { return m_bAllCardsIdentified; }
	//
	virtual int GetNumRemainingCards();

	//
	// misc functions
	//
	void SetSuit(int nSuit) { m_nSuit = nSuit; }
//	CGuessedCard* operator[](int nIndex) const { VERIFY(nIndex < m_numCards); return m_cards[nIndex]; }
	
// public data
public:

// private data
protected:
	int		m_nSuit;
	BOOL	m_bShownOut;
	BOOL	m_bSuitWasVoid;
	BOOL	m_bAllCardsIdentified;

// construction/destruction
public:
	CGuessedSuitHoldings();
	~CGuessedSuitHoldings();
};

#endif