//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DruryConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "Player.h"
#include "PlayerStatusDialog.h"
#include "DruryConvention.h"
#include "OvercallsConvention.h"
#include "ConventionSet.h"




//
//===============================================================================
//
// TryConvention()
//
// check if we can use an Drury Bid here
//
BOOL CDruryConvention::TryConvention(const CPlayer& player, 
									 const CConventionSet& conventions, 
									 CHandHoldings& hand, 
									 CCardLocation& cardLocation, 
									 CGuessedHandHoldings** ppGuessedHands,
									 CBidEngine& bidState,  
									 CPlayerStatusDialog& status)
{
	//
	// the requirements for an Drury Bid are:
	// 1: we must have passed at first opportunity
	// 2: Partner opens the bidding in 3rd of 4th seat
	// 3: Partner must have opened a major
	// 4: Partner must have bid at the 1-level
	// 5: we have 11-12 pts and 3+ card support
	//
	int nOpeningBid = pDOC->GetOpeningBid();
	int nPartnersBid = bidState.nPartnersBid;
	int numTotalBidTurns = pDOC->GetNumBidsMade();
	// test conditions 1 - 4
	if ( (bidState.m_numBidTurns == 1) && (bidState.nPreviousBid == BID_PASS) && 
		 ISBID(nOpeningBid) && (nOpeningBid == nPartnersBid) && 
		 ((numTotalBidTurns == 4) || (numTotalBidTurns == 5)) && 
		 (ISMAJOR(BID_SUIT(bidState.nPartnersBid))) && (bidState.nPartnersBidLevel == 1) )
	{
		 // passed the test
	}
	else
	{
		return FALSE;
	}

	// test condition #5
	int nSuit = BID_SUIT(bidState.nPartnersBid);
	double fPts = hand.RevalueHand(REVALUE_DUMMY, nSuit, TRUE);
	if ((fPts >= 11) && (fPts <= 12) && (bidState.numSupportCards >= 3))
	{
		// passed test #5
	}
	else
	{
		return FALSE;
	}
	
	//
	status << "DRUR1! Partner opened " & BTS(nPartnersBid) & " in " &
			  ((numTotalBidTurns == 4) ? "3rd" : "4th") & " position after our initial pass, so with " &
			  fPts & " pts and " & bidState.numSupportCards & 
			  "-card trump support, we can bid Drury at 2C.\n";
	bidState.SetBid(BID_2C);
	bidState.SetConventionStatus(this, CONV_INVOKED);
	return TRUE;
}





//
//-----------------------------------------------------
//
// respond to partner's Drury Bid
//
BOOL CDruryConvention::RespondToConvention(const CPlayer& player, 
										   const CConventionSet& conventions, 
										   CHandHoldings& hand, 
										   CCardLocation& cardLocation, 
										   CGuessedHandHoldings** ppGuessedHands,
										   CBidEngine& bidState,  
										   CPlayerStatusDialog& status)
{
	// first see if another convention is active
	if ((bidState.GetActiveConvention() != NULL) &&
					(bidState.GetActiveConvention() != this))
		return FALSE;

	//
	// make a responding bid
	//
	int nPartnersBid = bidState.nPartnersBid;
	int nPreviousBid = bidState.nPreviousBid;
	int numTotalBidTurns = pDOC->GetNumBidsMade();

	//
	int nBid;
	double fPts = bidState.fPts;
	double fCardPts = bidState.fCardPts;

	// 
	// see what round this is
	//
	int nStatus = bidState.GetConventionStatus(this);
	if (nStatus == CONV_INACTIVE)
	{
		//
		// Bidding in response to partner's Drury bid? check requirements
		//
		// the requirements for an Drury Bid are:
		// 1: we must have opened the bidding with 1 of a major in 3rd or 4th position
		// 2: Partner passed at first opportunity, then bid 2C
		int nOpeningBid = pDOC->GetOpeningBid();
		int nPartnersBid = bidState.nPartnersBid;
		int numTotalBidTurns = pDOC->GetNumBidsMade();

		// test conditions 1 - 4
		if ( (bidState.m_numBidTurns == 1) && (ISMAJOR(BID_SUIT(nPreviousBid))) && 
			 (BID_LEVEL(nPreviousBid) == 1) && (nOpeningBid == nPreviousBid) &&
			 ((numTotalBidTurns == 6) || (numTotalBidTurns == 7)) && 
			 (bidState.m_numPartnerBidTurns == 2) &&
			 (bidState.nPartnersOpeningBid == BID_PASS) && (nPartnersBid == BID_2C) )
		{
			 // passed the test
		}
		else
		{
			return FALSE;
		}

		status << "DRUR10! Partner has made a Drury bid of 2 Club, indicating " & 
				  OPEN_PTS(11) & "-" & OPEN_PTS(12) &" pts and 3+ card support.\n";

		// delay adjusting points until later count as declarer
		int nSuit = bidState.nPreviousSuit;
		bidState.SetAgreedSuit(nSuit);

		// partner has 11-12 pts
		bidState.AdjustPartnershipPoints(11, 12);

		// if we opened light, respond in the suit at the 2-level
		if (fCardPts <= PT_COUNT(12))
		{
			// we opened with <= 11 pts (actual, before adjusting for trump fit)
			nBid = MAKEBID(nSuit, 2);
			status << "DRUR12! But since we opened light with only " & fCardPts & 
					  & " HCPs, we have to respond in the original " & STSS(nSuit) & 
					  " suit at the 2-level and bid " & BTS(nBid) & ".\n";
		}
		else
		{
			// indicate 12+ HCPs
			nBid = BID_2D;
			status << "DRUR14! And since we opened with a legitemate " & fCardPts & 
					  " HCPs, we can respond to Partner's Drury with 2 Diamonds.\n";
		}

		// NOW adjust points as declarer
		fPts = bidState.fPts = hand.RevalueHand(REVALUE_DECLARER, nSuit, TRUE);

		// and return
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;

	}

	//
	return FALSE;
} 







