//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// WeakTwoBidsConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "PlayerStatusDialog.h"
#include "Card.h"
#include "WeakTwoBidsConvention.h"
#include "HandHoldings.h"
#include "ConventionSet.h"



//
//===============================================================================
//
// TryConventon()
//
// check if we can open with a strong two-bid
//
BOOL CWeakTwoBidsConvention::TryConvention(const CPlayer& player, 
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
	// To open with a weak two bid, we need the following:
	// 1: 6-11 pts,
	// 2: a good 6-card major suit with 2+ high honors, 
	// 3: 1.5-2 Quick Tricks(?), (actually, ignore this rqmt)
	// 4: < 4 cards in the other major, and
	// 5: no voids, and no more than one singleton
	//
	if ( ((bidState.fPts >= OPEN_PTS(6)) && (bidState.fPts <= OPEN_PTS(11))) &&
				(ISMAJOR(bidState.nPrefSuit)) &&
				(bidState.numPrefSuitCards >= 6) && 
				(bidState.numHonorsInPrefSuit >= 2) && 
				(bidState.numPrefSuitPoints >= 4) && 
//			 	(bidState.numQuickTricks >= 1.5) &&
			 	(bidState.numCardsInSuit[OTHER_MAJOR(bidState.nPrefSuit)] < 4) &&
			 	(bidState.numVoids == 0) &&
			 	(bidState.numSingletons <= 1)) 
	{
		// this meets the requirements
		int nSuit = bidState.nPrefSuit;
		int nBid = MAKEBID(nSuit,2);
		status << "WEAK2! Have a good " & bidState.numCardsInSuit[nSuit] & 
				  "-card " & STSS(nSuit) & " suit with " & 
				  bidState.fCardPts & "/" & bidState.fPts & 
				  " points (less than normal opening requirements), so bid a weak " &
				  BTS(nBid) & ".\n";
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_INVOKED);
		return TRUE;
	}
	//
	return FALSE;
}








