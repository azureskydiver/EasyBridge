//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Bidrbid2.cpp
//
// Responder's rebidding routines
//
#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "PlayerStatusDialog.h"
#include "bidengine.h"
#include "ConventionSet.h"



//
//---------------------------------------------------------------
//
// MakeRebidAsResponder()
//
// 4th bid -- i.e., responding to opening partner's rebid
//
int CBidEngine::MakeRebidAsResponder()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;

	//
	status << "2Rebidding as responder after bids of " & szPPB & ", " &
			  szPVB  & ", and " & szPB & ".\n";
						

	// defer point count adjustment until later



	//
	//##########################################################
	//
	// In general, opener may have made the following responses 
	// to our initial response:
	// -------------------
	//   pass
	//   special convention (Blackwood, etc.)
	//   minimum rebid in opened suit
	//   jump rebid in opened suit
	//   1-level -> 1NT
	//   2-level -> 2NT minumum
	//   1-level -> 2NT jump
	//   1-level/2-level ->3NT jump
	//   1NT/2NT/3NT -> 2NT/3NT/4NT/5NT/6NT/7NT
	//   1-level -> 2-level (single raise)
	//   2-level -> 3-level (single raise of our suit)
	//   2-level -> 3-level (re-raise of orig suit, invitational)
	//   1-level -> 3-level (double raise)
	//   2-level -> 4-level (double raise)
	//   1-level -> 4-level (triple raise)
	//   New suit after our single raise (showing a support suit)
	//   New suit after our shift response (showing 3rd suit)
	//   New suit after our response to NT opening (2nd suit)
	//   Jump response in a new suit
	//   anything else -- unknown action, so pass
	//





	//
	//==========================================================
	//
	// first check if a convention is appropriate
	//
	if (pCurrConvSet->ApplyConventionTests(*m_pPlayer, *m_pHand, *m_pCardLocation, m_ppGuessedHands, *this, *m_pStatusDlg))
	{
		return ValidateBid(m_nBid);
	}




	//
	//==========================================================
	//
	// Rebidding after partner has passed
	//
	// we can only pass
	//
	if (nPartnersBid == BID_PASS) 
	{

		//
		m_nBid = BID_PASS;
			return ValidateBid(m_nBid);
		
	}  // end of pass response section



	
	


	//
	//==========================================================
	//
	// Rebidding after opener made minimum rebid in original suit
	// e.g., 1h -> 1s -> 2h, showing: 12-16 pts and 6 cards (or maybe 5)
	//
	//
	if ((nPartnersSuit == nPartnersPrevSuit) &&
					((nPartnersBid - nPartnersPrevBid) <= 5) &&
		  					(nPartnersSuit != NOTRUMP)) 
	{

		// check original opening strength
/*
 * 2-level openings are now handled separately in convention classes
 * 
		if (nPartnersOpeningBidLevel == 2) 
		{
			// partner originally bid at the 2-level
			if (nPartnersOpeningBid == BID_2C) 
			{
				// partner opened with a strong 2C
				m_fPartnersMin = pCurrConvSet->GetValue(tn2ClubOpeningPoints);
				m_fPartnersMax = MIN(m_fPartnersMin, 40 - fCardPts);
				status << "2B4C0! Partner made a minimum rebid of his " & szPSS &
						  " suit at " & szPB & " after his initial strong " & szPPB & 
						  " opening, showing " & m_fPartnersMin & 
						  "+ points and 5+ trump cards.\n";
			} 
			else if (pCurrConvSet->IsConventionEnabled(tidStrongTwoBids)) 
			{
				// playing strong 2s
				m_fPartnersMin = MAX(m_fPartnersMin, 16);
				m_fPartnersMax = MAX(m_fPartnersMax, 22);
				status << "2B4C1! Partner made a minimum rebid of his " & szPSS &
						  " suit at " & szPB & " after his initial strong " & szPPB & 
						  " opening, showing " & m_fPartnersMin & "+ points and 5+ trump cards.\n";
			} 
			else 
			{
				// playing weak 2s
				m_fPartnersMin = MAX(m_fPartnersMin, 6);
				m_fPartnersMax = MAX(m_fPartnersMax, 12);
				status << "2B4C2! Partner made a minimum rebid of his " & szPSS & 
						  " suit at " & szPB & " after his weak " & szPPB &
						  " opening, showing " & m_fPartnersMin & "-" & m_fPartnersMax & 
						  " points and 6+ trump cards.\n";
			}

		} 
		else 
*/
		{
			// else a plain old 1-level opening
			m_fPartnersMin = theApp.GetMinimumOpeningValue(m_pPartner);
			m_fPartnersMax = 16;
			status << "2B4C4! Partner made a minimum rebid of his " & szPSS & 
					  " suit at " & szPB & ", showing " & m_fPartnersMin & "-" & m_fPartnersMax & 
					  " points and 5+ trump cards.\n";
		}

		// get adjusted point count in support of partner
		fAdjPts = m_pHand->RevalueHand(REVALUE_DUMMY, nPartnersSuit, TRUE);
		m_fMinTPPoints = fAdjPts + m_fPartnersMin;
		m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;

		//
		//---------------------------------------------------------
		//
		// pass with <= 20 min total points
		//
		if (m_fMinTPPoints <= PTS_GAME-6) 
		{
			m_nBid = BID_PASS;
			status << "B4C10! With " & fCardPts & "/" & fPts & 
					  " points in the hand, for a total in the partnership of " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points, we have to settle for the current bid and pass.\n";
			return ValidateBid(m_nBid);
		}

		//
		//---------------------------------------------------------
		//
		// with 21-25 TPs, raise partner with 3 trumps,
		// rebid a new suit, or bid NT
		//

		// raise partner's major to the 3-level with 
		// 22-25 total pts and 3-card trump support
		if (RaisePartnersSuit(SUIT_MAJOR,RAISE_ONE, PTS_GAME-4, PTS_GAME-1, SUPLEN_3))
			return ValidateBid(m_nBid);
		// or with a doubleton honor
		if (RaisePartnersSuit(SUIT_MAJOR,RAISE_ONE, PTS_GAME-4, PTS_GAME-1, SUPLEN_3, SS_ANY, HONORS_1, 1))
			return ValidateBid(m_nBid);

		// with 21-22 TPs, bid a second 5-card suit
		if (BidNextBestSuit(SUIT_ANY,SHIFT_CHEAPEST, PTS_GAME-5, PTS_GAME-4, LENGTH_FIVE))
			return ValidateBid(m_nBid);

		// with 21-22 TPs, rebid a good 6-card suit
		if (RebidSuit(SUIT_ANY,REBID_CHEAPEST, PTS_GAME-5, PTS_GAME-4, LENGTH_6, SS_STRONG|SS_SOLID))
			return ValidateBid(m_nBid);

		// with 23-25 total HCPs, bid 2NT with the other suits stopped
		if (nPartnersBidLevel == 2)
			if (BidNoTrump(LEVEL_2, PTS_GAME-3, PTS_GAME-1, FALSE, STOPPED_UNBID))
				return ValidateBid(m_nBid);

		// with 23-25 TPs, jump rebid a good 6-card major 
		if (RebidSuit(SUIT_MAJOR,JUMP_REBID_1, PTS_GAME-3, PTS_GAME-1, LENGTH_6, SS_STRONG|SS_SOLID))
			return ValidateBid(m_nBid);

		// or rebid a good 6-card minor with 21-25 pts
		if (RebidSuit(SUIT_MINOR,REBID_CHEAPEST, PTS_MINOR_GAME-8, PTS_MINOR_GAME-4, LENGTH_6, SS_STRONG|SS_SOLID))
			return ValidateBid(m_nBid);

		// if none of the above apply, escape into NT or pass with <= 25 pts
		if (m_fMinTPPoints < PTS_GAME)
		{
			if ((numSupportCards >= 2) || (nPartnersBidLevel >= 4))
			{
				// either partner shut us out, or we can't raise
				m_nBid = BID_PASS;
				if (nPartnersBid < GetGameBid(nPartnersSuit))
					status << "B4C18! with a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
							  " pts in the partnership and no clear direction in the bidding, pass.\n";
				else
					status << "B4C18a! with a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
							  " pts in the partnership, sign off on partner's " & szPB & " bid.\n";
			}
			else
			{
				// can't support partner!
				m_nBid = GetCheapestShiftBid(NOTRUMP);
				status << "B4C19! with " &
						  ((numSupportCards == 1)? "only a single trump" : "zero trump support") &
						  ", we have to escape from partner's " & szPS & 
						  " and bid " & BTS(m_nBid) & ".\n";
			}
			return ValidateBid(m_nBid);
		}

		//
		//---------------------------------------------------------
		//
		// Here, we have 26+ min total points
		//

		// 26-32 TPs: raise partner's major to 4 with 3+ trumps 
		if (RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_4, PTS_GAME, PTS_SLAM-1, SUPLEN_3))
			return ValidateBid(m_nBid);
		// or with a doubleton honor
		if (RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_4, PTS_GAME, PTS_SLAM-1, SUPLEN_2, STRENGTH_ANY,HONORS_1))
			return ValidateBid(m_nBid);

		// bid 3NT with the other suits stopped and 26-32 HCPs
		if (BidNoTrump(LEVEL_3, PTS_NT_GAME, PTS_SLAM-1, FALSE,STOPPED_UNBID))
			return ValidateBid(m_nBid);
		// or 2NT without the suits stopped and but 29-32 HCPs
		if (BidNoTrump(LEVEL_2, PTS_NT_GAME+3, PTS_SLAM-1,  FALSE, STOPPED_DONTCARE))
			return ValidateBid(m_nBid);
		
		// jump to 4 in a self-supporting major 
		if (RebidSuit(SUIT_MAJOR, REBID_AT_4, PTS_GAME, PTS_SLAM-1, LENGTH_6, SS_SELFSUPPORTING, HONORS_4))
			return ValidateBid(m_nBid);
		// or one with 7+ cards and 3 honors
		if (RebidSuit(SUIT_MAJOR, REBID_AT_4, PTS_GAME, PTS_SLAM-1, LENGTH_6, SS_STRONG, HONORS_3))
			return ValidateBid(m_nBid);

		// raise partner's minor to game with 29 pts & 4 trumps
		if (RaisePartnersSuit(SUIT_MINOR,RAISE_TO_5, PTS_MINOR_GAME, PTS_SLAM-1, SUPLEN_4))
			return ValidateBid(m_nBid);
		// else raise partner's minor to 4 with 4+ trumps and 26-28 pts
		if (RaisePartnersSuit(SUIT_MINOR,RAISE_TO_4, PTS_MINOR_GAME-3, PTS_MINOR_GAME-1, SUPLEN_4))
			return ValidateBid(m_nBid);
		// or with a TRIPLETON honor and 26-28 pts
		if (RaisePartnersSuit(SUIT_MINOR,RAISE_TO_4, PTS_MINOR_GAME-3, PTS_MINOR_GAME-1, SUPLEN_3,STRENGTH_ANY,HONORS_ONE))
			return ValidateBid(m_nBid);

		// 27-32 TPs: raise partner's rebid major to 4 with 2+ trumps 
		if (RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_4, PTS_GAME+1, PTS_SLAM-1, SUPLEN_2))
			return ValidateBid(m_nBid);

		// bid 3NT with 28+ HCPs without all suits stopped
		if (BidNoTrump(LEVEL_3, PTS_NT_GAME+2, PTS_SLAM-1, FALSE, STOPPED_DONTCARE))
			return ValidateBid(m_nBid);
		// or 2NT with 25-27 HCPs without all suits stopped
		if (BidNoTrump(LEVEL_2, PTS_NT_GAME-1, PTS_NT_GAME+1, FALSE, STOPPED_DONTCARE))
			return ValidateBid(m_nBid);

		// if none of the above apply, pass with < 29 pts
		if (m_fMinTPPoints < PTS_MINOR_GAME)
		{
			m_nBid = BID_PASS;
			if (nPartnersBid < GetGameBid(nPartnersSuit))
				status << "B4C28! with a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " pts in the partnership but no clear direction in the bidding, pass.\n";
			else
				status << "B4C28a! with a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " pts in the partnership, sign off on partner's " & szPB & " bid.\n";
			return ValidateBid(m_nBid);
		}

		//
		//---------------------------------------------------------
		//
		// otherwise we have 29+ min total pts, we have slam prospects, so 
		// jump in our suit or in a new one

		if (m_fMinTPPoints >= PTS_MINOR_GAME) 
		{
			status << "B4C20! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points in the partnership, we want to explore slam opportunities.\n";
		}

		// re-evaluate support for partner's suit in light of his rebid
		// i.e., prefer partner's suit with any decent trump support,
		// unless we hold a good suit of our own
		if ((nPartnersSuitSupport >= SS_MODERATE_SUPPORT) &&
									(nPrefSuitStrength < SS_STRONG))
		{
			// hold Qxx or better suppport (in 5-card majors) and no strong suit
			m_nAgreedSuit = nPartnersSuit;
		}
		if ((nPartnersSuitSupport >= SS_WEAK_SUPPORT) &&
									(nPrefSuitStrength < SS_OPENABLE))
		{
			// hold xxx or better suppport (in 5-card majors) and no openable suit
			m_nAgreedSuit = nPartnersSuit;
		}

		// if there is an agreed-on suit, go to blackwood
		if ((m_nAgreedSuit >= CLUBS) && (m_nAgreedSuit < NOTRUMP))
		{
			// with 33+ points, invoke Blackwood
			if (m_fMinTPPoints >= PTS_SLAM) 
			{
				InvokeBlackwood(m_nAgreedSuit);
				return ValidateBid(m_nBid);
			}
			else
			{
				// else make an invitation towards slam -- double jump
				// if that doesn;t exceed game (e.g., 2S -> 4S)
				int nTestBid = MAKEBID(m_nAgreedSuit,nPartnersBidLevel+2);
				if (nTestBid <= GetGameBid(m_nAgreedSuit))
				{
					m_nBid = nTestBid;
					status << "B4C24! With " & fCardPts & "/" & fPts & "/" & fAdjPts &
							  " pts in hand, for a total in the partnership of " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
							  " pts, and with " & numSupportCards &"-card trump support in " & 
							  STS(m_nAgreedSuit) & 
							  ", explore slam possibilities with an invitational bid of " & BTS(m_nBid) & ".\n";
				}
				else
				{
					// else invoke Blackwood or bid slam
					InvokeBlackwood(m_nAgreedSuit);
				}
				return ValidateBid(m_nBid);
			}
		}

		// jump rebid with a solid suit
		if (RebidSuit(SUIT_ANY,JUMP_REBID_1,PTS_MINOR_GAME,0,LENGTH_5,SS_STRONG|SS_SOLID))
			return ValidateBid(m_nBid);

		// or jump shift to another solid suit
		if (BidNextBestSuit(SUIT_ANY,JUMP_SHIFT_1,PTS_MINOR_GAME,0,LENGTH_5,SS_STRONG|SS_SOLID))
			return ValidateBid(m_nBid);

		// with no suit agreement but with 33+ HCPs, invoke Blackwood 
		// with an eventual NT contract in  mind
		if (m_fMinTPCPoints >= PTS_SLAM) 
		{
			InvokeBlackwood(NOTRUMP);
			return ValidateBid(m_nBid);
		}

		// or if nothing else is available, jump rebid a decent 5-card suit
		if (RebidSuit(SUIT_ANY,JUMP_REBID_1,PTS_MINOR_GAME,0,LENGTH_5,SS_OPENABLE))
			return ValidateBid(m_nBid);


		//
		//---------------------------------------------------------
		//
		// here, we've fallen through without hitting any of the 
		// above conditions, so handle generically
		//

		// pass with less than 23 TPs
		if (m_fMinTPPoints < PTS_GAME-3) 
		{
			m_nBid = BID_PASS;
			status << "B4C30! With " & fCardPts & "/" & fPts & 
					  " pts in hand, no rebiddable suit or second openable suit, and poor fit for partner's " & 
				      szPS & ", simply pass.\n";
			return ValidateBid(m_nBid);
		}

		// else bid any new suit with 23+ TPs
		if (BidNextBestSuit(SUIT_ANY))
		{
			status << "B4C34! We have " & fCardPts & "/" & fPts & 
					  " pts in hand and " & m_fMinTPPoints & "-" & m_fMaxTPPoints &  
					  " pts in the partnership, but have no good fit with partner and no rebiddable suit, so we have no choice but to shift to yet another suit.\n";
		}
		else
		{
			m_nBid = BID_PASS;
			status << "B4C35! We have " & fCardPts & "/" & fPts & 
					  " pts in hand and " & m_fMinTPPoints & "-" & m_fMaxTPPoints &  
					  " pts in the partnership, but have no good fit with partner and no rebiddable suit, and we've run out of safe bidding space, so we're forced to pass.\n";
		}
		return ValidateBid(m_nBid);

	} // end of minimum rebid section







	//
	//==========================================================
	//
	// Rebidding after opener made a jump rebid in original suit
	// e.g., 1h -> 1s -> 3h, or even
	//       1h -> 1s -> 4h, showing 17-19 points
	//
	//

	//
	if ( (nPartnersSuit == nPartnersPrevSuit) && 
						(nPartnersSuit != nPreviousSuit) &&
						(nPartnersSuit != NOTRUMP) &&
						((nPartnersBid - nPreviousBid) >5) ) 
	{
		//
		m_fPartnersMin = MAX(m_fPartnersMin, 17);
		m_fPartnersMax = MAX(m_fPartnersMax, 19);
		status << "2B4D0! Partner made a jump rebid of his " & szPSS & " suit to " & 
				  szPB & ", showing " & m_fPartnersMin & "-" & m_fPartnersMax & " points.\n";

		// get adjusted point count in support of partner
		fAdjPts = m_pHand->RevalueHand(REVALUE_DUMMY, nPartnersSuit, TRUE);
		m_fMinTPPoints = fAdjPts + m_fPartnersMin;
		m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;
		//
		int nJumpLevel = (nPartnersBid - nPreviousBid) / 5;


		//
		//---------------------------------------------------------
		// see if partner already jumped to game or beyond
		//
		if (nPartnersBid >= GetGameBid(nPartnersSuit))
		{
			// either raise to slam or pass
			m_nAgreedSuit = nPartnersSuit;
			if ((nPartnersBidLevel <= 5) &&
					 (m_fMinTPPoints >= PTS_SMALL_SLAM) &&
					 (numSupportCards >= 3))
			{
				m_nBid = MAKEBID(nPartnersSuit, 6);
				status << "B4D1! With a total of " & 
						  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " points in the partnership, go ahead and raise partner to a small slam at " &
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
			if ((nPartnersBidLevel <= 6) &&
					 (m_fMinTPPoints >= PTS_GRAND_SLAM) &&
					 (numSupportCards >= 3))
			{
				m_nBid = MAKEBID(nPartnersSuit, 7);
				status << "B4D2! With a total of " & 
						  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " points in the partnership, go ahead and raise partner to a grand slam at " &
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
			else
			{
				m_nBid = BID_PASS;
				status << "B4D3! With a total of " & 
						  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " points in the partnership, we have to defer to partner's unusual jump rebid and pass.\n";
				return ValidateBid(m_nBid);
			}
		}

		//
		//---------------------------------------------------------
		//
		// pass with < 24 total points
		//
		if (m_fMinTPPoints < PTS_GAME-1) 
		{
			m_nBid = BID_PASS;
			status << "B4D10! With a bare minimum holding of " & 
					  fCardPts & "/" & fPts & "/" & fAdjPts & 
					  " points in the hand, for a total in the partnership of " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points, we don't have quite enough for game, so pass.\n";
			return ValidateBid(m_nBid);
		}

		//
		//---------------------------------------------------------
		//
		// with 25-32 points, think game
		//

		// raise opener's major to 4 with 25-32 TPs and 2+ trumps
		if (RaisePartnersSuit(SUIT_MAJOR, RAISE_TO_4, PTS_GAME-1, PTS_SLAM-1, SUPLEN_2))
			return ValidateBid(m_nBid);

		// if partner made a double jump to game or higher in a major, 
		// leave him alone if holding 1 or more trumps and <= 29 pts
		if ((ISMAJOR(nPartnersSuit)) && (nPartnersBidLevel >= 4) && 
										(numSupportCards >= 1) &&
										(m_fMinTPPoints <= PTS_GAME+3) ) 
		{
			m_nBid = BID_PASS;
			status << "B4D16! Partner insists on playing in his " & szPSS & 
					  " suit and has jumped all the way to game, so let him play there.\n";
			return ValidateBid(m_nBid);
		}

		//
		//--------------------------------------------------------
		//
		// here, we have 24-29 pts and can't raise partner's major
		//

		// bid 3NT with 27-29 HCPs and all suits stopped
		// need 27+ total since partner may be counting dist points
		// with a long rebid suit
		if (nPartnersBidLevel < 4) 
		{
			if (BidNoTrump(LEVEL_3, PTS_NT_GAME+1, PTS_NT_GAME+3, FALSE,STOPPED_UNBID))
				return ValidateBid(m_nBid);
		}

		// rebid any good 6-card suit
		if (RebidSuit(SUIT_ANY,REBID_CHEAPEST, PTS_GAME+1, PTS_GAME+3, LENGTH_6,SS_STRONG))
			return ValidateBid(m_nBid);

		// bid a new suit if openable
		if (BidNextBestSuit(SUIT_ANY,SHIFT_CHEAPEST, PTS_GAME+1, PTS_GAME+3, LENGTH_6,SS_OPENABLE))
			return ValidateBid(m_nBid);
		
		// raise opener's minor to 4 with 2 or more trumps (forcing) and 25+ pts
		if (RaisePartnersSuit(SUIT_MINOR, RAISE_TO_4, PTS_MINOR_GAME-4, PTS_MINOR_GAME, SUPLEN_2))
			return ValidateBid(m_nBid);

		// if none of the above apply, pass with < 26 pts
		if (m_fMinTPPoints < PTS_GAME)
		{
			m_nBid = BID_PASS;
			if (nPartnersBid < GetGameBid(nPartnersSuit))
				status << "B4D18! with a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " pts in the partnership but no clear direction in the bidding, pass.\n";
			else
				status << "B4D18a! with a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " pts in the partnership, sign off on partner's " & szPB & " bid.\n";
			return ValidateBid(m_nBid);
		}
		else if (m_fMinTPPoints <= PTS_MINOR_GAME)
		{
			// we have 26-29 pts, so we wanna bid something
			if (nPartnersBid < BID_3NT)
			{
				// goto 3NT 
				m_nBid = BID_3NT;
				status << "B4D19a! We have " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " pts in the partnership, which is enough for game, but there is no suit agreement -- so we're forced to bid " &
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
			else
			{
				m_nBid = BID_PASS;
				status << "B4D19b! We have " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " pts in the partnership, but which is enough for game, but there is no suit agreement, and we can't bid 3NT -- so we're forced to pass.\n";
				return ValidateBid(m_nBid);
			}
		}

		//
		//---------------------------------------------------------
		//
		// with 30-36 points, think game or slam if a fit can be found
		//

		if ((m_fMinTPPoints >= PTS_SLAM-3) && (m_fMinTPPoints < PTS_GRAND_SLAM)) 
		{
			status << "B4D20! With " & fCardPts & "/" & fPts & "/" & fAdjPts & 
					  " pts in hand, for a total in the partnership of " & 
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " pts, we have the points to make game or possibly slam, but so far with no agreed on suit.\n";
		}
					
		// with 33+ total points and 3 or more trumps in support of
		// partner's suit, invoke Blackwood
		// (partner's jump rebid suit indicates 5 or 6 trumps)
		if ((m_fMinTPPoints >= PTS_SLAM) && (numSupportCards >= 3))
		{
			InvokeBlackwood(nPartnersSuit);
			return ValidateBid(m_nBid);
		}

		// raise partner's major to game with 30-32 pts and 2+ card support
		if (RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_4, PTS_MAJOR_GAME+4, PTS_SMALL_SLAM-1, SUPLEN_2))
			return ValidateBid(m_nBid);
					
		// raise partner's minor to game at the 5-level with 31-32 pts and 3+ card support
		if (RaisePartnersSuit(SUIT_MINOR,RAISE_TO_5, PTS_MINOR_GAME+2, PTS_SMALL_SLAM-1, SUPLEN_2))
			return ValidateBid(m_nBid);

		// rebid a good 6-card suit w/ 30-32 pts
		if (RebidSuit(SUIT_ANY,REBID_CHEAPEST, PTS_MINOR_GAME+1, PTS_SLAM-1, LENGTH_6,SS_STRONG))
			return ValidateBid(m_nBid);

		// else bid another suit with 30-36 TPs
		if (RebidSuit(SUIT_ANY,REBID_CHEAPEST, PTS_MINOR_GAME+1, PTS_SLAM-3))
			return ValidateBid(m_nBid);


		//
		//---------------------------------------------------------
		// with 37+ min total pts, think slam in any event
		//
		if (m_fMinTPPoints >= PTS_GRAND_SLAM) 
		{
			status << "B4D30! With " & fCardPts & "/" & fPts & "/" & fAdjPts & 
					  " pts in hand, for a total in the partnership of " & 
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & " pts, move towards slam.\n";
		}

		// 37+ TPs: bid a small slam with our own self-supporting suit
		if (RebidSuit(SUIT_ANY,REBID_AT_6,PTS_GRAND_SLAM,0,LENGTH_6,SS_SELFSUPPORTING, HONORS_4))
			return ValidateBid(m_nBid);

		// with 2 or more trumps,  and 37+ total points in support of
		// partner's suit, invoke Blackwood
		if ((m_fMinTPPoints >= PTS_GRAND_SLAM) && (numSupportCards >= 2)) 
		{
			InvokeBlackwood(nPartnersSuit);
			return ValidateBid(m_nBid);
		}

/*
		// with 2 or more trumps and 35+ total points in support of
		// partner's suit, bid 6 directly
		if ((m_fMinTPPoints >= PTS_SLAM+3) && (numSupportCards >= 2)) 
		{
			m_nBid = MAKEBID(nPartnersSuit,6);
			status << "B4D50! With " & fCardPts & "/" & fPts & "/" & fAdjPts & 
					  " pts in hand, for a total in the partnership of " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " pts, and with " & numSupportCards & 
					  "-card trump support, go directly to slam with a bid of " & 
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}
*/

		// with 37+ HCPs, check for a NT slam
		if (m_fMinTPCPoints >= PTS_GRAND_SLAM) 
		{
			InvokeBlackwood(NOTRUMP);
			return ValidateBid(m_nBid);
		}

		// with 33-36 total HCPs, bid a small slam in NT directly
		if (BidNoTrump(LEVEL_6, PTS_SMALL_SLAM, PTS_GRAND_SLAM-1, FALSE, STOPPED_UNBID))
			return ValidateBid(m_nBid);

		// with any other 37+ total point holding, bid another suit
		if (m_fMinTPPoints >= PTS_GRAND_SLAM) 
		{
			status << "B4D64! With " & fCardPts & "/"& fPts & 
					  " points in slam territory but no good suit fit and a hand not suitable for NT, bid another suit at " &
					  BTS(m_nBid) & ".\n";
			BidNextBestSuit(SUIT_ANY);
			return ValidateBid(m_nBid);
		}

		//
		//---------------------------------------------------------
		//
		// here, we've fallen through without hitting any of the 
		// above conditions, so handle generically
		//

		// bid another suit
		if (BidNextBestSuit(SUIT_ANY))
		{
			status << "B4D68! We have " & fCardPts & "/" & fPts &
					  " pts in hand but no good fit with partner and no rebiddable suit, so we have no choice but to shift to yet another suit.\n";
		}
		else
		{
			m_nBid = BID_PASS;
			status << "B4D69! We have " & fCardPts & "/" & fPts &
					  " pts in hand but no good fit with partner and no rebiddable suit, and we've run out of safe bidding room, so we're forced to pass.\n";
		}
		return ValidateBid(m_nBid);


	} // end of partner's jump rebid section






	//
	//==========================================================
	//
	// Rebidding after opener made a rebid of his original suit
	// after _our_ jump shift 
	// e.g., 1h -> 2s -> 3h, or even
	//       1h -> 2s -> 4h, showing 
	//
	//

	//
	if ( (nPartnersSuit == nPartnersPrevSuit) &&					// pard rebid his suit
						(nPreviousSuit != nPartnersSuit) &&			// we shifted last time
						((nPreviousBid - nPartnersPrevBid) > 5) )	// and it was a jump shift
	{
		//
		//---------------------------------------------------------
		//
		// this is a forcing situation -- we need to at least get to game
		//

		//
		status << "2B4K! Partner rebid his " & szPSS & " suit after our jump shift of " & 
				  szPVB & ", indicating his dislike for our suit." &
				  "  However, we still wish to get to at least game.\n";

		//
		// here, there's at least 30+ pts in the partnership
		//

		// with <= 32 total pts & a 6-card minor suit, rebid suit at game
		if (RebidSuit(SUIT_MINOR,REBID_AT_4,PTS_MINOR_GAME,PTS_SLAM-1,LENGTH_6))
			return ValidateBid(m_nBid);

		// or with <= 32 pts, raise opener's major to game with 3+ trumps
		if (RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_4,PTS_GAME,PTS_SLAM-1,SUPLEN_3))
			return ValidateBid(m_nBid);

		// alternatively with <= 32 pts, bid any another suit
		// (if partner bid below game level)
		if (nPartnersBid < GetGameBid(nPartnersSuit))
			if (BidNextBestSuit(SUIT_ANY,SHIFT_CHEAPEST,PTS_GAME,PTS_SLAM-1))
				return ValidateBid(m_nBid);

		// at this point, determine the better of our or partner's suit, or NT
		int nSuit = NONE;
		if (nPrefSuitStrength >= SS_ABSOLUTE || bPrefSuitIsSelfSupporting)
			nSuit = nPrefSuit;
		else if (numSupportCards >= 2)
			nSuit = nPartnersSuit;
		else
			nSuit = NOTRUMP;

		// if none of the above conditions apply, bid game with <= 32 pts
		if (m_fMinTPPoints < PTS_SLAM)
		{
			// see if game was already bid
			if (nPartnersBid >= GetGameBid(nPartnersSuit))
			{
				m_nBid = BID_PASS;
				status << "B4K5! with a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " pts in the partnership and no clear direction in the bidding, pass partner's " & szPB & " bid.\n";
				return ValidateBid(m_nBid);
			}

			// else bid game
			m_nBid = GetGameBid(nSuit);
			if (nSuit == nPrefSuit)
				status << "B4K10a! with a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " pts in the partnership and a self-supporting suit in "& szPrefS & ", bid " & BTS(m_nBid) & ".\n";
			else if (nSuit == nPartnersSuit)
				status << "B4K10b! with a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " pts in the partnership and " & numSupportCards & 
						  "-card support for partner's " & szPS & ", bid game at " & BTS(m_nBid) & ".\n";
			else 
				status << "B4K10c! with a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " pts in the partnership and no suit agreement, bid " & BTS(m_nBid) & ".\n";
			//
			return ValidateBid(m_nBid);
		}

		//
		//---------------------------------------------------------
		//
		// with 33+ total points, bid a slam
		//
		InvokeBlackwood(nSuit);
		return ValidateBid(m_nBid);
/*
		if (m_fMinTPPoints < PTS_GRAND_SLAM)
			m_nBid = MAKEBID(nSuit, 6);
		else 
			m_nBid = MAKEBID(nSuit, 7);
		//
		if (nSuit == nPrefSuit)
			status << "B4K12a! with a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " pts in the partnership and a self-supporting suit in "& szPrefS & ", bid slam at " & BTS(m_nBid) & ".\n";
		else if (nSuit == nPartnersSuit)
			status << "B4K12b! with a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " pts in the partnership and " & numSupportCards & 
					  "-card support for partner's " & szPS & ", bid slam at " & BTS(m_nBid) & ".\n";
		else 
			status << "B4K12c! with a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " pts in the partnership but no suit agreement, bid NT slam at " & BTS(m_nBid) & ".\n";
		//
		return ValidateBid(m_nBid);
*/
	}






	//
	//==========================================================
	//
	// Rebidding after opener made a rebid of 1NT
	// e.g., 1H -> 1S -> 1NT, showing 12-16 points
	// (mostly HCPs)
	//
	if ((nPartnersBid == BID_1NT) && (nPartnersPrevBid != BID_PASS)) {

		//
		m_fPartnersMin = MAX(m_fPartnersMin, 12);
		m_fPartnersMax = MAX(m_fPartnersMax, 16);

		// revalue hand for partner's PREVIOUS suit
		fAdjPts = m_pHand->RevalueHand(REVALUE_DUMMY, nPartnersPrevSuit, TRUE);
		m_fMinTPPoints = fAdjPts + m_fPartnersMin;
		m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;

		//
		status << "B4E00! Partner made a rebid of 1NT, showing " & m_fPartnersMin & "-" & m_fPartnersMax & 
				  " points with a balanced hand, for a total of " &
				  m_fMinTPPoints & "-" & m_fMaxTPPoints &
				  " pts in the partnership.\n";

		//
		//---------------------------------------------------------
		//

		// pass with fewer than 20 min total points
		if (m_fMinTPPoints < PTS_GAME-6) 
		{
			m_nBid = BID_PASS;
			status << "B4E10! With only " & fCardPts & "/" & fPts &
					  " pts in hand, for a total in the partnership of approx. " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " HCPs, making a game seems unlikely, so pass.\n";
			return ValidateBid(m_nBid);
		}

		//
		//--------------------------------------------------------
		//
		// here, we have 20-29 TPs
		//

		// rebid a six-card suit with 20-24 TPs
		if (RebidSuit(SUIT_ANY,REBID_CHEAPEST,PTS_GAME-6,PTS_GAME-2,LENGTH_6))
			return ValidateBid(m_nBid);

		// bid another openable suit at the 2-level with 20-24 TPs
		if (BidNextBestSuit(SUIT_ANY,SHIFT_CHEAPEST,PTS_GAME-6,PTS_GAME-2,LENGTH_4,SS_OPENABLE))
			return ValidateBid(m_nBid);

		// jump to four in a six-card major with 26-29 points
		if (RebidSuit(SUIT_MAJOR,REBID_AT_4,PTS_GAME,PTS_GAME+3,LENGTH_6))
			return ValidateBid(m_nBid);

		// raise opener's major with 20-24 Adj. points and 4 trumps
		if (RaisePartnersSuit(SUIT_MAJOR,RAISE_ONE,PTS_GAME-6,PTS_GAME-2,SUPLEN_4))
			return ValidateBid(m_nBid);

		// or raise opener's minor with 20-24 Adj. points and 5 trumps
		if (RaisePartnersSuit(SUIT_ANY,RAISE_ONE,PTS_GAME-6,PTS_GAME-2,SUPLEN_5))
			return ValidateBid(m_nBid);

		// or raise opener's major with 23-24 Adj. points and 3 trumps
		if (RaisePartnersSuit(SUIT_MAJOR,RAISE_ONE,PTS_GAME-3,PTS_GAME-2,SUPLEN_3))
			return ValidateBid(m_nBid);


		// with 23-25/26-29 total HCPs and all suits stopped, 		// bid 2NT/3NT
		if (BidNoTrump(LEVEL_2, PTS_NT_GAME-3, PTS_NT_GAME-1, TRUE,STOPPED_ALLOTHER))
			return ValidateBid(m_nBid);
		if (BidNoTrump(LEVEL_3, PTS_NT_GAME, PTS_NT_GAME+3, TRUE,STOPPED_ALLOTHER))
			return ValidateBid(m_nBid);

		// else with length in the minors, bid 2NT or 3NT
		// with slightly less
		if (((numCardsInSuit[CLUBS] >= 5) && (numHonorsInSuit[CLUBS] >= 2)) ||
		    ((numCardsInSuit[DIAMONDS] >= 5) && (numHonorsInSuit[DIAMONDS] >= 2))) 
		{
			if (BidNoTrump(LEVEL_2, PTS_NT_GAME-3, PTS_NT_GAME-2, FALSE, STOPPED_ALLOTHER))
				return ValidateBid(m_nBid);
			if (BidNoTrump(LEVEL_3, PTS_NT_GAME-1, PTS_NT_GAME+3, FALSE, STOPPED_ALLOTHER))
				return ValidateBid(m_nBid);
		}

		// with 25-29 adj points and a hand not fit for NT, 
		// jump to 3 in partner's major suit with 3 trumps 
		if (RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_3,PTS_GAME-1,PTS_GAME+3,SUPLEN_3))
			return ValidateBid(m_nBid);

		// or jump to 3 in partner's minor suit with 4 trumps and 25 pts
		if (RaisePartnersSuit(SUIT_MINOR,RAISE_TO_3,PTS_MINOR_GAME-4,PTS_MINOR_GAME+1,SUPLEN_4))
			return ValidateBid(m_nBid);

		// else jump in our major suit with 5 cards and 25+ pts
		if (RebidSuit(SUIT_MAJOR,JUMP_REBID_1,PTS_GAME,PTS_MAJOR_GAME+3,LENGTH_5))
			return ValidateBid(m_nBid);

		// else jump to 3 in a new suit (forcing to game) and 25+ pts
		if (BidNextBestSuit(SUIT_ANY,JUMP_SHIFT_1,PTS_GAME-1,PTS_GAME+4))
			return ValidateBid(m_nBid);

		// if we still haven't found a bid yet, 
		// bid 2NT w/ 24-25 HCPs, or 3NT with 26-32 and NO VOIDS
		if (numVoids == 0)
		{
			if (BidNoTrump(LEVEL_2,PTS_NT_GAME-2,PTS_NT_GAME-1,FALSE,STOPPED_DONTCARE))
				return ValidateBid(m_nBid);
			if (BidNoTrump(LEVEL_3,PTS_GAME,PTS_SLAM-1,FALSE,STOPPED_DONTCARE))
				return ValidateBid(m_nBid);
		}

		// if none of the above apply, pass with < 29 pts
		if (m_fMinTPPoints < PTS_MINOR_GAME)
		{
			m_nBid = BID_PASS;
			if (nPartnersBid < GetGameBid(nPartnersSuit))
				status << "B4E18! with a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " pts in the partnership but no clear direction in the bidding, pass.\n";
			else
				status << "B4E18a! with a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " pts in the partnership, sign off on partner's " & szPB & " bid.\n";
			return ValidateBid(m_nBid);
		}

		//
		//--------------------------------------------------------
		//
		// with 32+ total points, think slam
		//

		if (m_fMinTPPoints >= PTS_SLAM-1) 
		{
			status << "B4E20! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " points in the partnership, we want to explore slam opportunities.\n";
		}
		
		// jump to 3 of another suit
		if (BidNextBestSuit(SUIT_ANY,JUMP_SHIFT_1,PTS_MINOR_GAME+2))
			return ValidateBid(m_nBid);


		//
		//--------------------------------------------------------
		//
		// here, we've fallen through without meeting any of the above
		// conditions
		//

		// 20-24 TPs: bid a new suit
		if (m_fMinTPPoints <= PTS_GAME-2) 
		{
			if (BidNextBestSuit(SUIT_ANY))
			{
				status << "B4E30! We have " & fCardPts & "/" & fPts &
						  " pts in hand and " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " pts in the partnership, but no good fit with partner, no rebiddable suit, and no NT prospects, so we have no choice but to shift to another suit.\n";
			}
			else
			{
				m_nBid = BID_PASS;
				status << "B4E31! We have " & fCardPts & "/" & fPts &
						  " pts in hand and " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " pts in the partnership, but no good fit with partner, no rebiddable suit, and no more safe bidding room, we're forced to pass.\n";
			}
			return ValidateBid(m_nBid);
		}

		// 29+ pts here; jump in own suit
		status << "B4E32! We have " & fCardPts & "/" & fPts & 
				  "pts in hand and " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
				  "pts in the partnership, but have no good fit with partner and no rebiddable suit, so we have no choice but to shift to another suit.\n";
		RebidSuit(SUIT_ANY,JUMP_REBID_1);
		return ValidateBid(m_nBid);

	} // end of partner's 1NT rebid section









	//
	//==========================================================
	//
	// Rebidding after opener made a 2NT rebid, showing 
	// 15-19 pts and all suits stopped
	//   2NT after 2/1 response shows 15-16 points
	//   2NT after 1NT shows 16-19 points
	//   2NT after 1/1 response shows 17-19 points
	//
	//
	if (nPartnersBid == BID_2NT) 
	{

		// check point count
		CalcNoTrumpPoints();


		//
		//---------------------------------------------------------
		//
		// with < 23 total pts, pass
		//
		if (m_fMinTPPoints < PTS_GAME-3) 
		{
			m_nBid = BID_PASS;
			status << "B4F10! with " & fCardPts & "/" & fPts & 
					  " points in hand, and a total of " & 
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " in the partnership, we can't make game, so pass.\n";
			return ValidateBid(m_nBid);
		}


		// with 23-24 total pts and a long suit, rebid it
		//
		if (RebidSuit(SUIT_ANY,REBID_CHEAPEST,PTS_GAME-3,PTS_GAME-2,LENGTH_6))
			return ValidateBid(m_nBid);
		
		// or with 23-24 total pts, support partner's prev major suit 
		// at the 3-level with 3 trumps
		if (RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_3,PTS_GAME-3,PTS_GAME-2,SUPLEN_3))
			return ValidateBid(m_nBid);
		// or raise a minor with the same number of pts but with 4 cards
		if (RaisePartnersSuit(SUIT_MINOR,RAISE_TO_3,PTS_GAME-3,PTS_GAME-2,SUPLEN_4))
			return ValidateBid(m_nBid);

		// otherwise pass with < 26 pts and no good suit
		if (m_fMinTPPoints < PTS_GAME) 
		{
			m_nBid = BID_PASS;
			status << "B4F14! with " & fCardPts & "/" & fPts &
					  " points in hand, and a total of " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " in the partnership, sign off on partner's contract of 2NT.\n";
			return ValidateBid(m_nBid);
		}

		//
		//---------------------------------------------------------
		//
		// here, we have 26-31 total points minimum
		//

		// with 26-32 total pts & a 6-card major suit, bid game
		if (RebidSuit(SUIT_MAJOR,REBID_AT_4,PTS_GAME,PTS_SLAM-1,LENGTH_6))
			return ValidateBid(m_nBid);

		// with 26-32 total HCPs, bid 3NT
		// (assume partner has all suits stopped)
		if (BidNoTrump(LEVEL_3,PTS_GAME,PTS_SLAM-1,FALSE,STOPPED_DONTCARE))
			return ValidateBid(m_nBid);

		// with 28-32 total pts & a 6-card minor suit, bid game
		if (RebidSuit(SUIT_MINOR,REBID_AT_4,PTS_MINOR_GAME-1,PTS_SLAM-1,LENGTH_6))
			return ValidateBid(m_nBid);

		// 27-32 total pts & a 7-card minor suit also qualifies
		if (RebidSuit(SUIT_MINOR,REBID_AT_4,PTS_MINOR_GAME-2,PTS_SLAM-1,LENGTH_7))
			return ValidateBid(m_nBid);

		// or with 25-32 pts, raise opener's major to game with 3+ trumps
		if (RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_4,PTS_GAME-1,PTS_SLAM-1,SUPLEN_3))
			return ValidateBid(m_nBid);

		// alternatively with 25-32 pts, bid any another suit
		// (if partner bid below game level)
		if (nPartnersBid < GetGameBid(nPartnersSuit))
			if (BidNextBestSuit(SUIT_ANY,SHIFT_CHEAPEST,PTS_GAME-1,PTS_SLAM-1))
				return ValidateBid(m_nBid);

		// if none of the above apply, pass with < 30 pts
		if (m_fMinTPPoints < PTS_SLAM-3)
		{
			m_nBid = BID_PASS;
			if (nPartnersBid < GetGameBid(nPartnersSuit))
				status << "B4D18! with a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " pts in the partnership but no clear direction in the bidding, pass.\n";
			else
				status << "B4D18a! with a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " pts in the partnership, sign off on partner's " & szPB & " bid.\n";
			return ValidateBid(m_nBid);
		}


		//
		// try bidding NT
		//
		if (BidNoTrumpAsAppropriate(TRUE,STOPPED_DONTCARE))
			return ValidateBid(m_nBid);
