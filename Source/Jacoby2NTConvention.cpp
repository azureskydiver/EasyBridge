//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Jacoby2NTConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "Player.h"
#include "PlayerStatusDialog.h"
#include "Jacoby2NTConvention.h"
#include "OvercallsConvention.h"
#include "ConventionSet.h"




//
//===============================================================================
//
// TryConvention()
//
// check if we can use an Jacoby 2NT Bid here
//
BOOL CJacoby2NTConvention::TryConvention(const CPlayer& player, 
										 const CConventionSet& conventions, 
										 CHandHoldings& hand, 
										 CCardLocation& cardLocation, 
										 CGuessedHandHoldings** ppGuessedHands,
										 CBidEngine& bidState,  
										 CPlayerStatusDialog& status)
{
	//
	// the requirements for an Jacoby 2NT Bid are:
	// 1: Partner must have opened with 1 of a major
	// 2: we must not have bid yet
	// 3: we have 13+ points and 4+ card trump support
	// 4: RHO passed or bid at one level NCR-253

	int nOpeningBid = pDOC->GetOpeningBid();
	int nPartnersBid = bidState.nPartnersBid;

	// test conditions 1 - 4
	if ( ISBID(nOpeningBid) && (nOpeningBid == nPartnersBid) && 
		 (ISMAJOR(BID_SUIT(bidState.nPartnersBid))) && (bidState.nPartnersBidLevel == 1) &&
		 (bidState.m_numBidTurns == 0) && (bidState.fAdjPts >= OPEN_PTS(13)) &&  // NCR-599 fAdjPts vs fPts
		 (bidState.numSupportCards >= 4) 
		 && ((bidState.nRHOBid == BID_PASS) || (bidState.nRHOBidLevel == 1)) ) // NCR-253
	{
		 // passed the test
	}
	else
	{
		return FALSE;
	}
	
	// calc adjusted pts
	int nSuit = bidState.nPartnersSuit;
	bidState.SetAgreedSuit(nSuit);
	bidState.fAdjPts = hand.RevalueHand(REVALUE_DUMMY, nSuit, TRUE);

	//
	status << "J2N1! Partner opened " & BTS(nPartnersBid) & ", and with " &
			  bidState.fAdjPts & " pts in hand and " & bidState.numSupportCards & 
			  "-card trump support, we can bid Jacoby 2NT to ask for partner's strength.\n";
	bidState.SetBid(BID_2NT);
	bidState.SetConventionStatus(this, CONV_INVOKED);
	return TRUE;
}





