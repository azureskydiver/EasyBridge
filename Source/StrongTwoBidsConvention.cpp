//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// StrongTwoBidsConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "PlayerStatusDialog.h"
#include "StrongTwoBidsConvention.h"
#include "ConventionSet.h"




//
//===============================================================================
//
// TryConvention()
//
// check if we can open with a strong two-bid
//
BOOL CStrongTwoBidsConvention::TryConvention(const CPlayer& player, 
											 const CConventionSet& conventions, 
											 CHandHoldings& hand, 
											 CCardLocation& cardLocation, 
											 CGuessedHandHoldings** ppGuessedHands,
											 CBidEngine& bidState,  
											 CPlayerStatusDialog& status)
{
	// basic requirements: no non-pass bid must have been entered yet
	if (bidState.nLastValidRecordedBid != BID_PASS)
		return FALSE;	// someone else has opened already
	
	// To open with a strong two bid, we need either a long, very good suit 
	// or 2 string suits, with 9+ playing tricks for majors (10+ for minors)
	// plus 4 quick tricks.
	// we also need a _minimum_ of 16 HCPs, no matter what
	if ( ((bidState.numAbsoluteSuits >= 1) || (bidState.numSolidSuits >= 2)) &&
		  (bidState.numLikelyWinners >= 9) && (bidState.numQuickTricks >= 4) &&
			  								(bidState.fCardPts >= 16)) 
	{
		// bid 2 of lowest openable solid suit
		double fPts = bidState.fPts;
		double fCardPts = bidState.fCardPts;
		int nBid = bidState.GetLowestOpenableBid(SUITS_ANY, OT_STRONG, 2);
		if ((nBid == BID_2C) && (pCurrConvSet->IsConventionEnabled(tidArtificial2ClubConvention)))
		{
			// we can't open a strong 2C when playing the 2Club convention,
			// so see if we can find another strong suit
			int nextBestSuit = bidState.nPrefSuitList[1];
			if (bidState.nSuitStrength[nextBestSuit] >= SS_STRONG)
			{
				nBid = MAKEBID(nextBestSuit ,2);
				status << "STR2C1! Have a strong Club suit and " & fPts & 
						  " points, but can't open at 2 Club, so we'll have to bid " & 
						  STS(nextBestSuit) & " instead.\n";
			}
			else
			{
				// can't bid at the 1-level
				status << "STR2C2! Have a strong Club suit and no others, but can't open 2C with only " &
						   fCardPts & " HCPs, so bid " & BTS(nBid) & " instead.\n";
				return FALSE;
			}
		}
		// a biddable suit has been established
		if (bidState.numSolidSuits > 1)
		{
			status << "STR2C10! Have " & bidState.numSolidSuits & " solid " & 
					  ((bidState.numSolidSuits>1)? "suits" :  "suit") &
					  " with " & bidState.numLikelyWinners & " playing tricks and " &
					  bidState.numQuickTricks & " quick tricks, so bid a strong " &
					  BTS(nBid) & ".\n";
		}
		else
		{
			status << "STR2C12! Have a solid " & STSS(bidState.nPrefSuit) & 
					  " suit with " & bidState.numLikelyWinners & 
					  " playing tricks and " & bidState.numQuickTricks & 
					  " quick tricks, so bid a strong " & BTS(nBid) & ".\n";
		}
		bidState.SetBid(nBid);
//		bidState.SetConventionStatus(this, CONV_INVOKED);
		return TRUE;
	}
	// failed the test
	return FALSE;
}







