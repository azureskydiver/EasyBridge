//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// MichaelsCueBidConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "Player.h"
#include "PlayerStatusDialog.h"
#include "MichaelsCueBidConvention.h"
#include "OvercallsConvention.h"
#include "ConventionSet.h"




//
//===============================================================================
//
// TryConvention()
//
// check if we can use a Michaels Cue Bid here
//
BOOL CMichaelsCueBidConvention::TryConvention(const CPlayer& player, 
											  const CConventionSet& conventions, 
											  CHandHoldings& hand, 
											  CCardLocation& cardLocation, 
											  CGuessedHandHoldings** ppGuessedHands,
											  CBidEngine& bidState,  
											  CPlayerStatusDialog& status)
{
	//
	// the requirements for a Michaels Cue Bid are:
	// 1: RHO must have opened a suit (not NT) at the 1-level
	// 2: this must be our first bidding opportunity
	// 3: need a 2-suited hand; i.e., 
	//    - if RHO bid a minor, need 5/5 or 5/(good)4 in the majors
	//    - if RHO bid a major, need 5 in the other major, plus 5 in a minor
	//    the two suits should also be rather comparable
	// 4: need either 6-11 pts or 17+ pts
	//    (with 12-16 pts, we generally overcall or double for takeout)
	// 5: if we have 12+ pts and playing takeout doubles, cannot have a 
	//    hand suitable for a takeout (???)

	// test conditions 1, 2, & 4
	if ( ((bidState.nRHOBid >= BID_1C) && (bidState.nRHOBid <= BID_1S)) &&
		 ( ((bidState.fPts >= OPEN_PTS(6)) && (bidState.fPts <= OPEN_PTS(11))) || ((bidState.fPts >= OPEN_PTS(17))/*&& (bidState.fPts <= 18)*/) ) &&
 		  (bidState.m_numBidTurns == 0) && (bidState.m_numPartnerBidTurns == 0) )
	{
		 // passed
	}
	else
	{
		return FALSE;
	}

	// test condition #3
	int nOppSuit = bidState.nRHOSuit;
	BOOL bEnoughLength = FALSE;
	//
	if (ISMINOR(nOppSuit))
	{
		// need 5/5 in the majors, or 5 + good 4
		int numHearts = hand.GetNumCardsInSuit(HEARTS);
		int numSpades = hand.GetNumCardsInSuit(SPADES);
		if ((hand.GetSuitStrength(HEARTS) < SS_MARGINAL_OPENER) || (hand.GetSuitStrength(SPADES) < SS_MARGINAL_OPENER) ||
			(Abs(hand.GetSuitStrength(HEARTS) - hand.GetSuitStrength(SPADES)) > 1) )
			return FALSE;
		//
		if ((numHearts >= 5) && (numSpades >= 5))
		{
			bEnoughLength = TRUE;
			status << "2MCL1! We have at least 5/5 in the majors, so we can make a Michaels cue bid.\n";
		}
		else if ((numHearts >= 5) && (numSpades == 4) && 
						(hand.GetSuitStrength(SPADES) >= SS_STRONG))
		{
			bEnoughLength = TRUE;
			status << "2MCL2! We have 5/4 in the majors, but the 4 Spades are strong, so we can make a Michaels cue bid.\n";
		}
		else if ((numHearts == 4) && (numSpades >= 5) && (hand.GetSuitStrength(HEARTS) >= SS_STRONG))
		{
			bEnoughLength = TRUE;
			status << "2MCL3! We have 5/4 in the majors, but the 4 Hearts are strong, so we can make a Michaels cue bid.\n";
		}
	}
	else
	{
		// need 5 in the unbid major, and 5 in a minor
		int nOtherMajor = (nOppSuit == HEARTS)? SPADES : HEARTS;
		int numMajors = hand.GetNumCardsInSuit(nOtherMajor);
		int numClubs = hand.GetNumCardsInSuit(CLUBS);
		int numDiamonds = hand.GetNumCardsInSuit(CLUBS);

		// check that the major suit is OK
		if ((numMajors < 5) || (hand.GetSuitStrength(nOtherMajor) < SS_MARGINAL_OPENER))
			return FALSE;

		// then check the minor & compare the suits
		if ((numClubs >= 5) && (hand.GetSuitStrength(CLUBS) >= SS_MARGINAL_OPENER) &&
			(Abs(hand.GetSuitStrength(nOtherMajor) - hand.GetSuitStrength(CLUBS)) <= 1) )
		{
			bEnoughLength = TRUE;
			status << "2MCL5! We have 5+ cards in " & STS(nOtherMajor) & 
					  " and 5+ cards in a minor (Clubs), so we can make a Michaels cue bid.\n";
		}
		else if ((numDiamonds >= 5) && (hand.GetSuitStrength(DIAMONDS) >= SS_MARGINAL_OPENER) &&
				(Abs(hand.GetSuitStrength(nOtherMajor) - hand.GetSuitStrength(DIAMONDS)) <= 1) )
		{
			bEnoughLength = TRUE;
			status << "2MCL5! We have 5+ cards in " & STS(nOtherMajor) & 
					  " and 5+ cards in a minor (Diamonds), so we can make a Michaels cue bid.\n";
		}
	}

	// check condition
	if (!bEnoughLength)
		return FALSE;

	// OK, go ahead and cue-bid the enemy suit
	int nBid = MAKEBID(nOppSuit, 2);
	status << "MCL9! With a 2-suited hand, make a Michaels cue bid over RHO's " & STS(nOppSuit) & 
			  " with a bid of " & BTS(nBid) & ".\n";
	bidState.SetBid(nBid);
	bidState.SetConventionStatus(this, CONV_INVOKED);
	return TRUE;
}





