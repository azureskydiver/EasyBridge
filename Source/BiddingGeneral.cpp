//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////
//
// Bidgenl.cpp
//
// General routines for bidding
//
#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "PlayerStatusDialog.h"
#include "bidengine.h"
#include "BlackwoodConvention.h"
#include "GerberConvention.h"   // NCR-246a
#include "CueBidConvention.h"
#include "Convention.h"
#include "ConventionSet.h"




//
//---------------------------------------------------------------
//
// RespondToTripleRaise()
//
// Respond to a triple raise
//
int CBidEngine::RespondToTripleRaise()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;

	//
	// Rebidding after partner has made a triple raise
	// partner has shown < 9 HCPs
	//   pass with < 20 pts
	//
	
	//
	if (m_nAgreedSuit == NONE)
		m_nAgreedSuit = nPreviousSuit;

	//
	m_fPartnersMin = MAX(m_fPartnersMin, 6);
	m_fPartnersMax = MAX(m_fPartnersMax, 9);
	status << "2Q00! With a triple raise, partner is showing 5 trumps and " &
			  m_fPartnersMin & "-" & m_fPartnersMax & 
			  " HCPs, and expects us to sign off at game.\n";

	// get adjusted hand point count
	double fAdjPts = m_pHand->RevalueHand(REVALUE_DECLARER, m_nAgreedSuit, TRUE);
	m_fMinTPPoints = fAdjPts + m_fPartnersMin;
	m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
	m_fMinTPCPoints = fCardPts + m_fPartnersMin;
	m_fMaxTPCPoints = fCardPts + m_fPartnersMax;

	//
	// < 3 TPs:  pass
	//		
	if (m_fMinTPPoints < PTS_SLAM)
	{
		m_nBid = BID_PASS;
		status << "Q10! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
			" total partnership points, we have to agree and pass.\n";
		return ValidateBid(m_nBid);
	}

	//
	// with 3+ TPs & > 9 playing tricks, 
	// explore slam possibilities
	//
	if (nPartnersBid < BID_4NT)
	{
		InvokeBlackwood(m_nAgreedSuit);
	}
	else 
	{
		// oops, too high -- bid slam directly
		if ((m_fMinTPPoints >= PTS_GRAND_SLAM) && (nPartnersBidLevel <= 6))
		{
			m_nBid = MAKEBID(m_nAgreedSuit, 7);
			status << "Q20! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
				" total partnership points, we can bid a grand slam at " & 
				BTS(m_nBid) & ".\n";
		}
		else if (nPartnersBidLevel < 6)
		{
			m_nBid = MAKEBID(m_nAgreedSuit, 6);
			status << "Q21! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
				" total partnership points, we can bid a small slam at " & 
				BTS(m_nBid) & ".\n";
		}
		else
		{
			m_nBid = BID_PASS;
			status << "Q22! And with " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
				" total partnership points, we sign off on partner's " &
				szPB & " slam attempt.\n";
		}
	}
	return ValidateBid(m_nBid);
}






