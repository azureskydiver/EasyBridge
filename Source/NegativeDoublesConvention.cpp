//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// NegativeDoubleConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "Player.h"
#include "PlayerStatusDialog.h"
#include "NegativeDoublesConvention.h"
#include "OvercallsConvention.h"
#include "ConventionSet.h"


extern COvercallsConvention	overcallsConvention;




//
//===============================================================================
//
// TryConvention()
//
// check if we can bid a takeout double here
//
BOOL CNegativeDoublesConvention::TryConvention(const CPlayer& player, 
											   const CConventionSet& conventions, 
											   CHandHoldings& hand, 
											   CCardLocation& cardLocation, 
											   CGuessedHandHoldings** ppGuessedHands,
											   CBidEngine& bidState,  
											   CPlayerStatusDialog& status)
{
	//
	// Negative doubles are made in the third position after
	// partner opens and RHO overcalls up to 2S.
	// The requirements for a negative doubles are:
	// 1: partner must have opened the bidding,
	// 2: we must either have passesed or not bid yet
	// 3: RHO must have overcalled at a level no higher than 2S, 
	// 4: have 4+ cards in each unbid major, (5+ at the 2-level), 
	// 5: holding 6+ points at the 1-level, or 11+ at the 2-level, and finally,
	// 6: don't have decent support for partner's suit

	// test conditions 1, 2, 3, and 5
	if ((bidState.m_bPartnerOpenedForTeam) && (bidState.m_numBidTurns <= 1) &&
		(bidState.nLHOBid <= BID_PASS) && (bidState.nRHOBid > BID_PASS) &&
		(bidState.nRHOBid <= BID_2S) && (bidState.fCardPts >= OPEN_PTS(6)))
	{
		 // passed the initial tests
	}
	else
	{
		return FALSE;
	}

	// test condition #6
	// if we can raise partner or shift over the opponent, 
	// don't bother with a negative double
	int nRHOBid = bidState.nRHOBid;
	int nRHOBidLevel = bidState.nRHOBidLevel;
	int nRHOSuit = bidState.nRHOSuit;
	//
	if (nRHOBidLevel == 1) 
	{
/*
		// see if we can raise partner at the 2-level
		if (pCurrConvSet->IsConventionEnabled(tidLimitRaises))
		{
			if ((bidState.numSupportCards >= 3) && (bidState.fPts >= OPEN_PTS(6)))
			{
				// we can raise partner at the 2-level, so forget the neg double
				return FALSE;
			}
			else if (bidState.numSupportCards >= 3) && (bidState.fPts >= OPEN_PTS(6)))
			{
				// likewise, w/o limit raises
				return FALSE;
			}
		}
*/
		if ((bidState.numSupportCards >= 3) &&
			(bidState.numSupportCards >= 3) && (bidState.fPts >= OPEN_PTS(6)))
		{
			// if we can raise partner at the 2-level, forget the neg double
			if (pCurrConvSet->IsConventionEnabled(tidLimitRaises) && 
						(bidState.fPts >= OPEN_PTS(11)) && (bidState.fPts < OPEN_PTS(13)))
				return FALSE;	// limit raise
			else if (!pCurrConvSet->IsConventionEnabled(tidLimitRaises) && 
						(bidState.fPts >= OPEN_PTS(13)) && (bidState.fPts < OPEN_PTS(17)))
				return FALSE;	// single raise
		}
		// check up to 2 suits
		for (int i=0;i<2;i++)
		{
			// see if we can switch to this suit
			int nSuit = hand.GetSuitsByPreference(i);
			if (bidState.IsSuitShiftable(nSuit) && (nSuit > nRHOSuit))
				return FALSE;
		}
	}
	//
	if (nRHOBidLevel == 2) 
	{
		// see if we can safely raise partner at the 2-level
		if ((bidState.numSupportCards >= 3) && (bidState.fPts >= OPEN_PTS(6)) && (bidState.nPartnersSuit > nRHOSuit))
		{
			// we can raise partner at the 2-level, so forget the neg double
			return FALSE;
		}
		// see if we can raise partner to the 3-level (either 11 or 13 pts)
		double nReqPts = pCurrConvSet->IsConventionEnabled(tidLimitRaises)? OPEN_PTS(11) : OPEN_PTS(13);
		if ((bidState.numSupportCards >= 4) && (bidState.fPts >= nReqPts))
		{
			// we can raise partner to the 3-level, so forget the neg double
			return FALSE;
		}
		// check up to 2 suits for a shift at the 2-level
		for (int i=0;i<2;i++)
		{
			// see if we can switch to this suit
			int nSuit = hand.GetSuitsByPreference(i);
			if (bidState.IsSuitShiftable(nSuit))
			{
				// we can shift to the suit, but see if we have the pts
				if ((nSuit < bidState.nPartnersSuit) && (bidState.fPts >= OPEN_PTS(10)))
					return FALSE;	// can shift to a lower suit at the 2-level
				else if ((nSuit > bidState.nPartnersSuit) && (bidState.fPts >= OPEN_PTS(19)))
					return FALSE;	// can (jump) shift to a higher suit at the 2-level
			}
		}
	}
	else if (nRHOBidLevel == 3) 
	{
		// opp. overcalled at 3-level; too high a level for a neg. double
		return FALSE;
	}

	// test condition 4
	// mark which majors are unbid
	int nMajorSuits[2] = { 1, 1 };
	if (bidState.nPartnersSuit == HEARTS)
		nMajorSuits[0] = 0;	// Hearts have been bid
	else if (bidState.nPartnersSuit == SPADES)
		nMajorSuits[1] = 0;	// spades have been bid
	//
	if (bidState.nRHOSuit == HEARTS)
		nMajorSuits[0] = 0;
	else if (bidState.nRHOSuit == SPADES)
		nMajorSuits[1] = 0;

	// if both majors have been bid, we can't use negative doubles
	if ((!nMajorSuits[0]) && (!nMajorSuits[1]))
		return FALSE;
	//
	if (bidState.nRHOBidLevel == 1)
	{
		// at the 1-level, need 4+ cards in each unbid major and 6+ pts
		if ( ((nMajorSuits[0]) && (hand.GetNumCardsInSuit(HEARTS) < 4)) || 
			 ((nMajorSuits[1]) && (hand.GetNumCardsInSuit(SPADES) < 4)) ||
			 (bidState.fPts < OPEN_PTS(6)))
			return FALSE;
	}
	else
	{
		// at the 2-level, need 5+ cards in each unbid major and 11+ pts
		if ( ((nMajorSuits[0]) && (hand.GetNumCardsInSuit(HEARTS) < 5)) || 
			 ((nMajorSuits[1]) && (hand.GetNumCardsInSuit(SPADES) < 5)) ||
			 (bidState.fPts < OPEN_PTS(11)))
			return FALSE;
	}

	//
	// finally, see if we have _too many_ points for a neg. double (19+)
	//
	if (bidState.fPts >= OPEN_PTS(19))
	{
//		status << "NEGDBLX! We have the holdings for a negative double, but our " &
//				  bidState.fPts & " points are too much\n";
		return FALSE;
	}

	//
	// we've now passed all the tests, so make the bid
	//
	if (bidState.nRHOBidLevel == 1)
		status << "NEGDBL! With " & bidState.fPts & "/" & bidState.fPts & 
				  " points and 4 cards in the unbid majors, bid a negative double.\n";
	else
		status << "NEGDBL! With " & bidState.fPts & "/" & bidState.fPts & 
				  " points and 5+ cards in the unbid majors, bid a negative double.\n";
	int nBid = BID_DOUBLE;
	bidState.SetBid(nBid);
//	bidState.SetConventionStatus(this, CONV_INVOKED);
	bidState.SetConventionStatus(this, CONV_FINISHED);
	return TRUE;
}





