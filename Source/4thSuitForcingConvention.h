//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// 4thSuitForcingConvention.h
//
#ifndef __C4THSUITFORCINGCONVENTION__
#define __C4THSUITFORCINGCONVENTION__

#include "Convention.h"


//
// The C4thSuitForcingConvention class
//
class C4thSuitForcingConvention : public CConvention
{
	BOOL TryConvention(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status);

// public routines
public:
#include "Convention.h"

// private routines
private:

// data
private:

	
// 
public:
	C4thSuitForcingConvention();
	virtual ~C4thSuitForcingConvention();
};


#endif

extern C4thSuitForcingConvention fourthSuitForcingConvention;