//
//---------------------------------------------------------------
//
// RespondToSingleRaise()
//
// Respond to a single raise
//
int CBidEngine::RespondToSingleRaise(int nPartnersBid)
{
	//
	// Rebidding after partner has made a single raise
	// if this is partner's first bid, it shows 6-10 pts
	//   with <= 21 total pts, pass
	//   with 22-24 total pts:
	//       with < 7 playing tricks, bid a new (support) suit
	//       with 7+ playing tricks, jump to game
	//   with 25+ total pts, jump to game or bid a new suit
	//
	CPlayerStatusDialog& status = *m_pStatusDlg;

	//
	if (m_nAgreedSuit == NONE)
		m_nAgreedSuit = nPreviousSuit;

	// set or adjust partner's point expectations
	// NCR-238 Freebid worth more than simple response. See also 229 and 222
	// NCR-238 use variables for values
	double minPts = 6;
	double maxPts = 10;
	// NCR-278 Is bidding over a double a free bid?
	if (((GetBidType(nPartnersBid) & BT_Overcall) != 0) && (nLHOBid != BID_DOUBLE) )
	{  // NCR set points for Freebid
		minPts = PTS_FREEBID;
		maxPts = 12;
		if(nOpponentsBidLevel >= 3) {   // NCR-757 More points at higher level
			minPts = 12;
			maxPts = 15;
		}
	}
	m_fPartnersMin = MAX(m_fPartnersMin, minPts);
	m_fPartnersMax = MAX(m_fPartnersMax, maxPts);
	status << "RDR0! With a single raise, partner is showing " & 
			  m_fPartnersMin & "-" & m_fPartnersMax & 
			  " points and decent (3-4 card) support for our " & STSS(m_nAgreedSuit) & " suit.\n";
	int nTricks = CountModifiedPlayingTricks(m_nAgreedSuit);

	// get adjusted hand point count as declarer
	int nSuit;
	// NCR NB: Following local variable hides member variable
	double fAdjPts = m_pHand->RevalueHand(REVALUE_DECLARER, m_nAgreedSuit, TRUE);
	m_fMinTPPoints = fAdjPts + m_fPartnersMin;
	m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
	m_fMinTPCPoints = fCardPts + m_fPartnersMin;
	m_fMaxTPCPoints = fCardPts + m_fPartnersMax;

	//
	// 18-22 pts:  pass
	//		
	if (m_fMinTPPoints <= PTS_GAME-4) 
	{
	    // NCR-63 If partner bid a new suit and we don't have points, make sure we have enough cards
		if(numCardsInSuit[nPartnersSuit] > 3) {
			m_nBid = BID_PASS;
			if (nPartnersBid > nOpponentsBid)
				status << "RDR2! We have a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points in the partnership, which is not enough for game, so settle for a part score and pass.\n";
			else
				status << "RDR2a! We have a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points in the partnership, which is not enough for a further raise, so pass.\n";
		}
		else if(numCardsInSuit[m_nAgreedSuit] > 4)  // NCR-64
		{
			// NCR-63 go back to our suit or ???
			m_nBid = GetCheapestShiftBid(m_nAgreedSuit);
			status << "R03! Don't like partner's suit (less than 4 cards), return to our suit with " &
				       BTS(m_nBid) & ".\n";
		}
		else 
		{
			// NCR-64 try Notrump
			StoppedCode sCode = ( theApp.GetBiddingAgressiveness() < 2) ? STOPPED_ALLOTHER : STOPPED_DONTCARE;
			if(BidNoTrumpAsAppropriate(FALSE, sCode, nPartnersSuit))
			{
				status << "R03A! With a total of " & 
						  m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
						  " HCPs in the partnership, we can bid " & BTS(m_nBid) & ".\n";
			}else{
				m_nBid = BID_PASS;
				status << "RDR2b! We have a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points in the partnership, which is not enough for game, so settle for a part score and pass.\n";
			}
		}
		return ValidateBid(m_nBid);
	}

	//
	// 23-25 pts:
	// with 23-25 pts and 6-7 playing tricks, bid a support suit
	// with 24-25 pts and 7+ playing tricks, bid 3 of the suit to invite game
	//		
	if ((m_fMinTPPoints >= PTS_GAME-4) && (m_fMinTPPoints < PTS_GAME)) 
	{
		//
		// first see if partner has already made a game-level bid
		// this could happen if partner raised in a later round
		//
		if (IsGameBid(nPartnersBid)) 
		{
			// sign off on the game bid
			m_nBid = BID_PASS;
			if (nPartnersBid > nOpponentsBid)
				status << "RDR4! With a game contract reached at " & szPB & ", we pass.\n";
			else
				status << "RDR4! With insufficient strength to raise partner any further, we have to pass.\n";
			return ValidateBid(m_nBid);
		}

		//
		// see if we're already at the 3 or 4 level -- if so, pass
		//
		if (nPartnersBidLevel >= 3)
		{
			// sign off on the game bid
			m_nBid = BID_PASS;
			status << "RDR5! With a total of only " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " points in the partnership, we have to pass partners's  " & 
					  BTS(nPartnersBid) & " bid.\n";
			return ValidateBid(m_nBid);
		}
		
		//
		// else we're at the 2-level
		//
		//  NCR-137 Ignore nTricks if enough max HCPs
		if ((nTricks < 6) && (m_fMaxTPCPoints < 23)) 
		{
			// pass with fewer than 6 playing tricks
			m_nBid = BID_PASS;
			status << "R08! Using the modified trick count, we have only " & nTricks & 
					  " playing tricks in hand and a total of " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points in the partnership, so pass.\n";
			return ValidateBid(m_nBid);

		} 
		else if (nTricks == 6) // < 7)  // NCR-496 test == 6 vs < 7 ???
		{
			// bid a support suit with 6 playing tricks and 22-24 pts
			// NCR-59 why bid a support suit? What should the suit have??? Is AT good enough?
			nSuit = GetNextBestSuit(m_nAgreedSuit);
			// NCR-200 pass if bidlevel >= 3 and nSuit <= current bid  // NCR-246 added = to <=
			if((nRHOBid != BID_PASS) && (BID_LEVEL(nRHOBid) >= 3) && (nSuit <= BID_SUIT(nRHOBid)) )
			{
				m_nBid = BID_PASS;
				status << "R12! Using the modified trick count, we have only " & nTricks & 
					  " playing tricks in hand and a total of " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points in the partnership, so pass.\n";
			}
			else
			{
				// NCR-103 This suit can be JUNK!!! eg C863 ???
				// NCR-235 Need to test if bid is a Reverse!
				if(TestReverseRules(nSuit)) // NCR-340 Ok to reverse?
				{
					m_nBid = GetCheapestShiftBid(nSuit);
					status << "R14! Using the modified trick count, we have " & nTricks &
					  " playing tricks in hand and a total of " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points in the partnership, so show the " & STSS(nSuit) &
					  " support suit in a bid of " & BTS(m_nBid) & ".\n";
				}
				else  // NCR-340 Pass if we can't reverse
				{
				   m_nBid = BID_PASS;
				   status << "R13! Using the modified trick count, we have only " & nTricks & 
					  " playing tricks in hand and a total of " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points in the partnership, so pass.\n";

				}
			}
			return ValidateBid(m_nBid);
		} 
		else if (ISMAJOR(m_nAgreedSuit)) 
		{
			// else we have 7+ tricks and 22-25 pts; invite to game at 3
			m_nBid = MAKEBID(m_nAgreedSuit,3);
			status << "R18! Using the modified trick count, we have " & nTricks &
					  " playing tricks in hand and a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  "points in the partnership, so invite game with a bid of " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}
		// NCR-339 Make some bid. Do NOT fall out of this block to following block
		else 
		{
			m_nBid = GetCheapestShiftBid(m_nAgreedSuit);
			status << "R18! Using the modified trick count, we have " & nTricks &
					  " playing tricks in hand and a total of " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points in the partnership, make a bid of " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		} // NCR-339 end making some bid
/*
		else if (nTricks < 7) 
		{

			// else w/ 6-7 playing tricks, look at point count
			// bid 3 with only 22 TPs
			if (m_fMinTPPoints == PTS_GAME-3) 
			{
				m_nBid = MAKEBID(m_nAgreedSuit,3);
				status << "R12! Using the modified trick count, we have " & nTricks & 
						  " playing tricks in hand and a total of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " points in the partnership, so make an invitation towards game with a bid of " &
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
			// else with 23-24 TPs, bid a support suit
			nSuit = GetNextBestSuit(m_nAgreedSuit);
			m_nBid = GetCheapestShiftBid(nSuit);
			status << "R14! Using the modified trick count, we have " & nTricks &
					  " playing tricks in hand and a total of " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points in the partnership, so show the " & STSS(nSuit) &
					  " support suit in a bid of " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		} 
		else if (ISMAJOR(m_nAgreedSuit)) 
		{
			// else we have 7+ tricks; jump to game in a major
			m_nBid = MAKEBID(m_nAgreedSuit,4);
			status << "R18! Using the modified trick count, we have " & nTricks &
					  " playing tricks in hand and a total of " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  "points in the partnership, so " &
					  ((nPartnersBidLevel == 2)? "jump to a game bid of " : "bid game at ") &
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}
*/
	} // end less than game points

	//
	// 26-32 pts:
	//  with a major suit, bid game
	//  with a minor, bid a support suit or game
	//		
	if ((m_fMinTPPoints >= PTS_GAME) && (m_fMinTPPoints < PTS_SLAM)) 
	{
		if (IsGameBid(nPartnersBid)) 
		{
			// sign off on the game bid
			m_nBid = BID_PASS;
			status << "R21! With a game contract reached at " & szPB & ", we pass.\n";
			return ValidateBid(m_nBid);
		}
		//
		if (ISMAJOR(m_nAgreedSuit)) 
		{
			// jump to game in a major suit
			m_nBid = MAKEBID(m_nAgreedSuit,4);
			status << "R22! We have a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points in the partnership, which should be enough for game, so " &
					  ((nPartnersBidLevel == 2)? "jump to " : "bid ") &
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		//
		//-----------------------------------------------
		// else we've got a minor suit
		//
	
		//
		// first see if we can play in notrumps (need 26+ HCPs)
		//
		if ((m_fMinTPCPoints >= PTS_NT_GAME) && (nPartnersBidLevel <= 3) &&
					(m_pHand->AllOtherSuitsStopped(m_nAgreedSuit))) 
		{
			// jump to 3NT
			m_nBid = BID_3NT;
			status << "R24! We have a total of approx. " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
				      " HCPs in the partnership and all unbid suits stopped, we can bid game at " &
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		// otherwise try for a game in minors; 
		// show a support suit with < 29 TPs
		if (m_fMinTPPoints < PTS_MINOR_GAME)
		{
			// find a support suit, if appropriate
			if (nPartnersBidLevel < 3)
			{
				nSuit = GetNextBestSuit(m_nAgreedSuit);
				m_nBid = GetCheapestShiftBid(nSuit);
				status << "R26! We have a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " points in the partnership for a minor suit, so show the " & 
						  STSS(nSuit) & " support suit in a bid of " & BTS(m_nBid) & ".\n";
			}
			else
			{
				// else pass
				m_nBid = BID_PASS;
				status << "R27! We have a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " points in the partnership, which is not quite enough for game in a minor suit, so pass.\n";
			}
			return ValidateBid(m_nBid);
		}

		// here, we've got a minor with 28-31 TPs, so jump to game
		m_nBid = MAKEBID(m_nAgreedSuit,5);
		status << "R30! We have a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
				  " points in the partnership for a minor suit, so " &
				  ((nPartnersBidLevel <= 3)? "jump to a game bid of " : "bid game at ") &
				  BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);

	}

	//
	// 3+ TPs:  try for slam
	//		
	//
	status << "R40! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
			  " total points in the partnership, we want to explore slam possibilities.\n";

	//
	if (numAces == 4) 
	{
		// the Blackwood code will bid a slam directly when we have
		// all four aces
		InvokeBlackwood(m_nAgreedSuit);
		return ValidateBid(m_nBid);
	} 
	else if (pCurrConvSet->IsConventionEnabled(tidCueBids) && TryCueBid()) // NCR-331 Test if using CueBids
	{
		//
		return ValidateBid(m_nBid);
	} 
	else 
	{
		// cue bids not available, so make direct slam try
		m_nBid = MAKEBID(m_nAgreedSuit,5);
		status << "R60! But since we cannot make a cue bid, make a slam invitation at " &
				  BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);
	}

}






//
//---------------------------------------------------------------
//
// RespondToDoubleRaise()
//
// Respond to a double raise
//
int CBidEngine::RespondToDoubleRaise(int nPartnersBid)
{
	//
	//==========================================================
	//
	// Rebidding after partner has made a double raise
	// in general, partner has shown 11-12 pts if playing limit raises 
	// or 13-16 pts if not
	//   with 26-30 min total pts, bid 3NT or 4 of the suit
	//   with 31+ min total points, move towards slam
	//
	CPlayerStatusDialog& status = *m_pStatusDlg;

	//
	if (m_nAgreedSuit == NONE)
		m_nAgreedSuit = nPreviousSuit;
	
	//
	if (PlayingConvention(tidLimitRaises))
	{
		// set or adjust partner's point expectations
		m_fPartnersMin = MAX(m_fPartnersMin, 10);  // NCR changed limit raise lower from 11 to 10
		m_fPartnersMax = MAX(m_fPartnersMax, 12);
		status << "2S00! With a double raise and playing limit raises, partner is showing "& 
				  m_fPartnersMin & "-" & m_fPartnersMax & 
				  " points and 4+ card support for our " & STSS(m_nAgreedSuit) & " suit.\n";
	} 
	else 
	{
		m_fPartnersMin = MAX(m_fPartnersMin, 13);
		m_fPartnersMax = MAX(m_fPartnersMax, 18);
		// if partner passed previously, adjust partner's max down
		if (nPartnersPrevBid == BID_PASS)
		{
			m_fPartnersMax = 13;
			status << "2S01a! With a double raise after passing previously, partner is showing approx. " &
					  m_fPartnersMin & " points and good 4-card support for our " & STSS(m_nAgreedSuit) & " suit.\n";
		}
		else
		{
			status << "2S01b! With a double raise, partner is showing " &
					  m_fPartnersMin & "-" & m_fPartnersMax	& 
					  " points and good 4-card support for our " & STSS(m_nAgreedSuit) &
					  " suit" & ((nPartnersBid < GetGameBid(m_nAgreedSuit))? ".  The bid is forcing to game." : ".") & "\n";
		}
	}

	// get adjusted hand point count as declarer
	int nSuit;
	double fAdjPts = m_pHand->RevalueHand(REVALUE_DECLARER, m_nAgreedSuit, TRUE);
	m_fMinTPPoints = fAdjPts + m_fPartnersMin;
	m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
	m_fMinTPCPoints = fCardPts + m_fPartnersMin;
	m_fMaxTPCPoints = fCardPts + m_fPartnersMax;


	//
	// 24-30 total min pts:  move towards game
	//		
	if (m_fMinTPPoints <= PTS_MINOR_GAME+1) 
	{
		//
		// first see if partner has already made a game-level bid
		// this could happen if partner raised in a later round
		//
		if (IsGameBid(nPartnersBid)) 
		{
			if (nPartnersBidLevel < 6)
			{
				// sign off on the game bid
				m_nBid = BID_PASS;
				if (nPartnersBid > nOpponentsBid)
					status << "S01! With a game contract reached at " & szPB & ", we pass.\n";
				else
					status << "S01a! We can't raise partner's bid beyond game, so pass.\n";
			}
			else
			{
				// sign off on a slam bid
				m_nBid = BID_PASS;
				if (nPartnersBid > nOpponentsBid)
					status << "S02! With a slam contract reached at " & szPB & ", we pass.\n";
				else
					status << "S02a! We can't raise partner's bid beyond game, so pass.\n";
			}
			return ValidateBid(m_nBid);
		}
		else
		{
			//
			if (ISMAJOR(m_nAgreedSuit)) 
			{
				// bid at the 3-level or straight to game
				if ((m_fMinTPPoints >= PTS_GAME) && (nPartnersBidLevel < 4))
				{
					m_nBid = MAKEBID(m_nAgreedSuit,4);
					status << "S10! We have " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
							  " total points in the partnership, so bid" &
							  ((m_fMinTPPoints >= PTS_GAME)? " game at " :  " ") & BTS(m_nBid) & ".\n";
				}
				else
				{
					m_nBid = BID_PASS;
					status << "S11! We have " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
							  " total points in the partnership, which is insufficient for a higher contract than the " &
							  szPB & " bid by partner, so pass.\n";
				}
				return ValidateBid(m_nBid);
			}

			// else we have a minor, so try to steer towards NT
			// need zero voids and singletons, 26 HCPs,
			// & all suits stopped
			if ((numVoids == 0) && (numSingletons == 0) &&
						(m_fMinTPCPoints >= PTS_NT_GAME) && (nPartnersBidLevel <= 3) &&
						m_pHand->AllOtherSuitsStopped(m_nAgreedSuit)) 
			{
				m_nBid = BID_3NT;
				status << "S14! With approx. " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
						  " HCPs in the partnership and all unbid suits stopped, bid " &
						  BTS(m_nBid) & " over the raised minor " & STSS(m_nAgreedSuit) & " suit.\n";
				return ValidateBid(m_nBid);
			}

			// with a minor and < 26 pts, pass
			if (m_fMinTPPoints < PTS_MINOR_GAME-3) 
			{
				// find a support suit
				m_nBid = BID_PASS;
				status << "S16! We have only " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " total points in the partnership for a minor suit, so pass.\n";
				return ValidateBid(m_nBid);
			}

			// else with 26-28 pts, steer towards a minor game
			if (m_fMinTPPoints < PT_COUNT(PTS_MINOR_GAME))  // NCR added PT_COUNT 
			{
				// bid a support suit if possible
				if (nPartnersBidLevel < 4)
				{
					// find the support suit
					nSuit = GetNextBestSuit(m_nAgreedSuit);
					m_nBid = GetCheapestShiftBid(nSuit);
					status << "S18! We have " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
							  " total points in the partnership for a minor suit, so show the " &
							  STSS(nSuit) & " support suit in a bid of " & BTS(m_nBid) & ".\n";
				}
				else
				{
					status << "S18a! We have " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
							  " total points in the partnership for a minor suit, so we'd like to show a support suit and push towards game, but we're already at too high a level, so we have to pass.\n";
					m_nBid = BID_PASS;
				}
				return ValidateBid(m_nBid);
			}

			// here, we have a minor suit with 29-30 total pts;
			// try for a game at the 5-level
			if (m_fMinTPPoints >= PT_COUNT(PTS_MINOR_GAME))  // NCR added PT_COUNT
			{
				if (nPartnersBidLevel <= 4)
				{
					m_nBid = MAKEBID(m_nAgreedSuit,5);
					status << "S22! We have " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
							  " total points in the partnership for a minor suit, so " &
	  						  ((nPartnersBidLevel <= 3)? "jump to a game bid of " : "bid game at ") &
							  BTS(m_nBid) & ".\n";
				}
				else
				{
					m_nBid = BID_PASS;
					status << "S23! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
							  " total points in the partnership, accept partner's " & szPB &
							  " bid and pass.\n";
				}
				return ValidateBid(m_nBid);
			}
		}
	}

	//
	// Else we have approx. 30+ total pts:  show a cue bid, nudging 
	// towards slam
	//		

	//
	status << "S40! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
			  " total points in the partnership, we want to explore slam possibilities.\n";


	// check how many outside aces we have
	int numOutsideAces = 0;
	for(int i=0;i<4;i++)
	{
		if ((i != m_nAgreedSuit) && m_pHand->SuitHasAce(i))
			numOutsideAces++;
	}

	// use Blackwood if we have either all 4 Aces or no aces outside the trump suit
	// (can't use cue buds if we have no outside aces)
	if ((numAces == 4) || (numOutsideAces == 0))
	{
		// the Blackwood code will bid slam directly when we hold
		// all four aces
		InvokeBlackwood(m_nAgreedSuit);
		return ValidateBid(m_nBid);
	} 
	else if (pCurrConvSet->IsConventionEnabled(tidCueBids) && TryCueBid())  // NCR-331 Test if using Cue Bids
	{
		//
		return ValidateBid(m_nBid);
	} 
	else 
	{
		// cue bids not available, so make direct slam try
		m_nBid = MAKEBID(m_nAgreedSuit,5);
		status << "S60! But since we cannot make a cue bid, make a slam invitation at " &
			      BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);
	}

}







//
//---------------------------------------------------------------
//
// RebidSuit()
//
// Rebid our last bid suit if it meets the requirements
//
// Parameters
//   nSuitType:  suit type to be rebid
//      SUITTYPE_ANY:  rebid any suit, SUITTYPE_MAJOR=major only, etc.
//   nShiftLevel:  level the suit is to be rebid at
//                if 0, rebid is at the cheapest level;
//                if > 0, jump n levels; 
//                if < 0, jump to absolute level
//   fMinPts, fMaxPts:  min & max points in the partnership
//					   if negative, pts in the player's hand			
//   nLength:  min # of cards
//   nStrength(optional):  strength of suit (SS_*) 
//   numHonors(optional):  # of honors in the suit
//   numHonorPts(optional):  # of honor points in the suit
//	 
BOOL CBidEngine::RebidSuit(int nSuitType,  RebidLevel enShiftLevel, 
						   double fMinPts, double fMaxPts, SuitLength enLength, 
					       int nStrength, HonorsCount enumHonors, double numHonorPts)
{
	CPlayerStatusDialog& status = *m_pStatusDlg;

	//
	int nShiftLevel = (int) enShiftLevel;
	int nLength = (int) enLength;
	int numHonors = (int) enumHonors;

	//
//	ASSERT((nSuitType >= SUIT_PREFERRED) && (nSuitType <= SUIT_PREV_MINOR));
	ASSERT(nLength <= 13);
	ASSERT(nShiftLevel < 6);
	if (fMaxPts == 0) fMaxPts = 99;
	//
	int nPartnersBid = m_pPartner->InquireLastBid();
	int nPartnersSuit = (nPartnersBid-1) % 5;

	// check suit type requirement
	int nSuit;
	int nPreviousBid = m_nBid;
	if (nSuitType == SUIT_PREFERRED)
	{
		nSuit = nPrefSuit;
	}
	else if (nSuitType <= SUIT_DIRECT)
	{
		nSuit = -(nSuitType + 10);
	}
	else
	{
		// get the last bid suit
		nSuit = nPreviousSuit;
		if (nSuit == NOTRUMP)
			nSuit = nFirstRoundSuit;
		// NCR-66 What is nFirstRoundSuit if first bid was NT ???
		if(!ISSUIT(nSuit))  // NCR-66 test valid suit
			return FALSE;   // and fail if not
		if ((nSuitType == SUIT_MAJOR) && (!ISMAJOR(nSuit)))
			return FALSE;
		if ((nSuitType == SUIT_MINOR) && (!ISMINOR(nSuit)))
			return FALSE;
	}
	// check point requirement
	if (fMinPts >= 0) 
	{
		if (fMaxPts == 0)
			fMaxPts = 99;
		if ((m_fMinTPPoints < fMinPts) || (m_fMinTPPoints > fMaxPts))
			return FALSE;
	} 
	else 
	{
		if (fMaxPts == 0)
			fMaxPts = -99;
		if ((m_pHand->GetTotalPoints() < -fMinPts) || (m_pHand->GetTotalPoints() > -fMaxPts))
			return FALSE;
	}

	// check special requirements
	BOOL bSolidRqmt = nStrength & SS_SOLID;
	BOOL bSelfSupportRqmt = nStrength & SS_SELFSUPPORTING;
	nStrength &= ~(SS_SOLID | SS_SELFSUPPORTING);

	//
	// perform basic acid tests
	//
	if ((numCardsInSuit[nSuit] < nLength) ||
					(nSuitStrength[nSuit] < nStrength) ||
					(numHonorsInSuit[nSuit] < numHonors) ||
					(numSuitPoints[nSuit] < numHonorPts)) 
		return FALSE;

	// see if the suit needs to be self-supporting
	if ((bSelfSupportRqmt) && (!m_pHand->IsSuitSelfSupporting(nSuit)))
		return FALSE;
	// see if the suit needs to be solid
	if ((bSolidRqmt) && (!m_pHand->IsSuitSolid(nSuit)))
		return FALSE;

	//
	// at this point, we passed all the tests
	//
	int nBid;
	nPartnersBid = m_pPartner->InquireLastBid();
	if (nShiftLevel == 0) 
		nBid = GetCheapestShiftBid(nSuit);
	else if (nShiftLevel > 0)
		nBid = GetJumpShiftBid(nSuit,nPartnersBid,nShiftLevel);
	else
		nBid = MAKEBID(nSuit,-nShiftLevel);

	// NCR-573 Make sure were not bidding 4C with Gerber
	if ((nBid == BID_4C) && (nPartnersBid == BID_2NT)
		&& (pCurrConvSet->IsConventionEnabled(tidGerber)) )
		return FALSE;

	// see if bid is legal
	if (nBid <= pDOC->GetLastValidBid())
		return FALSE;
	else
		m_nBid = nBid;

	//
	if (nPartnersSuit == nSuit) 
		status << "YR1! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints & " total points and a " &
				  SSTS(nSuit) & " " & numCardsInSuit[nSuit] & "-card " & STSS(nSuit) & 
				  " suit (holding " & SHTS(nSuit) & "), re-raise it to " & BTS(m_nBid)  & ".\n";
	else
		status << "YR2! With With " & m_fMinTPPoints & "-" & m_fMaxTPPoints & " total points and a " &
				  SSTS(nSuit) & " " & numCardsInSuit[nSuit] & "-card " & STSS(nSuit) &
				  " suit (holding " & SHTS(nSuit) & "), " & 
				  ((nShiftLevel > 0)? "jump rebid" :  "rebid") &
				  " it at " & BTS(m_nBid) & ".\n";

	// no suit agreement
	m_nAgreedSuit = NONE;
	return TRUE;
}






//
//---------------------------------------------------------------
//
// BidNextBestSuit()
//
// Bid the next best suit after the ones listed
//
// Parameters
//   nSuitType:  suit type to be rebid
//   nShiftLevel:  level the suit is to be jump shifted, if >= 0
//                absolute level, if < 0
//   fMinPts, fMaxPts:  min & max points in the partnership
//					   if negative, pts in the player's hand			
//   nLength:  min # of cards
//   nStrength(optional):  strength of suit (BS_*) 
//   numHonors(optional):  # of honors in the suit
//   numHonorPts(optional):  # of honor points in the suit
//   nSuit1, nSuit2, nSuit3:  the suits NOT to rebid
//	 
BOOL CBidEngine::BidNextBestSuit(int nSuitType, RebidLevel enShiftLevel, 
								   double fMinPts, double fMaxPts, SuitLength enLength,  
								   int nStrength, HonorsCount enumHonors, double numHonorPts,
						 		   int nSuit1, int nSuit2, int nSuit3)
{
	CPlayerStatusDialog& status = *m_pStatusDlg;

	//
	int nShiftLevel = (int) enShiftLevel;
	int nLength = (int) enLength;
	int numHonors = (int) enumHonors;

	//
	ASSERT((nSuitType >= SUIT_ANY) && (nSuitType <= SUIT_MAJOR));
	ASSERT(nLength <= 13);
	ASSERT(nShiftLevel < 6);
	if (fMaxPts == 0) fMaxPts = 99;
	//
	int nPreviousBid = m_nBid;
	int nPreviousSuit = (nPreviousBid-1) % 5;
	int nSuit;
	//
	if (nSuit3 > NONE) 
	{
		// 3 suits were specified, get 4th one
		nSuit = GetFourthSuit(nSuit1,nSuit2,nSuit3);
	} 
	else if (nSuit2 > NONE) 
	{
		// 2 suits were specified, get third one
		nSuit = GetNextBestSuit(nSuit1,nSuit2);
	} 
	else 
	{
		// 1 or zero suits were specified (left empty)
		// so get our and partner's suits
		nSuit1 = nPreviousSuit;
		nSuit2 = nPartnersSuit;
		if (nSuit2 == NOTRUMP)
			nSuit2 = nPartnersPrevSuit;
		nSuit = GetNextBestSuit(nSuit1,nSuit2);
		// NCR-539 Problem if two suited hand and third suit is singleton
		// Require at least 4 cards in suit for jump bid
		if((numCardsInSuit[nSuit] < MAX(4, nLength)) && (nShiftLevel > 0)) {
			nSuit = GetNextBestSuit(nSuit1); // NCR-539 try again
		}
	}
	//
	int nPartnersBid = m_pPartner->InquireLastBid();
	// check point requirement
	if (fMinPts >= 0) 
	{
		if (fMaxPts == 0)
			fMaxPts = 99;
		if ((m_fMinTPPoints < fMinPts) ||
							(m_fMinTPPoints > fMaxPts))
			return FALSE;
	} 
	else 
	{
		if (fMaxPts == 0)
			fMaxPts = -99;
		if ((m_pHand->GetTotalPoints() < -fMinPts) ||
							(m_pHand->GetTotalPoints() > -fMaxPts))
			return FALSE;
	}
	//
	if ((numCardsInSuit[nSuit] >= nLength) &&
					(nSuitStrength[nSuit] >= nStrength) &&
					(numHonorsInSuit[nSuit] >= numHonors) &&
					(numSuitPoints[nSuit] >= numHonorPts)) 
	{
		//
		int nTestBid = NONE; // NCR-589 use this vs m_nBid
		if (nShiftLevel == 0)
			nTestBid = GetCheapestShiftBid(nSuit);
		else if (nShiftLevel > 0)
			nTestBid = GetJumpShiftBid(nSuit,nPartnersBid,nShiftLevel);
		else
			nTestBid = MAKEBID(nSuit,-nShiftLevel);

		if (!IsBidSafe(nTestBid))
			return FALSE;

		// else all's OK
		status << "YR4! Rebid the" & ((nPreviousSuit == NOTRUMP)? " " :  " next ") &
				  "best suit of " & STS(nSuit) & " in a " &
				  ((nShiftLevel > 0)? "jump bid":  "bid") &
				  " of " & BTS(nTestBid) & ".\n";
		m_nBid = nTestBid; // NCR-589 Save the bid
		// mark lack of suit agreement
		m_nAgreedSuit = NONE;
		return TRUE;
	}
	// else failed
	return FALSE;
}





//
//---------------------------------------------------------------
//
// RaisePartnersSuit()
//
// Raise partner's last bid suit if it meets the criteria
//
// Parameters
//   nSuitType:  suit type to be rebid
//   nLevel:  raise level (>0:  relative; <0:  absolute)
//   fMinPts, fMaxPts:  min & max points in the partnership
//					   if negative, pts in the player's hand			
//   nLength:  min # of support cards
//   nStrength(optional):  strength of suit (BS_*) 
//   numHonors(optional):  # of honors in the suit
//   numHonorPts(optional):  # of honor points in the suit
//	 
BOOL CBidEngine::RaisePartnersSuit(int nSuitType, RaiseLevel enLevel,
	 							 double fMinPts, double fMaxPts,
					             SupportLength enLength, int nStrength, 
					             HonorsCount eNumHonors, double numHonorPts)
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	
	//
	int nLevel = (int) enLevel;
	int nLength = (int) enLength;
	int numHonors = (int) eNumHonors;

	//
	ASSERT((nSuitType >= SUIT_ANY) && (nSuitType <= SUIT_PREV_MINOR));
//	ASSERT((nLevel >= -7) && (nLevel <= 7) && (nLevel != 0));
	ASSERT(nLength <= 13);
	if (fMaxPts == 0) fMaxPts = 99;

	//
	int nTargetSuit;
	int nPartBidLevel;

	// check partner's last bid suit, if appropriate
	if (nSuitType == SUIT_PREV) 
	{
		// get previously bid suit
		nTargetSuit = nPartnersPrevSuit;
		nPartBidLevel = nPartnersPrevBidLevel;
	} 
	else if (nSuitType == SUIT_PREV_MAJOR) 
	{
		// get previously bid suit, but only if it's a major
		nTargetSuit = nPartnersPrevSuit;
		if (!ISMAJOR(nTargetSuit))
			return FALSE;
		nPartBidLevel = nPartnersPrevBidLevel;
	} 
	else if (nSuitType == SUIT_PREV_MINOR) 
	{
		// get previously bid suit, but only if it's a minor
		nTargetSuit = nPartnersPrevSuit;
		if (!ISMINOR(nTargetSuit))
			return FALSE;
		nPartBidLevel = nPartnersPrevBidLevel;
	} 
	else 
	{
		// get partner's most recently bid suit
		if (nPartnersSuit != NOTRUMP) 
		{
			nTargetSuit = nPartnersSuit;
			nPartBidLevel = nPartnersBidLevel;
		}
		else
		{
			// unless he bid NoTrumps, in which case 
			// we get the suit bid before the No Trump bid
			nTargetSuit = nPartnersPrevSuit;
			nPartBidLevel = nPartnersPrevBidLevel;
		}
	}

	//
	if ((nSuitType == SUIT_MAJOR) && (!ISMAJOR(nTargetSuit)))
		return FALSE;
	if ((nSuitType == SUIT_MINOR) && (!ISMINOR(nTargetSuit)))
		return FALSE;
	if (nTargetSuit == NOTRUMP)
		return FALSE;

	// found an eligible suit!

	//
	// first check the point count requirement
	//
	if (fMinPts >= 0) 
	{
		if (fMaxPts == 0)
			fMaxPts = 99;
		if ((m_fMinTPPoints < fMinPts) ||
							(m_fMinTPPoints > fMaxPts))
			return FALSE;
	} 
	else 
	{
		if (fMaxPts == 0)
			fMaxPts = -99;
		if ((m_pHand->GetTotalPoints() < -fMinPts) ||
							(m_pHand->GetTotalPoints() > -fMaxPts))
			return FALSE;
	}

	// then check other requirements
	if ((numCardsInSuit[nTargetSuit] < nLength) ||
					(nSuitStrength[nTargetSuit] < nStrength) ||
					(numHonorsInSuit[nTargetSuit] < numHonors) ||
					(numSuitPoints[nTargetSuit] < numHonorPts)) 
		return FALSE;

	//
	//--------------------------------------------------------------
	// here we've passed all the requirements, 
	// so formulate the bid
	//

	// check the level
	int newLevel;
	if ((nLevel >= 1) && (nLevel <= 6))
	{
		// relative raise -- raise partner's buid by N level(s)
		newLevel = nPartBidLevel + nLevel;
		if (newLevel <= 7)
			m_nBid = MAKEBID(nTargetSuit, newLevel);
		else
			return FALSE;
	}
	else if (nLevel == RAISE_TO_GAME)
	{
		// raise partner's bid to game
		if (ISMINOR(nTargetSuit))
		{
			if (nPartBidLevel < 5)
				m_nBid = MAKEBID(nTargetSuit, 5);
			else
				return FALSE;
		}
		else if (ISMAJOR(nTargetSuit))
		{
			if (nPartBidLevel < 4)
				m_nBid = MAKEBID(nTargetSuit, 4);
			else
				return FALSE;
		}
		else
		{
			// notrumps
			if (nPartBidLevel < 3)
				m_nBid = MAKEBID(nTargetSuit, 3);
			else
				return FALSE;
		}
	}	
	else if (nLevel == RAISE_TO_NO_MORE_THAN_GAME)
	{
		// raise partner's bid by 1, but to no more than game
		newLevel = nPartBidLevel + 1;
		if ((ISMINOR(nTargetSuit)) && (newLevel <= 5))
			m_nBid = MAKEBID(nTargetSuit, newLevel);
		else if ((ISMAJOR(nTargetSuit)) && (newLevel <= 4))
			m_nBid = MAKEBID(nTargetSuit, newLevel);
		else if ((ISNOTRUMP(nTargetSuit)) && (newLevel <= 3))
			m_nBid = MAKEBID(nTargetSuit, newLevel);
		else
			return FALSE;
	}
	else if ((nLevel <= -2) && (nLevel >= -7))
	{
		// direct raise to a specific level
		newLevel = -nLevel;
		if (newLevel > nPartBidLevel)
			m_nBid = MAKEBID(nTargetSuit, newLevel);
		else
			return FALSE;
	}
	else
	{
		// illegal specification!!!
		return FALSE;
	}

	// check if the bid would be too low
	if (BID_LEVEL(m_nBid) <= nPartnersBidLevel)
		return FALSE;

	//
	// the bid has now been determined, so provide feedback
	//
	if (fMinPts < 0)
	{
		// counting pts in the player's hand only
		status << "YN0! With " & fCardPts & "/" & fPts & "/" & fAdjPts & 
				  " points and " & numCardsInSuit[nTargetSuit] & 
				  "-card support for partner's" & 
				  ((nSuitType == SUIT_PREV)? " earlier " : " ") &
				  ((ISMAJOR(nTargetSuit))? "major " :  " ") & STSS(nTargetSuit) &
				  " suit (holding " & SHTS(nTargetSuit) & "), " & 
				  ((nLevel <= 1)? "raise" :  "jump raise") & " to " & BTS(m_nBid) & ".\n";
	}
	else
	{
		// counting total pts in the partnership
		status << "YN1! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
				  " points in the partnership and " & numCardsInSuit[nTargetSuit] & 
				  "-card support for partner's" &
				  ((nSuitType == SUIT_PREV)? " earlier " : " ") &
				  ((ISMAJOR(nTargetSuit))? "major " :  " ") & STSS(nTargetSuit) &
				  " suit (holding " & SHTS(nTargetSuit) & "), " & 
				  ((nLevel <= 1)? "raise" :  "jump raise") & " to " & BTS(m_nBid) & ".\n";
	}
	// done
	m_nAgreedSuit = nTargetSuit;
	return TRUE;
}

