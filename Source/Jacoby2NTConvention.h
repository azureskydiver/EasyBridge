//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CJacoby2NTConvention.h
//
#ifndef __CJACOBY2NTCONVENTION__
#define __CJACOBY2NTCONVENTION__

#include "Convention.h"


//
// The CJacoby2NTConvention class
//
class CJacoby2NTConvention : public CConvention
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
	CJacoby2NTConvention();
	virtual ~CJacoby2NTConvention();
};


#endif

extern CJacoby2NTConvention jacoby2NTConvention;
