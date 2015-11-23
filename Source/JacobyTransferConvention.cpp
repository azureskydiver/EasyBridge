//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------
/*  Changes - 
     NCR added 2NT as base for the Transfer
*/

//
// JacobyTransferConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "PlayerStatusDialog.h"
#include "JacobyTransferConvention.h"
#include "ConventionSet.h"




//
//---------------------------------------------------------------
//
// TryConvention()
//
//
BOOL CJacobyTransferConvention::TryConvention(const CPlayer& player, 
											  const CConventionSet& conventions, 
											  CHandHoldings& hand, 
											  CCardLocation& cardLocation, 
											  CGuessedHandHoldings** ppGuessedHands,
											  CBidEngine& bidState,  
											  CPlayerStatusDialog& status)
{
	int nSuit = ANY,  nBid = BID_NONE;

	// see if we can ply Jacoby here 
	// requirements are:
	// 1: partner must have opned bidding with 1NT
	// 2: we must have 6+ HCPs
	// 3: RHO must have passed
	// 4: must have a 5+ card major
	// NCR-289 allow fewer pts if pard opened with 2NT
	int nReqPts = (bidState.nPartnersBid == BID_2NT) ? 4 : 6;
	if (((bidState.nPartnersBid == BID_1NT) || (bidState.nPartnersBid == BID_2NT)) //NCR add 2NT
		 && (bidState.m_bPartnerOpenedForTeam) &&
				(bidState.m_numPartnerBidsMade == 1) &&
				(bidState.bRHOPassed) &&
				(hand.GetNumSuitsOfAtLeast(5) >= 1) &&
				((bidState.fPts >= nReqPts)   // NCR-289 use var vs HC 6
    				// NCR-165 push pard to our long suit
 				 || (hand.GetNumSuitsOfAtLeast(6) >= 1)) )
	{
		// met the requirements
	}
	else
	{
		return FALSE;
	}

	// if we have game values in NoTrump and a balanced hand, 
	// don't even bother with Jacoby transfers 
	// (will bid NT game or higher directly)
	if ((bidState.bBalanced) && 
		((bidState.fCardPts + pCurrConvSet->GetNTRangeMin(1)) >= 26))
		return FALSE;

	//NCR set bid level (NT + 1)
	int nBidLevel = 2;
	if(bidState.nPartnersBid == BID_2NT)
		nBidLevel = 3;

	// see if we have a 5-card major
	if ((bidState.numCardsInSuit[HEARTS] >= 5) || (bidState.numCardsInSuit[SPADES] >= 5)) 
	{
		nSuit = bidState.GetBestSuitofAtLeast(HEARTS,SPADES,5);
		nBid = MAKEBID(nSuit-1, nBidLevel);  // NCR use nBidLevel vs hardcoded 2
  		status << "JAC0! With a " & bidState.numCardsInSuit[nSuit] & 
				  "-card major in " & STS(nSuit) &
				  ", use Jacoby to transfer by bidding " & BTS(nBid) & ".\n";
	} 
	else 
	{
		// the 5-card suit is a minor
		nSuit = bidState.GetBestSuitofAtLeast(CLUBS,DIAMONDS,5);
		if (pCurrConvSet->IsOptionEnabled(tb4SuitTransfers)) 
		{
			// with 4-card transfers, can bid 2S/2NT
			if (nSuit == CLUBS)
				nBid = MAKEBID(SPADES, nBidLevel); //BID_2S;
			else
				nBid = MAKEBID(NOTRUMP, nBidLevel); //BID_2NT;
			status << "JAC2! With a " & bidState.numCardsInSuit[nSuit] & 
					  "-card minor in " & STS(nSuit) &
					  ", and playing 4-suit transfers, use Jacoby to transfer by bidding " &
					  BTS(nBid) & ".\n";
		} 
		else 
		{
			// with simple transfers, bid 2S
			nBid = MAKEBID(SPADES, nBidLevel); //BID_2S;
			if (nSuit == CLUBS)
				status << "JAC4! With a " & bidState.numCardsInSuit[nSuit] & 
						  "-card minor in " & STS(nSuit) &
						                                // NCR added +1 below  
						  ", use Jacoby to transfer to " & (nBidLevel+1) & "C by bidding " & 
						  BTS(nBid) & ".\n";
			else
				status << "JAC6! With a " & bidState.numCardsInSuit[nSuit] & 
						  "-card minor in " & STS(nSuit) &
						  ", and playing only simple transfers, use Jacoby to transfer to 3C by bidding " &
						  BTS(nBid) & ".  We can then rebid " & (nBidLevel+1)   // NCR added +1
						  & "D at the next opportunity.\n";
		}
	}
	//
	bidState.m_nIntendedSuit = nSuit;
	bidState.SetBid(nBid);
	bidState.SetConventionStatus(this, CONV_INVOKED);
	return TRUE;
}







