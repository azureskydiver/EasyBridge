//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// TypeAFinesse.h
//

#ifndef __CTypeAFinesse__
#define __CTypeAFinesse__

#include "Finesse.h"


//
// The CTypeAFinesse class
//
class CTypeAFinesse : public CFinesse {

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


// construction/destruction
public:
	CTypeAFinesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, int nSuit, int nCardVal);	
	CTypeAFinesse(int nPlayerPosition, int nTargetHand, CCardList* pGapCards, CCard* pCard);
	virtual ~CTypeAFinesse();
};


#endif