//
//--------------------------------------------------------------------------
//
// TestIfOpponentsBidThis()  NCR-513
//
// Returns true if the suit was bid by opponents
//         false if the suit was NOT bid by opponents
//
bool CBidEngine::TestIfOpponentsBidThis(int nSuit)
{
	int i, nBid;
	for(i=0; i < m_pRHOpponent->GetNumBidsMade(); i++) {
		nBid = pDOC->GetBidByPlayer(m_pRHOpponent->GetPosition(), i);
		if(BID_SUIT(nBid) == nSuit)
			return true;
	}
	// Now look at LHO
	for(i=0; i < m_pLHOpponent->GetNumBidsMade(); i++) {
		nBid = pDOC->GetBidByPlayer(m_pLHOpponent->GetPosition(), i);
		if(BID_SUIT(nBid) == nSuit)
			return true;
	}
	return false; // not bid by opponents
} // end NCR-513




//
//---------------------------------------------------------------
//
// BidNoTrump()
//
// Bid No Trump at the appropriate level if the suits
// specified are stopped
//
// Parameters
//   nLevel:  bid level
//   fMinPts, fMaxPts:  min & max points in the partnership
//					   if negative, pts in the player's hand			
//   bMustBeBalanced:  hand must be balanced
//   nCode:  suits stopped code
//       STOPPED_DONTCARE:  No suits need be stopped
//       STOPPED_ALL: 	    All four suits must be stopped
//       STOPPED_ALLOTHER:  All suits must be stopped other 
//                         than the one specified (presumably
//                         partner has a stopper there)
//   nSuit:  the suit that need not be stopped
//	 
BOOL CBidEngine::BidNoTrump(int nLevel, double fMinPts, double fMaxPts,
						    bool bMustBeBalanced, StoppedCode enCode, int nSuit, bool bMustHaveNoVoids)
{
	CPlayerStatusDialog& status = *m_pStatusDlg;

	//
	int nCode = (int) enCode;

	//
	ASSERT((nLevel > 0) && (nLevel <= 7));
	if (fMaxPts == 0) fMaxPts = 99;

	// check if this bid would be lower than partner's
	if (MAKEBID(NOTRUMP, nLevel) < nPartnersBid)
		return FALSE;

	// check balanced-ness reqirement
//	if ((bMustBeBalanced) && (!m_pHand->IsBalanced()))
	if ((bMustBeBalanced) && (!m_pHand->IsSemiBalanced()))
		return FALSE;
	// check stoppers
	if (nCode == STOPPED_ALL) 
	{
		if (numSuitsStopped < 4)
			return FALSE;
	} 
	else if (nCode == STOPPED_ALLOTHER) 
	{
		if (nSuit == NONE)
			nSuit = (m_pPartner->InquireLastBid()-1) % 5; 
		if (!m_pHand->AllOtherSuitsStopped(nSuit))
			return FALSE;
	} 
	else if (nCode == STOPPED_ONE) 
	{
		if (!m_pHand->IsSuitStopped(nSuit))
			return FALSE;
	}
	else if (nCode == STOPPED_UNBID) 
	{
		// all unbid suits must be stopped
		BOOL bMustBeStopped[4] = { TRUE, TRUE, TRUE, TRUE };
		// NCR-179 Don't use our previous bid as proof suit is stopped!!
/*  	// NCR-179 remove following 4 lines 
//		if (ISSUIT(nPreviousSuit))
//			bMustBeStopped[nPreviousSuit] = FALSE;
//		if (ISSUIT(nFirstRoundSuit))
//			bMustBeStopped[nFirstRoundSuit] = FALSE; 
*/ 
		if (ISSUIT(nPartnersSuit))
			bMustBeStopped[nPartnersSuit] = FALSE;
		if (ISSUIT(nPartnersPrevSuit))
			bMustBeStopped[nPartnersPrevSuit] = FALSE;
		for(int i=0;i<4;i++)
			if ((bMustBeStopped[i]) &&
				(!m_pHand->IsSuitStopped(i)))
				return FALSE;
	}
	else if(nCode == STOPPED_OPPBID)
	{
		// NCR-270 Need to see ALL of opponents' bids
		// NCR-270 First test RHO's bids
		int i, nBid;
		for(i=0; i < m_pRHOpponent->GetNumBidsMade(); i++) {
			nBid = pDOC->GetBidByPlayer(m_pRHOpponent->GetPosition(), i);
			Suit oppSuit = (Suit)BID_SUIT(nBid);
			if(ISSUIT(oppSuit) && !m_pHand->IsSuitStopped(oppSuit))
				return FALSE;
		}
		// NCR-270 Now look at LHO
		for(i=0; i < m_pLHOpponent->GetNumBidsMade(); i++) {
			nBid = pDOC->GetBidByPlayer(m_pLHOpponent->GetPosition(), i);
			Suit oppSuit = (Suit)BID_SUIT(nBid);
			if(ISSUIT(oppSuit) && !m_pHand->IsSuitStopped(oppSuit))
				return FALSE;
		}
	}
	else if(nCode == STOPPED_DONTCARE) {
		// NCR ignore this one

	}	else {
		ASSERT(false);  // NCR should never get here
		return FALSE; // NCR???
	}

	// can't have any void suits!
	if ((bMustHaveNoVoids) && (numVoids > 0))
		return FALSE;

	// check if the bid would be too low
	if (MAKEBID(NOTRUMP, nLevel) <= nPartnersBid)
		return FALSE;

	//
	// check point requirement
	//
	if (fMinPts >= 0) 
	{
		if (fMaxPts == 0)
			fMaxPts = 99;
		if ((m_fMinTPCPoints < fMinPts) ||
							(m_fMinTPCPoints > fMaxPts))
			return FALSE;
	} 
	else 
	{
		if (fMaxPts == 0)
			fMaxPts = -99;
		if ((m_pHand->GetHCPoints() < -fMinPts) ||
							(m_pHand->GetHCPoints() > -fMaxPts))
			return FALSE;
	}
	// else passed the hurdles
	m_nBid = MAKEBID(NOTRUMP,nLevel);
	if (fMinPts < 0)
	{
		// counting pts in the player's hand only
		status << "BN0! With " & fCardPts & " HCPs" &
				   ((numVoids == 0)? " and a reasonably balanced hand, " : ", ") & 
				   ((numSuitsStopped == 4)? "and all suits stopped," : 
				   (nCode == STOPPED_UNBID)? "and all unbid suits stopped," :
				   "though without stoppers in all suits,") & 
				   " bid " & BTS(m_nBid) & ".\n";
	}
	else
	{
		// counting total pts in the partnership
		status << "BN1! With " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints &  // NCR added Max
				  " HCPs in the partnership" &
				   ((numVoids == 0)? " and a reasonably balanced hand, " : ", ") & 
				  ((numSuitsStopped == 4)? "and all suits stopped," : 
				   (nCode == STOPPED_UNBID)? "and all unbid suits stopped," : 
				   "though without stoppers in all suits,") & 
				   " bid " & BTS(m_nBid) & ".\n";
	}
	return TRUE;
}




