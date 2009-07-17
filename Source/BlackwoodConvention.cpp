//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// BlackwoodConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "PlayerStatusDialog.h"
#include "PlayerStatusDialog.h"
#include "BlackwoodConvention.h"
#include "ConventionSet.h"



//
//==================================================================
// 
// ApplyTest()
//

//
// Blackwood can't be started automatically from within this test routine;
// rather, it has to be triggered by the general bidding code under
// certain spoecific circumstances.  
// But responding to Blackwood can take place here.
//
BOOL CBlackwoodConvention::ApplyTest(const CPlayer& player, 
									 const CConventionSet& conventions, 
									 CHandHoldings& hand, 
									 CCardLocation& cardLocation, 
								     CGuessedHandHoldings** ppGuessedHands,
									 CBidEngine& bidState, 
								     CPlayerStatusDialog& status)
{
	// basic test
//	if (!pCurrConvSet->IsConventionEnabled(tidBlackwood))
//		return FALSE;

	// see if another convention is active
//	if (CheckForOtherConventions(bidState))
//		return FALSE;

	//
	if (RespondToConvention(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status))
		return TRUE;
	if (HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status))
		return TRUE;
	// 
	return FALSE;
}






//
//---------------------------------------------------------------
//
// InvokeBlackwood()
//
// Start the Blackwood convention
//	 
BOOL CBlackwoodConvention::InvokeBlackwood(CHandHoldings& hand, CBidEngine& bidState, CPlayerStatusDialog& status, int nEventualSuit)
{
	// record intended suit for later use
	ASSERT(nEventualSuit != NONE);
	bidState.m_nAgreedSuit = nEventualSuit;
	double fMinTPPoints = bidState.m_fMinTPPoints;
	double fMaxTPPoints = bidState.m_fMaxTPPoints;
	double fMinTPCPoints = bidState.m_fMinTPCPoints;
	double fMaxTPCPoints = bidState.m_fMaxTPCPoints;
	double fCardPts = bidState.fCardPts;
	double fPts = bidState.fPts;
	double fAdjPts = bidState.fAdjPts;
	int nBid;


	// if Blackwood has already been used, return
	if (bidState.GetConventionStatus(this) >= CONV_INVOKED) 
		return FALSE;

	//
	// if Blackwood is not enabled, just bid slam directly
	//
	if (!pCurrConvSet->IsConventionEnabled(tidBlackwood))
	{
		// no Blackwood? go ahead and bid slam directly
		status << "BKWDX! we have " & fCardPts & "/" & fPts & "/" & fAdjPts &
				  " points in hand, for a total of approx. " &
				  fMinTPPoints & "-" & fMaxTPPoints & " / " &
				  fMinTPCPoints & "-" & fMaxTPCPoints & 
				  " pts in the partnership, but we can't use Blackwood to investigate Aces and Kings since it's currently not enabled.\n";
		// bid a grand slam if we have 37+ pts and the trump ace
		// or a small slam with 33+ points
		// or make the cheapest shift bid otherwise (D'oh!)
		if ((fMinTPPoints >= PTS_GRAND_SLAM+1) && (hand.SuitHasCard(nEventualSuit, ACE)))
		{
			nBid = MAKEBID(nEventualSuit, 7);
			status << "BKWDX1! but since we have the points for a grand slam and we hold the trump ace, go ahead and bid " & 
					  BTS(nBid) & ".\n";
		}
		else if (fMinTPPoints >= PTS_SLAM)
		{
			nBid = MAKEBID(nEventualSuit, 6);
			status << "BKWDX2! but since we have the points for a small slam, go ahead and bid " & 
					  BTS(nBid) & ".\n";
		}
		else
		{
			// get cheapest shift bid
			nBid = bidState.GetCheapestShiftBid(nEventualSuit);
			status << "BKWDX3! so we just bid " & BTS(nBid) & ".\n";
		}
		//
		bidState.SetBid(nBid);
		return TRUE;
	}

	
	//
	// shouldn't use Blackwood if we have all four aces
	//
	if (bidState.numAces == 4)
	{
		// no Blackwood? go ahead and bid slam directly
		status << "BKWDY! Unfortunately, since we hold all four aces, Blackwood is of little value to us.\n";
		// bid a grand slam if we have 37+ pts and the trump ace
		// or a small slam with 33+ points
		// or make the cheapest shift bid otherwise (D'oh!)
		if ((fMinTPPoints >= PTS_SLAM+1)   //NCR added NOTRUMP test 
			&& ((nEventualSuit != NOTRUMP) && hand.SuitHasCard(nEventualSuit, ACE)))
		{
			nBid = MAKEBID(nEventualSuit, 7);
			status << "BKWDY1! so since we have the points for a grand slam, go ahead and bid " & 
					  BTS(nBid) & ".\n";
		}
		else if (fMinTPPoints >= PTS_SLAM)
		{
			nBid = MAKEBID(nEventualSuit, 6);
			status << "BKWDY2! so since we have the points for a small slam, go ahead and bid " & 
					  BTS(nBid) & ".\n";
		}
		else
		{
			nBid = bidState.GetCheapestShiftBid(nEventualSuit, pDOC->GetLastValidBid());
			status << "BKWDY3! so without quite enough points for a slam, we just bid " & BTS(nBid) & ".\n";
		}
		//
		bidState.SetBid(nBid);
		return TRUE;
	}


	//
	// else we're playing Blackwood and do not hold all four aces
	// so make the bid to ask for aces
	//
	if (bidState.nPartnersBid < BID_4NT)
	{
		nBid = BID_4NT;
		status << "BLKWD! with " & bidState.fCardPts & "/" & bidState.fPts & "/" & bidState.fAdjPts & 
				  " points in the hand, for a total of approx. " & 
				  fMinTPCPoints & "-" & fMaxTPCPoints & " / " &
				  fMinTPPoints & "-" & fMaxTPPoints & 
				  " pts in the partnership, explore slam possibilities with Blackwood at " &
				  BTS(nBid) & ". The eventual contract in " & STSS(nEventualSuit) &".\n";
	}
	else
	{
		nBid = BID_PASS;
		status << "BLKWDx! we'd like to use Blackwood, but partner has already bid higher than 4NT, so we have to pass.\n";
	}
	bidState.SetBid(nBid);
	bidState.SetConventionStatus(this, CONV_INVOKED_ROUND1);

	// done	
	return TRUE;
}






