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
	if (hand.GetNumSuitsOfAtLeast(6) < 1)
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
	if ( ((numCards == 6) && (numHonors < 3)) ||
					 ((numCards >= 7) && (numHonors < 2)) )
		 return FALSE;

	//
	// then check for Aces, Kings, or Queens in outside suits
	int nBid;
	BOOL bViolation = FALSE;
	for(int i=0;i<4;i++) 
	{
		if (i == nSuit)
			continue;
		// a solitary Q is okay for a Shutout bid, but any higher honors
		// are not
		CSuitHoldings& suit = hand.GetSuit(i);
		if (suit.HasAce() || suit.HasKing())
			return FALSE;
	}

	//
	if (bidState.numCardsInSuit[nSuit] >= 8) 
	{
		nBid = MAKEBID(nSuit,4);
		status << "D00! Have a " & bidState.numCardsInSuit[nSuit] & 
				  "-card " & STSS(nSuit) & 
				  " suit with no tricks outside the suit, so make a shutout bid of " & 
				  BTS(nBid) & ".\n";
	} 
	else if (bidState.numCardsInSuit[nSuit] >= 7) 
	{
		nBid = MAKEBID(nSuit,3);
		status << "D04! Have a " & bidState.numCardsInSuit[nSuit] & 
				  "-card " & STSS(nSuit) & 
				  " suit with no tricks outside the suit, so make a shutout bid of " & 
				  BTS(nBid) & ".\n";
	}
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
	// the requiremetns are:
	// 1: 4 cards in the suit,
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
	
	//
	if ((nPartnersBidLevel == 3) && (ISMAJOR(nPartnersSuit)) &&
		(numSupportCards >= 4) && (numQuickTricks >= 2) && 
		(numLikelyWinners >= 5) && (fMinTPPoints >= PTS_GAME)) 
	{
		nBid = MAKEBID(nPartnersSuit,4);
		status << "SHUTR2! We have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
				  " points in hand, for a total of " & fMinTPPoints & "-" & fMaxTPPoints &
				  " points in the partnership, strong support for partner's long " &
				  bidState.szPSS & " suit (holding " & bidState.szHP & 
				  "), plus " & numQuickTricks & " QTs and " & numLikelyWinners & 
				  " likely winners, so we can safely bid game at " & BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		return TRUE;
	}

	// raise a minor to game as above, but with 5-card
	// support && 6+ winners && 28+ TPs
	if ((nPartnersBidLevel <= 4)  && (ISMINOR(nPartnersSuit)) &&
		(numSupportCards >= 5) && (numQuickTricks >= 2) && 
		(numLikelyWinners >= 6) && (fMinTPPoints >= PTS_MINOR_GAME)) 
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
CShutoutBidsConvention::HandleConventionResponse(const CPlayer& player, 
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