//
// BidNoTrumpAsAppropriate()
//
// - bids NT at the appropriate level
//
// bMustBeBalanced = Balanced hand requirement flag
// nCode = suit stoppage code
// nSuit = goes with above
//
BOOL CBidEngine::BidNoTrumpAsAppropriate(bool bMustBeBalanced, StoppedCode enCode, int nSuit)
{
	// bid 2NT with 23-25 HCPs
	if (BidNoTrump(2,PTS_NT_GAME-3,PTS_NT_GAME-1,bMustBeBalanced,enCode,nSuit))
		return TRUE;
	// or 3NT with 26-31 HCPs
	if (BidNoTrump(3,PTS_NT_GAME,PTS_SLAM-1,bMustBeBalanced,enCode,nSuit))
		return TRUE;
	// a bid of 4NT is invitational to a small slam
	if (BidNoTrump(4,PTS_SLAM-2,PTS_SLAM-1,bMustBeBalanced,enCode,nSuit))
		return TRUE;
	// bid 6NT directly with with 33-34 pts
	if (BidNoTrump(6,PTS_SLAM,PTS_SLAM+1,bMustBeBalanced,enCode,nSuit))
		return TRUE;
	// a bid of 5NT is invitational to a grand slam
	if (BidNoTrump(5,PTS_GRAND_SLAM-2,PTS_GRAND_SLAM-1,bMustBeBalanced,enCode,nSuit))
		return TRUE;
	// bid 7NT directly with 37+ pts
	if (BidNoTrump(7,PTS_GRAND_SLAM,0,bMustBeBalanced,enCode,nSuit))
		return TRUE;
	//
	return FALSE;
}





