//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CArtificial2ClubConvention.h
//
#ifndef __C2CLUBCONVENTION__
#define __C2CLUBCONVENTION__

#include "Convention.h"


//
// The CArtificial2ClubConvention class
//
class CArtificial2ClubConvention : public CConvention
{

// public routines
public:
	BOOL TryConvention(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status);
	BOOL RespondToConvention(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status);
	BOOL HandleConventionResponse(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status);

// private routines
private:

// data
private:

	
// 
public:
	CArtificial2ClubConvention();
	virtual ~CArtificial2ClubConvention();
};


#endif

extern CArtificial2ClubConvention Artificial2ClubConvention;
