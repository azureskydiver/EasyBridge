//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Convention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
//#include "EasyBdoc.h"
#include "Convention.h"
#include "PlayerStatusDialog.h"



//
//==================================================================
// constructon & destruction
CConvention::CConvention() 
{
	m_bEnabled = FALSE;
	m_nID = -1;
}

CConvention::~CConvention() 
{
}


//
void CConvention::Initialize() 
{
}

//
void CConvention::Terminate() 
{
}



//
BOOL CConvention::ApplyTest(const CPlayer& player, 
						    const CConventionSet& conventions, 
						    CHandHoldings& hand, 
						    CCardLocation& cardLocation, 
						    CGuessedHandHoldings** ppGuessedHands,
						    CBidEngine& bidState, 
						    CPlayerStatusDialog& status)
{
	// see if another convention is active
	if (CheckForOtherConventions(bidState))
		return FALSE;
	//
	if (TryConvention(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status))
		return TRUE;
	//
	if (RespondToConvention(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status))
		return TRUE;
	//
	if (HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status))
		return TRUE;
	// 
	return FALSE;
}



//
BOOL CConvention::CheckForOtherConventions(CBidEngine& bidState) 
{
	// check to see if another convention is present
	CConvention* pActiveConvention = bidState.GetActiveConvention();
	if ((pActiveConvention) && (pActiveConvention != this))
		return TRUE;
	else
		return FALSE;
}


//
BOOL CConvention::HandleConventionResponse(const CPlayer& player, const CConventionSet& conventions, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status) 
{
	// called here if the derived class did not understand the response
	int nPartnersBid = bidState.nPartnersBid;
	if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
	{
		// the convention is cancelled!
//		status << "4CNVX0: We do not understand partner's " & BTS(nPartnersBid) & 
//				  " in this context, so cancel the current convention.\n";
		bidState.SetConventionStatus(this, CONV_ERROR);
		return FALSE;
	}

	//
	if (bidState.GetConventionStatus(this) > 0)
		bidState.SetConventionStatus(this, CONV_INACTIVE);
	return FALSE;
}