//
//-----------------------------------------------------
//
// respond to partner's Michaels Cue Bid
//
BOOL CMichaelsCueBidConvention::RespondToConvention(const CPlayer& player, 
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
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int nPartnersSuit = bidState.nPartnersSuit;
	int nPartnersPrevSuit = bidState.nPartnersPrevSuit;
	//
	int nBid, nSuit;
	double fPts = bidState.fPts;
	double fCardPts = bidState.fCardPts;

	// 
	// see what round this is
	//
	int nStatus = bidState.GetConventionStatus(this);
	if (nStatus == CONV_INACTIVE)
	{
		//
		// Bidding in response to partner's Michael's bid? check requirements
		//
		// the identifying marks of a Michaels bid are:
		// 1: we must not have bid yet
		// 2: LHO must have bid a suit at the 1 level, and 
		// 3: partner overcalled LHO's suit at the 2 level
		int nLastValidBid = pDOC->GetLastValidBid();

		// apply tests #1, 2, and 3
		int nOpeningBid = pDOC->GetOpeningBid();
		int nOpeningBidder = pDOC->GetOpeningBidder();
		BOOL bLHOMajor = ISMAJOR(nOpeningBid);
		if (ISBID(nOpeningBid) && (GetPlayerTeam(nOpeningBidder) != player.GetTeam()) &&
			 ((nOpeningBid >= BID_1C) && (nOpeningBid <= BID_1S)) &&
			  (nPartnersSuit == bidState.nLHOSuit) && (nPartnersBidLevel == 2) &&
			  (bidState.m_numBidsMade == 0) )
		{
			//
			status << "MCLR10! Partner has made a Michaels Cue bid of " & BTS(nPartnersBid) &
					  ", indicating 5/5 length in " &
					  (bLHOMajor? ((bidState.nLHOSuit == HEARTS)? "Spades and a minor" : "Hearts and a minor") : "the majors") & ".\n";
		}
		else
		{
			return FALSE;
		}

		// did partner bid a minor, indicating both majors?
		if (ISMINOR(nPartnersSuit))
		{
			// Pard has both majors -- pick the preferred one
			nSuit = bidState.PickSuperiorSuit(HEARTS, SPADES);
			if (hand.GetNumCardsInSuit(nSuit) < 3)
			{
				// should have at least 3 trumps
				int nOtherSuit = (nSuit == HEARTS)? SPADES : HEARTS;
				if (hand.GetNumCardsInSuit(nOtherSuit) >= 3)
					nSuit = nOtherSuit;
			}
		}
		else if (nPartnersSuit == HEARTS)
		{
			// pard has Spades + a minor
			// see if Spades are decent (3-card support)
//			if ((hand.GetNumCardsInSuit(SPADES) >= 3) && (hand.GetSuitStrength(SPADES) >= SS_MODERATE_SUPPORT))
			if (hand.GetNumCardsInSuit(SPADES) >= 3)
			{
				// fine, go with Spades
				nSuit = SPADES;
			}
			else 
			{
				// Spades are too weak; look for a minor
				if ((hand.GetSuitStrength(CLUBS) >= SS_MODERATE_SUPPORT) || (hand.GetSuitStrength(DIAMONDS) >= SS_MODERATE_SUPPORT))
					nSuit = NOTRUMP;
				else
					nSuit = SPADES;	// minors are no good either, so go with Spades
			}
		}
		else 
		{
			// pard has Hearts + a minor
			// see if Hearts are decent (3-card support)
//			if ((hand.GetNumCardsInSuit(HEARTS) >= 3) && (hand.GetSuitStrength(HEARTS) >= SS_MODERATE_SUPPORT))
			if (hand.GetNumCardsInSuit(HEARTS) >= 3)
			{
				// fine, go with Hearts
				nSuit = HEARTS;
			}
			else 
			{
				// Spades are too weak; look for a minor
				if ((hand.GetSuitStrength(CLUBS) >= SS_MODERATE_SUPPORT) || (hand.GetSuitStrength(DIAMONDS) >= SS_MODERATE_SUPPORT))
					nSuit = NOTRUMP;
				else
					nSuit = HEARTS;	// minors are no good either, so go with Hearts
			}
		}

		// now adjust point count
		if (ISSUIT(nSuit))
		{
			bidState.SetAgreedSuit(nSuit);
			fPts = bidState.fAdjPts = hand.RevalueHand(REVALUE_DECLARER, nSuit, TRUE);
		}
		// partner may have 6-11 OR 17+ pts; assume it's a weak hand
		bidState.AdjustPartnershipPoints(6, 11);

		// and make a responding bid
		CString strChoices = ISMINOR(nPartnersSuit)? "the two majors" : 
					(nPartnersSuit == HEARTS)? "Spades and a minor" : "Hearts and an unknown minor";
		int numTrumps = ISSUIT(nSuit)? hand.GetNumCardsInSuit(nSuit) : 0;

		// set initial convention status to finished
		bidState.SetConventionStatus(this, CONV_FINISHED);

		// see if we need to look at the minor
		if (nSuit == NOTRUMP)
		{
			// bid 2NT to ask for the minor
			nBid = BID_2NT;
			if (nBid > nLastValidBid)
			{
				if (nPartnersSuit == HEARTS)
					status << "MCLR12! Given a choice between a " & hand.GetNumCardsInSuit(SPADES) &
							  "-card Spade suit and an unknown minor, we're forced to ask for the minor by bidding " & BTS(nBid) & ".\n";
				else
					status << "MCLR12! Given a choice between a " & hand.GetNumCardsInSuit(HEARTS) &
							  "-card Heart suit and an unknown minor, we're forced to ask for the minor by bidding " & BTS(nBid) & ".\n";
				// this convention will go another round
				bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND1);
			}
			else
			{
				nBid = BID_PASS;
				status << "MCLR13! We'd like to bid 2NT to ask for partner's minor, but we can't do so after RHO's interference, so we have to pass.\n";
			}
		}
		else if ((numTrumps >= 5) && (bidState.m_fMinTPPoints <= PT_COUNT(20)) &&
			ISMAJOR(nSuit) && (MAKEBID(nSuit, 4) > nLastValidBid))
		{
			// make a preemptive bid if possible
			nBid = MAKEBID(nSuit, 4);
			status << "MCLR15! With " & numTrumps & " " & STS(nSuit) & " and a total of " & 
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " pts in the partnership, make a shutout bid in " & STS(nSuit) & 
					  " by jumping to " & BTS(nBid) & ".\n";
		}
		else if (bidState.m_fMinTPPoints <= PT_COUNT(21))
		{
			// respond at the 2-level
			nBid = bidState.GetCheapestShiftBid(nSuit, nLastValidBid);
			if (BID_LEVEL(nBid) == 2)
			{
				status << "MCLR20! Given a choice between " & strChoices & 
						   ", respond to partner's Michaels with the preferred " & 
						   STSS(nSuit) & " suit with a bid of " & BTS(nBid) & ".\n";
			}
			else
			{
				nBid = BID_PASS;
				status << "MCLR21! After RHO interference, we don't have the points to go to the 3-level in response to partner's Michaels, so pass.\n";
			}
		}
		else if (bidState.m_fMinTPPoints <= PT_COUNT(24))
		{
			// bid at the 3-level
			nBid = MAKEBID(nSuit, 3);
			if (nBid > nLastValidBid)
			{
				status << "MCLR24! With a total of " & bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
						   " pts in the partnership and a choice between " & strChoices &
						   ", invite game in " & STS(nSuit) & " by jumping to " & BTS(nBid) & ".\n";
			}
			else
			{
				nBid = BID_PASS;
				status << "MCLR25! After RHO interference, we don't have the points to go to the 4-level in response to partner's Michaels, so pass.\n";
			}
		}
		else if (ISMINOR(nSuit) && (bidState.m_fMinTPPoints <= PT_COUNT(27)))
		{
			// with 25+ pts, bid game
			nBid = bidState.GetGameBid(nSuit);
			if (nBid > nLastValidBid)
			{
				status << "MCLR27! With a total of " & bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
						   " pts in the partnership and a choice between " & strChoices & 
						   ", jump to the 4-level in " & STS(nSuit) & " with a bid of " & BTS(nBid) & ".\n";
			}
			else
			{
				nBid = BID_PASS;
				status << "MCLR28! After RHO interference, we don't have the points to go to the 5-level in response to partner's Michaels, so pass.\n";
			}
		}
		else
		{
			// with 25+ pts in a major, or 28+ pts in a minor, bid game
			nBid = bidState.GetGameBid(nSuit);
			if (nBid > nLastValidBid)
			{
				status << "MCLR30! With a total of " & bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
						   " pts in the partnership and a choice between " & strChoices & 
						   ", jump to game in " & STS(nSuit) & " at " & BTS(nBid) & ".\n";
			}
			else
			{
				nBid = BID_PASS;
				status << "MCLR31! After RHO interference, we don't want to bid past game, so pass.\n";
			}
		}

		// done!
		bidState.SetBid(nBid);
		return TRUE;

	}
	else if (nStatus == CONV_RESPONDED_ROUND1)
	{
		//
		// round 2 -- we must've bid 2NT last time to ask for partner's minor
		//

		// see if partner cue bid the enemy suit again
		if (nPartnersSuit == nPartnersPrevSuit)
		{
			// partner has 17+ pts
			bidState.AdjustPartnershipPoints(OPEN_PTS(17), MIN(17,40 - bidState.fCardPts));

			// bid 4NT to ask for the minor again
			if (fPts >= PT_COUNT(15))
			{
				// else make a natural game bid
				nBid = BID_4NT;
				status << "MCLR35! Partner cue bid the enemy suit again, indicating a strong Michaels opening hand with " &
						   OPEN_PTS(17) & "+ pts; so with " & 
						   bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
						   " pts in the partnership, ask for the other minor by bidding " & BTS(nBid) & ".\n";
				bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
			}
			else
			{
				// else make a natural 3NT game bid
				nBid = BID_3NT;
				status << "MCLR36! Partner cue bid the enemy suit again, indicating a strong Michaels opening hand with " &
						   OPEN_PTS(17) & "+ pts; but with " & 
						   bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
						   " pts in the partnership, stop at game with a natural bid of " & BTS(nBid) & ".\n";
				bidState.SetConventionStatus(this, CONV_FINISHED);
			}
			// done
			bidState.SetBid(nBid);
			return TRUE;
		}

		// see if partner's bid is valid
		if (!ISMINOR(nPartnersSuit))
		{
			status << "MCLR40! After his opening Michaels Cue Bid, partner did not respond properly to our 2NT minor suit inquiry, so michaels is off.\n";
			bidState.SetConventionStatus(this, CONV_ERROR);
			return FALSE;
		}

		// see if we can live with partner's minor
		int nAgreedSuit;
		if (hand.GetNumCardsInSuit(nPartnersSuit) >= 3)
		{
			// stick with the minor
			status << "4MCLR41! Since we have " & hand.GetNumCardsInSuit(nPartnersSuit) & 
					  " cards in the " & STSS(nPartnersSuit) & " suit, choose that suit for our response.\n";
			nAgreedSuit = nPartnersSuit;
		}
		else
		{
			// minor's not so great; select the better of the minor or the major
			nAgreedSuit = bidState.PickSuperiorSuit(nPartnersSuit, bidState.nPartnersPrevSuit);
			if (nAgreedSuit == nPartnersSuit)
				status << "4MCLR42! The " & STSS(nPartnersSuit) & " suit isn't great, but it's better than " &
						   STS(bidState.nPartnersPrevSuit) & ", so choose that suit for our response.\n";
			else
				status << "4MCLR43! The " & STSS(nPartnersSuit) & " suit is poor, so pick the major suit ("& 
						   STS(bidState.nPartnersPrevSuit) & ") for our response.\n";
			bidState.SetAgreedSuit(bidState.PickSuperiorSuit(nPartnersSuit, bidState.nPartnersPrevSuit));
		}

		// recalc points as dummy
		bidState.SetAgreedSuit(nAgreedSuit);
		fPts = bidState.fAdjPts = hand.RevalueHand(REVALUE_DECLARER, nAgreedSuit, TRUE);
		bidState.AdjustPartnershipPoints();

		// select the bid level
		if (ISMINOR(nAgreedSuit))
		{
			// Clubs or Diamonds
			if (bidState.m_fMinTPPoints <= PTS_MINOR_GAME-6)	// <= 22
				nBid = BID_PASS;
			else if (bidState.m_fMinTPPoints <= PTS_MINOR_GAME-4)	// <= 24
				nBid = MAKEBID(nPartnersBid, 4);
			else 
				nBid = MAKEBID(nPartnersBid, 5);	// go for game
			//
			if (nBid == BID_PASS)
				status << "MCLR45! Partner showed his minor to be " & STS(nPartnersSuit) & 
						  ", which we can support with " & hand.GetNumCardsInSuit(nPartnersSuit) &
						  " trumps, but with only " & bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
						  " pts in the partnership, we have to pass.\n";
			else
				status << "MCLR46! Partner showed his minor to be " & STS(nPartnersSuit) & 
						  ", which we can support with " & hand.GetNumCardsInSuit(nPartnersSuit) &
						  " trumps, so with " & bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
						  " pts in the partnership, we bid " & BTS(nBid) & ".\n";
		}
		else
		{
			// sticking with the major (though not a very good one!)
			// sign off at the 3-level or go to game
			if (bidState.m_fMinTPPoints < PTS_MAJOR_GAME)	// < 25
				nBid = MAKEBID(nPartnersBid, 3);
			else	
				nBid = MAKEBID(nPartnersBid, 4);
			//
			if (BID_LEVEL(nBid) == 3)
				status << "MCLR47! With " & hand.GetNumCardsInSuit(nAgreedSuit) & 
						  "-card support for partner's " & STS(nAgreedSuit) & 
						  ", and with only " & bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
						  " pts in the partnership, we have to pass.\n";
			else
				status << "MCLR47! With " & hand.GetNumCardsInSuit(nAgreedSuit) & 
						  "-card support for partner's " & STS(nAgreedSuit) & 
						  ", and with " & bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
						  " pts in the partnership, we can go to game at " & BTS(nBid) & ".\n";
		}

		// done!
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;
	}
	else if (nStatus == CONV_RESPONDED_ROUND2)
	{
		//
		// round 3 -- we must've bid 4NT last time to ask for partner's minor
		//            after partner's second Michaels cue bid
		//

	}


	//
	return FALSE;
} 