//
//===============================================================================
//
// RespondToConvention()
//
// partner bid at the 2-level (strong 2 bid)
//
// in general, partner's opening 2-bid denotes an extremely
// powerful hand.  So the question is whether to try for game
// or slam.  Therefore, we respond positively if we want attempt 
// a slam, or negatively for game
//
BOOL CStrongTwoBidsConvention::RespondToConvention(const CPlayer& player, 
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
	// Bidding in response to an opening strong 2 bid? check requirements
	//
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int nPartnersSuit = bidState.nPartnersSuit;
	int nPartnersSuitSupport = bidState.nPartnersSuitSupport;
	int numSupportCards = bidState.numSupportCards;
	int numPartnerBidsMade = bidState.m_numPartnerBidsMade;
	//
	//
	// partner must've bid at the 2 level, but not 2C,
	// and partner's bid must have been the first bid made
	//
	if ((nPartnersBidLevel == 2) && (nPartnersBid != BID_2C) &&
					(bidState.m_bPartnerOpenedForTeam) &&
					(numPartnerBidsMade == 1) &&
					(nPartnersBid == pDOC->GetValidBidRecord(0)))
	{
		// okay, met requirements
	}
	else
	{
		// 
		return FALSE;
	}

	//
	int nBid;
	double fPts = bidState.fPts;
	double fAdjPts = bidState.fAdjPts;
	double fCardPts = bidState.fCardPts;
	int nPrefSuit = bidState.nPrefSuit;
	int numPrefSuitCards = bidState.numPrefSuitCards;
	double numQuickTricks = bidState.numQuickTricks;

	// state expectations
	bidState.m_fPartnersMin = 16;
	bidState.m_fPartnersMax = 22;
	status << "RSTRT! Partner made a strong 2-bid, showing a very good suit or two solid suits, 9+ playing tricks, and 4+ quick tricks.  We have to respond positively if interested in slam, or negatively otherwise.\n";

	// set partnership point count minimums & maximums
	bidState.m_fMinTPPoints = fAdjPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPPoints = fAdjPts + bidState.m_fPartnersMax;
	bidState.m_fMinTPCPoints = fCardPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPCPoints = fCardPts + bidState.m_fPartnersMax;

	// the bid is forcing to game
	bidState.m_bGameForceActive = TRUE;		

	//
	// if we have less than 1 Quick trick, respond negatively
	//
	if (numQuickTricks < 1.0) 
	{
		nBid = BID_2NT;
		status << "RSTRT10! But with only " & numQuickTricks & 
				  " QT's, we have to make the negative response of " & BTS(nBid) & 
				  " to deny slam values.\n";
		bidState.SetBid(nBid);
		return TRUE;
	}

	//
	// else we have >= 1 Quick trick, so respond positively
	//
	status << "2RSTRT20! We have " & numQuickTricks & 
			  " QT's, so we can make a positive response.\n";

	// if we have even half-decent support for partner's suit, show it
	if (nPartnersSuitSupport >= SS_WEAK_SUPPORT) 
	{

		nBid = MAKEBID(nPartnersSuit,3);
		status << "RSTRT22! And we have " & bidState.SLTS(nPartnersSuit) & 
				  " support for partner's long " & bidState.szPSS &
				  " suit (holding " & bidState.szHP & 
				  "), so raise partner's bid to " & BTS(nBid) & ".\n";
	} 
	else if ((nPrefSuit != nPartnersSuit) &&
			   (bidState.nPrefSuitStrength >= SS_OPENABLE) &&
			   (numPrefSuitCards >= 5)) 
	{

		// or show our own suit if we have a good 5+ suiter
		int nSuit = nPrefSuit;
		// jump shift if the suits is really strong
		if (bidState.nPrefSuitStrength >= SS_ABSOLUTE) 
		{
			nBid = bidState.GetJumpShiftBid(nSuit,nPartnersBid);
			status << "RSTRT24! We lack good support for partner's " & 
					  bidState.szPSS & " suit (holding " & bidState.szHP & 
					  "), but we have an excellent " & 
					  bidState.LEN(nSuit) & "-card suit of our own in " & STS(nSuit) & 
					  " (holding " & bidState.SHTS(nSuit) & "), so show it in a bid of " &
					  BTS(nBid) & ".\n";
		} 
		else 
		{
			nBid = bidState.GetCheapestShiftBid(nSuit);
			status << "RSTRT26! But we lack good support for partner's " & 
					  bidState.szPSS & " suit (holding " & bidState.szHP & 
					  "), so show our preferred " & 
					  bidState.LEN(nSuit) & "-card " & STSS(nSuit) & " suit in a bid of " &
					  BTS(nBid) & ".\n";
		}

	} 
	else if ((bidState.bBalanced) && (bidState.m_fMinTPCPoints >= 26)) 
	{
		// here, we lack good support for partner's suit, and don't have a good
		// suit of our own.  so we jump to 3NT if we have a
		// balanced hand & 26+ total HCPs
		nBid = BID_3NT;
		status << "RSTRT28! But we lack good support for partner's " & 
				  bidState.szPSS & " suit (holding " & bidState.szHP & 
				  ") with a balanced hand, so jump to " &
				  BTS(nBid) & ".\n";
	} 
	else 
	{

		// here, we don't have good support for partner's
		// suit, nor a good suit of our own, nor a balanced
		// hand.  So bid 2NT in a negative response.
		nBid = BID_2NT;
		status << "RSTRT40! But unfortunately we have poor support for partner's " & 
				  bidState.szPSS & " suit (holding " & bidState.szHP & 
				  "), no good suit of our own, and an unbalanced hand, so we have to make the negative response of " & BTS(nBid) & ".\n";
	}
	//
	bidState.SetBid(nBid);
	return TRUE;
}






