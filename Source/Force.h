//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Force.h
//

#ifndef __CFORCE__
#define __CFORCE__

#include "Play.h"


//
// The CForce class
//
class CForce : public CPlay {

// public data
public:

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
	int		m_nTargetCardVal;	// lowest target card to force out
	int		m_nCardVal;	// 2..14 (deuce through Ace)

// construction/destruction
public:
	CForce(int nTargetHand, int nTargetCardVal, CCardList* pReqPlayedList, int nSuit, int nCardVal);
	CForce(int nTargetHand, int nTargetCardVal, CCardList* pReqPlayedList, CCard* pCard);
	virtual ~CForce();
};


#endif