//
//-----------------------------------------------------
//
// respond to partner's negative double
//
BOOL CNegativeDoublesConvention::RespondToConvention(const CPlayer& player, 
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
	// Bidding in response to partner's negative bid? check requirements
	//
	// the identifying marks of a negative double are:
	// 1: this must be round #1 or 2
	// 2: we must have opened bidding, and 
	// 3: LHO must have bid over our initial bid, up to 2S
	//

	int nBid;
	int nPartnersBid = bidState.nPartnersBid;
	int nMajorSuits[2] = { 1, 1 };
	if (bidState.nPreviousSuit == HEARTS)
		nMajorSuits[0] = 0;
	else if (bidState.nPreviousSuit == SPADES)
		nMajorSuits[1] = 0;
	//
	if (bidState.nLHOSuit == HEARTS)
		nMajorSuits[0] = 0;
	else if (bidState.nLHOSuit == SPADES)
		nMajorSuits[1] = 0;

	// apply tests #1, 2, and 3
	if ( (nPartnersBid == BID_DOUBLE) &&
		 ((bidState.nRound == 0) || (bidState.nRound == 1)) && 
		 (bidState.m_bOpenedBiddingForTeam) && 
		 (bidState.nLHOBid > BID_PASS) && (bidState.nLHOBid <= BID_2S))
	{
		//
		status << "NEGDR! Partner has bid a negative double, indicating 4+ cards in the unbid majors " &
				  ((nMajorSuits[0] && nMajorSuits[1])? "(Hearts and Spades)" :
					nMajorSuits[0]? "(Hearts)" : "(Spades)") & ".\n";
	}
	else
	{
		return FALSE;
	}

	//
	// estimate partner's strength
	//
	if (bidState.nLHOBidLevel == 1)
	{
		// 6-9 pts for a neg dbl the 1-level
		bidState.m_fPartnersMin = OPEN_PTS(6);
		bidState.m_fPartnersMax = Min(OPEN_PTS(9),40 - bidState.fCardPts);
		bidState.m_fMinTPPoints = bidState.fPts + bidState.m_fPartnersMin;
		bidState.m_fMaxTPPoints = bidState.fPts + bidState.m_fPartnersMax;
		bidState.m_fMinTPCPoints = bidState.fCardPts + bidState.m_fPartnersMin;
		bidState.m_fMaxTPCPoints = bidState.fCardPts + bidState.m_fPartnersMax;
		status << "2NEGDR8! Partner's negative double indicates " &  
				  bidState.m_fPartnersMin & "-" & bidState.m_fPartnersMax &
				  " points, for a total in the partnership of " &
				  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & 
				  " points.\n";
	}
	else
	{
		// 11-18 pts for a neg double at the 2 level
		bidState.m_fPartnersMin = OPEN_PTS(11);
		bidState.m_fPartnersMax = Min(OPEN_PTS(18),40 - bidState.fCardPts);
		bidState.m_fMinTPPoints = bidState.fPts + bidState.m_fPartnersMin;
		bidState.m_fMaxTPPoints = bidState.fPts + bidState.m_fPartnersMax;
		bidState.m_fMinTPCPoints = bidState.fCardPts + bidState.m_fPartnersMin;
		bidState.m_fMaxTPCPoints = bidState.fCardPts + bidState.m_fPartnersMax;
		status << "2NEGDRT8! Partner's negative double indicates 11+ pts, for a total in the partnership of " &
				  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & 
				  " points.\n";
	}

	//
	// and respond to the bid
	//
	int nSuit;
	double fCardPts = bidState.fCardPts;
	double fPts = bidState.fPts;
	int nLHOBid = bidState.nLHOBid;
	int nRHOBid = bidState.nRHOBid;

	//
	// with support for one or the other major, raise
	// but discount the majors already bid by us or LHO
	//
	int numHearts = hand.GetNumCardsInSuit(HEARTS);
	int numSpades = hand.GetNumCardsInSuit(SPADES);

	// gotta beat RHO's bid if possible
	int nTopBid = nLHOBid;
	if ((nRHOBid > BID_PASS) && (nRHOBid != BID_REDOUBLE))
		nTopBid = nRHOBid;
	// see if the majors are available
	if ( (nMajorSuits[0] && nMajorSuits[1]) &&
		 ((numHearts >= 3) || (numSpades >= 3)) )
	{
		// both majors are available, so pick the better one
		int nSuit = bidState.PickSuperiorSuit(HEARTS,SPADES);
		nBid = bidState.GetCheapestShiftBid(nSuit, nTopBid);
		// see if the bid is affordable
		if (bidState.IsBidSafe(nBid))
		{
			status << "NEGDTR20! With " & bidState.m_fMinTPPoints & " pts in the partnership and " &
				      " both majors available in response to the negative double, go with the " &
					  hand.GetNumCardsInSuit(nSuit) & "-card " & STSS(nSuit) & " suit and bid " & 
					  BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
		}
		else
		{
			status << "NEGDTR21! We have both majors avaialblem but with " & bidState.m_fMinTPPoints & 
					  " pts in the partnership, we do not have enough points to bid either suit and have to pass.\n";
			bidState.SetBid(BID_PASS);
		}
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;
	}
	else if ( ((nMajorSuits[0]) && (numHearts >= 3)) ||
			  ((nMajorSuits[1]) && (numSpades >= 3)) )
	{
		// bid the suit
		if ((nMajorSuits[0]) && (numHearts >= 3))
			nSuit = HEARTS;
		else
			nSuit = SPADES;
		nBid = bidState.GetCheapestShiftBid(nSuit, nTopBid);
		// see if the bid is affordable
		if (bidState.IsBidSafe(nBid))
		{
			status << "NEGDTR30! With " & bidState.m_fMinTPPoints & " pts in the partnership and " &
				      hand.GetNumCardsInSuit(nSuit) & " cards available in " &
					  STS(nSuit) &", bid " & BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
		}
		else
		{
			status << "NEGDTR31! We have " & hand.GetNumCardsInSuit(nSuit) & " cards in the " & 
					  STSS(nSuit) & " suit, but with only " & bidState.m_fMinTPPoints & 
					  " pts in the partnership, we can't go any higher.\n";
			bidState.SetBid(BID_PASS);
		}
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;
	}

	//
	// we have no majors available, so bid NT if possible
	//
	if (bidState.BidNoTrumpAsAppropriate(FALSE))
	{
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;
	}

	// else bid something, anything
	nSuit = bidState.GetRebidSuit(bidState.nPreviousSuit);
	nBid = bidState.GetCheapestShiftBid(nSuit, nTopBid);
	if (!ISMAJOR(nSuit))
		status << "NEGDTR40! Unfortunately we don't have a major to respond with, so bid " & BTS(nBid) & ".\n";
	else
		status << "NEGDTR41! Our best response is " & BTS(nBid) & ".\n";
	bidState.SetBid(nBid);
	bidState.SetConventionStatus(this, CONV_FINISHED);
	return TRUE;
} 






