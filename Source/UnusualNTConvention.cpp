//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// UnusualNTConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "Player.h"
#include "PlayerStatusDialog.h"
#include "UnusualNTConvention.h"
#include "OvercallsConvention.h"
#include "ConventionSet.h"




//
//===============================================================================
//
// TryConvention()
//
// check if we can use an Unusual NT Bid here
//
BOOL CUnusualNTConvention::TryConvention(const CPlayer& player, 
										 const CConventionSet& conventions, 
										 CHandHoldings& hand, 
										 CCardLocation& cardLocation, 
										 CGuessedHandHoldings** ppGuessedHands,
										 CBidEngine& bidState,  
										 CPlayerStatusDialog& status)
{
	//
	// the requirements for an Unusual NT Bid are:
	// 1: RHO must have opened a suit (not NT) at the 1-level
	// 2: this must be our first bidding opportunity
	// 3: need 5 cards in each of the two lowest unbid suits; i.e., 
	//    - if RHO bid 1C, 5 Diamonds and 5 Hearts
	//    - if RHO bid 1D, 5 Clubs and 5 Hearts
	//    - if RHO bid 1 of a major, 5 Clubs and 5 Diamonds
	// 4: need either 6-11 pts or 17+ pts
	//    (with 12-16 pts, we generally overcall or double for takeout)
	// 5: if we have 12+ pts and playing takeout doubles, cannot have a 
	//    hand suitable for a takeout (???)

	// test conditions 1, 2, & 4
	if ( ((bidState.nRHOBid >= BID_1C) && (bidState.nRHOBid <= BID_1S)) &&
		 ( ((bidState.fPts >= OPEN_PTS(6)) && (bidState.fPts <= OPEN_PTS(11))) || ((bidState.fPts >= OPEN_PTS(17))/*&& (bidState.fPts <= 18)*/) ) &&
 		  (bidState.m_numBidTurns == 0) && (bidState.m_numPartnerBidTurns == 0) )
	{
		 // passed the test
	}
	else
	{
		return FALSE;
	}

	// test condition #3
	int nOppSuit = bidState.nRHOSuit;
	BOOL bEnoughLength = FALSE;
	//
	if (nOppSuit == CLUBS)
	{
		// need 5/5 in diamonds and hearts
		if ((hand.GetNumCardsInSuit(DIAMONDS) < 5) || (hand.GetNumCardsInSuit(HEARTS) < 5))
			return FALSE;	// no can do
		status << "2UNT5! We have at least 5/5 in Diamonds and Hearts, so we can make an Unusual NT bid over the opponent's 1D opening.\n";
	}
	else if (nOppSuit == DIAMONDS)
	{
		// need 5/5 in clubs and hearts
		if ((hand.GetNumCardsInSuit(CLUBS) < 5) || (hand.GetNumCardsInSuit(HEARTS) < 5))
			return FALSE;	// no can do
		status << "2UNT6! We have at least 5/5 in Clubs and Hearts, so we can make an Unusual NT bid over the opponent's 1D opening.\n";
	}
	else 
	{
		// need 5/5 in clubs and diamonds
		if ((hand.GetNumCardsInSuit(CLUBS) < 5) || (hand.GetNumCardsInSuit(DIAMONDS) < 5))
			return FALSE;	// no can do
		status << "2UNT6! We have at least 5/5 in Clubs and Diamonds, so we can make an Unusual NT bid over the opponent's opening major.\n";
	}

	// OK, go ahead and bid 2NT
	int nBid = BID_2NT;
	status << "UNT9! With a 2-suited hand, make an Unusual NT bid over RHO's " & STS(nOppSuit) & 
			  " bid at " & BTS(nBid) & ".\n";
	bidState.SetBid(nBid);
	bidState.SetConventionStatus(this, CONV_INVOKED);
	return TRUE;
}





