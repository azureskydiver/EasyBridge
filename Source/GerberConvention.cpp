//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// GerberConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "PlayerStatusDialog.h"
#include "PlayerStatusDialog.h"
#include "GerberConvention.h"
#include "ConventionSet.h"



//
//==================================================================
// 
// ApplyTest()
//

//
// Gerbercan't be started automatically from within this test routine;
// rather, it has to be triggered by the general bidding code under
// certain spoecific circumstances.  
// But responding to Gerber can take place here.
//
BOOL CGerberConvention::ApplyTest(const CPlayer& player, 
								  const CConventionSet& conventions, 
								  CHandHoldings& hand, 
								  CCardLocation& cardLocation, 
								  CGuessedHandHoldings** ppGuessedHands,
								  CBidEngine& bidState, 
								  CPlayerStatusDialog& status)
{
	// basic test
//	if (!pCurrConvSet->IsConventionEnabled(tidGerber))
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
// InvokeGerber()
//
// Start the Gerber convention
//	 
BOOL CGerberConvention::InvokeGerber(CHandHoldings& hand, CBidEngine& bidState, CPlayerStatusDialog& status, int nEventualSuit)
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


	// if Gerber has already been used, return
	if (bidState.GetConventionStatus(this) >= CONV_INVOKED) 
		return FALSE;

	//
	// if Gerber is not enabled, just bid slam directly
	//
	if (!pCurrConvSet->IsConventionEnabled(tidGerber))
	{
		// no Gerber? go ahead and bid slam directly
		status << "2GERBX! we have " & fCardPts & "/" & fPts & "/" & fAdjPts &
				  " points in hand, for a total of approx. " &
				  fMinTPPoints & "-" & fMaxTPPoints & " / " &
				  fMinTPCPoints & "-" & fMaxTPCPoints & 
				  " pts in the partnership, but we can't use Gerber to investigate Aces and Kings since it's currently not enabled.\n";
		// bid a grand slam if we have 37+ pts and the trump ace
		// or a small slam with 32+ points
		// or make the cheapest shift bid otherwise (D'oh!)
		if ((fMinTPPoints >= PTS_GRAND_SLAM+1) && (hand.SuitHasCard(nEventualSuit, ACE)))
		{
			nBid = MAKEBID(nEventualSuit, 7);
			status << "2GERBX1! but since we have the points for a grand slam and we hold the trump ace, go ahead and bid " & 
					  BTS(nBid) & ".\n";
		}
		else if (fMinTPPoints >= PTS_SLAM)
		{
			nBid = MAKEBID(nEventualSuit, 6);
			status << "2GERBX2! but since we have the points for a small slam, go ahead and bid " & 
					  BTS(nBid) & ".\n";
		}
		else
		{
			// get cheapest shift bid
			nBid = bidState.GetCheapestShiftBid(nEventualSuit);
			status << "2GERBX3! so we just bid " & BTS(nBid) & ".\n";
		}
		//
		bidState.SetBid(nBid);
		return TRUE;
	}

	
	//
	// shouldn't use Gerber if we have all four aces
	//
	if (bidState.numAces == 4)
	{
		// no Gerber? go ahead and bid slam directly
		status << "2GERBY! Unfortunately, since we hold all four aces, Gerber is of little value to us.\n";
		// bid a grand slam if we have 37+ pts and the trump ace
		// or a small slam with 32+ points
		// or make the cheapest shift bid otherwise (D'oh!)  
		if ((fMinTPPoints >= PTS_SLAM+1) 
			    // NCR-324 added NOTRUMP test
			 && ((nEventualSuit == NOTRUMP) || (hand.SuitHasCard(nEventualSuit, ACE))))
		{
			nBid = MAKEBID(nEventualSuit, 7);
			status << "2GERBY1! so since we have the points for a grand slam, go ahead and bid " & 
					  BTS(nBid) & ".\n";
		}
		else if (fMinTPPoints >= PTS_SLAM)
		{
			nBid = MAKEBID(nEventualSuit, 6);
			status << "2GERBY2! so since we have the points for a small slam, go ahead and bid " & 
					  BTS(nBid) & ".\n";
		}
		else
		{
			nBid = bidState.GetCheapestShiftBid(nEventualSuit, pDOC->GetLastValidBid());
			status << "2GERBY3! so without quite enough points for a slam, we just bid " & BTS(nBid) & ".\n";
		}
		//
		bidState.SetBid(nBid);
		return TRUE;
	}


	//
	// else we're playing Gerber and do not hold all four aces
	// so make the bid to ask for aces
	//
	int nPartnersbid = bidState.nPartnersBid;
	if ((nPartnersbid == BID_1NT) || (nPartnersbid == BID_2NT) 
		// NCR-246a Allow Gerber after Stayman 2C
		|| ((nPartnersbid < BID_3NT) && bidState.nPreviousBid == BID_2C) )
	{
		nBid = BID_4C;
		status << "GERBD! with " & bidState.fCardPts & "/" & bidState.fPts & "/" & bidState.fAdjPts & 
				  " points in the hand, for a total of approx. " & 
				  fMinTPCPoints & "-" & fMaxTPCPoints & " / " &
				  fMinTPPoints & "-" & fMaxTPPoints & 
				  " pts in the partnership, explore slam possibilities with Gerber at " &
				  BTS(nBid) & ".\n";
	}
	else
	{
		nBid = BID_PASS;
		status << "GERBDx! we'd like to use Gerber, but partner has already bid higher than 4C, so we have to pass.\n";
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
// Check to see if a Gerber query has been made, 
// and if so, respond as appropriate
//
//	 
BOOL CGerberConvention::RespondToConvention(const CPlayer& player, 
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
	// first look for a Gerber _Query_ from partner -- 
	// any 4C in response to a natural NT bid is Gerber
	// NCR unless partner has bid clubs before
	//
	int nGerberStatus = bidState.GetConventionStatus(this);
	int numAces = bidState.numAces;
	int numKings = bidState.numKings;
	int nPrevioousBid = bidState.nPreviousBid;
	int nBid;

	// is partner asking for aces?
	if ( (bidState.nPartnersBid == BID_4C) 
		  // NCR-552 I didn't open 1C
		  && (bidState.nNextPrevBid != BID_1C) // Or test if CLUBs ???
		  // NCR-521 4C must be Jump to be Gerber
          && ((bidState.GetBidType(bidState.nPartnersBid) & (BT_Jump + BT_Leap)) != 0) // NCR-551 Also Leap
		  && ((nPrevioousBid == BID_1NT) || ((nPrevioousBid == BID_2NT) 
		                                     // NCR-184 was it Unusual 2NT ? use pts  ???
		                                     && (bidState.fCardPts >= OPEN_PTS(13)))  // NCR-324 use OPEN_PTS
/* NCR-466		      || ((nPrevioousBid == BID_3NT) 
			      // NCR-152 & NCR-170 Check if clubs are partner's suit
				                                                // NCR-299 Not if I bid clubs
			       && ((bidState.nPartnersPrevSuit != CLUBS) && (bidState.nNextPrevSuit != CLUBS)) ) */  // NCR-14 After 3NT ???
                 //NCR-246a Also after Stayman 2C
			  || ((bidState.nPartnersPrevBid == BID_2C) && (bidState.nNextPrevSuit == NOTRUMP)
			      // NCR-509 Pard is using Gerber only if I opened NT
			      && bidState.m_bOpenedBiddingForTeam)	 ) )
	{
		// respond
		if ((numAces == 0) || (numAces == 4))
			nBid = BID_4D;
		else if (numAces == 1)
			nBid = BID_4H;
		else if (numAces == 2)
			nBid = BID_4S;
		else if (numAces == 3)
			nBid = BID_4NT;  // NCR 4NT NOT 5
		//
		status << "GRB10! With " & numAces & " Ace" & ((numAces > 1)? "s," : ",") &
				  " respond to partner's Gerber inquiry with " & BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND1);	// answered Aces, ready for Kings
		return TRUE;
	}

	// is partner asking for kings?
	if ((nGerberStatus == CONV_RESPONDED_ROUND1) && 
					(bidState.nPartnersBid == BID_5C))	
	{
		// respond
		if ((numKings == 0) || (numKings == 4))
			nBid = BID_5D;
		else if (numKings == 1)
			nBid = BID_5H;
		else if (numKings == 2)
			nBid = BID_5S;
		else if (numKings == 3)
			nBid = BID_5NT;
		//
		status << "GRB12! With " & numKings & " King" & ((numKings > 1)? "s," : ",") &
				  " respond to partner's Gerber inquiry with " & BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);	// Gerber complete
		return TRUE;
	}

	// no Gerber
	return FALSE;
}






