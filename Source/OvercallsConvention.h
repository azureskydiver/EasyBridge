//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// COvercallsConvention.h
//
#ifndef __COVERCALLS__
#define __COVERCALLS__

#include "Convention.h"


//
// The COvercallsConvention class
//
class COvercallsConvention : public CConvention
{
// private routines
private:
	BOOL TryConvention(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status);
	BOOL RespondToConvention(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status);
	BOOL HandleConventionResponse(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status);
	// 
	BOOL ApplySuitStrengthTest(int nSuit, int nOppBid, CBidEngine& bidState);

// data
private:
	
// 
public:
	COvercallsConvention();
	virtual ~COvercallsConvention();
};


#endif

extern COvercallsConvention OvercallsConvention;
