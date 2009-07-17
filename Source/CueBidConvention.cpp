//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CueBidConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "PlayerStatusDialog.h"
#include "CueBidConvention.h"


//
//==================================================================
// 
//
BOOL CCueBidConvention::ApplyTest(const CPlayer& player, 
								  const CConventionSet& conventions, 
								  CHandHoldings& hand, 
								  CCardLocation& cardLocation, 
								  CGuessedHandHoldings** ppGuessedHands,
								  CBidEngine& bidState, 
								  CPlayerStatusDialog& status)
{
	// basic test
//	if (!pCurrConvSet->IsConventionEnabled(tidCueBids))
//		return FALSE;

	// see if another convention is active
	if (CheckForOtherConventions(bidState))
		return FALSE;

	//
	if (TryCueBid(hand, bidState, status))
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
//---------------------------------------------------------------
//
// TryCueBid()
//
BOOL CCueBidConvention::TryCueBid(CHandHoldings& hand, CBidEngine& bidState,  CPlayerStatusDialog& status)
{
	// check basic requirements
	if (bidState.m_nAgreedSuit == NONE)
		return FALSE;

	// don't make a cue bid if we're already done with it 
	int nStatus = bidState.GetConventionStatus(this);
	if (nStatus != CONV_INACTIVE)
		return FALSE;

	// get adjusted point count as declarer
	// NCR BIG PROBLEM HERE - This changes a global variable. The changes are now always wanted ???
	bidState.fAdjPts = hand.RevalueHand(REVALUE_DECLARER, bidState.m_nAgreedSuit, TRUE);
	bidState.m_fMinTPPoints = bidState.fAdjPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPPoints = bidState.fAdjPts + bidState.m_fPartnersMax;
	bidState.m_fMinTPCPoints = bidState.fCardPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPCPoints = bidState.fCardPts + bidState.m_fPartnersMax;

	//
	// try a cue bid only if we have an interest in slam 
	// 
	if (bidState.m_fMinTPPoints < 30)
		return FALSE;

	// also, we shouldn't use cue bid if we have all four aces
	if (bidState.numAces == 4)
		return FALSE;

	// or if we're already at a slam level
	if (bidState.nPartnersBidLevel >= 6)
		return FALSE;
	int nLastBid = pDOC->GetLastValidBid();
	if (nLastBid >= bidState.GetGameBid(BID_SUIT(nLastBid)))
		return FALSE;

	// special test -- dont' cue bid if partner just raised a suit that we
	// shifted to artificially
	if ((bidState.nPartnersSuit == bidState.nPreviousSuit) && 
				( (ISSUIT(bidState.m_nAgreedSuit) && (bidState.nPreviousSuit != bidState.m_nAgreedSuit)) ||
				  (ISSUIT(bidState.m_nIntendedSuit) && (bidState.nPreviousSuit != bidState.m_nIntendedSuit)) ) )
		return FALSE;


	//
	// see what stage we're at
	//
	if (nStatus == CONV_INACTIVE)
	{
		//
		// showing first round controls 
		//
//		bidState.SetConventionStatus(this, CONV_INVOKED_ROUND1);
		// find cheapest ace
		int nBid;
		int nAgreedSuit = bidState.m_nAgreedSuit;
		int nSuit = GetCheapestAce(hand, nAgreedSuit);

		// 
		if ((nSuit != bidState.m_nAgreedSuit) && (nSuit != bidState.nPartnersSuit))
		{
			// found a suit to cue bid
			// the bid must be at a level that commits the partnership to game
			// i.e., for a major, it must be > 3 of the suit; for a minor, > 4
			nBid = bidState.GetCheapestShiftBid(nSuit);
			int nBidLevel = BID_LEVEL(nBid);
			// see if the bid is too high to qualify as a cue bid
		    if ( (ISMAJOR(nAgreedSuit) && (nBidLevel >= 5)) ||
			     (ISMINOR(nAgreedSuit) && (nBidLevel >= 6)) )
				return FALSE;
			// else if it's too low, adjust it upwards
		    if ( (ISMAJOR(nAgreedSuit) && (nBid < MAKEBID(nAgreedSuit,3))) ||
			     (ISMINOR(nAgreedSuit) && (nBid < MAKEBID(nAgreedSuit,4))) )
				nBid += 5;
			// alternatively, see if the bid is too high
			if (nBid > bidState.GetGameBid(nAgreedSuit))
				return FALSE;
			status << "CUET10! With " & bidState.m_fMinTPPoints & 
					  "+ pts and possible slam aspirations, make a cue bid, showing the cheapest ace (" & 
					  STS(nSuit) & ") with a bid of " & BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_INVOKED_ROUND1);
			return TRUE;
		}
		else
		{
			// either we don't have any aces, or the ace is in the trump suit
			// either way, we can't make a cue bid with this holding
			return FALSE;
		}
	}
	else if (nStatus == CONV_INVOKED_ROUND1)
	{
		//
		// showing second round controls, necesary if we want to proceed to a grand slam
		//
		bidState.SetConventionStatus(this, CONV_INVOKED_ROUND2);
		// find cheapest king or void suit
		int nSuit = GetCheapestKingOrVoid(hand, bidState.m_nAgreedSuit);
		int nPartnersBid = bidState.nPartnersBid;
		// 
		if (nSuit != bidState.m_nAgreedSuit) 
		{
			// found a suit to cue bid
			int nBid = bidState.GetCheapestShiftBid(nSuit);
			status << "CUET20! Make a second-round cue bid, showing the cheapest " &
					  ((hand.GetSuitLength(nSuit) == 0)? "void suit" : "king") &
					  " (in " & STS(nSuit) & ") with a bid of " & BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_INVOKED_ROUND2);
			return TRUE;
		}
		else
		{
			// either we don't have any second-round control to show,
			// so stop cue bidding and return to the trump suit
			int nBid = bidState.GetCheapestShiftBid(bidState.m_nAgreedSuit);
			status << "CUET24! With no second-round controls to cue bid, we have to return to the trump suit at a bid of " &
					  BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_INVOKED_ROUND2);
			return TRUE;
		}
	}
	else if (nStatus == CONV_INVOKED_ROUND2)
	{
		// both we and partner have shown first and second-round controls, so
		// it;s time to put up or shut up
	}
	else
	{	
		// error!
		AfxMessageBox("Error while attempting cue bid!");
		bidState.SetConventionStatus(this, CONV_ERROR);
	}
	//
	return TRUE;
}





