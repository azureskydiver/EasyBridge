//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// SplinterBidsConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "PlayerStatusDialog.h"
#include "SplinterBidsConvention.h"




//
//---------------------------------------------------------------
//
// TryConvention()
//
//
BOOL CSplinterBidsConvention::TryConvention(const CPlayer& player, 
										    const CConventionSet& conventions, 
										    CHandHoldings& hand, 
										    CCardLocation& cardLocation, 
										    CGuessedHandHoldings** ppGuessedHands,
										    CBidEngine& bidState,  
										    CPlayerStatusDialog& status)
{
	// see if we can use SplinterBids here
	// requirements:
	// 1: partner must have opened bidding with 1 of a suit
	// 2: RHO must have passed
	// 3: this must be our first bidding opportunity
	// 4: we must have 4+ trumps and 13-18 pts
	// 4: need a singleton or void suit
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int nPartnersSuit = bidState.nPartnersSuit;
	int numPartnerBidsMade = bidState.m_numPartnerBidsMade;
	//
	if ( (nPartnersBidLevel == 1) && (nPartnersBid != BID_1NT) &&
				(bidState.m_bPartnerOpenedForTeam) && 
				(bidState.m_numPartnerBidsMade == 1) &&
				(bidState.bRHOPassed) &&
				(bidState.m_numBidTurns == 0) &&
				(bidState.numSupportCards >= 4) &&
				(bidState.fAdjPts >= 13) && (bidState.fAdjPts <= 18) &&
				((bidState.numVoids >= 1) || (bidState.numSingletons >= 1)) )
	{
		// met rqmts
	}
	else
	{
		// haven't met the rqmts
		return FALSE;
	}

	// might change this later, but for now don't respond with a splinter
	// on a minor suit
	if (ISMINOR(nPartnersSuit))
		return FALSE;

	//
	// determine the suit to respond in
	// if we have one or more void suits, pick the lowest one
	//
	int nSuit, nBid;

	if (bidState.numVoids >= 1)
	{
		// find the lowest void suit
		int i; // NCR-FFS added here, removed below
		for(/*int*/ i=0;i<4;i++)
			if (bidState.numCardsInSuit[i] == 0)
				break;
		nSuit = i;
	}
	else
	{
		// find the lowest singleton
		int i; // NCR-FFS added here, removed below
		for(/*int*/ i=0;i<4;i++)
			if (bidState.numCardsInSuit[i] == 1)
				break;
		ASSERT(i < 4);
		nSuit = i;
	}

	//
	// now make the bid
	// remember that a splinter is a double jump shift
	// 
	nBid = bidState.GetJumpShiftBid(nSuit, nPartnersBid, JUMP_SHIFT_2);
	status << "SPLINT! With " & bidState.fCardPts & "/" & bidState.fPts & "/" & bidState.fAdjPts &
			  " points and " & bidState.numSupportCards & "-card trump support " &
			  " (holding " & bidState.szHP & "), bid a splinter at " & BTS(nBid) &
			  " to show possible interest in slam.\n";
	//
	bidState.m_nAgreedSuit = nPartnersSuit;	// record suit agreement
	bidState.SetBid(nBid);
//	bidState.SetConventionStatus(this, CONV_INVOKED);
	return TRUE;
}