//
// PickBestFinalSuit()
//
// selects the best suit for a final response from among 
// our own suit, partner's two suits, and No Trumps.
// typically called after three or four suits have been shown, and 
// there is still uncertainty over which suit to pick
//
int CBidEngine::PickBestFinalSuit(CPlayerStatusDialog& status)
{
	int nSuit,newSuit,numSuitsBid = 0;
	// grab a new suit (typically 3rd or 4th in the bidding)
	BOOL bSuitAvailable[4] = { TRUE, TRUE, TRUE, TRUE };
	if (ISSUIT(nPreviousSuit))
	{
		bSuitAvailable[nPreviousSuit] = FALSE;
		numSuitsBid++;
	}
	if (ISSUIT(nPartnersSuit))
	{
		bSuitAvailable[nPartnersSuit] = FALSE;
		numSuitsBid++;
	}
	if (ISSUIT(nPartnersPrevSuit))
	{
		bSuitAvailable[nPartnersPrevSuit] = FALSE;
		numSuitsBid++;
	}
	//
	int numSuitsAvailable = 0;
	int nSuitsAvailable[4];
	for(int i=0;i<4;i++)
	{
		if (bSuitAvailable[i])
		{
			nSuitsAvailable[numSuitsAvailable] = i;
			numSuitsAvailable++;
		}
	}
	ASSERT(numSuitsAvailable >= 1);
	if (numSuitsAvailable == 1)
		newSuit = nSuitsAvailable[0];
	else
		newSuit = PickSuperiorSuit(nSuitsAvailable[0],nSuitsAvailable[1]);

	// partner must have shown two suits
	ASSERT(nPartnersSuit != NONE);
//	ASSERT(nPartnersPrevSuit != NONE);	// may have been 2C

	//
	// the preferred choices are:
	// 1) raise partner's first suit with strong support
	// 2) raise partner's second suit with strong support
	// 3) go with a strong 6-card suit of our own
	// 4) raise partner's original suit with decent support, 
	// 5) raise partner's second suit with decent support
	// 6) otherwise go to no trumps (but only with no voids)
	// 7) else bid a new suit
	//
	if ((nPPrevSuitSupport >= SS_MODERATE_SUPPORT) &&
			  (nPPrevSuitSupport> nPartnersSuitSupport))
	{
		// go with partner's first (previous or original) suit
		nSuit = nPartnersPrevSuit;
		status << "2PKF1! With " & numPPrevSuitSupportCards & 
				  " trumps in partner's first suit (" & STS(nSuit) &
				  "), we prefer to support that suit.\n";
	}
	else if (nPartnersSuitSupport >= SS_MODERATE_SUPPORT)
	{
		// go with partner's second suit
		nSuit = nPartnersSuit;
		status << "2PKF2! With " & numSupportCards & 
				  " trumps in partner's second suit (" & STS(nSuit) &
				  "), we prefer to support that suit.\n";
	}
	else if ((nPrefSuitStrength >= SS_STRONG) && (numPrefSuitCards >= 6))
	{
		// gotta go with our good 6-card suit
		nSuit = nPrefSuit;
		status << "2PKF3! With " & 
				  ((numSuitsBid==3)? "three" : "two") &
				  " suits bid so far, we prefer to " &
				  ((nPreviousSuit == nPrefSuit)? "rebid" : "bid") &
				  " our own " & numPrefSuitCards & "-card " & 
				  STSS(nSuit) & " suit.\n";
	}                                                
	else if (((nPPrevSuitSupport >= SS_WEAK_SUPPORT) 
			  && (nPPrevSuitSupport > nPartnersSuitSupport))
			 // NCR-383 Support with 3 if pard has bid twice 
			 || (ISSUIT(nPartnersPrevSuit) && (m_numPartnerBidsMade >= 2) 
			      && (numCardsInSuit[nPartnersPrevSuit] >= 3)) )
	{
		// go with partner's first (previous or original) suit
		nSuit = nPartnersPrevSuit;
		status << "2PKF4! Without a compelling suit of our own, we prefer to support partner's first suit of " & 
				   STS(nSuit) & ".\n";
	}
	else if (nPartnersSuitSupport >= SS_WEAK_SUPPORT)
	{
		// go with partner's second suit
		nSuit = nPartnersSuit;
		status << "2PKF5! Without a compelling suit of our own, we prefer to support partner's second suit of " & 				   
				  STS(nSuit) & ".\n";
	}
	else if (((numSingletons == 0) && (numVoids == 0))  
		     // NCR-480 what if partner bid the suit with the singleton
		     || (bSemiBalanced && (numSingletons == 1) && ISSUIT(nPartnersSuit) && (numCardsInSuit[nPartnersSuit] == 1)) )
	{
		// go with notrumps
		nSuit = NOTRUMP;
		status << "2PKF6! Without a compelling suit of our own, and with lackluster support for partner's two suits, we prefer to play in No Trumps.\n";
	}
	else if (nSuitStrength[newSuit] >= SS_OPENABLE)
	{
		// go with the 4th suit, if possible
		nSuit = newSuit;
		status << "2PKF7! Since our first suit is not rebiddable, and we lack good support for either of partner's suits, we prefer to bid " &
				  (((numSuitsAvailable==1))? "the 4th suit of" : "a new suit in ") &
				  STS(nSuit) & ".\n";
	}
	else if ((numSingletons <= 1) && (numVoids == 0))
	{
		// if we have no more than 1 singleton, 
		// go with notrumps for lack of anything better
		nSuit = NOTRUMP;
		status << "2PKF8! Without any rebiddable suit of our own, and with lackluster support for partner's two suits, prefer to fall back to No Trumps.\n";
	}

	//
	// at this point, we don't have many good options
	// so pick either: 
	// 1) any 6-card suit of our own
	// 2) the better of partner's two suits, 
	//    with at least 1-card support, or
	// 3) No trumps, even with voids
	//
	else if (numPrefSuitCards >= 6)
	{
		// gotta go with our suit
		nSuit = nPrefSuit;
		status << "2PKF11! With lackluster support for partner's two suits, we choose to rebid our own " & 
				  numPrefSuitCards & "-card " & STSS(nSuit) & " suit.\n";
	}
	else if ( (nPPrevSuitSupport >= nPartnersSuitSupport) &&
			  (numPPrevSuitSupportCards >= 2))
	{
		// go with partner's first suit
		nSuit = nPartnersPrevSuit;
		status << "2PKF12! Without a good suit of our own, we pick partner's first suit of " & 
				   STS(nSuit) & ".\n";
	}
	else if (numSupportCards >= 2)
	{
		// go with partner's second suit
		nSuit = nPartnersSuit;
		status << "2PKF13! Without a good suit of our own, we pick partner's second suit of " & 
				   STS(nSuit) & ".\n";
	}
	else if (numVoids == 0)
	{
		// go with notrumps with no voids
		nSuit = NOTRUMP;
		status << "2PKF14! Without a good suit of our own, and with very poor support for partner's two suits, we are stuck with No Trumps.\n";
	}
	else
	{
		// else go with the 4th suit, no matter what
		nSuit = newSuit;
		status << "2PKF15! Without a rebiddable suit or any decent support for partner's suits, and with a hand unsuitable for Notrump, we have little choice but to bid " &
				  (((numSuitsAvailable==1))? "the 4th suit of" : "a new suit in") &
				   STS(nSuit) & ".\n";
	}
	// NCR Returned NONE ???
	if(nSuit == NONE) {
		ASSERT(FALSE);  // trap here
	}

	// done
	return nSuit;
}








