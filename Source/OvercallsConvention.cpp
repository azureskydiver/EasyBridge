//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// OvercallsConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "PlayerStatusDialog.h"
#include "OvercallsConvention.h"
#include "HandHoldings.h"
#include "TakeoutDoublesConvention.h"
#include "ShutoutBidsConvention.h" // NCR-254
#include "ConventionSet.h"
#include "ConvCodes.h"

extern CTakeoutDoublesConvention takeoutDoublesConvention;

extern CShutoutBidsConvention		shutoutBidsConvention;  // NCR-254 for preemptive overcall


const int DidJumpShift = 1;  // NCR vs magic '1'

//
//===============================================================================
//
// TryConvention()
//
// check if we can overall the opponents' bid
//
BOOL COvercallsConvention::TryConvention(const CPlayer& player, 
									     const CConventionSet& conventions, 
									     CHandHoldings& hand, 
									     CCardLocation& cardLocation, 
									     CGuessedHandHoldings** ppGuessedHands,
									     CBidEngine& bidState,  
									     CPlayerStatusDialog& status)
{
	// see if we can overcall here
	// - this must be the first round
	// - partner must not have bid yet
	if ( ((bidState.nLHOBid > BID_PASS) || (bidState.nRHOBid > BID_PASS)) &&
		 (bidState.nPartnersBid <= BID_PASS) &&
 		 (bidState.m_numBidTurns == 0))
	{
		// passed the initial tests
		status << "2OVRCL0! Opponents have opened the bidding; see if we can overcall.\n";
	}
	else
	{
		// N/A
		return FALSE;
	}

	// NCR-362 Preempt if 7-8 cards and not an opening hand
    if ((bidState.numPrefSuitCards >= 7) && (bidState.fCardPts > OPEN_PTS(6)) 
		&& (bidState.fPts <= 11) // raw 11 or OPEN_PTS(11)?
		&& (bidState.nRHOBidLevel == 1) && (theApp.GetBiddingAgressiveness() >= 1) ) 
	{
		int nOCLevel = 3; // overcall level 3 unless 8+ or lowerSuit
		if (bidState.numPrefSuitCards > 7) //|| (bidState.nPrefSuit < bidState.nRHOSuit))
			nOCLevel = 4;
		int nBid = MAKEBID(bidState.nPrefSuit, nOCLevel);
		status << "OVRCSOa! With " & bidState.fCardPts & "/" & bidState.fPts 
				  & " points, a " & bidState.numPrefSuitCards
				  & "-card " & STSS(bidState.nPrefSuit) & " suit, make a preempt overcall to " & BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		// foreign invocation is kludgy, but hey...
		bidState.SetConventionStatus(this, CONV_SUBSUMED);
		bidState.SetConventionStatus(&shutoutBidsConvention, CONV_INVOKED);
		return TRUE;
	} // end NCR-362 preempting with long suit, weak hand

	
	// the basic requirements for a simple overcall are:
	// - a good 5+ card suit,
	// - 5 playing tricks at the 1-level, or 6 at the 2-level, and
	// - 8-15 points at the 1-level, or 10-15 at the 2 level (taken care of later)
	// NCR-319 >=8 pts and 6 card suit
	// NCR-532 >= 10 pts and a good 5 card suit (same as first ?)
	if ( ((bidState.fPts >= OPEN_PTS(8)) && ((bidState.numLikelyWinners >= 5)
		                                    || (bidState.numPrefSuitCards >= 6)) ) // NCR-319
         || ((bidState.fPts >= OPEN_PTS(10)) && (bidState.numPrefSuitCards >= 5) 
		     && (bidState.nSuitStrength[bidState.nPrefSuit] >= SS_GOOD_SUPPORT) )   // NCR-532 Open with 10 pts and a good suit
         || ((bidState.fPts >= OPEN_PTS(13)) && (bidState.numPrefSuitCards >= 5) ) ) // NCR-322 Opening hand w/5 cards 
	{
		 // passed the initial tests
	}
	else
	{
		// can't overcall -- but maybe can make some other bid
		status << "2OVRCL3! But we lack the strength with " & bidState.fPts & " pts for a simple overcall.\n";
		return FALSE;
	}

	//
	// first see what the opponents have bid
	//
	int nSuit,nOppSuit,nOppBid,nOppBidLevel;
	int nOppSuit2 = NONE; 
	if (bidState.nLHOBid > BID_PASS)	// get the first enemy suit
	{
		// LHO has bid
		nOppSuit = bidState.nLHOSuit;
		nOppBid = bidState.nLHOBid;
		nOppBidLevel = bidState.nLHOBidLevel;
		// see if RHO also bid, using a different suit
		if (bidState.nRHOBid > BID_PASS)
		{
			if (BID_SUIT(bidState.nRHOBid) != nOppSuit)
				nOppSuit2 = bidState.nRHOSuit;
			nOppBid = bidState.nRHOBid;
			nOppBidLevel = bidState.nRHOBidLevel;
		}
	}
	else
	{
		// only RHO has bid
		nOppSuit = bidState.nRHOSuit;
		nOppBid = bidState.nRHOBid;
		nOppBidLevel = bidState.nRHOBidLevel;
	}

	// see if opponents have bid too high for us to overcall
	if (nOppBidLevel >= 3)
		return FALSE;

	// see if we have a good 5-card suit that has not been bid yet
	nSuit = bidState.nPrefSuit;
	if ((bidState.numCardsInSuit[nSuit] >= 5) && 
		(ApplySuitStrengthTest(nSuit, nOppBid, bidState)) &&
		(bidState.nPrefSuit != nOppSuit) && (bidState.nPrefSuit != nOppSuit2))
	{
		// okeedokee, got a suit
	}
	else
	{
		// try the second-best suit
		nSuit = hand.GetSuitsByPreference(1);
		if ((bidState.numCardsInSuit[nSuit] >= 5) && 
			(ApplySuitStrengthTest(nSuit, nOppBid, bidState)) &&
			(bidState.nPrefSuit != nOppSuit) && (bidState.nPrefSuit != nOppSuit2)
			// NCR-645 don't use suit if same as RHO and Michaels
			&& !((nOppSuit == nSuit) && conventions.IsConventionEnabled(tidMichaels)) )
		{
			// we'll use the second suit
		}
		// NCR-576 One more chance: 5 cards and points
		else if((bidState.numCardsInSuit[bidState.nPrefSuit] >= 5) 
			    && (bidState.fPts > OPEN_PTS(14))
			    && (bidState.nPrefSuit != nOppSuit) && (bidState.nPrefSuit != nOppSuit2) ) 
		{
			// Use it
			nSuit = bidState.nPrefSuit; // NCR-576
		}
		else
		{
			// NCR-669 See if we can overcall 1NT
			bidState.m_fMinTPCPoints = bidState.fAdjPts; // Kludge for BidNoTrump()
			if (bidState.BidNoTrump(1, OPEN_PTS(pCurrConvSet->GetValue(tn1NTRangeMinPts)), 
									PTS_NT_GAME-4,FALSE, STOPPED_OPPBID))
			return TRUE;


			// failed to find an appropriate overcall suit
			int nLevel = (nSuit > BID_SUIT(nOppBid))? BID_LEVEL(nOppBid) : BID_LEVEL(nOppBid) + 1;
			if (nLevel == 1)
				status << "2OVRCL6! But we don't have a sufficiently strong overcall suit.\n";
			else
				status << "2OVRCL6a! But we don't have a suit strong enough to overcall with at the " &
						nLevel & "-level.\n";
			return FALSE;
		}
	}

	//
	// at this point, we've found an appropriate suit; 
	// so see what our overcall would be
	//
	int nBid,nBidLevel;

	// see if we should bid a jump overcall, strong or weak
	nBid = bidState.GetJumpShiftBid(nSuit, nOppBid);
	int testLvl = 3;  // NCR-125 allow level = 3 for minor
	if(ISMINOR(nSuit)) {
		testLvl = 4;  // allow jump shift to level 3 for minors
	}
	// NCR-525 Allow bid if opening bid was a weak two
	bool bOpenWasWeakTwo = (conventions.IsConventionEnabled(tidWeakTwoBids)
		                    && bidState.nRound == 0 && (nOppBid >= BID_2D || nOppBid <= BID_2S) );  // NCR-525

	if ((BID_LEVEL(nBid) < testLvl) || bOpenWasWeakTwo)  // NCR-125 use var  NCR-525 allow if weaktwo
	{
		// see if we can make a weak jump shift
		if (conventions.IsConventionEnabled(tidWeakJumpOvercalls))
		{
			// we're playing weak jump shifts, so see if the hand qualifies
			// need 6-10 pts, a decent 6-card suit, and 6+ winners(?)
			if ((bidState.fPts >= OPEN_PTS(6)) && (bidState.fPts <= OPEN_PTS(10)) && 
				(bidState.numCardsInSuit[nSuit] >= 6) && (bidState.nSuitStrength[nSuit] >= SS_OPENABLE))
//				(bidState.numLikelyWinners >= 6))
			{
				// yup, it does
				status << "OVRCW1! With " & bidState.fPts & " pts and a " & bidState.numCardsInSuit[nSuit] &
						  "-card " & STSS(nSuit) & " suit, make a weak jump overcall to " & BTS(nBid) & ".\n";
				bidState.SetBid(nBid);
				bidState.SetConventionStatus(this, CONV_INVOKED);
				bidState.SetConventionParameter(this, DidJumpShift);	// indicate that we jump shifted
				return TRUE;
			}
		}
		else
		{
			// only strong jump shifts are possible, so see if we can use it
			// need 15-18 pts, a decent 6-card suit, and 7+ winners
			if ((bidState.fPts >= OPEN_PTS(15)) && (bidState.fPts <= OPEN_PTS(18)) && 
				(bidState.numCardsInSuit[nSuit] >= 6) && (bidState.nSuitStrength[nSuit] >= SS_OPENABLE) &&
				(bidState.numLikelyWinners >= 7))
			{
				// yup, it does
				status << "OVRCS1! With " & bidState.fPts & " pts, a " & bidState.numCardsInSuit[nSuit] &
						  "-card " & STSS(nSuit) & " suit, and " & bidState.numLikelyWinners & 
						  " likely winners in the hand, make a strong jump overcall to " & BTS(nBid) & ".\n";
				bidState.SetBid(nBid);
				bidState.SetConventionStatus(this, CONV_INVOKED);
				bidState.SetConventionParameter(this, DidJumpShift);	// indicate that we jump shifted
				return TRUE;
			}
		}
	}

	// here, forget about jump overcalls
	bidState.ClearConventionParameter(this);		// no jump shift
	bidState.m_bGameForceActive = FALSE;   // NCR-625 No force now
	nBid = bidState.GetCheapestShiftBid(nSuit, nOppBid);
	nBidLevel = BID_LEVEL(nBid);

	//
	if (nBidLevel >= 3 && !bOpenWasWeakTwo)   // NCR-525 Allow if weaktwo
		return FALSE;	// can't overcall at the 3 level!

	// see if we meet the rqmts
	// need 10+ pts and 6 winners to overcall at the 2-level
	if ((nBidLevel == 2) && 
		(((bidState.fPts < OPEN_PTS(10)) || ((bidState.numLikelyWinners < 6) 
		                                    && (bidState.fPts < OPEN_PTS(13))))  // NCR-285 open with pts!
         && (bidState.numPrefSuitCards < 6) ) ) // NCR-319 overcall with 6+ cards
		return FALSE;

	// need 8+ pts and 5 winners to overcall at the 1-level
	if ((nBidLevel == 1)
		&& ((bidState.fPts < OPEN_PTS(8))           // NCR-460 Open with points  
		     || ((bidState.numLikelyWinners < 5) 
			     && ((bidState.fPts + bidState.fDistPts) < OPEN_PTS(11)))) ) // NCR-532 add in distribution
		return FALSE;

	// we're normally restricted to overcalling with 17 or fewer points
	if (bidState.fPts > OPEN_PTS(17))
	{
		// too strong for a simple overcall?
		if (pCurrConvSet->IsConventionEnabled(tidTakeoutDoubles))
		{
			// use a takeout double if it's available
			status << "OVRCTK! With " & bidState.fCardPts & "/" & bidState.fPts & 
					  " points, the hand is too strong for a simple overcall, so start by doubling.\n";
			bidState.SetBid(BID_DOUBLE);
			// foreign invocation is kludgy, but hey...
			bidState.SetConventionStatus(this, CONV_SUBSUMED);
			bidState.SetConventionStatus(&takeoutDoublesConvention, CONV_INVOKED);
			return TRUE;
		}
		else
		{
			// not playing takeout doubles, so overcall anyway
		}
	}

	// NCR-254 Preempt if 7-8 cards and not an opening hand
    if ((bidState.numCardsInSuit[nSuit] >= 7) && (bidState.fPts > OPEN_PTS(6)) 
		&& (bidState.fPts <= 11) // raw 11 or OPEN_PTS(11)?
		&& (nOppBidLevel == 1) ) 
	{
		int nOCLevel = 3; // overcall level 3 unless 8+ or lowerSuit
		if ((bidState.numCardsInSuit[nSuit] > 7) || (nSuit < nOppSuit))
			nOCLevel = 4;
		nBid = MAKEBID(nSuit, nOCLevel);
		status << "OVRCSO! With " & bidState.fCardPts & "/" & bidState.fPts 
				  & " points, a " & bidState.numCardsInSuit[nSuit]
				  & "-card " & STSS(nSuit) & " suit, make a preempt overcall to " & BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		// foreign invocation is kludgy, but hey...
		bidState.SetConventionStatus(this, CONV_SUBSUMED);
		bidState.SetConventionStatus(&shutoutBidsConvention, CONV_INVOKED);
		return TRUE;
	} // end NCR-254 preempting with long suit, weak hand

	//
	// whew! we've passed all the tests, so bid
	//
	CString strOppBid = BTS(nOppBid);
	status << "OVERCL8! Have a good " & bidState.numCardsInSuit[nSuit] & 
			  "-card " & STSS(nSuit) & " suit with " & 
			  bidState.fCardPts & "/" & bidState.fPts & 
			  " points and " & bidState.numLikelyWinners & " playing tricks, so overcall the opponents' " & 
			  (LPCTSTR)strOppBid & " with a bid of " & BTS(nBid) & ".\n";
	bidState.SetBid(nBid);
	bidState.SetConventionStatus(this, CONV_INVOKED);
	return TRUE;
}



