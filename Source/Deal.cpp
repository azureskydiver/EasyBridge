//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// EasyBdoc.cpp : implementation of the CEasyBDoc class
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "EasyBvw.h"
#include "Player.h"
#include "mainfrm.h"
#include "progopts.h"
#include "deck.h"
#include "Card.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const int MAX_SPECIAL_DEAL_ITERATIONS	= 200;


//
// DealSpecial()
//
// Possible codes:
// ---------------
// Game Code: 0 = Any (Random)
// 			  1 = Game
// 			  2 = Slam
// Suit Code: 0 = Any
// 			  1 = Minor
// 			  2 = Major
// 			  3 = NoTrump
// Slam Code: 0 = Any
//			  1 = Small Slam
//			  2 = Grand Slam
//
void CEasyBDoc::DealSpecial(int nGameCode, int nSuitCode, int nSlamCode, int nTeam, int nDealNumber) 
{
	CString strFeedback;

	//
	CMainFrame::SetStatusMessage("Performing deal...");

	// validate inputs
	if ((nGameCode < 0 ) || (nGameCode > 2) ||
				(nSuitCode < 0 ) || (nSuitCode > 3) ||
				(nSlamCode < 0 ) || (nSlamCode > 2) )
		return;

	// first clear all hands
	InitNewHand();

	// delete document info
	if (!m_bReviewingGame)
		DeleteContents();

	// save special deal code
	m_nSpecialDealCode = (nGameCode << 6) | (nSuitCode << 4) | (nSlamCode << 2) | nTeam;

	// perform the initial shuffle
	m_nDealNumber = deck.Shuffle(nDealNumber);

	// set busy cursor
	pVIEW->SetCurrentMode(CEasyBView::MODE_WAITSPECIALDEAL);

	// first set target points
	int nMin, nMax;
	double fScoreTarget;
	//
	switch (nGameCode) 
	{

		case 0:
			// any random deal
			break;

		case 1:
			// game deal
			nMin = theApp.GetValue(tnRequiredPointsForGame,nSuitCode,0);
			nMax = theApp.GetValue(tnRequiredPointsForGame,nSuitCode,1);
			fScoreTarget = nMin + GetRandomValue(nMax - nMin);
			switch(nSuitCode) 
			{
				case 0:
					// game hand, any suit
					strFeedback.Format("Dealt game hand\n");
					break;
				case 1:
					// game hand, minor suit
					strFeedback.Format("Dealt Minor game hand\n");
					break;
				case 2:
					// game hand, major suit
					strFeedback.Format("Dealt Major game hand\n");
					break;
				case 3:
					// game hand, notrumps
					strFeedback.Format("Dealt NoTrump game hand\n");
					break;
			}
			break;

		case 2:
			// slam deal
			nMin = theApp.GetValue(tnRequiredPointsForSlam,nSlamCode,0);
			nMax = theApp.GetValue(tnRequiredPointsForSlam,nSlamCode,1);
			fScoreTarget = nMin + GetRandomValue(nMax - nMin);
			switch(nSlamCode) 
			{
				case 0:
					// any slam
					strFeedback.Format("Dealt slam hand\n");
					break;
				case 1: 
					// small slam
					strFeedback.Format("Dealt small slam hand\n");
					break;
				case 2: 
					// grand slam
					strFeedback.Format("Dealt grand slam hand\n");
					break;
			}
			break;
	}

	// declare loop counters
	int i;
	int nOuterLoopCount = 0;
	int nAceLoopCount1, nAceLoopCount2;
	int nKingLoopCount1, nKingLoopCount2;
	int nPointSwapLoopCount, nFailCount;

shuffle:
	nOuterLoopCount++;
	// check if we've exceeded the limit on iterations
	if (nOuterLoopCount >= MAX_SPECIAL_DEAL_ITERATIONS) 
	{
		AfxMessageBox("Failed to meet hand requirements.  Try again.");
		pVIEW->ClearMode();
		return;
	}

/*
	// every 100 count, reseed the random # generator
	if (((nOuterLoopCount % 100) == 0) && (nOuterLoopCount > 0))
		srand((unsigned)time(NULL));
*/

	// assign new hands
	DealCards();


	//
	//----------------------------------------------------------
	//
	// Now check the strength of N/S's hands
	//
	double fSouthPoints, fNorthPoints;
	if (nSuitCode != 3) 
	{
		fSouthPoints = PLAYER(SOUTH).GetTotalPoints();
		fNorthPoints = PLAYER(NORTH).GetTotalPoints();
	} 
	else 
	{
		// No Trumps; count high card points only
		fSouthPoints = PLAYER(SOUTH).GetHCPoints();
		fNorthPoints = PLAYER(NORTH).GetHCPoints();
	}
	double fTotalPoints = fSouthPoints + fNorthPoints;
	double fDiff, fSwapped;
	int nSource,nDest;

	// at this point, check cards' distribution
	int numInSuit[4];
	BOOL bSuitFit[4];
	for(i=0;i<4;i++) 
	{
		numInSuit[i] = PLAYER(SOUTH).GetNumCardsInSuit(i) +
						PLAYER(NORTH).GetNumCardsInSuit(i);
		// check for a proper card distribution (min 4/3)
		int nDistIndex = theApp.GetValue(tnMinSuitDistributions,nSuitCode-1);
		int nDistVal[2];
		nDistVal[0] = theApp.GetValue(tnMinSuitDistributions,nSuitCode-1,nDistIndex,0);
		nDistVal[1] = theApp.GetValue(tnMinSuitDistributions,nSuitCode-1,nDistIndex,1);
		if ((nSuitCode == 1) || (nSuitCode == 2)) 
		{
			if ( ((PLAYER(SOUTH).GetNumCardsInSuit(i) >= nDistVal[0]) &&
						  (PLAYER(NORTH).GetNumCardsInSuit(i) >= nDistVal[1])) ||
			     ((PLAYER(NORTH).GetNumCardsInSuit(i) >= nDistVal[0]) &&
						  (PLAYER(SOUTH).GetNumCardsInSuit(i) >= nDistVal[1])) )
				bSuitFit[i] = TRUE;
			else
				bSuitFit[i] = FALSE;
		}
		else 
		{
			// generic suit or no trumps specified; default to
			// 4/3 for fit determination
			if ( ((PLAYER(SOUTH).GetNumCardsInSuit(i) >= 3) &&
				  (PLAYER(NORTH).GetNumCardsInSuit(i) >= 4)) ||
			     ((PLAYER(NORTH).GetNumCardsInSuit(i) >= 3) &&
				  (PLAYER(SOUTH).GetNumCardsInSuit(i) >= 4)) )
				bSuitFit[i] = TRUE;
			else
				bSuitFit[i] = FALSE;
		}
	}

	//
	if (nSuitCode == 3) 
	{

		// No Trump contract -- check hand balance
		int nMaxImbalance = theApp.GetValue(tnMaxImbalanceForNT);
		if (theApp.GetValue(tbNeedTwoBalancedTrumpHands)) 
		{
			// both players need balanced hands
			if ((PLAYER(SOUTH).GetBalanceValue() > nMaxImbalance) ||
					(PLAYER(NORTH).GetBalanceValue() > nMaxImbalance))
				goto shuffle;
		} 
		else 
		{
			// at least one player has balanced hand
			if ((PLAYER(SOUTH).GetBalanceValue() > nMaxImbalance) && 
					(PLAYER(NORTH).GetBalanceValue() > nMaxImbalance))
				goto shuffle;
		}

	} 
	else if (nSuitCode == 2) 
	{

		// major suit game -- need at least one major suit
		// that has >= min cards
		int nCardsInMajor = theApp.GetValue(tnMinCardsInMajor);	
		if ((numInSuit[HEARTS] < nCardsInMajor) &&
					(numInSuit[SPADES] < nCardsInMajor))
			goto shuffle;
		// also need a minimum 4-3 fit in a major suit
		if ((!bSuitFit[HEARTS]) && (!bSuitFit[SPADES]))
			goto shuffle;
		// and also check that the suit is adequately topped
		BOOL bTopped = FALSE;
		int nMinTopMajorCard = theApp.GetValue(tnMinTopMajorCard);
		if ((bSuitFit[HEARTS]) && (PLAYER(SOUTH).GetNumCardsInSuit(HEARTS) >= 4) &&
			(PLAYER(SOUTH).GetCardInSuit(HEARTS,0)->GetFaceValue() >= nMinTopMajorCard))
			bTopped = TRUE;
		if ((bSuitFit[SPADES]) && (PLAYER(SOUTH).GetNumCardsInSuit(SPADES) >= 4) &&
			(PLAYER(SOUTH).GetCardInSuit(SPADES,0)->GetFaceValue() >= nMinTopMajorCard))
			bTopped = TRUE;
		if ((bSuitFit[HEARTS]) && (PLAYER(NORTH).GetNumCardsInSuit(HEARTS) >= 4) &&
			(PLAYER(NORTH).GetCardInSuit(HEARTS,0)->GetFaceValue() >= nMinTopMajorCard))
			bTopped = TRUE;
		if ((bSuitFit[SPADES]) && (PLAYER(NORTH).GetNumCardsInSuit(SPADES) >= 4) &&
			(PLAYER(NORTH).GetCardInSuit(SPADES,0)->GetFaceValue() >= nMinTopMajorCard))
			bTopped = TRUE;
		if (!bTopped)
			goto shuffle;

	} 
	else if (nSuitCode == 1) 
	{

		// minor suit game -- need at least one major suit
		// that has >= min cards
		int nCardsInMinor = theApp.GetValue(tnMinCardsInMinor);	
		if ((numInSuit[CLUBS] < nCardsInMinor) &&
					(numInSuit[DIAMONDS] < nCardsInMinor))
			goto shuffle;
		// also need a minimum 4-3 fit in a minor suit
		if ((!bSuitFit[CLUBS]) && (!bSuitFit[DIAMONDS]))
			goto shuffle;
		// and also check that the suit is adequately topped
		BOOL bTopped = FALSE;
		int nMinTopMinorCard = theApp.GetValue(tnMinTopMinorCard);
		if ((bSuitFit[CLUBS]) && (PLAYER(SOUTH).GetNumCardsInSuit(CLUBS) >= 4) &&
			(PLAYER(SOUTH).GetCardInSuit(CLUBS,0)->GetFaceValue() >= nMinTopMinorCard))
			bTopped = TRUE;
		if ((bSuitFit[DIAMONDS]) && (PLAYER(SOUTH).GetNumCardsInSuit(DIAMONDS) >= 4) &&
			(PLAYER(SOUTH).GetCardInSuit(DIAMONDS,0)->GetFaceValue() >= nMinTopMinorCard))
			bTopped = TRUE;
		if ((bSuitFit[CLUBS]) && (PLAYER(NORTH).GetNumCardsInSuit(CLUBS) >= 4) &&
			(PLAYER(NORTH).GetCardInSuit(CLUBS,0)->GetFaceValue() >= nMinTopMinorCard))
			bTopped = TRUE;
		if ((bSuitFit[DIAMONDS]) && (PLAYER(NORTH).GetNumCardsInSuit(DIAMONDS) >= 4) &&
			(PLAYER(NORTH).GetCardInSuit(DIAMONDS,0)->GetFaceValue() >= nMinTopMinorCard))
			bTopped = TRUE;
		if (!bTopped)
			goto shuffle;

	}

	//
	//----------------------------------------------------------
	//
	// Fudging time --
	//
	// for a grand slam hand, check to make sure there are some 
	// non-ace, non-king honors in the N/S suits.  This will make
	// it easier to trade down points later on
	if ((nGameCode == 2) && (nSlamCode == 2)) 
	{
		double fDiff1 = m_pPlayer[SOUTH]->GetHCPoints() - (m_pPlayer[SOUTH]->GetNumCardsOf(ACE)*4 + m_pPlayer[SOUTH]->GetNumCardsOf(KING)*3);
		double fDiff2 = m_pPlayer[NORTH]->GetHCPoints() - (m_pPlayer[NORTH]->GetNumCardsOf(ACE)*4 + m_pPlayer[NORTH]->GetNumCardsOf(KING)*3);
		// #### TEMP ####
		double fHC1 = m_pPlayer[SOUTH]->GetHCPoints();
		int nAces1 = m_pPlayer[SOUTH]->GetNumCardsOf(ACE);
		int nKings1 = m_pPlayer[SOUTH]->GetNumCardsOf(KING);
		double fHC2 = m_pPlayer[NORTH]->GetHCPoints();
		int nAces2 = m_pPlayer[NORTH]->GetNumCardsOf(ACE);
		int nKings2 = m_pPlayer[NORTH]->GetNumCardsOf(KING);
		ASSERT((fDiff1 >= 0) && (fDiff2 >= 0));
		//
		if ((fDiff1 < 2) || (fDiff2 < 2))
			goto shuffle;
	}

	//
	//----------------------------------------------------------
	//
	// slam hand -- check for aces if necessary
	//
	int numAcesRequired;
	int numAcesHeld;
	if (nGameCode == 2) 
	{
		numAcesRequired = theApp.GetValue(tnumAcesForSlam,nSlamCode);
		numAcesHeld = PLAYER(SOUTH).GetNumCardsOf(ACE) + PLAYER(NORTH).GetNumCardsOf(ACE);
		int numCards,nSrcPlayer,nDestPlayer,nSuit1,nSuit2,nDestCard;
		int fDiff = numAcesRequired - numAcesHeld;
		// trade aces with opponents
		if (fDiff > 0) 
		{
			for(i=0;i<fDiff;i++) 
			{
				// first pick a source opponent, semi-randomly
				// 0 or 1; 0 means west, unless west has zero aces
				if ( ((GetRandomValue(1) == 0) &&
					  		(PLAYER(WEST).GetNumCardsOf(ACE) > 0)) ||
					 (PLAYER(EAST).GetNumCardsOf(ACE) == 0) )
					nSrcPlayer = WEST;
				else
					nSrcPlayer = EAST;
				ASSERT(PLAYER(nSrcPlayer).GetNumCardsOf(ACE) != 0);
				// and likewise pick a dest player; 0=South
				int nVal = GetRandomValue(1);
				if ( ((nVal == 0) && (PLAYER(SOUTH).GetNumCardsOf(ACE) < 4)) ||
					 (PLAYER(NORTH).GetNumCardsOf(ACE) == 4) )
					nDestPlayer = SOUTH;
				else
					nDestPlayer = NORTH;
				// now pick a source suit and a dest suit
				nAceLoopCount1 = 0;
				do 
				{
					// search for a source suit with an ace
					nSuit1 = GetRandomValue(3);
					if ((PLAYER(nSrcPlayer).GetNumCardsInSuit(nSuit1) > 0) &&
						(PLAYER(nSrcPlayer).GetCardInSuit(nSuit1,0)->GetFaceValue() == ACE)) {
						break;
					}
					nAceLoopCount1++;
				} 
				while (nAceLoopCount1 < 100);
				if (nAceLoopCount1 >= 100) 
				{
					AfxMessageBox("Failed to meet deal constraints (Ace search stage 1 failure).");
					break;
				}
				//
				nAceLoopCount2 = 0;
				do 
				{
					nSuit2 = GetRandomValue(3);
					// make sure the dest suit has > 1 cards in it,
					// or if it has only one card, that it's not an ace
					if ((PLAYER(nDestPlayer).GetNumCardsInSuit(nSuit2) > 1) ||
					    ((PLAYER(nDestPlayer).GetNumCardsInSuit(nSuit2) == 1) &&
						 (PLAYER(nDestPlayer).GetCardInSuit(nSuit2,0)->GetFaceValue() != ACE)) ) {
						break;
					}
					nAceLoopCount2++;
				} while (nAceLoopCount2 < 100);
				if (nAceLoopCount2 >= 100) 
				{
					AfxMessageBox("Failed to meet deal constraints (Ace search stage 2 failure).");
					break;
				}
				// and then pick a nonace card from the dest suit
				numCards = PLAYER(nDestPlayer).GetNumCardsInSuit(nSuit2);
				do 
				{
					nDestCard = GetRandomValue(numCards-1);
				} while (PLAYER(nDestPlayer).GetCardInSuit(nSuit2,nDestCard)->GetFaceValue() == ACE);
				// and finally, then swap cards
				SwapPlayersCards(nSrcPlayer,nDestPlayer, nSuit1, nSuit2, 0, nDestCard, TRUE);
			}
		}		
	}

	//
	//----------------------------------------------------------
	//
	// likewise, check for kings if necessary
	//
	int numKingsRequired;
	int numKingsHeld;
	if (nGameCode == 2) 
	{
		numKingsRequired = theApp.GetValue(tnumKingsForSlam,nSlamCode);
		numKingsHeld = PLAYER(SOUTH).GetNumCardsOf(KING) + PLAYER(NORTH).GetNumCardsOf(KING);
		int numCards,nSrcPlayer,nDestPlayer,nSuit1,nSuit2,nDestCard,nSrcCard;
		int fDiff = numKingsRequired - numKingsHeld;

		// trade kings with opponents
		if (fDiff > 0) 
		{
			for(i=0;i<fDiff;i++) 
			{
				// first pick a source opponent, semi-randomly
				// 0 or 1; 0 means west, unless west has zero kings
				if ( ((GetRandomValue(1) == 0) &&
					  		(PLAYER(WEST).GetNumCardsOf(KING) > 0)) ||
					 (PLAYER(EAST).GetNumCardsOf(KING) == 0) )
					nSrcPlayer = WEST;
				else
					nSrcPlayer = EAST;
				// and likewise pick a dest player; 0=South
				if ( ((GetRandomValue(1) == 0) &&
					  		(PLAYER(SOUTH).GetNumCardsOf(KING) < 4)) ||
					 (PLAYER(NORTH).GetNumCardsOf(KING) == 4) )
					nDestPlayer = SOUTH;
				else
					nDestPlayer = NORTH;
				// now pick a source suit and a dest suit
				nKingLoopCount1 = 0;
				do 
				{
					// search for a source suit with a king
					nSuit1 = GetRandomValue(3);
					if ((PLAYER(nSrcPlayer).GetNumCardsInSuit(nSuit1) >= 1) &&
 						(PLAYER(nSrcPlayer).GetCardInSuit(nSuit1,0)->GetFaceValue() == KING)) 
					{
						nSrcCard = 0;
						break;
					}
					if ((PLAYER(nSrcPlayer).GetNumCardsInSuit(nSuit1) >= 2) &&
					    (PLAYER(nSrcPlayer).GetCardInSuit(nSuit1,1)->GetFaceValue() == KING)) 
					{
						nSrcCard = 1;
						break;
					}
					nKingLoopCount1++;
				} 
				while (nKingLoopCount1 < 100);
				if (nKingLoopCount1 >= 100) 
				{
					AfxMessageBox("Failed to meet deal constraints (King search stage 1 failure).");
					break;
				}
				//
				nKingLoopCount2 = 0;
				do 
				{
					nSuit2 = GetRandomValue(3);
					// make sure the dest suit has > 1 cards in it, including a 
					// card lower than a king which can be swapped out 
					int nNum = PLAYER(nDestPlayer).GetNumCardsInSuit(nSuit2);
					if ((nNum >= 1) &&
						(PLAYER(nDestPlayer).GetCardInSuit(nSuit2,nNum-1)->GetFaceValue() < KING)) {
						break;
					}
					nKingLoopCount2++;
				} while (nKingLoopCount2 < 100);
				if (nKingLoopCount2 >= 100) 
				{
					AfxMessageBox("Failed to meet deal constraints (King search stage 2 failure).");
					break;
				}
				// and then pick a non-king, non-ace card from the dest suit
				numCards = PLAYER(nDestPlayer).GetNumCardsInSuit(nSuit2);
				do 
				{
					nDestCard = GetRandomValue(numCards-1);
				} while ((PLAYER(nDestPlayer).GetCardInSuit(nSuit2,nDestCard)->GetFaceValue() == KING) ||
						 (PLAYER(nDestPlayer).GetCardInSuit(nSuit2,nDestCard)->GetFaceValue() == ACE));
				// and finally, then swap cards
				SwapPlayersCards(nSrcPlayer,nDestPlayer,nSuit1,nSuit2,nSrcCard,nDestCard,TRUE);
			}
		}	
	}

	//
	//----------------------------------------------------------
	//
	// now, adjust hand points if necessary
	// note that we must maintain card topping requirements
	//
	fDiff = fTotalPoints - fScoreTarget;
//	srand((unsigned)time(NULL));
	BOOL bSouthHoldsExtraHonors,bNorthHoldsExtraHonors;
	nPointSwapLoopCount = 0;
	nFailCount = 0;
	//
	while (fDiff != 0) 
	{
		// check some hand parameters to ensure success of swap routine
		// see if N/S have high cards available to swap down
		int nSouthAces = m_pPlayer[SOUTH]->GetNumCardsOf(ACE);
		int nSouthKings = m_pPlayer[SOUTH]->GetNumCardsOf(KING);
		if ((m_pPlayer[SOUTH]->GetHCPoints() > nSouthAces*4 + nSouthKings*3) ||
			((nSouthAces > 0) && (numAcesHeld > numAcesRequired)) ||
			((nSouthKings > 0) && (numKingsHeld > numKingsRequired))	)
			bSouthHoldsExtraHonors = TRUE;
		else
			bSouthHoldsExtraHonors = FALSE;
		int nNorthAces = m_pPlayer[NORTH]->GetNumCardsOf(ACE);
		int nNorthKings = m_pPlayer[NORTH]->GetNumCardsOf(KING);
		if ((m_pPlayer[NORTH]->GetHCPoints() > nNorthAces*4 + nNorthKings*3) ||
			((nNorthAces > 0) && (numAcesHeld > numAcesRequired)) ||
			((nNorthKings > 0) && (numKingsHeld > numKingsRequired)) )
			bNorthHoldsExtraHonors = TRUE;
		else
			bNorthHoldsExtraHonors = FALSE;
		//
 		nDest = (GetRandomValue(1)==0)? EAST: WEST;
		if (fDiff < 0) 
		{
			// adjust hand upwards
			if ((theApp.GetValue(tbBalanceTeamHands)) &&
				((fSouthPoints >= 13) || (fNorthPoints >= 13)) ) 
			{
				if (fSouthPoints < fNorthPoints)
					nSource = SOUTH;
				else
					nSource = NORTH;
			} 
			else 
			{
				nSource = (GetRandomValue(1)==0)? NORTH: SOUTH;
			}
			fSwapped = SwapPoints(nSource, nDest, Abs(fDiff), nGameCode, nSuitCode, nSlamCode);
		} 
		else 
		{
			// adjust hand down
			if ((numAcesHeld <= numAcesRequired) &&
				(numKingsHeld <= numKingsRequired) &&
				(!bSouthHoldsExtraHonors) && (!bNorthHoldsExtraHonors)) 
			{
				// neither hand has a high card to trade down while
				// meeting Aces/Kings requirement, just give up
				break;
			}
			// else proceed
			if ((theApp.GetValue(tbBalanceTeamHands))  &&
				((fSouthPoints >= 13) || (fNorthPoints >= 13)) ) 
			{
				// deduct points from hand with the most points, unless that hand
				// lacks honors below a king (which must be held)
				if ( ((fSouthPoints > fNorthPoints) && (bSouthHoldsExtraHonors)) ||
					 ((bSouthHoldsExtraHonors) && (!bNorthHoldsExtraHonors)) )
					nSource = SOUTH;
				else
					nSource = NORTH;
			} 
			else 
			{
				nSource = (GetRandomValue(1)==0)? NORTH: SOUTH;
			}
			fSwapped = SwapPoints(nDest, nSource, Abs(fDiff), nGameCode, nSuitCode, nSlamCode);
		}
		//
		if (fSwapped == 0) 
		{
			nFailCount++;
			if (nFailCount > 40)
				break;	// could've run out of high cards
		}
		// and re-evaluate
		PLAYER(SOUTH).CountCardPoints(TRUE);
		PLAYER(NORTH).CountCardPoints(TRUE);
		if (nSuitCode != 3) 
		{
			fSouthPoints = PLAYER(SOUTH).GetTotalPoints();
			fNorthPoints = PLAYER(NORTH).GetTotalPoints();
		} 
		else 
		{
			// No Trumps; count high card points only
			fSouthPoints = PLAYER(SOUTH).GetHCPoints();
			fNorthPoints = PLAYER(NORTH).GetHCPoints();
		}
		fTotalPoints = fSouthPoints + fNorthPoints;
		fDiff = fTotalPoints - fScoreTarget;
		nPointSwapLoopCount++;
	}

	//
	//----------------------------------------------------------------
	// now report on success of the special deal
	//
	CString strTemp;
/*
	strTemp.Format("Iterations: O=%d, A1=%d, A2=%d, K1=%d, K2=%d, PS=%d, FL=%d\n",
							nOuterLoopCount,nAceLoopCount1,nAceLoopCount2,
							nKingLoopCount1, nKingLoopCount2,
							nPointSwapLoopCount,nFailCount);
	strFeedback += strTemp;
*/

	for(i=0;i<4;i++)
		PLAYER(i).CountCardPoints(TRUE);
	//
	strTemp.Format("S: %d/%d pts;  N: %d/%d pts (Total: %d/%d/%d)\nEast: %d/%d pts;  West: %d/%d pts (Total E/W: %d)",
					PLAYER(SOUTH).GetHCPoints(),
					PLAYER(SOUTH).GetTotalPoints(),
					PLAYER(NORTH).GetHCPoints(),
					PLAYER(NORTH).GetTotalPoints(),
					PLAYER(SOUTH).GetHCPoints() + PLAYER(NORTH).GetHCPoints(),
					fTotalPoints,fScoreTarget,
					PLAYER(EAST).GetHCPoints(),
					PLAYER(EAST).GetTotalPoints(),
					PLAYER(WEST).GetHCPoints(),
					PLAYER(WEST).GetTotalPoints(),
					PLAYER(EAST).GetTotalPoints() + PLAYER(WEST).GetTotalPoints());
	strFeedback += strTemp;
	strTemp.Format("\nQT's: N: %3.1f S: %3.1f;   Stoppers: N: %d S: %d",
					PLAYER(NORTH).GetNumQuickTricks(),
					PLAYER(SOUTH).GetNumQuickTricks(),
					PLAYER(NORTH).GetNumSuitsStopped(),
					PLAYER(SOUTH).GetNumSuitsStopped());
	strFeedback += strTemp;
	if (nGameCode == 2) 
	{
		int numAcesRequired = theApp.GetValue(tnumAcesForSlam,nSlamCode);
		int numAcesHeld = PLAYER(SOUTH).GetNumCardsOf(ACE) + PLAYER(NORTH).GetNumCardsOf(ACE);
		int numKingsRequired = theApp.GetValue(tnumKingsForSlam,nSlamCode);
		int numKingsHeld = PLAYER(SOUTH).GetNumCardsOf(KING) + PLAYER(NORTH).GetNumCardsOf(KING);
		strTemp.Format("\nAces, Kings held by team = %d/%d, %d/%d",
						numAcesHeld,numAcesRequired,
						numKingsHeld,numKingsRequired);
		strFeedback += strTemp;
	}
	FEEDBACK(strFeedback);

	// swap hands if desired
	if (nTeam == EAST_WEST)
		RotatePlayersHands(0, FALSE, FALSE);

	// copy hands to the initial hands
	for(i=0;i<4;i++)
		PLAYER(i).InitializeHand();

	// turn off game auto-play
	if ((theApp.GetValue(tnCardPlayMode) == CEasyBApp::PLAY_FULL_AUTO) || (theApp.GetValue(tnCardPlayMode) == CEasyBApp::PLAY_FULL_AUTO_EXPRESS))
		theApp.SetValue(tnCardPlayMode, CEasyBApp::PLAY_NORMAL);

	// done dealing
	CMainFrame::ResetStatusMessage();
	
	// mark that the deal # is available
	m_bDealNumberAvailable = TRUE;

	// reset view mode
	pVIEW->ClearMode();

	// and begin play
	InitPlay();
}


