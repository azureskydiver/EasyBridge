//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Type2Finesse.h
//

#ifndef __CTYPE2FINESSE__
#define __CTYPE2FINESSE__

#include "Finesse.h"
class CCardList;

//
// The CType2Finesse class
//
class CType2Finesse : public CFinesse {

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

// construction/destruction
public:
	CType2Finesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, CCardList* pCoverCards, int nSuit, int nCardVal);
	CType2Finesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, CCardList* pCoverCards, CCard* pCard);
	virtual ~CType2Finesse();
};


#endif
