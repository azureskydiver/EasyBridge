//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Bidopen.cpp
//
// Opening bidding routines
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
// MakeOpeningBid()
//
// make initial bid
//
int CBidEngine::MakeOpeningBid()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;

	//
	status << "2Opening bid for team.\n";



	//
	//--------------------------------------------------------
	//
	// First check to see if we can open using a convention
	// e.g., weak 2, stong 2, preemptive 3/4, etc.
	//
	if (pCurrConvSet->ApplyConventionTests(*m_pPlayer, *m_pHand, *m_pCardLocation, m_ppGuessedHands, *this, *m_pStatusDlg))
	{
		return ValidateBid(m_nBid);
	}




	//
	//--------------------------------------------------------
	//
	// see if we can open No Trumps
	//

	int	NTMin[3], NTMax[3];
	//
	NTMin[0] = pCurrConvSet->GetValue(tn1NTRangeMinPts);
	NTMax[0] = pCurrConvSet->GetValue(tn1NTRangeMaxPts);
	NTMin[1] = pCurrConvSet->GetValue(tn2NTRangeMinPts);
	NTMax[1] = pCurrConvSet->GetValue(tn2NTRangeMaxPts);
	NTMin[2] = pCurrConvSet->GetValue(tn3NTRangeMinPts);
	NTMax[2] = pCurrConvSet->GetValue(tn3NTRangeMaxPts);

	//
	if ((bBalanced) && (fCardPts >= OPEN_PTS(NTMin[0]))) 
	{
		//
		status << "A00! Have " & fCardPts & " HCP's with a balanced hand.\n";

		// but if we have a good 5-card major, open it in a suit
		// but not if we have to overcall to do so
		BOOL bNeedToOvercall = FALSE;
		if ( ((nLHOBid > BID_PASS) || (nRHOBid > BID_PASS)) && (nPartnersBid <= BID_PASS))
			bNeedToOvercall = TRUE;
		//
		if (IsSuitOpenable(HEARTS) && !bNeedToOvercall)
		{
			status << "A02! But with an openable " & numCardsInSuit[HEARTS] &
					  "-card Heart suit, prefer to open in the major instead of in No Trump.\n";
			goto escape1;
		}
		if (IsSuitOpenable(SPADES) && !bNeedToOvercall) 
		{
			status << "A04! But with an openable " & numCardsInSuit[SPADES] &
					  "-card Spade suit, prefer to open in the major instead of in No Trump.\n";
			goto escape1;
		}

		// check range for 1NT
		// NCR-285 Need stopper if opponents have bid!!!
		BOOL bHaveOppsStopped = TRUE;
		if ((nRHOBid != BID_PASS) && ISSUIT(nRHOSuit))
			bHaveOppsStopped = m_pHand->IsSuitStopped(nRHOSuit);  // NCR-285 consider if their suit is stopped
		else if ((nLHOBid != BID_PASS) && ISSUIT(nLHOSuit))
			bHaveOppsStopped = m_pHand->IsSuitStopped(nLHOSuit);  // NCR-285 ditto

		if ((fCardPts >= OPEN_PTS(NTMin[0])) && (fCardPts <= NTMax[0]) && bHaveOppsStopped) // NCR-285 added test
		{
			// NCR-18 Check if RHO bid NT and double 
			if(nRHOBid == BID_1NT){
				m_nBid = BID_DOUBLE;	// NCR-337 Problem here. This is not a Takeout double???
				status << "A07! This meet the rqmts for the 1NT opening range (" & NTMin[0] & "-" & NTMax[0] 
						& "), but RHO has bid 1NT, so Double.\n";
			}else {
				// open 1NT
				m_nBid = BID_1NT;
				status << "A08! This meet the rqmts for the 1NT opening range (" & NTMin[0] & "-" & NTMax[0] & "), so bid 1NT.\n";
			}
			return ValidateBid(m_nBid);
			
		} 
		else if ((fCardPts > OPEN_PTS(NTMax[0])) && (fCardPts < NTMin[1])) 
		{
			// in-between 1 & 2 situation; bid interim suit
			m_nNextIntendedBid = NIB_JUMP_NT;
			if (numOpenableSuits > 0) 
			{
				// bid 1 of lowest openable suit
				m_nBid = GetLowestOpenableBid(SUITS_ANY,OT_OPENER,1);
				// NCR-335 Don't bid 4 card major with 5CardMajor convention
				if (ISMAJOR(BID_SUIT(m_nBid)) && pCurrConvSet->IsConventionEnabled(tid5CardMajors)
					&& (numCardsInSuit[BID_SUIT(m_nBid)] < 5) ) 
				{
					// Use Diamonds if 4, else clubs (have balanced hand here)
					Suit bidThis = numCardsInSuit[DIAMONDS] >= 4 ? DIAMONDS : CLUBS;
					m_nBid = MAKEBID(bidThis, 1);  // NCR-335 use minor
                }
				status << "A10! This exceeds the max pts for a 1NT opening (" & NTMax[0] & 
								 "), but is less than min pts for 2NT (" & NTMin[1] & 
								 ").  So bid the lowest openable suit of " & BTS(m_nBid) & 
								 " for now, then try for a jump shift to 2NT later.\n";
			} 
			else 
			{
				// go ahead and bid the preferred suit
				if (!pCurrConvSet->IsConventionEnabled(tid5CardMajors) )
				{
					m_nBid = MAKEBID(nPrefSuit,1);
				} 
				else 
				{
					m_nBid = BID_1C;
				}
				status << "A14! This exceeds the max pts for a 1NT opening (" & NTMax[0] &
					      "), but is less than min pts for 2NT (" & NTMin[1] &
						  " and we have no good openable suit, so just bid " & BTS(m_nBid) &
						  " for now.\n";
			}
			return ValidateBid(m_nBid);

		} 
		// NCR can't use 2NT if using unusual NT convention
		else if ((fCardPts >= OPEN_PTS(NTMin[1])) && (fCardPts <= NTMax[1]) 
//			     && !pCurrConvSet->IsConventionEnabled(tidUnusualNT)  // NCR not relevant for Opening???
				 // NCR test that hand does NOT have a worthless doubleton
				 && ((m_pHand->GetNumDoubletons() == 0)  // OK if no doubletons
				     || (m_pHand->GetNumDoubletons() >= 1) 
				         && !m_pHand->HasWorthlessDoubleton()) //or if its not worthless
				) 
		{
			// open 2NT
			m_nBid = BID_2NT;
			status << "A20! This meets the rqmts for a 2NT opening (" & 
					  NTMin[1] & "-" & NTMax[1] & "), so bid 2NT.\n";
			return ValidateBid(m_nBid);

		} 
		else if ((fCardPts >= OPEN_PTS(NTMax[1])) && (NTMin[2] < 0))  // NCR ??? see next else if
		{

			// > pts for 2NT, but 3NT opening not allowed  NCR what does not allowed mean???
			// so bid 2C
			m_nBid = BID_2C;
			status << "A30! This exceeds the max pts for a 2NT opening (" & NTMax[1] & 
					   "), but a 3NT opening has been disallowed, so open with " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);

		} 
		else if ((fCardPts > OPEN_PTS(NTMax[1])) && (fCardPts < NTMin[2])) 
		{

			// in-between 2 & 3 situation; bid 2 of interim suit
			// and bid NT later
			m_nNextIntendedBid = NIB_NT;
			if (numOpenableSuits > 0) 
			{
				// bid 1 of lowest openable suit
				m_nBid = GetLowestOpenableBid(SUITS_ANY,OT_OPENER,1);
				status << "A40! This exceeds the max pts for a 2NT opening (" &
						  NTMax[1] & "), but is less than min pts for 3NT (" &
						  NTMin[2] & ").  So bid the lowest openable suit of " &
						  BTS(m_nBid) & " for now, then jump shift to NT later.\n";
			} 
			else 
			{
				// no good openable suits; just bid preferred suit
				m_nBid = MAKEBID(nPrefSuit,1);;
				status << "A42! This exceeds the max pts for a 2NT opening (" &
						  NTMax[1] & "), but is less than min pts for 3NT (" &
						  NTMin[2] & "), and have no good openable suit, so bid " &
						  BTS(m_nBid) & " for now.\n";
			}
			return ValidateBid(m_nBid);

		} 
		else if ((fCardPts >= OPEN_PTS(NTMin[2])) && (fCardPts <= NTMax[2])
				 // NCR test that hand does NOT have a worthless doubleton
				 && ((m_pHand->GetNumDoubletons() == 0)  // OK if no doubletons
				     || (m_pHand->GetNumDoubletons() >= 1) 
				         && !m_pHand->HasWorthlessDoubleton()) //or if its not worthless

			     ) 
		{
			// open 3NT
			m_nBid = BID_3NT;
			status << "A50! Bid 3NT.\n";
			return ValidateBid(m_nBid);
		} 
