//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Type1Finesse.h
//

#ifndef __CTYPE1FINESSE__
#define __CTYPE1FINESSE__

#include "Finesse.h"
class CCardList;


//
// The CType1Finesse class
//
class CType1Finesse : public CFinesse {

// public routines
public:
	// overloadable functions
	virtual void	Clear();
	virtual void	Init();
	virtual CString GetFullDescription();
	virtual PlayResult	Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
								CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, 
								CPlayerStatusDialog& status, CCard*& pPlayCard);

// protected routines
protected:


// protected data
protected:
	CCardList*	m_pCoverCards;
	CCardList*	m_pLeadCards;

// construction/destruction
public:
	CType1Finesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, CCardList* pCoverCards, CCardList* m_pLeadCards, int nSuit, int nCardVal);
	CType1Finesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, CCardList* pCoverCards, CCardList* m_pLeadCards, CCard* pCard);
	virtual ~CType1Finesse();
};


#endif
