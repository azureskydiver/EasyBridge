//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Gambling3NTConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "Player.h"
#include "PlayerStatusDialog.h"
#include "Gambling3NTConvention.h"
#include "OvercallsConvention.h"
#include "ConventionSet.h"
#include "Card.h"




//
//===============================================================================
//
// TryConvention()
//
// check if we can use an Gambling 3NT Bid here
//
BOOL CGambling3NTConvention::TryConvention(const CPlayer& player, 
										 const CConventionSet& conventions, 
										 CHandHoldings& hand, 
										 CCardLocation& cardLocation, 
										 CGuessedHandHoldings** ppGuessedHands,
										 CBidEngine& bidState,  
										 CPlayerStatusDialog& status)
{
	//
	// the requirements for opening a STANDARD Gambling 3NT Bid are:
	// 1: a solid 7+ card minor suit,
	// 2: 10-12 HCPs,
	// 3: no voids or small singletons, and
	// 4: no outside stoppers
	//
	// the requirements for opening an ACOL Gambling 3NT Bid are:
	// 1: a solid 7+ card minor suit,
	// 2: 16-21 HCPs,
	// 3: no voids or small singletons, and
	// 4: stoppers in at least 2 of the remaining 3 suits

	// see which version we're playing
	int bStandardGambling3NT = (pCurrConvSet->GetValue(tnGambling3NTVersion) == 0);

	// see if we have any small singletons
	bool bSmallSingletons = false;
	if (bidState.numSingletons > 0)
	{
		for(int i=0;i<4;i++)
		{
			CSuitHoldings& suit = hand.GetSuit(i);
			if ((suit.GetNumCards() == 1) && (suit[0]->GetFaceValue() < TEN))
			{
				bSmallSingletons = true;
				break;
			}
		}
	}

	// see if we have a solid suit
	int nSuit = NONE;
	if (hand.GetSuit(CLUBS).IsSolid() && (bidState.numCardsInSuit[CLUBS] >= 7))
		nSuit = CLUBS;
	else if (hand.GetSuit(DIAMONDS).IsSolid() && (bidState.numCardsInSuit[DIAMONDS] >= 7))
		nSuit = DIAMONDS;

	// test for std and ACOL Gambling 3NT conditions
	int numValidBidsMade = pDOC->GetNumValidBidsMade();
	if ( bStandardGambling3NT && (numValidBidsMade  == 0) && (ISSUIT(nSuit)) &&
		 (bidState.fCardPts >= OPEN_PTS(10)) && (bidState.fCardPts <= OPEN_PTS(12)) &&
		 (bidState.numVoids == 0) && !bSmallSingletons &&
		 (bidState.numSuitsStopped == 1) )
	{
		 // passed the test for standard gambling 3NT
		status << "G3NT1! With a solid " & bidState.numCardsInSuit[nSuit] & 
				  "-card " & STSS(nSuit) & " suit, " & bidState.fCardPts &
				  " HCPs, no voids, no small singletons, and no outside stoppers, "
				  " go ahead and bid a Gambling 3NT.\n";
	}
	else if ( !bStandardGambling3NT && (numValidBidsMade == 0) && (ISSUIT(nSuit)) &&
			 (bidState.fCardPts >= OPEN_PTS(16)) && (bidState.fCardPts <= OPEN_PTS(21)) &&
			 (bidState.numVoids == 0) && !bSmallSingletons &&
			 (bidState.numSuitsStopped >= 3) )
	{
		 // passed the test for ACOL gambling 3NT
		status << "G3NT2! With a solid " & bidState.numCardsInSuit[nSuit] & 
				  "-card " & STSS(nSuit) & " suit, " & bidState.fCardPts &
				  " HCPs, no voids, no small singletons, and stoppers in " & 
				  ((bidState.numSuitsStopped == 4)? "all four suits" : "two outside suits") &
				  ", go ahead and bid an ACOL Gambling 3NT.\n";
	}
	else
	{
		return FALSE;
	}

	// OK, go ahead and bid 3NT
	bidState.SetBid(BID_3NT);
	bidState.SetConventionStatus(this, CONV_INVOKED);
	return TRUE;
}





