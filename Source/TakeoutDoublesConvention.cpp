//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// TakeoutDoubleConvention.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "Player.h"
#include "PlayerStatusDialog.h"
#include "TakeoutDoublesConvention.h"
#include "OvercallsConvention.h"
#include "ConventionSet.h"


extern COvercallsConvention	overcallsConvention;



//
//===============================================================================
//
// TryConvention()
//
// check if we can bid a takeout double here
//
BOOL CTakeoutDoublesConvention::TryConvention(const CPlayer& player, 
											  const CConventionSet& conventions, 
											  CHandHoldings& hand, 
											  CCardLocation& cardLocation, 
											  CGuessedHandHoldings** ppGuessedHands,
											  CBidEngine& bidState,  
											  CPlayerStatusDialog& status)
{
	//
	// the requirements for a takeout double are:
	// 1: one or both of the opponents must've opened (LHO/RHO) at the 1, 2, or 3 level
	// 2: partner must not have bid yet, or else passed,
	// 3: need 12+ HCPs (opening strength),
	// 4: we must either not have bid yet, or doubled once (e.g., 1H->X->2H->p->X)
	// 5: need at least 3+ cards in all the unbid suits, 
	// 6: no more than 3 cards in the enemy suit, and
	// 7: the hand is unsuitable for a 1NT overcall (???)

	// test conditions 1,2, 3, & 4
	if ( ((bidState.nLHOBid > BID_PASS) || (bidState.nRHOBid > BID_PASS)) &&
		 (bidState.nPartnersBid <= BID_PASS) &&
		 (bidState.fCardPts >= OPEN_PTS(12)) &&
 		 ((bidState.m_numBidTurns == 0) ||
		  ((bidState.m_numBidTurns == 1) && (bidState.m_nBid == BID_DOUBLE))) )
	{
		 // passed
	}
	else
	{
		return FALSE;
	}

	// test condition #5
	int i,nSuit,nOppSuit,nOppBidLevel;
	int nOppSuit2 = NONE; 
	BOOL bEnoughLength = TRUE;
	if (bidState.nLHOBid > BID_PASS)	// get the first enemy suit
	{
		// LHO has bid
		nOppSuit = bidState.nLHOSuit;
		nOppBidLevel = bidState.nLHOBidLevel;
		// see if RHO also bid, perhaps using a different suit
		if ((bidState.nRHOBid > BID_PASS)  && (bidState.nRHOBid < BID_DOUBLE))
		{
			nOppBidLevel = bidState.nRHOBidLevel;
			if (BID_SUIT(bidState.nRHOBid) != nOppSuit)
				nOppSuit2 = bidState.nRHOSuit;
		}
	}
	else
	{
		// only RHO has bid
		nOppSuit = bidState.nRHOSuit;
		nOppBidLevel = bidState.nRHOBidLevel;
	}
	//
	for(i=0,nSuit=nOppSuit;i<3;i++)	// and examine the other suits
	{
		nSuit = GetNextSuit(nSuit);
		if ((nSuit != nOppSuit2) && 
			((bidState.numCardsInSuit[nSuit] < 3) || 
					(bidState.nSuitStrength[nSuit] < SS_WEAK_SUPPORT)) )
		{
			// violation
			bEnoughLength = FALSE;
			break;	
		}
	}
	// can't use a takeout double here
	if (!bEnoughLength)
		return FALSE;

	// also can't takeout at a level higher than 3
	if (nOppBidLevel > 3)
		return FALSE;

	// or if 3-level takeouts are disabled
	if (!conventions.IsOptionEnabled(tb3LevelTakeouts))
		return FALSE;

	// if the opponents bid at the 3-level, we need 13+ pts
	if ((nOppBidLevel == 3) && (bidState.fCardPts < OPEN_PTS(13)))
		return FALSE;

	// test condition #6
	if (bidState.numCardsInSuit[nOppSuit] >= 4)
		return FALSE;

	// if we're doubling a second time, need 14+ HCPs
	BOOL bDoublingTwice = FALSE;
	 if ((bidState.m_numBidTurns == 1) && (bidState.m_nBid == BID_DOUBLE) &&
		 (bidState.GetConventionStatus(this) == CONV_INVOKED))
	{
		 // need 14+ HPCs
		 if (bidState.fCardPts < OPEN_PTS(14))
		 {
			 // spout a message here?
			return FALSE;
		 }
		 //
		 bDoublingTwice = TRUE;
	 }	 


	// finally, apply test #7
	int n1NTMin = pCurrConvSet->GetNTRangeMin(1);
	int n1NTMax = pCurrConvSet->GetNTRangeMax(1);
	int n2NTMin = pCurrConvSet->GetNTRangeMin(2);
	int n2NTMax = pCurrConvSet->GetNTRangeMax(2);
	int n3NTMin = pCurrConvSet->GetNTRangeMin(3);
	int n3NTMax = pCurrConvSet->GetNTRangeMax(3);
	double fCardPts = bidState.fCardPts;
	if ((bidState.bBalanced) && 
		(((fCardPts >= OPEN_PTS(n1NTMin)) && (fCardPts <= n1NTMax) && (nOppBidLevel == 1)) ||
		 ((fCardPts >= OPEN_PTS(n2NTMin)) && (fCardPts <= n2NTMax) && (nOppBidLevel <= 2)) ||
		 ((fCardPts >= OPEN_PTS(n3NTMin)) && (fCardPts <= n3NTMax) && (nOppBidLevel <= 3))) )
		return FALSE;

	//
	// we've now jumped through all the hoops and passed the tests
	// so make the bid
	//
	status << "TKOUT! With " & bidState.fPts & "/" & bidState.fPts & 
			  " points and support for all unbid suits, bid a takeout double" &
			  (bDoublingTwice? "again" : "") & ".\n";
	int nBid = BID_DOUBLE;
	bidState.SetBid(nBid);
	bidState.SetConventionStatus(this, CONV_INVOKED);
	return TRUE;
}





