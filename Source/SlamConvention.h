//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// SlamConvention.h
//
#ifndef __SLAMCONVENTION__
#define __SLAMCONVENTION__

#include "Convention.h"


//
// The CSlamConvention class
//
class CSlamConvention : public CConvention
{

// public routines
public:
	BOOL ApplyTest(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, 
				   CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands,
				   CBidEngine& bidState, CPlayerStatusDialog& status);

// private routines
private:

// data
private:

	
// 
public:
	CSlamConvention();
	virtual ~CSlamConvention();
};


#endif