//
//---------------------------------------------------------------
//
// RespondToConvention()
//
// Check to see if a Blackwood query has been made, 
// and if so, respond as appropriate
//
//	 
BOOL CBlackwoodConvention::RespondToConvention(const CPlayer& player, 
											   const CConventionSet& conventions, 
											   CHandHoldings& hand, 
											   CCardLocation& cardLocation, 
											   CGuessedHandHoldings** ppGuessedHands,
											   CBidEngine& bidState,  
											   CPlayerStatusDialog& status)
{
	// first see if another convention is active
//	if ((bidState.GetActiveConvention() != NULL) &&
//					(bidState.GetActiveConvention() != this))
//		return FALSE;

	//
	// first look for a Blackwood _Query_ from partner -- 
	// any 4NT not in response to an NT bid is Blackwood
	//
	int nBlackwoodStatus = bidState.GetConventionStatus(this);
	int numAces = bidState.numAces;
	int numKings = bidState.numKings;
	int nBid;

	// is partner asking for aces?
	if ((bidState.nPartnersBid == BID_4NT) 
//					(ISBID(bidState.nPartnersPrevBid)) && 
        // NCR-246 Not Blackwood if we opened NT ??? 
		// NCR-246a Not Blackwood if 4NT immediately after Stayman or Jacoby transfer
		// NCR-246a Eg 1NT - 2C - 2D - 4NT => Invitation to Slam ???
		// NCR-314 Its not Blackwood if pard opened NT unless we have an agreed suit
	    && ((bidState.nFirstRoundSuit != NOTRUMP) || (bidState.m_nAgreedSuit != NONE)) 	
        // NCR-309 NT ok if conventional and we have an agreed suit		
		&& ((bidState.nPreviousSuit != NOTRUMP) || (bidState.m_nAgreedSuit != NONE)) 
		// NCR-324 Not Blackwood if we bid NT before
		&& ((bidState.nNextPrevSuit != NOTRUMP) 
		// NCR-518 Unless it was Jacoby 2NT
		    || (bidState.nNextPrevBid == BID_2NT) && (bidState.m_nAgreedSuit != NONE))
		// NCR-368 What if pard wants for force NT?     // NCR-379 Jacoby 2NT would set agreedSuit
		&& ((bidState.nPartnersPrevSuit != NOTRUMP) || (bidState.m_nAgreedSuit != NONE)) )	
	{
		// respond to Blackwood request for Aces
		if ((numAces == 0) || (numAces == 4))
			nBid = BID_5C;
		else if (numAces == 1)
			nBid = BID_5D;
		else if (numAces == 2)
			nBid = BID_5H;
		else if (numAces == 3)
			nBid = BID_5S;
		//
		status << "BK10! With " & numAces & " Ace" & ((numAces > 1)? "s," : ",") &
				  " respond to partner's Blackwood inquiry with " & BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND1);	// answered Aces, ready for Kings
		return TRUE;
	}

	// is partner asking for kings?
	if ((nBlackwoodStatus == CONV_RESPONDED_ROUND1) && 
					(bidState.nPartnersBid == BID_5NT))	
	{
		// respond
		if ((numKings == 0) || (numKings == 4))
			nBid = BID_6C;
		else if (numKings == 1)
			nBid = BID_6D;
		else if (numKings == 2)
			nBid = BID_6H;
		else if (numKings == 3)
			nBid = BID_6S;
		//
		status << "BK12! With " & numKings & " King" & ((numKings > 1)? "s," : ",") &
				  " respond to partner's Blackwood inquiry with " & BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);	// Blackwood complete
		return TRUE;
	}

	// NCR what if partner bails out in a new suit??? 
	if(nBlackwoodStatus == CONV_RESPONDED_ROUND1) 
	{
		status << "BK13! Partner appears to have bailed out, so we'll pass.\n";
		bidState.SetBid(BID_PASS);
		bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);	// Blackwood complete
		return TRUE;
	}

	// no Blackwood
	return FALSE;
}






