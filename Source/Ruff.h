//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Ruff.h
//

#ifndef __CRUFF__
#define __CRUFF__

#include "Play.h"


//
// The CRuff class
//
class CRuff : public CPlay {

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
	//
	virtual int		UsesUpEntry();
	virtual int		GetNumDiscardsRequired() const { return m_numDiscardsRequired; }


// protected routines
protected:


// protected data
protected:
	int		m_numDiscardsRequired;

// construction/destruction
public:
	CRuff(int nTargetHand, int numDiscardsRequired, int nSuit=NONE, PlayProspect=PP_LIKELY_WINNER);
	CRuff(int nTargetHand, int numDiscardsRequired, CCard* pCard, PlayProspect nPlayProspect=PP_LIKELY_WINNER);
	CRuff(CRuff& srcPlay);
	virtual ~CRuff();
};


#endif
