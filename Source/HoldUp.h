//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// HoldUp.h
//

#ifndef __CHOLDUP__
#define __CHOLDUP__

#include "Play.h"


//
// The CHoldUp class
//
class CHoldUp: public CPlay {

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


// construction/destruction
public:
	CHoldUp(int nSuit=NONE);
	CHoldUp(CHoldUp& srcPlay);
	virtual ~CHoldUp();
};


#endif