//
//---------------------------------------------------------------
//
// RespondToConvention()
//
BOOL CCueBidConvention::RespondToConvention(const CPlayer& player, 
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

	// basic test -- see if we had agreed on a suit last time
	if (bidState.m_nAgreedSuit == NONE)
		return FALSE;

	// get status
	int nStatus = bidState.GetConventionStatus(this);

	//
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int nPartnersSuit = bidState.nPartnersSuit;
	int nPartnersPrevSuit = bidState.nPartnersPrevSuit;
	int nAgreedSuit = bidState.m_nAgreedSuit;
	int nSupportLevel = bidState.nPartnersSuitSupport;
	int nBid;

	// see if partner made a cue bid
	// it needs to be a bid that commits the partnership to game, 
	// after a suit has been agreed upon
	if ((nAgreedSuit != NONE) && (nPartnersSuit != NOTRUMP) && (nPartnersSuit != nAgreedSuit) &&
		((nPartnersBid > MAKEBID(nAgreedSuit,3)) && (nPartnersBid < bidState.GetGameBid(nAgreedSuit))))
	{
		//NCR what if suit was previously bid by this player???
		// EG: N->1H, S->2C, N->3C, S->3H  3H is NOT a cue bid, it's a response to this player's  previous bid
		// NCR how to test if partner's current bid was in suit previously bid ???
		// NCR what about a game bid???
		int nSuit =  BID_SUIT(nPartnersBid);
		for(int i=0; i < player.GetNumBidsMade(); i++) { // NCR had to add const to this function def
			int nSuitBid = pDOC->GetBidByPlayer(player.GetPosition(), i);
			if(BID_SUIT(nSuitBid) == nSuit)
				return FALSE;  // NCR not cue if partner bid before ???
		}
                                            // NCR-295 game bid in agreed suit???
		if(bidState.IsGameBid(nPartnersBid) && (nSuit == nAgreedSuit) ) // || CheckIfSuitBid(player, BID_SUIT(nPartnersBid))) 
		{
			return FALSE;  //NCR don't treat Game bid as cue bid
		}
		// met the requirements
		status << "CUR0! Partner made a cue bid of " & BTS(nPartnersBid) &
				  ", hinting at slam prospects.\n";
	}
	else
	{
		// this is not a cue bid
		return FALSE;
	}

	//
	// respond to a cue bid only if we have an interest in slam 
	// 
	// only qualify if we have 30+ team points, _OR_
	// strong trump support and 28+ poitns
	//
	if (bidState.m_fMinTPPoints >= 30)
	{
		// 30+ team points
		status << "2CUR1! And since we have " & 
				  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
				  "+ points in the partnership, we want to respond favorably.\n";
	}
	else if ((nSupportLevel >= SS_GOOD_SUPPORT) && 
								(bidState.m_fMinTPPoints >= 28))
	{
		// good support (4 good or 5 moderate cards)
		status << "2CUR2! And since we have " & bidState.SLTS(nPartnersSuit) &
				   " trump support, " & 
				   ((bidState.m_fMinTPPoints >= 30)? " as well as " : " albeit only with ") &
				   bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
				   " points in the partnership, we want to respond favorably.\n";
	}
	else
	{
		// insufficient strength for slam -- correct back to the agreed suit
		nBid = bidState.GetCheapestShiftBid(nAgreedSuit);
		status << "CUR4! But we don't have the points (only " & 
				  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
				  " pts in the partnership) or the excellent trump support needed for a marginal slam, so we decline by returning to the agreed suit at a bid of " &
				  BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;
	}

	//
	// else we're playing along -- find the cheapest response
	//
	if (nStatus == CONV_INACTIVE)
	{
		// first invocation -- find the cheapest Ace
		int nSuit = GetCheapestAce(hand, nPartnersSuit, bidState.m_nAgreedSuit);
		// found a suit with an ace?
		if ((nSuit != nPartnersSuit) && (nSuit != nAgreedSuit)) 
		{
			// found a suit to cue bid
			nBid = bidState.GetCheapestShiftBid(nSuit);
			status << "CUR20! Respond to partner's cue bid, showing our cheapest ace (" &
					  STS(nSuit) & ") with a bid of " & BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND1);
			return TRUE;
		} 
		else 
		{
			// found no ace, or else it's in the trump suit, so either way
			// just sign off at the agreed suit
			nBid = bidState.GetCheapestShiftBid(nAgreedSuit);
			if (hand.SuitHasCard(nSuit, ACE))
				status << "CUR22! But our only Ace is in the trump suit of " &
						  STSS(nAgreedSuit) & ", so sign off at a bid of " & BTS(nBid) & ".\n";
			else
				status << "CUR24! We have no other Aces to offer, so sign off with the agreed " &
						  STSS(nAgreedSuit) & " suit at a bid of " & BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return TRUE;
		}
	}
	else if (nStatus == CONV_RESPONDED_ROUND1)
	{
		// second invocation -- find cheapest King or void
		// first invocation -- find teh cheapest Ace
		int nSuit = GetCheapestKingOrVoid(hand, nPartnersSuit, bidState.m_nAgreedSuit);
		// found an appropriate suit?
		if ((nSuit != nPartnersSuit) && (nSuit != nAgreedSuit)) 
		{
			// found a suit to cue bid
			nBid = bidState.GetCheapestShiftBid(nSuit);
			status << "CUR30! Respond to partner's cue bid, showing our cheapest " &
					  ((hand.GetSuitLength(nSuit) > 0)? "King" : "void suit") &
					  " (in " & STS(nSuit) & ") with a bid of " & BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
//			bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return TRUE;
		} 
		else 
		{
			// found no king or void, or else found a king in the trump suit, 
			// so either way, just sign off at the agreed suit
			nBid = bidState.GetCheapestShiftBid(nAgreedSuit);
			if (hand.SuitHasCard(nSuit, KING))
				status << "CUR32! But our only Ace is in the trump suit of " &
						  STSS(nAgreedSuit) & ", so sign off at a bid of " & BTS(nBid) & ".\n";
			else
				status << "CUR34 We have no other Kings or void suits to offer, so sign off with the agreed " &
						  STSS(nAgreedSuit) & " suit at a bid of " & BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return TRUE;
		}
	}
	else
	{
		// error!
		bidState.SetConventionStatus(this, CONV_ERROR);
		return FALSE;
	}
}
/*
bool CheckIfSuitBid(const CPlayer& player, int nSuit) const {
	for(int i=0; i < player.GetNumBidsMade(); i++) {
		int nSuitBid = pDOC->GetBidByPlayer(player.GetPosition(), i);
		if(BID_SUIT(nSuitBid) == nSuit)
			return true;
	}
	return false;
}
*/




