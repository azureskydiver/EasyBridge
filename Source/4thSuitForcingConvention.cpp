//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// 4thSuitForcingConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "PlayerStatusDialog.h"
#include "4thSuitForcingConvention.h"



//
//==================================================================
// 
//
BOOL C4thSuitForcingConvention::TryConvention(const CPlayer& player, 
											  const CConventionSet& conventions, 
											  CHandHoldings& hand, 
											  CCardLocation& cardLocation, 
											  CGuessedHandHoldings** ppGuessedHands,
											  CBidEngine& bidState, 
											  CPlayerStatusDialog& status)
{
	//
//	if (!pCurrConvSet->IsConventionEnabled(tid4thSuitForcing))
//		return FALSE;

	//
	// see if the conditions are right to apply this convention
	//
	int nPartnersSuit = bidState.nPartnersSuit;
	int nPartnersPrevSuit = bidState.nPartnersPrevSuit;
	int nFirstRoundSuit = bidState.nFirstRoundSuit;
	int nPreviousSuit = bidState.nPreviousSuit;
	if ((bidState.m_numBidsMade == 1) && 
		(bidState.m_numPartnerBidsMade == 2) && 
		(nPartnersSuit != nPartnersPrevSuit) && (nPartnersPrevSuit != NONE) &&
		(nFirstRoundSuit != nPreviousSuit) && (nFirstRoundSuit != NONE) &&
		(nPartnersSuit != NOTRUMP)) 
	{
		//
		// since we're playing 4th suit forcing, we can't pass 
		// partner's bid, no matter what
		//
		status << "SF0! Since we're playing fourth-suit forcing, we are required to respond to partner's bid of the 4th suit.\n";

		int nPartnersBid =  bidState.nPartnersBid;
		int nPartnersBidLevel =  bidState.nPartnersBidLevel;
		int nPrefSuit = bidState.nPrefSuit;
		int nPrefSuitStrength = bidState.nPrefSuitStrength;
		double fMinTPPoints = bidState.m_fMinTPPoints;
		double fMaxTPPoints = bidState.m_fMaxTPPoints;
		double fMinTPCPoints = bidState.m_fMinTPCPoints;
		double fMaxTPCPoints = bidState.m_fMaxTPCPoints;
		int nBid;

		//
		// choice 1: rebid a strong major, if it's our preferred suit, with 22+ pts
		//
		if ((ISMAJOR(nPrefSuit)) && (nPrefSuitStrength >= SS_STRONG) &&
												(fMinTPPoints >= 22))
		{
			nBid = bidState.GetCheapestShiftBid(nPrefSuit);
			status << "SF2! With no agreement in suits, along with " &
					  fMinTPPoints & "-" & fMaxTPPoints &
					  " partnership points, return to our preferred " & 
					  bidState.szPrefSS & " suit at " & BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
			return TRUE;
		}

		//
		// bid 3NT with no voids and 26+ HCPs
		//
		if ((bidState.numVoids == 0) && (nPartnersBidLevel <= 3) && 
										(fMinTPCPoints >= PTS_NT_GAME))
		{
			nBid = BID_3NT;
			status << "SF4! With no agreement in suits, but with " & 
					  fMinTPCPoints & "-" & fMaxTPCPoints &
					  " high card points in the partnership and no void suits, bid " &
					  BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
			return TRUE;
		}

		//
		// raise one of partner's suits with 23+ points and 2-card support
		//
		int nSuit;
		if (bidState.nPPrevSuitSupport > bidState.nPartnersSuitSupport)
			nSuit = SUIT_PREV;
		else
			nSuit = SUIT_ANY;

		// raise to game level with 26+ total pts
		if (bidState.RaisePartnersSuit(SUIT_PREV,RAISE_TO_NO_MORE_THAN_GAME,PTS_GAME,99,SUPLEN_2))
		{
			// the bid (m_nBid) is set automatically
			return TRUE;
		}

		// raise to 3-level with 23 pts
		if (bidState.RaisePartnersSuit(SUIT_PREV,RAISE_TO_3,PTS_GAME-3,99,SUPLEN_2))
		{
			return TRUE;
		}

		//
		// bid 2NT with no voids 
		//
		if ((bidState.numVoids == 0) && (nPartnersBidLevel <= 2))
		{
			nBid = BID_2NT;
			status << "SF4! With no agreement in suits, but with " & 
					  fMinTPCPoints & "-" & fMaxTPCPoints &
					  " high card points in the partnership and no void suits, bid " &
					  BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
			return TRUE;
		}

		//
		// else we've run out of other options, so 
		// raise partner's first or second suit with single-card support
		//
		if (bidState.numCardsInSuit[nPartnersPrevSuit] >= 1)
		{
			nBid = bidState.GetCheapestShiftBid(nPartnersPrevSuit);
			status << "SF20! With no better options, raise partner's first suit to " &
					  BTS(nBid) & ".\n";
		}
		else
		{
			nBid = MAKEBID(nPartnersSuit, nPartnersBidLevel+1);
			status << "SF24! With no better options, raise partner's second suit to " &
					  BTS(nBid) & ".\n";
		}
		// done
		bidState.SetBid(nBid);
		return TRUE;

	}
	else
	{

		// the 4th suit forcing convention didn't apply here
		return FALSE;

	}

}







//
//==================================================================
// construction & destruction
//
C4thSuitForcingConvention::C4thSuitForcingConvention() 
{
	// from ConvCodes.h
	m_nID = tidWeakTwoBids;
}

C4thSuitForcingConvention::~C4thSuitForcingConvention() 
{
}