/*
		// explore slam with an invitational bid
		if (m_fMinTPCPoints >= PTS_SLAM) 
		{
			m_nBid = BID_4NT;	// not Blackwood
			status << "B4F20! with " & fCardPts & 
					  " HCPs in hand, for a total of approx. " &
					  m_fMinTPCPoints & "-" &  m_fMaxTPCPoints &
					  " in the partnership, explore slam possibilities with an inviational bid of " &
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}
*/

		//
		//---------------------------------------------------------
		//
		// here' we've dropped through
		//

		// with 23-25 pts, bid our own suit
		if (RebidSuit(SUIT_ANY,REBID_CHEAPEST,PTS_GAME-3,PTS_GAME-1))
			return ValidateBid(m_nBid);

		// with 26-31 pts, raise partner's suit to game 
		// w/ 2 trumps or bid 3NT 
		if (RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_GAME,PTS_GAME,PTS_SLAM-1,SUPLEN_2))
			return ValidateBid(m_nBid);
		if (BidNoTrump(LEVEL_3,PTS_NT_GAME,PTS_SLAM-1,FALSE,STOPPED_DONTCARE))
			return ValidateBid(m_nBid);
		if (RaisePartnersSuit(SUIT_MINOR,RAISE_TO_GAME,PTS_MINOR_GAME,PTS_SLAM-1,SUPLEN_2))
			return ValidateBid(m_nBid);

		// else bid another suit, any suit
		status << "B4F30! We have " & fCardPts & "/" & fPts &
				  " pts in hand but no good fit with partner and no rebiddable suit, so we have no choice but to shift to another suit.\n";
		RebidSuit(SUIT_ANY,REBID_CHEAPEST,0,0,LENGTH_4,SS_NONE);
		return ValidateBid(m_nBid);

	} // end of 2NT response section








	//
	//==========================================================
	//
	// Rebidding after opener made a 3NT or higher NT rebid 
	// (but not Blackwood NT) after our own response of 1x or higher
	//
	// possibilities:
	// --------------
	//   1x  -> 3NT
	//   2x  -> 3NT
	//   1NT -> 3NT
	//   1NT -> 4NT/5NT/6NT/7NT
	//   2NT -> 3NT
	//   2NT -> 4NT/5NT/6NT/7NT
	//   xNT -> xNT
	//
	if ((nPartnersSuit == NOTRUMP) && (nPartnersBidLevel >= 3)) 
	{

		// check point counts 
		CalcNoTrumpPoints();


		//
		//---------------------------------------------------------
		//
		// with a max possible partnership total of < 30 points, 
		// pass or rebid a really good major
		//
		if (m_fMinTPCPoints < PTS_MINOR_GAME+2)		// < 27 pts
		{
			// only rebid a solid 6-card major w/ 26 pts
			if (nPartnersBid <= BID_4NT)
				if (RebidSuit(SUIT_MAJOR,REBID_AT_4,PTS_GAME-3,0,LENGTH_6,SS_STRONG | SS_SOLID))
					return ValidateBid(m_nBid);

			// or a solid 7-card minor w/ 25+ pts
			if (nPartnersBid <= BID_4NT)
				if (RebidSuit(SUIT_MINOR,REBID_AT_4,PTS_MINOR_GAME-4,0,LENGTH_7,SS_STRONG | SS_SOLID))
					return ValidateBid(m_nBid);

			// else sign off at game
			m_nBid = BID_PASS;
			status << "B4G40! With approx. " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
					  " total HCPs in the partnership, sign off at game and pass.\n";
			return ValidateBid(m_nBid);
		}


		//
		//---------------------------------------------------------
		//
		// with 30-32 points, look into a possible slam
		//
		if ((m_fMinTPCPoints >= PTS_SLAM-3) && (PTS_SLAM <= PTS_SLAM-1) &&
								(nPartnersBid <= BID_4NT)) 
		{
			if ((nPartnersBid == BID_4NT) && (m_bInvitedToSlam))
			{
				// partner is inviting a slam, but we don't have enough
				m_nBid = BID_PASS;
				status << "B4G42! With " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
						   " total HCPs in the partnership, we don't have quite enough for a slam, so we have to decline partner's invitation and pass.\n";
			}
			else
			{
				m_nBid = MAKEBID(NOTRUMP,nPartnersBidLevel+1);
				status << "B4G44! With " & m_fMinTPCPoints & 
						   "+ total HCPs in the partnership, look into slam opportunities by bidding an invitational " &
				 		   BTS(m_nBid) & ".\n";
			}
			return ValidateBid(m_nBid);
		}


		//
		//---------------------------------------------------------
		//
		// else we have 33+ pts -- try for a slam
		//
		// note that we can't use Blackwood in this situation
		// (i.e., after partner's NT bid) 
		//

		// check if partner has already bid slam
		if (nPartnersBid >= BID_6NT) 
		{
			m_nBid = BID_PASS;
			status << "B4G48! With " & m_fMinTPPoints & 
					  "+ total partnership points, sign off on partner's slam bid of " & 
					  szPB & " and pass.\n";
			return ValidateBid(m_nBid);
		}

		// else try for slam ourselves
		if (m_fMinTPCPoints >= PTS_SLAM) 
		{
			status << "B4G50! With " & m_fMinTPPoints & 
					  "+ total partnership points, bid a small slam.\n";
		}

		// bid slam with a solid 7-card major
		if (m_fMinTPPoints < PTS_GRAND_SLAM)
			if (RebidSuit(SUIT_MAJOR,REBID_AT_6,PTS_SLAM,0,LENGTH_7,SS_STRONG))
				return ValidateBid(m_nBid);
		// or at NT otherwise
		if (BidNoTrump(LEVEL_6,PTS_SLAM,PTS_SLAM+3,FALSE,STOPPED_DONTCARE))
			return ValidateBid(m_nBid);

		//
		// grand slam time
		//

		// again, look for a 7-card major
		if (RebidSuit(SUIT_MAJOR,REBID_AT_7,PTS_GRAND_SLAM,0,LENGTH_7,SS_STRONG))
			return ValidateBid(m_nBid);
		// or NT otherwise
		if (BidNoTrump(LEVEL_7,PTS_GRAND_SLAM,40,FALSE,STOPPED_DONTCARE))
			return ValidateBid(m_nBid);

	} // end of 3NT+ section








	//
	//==========================================================
	//
	// Rebidding after opener made a single raise of our suit
	// in stage 3
	// e.g., 1H -> 1S -> 2S, showing 12-16 points, or
	//       1S -> 2H -> 3H, or
	//       1S -> 3S -> 4S, or 
	//       1S -> 2S -> 3S, invitational to game
	//
	if ((nPartnersSuit == nPreviousSuit) && 
				(nPartnersBidLevel == nPreviousBidLevel+1) &&
					(nPartnersSuit != NOTRUMP) && 
						(nPartnersBidLevel <= 4)) 
	{
		// see if partner raised our suit after we jump shifted 
		// in the first round
		// e.g., 1C -> 2S -> 3S, or
		//       1D -> 3C -> 4C
		if ( ((nPreviousBid - nPartnersPrevBid) > 5) &&
			 (nPreviousSuit != nPartnersPrevSuit) )
		{
			int nSuit;
			// see whether we prefer our own bid suit or partner's suit
			// (our jump shift suit might have been artificial)
			// condition 1: partner's prev suit is a major, our suit is a minor, and we have good support for partner' ssuit
			// condition 2: we have more support for partner's suit than for own suit
			// condition 3: we have 4+ strong support cards for partner's suit, and
			//              have < 6 cards in our own suit
			// condition 4: we have 5+ good support cards for partner's suit, and
			//              have < 6 cards in our own suit
//			if ( (ISMAJOR(nPartnersPrevSuit) && (nSuitStrength[nPartnersPrevSuit] >= SS_MODERATE_SUPPORT)) ||
//				 (ISMINOR(nPartnersPrevSuit) && (nSuitStrength[nPartnersPrevSuit] >= SS_STRONG_SUPPORT)) )
			if ( (m_nIntendedSuit == nPartnersPrevSuit) || (m_nAgreedSuit == nPartnersPrevSuit) ||
				 (ISMAJOR(nPartnersPrevSuit) && ISMINOR(nPreviousSuit) && (nSuitStrength[nPartnersPrevSuit] >= SS_MODERATE_SUPPORT)) ||
				 ((nSuitStrength[nPartnersPrevSuit] > nSuitStrength[nPreviousSuit]) && (nSuitStrength[nPartnersPrevSuit] >= SS_MODERATE_SUPPORT)) ||
				 ((numCardsInSuit[nPartnersPrevSuit] >= 4) && (nSuitStrength[nPartnersPrevSuit] >= SS_STRONG_SUPPORT) && (numCardsInSuit[nPartnersPrevSuit] < 6)) ||
				 ((numCardsInSuit[nPartnersPrevSuit] >= 5) && (nSuitStrength[nPartnersPrevSuit] >= SS_GOOD_SUPPORT) && (numCardsInSuit[nPartnersPrevSuit] < 6)) )
			{
				// return to partner's previous suit w/ good support
				nSuit = nPartnersPrevSuit;
				m_nAgreedSuit = nPartnersPrevSuit;
				status << "2B4H00! Though partner raised our " & szPVSS & 
						  " suit after our jump shift, we really want to play in partner's " &
						  szPPSS & " suit.\n";
				// no need to revalue hand; already revalued as dummy
			}
			else
			{
				// else stick with our own suit, which partner raised
				nSuit = nPreviousSuit;
				m_nAgreedSuit = nPreviousSuit;
				status << "2B4H01! Partner raised our " & szPVS & 
						  " after our jump shift, so stick with that suit.\n";
				// revalue hand for our suit as declarer
				fAdjPts = m_pHand->RevalueHand(REVALUE_DECLARER, nPreviousSuit, TRUE);
			}
			
			// tally points
			m_fMinTPPoints = fAdjPts + m_fPartnersMin;
			m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
			m_fMinTPCPoints = fCardPts + m_fPartnersMin;
			m_fMaxTPCPoints = fCardPts + m_fPartnersMax;

			// now that we have the correct suit, we can proceed
			// and bid game or slam (invoking Blackwood)
			if (m_fMinTPPoints < PTS_SLAM)
			{
				if (nPartnersBid < GetGameBid(nSuit))
				{
					m_nBid = GetGameBid(nSuit);
					status << "B4H04! Now with " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " pts in the partnership, raise partner to game at a bid of " &
							  BTS(m_nBid) & ".\n";
				}
				else
				{
					m_nBid = BID_PASS;
					status << "B4H05! And with " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " pts in the partnership, sign off on partner's bid of " &
							  szPB & ".\n";
				}
			}
			else
			{
				InvokeBlackwood(nSuit);
			}
			return ValidateBid(m_nBid);
		}
		else
		{
			// else we didn't jump shift last time
			m_nAgreedSuit = nPreviousSuit;
			// revalue hand for our suit as declarer
			fAdjPts = m_pHand->RevalueHand(REVALUE_DECLARER, nPreviousSuit, TRUE);
			// tally points
			m_fMinTPPoints = fAdjPts + m_fPartnersMin;
			m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
			m_fMinTPCPoints = fCardPts + m_fPartnersMin;
			m_fMaxTPCPoints = fCardPts + m_fPartnersMax;
		}

		//
		//
		if (nPartnersBid >= GetGameBid(nPartnersSuit))
		{
			// partner bid game -- e.g., 1S -> 3S -> 4H
			m_fPartnersMin = MAX(m_fPartnersMin, 12);
			if (m_fPartnersMax < MIN(22, 40 - fCardPts))
				m_fPartnersMax = MIN(22, 40 - fCardPts);
			status << "2B4H10! Partner has raised to game, showing at least " &
					  m_fPartnersMin & " points.\n";
		}
		else if (nPartnersBidLevel == 2) 
		{
			// 1 -> 2 raise: 12-16 pts
			m_fPartnersMin = MAX(m_fPartnersMin, 12);
			m_fPartnersMax = MAX(m_fPartnersMax, 16);
			status << "2B4H11! Partner made a single raise of our " & szPVSS & 
					  " suit, showing " & m_fPartnersMin & "-" & m_fPartnersMax & 
					  " points with 3+ trump cards.\n";
		} 
		else if (nPartnersSuit == nPartnersPrevSuit) 
		{
			// 1->2->3 invitational raise: ~ 16-18 pts
			m_fPartnersMin = MAX(m_fPartnersMin, 16);
			m_fPartnersMax = MAX(m_fPartnersMax, 18);
			status << "2B4H12! Partner raised the agreed suit to the 3-level, which is invitational towards game and indicates approximately " &
					  m_fPartnersMin & "-" & m_fPartnersMax &
					  " points.\n";
		} 
		else 
		{
			// 2 -> 3 raise: 12-18 pts
			m_fPartnersMin = MAX(m_fPartnersMin, 12);
			m_fPartnersMax = MAX(m_fPartnersMax, 18);
			status << "2B4H14! Partner made a single raise of our "& szPVB & 
					  " bid to " & szPB & ", showing " &
					  m_fPartnersMin & " -" & m_fPartnersMax &
					  " points and 3+ trump cards.\n";
		}

		//
		//---------------------------------------------------------
		//

		//
		// if partner raised to the 2-level, pass with fewer then 
		// 22 min total points, or with 21 total pts and a minor
		if (nPartnersBidLevel == 2) 
		{
/*
			if ((m_fMinTPPoints < PTS_GAME-4) ||
				((m_fMinTPPoints <= PTS_GAME-5) && (ISMINOR(nPreviousSuit)))) 
			{
				m_nBid = BID_PASS;
				status << "B4H18! With only " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  " pts in hand, for a total in the partnership of " &
						   m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points, a game is unlikely, so pass.\n";								   
				return ValidateBid(m_nBid);
			}
*/
			if (m_fMinTPPoints < PTS_GAME-4)
			{
				m_nBid = BID_PASS;
				status << "B4H18! With only " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  " pts in hand, for a total in the partnership of " &
						   m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points, a game is unlikely, so pass.\n";								   
				return ValidateBid(m_nBid);
			}
		} 
		else if (nPartnersBidLevel == 3)
		{
			if (nPartnersSuit == nPartnersPrevSuit)
			{
				// if partner made an invitational game raise (1->2->3 of a suit)
				// indicating 16 points, 
				// bid game with 26 min total pts, or pass otherwise
				if ((ISMAJOR(nPreviousSuit)) && (m_fMinTPPoints >= PTS_GAME)) 
				{
					m_nBid = MAKEBID(nPreviousSuit,4);
					status << "B4H20! And with " & fCardPts & "/" & fPts & "/" & fAdjPts &
							  " pts in hand, for a total in the partnership of approx. " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " points, try for a game by bidding " & BTS(m_nBid) & ".\n";  
					return ValidateBid(m_nBid);
				} 
				else 
				{
					m_nBid = BID_PASS;
					status << "B4H24! But with only " & fCardPts & "/" & fPts & "/" & fAdjPts &
							  " pts in hand, for a total in the partnership of " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " points, game is unlikely, so pass.\n";
					return ValidateBid(m_nBid);
				}

			} 
			else 
			{

				// else partner raised our 2-bid to 3; pass with < 10 pts
				// we require a higher minimum here 'cuz we're at a higher
				// level (3-level instead of 2)
				if (m_fMinTPPoints < PTS_GAME-3) 
				{
					m_nBid = BID_PASS;
					status << "B4H30! With only " & fCardPts & "/" & fPts & "/" & fAdjPts &
							  " pts in hand, for a total in the partnership of " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " points, a game is unlikely, so pass.\n"; 
					return ValidateBid(m_nBid);
				}
			}
		}
		else
		{
			// else partner bid game or higher after our 
			// invitational double raise
			// continue on to slam with 33+ pts, or pass otherwise
			if (m_fMinTPPoints >= PTS_SLAM) 
			{
				InvokeBlackwood(nPreviousSuit);
				status << "B4H32! With a total of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points in the partnership and a good trump suit, explore slam possiblities.\n"; 
				return ValidateBid(m_nBid);
			}
			else
			{
				m_nBid = BID_PASS;
				status << "B4H33! With a total of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points in the partnership, sign off at partner's " &
						  (IsGameBid(nPartnersBid)? "game " : "") &
						  "bid and pass.\n"; 
				return ValidateBid(m_nBid);
			}
		}


		//
		//--------------------------------------------------------
		//
		// here, we have at least 22+ min total points
		//

		// 22-23 pts: bid 3 in a 6-card major suit
		if (RebidSuit(SUIT_MAJOR,REBID_CHEAPEST,PTS_GAME-4,PTS_GAME-3,LENGTH_6))
			return ValidateBid(m_nBid);

		// 24-32 pts: bid game in a 6-card major suit
		if (RebidSuit(SUIT_MAJOR,REBID_AT_4,PTS_GAME-2,PTS_SLAM-1,LENGTH_6))
			return ValidateBid(m_nBid);

		// 26-32 pts: bid game in a 5-card major
		if (RebidSuit(SUIT_MAJOR,REBID_AT_4,PTS_GAME,PTS_SLAM-1,LENGTH_5,SS_OPENABLE))
			return ValidateBid(m_nBid);

		// 22-26 pts: bid 3 in a 4-card major
		if (RebidSuit(SUIT_MAJOR,REBID_AT_3,PTS_GAME-4,PTS_GAME,LENGTH_4,SS_OPENABLE))
			return ValidateBid(m_nBid);

		// 27-32 pts: bid game in a 4-card major
		if (RebidSuit(SUIT_MAJOR,REBID_AT_4,PTS_GAME+1,PTS_SLAM-1,LENGTH_4,SS_OPENABLE))
			return ValidateBid(m_nBid);

		// 23-25 pts: bid 3 in a 5-card minor
		if (RebidSuit(SUIT_MINOR,REBID_AT_3,PTS_MINOR_GAME-6,PTS_MINOR_GAME-4,LENGTH_5))
			return ValidateBid(m_nBid);

		// 26-28 pts: bid 4 in a 5-card minor
		if (RebidSuit(SUIT_MINOR,REBID_AT_4,PTS_MINOR_GAME-3,PTS_MINOR_GAME-1,LENGTH_5))
			return ValidateBid(m_nBid);

		// 29-32 pts: bid game in a 5-card minor
		if (RebidSuit(SUIT_MINOR,REBID_AT_5,PTS_MINOR_GAME,PTS_SLAM-1,LENGTH_5))
			return ValidateBid(m_nBid);

		// 30-32 pts: bid game in a 4-card minor
		if (RebidSuit(SUIT_MINOR,REBID_AT_5,PTS_MINOR_GAME+1,PTS_SLAM-1,LENGTH_4,SS_OPENABLE))
			return ValidateBid(m_nBid);

		// 23-25 HCPs: bid 2NT 
		if (nPartnersBidLevel == 2)
			if (BidNoTrump(LEVEL_2,PTS_NT_GAME-3,PTS_GAME-1,FALSE,STOPPED_DONTCARE))
				return ValidateBid(m_nBid);

		// 26-32 HCPs: bid 3NT with all suits stopped
		if (BidNoTrump(LEVEL_3,PTS_NT_GAME,PTS_SLAM-1,FALSE,STOPPED_UNBID))
			return ValidateBid(m_nBid);

		// 26-27 HCPs: bid 2NT without stoppers in all suits
		if (BidNoTrump(LEVEL_2,PTS_NT_GAME,PTS_NT_GAME+1,FALSE,STOPPED_DONTCARE))
			return ValidateBid(m_nBid);

		// 28-32 HCPs: bid 3NT without all stoppers
		if (BidNoTrump(LEVEL_3,PTS_NT_GAME+2,PTS_SLAM-1,FALSE,STOPPED_DONTCARE))
			return ValidateBid(m_nBid);

		// else pass
		if (m_fMinTPPoints < PTS_SLAM) 
		{
			m_nBid = BID_PASS;
			status << "B4H35! We have a total of approx. " & 
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " in the partnership, but we do not have a rebiddable suit, nor do we have enough stoppers for a NT contract, so we have to sign off on partner's " & szPB & " bid.\n";
			return ValidateBid(m_nBid);
		}

		//
		//--------------------------------------------------------
		//
		// with 33+ pts, think slam
		//

		// with 33+ pts, invoke Blackwood
		if (m_fMinTPPoints >= PTS_SLAM) 
		{
			InvokeBlackwood(nPreviousSuit);
			return ValidateBid(m_nBid);
		}


		//
		//--------------------------------------------------------
		//
		// here, we fell through
		//

		// with no good options, pass
		m_nBid = BID_PASS;
		status << "B4H50! With no good options, pass.\n";
//		if (BidNextBestSuit(SUIT_ANY))
			return ValidateBid(m_nBid);

	} // end if single raise section




	




	//
	//==========================================================
	//
	// Rebidding after opener made a double raise of our suit
	// e.g., 1H -> 1S -> 3S, or
	//       1S -> 2H -> 4H, showing 15-21 pts (17-21 for a minor)
	//
	//
	if ((nPartnersSuit == nPreviousSuit) && 
			(nPartnersSuit != NOTRUMP) && 
				(nPartnersBidLevel == nPreviousBidLevel+2)) 
	{
		// value partner's holdings
		if (ISMAJOR(nPreviousSuit)) 
		{
			m_fPartnersMin = 15;
			m_fPartnersMax = 21;
		} 
		else 
		{
			m_fPartnersMin = 17;
			m_fPartnersMax = 21;
		}
		status << "2B4M00! Partner made a double raise of our " & szPVSS &	
				  " suit to " & szPB & ", showing " & 
				  m_fPartnersMin & "-" & m_fPartnersMax &
				  " points.\n";

		//
		// see if we had shifted last time while intending to support partner
		// or if partner's prev suit is preferable to the one we shifted to 
		//
		if ( (m_nIntendedSuit == nPartnersPrevSuit) ||
			 (ISMAJOR(nPartnersPrevSuit) && ISMINOR(nPreviousSuit) && (nSuitStrength[nPartnersPrevSuit] >= SS_MODERATE_SUPPORT)) ||
			 ((nSuitStrength[nPartnersPrevSuit] > nSuitStrength[nPreviousSuit]) && (nSuitStrength[nPartnersPrevSuit] >= SS_MODERATE_SUPPORT) && ISMAJOR(nPartnersPrevSuit)) )
		{
			// revalue hand for partner's prev suit
			m_nAgreedSuit = nPartnersPrevSuit;
			// this should already have been done
//			fAdjPts = m_pHand->RevalueHand(REVALUE_DUMMY, nPartnersPrevSuit, TRUE);
			status << "B4M02! Though partner raised our " & szPVSS & 
					  " suit after our shift response, we really want to play in partner's " &
					  szPPSS & " suit.\n";
		}
		else
		{
			// revalue hand for our own prev suit that partner just raised
			m_nAgreedSuit = nPreviousSuit;
			fAdjPts = m_pHand->RevalueHand(REVALUE_DECLARER, nPreviousSuit, TRUE);
		}

		// add up totals
		m_fMinTPPoints = fAdjPts + m_fPartnersMin;
		m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;

		//
		//---------------------------------------------------------
		//
		// move towards game with 26-32 pts, or to slam with 
		// anything higher
		//

		// < 24 min total pts? sign off at whatever bid partner made
		if (m_fMinTPPoints < PTS_GAME-2) 
		{
			//
			m_nBid = BID_PASS;
			status << "B4M10! With " & fCardPts & "/" & fPts & "/" & fAdjPts &
					  " points in hand, for a total in partnership of " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " points, sign off at partner's bid of " & szPB & ".\n";
			return ValidateBid(m_nBid);
		}

		// < 33 pts and partner bid game? sign off
		if ((m_fMinTPPoints < PTS_SLAM) && IsGameBid(nPartnersBid))
		{
			//
			m_nBid = BID_PASS;
			status << "B4M20! With " & fCardPts & "/" & fPts & "/" & fAdjPts &
					  " points in hand, for a total in partnership of " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " points, sign off at partner's bid of " & szPB & ".\n";
			return ValidateBid(m_nBid);
		}

		// 23-25 min total pts?  show a support suit if possible
		if (m_fMinTPPoints < PTS_GAME)
		{
			if (nPartnersBidLevel < 3)
			{
				// show a support suit (game try)
				nSuit = GetNextBestSuit(nPreviousSuit, nPartnersPrevSuit);
				m_nBid = GetCheapestShiftBid(nSuit);
				status << "B4M14! With " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points in hand, for a total in partnership of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points, invite game by bidding the " & STSS(nSuit) & 
						  " support suit at " & BTS(m_nBid) & ".\n";
			}
			else
			{
				// gotta pass
				m_nBid = BID_PASS;
				status << "B4M15! We have " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points in hand, for a total in partnership of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points, so we want to invite game, but we're at too high a level to show a support suit, so we have to pass.\n";
			}
			return ValidateBid(m_nBid);
		}

		//
		// else with 26-32 TPs, bid game or try for slam
		//
		if (m_fMinTPPoints < PTS_SLAM) 
		{
			// bid game with 26-32 TPs and a major
			if (RebidSuit(MakeDirectSuit(m_nAgreedSuit),REBID_AT_4,PTS_GAME,PTS_SLAM-1,LENGTH_ANY,SS_MARGINAL_OPENER))
				return ValidateBid(m_nBid);
			// bid game with 29-32 TPs and a minor
			if (RebidSuit(MakeDirectSuit(m_nAgreedSuit),REBID_AT_5,PTS_MINOR_GAME,PTS_SLAM-1,LENGTH_ANY,SS_MARGINAL_OPENER))
				return ValidateBid(m_nBid);
			// or bid a minor at the 4-level with 26-29 TPs
			if (RebidSuit(MakeDirectSuit(m_nAgreedSuit),REBID_AT_4,PTS_MINOR_GAME-3,PTS_MINOR_GAME,LENGTH_ANY,SS_MARGINAL_OPENER))
				return ValidateBid(m_nBid);
			// if here, we're somehow stuck with a weak suit, so correct to NT
			if (pDOC->GetLastValidBid() < BID_3NT)
			{
				m_nBid = BID_3NT;
				status << "B4M20! Somehow partner raised our weak suit, so correct to " & 
						  BTS(m_nBid) & ".\n";
			}
			else
			{
				// else pass
				m_nBid = BID_PASS;
				status << "B4M21! Without enough strength to move further, we pass.\n";
			}
			return ValidateBid(m_nBid);
		}

		//
		// else with 33+ pts, it's slam time -- bid Blackwood
		//
		InvokeBlackwood(m_nAgreedSuit);
		return ValidateBid(m_nBid);

	} // end of double raise section







	//
	//==========================================================
	//
	// Rebidding after opener made a triple raise
	// e.g., 1H -> 1S -> 4S, or
	//       1S -> 2D -> 5D, showing 17-21 pts and 4 trumps
	//
	//
	if ((nPartnersSuit == nPreviousSuit) && 
				(nPartnersBidLevel == nPreviousBidLevel+3)) 
	{

		//
		m_fPartnersMin = 17;
		m_fPartnersMax = 21;
		status << "2B4N00! Partner made a triple raise of our " & szPVSS & " suit to " & 
		  		  szPB & ", showing " & m_fPartnersMin & "-" & m_fPartnersMax &
				  " points and 4+ trumps.\n";

		// revalue hand for our suit
		m_nAgreedSuit = nPreviousSuit;
		fAdjPts = m_pHand->RevalueHand(REVALUE_DECLARER, nPreviousSuit, TRUE);
		m_fMinTPPoints = fAdjPts + m_fPartnersMin;
		m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;

		//
		//---------------------------------------------------------
		//
		// partner has already bid game; move towards slam with 
		// 33+ total points
		//
		if (m_fMinTPPoints < PTS_SLAM) 
		{
			//
			if (ISMAJOR(nPreviousSuit)) 
			{
				m_nBid = BID_PASS;
				status << "B4N10! With " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points in hand, for a total in partnership of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points, sign off at partner's game bid of " & szPB & ".\n";
				return ValidateBid(m_nBid);
			}

			// else this is a minor
			if ((nPartnersBidLevel >= 5) || (m_fMinTPPoints < PTS_MINOR_GAME)) 
			{
				// if partner bid game, or we don't have values
				// for game, just pass
				m_nBid = BID_PASS;
				status << "B4N12! With " & fCardPts & "/" & fPts & "/" & fAdjPts & 
						  " points in hand, for a total in partnership of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points, sign off at partner's bid of " & szPB & ".\n";
				return ValidateBid(m_nBid);
			} 
			else 
			{
				// else raise to game
				m_nBid = MAKEBID(nPreviousSuit,5);
				status << "B4N14! With " & fCardPts & "/" & fPts & "/" & fAdjPts & 
						  " points in hand, for a total in partnership of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points, raise partner's bid to game at " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

		}

		//
		// else try for slam -- bid Blackwood
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
				status << "B4N18a! And with " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					" total partnership points, we can bid a grand slam at " & 
					BTS(m_nBid) & ".\n";
			}
			else if (nPartnersBidLevel < 6)
			{
				m_nBid = MAKEBID(m_nAgreedSuit, 6);
				status << "B4N18b! And with " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					" total partnership points, we can bid a small slam at " & 
					BTS(m_nBid) & ".\n";
			}
			else
			{
				m_nBid = BID_PASS;
				status << "B4N19! And with " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
				 		 " total partnership points, we sign off on partner's " &
		 				szPB & " slam attempt.\n";
			}
		}
		return ValidateBid(m_nBid);
	
	} // end of triple raise section





	//
	//==========================================================
	//
	// Rebidding after opener raised our suit 4 or more levels --
	// unlikely, but possible
	// e.g., 1C -> 1D -> 5D, or
	//       1S -> 2H -> 6H
	//
	if ((nPartnersSuit == nPreviousSuit) && 
				((nPartnersBidLevel - nPreviousBidLevel) >= 4) ) 
	{
		//
		m_nAgreedSuit = nPreviousSuit;

		//
		//---------------------------------------------------------
		// either raise to slam or pass
		//
		if ((nPartnersBidLevel <= 5) &&
				 (m_fMinTPPoints >= PTS_SLAM) && (numSupportCards >= 3))
		{
			m_nBid = MAKEBID(nPartnersSuit, 6);
			status << "B4X1! With a total of " & 
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points in the partnership, go ahead and raise partner to a small slam at " &
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}
		if ((nPartnersBidLevel <= 6) &&
				 (m_fMinTPPoints >= PTS_GRAND_SLAM) && (numSupportCards >= 3))
		{
			m_nBid = MAKEBID(nPartnersSuit, 7);
			status << "B4X2! With a total of " & 
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points in the partnership, go ahead and raise partner to a grand slam at " &
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}
		else
		{
			m_nBid = BID_PASS;
			status << "B4X3! With a total of " & 
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points in the partnership, we have to defer to partner's unusual jump rebid and pass.\n";
			return ValidateBid(m_nBid);
		}
	}








	//
	//==========================================================
	//
	// Rebidding after partner bid a new suit after our 
	// raise of his suit
	// (i.e., partner made a trial bid, showing a support suit)
	//
	// e.g., 1H -> 2H -> 2S, showing 16-18 pts and < 7 playing tricks
	//       1H -> 3H -> 4C would be a cue bid, unless we're
	//                      not playin cue bids
	//
	//
	if ((nPartnersSuit != nPartnersPrevSuit) && 
				(nPartnersPrevSuit == nPreviousSuit) && 
					(nPartnersSuit != NOTRUMP) && 
						((nPartnersBid - nPreviousBid) < 5) ) 
	{

		//
		m_fPartnersMin = 16;
		m_fPartnersMax = 18;
		if ((nPreviousBid - nPartnersPrevBid) == 5)
			status << "B4O00! Partner bid a support suit at " & szPB & 
					  " after our single raise, showing " & 
					  m_fPartnersMin & "-" & m_fPartnersMax &
					  " points and less than 7 playing tricks.\n";
		else
			status << "B4O01! Partner bid a new suit at " & szPB & 
				      " after our jump raise, showing " & 
					  m_fPartnersMin & "-" & m_fPartnersMax & " points.\n";

		// tally totals (keep the adjustments from the prev round)
		m_fMinTPPoints = fAdjPts + m_fPartnersMin;
		m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;


		//
		//---------------------------------------------------------
		//
		// raise partner's bid to game with strength in the
		// support suit (if a major suit), or move towards a game
		// in No Trump (if minor), or else sign off at the 3-level
		//

		// 
		if (ISMAJOR(nPartnersPrevSuit)) 
		{
			// raise w/ 25+ pts
			if ((m_fMinTPPoints >= PTS_GAME-1) &&
					(numCardsInSuit[nPartnersSuit] >= 4) &&	
							(numHonorsInSuit[nPartnersSuit] >= 1)) 
			{
				// raise partner's (previous) to game
				m_nBid = MAKEBID(nPartnersPrevSuit,4);
				status << "B4O10! With strength in the " & szPSS & 
						  " support suit (holding " & szHP & ") and " & 
						  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " points in the partnership, raise partner to game at " & 
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			} 
/*
			else if ((m_fMinTPPoints >= PTS_GAME-1) &&
							(numCardsInSuit[nPartnersSuit] <= 1)) 
			{
				// also raise to game if short in the support suit
				m_nBid = MAKEBID(nPartnersPrevSuit,4);
				status << "B4O12! With " &
						  ((numCardsInSuit[nPartnersSuit] == 0)? "a void" : "shortness") &
						  " in partner's " & szPSS & 
						  " support suit (holding " & szHP & ") and " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " points in the partnership, raise partner to game at " & 
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);

			} 
*/
			else 
			{
				// otherwise sign off at the cheapest level
				m_nBid = GetCheapestShiftBid(nPartnersPrevSuit);
				status << "B4O14! Without much strength in the " & szPSS & 
						  " support suit (holding " & szHP & ") and " & 
						  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " points in the partnership, sign off at " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
		}
		
		//
		// else we've got a minor suit 
		// try to steer towards a NT contract
		//
		if (m_pHand->AllOtherSuitsStopped(nPartnersSuit,nPartnersPrevSuit)) 
		{
			// try for a NT contract
			if (m_fMinTPCPoints >= PTS_NT_GAME)
				m_nBid = BID_3NT;
			else
				m_nBid = BID_2NT;
		}
		if (m_nBid > nPartnersBid)
		{
			// bid NT if possible
			status << "B4O40! With stoppers in the unbid suits, move from partner's minor " & 
				      szPSS & " suit to a contract of " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		} 
		else if ((m_fMinTPPoints >= PTS_MINOR_GAME) && (nPPrevSuitSupport >= SS_MODERATE_SUPPORT))
		{
			// else raise the minor to game with 29 pts && good support
			m_nBid = MAKEBID(nPartnersPrevSuit,5);
			status << "B4O45! Without stoppers in the unbid suits we can't bid a NT contract, but with " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " pts in the partnership and " & SLTS(nPartnersPrevSuit) & 
					  " trump support, we can raise partner to game at " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		} 
		else if ((m_fMinTPPoints >= PTS_MINOR_GAME-1) && (nPPrevSuitSupport >= SS_MODERATE_SUPPORT) && 
				 (numCardsInSuit[nPartnersSuit] >= 4) && (numHonorsInSuit[nPartnersSuit] >= 1))
		{
			// else raise the minor to game with 28 pts and 
			// strength in the support suit
			m_nBid = MAKEBID(nPartnersPrevSuit,5);
			status << "B4O48! Without stoppers in the unbid suits we can't bid a NT contract, but with strength in partner's " & 
					  szPSS & " support suit (holding " & szHP & ") and " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " points in the partnership, raise partner to game at " & 
					  BTS(m_nBid) & ".\n";
/*
			else
				status << "B4O49! Without stoppers in the unbid suits we can't bid a NT contract, but with " &
						  ((numCardsInSuit[nPartnersSuit] == 0)? "a void" : "shortness") &
						  " in partner's " & szPSS & " support suit and " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " points in the partnership, raise partner to game at " & 
						  BTS(m_nBid) & ".\n";
*/
			return ValidateBid(m_nBid);
		} 
		else 
		{
			// otherwise sign off at the cheapest level
			m_nBid = GetCheapestShiftBid(nPartnersPrevSuit);
			status << "B4O50! With " & m_pHand->GetUnstoppedSuitNames() & 
					  " unstopped, we can't steer towards a NT contract, and with only " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " team pts we sign off on the minor " & 
					  szPVSS & " suit at " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}
	
	} // end of support suit section








	//
	//==========================================================
	//
	// Rebidding after partner bid a new suit (minimum rebid) 
	// after our response of a new suit
	// (i.e., partner showed a third suit, possibly a reverse)
	//
	// e.g., 1H -> 1S -> 2C, or
	//       1S -> 2C -> 2H, showing 12-18 pts, or even
	//       1H -> 2D -> 3C, or
	//        X -> 2C -> 2S
	//
	if ((nPartnersSuit != nPreviousSuit) && 
			(nPartnersSuit != nPartnersPrevSuit) && 
				(nPreviousSuit != nPartnersPrevSuit) && 
					(nPartnersSuit != NOTRUMP) && 
						(nPartnersPrevSuit != NOTRUMP) && 
//							(nPreviousSuit != NOTRUMP) && 
								((nPartnersBid - nPreviousBid) < 5) ) 
	{

		//
		// check previous action
		if (nPartnersOpeningBidLevel == 2) 
		{
			// partner opened at the 2-level
			if (nPartnersOpeningBid == BID_2C) 
			{
				m_fPartnersMin = PTS_GAME-2;
				m_fPartnersMax = MIN(22, 40 - fCardPts);
				status << "2B4P00! Partner bid another suit at " & szPB &
					      " after opening an artificial 2C, indicating " &
						  m_fPartnersMin & "-" & m_fPartnersMax &
						  " points in his hand.\n";
			} 
			else if (pCurrConvSet->IsConventionEnabled(tidStrongTwoBids)) 
			{
				m_fPartnersMin = 16;
				m_fPartnersMax = 22;
				status << "2B4P01! Partner bid another suit (third suit in partnership) at " & szPB &
						  " after opening a strong " & szPPB & 
						  ", indicating " & m_fPartnersMin & "-" & m_fPartnersMax &
						  " points in his hand.\n";
			} 
			else 
			{
				m_fPartnersMin = 6;
				m_fPartnersMax = 12;
				status << "2B4P02! Partner bid another suit (third suit in partnership) at " & szPB & 
						  " after opening a weak " & szPPB & 
						  ", indicating " & m_fPartnersMin & "-" & m_fPartnersMax &
						  " points in his hand.\n";
			}
		} 
		else 
		{
			// partner previously opened at the 1-level
			// see if he reversed
			if ((pCurrConvSet->IsOptionEnabled(tbStructuredReverses)) &&
							(nPartnersPrevSuit != NOTRUMP) && 
							(nPartnersPrevSuit != NONE) && 
							(nPartnersSuit > nPartnersPrevSuit) &&
							(nPartnersBidLevel == 2) &&
							(nPreviousBid != BID_DOUBLE))	// neg double negates reverse
			{
				m_fPartnersMin = 17;
				m_fPartnersMax = 18;
				status << "2B4P0RV! Partner reversed and bid a higher-ranking suit at the 2-level (" &
						  szPB & " over " & szPPB & ") after our " & 
						  ((nPreviousBidLevel == 1)? "1/1" : (nPreviousBidLevel == 2)? "2/1" : "jump") &
						  " response, showing " & m_fPartnersMin & "-" & m_fPartnersMax & " points.\n";
			}
			else if ((nPreviousBid == BID_DOUBLE) && pCurrConvSet->IsConventionEnabled(tidNegativeDoubles))
			{
				// we bid a negative double last time
				m_fPartnersMin = 12;
//				m_fPartnersMax = 15;	// is this right?
				m_fPartnersMax = 18;
				status << "2B4P03! Partner bid a second suit after our negative double, showing " & 
						  m_fPartnersMin & "-" & m_fPartnersMax & " points.\n";
			}
			else
			{
				// partner did not reverse, and we didn't bid a neg double last time
				m_fPartnersMin = 12;
				m_fPartnersMax = 18;
				status << "2B4P04! Partner bid another suit (third suit in partnership) at " & szPB & 
						  " after our " & 
						  ((nPreviousBidLevel == 1)? "1/1" : (nPreviousBidLevel == 2)? "2/1" : "jump") &
						  " response, showing " & m_fPartnersMin & "-" & m_fPartnersMax & " points.\n";
			}
		}

		// revalue hand for partner's new suit
		fAdjPts = m_pHand->RevalueHand(REVALUE_DUMMY, nPartnersSuit, TRUE);
		m_fMinTPPoints = fAdjPts + m_fPartnersMin;
		m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;

		// get a new suit -- 3rd or 4th suit
//		int n4thSuit = GetFourthSuit(nPreviousSuit,nPartnersSuit,nPartnersPrevSuit);
		int newSuit = PickBestFinalSuit(status);

		//
		//---------------------------------------------------------
		//
		//

		// pass with < 18 total minimum pts
		if (m_fMinTPPoints < 18) 
		{
			m_nBid = BID_PASS;
			status << "B4P10! With only " & fCardPts & "/" & fPts  &
					  " points in hand, for a total in the partnership of " & 
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " points, we have to pass.\n";
			return ValidateBid(m_nBid);
		}

		// with 18-19 total min pts, pass with a preference for the 
		// second suit, pass, or rebid his first suit
		if (m_fMinTPPoints <= 19) 
		{
			// higher ranking suits ==> lower numbers
			if (m_pHand->GetSuitRank(nPartnersSuit) < m_pHand->GetSuitRank(nPartnersPrevSuit)) 
			{
				m_nBid = BID_PASS;
				status << "B4P14! With " & fCardPts & "/" & fPts  &
						  " points in hand, for a total in the partnership of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points, and a preference for partner's second suit, we pass.\n";
				return ValidateBid(m_nBid);
			} 
			else 
			{
				m_nBid = GetCheapestShiftBid(nPartnersPrevSuit);
				m_nAgreedSuit = nPartnersPrevSuit;
				status << "B4P16! With " & fCardPts & "/" & fPts  &
						  " points in hand, for a total in the partnership of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points, and a preference for partner's first suit, we shift back to " &
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
		}
		
		//
		//----------------------------------------------------------
		//
		// here, we have 20+ minimum total points
		//

		// with 20-24 min total pts, raise the second suit to 
		// the 2 or 3-level with 4-card trump support
		if (RaisePartnersSuit(SUIT_ANY,RAISE_TO_2,PTS_GAME-6,PTS_GAME-4,SUPLEN_4))
			return ValidateBid(m_nBid);
		if (RaisePartnersSuit(SUIT_ANY,RAISE_TO_3,PTS_GAME-3,PTS_GAME-1,SUPLEN_4))
			return ValidateBid(m_nBid);

		// with 20-24 min total pts and 3-card support for 
		// the first suit, return to that suit
//		if (m_suit[nPartnersPrevSuit].GetRank() < m_suit[nPartnersSuit].GetRank())
		if (numPPrevSuitSupportCards >= 3) 
		{
			// with 20-22 pts, return to first suit at the lowest level
			if ((m_fMinTPPoints >= PTS_GAME-6) && (m_fMinTPPoints <= PTS_GAME-4)) 
			{
				m_nAgreedSuit = nPartnersPrevSuit;
				m_nBid = GetCheapestShiftBid(nPartnersPrevSuit);
				status << "B4P20! With " & fCardPts & "/" & fPts &
						  " points and " & numPPrevSuitSupportCards & 
						  "-card support for partner's first suit (" & szPPS & 
						  "), return to it by bidding " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
			// with 23-25 min total pts, (jump) shift back to the first suit
			if ((m_fMinTPPoints >= PTS_GAME-3) && (m_fMinTPPoints < PTS_GAME)) 
			{
				m_nAgreedSuit = nPartnersPrevSuit;
				// jump shift in a major if we originally responded 1/1, or just
				// a single shift it was 2/1 (or 1/1 in a minor)
				if ((nPreviousBidLevel == 1) && (ISMAJOR(nPartnersPrevSuit)))
				{
					m_nBid = GetJumpShiftBid(nPartnersPrevSuit,nPartnersBid);
					status << "B4P22! With " & fCardPts & "/" & fPts &
							  " points in hand, for a total in the partnership of " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " points, and with " & numPPrevSuitSupportCards & 
							  "-card support for partner's first suit (" & szPPS &
							  "), return to it by jump shifting to " & BTS(m_nBid) & ".\n";
				} 
				else 
				{
					m_nBid = GetCheapestShiftBid(nPartnersPrevSuit);
					status << "B4P24! With " & fCardPts & "/" & fPts &
							  " points in hand, for a total in the partnership of " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " points, and " & numPPrevSuitSupportCards & 
							  "-card support for partner's first suit (" & szPPS &
							  "), return to it by shifting back to " & BTS(m_nBid) & ".\n";
				}
				return ValidateBid(m_nBid);
			}
		}

		// with 23-25 min tot HCPs, no good suit fit, and 
		// a stopper in the 4th suit, bid 2NT
		if ((nPartnersBidLevel == 2) &&
			(nPrefSuitStrength < SS_STRONG) && 
			(numSupportCards < 4) && (numPPrevSuitSupportCards < 4))
			if (BidNoTrump(LEVEL_2,PTS_GAME-3,PTS_GAME-1,FALSE,STOPPED_UNBID))
				return ValidateBid(m_nBid);

		// else rebid a 6-card suit with 22-24 pts
		if (ISSUIT(nPreviousSuit) && RebidSuit(SUIT_PREV,REBID_CHEAPEST,PTS_GAME-4,PTS_GAME-2,LENGTH_6))
			return ValidateBid(m_nBid);

		// else with 20-25 min total pts, bid the 3rd/4th suit if we have the strength;
		// otherwise pass
		if (m_fMinTPPoints < PTS_GAME) 
		{
			// see what the bid would be
			m_nBid = GetCheapestShiftBid(newSuit);
			int newBidLevel = BID_LEVEL(m_nBid);
			// can't bid above the 3 level, or even at the 3 level 
			// without sufficient strength
			if ( (newBidLevel > 3) ||
				 ((newBidLevel == 3) && (m_fMinTPPoints < PTS_GAME-1)) ||
				 ((newBidLevel == 2) && (newSuit == NOTRUMP) && (m_fMinTPCPoints <= PTS_GAME-3)) )
			{
				// not 'nuff pts, so either pass or return to the first suit
				// but we can return to the first suit only if it's at the 
				// same bid level as partner's last bid
				int nTestBid = GetCheapestShiftBid(nPartnersPrevSuit);
				if ( (numPPrevSuitSupportCards >= numSupportCards) &&
					 (BID_LEVEL(nTestBid) == BID_LEVEL(nPartnersBid)) )
				{
					// we can safely return to the first suit
					m_nBid = nTestBid;
					status << "B4P26! With only " & fCardPts & "/" & fPts &
							  " points in hand and poor to moderate support for partner's suits, we return to partner's first suit at a bid of " &
							  BTS(m_nBid) & ".\n";
				}
				else
				{
					// just pass
					m_nBid = BID_PASS;
					status << "B4P27! But with only " & fCardPts & "/" & fPts & 
							  " points in hand and poor support for partner's suits, we have to pass.\n";
				}
			}
			else
			{
				status << "B4P28! With " & fCardPts & "/" & fPts &
						  " points and a lack of support for either of partner's suits, bid" & 
						  ((newSuit == NOTRUMP)? " " : " another suit at ") & BTS(m_nBid) & ".\n";
			}
			return ValidateBid(m_nBid);
		}

		//
		//--------------------------------------------------------
		//
		// here, we have 26+ min total points
		//

		// 26-31 total pts: raise a major to game with 4-card support
		// first try the first suit
		if ((m_fMinTPPoints < PTS_SLAM) && (ISMAJOR(nPartnersPrevSuit)) && 
						(numPPrevSuitSupportCards >= 4)) 
		{
			m_nAgreedSuit = nPartnersPrevSuit;
			if (nPartnersBid < MAKEBID(nPartnersPrevSuit, 4))
			{
				m_nBid = MAKEBID(nPartnersPrevSuit,4);
				status << "B4P40! With " & numPPrevSuitSupportCards & 
						  "-card support for partner's first suit (" & szPPS &
						  ", holding " & szHPP & ") and a total of " & 
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " pts in the partnership, we can bid game at " & 
						  BTS(m_nBid) & ".\n";
			}	
			else
			{
				m_nBid = BID_PASS;
				status << "B4P40a! We have " & numSupportCards & 
						  "-card support for partner's first suit (" & szPS &
						  ", holding " & szHP & "), but with only " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " pts in the partnership, we can't raise partner beyond his " &
						  szPB & " bid, so pass.\n";
			}
			return ValidateBid(m_nBid);
		}

		// then try the second suit
		if ((m_fMinTPPoints < PTS_SLAM) && (ISMAJOR(nPartnersSuit)) && 
								(numCardsInSuit[nPartnersSuit] >= 4))
		{
			m_nAgreedSuit = nPartnersSuit;
			if (nPartnersBidLevel < 4)
			{
				m_nBid = GetGameBid(nPartnersSuit);
				status << "B4P42! With " & fCardPts & "/" & fPts & "/" & fAdjPts & 
						  " points and " & numSupportCards & 
						  "-card support for partner's second suit (" & szPS &
						  ", holding " & szHP & ") and a total of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " pts in the partnership we can bid game at " & BTS(m_nBid) & ".\n";
			}
			else
			{
				m_nBid = BID_PASS;
				status << "B4P42a! We have " & numSupportCards & 
						  "-card support for partner's second suit (" & szPS &
						  ", holding " & szHP & "), but with only " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " pts in the partnership we can't raise partner beyond his " &
						  szPB & " bid, so pass.\n";
			}
			return ValidateBid(m_nBid);
		}

		// jump to game in a self-supporting major
		if (ISSUIT(nPreviousSuit) && RebidSuit(SUIT_MAJOR,REBID_AT_4,PTS_MAJOR_GAME,PTS_SLAM-1,LENGTH_6,SS_SELFSUPPORTING,HONORS_3))
		{
			status << "B4P50! With a self-supporting " & STSS(nPrefSuit) &
					  " suit and " & m_fMinTPPoints & 
					  "+ team points, jump to game at " & BTS(m_nBid)  & ".\n";
			return ValidateBid(m_nBid);
		}

		// Bid 3NT with 27-32 min HCPs, a semi-balanced hand,
		// and the 4th suit stopped
		if (BidNoTrump(LEVEL_3,PTS_NT_GAME+1,PTS_SLAM-1,TRUE,STOPPED_UNBID))
			return ValidateBid(m_nBid);
		// or 2NT without four stoppers (????) && 25+ HCPs
		if (BidNoTrump(LEVEL_2,PTS_NT_GAME-1,PTS_SLAM-1,TRUE,STOPPED_DONTCARE))
			return ValidateBid(m_nBid);
		
		// raise partner's major to the 3-level with 3 support cards
		// and 26+ points, if possible
		// try the first suit
		if (ISMAJOR(nPartnersPrevSuit))
			if (RaisePartnersSuit(SUIT_PREV,RAISE_TO_3,PTS_GAME,PTS_SLAM-1,SUPLEN_3))
				return ValidateBid(m_nBid);
		// then try the second suit
		if (ISMAJOR(nPartnersSuit))
			if (RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_3,PTS_GAME,PTS_SLAM-1,SUPLEN_3))
				return ValidateBid(m_nBid);

		// jump to game in a self-supporting minor (7 cards min)
		if (ISSUIT(nPreviousSuit) && RebidSuit(SUIT_MINOR,REBID_AT_5,PTS_MINOR_GAME,PTS_SLAM-1,LENGTH_7,SS_SELFSUPPORTING,HONORS_3))
		{
			status << "B4P55! With a self-supporting " & STSS(nPrefSuit) &
					  " suit and " & m_fMinTPPoints & 
					  "+ team points, jump to game at " & BTS(m_nBid)  & ".\n";
			return ValidateBid(m_nBid);
		}
		
		// raise partner's first suit (a minor) to the 4-level with 5 trumps
		if (RaisePartnersSuit(SUIT_PREV,RAISE_TO_4,PTS_GAME,PTS_SLAM-1,SUPLEN_5))
			return ValidateBid(m_nBid);

		// raise partner's second suit (minor) to the 4-level with 5 trumps
		if (RaisePartnersSuit(SUIT_MINOR,RAISE_TO_4,PTS_MINOR_GAME-3,PTS_SLAM-1,SUPLEN_5))
			return ValidateBid(m_nBid);

		// rebid a good 6-card suit
		if (ISSUIT(nPreviousSuit) && RebidSuit(SUIT_MAJOR,REBID_CHEAPEST,PTS_GAME,PTS_SLAM-1,LENGTH_6,SS_STRONG))
			return ValidateBid(m_nBid);
//		if (ISSUIT(nPreviousSuit) && RebidSuit(SUIT_MINOR,REBID_CHEAPEST,PTS_MINOR_GAME,PTS_SLAM-1,LENGTH_6,SS_STRONG))
		if (ISSUIT(nPreviousSuit) && RebidSuit(SUIT_MINOR,REBID_CHEAPEST,PTS_GAME,PTS_SLAM-1,LENGTH_6,SS_STRONG))
			return ValidateBid(m_nBid);

		// else with <= 32 min total pts, either bid the 4th suit, if it's decent,
		// or one of partner's suit, or NT.
		if (m_fMinTPPoints < PTS_SLAM) 
		{
			// raise partner if below game
			if (nPartnersBid < GetGameBid(newSuit))
			{
				m_nBid = GetCheapestShiftBid(newSuit);
				// see what suit we bid
				if (newSuit == nPartnersPrevSuit)
				{
					m_nAgreedSuit = nPartnersPrevSuit;
					status << "B4P64a! With a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " pts in the partnership, no rebiddable suit, and no good second suit, support partner's first suit at " &
							  BTS(m_nBid) & ".\n";
				}
				else if (newSuit == nPartnersSuit)
				{
					m_nAgreedSuit = nPartnersSuit;
					status << "B4P64b! With a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " pts in the partnership, no rebiddable suit, and no good second suit, support partner's second suit at " &
							  BTS(m_nBid) & ".\n";
				}
				else if (newSuit == nPreviousSuit)
					status << "B4P64c! With a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " pts in the partnership and a lack of good support for either of partner's suits, rebid our own " & 
							  STSS(nPreviousSuit) & " suit at " & BTS(m_nBid) & ".\n";
				else if (newSuit == NOTRUMP)
					status << "B4P64d! With a total of " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
							  " HCPs in the partnership, no rebiddable suit or good second suit, and a lack of good support for either of partner's suits, bid " &
							  BTS(m_nBid) & ".\n";
				else 
					status << "B4P64e! With a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " pts in the partnership and a lack of good support for either of partner's suits, bid the fourth suit at " &
							  BTS(m_nBid) & ".\n";
			}
			else
			{
				m_nBid = BID_PASS;
				status << "B4P68! We have a total of " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " pts in the partnership with modest support for partner's suits, and don't want to bid on past game, so we have to pass.\n";
			}
			return ValidateBid(m_nBid);
		}


		//
		//---------------------------------------------------------
		//
		// with 33+ min total points, think slam
		//

		//
		// bid Blackwood, with the intended suit being either our best suit,
		// one of partner's suits, or notrump
		//
		if (m_fMinTPPoints >= PTS_SLAM) 
		{
			int nSuit = PickBestFinalSuit(status);
			InvokeBlackwood(nSuit);
			return ValidateBid(m_nBid);
		}


		//
		//---------------------------------------------------------
		//
		// here, we've fallen through
		//

		// with < 22 min total pts, bid 1NT
		if (m_fMinTPPoints < PTS_GAME-4) 
		{
			m_nBid = BID_1NT;
			return ValidateBid(m_nBid);
		}
		// else bid a new suit
		m_nBid = GetCheapestShiftBid(newSuit);
		status << "B4P80! With " & fCardPts & "/" & fPts &
				  " points in hand and " & 
				  m_fMinTPPoints & "-" & m_fMaxTPPoints &
				  " points in the partnership, no good support for partner's suits, and no other good options, bid " &
				  ((nPreviousSuit == NOTRUMP)? "a new" : "the fourth") &
				  " suit at " &
				  BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);

	} // end of 3rd suit section









	//
	//==========================================================
	//
	// Rebidding after partner bid a new suit
	// after bidding 1NT/2NT/3NT previously
	//
	// e.g., 1NT -> 2C -> 2D, (not playing Stayman), or
	//       2NT -> 3H -> 3S or
	//       3NT -> 4H -> 4S
	//
	//
	if ((nPartnersSuit != nPreviousSuit) && 
					(nPartnersSuit != nPartnersPrevSuit) && 
							(nPartnersPrevSuit == NOTRUMP) && 
									(nPartnersPrevBidLevel <= 3)) 
	{

		//
		// check previous action
		if (nPartnersPrevBidLevel == 3) 
		{
			// partner opened with 3NT
			m_fPartnersMin = pCurrConvSet->GetNTRangeMin(3);
			m_fPartnersMax = pCurrConvSet->GetNTRangeMax(3);
			status << "2B4T00! Partner bid another suit at " & szPB & 
	  				  " after his opening of 3NT and our response of " & szPVB & ".\n";
		} 
		else if (nPartnersPrevBidLevel == 2) 
		{
			// partner opened with 2NT
			m_fPartnersMin = pCurrConvSet->GetNTRangeMin(2);
			m_fPartnersMax = pCurrConvSet->GetNTRangeMax(2);
			status << "2B4T01! Partner bid another suit at " & szPB & 
					  " after his opening of 2NT and our response of " & szPVB & ".\n";
		} 
		else 
		{
			// partner opened with 1NT
			m_fPartnersMin = pCurrConvSet->GetNTRangeMin(1);
			m_fPartnersMax = pCurrConvSet->GetNTRangeMax(1);
			status << "2B4T02! Partner bid another suit at " & szPB &
					  " after his opening of 1NT and our response of " & szPVB & ".\n";
		}

		// revalue hand for partner's new suit
		fAdjPts = m_pHand->RevalueHand(REVALUE_DUMMY, nPartnersSuit, TRUE);
		m_fMinTPPoints = fAdjPts + m_fPartnersMin;
		m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;


		//
		//---------------------------------------------------------
		//
		//

		// pass with < 21 total minimum pts
		if (m_fMinTPPoints < PTS_GAME-5) 
		{
			m_nBid = BID_PASS;
			status << "B4T10! With only " & fCardPts & "/" & fPts &
					  " points in hand, for a total in the partnership of " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " points, we have to pass partner's " & szPB & " bid.\n";
			return ValidateBid(m_nBid);
		}

		//
		//----------------------------------------------------------
		//
		// here, we have 21+ pts
		//

		// with 21-24 min total pts, raise partner's suit to 
		// the 2 or 3-level with 4-card trump support
		if (RaisePartnersSuit(SUIT_ANY,RAISE_TO_2,PTS_GAME-5,PTS_GAME-4,SUPLEN_4))
			return ValidateBid(m_nBid);
		if (RaisePartnersSuit(SUIT_ANY,RAISE_TO_3,PTS_GAME-3,PTS_GAME-1,SUPLEN_4))
			return ValidateBid(m_nBid);

		// with 23-25 min HCPs and a stopper in the unbid suits, bid 2NT
		if (BidNoTrump(LEVEL_2,PTS_NT_GAME-3,PTS_NT_GAME-1,FALSE,STOPPED_UNBID))
			return ValidateBid(m_nBid);

		// else if we have a balanced hand and 23-25 HCPs, 
		// bid 2NT if possible
		if ((bBalanced) && (nPartnersBid < BID_2NT))
			if (BidNoTrump(LEVEL_2,PTS_GAME-3,PTS_GAME-1,FALSE,STOPPED_DONTCARE))
				return ValidateBid(m_nBid);

		// else rebid a 6-card suit at the 2 or 3 level with 22-24 pts
		if (RebidSuit(SUIT_ANY,REBID_CHEAPEST,PTS_GAME-4,PTS_GAME-2, LENGTH_6))
			return ValidateBid(m_nBid);

		// else with 21-25 min total pts, bid another suit at the 2-level or pass
		if (m_fMinTPPoints < PTS_GAME) 
		{
			if (BidNextBestSuit(SUIT_ANY,SHIFT_CHEAPEST,PTS_GAME-5,PTS_GAME-1))
			{
				status << "B4T20! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points in the partnership but without good support for partner's new " &
						  szPSS & " suit (holding " & szHP & "), bid another suit at " &
						  BTS(m_nBid) & ".\n";
			}
			else
			{
				m_nBid = BID_PASS;
				status << "B4T21! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points in the partnership but without good support for partner's new " &
						  szPSS & " suit (holding " & szHP & "), and with no more bidding room, we're forced to pass.\n";
			}
			return ValidateBid(m_nBid);
		}

		//
		//--------------------------------------------------------
		//
		// here, we have 26+ min total points
		//

		// 26-32 total pts: raise a major to game with 4-card support
		if ((m_fMinTPPoints < PTS_SLAM) && (ISMAJOR(nPartnersSuit)) && 
										  (numSupportCards >= 4)) 
		{
			m_nAgreedSuit = nPartnersSuit;
			m_nBid = MAKEBID(nPartnersSuit,4);
			status << "B4T24! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
				" points in the partnership and " & numSupportCards &
				"-card support for partner's new " & szPSS & " suit (holding " & szHP &
				"), we can bid game at " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		// Bid 3NT with 26-31 min HCPs and all suits stopped
		if (BidNoTrump(LEVEL_3,PTS_NT_GAME,PTS_SLAM-1,FALSE,STOPPED_UNBID))
			return ValidateBid(m_nBid);

		// jump to game in a self-supporing major
		if (RebidSuit(SUIT_MAJOR,REBID_AT_4,PTS_MAJOR_GAME,PTS_SLAM-1,LENGTH_6,SS_SELFSUPPORTING, HONORS_4))
			return ValidateBid(m_nBid);
		
		// raise partner's minor with 5-card support
		if (RaisePartnersSuit(SUIT_MINOR,RAISE_ONE,PTS_MINOR_GAME-3,PTS_SLAM-1,SUPLEN_5))
			return ValidateBid(m_nBid);

		// else with < 33 min total pts, bid another suit
		if (m_fMinTPCPoints < PTS_SLAM) 
		{
			if (BidNextBestSuit(SUIT_ANY,SHIFT_CHEAPEST,PTS_GAME-4,PTS_SLAM-1))
			{
				status << "B4T28! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points in the partnership but without good support for partner's new " &
						  szPSS & " suit (holding " & szHP & "), bid another suit at " & 
						  BTS(m_nBid) & ".\n";
			}
			else
			{
				m_nBid = BID_PASS;
				status << "B4T29! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points in the partnership but without good support for partner's new " &
						  szPSS & " suit (holding " & szHP & "), and with no more safe bidding room, we're forced to pass.\n";
			}
			return ValidateBid(m_nBid);
		}


		//
		//---------------------------------------------------------
		//
		// with 33+ min total points, think slam
		//

		// bid Blackwood
		//
		if (m_fMinTPCPoints >= PTS_SLAM) 
		{
			InvokeBlackwood(NOTRUMP);
			return ValidateBid(m_nBid);
		}


		//
		//---------------------------------------------------------
		//
		// here, we've fallen through
		//

		// with <= 22 min total pts, bid 1NT
		// with 23-25, bid 2NT
		// with 26-32, bid 3NT
		if (BidNoTrump(LEVEL_1,0,PTS_NT_GAME-4))
			return ValidateBid(m_nBid);
		if (BidNoTrump(LEVEL_2,PTS_NT_GAME-3,PTS_NT_GAME-1))
			return ValidateBid(m_nBid);
		if (BidNoTrump(LEVEL_3,PTS_NT_GAME,PTS_SLAM-1,FALSE,STOPPED_UNBID))
			return ValidateBid(m_nBid);

		// else bid another suit
		if (BidNextBestSuit(SUIT_ANY,SHIFT_CHEAPEST,PTS_GAME-4,PTS_GAME-1))
		{
			status << "B4T28! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " points in the partnership but without good support for partner's new " &
					  szPSS & " suit (holding " & szHP & "), bid another suit at " & 
					  BTS(m_nBid) & ".\n";
		}
		else
		{
			m_nBid = BID_PASS;
			status << "B4T29! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " points in the partnership but without good support for partner's new " &
					  szPSS & " suit (holding " & szHP & "), and no more safe bidding room, we're forced to pass\n";
		}
		return ValidateBid(m_nBid);

	} // end of partner's rebid after NT section












	//
	//==========================================================
	//
	// Rebidding after partner made a jump shift in a new suit 
	// e.g., 1H -> 1S  -> 3C, showing 19+ points
	// or    1H -> 1NT -> 3C
	//
	//
	if ((nPartnersSuit != nPreviousSuit) && 
			(nPartnersSuit != NOTRUMP) && 
				(ISSUIT(nPartnersPrevSuit)) &&
					(nPartnersPrevSuit != NOTRUMP) && 
						((nPartnersBid - nPreviousBid) > 5)) 
	{
		// get a new suit / 4th suit
		int newSuit = PickBestFinalSuit(status);

		//
		//
		// check previous action
		if (nPartnersOpeningBid == BID_2C) 
		{
			m_fPartnersMin = 23;
			m_fPartnersMax = MIN(22, 40 - fCardPts);
			status << "2B4Q00! Partner made a jump shift to " & szPB & 
					  " in response to our " & BTS(nPreviousBid) & 
					  " bid and after his strong 2C opening, showing 23+ points.  The bid is forcing to game.\n";
		} 
		else if ((nPartnersPrevBidLevel == 2) && 
						(pCurrConvSet->IsConventionEnabled(tidStrongTwoBids))) 
		{
			m_fPartnersMin = 19;
			m_fPartnersMax = 22;
			status << "2B4Q01! Partner made a jump shift to " & szPB & 
					  " in response to our " & szPVB & 
					  " bid and after his strong " & szPPB & " opening, showing " &
					  m_fPartnersMin & "-" & m_fPartnersMax &
					  " points.  The bid is forcing to game.\n";
		} 
		else 
		{
			// partner opened at the 1-level
			m_fPartnersMin = 19;
			m_fPartnersMax = 22;
			status << "2B4Q02! Partner made a jump shift to " & szPB & 
					  " in response to our " & szPVB & " bid, showing " &
					  m_fPartnersMin & "-" & m_fPartnersMax &
					  " points.  The bid is forcing to game.\n";
		}

		// the bid is forcing to game
		m_bGameForceActive = TRUE;		

		// revalue hand for partner's new suit
		fAdjPts = m_pHand->RevalueHand(REVALUE_DUMMY, nPartnersSuit, TRUE);
		m_fMinTPPoints = fAdjPts + m_fPartnersMin;
		m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;

		//
		//---------------------------------------------------------
		//
		// bid towards game with < 33 points (up to 12 in hand)
		//

		if (m_fMinTPPoints < PTS_SLAM) 
		{

			// rebid a good 5-card or decent 6-card major
			if (RebidSuit(SUIT_MAJOR,REBID_CHEAPEST,0,PTS_SLAM-1,LENGTH_5,SS_STRONG))
					return ValidateBid(m_nBid);

			// rebid NT with stoppers
			if (nPartnersBidLevel == 3)
				if (BidNoTrump(LEVEL_3,0,PTS_SLAM-1,FALSE,STOPPED_UNBID))
					return ValidateBid(m_nBid);

			// rebid any 6-card suit
			if (RebidSuit(SUIT_ANY,REBID_CHEAPEST,0,PTS_SLAM-1,LENGTH_6))
				return ValidateBid(m_nBid);

			// return to partner's first major suit with 
			// 3-card support
			if ((ISMAJOR(nPartnersPrevSuit)) && 
						(numPPrevSuitSupportCards >= 3)) 
			{
				m_nAgreedSuit = nPartnersPrevSuit;
				m_nBid = GetCheapestShiftBid(nPartnersPrevSuit);
				status << "B4Q10! With " & fCardPts & "/" & fPts &
						  " points and " & numPPrevSuitSupportCards & 
						  "-card support for partner's first suit (" & szPPS &
						  ", holding " & szHPP & "), we can raise to " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// then try the second suit -- need 4 trumps since partner
			// may have shifted to a 4-card suit
			if ((ISMAJOR(nPartnersSuit)) && 
//							(numCardsInSuit[nPartnersSuit] >= 3)) 
							(numCardsInSuit[nPartnersSuit] >= 4)) 
			{
				m_nAgreedSuit = nPartnersSuit;
				m_nBid = GetCheapestShiftBid(nPartnersSuit);
				status << "B4Q20! With " & fCardPts & "/" & fPts &
						  " points and " & numSupportCards & 
						  "-card support for partner's second suit (" & szPS &
						  ", holding " & szHP & "), we can raise to "& 
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// else support a minor if necessary -- 
			// but pick the better suit
			if (m_pHand->GetSuitRank(nPartnersSuit) < m_pHand->GetSuitRank(nPartnersPrevSuit)) 
			{
				// prefer the second suit 
				if (numCardsInSuit[nPartnersSuit] >= 3) 
				{
					m_nAgreedSuit = nPartnersSuit;
					m_nBid = GetCheapestShiftBid(nPartnersSuit);
					status << "B4Q30! With " & fCardPts & "/" & fPts &
							  " points and " & numSupportCards & 
							  "-card support for partner's second suit (" & szPS &
							  ", holding " & szHP & "), we can raise to " & 
							  BTS(m_nBid) & ".\n";
					return ValidateBid(m_nBid);
				}
			} 
			else 
			{
				// prefer the first suit 
				if (numCardsInSuit[nPartnersPrevSuit] >= 3) 
				{
					m_nAgreedSuit = nPartnersPrevSuit;
					m_nBid = GetCheapestShiftBid(nPartnersPrevSuit);
					status << "B4Q35! With " & fCardPts & "/" & fPts &
							  " points and " & numPPrevSuitSupportCards &
							  "-card support for partner's first suit (" & szPPS &
							  ", holding " & szHPP & "), we can raise to " &
							  BTS(m_nBid) & ".\n";
					return ValidateBid(m_nBid);
				}
			}

			// else bid another suit
			m_nBid = GetCheapestShiftBid(newSuit);
			status << "B4Q38! With " & fCardPts & "/" & fPts &
					  " points, no good support for partner's suits, and no other good options, shift to " &
					  ((nPreviousSuit == NOTRUMP)? "another" : "the fourth") &
					  " suit at " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}


		//
		// else try for slam -- bid Blackwood
		// pick a suit to play in -- our own, if it's strong, or else
		// one of partner's -- prefer the first one, ceteris peribus
		//
		int nSuit;
		// pick our own strong suit if we have weak support 
		// for partner's suits
		if ((numSupportCards <= 3) && (numPPrevSuitSupportCards <= 3) &&
				(nSuitStrength[nPrefSuit] >= SS_STRONG) &&
				(numCardsInSuit[nPrefSuit] >= 6))
		{
			// pick our own suit
			nSuit = nPrefSuit;
		}
		else
		{
			// pick one of partner's suits
			nSuit = PickSuperiorSuit(nPartnersPrevSuit,nPartnersSuit,SP_FIRST);
			// see if we have enough length in the suit (3+ cards)
			if (numCardsInSuit[nSuit] < 3)
			{
				// no? then try the other suit
				if (nSuit == nPartnersPrevSuit)
					nSuit = nPartnersSuit;
				else
					nSuit = nPartnersPrevSuit;
				if (numCardsInSuit[nSuit] < 3)
					// hmpf -- resort to NT
					nSuit = NOTRUMP;
			}
		}
		InvokeBlackwood(nSuit);
		if (nSuit != NOTRUMP)
			status << "B4Q40! We pick " & STS(nSuit) & 
					  " for the ultimate slam contract suit.\n";
		else
			status << "B4Q41! Since we lack good support for either of partner's suits, the ultimate suit for the slam contract will be No Trump.\n";
		return ValidateBid(m_nBid);
	
	} // end jump shift response section











	//
	//===============================================================
	//
	// Here, reached an unknown state
	//
	// partner made an unusual response

	m_nBid = BID_PASS;
	status << "B4Z00! Partner made a response which we don't understand; simply pass.\n";
	return ValidateBid(m_nBid);


}