//
//
//=========================================================
//
// see if partner made a weak 2-bid
//
// in general, partner's opening weak-2 bid denotes a hand that
// has less than normal opening values and a good long (6+) suit.
// The exception of course if the 2C bid, which is handled
// in a prior section
//
BOOL CWeakTwoBidsConvention::RespondToConvention(const CPlayer& player, 
											     const CConventionSet& conventions, 
											     CHandHoldings& hand, 
											     CCardLocation& cardLocation, 
												 CGuessedHandHoldings** ppGuessedHands,
											     CBidEngine& bidState,  
											     CPlayerStatusDialog& status)
{
	// first see if another convention is active
	if (bidState.GetActiveConvention() && 
				(bidState.GetActiveConvention() != this))
		return FALSE;

	//
	// Bidding in response to an opening weak 2 bid? check requirements
	//
	int nPreviousBid = bidState.nPreviousBid;
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersSuit = bidState.nPartnersSuit;
	int nPartnersPrevSuit = bidState.nPartnersPrevSuit;
	int nPartnersSuitSupport = bidState.nPartnersSuitSupport;
	int numSupportCards = bidState.numSupportCards;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int numPartnerBidsMade = bidState.m_numPartnerBidsMade;

	//
	// first see if this is our second response to partner's weak 2 opening
	//
	if (bidState.GetConventionStatus(this) == CONV_RESPONDED)
	{
		// no more actions beyond this stage
		bidState.SetConventionStatus(this, CONV_FINISHED);

		// need to check only if partner responded to our 2NT response
		if ((nPreviousBid == BID_2NT) && ISBID(nPartnersBid))
		{
			int nBid;
			// see if partner bid a suit or bailed out
			if (nPartnersSuit != nPartnersPrevSuit)
			{
				// partner showed an outside Ace or King
				// if we're strong in that suit && have good support, push towards game
//				if ((bidState.nSuitStrength[nPartnersSuit] >= SS_OPENABLE) &&
				if ((hand.GetSuit(nPartnersSuit).GetCardPoints() >= 3) &&
					(bidState.m_fMinTPPoints >= PTS_GAME-2) &&
					(bidState.nPartnersSuitSupport >= STRENGTH_WEAK_SUPPORT))
				{
					// raise to game
					nBid = bidState.GetGameBid(nPartnersPrevSuit);
					status << "RWKT2B1! Partner has shown an outside Ace or King in the " &
							  STSS(nPartnersSuit) & " suit, so with strength in " & STS(nPartnersSuit) & ", " & 
							  bidState.numSupportCards & "-card trump support, and a total of " &
							  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
							  " points in the partnership, raise to " & BTS(nBid) & ".\n";
				}
				else
				{
					// return to partner's suit
					nBid = bidState.GetCheapestShiftBid(nPartnersPrevSuit);
					status << "RWKT2B2! Partner has shown an outside Ace or King in the " &
							  STSS(nPartnersSuit) & " suit, but with a total of " &
							  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
							  " pts in the partnership we lack the strength to raise further, so return to partner's suit at " &
							  BTS(nBid) & ".\n";
				}
			}
			else
			{
				// partner returned to his old suit
				nBid = BID_PASS;
				status << "RWKT2B5! Partner has returned to his original " & STSS(nPartnersPrevSuit) &
						  " suit, indicating no interest in game, so we pass.\n";
			}
			bidState.SetBid(nBid);
			return TRUE;
		}
		else
		{
			// nothing more here
			return FALSE;
		}
	}


	//
	// partner must've bid at the 2 level, but not 2C,
	// and partner's bid must have been the first bid made
	//
	if ((nPartnersBidLevel == 2) && 
					(nPartnersBid != BID_2C) &&
					(nPartnersBid != BID_2NT) &&
					(bidState.m_bPartnerOpenedForTeam) &&
					(numPartnerBidsMade == 1) &&
					(nPartnersBid == pDOC->GetValidBidRecord(0)))
	{
		// okay, met requirements
	}
	else
	{
		// 
		return FALSE;
	}

	//
	int nBid;
	// revalue our hand as dummy
	double fPts = bidState.fPts;
	double fAdjPts = bidState.fAdjPts = hand.RevalueHand(REVALUE_DUMMY, bidState.m_nAgreedSuit, TRUE);
	double fCardPts = bidState.fCardPts;
	int nPrefSuit = bidState.nPrefSuit;

	// state expectations
	bidState.m_fPartnersMin = OPEN_PTS(6);
	bidState.m_fPartnersMax = OPEN_PTS(11);
	status << "RWKT! Partner made a weak 2-bid, showing a a 6+ card suit with " &
			  bidState.m_fPartnersMin & "-" & bidState.m_fPartnersMax & 
			  " points, less than 4 cards in the other major, no voids, and no more than one singleton.\n";

	// set partnership point count minimums & maximums
	bidState.m_fMinTPPoints = fAdjPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPPoints = fAdjPts + bidState.m_fPartnersMax;
	bidState.m_fMinTPCPoints = fCardPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPCPoints = fCardPts + bidState.m_fPartnersMax;


	//
	//---------------------------------------------------------------------
	// < 6 points:  pass, since hand is too weak
	//
	if (fAdjPts < OPEN_PTS(6)) 
	{
		if ((numSupportCards >= 4) && (fPts >= OPEN_PTS(4))) 
		{
			// make a shutout bid
			nBid = MAKEBID(bidState.nPartnersSuit,4);
			bidState.SetAgreedSuit(nPartnersSuit);
			status << "RWKT1! We have only " & fCardPts & "/" & fPts & "/" & fAdjPts &
					  " points but have " & numSupportCards &
					  " support cards, so make a shutout response of " & BTS(nBid) & ".\n";
		}
		else
		{
			nBid = BID_PASS;
			status << "RWKT2! With only " & fCardPts & "/" & fPts & "/" & fAdjPts &
					  " points, the hand is too weak for a response, so pass.\n";
		}
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_INACTIVE);
		return TRUE;
	}

	///
	//---------------------------------------------------------------------
	// 6-12 points:  
	// not enough points for game, so raise or shift
	//
	if ( ((nPartnersSuitSupport < SS_WEAK_SUPPORT) && 
							(fPts >= OPEN_PTS(6)) && (fPts <= OPEN_PTS(12))) ||
		 ((nPartnersSuitSupport >= SS_WEAK_SUPPORT) && 
					 		(fAdjPts >= OPEN_PTS(6)) && (fAdjPts <= OPEN_PTS(12))) ) 
	{
		//
		status << "RWKT4! We only have " & fCardPts & "/" & fPts & "/" & fAdjPts &
				  " points, for a partnership total of " &
				  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
				  " points, which is probably not enough for game.\n";
		// if we like partner's suit, raise if possible
		if ( (numSupportCards >= 3) || 
			 ((numSupportCards >= 2) && (bidState.numHonorsInPartnersSuit >= 1)) )
		{
			// raise with 3+ trumps or 2 good trumps
			nBid = MAKEBID(bidState.nPartnersSuit,3);
			bidState.SetAgreedSuit(nPartnersSuit);
			status << "RWKT6! And with at least minimum (" & numSupportCards & 
					  "-card) support for partner's " & bidState.szPS &
					  " (holding " & bidState.szHP & 
					  "), make a weak shutout raise of " & BTS(nBid) & ".\n";
		} 
		else if ((nPrefSuit != nPartnersSuit) && (bidState.numPrefSuitCards >= 5))
		{
			// here we have no more then two low trump cads
			// since don't like partner's suit, so bid our own
			nBid = bidState.GetCheapestShiftBid(nPrefSuit);
			status << "RWKT8! And we don't like partner's " & 
					  bidState.szPSS & " suit (holding " & bidState.szHP & 
					  "), so shift to our preferred " & bidState.szPrefSS & 
					  " suit in a bid of " & BTS(nBid) & ".\n";
		} 
		else if ((bidState.numVoids == 0) && (bidState.numSingletons == 0))
		{
			// bid 2NT if not too unbalanced
			nBid = BID_2NT;
			status << "RWKT9! And since we don't like partner's " & 
					  bidState.szPSS & " suit (holding " & bidState.szHP & 
					  "), and have no good suit of our own, bid " & BTS(nBid) & ".\n";
		}
		else 
		{
			// no alternative, so pass
			nBid = BID_PASS;
			status << "RWKT10! And since we don't like partner's " & 
					  bidState.szPSS & " suit (holding " & bidState.szHP & 
					  "), have no good suit of our own, and hold an imbalanced hand, we have to pass.\n";
		}
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_RESPONDED);
		return TRUE;
	}

	//
	//-----------------------------------------------------------------
	// 13-19 pts:
	// try for game
	//
	if ( ((nPartnersSuitSupport < SS_WEAK_SUPPORT) && 
							(fPts >= OPEN_PTS(13)) && (fPts <= PTS_GAME-6)) ||
		 ((nPartnersSuitSupport >= SS_WEAK_SUPPORT) && 
					 		(fAdjPts >= OPEN_PTS(13)) && (fAdjPts <= PTS_GAME-6)) ) 
	{
		//
		status << "We have " & fCardPts & "/" & fPts & "/" & fAdjPts &
				  " points, for a partnership total of " &
				  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
				  " points, so we are interested in game if a good fit can be found.\n";

		// if we like partner's suit and have the pts, raise directly to 4 of the suit
		if ( ((nPartnersSuitSupport >= SS_WEAK_SUPPORT) && (fAdjPts >= 18)) ||
			 ((nPartnersSuitSupport >= SS_MODERATE_SUPPORT) && (fAdjPts >= 17)) )
		{
			nBid = MAKEBID(nPartnersSuit,4);
			bidState.SetAgreedSuit(nPartnersSuit);
			status << "RWKT20! And with " & bidState.SLTS(bidState.nPartnersSuit) &
					  " support for partner's " & bidState.szPSS & " suit (holding " & 
					  bidState.szHP & "), and a total of " & 
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " pts in the partnership, we raise to " & BTS(nBid) & ".\n";
		} 
		// else if we otherwise have a decent fit, invite to game with 2NT
		else if (nPartnersSuitSupport >= SS_WEAK_SUPPORT)
		{
			nBid = BID_2NT;
			bidState.SetAgreedSuit(nPartnersSuit);
			status << "RWKT21! And we like partner's " & bidState.szPSS & 
					  " suit (holding " & bidState.szHP & " support), but with only " &
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " total pts we don't quite have enough for a direct game bid, so invite partner to show an outside Ace or King with a bid of 2NT.\n";
		}
		else if ((bidState.bBalanced) && (bidState.m_fMinTPCPoints >= PTS_GAME-1)) 
		{
			// have a balanced hand with all suits stopped?
			if (hand.AllOtherSuitsStopped(nPartnersSuit)) 
			{
				// got all suits covered; bid 2NT or 3NT
				if (bidState.m_fMinTPCPoints >= PTS_GAME) 
					nBid = BID_3NT;
				else 
					nBid = BID_2NT;
				status << "RWKT22! And since we have a balanced hand with " & fCardPts & 
						  " HCPs, for a total in the partnership of approx. " & 
						  bidState.m_fMinTPCPoints & "-" & bidState.m_fMaxTPCPoints &
						  " HCPs, and all" & 
						  ((bidState.numSuitsStopped == 4)? " " : " unbid ") &
						  "suits are stopped, we can bid " & BTS(nBid) & ".\n";
			} 
			else 
			{
				// without all suits topped, bid 2NT
				nBid = BID_2NT;
				status << "RWKT24! And since we lack good support for partner's " & 
						  bidState.szPSS & " suit (holding " & bidState.szHP & 
						  ") but have a balanced hand, albeit lacking stoppers in " &
						  hand.GetUnstoppedSuitNames() & ", bid " & BTS(nBid) & ".\n";
			}
		} 
		else if (nPrefSuit == nPartnersSuit) 
		{
			// here, we don't have great support for partner's
			// suit, but that's the best suit we've got, so raise
			nBid = MAKEBID(nPartnersSuit,3);
			bidState.SetAgreedSuit(nPartnersSuit);
			status << "RWKT26! Our support for partner's " & bidState.szPSS & 
					  " suit isn't great (holding " & bidState.szHP & 
					  "), but we have have no better suit, so raise partner one level to " & 
					  BTS(nBid) & ".\n";
		} 
		else if ((bidState.numPrefSuitCards >= 6) &&
					(bidState.nPrefSuitStrength >= SS_OPENABLE)) 
		{
			// don't like partner's suit & have a 6-card 
			// openable suit of our own, so show it
			nBid = bidState.GetCheapestShiftBid(nPrefSuit);
			status << "RWKT28! But we don't like partner's " & 
					  bidState.szPSS & " suit (holding " & bidState.szHP & 
					  "), so shift to our preferred " & bidState.szPrefSS & 
					  " suit in a bid of " & BTS(nBid) & ".\n";
		} 
		else if (numSupportCards >= 2) 
		{
			// else just raise partner
			nBid = SINGLE_RAISE(nPartnersBid);
			bidState.SetAgreedSuit(nPartnersSuit);
			status << "RWKT30! We lack good support for partner's " & 
					  bidState.szPSS & " (holding " & bidState.szHP & 
					  "), but our hand is not appropriate for NT and we have no good suit of our own, so the best we can do is raise partner to " & 
					  BTS(nBid) & ".\n";
		} 
		else 
		{
			// else gotta pass
			nBid = BID_PASS;
			status << "RWKT32! We have very poor support for partner's " & 
					  bidState.szPSS & " (holding " & bidState.szHP & 
					  "), and no good suit of our own, so we have to pass.\n";
		}
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_RESPONDED);
		return TRUE;
	}


	//
	//---------------------------------------------------------------
	// Here, we have 20+ pts:
	// look for a possible slam
	//
	status << "RWKT40! We have " & fCardPts & "/" & fPts & "/" & fAdjPts &
			  " points, for a partnership total of " & 
			  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
			  " points, so we may be interested in a possible slam.\n";

	// if the trump support is solid, raise to 6 of the suit
	if (nPartnersSuitSupport >= SS_STRONG_SUPPORT) 
	{
		nBid = MAKEBID(nPartnersSuit,6);
		bidState.SetAgreedSuit(nPartnersSuit);
		status << "RWKT42! And since we have a strong holding in the " & 
				  bidState.szPSS & " suit (" & bidState.szHP & 
				  "), we raise to " & BTS(nBid) & ".\n";
	} 
	else if (nPartnersSuitSupport >= SS_GOOD_SUPPORT) 
	{
		// if we like partner's suit, invoke Blackwood
		status << "RWKT44! And since we like partner's " & 
				  bidState.szPSS & " suit (holding " & bidState.szHP & 
				  " support), move towards slam.\n";
		bidState.InvokeBlackwood(nPartnersSuit);
		return TRUE;
	} 
	else if ((bidState.bBalanced) & 
				(hand.AllOtherSuitsStopped(nPartnersSuit))) 
	{
		// with a balanced hand & all other suits stopped, 
		// bid 4NT for Blackwood, intending to go to 6NT or 7NT
		status << "RWKT46! And we have a balanced hand with all" & 
				  ((bidState.numSuitsStopped == 4)? " " :  " unbid ") &
				  "suits stopped, so move towards slam.\n";
		bidState.InvokeBlackwood(NOTRUMP);
		return TRUE;
	} 
	else 
	{
		// here, we don't have great or even good support for 
		// partner's suit, and also do not have a balanced hand, so
		// do a jump shift in our preferred suit
		nBid = bidState.GetJumpShiftBid(nPrefSuit, nPartnersBid);
		status << "RWKT48! We don't have very good support for partner's " & 
				  bidState.szPSS & " suit (holding " & bidState.szHP & 
				  ", and don't have the proper distribution for a NT response, so jump shift to " & 
				  BTS(nBid) & ".  Slam possibilities may be explored later.\n";
	}
	// report
	bidState.SetBid(nBid);
	bidState.SetConventionStatus(this, CONV_RESPONDED);
	return TRUE;
}