//
//---------------------------------------------------------------
//
// RespondToConvention()
//
//	 
BOOL CJacobyTransferConvention::RespondToConvention(const CPlayer& player, 
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

	// Jacoby Transfer convention
	int nPartnersBid = bidState.nPartnersBid;
//	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int nPartnersSuit = bidState.nPartnersSuit;
	int nPreviousBid = bidState.nPreviousBid;

	// check conditions to see if partner bid Jacoby
	// requirements are:
	// 1: we must have opned bidding with 1NT
	// 2: partner's response must be between 2D and 2NT
	// 3: LHO must have passed
	if ( (bidState.m_numBidTurns == 1) && (bidState.m_numPartnerBidsMade == 1)
		    // NCR-312 convention off if interference
//		&& ((bidState.nRHOBid == BID_PASS) || (bidState.nRHOBid == BID_DOUBLE))
                                                                     // NCR-380 Ok if LHO doubled
		&& (((nPreviousBid == BID_1NT) && ((bidState.bLHOPassed) || (bidState.nLHOBid == BID_DOUBLE))
			 && (nPartnersBid >= BID_2D) && (nPartnersBid <= BID_2NT))
			  // NCR allow 2NT also ??? what about numBidTurns and BidsMade
		    || ((nPreviousBid == BID_2NT) && (bidState.bLHOPassed) &&
			(nPartnersBid >= BID_3D) && (nPartnersBid <= BID_3NT))) ) 
	{
		// possible Jacoby transfer
	} 
	else 
	{
		return FALSE;
	}
		
	// estimate partner's strength
	int nNewSuit = NONE, nBid = BID_NONE;
	double fPts = bidState.fPts;
	double fCardPts = bidState.fCardPts;
	bidState.m_fPartnersMin = 8;
	bidState.m_fPartnersMax = MIN(22, 40 - fCardPts);
	bidState.m_fMinTPPoints = fPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPPoints = fPts + bidState.m_fPartnersMax;
	bidState.m_fMinTPCPoints = fCardPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPCPoints = fCardPts + bidState.m_fPartnersMax;

	// 4-suit transfers
	if (pCurrConvSet->IsOptionEnabled(tb4SuitTransfers)) 
	{                                    // NCR-37 also Jacoby w/4suit over 2NT: 3NT->Diamonds
		if ((nPartnersBid != BID_2NT) && (nPartnersBid != BID_3NT))	// xfer to next suit
			nNewSuit = GetNextSuit(nPartnersSuit);
		else
			nNewSuit = DIAMONDS;	// NT -> Diamonds transfer
	} 
	else 
	{
		// simple transfers
		if (nPartnersBid != BID_2NT)
			nNewSuit = GetNextSuit(nPartnersSuit);
		else
			nNewSuit = NONE;		// 2NT = no explicit transfer
	}

	//
	// what do do if partner bids 2NT w/o playing 4-suit transfers?
	// just return for now
	//
	if (nNewSuit == NONE)
		return FALSE;

	//
	// output descriptive message
	//
	if (pCurrConvSet->IsOptionEnabled(tb4SuitTransfers)) 
	{
		status << "JC10! Playing Four-suit Jacoby Transfers, partner is asking to transfer to " &
				  STS(nNewSuit) & ".\n";
	} 
	else 
	{
		if (nPartnersBid != BID_2NT)
			status << "JC12! Playing Simple Jacoby Transfers, partner is asking to transfer to " &
					   STS(nNewSuit) & ".\n";
	}

	//
	// bid the asked-for suit, but check some conditions first
	//
	// see if we had the max opening points in the range
	//
	BOOL bMaxRange = FALSE;
	int n1NTMax = pCurrConvSet->GetNTRangeMax(1);
	int n2NTMax = pCurrConvSet->GetNTRangeMax(2);
	if ((nPreviousBid == BID_1NT) && (fCardPts == n1NTMax))
		bMaxRange = TRUE;
	if ((nPreviousBid == BID_2NT) && (fCardPts == n2NTMax))
		bMaxRange = TRUE;


	//
	// Indicate strong support with a jump response 
	//
	
	//
	// see if we have a strong major suit & a maximum NT
	// opening -- make a jump response
	//
	if ((ISMAJOR(nNewSuit)) && (bMaxRange) &&
			(bidState.nSuitStrength[nNewSuit] >= SS_GOOD_SUPPORT)) 
	{
		nBid = bidState.GetJumpShiftBid(nNewSuit,nPartnersBid);
		status << "JC20! Responding with strong support in the major " & STSS(nNewSuit) &
				  " suit (holding " & bidState.SHTS(nNewSuit) & 
				  ") and max NT opening points, jump respond to " & BTS(nBid) & ".\n";
	}

	//
	// For responding strong in minors, perform special handling 
	// if we're re playing 4-suit transfers
	//
	if ((ISMINOR(nNewSuit)) &&  (bMaxRange) &&
				(bidState.nSuitStrength[nNewSuit] >= SS_GOOD_SUPPORT)) 
	{
		// 
		if (pCurrConvSet->IsOptionEnabled(tb4SuitTransfers)) 
		{
			// cheapest bid up = strong respnse
			nBid = nPartnersBid + 1;
			status << "JC22! Responding with strong support in the major " & STSS(nNewSuit) &
					  " suit (holding " & bidState.SHTS(nNewSuit) & 
					  ") and max NT opening points, respond with " & BTS(nBid) & 
					  " to indicate strong support and max opening strength (playing 4-suit transfers).\n";
		} 
		else 
		{
		    // can't really show strong support for a minor
			// when playing simple transfers
		}
	}

	//
 	// else at this point we're either playing simple transfers, 
 	// or have a plain-jane case of 4-suit transfers
	// (i.e., one that doesn't warrant a jump response)
 	// so just bid the asked-for suit
	//
	if (nNewSuit >= CLUBS) 
	{
		int nBidToBeat = (nPartnersBid > bidState.nRHOBid) ? nPartnersBid : bidState.nRHOBid; // NCR-372
		if(bidState.nRHOBid == BID_DOUBLE) 
			nBidToBeat = nPartnersBid; // NCR-377 Whoops forgot double
		nBid = bidState.GetCheapestShiftBid(nNewSuit, nBidToBeat); // NCR-312 added nRHOBid NCR-372
		// NCR-312 Test if RHO has bid and if we have the points for that level
		if((bidState.nRHOBid == BID_PASS) 
			|| ((BID_LEVEL(nBid) <= 3) &&(bidState.m_fMinTPCPoints > PTS_GAME-3)) 
			|| ((BID_LEVEL(nBid) <= 4) && (bidState.m_fMinTPCPoints >= PTS_GAME)) )
		{
			bidState.m_nIntendedSuit = nNewSuit;   // NCR-636 set suit we want
			status << "JC24! Respond to the requested " & STSS(nNewSuit) & 
					  " transfer suit with a " & BTS(nBid) & " bid.\n";
		} else { 
			nBid = BID_PASS;
			status << "JC26! Having only " & bidState.m_fMinTPCPoints & " points at the " 
				      & bidState.nRHOBidLevel & " level, we'll pass after interference\n";
		}
	}

	// done
	bidState.SetBid(nBid);
	bidState.SetConventionStatus(this, CONV_RESPONDED);
	return TRUE;
}