//
//---------------------------------------------------------------
//
// HandleConventionResponse()
//
BOOL CCueBidConvention::HandleConventionResponse(const CPlayer& player, 
												 const CConventionSet& conventions, 
												 CHandHoldings& hand, 
												 CCardLocation& cardLocation, 
												 CGuessedHandHoldings** ppGuessedHands,
												 CBidEngine& bidState,  
												 CPlayerStatusDialog& status)
{
	// check state
	int nStatus = bidState.GetConventionStatus(this);
	if (nStatus <= 0)
		return FALSE;

	//
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int nPartnersSuit = bidState.nPartnersSuit;
	int nPreviousBid = bidState.nPreviousBid;
	int nAgreedSuit = bidState.m_nAgreedSuit;
	int nBid;

	//
	// first check if partner returned to the trump suit, regardless of 
	// which cue bidding round this is 
	//
	if ( ((nStatus == CONV_INVOKED_ROUND1) || (nStatus == CONV_INVOKED_ROUND2)) &&
						(nPartnersSuit == nAgreedSuit) )
	{
		// first check for a strange response
		if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
		{
			// we don't understand partner's bid
			return CConvention::HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status);
		}

		// first check for a direct raise to slam by partner in response to our
		// cue bid(unlikely, but hey...)
		if (nPartnersBidLevel >= 6)
		{
			status << "HRCB0! In response to our cue bid, partner returned to the agreed " & STSS(nAgreedSuit) &
					  " trump suit with a slam bid at " & BTS(nPartnersBid) & ".\n";
			// raise to a grand slam if possible
			if ((nPartnersBidLevel == 6) && (bidState.m_fMinTPPoints >= 37))
			{
				nBid = MAKEBID(nAgreedSuit,7);
				status << "HRCB1! And with " &
						  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
						  " points in the partnership, go ahead and raise to " &
						  BTS(nBid) & ".\n";
			}
			else
			{
				// sign off on the slam
				nBid = BID_PASS;
				status << "HRCB2! And with " &
						  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
						  " points in the partnership, sign off on the slam bid and pass.\n";
			}
			//
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return TRUE;
		}

		//
		// otherwise partner returned to the agreed suit below slam, 
		// a discouraging sign
		//
		status << "HRCB5! Partner returned to the agreed " & STSS(nAgreedSuit) &
				  " trump suit in response to our cue bid of " & BTS(nPreviousBid) &
				  ", which is a discouraging sign.\n";
		// pass with < 33 team points
		if (bidState.m_fMinTPPoints < 33)
		{
			//
			nBid = BID_PASS;
			status << "HRCB6! And with only " & 
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " points in the partnership, we have to forget about slam and pass.\n" &
					  BTS(nBid) & ".\n";
		}
		else if (bidState.m_fMinTPPoints < 37)
		{
			// with 33-36 pts, bid small slam anyway
			nBid = MAKEBID(nAgreedSuit, 6);
			status << "HRCB7! But with " & 
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " points in the partnership, go ahead and bid a small slam anyway at " &
					  BTS(nBid) & ".\n";
		}
		else
		{
			// with 33-37 pts, bid a grand slam anyway
			nBid = MAKEBID(nAgreedSuit, 7);
			status << "HRCB8! But with " & 
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " points in the partnership, go ahead and bid a grand slam anyway at " &
					  BTS(nBid) & ".\n";
		}
		//
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;
	}



	//
	//-----------------------------------------------------------------
	//
	// else partner made a responding cue bid, an encouraging sign
	// now see what stage we're at
	//
	if (nStatus == CONV_INVOKED_ROUND1)
	{
		// first check for a strange response
		if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
		{
			// we don't understand partner's bid
			return CConvention::HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status);
		}

		//
		status << "HRCB40! Partner responded with a cue bid of " & BTS(nPartnersBid) &
				  ", showing an Ace in " & STS(nPartnersSuit) & ".\n";
		//
		// see if we want to proceed to a second round of cue bidding
		// we need 36+ pts and controls in all four suits
		//
		BOOL bAllFourControls;
		if ((bidState.numAces + 1) == 4)
		{
			status << "HRCB42! And with partner's " & STSS(nPartnersSuit) & 
					  ", we have all four Aces for full first-round control.\n";
			bAllFourControls = TRUE;
		}
		else
		{
			status << "HRCB43! But even with partner's " & STSS(nPartnersSuit) & 
					  ", it's not clear we have all four Aces for full first-round control.\n";
			bAllFourControls = FALSE;
		}

		//
		if ((bidState.m_fMinTPPoints < PTS_SLAM) || (!bAllFourControls))
		{
			// gotta stop below slam
			nBid = bidState.GetCheapestShiftBid(nAgreedSuit);
			status << "HRCB46! With only " & 
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " points in the partnership" &
					  ((bAllFourControls)? "," : "and without clear first round controls,") &
					  " we can't afford to bid slam, so settle for a contract of " & 
					  BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return TRUE;
		}
		// else we have >= 33 pts and all four controls
		else if (bidState.m_fMinTPPoints < 36)
		{
			// can only make a small slam
			nBid = MAKEBID(nAgreedSuit, 6);
			status << "HRCB46! But with only " & 
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " points in the partnership, we have to stop at a small with a contract of " &
					  BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return TRUE;
		}

		//
		// at this point, we have 36+ points, so trigger another round of cue bidding
		// find cheapest king or void suit
		//
		int nSuit = GetCheapestKingOrVoid(hand, bidState.m_nAgreedSuit);
		if (nSuit != nAgreedSuit) 
		{
			// found a suit to cue bid for second round
			nBid = bidState.GetCheapestShiftBid(nSuit);
			status << "HRCB60! Make a second-round cue bid, showing the cheapest " &
					  ((hand.GetSuitLength(nSuit) == 0)? "void suit" : "king") &
					  " (in " & STS(nSuit) & ") with a bid of " & BTS(nBid) & ".\n";
		}
		else
		{
			// either we don't have any second-round control to show, or the
			// second round card is in the trump
			// so stop cue bidding and return to the trump suit at a small slam
			nBid = MAKEBID(nAgreedSuit, 6);
			status << "HRCB80! With no second-round controls to cue bid, we have to return to the trump suit with a small slam at " &
					  BTS(nBid) & ".\n";
		}
		//
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

		//
		// both we and partner have shown first and second-round controls, 
		// so now it's time to put up or shut up
		//
		// if we like the controls and have enough points, bid grand slam;
		// else bid a small slam or game
		//
		//
		// see if we want to proceed to a second round of cue bidding
		// we need 36+ pts and controls in all four suits
		//
		BOOL bAllFourSecondRoundControls;
		if ((bidState.numKings + 1) == 4)
		{
			status << "HRCBS10! And with partner's " & STSS(nPartnersBid) & 
					  " bid showing a King or void, in " & STS(nPartnersSuit) &
					  ", we have scond-round control of all four suits.\n";
			bAllFourSecondRoundControls = TRUE;
		}
		else
		{
			status << "HRCBS12! But even with partner's " & STSS(nPartnersBid) & 
					  ", it's not clear we have all full second-round control.\n";
			bAllFourSecondRoundControls = FALSE;
		}

		//
		if ((bidState.m_fMinTPPoints < 32) || (!bAllFourSecondRoundControls))
		{
			// gotta stop below slam
			nBid = bidState.GetCheapestShiftBid(nAgreedSuit);
			status << "HRCBS20! With only " & 
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " points in the partnership " &
					  ((bAllFourSecondRoundControls)? "," : "and without clear second round controls,") &
					  " we can't afford to bid slam, so settle for a contract of " & 
					  BTS(nBid) & ".\n";
		}
		// else we have >= 32 pts and all four controls
		else if (bidState.m_fMinTPPoints < 36)
		{
			// can only make a small slam
			nBid = MAKEBID(nAgreedSuit, 6);
			status << "HRCBS24! But with only " & 
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " points in the partnership, we have to stop at a small with a contract of " &
					  BTS(nBid) & ".\n";
		}
		else
		{
			//
			// else we have 36+ points, so go ahead and bid a grand slam
			// 
			int nBid = MAKEBID(nAgreedSuit, 7);
			status << "HRCBS40! With a total of " &
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " points in the partnership and full first and second-round controls, go head and bid a grand slam at " &
					  BTS(nBid) & ".\n";
		}
		//
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;

	}
	else
	{	

		// N/A
		return FALSE;

	}
	//
	return TRUE;
}