//
void CEasyBDoc::DealSpecial(int nDealNumber, int nSpecialDealCode) 
{
	int nGameCode = (nSpecialDealCode & 0xC0) >> 6;
	int nSuitCode = (nSpecialDealCode & 0x30) >> 4;
	int nSlamCode = (nSpecialDealCode & 0x0C) >> 2;
	int nTeam = nSpecialDealCode & 0x00000003;
	DealSpecial(nGameCode, nSuitCode, nSlamCode, nTeam, nDealNumber);
}



#define ISHONOR(x,y)  (PLAYER(x).GetCardByPosition(y)->GetFaceValue() > 10)
//
// Swap cards between two hands
//
// - take a high face card from the nSource hand and swap it 
//   for a lower card in the nDest hand
//
double CEasyBDoc::SwapPoints(int nDest, int nSource, double fMax, 
							 int nGameCode, int nSuitCode, int nSlamCode)
{
	//
	double fSrcPoints = PLAYER(nSource).CountCardPoints();
	double fDestPoints = PLAYER(nDest).CountCardPoints();
	CCard *pSrcCard,*pDestCard;
	double fDiff, fTotalLeft = fMax;
	int i,nSrcCard,nDestCard,nSrcVal,nDestVal;
	int nSuit,nOuterLoopCount,nInnerLoopCount;
	int	nTopmostLoopCount = 0;
	double fValSwapped = 0;
	do 
	{
		// values needed to observe Ace/King constraints
		int numAcesRequired = theApp.GetValue(tnumAcesForSlam,nSlamCode);
		int numAcesHeld = PLAYER(SOUTH).GetNumCardsOf(ACE) + PLAYER(NORTH).GetNumCardsOf(ACE);
		int numKingsRequired = theApp.GetValue(tnumKingsForSlam,nSlamCode);
		int numKingsHeld = PLAYER(SOUTH).GetNumCardsOf(KING) + PLAYER(NORTH).GetNumCardsOf(KING);
		// first see if there's an honor card in the source hand
		int nSum = 0;
		BOOL bIsHonor = FALSE;
		for(i=0;i<13;i++) 
		{
			if (ISHONOR(nSource,i)) 
			{
				bIsHonor = TRUE;
				break;
			}
//			nSum += HONOR_VALUE(PLAYER(nSource).m_holdings[i]->GetFaceValue());
		}
//		if (nSum == 0)
		if (!bIsHonor)
			return fValSwapped;	// out of high cards

		// else swap cards randomly
		// begin outer loop
		nOuterLoopCount = 0;
		do {
			// first grab an honor from the source hand
			BOOL bInvalid;
			nInnerLoopCount = 0;
			do 
			{
				bInvalid = FALSE;
				nSrcCard = GetRandomValue(12);
				pSrcCard = PLAYER(nSource).GetCardByPosition(nSrcCard);
				nSrcVal = pSrcCard->GetFaceValue();
				nSuit = pSrcCard->GetSuit();
				// check that dest has cards in this suit
				if (PLAYER(nDest).GetNumCardsInSuit(nSuit) == 0) 
				{
					bInvalid = TRUE;
				} 
				else if (nSuitCode == 1) 
				{
					// check for suit preference
					// if minor suit, don't pull a clubs or
					// diamonds honor above minimum
					if ( ((nSuit == CLUBS) || (nSuit == DIAMONDS)) &&
						 ((nSource == SOUTH) || (nSource == NORTH)) &&
						 (nSrcVal >= theApp.GetValue(tnMinTopMinorCard)) )
						bInvalid = TRUE;
				} 
				else if (nSuitCode == 2) 
				{
					if ( ((nSuit == HEARTS) || (nSuit == SPADES)) &&
						 ((nSource == SOUTH) || (nSource == NORTH)) &&
						 (nSrcVal >= theApp.GetValue(tnMinTopMajorCard)) )
						bInvalid = TRUE;
				}
				// if dealing slam, don't pull an ace from N/S
				// if that will put partnership below limit
				if ((nGameCode == 2) && (nSrcVal == ACE) && 
						((nSource == SOUTH) || (nSource == NORTH)) ) 
				{
					if (numAcesHeld <= numAcesRequired)
						bInvalid = TRUE;
				}
				// likewise, also don't pull a king from N/S
				// if that will put partnership below limit
				if ((nGameCode == 2) && (nSrcVal == KING) && 
						((nSource == SOUTH) || (nSource == NORTH)) ) 
				{
					if (numKingsHeld <= numKingsRequired)
						bInvalid = TRUE;
				}
				nInnerLoopCount++;
			} while ( ((!ISHONOR(nSource, nSrcCard)) || (bInvalid))
							&& (nInnerLoopCount < 40));
			if (nInnerLoopCount >= 40)
				break;
			// then grab a lesser card from the dest hand of 
			// the same suit
			// this could fail under certain circumstances
			nInnerLoopCount = 0;
			bInvalid = FALSE;
			do 
			{
				nDestCard = GetRandomValue(PLAYER(nDest).GetNumCardsInSuit(nSuit)-1);
				pDestCard = PLAYER(nDest).GetCardInSuit(nSuit,nDestCard);
				if (pDestCard == NULL) 
				{	
					// shouldn't happen, but does -- dunno why
					nInnerLoopCount++;
					continue;
				}
				nDestVal = (pDestCard->GetFaceValue() > 10)? pDestCard->GetFaceValue() : 10;
				fDiff = nSrcVal - nDestVal;
				// check to see that we don't replace a king with an ace
				// thus possibly violating a minimum king count requirement
				if ((nGameCode == 2) && (nSrcVal == ACE) && (nDestVal == KING) &&
							((nDest == SOUTH) || (nDest == NORTH)) ) 
				{
					if (numKingsHeld <= numKingsRequired)
						bInvalid = TRUE;
				}
				// increment counter -- this loop could possibly fail
				nInnerLoopCount++;
			} while (((fDiff <= 0) || (fDiff > fTotalLeft) || (bInvalid))
									 && (nInnerLoopCount < 40));
			if (nInnerLoopCount >= 40)
				break;
			nOuterLoopCount++;
			// continue looping until a suitable pair of cards is found
		} while ((fDiff > fTotalLeft) && (nOuterLoopCount < 40));
		//
		if ((fDiff > 0) && (fDiff <= fTotalLeft) && 
				(nInnerLoopCount < 40) && (nOuterLoopCount < 40)) 
		{
			// success -- swap the two cards
			PLAYER(nSource).RemoveCardFromHand(pSrcCard);
			PLAYER(nDest).RemoveCardFromHand(pDestCard);
			PLAYER(nDest).AddCardToHand(pSrcCard,TRUE);
			PLAYER(nSource).AddCardToHand(pDestCard,TRUE);
			//
			PLAYER(nDest).CountCardPoints(TRUE);
			PLAYER(nSource).CountCardPoints(TRUE);
			//
			fValSwapped += fDiff;
			fTotalLeft -= fDiff;
		}
		nTopmostLoopCount++;

	} while ((fTotalLeft > 0) && (nTopmostLoopCount < 30));
	//
	return fValSwapped;
}