//
//==========================================================
//
// Rebidding as opener after a weak 2-level opening
//
//
CWeakTwoBidsConvention::HandleConventionResponse(const CPlayer& player, 
									  			 const CConventionSet& conventions, 
												 CHandHoldings& hand, 
												 CCardLocation& cardLocation, 
												 CGuessedHandHoldings** ppGuessedHands,
												 CBidEngine& bidState,  
												 CPlayerStatusDialog& status)
{
	if (bidState.GetConventionStatus(this) != CONV_INVOKED)
		return FALSE;
	bidState.ClearConventionStatus(this);	// no more actions after this round

	//
	// estimate partner's strength
	//
	int nBid;
	double fPts = bidState.fPts;
	double fAdjPts = bidState.fAdjPts;
	double fCardPts = bidState.fCardPts;
	int nPrefSuit = bidState.nPrefSuit;
	int nPrefSuitStrength = bidState.nPrefSuitStrength;
	int nPreviousBid = bidState.nPreviousBid;
	int nPreviousSuit = bidState.nPreviousSuit;
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int nPartnersSuit = bidState.nPartnersSuit;
	int nPartnersSuitSupport = bidState.nPartnersSuitSupport;
	int numSupportCards = bidState.numSupportCards;

	// estimate partner's pts
	if (nPartnersBid == BID_2NT)
	{
		// a bid of 2NT indicates 13-18 pts
		bidState.m_fPartnersMin = 13;
		bidState.m_fPartnersMax = 19;
	}
	else if (nPartnersBid == BID_3NT)
	{
		// a bid of 3NT indicates 19+ pts
		bidState.m_fPartnersMin = 19;
		bidState.m_fPartnersMax = 40 - bidState.fCardPts;
	}
	else if ((nPartnersBid - nPreviousBid) <= 5)
	{
		// a (single) raise or a suit shift indicates 6-12 pts
		bidState.m_fPartnersMin = 6;
		bidState.m_fPartnersMax = 12;
		// revalue holdings
		if (nPartnersSuit == nPreviousSuit)
			fAdjPts = bidState.fAdjPts = hand.RevalueHand(REVALUE_DECLARER, nPreviousSuit, TRUE);
	}
	else
	{
		// we don't understand partner's bid
		return CConvention::HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status);
	}

	// set values
	bidState.m_fMinTPPoints = fAdjPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPPoints = fAdjPts + bidState.m_fPartnersMax;
	bidState.m_fMinTPCPoints = fCardPts + bidState.m_fPartnersMin;
	bidState.m_fMaxTPCPoints = fCardPts + bidState.m_fPartnersMax;

	// a shift to another suit is forcing for one round
	if (((nPartnersBidLevel == 2) || (nPartnersBidLevel == 3)) && 
					(nPartnersSuit != nPreviousSuit) && (nPartnersSuit != NOTRUMP)) 
	{
		status << "HRW2! Partner responded to our weak " & bidState.szPVB & 
				  " opening by shifting to " & bidState.szPS & 
				  " which is forcing for one round, and asks for a raise with 3-card trump support or a doubleton honor.\n";

		// raise with 3-card support or doubleton honors
		if ((numSupportCards >= 3) || 
			((numSupportCards == 2) &&
			 (bidState.numHonorsInPartnersSuit >= 1)) ) {
			nBid = MAKEBID(nPartnersSuit,nPartnersBidLevel+1);
			status << "HRWT10! And with adequate support for partner's " & bidState.szPS & 
					  " (holding " & bidState.szHP & 
					  "), we dutifully raise to " & BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
			return TRUE;
		}

		// else rebid our own suit if possible
		if ((bidState.nPreviousSuitStrength >= SS_STRONG) && (fPts >= 8)) 
		{
			nBid = bidState.GetCheapestShiftBid(nPreviousSuit);
			status << "HRWT14! But without adequate support for partner's " & 
					  bidState.szPS & " (holding " & bidState.szHP & 
					  "), and with " & fCardPts & "/" & fPts & 
					  " points in hand, we have to rebid our own " & 
					  bidState.szPVS & " suit at " & BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
			return TRUE;
		}

		// else pass
		nBid = BID_PASS;
		status << "HRWT20! But we lack adequate support for partner's " & 
				  bidState.szPS & " (holding " & bidState.szHP & 
				  "), and don't have enough strength to rebid our own suit, so we have to pass.\n";
		bidState.SetBid(nBid);
		return TRUE;
	}

	//
	// a bid of 2NT is a quality inquiry (Ogust convention)
	//
	if ((nPartnersBid == BID_2NT) && (pCurrConvSet->IsConventionEnabled(tidOgust))) 
	{
		status << "B3F20! Partner bid 2NT in response to our weak " & bidState.szPVB & 
				  ". asking for the quality of our hand (Ogust convention).\n";
		// 3C: < 9 points & moderate suit, respond 3C
		// 3D: < 9 pts & good suit (AK, AQ, KQ)
		// 3H: >= 9 pts & moderate suit
		// 3S: >= 9 pts & good suit
		// 3NT: Best possible suit -- 6 cards & 3+ honors
		//      (point count unimportant)
		if ((bidState.numPreviousSuitCards >= 6) &&
				(bidState.numPreviousSuitPoints >= 9)) 
		{
			nBid = BID_3NT;
			status << "HRWT30! And with an excellent " & bidState.szPVS & 
					  " suit (holding " & bidState.szHPV & 
					  "), respond to Ogust with 3NT.\n";
			bidState.SetBid(nBid);
			return TRUE;
		} 
		else if ((bidState.numPreviousSuitPoints >= 5) &&
				  (hand.GetNumCardsInSuit(nPreviousSuit) > 1) &
				  (hand.GetCardByPosition(nPreviousSuit,1)->GetFaceValue() != JACK)) 
		{
			// good suit
			if (fPts <= 8) 
			{
				nBid = BID_3D;
				status << "HRWT32! And with a good suit " & bidState.szPVS & 
						  " suit (holding " & bidState.szHPV & ") and " & fPts & 
						  " points, respond to Ogust with 3D.\n";
				bidState.SetBid(nBid);
				return TRUE;
			} 
			else 
			{
				nBid = BID_3S;
				status << "HRWT34! And with a good suit " & bidState.szPVS & 
						  " suit (holding " & bidState.szHPV & "s) and " & fPts & 
						  " points, respond to Ogust with 3S.\n";
				bidState.SetBid(nBid);
				return TRUE;
			}
		} 
		else 
		{
			// moderate suit
			if (fPts <= 8) 
			{
				nBid = BID_3C;
				status << "HRWT36! And with a moderate " & 
						  bidState.szPVS & " suit (holding " & 
						  bidState.szHPV & ") and " & fPts & 
						  " points, respond to Ogust with 3C.\n";
				bidState.SetBid(nBid);
				return TRUE;
			} 
			else 
			{
				nBid = BID_3H;
				status << "HRWT38! And with a good suit " & 
						  bidState.szPVS & " suit (holding " & 
						  bidState.szHPV & ") and " & fPts & 
						  " points, respond to Ogust with 3H.\n";
				bidState.SetBid(nBid);
				return TRUE;
			}
		}
	}

	//
	// a bid of 2NT without playing Ogust indicates 
	// a balanced hand
	//
	if ((nPartnersBid == BID_2NT) && !pCurrConvSet->IsConventionEnabled(tidOgust)) 
	{
		//
		status << "HRWT40! Partner responded to our opening weak " & bidState.szPVB & 
				  " bid with 2NT, showing a trump fit and asking for an outside Ace or King.\n";
		// find an outside King or Ace
		int nSuit = NONE;
		BOOL bAceFound = FALSE;
		if ( (hand.GetNumAces() > 1) || 
			 ((hand.GetNumAces() == 1) && !hand.SuitHasAce(nPreviousSuit)) )
		{
			// there's an ace in at least one outside suit
			for(int i=0;i<4;i++)
			{
				if ((i != nPreviousSuit) && hand.SuitHasAce(i))
				{
					nSuit = i;
					bAceFound = TRUE;
					break;
				}
			}
		}
		// have we found an Ace?  if not, look for an outside King
		if (!ISSUIT(nSuit))
		{
			if ( (hand.GetNumKings() > 1) || 
				 ((hand.GetNumKings() == 1) && !hand.SuitHasKing(nPreviousSuit)) )
			{
				// there's an ace in at least one outside suit
				for(int i=0;i<4;i++)
				{
					if ((i != nPreviousSuit) && hand.SuitHasKing(i))
					{
						nSuit = i;
						break;
					}
				}
			}
		}

		// if we found an outside suit with an Ace or King, show it;
		// else bail out
		if (ISSUIT(nSuit))
		{
			// show the suit
			nBid = bidState.GetCheapestShiftBid(nSuit);
			status << "HRWT41! Show our outside " &  (bAceFound? "Ace" : "King") & 
					  " with a bid of " & BTS(nBid) & ".\n";
		}
		else
		{
			// bail out
			nBid = bidState.GetCheapestShiftBid(nPreviousSuit);
			status << "HRWT42! Unfortunately, we have no outside Aces or Kings, so bail out at " & 
					  BTS(nBid) & ".\n";
		}
		//
		bidState.SetBid(nBid);
		return TRUE;
	}


	// a raise to the 3-level is weak (preemptive)
	//
	if ((nPartnersBidLevel == 3) && (nPartnersSuit == nPreviousSuit)) 
	{
		// 
		nBid = BID_PASS;
		bidState.SetAgreedSuit(nPreviousSuit);
		status << "HRWT42! Partner responded to our weak " & bidState.szPVS & 
				  " bid with a weak raise to " & bidState.szPB & 
				   ".  The partnership likely lacks enough points for game, so pass.\n";
		bidState.SetBid(nBid);
		return TRUE;
	}

	// a raise to the 4-level is forcing to game
	//
	if ((nPartnersBidLevel == 4) && (nPartnersSuit == nPreviousSuit)) 
	{
		// revalue partner's strength
		bidState.m_fPartnersMin = 13;
		bidState.m_fPartnersMax = 19;
		bidState.m_fMinTPPoints = fAdjPts + bidState.m_fPartnersMin;
		bidState.m_fMaxTPPoints = fAdjPts + bidState.m_fPartnersMax;
		bidState.m_fMinTPCPoints = fCardPts + bidState.m_fPartnersMin;
		bidState.m_fMaxTPCPoints = fCardPts + bidState.m_fPartnersMax;
		// 
		nBid = BID_PASS;
		bidState.SetAgreedSuit(nPreviousSuit);
		status << "HRWT50! Partner responded to our weak " & bidState.szPVS & 
				  " bid with a game raise to " & bidState.szPB & 
				  ", so accept it and pass.\n";
		bidState.SetBid(nBid);
		return TRUE;
	}

	// any other response is to play
	//
	nBid = BID_PASS;
	status << "HRWT80! After our weak " & bidState.szPVB & 
			  " opening bid, assume partner's " & bidState.szPB & 
			  " bid is to play, so pass.\n";
/*
	status << "HRWT80! After our weak " & bidState.szPVB & 
			  " opening bid, we don't understand partner's " & 
			  bidState.szPB & " bid, so pass.\n";
*/
	bidState.SetBid(nBid);
	return TRUE;
}







//
//==================================================================
// construction & destruction
//
CWeakTwoBidsConvention::CWeakTwoBidsConvention() 
{
	// from ConvCodes.h
	m_nID = tidWeakTwoBids;
}

CWeakTwoBidsConvention::~CWeakTwoBidsConvention() 
{
}