//
//==================================================================
//
// Misc utility functions
//


//
// GetCheapestAce()
//
// returns the next cheapest suit with an Ace
// used for cue bidding
//
int CCueBidConvention::GetCheapestAce(CHandHoldings& hand, int nBaseSuit, int nSecondSuit)
{
	int i;
	//
	if (nBaseSuit < CLUBS)
		return NONE;
	//
	int nSuit;
	if (nBaseSuit == NOTRUMP)
		nSuit = CLUBS;
	else
		nSuit = GetNextSuit(nBaseSuit);
	for(i=0;i<4;i++) 
	{
		if (hand.SuitHasCard(nSuit, ACE) && (nSuit != nSecondSuit))
			break;
		nSuit = GetNextSuit(nSuit);
	}
	//
	if (i < 4)
		return nSuit;
	else
		return nBaseSuit;
}



//
// GetCheapestKingOrVoid()
//
// returns the next cheapest suit with a king or void
//
int CCueBidConvention::GetCheapestKingOrVoid(CHandHoldings& hand, int nBaseSuit, int nSecondSuit)
{
	int i;
	//
	if (nBaseSuit < CLUBS)
		return NONE;
	//
	int nSuit;
	if (nBaseSuit == NOTRUMP)
		nSuit = CLUBS;
	else
		nSuit = GetNextSuit(nBaseSuit);
	for(i=0;i<4;i++) 
	{
		// suit should be void or else have a king
		if ( ((hand.GetSuitLength(nSuit) == 0) || hand.SuitHasCard(nSuit, KING)) &&
			 (nSuit != nSecondSuit) )
			break;
		nSuit = GetNextSuit(nSuit);
	}
	//
	if (i < 4)
		return nSuit;
	else
		return nBaseSuit;
}





//
//==================================================================
// construction & destruction
//
CCueBidConvention::CCueBidConvention() 
{
	// from ConvCodes.h
	m_nID = tidCueBids;
}

CCueBidConvention::~CCueBidConvention() 
{
}