//
// Swap two actual cards
//
BOOL CEasyBDoc::SwapPlayersCards(int nPlayer1, int nPlayer2,
								int nSuit1, int nSuit2,
								int nCard1, int nCard2,
								BOOL bResetCounts)
{
	CCard *pCard1,*pCard2;

	if ((PLAYER(nPlayer1).GetNumCardsInSuit(nSuit1) == 0) ||
		(PLAYER(nPlayer2).GetNumCardsInSuit(nSuit2) == 0))
		return 0;
	pCard1 = PLAYER(nPlayer1).GetCardInSuit(nSuit1,nCard1);
	pCard2 = PLAYER(nPlayer2).GetCardInSuit(nSuit2,nCard2);
	PLAYER(nPlayer1).RemoveCardFromHand(pCard1);
	PLAYER(nPlayer2).RemoveCardFromHand(pCard2);
	PLAYER(nPlayer2).AddCardToHand(pCard1,TRUE);
	PLAYER(nPlayer1).AddCardToHand(pCard2,TRUE);
	//
	if (bResetCounts) 
	{
		PLAYER(nPlayer1).CountCards();
		PLAYER(nPlayer2).CountCards();
		PLAYER(nPlayer1).CountCardPoints(TRUE);
		PLAYER(nPlayer2).CountCardPoints(TRUE);
	}
	return 1;
}