//
//-----------------------------------------------------
//
// respond to partner's Gambling 3NT Bid
//
BOOL CGambling3NTConvention::RespondToConvention(const CPlayer& player, 
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
	int nOpeningBid = pDOC->GetOpeningBid();
	int nBid = NONE;

	//
	int nStatus = bidState.GetConventionStatus(this);
	if (nStatus == CONV_INACTIVE)
	{
		//
		// Bidding in response to partner's Gambling 3NT 
		//

		// and test conditions
		// partner must have opened at 3NT
		if ( (nOpeningBid == nPartnersBid) && (bidState.nPartnersBid == BID_3NT) )
		{
			 // passed the test
		}
		else
		{
			return FALSE;
		}

		// see which version of gambling 3NT we're playing
		int bStandardGambling3NT = (pCurrConvSet->GetValue(tnGambling3NTVersion) == 0);

		//
		if (bStandardGambling3NT)
		{
			// standard Gambling 3NT
			status << "G3NT10! Partner bid a Gambling 3NT, indicating " &
					   OPEN_PTS(10) & "-" & OPEN_PTS(12) & " HCPs, a solid 7+ card minor, "
					   " no voids or small singletons, and no outside stoppers.\n";

			// revalue partnership totals
			bidState.AdjustPartnershipPoints(10, 12);

			// if we have stoppers in at least 3 suits and no void minors, pass
			if ((bidState.numSuitsStopped >= 3) && 
					(bidState.numCardsInSuit[CLUBS] > 0) && (bidState.numCardsInSuit[DIAMONDS] > 0))
			{
				status << "G3NT12! Since we have " & bidState.numSuitsStopped & " suits stopped and no minor suit voids, we can pass.\n";
				nBid = BID_3NT;
			}
			else
			{
				nBid = BID_4C;
				if (bidState.numSuitsStopped < 3)
					status << "G3NT13! But since we have only " & bidState.numSuitsStopped & " suits stopped, we have to respond with " & BTS(nBid) & 
							  ", which partner can correct to 4D if necessary.\n";
				else
					status << "G3NT14! But since we are void in the " & 
							  ((bidState.numCardsInSuit[CLUBS] == 0)? "Club" : "Diamond") &
							  " suit, we have to respond at " & BTS(nBid) & 
							  ", which partner can correct to 4D if necessary.\n";
				//
				bidState.SetBid(nBid);
				bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND1);
				return TRUE;
			}
		}
		else
		{
			// ACOL Gambling 3NT
			// pass unless there's interest in slam
			status << "G3NT20! Partner bid an ACOL Gambling 3NT, indicating " &
					   OPEN_PTS(16) & "-" & OPEN_PTS(21) & " HCPs, a solid 7+ card minor, "
					   " no voids or small singletons, and stoppers in at least 2 outside suits.\n";

			// revalue partnership totals
			bidState.AdjustPartnershipPoints(16, 21);

			// respond positively only if there's interest in slam
			if (bidState.m_fMinTPPoints >= PTS_SLAM)
			{
				// bid slam directly
				if (bidState.m_fMinTPPoints >= PTS_GRAND_SLAM)
				{
					nBid = BID_7NT;
					status << "G3NT21! With a total of " & 
							  bidState.m_fMinTPPoints & "-" & bidState.m_fMinTPPoints & 
							  " pts in the partnership, bid a grand slam directly at " & BTS(nBid) & ".\n";
				}
				else
				{
					nBid = BID_6NT;
					status << "G3NT22! With a total of " & 
							  bidState.m_fMinTPPoints & "-" & bidState.m_fMinTPPoints & 
							  " pts in the partnership, bid slam directly at " & BTS(nBid) & ".\n";
				}
			}
			else
			{
				// gotta pass
				status << "G3NT29! But with a total of " & 
						  bidState.m_fMinTPPoints & "-" & bidState.m_fMinTPPoints & 
						  " pts in the partnership, we have no interest in slam and have to pass.\n";
				nBid = BID_PASS;
			}
		}
		//
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;
	}
	else if (nStatus == CONV_RESPONDED_ROUND1)
	{
		// we must have responded with a 4C, so pass here
		// see if partner made sense
		if ((nPartnersBid == BID_4D) || (nPartnersBid == BID_PASS))
			status << "G3NT40! Pass partner's " & BTS(nPartnersBid) & " and conclude the Gambling 3NT convention and pass.\n";
		else
			status << "G3NT41! Partner's " & BTS(nPartnersBid) & " bid doesn't make sense, so end the Gambling 3NT convention and pas.\n";
		//
		bidState.SetBid(BID_PASS);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;
	}

	//
	return FALSE;
} 







//
//==========================================================
//
// Rebidding as opener after partner responds to an Gambling 3NT Bid
//
BOOL CGambling3NTConvention::HandleConventionResponse(const CPlayer& player, 
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
	int nBid = NONE;
	int nPartnersBid = bidState.nPartnersBid;

	// first check for a strange response
	if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
	{
		// we don't understand partner's bid
		return CConvention::HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status);
	}

	//
	// handling partner's Gambling 3NT response
	//
	int nStatus = bidState.GetConventionStatus(this);

	//
	if (nStatus == CONV_INVOKED)
	{
		// if partner passed, we're done
		if (nPartnersBid == BID_PASS)
		{
			status << "G3NT60! Partner passed our Gambling 3NT bid, so we pass as well.\n";
			nBid = BID_PASS;
		}

		// else partner shold have bid 4C on a standard Gambling 3NT
		int bStandardGambling3NT = (pCurrConvSet->GetValue(tnGambling3NTVersion) == 0);
		//
		if (bStandardGambling3NT && (nPartnersBid == BID_4C))
		{
			// partner bid 4C, so correct to 4D if necesary
			status << "G3NT61! Partner responded at 4 Club to our opening Gambling 3NT bid, indicating a lack of stoppers or a void minor suit.\n";
			//
			if (bidState.nPrefSuit == DIAMONDS)
			{
				nBid = BID_4D;
				status << "G3NT62! But since our desired suit is Diamonds, correct to " & 
						  BTS(nBid) & ".\n";
			}
			else
			{
				status << "G3NT63! And since Clubs is our desired suit, we can pass and accept the 4C contract.\n";
				nBid = BID_PASS;
			}
		}
		else
		{
			// oops, error
			status << "G3NT65! Oops, partner's " & BTS(nPartnersBid) & 
					  " is not a proper response to our Gambling 3NT opening bid, so we have to pass.\n";
			nBid = BID_PASS;
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
CGambling3NTConvention::CGambling3NTConvention() 
{
	// from ConvCodes.h
	m_nID = tidGambling3NT;
}

CGambling3NTConvention::~CGambling3NTConvention() 
{
}


