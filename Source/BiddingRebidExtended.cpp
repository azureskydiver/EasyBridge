//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Bidrbidx.cpp
//
// Extended rebidding routines
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
// MakeRebidExtended()
//
// Rebid after the first two rounds -- i.e., 5th & later bids
//
int CBidEngine::MakeRebidExtended()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;

	status << "2Rebidding in round " & nRound+1 & ".\n";



	//
	//##########################################################
	//
	// In general, we're doing one of the following at this point:
	//   - following through on a convention (e.g., Blackwood)
	//   - trying to decide on a suit, after 2, 3, or 4 suits were
	//     shown by the partnership
	//   - following through on a late raise by partner
	//



	//
	// just for kicks, check if partner bid slam
	//
	if (nPartnersBid == BID_7NT)
	{
		m_nBid = BID_PASS;
		status << "BXPNTa! Partner has gone ahead and bid a grand slam, so cross our fingers and pass.\n";
		return ValidateBid(m_nBid);
	}




	//
	//==========================================================
	//
	// Following through on a convention?
	//
	// Possibilities:
	// ---------------
	//  - later round of Blackwood
	//  - later round of Gerber
	//  - later round of cue bidding
	//  - later round of Jacoby
	//  handle each accordingly
	//
	if (pCurrConvSet->ApplyConventionTests(*m_pPlayer, *m_pHand, *m_pCardLocation, m_ppGuessedHands, *this, *m_pStatusDlg))
	{
		return ValidateBid(m_nBid);
	}



	//
	//============================================================
	//
	// did partner pass?
	//
	if (nPartnersBid == BID_PASS)
	{
		m_nBid = BID_PASS;
		status << "BXPNTp! Partner has passed, so we pass also.\n";
		return ValidateBid(m_nBid);
	}




	//
	//============================================================
	//
	// See if partner finally raised our suit 
	// e.g., 
	//   1S -> 2H
	//   2S -> 3S, raising our suit after we rebid it
	// or
	//	 1S -> 2H
	//	 3D -> 4D, raising our new suit
	// we want to raise the contract to game if possible, and also see 
	// if we have any pretensions towards slam
	//
	if ((m_nAgreedSuit == NONE) && (nPartnersSuit == nPreviousSuit) &&
											(nPartnersSuit != NOTRUMP))
	{
		// see if we had shifted last time
		if ((nPreviousSuit != nFirstRoundSuit) && ISSUIT(m_nIntendedSuit) && 
			(m_nIntendedSuit == nPartnersPrevSuit))
		{
			m_nAgreedSuit = nPartnersPrevSuit;

			// this is a bit strange, as we need to return to partner's suit
			// see if partner's already gone past game
			if (nPartnersBid > GetGameBid(m_nAgreedSuit))
			{
				status << "BXA10! Partner has bid " & ((nPartnersBidLevel < 6)? "game" : "a slam") &
						  " in the " & szPVSS & " suit, which we really didn't intend.\n";
				// bid on only if we have aspirations to slam
				if (m_fMinTPPoints >= PTS_SLAM)
				{
					status << "BXA20! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " pts in the partnership, try for a slam in partner's original " & 
							  STSS(m_nAgreedSuit) & " suit.\n";
					InvokeBlackwood(m_nAgreedSuit);
					return ValidateBid(m_nBid);
				}
				else
				{
					m_nBid = BID_PASS;
					status << "BXA30! Partner has bid " & ((nPartnersBidLevel < 6)? "game" : "a slam") &
							  " in the " & szPVSS & " suit, which we really didn't intend to play in, but with only " & 
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " pts in the partnership, we don't want to bid further, so pass.\n";
					return ValidateBid(m_nBid);
				}
			}

			// else raise to game or close to it, if possible
			// note: we make an extra effort here to raise partner,
			// so that we can get back to the "correct" suit
			status << "BXA40! Partner has raised us in the " & szPVSS & 
					  " suit, which we really didn't intend to play in, so try to switch back to partner's " & 
					  STSS(m_nAgreedSuit) & " suit.\n";

			// raise to the 3-level if possible
			if (RaisePartnersSuit(SUIT_PREV, RAISE_TO_3, PTS_GAME-3, PTS_GAME-1, SUPLEN_3))
				return ValidateBid(m_nBid);
			
			// raise a major to game with 3-card support & 26 pts
			if (RaisePartnersSuit(SUIT_PREV_MAJOR, RAISE_TO_GAME, PTS_GAME, 99, SUPLEN_3))
				return ValidateBid(m_nBid);

			// raise a minor to the 4-level if possible
			if (RaisePartnersSuit(SUIT_PREV_MINOR, RAISE_TO_4, PTS_MINOR_GAME-3, PTS_MINOR_GAME-1, SUPLEN_3))
				return ValidateBid(m_nBid);

			// raise a minor to game with 4-card support & 29 pts
			if (RaisePartnersSuit(SUIT_PREV_MINOR, RAISE_TO_GAME, PTS_MINOR_GAME, 99, SUPLEN_4))
				return ValidateBid(m_nBid);

			// or raise a minor to game with 3-card support & 30 pts
			if (RaisePartnersSuit(SUIT_PREV_MINOR, RAISE_TO_GAME, PTS_MINOR_GAME+1, 99, SUPLEN_3))
				return ValidateBid(m_nBid);

			// else pass
			m_nBid = BID_PASS;
			status << "BXA60! However, we don't have enough points to raise partner any further, so pass.\n";
			return ValidateBid(m_nBid);

		}
		else
		{
			// no problemo, partner agreed to the suit after our rebid 
			// so set suit agreement
			m_nAgreedSuit = nPreviousSuit;

			// see if partner bid slam in the suit
			if (nPartnersBidLevel >= 6)
			{
				if ((nPartnersBidLevel == 6) && (m_fMinTPPoints >= PTS_GRAND_SLAM))
				{
					m_nBid = MAKEBID(m_nAgreedSuit, 7);
					status << "BXA90! Partner has returned to our suit at a small slam level, and with a total of " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " pts in the partnership, push on to a grand slam at " & BTS(m_nBid) & ".\n";
				}
				else
				{
					m_nBid = BID_PASS;
					status << "BXA91! Partner has returned to our suit at slam level, so pass and go for it.\n";
				}
				return ValidateBid(m_nBid);
			}
			// else see if partner bid game in the suit
			else if (nPartnersBid >= GetGameBid(m_nAgreedSuit))
			{
				// bid on only if we have aspirations to slam
				if (m_fMinTPPoints >= PTS_SLAM)
				{
					InvokeBlackwood(m_nAgreedSuit);
					return ValidateBid(m_nBid);
				}
				else
				{
					m_nBid = BID_PASS;
					status << "BXB30! Partner has bid " &
						((nPartnersBidLevel < 6)? "game" : "a slam") &
							  " in the " & szPVSS & 
							  " suit, and with only " & 
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " pts in the partnership, we don't want to raise further, so pass.\n";
					return ValidateBid(m_nBid);
				}
			}

			// check raise level
			int nRaiseLevel = nPartnersBidLevel - nPreviousBidLevel;
			if (nRaiseLevel == 1)
			{
				// single raise
				return RespondToSingleRaise(nPartnersBid);
			}
			else if (nRaiseLevel == 2)
			{
				// double raise
				return RespondToDoubleRaise(nPartnersBid);
			}
			else if ((nRaiseLevel == 3)  &&
					 ( (ISMAJOR(nPartnersSuit) && (nPartnersBidLevel == 4)) ||
					   (ISMINOR(nPartnersSuit) && (nPartnersBidLevel == 5)) ))
			{
				// triple raise to game
				return RespondToTripleRaise();
			}
			else
			{
				// strange raise -- just pass
				m_nBid = BID_PASS;
				status << "BXB40! With an unusual raise by partner from " &
						  szPVB & " to " & szPB & ", we just pass.\n";
				return ValidateBid(m_nBid);
			}
		}

	}



	//
	//============================================================
	//
	// See if partner raised our first suit after we showed another suit
	// e.g., 
	//   1S -> 2H
	//   3C -> 3S, raising our first suit instead of the second
	// this is similar to the cases above, but cannot be handled 
	// in quite the same way as a simple raise
	//
	if ((m_nAgreedSuit == NONE) && (nPreviousSuit != nFirstRoundSuit) &&
								   (nPartnersSuit == nFirstRoundSuit) &&
								   (nPartnersSuit != NOTRUMP))
	{
		// partner has finally come around
		m_nAgreedSuit = nFirstRoundSuit;
		int numCards = numCardsInSuit[m_nAgreedSuit];
		status << "BXC00! Partner has returned to our first suit, raising it to " & 
				  BTS(nPartnersBid) & ".\n";
		// adjust points as declarer
		fAdjPts = m_pHand->RevalueHand(REVALUE_DECLARER, nPartnersSuit, TRUE);
		m_fMinTPPoints = fAdjPts + m_fPartnersMin;
		m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;

		// see if we have the points for a slam
		if (m_fMinTPPoints < PTS_SLAM) 
		{
			// no slam, but maybe we can still raise the bid further			
			if (nPartnersBid < GetGameBid(m_nAgreedSuit))
			{
				// raise to game if possible
				if (ISMAJOR(m_nAgreedSuit) && (numCards >= 4))
				{
					// in majors
					if (m_fMinTPPoints >= PTS_GAME)
					{
						m_nBid = GetGameBid(m_nAgreedSuit);
						status << "BXC02! And with " & numPrefSuitCards & " trump cards and " &
								  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
								  " team points, push on to game at " & BTS(m_nBid) & ".\n";
						return ValidateBid(m_nBid);
					}
					else if ((m_fMinTPPoints >= PTS_GAME-3) && (nPartnersBidLevel == 2))
					{
						m_nBid = MAKEBID(m_nAgreedSuit, 3);
						status << "BXC03! But with only " &
								  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
								  " team points, we don't have quite enough for game, so raise only to " & BTS(m_nBid) & ".\n";
						return ValidateBid(m_nBid);
					}
				}
				else if (numCards >= 4)
				{
					// in minors with 4+ trumps
					if (m_fMinTPPoints >= PTS_MINOR_GAME)
					{
						m_nBid = GetGameBid(m_nAgreedSuit);
						status << "BXC06! And with " & numPrefSuitCards & " trump cards and " &
								  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
								  " team points, push on to a minor game at " & BTS(m_nBid) & ".\n";
						return ValidateBid(m_nBid);
					}
					else
					{
						// raise to 3 or 4 of the minor if possible
						if ((m_fMinTPPoints >= PTS_MINOR_GAME-3) && (nPartnersBidLevel <= 3))
						{
							m_nBid = MAKEBID(m_nAgreedSuit, 4);
							status << "BXC07! But with only " &
									  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
									  " team points, we don't have quite enough for game, so raise only to " & BTS(m_nBid) & ".\n";
							return ValidateBid(m_nBid);
						}
						else if ((m_fMinTPPoints >= PTS_MINOR_GAME-6) && (nPartnersBidLevel == 2))
						{
							m_nBid = MAKEBID(m_nAgreedSuit, 3);
							status << "BXC08! But with only " &
									  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
									  " team points, we don't have quite enough for game, so raise only to " & BTS(m_nBid) & ".\n";
							return ValidateBid(m_nBid);
						}
					}
				}
				// else just pass
				m_nBid = BID_PASS;
				status << "BXC20! With " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
						  " points in the partnership and no aspirations for a higher contract, we sign off on partner's bid of " &
						  szPB & ".\n";
				return ValidateBid(m_nBid);
			}
			else
			{
				// partner already made at least a game-level bid
				if (IsGameBid(nPartnersBid)) 
				{
					if (nPartnersBidLevel < 6)
					{
						// sign off on the game bid
						m_nBid = BID_PASS;
						status << "BXC30! With a game contract reached at " & szPB & ", we pass.\n";
					}
					else
					{
						// sign off on a slam bid
						m_nBid = BID_PASS;
						status << "BXC32! With a contract reached at " & szPB & ", we pass.\n";
					}
				}
				return ValidateBid(m_nBid);
			}
		}

		// if we have the points for a slam, bid on past game and to a slam
		if ((m_fMinTPPoints <= PTS_GRAND_SLAM) && (nPartnersBidLevel < 6))
		{
			m_nBid = MAKEBID(nPartnersSuit, 6);
			status << "BXC40! With an agreed suit of " & STS(nPartnersSuit) & 
				      " and " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " total partnership points, bid on to slam at " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}
		// here, we have 37+ pts (whoopee!), so bid 7 of the suit
		if ((m_fMinTPPoints >= PTS_GRAND_SLAM) && (nPartnersBidLevel < 7))
		{
			m_nBid = MAKEBID(nPartnersSuit, 7);
			status << "BXC44! With an agreed suit of " & STS(nPartnersSuit) & 
				      " and " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
					  " total partnership points, bid on to slam at " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}
		// otherwise accept the slam contract & pass
		m_nBid = BID_PASS;
		status << "BXC48! With a slam contract reached at " & szPB & 
				  " and holding " & m_fMinTPPoints & "-" & m_fMaxTPPoints & 
				  " pts in the partnership, sign off on partner's bid.\n";
		return ValidateBid(m_nBid);
	}



	

	//
	//============================================================
	//
	// See if partner returned to our suit after we ventured out
	// to show a side suit (not cue bid though)
	// e.g.,
	//   1S -> 2S
	//   3C -> 3S
	// i.e., we have an agreed suit, which partner supported,
	// but we bid a different suit last time
	//
	if ((m_nAgreedSuit != NONE) && (nPreviousSuit != m_nAgreedSuit) &&
								   (nPartnersSuit == m_nAgreedSuit) &&
								   (nPartnersSuit != NOTRUMP))
	{
		status << "2BXU00! Partner returned to our agreed " & STSS(m_nAgreedSuit) &
				  " at a bid of " & BTS(nPartnersBid) & 
				  " after we showed the " & STSS(nPreviousSuit) & 
				  " side suit.\n";
		//
		// either pass or return to the agreed suit at game or slam
		//
		if (m_fMinTPPoints < PTS_SLAM)
		{
			//
			// bid on to game with 26+ pts, otherwise pass
			//
			if (nPartnersBid < GetGameBid(m_nAgreedSuit))
			{
				if ( (ISMAJOR(m_nAgreedSuit) && (m_fMinTPPoints >= PTS_GAME)) ||
					 (ISMINOR(m_nAgreedSuit) && (m_fMinTPPoints >= PTS_MINOR_GAME)) )
				{
					// bid on to game
					m_nBid = GetGameBid(m_nAgreedSuit);
					status << "BXU20! With a total of " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " points in the partnership, go ahead and bid game at " &
							  BTS(m_nBid) & ".\n";
				}
				else
				{
					// not enough points for game, so pass
					m_nBid = BID_PASS;
					status << "BXU22! With a total of " & 
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " partnership points, we don't have the strength to bid game, so pass.\n";
				}
			}
			else
			{
				m_nBid = BID_PASS;
				status << "BXU30! Partner has bid game, and we see no reason to move further, so pass.\n";
			}
		}
		else
		{
			//
			// we have 33+ points -- bid on to slam with Blackwood
			//
			if (nPartnersBidLevel < 6)
			{
				InvokeBlackwood(m_nAgreedSuit);
			}
			else if (nPartnersBidLevel == 6)
			{
				// partner bid a small slam
				if (m_fMinTPPoints <= PTS_GRAND_SLAM)
				{
					m_nBid = BID_PASS;
					status << "BXU50! Partner has bid a small slam, and with " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						      " points in the partnership, we're satisfied with the bid, so pass.\n";
				}
				else
				{
					m_nBid = MAKEBID(m_nAgreedSuit, 7);
					status << "BXU55! Partner has bid a small slam, and with " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						      " points in the partnership, we boldly raise to " &
							  BTS(m_nBid) & ".\n";
				}
			}
			else
			{
				// partner bid a grand slam!
				status << "BXU90! Partner has bid a grand slam, so pass and hope for the best.\n";
			}
		}
		// done
		return ValidateBid(m_nBid);
	}




	//
	//==========================================================
	//
	// Rebidding at stage 5,after 2 suits have been shown
	//
	// both players have rebid their own suits
	// in this event, use the following rules to arbitrate:
	// ---------------
	//
	// Bidding a MAJOR vs. a MAJOR or minor:
	//   holding 6 or fewer cards:
	//     - pass if above game in own suit
	//     - support partner if holding 2+ card support
	//     - else rebid own suit
	//   holding 7 or more cards:
	//     - rebid the suit if below game
	//     - else pass
	//
	// Bidding a minor vs. a MAJOR:
	//   holding 7 or fewer cards:
	//     - pass if above game in own suit
	//     - support partner if holding 2+ card support
	//     - rebid otherwise
	//   holding 8 or more cards:
	//     - rebid the suit if below game
	//     - pass otherwise
	//
	// Bidding a minor vs. a minor:
	//   holding 6 or fewer cards:
	//	   - bid 3NT with all suits stopped, if possible
	//     - pass if above game in own suit
	//     - support partner if holding 3+ card support
	//     - rebid otherwise
	//   holding 7 or more cards:
	//     - rebid the suit if below game
	//	   - bid 3NT with all suits stopped, if possible
	//     - pass otherwise
	//

	if ((nRound == 2) && (nPartnersSuit == nPartnersPrevSuit) &&
					(nPrefSuit == nPreviousSuit) &&
					(nPartnersSuit != nPreviousSuit) &&
//					((nPartnersBid - nPreviousBid) < 5) &&
		  					(nPartnersSuit != NOTRUMP)) 
	{
		//
		// see if partner jump rebid his suit
		//
		if ((nPartnersBid - nPreviousBid) > 5)
		{
			m_fPartnersMin = MAX(m_fPartnersMin, 11);
			m_fPartnersMax = MAX(m_fPartnersMax, 18);
			m_fMinTPPoints = fAdjPts + m_fPartnersMin;
			m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
			m_fMinTPCPoints = fCardPts + m_fPartnersMin;
			m_fMaxTPCPoints = fCardPts + m_fPartnersMax;
			status << "2BXF01! Partner jump rebid his " & szPSS & " suit at " & szPB &
					  ", indicating 11-18 pts.\n";
		}

		//		
		//--------------------------------------------------------------------
		// see if we're bidding a major
		//
//		if (ISMAJOR(nPrefSuit) && ISMAJOR(nPartnersSuit))
		if (ISMAJOR(nPrefSuit))
		{

			// see if we have <= 6 cards in our suit
			if (numPrefSuitCards <= 6)
			{
				// with <= 6 cards, pass if above game in own suit
				if ((nPartnersBid >= GetGameBid(nPrefSuit)) &&
											(m_fMaxTPPoints < PTS_SLAM))
				{
					m_nBid = BID_PASS;
					status << "BXF10! At this point, the partnership has " &
							   m_fMinTPPoints & "-" & m_fMaxTPPoints & 
							   " total points, but lacks agreement on a suit.  In addition, we are above game level in our own " &
							   szPrefS & " suit.  So at this point, we pass.\n";
					return ValidateBid(m_nBid);
				}

				// support partner if holding 2+ cards in support
				// bid towards game with 26 - 32 TP points
				if (nPartnersBidLevel < 4)
					if (RaisePartnersSuit(SUIT_MAJOR,RAISE_ONE,PTS_GAME,PTS_SLAM-1,SUPLEN_2))
						return ValidateBid(m_nBid);

				// or towards slam with 33+ pts and 2+ card support
				if (RaisePartnersSuit(SUIT_MAJOR,RAISE_ONE,PTS_SLAM,99,SUPLEN_2))
					return ValidateBid(m_nBid);

				// else we lack 2+ card support for partner, so
				// rebid our own strong suit with 26+ TP's
				if (RebidSuit(SUIT_MAJOR,REBID_CHEAPEST,PTS_GAME,PTS_SLAM-1,LENGTH_6,SS_STRONG))
					return ValidateBid(m_nBid);

				// else pass
				m_nBid = BID_PASS;
				status << "BXF14! At this point, the partnership has " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " total points, and lacks agreement on a suit.  With only weak (" & 
						  szHP & ") support for partner, we have to pass.\n";
				return ValidateBid(m_nBid);
			}
			else
			{
				// else we have 7+ cards in our preferred suit
				// rebid our own suit if below game
				if (GetGameBid(nPrefSuit) > nPartnersBid)
				{
					m_nBid = GetCheapestShiftBid(nPrefSuit);
					status << "BXF20! With " & numPrefSuitCards & 
							  " cards in our preferred " & szPrefS & 
							  " suit, rebid it again at " & BTS(m_nBid) & ".\n";
					return ValidateBid(m_nBid);
				}

				// support partner if holding 2+ cards in support
				// bid towards game with 26 - 32 TP points
				if (nPartnersBidLevel < 4)
					if (RaisePartnersSuit(SUIT_MAJOR,RAISE_ONE,PTS_GAME,PTS_SLAM-1,SUPLEN_2))
						return ValidateBid(m_nBid);

				// else just pass with <= 32 total points
				if (m_fMaxTPPoints < PTS_SLAM)
				{
					m_nBid = BID_PASS;
					status << "BXF24! Despite having " & numPrefSuitCards & 
							  " cards in our preferred " & szPrefS & 
							  " suit and " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " partnership points, we can't rebid our preferred suit without exceeding game, so pass.\n";
					return ValidateBid(m_nBid);
				}

				// with 33+ points, explore slam somehow
				// with 2+ card support, bid 6 of partner's suit
				if (RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_6,PTS_SLAM,99,SUPLEN_2))
					return ValidateBid(m_nBid);

				// else with a solid 8+ card suit, bid 6 of our suit
				if (RebidSuit(SUIT_MAJOR,REBID_AT_6,PTS_SLAM,99,LENGTH_8,SS_STRONG|SS_SOLID))
					return ValidateBid(m_nBid);

				// else plan on 6/7NT and invoke Blackwood
				InvokeBlackwood(NOTRUMP);
				return ValidateBid(m_nBid);
			}
		}

		//
		//--------------------------------------------------------------------
		// see if we're bidding a minor against a MAJOR
		//
		if (ISMINOR(nPrefSuit) && ISMAJOR(nPartnersSuit))
		{

			// see if we have <= 7 cards in our suit
			if (numPrefSuitCards <= 7)
			{
				// holding 7 or fewer cards:
				//   - pass if above game in own suit
				//   - support partner if holding 2+ card support
				//   - rebid otherwise

				// pass if above game in our own suit		
				if (nPartnersBid >= GetGameBid(nPrefSuit) &&
											(m_fMaxTPPoints < PTS_SLAM))
				{
					m_nBid = BID_PASS;
					status << "BXG10! At this point, the partnership has " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " total points, but lacks agreement on a suit.  In addition, we are above game level in our own " & 
							  szPrefS & " suit.  So at this point, we pass.\n";
					return ValidateBid(m_nBid);
				}

				// support partner if holding 2+ cards in support
				// bid towards game with 26 - 32 TP points
				if (nPartnersBidLevel < 4)
					if (RaisePartnersSuit(SUIT_MAJOR,RAISE_ONE,PTS_GAME,PTS_SLAM-1,SUPLEN_2))
						return ValidateBid(m_nBid);
			
				// otherwise rebid with 6 or 7 cards
				if (RebidSuit(SUIT_MINOR,REBID_CHEAPEST,0,99,LENGTH_6,SS_STRONG))
					return ValidateBid(m_nBid);

				// else pass
				m_nBid = BID_PASS;
				status << "BXG40! Despite having " & numPrefSuitCards & 
						  " cards in our preferred " & szPrefS & " suit and " & 
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " partnership points, we can't push our minor and can't support partner's major, so pass.\n";
				return ValidateBid(m_nBid);
			}
			else
			{
				// holding 8 or more cards:
				//   - rebid the suit if below game
				//   - support partner if holding 2+ card support
				//   - pass otherwise

				// rebid if below game
				if (nPartnersBid < GetGameBid(nPrefSuit))
					if (RebidSuit(SUIT_MINOR,REBID_CHEAPEST,0,99,LENGTH_6,SS_STRONG))
						return ValidateBid(m_nBid);

				// support partner if holding 2+ cards in support
				// bid towards game with 26 - 32 TP points
				if (nPartnersBidLevel < 4)
					if (RaisePartnersSuit(SUIT_MAJOR,RAISE_ONE,PTS_GAME,PTS_SLAM-1,SUPLEN_2))
						return ValidateBid(m_nBid);
			
				// else pass
				m_nBid = BID_PASS;
				status << "BXG50! Despite having " & numPrefSuitCards & 
						  " cards in our preferred " & szPrefS & 
						  " suit and " & m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " partnership points, we can't push our minor and can't support partner's major, so pass.\n";
				return ValidateBid(m_nBid);
			}

		}

		//
		//--------------------------------------------------------------------
		// see if we're bidding a minor against another minor
		//
		if (ISMINOR(nPrefSuit) && ISMINOR(nPartnersSuit))
		{

			// see if we have <= 6 cards in our suit
			if (numPrefSuitCards <= 6)
			{

				// with 6 or fewer cards,
				//   - bid 3NT with all suits stopped, if possible
				//   - support partner if holding 3+ card support
				//   - rebid with 33+ points
				//   - else pass

				//
				// with all suits stopped, bid 3NT
				if (m_pHand->AllOtherSuitsStopped(nPartnersSuit))
				{
					m_nBid = BID_3NT;
					status << "BXH10! With a " & numPrefSuitCards & 
							  "-card minor and all other suits stopped, bid " & 
							  BTS(m_nBid) & ".\n";
					return ValidateBid(m_nBid);
				}

				// support partner with 3+ card support and 22+ pts
				if (nPartnersBidLevel < 5)
					if (RaisePartnersSuit(SUIT_MINOR,RAISE_ONE,PTS_MINOR_GAME-7,99,SUPLEN_3))
						return ValidateBid(m_nBid);

				// pass if above game in own suit
				if (nPartnersBid >= GetGameBid(nPrefSuit) &&
											(m_fMaxTPPoints < PTS_SLAM))
				{
					m_nBid = BID_PASS;
					status << "BXH14! At this point, the partnership has " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " total points, but lacks agreement on a suit.  In addition, we are above game level in our own " &
							  szPrefS & " suit.  So at this point, we pass.\n";
					return ValidateBid(m_nBid);
				}
				
				// else rebid suit with 6 cards
				if (RebidSuit(SUIT_MINOR,REBID_CHEAPEST,0,99,LENGTH_6,SS_STRONG))
					return ValidateBid(m_nBid);
				
				// or with 33+ points and 4+ cards
				if (RebidSuit(SUIT_MINOR,REBID_CHEAPEST,PTS_SLAM,99,LENGTH_4,SS_STRONG))
					return ValidateBid(m_nBid);

				// else pass
				m_nBid = BID_PASS;
				status << "BXH40! We have a " & numPrefSuitCards & "-card " & szPrefS & 
						  " suit, and partner is pushing the " & szPSS & 
						  " suit, but we have no agreement, and lack complete stoppers, so we have to pass.\n";
				return ValidateBid(m_nBid);
			
			}
			else
			{
				// here, we have 7+ cards, so:
				//   - rebid the suit if below game
				//	 - bid 3NT with all suits stopped, if possible
				//   - pass otherwise

				// rebid the suit if below game
				if (nPartnersBid < GetGameBid(nPrefSuit))
					if (RebidSuit(SUIT_MINOR,REBID_CHEAPEST,0,99,LENGTH_6,SS_STRONG))
						return ValidateBid(m_nBid);

				// with all suits stopped, bid 3NT
				if (m_pHand->AllOtherSuitsStopped(nPartnersSuit))
				{
					m_nBid = BID_3NT;
					status << "BXI10! With a " & numPrefSuitCards &
							  "-card minor and all other suits stopped, bid " & 
							  BTS(m_nBid) & ".\n";
					return ValidateBid(m_nBid);
				}

				m_nBid = BID_PASS;
				status << "BXI40! Despite holding a " & numPrefSuitCards & "-card " & szPrefS &
						  " suit, we can't come to agreement with partner, so we have to pass.\n";
				return ValidateBid(m_nBid);
				// else pass
			}

		}
	}





	//
	//==========================================================
	//
	// Rebidding at stage 5 or later, after 3 suits have been shown
	//
	// partner rebid his suit (2nd in the partnership) after we 
	// shifted in response
	// e.g., 1H -> 1S,
	//		 2C -> 2S
	//
	// alternatively, partner bid a new suit after we rebid our suit
	// e.g., 1H -> 1S
	//       2H -> 3C
	//
	// most likely, we're at the 3-level now, so need 22+ points 
	// to continue bidding
	//
	// Arbitrate according to the following rules
	//
	//  - support partner with 3+ support cards
	//  - rebid a self-supporting suit
	//  - support partner's rebid major with 2+ trumps
	//  - bid 3NT with the other suits stopped
	//  - support partner's rebid minor with 2+ trumps
	//  - pass otherwise
	//

	if ((nRound >= 2) && (nPartnersSuit == nPartnersPrevSuit) &&
						    (nPreviousSuit != nPartnersSuit) &&
						    (nPrefSuit != nPreviousSuit) &&
		  					(nPartnersSuit != NOTRUMP)) 
	{
		// see if partner rebid at the game level
		int nPartnersGameBid = GetGameBid(nPartnersSuit);
		if ((nPartnersBidLevel >= nPartnersGameBid) &&
									(m_fMinTPPoints < PTS_SLAM))
		{
			m_nBid = BID_PASS;
			status << "BXK05! Partner has rebid his suit at " &
					  ((nPartnersBidLevel < 6)? "game" : "slam") &
					  "level, and we just don't have the strength to venture further, so pass.\n";
			return ValidateBid(m_nBid);
		}

		//
		BOOL bRebidTwice = FALSE;
		if (nPartnersSuit == BID_SUIT(m_pPartner->GetPriorBid(0)))
		{
			status << "BXK06! Partner has rebid his suit twice, indicating 6+ cards.\n";
			bRebidTwice = TRUE;
		}

		//
		// if below game, support partner's suit with 2 or 3+ support cards
		//
		if (nPartnersBid < nPartnersGameBid)
		{
			// 26-32 pts
			if (RaisePartnersSuit(SUIT_ANY,RAISE_TO_4,PTS_GAME,PTS_SLAM-1,
								  (bRebidTwice)? SUPLEN_2:SUPLEN_3,STRENGTH_ANY))
				return ValidateBid(m_nBid);
			// 23-32 pts
			if (RaisePartnersSuit(SUIT_ANY,RAISE_TO_3,PTS_GAME-3,PTS_SLAM-1,
								  (bRebidTwice)? SUPLEN_2:SUPLEN_3,STRENGTH_ANY))
				return ValidateBid(m_nBid);
		}

		//
		// bid a self-supporting suit at the 3 or 4 level
		//
		if (bPrefSuitIsSelfSupporting)
		{	
			int nBid = GetCheapestShiftBid(nPrefSuit);
			int nBidLevel = BID_LEVEL(nBid);
			if (IsBidSafe(nBid))
			{
				m_nBid = nBid;
				status << "BXK10! With only " & numSupportCards & 
						  "-card support for partner's " & szPS & 
						  ", bid our self-supporting " & szPrefSS & " suit at " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
		}

		//
		// support partner's rebid MAJOR suit with 2 support cards
		// raise to the 3-level with 23+ TPs, or to 4 with 26
		//
		if (ISMAJOR(nPartnersSuit) && (nPartnersSuit < nPartnersPrevSuit) &&
					(nPartnersBid < nPartnersGameBid))
		{
			// 27-32: raise to 4
			if (RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_4,PTS_GAME+1,PTS_SLAM-1,SUPLEN_2,STRENGTH_ANY))
				return ValidateBid(m_nBid);
			// 24-32: raise to 3
			if (RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_3,PTS_GAME-2,PTS_SLAM-1,SUPLEN_2,STRENGTH_ANY))
				return ValidateBid(m_nBid);
		}

		//
		// bid 3NT with the other suits stopped and 26-32 HCPs
		//
		if ((m_pHand->AllOtherSuitsStopped(nPrefSuit, nPartnersSuit)) &&
					(nPartnersBid < BID_3NT) && 
					(m_fMinTPCPoints >= PTS_GAME) && (m_fMinTPCPoints < PTS_SLAM))
		{
			m_nBid = BID_3NT;
			status << "BXK20! With no agreement in suits, and all other suits stopped, bid " & 
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		//
		// with our options running out, support partner's rebid 
		// MINOR with 2 or 3 support cards
		// raise to the 3-level with 24+ pts, to the 4-level with 27+ pts, 
		// or to game with 30+
		//
		if (ISMINOR(nPartnersSuit) && (nPartnersSuit < nPartnersPrevSuit) &&
					(nPartnersBid < nPartnersGameBid))
		{
			if (RaisePartnersSuit(SUIT_ANY,RAISE_TO_3,PTS_GAME-1,PTS_GAME,
								  (bRebidTwice)? SUPLEN_2:SUPLEN_3,STRENGTH_ANY))
				return ValidateBid(m_nBid);
			if (RaisePartnersSuit(SUIT_ANY,RAISE_TO_4,PTS_GAME+1,PTS_SLAM-1,
								  (bRebidTwice)? SUPLEN_2:SUPLEN_3,STRENGTH_ANY))
				return ValidateBid(m_nBid);
			if (RaisePartnersSuit(SUIT_ANY,RAISE_TO_5,PTS_MINOR_GAME+1,PTS_SLAM-1,
								  (bRebidTwice)? SUPLEN_2:SUPLEN_3,STRENGTH_ANY))
				return ValidateBid(m_nBid);
		}

		//
		// rebid a strong 6-card suit w/ 23+ pts
		//
		if (nPartnersBid < nPartnersGameBid)
			if (RebidSuit(SUIT_ANY,SHIFT_CHEAPEST,PTS_GAME-3,99,LENGTH_6,SS_STRONG))
				return ValidateBid(m_nBid);

		//
		// raise partner's _rebid_ suit to game or slam with exceptional strength
		// need 2 cards and 33/37 pts for slam, or 1 card and 28/31 pts for game
		//
		if (RaisePartnersSuit(SUIT_ANY,RAISE_TO_7,PTS_GRAND_SLAM+1,0,SUPLEN_2,STRENGTH_ANY))
			return ValidateBid(m_nBid);
		if (RaisePartnersSuit(SUIT_ANY,RAISE_TO_6,PTS_SLAM+1,0,SUPLEN_2,STRENGTH_ANY))
			return ValidateBid(m_nBid);
		if (RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_GAME,PTS_GAME+2,0,
							  (bRebidTwice)? SUPLEN_1:SUPLEN_2,STRENGTH_ANY))
			return ValidateBid(m_nBid);
		if (RaisePartnersSuit(SUIT_MINOR,RAISE_TO_GAME,PTS_MINOR_GAME+2,0,
							  (bRebidTwice)? SUPLEN_1:SUPLEN_2,STRENGTH_ANY))
			return ValidateBid(m_nBid);

		//
		// with slam points and no suit agreement, bid notrump
		//
		if ((m_fMinTPCPoints >= PTS_SLAM) && (bSemiBalanced))
		{
			if (m_fMinTPCPoints >= PTS_GRAND_SLAM)
				m_nBid = BID_7NT;
			else 
				m_nBid = BID_6NT;
			status << "BXK30! With no agreement in suits but with " & 
					  m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
					  " HCPs in the partnership, go ahead and bid slam at " & 
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		//
		// if partner bid at the 3-level and we have absolutely no support cards,
		// bail out at 3NT if possible
		//
		if ((nPartnersBidLevel == 3) && (nPartnersSuit != NOTRUMP) && 
					(numSupportCards == 0) &&
					((m_fMinTPCPoints >= PTS_GAME) || (m_fMinTPPoints >= PTS_GAME+1)) )
		{
			m_nBid = BID_3NT;
			status << "BXK36! With no agreement in suits and zero trump support for partner's " & szPS &
					  ", bail out at " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}


		//
		// we may still fall through the cracks in the above code --
		// e.g., partner bid at the game level, and we have good strength, but
		// can't agree on a suit
 		//

		//
		// otherwise pass
		//
		m_nBid = BID_PASS;
		status << "BXK40! With no agreement in suits and " & 
				  m_fMinTPPoints & "-" & m_fMaxTPPoints &
				  " partnership points, pass partner's " & szPB & " bid.\n";
		return ValidateBid(m_nBid);
	}









	//
	//=============================================================
	//
	// Rebidding at stage 5, after 3 or 4 suits have been shown
	//
	// partner shifted to another suit after our own shift
	// shifted in response
	// e.g., 1H -> 1S -> 2C -> 2D
	//  or   1H -> 1S -> 2S -> 3H
	//
	// We can't agree on a suit at this point, so arbitrate according 
	// to the following rules
	// ---------------
	//  - rebid a strong major, if it's our preferred suit, with 22+ pts
	//  - bid 3NT with no voids, with 26+ HCPs
	//  - else support one of partner's suits with 22+ pts
	//  - pass otherwise

	if ((nRound == 2) && (nPartnersSuit != nPartnersPrevSuit) &&
							(nFirstRoundSuit != nPreviousSuit) &&
		  							(nPartnersSuit != NOTRUMP)) 
	{
		// remark on partner's move
		if (nPartnersSuit == nNextPrevSuit)
		{
			// partner returned to our suit from two rounds ago
			status << "2BXL00! Partner has returned to the " & szNPSS & 
					  " suit that we had bid earlier with a bid of " & szPB & ".\n";
			// re-evaluate pts as declarer
			int nContractPos = PlayerOpenedSuit(nPartnersSuit)? REVALUE_DECLARER : REVALUE_DUMMY;
			status << "3BXL01! We need to re-evaluate holdings for the " & szNPSS & " suit as " &
					  ((nContractPos == REVALUE_DECLARER)? "declarer" : "dummy") & ".\n";
			fAdjPts = m_pHand->RevalueHand(nContractPos, nPartnersSuit, TRUE, TRUE);
			m_fMinTPPoints = fAdjPts + m_fPartnersMin;
			m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
			m_fMinTPCPoints = fCardPts + m_fPartnersMin;
			m_fMaxTPCPoints = fCardPts + m_fPartnersMax;
		}
		else
		{
			// partner bid another suit
			status << "BXL05! Partner has bid yet another suit, " & szPS & ".\n";
		}

		//
		// rebid a strong major, if it's our preferred suit, with 22+ pts
		//
		if ((ISMAJOR(nPrefSuit)) && (nPrefSuitStrength >= SS_STRONG) &&
						(nPartnersBidLevel <= 2) && (m_fMinTPPoints >= PTS_GAME-3))
		{
			m_nBid = GetCheapestShiftBid(nPrefSuit);
			// see if the bid is low enough
			if (IsBidSafe(m_nBid,1))
			{
				status << "BXL10! With no agreement in suits, but with " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " partnership points, return to our preferred " & szPrefSS &
						  " suit at " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
		}

		//
		// bid 3NT with no voids and 26+ HCPs
		//
		if ((numVoids == 0) && (nPartnersBidLevel <= 3) && 
										(m_fMinTPCPoints >= PTS_GAME))
		{
			m_nBid = BID_3NT;
			status << "BXL20! With 4 suits bid and no agreement, but with " & 
					  m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
					  " high card points in the partnership and no void suits, bid " &
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		//
		// see if partner jump shifted, e.g.,
		//   1C -> 1S -> 1N -> 3D,
		// partner might also have jumped directly to game
		//

		// in general, support partner's first suit if possible
		// BUT, see if partner returned to our old suit, and 
		// also pick partner's suit if he just jump shifted
		if ((nPartnersSuit == nNextPrevSuit) && (nPrefSuit == nNextPrevSuit))
		{
			// return the first suit
			m_nAgreedSuit = nNextPrevSuit;
		}
		else if ((nPartnersSuit != nPartnersPrevSuit) && ((nPartnersBid - nPreviousBid) > 5))
		{
			// partner really want to play in this suit, so let him have it
			m_nAgreedSuit = nPartnersSuit;
		}
		else if ((nPPrevSuitSupport >= SS_WEAK_SUPPORT) &&
					(nPPrevSuitSupport >= nPartnersSuitSupport))
		{
			if (nPartnersPrevSuit != nPartnersSuit)
				status << "BXL30! Although partner bid " & szPS & 
						  ", we prefer to return to his previous " & szPPSS & " suit.\n";
			m_nAgreedSuit = nPartnersPrevSuit;
		}
		else if (nPartnersSuitSupport >= SS_WEAK_SUPPORT)
		{
			m_nAgreedSuit = nPartnersSuit;
		}

		//
		// pass if partner bid game, unless we have 33+ pts
		//
		if (IsGameBid(nPartnersBid))
		{
			// push on to slam if we have the pts
			if ((m_fMinTPPoints >= PTS_SLAM) && (nPartnersBidLevel < 6))
			{
				// we want to play slam, so pick a suit, even if it's NT
				if (!ISSUIT(m_nAgreedSuit))
					m_nAgreedSuit = NOTRUMP;
				//
				if (m_fMinTPPoints >= PTS_GRAND_SLAM)
					m_nBid = MAKEBID(m_nAgreedSuit, 7);
				else
					m_nBid = MAKEBID(m_nAgreedSuit, 6);
				//
				if (ISSUIT(m_nAgreedSuit))
					status << "BXL34! With a total of " & m_fMinTPPoints & 
							  "+ pts in the partnership, raise partner's " & 
							  ((m_nAgreedSuit == nPartnersPrevSuit)? "previous " : "") &
							  STSS(m_nAgreedSuit) & " suit to " & 
							  ((BID_LEVEL(m_nBid) == 7)? "a grand slam" : "slam") & 
							  " at " & BTS(m_nBid) & ".\n";
				else
					status << "BXL35! With a total of " & m_fMinTPPoints & 
							  "+ pts in the partnership and no suit agreement, push on to " &
							  ((BID_LEVEL(m_nBid) == 7)? "a grand slam" : "slam") & 
							  " at " & BTS(m_nBid) & ".\n";
			}
			else if (nPartnersBidLevel < 6)
			{
				// else pass
				m_nBid = BID_PASS;
				status << "BXL36! Without the strength to push further at " &
						   m_fMinTPPoints & "-" & m_fMaxTPPoints &
						   " total team points, pass and accept the contract of " & 
						   BTS(nPartnersBid) & ".\n";
			}
			else
			{
				// pass partner's slam
				m_nBid = BID_PASS;
				status << "BXL37! Pass and accept partner's slam bid.\n";
			}
			return ValidateBid(m_nBid);
		}
		else if (ISSUIT(m_nAgreedSuit))
		{
			// partner is still below game, so raise
//			m_nBid = MAKEBID(m_nAgreedSuit, nPartnersPrevBidLevel+1);
			m_nBid = GetCheapestShiftBid(m_nAgreedSuit);
			status << "BXL38! Support partner's " & STSS(m_nAgreedSuit) & 
					  " suit with " & numCardsInSuit[m_nAgreedSuit] & "-card support and " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " total team pts by bidding " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		// else we have no agreed suit -- 
		// bid 3NT (or higher) with no voids and 23+ HCPs
		if (numVoids == 0)
		{
			if ((m_fMinTPCPoints >= PTS_SLAM) && (nPartnersBid < BID_6NT))
			{
				m_nBid = BID_6NT;
				status << "BXL42! With no agreement in suits but with partner showing strength with is jump shift, and with a total of " &
						  m_fMinTPCPoints & "+ HCPs in the partnership, bid " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
			else if ((m_fMinTPCPoints >= PTS_GAME-1) && (nPartnersBid < BID_3NT))
			{
				m_nBid = BID_3NT;
				status << "BXL45! With no agreement in suits but with partner showing strength with is jump shift, bid " & 
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
		}


		//
		// raise one of partner's suits with 24+ points and 2-card support
		//
		if (nPPrevSuitSupport >= nPartnersSuitSupport)
			nSuit = SUIT_PREV;
		else
			nSuit = SUIT_ANY;

		// raise to 4-level with 26+ total pts
		if (RaisePartnersSuit(nSuit,RAISE_TO_4,PTS_GAME,99,SUPLEN_2))
			return ValidateBid(m_nBid);
		// raise to 3-level with 24 pts
		if (RaisePartnersSuit(nSuit,RAISE_TO_3,PTS_GAME-2,99,SUPLEN_2))
			return ValidateBid(m_nBid);

		//
		// else pass
		//
		m_nBid = BID_PASS;
		status << "BXL60! With no agreement in suits and " & 
				  m_fMinTPPoints & "-" & m_fMaxTPPoints &
				  " partnership points, pass.\n";
		return ValidateBid(m_nBid);
	}






	//
	//==========================================================
	//
	// Rebidding after partner bids NoTrump in a late round
	// e.g  1NT -> 2C  (not Stayman)
	//      2D  -> 2H
	//      2S  -> 2NT
	//      2S  -> 3NT
	//	    2NT -> 3NT
	//	    3NT -> 4NT
	// In general, if we have a a self-supporting suit, rebid it;
	// else pass on or raise No Trumps, or otherwise raise one of 
	// partner's earlier suits
	//
	if ((nRound >= 2) && (nPartnersSuit == NOTRUMP))
	{
		if (nPreviousSuit != NOTRUMP)
			status << "BXPNTA! Partner bid Notrump, indicating a dislike for our " & szPVSS & " suit.\n";

		//
		// first see if partner jumped to 3NT
		//
		if ((nPartnersBid - nPreviousBid) > 5)
		{
			// adjust points accordingly
			double fAllowance;
			int nEarlierSuit = m_pPlayer->GetPriorSuit(1);
			// see how we bid earlier
			if ((pCurrConvSet->IsOptionEnabled(tbStructuredReverses)) &&
						(ISSUIT(nPreviousSuit)) && (ISSUIT(nEarlierSuit)) && 
						(nPreviousSuit > nEarlierSuit) &&
						(nPreviousBidLevel == 2))
			{
				// see if we reversed last time
				fAllowance = PTS_GAME - 17;		// 26 pts - what partner expects, 17 for a reverse
			}
			else if (nFirstRoundBidLevel == 2)
			{
				if ((nFirstRoundBid == BID_2C) && 
					(pCurrConvSet->IsConventionEnabled(tidArtificial2ClubConvention)))
					fAllowance = PTS_GAME - pCurrConvSet->GetValue(tn2ClubOpeningPoints);
				else if (pCurrConvSet->IsConventionEnabled(tidWeakTwoBids))
					fAllowance = PTS_GAME - 6;	// 26 pts - 6 for a weak 2 opening
				else
					fAllowance = PTS_GAME - pCurrConvSet->GetValue(tn2ClubOpeningPoints);
					// 26 pts - 22 for a strong 2 opening
			}
			else
			{
				fAllowance = PTS_GAME - 12;	// 26 pts - 12 for standard 1 opening
			}
			m_fPartnersMin = MAX(m_fPartnersMin, fAllowance);
			m_fPartnersMax = MAX(22, 31 - fAllowance);
			m_fMinTPPoints = fAdjPts + m_fPartnersMin;
			m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
			status << "BXPNTJ! Partner jumped to a bid of " & szPB & " over our " &
					  szPVB & ", indicating " & m_fPartnersMin & "-" & m_fPartnersMax & 
					  " points.\n";
		}

		//
		// see if we have a self-supporting suit, AND we didn't bid NT previousy
		//
		if (m_pHand->IsSuitSelfSupporting(nPrefSuit) && (nPreviousSuit != NOTRUMP))
		{
			status << "BXPNT0! Partner prefers NoTrump, but we want to insist on our self-supporting " & 
					  STSS(nPrefSuit) & " suit.\n";
			// rebid our self-supporting suit, or invoke 
			// Blackwood with slam values
			if (m_fMinTPPoints < PTS_SLAM)
			{
				// jump to game with enough points
				if (ISMAJOR(nPrefSuit) && (m_fMinTPPoints >= PTS_GAME+1) &&
							(nPartnersBid < GetGameBid(nPrefSuit)))
				{
					// jumping in our preferred suit (a major)
					m_nBid = GetGameBid(nPrefSuit);
					status << "BXPNT1! So we jump with it to game at " & BTS(m_nBid) & ".\n";
				}
				else if ( ((nPartnersBidLevel == 2) && (m_fMinTPPoints >= PTS_GAME-3)) ||
						  ((nPartnersBidLevel == 3) && (m_fMinTPPoints >= PTS_GAME)) )
				{
					// else shift back to our suit, if we have the points
					m_nBid = GetCheapestShiftBid(nPrefSuit);
					status << "BXPNT2! So we rebid it at " & BTS(m_nBid) & ".\n";
				}
				else 
				{
					// else pass without enough strength
					m_nBid = BID_PASS;
					status << "BXPNT3! But without enough points to press the issue, we have to pass.\n";
				}
				return ValidateBid(m_nBid);
			}
			else
			{
				// we have 33+ points -- go for a slam
				InvokeBlackwood(nPrefSuit);
				return ValidateBid(m_nBid);
			}
		}

		//
		// here, see if we have a hand that's at least remotely balanced
		//
		if ((bSemiBalanced) || (numVoids == 0))
		{
			//
			// see if partner made an invitational NT raise
			//
			if ((nPreviousSuit == NOTRUMP) && (nPartnersSuit == NOTRUMP) &&
				((nPartnersBidLevel == 4) || (nPartnersBidLevel == 5)) )
			{
				status << "BXPNTI! Partner made an invitational NT Raise to " & szPB &
						  ", inviting us to" &
						  ((nPartnersBidLevel == 4)? " " : " a grand ") &
						  "slam with a maximum.\n";
				if (m_fMinTPCPoints >= PTS_SLAM)
				{
					// got enough points
					if (nPartnersBidLevel == 5)
					{
						// 5NT = go to 7NT
						status << "BXPNTI0! And with a total of " & 
							   m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
							   " total HCPs, try for the grand slam.\n";
						m_nBid = BID_7NT;
					}
					else
					{
						// 4NT = go to 6NT
						status << "BXPNTI1! And with a total of " & 
							   m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
							   " total HCPs, try for the slam.\n";
						m_nBid = BID_6NT;
					}
				}
				else
				{
					// not enough points
					if (nPartnersBidLevel == 5)
					{
						// 5NT = go to 6NT
						status << "BXPNTI5! But with a total of only " & 
							   m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
							   " total HCPs, we have to settle for the small slam.\n";
						m_nBid = BID_6NT;
					}
					else
					{
						// 4NT = pass
						status << "BXPNTI6! But with a total of only " & 
							   m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
							   " total HCPs, we have to pass on the slam.\n";
						m_nBid = BID_PASS;
					}
				}
				return ValidateBid(m_nBid);
			}

			// else partner made a simple NT bid or raise
			// accept partner's contract with < 33 pts and a balanced hand
			if ((bBalanced) && (m_fMinTPCPoints < PTS_SLAM))
			{
				status << "BXPNTK! With a balanced hand and " &
						   m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
						   " total HCPs, sign off on partner's " & szPB & " contract.\n";
				m_nBid = BID_PASS;
				return ValidateBid(m_nBid);
			}

			// see if we had preferred a suit 
			if ((!bBalanced) && (nPreviousSuit != NOTRUMP) && (nPartnersBidLevel < 3))
				status << "BXPNT4! Partner's " & szPB & " bid indicates a preference for NoTrump.\n";

			// respond at the 2-level with 23-25 HCPs
			if (BidNoTrump(2,PTS_GAME-3,PTS_GAME-1,TRUE,STOPPED_DONTCARE))
				return ValidateBid(m_nBid);

			// or at the 3-level with 26-32 HCPs
			// presumably partner sees that all suits are stopped
			if (BidNoTrump(3,PTS_GAME,PTS_SLAM-1,TRUE,STOPPED_DONTCARE))
				return ValidateBid(m_nBid);

			// or try a slam with more points
			if (BidNoTrump(6,PTS_SLAM,PTS_GRAND_SLAM,TRUE,STOPPED_DONTCARE))
				return ValidateBid(m_nBid);
			if (BidNoTrump(7,PTS_GRAND_SLAM,0,TRUE,STOPPED_DONTCARE))
				return ValidateBid(m_nBid);

			// alas, we lack the necessary strength to raise further
			m_nBid = BID_PASS;
			status << "BXPNT5! With a total of " & 
					  m_fMinTPCPoints & "-" & m_fMaxTPCPoints & " / " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " pts in the partnership, we lack adequate strength for a further raise beyond partner's " & 
					  szPB & " bid, so we have to pass.\n";
			return ValidateBid(m_nBid);
		}

		//
		// else we have a really unbalanced hand
		// rebid a good 6 or 7 card suit if we have one
		//
		if (numPrefSuitCards >= 6)
		{
			// rebid a strong 6-card suit at the 3-level with 23-25 pts
			if (RebidSuit(SUIT_PREFERRED,REBID_AT_3,PTS_GAME-3,PTS_GAME-1,LENGTH_6,SS_STRONG))
				return ValidateBid(m_nBid);
			// rebid a _solid_ 6-card major at the 4-level with 26-32 pts
			if (RebidSuit(SUIT_MAJOR,REBID_AT_4,PTS_GAME,PTS_SLAM-1,LENGTH_6,SS_STRONG|SS_SOLID))
				return ValidateBid(m_nBid);
			// rebid a _solid_ 7-card minor at the 4-level with 26-29 pts, 
			if (RebidSuit(SUIT_MINOR,REBID_AT_4,PTS_MINOR_GAME-3,PTS_MINOR_GAME,LENGTH_7,SS_STRONG|SS_SOLID))
				return ValidateBid(m_nBid);
			// or at game with 30-32 pts
			if (RebidSuit(SUIT_MINOR,REBID_AT_5,PTS_MINOR_GAME+1,PTS_SLAM-1,LENGTH_7,SS_STRONG|SS_SOLID))
				return ValidateBid(m_nBid);
			// or bid slam with a solid suit, if we have the points
			if (RebidSuit(SUIT_PREFERRED,REBID_AT_6,PTS_SLAM,PTS_GRAND_SLAM-1,LENGTH_6,SS_STRONG|SS_SOLID))
				return ValidateBid(m_nBid);
			if (RebidSuit(SUIT_PREFERRED,REBID_AT_7,PTS_GRAND_SLAM,0,LENGTH_6,SS_STRONG|SS_SOLID))
				return ValidateBid(m_nBid);
		}

		//
		// see if partner bid a suit in the previous round
		// and if so, raise it if we prefer it to notrump
		//
		if (nPartnersPrevSuit != NOTRUMP)
		{
			// raise to the 3-level with 23-25 pts & 3 trumps
			if (RaisePartnersSuit(SUIT_PREV,RAISE_TO_3,PTS_GAME-3,PTS_GAME-1,SUPLEN_3))
				return ValidateBid(m_nBid);
			// raise a major to game with 26-32 pts
			if (RaisePartnersSuit(SUIT_PREV_MAJOR,RAISE_TO_GAME,PTS_GAME,PTS_SLAM-1,SUPLEN_3))
				return ValidateBid(m_nBid);
			// raise a minor to the 4-level with 26-28 pts
			if (RaisePartnersSuit(SUIT_PREV_MINOR,RAISE_TO_4,PTS_MINOR_GAME-3,PTS_MINOR_GAME-1,SUPLEN_3))
				return ValidateBid(m_nBid);
			// or to game with 29-32 pts & 4 support cards
			if (RaisePartnersSuit(SUIT_PREV_MINOR,RAISE_TO_GAME,PTS_MINOR_GAME,PTS_SLAM-1,SUPLEN_4))
				return ValidateBid(m_nBid);
			// or bid a slam otherwise
			if (RaisePartnersSuit(SUIT_PREV,RAISE_TO_6,PTS_SLAM,PTS_GRAND_SLAM-1,SUPLEN_3))
				return ValidateBid(m_nBid);
			if (RaisePartnersSuit(SUIT_PREV,RAISE_TO_7,PTS_GRAND_SLAM,0,SUPLEN_3))
				return ValidateBid(m_nBid);
		}

		//
		// here, we have a strange hand that doesn't fit into any
		// of the above categories -- eg., XXXXX XXXXX XXX
		//

		// raise to 2NT with 24-26 HCPs, even with bad distribution
		if (BidNoTrump(2,PTS_NT_GAME-2,PTS_NT_GAME,FALSE,STOPPED_DONTCARE,NONE,FALSE))
			return ValidateBid(m_nBid);

		// likewise, raise to 3NT 27-32 HCPs
		// presumably partner sees that all suits are stopped
		if (BidNoTrump(3,PTS_NT_GAME+1,PTS_SLAM-1,FALSE,STOPPED_DONTCARE,NONE,FALSE))
			return ValidateBid(m_nBid);
		
		//
		// otherwise we have no more options, so pass
		//
		m_nBid = BID_PASS;
		status << "BXPNTZ! With a total of " &
				  m_fMinTPCPoints & "-" & m_fMaxTPCPoints & " / " &
				  m_fMinTPPoints & "-" & m_fMaxTPPoints & 
				  " pts in the partnership and no better options, we find it best to pass.\n";
		return ValidateBid(m_nBid);
	}






	//
	//==========================================================
	//
	// Rebidding in general, stages 5,6 or later
	//
	// If we've reached this point, we couldn't agree on a suit, and
	// have to settle on something
	//
	// To continue bidding at this point, we need:
	//   20+ total points to bid on to the 2 level, 
	//   23+ points if bidding on to the 3-level,
	//   26+ points to go on to the 4 level, or
	//   29+ points to go on to the 5 level, or
	//   33+ points to contemplate slam.
	//
	// Arbitrate according to the following rules
	//
	// with 33+ points:
	//  - invoke Blackwood, selecting as the trump suit,
	//    - our preferred 7+ card strong suit
	//    - partner's suit with 2+ card support
	//    - notrump with all suits stopped
	//  - if not playing Blackwood, jump to 6 or 7
	//
	// with < 33 total points:
	//  - go to game in majors with 26 pts if partner raised
	//  - go to game in minors with 29 pts if partner raised
	//  - go to game in majors with 27 pts and 6 strong cards
	//  - go to game in minors with 30 pts and 6 strong cards
	//  - rebid a strong 7-card suit
	//  - support partner's major with 2+ trumps
	// else with < 26 points, pass

	//
	if (nRound >= 2)
	{
		//
		// with > 33 points, drive towards a slam
		//
		if (m_fMinTPPoints >= PTS_SLAM)
		{
			// first figure out the proper suit
			if (nPartnersSuit == nPreviousSuit)
			{
				status << "2BXM05! Partner raised the " & szPSS & " suit to " & 
						 ((nPartnersBidLevel >= 6)? "slam at " :
						  (nPartnersBid >= GetGameBid(nPreviousSuit))? "game at " : " ") &
						 szPB & ".\n";
				nSuit = nPreviousSuit;
				m_nAgreedSuit = nSuit;
			}
			else if (nPartnersSuit == nNextPrevBid)
			{
				// partner bid a suit we bid 2 rounds ago
				nSuit = nNextPrevBid;
				m_nAgreedSuit = nSuit;
			}
			else
			{
				if ((numPrefSuitCards >= 7) && (nPrefSuitStrength >= SS_STRONG))
				{
					status << "2BXM10! With no strong agreement in suits but with " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " partnership points and a " & numPrefSuitCards & 
							  " card " & szPrefSS & " suit, we want a contract in our " & 
							  szPrefS & " suit.\n";
					nSuit = nPrefSuit;
				}
				else if ((numSupportCards >= 2) || (nPPrevSuitSupport >= 2))
				{
					nSuit = PickSuperiorSuit(nPartnersSuit, nPartnersPrevSuit);
					status << "2BXM11! We have no strong agreement in suits and don't have a very strong suit, but do have " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " points in the partnership, so we want a slam in partner's " & 
							  STSS(nSuit) & " suit.\n";
				}
				else
				{
					nSuit = PickSuperiorSuit(nPartnersSuit, nPartnersPrevSuit);
					status << "2BXM12! We have no strong agreement in suits and don't have a very strong suit or support for either of partner's suits, but have " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
							  " points in the partnership, so we want a slam in notrump.\n";
					nSuit = NOTRUMP;
				}
			}

			// withe 33+ points, bid on to slam with Blackwood
			if (nPartnersBidLevel < 5)
			{
				InvokeBlackwood(nSuit);
			}
			else if (nPartnersBidLevel < 6)
			{
				// too high to bid Blackwood? bid slam directly
				if (m_fMinTPPoints >= PTS_GRAND_SLAM)
					m_nBid = MAKEBID(nSuit,7);
				else
					m_nBid = MAKEBID(nSuit,6);
				status << "BXM18! We would like a slam, but since the bidding has gotten too high for Blackwood, bid slam directly at " &
					      BTS(m_nBid) & ".\n";
			}
			else if (nPartnersBidLevel == 6)
			{
				// partner bid a small slam
				if (m_fMinTPPoints <= PTS_GRAND_SLAM)
				{
					// correct the suit if necessary
					if (nSuit > nPartnersSuit)
					{
						m_nBid = MAKEBID(nSuit,6);
						status << "BXM20! Partner has bid a small slam in " & szPS &
								  ", but it's the wrong suit for us, so we correct it to the desired " & BTS(m_nBid) & ".\n";
					}
					else
					{
						m_nBid = BID_PASS;
						status << "BXM21! And partner has bid a small slam, and with " &
								  m_fMinTPPoints & "-" & m_fMaxTPPoints &
								  " points in the partnership, we're satisfied with the bid, so pass.\n";
					}
				}
				else
				{
					m_nBid = MAKEBID(nSuit, 7);
					status << "BXM24! Partner has bid a small slam, and with " &
							  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						      " points in the partnership, we boldly raise to " &
							  BTS(m_nBid) & ".\n";
				}
			}
			else
			{
				// partner bid a grand slam!
				// correct the suit if necessary
				if (nSuit > nPartnersSuit)
				{
					m_nBid = MAKEBID(nSuit,7);
					status << "BXM26! Partner has bid a grand slam in " & szPS &
							  ", but it's the wrong suit for us, so we correct it to the desired " & BTS(m_nBid) & ".\n";
				}
				else
				{
					m_nBid = BID_PASS;
					status << "BXM27! Partner has bid a grand slam, so pass and hope for the best.\n";
				}
			}
			// done
			return ValidateBid(m_nBid);
		}

		//
		// with < 33 points, bid according to the following rules:
		// 
		//  - if partner raised our suit, go to game in majors with 26 pts 
		//    or in minors with 29 pts
		//
		if ((nPartnersSuit == nPrefSuit) || (nPartnersSuit == nNextPrevSuit) ||
			(nPartnersSuit == nPreviousSuit) && (nPreviousBid != BID_2C))
		{
			if (nPartnersSuit == nPrefSuit)
				nSuit = nPrefSuit;
			else if (nPartnersSuit == nNextPrevSuit)
				nSuit = nNextPrevSuit;
			else
				nSuit = nPreviousSuit;
			if (ISMAJOR(nSuit) && (m_fMinTPPoints >= PTS_GAME) && (nPartnersBidLevel < 4))
			{
				m_nBid = MAKEBID(nSuit,4);
				status << "BXM30! Since partner raised our major " & szPrefSS & 
						  " suit, and we have a total of " &
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points in the partnership, we can go to game at " &
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
			else if (ISMINOR(nSuit) && (m_fMinTPPoints >= PTS_MINOR_GAME) && (nPartnersBidLevel < 5))
			{
				m_nBid = MAKEBID(nSuit,5);
				status << "BXM32! Since partner raised our major " & szPrefSS & 
						  " suit, and we have a total of " & 
						  m_fMinTPPoints & "-" & m_fMaxTPPoints &
						  " points in the partnership, we can go to game at " & 
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
		}

		//
		//  - go to game in majors with 27 pts and 6 strong cards
		//    (partner did not raise)
		//
		if (ISMAJOR(nPrefSuit) && (numPrefSuitCards >= 6) && (m_fMinTPPoints >= PTS_GAME+1) && 
								(nPartnersBid < MAKEBID(nPrefSuit,4)))
		{
			m_nBid = MAKEBID(nPrefSuit,4);
			status << "BXM36! Regradless of partner's actions, we have " & numPrefSuitCards & 
					  " strong " & szPrefS & " and " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " points in the partnership, so bid game at " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		//
		//  - go to game in minors with 30 pts and 6 strong cards
		//    (partner did not raise)
		//
		if (ISMINOR(nPrefSuit) && (numPrefSuitCards >= 6) && (m_fMinTPPoints >= PTS_MINOR_GAME+1) && 
								(nPartnersBid < MAKEBID(nPrefSuit,5)))
		{
			m_nBid = MAKEBID(nPrefSuit,4);
			status << "BXM38! Regradless of partner's actions, we have " & numPrefSuitCards & 
					  "strong " & szPrefS & " and " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " points in the partnership, so bid game at " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		
		//
		// - rebid a 7-card suit with 22+ points at the 3-level
		//
		nBid = GetCheapestShiftBid(nPrefSuit);
		nLevel = BID_LEVEL(nBid);
		if ((numPrefSuitCards >= 7) && (m_fMinTPPoints >= PTS_GAME-4) && (nLevel <= 3))
		{
			m_nBid = nBid;
			status << "BXM40! With a " & numPrefSuitCards & "-card " & szPrefSS & 
					  " suit, insist on rebidding it at " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		//
		// - rebid an 8-card suit with 24+ points at the 4-level
		//
		if ((numPrefSuitCards >= 8) && (m_fMinTPPoints >= PTS_GAME-2) && (nLevel <= 4))
		{
			m_nBid = nBid;
			status << "BXM44! With a " & numPrefSuitCards & "-card " & szPrefSS &
					  " suit, insist on rebidding it at " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		//
		// - support partner's suit (or previous suit) with 2+ trumps and
		// 22+ points at the 3-level
		//
		if (RaisePartnersSuit(SUIT_ANY,RAISE_TO_3,PTS_GAME-4,99,SUPLEN_2))
		{
			status << "BXM48! With " & numSupportCards & "-card support for partner's " & 
					  szPSS & " suit, raise it to the 3-level at " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}
		if (RaisePartnersSuit(SUIT_PREV,RAISE_TO_3,PTS_GAME-4,99,SUPLEN_2))
		{
			status << "BXM52! With " & numSupportCards & 
					  "-card support for partner's previous suit (" & szPPS & 
					  "), raise it to the 3-level at " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		//
		// - support partner's suit with 2+ trumps with 26+ points at the 4-level
		//
		if (RaisePartnersSuit(SUIT_ANY,RAISE_TO_4,PTS_GAME,99,SUPLEN_2))
		{
			status << "BXM60! With " & numSupportCards & 
					  "-card support for partner's " & szPSS & 
					  " suit, raise it to the 4-level at " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}
		if (RaisePartnersSuit(SUIT_PREV,RAISE_TO_4,PTS_GAME,99,SUPLEN_2))
		{
			status << "BXM64! With " & numSupportCards & 
					  "-card support for partner's previous suit (" & szPPS & 
					  "), raise it to the 4-level at " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		//
		// if we have no other options, pass unless we're exceedingly weak
		// in partner's suit
		//
		if (numSupportCards == 0)
		{
			// bid 3NT if possible
			if ((nPartnersBidLevel <= 3) && (nPartnersSuit != NOTRUMP) &&
					((m_fMinTPCPoints >= PTS_GAME) || (m_fMinTPPoints >= PTS_GAME+1)) )
			{
				m_nBid = BID_3NT;
				status << "BXM68! With zero trump support for partner's " & szPSS & 
						  " suit, bail out at " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}
		}


		// else if partner switched suits, try to bid 3Nt
		if ((nRound >= 2) &&  (nPartnersSuit != nPartnersPrevSuit) && 
			(nPartnersSuit != nPreviousSuit) && (nPartnersSuit != nNextPrevSuit) &&
			(nPartnersBid < BID_3NT))
		{
			m_nBid = BID_3NT;
			status << "BXM70! Partner switched suits again, which forces us to bid again -- so with no other good options, bid " & 
					  BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}


		//
		// else pass
		//
		m_nBid = BID_PASS;
		if ((nPartnersSuit != nPreviousSuit) && (nPartnersSuit != nNextPrevSuit))
			status << "BXM80! With no agreement in suits, " &
					  m_fMinTPCPoints & "-" & m_fMaxTPCPoints & " / " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " partnership points, and no good alternatives, pass.\n";
		else
			status << "BXM81! We've reached agreement in suits, but with only " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints &
					  " total points we can't raise partner's " & szPB & 
					  " bid any further, so pass.\n";
		return ValidateBid(m_nBid);
	}















	//
	//==========================================================
	//
	// Rebidding in 4th or later round
	//
	// Normally, bidding should not go this far, at least without
	// interference.
	//
	// Arbitrate according to the following rules
	//
	// ---------------
	//

	if (nRound >= 2)
	{
		m_nBid = BID_PASS;
		status << "BXX00! In 3rd round with an agreed suit?  We shouldn't be here!\n";
		return ValidateBid(m_nBid);
	}








	//
	//===============================================================
	//
	// Here, reached an unknown state
	//
	// partner made an unusual response

	m_nBid = BID_PASS;
	status << "BXZ40! Partner made a response which we don't understand; simply pass.\n";
	return ValidateBid(m_nBid);



/*
	status << "BX<<Error>> -- fell out of rebid decision loop.  Initial bid was " & 
			  BTS(nLastBid) & ", and partner's bid was " & szPB & 
			  ".  Points are " & fCardPts & "/" & fPts & "/" & fAdjPts & 
			  ", preferred suits is " & szPrefSS & " (holding " & szHPref & 
			  ").  Returning PASS.\n";
*/
	return ValidateBid(m_nBid);

}







