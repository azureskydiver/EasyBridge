//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Type3Finesse.h
//

#ifndef __CTYPE3FINESSE__
#define __CTYPE3FINESSE__

#include "Finesse.h"


//
// The CType3Finesse class
//
class CType3Finesse : public CFinesse {

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
	CCardList*	m_pLeadCards;

// construction/destruction
public:
	CType3Finesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, CCardList* pLeadCards, int nSuit, int nCardVal);	
	CType3Finesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, CCardList* pLeadCards, CCard* pCard);
	virtual ~CType3Finesse();
};


#endif