//
//==========================================================
//
// Rebidding as opener after partner responds to a takeout double
//
BOOL CNegativeDoublesConvention::HandleConventionResponse(const CPlayer& player, 
														  const CConventionSet& conventions, 
														  CHandHoldings& hand, 
														  CCardLocation& cardLocation, 
														  CGuessedHandHoldings** ppGuessedHands,
														  CBidEngine& bidState,  
														  CPlayerStatusDialog& status)
{
	// there's no code here for now
	return FALSE;

  
	// check status
	if ((bidState.GetConventionStatus(this) != CONV_INVOKED_ROUND1) &&
		(bidState.GetConventionStatus(this) != CONV_INVOKED_ROUND2))
		return FALSE;

	//
	// get some info
	//
//	int nBid;
	double fPts = bidState.fPts;
	double fAdjPts = bidState.fAdjPts;
	double fCardPts = bidState.fCardPts;
	int nPrefSuit = bidState.nPrefSuit;
	int nPrefSuitStrength = bidState.nPrefSuitStrength;
	int nPreviousSuit = bidState.nPreviousSuit;
	BOOL bBalanced = bidState.bBalanced;
	//
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int nPartnersSuit = bidState.nPartnersSuit;
	int nPartnersSuitSupport = bidState.nPartnersSuitSupport;
	int nPartnersPrevSuit = bidState.nPartnersPrevSuit;
	int numSupportCards = bidState.numSupportCards;

	// first check for a strange response
	if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
	{
		// we don't understand partner's bid
		return CConvention::HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status);
	}

	//
	if (bidState.GetConventionStatus(this) == CONV_INVOKED_ROUND1) 
	{
		//
		//--------------------------------------------------------
		// responding to partner's forced bid
		// - estimate partner's strength
		//

		//
		// did partner pass? (horror of horrors!)
		//
		if (nPartnersBid == BID_PASS) 
		{
			if (bidState.nLHOBid >= BID_PASS)
				status << "NGDRb10! After interference from the left-hand opponent, partner passed our takeout.\n";
			else
				status << "NGDRb12! Partner unexpectedly passed our takeout double, which is supposed to be forcing.  Bidding will proceed as if the takeout was not made\n";
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return FALSE;
		}

		// set team point estimates -- be conservative
		BOOL bPartnerJumped = FALSE;
		BOOL bPartnerJumpedToGame = FALSE;
		int nEnemyBid = pDOC->GetValidBidRecord(0);
		int nEnemyBidLevel = BID_LEVEL(nEnemyBid);
		int nEnemySuit = BID_SUIT(nEnemyBid);
		if (nPartnersBidLevel > (nEnemyBidLevel + 1))
			bPartnerJumped = TRUE;
		if (nPartnersBid == bidState.GetGameBid(nPartnersSuit))
			bPartnerJumpedToGame = TRUE;
		// flag to see if we doubled in preference to an overcall
		BOOL bWantedToOvercall = FALSE;

		//
		if (nPartnersSuit == NOTRUMP)
		{
			//
			if (nPartnersBid == BID_1NT)
			{
				// partner has 6-9 HCPs
				bidState.m_fPartnersMin = 6;
				bidState.m_fPartnersMax = 9;
			}
			else if (nPartnersBid == BID_2NT)
			{
				// partner has 10-12 HCPs, maybe more
				bidState.m_fPartnersMin = 10;
				bidState.m_fPartnersMax = 12;
			}
			else if (nPartnersBid == BID_3NT)
			{
				// partner has 13+ HCPs
				bidState.m_fPartnersMin = 13;
				bidState.m_fPartnersMax = 40 - fCardPts;
				status << "NGDRb20! Partner's response of 3NT to our takeout double indicates that the opponent's suit is well stopped.\n";
			}
			else
			{
				// partner has 13+ HCPs???
				status << "NGDRb21! Partner's response of " & BTS(nPartnersBid) & 
						  " to our takeout double is unorthodox; treating it like a 3NT response.\n";
				bidState.m_fPartnersMin = 13;
				bidState.m_fPartnersMax = MIN(22, 40 - fCardPts);
			}
			// accept NT if we hold at least a semi-balanced 
			// and we don't have a 6-card major
			if ( !(ISMAJOR(bidState.nPrefSuit) && (bidState.numPrefSuitCards >= 6)) &&
				 (bidState.bSemiBalanced) )
				bidState.m_nAgreedSuit = NOTRUMP;
			//
			bidState.m_fMinTPPoints = fAdjPts + bidState.m_fPartnersMin;
			bidState.m_fMaxTPPoints = fAdjPts + bidState.m_fPartnersMax;
			bidState.m_fMinTPCPoints = fCardPts + bidState.m_fPartnersMin;
			bidState.m_fMaxTPCPoints = fCardPts + bidState.m_fPartnersMax;
			status << "NGDRb29! Partner's response of " & BTS(nPartnersBid) & 
					  " to our takeout double indicates " & 
					  bidState.m_fPartnersMin & "-" & bidState.m_fPartnersMax & 
					  " HCPs, for a total of " &
					  bidState.m_fMinTPCPoints & "-" & bidState.m_fMaxTPCPoints & " / " &
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & 
					  " pts in the partnership.\n";

		}
		else
		{
			// partner bid a suit
			// see if we really intended to overcall last time
			if (bidState.GetConventionStatus(&overcallsConvention) == CONV_SUBSUMED)
				bWantedToOvercall = TRUE;
			//
			if (nPartnersSuit == nEnemySuit)
			{
				// partner bid the enemy suit, showing 13+ pts.
				status << "NEGDRb40! Partner has responded in the enemy suit, indicating 13+ pts but no long suits.\n";
				bidState.m_fPartnersMin = 13;
				bidState.m_fPartnersMax = MIN(22, 40 - bidState.fCardPts);
				bidState.m_bGameForceActive = TRUE;
			}
			else if (bPartnerJumpedToGame)
			{
				// partner had 13+ pts & a 5-card major
				status << "NEGDRb41! Partner has jumped to game in " & STS(nPartnersSuit) &
						  ", indicating 13+ pts and a 5+ card suit.\n";
				bidState.m_fPartnersMin = 30;
				bidState.m_fPartnersMax = MIN(22, 40 - bidState.fCardPts);
				if (!bWantedToOvercall)
					bidState.m_nAgreedSuit = nPartnersSuit;
			}
			else if (bPartnerJumped)
			{
				// partner had 10-12 pts
				status << "NEGDRb42! Partner has made a jump response of " & BTS(nPartnersBid) &
						  ", indicating 10-12 pts and a 4-5 card suit.\n";
				bidState.m_fPartnersMin = 10;
				bidState.m_fPartnersMax = 12;
				if (!bWantedToOvercall)
					bidState.m_nAgreedSuit = nPartnersSuit;
			}
			else
			{
				// partner had <= 9 pts
				status << "NEGDRb43! Partner has made a minimum response of " & BTS(nPartnersBid) &
						  ", indicating no more than 9 points.\n";
				bidState.m_fPartnersMin = 0;
				bidState.m_fPartnersMax = 9;
				if (!bWantedToOvercall)
					bidState.m_nAgreedSuit = nPartnersSuit;
			}
			//
			bidState.m_fMinTPPoints = fAdjPts + bidState.m_fPartnersMin;
			bidState.m_fMaxTPPoints = fAdjPts + bidState.m_fPartnersMax;
			bidState.m_fMinTPCPoints = fCardPts + bidState.m_fPartnersMin;
			bidState.m_fMaxTPCPoints = fCardPts + bidState.m_fPartnersMax;
			status << "NGDRb49! The total point count in the partnership is therefore " &
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & 
					  " pts.\n";
		}

			
		//
		// see if RHO bid after partner's response -- that mitigates our responsibility
		// to bid again, unless partner's bid was game forcing
		//
	//	if (bidState.nRHOBid > BID_PASS)
		if ((bidState.nRHOBid > BID_PASS) && 
			(bidState.nRHOBid != BID_DOUBLE) && (bidState.nRHOBid != BID_REDOUBLE))
		{
			status << "2NGDRb51! The right-hand opponent has " &
					  ((bidState.nRHOBid == BID_DOUBLE)? "doubled" : "bid") &
					  " after partner's response, interfering with our communication.\n";
		}


		//
		//---------------------------------------------------------------------
		// see if we have an agreed suit
		//
		int nBid;
		if (bidState.m_nAgreedSuit > NONE)
		{
			if (bidState.m_nAgreedSuit == NOTRUMP)
			{
				// we've agreed to play in NoTrump
				// see if we can raise partner to a higher NT contract
				if (bidState.BidNoTrumpAsAppropriate(FALSE,STOPPED_DONTCARE))
				{
					bidState.SetConventionStatus(this, CONV_FINISHED);
					return TRUE;
				}
				// else pass
				nBid = BID_PASS;
				status << "NGDRb69! With a total of " &
						  bidState.m_fMinTPCPoints & "-" & bidState.m_fMaxTPCPoints &
						  " HCPs in the partnership, we have insufficient strength to raise partner's " &
						  BTS(nPartnersBid) & " bid, so we have to pass.\n";
				bidState.SetBid(nBid);
				bidState.SetConventionStatus(this, CONV_FINISHED);
				return TRUE;
			}
			else
			{
				// we've agreed on a suit, so raise if possible
				// if partner didn't jump, we may have credited him with
				// zero pts, so adjust rqmts accordingly

				// if partner jumped to game && we have < 32 pts, pass
				if ((bPartnerJumpedToGame) && (bidState.m_fMinTPPoints < 32))
				{
					status << "NGDRb70! Partner jumped to game in his " & bidState.szPSS & 
							  " suit, so with a team total of " &
							  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
							  " points, we pass.\n";
					bidState.SetBid(BID_PASS);
					bidState.SetConventionStatus(this, CONV_FINISHED);
					return TRUE;
				}
				// raise partner if possible -- bearing in mind that 
				// partner may have a wide range of points

				// raise a major to game with 23-31 pts and 4 trumps
				//					  or with 26-31 pts and 3 trumps
				// or raise to the 3-level with 20-24 pts and 3 trumps
				if ( (bidState.RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_4,PTS_MAJOR_GAME-3,31,SUPLEN_4)) ||
				     (bidState.RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_4,PTS_MAJOR_GAME,31,SUPLEN_3)) ||
					 (bidState.RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_3,PTS_MAJOR_GAME-6,24,SUPLEN_3)))
				{
					if (!bPartnerJumped)
						status << "NGDRb71a! (we can assume partner has some strength in the " & bidState.szPSS & 
								  " suit, so we are shading the requirements slightly.\n";
					bidState.SetConventionStatus(this, CONV_INVOKED_ROUND2);
					return TRUE;
				}
				// raise a minor to game with 28-32 pts and 4 trumps
				//                    or with 29-32 pts and 3 trumps
				// or raise to the 4-level with 26-28 pts and 3 trumps
				// or raise to the 3-level with 23-25 pts and 3 trumps
				if ( (bidState.RaisePartnersSuit(SUIT_MINOR,RAISE_TO_5,PTS_MINOR_GAME-1,PTS_SLAM-1,SUPLEN_4)) ||
					 (bidState.RaisePartnersSuit(SUIT_MINOR,RAISE_TO_5,PTS_MINOR_GAME,PTS_SLAM-1,SUPLEN_3)) ||
					 (bidState.RaisePartnersSuit(SUIT_MINOR,RAISE_TO_4,PTS_MINOR_GAME-3,PTS_MINOR_GAME-1,SUPLEN_3)) ||
					 (bidState.RaisePartnersSuit(SUIT_MINOR,RAISE_TO_3,PTS_MINOR_GAME-6,PTS_MINOR_GAME-4,SUPLEN_3)) )
				{
					if (!bPartnerJumped)
						status << "NGDRb71b! We can assume partner has some strength in the " & bidState.szPSS & 
								  " suit, so we can shade the requirements slightly.\n";
					bidState.SetConventionStatus(this, CONV_INVOKED_ROUND2);
					return TRUE;
				}
				// with 32+ pts, invoke Blackwood
				if (bidState.m_fMinTPCPoints >= 32)
				{
					bidState.InvokeBlackwood(bidState.m_nAgreedSuit);
					bidState.SetConventionStatus(this, CONV_INVOKED_ROUND2);
					return TRUE;
				}
				// else pass
				nBid = BID_PASS;
				status << "NGDRb90! With a total of " &
						  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
						  " points in the partnership, we have insufficient strength to raise partner's " &
						  BTS(nPartnersBid) & " bid, so we have to pass.\n";
				bidState.SetBid(nBid);
				bidState.SetConventionStatus(this, CONV_FINISHED);
				return TRUE;
			}
		}


		//
		//--------------------------------------------------------------------------
		// here, we have no suit agreement (e.g., partner bid the opponents' suit)
		//
		int nLastBid = pDOC->GetLastValidBid();
		if (bBalanced)
		{
			// try notrumps
			if (hand.IsSuitProbablyStopped(nEnemySuit))
			{
				status << "NGDRb80! Without clear suit agreement, and holding a blanaced hand, we want to steer towards a contract in No Trump.\n";
				if (bidState.BidNoTrumpAsAppropriate(FALSE,STOPPED_DONTCARE))
				{
					bidState.SetConventionStatus(this, CONV_INVOKED_ROUND2);
					return TRUE;
				}
			}
			// else pass
			status << "NGDRb81! But as we do not have a proper hand to bid No Trump at the appropriate level, we have to pass.\n";
			bidState.SetBid(BID_PASS);
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return TRUE;
		}
		else if (bidState.numPrefSuitCards >= 5)
		{
			// bid the suit
			nBid = bidState.GetCheapestShiftBid(nPrefSuit, nLastBid);
			if (bidState.IsBidSafe(nBid, 4))
			{
				if (bWantedToOvercall)
					status << "NGDRb90! Partner's forced response of " & bidState.szPB & 
							 " not withstanding, we can now show the " & 
							  STSS(nPrefSuit) & " suit that we wanted to overcall with last round by bidding " &
							  BTS(nBid) & ".\n";
				else
					status << "NGDRb91! Without clear suit agreement, we bid our " &
							  bidState.numPrefSuitCards & "-card " & STSS(bidState.nPrefSuit) &
							  " suit at " & BTS(nBid) & ".\n";
				bidState.SetBid(nBid);
				bidState.SetConventionStatus(this, CONV_INVOKED_ROUND2);
				return TRUE;
			}
		}


		//
		//--------------------------------------------------------------------------
		// else we have no other options, so pass
		status << "NEGDRb99! We see no good fit with partner and no other options, so pass.\n";
		bidState.SetBid(BID_PASS);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;

	}
	else
	{
		//
		// responding after partner's second response to our takeout
		//

		// did partner pass?
		if (nPartnersBid == BID_PASS) 
		{
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return FALSE;
		}

		//
		// otherwise, consider the choices:
		//
		// - with suit agreement, raise if possible
		//         S    N     S     N     S
		//   e.g., X -> 1C -> 1S -> 2S -> ???
		// - without suit agreement,
		//   e.g., X -> 1C -> 1S -> 2H -? ???
		//   bid the 4th suit if we have the pts (26+)
		// - without suit agreement, but if partner bids NT,
		//   or if we have a balanaced hand, bid NT
		//
		double fMinTPPoints = bidState.m_fMinTPPoints;
		double fMaxTPPoints = bidState.m_fMaxTPPoints;
		double fMinTPCPoints = bidState.m_fMinTPCPoints;
		double fMaxTPCPoints = bidState.m_fMaxTPCPoints;
		int nBid;
		
		//
		// see if we have suit agreement
		//
		if (nPartnersSuit == nPreviousSuit)
		{
			// partner raised our suit -- re-raise if possible
			bidState.m_nAgreedSuit = nPreviousSuit;
			if (nPartnersBid >= bidState.GetGameBid(nPreviousSuit))
			{
				// partner bid game or beyond
				if ((nPartnersBidLevel == 7) ||
					((nPartnersBidLevel == 6) && (fMinTPPoints <= 36)) )
				{
					nBid = BID_PASS;
					status << "NGDRc10! Partner raised our " & bidState.szPVSS & 
							  " suit to a slam, so pass.\n";
				}
				else if ((nPartnersBidLevel <= 6) && (fMinTPPoints >= 37))
				{
					nBid = MAKEBID(nPreviousSuit, 7);
					status << "NGDRc11! Partner raised our " & bidState.szPVSS & 
							  " suit to " & 
							  ((nPartnersBidLevel == 6)? "slam" : "game") &
							  ", but we have the poitns to push to a grand slam, so bid " & 
							  BTS(nBid) & ".\n";
				}
				else if ((nPartnersBidLevel < 6) && (fMinTPPoints >= 33))
				{
					nBid = MAKEBID(nPreviousSuit, 6);
					status << "NGDRc12! Partner raised our " & bidState.szPVSS & 
							  " suit to game, but we have the poitns for a slam, so bid " & 
							  BTS(nBid) & ".\n";
				}
				else
				{
					nBid = BID_PASS;
					status << "NGDRc13! Partner raised our " & bidState.szPVSS & 
							  " suit to game, which is acceptable with " & 
							  fMinTPPoints & "-" & fMaxTPPoints & 
							  " pts in the partnership, so pass.\n";
				}
			}
			else
			{
				// partner raised below game
				// re-raise if possible
				if ( (ISMAJOR(nPreviousSuit) && (fMinTPPoints >= PTS_MAJOR_GAME)) ||
					 (ISMINOR(nPreviousSuit) && (fMinTPPoints >= PTS_MINOR_GAME)) )
				{
					nBid = bidState.GetGameBid(nPreviousSuit);
					status << "NGDRc20! With a total of " & 
							  fMinTPPoints & "-" & fMaxTPPoints & 
							  " pts in the partnership, raise to game in the " &
							  bidState.szPVSS & " suit at " & BTS(nBid) & ".\n";
				}
				else
				{
					// else try to raise cheaply
					nBid = bidState.GetCheapestShiftBid(nPreviousSuit);
					if (bidState.IsBidSafe(nBid))
					{
						status << "NGDRc22! With a total of " & 
								  fMinTPPoints & "-" & fMaxTPPoints & 
								  " pts in the partnership, we can raise again to " &
								  BTS(nBid) & ".\n";
					}
					else
					{
						nBid = BID_PASS;
						status << "NGDRc29! With a total of " & 
								  fMinTPPoints & "-" & fMaxTPPoints & 
								  " pts in the partnership, we cannot safely raise any partner further, so pass.\n";
					}
				}
			}
		}
		else if ((nPartnersSuit == NOTRUMP) || (bidState.bBalanced))
		{
			//
			// partner bid NT, or else we have a balanced hand
			//
			if (bidState.BidNoTrumpAsAppropriate(FALSE, STOPPED_DONTCARE))
			{
				nBid = bidState.m_nBid;
				status << "NGDRc30! With a total of " & 
						  fMinTPCPoints & "-" & fMaxTPCPoints & 
						  " HCPs in the partnership, we can bid " & BTS(nBid) & ".\n";
			}
			else
			{
				nBid = BID_PASS;
				if (nPartnersSuit == NOTRUMP)
					status << "NGDRc35! We're willing to accept a contract in NoTrumps, but don't have the points to raise further, so pass.\n";
				else
					status << "NGDRc36! We'd like to play in NoTrumps, but don't have the points to bid agian, so pass.\n";
			}
		}
		else
		{
			//
			// else we have no suit agreement, and can't play NT
			//

			//
			// bid the 4th suit if we have enough pts
			//
			int nSuit = bidState.GetFourthSuit(nPreviousSuit, nPartnersSuit, nPartnersPrevSuit);
			nBid = bidState.GetCheapestShiftBid(nSuit);

			if ((fMinTPPoints >= PTS_GAME) && (nBid < bidState.GetGameBid(nSuit)))
			{
				status << "NGDRc40! With a total of " & 
						  fMinTPPoints & "-" & fMaxTPPoints & 
						  " pts in the partnership and no suit agreement, bid another suit (" &
						  STS(nSuit) & ") at " & BTS(nBid) & ".\n";
			}
			else
			{
				// gotta pass
				nBid = BID_PASS;
				if (nPartnersBid >= bidState.GetGameBid(nPartnersSuit))
					status << "NGDRc45! With a total of " & 
							  fMinTPPoints & "-" & fMaxTPPoints & 
							  " pts in the partnership, partner has gone to game in his suit at " &
							  bidState.szPB & ", so pass.\n";
				else if (fMinTPPoints >= PTS_GAME)
					status << "NGDRc46! With a total of " & 
							  fMinTPPoints & "-" & fMaxTPPoints & 
							  " pts in the partnership, but having run out of bidding room, we have to bail out and pass.\n";
				else 
					status << "NGDRc47! With a total of only " & 
							  fMinTPPoints & "-" & fMaxTPPoints & 
							  " pts in the partnership, and no agreement in suits, we have to pass.\n";
			}
		}
		// done with the second rebid 
		bidState.SetBid(nBid);
		bidState.ClearConventionStatus(this);
		return TRUE;
	}
}





//
//==================================================================
// construction & destruction
//
CNegativeDoublesConvention::CNegativeDoublesConvention() 
{
	// from ConvCodes.h
	m_nID = tidNegativeDoubles;
}

CNegativeDoublesConvention::~CNegativeDoublesConvention() 
{
}