//
//-----------------------------------------------------
//
// respond to partner's takeout double
//
BOOL CTakeoutDoublesConvention::RespondToConvention(const CPlayer& player, 
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
	// Bidding in response to partner's takeout bid? check requirements
	//
	// the identifying marks of a takeout double are:
	// 1: this must be round #1, 2, or 3
	// 2: we must not have bid yet, or else passed
	// 3: at least one opponent must have bid ahead of partner
	// 4: opponent has not bid after partner's double, satisfied by #3 above
	// 5: the opponent's bid must be at the 1 or 2 level

	int nBid,nRound = 1;
	int nPartnersBid = bidState.nPartnersBid;
	int nLastValidBid = pDOC->GetLastValidBid();
	int nLastValidBidLevel = BID_LEVEL(nLastValidBid);
	bool bSecondDouble = (bidState.nPartnersPrevBid == BID_DOUBLE);
	int nConventionStatus = bidState.GetConventionStatus(this);

	// apply tests #1, 2, and 3
	if ( (nPartnersBid == BID_DOUBLE) && (bidState.nRound <= 2) && 
		 (bidState.m_nBid <= BID_PASS) && (nLastValidBidLevel <= 3))
	{
		//
		status << "TKOTR! Partner has doubled for takeout.\n";
	}
	else if ((bidState.GetConventionStatus(this) == CONV_RESPONDED_ROUND1) &&
		     (nPartnersBid != BID_PASS))
	{
		// second bid after partner's takeout
		nRound = 2;
		status << "2TKOTRx! Partner bid " & bidState.szPB & " after his takeout and our " &
				  bidState.szPVB & " response.\n";
	}
	else
	{
		return FALSE;
	}

	// see if partner's double for takeout at the 3-level is valid
	if ((nLastValidBidLevel == 3) && !conventions.IsOptionEnabled(tb3LevelTakeouts))
	{
		status << "5TKOTR2! Since 3-level takeouts are not enabled, partner's double here must be for penalties.\n";
		return FALSE;
	}

	//
	// see what round this is
	//
	if (nConventionStatus == CONV_INACTIVE)
	{

		//
		//==========================================================
		//
		// handle first bid after the takeout
		//

		//
		// estimate partner's strength
		//
		int nPartnersPoints = 12;

		// if partner used a takeout at the 3-level, it indicates 13+ pts
		if (nLastValidBidLevel == 3)
			nPartnersPoints++;
		// if partner doubled twice, it indicates more points again
		if (bSecondDouble)
		{
			status << "4TKORT5! Partner's second double in a row indicates extra points.\n";
			nPartnersPoints += 2;
		}

		bidState.m_fPartnersMin = OPEN_PTS(nPartnersPoints);
		bidState.m_fPartnersMax = Min(OPEN_PTS(22),40 - bidState.fCardPts);
		bidState.m_fMinTPPoints = bidState.fPts + bidState.m_fPartnersMin;
		bidState.m_fMaxTPPoints = bidState.fPts + bidState.m_fPartnersMax;
		bidState.m_fMinTPCPoints = bidState.fCardPts + bidState.m_fPartnersMin;
		bidState.m_fMaxTPCPoints = bidState.fCardPts + bidState.m_fPartnersMax;
		status << "2TKORT8! Partner's takeout indicates " & bidState.m_fPartnersMin & 
				  "+ HCPs, for a total in the partnership of " &
				  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & 
				  " points.\n";

		// see if RHO has redoubled
		if (bidState.nRHOBid == BID_REDOUBLE)
		{
			status << "TKOTR10! Right-hand opponent has redoubled, but ignore it.\n";
		}
		// see if RHO has bid something
		else if (bidState.nRHOBid > BID_PASS)
		{
			status << "TKOTR12! But Right-hand opponent has bid after partner's double, so the takeout is no longer in effect.\n";

			// try to compete and outbid RHO, if possible
			nBid = bidState.GetCheapestShiftBid(bidState.nPrefSuit, bidState.nRHOBid);
			int nLevel = BID_LEVEL(nBid);
			// need 18+ total pts at the 1 level, 22+ at the 2 level, 24+ at the 3 level, 
			// and 26+ at the 4 level (assume partner contributes 12 HCPs)
			int nPartnersContrib = 12;
			if ( ((nLevel == 1) && (bidState.fCardPts >= PTS_NT_GAME-7-nPartnersContrib)) ||
				 ((nLevel == 2) && (bidState.fCardPts >= PTS_NT_GAME-3-nPartnersContrib)) ||
				 ((nLevel == 3) && (bidState.fCardPts >= PTS_NT_GAME-1-nPartnersContrib)) ||
				 ((nLevel == 4) && (bidState.fCardPts >= PTS_NT_GAME+1-nPartnersContrib)))
			{
				status << "TKOTR14! However, assuming partner has 12+ HCPs, that amount combined with our " &
						  bidState.fCardPts & "/" & bidState.fPts &
						  " points totals " & (bidState.fCardPts+12) & "/" & (bidState.fPts+12) &
						  " pts, which is enough for us to bid " & BTS(nBid) & ".\n";
				bidState.SetBid(nBid);
				bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND1);
			}
			else
			{
				// forget it; just pass
				status << "TKOTR17! And with only " & bidState.fCardPts & "/" & bidState.fPts &
						  " points, we choose to pass.\n";
				bidState.SetBid(BID_PASS);
				// exception: we mark the convention INACTIVE instead of FINISHED,
				// as partner may double again
//				bidState.SetConventionStatus(this, CONV_FINISHED);
				bidState.SetConventionStatus(this, CONV_INACTIVE);
			}
			//
			return TRUE;
		}


		//
		// now make the bid
		//
		int nSuit,nOrigSuit;
		double fCardPts = bidState.fCardPts;
		double fPts = bidState.fPts;
		int nEnemyBid = pDOC->GetLastValidBid();
		int nEnemyBidLevel = BID_LEVEL(nEnemyBid);
		int nEnemySuit = BID_SUIT(nEnemyBid);


		//
		// with 6-9.5 pts and a balanced hand, bid 1NT
		//
		if ((bidState.bBalanced) && (fCardPts >= OPEN_PTS(6)) && (fCardPts < OPEN_PTS(10)))
		{
			nBid = BID_1NT;
			status << "TKOTR20! With " & fCardPts & " HCPs and a balanced hand, respond with " &
					  BTS(nBid) & ".\n";
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND1);
			return TRUE;
		}


		//
		// else with 0-9.5 pts, bid the longest suit (or a reasonably long major)
		//
		if (fPts < OPEN_PTS(10))
		{
			// get the longest suit
			nSuit = hand.GetLongestSuit();
			// see if this is the enemy suit
			if (nSuit == nEnemySuit)
			{
				// find another suit to bid
				int nAltSuit = hand.GetSuitsByLength(1);
				// make sure it's got at least 4 cards
				if (hand.GetSuitLength(nAltSuit) >= 4)
				{
					status << "TKOTR20a! Our longest suit of " & STS(nSuit) & 
							  " happens to be the enemy suit, so respond in the next-longest suit of " &
							  STS(nAltSuit) & ".\n";
					nSuit = nAltSuit;
				}
				else
				{
					// can't bid at all, with only 9 pts
					nBid = BID_PASS;
					status << "TKOTR20c! Our longest suit of " & STS(nSuit) & 
							  " happens to be the enemy suit, and we don't have a decent alternate suit, nor do we have the 13+ pts required to cue-bid the enemy suit, so we have to pass.\n";
					bidState.SetConventionStatus(this, CONV_FINISHED);
					return TRUE;
				}
				// establish the bid
				nBid = bidState.GetCheapestShiftBid(nSuit, nEnemyBid);
			}
			else
			{
				nOrigSuit = nSuit;
				// if it's a minor, prefer a major suit even if it's not the longest
				if (ISMINOR(nSuit))
				{
					// get the second-longest suit
					int newSuit = hand.GetSuitsByLength(1);
					if (!ISMAJOR(newSuit))		// or the 3rd longest 
						newSuit = hand.GetSuitsByLength(2);
					// see if this is a decent suit
					// no fewer than n-1 cards and n-1 pts
					if ((bidState.numCardsInSuit[newSuit] >= (bidState.numCardsInSuit[nSuit] - 1)) &&
						(bidState.numSuitPoints[newSuit] >= (bidState.numSuitPoints[nSuit] - 1)) &&
						(newSuit != nEnemySuit))
						nSuit = newSuit;
				}
				else
				{
					// else it's a major, so see if the other major is the same 
					// length and better quality, and is not the enemy suit
					int nOtherMajor = (nSuit == SPADES)? HEARTS : SPADES;
					if ((bidState.numCardsInSuit[nOtherMajor] == bidState.numCardsInSuit[nSuit]) &&
						(bidState.nSuitStrength[nOtherMajor] > bidState.nSuitStrength[nSuit]) &&
						(nOtherMajor != nEnemySuit))
						nSuit = nOtherMajor;
				}
				// now set the bid
				nBid = bidState.GetCheapestShiftBid(nSuit, nEnemyBid);
				// see if we made a suit substitution
				if ((nSuit != nOrigSuit) && 
						(bidState.numCardsInSuit[nSuit] < bidState.numCardsInSuit[nOrigSuit]))
				{
					status << "TKOTR21! With " & fCardPts & "/" & fPts & " pts, respond in the major suit of " & 
							  STS(nSuit) & " (even though it's not our longest suit) at a bid of " & BTS(nBid) & ".\n";
				}
				else
				{
					status << "TKOTR22! With " & fCardPts & "/" & fPts & " pts, respond with our longest suit of " & 
							  STS(nSuit) & " at a bid of " & BTS(nBid) & ".\n";
				}
			}
			// done
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND1);
			return TRUE;
		}


		//
		//---------------------------------------------------------------
		// with 10+ pts, we either:
		// - bid a good major at the 3 or 4 level,
		// - bid 2NT or 3NT, or
		// - bid a 4/5 card suit, or
		// - bid the enemy suit
		//
		int nPrefSuit = bidState.nPrefSuit;
		// check the best two suits for a 5 or 6-card major 
		// that we can jump in
		nSuit = NONE;
		int nSecondSuit = bidState.GetNextBestSuit(nPrefSuit);
		int nLength;
		// make sure we're not bidding the enemy suit unless we have 13+ pts
		if ( (ISMAJOR(nPrefSuit) && (bidState.numPrefSuitCards >= 5)) &&
			 ((nPrefSuit != nEnemySuit) || (fPts >= 13)) )
		{
			nSuit = nPrefSuit;
			nLength = bidState.numPrefSuitCards;
		}
		else if ( (ISMAJOR(nSecondSuit) && (bidState.numCardsInSuit[nSecondSuit] >= 5)) &&
				  ((nSecondSuit != nEnemySuit) || (fPts >= 13)) )
		{
			nSuit = nSecondSuit;
			nLength = bidState.numCardsInSuit[nSecondSuit];
		}
		if (nSuit != NONE)
		{
			// jump to game with 13+ pts, or bid at the 3-level with less
			if (fPts >= OPEN_PTS(13))
			{
				nBid = bidState.GetGameBid(nSuit);
				status << "TKOTR20! With " & fCardPts & "/" & fPts & 
						  " pts and a " & nLength & "-card " & STSS(nSuit) & 
						  " suit, jump to game at " & BTS(nBid)  & ".\n";
			}
			else
			{
				nBid = MAKEBID(nSuit, 3);
				status << "TKOTR22! With " & fCardPts & "/" & fPts & 
						  " pts and a " & nLength & "-card " & STSS(nSuit) & " suit, " & 
						  ((nEnemyBidLevel < BID_1NT)? " jump to " : " bid ") &
						  BTS(nBid) & ".\n";
			}
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND1);
			return TRUE;
		}

		//
		// else see if we can bid 2NT or 3NT
		//
		if (bidState.bBalanced)
		{
			// bid 3NT with >= 13 HCPS and all suits stopped, or 2NT otherwise
			if (fCardPts >= OPEN_PTS(13)) 
			{
				if (hand.IsSuitStopped(nEnemySuit) && (nEnemySuit != NOTRUMP))
				{
					nBid = BID_3NT;
					status << "TKOTR30! With " & fCardPts & 
							  " HCPs, a balanced hand, and the opponents' " & STSS(nEnemySuit) & 
							  " stopped, respond with a bid of " & BTS(nBid) & ".\n";
				}
				else if (nEnemySuit == NOTRUMP)
				{
					nBid = BID_3NT;
					status << "TKOTR30! With " & fCardPts & 
							  " HCPs, a balanced hand, respond with a bid of " & BTS(nBid) & ".\n";
				}
				else
				{
					nBid = BID_2NT;
					status << "TKOTR32! We have " & fCardPts & 
							  " HCPs and a balanced hand, which would be enough for a response of 3NT, but the opponents' " 
							  & STSS(nEnemySuit) & " is not securely stopped, so just bid " & BTS(nBid) & ".\n";
				}
			}
			else
			{
				// don't have the points for a 3NT response
				nBid = BID_2NT;
				status << "TKOTR33! With " & fCardPts & 
						  " HCPs and a balanced hand, respond with a bid of " & 
						  BTS(nBid) & ".\n";
			}
			bidState.SetBid(nBid);
			bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND1);
			return TRUE;
		}

		//
		// here, we don't have a good major or a hand worthy of a NT response
		// with >= 13 pts, bid the enemy suit; with less (10-13), jump in the suit
		//
		if ((fPts >= OPEN_PTS(13)) && (nEnemySuit != NOTRUMP))
		{
			nBid = MAKEBID(nEnemySuit, nEnemyBidLevel+1);
			status << "TKOTR50! With " & fCardPts & "/" & fPts & 
					  " pts but without a long suit, make a game-forcing bid by bidding the opponents' suit at " & 
					  BTS(nBid)  & ".\n";
		}
		else
		{
			nSuit = hand.GetLongestSuit();
			nBid = bidState.GetJumpShiftBid(nSuit, nEnemyBid, JUMP_SHIFT_1);
			status << "TKOTR55! With " & fCardPts & "/" & fPts & 
					  " pts and no long major suit, jump to " & BTS(nBid) & 
					  ((nSuit == nEnemySuit)? ", even though it's the enemy suit." : ".") & "\n";
		}
		bidState.SetBid(nBid);
		bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND1);
		return TRUE;

	}
	else if (nConventionStatus == CONV_RESPONDED_ROUND1)
	{
		//
		//==========================================================
		//
		// handle the second bid after the takeout
		//

		// revalue hand for partner's new suit
		int nPartnersSuit = bidState.nPartnersSuit;
		int nPreviousSuit = bidState.nPreviousSuit;
		int nPrefSuit = bidState.nPrefSuit;

		// if partner bid a new suit after our own suit response to the 
		// takeout, it indicates 16+ pts in his hand.
		if ((nPartnersSuit != nPreviousSuit) && (nPreviousSuit != NOTRUMP))
		{
			// partner did indeed shift
			bidState.m_fPartnersMin = 16;
			// the most pts partner could have is approx. 40 - our HCPs - opener's HCPs
			bidState.m_fPartnersMax = MAX(16, 40 - bidState.fCardPts - 12);
			bidState.m_fMinTPPoints = bidState.fAdjPts + bidState.m_fPartnersMin;
			bidState.m_fMaxTPPoints = bidState.fAdjPts + bidState.m_fPartnersMax;
			bidState.m_fMinTPCPoints = bidState.fCardPts + bidState.m_fPartnersMin;
			bidState.m_fMaxTPCPoints = bidState.fCardPts + bidState.m_fPartnersMax;
			status << "TKOTR60! Partner shifted to a new suit after our response of " &
					  bidState.szPVB & " to his takeout, indicating that he has " &
					  bidState.m_fPartnersMin & "+ pts, for a total in the partnership of " &
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
					  " points.\n";
		} 
		else
		{
			// partner raised us, or bid a suit after out NT bid
			bidState.fAdjPts = hand.RevalueHand(REVALUE_DECLARER, nPreviousSuit, TRUE);
			bidState.m_fMinTPPoints = bidState.fAdjPts + bidState.m_fPartnersMin;
			bidState.m_fMaxTPPoints = bidState.fAdjPts + bidState.m_fPartnersMax;
			bidState.m_fMinTPCPoints = bidState.fCardPts + bidState.m_fPartnersMin;
			bidState.m_fMaxTPCPoints = bidState.fCardPts + bidState.m_fPartnersMax;
		}
		//
		double fAdjPts = bidState.fAdjPts;
		double fMinTPPoints = bidState.m_fMinTPPoints;
		double fMaxTPPoints = bidState.m_fMaxTPPoints;

		//
		//---------------------------------------------------------
		//
		//
		// pass with < 20 total minimum pts
		if (fMinTPPoints < OPEN_PTS(20)) 
		{
			status << "TKOTR62! With a total of only " & 
					  fMinTPPoints & "-" & fMaxTPPoints &
					  " points in the partnership, we have to pass.\n";
			bidState.SetBid(BID_PASS);
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return TRUE;
		}

		//
		//----------------------------------------------------------
		//
		// here, we have 20+ minimum total points
		//

		int nEnemyBid = pDOC->GetBidByPlayer(bidState.m_pLHOpponent->GetPosition(),0);
		int nEnemySuit = BID_SUIT(nEnemyBid);

		// see if partner raised our suit
		if (nPartnersSuit == nPreviousSuit)
		{
			// re-raise the suit as appropriate
			// raise to the 2-level with 20-22 pts and 4 trumps
			// raise to the 3-level with 23-24 pts and 5 trumps
			// raise to the 4-level with 26-32 pts (major) and 5 trumps
			// raise to the 4-level with 26-28 pts (minor) and 5 trumps
			// raise to the 5-level with 29-32 pts (minor) and 5 trumps
			if ( (bidState.RebidSuit(SUIT_ANY,REBID_AT_2,PTS_GAME-6,PTS_GAME-4,LENGTH_4)) ||
			     (bidState.RebidSuit(SUIT_ANY,REBID_AT_3,PTS_GAME-3,PTS_GAME-1,LENGTH_5)) ||
				 (bidState.RebidSuit(SUIT_ANY,REBID_AT_4,PTS_GAME,PTS_SLAM-1,LENGTH_5)) ||
				 (bidState.RebidSuit(SUIT_MINOR,REBID_AT_4,PTS_MINOR_GAME-3,PTS_MINOR_GAME-1,LENGTH_5)) ||
				 (bidState.RebidSuit(SUIT_MINOR,REBID_AT_5,PTS_MINOR_GAME,PTS_SLAM-1,LENGTH_5)) )
			{
				bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
				return TRUE;
			}

			// else pass with <= 32 pts
			if (fMinTPPoints < PTS_SLAM) 
			{
				status << "TKOTR65! With a total in the partnership of " & 
						  fMinTPPoints & "-" & fMaxTPPoints &
						  " points, we sign off on partner's bid of " & 
						  bidState.szPB & ".\n";
				bidState.SetBid(BID_PASS);
				bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
				return TRUE;
			}
			else
			{
				//
				// with 32+ min total points, think slam
				//
				// bid Blackwood, with the intended suit being either our best suit,
				// partner's suit, or notrump
				bidState.SetConventionStatus(this, CONV_FINISHED);
				bidState.InvokeBlackwood(nPreviousSuit);
				return TRUE;
			}
		}
		else
		{
			//
			// else partner shifted to a different suit
			//
			// with 20-24 min total pts, raise partner's suit to 
			// the 2 or 3-level with 3-card trump support
			if ( (bidState.RaisePartnersSuit(SUIT_ANY,RAISE_TO_2,PTS_GAME-5,PTS_GAME-3,SUPLEN_3)) ||
				 (bidState.RaisePartnersSuit(SUIT_ANY,RAISE_TO_3,PTS_GAME-2,PTS_GAME-1,SUPLEN_3)) )
			{
				bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
				return TRUE;
			}

			// with 23-24 HCPs, no good suit fit, and both of 
			// the other suits stopped, bid 2NT
			if ((bidState.nPartnersBidLevel == 2) && (bidState.numSupportCards < 3))
			{
				if (bidState.BidNoTrump(LEVEL_2,PTS_GAME-2,PTS_GAME-1,FALSE,STOPPED_ALLOTHER,nPartnersSuit))
				{
					bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
					return TRUE;
				}
			}

			// else rebid a 6-card suit with 22-24 pts
			if (bidState.RebidSuit(SUIT_ANY,REBID_CHEAPEST,PTS_GAME-3,PTS_GAME-1,LENGTH_6))
			{
				bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
				return TRUE;
			}

			// else with 20-24 min total pts, bid another good suit;
			// or else pass
			if (fMinTPPoints < PTS_GAME) 
			{
				// see what the bid would be
				int nSuit = bidState.GetNextBestSuit(nPreviousSuit, nPartnersSuit);
				if (nSuit == nEnemySuit)
					nSuit = bidState.GetFourthSuit(nPreviousSuit, nPartnersSuit, nEnemySuit);
				nBid = bidState.GetCheapestShiftBid(nSuit);
				if ( (bidState.nSuitStrength[nSuit] >= SS_MARGINAL_OPENER) && 
							(bidState.IsBidSafe(nBid)) )
				{
					// OK to make this bid
					status << "TKOTR68! With " & fMinTPPoints & "-" & fMaxTPPoints &
							  " points in the partnership and " & bidState.SLTS(bidState.nPartnersSuit) &
							  " support for partner's " & bidState.szPS & 
							  ", we shift to " & STS(nSuit) & " at a bid of " & BTS(nBid) & ".\n";
					bidState.SetBid(nBid);
					bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
				}
				else
				{
					// not 'nuff pts, so either pass or return to partner's suit
					// with 2 trumps
					if (bidState.RaisePartnersSuit(SUIT_ANY,RAISE_TO_2,PTS_GAME-5,PTS_GAME-3,SUPLEN_2))
					{
						bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
					}
					else if (bidState.RaisePartnersSuit(SUIT_ANY,RAISE_TO_3,PTS_GAME-2,PTS_GAME-1,SUPLEN_2))
					{
						bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
					}
					else
					{
						// gotta pass
						status << "TKOTR70! Unfortunately, with only " &
								  fMinTPPoints & "-" & fMaxTPPoints &
								  " pts in the partnership, " &
								  bidState.SLTS(bidState.nPartnersSuit) & " support for partner's " &bidState.szPS & 
								  ", and no other good suits we can bid safely, we have to pass.\n";
						bidState.SetBid(BID_PASS);
						bidState.SetConventionStatus(this, CONV_FINISHED);
					}
				}
				return TRUE;
			}

			//
			//--------------------------------------------------------
			//
			// here, we have 26+ min total points, so try to reach game
			//

			// raise a major to game with 3-card support & 26+ pts
			// or with 2-card support & 26+ pts
			if ( (bidState.RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_4,PTS_GAME,PTS_SLAM-1,SUPLEN_3)) ||
				(bidState.RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_4,PTS_GAME+1,PTS_SLAM-1,SUPLEN_2)) )
			{
				bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
				return TRUE;
			}

			// Bid 3NT with 26-31 HCPs and all suits stopped
			if (bidState.BidNoTrump(LEVEL_3,PTS_NT_GAME,PTS_SLAM-1,FALSE,STOPPED_ALLOTHER,nPartnersSuit))
			{
				bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
				return TRUE;
			}
			// or 2NT without all stoppers
			if (bidState.BidNoTrump(LEVEL_2,PTS_GAME,PTS_SLAM-1,FALSE,STOPPED_DONTCARE))
			{
				bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
				return TRUE;
			}

			// jump to game in a self-supporing major
			if (bidState.RebidSuit(SUIT_MAJOR,REBID_AT_4,PTS_GAME,PTS_SLAM-1,LENGTH_6,SS_SELFSUPPORTING,HONORS_3))
			{
				status << "TKOTR80! With a self-supporting " & bidState.szPrefS &
						  " suit and " & fMinTPPoints & "-" & fMaxTPPoints &
						  "+ team points, jump to game at " & BTS(bidState.m_nBid)  & ".\n";
				bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
				return TRUE;
			}
			
			// raise partner's major to the 3-level with 2 support cards
			// and 26+ points, if possible
			if (bidState.RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_3,PTS_GAME,PTS_SLAM-1,SUPLEN_2))
			{
				bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
				return TRUE;
			}

			// jump to game in a self-supporting minor (7 cards min)
			if (bidState.RebidSuit(SUIT_MINOR,REBID_AT_5,PTS_MINOR_GAME,PTS_SLAM-1,LENGTH_7,SS_SELFSUPPORTING,HONORS_3))
			{
				status << "TKOTR82! With a self-supporting " & bidState.szPrefS &
						  " suit and " & fMinTPPoints & 
						  "+ team points, jump to game at " & BTS(bidState.m_nBid)  & ".\n";
				bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
				return TRUE;
			}
			
			// raise partner's minor to 4 or 5 with 3 trumps
			if ( (bidState.RaisePartnersSuit(SUIT_MINOR,RAISE_TO_5,PTS_MINOR_GAME,PTS_SLAM-1,SUPLEN_3)) ||
				 (bidState.RaisePartnersSuit(SUIT_MINOR,RAISE_TO_4,PTS_GAME,PTS_SLAM-1,SUPLEN_3)) )
			{
				bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
				return TRUE;
			}

			// rebid a good 6-card suit
			if ( (bidState.RebidSuit(SUIT_MAJOR,REBID_CHEAPEST,PTS_GAME,PTS_SLAM-1,LENGTH_6,SS_STRONG)) ||
				 (bidState.RebidSuit(SUIT_MINOR,REBID_CHEAPEST,PTS_MINOR_GAME,PTS_SLAM-1,LENGTH_6,SS_STRONG)) )
			{
				bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
				return TRUE;
			}

			// else rebid our suit at the cheapest level
			else if (fMinTPPoints < PTS_SLAM) 
			{
				nBid = bidState.GetCheapestShiftBid(nPreviousSuit);
				status << "TKOTR85! With " & fMinTPPoints & "-" & fMaxTPPoints &
						  " points in the partnership but only " & bidState.SLTS(bidState.nPartnersSuit) &
						  " support for partner's " & bidState.szPS & 
						  ", we rebid our " & bidState.szPVSS & " suit at " & BTS(nBid) & ".\n";
				bidState.SetBid(nBid);
				bidState.SetConventionStatus(this, CONV_RESPONDED_ROUND2);
				return TRUE;
			}
			
			//
			// with 32+ min total points, think slam
			//
			// bid Blackwood, with the intended suit being either our best suit,
			// partner's suit, or notrump
			//
			// pick a suit
			int nSuit;
			if ((bidState.numPrefSuitCards >= 6) && 
				(bidState.nPrefSuitStrength >= SS_STRONG))
				nSuit = nPrefSuit;
			else if (bidState.numSupportCards >= 3)
				nSuit = nPartnersSuit;
			else
				nSuit = NOTRUMP;
			bidState.SetConventionStatus(this, CONV_FINISHED);
			bidState.InvokeBlackwood(nSuit);
			return TRUE;
		}
	}
	else
	{
		//
		return FALSE;
	}
} 







