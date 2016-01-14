//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Bidresp.cpp
//
// Responding bidding routines
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
// MakeRespondingBid()
//
// respond to partner's opening-round bid
//
int CBidEngine::MakeRespondingBid()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;

	// print opening line
	if (nPartnersBid != BID_DOUBLE)
		status << "Responding to partner's" & 
				  ((m_numPartnerBidTurns == 1)? " opening" : " ") &
				  " bid of " & BTS(nPartnersBid) & ".\n";
	else
		status << "Responding to partner's" & 
				  ((m_numPartnerBidTurns == 1)? " opening" : " ") & " double.\n";

	// calculate adjusted dist points
	if ((nPartnersSuit != NOTRUMP) && (nPartnersBid != BID_2C) && (nPartnersBid < BID_DOUBLE))
		fAdjPts = m_pHand->RevalueHand(REVALUE_DUMMY, nPartnersSuit, TRUE);
	else
		fAdjPts = fPts;


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
	// Responding to a 1NT, 2NT, or 3NT bid by partner
	// in general,
	// <=23 tot HCPs:  pass a balanced hand, bid with a long suit
	// 24-30 tot HCPs:  move towards game
	// 31+ tot HCPs:  invite slam
	// exceptions:  Stayman, Jacoby
	//
	if ((nPartnersBid == BID_1NT) || (nPartnersBid == BID_2NT) ||
		(nPartnersBid == BID_3NT)) 
	{

		// since the point count required to respond can vary by 
		// NT bid ranges, calculate total min. estimated points
		int n1NTMIN = pCurrConvSet->GetValue(tn1NTRangeMinPts);
		int n1NTMAX = pCurrConvSet->GetValue(tn1NTRangeMaxPts);
		int n2NTMIN = pCurrConvSet->GetValue(tn2NTRangeMinPts);
		int n2NTMAX = pCurrConvSet->GetValue(tn2NTRangeMaxPts);
		int n3NTMIN = pCurrConvSet->GetValue(tn3NTRangeMinPts);
		int n3NTMAX = pCurrConvSet->GetValue(tn3NTRangeMaxPts);
		int	nMin,nMax;
		//
		if (nPartnersBid == BID_1NT) 
		{
			nMin = n1NTMIN;
			nMax = n1NTMAX;
			status << "2A10! Partner's bid indicates " & nMin & "-" & nMax &
					  " points with balanced hand.\n";
		} 
		else if (nPartnersBid == BID_2NT) 
		{
			nMin = n2NTMIN;
			nMax = n2NTMAX;
			status << "2A20! Partner's bid indicates " & nMin & "-" & nMax &
					  " points with balanced hand.\n";
		} 
		else if (nPartnersBid == BID_3NT) 
		{
			nMin = n3NTMIN;
			nMax = n3NTMAX;
			if (nMin > 0)
				status << "2A30! Partner's bid indicates " & nMin & "-" & nMax &
					      " points with balanced hand.\n";
			else
				status << "2A31! Partner's bid is suspect given that the 3NT opening was precluded; assuming " &
					       nMin & "-" & nMax & " pts.\n";
		}
		m_fMinTPPoints = nMin + fPts;
		m_fMaxTPPoints = nMax + fPts;
		m_fMinTPCPoints = nMin + fCardPts;
		m_fMaxTPCPoints = nMax + fCardPts;

		// see if we have a reasonably balanced hand
//		if (bBalanced) 
		if (bSemiBalanced) 
		{
			//
			status << "2B00! Responding with a" &
					  (bBalanced? " " : " reasonably") &
					  " balanced hand and " & fCardPts &
				      " HCP's, for a total in partnership of " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
					  " HCPs.\n";
			// 0-7 HCPs (up to 23 pts min. based on 16-18 
			// 1NT count):  pass
			// this means we'd pass w/ 8 pts when playing 15-17 1NT
			if (m_fMinTPCPoints <= PTS_GAME-2) 
			{
				m_nBid = BID_PASS;
				status << "B10! We do not have quite enough for game even if partner has max opening points, so pass.\n";
			// 24+ tot. HCPs:  raise to 2NT
			} 
			else if (m_fMinTPCPoints == PTS_GAME-1) 
			{
				// see if we're playing 4-suit Jacoby -- then
				// we have to bid 2C
				if (!pCurrConvSet->IsOptionEnabled(tb4SuitTransfers))
				{
					m_nBid = BID_2C;
					status << "B20! If partner has a max opening hand (" & nMax &
							  " HCPs), we can make game at 3NT, and so we want to bid an invitational 2NT.  However, since we're playing 4-suit Jacoby Transfers, we have to bid 2C instead.\n";
				} 
				else 
				{
					m_nBid = BID_2NT;
					status << "B22! If partner has a max opening hand (" & nMax & 
							  " HCPs), we can make game, so make an invitational " & BTS(m_nBid) &
							  " bid.\n";
				}
			} 
			else if ((m_fMinTPCPoints >= PTS_NT_GAME) && (m_fMinTPCPoints <= PTS_SLAM-3)) 
			{
				// 26-30 tot HCPs:  raise to 3NT
				m_nBid = BID_3NT;
				status << "B30! With " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
						  " combined HC points, game is reachable.  Bid " & BTS(m_nBid) & ".\n";
			} 
			else if ((m_fMinTPCPoints >= PTS_SLAM-2) && (m_fMinTPCPoints <= PTS_SLAM-1)) 
			{
				// 31-32 tot HCPs:  raise to 4NT
				m_nBid = BID_4NT;
				status << "B40! With " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
						  " combined HC points, entertain slam possibilities with an invitational " &
						  BTS(m_nBid) & " bid.\n";
			} 
			else if ((m_fMinTPCPoints >= PTS_SLAM) && (m_fMinTPCPoints <= PTS_SLAM+1)) 
			{
				// 33-34 tot HCPs:  raise to 6NT
				m_nBid = BID_6NT;
				status << "B50! With " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
						  " combined HC points, a small slam should be reachable.  Bid " &
						  BTS(m_nBid) & ".\n";
			} 
			else if ((m_fMinTPCPoints >= PTS_SLAM+2) && (m_fMinTPCPoints <= PTS_SLAM+3)) 
			{
				// 35-36 tot HCPs:  raise to 5NT; inviting to 
				// grand slam if opener has more than min. pts
				m_nBid = BID_5NT;
				status << "B60! With " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
						  " combined HC points, a grand slam may be possible if partner has a maximum opening hand.  So make an invitational bid of " &
						  BTS(m_nBid) & ".\n";
			} 
			else 
			{
				// 37+ tot HCPs:  raise to 7NT
				m_nBid = BID_7NT;
				status << "B70! With " & m_fMinTPCPoints & "-" & m_fMaxTPCPoints &
					   	  " combined HC points, we have enough for a grand slam.  So bid " &
						  BTS(m_nBid) & ".\n";
			}
			//
			return ValidateBid(m_nBid);
		}


		//
		//======================================================
		//	
		// here, we don't have a balanced hand
		//


		//
		//-----------------------------------------------------
		//
		// with < 23 min team points, pass
		//
		if (m_fMinTPPoints < PTS_GAME-3) 
		{
			m_nBid = BID_PASS;
			status << "C00! With only " & fCardPts & "/" & fPts &
					  " pts in hand, for a total in the partnership of " &
					  m_fMinTPPoints & "-" & m_fMaxTPPoints & " pts, pass.\n";
			return ValidateBid(m_nBid);
		}

			
		//
		//-----------------------------------------------------
		//
		// 23-25 team points (7-8 in hand playing 16-18 1NT):  
		// bid with a long suit, pass otherwise
		//
		if (m_fMinTPPoints < PTS_GAME) 
		{
			//
			// with 25 pts and a 6-card major suit, jump to 4
			//
			if ((numCardsInSuit[HEARTS] >= 6) || 
						(numCardsInSuit[SPADES] >= 6) &&
											(fPts >= PTS_GAME-1) )
			{
				nSuit = GetBestSuitofAtLeast(HEARTS,SPADES,6);
				m_nBid = MAKEBID(nSuit,4);
				status << "C02! Have " & fCardPts & "/" & fPts &
						  " points with a " & LEN(nSuit) & "-card " &
						  STSS(nSuit) & " suit, so jump to " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			//
			// with 24+ HCPs and a 6-card minor, jump to 3NT
			//
			if ((m_fMinTPCPoints >= PTS_GAME-2) &&
				((numCardsInSuit[CLUBS] >= 6) || 
						(numCardsInSuit[DIAMONDS] >= 6)) ) 
			{
				nSuit = GetBestSuitofAtLeast(CLUBS,DIAMONDS,6);
				m_nBid = BID_3NT;
				status << "C04! Have " & fCardPts & "/" & fPts &
						  " points with a " & LEN(nSuit) & "-card " &
						  STSS(nSuit) & " (minor) suit, so jump to " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			//
			// here, we have no 6-card suits
			//
			// with at least one 4-card major suit,
			// bid 2C (if playing Stayman), 
			// else bid the major directly
			//
			if ((numCardsInSuit[HEARTS] >= 4) || (numCardsInSuit[SPADES] >= 4)) 
			{
				// Stayman is either not applicable or not being played, 
				// so bid the major directly
				if (ISMAJOR(nPrefSuit)) 
				{
					nSuit = nPrefSuit;					
					m_nBid = MAKEBID(nSuit,2);
					status << "C12! Have " & fCardPts & "/" & fPts &
							  " points with a " & SSTS(nSuit) & " " & STSS(nSuit) &
							  " suit (holding " & SHTS(nSuit) & "), so bid " & 
							  BTS(m_nBid) & 
							  (pCurrConvSet->IsConventionEnabled(tidStayman)? "." : " (not playing Stayman).") & "\n";
				}
				// no preferred major; look for any 4-card major
				if ((numCardsInSuit[HEARTS] >= 4) || (numCardsInSuit[SPADES] >= 4)) 
				{
					nSuit = GetBestSuitofAtLeast(HEARTS,SPADES,4,SP_FIRST);
					status << "C16! Have " & fCardPts & "/" & fPts &
							  " points with a " & LEN(nSuit) & "-card " &
							  STSS(nSuit) & " suit (holding " & SHTS(nSuit) & 
							  "), so bid " & BTS(m_nBid) & 
							  (pCurrConvSet->IsConventionEnabled(tidStayman)? "." : " (not playing Stayman).") & "\n";
				}
				// and bid
				return ValidateBid(m_nBid);
			}

			//
			// else bid 2NT
			//
			m_nBid = BID_2NT;
			status << "C20! Have " & fCardPts & " HCPs and no decent major suit, so bid " &
					  BTS(m_nBid) & "; partner should raise to 3NT with max opening points.\n";
			return ValidateBid(m_nBid);

		}


		//
		//--------------------------------------------------------
		//
		// 26-31 team points:  
		//   with a 6-card major:  jump to game
		//   with a 6-card minor:  jump to 3NT
		//   with a 5-card suit:  bid 3 in the suit (game force)
		//   with at least 1 major:  bid 2C (Stayman)
		//   with at least 1 5-card suit:  bid 3 of the suit
		//   with no length anywhere:  jump to 3NT
		//   with 14+ points, bid 3 first, then try for slam later
		//
		if ((m_fMinTPPoints >= PTS_GAME) && (m_fMinTPPoints < PTS_SLAM))
		{

			// with a 6-card major suit, jump to 4 (game)
			if ((numCardsInSuit[HEARTS] >= 6) || (numCardsInSuit[SPADES] >= 6)) 
			{
				nSuit = GetBestSuitofAtLeast(HEARTS,SPADES,6);
				m_nBid = MAKEBID(nSuit,4);
				status << "E00! Have " & fCardPts & "/" & fPts &
					      " points with a " & LEN(nSuit) & "-card " &
						  STSS(nSuit) & " suit, so jump to game at " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// with 25 HCPs and a 6-card minor, jump to 3NT
			if ( (m_fMinTPCPoints >= PTS_GAME-1) &&
				 ((numCardsInSuit[CLUBS] >= 6) || (numCardsInSuit[DIAMONDS] >= 6)) ) 
			{
				nSuit = GetBestSuitofAtLeast(CLUBS,DIAMONDS,6);
				m_nBid = BID_3NT;
				status << "E04! Have " & fCardPts & "/" & fPts &
						  " points with a " & LEN(nSuit) & "-card " &
						  STSS(nSuit) & " (minor) suit, so " &
						  ((nPartnersBid == BID_1NT)? "jump to" :  "bid") &
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// with a 5-card major suit, bid at the 3-level
			if ((ISMAJOR(nPrefSuit)) && (numPrefSuitCards >= 5)) 
			{
				nSuit = nPrefSuit;
				m_nBid = MAKEBID(nPrefSuit,3);
				status << "E08! Have " & fCardPts & "/" & fPts &
						  " points with a " & LEN(nSuit) & "-card major " &
						  STSS(nSuit) & " suit, so " &
						  ((nPartnersBid == BID_1NT)? "jump to " :  "bid ") &
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// also check for a 5-card minor
			if ((ISMINOR(nPrefSuit)) && (numPrefSuitCards >= 5)) 
			{
				nSuit = nPrefSuit;
				m_nBid = MAKEBID(nPrefSuit,3);
				status << "E12! Have " & fCardPts & "/" & fPts &
						  " points with a " & LEN(nSuit) & "-card minor " &
						  STSS(nSuit) & " suit, so " & 
						  ((nPartnersBid == BID_1NT)? "jump to" :  "bid") &
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// here, we have no 6-card or good 5-card suits
			// bid any 4-card major at the 2-level
			if ((numCardsInSuit[HEARTS] >= 4) || (numCardsInSuit[SPADES] >= 4)) 
			{
				// no Stayman, so bid the major directly
				// bid the preferred suit if it's a major
				if (ISMAJOR(nPrefSuit)) 
				{
					nSuit = nPrefSuit;					
					m_nBid = MAKEBID(nSuit,2);
					status << "E16! Have " & fCardPts & "/" & fPts &
						      " points with a " & SSTS(nSuit) & " " & STSS(nSuit) & 
							  " suit (holding " & SHTS(nSuit) & "), so bid " & 
							  BTS(m_nBid) & 
							  (pCurrConvSet->IsConventionEnabled(tidStayman)? "." : " (not playing Stayman).") & "\n";
				}
				// no preferred major; look for any 4-card major
				if ((numCardsInSuit[HEARTS] >= 4) || (numCardsInSuit[SPADES] >= 4)) 
				{
					nSuit = GetBestSuitofAtLeast(HEARTS,SPADES,4,SP_FIRST);
					m_nBid = MAKEBID(nSuit,2);
					status << "E20! Have " & fCardPts & "/" & fPts &
							  " points with a " & LEN(nSuit) & "-card " &
							  STSS(nSuit) & " suit (holding " & SHTS(nSuit) & 
							  "), so bid " & BTS(m_nBid) & 
							  (pCurrConvSet->IsConventionEnabled(tidStayman)? "." : " (not playing Stayman).") & "\n";
				}
				// and bid
				return ValidateBid(m_nBid);
			}

			// otherwise, with no length anywhere, default to 3NT
			m_nBid = BID_3NT;
			status << "E24! Have " & fCardPts & "/" & fPts &
					  " points with no long suit, so respond with " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);
		}

		//
		//--------------------------------------------------------
		//
		// 33+ team pts points:  
		//   this is slam territory --
		//   bid 3 in a suit, followed by a slam try later
		//   the 3-bid is invitational to slam if playing Jacoby,
		//   or just invitational otherwise

		//
		// bid 3 directly with a solid suit
		if ((bPrefSuitIsSolid) && (numPrefSuitCards >= 6)) 
		{
			nSuit = nPrefSuit;
			m_nBid = GetJumpShiftBid(nSuit,nPartnersBid);
			status << "E30! With " & fCardPts & "/" & fPts &
					  " points and a solid " & STSS(nSuit) & " suit (holding " &
					  SHTS(nSuit) & "), jump shift to " & BTS(m_nBid) & 
					  " to show slam prospects.\n";
			return ValidateBid(m_nBid);
		}

		// else with no Stayman, just bid the best suit at the 3 level
		nSuit = nPrefSuit;
		m_nBid = GetJumpShiftBid(nSuit,nPartnersBid);
		status << "E40! With " & fCardPts & "/" & fPts &
				  " points (though without a solid suit), jump shift to " & 
				  BTS(m_nBid) & " to show slam prospects.\n";
		return ValidateBid(m_nBid);

	}



	//
	//==========================================================
	//
	// Responding to an opening 1-bid by partner 
	//
	// Classify hands according to strength
	// <6 pts:  useless
	//  6-10 pts:  weak
	// 11-12 pts:  fair
	// 13-18 pts:  strong
	// 19+ pts:  very strong
	// 

	if (nPartnersBidLevel == 1) 
	{

		//
		m_fPartnersMin = theApp.GetMinimumOpeningValue(m_pPartner);
		m_fPartnersMax = OPEN_PTS(pCurrConvSet->GetValue(tn2ClubOpeningPoints)) - 1;
		//
		if ((m_pPartner->GetOpeningPosition() == 0) ||		
				(m_pPartner->GetOpeningPosition() == 1)) 
		{
			status << "2F00! Partner is showing " & m_fPartnersMin & "+ points and 2+ QT's.\n";
		} 
		else 
		{
			status << "2F01! Partner, opening in " & 
					  ((m_pPartner->GetOpeningPosition() == 2)? "third" :  "fourth") &
					  " position, is showing " & m_fPartnersMin & "+ points and 2+ QT's.\n";
		}

		//
		m_fMinTPPoints = fAdjPts + m_fPartnersMin;
		m_fMaxTPPoints = fAdjPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;


		//
		//--------------------------------------------------------
		//
		// Responding to a 1-bid with < 6 pts -- gotta pass
		//
		// also pass if we have < 6 unadjusted pts with nothing but 
		// support for partner's minor
		//
		if (((nPartnersSuitSupport < SS_WEAK_SUPPORT) && (fPts < OPEN_PTS(6))) ||
			((nPartnersSuitSupport >= SS_WEAK_SUPPORT) && (fAdjPts < OPEN_PTS(6))) ||
			(ISMINOR(nPartnersSuit) && (fPts < OPEN_PTS(6))) ) 
		{
			// pass
			m_nBid = BID_PASS;
			// check if the extra support points are wasted
			if ((fAdjPts > OPEN_PTS(5)) && (nPartnersSuitSupport < SS_WEAK_SUPPORT))
				status << "F02! But we have only " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points with weak trump support, so we have to pass.\n";
			else if (ISMAJOR(nPartnersSuit))
				status << "F03! But we have only " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points, so we have to pass.\n";
			else
				status << "F04! But we have only " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points against a minor suit bid, so we prefer to pass.\n";
			return ValidateBid(m_nBid);
		}


		//
		//--------------------------------------------------------
		//
		// Responding to a 1-bid, with 6-10.5 pts in hand
		// in general,
		// 0) check for a possible triple raise
		// 0.5) bid 1 of a higher major if it's really good
		// 1) raise a major suit to the 2-level if possible
		// 2) bid 1 of a higher suit if holding a good suit
		// 3) bid a major over a weak minor
		// 4) bid a lower suit 2/1 with 10 points
		// 5) raise a minor suit if we must
		// 6) bid 1NT as a last resort
		//
		// in general, we wish to avoid raising a minor, so the rqmts
		// for responding are stricter if we're responding to a minor
		//
		if ( ((nPartnersSuitSupport < SS_WEAK_SUPPORT) && 
							(fPts >= OPEN_PTS(6)) && (fPts < OPEN_PTS(11))) ||
			 (ISMAJOR(nPartnersSuit) && 
							(nPartnersSuitSupport >= SS_WEAK_SUPPORT) && 
				 			(fAdjPts >= OPEN_PTS(6)) && (fAdjPts < OPEN_PTS(11))) || 
			 (ISMINOR(nPartnersSuit) && 
							(nPartnersSuitSupport >= SS_WEAK_SUPPORT) && 
				 			(fPts >= OPEN_PTS(6)) && (fAdjPts < OPEN_PTS(11))) ) 
		{

			// look for a triple raise in a major suit
			// need 5+ trumps, 6-10 HCPs, 
			// and a void or a singleton
			if ((ISMAJOR(nPartnersSuit)) && 
				(fCardPts >= OPEN_PTS(6)) && (fCardPts < OPEN_PTS(10)) &&
				(numSupportCards >= 5) &&
				((numVoids > 0) || (numSingletons > 0)) ) 
			{
				m_nAgreedSuit = nPartnersSuit;
				m_nBid = MAKEBID(nPartnersSuit,4);
				status << "G00! Have " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points with " & numSupportCards & " trumps and ";
				if (numVoids == 1)
					status < "a void suit";
				else if (numVoids > 1)
					status < numVoids & " void suits";
				else if (numSingletons == 1)
					status < "a singleton";
				else
					status & numSingletons & " singleton suits";
				status < ", so make a triple raise of " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			//
			// if partner opened a major, raise if possible
			// or shift to a higher major
			if (ISMAJOR(nPartnersSuit)) 
			{
				// if our (hearts) trump support is weak AND we have a really good
				// higher major (spades), bid 1 spade
				if ((nPartnersSuitSupport <= SS_WEAK_SUPPORT)  &&
					(nPartnersSuit == HEARTS) && (nSuitStrength[SPADES] >= SS_OPENABLE))
				{
					// bid the preferred Spade suit
					m_nBid = MAKEBID(SPADES,1);
					status << "G3! Have " & fCardPts & "/" & fPts & "/" & fAdjPts &
							  " points with " & SLTS(nPartnersSuit) & 
							  " (" & szHP & ") support for partner's " & szPSS & 
							  " suit but hold a " & SSTS(SPADES) & " " & numCardsInSuit[SPADES] & 
							  "-card Spade suit, so bid " & BTS(m_nBid) & " instead.\n";
					return ValidateBid(m_nBid);
				}

				// else check if the trump support is at least decent
				if (nPartnersSuitSupport >= SS_WEAK_SUPPORT) 
				{
					// weak 4-card or good 3-card support
					// playing 4-card majors, or 3+ cards playing
					// 5-card majors
					m_nAgreedSuit = nPartnersSuit;
					m_nBid = MAKEBID(nPartnersSuit,2);
					status << "G08! Have " & fCardPts & "/" & fPts & "/" & fAdjPts &
							  " points and " & numSupportCards & "-card (" & 
							  szHP & ") " & szPSS & " trump support" &
							  ((pCurrConvSet->IsConventionEnabled(tid5CardMajors) && 
							    (numSupportCards>=3))? " playing 5-card majors," :  ",") &
							  " so raise to " & BTS(m_nBid) & ".\n";
					return ValidateBid(m_nBid);
				}

				// at this point, our trump support is really weak;
				// (weak 3 cards at 4-card majors or 2 cards at 5-card majors); 
				// bid 1 of a higher major (Spades over Hearts) if we have 
				// a good spade suit (5 cards or 4 good cards)
				if ((nPartnersSuit == HEARTS) && 
						((nPrefSuit == SPADES) || (numCardsInSuit[SPADES] >= 5)) )
				{
					// bid the preferred Spade suit
					m_nBid = MAKEBID(SPADES,1);
					if (numCardsInSuit[SPADES] >= 5)
						status << "G12! Have " & fCardPts & "/" & fPts & "/" & fAdjPts &
								  " points with " & SLTS(nPartnersSuit) & 
								  " (" & szHP & ") support for partner's " & szPSS & 
								  " suit but hold a " & numCardsInSuit[SPADES] &
								  "-card Spade suit, so bid " & BTS(m_nBid) & " instead.\n";
					else
						status << "G13! Have " & fCardPts & "/" & fPts & "/" & fAdjPts &
								  " points with " & SLTS(nPartnersSuit) & 
								  " (" & szHP & ") support for partner's " & szPSS & 
								  " suit but hold a " & SSTS(SPADES) & " Spade suit, so bid " &
								  BTS(m_nBid) & " instead.\n";
					return ValidateBid(m_nBid);
				}
			}
			
			// if partner's bid is a minor, bid any decent major
			// unless there is no biddable major suit
			if (ISMINOR(nPartnersSuit)) 
			{
				// bid a 4-card major if we have one we like
				if ((ISMAJOR(nPrefSuit)) && (numPrefSuitCards >= 4)) 
				{
					m_nBid = MAKEBID(nPrefSuit,1);
					status << "G20! Have " & fCardPts & "/" & fPts & "/" & fAdjPts &
							  " pts with an available " & numPrefSuitCards & 
							  "-card major suit in " & szPrefS & " (" & szHPref & 
							  "), so bid " & BTS(m_nBid) & 
							  " over partner's minor suit " & szPB & " bid.\n";
					return ValidateBid(m_nBid);
				}
				// else search for the cheapest 4+ card major
				if ((numCardsInSuit[HEARTS] >= 4) || (numCardsInSuit[SPADES] >= 4)) 
				{
					nSuit = GetBestSuitofAtLeast(HEARTS,SPADES,4,SP_FIRST);
					// but the suit had better not be junk
					if (numSuitPoints[nSuit] >= 2) {
						m_nBid = MAKEBID(nSuit,1);
						status << "G28! Have " & fCardPts & "/" & fPts & "/" & fAdjPts &
								  " pts with an available " & LEN(nSuit) & 
								  "-card major " & STSS(nSuit) & " suit, so bid " &
								  BTS(m_nBid) & " over partner's minor suit " &
								  szPB & " bid.\n";
						return ValidateBid(m_nBid);
					}
				}
			}

			// bid 2/1 with 10 HCPs and a good suit
			if ((nPrefSuit < nPartnersSuit) && (fCardPts >= OPEN_PTS(10)) &&
					(nPrefSuitStrength >= SS_OPENABLE)) 
			{
				// move to the 2 level with the lower suit
				m_nBid = MAKEBID(nPrefSuit,2);
				status << "G32! Have " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points with " & SLTS(nPartnersSuit) & " support for partner's " & szPSS & 
						  " suit (holding " & szHP & "; bid the preferred " & szPrefSS &
						  " suit 2/1 at " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// bid 2/1 with 9 pts and a really good suit
			if ((nPrefSuit < nPartnersSuit) && (fCardPts >= OPEN_PTS(9)) &&
					 (nPrefSuitStrength >= SS_STRONG) && (bPrefSuitIsSolid)) 
			{
				// move to the 2 level with the lower suit
				m_nBid = MAKEBID(nPrefSuit,2);
				status << "G36! Have " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points with " & SLTS(nPartnersSuit) & " support for partner's " & szPSS & 
						  " suit (holding " & szHP & "); bid the preferred " & szPrefSS & 
						  " suit 2/1 at " & BTS(m_nBid) & " (shade the 10 pt 2/1 rqmt because of the good suit).\n";
				return ValidateBid(m_nBid);
			}

			// if we have nothing but support for the partner's
			// minor, see if we have a semi-balanced distribution
			// with strength in 3 or more suits
			if ((ISMINOR(nPartnersSuit)) && (numVoids == 0) &&
				(numSingletons == 0) && (numSuitsProbStopped >= 3) &&
				(fPts >= 6)) 
			{
				// bid 1NT
				m_nBid = BID_1NT;
				status << "G40! Have " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  "% points with no good majors, no other good suit of our own, and a semi-balanced distribution with strength in " &
						  ((numSuitsProbStopped == 4)? "all four" :  "three") &
						  " suits, bid " & BTS(m_nBid) & " in preference to partner's minor.\n";
				return ValidateBid(m_nBid);
			}

			// bid a higher minor if possible
			// i.e., Diamonds over clubs
			if ((nPartnersSuit == CLUBS) && 
				(nPrefSuit == DIAMONDS) && (fPts >= OPEN_PTS(6)) &&
				(nPrefSuitStrength >= SS_OPENABLE)) 
			{
				m_nBid = BID_1D;
				status << "G44! Have " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  " pts with no 4-card major suits, so bid " & BTS(m_nBid) &
						  " in preference to raising partner's minor " & szPS & " suit.\n";
				return ValidateBid(m_nBid);
			}

			// if the above tests fail & we have strong support
			// for partner's minor suit, raise the minor
			// (we prefer not to do this)
			if ((ISMINOR(nPartnersSuit)) &&
					(nPartnersSuitSupport >= SS_GOOD_SUPPORT)) 
			{
				// raise the minor suit
				m_nAgreedSuit = nPartnersSuit;
				m_nBid = MAKEBID(nPartnersSuit,2);
				status << "G48! Have " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points with " & SLTS(nPartnersSuit) & 
						  " support for partner's " & szPSS &
						  " suit (holding " & szHP & ") and no good major suits, so raise to " &
						  BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// finally, bid 1NT as a last resort
			if (ISMINOR(nPartnersSuit)) 
			{
				if (nPrefSuit < nPartnersSuit)
					status << "G60! We have " & fCardPts & "/" & fPts & "/" & fAdjPts &
							  " points with " & SLTS(nPartnersSuit) & 
							  " support for partner's minor " & szPSS &
							  " suit (holding " & szHP & ") which we prefer not to raise, and not enough strength for a 2/1 response to show our " &
							  szPrefSS & " suit, so bid 1NT.\n";
				else
					status << "G62! We have " & fCardPts & "/" & fPts & "/" & fAdjPts &
							  " points with " & SLTS(nPartnersSuit) & 
							  " support for partner's minor " & szPSS &
							  " suit (holding " & szHP & ") which we prefer not to raise, and we have no good major suit of our own, so bid 1NT.\n";
			} 
			else 
			{
				if (nPrefSuit < nPartnersSuit)
					status << "G64! We have " & fCardPts & "/" & fPts & "/" & fAdjPts &
							  " points with " & SLTS(nPartnersSuit) & 
							  " support for partner's " & szPSS & 
							  " suit (holding " & szHP & ") and not enough strength for a 2/1 response to show our " & 
							  szPrefSS & " suit, so bid 1NT.\n";
				else
					status << "G68! We have " & fCardPts & "/" & fPts & "/" & fAdjPts &
							  " points with " & SLTS(nPartnersSuit) & 
							  " support for partner's " & szPSS & 
							  " suit (holding " & szHP & ") and no good major of our own, so bid 1NT.\n";
			}
			m_nBid = BID_1NT;
			return ValidateBid(m_nBid);
		}


		//
		//--------------------------------------------------------
		//
		// Responding to a 1-bid, with 11-12.5 pts in hand
		// (11-12 orig pts w/o support or 11-12.5 revalued pts w/ support)
		// in general, make two responses:  first in own suit, then
		// (if applicable) in partner's suit or NT
		// but if playing limit raises, can raise w/ 4 trumps
		//
		if ( ((nPartnersSuitSupport < SS_WEAK_SUPPORT) && 
						(fPts >= OPEN_PTS(11)) && (fPts < OPEN_PTS(13))) ||
			 ((nPartnersSuitSupport >= SS_WEAK_SUPPORT) &&
					    (fAdjPts >= OPEN_PTS(11)) && (fAdjPts < OPEN_PTS(13))) ) 
		{

			// note, however, that this point range can still
			// look attractive for a triple raise if we have
			// less than 10 High Card points
			if ((ISMAJOR(nPartnersSuit)) && 
				(fCardPts > OPEN_PTS(5)) && (fCardPts < OPEN_PTS(10)) &&
				(numSupportCards >= 5) &&
				((numVoids > 0) || (numSingletons > 0)) ) 
			{
				m_nAgreedSuit = nPartnersSuit;
				m_nBid = MAKEBID(nPartnersSuit,4);
				status << "H00! Have " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points with " & numSupportCards & " trumps and ";
				if (numVoids == 1)
					status < "a void suit";
				else if (numVoids > 1)
					status < numVoids & " void suits";
				else if (numSingletons == 1)
					status < "a singleton suit";
				else
					status < numSingletons & " singleton suits";
				status < ", so make a triple raise of " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// look for a limit raise of partner's major suit
			if ((ISMAJOR(nPartnersSuit)) &&
				  (PlayingConvention(tidLimitRaises)) &&
									(fAdjPts >= OPEN_PTS(11)) && 
									(numSupportCards >= 4)) 
			{	
				m_nAgreedSuit = nPartnersSuit;
				m_nBid = MAKEBID(nPartnersSuit,3);
				status << "H02! Have " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points with " & numSupportCards & 
						  "-card trump support (holding " & szHP & 
						  ") playing limit raises, so raise partner's " & 
						  szPS & " to " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// if partner has bid a minor, respond in any decent 5-card major,
			// even if it's not our preferred suit
			if (ISMINOR(nPartnersSuit) &&
				( ((numCardsInSuit[HEARTS] >= 5) && (nSuitStrength[HEARTS] >= SS_OPENABLE)) || 
				  ((numCardsInSuit[SPADES] >= 5) && (nSuitStrength[SPADES] >= SS_OPENABLE)) ) ) 
			{	
				int nSuit = PickSuperiorSuit(SPADES,HEARTS);
				m_nBid = MAKEBID(nSuit, 1);
				status << "H03! We have " & fCardPts & "/" & fPts & "/" & fAdjPts &
						  " points and a " & numCardsInSuit[nSuit] & 
						  "-card " & STSS(nSuit) & " suit, so respond with " &
						  BTS(m_nBid) & " in preference to partner's minor.\n";
				return ValidateBid(m_nBid);
			}

			// see if we have a preferred suit different from partner's
			if (nPrefSuit != nPartnersSuit) 
			{
				// bid this suit at the cheapest level
				m_nBid = GetCheapestShiftBid(nPrefSuit);
				if (ISMAJOR(nPartnersSuit)) 
				{
					if (nPartnersSuitSupport <= SS_WEAK_SUPPORT)
						status << "H04! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & " points" &
								  ((PlayingConvention(tidLimitRaises))? " " :  " (in between single & double raise ranges) ") &
								  " and " & SLTS(nPartnersSuit) & " support for partner's " & szPSS & 
								  " suit (holding " & szHP & 
								  "), so show our preferred " & szPrefSS & 
								  " suit in a " & BTS(m_nBid) & " bid.\n";
					else
						status << "H05! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & " points" &
								  ((PlayingConvention(tidLimitRaises))? " " :  " (in between single & double raise ranges) ") &
								  " and " & SLTS(nPartnersSuit) & 
								  " support for partner's " & szPSS & " suit (holding " & szHP & 
								  "), but first show our preferred " & szPrefSS & 
								  " suit in a " & BTS(m_nBid) & " bid.\n";
					return ValidateBid(m_nBid);
				} 
				else 
				{
					// partner's suit is a minor
					// shift if support is weak, OR if we have
					// a major to bid instead
					if ((nPartnersSuitSupport <= SS_WEAK_SUPPORT) ||
									(ISMAJOR(nPrefSuit))) 
					{
						status << "H08! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & " points" &
								  ((PlayingConvention(tidLimitRaises))? " " :  " (in between single & double raise ranges) ") &
								  " but prefer to show our own " & szPrefSS & 
								  " suit instead of raising a minor, so bid " & BTS(m_nBid) & ".\n";
						return ValidateBid(m_nBid);
					}
				}
			}

			// no suit of our own to bid, so raise partner's 
			// major opening to a 2 or 3 level
			// raise to the 3 level with 11-12 pts 
			// only if holding _very_ strong trump support
			if ((ISMAJOR(nPartnersSuit)) && (fAdjPts >= OPEN_PTS(11)) && 
							(nPartnersSuitSupport > SS_GOOD_SUPPORT)) 
			{
				m_nAgreedSuit = nPartnersSuit;
				m_nBid = MAKEBID(nPartnersSuit,3);
				status << "H20! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
						  " points with " & SLTS(nPartnersSuit) & 
						  " support for partner's " & szPSS &
						  " suit (holding " & szHP & "), so we can raise to " & 
						  BTS(m_nBid) & "\n";
				return ValidateBid(m_nBid);
			}

	 		// else raise a major suit to 2 with 11-12.5 pts and weak trump support
			if ((ISMAJOR(nPartnersSuit)) && (fAdjPts < OPEN_PTS(13)) &&
							(nPartnersSuitSupport >= SS_WEAK_SUPPORT)) 
			{
				m_nAgreedSuit = nPartnersSuit;
				m_nBid = MAKEBID(nPartnersSuit,2);
				status << "H24! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
						  " points with " & SLTS(nPartnersSuit) &
						  " support for partner's " & szPSS & 
						  " suit (holding " & szHP & 
						  ") and no other good suit of our own, so raise to " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// shift from partner's minor to any decent 4-card major
			if ((ISMINOR(nPartnersSuit)) && 
				(IsSuitShiftable(HEARTS) || IsSuitShiftable(SPADES))) 
			{
				// shift to the major
				nSuit = GetBestSuitofAtLeast(HEARTS,SPADES,4,SP_SECOND);
				if (numSuitPoints[nSuit] >= 2) {
					m_nBid = GetCheapestShiftBid(nSuit);
					if (nPartnersSuitSupport < SS_GOOD_SUPPORT)
						status << "H28! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
							      " points but prefer not to raise a minor, so bid our " & LEN(nSuit) & 
								  "-card major " & STSS(nSuit) & " suit instead at " & BTS(m_nBid) & ".\n";
					else
						status << "H30! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
								  " points and " & SLTS(nPartnersSuit) & 
								  " support for partner's " & szPS &
								  " (holding " & szHP & "), but prefer not to raise a minor, so bid our " &
								  LEN(nSuit) & "-card major " & STSS(nSuit) & " suit instead at " & BTS(m_nBid) & ".\n";
					return ValidateBid(m_nBid);
				}
			}

	 		// finally, with no other choice, raise a minor 
	 		// to the 2 or 3 level 
			if ((ISMINOR(nPartnersSuit)) && 
							  (nPartnersSuitSupport >= SS_WEAK_SUPPORT)) 
			{
				// limit raise with 4 trumps
				if ((PlayingConvention(tidLimitRaises)) && 
										(numSupportCards >= 4)) 
				{
					m_nBid = MAKEBID(nPartnersSuit,3);
					status << "H40! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
							  " points with " & numSupportCards & 
							  "-card support for partner's minor " & szPSS &
							  " suit (holding " & szHP & 
							  ") playing limit raises, and we have no other good suits, so jump raise partner to " & BTS(m_nBid) & ".\n";
				} 
				else 
				{
					m_nBid = MAKEBID(nPartnersSuit,2);
					status << "H42! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
							  " points with no other good suits, so have to raise partner' minor " &
							  szPSS & " suit (holding " & szHP & ") to " & BTS(m_nBid) & ".\n";
				}
				m_nAgreedSuit = nPartnersSuit;
				return ValidateBid(m_nBid);
			}

			// if all the above failed, it means that we have no preferred 
			// suit of our own, & can't support partner's suit at even the 2 level.
			// this shouldn't happen at 11-12 points, but if it does,
			// just shift to any decent suit
			if (fPts >= 10) 
			{
				nSuit = GetNextBestSuit(nPartnersSuit);
				if (IsSuitShiftable(nSuit)) 
				{
					m_nBid = MAKEBID(nSuit,2);
					status << "H50! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
							  " points, but have " & SLTS(nPartnersSuit) & 
							  " support for partner's " & szPSS & 
							  " suit (holding " & szHP & 
							  "), and don't have a good suit of our own, so just shift to " & BTS(m_nBid) & ".\n";
					return ValidateBid(m_nBid);
				}
			}

			// here, we're really stuck for a bid, so bid 1NT
			m_nBid = BID_1NT;
			status << "H60! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
					  " points, but can't support partner's " & szPSS &
					  " suit at even the 2-level (holding " & szHP & "), and have only weak suits of our own, so give up and bid 1NT.\n";
			return ValidateBid(m_nBid);

		}


		//
		//--------------------------------------------------------
		//
		// Responding to a 1-bid, with 13-18.5 pts in hand
		// (13-18 orig pts or 13-18.5 reval. pts w/ trump support)
		// 
		if ( ((nPartnersSuitSupport < SS_WEAK_SUPPORT) && 
						(fPts >= OPEN_PTS(13)) && (fPts < OPEN_PTS(19))) ||
			 ((nPartnersSuitSupport >= SS_WEAK_SUPPORT) &&
			 			(fAdjPts >= OPEN_PTS(13)) && (fAdjPts < OPEN_PTS(19))) ) 
		{

			// see if we have good support for partner's major suit
			// if so, make a double raise
			if ((!PlayingConvention(tidLimitRaises)) &&
					(ISMAJOR(nPartnersSuit)) &&
						(nPartnersSuitSupport >= SS_GOOD_SUPPORT)) 
			{
				// make a double raise
				m_nAgreedSuit = nPartnersSuit;
				m_nBid = MAKEBID(nPartnersSuit,3);
				status << "I00! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
						  " points with " & SLTS(nPartnersSuit) & 
						  " support for partner's " & szPSS &
						  " suit (" & szHP & "), so make a double raise to " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// else see if we have a balanced hand worthy of a 
			// 2NT or 3NT bid -- also need the other suits stopped
			// need strength in the unbid suits (i.e., > than a Jack)
			if ((bBalanced) && (m_fMinTPCPoints >= PTS_NT_GAME-1) &&
					(m_pHand->AllOtherSuitsStopped(nPartnersSuit))) 
			{
				// need 27+ pts (alt) for a 3NT bid, 25+ for 2NT
				// because partner may not be balanced
				if (m_fMinTPCPoints > PTS_GAME+1)
					m_nBid = BID_3NT;
				else
					m_nBid = BID_2NT;
				status << "I04! Have " & fCardPts & 
						  " HCPs in hand, for a total in the partnership of approx. " &
						  m_fMinTPCPoints & "-" & m_fMaxTPCPoints & 
						  " HCPS, and all" &
						  ((numSuitsStopped == 4)? " four" : " unbid" ) &
						  " suits are stopped, so jump to " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// if playing limit raises, we shift to another suit
			// at this point range
			if ((PlayingConvention(tidLimitRaises)) && 
					(nPartnersSuitSupport >= SS_WEAK_SUPPORT)) 
			{
				// record the suit to return to 
				m_nIntendedSuit = nPartnersSuit;
				// and bid a new suit at the cheapest level
				nSuit = GetNextBestSuit(nPartnersSuit);
				m_nBid = GetCheapestShiftBid(nSuit);
				status << "I08! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
						  " points with " & numSupportCards & 
						  "-card support for partner's " & szPS & " (holding " & szHP & 
						  "), which is too strong for a limit raise, so we have to temporarily shift to " & STS(nSuit) & 
						  " with a bid of " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// else shift to preferred suit, if it's different from partner's
			if (nPrefSuit != nPartnersSuit) 
			{
				// bid the new suit at the cheapest level
				m_nBid = GetCheapestShiftBid(nPrefSuit);
				if (ISMAJOR(nPartnersSuit))
					status << "I12! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
							  " points but insufficient trump support (holding " & 
							  szHP & ") for a double raise, so show our own " & 
							  szPrefSS & " suit by bidding " & BTS(m_nBid) & ".\n";
				else
					status << "I14! Have "  & fCardPts & "/" & fPts & "/" & fAdjPts & 
							  " points but we prefer to shift instead of raising a minor " &
							  szPSS & " suit, so show our own " & szPrefSS & 
							  " suit by bidding " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// here, we don't quite have the trump support for a
			// double raise, but can do so because of high points
				   
			if ((!PlayingConvention(tidLimitRaises)) &&
				(ISMAJOR(nPartnersSuit)) && 
				(fAdjPts >= OPEN_PTS(16)) && (numSupportCards >= 4)) 
			{
				// we can sneak in a double raise with 16+ pts & 4 small trumps
				m_nAgreedSuit = nPartnersSuit;
				m_nBid = MAKEBID(nPartnersSuit,3);
				status << "I16! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
						  " points with " & SLTS(nPartnersSuit) & 
						  " support for partner's major " & szPSS &
						  " suit (holding " & szHP & "_; a double raise to " &
						  BTS(m_nBid) & " is reasonable.\n";
				return ValidateBid(m_nBid);
			}

			// shift from partner's minor to any decent 4-card major
			if ((ISMINOR(nPartnersSuit)) && 
				(IsSuitShiftable(HEARTS) || IsSuitShiftable(SPADES))) 
			{
				// shift to the major
				nSuit = GetBestSuitofAtLeast(HEARTS,SPADES,4,SP_SECOND);
				if (numSuitPoints[nSuit] >= 1) {
					m_nBid = GetCheapestShiftBid(nSuit);
					status << "I18! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
							  " points but prefer not to raise a minor, so bid our " &
							  LEN(nSuit) & "-card major " & STSS(nSuit) & 
							  " suit instead at " & BTS(m_nBid) & ".\n";
					return ValidateBid(m_nBid);
				}
			}

			// here, we have to double-raise a minor, which
			// which we'd normally rather avoid
			if ((ISMINOR(nPartnersSuit)) &&
						(nPartnersSuitSupport >= SS_GOOD_SUPPORT)) 
			{
				m_nAgreedSuit = nPartnersSuit;
				m_nBid = MAKEBID(nPartnersSuit,3);
				status << "I20! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
						  " points and " & SLTS(nPartnersSuit) & 
						  " support for partner's minor " & szPSS & 
						  " suit (holding " & szHP & 
						  ").  Although we'd prefer not to raise a minor, we have no other good options, so raise to " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// here, we've failed all the other tests, and hence
			// can only raise a suit to the 2 level
			m_nAgreedSuit = nPartnersSuit;
			m_nBid = MAKEBID(nPartnersSuit,2);
			status << "I24! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
					  " points, but have only " & SLTS(nPartnersSuit) & 
					  " support for partner's " & szPSS & 
					  " suit (holding " & szHP & 
					  "), and no good suit of our own, so we can only raise to " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);

		}

		//
		//--------------------------------------------------------
		//
		// Responding to a 1-bid, with 19+ pts in hand
		// Make a jump response in a new suit
		// 
		if (((nPartnersSuitSupport < SS_WEAK_SUPPORT) && (fPts >= OPEN_PTS(19))) ||
			((nPartnersSuitSupport >= SS_WEAK_SUPPORT) && (fAdjPts >= OPEN_PTS(19))) ) 
		{
		
			// if we have a different preferred suit, respond in that
			if (nPrefSuit != nPartnersSuit) 
			{
				m_nBid = GetJumpShiftBid(nPrefSuit,nPartnersBid);
				status << "J10! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
						  " points; show strength by jump shifting to " & BTS(m_nBid) & ".\n";
				return ValidateBid(m_nBid);
			}

			// else we like partner's suit, but have to show strength
			// by jump shifting somewhere, anywhere
			// but plan on returning to partner's suit
			m_nIntendedSuit = nPartnersSuit;
			nSuit = GetNextBestSuit(nPartnersSuit);
			m_nBid = GetJumpShiftBid(nSuit,nPartnersBid);
			status << "J20! Have " & fCardPts & "/" & fPts & "/" & fAdjPts & 
	  				  " points; show strength by jump shifting to " & BTS(m_nBid) & 
					  ", even though it's not the preferred suit.\n";
			return ValidateBid(m_nBid);
		}
	}






	//
	//===============================================================
	//
	// Partner doubled
	//
	// - if the particular double is not covered by a convention,
	// assume it's for penalties and pass.
	//
	if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
	{
		m_nBid = BID_PASS;
		status << "B2RspD! Assuming partner's " & szPB &
				  " is for penalties and pass.\n";
		return ValidateBid(m_nBid);
	}




	//
	//===============================================================
	//
	// Here, reached an unknown state
	status << "<<Error>> -- ZA0! fell out of decision loop.  Partner's bid was " & 
			  BTS(nPartnersBid) & "; points are " & fCardPts & "/" & fPts & "/" & fAdjPts & 
			  ", preferred suit is " & szPrefSS & " (holding " & szHPref & ").  Returning PASS\n";
	return ValidateBid(m_nBid);


}




