//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Discard.h
//

#ifndef __CDISCARD__
#define __CDISCARD__

#include "Play.h"


//
// The CDiscard class
//
class CDiscard : public CPlay {

// public data
public:
	enum { IN_HAND=0, 
		   IN_DUMMY=1, 
	};

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
	int		m_nCardVal;

// construction/destruction
public:
	CDiscard(int nTargetHand, int nSuit=NONE, int nCardVal=NONE);
	CDiscard(int nTargetHand, CCard* pCard);
	CDiscard(CDiscard& srcPlay);
	virtual ~CDiscard();
};


#endif
