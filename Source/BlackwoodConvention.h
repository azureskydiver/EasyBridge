//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// BlackwoodConvention.h
//
#ifndef __BLACKWOOD__
#define __BLACKWOOD__

#include "SlamConvention.h"


//
// The CBlackwoodConvention class
//
class CBlackwoodConvention : public CSlamConvention
{

// public routines
public:
	virtual BOOL ApplyTest(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, 
						   CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands,
						   CBidEngine& bidState, CPlayerStatusDialog& status);
	virtual BOOL InvokeBlackwood(CHandHoldings& hand, CBidEngine& bidState, CPlayerStatusDialog& status, int nEventualSuit);
	//
	BOOL TryConvention(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status) { ASSERT(FALSE); return FALSE; }
	BOOL RespondToConvention(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status);
	BOOL HandleConventionResponse(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status);

// private routines
private:

// data
private:

	
// 
public:
	CBlackwoodConvention();
	virtual ~CBlackwoodConvention();
};


#endif

extern CBlackwoodConvention blackwoodConvention;
