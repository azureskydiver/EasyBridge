//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// ShutoutBidsConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "PlayerStatusDialog.h"
#include "ShutoutBidsConvention.h"
#include "playeropts.h"              // NCR added for tbTeamIsVulnerable




//
//===============================================================================
//
// TryConvention()
//
// check if we can open with a preemptive 3 or 4 bid
//
BOOL CShutoutBidsConvention::TryConvention(const CPlayer& player, 
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

	//
	//--------------------------------------------------------
	//
	// Now check for a shutout bid of 3 or 4
	//
	// at the very minimum, should have a 6-card suit
	//
	// NCR-72 Don't Preempt with an openning hand
	if ((hand.GetNumSuitsOfAtLeast(6) < 1) || (hand.GetHCPoints() > 10)) // NCR-72 test HCPs vs total?
		return FALSE;

	//
	// first find (a) long suit
	//
/*
	for(i=0;i<4;i++) 
	{
		if (hand.numCardsInSuit[i] >= 6) 
		{
			// a 7-card suit needs 2 honors, and a a 6-card suit needs 3
			if ( ((hand.GetNumCardsInSuit(i) >= 7) && (hand.GetNumhonorsInSuit(i) >= 2)) ||
			     ((hand.GetNumCardsInSuit(i) == 6) && (hand.GetNumhonorsInSuit(i) >= 3)) )
			break;
		}
	}
*/

	// assume the long suit is the preferred one
	int nSuit = hand.GetPreferredSuit();
	CSuitHoldings& suit = hand.GetSuit(nSuit);
	int numCards = suit.GetNumCards();
	int numHonors = suit.GetNumHonors();
	// a 6-card suit needs 3 honors, and a 7+ card suit needs 2
	int nbrHonorsReq = (player.GetValue(tbTeamIsVulnerable) ? 2 : 1); // NCR fewer if not vulnerable
	if ( ((numCards == 6) && (numHonors < 3)) ||
					 ((numCards >= 7) && (numHonors < nbrHonorsReq)) ) // NCR use var vs HC 2
		 return FALSE;

	//
	// then check for Aces, Kings, or Queens in outside suits
	int nBid = BID_NONE;  // NCR init bid for later test!!!
	int nbrOutsideHonors = 0;  // NCR-67
	for(int i=0;i<4;i++) 
	{
		if (i == nSuit)
			continue;
		// a solitary Q is okay for a Shutout bid, but any higher honors
		// are not
		CSuitHoldings& suit = hand.GetSuit(i);
		if (suit.HasAce() || (suit.HasKing() && !suit.IsSingleton())) { // NCR-67 don't count singleton Kings
			nbrOutsideHonors++;  // NCR-67 count number of outside honors
//			return FALSE;
			if (suit.HasAce() && suit.HasKing())   // NCR-719 what if has both
				nbrOutsideHonors++;
		}
	}

	//
	if ((bidState.numCardsInSuit[nSuit] >= 8) && (nbrOutsideHonors <2)) //NCR-67
	{
		nBid = MAKEBID(nSuit,4);
		status << "D00! Have a " & bidState.numCardsInSuit[nSuit] & 
				  "-card " & STSS(nSuit) & 
				  " suit with no/few tricks outside the suit, so make a shutout bid of " & 
				  BTS(nBid) & ".\n";
	} 
	else if ((bidState.numCardsInSuit[nSuit] >= 7) && (nbrOutsideHonors < 2) // NCR-67
		    && (hand.GetHCPoints() >= OPEN_PTS(6))  )  // NCR-314 Have some HCPs
	{
		nBid = MAKEBID(nSuit,3);
		status << "D04! Have a " & bidState.numCardsInSuit[nSuit] & 
				  "-card " & STSS(nSuit) & 
				  " suit with no tricks outside the suit, so make a shutout bid of " & 
				  BTS(nBid) & ".\n";
	}
	if(nBid == BID_NONE)  // NCR make sure something found!!!
		return FALSE;

	bidState.SetBid(nBid);
	bidState.SetConventionStatus(this, CONV_INVOKED);
	return TRUE;
}