//
//=======================================================================
//
// Convention-related helper functions
//
//



//
//---------------------------------------------------------------
//
// PlayingConvention()
//
BOOL CBidEngine::PlayingConvention(int nConventionID)
{
	return pCurrConvSet->IsConventionEnabled(nConventionID);
}


//
//---------------------------------------------------------------
//
// InvokeBlackwood()
//
BOOL CBidEngine::InvokeBlackwood(int nEventualSuit)
{
	ASSERT(ISSUIT(nEventualSuit) || nEventualSuit == NOTRUMP);  //NCR added NOTRUMP test
	return blackwoodConvention.InvokeBlackwood(*m_pHand, *this, *m_pStatusDlg, nEventualSuit);
}

//---------------------------------------------------------------
//
// Gerber()  NCR-246a Use Gerber afte Stayman 2C
//
BOOL CBidEngine::InvokeGerber(int nEventualSuit)
{
	ASSERT(ISSUIT(nEventualSuit) || nEventualSuit == NOTRUMP);  //NCR added NOTRUMP test
	return gerberConvention.InvokeGerber(*m_pHand, *this, *m_pStatusDlg, nEventualSuit);
}



//
//---------------------------------------------------------------
//
// TryCueBid()
//
BOOL CBidEngine::TryCueBid()
{
	return cueBidConvention.TryCueBid(*m_pHand, *this, *m_pStatusDlg);
}