/*  NCR remove falling into a 3NT bid
		else // if(!pCurrConvSet->IsConventionEnabled(tidUnusualNT)) // NCR added Unusual test??? 
		{
			// > 3NT point range? unusual, but open 3NT for now
			m_nBid = BID_3NT;
			status << "A60! bid 3NT with balanced hand and " & fCardPts & " points.\n"; // NCR
			return ValidateBid(m_nBid);
		}
*/
	}  // end balanced hand with > min pts

escape1:


		



	//
	//-------------------------------------------------------------------
	//
	// See if we can open at the 1-level
	//
	// open at the 1-level if we have:
	//  1:  14+ points in high cards
	//  2:  13+ HCPs with 2 Quick Tricks
	//  3:  12+ HCPs with 2 QTs and a rebiddable suit
	//  4:  12+ HCPs with 2 QTs and a 5-card suit
	//  5:  12+ HCPs with balanced dist (weak NT mode only)
	//  6:  10+ HCPs opening in 3rd or 4th seat and a good suit  NCR ???
	//  7:  15+ Total points with 2 QTs
	//  8:  14+ Total points with 2 QTs and a rebiddable suit
	//  9:  13+ Total points with 2 QTs and a rebiddable suit of 6+ cards
	//
	
	// we can only open if the opponents haven't opened yet
	// if they have, the COvercallsCovention object should have looked into
	// a posssible overcall.
	if ((nLHOBid > BID_PASS) || (nRHOBid > BID_PASS))
	{
		status << "B00! We cannot overcall and the hand is not appropriate for any other bid, so pass.\n";
		m_nBid = BID_PASS;
		return ValidateBid(m_nBid);
	}

	// the suit to open us usually the best suit
	nSuit = GetBestOpeningSuit();

	// case 1:  14+ points in high cards
	if (fCardPts >= OPEN_PTS(14)) 
	{
		m_nBid = MAKEBID(nSuit,1);
		status << "E01! Have " & fCardPts & " points in high cards, so bid " & BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);
	}
	// case 2:  13+ HCP's && 2 QT's
	if ((fCardPts >= OPEN_PTS(13)) && (numQuickTricks >= 2)) 
	{
		m_nBid = MAKEBID(nSuit,1);
		status << "E02! Have " & fCardPts & " points in high cards and " & numQuickTricks & 
				  " quick tricks, so bid " & BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);;
	}
	// case 3:  12+ HCP's, 2 QT's, & a rebiddable suit
	// NCR-680  reduce QTs
	if ((fCardPts >= OPEN_PTS(12)) && (numQuickTricks >= PT_COUNT(2)) && 
								(numRebiddableSuits > 0)) 
	{
		m_nBid = MAKEBID(nSuit,1);
		status << "E03! Have " & fCardPts & " points in high cards, " & numQuickTricks &
				  " quick tricks, and a rebiddable suit in " & STS(nSuit) & 
				  ", so bid " & BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);;
	}
	// case 4:  12+ HCP's, 2 QT's, & a 5-card suit
	if ((fCardPts >= OPEN_PTS(12)) && (numQuickTricks >= 2) && 
							(m_pHand->GetNumSuitsOfAtLeast(5) > 0)) 
	{
		m_nBid = MAKEBID(nSuit,1);
		status << "E04! Have " & fCardPts & " points in high cards, " & numQuickTricks & 
				  " quick tricks, and a " & numCardsInSuit[nSuit] & 
				  "-card suit, so bid " & BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);
	}
	// case 5:  12+ HCP's & a balanced dist in Weak NTs
	if ((fCardPts >= OPEN_PTS(12)) && (bBalanced) && 
				(pCurrConvSet->GetValue(tn1NTRange) == 0)) 
	{
		m_nBid = BID_1NT;
		status << "E05! Have " & fCardPts & " points in high cards, a balanced hand, " &
				  "and playing Weak No Trumps, so bid " & BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);
	}
	// case 6:  10+ HCPs opening in 3rd or 4th seat & a good suit