//
// ApplySuitStrengthTest()
//
BOOL COvercallsConvention::ApplySuitStrengthTest(int nSuit, int nOppBid, CBidEngine& bidState)
{
	// apply the suit strength test for an overcall
	// the requirements are:
	// # cards in suit + # honors + # suit pts must be >= 14, 
	// e.g., KQJxx, AKxxx, AQxxxx, etc. are good enough,
	// but not KQxxx, AQxxx, AJxxxx, etc.
	double fCount = bidState.numCardsInSuit[nSuit] + 
						bidState.numHonorsInSuit[nSuit] +
								bidState.numSuitPoints[nSuit];  // NCR removed OPEN_PTS
	// NCR adjust further if number of cards > 5
	if(bidState.numCardsInSuit[nSuit] > 5)        // NCR-645 #cards > 4
		fCount += (bidState.numCardsInSuit[nSuit] - 4) * theApp.GetBiddingAgressiveness(); // NCR
	
	int nLevel;
	if (nSuit > BID_SUIT(nOppBid))
		nLevel = BID_LEVEL(nOppBid);
	else
		nLevel = BID_LEVEL(nOppBid) + 1;
	if (nLevel > 2)
		return FALSE;	// can't overcall at the 3 level here
	// need 12 "points" to overcall with the suit at the 1 level, and 14 at the 2 level
	if ( ((nLevel == 1) && (fCount >= OPEN_PTS(10))) ||  // NCR changed 12 to 10
		 ((nLevel == 2) && (fCount >= OPEN_PTS(12))) )   // NCR changed 14 to 12
		return TRUE;
	else
		return FALSE;
/*
	double fCount = bidState.numCardsInSuit[nSuit] + bidState.numHonorsInSuit[nSuit];
	if (fCount >= bidState.numLikelyWinners)
		return TRUE;
*/
}