//
//=========================================================
//
// RespondToConvention()
//
// respond to partner's preemptive shutout bid of 3, 4, or 5
//
BOOL CShutoutBidsConvention::RespondToConvention(const CPlayer& player, 
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
	// Bidding in response to an opening preemptive shutout bid? 
	// check requirements
	//
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int nPartnersSuit = bidState.nPartnersSuit;

	//
	// check to see if partner did in fact make a shutout bid
	//
	if (((nPartnersBidLevel >= 3) &&  (nPartnersBidLevel <= 5)) && 
						(bidState.m_numPartnerBidsMade == 1) &&
						(nPartnersSuit != NOTRUMP) &&
						(bidState.m_bPartnerOpenedForTeam))
	{
		// NCR one more test - was pards bid an overcall? eg 3C over a 2S weak
		BidType bidType = bidState.GetBidType(nPartnersBid);
/*
		int nTheBidBeforePards = nPartnersBid;  //NCR use last bid before pards
		for(int i = pDOC->GetNumBidsMade()-1; i >=0; i--) {
			int aBid = pDOC->GetBidByIndex(i);
			if(ISBID(aBid) && aBid != nPartnersBid) {
				nTheBidBeforePards = aBid;  // save
				break;  // exit when found
			}
		}
        if((nPartnersBid - nTheBidBeforePards) < 5)
*/
		                         // NCR-287 added Response   // NCR-155 and NOT leap
		if(((bidType & (BT_Overcall + BT_Response)) != 0) && ((bidType & BT_Leap) == 0))  // NCR test if overcall
			return FALSE;  // Overcall or Response and not Leap

		// okay, met requirements
	}
	else
	{
		// failed the test
		return FALSE;
	}

	//
	// in general, pass a shutout bid unless we have an exceedingly 
	// strong hand
	//
	 
	// first state expectations
	bidState.m_fPartnersMin = OPEN_PTS(6);
	bidState.m_fPartnersMax = OPEN_PTS(9);
	status << "SHUTR0! Partner made a preemptive " & bidState.szPB & 
			  " bid, showing a " & 
			  ((bidState.nPartnersBidLevel==3)? 7:(bidState.nPartnersBidLevel==4)? 8:9) &
			  "-card " & bidState.szPSS & " suit with no tricks outside the suit, and most likely " &
			  bidState.m_fPartnersMin & "-" & bidState.m_fPartnersMax & " points.\n";

	// adjust team point estimates
	double fPts = bidState.fPts;
	double fAdjPts = bidState.fAdjPts;
	double fCardPts = bidState.fCardPts;
	bidState.m_fMinTPPoints = fAdjPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPPoints = fAdjPts + bidState.m_fPartnersMax;
	bidState.m_fMinTPCPoints = fCardPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPCPoints = fCardPts + bidState.m_fPartnersMax;

	// raise a 3 bid to game if we have enough to make game
	// the requirements are:
	// 1: 4 cards in the suit, NCR change to 3
	// 2: 5 playing tricks for a major suit, or 6 for a minor
	// 3: 2 QT's, and
	// 4: >= 26 team pts for a major game, or >= 29 pts for a minor game
	int nBid;
	int numSupportCards = bidState.numSupportCards;
	int nPartnersSuitSupport = bidState.nPartnersSuitSupport;
	double numQuickTricks = bidState.numQuickTricks;
	int numLikelyWinners = bidState.numLikelyWinners;
	double fMinTPPoints = bidState.m_fMinTPPoints;
	double fMaxTPPoints = bidState.m_fMaxTPPoints;
	
	// NCR-728 Pard promised 7 tricks if vulnerable and 6 tricks if not
	// Compute how many winners we need to make 4: (7+3) or (6+4)
	int nbrWinnersNeeded = player.GetValue(tbTeamIsVulnerable) ? 3 : 4; // NCR-728
	// NCR-155 weaken requirements if being agressive
	if(theApp.GetBiddingAgressiveness() >= 1) // NCR-155
		nbrWinnersNeeded--;  // reduce ??
	// NCR-155 Add 1/2 QT and one winner if we have the King of trump
	if(bidState.numSuitPoints[nPartnersSuit] == 3) {
		numQuickTricks += 0.5;
		numLikelyWinners++;
	}
	if ((nPartnersBidLevel == 3) && (ISMAJOR(nPartnersSuit))
		   // NCR changed 4 to 3       /// NCR-359 allow 2 cards with points
		&& ((numSupportCards >= 3) 
		    || ((numSupportCards >= 2) && (fCardPts >= 13))
			|| ((numSupportCards >= 1) && (fCardPts >=19))   // NCR-546 fewer cards with more pts
			|| (numQuickTricks >= 5) )  // NCR-728 5 QT will be big help
		// NCR changed following to allow either tricks or points
		&& (((numQuickTricks >= 2) && (numLikelyWinners >= nbrWinnersNeeded)) 
		   // NCR-672 Also if QTs and suit points
		   || ((numQuickTricks > 3) && (bidState.numSuitPoints[nPartnersSuit] > 4))
		   || ((fMinTPPoints + fMaxTPPoints)/2 >= PTS_GAME)    // NCR-546 average min and max
		   || ((numSupportCards > 4) && (bidState.numVoids > 0) && (bidState.numSingletons <= 1)) ) ) // NCR-773 Distribution will help
	{
		nBid = MAKEBID(nPartnersSuit,4);
		status << "SHUTR2! We have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
				  " points in hand, for a total of " & fMinTPPoints & "-" & fMaxTPPoints &
				  " points in the partnership, strong support for partner's long " &
				  bidState.szPSS & " suit (holding " & bidState.szHP & 
				  "), plus " & numQuickTricks & " QTs and " & numLikelyWinners & 
				  " likely winners, so we can " 
				  & ((nbrWinnersNeeded == 5) ? "safely bid" : "try")  // NCR-155
				  & " game at " & BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		return TRUE;
	}

	// raise a minor to game as above, but with 5-card
	// support && 6+ winners && 28+ TPs

	// NCR-728 Pard promised 7 tricks if vulnerable and 6 tricks if not
	// Compute how many winners we need to make 5: (7+4) or (6+5)
	nbrWinnersNeeded = player.GetValue(tbTeamIsVulnerable) ? 4 : 5; // NCR-728

//	nbrWinnersNeeded = (theApp.GetBiddingAgressiveness() < 1) ? 6 : 5; // NCR-281
	if ((nPartnersBidLevel <= 4)  && (ISMINOR(nPartnersSuit))
		&& (((numSupportCards >= 3) && (numQuickTricks >= 2))    // NCR changed SupportCards fm 5 to 3
			|| (numQuickTricks >= 5))  // NCR-728
		// NCR-281 changed to match NCR-155 above. Use var vs HC 6 below
		&& ((numLikelyWinners >= nbrWinnersNeeded) || (fMinTPPoints >= PTS_MINOR_GAME)) 
		&& (bidState.numAces > 0) )  // NCR-734 Need an ace at 5 level
	{
		nBid = MAKEBID(nPartnersSuit,5);
		status << "SHUTR4! We have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
				  " points in hand, for a total of " & fMinTPPoints & "-" & fMaxTPPoints &
				  " points in the partnership, strong support for partner's long " &
				  bidState.szPSS & " suit (holding " & bidState.szHP & 
				  "), plus " & numQuickTricks & " QTs and " & numLikelyWinners & 
				  " likely winners, so we can safely bid a minor game at " & BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		return TRUE;
	}

	// raise a 3 or 4 bid to slam with strong support,
	// >= 32 TPs, and 4 QTs
	if ((nPartnersBidLevel < 5) && (nPartnersSuitSupport >= SS_GOOD_SUPPORT) &&
		(numQuickTricks >= 4) && (fMinTPPoints >= PTS_SLAM)) 
	{
		nBid = MAKEBID(nPartnersSuit,6);
		status << "SHUTR6! We have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
				  " points in hand, for a total of " & fMinTPPoints & "-" & fMaxTPPoints &
				  " points in the partnership, " & bidState.SLTS(nPartnersSuit) &
				  " support for partner's long " & bidState.szPSS & 
				  " suit (holding " & bidState.szHP & "), plus " & numQuickTricks & 
				  " QTs, so we can safely bid a slam at " & BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		return TRUE;
	}
		
	// NCR-219 Bid our suit if we have a long suit
	int nSuit = hand.GetPreferredSuit();
	CSuitHoldings& suit = hand.GetSuit(nSuit);
	int numCards = suit.GetNumCards();
	// NCR-502 Bid our suit if major and there is a possiblity
	if((numCards > 7) || (ISMAJOR(nSuit) && (numCards > 5) && (fMinTPPoints > PTS_GAME-3) 
		                   && (numLikelyWinners >= 9)) )
	{	
		nBid = MAKEBID(nSuit, 4);
		status << "SHUTR7! We have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
				  " points in hand, for a total of " & fMinTPPoints & "-" & fMaxTPPoints &
				  " points in the partnership, a long " & STS(nSuit) & " suit, "
				   & numQuickTricks & " QTs and " & numLikelyWinners & 
				  " likely winners, so we can try " & BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		return TRUE;
    }

	// else, just pass
	nBid = BID_PASS;
	status << "SHUTR9! With " & fCardPts & "/" & fPts & "/" & fAdjPts & 
			  " points in hand, for a total of " & fMinTPPoints & "-" & fMaxTPPoints &
			  " points in the partnership, holding " & bidState.szHP & 
			  " in support of partner's " & bidState.szPSS & " suit, and with only " & 
			  numQuickTricks & " QTs and " & numLikelyWinners & 
			  " likely winners, we have no real reason to bid anything, so just pass.\n";
	bidState.SetBid(nBid);
	return TRUE;
}





//
//==========================================================
//
// HandleConventionResponse()
//
// Rebidding after an opening shutout bid
//
//
BOOL CShutoutBidsConvention::HandleConventionResponse(const CPlayer& player,   // NCR added BOOL
											     const CConventionSet& conventions, 
											     CHandHoldings& hand, 
											     CCardLocation& cardLocation, 
												 CGuessedHandHoldings** ppGuessedHands,
											     CBidEngine& bidState,  
											     CPlayerStatusDialog& status)
{
	if (bidState.GetConventionStatus(this) != CONV_INVOKED)
		return FALSE;

	//
	//
	//
	return FALSE;
}







//
//==================================================================
// construction & destruction
//
CShutoutBidsConvention::CShutoutBidsConvention() 
{
	// from ConvCodes.h
	m_nID = tidShutoutBids;
}

CShutoutBidsConvention::~CShutoutBidsConvention() 
{
}