//
//-----------------------------------------------------
//
// respond to partner's Jacoby 2NT Bid
//
BOOL CJacoby2NTConvention::RespondToConvention(const CPlayer& player, 
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
	int nPreviousBid = bidState.nPreviousBid;
	int numTotalBidTurns = pDOC->GetNumBidsMade();

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
		// Bidding in response to partner's Jacoby 2NT bid? 
		//
		// the requirements for a Jacoby 2NT Bid are:
		// 1: we must have opened the bidding with 1 of a major
		// 2: Partner responded with 2NT
		int nOpeningBid = pDOC->GetOpeningBid();

		// test conditions
		if ( (bidState.m_numBidTurns == 1) && (ISMAJOR(BID_SUIT(nPreviousBid))) && 
			(nOpeningBid == nPreviousBid) && (BID_LEVEL(nPreviousBid) == 1) && 
			 (bidState.nPartnersBid == BID_2NT) )
		{
			 // passed the test
		}
		else
		{
			return FALSE;
		}

		status << "J2N20! Partner has made a Jacoby 2NT inquiry bid, indicating " & 
				  OPEN_PTS(13) & " pts and 4+ card support.\n";

		// adjust points as declarer
		int nSuit = bidState.nPreviousSuit;
		bidState.SetAgreedSuit(nSuit);
		fPts = bidState.fAdjPts = hand.RevalueHand(REVALUE_DECLARER, nSuit, TRUE);

		// partner has 13+ pts
		bidState.AdjustPartnershipPoints(13, 13);

		//
		// our options are as follows, in order:
		// NCR Points as per Root/Pavlicek in ()
		//
		// - with 18+(16) pts, rebid the suit at the 3 level
		// - with 15-17 pts and a strong 5-card side suit, bid that suit at the 4-level
		// - with 15-17 pts and a short suit, bid the short suit at the 3 level
		// - with 15-17 pts an no short suit, bid 3NT
		// - otherwise, sign off in game at the 4-level

		// check point count
		if (fPts >= PT_COUNT(18)) // NCR-228 Added PT_COUNT()
		{
			nBid = MAKEBID(nSuit, 3);
			status << "J2N21! With " & fPts & " points in hand, "
					  "respond to partner's Jacoby 2NT inquiry with a bid of " & BTS(nBid) & ".\n";
		}
		else if (fPts >= 15)
		{
			// see if we have a strong side suit
// NCR			int nSideSuit = NONE;
			int i; // NCR-FFS added here, removed below
			for(/*int*/ i=0;i<4;i++)
			{
				if ((i != nSuit) && (bidState.nSuitStrength[i] <= SS_STRONG) &&
						(bidState.numCardsInSuit[i] >= 5))
					break;
			}
			//
			                // NCR-437 Problem if we bid 4C and Gerber is active
			if ((i < 4) && ((i != CLUBS) || !pCurrConvSet->IsConventionEnabled(tidGerber)) )
			{
				// bid the suit at the 4 level
				nSuit = i;
				nBid = MAKEBID(nSuit, 4);
				status << "J2N22! With " & fPts & " points in hand and a good " & 
						  bidState.numCardsInSuit[nSuit] & "-card suit in " & STS(nSuit) & 
						  ", respond to partner's Jacoby 2NT inquiry with " & BTS(nBid) & ".\n";
			}
			else if (bidState.numVoids >= 1)
			{
				// bid the void suit
				for(nSuit=0;nSuit<4;nSuit++)
				{
					if (bidState.numCardsInSuit[nSuit] == 0)
						break;
				}
				nBid = MAKEBID(nSuit, 3);
				status << "J2N24! With " & fPts & " points in hand and a void suit in " & STS(nSuit) & 
						  ", respond to partner's Jacoby 2NT inquiry with a bid of " & BTS(nBid) & ".\n";
			}
			else if (bidState.numSingletons >= 1)
			{
				// bid the singleton
				for(nSuit=0;nSuit<4;nSuit++)
				{
					if (bidState.numCardsInSuit[nSuit] == 1)
						break;
				}
				nBid = MAKEBID(nSuit, 3);
				status << "J2N26! With " & fPts & " points in hand and a singleton in " & STS(nSuit) & 
						  ", respond to partner's Jacoby 2NT inquiry with a bid of " & BTS(nBid) & ".\n";
			}
			else
			{
				// bid 3NT
				nBid = BID_3NT;
				status << "J2N28! With " & fPts & " points in hand and no short suits, " 
						  " respond to partner's Jacoby 2NT inquiry with a bid of " & BTS(nBid) & ".\n";
			}
		}
		else
		{
			// sign off at the 4-level
			nBid = MAKEBID(nSuit, 4);
			status << "J2N31! With only " & fPts & " points in hand, sign off in game at " & 
					  BTS(nBid) & ".\n";
		}

		// and return
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;
	}

	//
	return FALSE;
} 







