//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Bidrebid.cpp
//
// Rebidding routines
//
#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "bidengine.h"
#include "PlayerStatusDialog.h"
#include "ConventionSet.h"
#include "Bidparams.h"



//
//---------------------------------------------------------------
//
// MakeRebidAsOpener()
//
// Rebid after partner's response
//
int CBidEngine::MakeRebidAsOpener()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;

	//
	if (nPartnersBid == BID_PASS)
		status << "2Rebidding after opening bid of " & szPVB & " and partner's pass.\n";
	else
		status << "2Rebidding after opening bid of " & szPVB & " and partner's response of " & szPB & ".\n";


	//
	//##########################################################
	//
	// In general, partner may have made the following responses 
	// to our opening bid:
	// -------------------
	//   pass
	//   1NT (negative)
	//   2NT/3NT
	//   4 of a suit (triple raise)
	//   special convention (Stayman, Blackwood, etc.)
	//   single raise
	//   double raise
	//   shift(1/1 or 2/1)
	//   jump shift
	//





	//
	//==========================================================
	//
	// First check for a convention response
	// possibilities:
	// --------------
	//    Stayman (2C/3C/4C over 1NT/2NT/3NT)
	//    Jacoby Transfer (to the 1NT and 2NT openings)
	//    Blackwood
	//	  Other
	//

	//
	if (pCurrConvSet->ApplyConventionTests(*m_pPlayer, *m_pHand, *m_pCardLocation, m_ppGuessedHands, *this, *m_pStatusDlg))
	{
		return ValidateBid(m_nBid);
	}


	
	



	//
	//==========================================================
	//
	// Rebidding after partner has passed
	// in general, pass unless we have a very strong hand
	// (i.e., 24+ pts and a solid suit)
	//
	if (nPartnersBid == BID_PASS) 
	{

		//
		m_fPartnersMin = 0;
		m_fPartnersMax = 5;
		m_fMinTPPoints = fAdjPts + m_fPartnersMin;
		m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;

		//
		if ((m_fMinTPPoints >= PTS_SLAM) && (m_pHand->GetNumWinners() >= 10) &
			(nSuitStrength[nPrefSuit] >= SS_ABSOLUTE)) 
		{
			status << "B3A00! Partner passed, but with " & fCardPts & "/" & fPts & 
					  " points, " & numLikelyWinners & 
					  " playing tricks and a solid " & szPrefS & 
					  " suit, we have enough to try for a slam.\n";
					  " (Blackwood).\n";
			InvokeBlackwood(nPreviousSuit);
			return ValidateBid(m_nBid);
		} 

		//
		if ((m_fMinTPPoints >= PTS_GAME-1) && (numLikelyWinners >= 9)) 
		{
			// rebid at the 2 or 3 level, overcalling if necessary
			m_nBid = GetCheapestShiftBid(nPrefSuit,nLastBid);
			if (m_nBid < BID_3NT)  
			{
				status << "B3A10! Partner passed, but with " & fCardPts & "/" & fPts & 
						  " points, " & numLikelyWinners & 
						  " playing tricks and a solid " & szPrefS & 
						  " suit, we still want to try for game, so rebid " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
		}
		
		// else we have no choice but to pass
		m_nBid = BID_PASS;
		status << "B3A20! Partner passed, showing < 6 points, so with " & fCardPts & "/" & fPts & 
				  " points in hand and " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
				  " total partnership points, we have no choice but to pass.\n";
		return ValidateBid(m_nBid);
		
	}  // end of pass response section









	//
	//==========================================================
	//
	// Rebidding after partner bid 1NT (negative)
	// partner has shown 6-10 pts
	//   with 12-17 pts (18-23 total) pts:
	//		 if balanced, pass with < 16 pts, or 2NT with 16-17
	//       or rebid suit or show new suit if unbalanced
	//   with 18-19 pts (24-25 total):
	//		 bid 2NT if balanced,
	//       rebid own suit or show new suit
	//   with 19 pts (25 total), bid 2NT or jump to 3 of a suit
	//   with 20+ pts (26+ total), jump to 3NT or 3 of a suit
	//
	if (nPartnersBid == BID_1NT) 
	{

		//
		m_fPartnersMin = OPEN_PTS(6);
		m_fPartnersMax = OPEN_PTS(10);
		//
		status << "B3J00! With a 1NT response, partner is showing " &
				  m_fPartnersMin & "-" & m_fPartnersMax & 
				  " points, a lack of good support for our " & szPVSS & 
				  " suit, and no other suit available to bid at an appropriate level.\n";

		//
		m_fMinTPPoints = fAdjPts + m_fPartnersMin;
		m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;

		// with a balanced hand, steer towards a NT game
		if (bBalanced) 
		{

			//
			status << "2B3J10! And with a balanced hand, we want to steer towards a contract in No Trumps.\n";

			// pass with 18-22 TPs & a balanced hand
			if (m_fMinTPCPoints <= PTS_GAME-4) 
			{
				m_nBid = BID_PASS;
				status << "B3J14! Have " & fCardPts & " HCPS with a balanced hand, and " & 
						  m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
						  " pts in the partnership, so we have to pass.\n";
				return ValidateBid(m_nBid);
			}
			//
			// bid 2NT with 23-25 HCPs
			//
			if ((m_fMinTPCPoints >= PTS_NT_GAME-3) && (m_fMinTPCPoints < PTS_GAME)) 
			{
				m_nBid = BID_2NT;
				status << "B3J18! Have " & fCardPts & " HCPS with a balanced hand, and " &
						  m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
						  " pts in the partnership, so raise to 2NT.\n";
				return ValidateBid(m_nBid);
			}
			// bid 3NT with 26-30 HCPs and all suits stopped
			// or 2NT without stoppers in all suits
			if ((m_fMinTPCPoints >= PTS_NT_GAME) && (m_fMinTPCPoints <= PTS_GAME+4))
			{
				if (bAllSuitsStopped)
				{
					m_nBid = BID_3NT;
					status << "B3J22! Have " & fCardPts & " HCPS with a balanced hand, and " &
							  m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
							  " pts in the partnership, with all suits stopped, so jump to 3NT.\n";
				}
				else
				{
					m_nBid = BID_2NT;
					status << "B3J23! Have " & fCardPts & " HCPS with a balanced hand, and " &
							  m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
							  " pts in the partnership, but not all suits are stopped, so just bid 2NT.\n";
				}
				return ValidateBid(m_nBid);
			}
			// with 31+ total HCPs, try for slam
			// can't bid Blackwood over a 1NT, so bid slam directly
			if (m_fMinTPCPoints >= PTS_SLAM-2) 
			{
				// grand slam with 37+ pts, small slam with 33+,
				// or 4NT invitational with 30-31
				if (m_fMinTPCPoints >= PTS_GRAND_SLAM) 
				{
					m_nBid = BID_7NT;
					status << "B3J26! Have " & fCardPts & " HCPS with a balanced hand, and " &
							  " approx. " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
							  " HCPs in the partnership, so go directly to a grand slam at " & BTS(m_nBid) & ".\n";
				} 
				else if (m_fMinTPCPoints >= PTS_SLAM) 
				{
					m_nBid = BID_6NT;
					status << "B3J28! Have " & fCardPts & " HCPS with a balanced hand, and " &
							  " approx. " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
							  " HCPs in the partnership, so go directly to a small slam at " & BTS(m_nBid) & ".\n";
				} 
				else 
				{
					m_nBid = BID_4NT;
					status << "B3J30! Have " & fCardPts & " HCPS with a balanced hand, and " &
							  "approx. " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
							  " HCPs in the partnership, so invite a slam by bidding " & BTS(m_nBid) & ".\n";
				}
				return ValidateBid(m_nBid);
			}
			//
		} 


		//
		// here, we're making a rebid with an unbalanced hand
		//

		// first find a rebid suit
		nSuit = GetRebidSuit(nPreviousSuit);
		TestReverseRules(nSuit);
		//
		if (nSuit == NONE) 
		{
			// default back to preferred suit if nothing found
			nSuit = nPrefSuit;
		}

		//
		// 18-23 TPs: rebid
		//
		if (m_fMinTPPoints <= PTS_GAME-3) 
		{
			m_nBid = GetCheapestShiftBid(nSuit,BID_1NT);
			if (nSuit == nPreviousSuit)
				status << "B3K10! Partner doesn't like our " & szPVS & 
						  ", but we don't have another suit we can bid, so rebid the suit at " & 
						  BTS(m_nBid) & ".\n";
			else
				status << "B3K12! Since partner doesn't like our " & szPVS & 
						  ", try " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		//
		// 24-25 pts: jump shift and rebid at the 3-level
		//		
		if ((m_fMinTPPoints >= PTS_GAME-2) && (m_fMinTPPoints < PTS_GAME)) 
		{
			m_nBid = GetJumpShiftBid(nSuit,BID_1NT);
			if (nSuit == nPreviousSuit)
				status << "B3K20! Partner doesn't like our " & szPVS & ", but with " &
						  fCardPts & "/" & fPts &
						  " points and no other good suit, jump rebid to " & BTS(m_nBid) & ".\n";
			else
				status << "B3K22! Partner doesn't like our " & szPVS & "; but with " & 
						  fCardPts & "/" & fPts & " points, jump shift to " & 
						  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}
	
		//
		// else we have 26+ TPs: bid 3 of a new suit (forcing to game)
		//	 	

		// find a new suit
		int newSuit = GetNextBestSuit(nPartnersSuit, nPreviousSuit);
		// bid the suit if it's decent, or else rebid the pref. suit
		if (nSuitStrength[newSuit] >= SS_OPENABLE)
		{
			nSuit = newSuit;
			m_nBid = GetJumpShiftBid(nSuit,nPartnersBid);
			status << "B3K30! With " & fCardPts & "/" & fPts & 
					  " points, jump shift to " & BTS(m_nBid) & " (forcing to game).\n";
			m_bGameForceActive = TRUE;		
		}
		else
		{
			// jump rebid our own suit
			m_nBid = GetJumpShiftBid(nSuit,nPartnersBid);
			status << "B3K30a! With " & fCardPts & "/" & fPts & 
					  " points and no other good suits, jump rebid the " & STSS(nSuit) &
					  " suit at " & BTS(m_nBid) & ".\n";
		}
		return ValidateBid(m_nBid);

	}  // end of 1NT response section
	
	
	




	//
	//==========================================================
	//
	// Rebidding after partner bid 2NT or 3NT in response to an
	// opening 1-level bid, or if they bid 4NT-7NT
	// in response to an opening NT bid (NOT Blackwood)
	//
	// partner's point count depends on our initial bid
	//
	//
	if ((nPreviousBidLevel == 1) &&
	    (nPartnersBid == BID_2NT) || (nPartnersBid == BID_3NT) ||
		((nPartnersBid == BID_4NT) && (nPreviousSuit == NOTRUMP)) ||
		((nPartnersBid == BID_5NT) && (nPreviousSuit == NOTRUMP)) ||
		((nPartnersBid == BID_6NT) && (nPreviousSuit == NOTRUMP)) ||
		((nPartnersBid == BID_7NT) && (nPreviousSuit == NOTRUMP)) ) 
	{

		// check point counts 
		CalcNoTrumpPoints();


		// with at least a semi-balanced hand, steer towards a NT game
//		if (bBalanced) 
		if (bSemiBalanced) 
		{

			// 
			if (nPreviousSuit != NOTRUMP)
				status << "2B3N10! With a balanced hand, we want to steer towards a contract in No Trumps.\n";

			//
			// < 26 TPs -- pass
			//
			if ((m_fMinTPCPoints < PTS_GAME) && (!m_bGameForceActive)) 
			{
				m_nBid = BID_PASS;
				status << "B3N12! But with only about " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
						  " HCPS in the partnership, we don't have enough for game, so pass.\n";
				return ValidateBid(m_nBid);
			}

			//
			// with a total of 26-31 pts, bid 3NT
			//
			if ((m_fMinTPCPoints >= PTS_GAME) && (m_fMinTPCPoints <= PTS_SLAM-2)) 
			{
				// 
				if (nPartnersBid >= BID_4NT) 
				{
					m_nBid = BID_PASS;
					status << "B3N14! With a total of approx. " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
							  " HCPS in the partnership, we have to" &
							  ((m_bInvitedToSlam)? " turn down the slam invitation and " : " ") &
							  "pass partner's " & szPB & " bid.\n";
				}
				else if (nPartnersBid == BID_2NT) 
				{
					m_nBid = BID_3NT;
					status << "B3N16! With a total of approx. " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
							  " HCPS in the partnership, try for game at a " & BTS(m_nBid) &
							  " contract.\n";
				} 
				else if (nPartnersBid == BID_3NT) 
				{
					m_nBid = BID_PASS;
					status << "B3N18! With approx. " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
							  " HCPS in the partnership, partner's game bid of 3NT seems appropriate, so pass.\n";
				}
				return ValidateBid(m_nBid);
			}

			//
			// 32+ pts -- Possible slam territory
			//
			if (m_fMinTPCPoints >= PTS_SLAM-1) 
			{
				// check what partner has bid
				if (nPartnersBid >= BID_4NT)
				{
					//
					// partner made an invitational or slam bid
					//
					if ((nPartnersBid == BID_4NT) || (nPartnersBid == BID_5NT))
					{
						// invitational response to 1NT
						if (m_fMinTPCPoints >= PTS_GRAND_SLAM)
						{
							m_nBid = BID_7NT;
							status << "B3N20! Partner's invitational bid of " & szPB & 
									  " indicates a minimum of " & m_fMinTPCPoints & 
									  "+ points in the partnership, which is enough for a grand slam, so bid " &
									  BTS(m_nBid) & ".\n";
						}
						else if (m_fMinTPCPoints >= PTS_SLAM)
						{
							m_nBid = BID_6NT;
							status << "B3N22! Partner's invitational bid of " & szPB & 
									  " indicates a minimum of " & m_fMinTPCPoints & 
									  "+ points in the partnership, which is enough for a small slam, so bid " &
									  BTS(m_nBid) & ".\n";
						}
						else
						{
							m_nBid = BID_PASS;
							status << "B3N24! Partner's invitational bid of " & szPB & 
									  " indicates approx. " & m_fMinTPCPoints & 
									  "+ points in the partnership, which is not quite enough for a small slam, so pass.\n";
						}
					}
					else
					{
						// partner already bid slam, either 6NT or 7NT
						if (nPartnersBid == BID_6NT)
						{
							if (m_fMinTPCPoints >= PTS_GRAND_SLAM)
							{
								m_nBid = BID_7NT;
								status << "B3N30! Partner has bid a small slam, and with " &
										  m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
										  "HCPs in the partnership, we raise to " &
										  BTS(m_nBid) & ".\n";
							}
							else
							{
								m_nBid = BID_PASS;
								status << "B3N32! Partner has bid a small slam, and with " &
										  m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
										  "HCPs in the partnership, pass.\n";
							}
						}
						else
						{
							m_nBid = BID_PASS;
							status << "B3N34! Partner has already bid a grand slam, so just pass.\n";
						}
					}
				}
				else
				{
					//
					// partner has bid 3NT or less
					// look for a slam
					if (m_fMinTPCPoints >= PTS_GRAND_SLAM) 
					{
						m_nBid = BID_7NT;	// bid grand slam directly
						status << "B3N40! With a total of approx. " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
								  " HCPS in the partnership, bid a grand slam directly at " & BTS(m_nBid) & ".\n";
					} 
					else if (m_fMinTPCPoints >= PTS_SLAM) 
					{
						m_nBid = BID_6NT;	// bid small slam directly
						status << "B3N42! With a total of approx. " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
								  " HCPS in the partnership, bid a small slam directly at " & BTS(m_nBid) & ".\n";
					} 
					else 
					{
						// 32 pts? bid 4NT invitational (NOT Blackwood)
						m_nBid = BID_4NT;	
						status << "B3N44! With a total of approx. " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
								  " HCPS in the partnership, make an invitation towards slam at " & BTS(m_nBid) & ".\n";
					}
				}
				return ValidateBid(m_nBid);
			}
		} 


		//
		//-------------------------------------------------------
		//
		// here, we're making a rebid with an unbalanced hand
		//
		// we previously made a suit bid at the 1 level
		//

		//
		// first find a rebid suit
		//
		nSuit = GetRebidSuit(nPreviousSuit);
		if (nSuit == NONE) 
		{
			// default back to preferred suit if nothing found
			nSuit = nPrefSuit;
		}

		//
		// partner made a 2NT response (13-15 HCPs)
		//
		if (nPartnersBid == BID_2NT) 
		{

			// with 33+ HCPs (19+ HCPs in hand), no singletons and 
			// all suits stopped, bid 6NT 
			if ((m_fMinTPCPoints >= PTS_SLAM) && (numSuitsStopped == 4) &&
						(numVoids == 0) && (numSingletons == 0)) 
			{
				if (m_fMinTPCPoints >= PTS_GRAND_SLAM) 
					m_nBid = BID_7NT;
				else
					m_nBid = BID_6NT;
				status << "B3N50! With a total of " & 
						  m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
						  " HCPs in the partnership and a reasonably balanced distribution, and all suits stopped, we can jump to slam at " &
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// with 33+ pts and a strong 6-card suit, jump to 6
			if ((m_fMinTPPoints >= PTS_SLAM) && (numPrefSuitCards >= 6) &&
						(nPrefSuitStrength >= SS_STRONG)) 
			{
				m_nBid = MAKEBID(nPrefSuit, 6);
				status << "B3N50a! With a total of " & 
						  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " pts in the partnership, plus a strong " &
						  numPrefSuitCards & "-card " & szPrefSS & 
						  " suit, go ahead and jump to slam at " &
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// with 26+ tot HCPs (12+ pts in hand), no singletons and 
			// all suits stopped, bid 3NT for game
			if ((m_fMinTPCPoints >= PTS_GAME) && (numSuitsStopped == 4) &&
						(numVoids == 0) && (numSingletons == 0)) 
			{
				m_nBid = BID_3NT;
				status << "B3N51! With " & fCardPts & 
						  " HCPs, reasonably balanced distribution, and all suits stopped, we can safely bid game at " &
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// rebid a suit with 26+ TPs, or pass with less
			if (m_fMinTPPoints >= PTS_GAME) 
			{
				m_nBid = GetCheapestShiftBid(nSuit);
				if (nSuit == nPreviousSuit)
					status << "B3N52! Partner wants to play in No Trumps, but we don't have a balanced distribution, so rebid " & BTS(m_nBid) & ".\n";
				else
					status << "B3N54! Partner wants to play in No Trumps, but we don't have a balanced distribution, so try bidding " & BTS(m_nBid) & ".\n";
			} 
			else if (m_bGameForceActive) 
			{
				// else if game force is active, gotta respond
				if (numVoids == 0) 
				{
					m_nBid = BID_3NT;
					status << "B3N56! As partner's 2NT bid was forcing to game, we have no choice but to bid " & BTS(m_nBid) & ".\n";
					return ValidateBid(m_nBid);
				} 
				else 
				{
					nSuit = GetRebidSuit(nPreviousSuit);
					m_nBid = GetCheapestShiftBid(nSuit);
					status << "B3N58! Partner's 2NT bid was forcing to game -- and with " &
							  ((numVoids == 1)? "a void suit" : 
							   (numVoids == 2)? "2 void suits" : "3 void suits") &
							  ", we have no choice but to rebid " & BTS(m_nBid) & ".\n";
					return ValidateBid(m_nBid);
				}
			} 
			else 
			{
				// just pass
				m_nBid = BID_PASS;
				status << "B3N62! Partner wants to play in No Trumps, which we don't like, but with only " &
						  fCardPts & "/" & fPts & " points, we can't argue and have to pass.\n";
			}
			return ValidateBid(m_nBid);
		}

		//
		// here, partner bid 3NT in response to our 1 suit bid,
		// showing 16-17 points and 4-3-3-3 distribution.
		//

		// with 32+ TPs (16+ points in hand), look for a slam
		if (m_fMinTPPoints >= PTS_SLAM-1) 
		{
			// look for a slam
			if (m_fMinTPCPoints >= PTS_GRAND_SLAM) 
			{
				m_nBid = BID_7NT;	// bid grand slam directly
				status << "B3N70! Partner's 3NT response indicates a total of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " points in the partnership, so bid a grand slam directly at " &
						  BTS(m_nBid) & ".\n";
			} 
			else if (m_fMinTPCPoints >= PTS_SLAM) 
			{
				m_nBid = BID_6NT;	// bid small slam directly
				status << "B3N72! Partner's 3NT response indicates a total of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " points in the partnership, so bid a small slam directly at " &
						  BTS(m_nBid) & ".\n";
			} 
			else 
			{
				// 32 pts: bid 4NT invitational (not Blackwood)
				m_nBid = BID_4NT;	
				status << "B3N74! Partner's 3NT response indicates a total of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " points in the partnership, so make an invitation towards slam at " &
						  BTS(m_nBid) & ".\n";
			}
			return ValidateBid(m_nBid);
		}

		// else we don't have enough for slam, so rebid a solid suit
		if ((ISMAJOR(nPreviousSuit)) && 
						(nPreviousSuitStrength >= SS_STRONG) &&
						(bPreviousSuitIsSolid)) 
		{
			m_nBid = GetCheapestShiftBid(nPreviousSuit);
			status << "B3N80! Partner wants to play at 3NT, but with a solid major in " &
					  szPVS & ", we prefer to play in the suit, so bid " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		// no solid suit? then pass
		if (!m_bGameForceActive) 
		{
			m_nBid = BID_PASS;
			status << "B3N85! Partner wants to play at 3NT, which we can't argue with since we lack a solid suit of our own.\n";
			return ValidateBid(m_nBid);
		} 
		else 
		{
			nSuit = GetRebidSuit(nPreviousSuit);
			m_nBid = GetCheapestShiftBid(nSuit);
			status << "B3N90! In response to partner's game forcing bid, we have no choice but to express dislike for NT by bidding "& BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

	}  // end of 2NT-7NT response section
	
	




	//
	//==========================================================
	//
	// Rebidding after partner has made a triple raise
	// partner has shown < 9 HCPs
	//   pass with < 20 pts
	//
	if ((nPartnersSuit == nPreviousSuit) && 
		(nPreviousBidLevel == 1) && (nPartnersBidLevel == 4)) 
	{
		//
		return (RespondToTripleRaise());

	}  // end of triple raise response sectio







	//
	//==========================================================
	//
	// Rebidding after partner has made a single raise
	// partner has shown 6-10 pts
	//   with 12-15 pts, pass
	//   with 16-18 pts:
	//       with < 7 playing tricks, bid a new (support) suit
	//       with 7+ playing tricks, jump to game
	//   with 19+ pts, jump to game or bid a new suit
	//
	if ((nPartnersSuit == nPreviousSuit) && (nPartnersBidLevel == 2)) 
	{
	
		//
		return (RespondToSingleRaise(nPartnersBid));

	}  // end of single raise response section






	//
	//==========================================================
	//
	// Rebidding after partner has made a double raise
	// partner has shown 13-16 pts (normal) or 11-12 pts (limit)
	//
	if ((nPartnersSuit == nPreviousSuit) && (nPartnersBidLevel == 3)) 
	{

		//
		return (RespondToDoubleRaise(nPartnersBid));

	}  // end of double raise section





	//
	//==========================================================
	//
	// Rebidding after opener raised our suit 4 or more levels --
	// unlikely, but possible
	// e.g., 1C -> 5C, or
	//       1H -> 6H
	//
	if ((nPartnersSuit == nPreviousSuit) && 
				((nPartnersBidLevel - nPreviousBidLevel) >= 4) ) 
	{
		// partner really likes our suit
		m_nAgreedSuit = nPreviousSuit;

		//
		//---------------------------------------------------------
		// either raise to slam or pass
		//
		if ((nPartnersBidLevel <= 5) &&
				 (m_fMinTPPoints >= PTS_SLAM) && (numSupportCards >= 3))
		{
			m_nBid = MAKEBID(nPartnersSuit, 6);
			status << "B3X1! With a total of " & 
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points in the partnership, go ahead and raise partner to a small slam at " &
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}
		if ((nPartnersBidLevel <= 6) &&
				 (m_fMinTPPoints >= PTS_GRAND_SLAM) && (numSupportCards >= 3))
		{
			m_nBid = MAKEBID(nPartnersSuit, 7);
			status << "B3X2! With a total of " & 
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points in the partnership, go ahead and raise partner to a grand slam at " &
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}
		else
		{
			m_nBid = BID_PASS;
			status << "B3X3! With a total of " & 
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points in the partnership, we have to defer to partner's unusual jump rebid and pass.\n";
			return ValidateBid(m_nBid);
		}
	}




	//
	//==========================================================
	//
	// Rebidding after partner has makes a 1 over 1 response
	// (not 1NT), showing 6-18 points
	//   with 12-16 points, make a minimum rebid
	//	 with 17-19 points, make an invitational bid
	//   with 20+ points, make a game forcing bd
	//
	//
	if ((nPartnersSuit != nPreviousSuit) && (nPartnersBidLevel == 1)) 
	{

		//
		m_fPartnersMin = OPEN_PTS(6);
		m_fPartnersMax = OPEN_PTS(18);
		// adjust max downwards if partner previously passed
		if (nPartnersPrevBid == BID_PASS)
		{
			m_fPartnersMax = OPEN_PTS(12);
			status << "2B3U00a! Partner has made a 1 over 1 bid after an earlier pass, " & szPS & " over " & szPVS & 
					  ", which shows " & m_fPartnersMin & "-" & m_fPartnersMax & 
					  " points and is forcing for one round.\n";
		}
		else
		{
			status << "2B3U00! Partner has made a 1 over 1 bid, " & szPS & " over " & szPVS & 
					  ", which shows " & m_fPartnersMin & "-" & m_fPartnersMax & 
					  " points and is forcing for one round.\n";
		}

		// get adjusted point count as dummy
		fAdjPts = m_pHand->RevalueHand(REVALUE_DUMMY, nPartnersSuit, TRUE);
		m_fMinTPPoints = fAdjPts + m_fPartnersMin;
		m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;

		//
		// with 23 or fewer TPs, bid 1NT, raise partner's suit, 
		//          rebid suit, or bid new suit
		//		
		if (m_fMinTPPoints <= PTS_GAME-3) 
		{
			// single raise parnter's major with support
			if ((ISMAJOR(nPartnersSuit)) && 
						(nPartnersSuitSupport >= SS_WEAK_SUPPORT)) 
			{
				m_nAgreedSuit = nPartnersSuit;
				m_nBid = MAKEBID(nPartnersSuit,2);
				status << "B3U14! With decent " & numSupportCards & 
						  "-card support for partner's major " & szPSS & 
						  " suit (holding " & szHP & 
						  "), no rebiddable suit, and no openable second suit, raise to " & 
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// bid another suit, if it's openable
			// (and if we don't violate reversing rules)
			nSuit = GetNextBestSuit(nPreviousSuit,nPartnersSuit);
			TestReverseRules(nSuit);
			if (IsSuitOpenable(nSuit))
			{
				m_nBid = GetCheapestShiftBid(nSuit);
				if (nSuit != nPreviousSuit)
					status << "B3U18! With " & SLTS(nPartnersSuit) & 
							  " support for partner's " & szPSS &
							  " suit (holding " & szHP & "), and a second openable suit in " & 
							  STS(nSuit) & " (holding " & SHTS(nSuit) & "), show it in a " & 
							  BTS(m_nBid) & " bid.\n";
				else
					status << "B3U18a! With " & SLTS(nPartnersSuit) & 
							  " support for partner's " & szPSS &" suit (holding " & szHP & 
							  "), and no other suit available to bid, rebid the " & szPVSS & 
							  " suit at " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// rebid our own suit if it's solid
			if (bPreviousSuitIsRebiddable) 
			{
				m_nBid = MAKEBID(nPreviousSuit,2);
				if (ISMAJOR(nPartnersSuit)) 
				{
					status << "B3U22! With a rebiddable " & szPVSS & 
							  " suit and insufficient support for partner's " & szPS &
							  " (holding " & szHP & "), rebid at " & BTS(m_nBid) & ".\n";
				} 
				else 
				{
					if (ISMAJOR(nPreviousSuit))
						status << "B3U24! With a rebiddable major " & szPVSS & 
							      " suit, show it in preference to raising partner's minor by rebidding " &
								  BTS(m_nBid) & ".\n";
					else
						status << "B3U26! With a rebiddable " & szPVSS & 
								  " suit, show it in preference to raising partner's suit by rebidding " & 
								  BTS(m_nBid) & ".\n";
				}
				return ValidateBid(m_nBid);
			}

			// else bid 1NT if hand is balanced
			if (bBalanced) 
			{
				m_nBid = BID_1NT;
				if (ISMINOR(nPartnersSuit))
					status << "B3U30! With a balanced distribution, bid " & BTS(m_nBid) &
						      " in preference to partner's minor.\n";
				else
					status << "B3U34! With a balanced distribution and weak support for partner's " & szPSS &
							  " suit (holding " & szHP & "), bid " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// else raise partner's minor with weak or good support
			if ((ISMINOR(nPartnersSuit)) && 
							(nPartnersSuitSupport >= SS_WEAK_SUPPORT)) 
			{
				m_nAgreedSuit = nPartnersSuit;
				m_nBid = MAKEBID(nPartnersSuit,2);
				status << "B3U40! With " & numSupportCards & 
						  "-card support for partner's minor " & szPS & 
						  " suit (holding " & szHP & 
						  "), no rebiddable suit or openable second suit, and unbalanced distribution, raise to " & 
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// else no choice but to bid a negative 1NT
			m_nBid = BID_1NT;
			status << "B3U44! With weak support for partner's " & szPS & 
					  " (holding " & szHP & "), and no other available bid, bid " & 
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		//
		// with 23-25 TPs, bid 2NT, jump bid in own or 
		// partner's suit, or shift to a new suit
		//
		if ((m_fMinTPPoints >= PTS_GAME-3) && (m_fMinTPPoints <= PTS_GAME-1)) 
		{

			// jump raise partner's major suit with 4 trumps
			if ((ISMAJOR(nPartnersSuit)) &&
							(numSupportCards >= 4)) 
			{
				m_nAgreedSuit = nPartnersSuit;
				m_nBid = MAKEBID(nPartnersSuit,3);
				status << "B3U50! With " & numSupportCards & 
						  "-card support for partner's " & szPS & 
						  " (holding " & szHP & ") and " &
						  fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points, jump raise to " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// bid another good suit
			nSuit = GetNextBestSuit(nPreviousSuit,nPartnersSuit);
			TestReverseRules(nSuit);
			if (IsSuitOpenable(nSuit)) 
			{
				m_nBid = GetCheapestShiftBid(nSuit);
				status << "B3U54! With weak support for partner's " & szPSS &
						  " suit (holding " & szHP & "), " &
						  fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points, and a second openable suit in " & STS(nSuit) & 
						  " (holding " & SHTS(nSuit) & "), show it in a " & 
						  BTS(m_nBid) & " bid.\n";
				return ValidateBid(m_nBid);
			}

			// jump rebid our own suit if it's a solid six-carder
			if ((nPreviousSuitStrength >= SS_STRONG) &&
								(bPreviousSuitIsSolid) &&
								(numPreviousSuitCards >= 6)) 
			{
				m_nBid = GetJumpShiftBid(nPreviousSuit,nPartnersBid);
				status << "B3U58! With a solid " & numPreviousSuitCards & 
						  "-card suit of our own in " & szPVS & 
						  ", and only " & SLTS(nPartnersSuit) & 
						  " support for partner's " & szPS &
						  " (holding " & szHP & "), jump rebid our suit at " & 
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// else bid 2NT if hand is balanced
			if ((bBalanced) && (m_fMinTPCPoints >= PTS_GAME-2)) 
			{
//				     (AllOtherSuitsStopped(nPartnersSuit))) {
				m_nBid = BID_2NT;
				status << "B3U62! With weak support for partner's " & szPS &
						  " (holding " & szHP & 
						  ") and a balanced hand with " & fCardPts & 
						  " HCPs, for a total of " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
						  " HCPs in the partnership, bid " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// else jump raise partner's minor with 4 good trumps
			if ((ISMINOR(nPartnersSuit)) &&
					(nPartnersSuitSupport >= SS_GOOD_SUPPORT)) 
			{
				m_nAgreedSuit = nPartnersSuit;
				m_nBid = MAKEBID(nPartnersSuit,3);
				status << "B3U64! With " & numSupportCards & 
						  "-card support for partner's minor " & szPS & 
						  " suit (holding " & szHP & 
						  "), no rebiddable suit or second suit of our own, and " &
						  fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points, jump raise to " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// else shift to another suit if it's decent
			nSuit = GetNextBestSuit(nPreviousSuit,nPartnersSuit);
			// but make sure to return to partner's suit later if we have good support
			if (nPartnersSuitSupport >= SS_MODERATE_SUPPORT)
				m_nIntendedSuit = nPartnersSuit;
			// check if we're reversing
			TestReverseRules(nSuit);
			// and shift to the new suit if it's OK
			if (IsSuitShiftable(nSuit))
			{
				m_nBid = GetCheapestShiftBid(nSuit);
				status << "B3U68! With " & numSupportCards & "-card support for partner's " & szPS & 
						  " (holding " & szHP & "), " &
						  fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points, an unbalanced hand, and our primary " & szPVSS & 
						  " suit not jump rebiddable, we have to show our strength by shifting to " & BTS(m_nBid) &
						  ", regardless of that suit's strength.\n";
			}
			else
			{
				// we really don't want to shift to a weak suit (e.g., a 3-carder),
				// only to have partner raise it, so bid 2NT
				m_nBid = BID_2NT;
				status << "B3U69! With " & numSupportCards & "-card support for partner's " & szPS & 
						  " (holding " & szHP & "), " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points, an unbalanced hand, our primary " & szPVSS & 
						  " suit not jump rebiddable, and no second suit available, bid 2NT.\n";
			}
			//
			return ValidateBid(m_nBid);
		}

		//
		// else we have 26+ team points, so make a game forcing bid
		//

		// see if we can support partner's major in game
		if ((ISMAJOR(nPartnersSuit)) && (numSupportCards >= 4)) 
		{
			m_nAgreedSuit = nPartnersSuit;
			m_nBid = MAKEBID(nPartnersSuit,4);
			status << "B3U72! With " & numSupportCards & "-card support for partner's " &
					  szPS & " (holding " & szHP & ") and " &
					  fCardPts & "/" & fPts & "/" & fAdjPts &
					  " points in hand (for " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points in the partnership), we have enough for game, so bid " &
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		// jump shift to another good suit
		nSuit = GetNextBestSuit(nPreviousSuit,nPartnersSuit);
		if (nSuitStrength[nSuit] >= SS_OPENABLE) 
		{
			m_nBid = GetJumpShiftBid(nSuit,nPartnersBid);
			status << "B3U74! With less than game-level support for partner's " & szPSS & 
					  " suit (holding " & szHP & "), but with " &
					  fCardPts & "/" & fPts & "/" & fAdjPts &
					  " points and a second suit in " & STS(nSuit) & 
					  " (holding " & SHTS(nSuit) & "), show it by jump shifting to " & 
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		// see if we can play No Trumps
		// need balanced hand -- bid 3NT with 26+ tot HCPS && 
		// all suits stopped, or 2NT otherwise
		if (bBalanced) 
		{
			if ((m_fMinTPCPoints >= PTS_NT_GAME) && 
					(m_pHand->AllOtherSuitsStopped(nPartnersSuit))) 
			{
				m_nBid = BID_3NT;
				status << "B3U76! With a balanced hand, approx. " &
						  m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
						  " HCPs in the partnership, and stoppers in all" &
						  ((numSuitsStopped == 4)? " " : " unbid ") &
						  "suits, we can make game at No Trumps, so bid " & 
						  BTS(m_nBid) & ".\n";
			} 
			else 
			{
				m_nBid = BID_2NT;
				status << "B3U78! With a balanced hand and approx. " &
						  m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
						  " HCPs in the partnership, we can bid " & BTS(m_nBid) & ".\n";
			}
			return ValidateBid(m_nBid);
		}

		// try an alternative NT qualifier -- a strong minor suit
		// with all unbid suits stopped && 25+ total HCPs
		if ((ISMINOR(nPreviousSuit)) && (m_fMinTPCPoints >= PTS_GAME-1) &&
					(numLikelyWinners >= 7) &&
				    (nPreviousSuitStrength >= SS_STRONG) &&
					(bPreviousSuitIsSolid) &&
					(m_pHand->AllOtherSuitsStopped(nPartnersSuit))) 
		{
			m_nBid = BID_3NT;
			status << "B3U80! With a solid minor suit, all" & 
					  ((numSuitsStopped == 4)? " " : " unbid ") &
					  "suits stopped, " & fCardPts & " HCPs, and " & numLikelyWinners & 
					  " playing tricks, bid " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		// see if we can make a game from our own suit
		// for this, we need a 6-card suit w/4 honors or
		// a 6-card suit w/ 3 honors
		if (bPreviousSuitIsSelfSupporting) 
		{
			if (ISMAJOR(nPreviousSuit)) 
			{
				m_nBid = MAKEBID(nPreviousSuit,4);
				status << "B3U82! With our solid " & szPVSS & " suit with " & 
						  numPreviousSuitCards & " cards and " & numHonorsInPreviousSuit & 
						  " honors, and " & fCardPts & "/" &fPts & 
						  " points in hand, we should be able to make a game with little help, so bid " &
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
			// if this is a minor, we need 29+ pts total
			if (m_fMinTPPoints >= PTS_MINOR_GAME) 
			{
				m_nBid = MAKEBID(nPreviousSuit,5);
				status << "B3U84! With our solid " & szPVSS & " suit with " & numPreviousSuitCards & 
						  " cards and " & numHonorsInPreviousSuit & " honors, and " & 
						  fCardPts & "/" &fPts & 
						  " points in hand, we should be able to make a minor suit game with little help, so bid " & 
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
		}

		// see if we can raise or make a game at partner's minors
		if ((ISMINOR(nPartnersSuit)) && (m_fMinTPPoints >= PTS_GAME) &&
										(numSupportCards >= 3)) 
		{
			// raise to minor game with 29+ pts & 4 support cards
			m_nAgreedSuit = nPartnersSuit;
			if ((m_fMinTPPoints >= PTS_MINOR_GAME) && (numSupportCards >= 4)) 
				m_nBid = MAKEBID(nPartnersSuit,5);
			else
				m_nBid = MAKEBID(nPartnersSuit,4);
			status << "B3U86! With " & numSupportCards & 
					  "-card support for partner's minor " & 
					  szPS & " suit (holding " & szHP & 
					  "), no rebiddable suit or second suit of our own, and " &
					  fCardPts & "/" & fPts & "/" & fAdjPts &
					  " points (for a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " points in the partnership), raise to" &
					  ((m_nBid == GetGameBid(nPartnersSuit))? " game at " : " ") &
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		// we're running out of options here -- we've got 26+ total pts, but
		// we don't like partner's suit, and do not have a good second suit,
		// and the hand is not suitable for an NT bid
		// so rebid our own suit if it's decent
		if (bPreviousSuitIsRebiddable) 
		{
			// jump rebid if it's possible to do so below game
			BOOL bJumpRebid = FALSE;
			int nResponse = GetCheapestShiftBid(nPreviousSuit);
			if (BID_LEVEL(nResponse) <= 3)
			{
				m_nBid = GetJumpShiftBid(nPreviousSuit, nPartnersBid);
				bJumpRebid = TRUE;
			}
			else
				m_nBid = nResponse;
			//
			if (BID_LEVEL(m_nBid) <= 4)
			{
				status << "B3U90! We have " & m_fMinTPPoints & "-" & 
						  " pts in the partnership, but don't like partner's suit, don't have a good second suit, and NT is not a good option, so " &
						  (bJumpRebid? "jump rebid" : "rebid") & " the " & szPVSS & " suit at " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
		}

		// else jump shift in a new suit
		nSuit = GetNextBestSuit(nPreviousSuit,nPartnersSuit);
		if (IsSuitShiftable(nSuit))
		{
			m_nBid = GetJumpShiftBid(nSuit,nPartnersBid);
			status << "B3U94! With unsatisfactory support for partner's " & szPS & 
					  " (holding " & szHP & "), an unbalanced hand, and " &
					  fCardPts & "/" & fPts & 
					  " points, we have to show the hand's strength by jump shifting to the next best suit at " & 
					  BTS(m_nBid) & ", regardless of that suit's strength.\n";
		}
		else
		{
			// bid NT
			if (m_fMinTPCPoints >= PTS_NT_GAME) 
				m_nBid = BID_3NT;
			else
				m_nBid = BID_2NT;
			status << "B3U96! With unsatisfactory support for partner's " & szPS & 
					  " (holding " & szHP & "), " & fCardPts & "/" & fPts & 
					  " points, and no good second suit, we have to show our strength by jump shifting to " & 
					  BTS(m_nBid) & ", despite having an unbalanced hand.\n";
		}
		return ValidateBid(m_nBid);

	}  // end of 1 over 1 response section






	//
	//==========================================================
	//
	// Rebidding after partner has mades a 2 over 1 response
	// (not 2NT), showing 10-18 points
	//   with 12-18 points, bid NT, raise, or shift
	//   with 19+ points, raise to game in NT or a suit
	//
	if ((nPartnersSuit != nPreviousSuit) && 
		(nPreviousBidLevel == 1) && (nPartnersBidLevel == 2) &&
		(!bJumpResponseByPartner)) 
	{

		//
		m_fPartnersMin = OPEN_PTS(10);
		m_fPartnersMax = OPEN_PTS(18);
		// adjust max downwards if partner previously passed
		if (nPartnersPrevBid == BID_PASS)
		{
			m_fPartnersMax = OPEN_PTS(12);
			status << "2B3W00a! Partner has made a 2 over 1 bid following an earlier pass, " & szPS & " over " & szPVS & 
					  ", which shows " & m_fPartnersMin & "-" & m_fPartnersMax & 
					  " points and is forcing for one round.\n";
		}
		else
		{
			status << "2B3W00! Partner has made a 2 over 1 bid, " & szPS & " over " & szPVS & 
					  ", which shows " & m_fPartnersMin & "-" & m_fPartnersMax & 
					  " points and is forcing for one round.\n";
		}

		// get adjusted point count as dummy
		fAdjPts = m_pHand->RevalueHand(REVALUE_DUMMY, nPartnersSuit, TRUE);
		m_fMinTPPoints = fAdjPts + m_fPartnersMin;
		m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;

		//
		// 22-29 TPs: bid 2NT, raise partner's suit, rebid suit,
		//            or bid new suit
		//		
		if (m_fMinTPPoints <= PTS_MINOR_GAME) 
		{

			// raise partner's major to 3 or 4 with a good fit
			if ((ISMAJOR(nPartnersSuit)) &&
							(numSupportCards >= 4)) 
			{
				m_nAgreedSuit = nPartnersSuit;
				// need 26+ TPs for a raise to the 4-level
				if ((nPartnersSuitSupport >= SS_GOOD_SUPPORT) &&
										(m_fMinTPPoints >= PTS_GAME))
					m_nBid = MAKEBID(nPartnersSuit,4);
				else
					m_nBid = MAKEBID(nPartnersSuit,3);
				status << "B3W10! With " & numSupportCards & "-card support for partner's " &
						  szPS & " (holding " & szHP & ") and " &
						  fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points (for a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points in the partnership), raise to " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// bid 2NT with 24-25 HCPs if hand is balanced 
			// or 3NT with 26+ HCPs and all unbid suits stopped
			if ((bBalanced) && (m_fMinTPCPoints >= PTS_NT_GAME-2)) 
			{
				if ((m_fMinTPCPoints >= PTS_NT_GAME) &&
						(m_pHand->AllOtherSuitsStopped(nPartnersSuit)))
					m_nBid = BID_3NT;	// 26+ total HCPs
				else 
					m_nBid = BID_2NT;	// 24-25 Total HCPs
				if (ISMINOR(nPartnersSuit))
					status << "B3W14! With a balanced distribution, approx. " &
							  m_fMinTPCPoints & "-"& m_fMaxTPCPoints &
						      " HCPs in the partnership, and " & 
							  ((m_nBid == BID_3NT)? "all unbid suit stopped" : " a balanced distribution") & 
							  ", bid " & BTS(m_nBid) & " in preference to partner's minor.\n";
				else
					status << "B3W16! With a balanced distribution, approx. " &
							  m_fMinTPCPoints & "-"& m_fMaxTPCPoints &
							  " HCPs in the partnership, weak support for partner's major, and " &
							  ((m_nBid == BID_3NT)? "all unbid suit stopped" : " a balanced distribution") &
							  ", bid " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// jump to game with a self-supporting major
			if (bPreviousSuitIsSelfSupporting)
			{
				// need 26 TPs for major
				if ((ISMAJOR(nPreviousSuit)) && (m_fMinTPPoints >= PTS_MAJOR_GAME)) 
				{
					m_nBid = MAKEBID(nPreviousSuit,4);
					status << "B3W20! With our self-supporting " & szPVSS & " suit with " & 
							  numPreviousSuitCards & " cards and " & numHonorsInPreviousSuit & 
							  " honors, and " & fCardPts & "/" & fPts & 
							  " points in hand, and a total of " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
							  " in the partnership, we can make a game contract in the suit with little help from partner, so bid " &
							  BTS(m_nBid) & ".\n";
					return ValidateBid(m_nBid);
				}
				// need 29+ TPs for a minor
				if ((ISMINOR(nPreviousSuit)) && (m_fMinTPPoints >= PTS_MINOR_GAME)) 
				{
					m_nBid = MAKEBID(nPreviousSuit,5);
					status << "B3W24! With our solid " & szPVSS & " suit with " & 
							  numPreviousSuitCards & " cards and " & numHonorsInPreviousSuit & 
							  " honors, and " & fCardPts & "/"& fPts & 
							  " points in hand, we should be able to make a game in minors with little help, so bid " &
							  BTS(m_nBid) & ".\n";
					return ValidateBid(m_nBid);
				}
			}

			// else show another suit, if it's openable
			nSuit = GetNextBestSuit(nPreviousSuit,nPartnersSuit);
			if (IsSuitOpenable(nSuit) && (TestReverseRules(nSuit)))
			{
				m_nBid = GetCheapestShiftBid(nSuit);
				status << "B3W28! With weak " & numSupportCards & 
						  "-card support for partner's " & szPS & 
						  " (holding " & szHP & 
						  ") and a " & SSTS(nSuit) & " second suit in " & STS(nSuit) & 
						  " (holding " & SHTS(nSuit) & "), show it in a " & 
						  BTS(m_nBid) & " bid.\n";
				return ValidateBid(m_nBid);
			}

			// else raise partner's minor with 4-card support
			if ((ISMINOR(nPartnersSuit)) &&	(numSupportCards >= 4)) 
			{
				m_nAgreedSuit = nPartnersSuit;
				if (m_fMinTPPoints >= PTS_MINOR_GAME-3)
					m_nBid = MAKEBID(nPartnersSuit,4);
				else
					m_nBid = MAKEBID(nPartnersSuit,3);
				status << "B3W32! With " & numSupportCards & 
						  "-card support for partner's minor " & szPS & 
						  " suit (holding " & szHP & 
						  "), no rebiddable suit or good second suit, and unbalanced distribution, raise to " &
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// don't have good support for partner's minor, 
			// so rebid our own suit if possible
			if (bPreviousSuitIsRebiddable) 
			{
				m_nBid = GetCheapestShiftBid(nPreviousSuit);
				status << "B3W36! With a rebiddable " &szPVSS & " suit and " &
						  SLTS(nPartnersSuit) & 
						  " support for partner's " & szPS & " (holding " & szHP & 
						  "), rebid " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// finally, show any decent suit (out of desperation)
			nSuit = GetNextBestSuit(nPreviousSuit,nPartnersSuit);
			TestReverseRules(nSuit);
			// if we have good support, make sure we return to partner's suit
			if (nPartnersSuitSupport >= SS_MODERATE_SUPPORT)
				m_nIntendedSuit = nPartnersSuit;
			if ((nSuit != nPreviousSuit) && IsSuitShiftable(nSuit))
			{
				m_nBid = GetCheapestShiftBid(nSuit);
				status << "B3W40! With " & SLTS(nPartnersSuit) & 
						  " support for partner's" & (ISMINOR(nPartnersSuit)? " minor " : " ") & 
						  szPSS & " suit (holding " & szHP & 
						  "), and our first suit not rebiddable, we have to shift to another suit at " & 
						  BTS(m_nBid) & ".\n";
			}
			else
			{
				m_nBid = GetCheapestShiftBid(nPreviousSuit);
				status << "B3W41! With " & SLTS(nPartnersSuit) & 
						  " support for partner's" & (ISMINOR(nPartnersSuit)? "minor " : " ") & 
						  szPSS & " suit (holding " & szHP & 
						  "), and no other good suits, we're forced to rebid our " & 
						  szPVS & " suit at " & BTS(m_nBid) & ".\n";
			}
			return ValidateBid(m_nBid);
		}

		//
		//--------------------------------------------------------------
		//
		// 29+ TPs: bid 3NT, raise partner's suit, rebid suit,
		//          or bid new suit
		//		

		// 29-32 TPs: raise partner's suit to game with a good fit
		if ((m_fMinTPPoints < PTS_SLAM) && (numSupportCards >= 4)) 
		{
			m_nAgreedSuit = nPartnersSuit;
			if (ISMAJOR(nPartnersSuit))
				m_nBid = MAKEBID(nPartnersSuit,4);
			else	
				m_nBid = MAKEBID(nPartnersSuit,5);
			status << "B3W44! With " & numSupportCards & "-card support for partner's " &
					  szPS & " (holding " & szHP & ") and " &
					  fCardPts & "/" & fPts & "/" & fAdjPts &
					  " points (for a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points in the partnership), raise to " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		// with 33+ TPs and a good fit, invite slam
		if ((m_fMinTPPoints >= PTS_SLAM) && (nPartnersSuitSupport >= SS_GOOD_SUPPORT)) 
		{
			InvokeBlackwood(nPartnersSuit);
			return ValidateBid(m_nBid);
		}

		// bid 3NT if hand is balanced && all suits are stopped
		if ((m_fMinTPCPoints >= PTS_NT_GAME) && (m_fMinTPCPoints < PTS_SLAM) && 
			(bBalanced) && (m_pHand->AllOtherSuitsStopped(nPartnersSuit))) 
		{
			m_nBid = BID_3NT;
			if (ISMINOR(nPartnersSuit))
				status << "B3W48! With a balanced distribution and all unbid suits stopped, along with " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints & " HCPs in the partnership, bid " & 
					      BTS(m_nBid) & " in preference to partner's minor.\n";
			else
				status << "B3W50! With a balanced distribution, all unbid suits stopped, " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " HCPs in the partnership, and weak support for partner's major " & 
					      szPSS & " suit, bid " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		// jump to game with a self-supporting suit
		if (bPreviousSuitIsSelfSupporting)
		{
			if (ISMAJOR(nPreviousSuit)) 
				m_nBid = MAKEBID(nPreviousSuit,4);
			else
				m_nBid = MAKEBID(nPreviousSuit,5);
			status << "B3W54! With our self-supporting " & szPVSS & 
					  " suit with " & numPreviousSuitCards & " cards and " &
					  numHonorsInPreviousSuit & " honors, and " & 
					  fCardPts & "/" & fPts & 
					  " points in hand, we should be able to make a game contract in the suit with little help from partner, so bid " &
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		// jump rebid a good six-card suit
		if ((numPreviousSuitCards >= 6) && (nPreviousSuitStrength >= SS_STRONG))
		{
			m_nBid = GetJumpShiftBid(nPreviousSuit, nPartnersBid, JUMP_REBID_ONE);
			status << "B3W56! With our strong " & numPreviousSuitCards & "-card " & 
					  szPVSS & " suit and " & fCardPts & "/" & fPts & 
					  " points in hand, we can make a jump rebid at " &
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		// else with 29+ TPs, jump shift in another suit
		nSuit = GetNextBestSuit(nPreviousSuit,nPartnersSuit);
		if (nSuitStrength[nSuit] >= SS_OPENABLE)
		{
			m_nBid = GetJumpShiftBid(nSuit,nPartnersBid);
			status << "B3W58! With weak support for partner's " & szPS & 
					  " (holding " & szHP & 
					  "), an unbalanced hand, and a " & SSTS(nSuit) & " second " & STSS(nSuit) & 
					  " suit (holding " & SHTS(nSuit) & 
					  "), show it by jump shifting to " & BTS(m_nBid) & ".\n";
		}
		else
		{
			// the other suit isn't very good, so go to Notrump
			m_nBid = GetJumpShiftBid(NOTRUMP,nPartnersBid);
			status << "B3W59! With weak support for partner's " & szPS & " (holding " & szHP & 
					  ") and no good second suit, we have show our strength by jump shifting in Notrumps to " &
					  BTS(m_nBid) & ", despite the unbalanced hand.\n";
		}
		return ValidateBid(m_nBid);

	}  // end of 2 over 1 response section






	//
	//==========================================================
	//
	// Rebidding after partner has made a jump shift in a new suit,
	// showing 19+ points
	// e.g., 1C -> 2S, or
	//       1H -> 3C
	// the bid is forcing to game. and invitational towards slam
	// however, the suit he bid may not be the one he really likes
	//
	if (((nPartnersSuit > nPreviousSuit) && (nPartnersBidLevel > nPreviousBidLevel)) ||
		((nPartnersSuit < nPreviousSuit) && (nPartnersBidLevel > nPreviousBidLevel+1))) 
	{

		// estimate partner's points -- but remember, partner may have 
		// jump shifted after a strong opening on our part
		if (nPreviousSuit == NOTRUMP)
		{
			int nLevel = nPreviousBidLevel;
			if (nLevel > 3)
				nLevel = 3;
			double fCreditedPts = pCurrConvSet->GetNTRangeMin(nLevel);
			m_fPartnersMin = PTS_NT_GAME - fCreditedPts;
			m_fPartnersMax = MIN(22, 40 - fCardPts);
		}
		else
		{
			m_fPartnersMin = OPEN_PTS(19);
			m_fPartnersMax = MIN(OPEN_PTS(pCurrConvSet->GetValue(tn2ClubOpeningPoints)), 40 - fCardPts);
		}
		status << "2B3Y00! Partner has made a jump shift to " & szPB & " after our opening " &
				   szPVB & " bid, which indicates " & m_fPartnersMin  &
				  "+ points and is forcing to game (and points to a possible slam).\n";

		// partner's bid is forcing to game
		m_bGameForceActive = TRUE;		

		// get adjusted point count as dummy
		if (nPartnersSuitSupport >= SS_WEAK_SUPPORT)
			fAdjPts = m_pHand->RevalueHand(REVALUE_DUMMY, nPartnersSuit, TRUE);
		m_fMinTPPoints = fAdjPts + m_fPartnersMin;
		m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;

		//
		// < 33 pts? strive towards game
		//
		if (m_fMinTPPoints < PTS_SLAM) 
		{

			// raise partner's major suit with 4-card support
			if (RaisePartnersSuit(SUIT_MAJOR,RAISE_ONE,0,PTS_SLAM-1,SUPLEN_4))
				return ValidateBid(m_nBid);

			// bid 3NT if hand is balanced && all suits are stopped
			if ((bBalanced) && (nPartnersBidLevel <= 3) &&
					(m_pHand->AllOtherSuitsStopped(nPartnersSuit))) 
			{
				m_nBid = BID_3NT;
				status << "B3Y14! With a balanced distribution and all unbid suits stopped, bid " & 
						  BTS(m_nBid) & " in preference to partner's minor.\n";
				return ValidateBid(m_nBid);
			}

			// jump to game with a self-supporting suit
			if (bPreviousSuitIsSelfSupporting)
			{
				if (ISMAJOR(nPreviousSuit))
					m_nBid = MAKEBID(nPreviousSuit,4);
				else
					m_nBid = MAKEBID(nPreviousSuit,5);
				status << "B3Y18! With our self-supporting " & szPVSS & " suit with " & 
						  numPreviousSuitCards & " cards and " & numHonorsInPreviousSuit & 
						  " honors, and " & fCardPts & "/" & fPts & 
						  " points in hand, we should be able to make a game contract in the suit with little help from partner, so bid " &
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// support one of partner's suits with 3 card support if below game
			if (nPPrevSuitSupport >= nPartnersSuitSupport)
			{
				// support is better for the first suit
				if (RaisePartnersSuit(SUIT_PREV,RAISE_TO_GAME,0,0,SUPLEN_3))
					return ValidateBid(m_nBid);
			}
			else
			{
				// prefer the second suit
				if (RaisePartnersSuit(SUIT_ANY,RAISE_TO_GAME,0,0,SUPLEN_3))
					return ValidateBid(m_nBid);
			}

			// rebid our own suit if possible
			m_nBid = GetCheapestShiftBid(nPreviousSuit);
			if ((bPreviousSuitIsRebiddable) && (IsBidSafe(m_nBid)))
			{
				status << "B3Y22! With a rebiddable " & szPVS & 
						  " suit and " & SLTS(nPartnersSuit) & 
						  " support for partner's " & szPS & 
						  " (holding " & szHP & "), rebid our suit at " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			//
			// if partner jumped to game after our NT opener, he must've 
			// a 6-card suit, so pass with 2-card support
			//
			if ((nPreviousSuit == NOTRUMP) && (nPartnersBid >= GetGameBid(nPartnersSuit)))
			{
				m_nAgreedSuit = nPartnersSuit;
				m_nBid = BID_PASS;
				status << "B3Y23! Partner's jump to game in " & szPS & 
						  " implies a 6+ card holding in the suit, so with " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " pts in the partnership, pass with " &
						  " our " & numSupportCards & "-card support.\n";
				return ValidateBid(m_nBid);
			}

			// else show another suit 
			// (we can't pass, as partner's jump shift is forcing to game)
			nSuit = GetNextBestSuit(nPreviousSuit,nPartnersSuit);
			if (IsSuitShiftable(nSuit))
			{
				m_nBid = GetCheapestShiftBid(nSuit);
				status << "B3Y24! With weak support for partner's " & szPS & " (holding " & szHP & 
						  ") and no rebiddable suit, we have to show our strength by shifting to " &
						  BTS(m_nBid) & ".\n";
			}
			else
			{
				m_nBid = GetCheapestShiftBid(NOTRUMP);
				status << "B3Y28! With weak support for partner's " & szPS & " (holding " & szHP & 
						  "), no rebiddable suit, and no good second suit, we have to show our strength by shifting to " &
						  BTS(m_nBid) & ".\n";
			}
			return ValidateBid(m_nBid);
		}
		else
		{
			//
			// 33+ pts -- move towards slam in either our suit, partner's
			// or our 2nd suit, or NT
			//
			int nextBestSuit = GetNextBestSuit(nPreviousSuit,nPartnersSuit);
			// do we like partner's suit?
			if (numSupportCards >= 3)
			{
//				m_nAgreedSuit = nPartnersSuit;
				// support partner or move to slam
				if (nPartnersBid < GetGameBid(nPartnersSuit))
				{
					// raise partner's suit
					m_nBid = GetCheapestShiftBid(nPartnersSuit, pDOC->GetLastValidBid());
					status << "B3Y40! With a total of " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " pts in the partnership, move towards slam in partner's " &
							  szPSS & " suit by raising to " & BTS(m_nBid) & ".\n";
				}
				else
				{
					// else partner jumped to game, so go directly to slam
					if (nPartnersBidLevel < 6)
					{
						m_nBid = MAKEBID(nPartnersSuit, 6);
						status << "B3Y40a! With a total of " &
								  m_fMinTPPoints & "-" & m_fMaxTPPoints &
								  " pts in the partnership, bid a slam in partner's " &
								  szPSS & " suit with a bid of " & BTS(m_nBid) & ".\n";
					}
					else if ((nPartnersBidLevel == 6) && (m_fMinTPPoints >= PTS_GRAND_SLAM))
					{
						// else pass
						m_nBid = MAKEBID(nPartnersSuit, 7);
						status << "B3Y40b! Partner has bid a small slam at " & 
								  szPB & ", but with " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
								  " pts in the partnership, go ahead and raise to a grand slam at " & 
								  BTS(m_nBid) & ".\n";
					}
					else
					{
						// else pass
						m_nBid = BID_PASS;
						status << "B3Y40c! Partner has bid slam, and with " &
								  m_fMinTPPoints & "-" & m_fMaxTPPoints &
								  " pts in the partnership, we sign off and pass..\n";
					}
				}
			}
			else if (bPreviousSuitIsSelfSupporting)
			{
				// pick our own self-supporting suit
				m_nBid = GetCheapestShiftBid(nPreviousSuit, pDOC->GetLastValidBid());
				status << "B3Y41! With a total of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " pts in the partnership but poor support for partner's suits, move towards slam in our own self-supporting " &
						  STSS(nPreviousSuit) & " suit with a bid of " & BTS(m_nBid) & ".\n";
			}
			else if (IsSuitOpenable(nextBestSuit))
			{
				// bid our next-best suit
				m_nBid = GetCheapestShiftBid(nextBestSuit, pDOC->GetLastValidBid());
				status << "B3Y42! With a total of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " pts in the partnership but poor support for partner's suits, bid our next best suit of " &
						  STS(nextBestSuit) & " at " & BTS(m_nBid) & ".\n";
			}
			else if (bSemiBalanced)
			{
				// go notrumps
				m_nBid = GetCheapestShiftBid(NOTRUMP, pDOC->GetLastValidBid());
				status << "B3Y43! With a total of " &
						  m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
						  " HCPs in the partnership, a " & (bBalanced? " " : "reasonably ") &
						  "balanced hand, and poor support for partner's suits, and no strong suit of our own, push towards a slam in notrumps with a bid of " &
						  BTS(m_nBid) & ".\n";
			}
			else if (bPreviousSuitIsRebiddable)
			{
				// rebid previous suit
				m_nBid = GetCheapestShiftBid(nPreviousSuit, pDOC->GetLastValidBid());
				status << "B3Y44! With a total of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " pts in the partnership but poor support for partner's suits, move towards slam in our own " &
						  STSS(nPreviousSuit) & " suit with a bid of " & BTS(m_nBid) & ".\n";
			}
			else
			{
				// go notrumps
				m_nBid = GetCheapestShiftBid(NOTRUMP, pDOC->GetLastValidBid());
				status << "B3Y45! With a total of " &
						  m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
						  " HCPs in the partnership but poor support for partner's suits, and no strong suit of our own, push towards a slam in notrumps with a bid of " &
						  BTS(m_nBid) & ".\n";
			}
			return ValidateBid(m_nBid);
		}

	}  // end of jump shift response section



	//
	//===============================================================
	//
	// Here, reached an unknown state
	//
	// partner made an unusual response

	m_nBid = BID_PASS;
	status << "B3Z00! Partner made a response which we don't understand; simply pass.\n";
	return ValidateBid(m_nBid);

/*
	status << "B3<<Error>> -- fell out of rebid decision loop.  Initial bid was " & 
			  BTS(nLastBid) & ", and partner's bid was " & szPB &
			  ".  Points are " & fCardPts & "/" & fPts & "/" & fAdjPts &
			  "; preferred suit is " & szPrefSS & " (holding " & szHPref & "
			  ").  Returning PASS.\n";
	return ValidateBid(m_nBid);
*/
}