//
//==========================================================
//
// Rebidding as opener after partner responds to a takeout double
//
BOOL CTakeoutDoublesConvention::HandleConventionResponse(const CPlayer& player, 
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
//	int nBid;
	double fPts = bidState.fPts;
	double fAdjPts = bidState.fAdjPts;
	double fCardPts = bidState.fCardPts;
	int nPrefSuit = bidState.nPrefSuit;
	int nPrefSuitStrength = bidState.nPrefSuitStrength;
	int nPreviousSuit = bidState.nPreviousSuit;
	BOOL bBalanced = bidState.bBalanced;
	//
	int nPartnersBid = bidState.nPartnersBid;
	int nPartnersBidLevel = bidState.nPartnersBidLevel;
	int nPartnersSuit = bidState.nPartnersSuit;
	int nPartnersSuitSupport = bidState.nPartnersSuitSupport;
	int nPartnersPrevSuit = bidState.nPartnersPrevSuit;
	int numSupportCards = bidState.numSupportCards;

	//
	if (bidState.GetConventionStatus(this) == CONV_INVOKED_ROUND1) 
	{
		//
		//--------------------------------------------------------
		// responding to partner's forced bid
		// - estimate partner's strength
		//

		// first check for a strange response
		if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
		{
			// we don't understand partner's bid
			return CConvention::HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status);
		}

		//
		// did partner pass? (horror of horrors!)
		//
		if (nPartnersBid == BID_PASS) 
		{
			if (bidState.nLHOBid >= BID_PASS)
				status << "2TKRb10! After interference from the left-hand opponent, partner passed our takeout.\n";
			else
				status << "2TKRb12! Partner unexpectedly passed our takeout double, which is supposed to be forcing.  Bidding will proceed as if the takeout was not made\n";
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return FALSE;
		}

		// set team point estimates -- be conservative
		BOOL bPartnerJumped = FALSE;
		BOOL bPartnerJumpedToGame = FALSE;
		int nEnemyBid = pDOC->GetValidBidRecord(0);
		int nEnemyBidLevel = BID_LEVEL(nEnemyBid);
		int nEnemySuit = BID_SUIT(nEnemyBid);
		if ((nPartnersBid - nEnemyBid) > 5)
			bPartnerJumped = TRUE;
		if (nPartnersBid == bidState.GetGameBid(nPartnersSuit))
			bPartnerJumpedToGame = TRUE;
		// flag to see if we doubled in preference to an overcall
		BOOL bWantedToOvercall = FALSE;

		//
		if (nPartnersSuit == NOTRUMP)
		{
			//
			if (nPartnersBid == BID_1NT)
			{
				// partner has 6-9 HCPs
				bidState.m_fPartnersMin = 6;
				bidState.m_fPartnersMax = 9;
			}
			else if (nPartnersBid == BID_2NT)
			{
				// partner has 10-12 HCPs, maybe more
				bidState.m_fPartnersMin = 10;
				bidState.m_fPartnersMax = 12;
			}
			else if (nPartnersBid == BID_3NT)
			{
				// partner has 13+ HCPs
				bidState.m_fPartnersMin = 13;
				bidState.m_fPartnersMax = 40 - fCardPts;
				status << "TKRb20! Partner's response of 3NT to our takeout double indicates that the opponent's suit is well stopped.\n";
			}
			else
			{
				// partner has 13+ HCPs???
				status << "TKRb21! Partner's response of " & BTS(nPartnersBid) & 
						  " to our takeout double is unorthodox; treating it like a 3NT response.\n";
				bidState.m_fPartnersMin = 13;
				bidState.m_fPartnersMax = MIN(22, 40 - fCardPts);
			}
			// accept NT if we hold at least a semi-balanced 
			// and we don't have a 6-card major
			if ( !(ISMAJOR(bidState.nPrefSuit) && (bidState.numPrefSuitCards >= 6)) &&
				 (bidState.bSemiBalanced) )
				bidState.m_nAgreedSuit = NOTRUMP;
			//
			bidState.m_fMinTPPoints = fAdjPts + bidState.m_fPartnersMin;
			bidState.m_fMaxTPPoints = fAdjPts + bidState.m_fPartnersMax;
			bidState.m_fMinTPCPoints = fCardPts + bidState.m_fPartnersMin;
			bidState.m_fMaxTPCPoints = fCardPts + bidState.m_fPartnersMax;
			status << "TKRb29! Partner's response of " & BTS(nPartnersBid) & 
					  " to our takeout double indicates " & 
					  bidState.m_fPartnersMin & "-" & bidState.m_fPartnersMax & 
					  " HCPs, for a total of " &
					  bidState.m_fMinTPCPoints & "-" & bidState.m_fMaxTPCPoints & " / " &
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & 
					  " pts in the partnership.\n";

		}
		else
		{
			// partner bid a suit
			// see if we really intended to overcall last time
			if (bidState.GetConventionStatus(&overcallsConvention) == CONV_SUBSUMED)
				bWantedToOvercall = TRUE;
			//
			if (nPartnersSuit == nEnemySuit)
			{
				// partner bid the enemy suit, showing 13+ pts.
				status << "TKORb40! Partner has responded in the enemy suit, indicating 13+ pts but no long suits.\n";
				bidState.m_fPartnersMin = 13;
				bidState.m_fPartnersMax = MIN(22, 40 - bidState.fCardPts);
				bidState.m_bGameForceActive = TRUE;
			}
			else if (bPartnerJumpedToGame)
			{
				// partner had 13+ pts & a 5-card major
				status << "TKORb41! Partner has jumped to game in " & STS(nPartnersSuit) &
						  ", indicating 13+ pts and a 5+ card suit.\n";
				bidState.m_fPartnersMin = 30;
				bidState.m_fPartnersMax = MIN(22, 40 - bidState.fCardPts);
				if (!bWantedToOvercall)
					bidState.m_nAgreedSuit = nPartnersSuit;
			}
			else if (bPartnerJumped)
			{
				// partner had 10-12 pts
				status << "TKORb42! Partner has made a jump response of " & BTS(nPartnersBid) &
						  ", indicating 10-12 pts and a 4-5 card suit.\n";
				bidState.m_fPartnersMin = 10;
				bidState.m_fPartnersMax = 12;
				if (!bWantedToOvercall)
					bidState.m_nAgreedSuit = nPartnersSuit;
			}
			else
			{
				// partner had <= 9 pts
				status << "TKORb43! Partner has made a minimum response of " & BTS(nPartnersBid) &
						  ", indicating no more than 9 points.\n";
				bidState.m_fPartnersMin = 0;
				bidState.m_fPartnersMax = 9;
				if (!bWantedToOvercall)
					bidState.m_nAgreedSuit = nPartnersSuit;
			}
			//
			bidState.m_fMinTPPoints = fAdjPts + bidState.m_fPartnersMin;
			bidState.m_fMaxTPPoints = fAdjPts + bidState.m_fPartnersMax;
			bidState.m_fMinTPCPoints = fCardPts + bidState.m_fPartnersMin;
			bidState.m_fMaxTPCPoints = fCardPts + bidState.m_fPartnersMax;
			status << "2TKRb49! The total point count in the partnership is therefore " &
					  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints & 
					  " pts.\n";
		}

			
		//
		// see if RHO bid after partner's response -- that mitigates our responsibility
		// to bid again, unless partner's bid was game forcing
		//
	//	if (bidState.nRHOBid > BID_PASS)
		if ((bidState.nRHOBid > BID_PASS) && 
			(bidState.nRHOBid != BID_DOUBLE) && (bidState.nRHOBid != BID_REDOUBLE))
		{
			status << "TKRb51! The right-hand opponent has " &
					  ((bidState.nRHOBid == BID_DOUBLE)? "doubled" : "bid") &
					  " after partner's response, interfering with our communication.\n";
/*
			status << "TKRb51! The right-hand opponent has bid after partner's response, interfering with our communication.\n";
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return FALSE;
*/
		}


		//
		//---------------------------------------------------------------------
		// see if we have an agreed suit
		//
		int nBid;
		if (bidState.m_nAgreedSuit > NONE)
		{
			if (bidState.m_nAgreedSuit == NOTRUMP)
			{
				// we've agreed to play in NoTrump
				// see if we can raise partner to a higher NT contract
				if (bidState.BidNoTrumpAsAppropriate(FALSE,STOPPED_DONTCARE))
				{
					bidState.SetConventionStatus(this, CONV_FINISHED);
					return TRUE;
				}
				// else pass
				nBid = BID_PASS;
				status << "TKRb69! With a total of " &
						  bidState.m_fMinTPCPoints & "-" & bidState.m_fMaxTPCPoints &
						  " HCPs in the partnership, we have insufficient strength to raise partner's " &
						  BTS(nPartnersBid) & " bid, so we have to pass.\n";
				bidState.SetBid(nBid);
				bidState.SetConventionStatus(this, CONV_FINISHED);
				return TRUE;
			}
			else
			{
				// we've agreed on a suit, so raise if possible
				// if partner didn't jump, we may have credited him with
				// zero pts, so adjust rqmts accordingly

				// if partner jumped to game && we have < 32 pts, pass
				if ((bPartnerJumpedToGame) && (bidState.m_fMinTPPoints < PTS_SLAM))
				{
					status << "TKRb70! Partner jumped to game in his " & bidState.szPSS & 
							  " suit, so with a team total of " &
							  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
							  " points, we pass.\n";
					bidState.SetConventionStatus(this, CONV_FINISHED);
					return TRUE;
				}
				// raise partner if possible -- bearing in mind that 
				// partner may have a wide range of points

				// raise a major to game with 24-32 pts and 4 trumps
				//					  or with 26-32 pts and 3 trumps
				// or raise to the 3-level with 21-25 pts and 3 trumps
				if ( (bidState.RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_4,PTS_GAME-2,PTS_SLAM-1,SUPLEN_4)) ||
				     (bidState.RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_4,PTS_GAME,PTS_SLAM-1,SUPLEN_3)) ||
					 (bidState.RaisePartnersSuit(SUIT_MAJOR,RAISE_TO_3,PTS_GAME-5,PTS_GAME-1,SUPLEN_3)))
				{
					if (!bPartnerJumped)
						status << "TKRb71a! (we can assume partner has some strength in the " & bidState.szPSS & 
								  " suit, so we are shading the requirements slightly.\n";
					bidState.SetConventionStatus(this, CONV_INVOKED_ROUND2);
					return TRUE;
				}
				// raise a minor to game with 26-32 pts and 4 trumps
				//                    or with 28-32 pts and 3 trumps
				// or raise to the 4-level with 26-27 pts and 3 trumps
				// or raise to the 3-level with 21-25 pts and 3 trumps
				if ( (bidState.RaisePartnersSuit(SUIT_MINOR,RAISE_TO_5,PTS_MINOR_GAME-3,PTS_SLAM-1,SUPLEN_4)) ||
					 (bidState.RaisePartnersSuit(SUIT_MINOR,RAISE_TO_5,PTS_MINOR_GAME-1,PTS_SLAM-1,SUPLEN_3)) ||
					 (bidState.RaisePartnersSuit(SUIT_MINOR,RAISE_TO_4,PTS_MINOR_GAME-3,PTS_MINOR_GAME-2,SUPLEN_3)) ||
					 (bidState.RaisePartnersSuit(SUIT_MINOR,RAISE_TO_3,PTS_MINOR_GAME-8,PTS_MINOR_GAME-4,SUPLEN_3)) )
				{
					if (!bPartnerJumped)
						status << "TKRb71b! We can assume partner has some strength in the " & bidState.szPSS & 
								  " suit, so we can shade the requirements slightly.\n";
					bidState.SetConventionStatus(this, CONV_INVOKED_ROUND2);
					return TRUE;
				}
				// with 33+ pts, invoke Blackwood
				if (bidState.m_fMinTPCPoints >= PTS_SLAM)
				{
					bidState.InvokeBlackwood(bidState.m_nAgreedSuit);
					bidState.SetConventionStatus(this, CONV_INVOKED_ROUND2);
					return TRUE;
				}
				// else pass
				nBid = BID_PASS;
				status << "TKRb90! With a total of " &
						  bidState.m_fMinTPPoints & "-" & bidState.m_fMaxTPPoints &
						  " points in the partnership, we have insufficient strength to raise partner's " &
						  BTS(nPartnersBid) & " bid, so we have to pass.\n";
				bidState.SetBid(nBid);
				bidState.SetConventionStatus(this, CONV_FINISHED);
				return TRUE;
			}
		}


		//
		//--------------------------------------------------------------------------
		// here, we have no suit agreement (e.g., partner bid the opponents' suit)
		//
		int nLastBid = pDOC->GetLastValidBid();
		if (bBalanced)
		{
			// try notrumps
			if ((nEnemySuit != NOTRUMP) && (hand.IsSuitProbablyStopped(nEnemySuit)))
			{
				status << "TKRb80! Without clear suit agreement, and holding a blanaced hand, we want to steer towards a contract in No Trump.\n";
				if (bidState.BidNoTrumpAsAppropriate(FALSE,STOPPED_DONTCARE))
				{
					bidState.SetConventionStatus(this, CONV_INVOKED_ROUND2);
					return TRUE;
				}
			}
			// else pass
			status << "TKRb81! But as we do not have a proper hand to bid No Trump at the appropriate level, we have to pass.\n";
			bidState.SetBid(BID_PASS);
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return TRUE;
		}
		else if (bidState.numPrefSuitCards >= 5)
		{
			// bid the suit
			nBid = bidState.GetCheapestShiftBid(nPrefSuit, nLastBid);
			if (bidState.IsBidSafe(nBid, 4))
			{
				if (bWantedToOvercall)
					status << "TKRb90! Partner's forced response of " & bidState.szPB & 
							 " not withstanding, we can now show the " & 
							  STSS(nPrefSuit) & " suit that we wanted to overcall with last round by bidding " &
							  BTS(nBid) & ".\n";
				else
					status << "TKRb91! Without clear suit agreement, we bid our " &
							  bidState.numPrefSuitCards & "-card " & STSS(bidState.nPrefSuit) &
							  " suit at " & BTS(nBid) & ".\n";
				bidState.SetBid(nBid);
				bidState.SetConventionStatus(this, CONV_INVOKED_ROUND2);
				return TRUE;
			}
		}


		//
		//--------------------------------------------------------------------------
		// else we have no other options, so pass
		status << "TKORb99! We see no good fit with partner and no other options, so pass.\n";
		bidState.SetBid(BID_PASS);
		bidState.SetConventionStatus(this, CONV_FINISHED);
		return TRUE;

	}
	else
	{
		//
		// responding after partner's second response to our takeout
		//

		// first check for a strange response
		if ((nPartnersBid == BID_DOUBLE) || (nPartnersBid == BID_REDOUBLE))
		{
			// we don't understand partner's bid
			return CConvention::HandleConventionResponse(player, conventions, hand, cardLocation, ppGuessedHands, bidState, status);
		}

		// did partner pass?
		if (nPartnersBid == BID_PASS) 
		{
			bidState.SetConventionStatus(this, CONV_FINISHED);
			return FALSE;
		}

		//
		// otherwise, consider the choices:
		//
		// - with suit agreement, raise if possible
		//         S    N     S     N     S
		//   e.g., X -> 1C -> 1S -> 2S -> ???
		// - without suit agreement,
		//   e.g., X -> 1C -> 1S -> 2H -? ???
		//   bid the 4th suit if we have the pts (26+)
		// - without suit agreement, but if partner bids NT,
		//   or if we have a balanaced hand, bid NT
		//
		double fMinTPPoints =  bidState.m_fMinTPPoints;
		double fMaxTPPoints =  bidState.m_fMaxTPPoints;
		double fMinTPCPoints = bidState.m_fMinTPCPoints;
		double fMaxTPCPoints = bidState.m_fMaxTPCPoints;
		int nBid;
		
		//
		// see if we have suit agreement
		//
		if (nPartnersSuit == nPreviousSuit)
		{
			// partner raised our suit -- re-raise if possible
			bidState.m_nAgreedSuit = nPreviousSuit;
			if (nPartnersBid >= bidState.GetGameBid(nPreviousSuit))
			{
				// partner bid game or beyond
				if ((nPartnersBidLevel == 7) ||
					((nPartnersBidLevel == 6) && (fMinTPPoints <= PTS_GRAND_SLAM)) )
				{
					status << "TKRc10! Partner raised our " & bidState.szPVSS & 
							  " suit to a slam, so pass.\n";
				}
				else if ((nPartnersBidLevel <= 6) && (fMinTPPoints >= PTS_GRAND_SLAM+1))
				{
					nBid = MAKEBID(nPreviousSuit, 7);
					status << "TKRc11! Partner raised our " & bidState.szPVSS & 
							  " suit to " & 
							  ((nPartnersBidLevel == 6)? "slam" : "game") &
							  ", but we have the poitns to push to a grand slam, so bid " & 
							  BTS(nBid) & ".\n";
				}
				else if ((nPartnersBidLevel < 6) && (fMinTPPoints >= PTS_SMALL_SLAM+1))
				{
					nBid = MAKEBID(nPreviousSuit, 6);
					status << "TKRc12! Partner raised our " & bidState.szPVSS & 
							  " suit to game, but we have the poitns for a slam, so bid " & 
							  BTS(nBid) & ".\n";
				}
				else
				{
					nBid = BID_PASS;
					status << "TKRc13! Partner raised our " & bidState.szPVSS & 
							  " suit to game, which is acceptable with " & 
							  fMinTPPoints & "-" & fMaxTPPoints & 
							  " pts in the partnership, so pass.\n";
				}
			}
			else
			{
				// partner raised below game
				// re-raise if possible
				if ( (ISMAJOR(nPreviousSuit) && (fMinTPPoints >= PTS_GAME)) ||
					 (ISMINOR(nPreviousSuit) && (fMinTPPoints >= PTS_MINOR_GAME)) )
				{
					nBid = bidState.GetGameBid(nPreviousSuit);
					status << "TKRc20! With a total of " & 
							  fMinTPPoints & "-" & fMaxTPPoints & 
							  " pts in the partnership, raise to game in the " &
							  bidState.szPVSS & " suit at " & BTS(nBid) & ".\n";
				}
				else
				{
					// else try to raise cheaply
					nBid = bidState.GetCheapestShiftBid(nPreviousSuit);
					if (bidState.IsBidSafe(nBid))
					{
						status << "TKRc22! With a total of " & 
								  fMinTPPoints & "-" & fMaxTPPoints & 
								  " pts in the partnership, we can raise again to " &
								  BTS(nBid) & ".\n";
					}
					else
					{
						nBid = BID_PASS;
						status << "TKRc29! With a total of " & 
								  fMinTPPoints & "-" & fMaxTPPoints & 
								  " pts in the partnership, we cannot safely raise any partner further, so pass.\n";
					}
				}
			}
		}
		else if ((nPartnersSuit == NOTRUMP) || (bidState.bBalanced))
		{
			//
			// partner bid NT, or else we have a balanced hand
			//
			if (bidState.BidNoTrumpAsAppropriate(FALSE, STOPPED_DONTCARE))
			{
				nBid = bidState.m_nBid;
				status << "TKRc30! With a total of " & 
						  fMinTPCPoints & "-" & fMaxTPCPoints & 
						  " HCPs in the partnership, we can bid " & BTS(nBid) & ".\n";
			}
			else
			{
				nBid = BID_PASS;
				if (nPartnersSuit == NOTRUMP)
					status << "TKRc35! We're willing to accept a contract in NoTrumps, but don't have the points to raise further, so pass.\n";
				else
					status << "TKRc36! We'd like to play in NoTrumps, but don't have the points to bid agian, so pass.\n";
			}
		}
		else
		{
			//
			// else we have no suit agreement, and can't play NT
			//

			//
			// bid the 4th suit if we have enough pts
			//
			int nSuit = bidState.GetFourthSuit(nPreviousSuit, nPartnersSuit, nPartnersPrevSuit);
			nBid = bidState.GetCheapestShiftBid(nSuit);

			if ((fMinTPPoints >= PTS_GAME) && (nBid < bidState.GetGameBid(nSuit)))
			{
				status << "TKRc40! With a total of " & 
						  fMinTPPoints & "-" & fMaxTPPoints & 
						  " pts in the partnership and no suit agreement, bid another suit (" &
						  STS(nSuit) & ") at " & BTS(nBid) & ".\n";
			}
			else
			{
				// gotta pass
				nBid = BID_PASS;
				if (nPartnersBid >= bidState.GetGameBid(nPartnersSuit))
					status << "TKRc45 With a total of " & 
							  fMinTPPoints & "-" & fMaxTPPoints & 
							  " pts in the partnership, partner has gone to game in his suit at " &
							  bidState.szPB & ", so pass.\n";
				else if (fMinTPPoints >= PTS_GAME)
					status << "TKRc46! With a total of " & 
							  fMinTPPoints & "-" & fMaxTPPoints & 
							  " pts in the partnership, but having run out of bidding room, we have to bail out and pass.\n";
				else 
					status << "TKRc47! With a total of only " & 
							  fMinTPPoints & "-" & fMaxTPPoints & 
							  " pts in the partnership, and no agreement in suits, we have to pass.\n";
			}
		}
		// done with the second rebid 
		bidState.SetBid(nBid);
		bidState.ClearConventionStatus(this);
		return TRUE;
	}
}





//
//==================================================================
// construction & destruction
//
CTakeoutDoublesConvention::CTakeoutDoublesConvention() 
{
	// from ConvCodes.h
	m_nID = tidTakeoutDoubles;
}

CTakeoutDoublesConvention::~CTakeoutDoublesConvention() 
{
}