//
//-----------------------------------------------------
//
// respond to partner's Unusual NT Bid
//
BOOL CUnusualNTConvention::RespondToConvention(const CPlayer& player, 
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
	int nLHOSuit = bidState.nLHOSuit;
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
		// Bidding in response to partner's Michael's bid? check requirements
		//
		// the identifying marks of an Unusual NT bid are:
		// 1: we must not have bid yet
		// 2: Opponent must have bid a suit at the 1 level, and 
		// 3: partner overcalled opponent's bid with 2NT
		// apply tests #1, 2, and 3
		int nOpeningBid = pDOC->GetOpeningBid();
		int nOpeningBidder = pDOC->GetOpeningBidder();
		BOOL bOppMajor = ISMAJOR(BID_SUIT(nOpeningBid));    // NCR added BID_SUIT()
		if (ISBID(nOpeningBid) && (GetPlayerTeam(nOpeningBidder) != player.GetTeam()) &&
			 ((nOpeningBid >= BID_1C) && (nOpeningBid <= BID_1S)) &&
			  (nPartnersBid == BID_2NT) && (bidState.m_numBidsMade == 0) )
		{
			status << "UNTR10! Partner has made an Unusual NT bid of " & BTS(nPartnersBid) &
					  ", indicating 5/5 length in " &
					  (bOppMajor? "Clubs and Diamonds" : 
					   (BID_SUIT(nOpeningBid) == CLUBS)? "Hearts and Diamonds" : "Hearts and Clubs" ) & ".\n";
		}
		else
		{
			return FALSE;
		}

		// pick the superior of partner's two suits
		int nSuit;
		if (nLHOSuit == CLUBS)
			nSuit = bidState.PickSuperiorSuit(HEARTS, DIAMONDS);
		else if (nLHOSuit == DIAMONDS)
			nSuit = bidState.PickSuperiorSuit(HEARTS, CLUBS);
		else
			nSuit = bidState.PickSuperiorSuit(DIAMONDS, CLUBS);
		//
		status << "UNTR12! Given a choice, we prefer the " & STSS(nSuit) & " suit.\n";
		// save this information
//		bidState.SetConventionData(this, nSuit);

		// now adjust point count as DECLARER
		// but don't penalize for short trumps, as partner has plenty
		bidState.SetAgreedSuit(nSuit);
		fPts = bidState.fAdjPts = hand.RevalueHand(REVALUE_DECLARER | REVALUE_NOPENALTY_SHORTTRUMP, nSuit, TRUE);

		// partner may have 6-11 OR 17+ pts; assume it's a weak hand
		bidState.AdjustPartnershipPoints(6, 11);

		//
//		bidState.SetConventionStatus(this, CONV_FINISHED);

		// if we have the points for a slam or game; else bid at the 3-level
		if (bidState.m_fMinTPPoints >= PTS_SLAM)
		{
			// cue-bid the enemy suit for a slam try
			nBid = bidState.GetCheapestShiftBid(nLHOSuit, pDOC->GetLastValidBid());
			status << "UNTR20! With a total of " & 
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " pts in the partnership, cue bid the enemy " & STSS(nLHOSuit) & 
					  " suit at " & BTS(nBid) & " for a slam try.\n";
		}
		else if (bidState.m_fMinTPPoints >= bidState.GetGamePoints(nSuit))
		{
			// bid game
			nBid = bidState.GetGameBid(nSuit);
			status << "UNTR25! With a total of " & 
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " pts in the partnership, go to game at " & BTS(nBid) & ".\n";
		}
		else if (bidState.m_fMinTPPoints >= bidState.GetGamePoints(nSuit) - 3)
		{
			// game may be possible if partner opened with a strong Unusual NT hand
			// so query as to the hand type
			nBid = bidState.GetCheapestShiftBid(nLHOSuit);
			status << "UNTR26! With a total of " & 
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " pts in the partnership, we may be able to make game in " & STS(nSuit) & 
					  " if partner has a strong opener, so cue bid the enemy " & STSS(nLHOSuit) & 
					  " at " & BTS(nBid) & " to inquire about partner's strength..\n";
		}
		else
		{
			// settle for a 3-level bid
			nBid = MAKEBID(nSuit, 3);
			status << "UNTR28! With a total of " & 
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " pts in the partnership, respond at the 3-level with " & BTS(nBid) & ".\n";
		}

		// done!
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND1);
		return TRUE;

	}
	else if (nStatus == CONV_RESPONDED_ROUND1)
	{
		//
		// round 2 -- we're here only if partner bid again, either 
		// in response to our invitational bid, or if we had cue bidd 
		// the enemy suit to show a strong opening hand
		//

		// se if we made an invitational bid last time
		int nOpeningBid = pDOC->GetOpeningBid();
		ASSERT(ISBID(nOpeningBid));
		int nEnemySuit = BID_SUIT(nOpeningBid);
		BOOL bPartnerHasStrongOpener = FALSE;

		// pick superior suit
		int nSuit1, nSuit2, nSuperiorSuit;
		nLHOSuit = BID_SUIT(nOpeningBid);         // NCR-713 Get suit for below 
		if (nLHOSuit == CLUBS)
		{
			nSuit1 = DIAMONDS;
			nSuit2 = HEARTS;
		}
		else if (nLHOSuit == DIAMONDS)
		{
			nSuit1 = CLUBS;
			nSuit2 = HEARTS;
		}
		else
		{
			nSuit1 = CLUBS;
			nSuit2 = DIAMONDS;
		}
		nSuperiorSuit = bidState.PickSuperiorSuit(nSuit1, nSuit2);

		// check our previous bid
		if (bidState.nPreviousSuit == nEnemySuit)
		{
			// we had made a cue bid to show interest in slam
			// if partner bid the lower of his two suits, he is showing a weak hand
			if (nPartnersSuit == nSuit1)
			{
				status << "UNTR40! Partner rebid his lower " & STSS(nSuit1) & 
						  " suit, denying a strong opening hand.\n";
				// partner has a weak hand, so pss or correct
				if (nSuperiorSuit == nSuit1)
				{
					nBid = BID_PASS;
					status << "UNTR41! As a result, we have to pass his bid of " & BTS(nPartnersBid) &
							  " to play in the " & STSS(nPartnersSuit) & " suit.\n";
				}
				else
				{
					nBid = MAKEBID(nSuit2, BID_LEVEL(nPartnersBid));   // NCR-713 swapped order of args
					status << "UNTR42! As a result, we settle for a contract of " & BTS(nBid) & ".\n";
				}
				//
				bidState.SetBid(nBid);
				bidState.SetConventionStatus(this, CONV_FINISHED);
				return TRUE;
			}
			else if (nPartnersBid != BID_PASS)
			{
				// partner has NOT rebid his lower suit, indicating a strong opener
				// so recalc points
				bidState.AdjustPartnershipPoints(17, Min(17,40));
				//
				status << "UNTR50! After our cue-bid of the enemy suit, partner has NOT bid the lower of his two suits, "
						  " indicating a strong opening Unusual NT hand with " & OPEN_PTS(17) & 
						  "+ pts, for a total in the partnership of " & 
						  bidState.m_fMinTPPoints & "-" & bidState.m_fMinTPPoints & " pts.\n";
				// drop in to later code						  
				bPartnerHasStrongOpener = TRUE;
			}
			else
			{
				// oops, partner passed
				status << "UNTR52! Partner passed, so we pass.\n";
				bidState.SetBid(BID_PASS);
				bidState.SetConventionStatus(this, CONV_FINISHED);
				return TRUE;
			}
		}
		else 
		{
			// partner has bid again after we made a normal bid, indicating 
			// a strong opener
			if (nPartnersBid != BID_PASS)
			{
				status << "UNTR60! Partner has made another bid after his opening Unusual NT bid and our response of " & 
						  BTS(bidState.nPreviousBid) & ", indicate a strong opening Unusual NT hand with " &
						  OPEN_PTS(17) & "+ pts, for a total in the partnership of " & 
						  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & " pts.\n";
				// drop in to later code						  
				bPartnerHasStrongOpener = TRUE;
			}
			else
			{
				// oops, partner passed
				status << "UNTR62! Partner passed, so we pass.\n";
				bidState.SetBid(BID_PASS);
				bidState.SetConventionStatus(this, CONV_FINISHED);
				return TRUE;
			}
		}

		// here, partner showed a strong opener
		if (bPartnerHasStrongOpener)
		{
			// so recalc points
			bidState.AdjustPartnershipPoints(17, Min(17,40));
					  
			// if we have the points, go to slam or game in our last bid suit
			int nSuit = bidState.nPreviousSuit;
			if (bidState.m_fMinTPPoints >= PTS_SLAM)
			{
				// slam calls us!
				status << "UNTR71! So we can proceed towards slam.\n";
				bidState.InvokeBlackwood(nSuperiorSuit);
				bidState.SetConventionStatus(this, CONV_FINISHED);
				return TRUE;
			}
			else if (bidState.m_fMinTPPoints >= bidState.GetGamePoints(nSuit))
			{
				// settle at game
				nBid = bidState.GetGameBid(nSuit);
				status << "UNTR72! So go ahead and bid game at " & BTS(nBid) &".\n";
			}
			// NCR-293 Bid something if partner has cue-bid
			else if (nPartnersSuit == nEnemySuit)
			{	
				nBid = bidState.GetCheapestShiftBid(nSuit, pDOC->GetLastValidBid());
				status << "UNTR74! Partner cue-bid, So go ahead and bid something at " & BTS(nBid) &".\n";
			}
			else
			{
				// else gotta pass
				nBid = BID_PASS;
				status << "UNTR73! But this is still not enough to raise any further, so just pass.\n";
			}
			// done
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return TRUE;
		}

		// oops
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return FALSE;
	}

	//
	return FALSE;
} 







