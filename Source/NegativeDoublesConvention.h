//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CNegativeDoublesConvention.h
//
#ifndef __CNEGATIVEDOUBLESCONVENTION__
#define __CNEGATIVEDOUBLESCONVENTION__

#include "Convention.h"


//
// The CNegativeDoublesConvention class
//
class CNegativeDoublesConvention : public CConvention
{
// private routines
private:
	BOOL TryConvention(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status);
	BOOL RespondToConvention(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status);
	BOOL HandleConventionResponse(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status);

// data
private:
	
// 
public:
	CNegativeDoublesConvention();
	virtual ~CNegativeDoublesConvention();
};


#endif

extern CNegativeDoublesConvention NegativeDoubleConvention;