//
//==========================================================
//
// Rebidding as opener after partner responds to an Drury Bid
//
BOOL CDruryConvention::HandleConventionResponse(const CPlayer& player, 
												const CConventionSet& conventions, 
												CHandHoldings& hand, 
												CCardLocation& cardLocation, 
												CGuessedHandHoldings** ppGuessedHands,
												CBidEngine& bidState,  
												CPlayerStatusDialog& status)
{
	// check status
	if ((bidState.GetConventionStatus(this) != CONV_INVOKED_ROUND1) &&
		(bidState.GetConventionStatus(this) != CONV_INVOKED_ROUND2))
		return FALSE;

	// get some info
	//
	int nBid = NONE;
	int nSuit = bidState.nPartnersPrevSuit;
	int numSupportCards = bidState.numSupportCards;

	//
	// handling partner's Drury response
	//
	int nStatus = bidState.GetConventionStatus(this);

	if (nStatus == CONV_INVOKED)
	{
		//
		// here, our actions depend on partner's response
		//
		if (bidState.nPartnersBid == BID_2D)
		{
			// partner opened with normal values (12+ HCPs)
			status << "DRUR40! Partner responded to our Drury with a bid of 2D, indicating a normal opening hand of " &
					  OPEN_PTS(12) & "+ HCPs.\n";

			// revalue hand
			double fPts = bidState.fAdjPts = hand.RevalueHand(REVALUE_DUMMY, nSuit, TRUE);
			bidState.AdjustPartnershipPoints(12, pCurrConvSet->GetValue(tn2ClubOpeningPoints));
			
			//
			if (bidState.m_fMinTPPoints >= PTS_MAJOR_GAME)
			{
				nBid = MAKEBID(nSuit, 4);
				status << "DRUR42! And with an adjusted count of " &  &
						  " pts in hand, for a total in the partnership of " &
						  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & 
						  " pts, we can go ahead and bid game at " & BTS(nBid) & ".\n";
			}
			else if (bidState.m_fMinTPPoints >= PTS_MAJOR_GAME - 3)
			{
				nBid = MAKEBID(nSuit, 3);
				status << "DRUR43! With an adjusted point of " &  &
						  " pts in hand, for a total in the partnership of " &
						  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & 
						  " pts, we can push to a bid of " & BTS(nBid) & ".\n";
			}
			else
			{
				nBid = MAKEBID(nSuit, 2);
				status << "DRUR44! But with an adjusted count of only " &  &
						  " pts in hand, for a total in the partnership of " &
						  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & 
						  " pts, we have to stop at " & BTS(nBid) & ".\n";
			}
		}
		else if (bidState.nPartnersBid == MAKEBID(nSuit, 2))
		{
			// partner bid 2 of the original suit
			nBid = BID_PASS;
			status << "DRUR46! Partner did his original " & STSS(nSuit) & 
					  " at the 2-level, indicating subpar opening values, so we have to pass.\n";
		}
		else
		{
			// partner made an illegal bid
			nBid = BID_PASS;
			status << "DRUR49! Partner did not respond with 2D or 2" & GetSuitLetter(nSuit) &
					  " to our Drury bid, so the convention is off and we have to pass.\n";
		}

		// done
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;

	}

	// oops!
	bidState.SetConventionStatus(this, CONV_ERROR);
	return FALSE;
}





//
//==================================================================
// construction & destruction
//
CDruryConvention::CDruryConvention() 
{
	// from ConvCodes.h
	m_nID = tidDrury;
}

CDruryConvention::~CDruryConvention() 
{
}