//
//==========================================================
//
// Rebidding as opener after partner responds to a Michaels Cue Bid
//
BOOL CMichaelsCueBidConvention::HandleConventionResponse(const CPlayer& player, 
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

	//
	// get some info
	//
	int nBid;
	double fPts = bidState.fPts;
	double fAdjPts = bidState.fAdjPts;
	double fCardPts = bidState.fCardPts;
	int nPrefSuit = bidState.nPrefSuit;
	int nPrefSuitStrength = bidState.nPrefSuitStrength;
	int nPreviousSuit = bidState.nPreviousSuit;
	int nPreviousBidLevel = bidState.nPreviousBidLevel;
	BOOL bBalanced = bidState.bBalanced;
	//
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int nPartnersSuit = bidState.nPartnersSuit;
	int nPartnersSuitSupport = bidState.nPartnersSuitSupport;
	int nPartnersPrevSuit = bidState.nPartnersPrevSuit;
	int numSupportCards = bidState.numSupportCards;

	//
	// handling partner's Michaels response
	//
	int nStatus = bidState.GetConventionStatus(this);

	if (nStatus == CONV_INVOKED)
	{
		// first check for a strange response
		if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
		{
			// we don't understand partner's bid
			return CConvention::HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status);
		}

		// if we're holding the STRONG Michaels hand indicate it by cue-bidding the
		// enemy suit again
		if (bidState.fPts >= PT_COUNT(17))
		{
			nBid = MAKEBID(nPreviousSuit, 3);
			status << "MCLRH12! with a strong Michaels hand (" & bidState.fPts & 
					  " pts), cue bid the enemy suit again at " & BTS(nBid) & 
					  " to indicate our strength.\n";
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_INVOKED_ROUND2);
			return TRUE;
		}

		// did partner bid 2NT, asking for the minor?
		if (ISMAJOR(nPreviousSuit) && (nPartnersBid == BID_2NT))
		{
			// with a weak Michaels opener, indicate the minor
			int nSuit = bidState.GetLongerSuit(CLUBS, DIAMONDS);
			nBid = MAKEBID(nSuit, 3);
			status << "MCLRH10! With the weak flavor of Michaels, respond to partner's Michaels minor inquiry with a bid of " & BTS(nBid) &
					  ", indicating " & STS(nSuit) & " as the unknown minor.\n";
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_INVOKED_ROUND2);
			return TRUE;
		}

		// did partner cue-bid the enemy suit (a slam try)?
		if ((nPartnersSuit == nPreviousSuit) && (nPartnersBidLevel == nPreviousBidLevel+1))
		{
			// determine the cheapest of the two suits
			int nCheapestSuit;
			if (ISMINOR(nPreviousSuit))
				nCheapestSuit = HEARTS;		// 5/5 in the majors, so Hearts is cheapest
			else
				nCheapestSuit = bidState.GetLongerSuit(CLUBS, DIAMONDS);

			// respond affirmatively only with a strong hand
			status << "2MCLRH20! Partner cue bid the enemy suit, which is a game or slam try.\n";
			if (bidState.fPts >= PT_COUNT(17))
			{
				// jump into Blackwood
				status << "MCLRH21! And with " & bidState.fPts & " pts in hand, proceed towards slam in the preferred "&
						  STSS(bidState.nPrefSuit) & " suit.\n";
				bidState.InvokeBlackwood(bidState.nPrefSuit);
			}
			else if ( (ISMAJOR(nCheapestSuit) && (bidState.fPts >= PT_COUNT(9))) ||
				(ISMINOR(nCheapestSuit) && (bidState.fPts >= PT_COUNT(12))) )
			{
				// stop at game in the cheapest suit
				nBid = bidState.GetGameBid(nCheapestSuit);
				status << "MCLRH22! But with only " & bidState.fPts & " pts in hand, forget about slam and stop at game in the cheapest suit ("&
						  STS(nCheapestSuit) & ") at " & BTS(nBid) & ".\n";
				bidState.SetBid(nBid);
			}
			else 
			{
				// 8 or fewer pts -- bail out in the cheapest suit
				nBid = bidState.GetCheapestShiftBid(nCheapestSuit);
				status << "MCLRH23! But with only " & bidState.fPts & " pts in hand, forget about slam and bail out in the cheapest suit (" &
						  STS(nCheapestSuit) & ") at " & BTS(nBid) & ".\n";
				bidState.SetBid(nBid);
			}
			// done
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return TRUE;
		}

		//
		// else partner made a signoff, invitational, or game bid
		//
		if (bidState.IsGameBid(nPartnersBid))
		{
			status << "MCLRH30! Partner responded to our Michaels Cue bid with a game bid of " & 
					  bidState.szPB & ", so we should pass.\n";
			nBid = BID_PASS;	
		}
		else if (nPartnersBid != BID_PASS) 
		{
			// see if we should raise partner to game
			status << "2MCLRH40! Partner responded to our Michaels Cue bid with a bid of " & 
					  bidState.szPB & ", a possible invitation to game.\n";
			// go to game with 17+ pts (strong hand)
			if (fPts < PT_COUNT(17))
			{
				nBid = BID_PASS;	
				status << "MCLRH41! But since we opened Michaels with a weak hand (" & fPts &
						  ") pts, we likely do not have the points for game and have to pass.\n";
			}
			else
			{
				nBid = bidState.GetGameBid(nPartnersSuit);	
				status << "MCLRH42! and since we opened Michaels with a strong hand (" & fPts &
						  ") pts, we can raise to game at " & BTS(nBid) & ".\n";
			}
		}
		else
		{
			status << "2MCLRH49! Partner passed" & (bidState.bLHOInterfered? "after opponents interference" : "") &
					  ", so Michaels is off.\n";
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return FALSE;
		}

		// done
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;

	}
	else if (nStatus == CONV_INVOKED_ROUND2)
	{
		// last time, we either showed our minor suit or indicated our strength 
		// with another cue bid of the opponents' suit

		// first check for a strange response
		if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
		{
			// we don't understand partner's bid
			return CConvention::HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status);
		}

		// did we have the strong hand?
		if (bidState.fPts >= PT_COUNT(17))
		{
			// see what partner's reaction was
			if (nPartnersSuit == BID_4NT)
			{
				// parter still wants to see our minor!!!
				int nSuit = bidState.GetLongerSuit(CLUBS, DIAMONDS);
				nBid = bidState.GetCheapestShiftBid(nSuit);
				status << "MCLRH50! After our second Michaels cue bid of the enemy suit, partner still wants to see the unknown minor, so show it (" &
						  STS(nSuit) & ") with a bid of " & BTS(nBid) & ".\n";
				bidState.SetBid(nBid);
				bidState.SetConventionStatus(this, CONV_INVOKED_ROUND3);
				return TRUE;
			}

			// else partner's bid was natural
			if (bidState.IsGameBid(nPartnersBid))
			{
				if (fPts < PT_COUNT(20))
				{
					status << "MCLRH55! Partner responded to our second Michaels cue bid with a game bid of " & 
							  bidState.szPB & ", so we should pass.\n";
					nBid = BID_PASS;
				}
				else
				{
					// jump to slam
					nBid = MAKEBID(nPartnersSuit, 6);
					status << "MCLRH55! Partner responded to our second Michaels cue bid with a game bid of " & 
							  bidState.szPB & ", and with " & fPts & "+ in hand, we can go ahead and jump to slam at " & BTS(nBid) & ".\n";
				}
			}
			else if (nPartnersBid != BID_PASS)
			{
				// see if we should raise partner to game
				nBid = bidState.GetGameBid(nPartnersSuit);
				status << "MCLRH56! Partner responded to our second Michaels cue bid of the enemy suit with a bid of " & 
						  bidState.szPB & ", an invitation to game -- so go ahead and bid game at " & BTS(nBid) & ".\n";
			}
			else
			{
				if (bidState.bLHOInterfered)
					status << "MCLRH58! Partner passed our second Michaels cue bid after interference, so we have to pass also.\n";
				else
					status << "MCLRH59! Partner passed in spite of our second Michaels cue bid, so we should pass also.\n";
				nBid = BID_PASS;
			}
		}
		else
		{
			// partner responded to our minor answer
			if (bidState.IsGameBid(nPartnersBid))
			{
				status << "MCLRH60! Partner responded to our Michaels minors answer with a game bid of " & 
						  bidState.szPB & ", so we should pass.\n";
				nBid = BID_PASS;
			}
			else if (nPartnersBid != BID_PASS)
			{
				// see if we should raise partner to game
				status << "MCLRH70! Partner responded to our Michaels minor answer with a bid of " & 
						  bidState.szPB & ", a possible invitation to game.\n";

				// go to game with 17+ pts (strong hand)
				if (fPts < PT_COUNT(17))
				{
					nBid = BID_PASS;	
					status << "MCLRH72! But since we opened Michaels with a weak hand (" & fPts &
							  ") pts, we likely do not have the points for game and have to pass.\n";
				}
				else
				{
					nBid = bidState.GetGameBid(nPartnersSuit);	
					status << "MCLRH72! and since we opened Michaels with a strong hand (" & fPts &
							  ") pts, we can raise to game at " & BTS(nBid) & ".\n";
				}
			}
			else
			{
				if (bidState.bLHOInterfered)
					status << "MCLRH75! Partner passed our Michaels minor answer after interference, so we have to pass also.\n";
				else
					status << "MCLRH76! Partner passed our Michaels minor answer, so we should pass also.\n";
				nBid = BID_PASS;
			}
		}

		// done
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;

	}
	else if (nStatus == CONV_INVOKED_ROUND3)
	{
		// first check for a strange response
		if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
		{
			// we don't understand partner's bid
			return CConvention::HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status);
		}

		// we indicated our strength with another cue bid, and partner asked
		// to see our minor, _then_ bid
		if (bidState.IsGameBid(nPartnersBid))
		{
			if (fPts < PT_COUNT(20))
			{
				status << "MCLRH80! Partner responded to our Michaels minor answer with a game bid of " & 
						  bidState.szPB & ", so we should pass.\n";
				nBid = BID_PASS;
			}
			else
			{
				// jump to slam
				nBid = MAKEBID(nPartnersSuit, 6);
				status << "MCLRH81! Partner responded to our Michaels minor answer with a game bid of " & 
						  bidState.szPB & ", and with " & fPts & "+ in hand, we can go ahead and jump to slam at " & BTS(nBid) & ".\n";
			}
		}
		else if (nPartnersBid != BID_PASS)
		{
			// see if we should raise partner to game
			nBid = bidState.GetGameBid(nPartnersSuit);
			status << "MCLRH85! Partner responded to our Michaels minor answer with a bid of " & 
					  bidState.szPB & ", an invitation to game -- so go ahead and bid game at " & BTS(nBid) & ".\n";
		}
		else
		{
			if (bidState.bLHOInterfered)
				status << "MCLRH86! Partner passed our Michaels minor answer after interference, so we have to pass also.\n";
			else
				status << "MCLRH87! Partner passed in spite of our Michaels minor answer, so we should pass also.\n";
			nBid = BID_PASS;
		}

		// done (finally!)
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
CMichaelsCueBidConvention::CMichaelsCueBidConvention() 
{
	// from ConvCodes.h
	m_nID = tidMichaels;
}

CMichaelsCueBidConvention::~CMichaelsCueBidConvention() 
{
}