//
//---------------------------------------------------------------
//
// HandleConventionResponse()
//
// Check on Blackwood response from the invoker, and act accordingly
//
//	 
BOOL CBlackwoodConvention::HandleConventionResponse(const CPlayer& player, 
											        const CConventionSet& conventions, 
											        CHandHoldings& hand, 
											        CCardLocation& cardLocation, 
												    CGuessedHandHoldings** ppGuessedHands,
											        CBidEngine& bidState,  
											        CPlayerStatusDialog& status)
{
	int nBlackwoodStatus = bidState.GetConventionStatus(this);
	int numAces = bidState.numAces;
	int numKings = bidState.numKings;
	double fMinTPPoints = bidState.m_fMinTPPoints;
	double fMaxTPPoints = bidState.m_fMaxTPPoints;
	double fMinTPCPoints = bidState.m_fMinTPCPoints;
	double fMaxTPCPoints = bidState.m_fMaxTPCPoints;
	int nAgreedSuit = bidState.m_nAgreedSuit;
	int nPartnersBid = bidState.nPartnersBid;
	int nBid;

	//
	// Did we ask partner for aces in our last bid?
	//
	if (nBlackwoodStatus == CONV_INVOKED_ROUND1) 
	{
		// first check for a strange response
		if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
		{
			// we don't understand partner's bid
			return CConvention::HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status);
		}

		//
		switch(nPartnersBid) 
		{
			//
			case BID_5C:	// 0 or 4 Aces
				if (numAces == 0)
					bidState.m_numPartnersAces = 4;
				else
					bidState.m_numPartnersAces = 0;
				break;

			case BID_5D:	// 1 Ace
				bidState.m_numPartnersAces = 1;
				break;
			
			case BID_5H:	// 2 Aces
				bidState.m_numPartnersAces = 2;
				break;

			case BID_5S:	// 3 Aces
				bidState.m_numPartnersAces = 3;
				break;

			default:		// choke, gag, spew!
				bidState.m_numPartnersAces = 0;
				break;
		}
		if (nPartnersBid <= BID_5S) 
		{
			status << "BK20! Partner responded to our 4NT Blackwood inquiry with " & 
					  BTS(nPartnersBid) & ", indicating " & 
					  bidState.m_numPartnersAces & 
					  (((bidState.m_numPartnersAces == 0) || (bidState.m_numPartnersAces > 1))? " Aces" : " Ace") & ".\n";
		} 
		else 
		{
			status << "BK22! Partner made an illegal " & BTS(bidState.nPartnersBid) & 
					  " response to 4NT Blackwood; assuming zero Aces in partner's hand.\n";
			bidState.SetConventionStatus(this, CONV_ERROR);	// Blackwood complete
			return TRUE;
		}

		//
		// at this point, decide whether to pass, proceed to asking for kings,
		// or jump to a contract.
		//
		int numTotalAces = bidState.numAces + bidState.m_numPartnersAces;

		//
		if (numTotalAces == 4)
		{
			// if we're close to a grand slam, go for it
			if (fMinTPPoints >= PTS_GRAND_SLAM-1)
			{
				nBid = BID_5NT;
				status << "BK32! Our team holds all four aces, so proceed to 5NT to ask for kings.\n";
				nBlackwoodStatus = CONV_INVOKED_ROUND2;	// asking for kings
			}
			else
			{
				nBid = MAKEBID(nAgreedSuit, 6);
				status << "BK32! Our team holds all four aces, but with an estimated " & 
						  fMinTPPoints & "-" & fMaxTPPoints &
					      " total partnership points, we lack the points for a grand slam, so settle for a contract of " & BTS(nBid) & ".\n";
				nBlackwoodStatus = CONV_FINISHED;	// Blackwood finished
			}
		}
		else
		{
			// oops, we don't have all 4 aces, so we gotta stop
			// if we have fewer then 3 aces or less than 33 pts. then really panic
			if ((numTotalAces < 3) || (bidState.m_fMinTPPoints < PTS_SLAM)
				|| hand.HasWorthlessDoubleton()  // NCR can't do slam with worthless doubleton
				|| (hand.GetNumSuitsStopped() < 4) )  // NCR-344 Don't do slam without stoppers? Use: AllOtherSuitsStopped()???
			{
				int nTestBid = bidState.GetCheapestShiftBid(nAgreedSuit);
				// NCR-260 Can't do 5NT - pard will think its more Blackwood
				if (nTestBid < BID_5NT)
				{
					// if we can return to the trump suit at the 5 level,
					// then do so
					nBid = nTestBid;
					if (numTotalAces < 3)
						status << "BK33! Oops, we're caught with only " & numTotalAces &
								  " Aces, so halt the bidding at the five-level.\n";
					else
						status << "BK34! Oops, we're caught with as few as " & bidState.m_fMinTPPoints &
								  " total points, so halt the bidding at the five-level.\n";
				}
				// NCR-260 bail out with ???
				else if (nTestBid == BID_5NT)
				{
					nBid = bidState.GetCheapestShiftBid(bidState.nPartnersPrevSuit); // NCR-260???
				}
				else
				{
					//NCR Check if the bidsuit is the agreed suit and leave it
					if(BID_SUIT(nPartnersBid) == nAgreedSuit) {
						nBid = BID_PASS;  //NCR leave by passing		
					}else{
						nBid = MAKEBID(nAgreedSuit, 6);
					}
				}
			}
			else
			{
				nBid = MAKEBID(nAgreedSuit, 6);
			}
			status << "BK38! Without all four aces we can't proceed to a grand slam, so stop at " & 
						BTS(nBid) & ".\n";
			nBlackwoodStatus = CONV_FINISHED;	// Blackwood finished
		}
	
		// update status and return
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, nBlackwoodStatus);
		return TRUE;

	}


	//
	// Did we ask for kings?
	//
	if (nBlackwoodStatus == CONV_INVOKED_ROUND2) 
	{
		// first check for a strange response
		if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
		{
			// we don't understand partner's bid
			return CConvention::HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status);
		}

		//
		switch(nPartnersBid) 
		{
			//	
			case BID_6C:	// 0 or 4 Kings
				if (numKings == 0)
					bidState.m_numPartnersKings = 4;
				else
					bidState.m_numPartnersKings = 0;
				break;

			case BID_6D:	// 1 King
				bidState.m_numPartnersKings = 1;
				break;
			
			case BID_6H:	// 2 Kings
				bidState.m_numPartnersKings = 2;
				break;

			case BID_6S:	// 3 Kings
				bidState.m_numPartnersKings = 3;
				break;

			default:		// choke, gag, spew!
				bidState.m_numPartnersKings = 0;
				break;
		}
		if (nPartnersBid <= BID_6S) 
		{
			status << "BK40! Partner responded to our 4NT Blackwood inquiry with " & 
					  BTS(nPartnersBid) & ", indicating " & 
					  bidState.m_numPartnersKings & 
					  ((bidState.m_numPartnersKings > 1)? " Kings" : " King") &
					  ".\n";
		} 
		else 
		{
			status << "BK44! Partner made an illegal " & BTS(nPartnersBid) & 
					  " response to 4NT Blackwood inquiry; assuming zero Kings in partner's hand.\n";
			nBid = BID_PASS;
			bidState.SetConventionStatus(this, CONV_ERROR);
			return FALSE;
		}

		//
		// at this point, decide on a contract.
		//
		int numTotalAces = bidState.numAces + bidState.m_numPartnersAces;
		ASSERT(numTotalAces == 4);
		int numTotalKings = bidState.numKings + bidState.m_numPartnersKings;
		//
		if (numTotalKings == 4)
		{
			// we have all Aces & Kings, so bid a grand slam
			nBid = MAKEBID(nAgreedSuit, 7);
			status << "BK45! We have all the aces and kings, plus a total of " &
					  fMinTPPoints & "-" & fMaxTPPoints & 
					  " points in the partnership, so go ahead and bid " & 
					  BTS(nBid) & ".\n";
		}
		else if ((numTotalKings == 3) && (fMinTPPoints >= PTS_GRAND_SLAM))
		{
			// we're missing a king but have good pts, so go for a grand
			nBid = MAKEBID(nAgreedSuit, 7);
			status << "BK46! We're missing one King, but with a total of " & 
					  fMinTPPoints & "-" & fMaxTPPoints & 
					  " points in the partnership, we can go ahead and bid " & BTS(nBid) & ".\n";
		}
		else if ((numTotalKings >= 3) && (fMinTPPoints >= PTS_GRAND_SLAM+1))
		{
			// we're missing two kings but have good pts, so go for a grand
			nBid = MAKEBID(nAgreedSuit, 7);
			status << "BK47! We may not have all the kings, but with a total of " & 
					  fMinTPPoints & "-" & fMaxTPPoints & 
					  " points in the partnership, go ahead and bid " & BTS(nBid) & ".\n";
		}
		else
		{
			// we're missing two or more kings, so settle for a small slam
			nBid = MAKEBID(nAgreedSuit, 6);
			if (nBid > nPartnersBid)
			{
				// bail out at 6 of the agreed suit
				status << "BK48! Since we're missing " & (4 - numTotalKings) & 
						  " Kings and have a total of only " & fMinTPPoints & "-" & fMaxTPPoints & 
						  " pts in the partnership, we lack sufficient strength for a grand slam and have to settle for a small slam at " &
						  BTS(nBid) & ".\n";
			}
			// NCR-223 Did pard's bid happen to be what we want?
			else if(nBid == nPartnersBid)
			{
				nBid = BID_PASS;  // leave pard's bid in
				status << "BK48a! Since we're missing " & (4 - numTotalKings) & 
						  " Kings and have a total of only " & fMinTPPoints & "-" & fMaxTPPoints & 
						  " pts in the partnership, we lack sufficient strength for a grand slam and have to settle for a small slam at " &
						  BTS(nPartnersBid) & ".\n";
			}
			else
			{
				// bail out at 6NT
				nBid = BID_6NT;
				status << "BK49! Since we're missing " & (4 - numTotalKings) & 
						  " Kings and have a total of only " & fMinTPCPoints & "-" & fMaxTPCPoints & 
						  " HCPs in the partnership, we lack sufficient strength for a grand slam and have to settle for a small slam at " &
						  BTS(nBid) & ".\n";
			}
		}
		// update status and return
		bidState.SetBid(nBid);
		nBlackwoodStatus = 3;
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;

	}

	//
	return FALSE;
}











//
//==================================================================
// construction & destruction
//
CBlackwoodConvention::CBlackwoodConvention() 
{
	// from ConvCodes.h
	m_nID = tidBlackwood;
}

CBlackwoodConvention::~CBlackwoodConvention() 
{
}


