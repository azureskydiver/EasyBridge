//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// GerberConvention.h
//
#ifndef __GERBER__
#define __GERBER__

#include "SlamConvention.h"


//
// The CGerberConvention class
//
class CGerberConvention : public CSlamConvention
{

// public routines
public:
	virtual BOOL ApplyTest(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, 
						   CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands,
						   CBidEngine& bidState, CPlayerStatusDialog& status);
	virtual BOOL InvokeGerber(CHandHoldings& hand, CBidEngine& bidState, CPlayerStatusDialog& status, int nEventualSuit);
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
	CGerberConvention();
	virtual ~CGerberConvention();
};


#endif

extern CGerberConvention gerberConvention;