//	if ((fCardPts >= OPEN_PTS(10)) 
	if ((fCardPts >= 10)  // NCR-206 Hard 10  and > 4 cards  
		 && (((nBiddingOrder == 2) && (numCardsInSuit[nSuit] > 4)) 
		                              // NCR require Having spades to open light in 4th
		     || ((nBiddingOrder == 3) & (numCardsInSuit[SPADES] > 3))
		                              && (nSuit == SPADES))
		 &&	(numOpenableSuits > 0)) 
	{
		m_nBid = MAKEBID(nSuit,1);
		status << "E07! Have " & fCardPts & " points in high cards in " 
				  & ((nBiddingOrder == 2)? "3rd" :  "4th") &
				  " position and a " & SSTS(nSuit) & " " & STSS(nSuit) & 
				  " suit, so bid " & BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);
	}
	// case 7:  15+ Total points and 10 HCPs, with 2 QTs
	if ((fPts >= OPEN_PTS(15)) && (fCardPts >= OPEN_PTS(10)) && (numQuickTricks >= 2)) 
	{
		m_nBid = MAKEBID(nSuit,1);
		status << "E08! Have " & fCardPts & "/" & fPts & " total points with " 
				  & numQuickTricks & " QT's, so bid " & BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);
	}
	//
	// case 8:  14+ Total points with 10+ HCPs, 2 QTs 
	// and a rebiddable suit
	if ((fPts >= OPEN_PTS(14)) && (fCardPts >= OPEN_PTS(10)) && 
		(numQuickTricks >= 2) && (numRebiddableSuits > 0)) 
	{
		m_nBid = MAKEBID(nSuit,1);
		status << "E09! Have " & fCardPts & "/" & fPts & " total points with "
				  & numQuickTricks & " QT's and a rebiddable suit, so bid "
				  & BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);;
	}
	//
	// case 9:  13+ Total points with 10+ HCPs, 2 QTs and a 
	// rebiddable suit of 6+ cards
	if ((fPts >= OPEN_PTS(13)) && (fCardPts >= OPEN_PTS(10)) && 
		(numQuickTricks >= 2) && (numRebiddableSuits > 0) && 
		(numPrefSuitCards >= 6) &&
		(nSuitStrength[nPrefSuit] > SS_OPENABLE)) 
	{
		m_nBid = MAKEBID(nSuit,1);
		status << "E10! Have " & fCardPts & "/" & fPts & " total points with "
				  & numQuickTricks & " QT's and a " & numCardsInSuit[nSuit] & 
				  "-card rebiddable suit, so bid " & BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);
	}



	//
	//---------------------------------------------------------
	//
	// now test for optional 1-level openings
	//
	int nAllowed1Opens = pCurrConvSet->GetValue(tnAllowable1Openings);

	//
	// option 1:  11+ HCPs with 2 QTs, a rebiddable suit, &
	// length in both majors
	//
	if ((nAllowed1Opens & OB_11_HCPS_RBS_LM) && (fCardPts >= OPEN_PTS(11)) && 
		(numQuickTricks >= 2) && (numRebiddableSuits > 0) &&
		(numCardsInSuit[HEARTS] >= 4) && (numCardsInSuit[SPADES] >= 4)) 
	{
		m_nBid = MAKEBID(nSuit,1);
		status << "E21! Have " & fCardPts & " points in high cards, " & numQuickTricks & 
				   " quick tricks, a rebiddable suit, and length in both majors " &
				   "(optional opening condition #1), so bid " & BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);
	}
	//
	// option 2:  11+ HCPs with 2 QTs and a 6-card suit
	//
	if ((nAllowed1Opens & OB_11_HCPS_6CS) && (fCardPts >= OPEN_PTS(11)) && 
				(numQuickTricks >= 2) && 
				(m_pHand->GetNumSuitsOfAtLeast(6) > 0)) 
	{
		m_nBid = MAKEBID(nSuit,1);
		status << "E22! Have " & fCardPts & " points in high cards with " & numQuickTricks & 
			      " QT's and a " & numCardsInSuit[nSuit] & 
				  "-card suit (optional opening condition #2), so bid " & BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);;
	}
	//
	// option 3:  14+ Total points with 10 HCPs, 2 QTs, and a 
	// good suit
	//
	if ((nAllowed1Opens & OB_14_TCPS_GS) && 
				(fPts >= OPEN_PTS(14)) && (fCardPts >= OPEN_PTS(10)) && 
				(numQuickTricks >= 2) && (numSolidSuits > 0)) 
	{
		m_nBid = MAKEBID(nSuit,1);
		status << "E23! Have " & fCardPts & "/" & fPts & "total points with " & numQuickTricks &
				  " QT's and a " & SSTS(nSuit) & " " & STSS(nSuit) & 
				  " suit (optional opening condition #3), so bid " & BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);;
	}
	//
	// option 4:  14+ Total points with 10 HCPs, 2 QTs, and a 
	// long suit (6+ cards)
	//
	if ((nAllowed1Opens & OB_14_TCPS_LS) && 
		(fPts >= OPEN_PTS(14)) && (fCardPts >= OPEN_PTS(10)) && 
		(numQuickTricks >= 2) && 
		(m_pHand->GetNumSuitsOfAtLeast(6) > 0)) 
	{
		m_nBid = MAKEBID(nSuit,1);
		status << "E24! Have " & fCardPts & "/" & fPts & " total points with " & numQuickTricks &
				  " QT's and a " & numCardsInSuit[nSuit] &"-card " & STSS(nSuit) & 
				  " suit (optional opening condition #4), so bid " & BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);;
	}
	//
	// option 5:  13+ Total points with 2 QTs, 10 HCPs,
	// and a 6-card suit
	//
	if ((nAllowed1Opens & OB_13_TCPS_LS) && 
		      // NCR-390 Add in distribution points here and below in status msg
		((fPts + fDistPts) >= OPEN_PTS(13)) && (fCardPts >= OPEN_PTS(10)) && 
		(numQuickTricks >= 2) && 
		(m_pHand->GetNumSuitsOfAtLeast(6) > 0)) 
	{
		m_nBid = MAKEBID(nSuit,1);
		status << "E25! Have " & fCardPts & "/" & (fPts+fDistPts) &" total points with " & numQuickTricks &
				  " QT's and a " & numCardsInSuit[nSuit] & "-card " & STSS(nSuit) & 
				  " suit (optional opening condition #5), so bid " & BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);
	}

