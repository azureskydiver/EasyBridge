//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// StaymanConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "PlayerStatusDialog.h"
#include "StaymanConvention.h"
#include "ConventionSet.h"




//
//---------------------------------------------------------------
//
// TryConvention()
//
//
BOOL CStaymanConvention::TryConvention(const CPlayer& player, 
									   const CConventionSet& conventions, 
									   CHandHoldings& hand, 
									   CCardLocation& cardLocation, 
								       CGuessedHandHoldings** ppGuessedHands,
									   CBidEngine& bidState,  
								       CPlayerStatusDialog& status)
{
	// see if we can use Stayman here
	// requirements:
	// 1: partner must have opened bidding with 1NT, 2NT, or 3NT
	// 2: RHO must have passed
	// 3: need 23+ HCPs (or 22+ pts with 4/4/4/1 distribution)
	// 4: need at least one 4+ card major
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int numPartnerBidsMade = bidState.m_numPartnerBidsMade;
	//
	if ( ((nPartnersBid == BID_1NT) || (nPartnersBid == BID_2NT) || (nPartnersBid == BID_3NT)) &&
				(bidState.m_bPartnerOpenedForTeam) && 
				(numPartnerBidsMade == 1) &&
				(bidState.bRHOPassed) &&
				(bidState.fCardPts >= 6) &&
				((bidState.numCardsInSuit[HEARTS] >= 4) || (bidState.numCardsInSuit[SPADES] >= 4)) )
	{
		// see if we have enough points (23 total, or 8 pts w/ 15-17 NT)
		// that is, unless we have a 4/4/4/1 distbn (with 1 club card)
		int fPtsRequired = 23 - pCurrConvSet->GetNTRangeMin(nPartnersBidLevel);
		if (bidState.fCardPts >= fPtsRequired)
		{
			// met rqmts
		}
		else if ((hand.GetNumSuitsOfAtLeast(4) == 3) && 
				 (hand.GetNumCardsInSuit(CLUBS) == 1) &&
				 (bidState.fCardPts >= (fPtsRequired-1)))
		{
			// barely met rqmts
			status << "STYX! Although we only have " & bidState.fCardPts & 
					  " HCPs, we can use Stayman since we have 4/4/4/1 distribution and can live with a 2D/2H/2S response from partner.\n";
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		// haven't met the rqmts
		return FALSE;
	}

	//
	// avoid using Stayman under certain circumstances
	// (i.e., holding a really good minor)
	//
	if ((bidState.nSuitStrength[CLUBS] >= SS_ABSOLUTE) ||
			(bidState.nSuitStrength[DIAMONDS] >= SS_ABSOLUTE))
		return FALSE;

	// also, if we have game values in NoTrump with a balanced hand, 
	// don't even bother with Stayman, as we will bid a NT contract directly
	if ((bidState.bBalanced) && 
		((bidState.fCardPts + pCurrConvSet->GetNTRangeMin(1)) >= 26))
		return FALSE;

	//
	// set partner's point count expectations
	//
	switch(nPartnersBid)
	{
		case BID_1NT:
			bidState.m_fPartnersMin = pCurrConvSet->GetValue(tn1NTRangeMinPts);
			bidState.m_fPartnersMax = pCurrConvSet->GetValue(tn1NTRangeMaxPts);
			break;
		case BID_2NT:
			bidState.m_fPartnersMin = pCurrConvSet->GetValue(tn2NTRangeMinPts);
			bidState.m_fPartnersMax = pCurrConvSet->GetValue(tn2NTRangeMaxPts);
			break;
		case BID_3NT:
			bidState.m_fPartnersMin = pCurrConvSet->GetValue(tn3NTRangeMinPts);
			bidState.m_fPartnersMax = pCurrConvSet->GetValue(tn3NTRangeMaxPts);
			break;
	}
	bidState.m_fMinTPPoints = bidState.m_fPartnersMin + bidState.fPts;
	bidState.m_fMaxTPPoints = bidState.m_fPartnersMax + bidState.fPts;
	bidState.m_fMinTPCPoints = bidState.m_fPartnersMin + bidState.fCardPts;
	bidState.m_fMaxTPCPoints = bidState.m_fPartnersMax + bidState.fCardPts;

	//		
	// we have at least one 4-card major
	//
	int nBid = bidState.GetCheapestShiftBid(CLUBS);
	status << "STY0! Have " & bidState.fCardPts & "/" & bidState.fPts & 
			  " points and at least one 4+ card major suit, so bid " & BTS(nBid) &
			  " (Stayman).\n";
	//
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
BOOL CStaymanConvention::RespondToConvention(const CPlayer& player, 
											 const CConventionSet& conventions, 
											 CHandHoldings& hand, 
											 CCardLocation& cardLocation, 
											 CGuessedHandHoldings** ppGuessedHands,
											 CBidEngine& bidState,  
											 CPlayerStatusDialog& status)
{
	// first see if another convention is active
	if (bidState.GetActiveConvention() &&
		bidState.GetActiveConvention() != this)
		return FALSE;

	// see if we already responded once to Stayman
	if (bidState.GetConventionStatus(this) == CONV_RESPONDED)
	{
		// check here for an invitational response by partner to our Stayman response
		// that is, partner bids a major suit after our negative 2D/3D response
		if ((bidState.nPreviousSuit == DIAMONDS) && ISMAJOR(bidState.nPartnersSuit))
		{
			// partner is indicating a 5 card major, so support with 3 cards
			int nBid, nSuit = bidState.nPartnersSuit;
			status << "STYRR0! Partner bid " & bidState.szPB & " after our " & bidState.szPVB & 
					  " response to his Stayman.  This is an invitational bid indicating 5+ " &
					  STS(nSuit) & ".\n";

			// look at our trump support & pt count
			if ((bidState.numSupportCards >= 4) || (bidState.fCardPts >= 17))
			{
				// raise to game with 4+ trumps or 17 pts
				nBid = bidState.GetGameBid(nSuit);
				if (nBid > bidState.nPartnersBid)
				{
					status << "STYRR2! And with " & bidState.numCardsInSuit[nSuit] &  " " &
							   STS(nSuit) & " and " & bidState.fCardPts & 
							   " HCPs, raise partner to " & BTS(nBid) & ".\n";
				}
				else
				{
					// partner already bid game
					nBid = BID_PASS;
					status << "STYRR3! But since partner has already bid game, just pass.\n";
				}
			}
			else if ((bidState.numSupportCards >= 3) || (bidState.fCardPts >= 16))
			{
				// raise to the 3-level with 3 trumps or 16 pts
				nBid = MAKEBID(nSuit, 3);
				if (nBid > bidState.nPartnersBid)
				{
					// raise!
					if (bidState.numSupportCards >= 3)
						status << "STYRR10! And with " & bidState.numCardsInSuit[nSuit] & " " &
								   STS(nSuit) & " and " & bidState.fCardPts & 
								   " HCPs, raise partner to " & BTS(nBid) & ".\n";
					else
						status << "STYRR11! And with " & bidState.fCardPts & 
								  " HCPs, raise partner to " & BTS(nBid) & ".\n";
				}
				else
				{
					// can't go any higher
					nBid = BID_PASS;
					status << "STYRR16! But with " & bidState.numCardsInSuit[nSuit] &  " " &
							   STS(nSuit) & " and " & bidState.fCardPts & 
							   " HCPs, we cannot raise partner any further, so pass.\n";
				}
			}
			else
			{
				// pass with only 2 trumps and 15 pts
				nBid = BID_PASS;
				status << "STYRR20! But with only " & bidState.numCardsInSuit[nSuit] &  " " &
						   STS(nSuit) & " and " & bidState.fCardPts & 
						   " HCPs, we have to pass.\n";
			}
			// done
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return TRUE;
		}
		else
		{
			// nothing more to do here
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return FALSE;
		}
	}

	//
	// check conditions -- prevous bid should have been 
	// 1NT, 2NT, or 3NT, and response whould have been the
	// cheapest club bid, and there should have been no interference
	// between the NT bid and club response (i.e., LHO should have passed)
	//
	int nPartnersBid = bidState.nPartnersBid;
	int nPreviousBid = bidState.nPreviousBid;
	int numPartnerBidsMade = bidState.m_numPartnerBidsMade;

	// see if we have interference
	if ( ((((nPreviousBid == BID_1NT) && (nPartnersBid == BID_2C)) ||
		   ((nPreviousBid == BID_2NT) && (nPartnersBid == BID_3C)) ||
		   ((nPreviousBid == BID_3NT) && (nPartnersBid == BID_4C)))) &&
		   (numPartnerBidsMade == 1) && (!bidState.bLHOPassed) ) 
	{
		// Stayman inactive
		status << "STYX! LHO interfered over our NT opening, so Stayman is off.\n";
		return FALSE;
	}

	//
	if ( ((((nPreviousBid == BID_1NT) && (nPartnersBid == BID_2C)) ||
		   ((nPreviousBid == BID_2NT) && (nPartnersBid == BID_3C)) ||
		   ((nPreviousBid == BID_3NT) && (nPartnersBid == BID_4C)))) &&
		   (numPartnerBidsMade == 1) && (bidState.bLHOPassed) ) 
	{
		// Stayman response found
		status << "STYR! Partner is using the Stayman convention, asking for majors.\n";
	} 
	else 
	{
		return FALSE;
	}

	// estimate partner's strength
	bidState.m_fPartnersMin = 8;
	bidState.m_fPartnersMax = MIN(22, 40 - bidState.fCardPts);
	bidState.m_fMinTPPoints = bidState.fPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPPoints = bidState.fPts + bidState.m_fPartnersMax;
	bidState.m_fMinTPCPoints = bidState.fCardPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPCPoints = bidState.fCardPts + bidState.m_fPartnersMax;

	//
	// look for a 4+ card major suit
	//
	int nSuit, nBid;
	if ((bidState.numCardsInSuit[HEARTS] >= 4) && (bidState.numCardsInSuit[SPADES] >= 4)) 
	{
		// if holding two 4-card majors, bid hearts
		nSuit = HEARTS;
		nBid = bidState.GetCheapestShiftBid(nSuit);
		status << "STYR10! With a "& bidState.numCardsInSuit[nSuit] & "-card " & STSS(nSuit) & 
				  " suit, reply to partner's Stayman inquiry with a " & 
				  BTS(nBid) & " bid.\n";
	} 
	else if ((bidState.numCardsInSuit[HEARTS] >= 4) || (bidState.numCardsInSuit[SPADES] >= 4)) 
	{
		// have only one 4-card major
		if (bidState.numCardsInSuit[HEARTS] >= 4)
			nSuit = HEARTS;
		else
			nSuit = SPADES;
		nBid = bidState.GetCheapestShiftBid(nSuit);
		status << "STYR11! With a "& bidState.numCardsInSuit[nSuit] & "-card " & STSS(nSuit) & 
				  " suit, reply to partner's Stayman inquiry with a " & 
				  BTS(nBid) & " bid.\n";
	} 
	else 
	{
	 	// no go
		nBid = bidState.GetCheapestShiftBid(DIAMONDS);
		status << "STYR12! But we have no 4-card majors, so reply negatively to Stayman by bidding " &
				  BTS(nBid) & ".\n";
	}

	// done
	bidState.SetBid(nBid);
	bidState.SetConventionStatus(this, CONV_RESPONDED);
//	bidState.SetConventionStatus(this, CONV_FINISHED);
	return TRUE;
}





//
//---------------------------------------------------------------
//
// HandleConventionResponse()
//
//	 
BOOL CStaymanConvention::HandleConventionResponse(const CPlayer& player, 
												  const CConventionSet& conventions, 
												  CHandHoldings& hand, 
												  CCardLocation& cardLocation, 
												  CGuessedHandHoldings** ppGuessedHands,
												  CBidEngine& bidState,  
												  CPlayerStatusDialog& status)
{
	//
	if (bidState.GetConventionStatus(this) != CONV_INVOKED)
		return FALSE;
	//
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int nPartnersSuit = bidState.nPartnersSuit;
	int nSupportLevel;
	int nSuit, nBid;

	//
	// then check for an illegal response
	//
 	if ((nPartnersBidLevel > BID_4S) || (nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
	{
		CString strTemp = BTS(nPartnersBid);
		status << "STYX01! Partner did not reply properly to our Stayman inquiry, bidding " &
				   strTemp & ". Treat this as a simple rebid.\n";
		bidState.SetConventionStatus(this, CONV_ERROR);
		return FALSE;
	}
	else if (nPartnersBid == BID_PASS)
	{
		BOOL bInterference = ISBID(bidState.nLHOBid);
		CString strTemp = BTS(nPartnersBid);
		status << "STYX04! Partner passed our Stayman inquiry " &
				  (bInterference? "after interference from the opponents" : "") &
				   ", so Stayman is no longer in effect.\n";
//		bidState.SetConventionStatus(this, CONV_ERROR);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return FALSE;
	}

	//
	// Did partner respond with a major?
	//
	if ((nPartnersSuit == CLUBS) || (nPartnersSuit == NOTRUMP) ||
												(nPartnersBid > BID_4S))  
	{
		// error
		status << "STYX10! Partner did not reply properly to our Stayman query, bidding " & 
				  BTS(nPartnersBid) & " instead of 2D/2H/2S.  Treat this like a simple rebid.\n";
		bidState.SetConventionStatus(this, CONV_ERROR);
		return FALSE;
	}

	//
	if (nPartnersSuit == DIAMONDS) 
	{
		// diamonds = negative response
		status << "STYX12! Partner replied " & BTS(nPartnersBid) & 
				  " to our Stayman query, indicating no 4-card majors.\n";
		nSuit = NONE;
	} 
	else 
	{
		// else it's a major suit response
		nSuit = nPartnersSuit;
		status << "STYX14! Partner replied " & BTS(nPartnersBid) & 
				" to our Stayman query, indicating a " &
				((nPartnersBidLevel == 3)? "strong" : "") &
				"4+ card " & STSS(nSuit)& " suit.\n";
	} 
		
	//
	// see if there's a match
	// 

	//
	if ((ISMAJOR(nSuit)) && (bidState.numCardsInSuit[nSuit] >= 4)) 
	{
		// found a suit fit; revalue hand as dummy
		bidState.fAdjPts = hand.RevalueHand(REVALUE_DUMMY, nSuit, TRUE);
		bidState.m_nAgreedSuit = nSuit;
		nSupportLevel = bidState.nSuitStrength[nSuit];
	} 
	else 
	{
		// no match; no adjustment to points
		nSuit = NONE;
	}
	int n1NTMIN = pCurrConvSet->GetNTRangeMin(1);
	int n1NTMAX = pCurrConvSet->GetNTRangeMax(1);
	double fAdjPts = bidState.fAdjPts;
	double fCardPts = bidState.fCardPts;
	bidState.m_fMinTPPoints = n1NTMIN + fAdjPts;
	bidState.m_fMaxTPPoints = n1NTMAX + fAdjPts;
	bidState.m_fMinTPCPoints = n1NTMIN + fCardPts;
	bidState.m_fMaxTPCPoints = n1NTMAX + fCardPts;


	//
	//----------------------------------------------------------------
	//
	// no suit was found -- invite to a major, or else bid NT
	//
	if (nSuit == NONE) 
	{
		// see if we have a good 5+ card major -- then bid it invitationally
		// but realistically we need 24+ total pts here even at the 2 level, 
		// since partner has a balanced (flat) distribuion
		if (!bidState.bBalanced && (bidState.m_fMinTPPoints >= PTS_GAME-1) &&
			( ((bidState.numCardsInSuit[HEARTS] >= 5) && (bidState.nSuitStrength[HEARTS] >= SS_OPENABLE)) || 
			  ((bidState.numCardsInSuit[SPADES] >= 5) && (bidState.nSuitStrength[SPADES] >= SS_OPENABLE)) ) )
		{
			int nSuit = bidState.GetBestSuitofAtLeast(HEARTS, SPADES, 5);
			// bid 2H/2S or 3H/3S
			nBid = MAKEBID(nSuit, nPartnersBidLevel);
			status << "STYX21! Partner has no 4-card major suit, but with a " & 
					  bidState.numCardsInSuit[nSuit] & "-card " & STSS(nSuit) &
					  " suit and an unbalanced hand, invite towards a contract in " & STS(nSuit) &
					  " with a bid of " & BTS(nBid) & ".\n";
		}

		// else bid NT
		else if (bidState.m_fMinTPCPoints < PTS_GAME-3) 
		{
			// really shouldn't even be here!
			nBid = BID_PASS;
			status << "STYX30! With no major suit fit and only " & 
					  bidState.m_fMinTPCPoints & "-" & bidState.m_fMaxTPCPoints &
					  " HCPs in the partnership, we have to pass.\n";
		} 
		else if (bidState.m_fMinTPCPoints < PTS_GAME) 
		{
			// cheapest NT may be 2NT or 3NT, depending on opening
			nBid = bidState.GetCheapestShiftBid(NOTRUMP);
			status << "STYX31! With no major suit fit and " & 
					  bidState.m_fMinTPCPoints & "-" & bidState.m_fMaxTPCPoints &
					  " HCPs in the partnership, bid " & BTS(nBid) & ".\n";
		} 
		else if (bidState.m_fMinTPCPoints < PTS_SLAM-3) 
		{
			// 26-30 pts:  bid game
			nBid = BID_3NT;
			status << "STYX32! With no major suit fit but with " & 
					  bidState.m_fMinTPCPoints & "-" & bidState.m_fMaxTPCPoints &
					  " HCPs in the partnership, bid game at 3NT.\n";
		} 
		else if (bidState.m_fMinTPCPoints <= PTS_SLAM) 
		{
			// 31-32 pts:  raise to 4NT (invitational towards slam)
			nBid = BID_4NT;
			status << "STYX33! With no major suit fit but with " & 
					  bidState.m_fMinTPCPoints & "-" & bidState.m_fMaxTPCPoints &
					  " HCPs in the partnership, invite towards slam with a bid of 4NT.\n";
		} 
		else if (bidState.m_fMinTPCPoints <= PTS_SLAM+1) 
		{
			// 33-34 pts:  bid 6NT directly
			nBid = BID_6NT;
			status << "STYX34! With no major suit fit but with " & 
					  bidState.m_fMinTPCPoints & "-" & bidState.m_fMaxTPCPoints &
					  " HCPs in the partnership, bid a small slam directly at 6NT.\n";
		} 
		else if (bidState.m_fMinTPPoints == PTS_GRAND_SLAM-1) 
		{
			// 35-36 pts -- bid 5NT, inviting towards grand slam
			nBid = BID_5NT;
			status << "STYX35! With no major suit fit but with " & 
					  bidState.m_fMinTPCPoints & "-" & bidState.m_fMaxTPCPoints &
					  " HCPs in the partnership, invite a grand slam with a bid of 5NT.\n";
		} 
		else 
		{
			// 37+ pts -- bid grand slam directly
			nBid = BID_7NT;
			status << "STYX36! With no major suit fit but with " & 
					  bidState.m_fMinTPCPoints & "-" & bidState.m_fMaxTPCPoints &
					  " HCPs in the partnership, bid a grand slam directly at 7NT.\n";
		}
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;
	}


	//
	//----------------------------------------------------------
	//
	// else we're here because we've found a major suit fit
	//
	int nCardFit = bidState.numCardsInSuit[nSuit] + 4;

	// clear the Stayman convention status
	bidState.SetConventionStatus(this, CONV_FINISHED);

	// pass with <= 22 pts
	if (bidState.m_fMinTPPoints < PTS_GAME-3) 
	{
		//
		nBid = BID_PASS;
		status << "STY60! With only " & 
				  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
				  " points in the partnership, settle for a part score and pass.\n";
	} 
	else if (bidState.m_fMinTPPoints < PTS_GAME) 
	{
		// raise to 3 with 23-25 pts
		if (nPartnersBidLevel < 3)
		{
			nBid = MAKEBID(nSuit,3);
			status << "STY62! With " & bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " points in the partnership and at least " &
					  ((nCardFit == 8)? "an " : "a ") & nCardFit &
					  "-card fit, raise to " & BTS(nBid) & ", and partner might raise to game.\n";
		}
		else
		{
			nBid = BID_PASS;
			status << "STY63! Although partner likes the " & bidState.szPSS &
					  " suit, we have only " &
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " points in the partnership, so we have to pass.\n";
		}
	} 
	else if (bidState.m_fMinTPPoints < PTS_SLAM)
	{
		// raise to game
		nBid = MAKEBID(nSuit,4);
		status << "STY64! With " & bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
				  " points in the partnership and at least " &
				  ((nCardFit == 8)? "an " : "a ") & nCardFit &
				  "-card fit, raise to game at " & BTS(nBid) & ".\n";
	} 
	else 
	{
		// 32+ pts -- bid Blackwood
		bidState.InvokeBlackwood(bidState.m_nAgreedSuit);
	}
	//
	bidState.SetBid(nBid);
	return TRUE;

}










//
//==================================================================
// construction & destruction
//
CStaymanConvention::CStaymanConvention() 
{
	// from ConvCodes.h
	m_nID = tidStayman;
}

CStaymanConvention::~CStaymanConvention() 
{
}