//
//---------------------------------------------------------------
//
// RespondToConvention()
//
//	 
BOOL CSplinterBidsConvention::RespondToConvention(const CPlayer& player, 
											      const CConventionSet& conventions, 
											      CHandHoldings& hand, 
											      CCardLocation& cardLocation, 
												  CGuessedHandHoldings** ppGuessedHands,
											      CBidEngine& bidState,  
											      CPlayerStatusDialog& status)
{
	// first see if another convention is active
	if (bidState.GetActiveConvention())
		return FALSE;

	// Responding to a splinter?
	// check conditions -- our prevous (opening) bid should have been 
	// 1 of a suit, with no interference from LHO, and partner's response 
	// (at his first bidding opportunity) should have been a double jump shift.
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int nPartnersSuit = bidState.nPartnersSuit;
	int nPreviousBid = bidState.nPreviousBid;
	int nPreviousBidLevel = bidState.nPreviousBidLevel;
	int nPreviousSuit = bidState.nPreviousSuit;
	int nJumpLevel = (nPartnersBid - nPreviousBid) / 5;
	//
	if ( (nPreviousBidLevel == 1) && (nPreviousBid != BID_1NT) && 
			   (bidState.bLHOPassed) &&
			   (bidState.m_numPartnerBidsMade == 1) && 
			   (bidState.m_numPartnerBidTurns == 1) &&
			   (nJumpLevel == 2) && 
			   (nPartnersSuit != bidState.nPreviousSuit) &&
			   (nPartnersSuit != NOTRUMP) ) 
	{
		// a Splinter response found
	} 
	else 
	{
		return FALSE;
	}

	// estimate partner's strength
	bidState.m_fPartnersMin = 13;
	bidState.m_fPartnersMax = MIN(22, 40 - bidState.fCardPts);
	bidState.m_nAgreedSuit = nPreviousSuit;

	// adjust our own point count to reflect the fit between the two hands
	double fAdjPts = bidState.fAdjPts = hand.RevalueHand(REVALUE_DECLARER, bidState.m_nAgreedSuit, TRUE);

	// special adjustment --
	// add 1 pt for each loser we hold in partner's void or singleton suit
	fAdjPts += hand.GetNumLosersInSuit(nPartnersSuit);

	// and sum the totals
	bidState.m_fMinTPPoints = fAdjPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPPoints = fAdjPts + bidState.m_fPartnersMax;
	bidState.m_fMinTPCPoints = bidState.fCardPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPCPoints = bidState.fCardPts + bidState.m_fPartnersMax;
	//
	double fMinTPPoints = bidState.m_fMinTPPoints;
	double fMaxTPPoints = bidState.m_fMaxTPPoints;

	//
	status << "SPLNTR! Partner's " & bidState.szPB & 
			  " bid is a splinter, indicating 13+ points, 4+ trumps, and a singleton or void in " &
			  STS(nPartnersSuit) & ".  The bid is invitational towards slam.\n";
	status << "2SPLNTRa! After adjusting for the fit with partner, we have a total of " &
			  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
			  " pts in the partnership.\n";


	// with a bare minimum holding, stop at game
	if (bidState.m_fMaxTPPoints < PTS_SLAM)
	{
		// can't hope to make a slam, so stop at game
		int nBid = bidState.GetGameBid(bidState.nPreviousSuit);
		status << "SPLNTR10! But with our weak hand, the maximum adjusted point count for both hands is " & 
				  fMinTPPoints & "-" & fMaxTPPoints &
				  " points, so stop at a game bid of " & BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
	}
	else
	{
		// otherwise bid on to slam
		status << "SPLNTR20! The total strength of both hands is " & 
				  fMinTPPoints & "-" & fMaxTPPoints &
				  " points, plus good distribution, so push on towards a slam.\n";
		bidState.InvokeBlackwood(bidState.nPreviousSuit);
	}
	// done
	bidState.SetConventionStatus(this, CONV_FINISHED);
	return TRUE;
}







//
//---------------------------------------------------------------
//
// HandleConventionResponse()
//
//	 
BOOL CSplinterBidsConvention::HandleConventionResponse(const CPlayer& player, 
												       const CConventionSet& conventions, 
												       CHandHoldings& hand, 
												       CCardLocation& cardLocation, 
													   CGuessedHandHoldings** ppGuessedHands,
												       CBidEngine& bidState,  
													   CPlayerStatusDialog& status)
{
	// see if the convention is active
	if (bidState.GetConventionStatus(this) != CONV_INVOKED)
		return FALSE;

	//
	// in response to our splinter, partner may have bid
	// Blackwood, returned to his original suit, or maybe cue bid a 
	// different suit.
	// but of course, if he bid Blackwood, we wouldn't have gotten here
	//
	int nPartnersSuit = bidState.nPartnersSuit;
	int nPartnersPrevSuit = bidState.nPartnersPrevSuit;
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;

	// first check for a strange response
	if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
	{
		// we don't understand partner's bid
		return CConvention::HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status);
	}

	//
	// so if partner did return to his suit to discourage a slam,
	// sign off on his bid
	//
	if ((nPartnersSuit == nPartnersPrevSuit) && (nPartnersBidLevel < 6))
	{
		status << "SPLNTRb1! Partner returned to his " & bidState.szPPSS &
				  " suit, showing no interest in slam, so sign off and pass.\n";
		bidState.SetBid(BID_PASS);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;
	}
	else
	{
		// otherwise he's shifting to a new suit -- may be a cue bid
		// so bail out of the splinter code
//		status << "SPLNTRb2! Partner shifted to a new suit after our splinter -- bailing out.\n";
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return FALSE;
	}
}










//
//==================================================================
// construction & destruction
//
CSplinterBidsConvention::CSplinterBidsConvention() 
{
	// from ConvCodes.h
	m_nID = tidSplinterBids;
}

CSplinterBidsConvention::~CSplinterBidsConvention() 
{
}



