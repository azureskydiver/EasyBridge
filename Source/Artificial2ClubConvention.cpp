//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Artificial2ClubConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "PlayerStatusDialog.h"
#include "Artificial2ClubConvention.h"
#include "ConventionSet.h"





//
//===============================================================================
//
// TryConvention()
//
// check if we can open with a strong 2C bid
//
BOOL CArtificial2ClubConvention::TryConvention(const CPlayer& player, 
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

	// check for a really strong hand with 23+ high card points 
	// or 10+ playing tricks 
	double fMinPts = pCurrConvSet->GetValue(tn2ClubOpeningPoints);
	if ((bidState.fCardPts >= fMinPts) || (bidState.numLikelyWinners >= 10)) 
	{
		int nBid = BID_2C;
		status << "STR2C! Have " & bidState.fCardPts & "/" & 
				   bidState.fPts &" points and " & bidState.numLikelyWinners & 
				   " playing tricks, so bid an artificial strong " & BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_INVOKED_ROUND1);
		return TRUE;
	}
	//
	return FALSE;
}





//
//-----------------------------------------------------
//
// handle an artificial 2C opening bid (23+ pts)
//
BOOL CArtificial2ClubConvention::RespondToConvention(const CPlayer& player, 
													 const CConventionSet& conventions, 
													 CHandHoldings& hand, 
													 CCardLocation& cardLocation, 
													 CGuessedHandHoldings** ppGuessedHands,
													 CBidEngine& bidState,  
													 CPlayerStatusDialog& status)
{
	// first see if another convention is active
	if (bidState.GetActiveConvention() &&
					(bidState.GetActiveConvention() != this))
		return FALSE;

	//
	// Bidding in response to an opening 2C bid? check requirements
	//
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersSuit = bidState.nPartnersSuit;
	int nPartnersSuitSupport = bidState.nPartnersSuitSupport;
	int numSupportCards = bidState.numSupportCards;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int numPartnerBidsMade = bidState.m_numPartnerBidsMade;
	int nBid;

	// see if this is our second response to partner's opening 2C bid
	if (bidState.GetConventionStatus(this) == CONV_RESPONDED)
	{
		// see if we responded 2D last time
		if (bidState.nPreviousBid == BID_2D)
		{
			status << "R2CLR2! Partner showed his best suit of " & bidState.szPS & 
					  " in response to our negative 2D response.\n";
			
			// what to do here???
			// if partner shows a suit, adjust point count as dummy
			double fAdjPts = bidState.fAdjPts = (ISSUIT(bidState.nPartnersSuit))? hand.RevalueHand(REVALUE_DUMMY, bidState.nPartnersSuit, TRUE) : bidState.fPts;
			double fCardPts = bidState.fCardPts;
		
			// adjust partnership point count minimums & maximums
			bidState.m_fMinTPPoints = fAdjPts + bidState.m_fPartnersMin;
			bidState.m_fMaxTPPoints = fAdjPts + bidState.m_fPartnersMax;
			bidState.m_fMinTPCPoints = fCardPts + bidState.m_fPartnersMin;
			bidState.m_fMaxTPCPoints = fCardPts + bidState.m_fPartnersMax;

			//
			if (bidState.m_fMinTPPoints <= PTS_GAME-2)
			{
				nBid = BID_PASS;
				status << "R2CLR10! But with only " & bidState.m_fMinTPPoints & " points in the partnership, we have to pass.\n";
			}
			else
			{
				// either shift to our own suit, raise partner to game, or bid 3NT
				if ((nPartnersSuitSupport >= SS_MODERATE_SUPPORT) && (nPartnersBid < bidState.GetGameBid(nPartnersSuit)))
				{
					nBid = bidState.GetGameBid(nPartnersSuit);
					status << "R2CLR20! Raise partner's & " & bidState.szPS &
							  " to game at "& BTS(nBid) & ".\n";
				}
				else if (bidState.IsSuitOpenable(bidState.nPrefSuit) && (nPartnersBidLevel <= 3))
				{
					nBid = bidState.GetCheapestShiftBid(bidState.nPrefSuit);
					status << "R2CLR22! Bid our own " & bidState.szPrefSS &
							  " suit in preference to partner's " & bidState.szPSS &
							  " suit .\n";
				}
				else if (nPartnersBid < BID_3NT)
				{
					nBid = BID_3NT;
					status << "R2CLR25! With only " & bidState.SLTS(nPartnersSuit) &
							  " support for partner and no good suit of our own, bid 3NT.\n";
				}
				else
				{
					nBid = BID_PASS;
					status << "R2CLR30! With only " & bidState.SLTS(nPartnersSuit) &
							  " support for partner and no good suit of our own, we have to pass.\n";
				}
			}
			// 
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return TRUE;
		}

		// mark this convention as completed
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return FALSE;
	}

	//
	//
	// partner must've bid 2 Club at his first opportunity,
	// and partner's bid must have been the first bid made
	//
	if ((nPartnersBid == BID_2C) && (bidState.m_bPartnerOpenedForTeam) &&
					(numPartnerBidsMade == 1) &&
					(nPartnersBid == pDOC->GetValidBidRecord(0)))
	{
		// condition valid
		// record that we responded
		bidState.SetConventionStatus(this, CONV_RESPONDED);
	}
	else
	{
		return FALSE;
	}

	// state expectations
	status << "R2CLUB! Partner made an artificial bid of 2 Clubs, showing either a game suit or " &
			  pCurrConvSet->GetValue(tn2ClubOpeningPoints) &
			  "+ points.  We want to respond positively if interested in a slam.\n";

	// the bid is forcing to game
	bidState.m_bGameForceActive = TRUE;		

	//
	double fPts = bidState.fPts;
	double fAdjPts = bidState.fAdjPts;
	double fCardPts = bidState.fCardPts;
	int nPrefSuit = bidState.nPrefSuit;
	int nPrefSuitStrength = bidState.nPrefSuitStrength;

	//
	bidState.AdjustPartnershipPoints(pCurrConvSet->GetValue(tn2ClubOpeningPoints), 40 - fCardPts);

	// respond negatively (2D) with < 33 points
	if (bidState.m_fMinTPPoints < PTS_SLAM) 
	{
		nBid = BID_2D;
		status << "R2C04! With only " & fCardPts & "/" & fPts &
				  " points, we deny interest in slam by making the negative response of " & BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		return TRUE;
	}


	//
	// else with 33+ points, state slam hopes
	//
	status << "2R2C08! With a total of " & bidState.m_fMinTPPoints & 
			  "+ points in the partnership, we want to invite slam.\n";

	// show an openable suit
	if (nPrefSuitStrength >= SS_OPENABLE) 
	{
		if (ISMAJOR(nPrefSuit))
			nBid = MAKEBID(nPrefSuit,2);
		else
			nBid = MAKEBID(nPrefSuit,3);
		status << "R2C14! With  " & fCardPts & "/" & fPts & " points in hand, and a total of " &
				  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & 
				  " points in the partnership, we express possible interest in slam by showing our best suit (" &
				  bidState.szPrefS & ") with a bid of " & BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		return TRUE;
	}

	// or a SOLID suit (jump shift)
	if (bidState.bPrefSuitIsSolid) 
	{
		if (ISMAJOR(nPrefSuit))
			nBid = MAKEBID(nPrefSuit,3);
		else
			nBid = MAKEBID(nPrefSuit,4);
		status << "R2C18! With " & fCardPts & "/" & fPts & " points in hand and a total of " &
				  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & 
				  " points in the partnership, we express interest in a slam by showing our solid " &
				  bidState.szPrefSS & " suit in a jump bid of " & BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		return TRUE;
	}

	// else assume a balanced hand
	// with a balanced hand & >= 26 total HCPs, bid 3NT; 
	// with < 26 total HCPs, 2NT
	if (bidState.bBalanced) 
	{
		if (bidState.m_fMinTPCPoints >= PTS_NT_GAME)
			nBid = BID_3NT;
		else
			nBid = BID_2NT;
		status << "R2C22! With " & fCardPts & 
				  " HCPs and a balanced hand, and a total of " & 
				  bidState.m_fMinTPCPoints & 
				  "+ HCPs in the partnership, we respond with " & BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		return TRUE;
	}

	// else just bid the best suit
	nBid = bidState.GetCheapestShiftBid(nPrefSuit, BID_2C);
	status << "R2C26! With " & fCardPts & "/" & fPts & 
			  " points but with no strong suit and an unbalanced hand, we have to respond with our best suit of " & 
			  bidState.szPrefS & " by bidding " & BTS(nBid) & ".\n";
	bidState.SetBid(nBid);
	return TRUE;

} // end of 2C opening section