//
//==========================================================
//
// Rebidding as opener after a strong 2-level opening
//
//
BOOL CStrongTwoBidsConvention::HandleConventionResponse(const CPlayer& player, 
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
	int nPreviousSuit = bidState.nPreviousSuit;
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int nPartnersSuit = bidState.nPartnersSuit;
	int nPartnersSuitSupport = bidState.nPartnersSuitSupport;
	int numSupportCards = bidState.numSupportCards;
	BOOL bBalanced = bidState.bBalanced;

	//
	// did we get a negative response from partner?
	//
	if (nPartnersBid == BID_2NT) 
	{
		status << "2S2Rb0! After our strong " & bidState.szPVB & 
			" opening bid, partner's 2NT bid is a negative response, denying slam values (less than 1 Quick Trick).\n";

		// estimate points -- 0 to 6 for now
		bidState.m_fPartnersMin = 0;
		bidState.m_fPartnersMax = 6;
		bidState.m_fMinTPPoints = fAdjPts + bidState.m_fPartnersMin;
		bidState.m_fMaxTPPoints = fAdjPts + bidState.m_fPartnersMax;
		bidState.m_fMinTPCPoints = fCardPts + bidState.m_fPartnersMin;
		bidState.m_fMaxTPCPoints = fCardPts + bidState.m_fPartnersMax;

		// after a negative response, bid 3NT if balanced
		// with 26+ HCPs
		if ((bBalanced) && (bidState.m_fMinTPCPoints >= PTS_NT_GAME)) 
		{
			nBid = BID_3NT;
			status << "S2RB1! With a balanced distribution and " &
					  fCardPts & " HCPs in hand, rebid " & BTS(nBid) & ".\n";
		}
		// else show a second preferred suit if available
		if (bidState.numPreferredSuits > 0) 
		{
			int nSuit = bidState.GetRebidSuit(nPreviousSuit);
			nBid = bidState.GetCheapestShiftBid(nSuit);
			status << "S2RB4! With a good second suit in " &
					  STS(nSuit) & ", show it in a rebid of " & BTS(nBid) & ".\n";
		}
		// otherwise rebid our original suit (if not 2C)
		if (bidState.nPreviousBid == BID_2C) 
		{
			nBid = bidState.GetCheapestShiftBid(nPrefSuit);
			status << "S2RB6! With no other good suits, go ahead and bid our " &
					  bidState.szPrefS & " suit at " & BTS(nBid) & ".\n";
		} 
		else 
		{
			nBid = bidState.GetCheapestShiftBid(nPreviousSuit);
			status << "B3E32! With no other good suits, go ahead and rebid our " &
					  bidState.szPVSS & " suit at " & BTS(nBid) & ".\n";
		}
		//
		bidState.SetBid(nBid);
		return TRUE;
	}
	
	//
	// otherwise, got a positive response, and partner has shown 
	// his long suit -- so either raise partner's suit, bid NT, or
	// rebid our own suit
	//
	status << "2S2Rb20! After our strong " & bidState.szPVB &
			  " opening bid, partner's " & bidState.szPB & 
			  " bid was a positive response, indicating 1+ Quick Tricks.\n";

	// estimate points -- 3+ pts for now
	bidState.m_fPartnersMin = 3;
	bidState.m_fPartnersMax = MIN(22, 40 - fCardPts);
	bidState.m_fMinTPPoints = fAdjPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPPoints = fAdjPts + bidState.m_fPartnersMax;
	bidState.m_fMinTPCPoints = fCardPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPCPoints = fCardPts + bidState.m_fPartnersMax;

	// bid No Trump if balanced
	if (bBalanced) 
	{
		nBid = BID_3NT;
		status << "With a balanced hand, bid game at " & BTS(nBid) & ".\n";
	} 
	// see if there's a suit agreement from a strong 2 bid
	if ((nPreviousSuit == nPartnersSuit) && (bidState.nLastBid != BID_2C)) 
	{
		// try for a slam; use Blackwood
		status << "S2RB24! Partner's encouraging raise to " & bidState.szPB & 
				  " indicates 1+ Quick Tricks and slam possibilities.\n";
		bidState.InvokeBlackwood(nPartnersSuit);
		return TRUE;
	}
	// raise partner's suit if possible
	if (nPartnersSuitSupport >= SS_GOOD_SUPPORT) 
	{
		// double raise partner if major, or go directly to Blackwood
		// if minor (double raise from 3C or 3D would exceed 4NT
		if (ISMAJOR(nPartnersSuit))
		{
			bidState.m_nAgreedSuit = nPartnersSuit;
			nBid = MAKEBID(nPartnersSuit,nPartnersBidLevel+2);
			status << "S2RB28! With " & bidState.SLTS(nPartnersSuit) & 
					  " support for partner's " & bidState.szPS & 
					  " (holding " & bidState.szHP & 
					  "), go ahead and jump raise to " & BTS(nBid) & ".\n";
		}
		else
		{
			status << "S2RB32! We have " & bidState.SLTS(nPartnersSuit) & 
					  " support for partner's " & bidState.szPS & 
					  " (holding " & bidState.szHP & 
					  ").\n";
			bidState.InvokeBlackwood(nPartnersSuit);
			return TRUE;
		}
	}

	// else show a second preferred suit if available
	if (bidState.numPreferredSuits > 1) 
	{
		int nSuit = bidState.GetRebidSuit(nPreviousSuit);
		nBid = bidState.GetCheapestShiftBid(nSuit);
		status << "S2RB36! But we don't like partner's " & bidState.szPSS & 
				  " suit (holding " & bidState.szHP & 
				  "), and we hold a good second suit in " & STS(nSuit) & 
				  ", so show it in a rebid of " & BTS(nBid) & ".\n";
	}
	// otherwise rebid our original suit (if not 2C)
	if (bidState.nPreviousBid == BID_2C) 
	{
		nBid = bidState.GetCheapestShiftBid(nPrefSuit);
		status << "S2RB44! But we don't like partner's " &
				  bidState.szPSS & " suit (holding " & bidState.szHP & 
				  "), so bid our own " & bidState.szPrefS & 
				  " suit at " & BTS(nBid) & ".\n";
	} 
	else 
	{
		nBid = bidState.GetCheapestShiftBid(nPreviousSuit);
		status << "S2RB50! But we don't like partner's " &
				  bidState.szPSS & " suit (holding " & bidState.szHP & 
				  "), so rebid our own " & bidState.szPVSS & 
				  " suit at " & BTS(nBid) & ".\n";
	}
	//
	bidState.SetBid(nBid);
	return TRUE;
}







//
//==================================================================
// construction & destruction
//
CStrongTwoBidsConvention::CStrongTwoBidsConvention() 
{
	// from ConvCodes.h
	m_nID = tidStrongTwoBids;
}

CStrongTwoBidsConvention::~CStrongTwoBidsConvention() 
{
}