/*
	//
	// special case:  13 pts + 1 for every 0.5 QTs missing
	if ((fPts > OPEN_PTS(13)) && (numQuickTricks < 2)) 
	{
		int nRqmt = 13 + (int)(((2.0 - numQuickTricks)*2));
		if (fPts >= nRqmt) 
		{
			m_nBid = MAKEBID(nSuit,1);
			status << "E27! Have " & fCardPts & "/" & fPts & 
					  " total points; shade opening requirement of 2 QT's and make do with " & numQuickTricks &
					  " QT's in light of the high total point count; open " & BTS(m_nBid) & ".\n";
			return ValidateBid(m_nBid);;
		}
	}
*/
	// NCR-356 Loosen up a bit here - bid 5 card major if ...
	if((fCardPts >= OPEN_PTS(13)) &&  ISMAJOR(nSuit) && (numCardsInSuit[nSuit] >= 5)
		&& pCurrConvSet->IsConventionEnabled(tid5CardMajors) )
	{
		m_nBid = MAKEBID(nSuit,1);
		status << "E28! Have " & fCardPts & "/" & fPts & 
				  " total points; shade opening requirement of 2 QT's and make do with " & numQuickTricks &
				  " QT's; open " & BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);;
	} // end NCR-356


	// NCR Is Convenient minor (goes with 5card Majors) appropriate - 13 HCPs and 3 clubs
	if((fCardPts >= OPEN_PTS(13)) && pCurrConvSet->IsConventionEnabled(tid5CardMajors)
		&& (numCardsInSuit[CLUBS] >= 3)) 
	{
		m_nBid = BID_1C;
		if(numCardsInSuit[DIAMONDS] > numCardsInSuit[CLUBS]) // NCR-507 Bid Diamonds if more of them
			m_nBid = BID_1D;
		status << "E31! Have " & fCardPts & " points in high cards , so bid a Convenient minor "
			        & BTS(m_nBid) & ".\n";
		return ValidateBid(m_nBid);
	}

	//
	//--------------------------------------------------------
	//
	// All attempts at opening failed, so return a PASS
	//
	status << "Z00! Hand is insufficient for any opening, so pass.\n";
	m_nBid = BID_PASS;
	return ValidateBid(m_nBid);;				

}