//
//==========================================================
//
// Rebidding as opener after a strong 2-level opening
//
//
BOOL CArtificial2ClubConvention::HandleConventionResponse(const CPlayer& player, 
													      const CConventionSet& conventions, 
														  CHandHoldings& hand, 
														  CCardLocation& cardLocation, 
														  CGuessedHandHoldings** ppGuessedHands,
														  CBidEngine& bidState,  
														  CPlayerStatusDialog& status)
{
	int nStatus = bidState.GetConventionStatus(this);
	if (nStatus <= 0)
		return FALSE;

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
	BOOL bSemiBalanced = bidState.bSemiBalanced;


	// see if this is our first rebid
	if (nStatus == CONV_INVOKED_ROUND1)
	{
		// first check for a strange response
		if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
		{
			// we don't understand partner's bid
			return CConvention::HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status);
		}

		//
		// did we get a negative response? (2D in response to the 2C)
		//
		if (nPartnersBid == BID_2D) 
		{
			status << "CRB0! After our strong 2 Club opening, partner's 2 Diamond bid is a negative response, denying slam values (less than 1 Quick Trick).\n";

			// estimate points -- 0 to 6 for now
			bidState.m_fPartnersMin = 0;
			bidState.m_fPartnersMax = 6;
			bidState.m_fMinTPPoints = fAdjPts + bidState.m_fPartnersMin;
			bidState.m_fMaxTPPoints = fAdjPts + bidState.m_fPartnersMax;
			bidState.m_fMinTPCPoints = fCardPts + bidState.m_fPartnersMin;
			bidState.m_fMaxTPCPoints = fCardPts + bidState.m_fPartnersMax;

			// bid game or slam with 26+ pts
			if ((bidState.m_fMinTPCPoints >= PTS_NT_GAME) && (bidState.m_fMinTPCPoints < PTS_SMALL_SLAM))
			{
				if (bBalanced || bSemiBalanced)
				{
					nBid = BID_3NT;
					status << "CRB1! With a balanced distribution and " &
							  fCardPts & " HCPs in hand, rebid " & BTS(nBid) & ".\n";
				}
				else if ( (ISMAJOR(nPrefSuit) && (bidState.m_fMinTPPoints >= PTS_MAJOR_GAME)) ||
						  (ISMINOR(nPrefSuit) && (bidState.m_fMinTPPoints >= PTS_MINOR_GAME)) )
				{
					nBid = bidState.GetGameBid(nPrefSuit);
					status << "CRB2! With no help from partner, we have to unilaterally push on to game at " & BTS(nBid) & ".\n";
				}
				bidState.SetConventionStatus(this, CONV_FINISHED);
			}
			else if ((bidState.m_fMinTPCPoints >= PTS_SMALL_SLAM) && (bidState.m_fMinTPCPoints < PTS_GRAND_SLAM))
			{
				if (bBalanced || bSemiBalanced)
				{
					nBid = BID_6NT;
					status << "CRB4! With a balanced distribution and " &
							  fCardPts & " HCPs in hand, bid a slam at " & BTS(nBid) & ".\n";
				}
				else
				{
					nBid = MAKEBID(nPrefSuit, 6);
					status << "CRB5! With no help from partner but with a total of " & bidState.m_fMinTPPoints &
							  " points in the partnership, we have to unilaterally push on to a slam at " & BTS(nBid) & ".\n";
				}
				bidState.SetConventionStatus(this, CONV_FINISHED);
			}
			else if (bidState.m_fMinTPCPoints >= PTS_GRAND_SLAM)
			{
				if (bBalanced || bSemiBalanced)
				{
					nBid = BID_7NT;
					status << "CRB6! With a balanced distribution and " &
							  fCardPts & " HCPs in hand, bid a grand slam at " & BTS(nBid) & ".\n";
				}
				else
				{
					nBid = MAKEBID(nPrefSuit, 7);
					status << "CRB5! With no help from partner but with a total of " & bidState.m_fMinTPPoints &
							  " points in the partnership, we have to unilaterally push on to a grand slam at " & BTS(nBid) & ".\n";
				}
				bidState.SetConventionStatus(this, CONV_FINISHED);
			}
			else
			{
				// else show our preferred suit and see if partner bids again
				nBid = bidState.GetCheapestShiftBid(nPrefSuit);
				status << "CRB8! Show our strongest suit (" & bidState.szPrefS &
						  ") with a bid of " & BTS(nBid) & ".\n";
				bidState.SetConventionStatus(this, CONV_INVOKED_ROUND2);
			}
			//
			bidState.SetBid(nBid);
			return TRUE;
		}

		//
		// otherwise, we got a positive response, and partner has shown 
		// his long suit -- so either raise partner's suit, bid NT, or
		// rebid our own suit
		//
		status << "2CRB20! After our strong 2 Club opening, partner's " & bidState.szPB & 
				  " bid was a positive response, indicating 7+ pts and 1+ Quick Tricks.\n";

		// estimate points -- 7+ pts for now
		bidState.m_fPartnersMin = 7;
		bidState.m_fPartnersMax = MIN(22, 40 - fCardPts);
		bidState.m_fMinTPPoints = fAdjPts + bidState.m_fPartnersMin;
		bidState.m_fMaxTPPoints = fAdjPts + bidState.m_fPartnersMax;
		bidState.m_fMinTPCPoints = fCardPts + bidState.m_fPartnersMin;
		bidState.m_fMaxTPCPoints = fCardPts + bidState.m_fPartnersMax;

		// respond to partner's NT bid here
		// bid No Trump if reasonably balanced or have poor support
		if (nPartnersBid == BID_2NT) 
		{
			if ( bidState.bSemiBalanced ||
				 ((nPartnersSuitSupport <= SS_WEAK_SUPPORT) && (bBalanced)) )
			{
				if (bidState.m_fMinTPCPoints >= PTS_SLAM)
					nBid = BID_6NT;
				else
					nBid = BID_3NT;
				if (ISSUIT(nPartnersSuit))
					status << "CRB21! With a balanced hand and " & 
							  bidState.SLTS(nPartnersSuit) & " support for partner's " & bidState.szPS &
							  ", plus a total of " & bidState.m_fMinTPCPoints & "-" & bidState.m_fMaxTPCPoints &
							  " HCPs in the partnership, move to Notrump and bid " & 
							  ((BID_LEVEL(nBid) >= 6)? "slam" : "game") & " at " & BTS(nBid) & ".\n";
				else
					status << "CRB22! With a " & (!bBalanced? "semi-" : "") &
							  "balanced hand and a total of " & 
							  bidState.m_fMinTPCPoints & "-" & bidState.m_fMaxTPCPoints &
							  " HCPs in the partnership, raise partner to " & ((BID_LEVEL(nBid) >= 6)? "slam" : "game") &
							  " at " & BTS(nBid) & ".\n";
			}
			else
			{
				// we're not balanced, so show our preferred suit if possible
				nBid = bidState.GetCheapestShiftBid(nPrefSuit);
				status << "CRB25! We don't like Notrump, so so bid our " & 
						  bidState.szPrefS & " suit at " & BTS(nBid) & ".\n";
			}
			//
			bidState.SetBid(nBid);
			return TRUE;
		} 

		// raise partner's suit if possible
		if ((nPartnersSuit != NOTRUMP) && (nPartnersSuitSupport >= SS_GOOD_SUPPORT))
		{
			// double raise partner if major, or go directly to Blackwood
			// if minor (double raise from 3C or 3D would exceed 4NT
			if (ISMAJOR(nPartnersSuit))
			{
				bidState.m_nAgreedSuit = nPartnersSuit;
				nBid = bidState.GetGameBid(nPartnersSuit);
				BOOL bJumped = (nPartnersBidLevel == 2);
				status << "CRB28! With " & bidState.SLTS(nPartnersSuit) & 
						  " support for partner's " & bidState.szPS & 
						  " (holding " & bidState.szHP & 
						  "), go ahead and " & (bJumped? "jump" : "") & 
						  " raise to " & BTS(nBid) & ".\n";
				bidState.SetBid(nBid);
			}
			else
			{
				status << "CRB32! We have " & bidState.SLTS(nPartnersSuit) & 
						  " support for partner's " & bidState.szPS & 
						  " (holding " & bidState.szHP & "), so push towards slam.\n";
				bidState.InvokeBlackwood(nPartnersSuit);
				bidState.SetConventionStatus(this, CONV_FINISHED);
			}
			return TRUE;
		}

		// else show our preferred suit
		nBid = bidState.GetCheapestShiftBid(nPrefSuit);
		status << "CRB44! But we don't like partner's " & bidState.szPSS & 
				  " suit (holding " & bidState.szHP & "), so bid our own " & 
				  bidState.szPrefS & " suit at " & BTS(nBid) & ".\n";

		// done
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;

	}
	else if (nStatus == CONV_INVOKED_ROUND2)
	{
		// first check for a strange response
		if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
		{
			// we don't understand partner's bid
			return CConvention::HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status);
		}

		// partner bid another suit after our rebid of our suit 
		// AFTER her negative response to our opening strong 2C
		int nPartnersSuit = bidState.nPartnersSuit;
		if ((nPartnersSuit != NOTRUMP) && (nPartnersSuit != bidState.nPreviousSuit))
		{
			status << "CRB60! Partner bid " & bidState.szPB & " after we bid our " & bidState.szPVS & 
					  " suit, indicating a strong preference for the " & bidState.szPS & " suit.\n";
			
			// support partner's suit if we have decent holdings, else bid our own suit
			if (bidState.nPartnersBid < bidState.GetGameBid(nPartnersSuit))
			{
				// see if we like partner's suit
				if (bidState.nPartnersSuitSupport >= SS_GOOD_SUPPORT)
				{
					nBid = bidState.GetGameBid(nPartnersSuit);
					status << "CRB62! So with " & bidState.SLTS(nPartnersSuitSupport) & " support for partner's " &
							   bidState.szPS & ", raise to game at " & BTS(nBid) & ".\n";
				}
				else
				{
					nBid = bidState.GetCheapestShiftBid(bidState.nPreviousSuit);
					status << "CRB62! But with only " & bidState.SLTS(nPartnersSuitSupport) & " support for partner's " &
							   bidState.szPS & ", return to our own suit at " & BTS(nBid) & ".\n";
				}
			}
			else
			{
				// partner bid game!
				if (bidState.m_fMinTPPoints < PTS_SLAM)
				{
					// we don't have points for a slam, so pass
					nBid = BID_PASS;
					if (bidState.nPartnersBidLevel < 6)
						status << "CRB68! Partner's " & bidState.szPB & " bid is at game level, so pass.\n";
					else
						status << "CRB69! Partner's " & bidState.szPB & " bid is at slam level, so pass.\n";
				}
				else
				{
					// we do have the points for a slam!
					if (bidState.nPartnersBidLevel < 6)
					{
						nBid = BID_6NT;
						status << "CRB72! While we do not have suit agreement, we have the points for a slam, so bid " & 
								  BTS(nBid) & ".\n";
					}
					else
					{
						nBid = BID_PASS;
						status << "CRB74! Partner has bid slam in his suit, so pass.\n";
					}
				}
			}
		}
		else if (nPartnersSuit == NOTRUMP)
		{
			// partner bid NT
			// bid 3NT if possible
			if ( bidState.bSemiBalanced ||
				 ((nPartnersSuitSupport <= SS_WEAK_SUPPORT) && (bBalanced)) )
			{
				if (bidState.m_fMinTPCPoints >= PTS_SLAM)
					nBid = BID_6NT;
				else
					nBid = BID_3NT;
				if (ISSUIT(nPartnersSuit))
					status << "CRB76! With a balanced hand and " & 
							  bidState.m_fMinTPCPoints & "-" & bidState.m_fMaxTPCPoints &
							  " HCPs in the partnership, respond at " & 
							  ((BID_LEVEL(nBid) >= 6)? "slam" : "game") & " with a bid of " & BTS(nBid) & ".\n";
				else
					status << "CRB77! With a " & (!bBalanced? "semi-" : "") &
							  "balanced hand and a total of " & 
							  bidState.m_fMinTPCPoints & "-" & bidState.m_fMaxTPCPoints &
							  " HCPs in the partnership, raise partner to " & ((BID_LEVEL(nBid) >= 6)? "slam" : "game") &
							  " at " & BTS(nBid) & ".\n";
			}
			else
			{
				// we're not balanced, so show our preferred suit if possible
				nBid = bidState.GetCheapestShiftBid(nPrefSuit);
				status << "CRB78! We don't like Notrump, so so bid our " & 
						  bidState.szPrefS & " suit at " & BTS(nBid) & ".\n";
			}
			//
			bidState.SetBid(nBid);
			return TRUE;
		}
		else
		{
			// partner raised, which is strange!
			status << "CRB80! Partner raised our " & bidState.szPVSS & " suit, which is strange, ";
			if (bidState.nPartnersBid < bidState.GetGameBid(nPartnersSuit))
			{
				nBid = bidState.GetGameBid(bidState.nPartnersSuit);
				status << "but go ahead and raise to game.";
			}
			else
			{
				nBid = BID_PASS;
				status << "but since we're at game, stop here and pass.\n";
			}
		}

		// and we're done
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;
	}
	
	// shouldn't be here!
	return FALSE;

}






//
//==================================================================
// construction & destruction
//
CArtificial2ClubConvention::CArtificial2ClubConvention() 
{
	// from ConvCodes.h
	m_nID = tidArtificial2ClubConvention;
}

CArtificial2ClubConvention::~CArtificial2ClubConvention() 
{
}


