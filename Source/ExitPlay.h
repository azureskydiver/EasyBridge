//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// ExitPlay.h
//

#ifndef __CEXITPLAY__
#define __CEXITPLAY__

#include "Play.h"


//
// The CExitPlay class
//
class CExitPlay: public CPlay {

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
	int		m_nHand;
	int		m_nSuit1;
	int		m_nSuit2;

// construction/destruction
public:
	CExitPlay(CPlayList* pPrerequisites, int nSuit1=ANY, int nSuit2=ANY);
	CExitPlay(CExitPlay& srcPlay);
	virtual ~CExitPlay();
};


#endif