//
//---------------------------------------------------------------
//
// HandleConventionResponse()
//
//	 
BOOL CJacobyTransferConvention::HandleConventionResponse(const CPlayer& player, 
														 const CConventionSet& conventions, 
														 CHandHoldings& hand, 
														 CCardLocation& cardLocation, 
														 CGuessedHandHoldings** ppGuessedHands,
														 CBidEngine& bidState,  
														 CPlayerStatusDialog& status)
{
	// NCR-636 Check if we've responded and pard bid 4NT as slam invite (see JC70)
	if((bidState.nPartnersBid == BID_4NT) && (bidState.GetConventionStatus(this) == CONV_RESPONDED))
	{
		//??  Should pts be adjusted now we know pard is thinking slam???
		int bidLvl = bidState.m_fMinTPPoints < PTS_SLAM ? 5 : 6;
		int nBid = MAKEBID(bidState.m_nIntendedSuit, bidLvl);
		status << "JC80! Partner invited slam with 4NT bid. With " & bidState.m_fMinTPPoints
				   & " estimated partnership points vs " & PTS_SLAM & " needed, we bid " & BTS(nBid) & "\n";
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;
	} // end NCR-636

	if (bidState.GetConventionStatus(this) != CONV_INVOKED)  // NCR-36 CONV_INVOKED vs 1
		return FALSE;
	//
	int nBid = BID_NONE;  // NCR can't use enum because functions return int
	Suit nNewSuit = NOSUIT, nTargetSuit = NOSUIT; // NCR use enum and NOSUIT
	int nPreviousSuit = bidState.nPreviousSuit;
	Bids nPartnersBid = (Bids)bidState.nPartnersBid;  // NCR use enum
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int nPartnersSuit = bidState.nPartnersSuit;
	Bids nPreviousBid = (Bids)bidState.nPreviousBid; // NCR use enum

	//
	if (pCurrConvSet->IsOptionEnabled(tb4SuitTransfers)) 
	{
		if (nPreviousSuit != NOTRUMP)
			nNewSuit = (Suit)GetNextSuit(nPreviousSuit);
		else
			nNewSuit = DIAMONDS;
		//
		nTargetSuit = nNewSuit;
	} 
	else 
	{
		// not playing 4-suit transfers
		if (nPreviousSuit != NOTRUMP)
			nNewSuit = (Suit)GetNextSuit(nPreviousSuit);
		else
			return FALSE;	// 2NT is natural

		// see if we had bid 2S or 3S, intending to transfer to Diamonds
		if ( ((nPreviousBid == BID_2S) || (nPreviousBid == BID_3S)) &&
			 (bidState.m_nIntendedSuit == DIAMONDS) ) // NCR-405 changed nPrefSuit to m_nIntendedSuit
			nTargetSuit = DIAMONDS;
		else
			nTargetSuit = nNewSuit;
	}


	// check for violation
 	if ((nPartnersBid > BID_4D) || (nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
	{
		CString strTemp = BTS(nPartnersBid);
		status << "2JC30! Partner did not reply properly to our Jacoby Transfer, bidding " &
				   strTemp & " instead of the asked-for " & BTS(MAKEBID(nNewSuit,2)) &
				   ". Treat this as a simple rebid.\n";
		bidState.SetConventionStatus(this, CONV_ERROR);
		return FALSE;
	}
	else if (nPartnersBid == BID_PASS)
	{
		BOOL bInterference = ISBID(bidState.nLHOBid);
		CString strTemp = BTS(nPartnersBid);
		status << "2JC31! Partner passed our Jacoby Transfer " &
				  (bInterference? "after interference from the opponents" : "") &
				   ", so Jacoby is no longer in effect.\n";
//		bidState.SetConventionStatus(this, CONV_ERROR);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return FALSE;
	}

	// check response level
	double fPts = bidState.fPts;
	double fAdjPts = bidState.fPts + bidState.fDistPts - bidState.fPenPts;  // NCR-54 add in Dist, sub Pen
	// NCR See if RevalueHand() gives reasonable/usable values
	double testPts = hand.RevalueHand(REVALUE_DUMMY, nPartnersSuit, TRUE);   // NCR-234

	double fCardPts = bidState.fCardPts;
	int nNTLevel = 1;  // NCR allow for 2NT openning
	// NCR-22 also check if we bid 2S last time to xfer to Clubs
	//   NCR level would be 3 over 2NT                             NCR-366 Did pard open 1NT
	if(((nPartnersBidLevel == 3) && (nPreviousBid < BID_2S) && (bidState.nPartnersPrevBid != BID_1NT))
       // NCR-114 Check if pard's first bid was 1NT to determine his points 
	   || ((nPartnersBidLevel == 4) && (bidState.nPartnersPrevBid != BID_1NT))  // NCR-36 2NT|3S|4C->4D (w/o 4-suit xfer)
	   || ((nPartnersBidLevel == 3) && (bidState.nPartnersPrevBid == BID_2NT)) ) // NCR-215
		nNTLevel = 2;
	int n1NTMIN = pCurrConvSet->GetNTRangeMin(nNTLevel); // NCR use variable vs HC 1
	int n1NTMAX = pCurrConvSet->GetNTRangeMax(nNTLevel);

	//
	if ((nPartnersBid == BID_2H) || (nPartnersBid == BID_2S) ||
		(nPartnersBid == BID_3C) || (nPartnersBid == BID_3D)
		|| (ISBID(bidState.nLHOBid) && (nPartnersBid == BID_4C)) ) // NCR-114 Pard overcalled LHO
	{
		// trump support is questionable, so don't revalue the hand  <-NCR ??? What does this mean?
		if(testPts > fAdjPts)
			fAdjPts = testPts; // NCR-234 Change to Revalued dummy hand values 
//		ASSERT(testPts <= fAdjPts + 3);   // NCR-234  See how many times this happens
		bidState.m_fMinTPPoints = n1NTMIN + fAdjPts;
		bidState.m_fMaxTPPoints = n1NTMAX + fAdjPts;
		bidState.m_fMinTPCPoints = n1NTMIN + fCardPts;
		bidState.m_fMaxTPCPoints = n1NTMAX + fCardPts;
		status << "2JC32! Partner replied to our Jacoby transfer with " & BTS(nPartnersBid) &
				  " indicating " & n1NTMIN & "-" & n1NTMAX  & 
				  " points for a total in the partership of " & 
				  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & " points.\n";
	} 
	else if ( (pCurrConvSet->IsOptionEnabled(tb4SuitTransfers)) &&
				(((nPreviousBid == BID_2S) && (nPartnersBid == BID_2NT)) ||
				 ((nPreviousBid == BID_2NT) && (nPartnersBid == BID_3C)))) 
	{
		// look for a strength signal
		bidState.m_fMinTPPoints = n1NTMAX + fAdjPts;
		bidState.m_fMaxTPPoints = n1NTMAX + fAdjPts;
		bidState.m_fMinTPCPoints = n1NTMIN + fCardPts;
		bidState.m_fMaxTPCPoints = n1NTMAX + fCardPts;
		CString strTemp = BTS(nPreviousBid);
		status << "2JC34! Partner replied to our " & strTemp & 
				  " Jacoby transfer with a response of " & BTS(nPartnersBid) & 
				  ", which indicates a maximum 1NT opening hand (" & n1NTMAX  & 
				  " HCPs, for a total of " & 
				  bidState.m_fMinTPCPoints & "-" & bidState.m_fMaxTPCPoints &
				  " HCPs in the partnership) and strong support for the minor " &
				  STSS(nPreviousSuit) & " suit.\n";
	} 
	else 
	{
		// with solid trump support, revalue the hand
		fAdjPts = bidState.fAdjPts = hand.RevalueHand(REVALUE_DECLARER, nPartnersSuit, TRUE);
		bidState.m_fMinTPPoints = n1NTMAX + fAdjPts;
		bidState.m_fMaxTPPoints = n1NTMAX + fAdjPts;
		bidState.m_fMinTPCPoints = n1NTMIN + fCardPts;
		bidState.m_fMaxTPCPoints = n1NTMAX + fCardPts;
		status << "2JC16! Partner replied to our Jacoby transfer with a jump/overcall response of " &
				  BTS(nPartnersBid) & ", indicating max "&nNTLevel&"NT opening points (" &  // NCR-36
				  n1NTMAX & ") and 4+ trumps, for a total partnership holding of " &
				  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & 
				  " points and 4+ trumps.\n";
	}

	// note if we need to correct
	bool needToCorrect = false;  // NCR-678 Need to prevent 4NT without an agreed suit
	if (nTargetSuit != nNewSuit) {
		status << "2JC18! We instructed partner to bid Clubs last time because we're " &
				  "not playing 4-suit transfers, so we now want to correct to Diamonds.\n";
		needToCorrect = true;  // NCR-678 tell'em 
	}


	//
	// see if we're below game
	//
	double testing = PTS_GAME; // NCR-DEBUG to see the pts >> 25
	if (bidState.m_fMinTPPoints < PTS_GAME)
	{
		// are we correctiug?             // NCR-626 Check pard has not bid game
		if ((nTargetSuit != nNewSuit) && !bidState.IsGameBid(nPartnersBid))
		{
			// correct to 3D (or 4D)
			nBid = bidState.GetCheapestShiftBid(DIAMONDS, nPartnersBid);
			status << "JC20! With " & fCardPts & "/" & fPts & "/" & fAdjPts & 
					  " points in the hand, and a total in the partnership of " &
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & 
					  " points, settle for a part score and correct to " & BTS(nBid) & ".\n";
		}
		else
		{
			//
			//-------------------------------------------------------
			// not correcting
			//
			// with < 23 pts, pass or bail out at the suit
			//
			if (bidState.m_fMinTPPoints <= PTS_GAME-3) 
			{
				nBid = BID_PASS;
				status << "JC40! With " & fCardPts & "/" & fPts & "/" & fAdjPts & 
						  " points in the hand, and a total in the partnership of " &
						  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & 
						  " points, we have to settle for a part score and pass.\n";
			}

			//
			//-------------------------------------------------------
			//
			// with 23-25 pts, raise to the 3-level
			//
			else if ((bidState.m_fMinTPPoints>= PTS_GAME-2) && (bidState.m_fMinTPPoints < PTS_GAME))
			{
				if (nPartnersBidLevel < 3) 
				{
					// raise to 3
					nBid = MAKEBID(nNewSuit,3);
					status << "JC50! With " & fCardPts & "/" & fPts & "/" & fAdjPts & 
							  " points in the hand, and a total in the partnership of " &
							  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & 
							  " points, raise to " & BTS(nBid) & ".\n";
				}
				else
				{
					// else pass
					nBid = BID_PASS;
					status << "JC52! We have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
							  " points in the hand, and a total in the partnership of " &
							  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & 
							  " points, so we cannot realistically raise further -- so pass.\n";
				}
			}
		}
	}

	//
	//--------------------------------------------------------------------------
	//
	// with 26-31 total points, bid game regardless of whether we're correcting
	//
	// need 26 pts for a major game
	//
	else if (((bidState.m_fMinTPPoints >= PTS_GAME) && (bidState.m_fMinTPPoints < PTS_SLAM)) 
			  || needToCorrect)   // NCR-678 Last chance to bid before 4NT
	{
		// NCR-232 Test if pard has bid at game level
		if(bidState.IsGameBid(nPartnersBid)){
			nBid = BID_PASS;  // NCR-232 pass if at game
		}
		// bid a major game
		else if (ISMAJOR(nTargetSuit))
		{
			nBid = MAKEBID(nTargetSuit,4);
		} 
		else 
		{
			if ((bidState.m_fMinTPPoints < PTS_MINOR_GAME) || (bidState.numAces == 0))  // NCR-765 Need Aces to bid 5
			{
				//  NCR - Should this be a min bid vs 4???
				nBid = bidState.GetCheapestShiftBid(nTargetSuit, nPartnersBid);  // NCR-737
				nBid = MAKEBID(nTargetSuit, 4); // NCR this precludes Opener going to 3NT ???
			}
			else
				nBid = MAKEBID(nTargetSuit, 5);
		}
		status << "JC60! With " & fCardPts & "/" & fPts & "/" & fAdjPts & 
				  " points in the hand, and a total in the partnership of " &
				  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
				  " points, bid " & BTS(nBid) & ".\n";
	}

	//
	//-------------------------------------------------------
	//
	// with 32+ pts, bid Blackwood
	//
	else
	{
		// bid Blackwood or somethig close
		// NCR-412 Root recommends NOT Blackwood after Jacoby Xfer
//NCR		bidState.InvokeBlackwood(nTargetSuit);
		// Use 4NT invitation to slam instead
		nBid = BID_4NT;  // NCR-412 Invite slam ???
		bidState.m_nIntendedSuit = nTargetSuit;  // NCR-636 set suit
		status << "JC70! With " & fCardPts & "/" & fPts & "/" & fAdjPts & 
				  " points in the hand, and a total in the partnership of " &
				  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
				  " points, invite slam by bidding " & BTS(nBid) & ".\n";

//		bidState.SetConventionStatus(this, CONV_FINISHED);
//		return TRUE;
	}

	// done
	bidState.SetBid(nBid);
	bidState.SetConventionStatus(this, CONV_FINISHED);
	return TRUE;
}






//
//==================================================================
// construction & destruction
//
CJacobyTransferConvention::CJacobyTransferConvention() 
{
	// from ConvCodes.h
	m_nID = tidJacobyTransfers;
}

CJacobyTransferConvention::~CJacobyTransferConvention() 
{
}



