//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CueBidConvention.h
//
#ifndef __CUEBID__
#define __CUEBID__

#include "SlamConvention.h"


//
// The CCueBidConvention class
//
class CCueBidConvention : public CSlamConvention
{

	// public routines
public:
	BOOL ApplyTest(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status);
	BOOL TryCueBid(CHandHoldings& hand, CBidEngine& bidState,  CPlayerStatusDialog& status);
	//
	BOOL TryConvention(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status) { ASSERT(FALSE); return FALSE; }
	BOOL RespondToConvention(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status);
	BOOL HandleConventionResponse(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status);

// private routines
private:
	int GetCheapestAce(CHandHoldings& hand, int nBaseSuit, int nSecondSuit = NONE);
	int GetCheapestKingOrVoid(CHandHoldings& hand, int nBaseSuit, int nSecondSuit = NONE);

// data
private:

	
// 
public:
	CCueBidConvention();
	virtual ~CCueBidConvention();
};


#endif

extern CCueBidConvention cueBidConvention;