//
//
//=========================================================
//
// see if partner made an overcall
//
//
BOOL COvercallsConvention::RespondToConvention(const CPlayer& player, 
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

	//
	// Bidding in response to partner's overcall? check requirements
	//
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersSuit = bidState.nPartnersSuit;
	int nPartnersSuitSupport = bidState.nPartnersSuitSupport;
	int numSupportCards = bidState.numSupportCards;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int numPartnerBidsMade = bidState.m_numPartnerBidsMade;
	int numPartnerBidTurns = bidState.m_numPartnerBidTurns;
	int nFirstBid = pDOC->GetValidBidRecord(0);

	// Requirements are:
	// - partner opened bidding for the team
	// - partner bid only once, at the 1 or 2 level
	// - at least one of the opponents bid ahead of partner
	if ( (bidState.m_bPartnerOpenedForTeam) &&
//				(numPartnerBidTurns == 1) &&
				(numPartnerBidsMade == 1) &&
				(nPartnersBidLevel >= 1) 
				&& ((nPartnersBidLevel <= 2) 
				    // NCR-131 allow J overcall at 3 level if pard's suit < LHO's
				    || ((nPartnersBidLevel == 3) && (nPartnersSuit < bidState.nLHOSuit))) 
				&& (nPartnersSuit != NOTRUMP) && 
				(nPartnersBid != nFirstBid))
	{
		// okay, met requirements
		bidState.m_bPartnerOvercalled = TRUE;
	}
	else
	{
		// 
		return FALSE;
	}

	//
	double fPts = bidState.fPts;
	double fAdjPts = bidState.fAdjPts;
	double fCardPts = bidState.fCardPts;
	int nPrefSuit = bidState.nPrefSuit;

	// state expectations
	if (nPartnersBidLevel == 1)
	{
		bidState.m_fPartnersMin = OPEN_PTS(8);
		bidState.m_fPartnersMax = OPEN_PTS(15);
		status << "ROVRC1! Partner overcalled at the 1-level, indicating a good 5+ card "
				  & STSS(nPartnersSuit) & " suit, 8-15 points, and probably 5+ playing tricks.\n";
	}
	else if ((nPartnersBidLevel == 2) || (nPartnersBidLevel == 3)) // NCR-131 added level 3
	{
		// partner overcalled at the 2-level -- see if it's a jump overcall
		BidType bidType = bidState.GetBidType(nPartnersBid); // NCR-346
//		int nGap = nPartnersBid - bidState.nLHOBid;
//		if (nGap > 5)
		if(bidType == (BT_Jump + BT_Overcall))  // NCR-346 Test if jump overcall
		{
			// this is indeed a jump overcall -- see if it's strong or weak
			if (/*(nGap > 5) &&*/ conventions.IsConventionEnabled(tidWeakJumpOvercalls))
			{
				// this is a weak jump overcall, 6-10 pts
				bidState.m_fPartnersMin = OPEN_PTS(6);
				bidState.m_fPartnersMax = OPEN_PTS(10);
				status << "ROVRC3! Partner made a weak jump overcall of " & bidState.szPB & 
						  ", indicating " & bidState.m_fPartnersMin & "-" & bidState.m_fPartnersMax & 
						  " pts and a good 6+ card " & STSS(nPartnersSuit) & " suit.\n";
			}
			else
			{
				// a strong jump overcall, 15-18 pts
				bidState.m_fPartnersMin = OPEN_PTS(15);
				bidState.m_fPartnersMax = OPEN_PTS(18);
				status << "ROVRC5! Partner made a strong jump overcall of " & bidState.szPB & 
						  ", indicating " & bidState.m_fPartnersMin & "-" & bidState.m_fPartnersMax & 
						  " pts, a good 6+ card " & STSS(nPartnersSuit) & " suit, and probably 6+ playing tricks.\n";
			}
		}
		else
		{
			// a simple 2-level overcall                    // NCR-739 13 pts at 3 level
			bidState.m_fPartnersMin = (nPartnersBidLevel == 3) ? OPEN_PTS(13) : OPEN_PTS(10);
			bidState.m_fPartnersMax = OPEN_PTS(15);
			status << "ROVRC5! Partner overcalled at the 2-level, indicating a good 5+ card "				  
					  & STSS(nPartnersSuit) & " suit, " & bidState.m_fPartnersMin & "-" & bidState.m_fPartnersMax & 
					  " points, and probably 6+ playing tricks.\n";
		}
	}
	else
	{
		// an overcall at the 3-level or above is always weak;
		// don't handle it here
		return FALSE;
	}

	//
	bidState.m_fMinTPPoints = fAdjPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPPoints = fAdjPts + bidState.m_fPartnersMax;
	bidState.m_fMinTPCPoints = fCardPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPCPoints = fCardPts + bidState.m_fPartnersMax;
	double fMinTPPoints = bidState.m_fMinTPPoints;
	double fMaxTPPoints = bidState.m_fMaxTPPoints;
	double fMinTPCPoints = bidState.m_fMinTPCPoints;
	double fMaxTPCPoints = bidState.m_fMaxTPCPoints;
	status << "ROVRC8! The total point count in the partnership is therefore " &
			  fMinTPPoints & "-" & fMaxTPPoints & " points.\n";

	//
	//---------------------------------------------------------------------
	// raise by one or to game with adequate support, or bid NT or
	// pass otherwise
	//

	//
	// with < 6 points, pass
	//
	if (fPts < OPEN_PTS(6))  // NCR-61 Added OPEN_PTS() to allow aggression here
	{
		status << "ROVRC10! This is not enough to bid any higher, so pass.\n";
		bidState.SetBid(BID_PASS);
		return TRUE;
	}


	//
	// see if we have decent support for partner
	//
	if (bidState.nPartnersSuitSupport >= SS_WEAK_SUPPORT)
	{
		// with < 20 total pts, pass
		if ((fMinTPPoints < OPEN_PTS(PTS_GAME-5)) && (nPartnersBid >= BID_1NT))  // NCR ie have to go to 2
		{
			status << "ROVRC20! This is insufficient to raise partner, so pass.\n";
			bidState.SetBid(BID_PASS);
			return TRUE;
		}

		// with 20-22 pts and 3 trumps, raise partner to the 2 level
		// NCR-110 Allow Raise to 2 with aggression by using PT_COUNT() below
		if (bidState.RaisePartnersSuit(SUIT_ANY,RAISE_TO_2, PT_COUNT(PTS_GAME-6), PTS_GAME-4,SUPLEN_3))
			return TRUE;
		// with 23-25.5 pts and 3 trumps, raise partner to the 3 level
		// NCR-332 Set min down if MAX high
		double minPts = (fMaxTPPoints >= PTS_GAME) ? PTS_GAME-4 : PTS_GAME-3; // NCR-600 0.5 vs 1
		if (bidState.RaisePartnersSuit(SUIT_ANY,RAISE_TO_3, minPts, PTS_GAME-0.5, SUPLEN_3)) // NCR-332 use minPts
			return TRUE;

		// with 26-31 pts and 3 trumps, raise partner to the 4 level     NCR-698 0.5 vs 1
		if (bidState.RaisePartnersSuit(SUIT_ANY,RAISE_TO_4, PTS_GAME, PTS_SLAM-0.5, SUPLEN_3))
			return TRUE;
		// or with 29-31 pts and 4 trumps, raise partner's minor to the 5 level   NCR-698 0.5 vs 1
		if (bidState.RaisePartnersSuit(SUIT_MINOR,RAISE_TO_5,PTS_MINOR_GAME,PTS_SLAM-0.5,SUPLEN_4))
			return TRUE;
		// else we have with 33+ pts, go straight to Blackwood
		// NCR-698 Changed below to fMinTPPoints from fPts
		if ((fMinTPPoints >= PTS_SLAM) && (bidState.numSupportCards >= 3))
		{
			bidState.InvokeBlackwood(nPartnersSuit);
			return TRUE;
		}
	}

	//
	// support for partner's suit was inadequate, 
	// so see if we have a strong suit to bid
	// need a good 6-card suit in order to bid it in preference to partner 
	//
	int nBid;
	int nLastBid = pDOC->GetLastValidBid();
	BOOL bJumped = FALSE;
	//
	if ((bidState.numPrefSuitCards >= 6)
		 && ((bidState.nPrefSuitStrength >= SS_STRONG)
		     // NCR-136 Allow bid if our suit below partner's 
		     || ((bidState.nPrefSuitStrength >= SS_OPENABLE) && (nPrefSuit < nPartnersSuit))) 
		 && (nPrefSuit != nPartnersSuit) &&
		(nPrefSuit != bidState.nLHOSuit) && (nPrefSuit != bidState.nRHOSuit) )
	{
		// got a suit we can bid -- see what level we would be bidding at
		// shift with < 16 pts, or jump shift with 16+ pts
		if (fPts < 16)
		{
			nBid = bidState.GetCheapestShiftBid(nPrefSuit, nLastBid);
		}
		else
		{
			// jump shift, but correct back to a cheap shift if we get past game
			nBid = bidState.GetJumpShiftBid(nPrefSuit, nLastBid, JUMP_SHIFT_ONE);
			if (nBid >= bidState.GetGameBid(nPrefSuit))
				nBid = bidState.GetCheapestShiftBid(nPrefSuit, nLastBid);
			if ((nBid - nLastBid) > 5)
				bJumped = TRUE;
		}
		//
		// NCR-136 Adjust Min points for test according to partner's possible pts
		const double nAdjMinPts = (bidState.m_fPartnersMax > 12) ? theApp.GetBiddingAgressiveness()*2 : 0;
		if (!bidState.IsBidSafe(nBid, nAdjMinPts)) // NCR-136 added nAdjMinPts
		{
			// cant' do it
			status << "ROVRC20! And although we want to bid our own " & 
					  bidState.numPrefSuitCards & "-card " & STSS(nPrefSuit) &
					  " suit, we can't bid it at the " & BID_LEVEL(nBid) & 
					  " with only " & fPts & " points, so pass.\n";
			bidState.SetBid(BID_PASS);
		}
		else
		{
			// else make the bid
			if (!bJumped)
				status << "ROVRC24! With a " & bidState.numPrefSuitCards & "-card " & 
						  STSS(nPrefSuit) & " suit, we bid it in preference to partner's " 
						  & STS(nPartnersSuit) & " at a bid of " & BTS(nBid) & ".\n";
			else
				status << "ROVRC26! With a " & bidState.numPrefSuitCards & "-card " & 
						  STSS(nPrefSuit) & " suit and " & fPts & 
						  " points, we jump in it in preference to partner's " 
						  & STS(nPartnersSuit) & " at a bid of " & BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
		}
		return TRUE;
	}

	//
	// see if we can bid NT
	//

	// bid 1NT with 16-22 total HCPs 
	// (the hand need not be balanced, nor do all suits need to be stopped)
	// NCR-110 Need Stoppers to bid 1NT unless agressive
	StoppedCode stopCode = ( theApp.GetBiddingAgressiveness() < 2) ? STOPPED_ALLOTHER : STOPPED_DONTCARE;
	// NCR-110 Use sCode from above vs STOPPED_DONTCARE
	if (bidState.BidNoTrump(1, OPEN_PTS(16), PTS_NT_GAME-4,FALSE, stopCode)) // NCR-69 added OPEN_PTS()
		return TRUE;
	// bid 2NT with 23-25 total HCPs and all suits stopped  // NCR-612 added .5 to close gap between 22 and 23
	if (bidState.BidNoTrump(2,PTS_NT_GAME-3.5,PTS_GAME-1,TRUE,STOPPED_ALLOTHER,nPartnersSuit))
		return TRUE;
	// bid 3NT with 26-32 total HCPs and all suits stopped
	if (bidState.BidNoTrump(3,PTS_NT_GAME,PTS_SLAM-1,TRUE,STOPPED_ALLOTHER,nPartnersSuit))
		return TRUE;
	// invoke Blackwood with 33+ total HCPs
	if ((fMinTPCPoints >= PTS_SLAM) && (bidState.bSemiBalanced))
	{
		bidState.InvokeBlackwood(NOTRUMP);
		return TRUE;
	}


	//
	// else with 16+ pts, poor support for partner, and a hand unsuitable for
	// NT, cue bid the enemy suit (but at no more than the 2-level)
	//
	if (fPts >= 16)
	{
		Suit nOppSuit = (Suit)bidState.nLHOSuit;
		if (nOppSuit == NOSUIT)
			nOppSuit = (Suit)bidState.nRHOSuit;
		// NCR-146 Check if this one is OK also
		if(nOppSuit != NOSUIT)
		{
			nBid = bidState.GetCheapestShiftBid(nOppSuit, nLastBid);
			status << "ROVRC60! With poor support for partner's " & STS(nPartnersSuit) &
					  " (holding " & bidState.szHP & 
					  "), no good 6-card suit of our own to bid, and a hand unsuited for NT, cue bid the opponents' " &
					  STSS(nOppSuit) & " suit at " & BTS(nBid) & ".  This bid is forcing.\n";
			// NCR-146 Set the bid here and return TRUE
			bidState.SetBid(nBid);
			return TRUE;
		}
	} // end have > 16 pts

	
	//
	// Otherwise we've fallen through, so pass
	//
	status << "ROVRC99! We have insufficient strength for either a raise, suit shift, or NT bid, so pass.\n";
	bidState.SetBid(BID_PASS);
	return TRUE;
}