//
//==========================================================
//
// Rebidding as opener after partner responds to an Unusual NT Bid
//
BOOL CUnusualNTConvention::HandleConventionResponse(const CPlayer& player, 
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
	int nBid = BID_NONE;
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
	// handling partner's Unusual NT response
	//
	int nStatus = bidState.GetConventionStatus(this);

	// first check for a strange response
	if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
	{
		// we don't understand partner's bid
		return CConvention::HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status);
	}

	//
	if (nStatus == CONV_INVOKED)
	{
		// first determine the enemy suit
		int nOpeningBid = NONE;
		for(int i=0;i<pDOC->GetNumBidsMade();i++)
		{
			nOpeningBid = pDOC->GetBidByIndex(i);
			if (nOpeningBid != BID_PASS)
				break;
		}
		int nEnemySuit = BID_SUIT(nOpeningBid);

		// first of all, did partner cue-bid the enemy suit (a game/slam try)?
		if (nPartnersSuit == nEnemySuit)
		{
			status << "UNTRH20! Partner cue bid the enemy suit, which is a game or slam try.\n";
			int nSuit = NONE;
			if (bidState.fPts >= PT_COUNT(17))
			{
				// we have a strong opener, so respond in the higher suit
				if ((nEnemySuit == CLUBS) || (nEnemySuit == DIAMONDS))
					nSuit = HEARTS;
				else // (nEnemySuit == HEARTS) || (nEnemySuit == SPADES)
					nSuit = DIAMONDS;
				nBid = bidState.GetCheapestShiftBid(nSuit);
				status << "UNTRH21! Since we have " & bidState.fPts & " pts in hand (a strong opener), we respond in the higher of our suits (" & 
						  STS(nSuit) & ") at " & BTS(nBid) & ".\n";
			}
			else
			{
				// weak opener, so respond negatively in the lower suit
				if (nEnemySuit == CLUBS)
					nSuit = DIAMONDS;
				else // (nEnemySuit == DIAMONDS, HEARTS, or SPADES
					nSuit = CLUBS;
				// 
				nBid = bidState.GetCheapestShiftBid(nSuit);
				status << "UNTRH23! But with only " & bidState.fPts & " pts in hand (a weak opener), respond negatively by bidding the lower of our suits (" &
						  STS(nSuit) & ") suit at " & BTS(nBid) & ".\n";
			}
			//
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return TRUE;
		}

		// now see how we should respond
		// 17+ points calls for a cue bid to show our strength
		if ((bidState.fPts >= PT_COUNT(17)) && (!bidState.IsGameBid(nPartnersBid)))
		{
			// here we cue bid the enemy suit, and wait for partner to make 
			// the next move
			nBid = MAKEBID(nEnemySuit, 3);
			status << "UNTRH12! with a strong Unusual NT hand (" & bidState.fPts & 
					  " pts), cue bid the enemy suit again at " & BTS(nBid) & 
					  " to indicate our strength.\n";
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_INVOKED_ROUND2);
			return TRUE;
		}

		//
		// here, we have less than 17 points, and partner made a signoff, 
		// invitational, or game bid
		//
		if (bidState.IsGameBid(nPartnersBid))
		{
			status << "UNTRH30! Partner responded to our Unusual NT bid with a game bid of " & 
					  bidState.szPB & ", so with a weak opener we should pass.\n";
			nBid = BID_PASS;	
		}
		else if (nPartnersBid != BID_PASS) 
		{
			// partner responded at below game level
			// see if we should raise partner to game
			status << "UNTRH40! Partner responded to our Unusual NT bid with a bid of " & 
					  bidState.szPB & ", a possible invitation to game.\n";
			// 
			if (fPts < PT_COUNT(17))
			{
				nBid = BID_PASS;	
				status << "UNTRH41! But since we opened Unusual NT with a weak hand (" & fPts &
						  ") pts, we likely do not have the points for game and have to pass.\n";
			}
/*
			else
			{
				nBid = bidState.GetGameBid(nPartnersSuit);	
				status << "UNTRH42! and since we opened Unusual NT with a strong hand (" & fPts &
						  ") pts, we can raise to game at " & BTS(nBid) & ".\n";
			}
*/
		}
		else
		{
			status << "UNTRH49! Partner passed" & (bidState.bLHOInterfered? "after opponents interference" : "") &
					  ", so the Unusual NT convention is off.\n";
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
		// last time, we cue bid of the opponents' suit to show our strength
//		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return FALSE;
	}

	// oops!
	bidState.SetConventionStatus(this, CONV_ERROR);
	return FALSE;
}





//
//==================================================================
// construction & destruction
//
CUnusualNTConvention::CUnusualNTConvention() 
{
	// from ConvCodes.h
	m_nID = tidUnusualNT;
}

CUnusualNTConvention::~CUnusualNTConvention() 
{
}


