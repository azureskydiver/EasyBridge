//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Convention.h
//
// this is a virtual base class from which specific conventions
// are derived
//

#ifndef __CCONVENTION__
#define __CCONVENTION__

#include "Player.h"
#include "HandHoldings.h"
#include "CardLocation.h"
#include "GuessedHandHoldings.h"
#include "BidParams.h"
#include "BidEngine.h"


// some constants
const int CONV_INACTIVE			= 0;
const int CONV_INVOKED			= 1;
const int CONV_INVOKED_ROUND1	= 1;
const int CONV_INVOKED_ROUND2	= 2;
const int CONV_INVOKED_ROUND3	= 3;
const int CONV_RESPONDED		= 10;
const int CONV_RESPONDED_ROUND1	= 10;
const int CONV_RESPONDED_ROUND2	= 11;
//
const int CONV_FINISHED			= -1;
const int CONV_REJECTED			= -10;
const int CONV_SUBSUMED			= -20;
const int CONV_ERROR			= -99;

//
// The Convention class
//
class CConvention {

	friend class CConventionSet;

// public routines
public:
	// overloadable functions
	virtual void Initialize();
	virtual void Terminate();
	virtual void ClearState() {}
	virtual void Enable() { m_bEnabled = TRUE; }
	virtual void Disable() { m_bEnabled = FALSE; }
	virtual BOOL IsEnabled() { return m_bEnabled; }
	virtual BOOL CheckForOtherConventions(CBidEngine& bidState);
	// pure virtual functions
	virtual BOOL ApplyTest(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, 
						   CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands,
						   CBidEngine& bidState, CPlayerStatusDialog& status);
	virtual BOOL TryConvention(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, 
							   CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands,
							   CBidEngine& bidContext, CPlayerStatusDialog& status) = 0;
	virtual BOOL RespondToConvention(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status) { return FALSE; }
	virtual BOOL HandleConventionResponse(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status);
	// non-virtual functions
	void SetID(int newID) { m_nID = newID; }
	int GetID() { return m_nID; }

// protected routines
protected:

// public data
private:

// protected data
protected:
	BOOL	m_bEnabled;
	int		m_nID;
	
// construction/destruction
public:
	CConvention();
	virtual ~CConvention();
};


#endif