//
//---------------------------------------------------------------
//
// HandleConventionResponse()
//
// Check on Gerber response from the invoker, and act accordingly
//
//	 
BOOL CGerberConvention::HandleConventionResponse(const CPlayer& player, 
											        const CConventionSet& conventions, 
											        CHandHoldings& hand, 
											        CCardLocation& cardLocation, 
												    CGuessedHandHoldings** ppGuessedHands,
											        CBidEngine& bidState,  
											        CPlayerStatusDialog& status)
{
	int nGerberStatus = bidState.GetConventionStatus(this);
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
	if (nGerberStatus == CONV_INVOKED_ROUND1) 
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
			case BID_4D:	// 0 or 4 Aces
				if (numAces == 0)
					bidState.m_numPartnersAces = 4;
				else
					bidState.m_numPartnersAces = 0;
				break;

			case BID_4H:	// 1 Ace
				bidState.m_numPartnersAces = 1;
				break;
			
			case BID_4S:	// 2 Aces
				bidState.m_numPartnersAces = 2;
				break;

			case BID_4NT:	// 3 Aces
				bidState.m_numPartnersAces = 3;
				break;

			default:		// choke, gag, spew!
				bidState.m_numPartnersAces = 0;
				break;
		}
		if (nPartnersBid <= BID_4NT) 
		{
			status << "GRB20! Partner responded to our 4C Gerber inquiry with " & 
					  BTS(nPartnersBid) & ", indicating " & 
					  bidState.m_numPartnersAces & 
					  (((bidState.m_numPartnersAces == 0) || (bidState.m_numPartnersAces > 1))? " Aces" : " Ace") & ".\n";
		} 
		else 
		{
			status << "GRB22! Partner made an illegal " & BTS(bidState.nPartnersBid) & 
					  " response to 4C Gerber; assuming zero Aces in partner's hand.\n";
			bidState.SetConventionStatus(this, CONV_ERROR);	// Gerber complete
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
				status << "GRB32! Our team holds all four aces, so proceed to 5NT to ask for kings.\n";
				nGerberStatus = CONV_INVOKED_ROUND2;	// asking for kings
			}
			else
			{
				nBid = MAKEBID(nAgreedSuit, 6);
				status << "GRB32! Our team holds all four aces, but with an estimated " & 
						  fMinTPPoints & "-" & fMaxTPPoints &
					      " total partnership points, we lack the points for a grand slam, so settle for a contract of " & BTS(nBid) & ".\n";
				nGerberStatus = CONV_FINISHED;	// Gerber finished
			}
		}
		else
		{
			// oops, we don't have all 4 aces, so we gotta stop
			// if we have 3 or fewer aces and less than 32 pts. then really panic
			// NCR-246a Need 3 aces and points to continue
			if ((numTotalAces < 3) || (bidState.m_fMinTPPoints < PTS_SLAM) || (bidState.m_fMaxTPCPoints < PTS_SLAM) )
			{
				// NCR-246a See if we need to bid
				int nTestBid = BID_PASS;  // NCR-246a pass if at game in our suit
				if(( bidState.nPartnersSuit != nAgreedSuit) || !bidState.IsGameBid(bidState.nPartnersBid)) {
					nTestBid = bidState.GetCheapestShiftBid(nAgreedSuit);
                } 
				if (nTestBid <= BID_5NT)
				{
					// if we can return to the trump suit at the 5 level,
					// then do so
					nBid = nTestBid;
					status << "GRB34! Oops, we're caught with only " & numTotalAces &
							  " Aces and as few as " & bidState.m_fMinTPPoints &
							  " total points, so halt the bidding at the " & BID_LEVEL(nPartnersBid) 
							  & "-level.\n";  // NCR replaced five with BID_LEVEL()
				}		
				else
				{  
					nBid = MAKEBID(nAgreedSuit, 6);
				}
			}
			else
			{
				if( bidState.numSuitsStopped < 4) // NCR-325 don't try slam without all 4 stopped
				{
					nBid = bidState.GetCheapestShiftBid(nAgreedSuit);  //NCR-325 get cheap bid
				}else
				    nBid = MAKEBID(nAgreedSuit, 6);
			}
			status << "GRB38! Without all four aces we can't proceed to a grand slam, so stop at " & 
			BTS(nBid) & ".\n";
			nGerberStatus = CONV_FINISHED;	// Gerber finished
		}
	
		// update status and return
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, nGerberStatus);
		return TRUE;

	}


	//
	// Did we ask for kings?
	//
	if (nGerberStatus == CONV_INVOKED_ROUND2) 
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
			case BID_5D:	// 0 or 4 Kings
				if (numKings == 0)
					bidState.m_numPartnersKings = 4;
				else
					bidState.m_numPartnersKings = 0;
				break;

			case BID_5H:	// 1 King
				bidState.m_numPartnersKings = 1;
				break;
			
			case BID_5S:	// 2 Kings
				bidState.m_numPartnersKings = 2;
				break;

			case BID_5NT:	// 3 Kings
				bidState.m_numPartnersKings = 3;
				break;

			default:		// choke, gag, spew!
				bidState.m_numPartnersKings = 0;
				break;
		}
		if (nPartnersBid <= BID_5NT) 
		{
			status << "GRB40! Partner responded to our 4C Gerber inquiry with " & 
					  BTS(nPartnersBid) & ", indicating " & 
					  bidState.m_numPartnersKings & 
					  ((bidState.m_numPartnersKings > 1)? " Kings" : " King") &
					  ".\n";
		} 
		else 
		{
			status << "GRB44! Partner made an illegal " & BTS(nPartnersBid) & 
					  " response to 4C Gerber inquiry; assuming zero Kings in partner's hand.\n";
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
			status << "GRB45! We have all the aces and kings, plus a total of " &
					  fMinTPPoints & "-" & fMaxTPPoints & 
					  " points in the partnership, so go ahead and bid " & 
					  BTS(nBid) & ".\n";
		}
		else if ((numTotalKings == 3) && (fMinTPPoints >= PTS_GRAND_SLAM))
		{
			// we're missing a king but have good pts, so go for a grand
			nBid = MAKEBID(nAgreedSuit, 7);
			status << "GRB46! We're missing one King, but with a total of " & 
					  fMinTPPoints & "-" & fMaxTPPoints & 
					  " points in the partnership, we can go ahead and bid " & BTS(nBid) & ".\n";
		}
		else if ((numTotalKings >= 3) && (fMinTPPoints >= PTS_GRAND_SLAM+1))
		{
			// we're missing two kings but have good pts, so go for a grand
			nBid = MAKEBID(nAgreedSuit, 7);
			status << "GRB47! We may not have all the kings, but with a total of " & 
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
				status << "GRB48! Since we're missing " & (4 - numTotalKings) & 
						  " Kings and have a total of only " & fMinTPPoints & "-" & fMaxTPPoints & 
						  " pts in the partnership, we lack sufficient strength for a grand slam and have to settle for a small slam at " &
						  BTS(nBid) & ".\n";
			}
			else
			{
				// bail out at 6NT
				nBid = BID_6NT;
				status << "GRB49! Since we're missing " & (4 - numTotalKings) & 
						  " Kings and have a total of only " & fMinTPCPoints & "-" & fMaxTPCPoints & 
						  " HCPs in the partnership, we lack sufficient strength for a grand slam and have to settle for a small slam at " &
						  BTS(nBid) & ".\n";
			}
		}
		// update status and return
		bidState.SetBid(nBid);
		nGerberStatus = 3;
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
CGerberConvention::CGerberConvention() 
{
	// from ConvCodes.h
	m_nID = tidGerber;
}

CGerberConvention::~CGerberConvention() 
{
}