//
//==========================================================
//
// Rebidding as opener after partner responds to an Jacoby 2NT Bid
//
BOOL CJacoby2NTConvention::HandleConventionResponse(const CPlayer& player, 
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

	// get some info
	//
	int nBid = NONE;
	int nPrevSuit = bidState.nPartnersPrevSuit;
	int nSuit = bidState.nPartnersSuit;
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int numSupportCards = bidState.numSupportCards;

	//
	// handling partner's Drury response
	//
	int nStatus = bidState.GetConventionStatus(this);

	if (nStatus == CONV_INVOKED)
	{
		// NCR-518 Use pard's previous suit if he passed
		int testSuit = (nPartnersBid == BID_PASS) ? nPrevSuit : nSuit;
		// NCR-265 Need to revalue hand as AssessPosition() @ ln 918 has set it to fPts
		double fTestAdjPts = hand.RevalueHand(REVALUE_DUMMY, testSuit, TRUE); // NCR-265 recompute
		//
		// here, our actions depend on partner's response
		//
		if (bidState.nPartnersBid == MAKEBID(nPrevSuit, 3))
		{
			// partner responded in the suit at the 3-level, for 18+ pts
			status << "J2N40! Partner responded to our Jacoby 2NT inquiry by rebidding his " & 
					   STSS(nPrevSuit) & " suit at the 3-level, indicating " & 
					   OPEN_PTS(18) & "+ points.\n";

			// revalue partnership totals
			bidState.fAdjPts = fTestAdjPts;  // NCR-265 restore to what we used originally
			bidState.AdjustPartnershipPoints(18, pCurrConvSet->GetValue(tn2ClubOpeningPoints));
		}
		else if ((nPartnersBidLevel == 3) && ISSUIT(nSuit) && (nSuit != nPrevSuit))
		{
			// partner responded in a different suit at the 3-level
			status << "J2N41! Partner responded to our Jacoby 2NT inquiry by bidding the " & 
					   STSS(nSuit) & " suit at the 3-level, indicating " & OPEN_PTS(15) & "-" & OPEN_PTS(17) & 
					   " points and a singleton or void in " & STS(nSuit) & ".\n";

			// revalue partnership totals
			bidState.fAdjPts = fTestAdjPts;  // NCR-265 restore to what we used originally
			bidState.AdjustPartnershipPoints(15, 17);
		}
		else if (nPartnersBid == BID_3NT)
		{
			// partner responded with 3NT
			status << "J2N42! Partner responded to our Jacoby 2NT inquiry by bidding 3NT, indicating " &
					   OPEN_PTS(15) & "-" & OPEN_PTS(17) & " points with a balanced hand.\n";

			// revalue partnership totals
			bidState.fAdjPts = fTestAdjPts;  // NCR-265 restore to what we used originally
			bidState.AdjustPartnershipPoints(15, 17);
		}
		else if ((nPartnersBidLevel == 4) && ISSUIT(nSuit) && (nSuit != nPrevSuit))
		{
			// partner responded in a different suit at the 4-level
			status << "J2N44! Partner responded to our Jacoby 2NT inquiry by bidding the " & 
					   STSS(nSuit) & " suit at the 4-level, indicating a strong 5-card side suit and " & 
					   OPEN_PTS(15) & "-" & OPEN_PTS(17) & " points in the hand.\n";

			// revalue partnership totals
			bidState.fAdjPts = fTestAdjPts;  // NCR-265 restore to what we used originally
			bidState.AdjustPartnershipPoints(15, 17);
		}
		else if ((nPartnersBidLevel == 4) && (nSuit == nPrevSuit))
		{
			// partner responded in the original suit at the 4-level
			status << "J2N44! Partner responded to our Jacoby 2NT inquiry by rebidding his " & 
					   STSS(nSuit) & " suit at the 4-level, indicating a minimum opener of approx. " & 
					   OPEN_PTS(12) & "-" & OPEN_PTS(14) & " points in the hand.\n";

			// revalue partnership totals
			bidState.fAdjPts = fTestAdjPts;  // NCR-265 restore to what we used originally
			bidState.AdjustPartnershipPoints(12, 14);
		}
		else if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
		{
			// the convention is cancelled!
			bidState.SetConventionStatus(this, CONV_ERROR);
			return FALSE;
		} 
		else if((bidState.nLHOBid != BID_PASS) && (nPartnersBid == BID_PASS))  // NCR-518 test if interference
		{
			bidState.fAdjPts = fTestAdjPts;  // NCR-265 restore to what we used originally
			bidState.AdjustPartnershipPoints(12, 14);
		} // end NCR-518

		//
		// now figure out what to do
		//
		if (bidState.m_fMinTPPoints >= PTS_SLAM)
		{
			// go to Blackwood
			status << "J2N60! With a total of " & bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " pts in the partnership, push on to slam in partner's " & STSS(nPrevSuit) & " suit.\n";
			bidState.InvokeBlackwood(nPrevSuit);
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return TRUE;
		}
		else if (bidState.m_fMinTPPoints >= PTS_MAJOR_GAME)
		{
			// we want to bid game
			if (nPartnersBid < bidState.GetGameBid(nPrevSuit))
			{
				// raise or shift to game
				nBid = bidState.GetGameBid(nPrevSuit);
				status << "J2N62! With a total of " & bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
						  " pts in the partnership, go to game in " & STS(nPrevSuit) &
						  " with a bid of " & BTS(nBid) & ".\n";
			}
			else
			{
				// here partner bid game or higher -- pass unless it needs correction
				if (nSuit == nPrevSuit)
				{
					nBid = BID_PASS;
					status << "J2N64! With a total of " & bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
							  " pts in the partnership, pass partner's " & BTS(nPartnersBid) & " bid.\n";
				}
				else
				{
					// correct to the original suit
					nBid = bidState.GetCheapestShiftBid(nPrevSuit, nPartnersBid);
					status << "J2N66! With a total of " & bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
							  " pts in the partnership, we want to stop at game in " & STS(nPrevSuit) & 
							  "; correct partner's " & BTS(nPartnersBid) & " bid to " & BTS(nBid) & ".\n";
				}
			}
		}
		else
		{
			// oops, caught with too few points
			// either pass 3NT , or return to the suit at the cheapest level possible
			if ((nPartnersBid == BID_3NT) || ((nSuit == nPrevSuit)))
			{
				nBid = BID_PASS;
				status << "J2N70! With a total of only " & bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
						  " pts in the partnership, we have to stop here at " & BTS(nPartnersBid) &
						  ", so pass.\n";
			}
			else
			{
				nBid = bidState.GetCheapestShiftBid(nPrevSuit, nPartnersBid);
				status << "J2N72! With a total of only " & bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
						  " pts in the partnership, we have to return to the " & STSS(nPrevSuit) & 
						  " and stop at " & BTS(nBid) & ".\n";
			}
		}

		// done
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
CJacoby2NTConvention::CJacoby2NTConvention() 
{
	// from ConvCodes.h
	m_nID = tidJacoby2NT;
}

CJacoby2NTConvention::~CJacoby2NTConvention() 
{
}