//
//==========================================================
//
// Rebidding as opener after an overcall
//
//
BOOL COvercallsConvention::HandleConventionResponse(const CPlayer& player,  // NCR added BOOL
									  		   const CConventionSet& conventions, 
											   CHandHoldings& hand, 
											   CCardLocation& cardLocation, 
										       CGuessedHandHoldings** ppGuessedHands,
											   CBidEngine& bidState,  
											   CPlayerStatusDialog& status)
{
	if (bidState.GetConventionStatus(this) != CONV_INVOKED)
		return FALSE;
	bidState.ClearConventionStatus(this);

	//
	// estimate partner's strength
	//
	int nBid;
	double fPts = bidState.fPts;
	double fAdjPts = bidState.fAdjPts;
	double fCardPts = bidState.fCardPts;
	int nPrefSuit = bidState.nPrefSuit;
	int nPrefSuitStrength = bidState.nPrefSuitStrength;
	int nPreviousBid = bidState.nPreviousBid;
	int nPreviousSuit = bidState.nPreviousSuit;
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int nPartnersSuit = bidState.nPartnersSuit;
	int nPartnersSuitSupport = bidState.nPartnersSuitSupport;
	int numSupportCards = bidState.numSupportCards;
	int nPreviousBidLevel = BID_LEVEL(nPreviousBid);
	int nPresumedPts;	
	
	// determine what partner presumed we had
	if (nPreviousBidLevel == 1)
		nPresumedPts = 8;
	else 
	{
		if (bidState.GetConventionParameter(this) == DidJumpShift)
		{
			// we jump shifted last time
			if (conventions.IsConventionEnabled(tidWeakJumpOvercalls))
				nPresumedPts = 10;	// weak jump overcall
			else
				nPresumedPts = 15;	// strong jump overcall
		}
		else
		{
			// no jump shift, just a 2-level overcall
			nPresumedPts = 10;
		}
	}

	// first check for a strange response
	if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
	{
		// we don't understand partner's bid
		return CConvention::HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status);
	}

	// NCR-495 May need to check against earlier opponent bids for cue bid
	Suit EarlierOpponentSuit = NOSUIT;
	bool bPartnersBidWasCue = false;
	if(!ISSUIT(bidState.nLHOSuit) && !ISSUIT(bidState.nRHOSuit) 
		&& ((bidState.nRHONumBidsMade > 0) ||(bidState.nLHONumBidsMade > 0) ))
	{
		CPlayer* pOppBidder = (bidState.nLHONumBidsMade > 0) ? bidState.m_pLHOpponent : bidState.m_pRHOpponent;
		int oppsBid = pDOC->GetBidByPlayer(pOppBidder->GetPosition(),0);   // Should this look for more than one???
		EarlierOpponentSuit = Suit(BID_SUIT(oppsBid));
	} // end NCR-495 check for earlier bids

	// see what partner bid
	if (nPartnersBid == BID_PASS)
	{
		// partner passed, meaning he thought we had < 20 pts total
		bidState.m_fPartnersMin = 0;
		bidState.m_fPartnersMax = 19 - nPresumedPts;
	}
	else if (nPartnersSuit == nPreviousSuit)
	{
		// partner raised us
		if (nPartnersBidLevel == 2)
		{
			// 20-22 total pts assumed  NCR-567 Root pg 100 7-11 pts
			bidState.m_fPartnersMin = 7; //PTS_GAME-6 - nPresumedPts;
			bidState.m_fPartnersMax = 11; //PTS_GAME-4 - nPresumedPts;
		}
		else if (nPartnersBidLevel == 3)
		{
			// 23-25 total pts assumed // NCR-567 Root pg 100 12-14
			bidState.m_fPartnersMin = 12; //PTS_GAME-3 - nPresumedPts;
			bidState.m_fPartnersMax = 14; //PTS_GAME-1 - nPresumedPts;
		}
		else if (nPartnersBidLevel >= 4)
		{
			// 26-31 total pts assumed
			bidState.m_fPartnersMin = PTS_GAME - nPresumedPts;
			bidState.m_fPartnersMax = PTS_SLAM - nPresumedPts;
		}
		status << "OVRRB10! Partner raised our initial " & bidState.szPVB &
				  " overcall to " & bidState.szPB & ", indicating 3+ trumps and " &
				  bidState.m_fPartnersMin & "-" & bidState.m_fPartnersMax & 
				  " points.\n";
	}
	else if ((nPartnersSuit != nPreviousSuit) && 
					(nPartnersSuit != NOTRUMP) &&
					(nPartnersSuit != bidState.nLHOSuit) && 
					(nPartnersSuit != bidState.nRHOSuit)
					&& (nPartnersSuit != EarlierOpponentSuit) )  // NCR-495
	{
		// partner bid a new suit of his own
		// see if he jumped
		if ( (((nPartnersBid - nPreviousBid) > 5) && (bidState.bLHOPassed)) ||
			 ((!bidState.bLHOPassed) && ((nPartnersBid - bidState.nLHOBid) > 5)) )
		{
			// partner jumped
			bidState.m_fPartnersMin = 16;
			bidState.m_fPartnersMax = MIN(22, 40 - bidState.fCardPts);
			status << "OVRRB20! Partner jumped in his " & bidState.szPS & 
					  " suit in response to our " & bidState.szPVB & 
					  " overcall, indicating 6+ cards and approx. " &
					  bidState.m_fPartnersMin & "-" & bidState.m_fPartnersMax & 
					  " points.\n";
		}
		else
		{
			// partner didn't jump
			if (nPartnersBidLevel == 1)
			{
				// 16-19 total pts assumed
				bidState.m_fPartnersMin = 16 - nPresumedPts;
				bidState.m_fPartnersMax = 19 - nPresumedPts;
			}
			else if (nPartnersBidLevel == 2)
			{
				// 20-22 total pts assumed
				bidState.m_fPartnersMin = 20 - nPresumedPts;
				bidState.m_fPartnersMax = 22 - nPresumedPts;
			}
			else if (nPartnersBidLevel >= 3)
			{
				// 23-31 total pts assumed
				bidState.m_fPartnersMin = 23 - nPresumedPts;
				bidState.m_fPartnersMax = MIN(22, 40 - bidState.fCardPts);
			}
			status << "OVRRB22! Partner bid his own " & bidState.szPS & 
					  " suit at the " & nPartnersBidLevel & 
					  " level in response to our " & bidState.szPVB & 
					  " overcall, indicating 6+ cards and approx. " &
					  bidState.m_fPartnersMin & "-" & bidState.m_fPartnersMax & 
					  " points.\n";
		}
	}
	else if ((nPartnersSuit != nPreviousSuit) && 
					((nPartnersSuit == bidState.nLHOSuit)
					  || (nPartnersSuit == bidState.nRHOSuit) 
			          || (nPartnersSuit == EarlierOpponentSuit)) ) // NCR-495
	{
		// partner cue-bid an enemy suit
		bPartnersBidWasCue = true;  // NCR-495 remember for below
		bidState.m_fPartnersMin = 16;
		bidState.m_fPartnersMax = MIN(22, 40 - bidState.fCardPts);
		status << "OVRRB30! Partner cue bid the enemy " & bidState.szPS & 
				  " suit in response to our " & bidState.szPVB & 
				  " overcall, indicating poor trump support but 16+ points.\n";
	}
	else if (nPartnersSuit == NOTRUMP)
	{
		// partner bid notrumps
		if (nPartnersBidLevel == 1)
		{
			// 16-22 total pts assumed
			bidState.m_fPartnersMin = 16 - nPresumedPts;
			bidState.m_fPartnersMax = PTS_GAME-4 - nPresumedPts;
		}
		else if (nPartnersBidLevel == 2)
		{
			// 23-25 total HPCs assumed
			bidState.m_fPartnersMin = PTS_GAME-3 - nPresumedPts;
			bidState.m_fPartnersMax = PTS_GAME-1 - nPresumedPts;
		}
		else if (nPartnersBidLevel >= 3)
		{
			// 26-32 total pts assumed
			bidState.m_fPartnersMin = PTS_GAME - nPresumedPts;
			bidState.m_fPartnersMax = PTS_SLAM-1 - nPresumedPts;
		}
		status << "OVRRB40! Partner bid " & bidState.szPB & 
				  " in response to our " & bidState.szPVB & 
				  " overcall, indicating a balanced hand with approx. " &
				  bidState.m_fPartnersMin & "-" & bidState.m_fPartnersMax & 
				  " HPCs.\n";
	}

	// Tally up the points team has
	bidState.m_fMinTPPoints = fAdjPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPPoints = fAdjPts + bidState.m_fPartnersMax;
	bidState.m_fMinTPCPoints = fCardPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPCPoints = fCardPts + bidState.m_fPartnersMax;
	double fMinTPPoints = bidState.m_fMinTPPoints;
	double fMaxTPPoints = bidState.m_fMaxTPPoints;
	double fMinTPCPoints = bidState.m_fMinTPCPoints;
	double fMaxTPCPoints = bidState.m_fMaxTPCPoints;




	//
	//-----------------------------------------------------------------
	// Now determine a plan of action!
	//
	int nLastBid = pDOC->GetLastValidBid();
	int nLastBidLevel = BID_LEVEL(nLastBid);
	int nBidLevel;

	if (nPartnersBid == BID_PASS)
	{

		// pass unless we have 6+ cards and enuff pts to rebid the suit
		nBid = bidState.GetCheapestShiftBid(nPrefSuit, nLastBid);
		nBidLevel = BID_LEVEL(nBid);
		if (bidState.numPrefSuitCards < 6)
		{
			// not nuff cards
			nBid = BID_PASS;
		}
		else if (bidState.IsBidSafe(nBid))
		{
			// rebid the suit
			status << "OVRRB50! Despite partner's pass, we have the strength to rebid the suit, so rebid at " & 
					  BTS(nBid) & ".\n";
		}
		else
		{
			// no go
			nBid = BID_PASS;
		}

	}
	else if (nPartnersSuit == nPreviousSuit)
	{
		// partner raised -- try to raise again if possible

		// go to slam with 32+ pts
		if (fMinTPPoints >= PTS_SLAM)
		{
			bidState.InvokeBlackwood(nPrefSuit);
			nBid = bidState.m_nBid;
		}
		else if ( ((ISMAJOR(nPartnersSuit) && (fMinTPPoints >= PTS_GAME)) ||
				  (ISMINOR(nPartnersSuit) && (fMinTPPoints >= PTS_MINOR_GAME)))
				  // NCR-369 Don't go to game with worthless doubletons
				  && (!hand.HasWorthlessDoubleton() && (hand.GetNumDoubletons() < 2)) )
		{
			// make a game bid with enuff points
			nBid = bidState.GetGameBid(nPreviousSuit);
			status << "OVRRB60! With a total of approx. " &
					  fMinTPPoints & "-" & fMinTPPoints & 
					  " pts in the partnership, we can raise to game at " &
					  BTS(nBid) & ".\n";
		}
		else if(bidState.IsGameBid(nPartnersBid))  // NCR-732 Pass if at game
		{
			status << "OVRRB62! Partner bid game, so pass.\n";
			nBid = BID_PASS;
		}
		else 
		{
			// otherwise make a more modest response
			nBid = bidState.GetCheapestShiftBid(nPreviousSuit, nLastBid);
			if (bidState.IsBidSafe(nBid))
			{
				// it's safe to bid at this level
				status << "OVRRB66! With a total of approx. " &
						  fMinTPPoints & "-" & fMinTPPoints & 
						  " pts in the partnership, we can raise to" & 
						  ((nBid >= bidState.GetGameBid(nPreviousSuit))? " game at " : " ") &
						  BTS(nBid) & ".\n";
			}
			else
			{
				status << "OVRRB68! But we lack the strength to raise further, so pass.\n";
				nBid = BID_PASS;
			}
		}

	}
	else if (nPartnersSuit == NOTRUMP)
	{

		// partner bid NT
		if (bidState.bSemiBalanced)
		{
			// raise or pass NT if semi-balanced
			if (bidState.BidNoTrumpAsAppropriate(FALSE,STOPPED_DONTCARE))
			{
				nBid = bidState.m_nBid;
				status << "OVRRB70! We have a reasonably balanced hand and a total of " & 
							  fMinTPCPoints & "-" & fMaxTPCPoints &
							  " HPCs in the partnership, so raise to " & BTS(nBid) & ".\n";
			}
			else
			{
				// pass
				nBid = BID_PASS;
				status << "OVRRB72! We can accept a NT contract, and with a total of " &
						  fMinTPCPoints & "-" & fMaxTPCPoints &
					      " HPCs n the partnership, we accept partner's " & bidState.szPB & 
						  "bid and pass.\n";
			}

		}
		// NCR-172 bid game with 8 cards
		else if ((bidState.numPrefSuitCards >= 8) && (theApp.GetBiddingAgressiveness() >= 1))
		{
			nBid = bidState.GetGameBid(nPrefSuit);
			status << "OVRRB75! We don't have a balanced hand, but do have a " &
						  bidState.numPrefSuitCards & "-card " & bidState.szPVSS & 
						  " suit, so bid game at " & BTS(nBid) & ".\n";
		}
		else if (bidState.numPrefSuitCards >= 6)
		{
			// try to rebid a 6-card suit
			nBid = bidState.GetCheapestShiftBid(nPrefSuit, nLastBid);
//NCR			nBidLevel = BID_LEVEL(nBid);
			if (bidState.IsBidSafe(nBid))
			{
				status << "OVRRB76! We don't have a balanced hand, but do have a " &
						  bidState.numPrefSuitCards & "-card " & bidState.szPVSS & 
						  " suit, so rebid it at " & BTS(nBid) & ".\n";
			}
			else
			{
				// pass
				nBid = BID_PASS;
				status << "OVRRB78! We don't have a balanced hand, but also lack the strength to rebid our suit, so we have to pass.\n";
			}
		}
		else
		{
			// else pass
			nBid = BID_PASS;
			status << "OVRRB79! We don't have a balanced hand, but also lack the strength to rebid our suit or another suit, so we have to pass.\n";
		}

	}  // end partner's suit is NOTrump
	else
	{
		// else we have no suit agreement
		// NCR-57 Change/fix following to be nRebid and compute nRebidLevel from that
		int nRebid = bidState.GetCheapestShiftBid(nPrefSuit, nLastBid);
		int nRebidLevel = BID_LEVEL(nRebid);  // NCR-57 get level from bid
		// NCR-495 Was pard's bid a cue bid?
		if(bPartnersBidWasCue && (fMinTPPoints >= PTS_GAME-1)) {
			// NCR-756 rebid with minimum hand
			if(bidState.fCardPts < 12) {
				nBid = nRebid;   //NCR-756 use old suit
				status << "OVRRB77! With " & fMinTPCPoints & "-" & fMaxTPCPoints 
					       & " points, go back to our suit by bidding " 
					       & BTS(nBid) & ".\n";
			}else {
				int nNewSuit = bidState.GetNextBestSuit(nPrefSuit);  //  NCR-756 get next best
				nBid = bidState.GetCheapestShiftBid(nNewSuit, nLastBid);  // NCR-756 bid in new suit
			    status << "OVRRB77a! With " & fMinTPCPoints & "-" & fMaxTPCPoints 
				       & " points, bid new suit by bidding " 
				       & BTS(nBid) & ".\n";
  			}
		} 
		else 
		// raise partner with 2 trumps
		if (!bPartnersBidWasCue && (nPartnersBidLevel == 1) &&
			(bidState.RaisePartnersSuit(SUIT_ANY,RAISE_ONE,PTS_GAME-5,PTS_SLAM-1,SUPLEN_2)))
		{
			nBid = bidState.m_nBid;
			status << "OVRRB80! With a lack of better options, we begrudgingly raise partner to the 2-level with " &
					  bidState.numSupportCards & " cards at a bid of " & BTS(nBid) & ".\n";
		}
		else if (!bPartnersBidWasCue && (nPartnersBidLevel == 2) &&   /// NCR-256 added else here so above does not fall thru 
			(bidState.RaisePartnersSuit(SUIT_ANY,RAISE_ONE,PTS_GAME-3,PTS_SLAM-1,SUPLEN_2)))
		{
			nBid = bidState.m_nBid;
//			status << "OVRRB81! With a lack of better options, we begrudgingly raise partner to the 3-level with " &
//					  bidState.numSupportCards & " at a bid of " & BTS(nBid) & ".\n";
		}
		else if ( ((nRebidLevel == 2) && (fMinTPPoints >= PTS_GAME-3) && (bidState.numPrefSuitCards >= 6)) ||
				   ((nRebidLevel == 3) && (fMinTPPoints >= PTS_GAME-1) && (bidState.numPrefSuitCards >= 6)) )
		{
			// rebid a 6-card suit, if possible
			nBid = nRebid; // NCR-57 bidState.m_nBid;
			status << "OVRRB82! We lack the strength to raise partner, so rebid our own " & 
					  bidState.numPrefSuitCards & "-card suit at " & BTS(nBid) & ".\n";
		}
		// NCR-13 if at 3 level: Raise pard's suit if we have >=3 and points
		// NCR-351 Allow bid at 4 level
		else if(!bPartnersBidWasCue && (nPartnersBidLevel >= 3) && (hand.GetSuit(nPartnersSuit).GetNumCards() >= 3) 
			    && (fMinTPPoints >= PTS_GAME-1))
		{
			nBid = bidState.GetCheapestShiftBid(nPartnersSuit); // NCR-13 Raise pard 
			status << "OVRRB83! With a lack of better options, we raise partner with " &
					  bidState.numSupportCards & " cards at a bid of " & BTS(nBid) & ".\n";
		}
		else if (bidState.BidNoTrumpAsAppropriate(TRUE,STOPPED_ALLOTHER,nPartnersSuit))
		{
			// bid NT
			nBid = bidState.m_nBid;
//			status << "OVRRB84! With a semi-balanced hand, we prefer to bid NT instead of raising partner with weak trump support, so bid " &
//					   BTS(nBid) & ".\n";
		}
		// NCR-431 go to 4 level with points and if out suit < pard's
		else if ( (nRebidLevel == 4) && (fMinTPPoints >= PTS_GAME) && (bidState.numPrefSuitCards >= 5) 
			       && (nPrefSuit < nPartnersSuit) )
		{
			nBid = nRebid; 
			status << "OVRRB85! We have strength to rebid but don't like partner's suit, so rebid our own "
					  & bidState.numPrefSuitCards & "-card suit at " & BTS(nBid) & ".\n";
		}  // end NCR-431
		else
		{
			// else pass
			nBid = BID_PASS;
			status << "OVRRB88! As we lack agreement in suits and have only " &  // NCR changed # to 88 (vs 80)
					  fMinTPPoints & "-" & fMaxTPPoints &
				      " team points, we have no good options other than to pass.\n";
		}
	}


	
	//
	// done
	//
	bidState.SetBid(nBid);
	return TRUE;
}







//
//==================================================================
// construction & destruction
//
COvercallsConvention::COvercallsConvention() 
{
	// from ConvCodes.h
	m_nID = tidOvercalls;
}

COvercallsConvention::~COvercallsConvention() 
{
}


