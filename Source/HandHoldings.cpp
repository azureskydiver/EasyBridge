//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//===========================================================
//
// CHandHoldings
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "Card.h"
#include "HandHoldings.h"
#include "Player.h"
#include "PlayEngine.h"
#include "PlayerStatusDialog.h"
#include "ConventionSet.h"
#include "bidparams.h"
#include "handopts.h"
#include "progopts.h"
#include "DeclarerPlayEngine.h"  // NCR-448




//
//=================================================================
//
// Construction/Destruction
//

// constructor
CHandHoldings::CHandHoldings()
{
	// clear all variables
}

// destructor
CHandHoldings::~CHandHoldings()
{
}


//
void CHandHoldings::Initialize(CPlayer* pPlayer, CPlayerStatusDialog* pStatusDlg) 
{ 
	m_pPlayer = pPlayer; 
	m_pStatusDlg = pStatusDlg;
	CCardHoldings::Init();
	m_initialHand.Init();
	m_displayHand.Init();
	for(int i=0;i<4;i++)
	{
		m_suit[i].Init();
		m_suit[i].SetSuit(i);
	}
}


//
//--------------------------------------------------------------
//
// Misc routines
//

//
void CHandHoldings::InitNewHand()
{
	// first sort the hand & suits
	Sort();

	// then assign card ranks
	// and hide the cards if not the player
	if (m_pPlayer->GetPosition() == SOUTH) 
	{
		for(int i=0;i<13;i++)
			m_cards[i]->SetFaceUp();
	} 
	else 
	{
		for(int i=0;i<13;i++)
			m_cards[i]->SetFaceDown();
	}

	// now save the cards as the initial hand
	m_initialHand.Clear();
	for(int i=0;i<13;i++)
		m_initialHand.Add(m_cards[i]);

	// and sort the initial hand
	m_initialHand.Sort();

	// then count points and then cards (follow this order!)
	m_bCountPerformed = FALSE;
	CountCards();
	CountPoints(TRUE);
//	CountCards();

	// done
}



//
// InitSwappedHand()
//
// Called after the player's hand has been swapped with another player's in midgame
//
void CHandHoldings::InitSwappedHand()
{
	// first sort the hand & suits
	Sort();

	// then assign card ranks
	// and hide the cards if not the player
	if (m_pPlayer->GetPosition() == SOUTH) 
	{
		for(int i=0;i<m_numCards;i++)
			m_cards[i]->SetFaceUp();
	} 
	else 
	{
		for(int i=0;i<m_numCards;i++)
			m_cards[i]->SetFaceDown();
	}

	// sort the initial hand
	m_initialHand.Sort();

	// then count points and then cards (follow this order!)
	m_bCountPerformed = FALSE;
	CountCards();
	CountPoints(TRUE);
}



//
void CHandHoldings::ClearHand(BOOL bClearInitialHand)
{
	CCardHoldings::Clear();
	m_displayHand.Clear();
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<4;i++) 
		m_suit[i].Clear();
	//
	if (bClearInitialHand)
		m_initialHand.Clear();
	//
	// clear hand holdings variables
	//
	m_numHCPoints = 0;
	m_numShortPoints = 0;
	m_numLongPoints = 0;
	m_numBonusPoints = 0;
	m_numPenaltyPoints = 0;
	m_numTotalPoints = 0;
	m_numAdjustedPoints = 0;	// this is modified later
	m_numAces = 0;
	m_numKings = 0;
	m_numQueens = 0;
	m_numJacks = 0;
	m_numTens = 0;
	m_nBalanceValue = 0;
	m_numVoids = 0;
	m_numSingletons = 0;
	m_numDoubletons = 0;
	for(i=0;i<4;i++) 
	{
		m_nVoidSuits[i] = NONE;
		m_nSingletonSuits[i] = NONE;
		m_nDoubletonSuits[i] = NONE;
	}
	for(i=0;i<=13;i++)
		m_numSuitsOfAtLeast[i] = 0;
	// clear analysis
	for(i=0;i<4;i++)  {
		m_bRevaluedForSuit[i] = FALSE;
		m_nRevaluedForPlayer[i] = NONE; // NCR-268
		m_nAdjPointsBySuit[i] = 0.0;    // NCR-468
	}
	//
	m_bCardsExposed = FALSE;
}



//
// ExposeCards()
//
void CHandHoldings::ExposeCards(BOOL bExpose)
{
	for(int i=0;i<m_numCards;i++)
		m_cards[i]->SetFaceUp(bExpose);
	m_bCardsExposed = bExpose;
}



//
// clear variables used during the bidding process
//
void CHandHoldings::RestartBidding()
{
	// clear analysis
	for(int i=0;i<4;i++) {
		m_bRevaluedForSuit[i] = FALSE;
		m_nRevaluedForPlayer[i] = NONE; // NCR-268
	}
}


//
void CHandHoldings::FormatHoldingsString()
{
	// format the normal ordered holdings
	m_strHoldings.Format("S:%s  H:%s  D:%s  C:%s",
							m_suit[3].GetHoldingsString(),
							m_suit[2].GetHoldingsString(),
							m_suit[1].GetHoldingsString(),
							m_suit[0].GetHoldingsString());
	// and also in GIB format
	CCardHoldings::FormatGIBHoldingsString();

	// and then the screen holding
	m_displayHand.FormatGIBHoldingsString();
	m_strScreenHolding = "";
	CString strTemp,strTemp2;
	for(int i=3;i>=0;i--) 
	{
		int nSuit = theApp.GetSuitSequence(i);
		strTemp.Format("%c:",GetSuitLetter(nSuit));
		// NCR change [i] to [nSuit] below
		if (m_suit[nSuit].GetLength() == 0) 
		{
			strTemp += "void ";
			if (i > 0)  // add blank except for last one
				strTemp += " ";
		} 
		else 
		{
			for(int j=0;j<m_suit[nSuit].GetLength();j++) 
			{
				strTemp2.Format("%c",GetCardLetter(m_suit[nSuit][j]->GetFaceValue()));
				strTemp += strTemp2;
			}
			if (i > 0)
				strTemp += " ";
		}
		m_strScreenHolding += strTemp;
	}

	// and then the initial holdings
	// let its class do all the work
	m_initialHand.FormatHoldingsString();
}


//
void CHandHoldings::Add(CCard* pCard, const BOOL bSort)
{
	CCardHoldings::Add(pCard, bSort);
	// mark the card as ours
	pCard->SetAssigned(TRUE);
	pCard->SetOwner(m_pPlayer->GetPosition());
	//
	m_displayHand.Add(pCard, bSort);
	m_suit[pCard->GetSuit()].Add(pCard, bSort);
	// need to call FormatStringHoldings() again to update w/ suits
	FormatHoldingsString();
}


//
void CHandHoldings::AddToInitialHand(CCard* pCard, const BOOL bSort)
{
	m_initialHand.Add(pCard, bSort);
}


//
CCard* CHandHoldings::RemoveByIndex(const int nIndex)
{
	ASSERT(nIndex >= 0);
	CCard* pCard = CCardHoldings::RemoveByIndex(nIndex);
	m_displayHand.Remove(pCard);

	// reorganize ordered suit holding
	for(int i=nIndex;i<m_numCards;i++) 
		m_cards[i]->DecrementHandIndex();

	// then update the suit
	m_suit[pCard->GetSuit()].Remove(pCard);

	// and update card info
	pCard->ClearAssignment();
	FormatHoldingsString();

	// done
	return pCard;
}




/*
 * right now, there's no real reason to redefine these operations
 *
//
BOOL CHandHoldings::HasCard(int nDeckValue)
{
	if (m_numCards == 0) return FALSE;
	if (m_bSorted)
	{
		for(int i=0;i<m_numCards;i++)
		{
			if (m_cards[i]->GetDeckValue() == nValue)
				return TRUE;
			if (m_cards[i]->GetDeckValue() < nValue)
				return FALSE;	// passed where it should have been
		}
		return FALSE;
	}
	else
	{
		return CCardHoldings::HasCard(nValue);
	}
}

//
BOOL CHandHoldings::HasCardOfFaceValue(int nFaceValue)
{
	if (m_numCards == 0) return FALSE;
	if (m_bSorted)
	{
		for(int i=0;i<m_numCards;i++)
		{
			if (m_cards[i]->GetFaceValue() == nFaceValue)
				return TRUE;
			if (m_cards[i]->GetFaceValue() < nFaceValue)
				return FALSE;	// passed where it should have been
		}
		return FALSE;
	}
	else
	{
		return CCardHoldings::HasCardOfFaceValue(nFaceValue);
	}
}
*/


//
void CHandHoldings::Sort()
{
	// first call base class to sort the cards in S-H-D-C order
	CCardHoldings::Sort();

	// then assign card ranks
	for(int i=0;i<m_numCards;i++) 
	{
		m_cards[i]->SetHandIndex(i);
		m_cards[i]->SetOwner(m_pPlayer->GetPosition());
		m_cards[i]->SetAssigned();
	}

	// and then be sure to sort each of the suits individually
	for(int j=0;j<4;j++) 
		m_suit[j].Sort();

	// and also sort the display hand
	m_displayHand.Sort();
}




//
//=================================================================
//=================================================================
//
// Main routines
//
//=================================================================
//=================================================================
//




//
// CountCards()
//
// called to count length in the hand's suits
// separated out from the point count cuz the card count can be done many times 
// during the course of a hand, while the point count is only done once, for bidding
//
void CHandHoldings::CountCards()
{
	// check suit length
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<4;i++) 
		m_suit[i].CountCards();
	
	// init
	for(i=0;i<4;i++) 
	{
		m_nVoidSuits[i] = NONE;
		m_nSingletonSuits[i] = NONE;
		m_nDoubletonSuits[i] = NONE;
	}

	//
	// then check for voids, singletons, & doubletons
	//
	m_numVoids = 0;
	m_numSingletons = 0;
	m_numDoubletons = 0;
	//
	for(i=0;i<4;i++) 
	{
		if (m_suit[i].IsVoid())
		{
			m_nVoidSuits[m_numVoids] = i;
			m_numVoids++;;
		}
		if (m_suit[i].IsSingleton())
		{
			m_nSingletonSuits[m_numSingletons] = i;
			m_numSingletons++;
		}
		if (m_suit[i].IsDoubleton())
		{
			m_nDoubletonSuits[m_numDoubletons] = i;
			m_numDoubletons++;
		}
	}

	// count aces and kings
	m_numAces = 0;
	m_numKings = 0;
	m_numQueens = 0;
	m_numJacks = 0;
	m_numTens = 0;
	for(i=0;i<4;i++)
	{
		if (m_suit[i].HasAce())
			m_numAces++;
		if (m_suit[i].HasKing())
			m_numKings++;
		if (m_suit[i].HasQueen())
			m_numQueens++;
		if (m_suit[i].HasJack())
			m_numJacks++;
		if (m_suit[i].HasTen())
			m_numTens++;
	}

	// mark indicators of how many suits have at least 
	// x # of cards in them
	for(i=13;i>=0;i--) 
	{
		// clear and set
		m_numSuitsOfAtLeast[i] = 0;
		for(int j=0;j<4;j++)
			if (m_suit[j].GetLength() >= i)
				m_numSuitsOfAtLeast[i]++;
	}
}




//
// CountPoints()
//
// called right after the deal to count points in the hand
//
double CHandHoldings::CountPoints(const BOOL bForceCount)
{
	// avoid repeating the count unless forced to do so
	if ((m_bCountPerformed) && (!bForceCount))
		return m_numTotalPoints;

	// first call the base class to count HC points
	CCardHoldings::CountPoints(bForceCount);
		
	// sort the hand
	if (!m_bSorted)
		Sort();

	// count the points for each suit
	m_numShortPoints = 0;
	m_numLongPoints = 0;
	m_numBonusPoints = 0;
	m_numPenaltyPoints = 0;
//	m_numTotalPoints = 0;
	//
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<4;i++) 
	{
		m_suit[i].CountPoints(bForceCount);
		m_numShortPoints += m_suit[i].GetShortPoints();
		m_numLongPoints += m_suit[i].GetLengthPoints();
		m_numBonusPoints += m_suit[i].GetBonusPoints();
		m_numPenaltyPoints += m_suit[i].GetPenaltyPoints();
//		m_numTotalPoints += m_suit[i].GetTotalPoints();
	}
	m_numDistPoints = m_numShortPoints + m_numLongPoints; // NCR uncommented
//	m_numDistPoints = m_numLongPoints; // NCR commented out
//	m_numAdjustedPoints = m_numTotalPoints;

	//
	// determine hand-level bonuses and penalties
	//

	// deduct 1 point for an aceless hand
	if ((theApp.GetValue(tbAcelessPenalty)) && (m_numAces == 0))
		m_numPenaltyPoints++;

	// and add 1 point for holding all 4 aces
	if ((theApp.GetValue(tb4AceBonus)) && (m_numAces == 4))
		m_numBonusPoints++;

	//
	// now add up all the points
	// but don't count short points yet
	//
	m_numTotalPoints = m_numHCPoints + m_numLongPoints + 
					   m_numBonusPoints - m_numPenaltyPoints;
	m_numAdjustedPoints = m_numTotalPoints;

	// determine balanced-ness of each suit & the overall hand
	m_nBalanceValue = 0;
	for(i=0;i<4;i++)
	{
		int numSuitCards = m_suit[i].GetNumCards();
		if (numSuitCards < 3) 
			m_nBalanceValue += 3 - numSuitCards;
		else if (numSuitCards > 4) 
			m_nBalanceValue += numSuitCards - 4;
	}
	// a 5/4/3/1 distribution violates the balanced hand rule
	if (m_numSingletons > 0)
		m_nBalanceValue = 3;

	//
	// now summarize the QTs & stoppers 
	//
	m_numQuickTricks = 0.0;
	m_numSuitsStopped = 0;
	m_numSuitsProbStopped = 0;
	m_numSuitsUnstopped = 0;
	m_strSuitsStopped.Empty();
	m_strSuitsUnstopped.Empty();
	//
	for(i=0;i<4;i++) 
	{
		m_nSuitsStopped[i] = NONE;
		m_nSuitsProbStopped[i] = NONE;
		m_nSuitsUnstopped[i] = NONE;
	}
	
	// count stoppers & quick tricks
	for(i=0;i<4;i++) 
	{
		m_numQuickTricks += m_suit[i].GetNumQuickTricks();
		if (m_suit[i].IsSuitStopped())
		{
			m_nSuitsStopped[m_numSuitsStopped] = i;
			m_numSuitsStopped++;
			// record the name of the suit stopped
			m_strSuitsStopped += SuitToString(i);
			m_strSuitsStopped += ", ";
		}
		else if (m_suit[i].IsSuitProbablyStopped())
		{
			m_nSuitsProbStopped[m_numSuitsStopped] = i;
			m_numSuitsProbStopped++;
			// record the name of the suit probably stopped
			m_strSuitsProbStopped += SuitToString(i);
			m_strSuitsProbStopped += ", ";
		}
		else
		{
			// suit not stopped
			m_nSuitsUnstopped[m_numSuitsUnstopped] = i;
			m_numSuitsUnstopped++;
			// record suit name
			m_strSuitsUnstopped += SuitToString(i);
			m_strSuitsUnstopped += ", ";
		}
	}
	// trim trailing characters in list of stopped suits (e.g., ", ")
	m_strSuitsStopped.TrimRight();
	if ((m_strSuitsStopped.GetLength() > 0) &&
		(m_strSuitsStopped[m_strSuitsStopped.GetLength()-1] == ','))
		m_strSuitsStopped.ReleaseBuffer(m_strSuitsStopped.GetLength()-1);
	m_strSuitsProbStopped.TrimRight();
	if ((m_strSuitsProbStopped.GetLength() > 0) &&
		(m_strSuitsProbStopped[m_strSuitsProbStopped.GetLength()-1] == ','))
		m_strSuitsProbStopped.ReleaseBuffer(m_strSuitsProbStopped.GetLength()-1);
	m_strSuitsUnstopped.TrimRight();
	if ((m_strSuitsUnstopped.GetLength() > 0) &&
		(m_strSuitsUnstopped[m_strSuitsUnstopped.GetLength()-1] == ','))
		m_strSuitsUnstopped.ReleaseBuffer(m_strSuitsUnstopped.GetLength()-1);

	// all done
	return m_numTotalPoints;
}






//
//----------------------------------------------------------
//
// EvaluateHoldings()
//
// called after hand is dealt and before bidding
//
void CHandHoldings::EvaluateHoldings()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;

	// clear the hand's analysis info
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<5;i++) 
	{
		m_nMarginalSuitList[i] = NONE;
		m_nOpenableSuitList[i] = NONE;
		m_nStrongSuitList[i] = NONE;
		m_nAbsoluteSuitList[i] = NONE;
		m_nPreferredSuitList[i] = NONE;
		m_nSolidSuitList[i] = NONE;
		m_nSuitsByPreference[i] = NONE;
	}
	for(i=0;i<4;i++)
		m_nSuitsByLength[i] = NONE;  // NCR changed [4] to [i]
	for(i=0;i<4;i++) { 
		m_bRevaluedForSuit[i] = FALSE;
		m_nRevaluedForPlayer[i] = NONE; // NCR-268
	}

	// clear hand analysis
	m_numMarginalSuits = 0;
	m_numOpenableSuits = 0;
	m_numStrongSuits = 0;
	m_numAbsoluteSuits = 0;
	m_numPreferredSuits = 0;
	m_numSolidSuits = 0;
	m_nLowestMarginalSuit = NONE;
	m_nLowestOpenableSuit = NONE;
	m_nLowestStrongSuit = NONE;
	m_nLowestAbsoluteSuit = NONE;
	m_nLowestPreferredSuit = NONE;
	m_nLowestSolidSuit = NONE;
	m_nHighestMarginalSuit = NONE;
	m_nHighestOpenableSuit = NONE;
	m_nHighestStrongSuit = NONE;
	m_nHighestAbsoluteSuit = NONE;
	m_nHighestPreferredSuit = NONE;
	m_nHighestSolidSuit = NONE;
	//
	m_nPreferredSuit = NONE;
	m_nLongestSuit = NONE;	

	//
	//-------------------------------------------------------
	//
	// first report the hand
	//
	CString strLine,str1,str2;
	status << "4=====================================\n";
	status << "4Received hand:\n  " & m_strHoldings & "\n";

	//
	//-------------------------------------------------------
	//
	// then analyze strength of hand
	//

	// first evaluate the individual suits
	for(i=0;i<4;i++) 
		m_suit[i].EvaluateHoldings();

/*
	// now do the same for no trumps
	if (m_nBalanceValue <= 2) 
	{
		// a balanced hand is the minimum rqmt to bid NT
		// but the number of stopped suits is the key
		if (m_numSuitsStopped == 4)
			m_nSuitStatus[4] = SS_STRONG;			
		else if (m_numSuitsStopped == 3)
			m_nSuitStatus[4] = SS_OPENABLE;			
		else if (m_numSuitsStopped == 2)
			m_nSuitStatus[4] = SS_GOOD_SUPPORT;			
		else
			m_nSuitStatus[4] = SS_WEAK_SUPPORT;			
	}
*/

	//
	// sort suits by length (and also by suit rank)
	//
	for(i=0;i<4;i++) 
		m_nSuitsByLength[i] = 3-i;	// init, high suit to low suit
	// here's that ol', inefficient bubble sort again
	for(i=0;i<4;i++) 
	{
		for(int j=0;j<3;j++) 
		{
			// move the suit back if it's shorter
			if (m_suit[m_nSuitsByLength[j]].GetLength() < m_suit[m_nSuitsByLength[j+1]].GetLength())
			{
				// swap suits and move this one back
				int nTemp = m_nSuitsByLength[j];
				m_nSuitsByLength[j] = m_nSuitsByLength[j+1];
				m_nSuitsByLength[j+1] = nTemp;
			}
		}
	}
	m_nLongestSuit = m_nSuitsByLength[0];

	//
	// summarize info about the suit holdings 
	//
	for(i=0;i<4;i++) 
	{
		if (m_suit[i].GetStrength() >= SS_MARGINAL_OPENER) 
		{
			if (m_numMarginalSuits == 0)
				m_nLowestMarginalSuit = i;
			m_nMarginalSuitList[m_numMarginalSuits] = i;
			m_numMarginalSuits++;
			if (m_nLowestMarginalSuit == NONE)
				m_nLowestMarginalSuit = i;
			if (i > m_nHighestMarginalSuit)
				m_nHighestMarginalSuit = i;
		}
		if (m_suit[i].GetStrength() >= SS_OPENABLE) 
		{
			if (m_numOpenableSuits == 0)
				m_nLowestOpenableSuit = i;
			m_nOpenableSuitList[m_numOpenableSuits] = i;
			m_numOpenableSuits++;
			if (m_nLowestOpenableSuit == NONE)
				m_nLowestOpenableSuit = i;
			if (i > m_nHighestOpenableSuit)
				m_nHighestOpenableSuit = i;
		}
		if (m_suit[i].GetStrength() >= SS_STRONG) 
		{
			if (m_numStrongSuits == 0)
				m_nLowestStrongSuit = i;
			m_nStrongSuitList[m_numStrongSuits] = i;
			m_numStrongSuits++;
			if (m_nLowestStrongSuit == NONE)
				m_nLowestStrongSuit = i;
			if (i > m_nHighestStrongSuit)
				m_nHighestStrongSuit = i;
		}
		if (m_suit[i].GetStrength() >= SS_ABSOLUTE) 
		{
			if (m_numAbsoluteSuits == 0)
				m_nLowestAbsoluteSuit = i;
			m_nAbsoluteSuitList[m_numAbsoluteSuits] = i;
			m_numAbsoluteSuits++;
			// set this suit as preferred
			m_nPreferredSuitList[m_numPreferredSuits] = i;
			m_numPreferredSuits++;
			if (m_nLowestPreferredSuit == NONE)
				m_nLowestPreferredSuit = i;
			if (i > m_nHighestPreferredSuit)
				m_nHighestPreferredSuit = i;
		}
	}

	// if there were no absolute suits found, set the preferred
	// suit among the strong or openable suits
	if (m_numPreferredSuits == 0) 
	{
		for(i=0;i<4;i++) 
		{
			if (m_suit[i].GetStrength() >= SS_STRONG) 
			{
				m_nPreferredSuitList[m_numPreferredSuits] = i;
				m_numPreferredSuits++;
				m_suit[i].SetPreferred();
				if (m_nLowestPreferredSuit == NONE)
					m_nLowestPreferredSuit = i;
				if (i > m_nHighestPreferredSuit)
					m_nHighestPreferredSuit = i;
			}
		}
	}
	// now check the openable suits
	if (m_numPreferredSuits == 0) 
	{
		for(i=0;i<4;i++) 
		{
			if (m_suit[i].GetStrength() == SS_OPENABLE) 
			{
				m_nPreferredSuitList[m_numPreferredSuits] = i;
				m_numPreferredSuits++;
				m_suit[i].SetPreferred();
				if (m_nLowestPreferredSuit == NONE)
					m_nLowestPreferredSuit = i;
				if (i > m_nHighestPreferredSuit)
					m_nHighestPreferredSuit = i;
			}
		}
	}
	// and the marginal opener/strong support suits
	if (m_numPreferredSuits == 0) 
	{
		for(i=0;i<4;i++) 
		{
			if (m_suit[i].GetStrength() == SS_MARGINAL_OPENER) 
			{
				m_nPreferredSuitList[m_numPreferredSuits] = i;
				m_numPreferredSuits++;
				m_suit[i].SetPreferred();
				if (m_nLowestPreferredSuit == NONE)
					m_nLowestPreferredSuit = i;
				if (i > m_nHighestPreferredSuit)
					m_nHighestPreferredSuit = i;
			}
		}
	}

	// else just pick the longest suit
	if (m_numPreferredSuits == 0) 
	{
		m_nPreferredSuitList[0] = m_nLongestSuit;
		m_numPreferredSuits = 1;
		m_suit[m_nLongestSuit].SetPreferred();
		m_nHighestPreferredSuit = m_nLowestPreferredSuit = m_nLongestSuit;
		int nLength = m_suit[m_nLongestSuit].GetLength();

		// see if there are other suits with the same length
		for(i=3;i>=0;i--)
		{
			if ((i == m_nLongestSuit) || (m_suit[i].GetLength() < nLength))
				continue;
			// found another suit of the same length
			m_nPreferredSuitList[m_numPreferredSuits] = i;
			m_numPreferredSuits++;
			m_suit[i].SetPreferred();
			m_nLowestPreferredSuit = i;
		}
	}

	//
	// test for solid suits
	//
	for(i=0;i<4;i++)
	{
		if (m_suit[i].IsSolid())
		{
			m_nSolidSuitList[m_numSolidSuits] = i;
			m_numSolidSuits++;
			if ((i < m_nLowestSolidSuit) || (m_nLowestSolidSuit == NONE)) // NCR added test for NONE
				m_nLowestSolidSuit = i;
			if (i > m_nHighestSolidSuit)
				m_nHighestSolidSuit = i;
		}	 
	}

	//
	// now rank the suits in order of preference
	//
	m_nPreferredSuit = RankSuits(4,SP_LAST,CLUBS,DIAMONDS,HEARTS,SPADES,m_nSuitsByPreference);
	for(i=0;i<4;i++)
		m_suit[m_nSuitsByPreference[i]].SetRank(i);	

	//
	// and see which suits (if any) are rebiddable
	//
	m_numRebiddableSuits = 0;
	for(i=0;i<4;i++) 
		if (m_suit[i].IsRebiddable())
			m_numRebiddableSuits++;

	//
	// count winners among the suits
	//
	m_numWinners = 0;
	m_numLikelyWinners = 0;
	m_numTopCards = 0;
	for(i=0;i<4;i++) 
	{
		m_numWinners += m_suit[i].GetNumWinners();
		m_numLikelyWinners += m_suit[i].GetNumLikelyWinners();
		m_numTopCards += m_suit[i].GetNumTopCards();
	}
	m_numLosers = m_numCards - m_numWinners;
	m_numLikelyLosers = 13 - m_numLikelyWinners;

	//
	//------------------------------------------------
	//
	// and finally report on the quality of the hand
	//
	CString strTemp;
	strLine = "Hand evaluation:\n";
	strTemp.Format("  HCPs = %.1f, Distn pts = %.1f, Bonus/Penalty = %.1f/%.1f;\r\nTotal pts = %.1f.\n",
					  m_numHCPoints, m_numDistPoints, m_numBonusPoints,
					  (m_numPenaltyPoints==0)? 0 : -m_numPenaltyPoints,
					  m_numTotalPoints);
	strLine += strTemp;
	strTemp.Format("  QT's = %3.1f,\n",m_numQuickTricks);
	strLine += strTemp;
	if (m_numSuitsStopped > 0) 
	{
		int nCount = 0;
		strTemp = "  Sure Stoppers in: ";
		for(i=3;i>=0;i--) 
		{
			if (m_suit[i].IsSuitStopped()) 
			{
//				str1.Format("%s",SuitToString(i));
				str1.Format("%c",GetSuitLetter(i));
				strTemp += str1;
				nCount++;
				if (nCount < m_numSuitsStopped)
					strTemp += ", ";
			}
		}
		strTemp += "\n";
		strLine += strTemp;
	} 
	else 
	{
//		strLine += "  Sure Stoppers: None\n";
		strLine += "  Sure Stoppers:\n";
	}
	//
	int nProbCount = m_numSuitsProbStopped - m_numSuitsStopped;
	if (nProbCount > 0) 
	{
		int nCount = 0;
		strTemp = "  Probable Stoppers in: ";
		for(i=3;i>=0;i--) 
		{
			if ((m_suit[i].IsSuitProbablyStopped()) &&
								(!m_suit[i].IsSuitStopped())) 
			{
//				str1.Format("%s",SuitToString(i));
				str1.Format("%c",GetSuitLetter(i));
				strTemp += str1;
				nCount++;
				if (nCount < nProbCount)
					strTemp += ", ";
			}
		}
		strTemp += "\n";
		strLine += strTemp;
	} 
	else 
	{
//		strLine += "  Probable Stoppers: None\n";
		strLine += "  Probable Stoppers in:\n";
	}
	//
	strTemp.Format("  %d Likely Winners (%d/%d/%d/%d)\n  %d Likely Losers (%d/%d/%d/%d)\n",
					 m_numLikelyWinners,
					 m_suit[3].GetNumLikelyWinners(),
					 m_suit[2].GetNumLikelyWinners(),
					 m_suit[1].GetNumLikelyWinners(),
					 m_suit[0].GetNumLikelyWinners(),
					 m_numLikelyLosers,
					 m_suit[3].GetNumLikelyLosers(),
					 m_suit[2].GetNumLikelyLosers(),
					 m_suit[1].GetNumLikelyLosers(),
					 m_suit[0].GetNumLikelyLosers());
	strLine += strTemp;		 
	//
	// check suit openability
	//
	if (pCurrConvSet->IsConventionEnabled(tid5CardMajors)) 
	{
		strTemp.Format("Playing five-card majors, ");
		strLine += strTemp;
		if ((m_suit[HEARTS].GetNumCards() >= 5) && (m_suit[SPADES].GetNumCards() >= 5)) 
		{
			// both major suits have 5+ cards
			strLine += "both majors qualify for opening.\n";
		} 
		else if (m_suit[HEARTS].GetNumCards() >= 5) 
		{
			// only hearts has 5+ cards
			strLine += "Hearts can be opened.\n";
		} 
		else if (m_suit[SPADES].GetNumCards() >= 5) 
		{
			// only spades has 5+ cards
			strLine += "Spades can be opened.\n";
		} 
		else 
		{
			strLine += "neither major suit qualifies for opening.\n";
		}
	}
	//
	if (m_numAbsoluteSuits > 0) 
	{
		if (m_numAbsoluteSuits == 1) 
		{
			strTemp.Format("Have a powerful suit in %s.",
								STS(m_nAbsoluteSuitList[0]));
		} 
		else 
		{
			strTemp = "Have powerful suits in ";
			for(i=0;i<m_numAbsoluteSuits;i++) 
			{
				strTemp += STS(m_nAbsoluteSuitList[i]);
				if (i < m_numAbsoluteSuits-1)
					strTemp += ", ";
			}
		}
		strLine += strTemp;
		strLine += "\n";
	}

	// see if there are strong suits that are not absolute suits, 
	// and if so, list them
	int nStrongCount = 0;
	int nStrong[5];
	for(i=0;i<m_numStrongSuits;i++) 
	{
		if (!m_suit[m_nStrongSuitList[i]].IsAbsolute()) 
		{
			nStrongCount++;
			nStrong[i] = m_nStrongSuitList[i];
		}
	}
	//
	if (nStrongCount > 0) 
	{
		if (nStrongCount == 1) 
		{
			strTemp.Format("Have a strong suit in %s.",STS(nStrong[0]));
		} 
		else 
		{
			strTemp = "Have strong suits in ";
			for(i=0;i<nStrongCount;i++) 
			{
				strTemp += STS(nStrong[i]);
				if (i < nStrongCount-1)
					strTemp += ", ";
			}
		}
		strLine += strTemp;
		strLine += "\n";
	}
	//
	if (m_numPreferredSuits == 1) 
	{
		strTemp.Format("Preferred suit is %s.",STS(m_nPreferredSuitList[0]));
	} 
	else 
	{
		// more than one preferred suit
		strTemp = "Preferred suits are  ";
		for(i=0;i<m_numPreferredSuits;i++) 
		{
			strTemp += STS(m_nPreferredSuitList[i]);
			if (i < m_numPreferredSuits-1)
				strTemp += ", ";
		}
		strTemp += "\n";
		strLine += strTemp;
		strTemp.Format("The best suit is %s.",STS(m_nPreferredSuit));
	}
	strLine += strTemp;
	strLine += "\n====================\n";
	//
	//
	status << strLine;
}





//
//----------------------------------------------------------
//
// ReevaluateHoldings()
//
// called after one a cards has been played
//
void CHandHoldings::ReevaluateHoldings(const CCard* pCard)
{
	if (pCard)
	{
		// reevaluate the specified suit
		m_suit[pCard->GetSuit()].ReevaluateHoldings(pCard);
	}
	else
	{
		// reevaluate all suits
		for(int i=0;i<4;i++)
			m_suit[i].ReevaluateHoldings(pCard);
	}

	// count the cards 
	CountCards();

	// then re-rank suits by general preference
	m_nPreferredSuit = RankSuits(4, SP_LAST, CLUBS, DIAMONDS, HEARTS, SPADES, m_nSuitsByPreference);
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<4;i++)
		m_suit[m_nSuitsByPreference[i]].SetRank(i);	

	// then sort the suits by length
	for(i=0;i<4;i++) 
		m_nSuitsByLength[i] = 3-i;	// init, high suit to low suit
	// 
	for(i=0;i<4;i++) 
	{
		for(int j=0;j<3;j++) 
		{
			// move the suit back if it's shorter
			if (m_suit[m_nSuitsByLength[j]].GetLength() < m_suit[m_nSuitsByLength[j+1]].GetLength())
			{
				// swap suits and move this one back
				int nTemp = m_nSuitsByLength[j];
				m_nSuitsByLength[j] = m_nSuitsByLength[j+1];
				m_nSuitsByLength[j+1] = nTemp;
			}
		}
	}
	m_nLongestSuit = m_nSuitsByLength[0];

	//
	// and finally adjust the winners count
	//
	m_numWinners = 0;
	m_numLikelyWinners = 0;
	m_numTopCards = 0;
	for(i=0;i<4;i++) 
	{
		m_numWinners += m_suit[i].GetNumWinners();
		m_numLikelyWinners += m_suit[i].GetNumLikelyWinners();
		m_numTopCards += m_suit[i].GetNumTopCards();
	}
	m_numLosers = m_numCards - m_numWinners;
	m_numLikelyLosers = m_numCards - m_numLikelyWinners;
}





//
// RestoreInitialHand()
//
// restores the hand that was dealt, before play began
//
void CHandHoldings::RestoreInitialHand()
{
	// clear existing cards
	CCardHoldings::Clear();
	m_displayHand.Clear();
	for(int i=0;i<4;i++) 
		m_suit[i].Clear();

	// and restore the original ones
	for(int j=0;j<m_initialHand.GetNumCards();j++) // NCR-FFS change i to j
		Add(m_initialHand[j]);

	// then re-initialize
	InitNewHand();
}




//
// RevalueHand()
//
// revalues a hand's points with respect to a trump suit
//
// nOrigin: 0 = player, 1=dummy ???
// nMode:  1 = dummy, 2 = declarer, 4 = No Penalty for short trump  NCR
// nSuit = trump suit
//
double CHandHoldings::RevalueHand(int nMode, int nTrumpSuit, BOOL bTrace, BOOL bForceRevalue)
{
	int nShortPts = 0;
	int nBonusPts = 0;
	CString strMessage,strTemp;
	CPlayerStatusDialog& status = *m_pStatusDlg;

	// start with old count
	double fAdjPts = m_numTotalPoints;
	int i, numLongSuits = 0;
	int nLongSuit[3];

	//
	if (nTrumpSuit == NOTRUMP)
		return m_numHCPoints;

	ASSERT(ISSUIT(nTrumpSuit)); // NCR added this

	// see if the hand has already been revalued
	//  // NCR-268 added ForPlayer test below and saving farther on
	if (m_bRevaluedForSuit[nTrumpSuit] && (m_nRevaluedForPlayer[nTrumpSuit] == nMode) && !bForceRevalue) {
//		return m_numAdjustedPoints;  // NCR ??? How can there only be one value ??? Seems like one for each suit and mode
//		ASSERT(m_nAdjPointsBySuit[nTrumpSuit] != 0); // This can be < 0 (eg singleton)
		return m_nAdjPointsBySuit[nTrumpSuit]; // NCR-468
	}else {
		m_bRevaluedForSuit[nTrumpSuit] = TRUE;
		m_nRevaluedForPlayer[nTrumpSuit] = nMode; // NCR-268 remember player also
	}

	//
	// first perform basic revaluation
	//
	strMessage.Format("3REVAL1: Revaluing hand as %s for %s contract...\n", 
		        ((nMode & REVALUE_DECLARER)? "declarer" : "dummy"), STSS(nTrumpSuit)); // NCR added suit

	// boost points for trumps and any solid side suits
	m_numLongPoints = 0;
	int nMinTrumpLength;  // # cards we need so partnership has 8 in the suit
	// NCR Following needs to consider if pard's bid was response to a transfer
	//  If a transfer, the required count is NOT same as if opened. <<<<<<????
	//  Also need to consider if a minor was bid again meaning at least 5 cards <<<<????
	if (nMode & REVALUE_DECLARER) {
		nMinTrumpLength = pCurrConvSet->IsConventionEnabled(tid5CardMajors)? 5 : 4;
	}
	else   //  Following for play as Dummy
	{
		// NCR-60 Need to consider if bid was a Convenient minor - if so, need more
		if(ISMINOR(nTrumpSuit)) {
			nMinTrumpLength = 4; // 4 for Diamonds, 5 for Clubs?
		}else{  // if 5CardMajor, pard has 5 so we only need 3
			nMinTrumpLength = pCurrConvSet->IsConventionEnabled(tid5CardMajors)? 3 : 4;
		}
	}
	int numTrumps = m_suit[nTrumpSuit].GetLength();
	//
	for (i=0;i<4;i++) 
	{
		// check the suit
		if (i == nTrumpSuit)
		{
			// trump suit -- add length pts for a long suit
			int nExtra = numTrumps - nMinTrumpLength;
			if (nExtra >= 2)
			{
				// 2 or more than necessary? add 1 pt for 1st, 2 pts each extra
				m_numLongPoints += 1 + (nExtra-1)*2;
			}
			else if (nExtra == 1)
			{
				// 1 more than necessary? add 2 pts
				m_numLongPoints += 1;
			}
			else if (nExtra < 0)
			{
				// oops, fewer than required trumps -- deduct 2 pts for each
				if (!(nMode & REVALUE_NOPENALTY_SHORTTRUMP))
					m_numLongPoints -= (-nExtra) * 2;
			}

			//
			if (m_numLongPoints > 0)
				strTemp.Format("3REVAL5: Adding %.0f point(s) for the %d extra trump(s).\n", m_numLongPoints, nExtra);			
			else if (m_numLongPoints < 0)
				strTemp.Format("3REVAL6: Deducting %.0f point(s) for the %d trump(s) short of %d.\n", -m_numLongPoints, -nExtra, nMinTrumpLength);
			else
				strTemp = "";
			strMessage += strTemp;
		}
		else if (IsSuitSolid(i))
		{
			// non-trump but solid suits -- 1 pt for a card over 4, plus 2 pts for each add'l
			nLongSuit[numLongSuits] = i;
			numLongSuits++;
			int nSolidPts = 0;
			//
			if (m_suit[i].GetLength() >= 6)
				nSolidPts = (m_suit[i].GetLength() - 5)*2 + 1;
			else if (m_suit[i].GetLength() == 5)
				nSolidPts = 1;
			//
			strTemp.Format("3REVAL7: Adding %d point(s) for the solid %s suit.\n", nSolidPts, STSS(i));
			strMessage += strTemp;
			m_numLongPoints += nSolidPts;
		}
	} // end for(i) thru suits

	// also remove any penalty points in the trump suit
	// so need to recalc total penalty
	m_numPenaltyPoints = 0;
	for(i=0;i<4;i++) 
	{
		if (i != nTrumpSuit)
			m_numPenaltyPoints += m_suit[i].GetPenaltyPoints();
	}

	// NCR-249 -1 pt if hand has no Ace. See CountPoints() also
	if(theApp.GetValue(tbAcelessPenalty) && (GetNumAces() == 0))
		m_numPenaltyPoints++;

	//
	// Now perform more specific processing for declarer and dummy
	//

	if (nMode & REVALUE_DECLARER) 
	{
		// we're declarer; so deduct points if we have poor trump honors 
		// i.e., if the suit is topped by less than a King
		if (numTrumps > 0)	// should always be true!
		{
			if (m_suit[nTrumpSuit].GetAt(0)->GetFaceValue() < KING)
			{  // NCR add braces to include following 3 statements (2 below were outside)
				nBonusPts -= (KING - m_suit[nTrumpSuit].GetAt(0)->GetFaceValue());
			//
			strTemp.Format("3REVAL8: Since the trump suit is only topped by a %s, deduct %d points.\n", 
				            m_suit[nTrumpSuit].GetAt(0)->GetFaceName(), -nBonusPts);
			strMessage += strTemp;
			} // NCR ending brace
		}

		// tally points and and store
//		fAdjPts = m_numHCPoints + m_numBonusPoints - m_numPenaltyPoints +
//				  nBonusPts + m_numShortPoints + m_numLongPoints;
		fAdjPts = m_numHCPoints + m_numBonusPoints - m_numPenaltyPoints +
				  nBonusPts + m_numLongPoints + m_numShortPoints;  // NCR-51 add in short points
		m_numAdjustedPoints = fAdjPts;
		m_nAdjPointsBySuit[nTrumpSuit] = fAdjPts; // NCR-468

		// and comment if there's a change in pts
		if (fAdjPts > m_numTotalPoints) 
		{
			// long trumps?
			if ((nTrumpSuit > NONE) && (numTrumps > nMinTrumpLength)) 
			{
				// NCR Should next line format strMessage or strTemp ??? Changed to strTemp
				strTemp.Format("2REVAL10: (with a %d-card trump suit in %s,", numTrumps, STS(nTrumpSuit));
				strMessage += strTemp; // NCR add on
				// with long side suits?
				if (numLongSuits == 1) 
				{
					strTemp.Format(" plus a solid %d-card second suit in %s,",
									m_suit[nLongSuit[0]].GetLength(), STS(nLongSuit[0]));
					strMessage += strTemp;
				} 
				else if (numLongSuits > 1) 
				{
					strMessage += " plus long outside suits in ";
					for(i=0;i<numLongSuits;i++) 
					{
						strMessage += GetSuitName(nLongSuit[i]);
						if (i<numLongSuits-1)
							strMessage += " and ";
						else
							strMessage += ", ";
					}
				}
			} 
			else if (numLongSuits == 1) 
			{
				// trumps aren't long, but do we have a long side suit?
				strTemp.Format("2REVAL12: (with a solid %d-card second suit in %s,",
								m_suit[nLongSuit[0]].GetLength(), STS(nLongSuit[0]));
				strMessage += strTemp;
			} 
			else if (numLongSuits > 1) 
			{
				// or multiple side suits?
				strMessage = "2REVAL14: (with long outside suits in ";
				for(i=0;i<numLongSuits;i++) 
				{
					strMessage += GetSuitName(nLongSuit[i]);
					if (i<numLongSuits-1)
						strMessage += " and ";
					else
						strMessage += ", ";
				}
			}
			else
			{
				strMessage += "2REVAL16: (after re-evaluation for distribution,";
			}

			//
			strTemp.Format(" declarer's hand is revalued for a suit contract from %.1f %sto %.1f points)\n",
							m_numTotalPoints, ((fAdjPts < m_numTotalPoints)? "down " : ""), fAdjPts);
			strMessage += strTemp;
		}
		else if (fAdjPts < m_numTotalPoints) 
		{
			strTemp.Format("2REVAL18: Declarer's hand is revalued for a suit contract in %s from %.1f %sto %.1f points)\n",
							STS(nTrumpSuit), m_numTotalPoints, ((fAdjPts < m_numTotalPoints)? "down " : ""), fAdjPts);  // NCR added STS()
			strMessage += strTemp;
		}
		else {  // NCR Msg for no change
			strTemp.Format("2REVAL19: Declarer's hand is unchanged in value for a suit contract in %s with %.1f points.\n",
							STS(nTrumpSuit), m_numTotalPoints); 
			strMessage += strTemp;
		}
		//
		if (bTrace)
			status << strMessage;
		m_numAdjustedPoints = fAdjPts;
		m_nAdjPointsBySuit[nTrumpSuit] = fAdjPts; // NCR-468
		return fAdjPts;

	} 
	else if(nMode & REVALUE_DUMMY) // NCR add test to make sure
	{
		//------------------------------------------------------------------
		// here we are dummy; boost points for specific support holdings
		//  NCR-695 Moved following outside the loop
		// NCR-125 Need to reduce points if honors are singleton or doubleton
		if((m_numDoubletons > 0) || (m_numSingletons > 0)) 
		{
			for(Suit suitX = CLUBS; suitX <= SPADES; GETNEXTSUIT(suitX)) 
			{
				if(suitX == nTrumpSuit)
					continue;				// skip 
				if((m_suit[suitX].IsDoubleton() 
					&& ((m_suit[suitX].GetTopCard()->GetFaceValue() >= JACK)  // NCR-248 added = to >=
						&& !m_suit[suitX].HasAce())) ) // NCR-360 split doubletons from singletons
				{
					nBonusPts = nBonusPts - 1;  // deduct  1 // NCR-360 vs 2
				}
				else if((m_suit[suitX].IsSingleton() 
					   && ((m_suit[suitX].GetTopCard()->GetFaceValue() >= JACK) // NCR-248 added = to >=
						   && !m_suit[suitX].HasAce())) ) 
				{                            // NCR-360 Deduct full value of card
					nBonusPts = nBonusPts - (m_suit[suitX].GetTopCard()->GetFaceValue() - TEN);
				}
			} // end for(suitX)
		}  // NCR-125 end deducting pts for unguarded honors

		// but need 3 or 4 trumps before we can count any bonuses
//		if ( (numTrumps >= 4) || ((numTrumps == 3) && (ISMAJOR(nTrumpSuit))) ) 
		if (numTrumps >= nMinTrumpLength) 
		{
			// add bonus for voids, singletons, & doubletons
			// NCR Need more trumps if using "Convenient minor" which goes with 5Card Major
			int neededNumTrumps = (pCurrConvSet->IsConventionEnabled(tid5CardMajors)
								   && (nTrumpSuit == CLUBS)) ? 5 : 4;
			if (numTrumps >= neededNumTrumps) 
			{
				// w/ 4+ trumps, void = 5 pts, singleton = 3, & doubleton = 1
				nShortPts = (m_numVoids * 5) + (m_numSingletons * 3) + (m_numDoubletons * 1);
			} 
			else if (numTrumps == (neededNumTrumps-1)) 
			{
				// with only 3 trumps, bonus schedule is 3/2/1
				nShortPts = (m_numVoids * 3) + (m_numSingletons * 2) + (m_numDoubletons * 1);
			}

			// add 1 pt for holding an honor in the trump suit
			if (m_suit[nTrumpSuit].GetHCPoints() > 0)
				nBonusPts++;
/*  NCR-695 Moved outside of if {}s so values available for other code
			// NCR-125 Need to reduce points if honors are singleton or doubleton
			if((m_numDoubletons > 0) || (m_numSingletons > 0)) 
			{
				for(Suit suitX = CLUBS; suitX <= SPADES; GETNEXTSUIT(suitX)) 
				{
					if(suitX == nTrumpSuit)
						continue;				// skip 
					if((m_suit[suitX].IsDoubleton() 
						&& ((m_suit[suitX].GetTopCard()->GetFaceValue() >= JACK)  // NCR-248 added = to >=
						    && !m_suit[suitX].HasAce())) ) // NCR-360 split doubletons from singletons
					{
						nBonusPts = nBonusPts - 1;  // deduct  1 // NCR-360 vs 2
					}
					else if((m_suit[suitX].IsSingleton() 
						   && ((m_suit[suitX].GetTopCard()->GetFaceValue() >= JACK) // NCR-248 added = to >=
						       && !m_suit[suitX].HasAce())) ) 
					{                            // NCR-360 Deduct full value of card
						nBonusPts = nBonusPts - (m_suit[suitX].GetTopCard()->GetFaceValue() - TEN);
					}
				} // end for(suitX)
			}  // NCR-125 end deducting pts for unguarded honors
*/
			// NCR report bonus points
			if((nBonusPts != 0) || (m_numPenaltyPoints != 0))
			{
				strTemp.Format("4REVAL21! Adjusting by %d point(s) for bonus points and %d points for penalty.\n", 
					                nBonusPts, m_numPenaltyPoints);  // NCR show points used
				strMessage += strTemp;
			}

			// and tally and store
			double MaxAdj = 5 + theApp.GetBiddingAgressiveness()*2;  // NCR-641 Set limit to adj pts
			double adjPts = m_numBonusPoints - m_numPenaltyPoints    // Sum the adjustments to HCpts
					     + nBonusPts + nShortPts + m_numLongPoints;
			if(adjPts > MaxAdj)
				adjPts = MaxAdj; // NCR-641 reset to the max allowed

			fAdjPts = m_numHCPoints + adjPts;    // NCR-641 add on adjustment
			m_numAdjustedPoints = fAdjPts;
			m_nAdjPointsBySuit[nTrumpSuit] = fAdjPts; // NCR-468

			// and comment if there's a change in pts
			if (fAdjPts > m_numTotalPoints) 
			{
				// NCR Should next line format strMessage or strTemp ??? Changed to strTemp
				strTemp.Format("2REVAL38! (with %d-card %s trump support", numTrumps, STSS(nTrumpSuit));
				strMessage += strTemp; // NCR add on

				if (m_suit[nTrumpSuit].GetHCPoints() > 0)
					strMessage += " including a trump honor";

				// mark doubletons
				if (m_numDoubletons > 0) 
				{
					if (m_numDoubletons > 1) 
					{
						strMessage += " & doubletons in ";
						for(int i=0;i<m_numDoubletons;i++) 
						{
							strMessage += STS(m_nDoubletonSuits[i]);
							if (i < m_numDoubletons-1)
								strMessage += " and ";
						}
					} 
					else 
					{
						strTemp.Format(" & a doubleton in %s", STS(m_nDoubletonSuits[0]));
						strMessage += strTemp;
					}
					if ((m_numSingletons > 0) || (m_numVoids > 0))
						strMessage += ", ";
				}

				// mark singletons
				if (m_numSingletons > 0) 
				{
					if (m_numSingletons > 1) 
					{
						strMessage += " & singletons in ";
						for(i=0;i<m_numSingletons;i++) 
						{
							strMessage += STS(m_nSingletonSuits[i]);
							if (i < m_numSingletons-1)
								strMessage += " and ";
						}
					} 
					else 
					{
						strTemp.Format(" & a singleton in %s", STS(m_nSingletonSuits[0]));
						strMessage += strTemp;
					}
					if (m_numVoids > 0)
						strMessage += ", ";
				}

				// mark voids
				if (m_numVoids > 0) 
				{
					if (m_numVoids > 1) 
					{
						strMessage += " & voids in ";
						for(i=0;i<m_numVoids;i++) 
						{
							strMessage += STS(m_nVoidSuits[i]);
							if (i<m_numVoids-1)
								strMessage += " and ";
						}
					} 
					else 
					{
						strTemp.Format(" & a void in %s",STS(m_nVoidSuits[0]));
						strMessage += strTemp;
					}
				}
				//
				strTemp.Format(", the hand is revalued for a suit contract as dummy from %.1f %sto %.1f points.\n",
								m_numTotalPoints, ((fAdjPts < m_numTotalPoints)? "down " : ""), fAdjPts);
				strMessage += strTemp;
			}
			else if (fAdjPts < m_numTotalPoints)  // NCR-695 This else was outside the enclosing {}s. Moved here
			{
				strTemp.Format("2REVAL40: Dummy's hand is revalued for a suit contract from %.1f %sto %.1f points.\n", 
							   m_numTotalPoints, ((fAdjPts < m_numTotalPoints)? "down " : ""), fAdjPts);
				strMessage += strTemp;
			}
		}
		else  // NCR-695 number of trumps less than min required
		{
			//  NCR-695 - NEED LOGIC HERE TO Change fAdjPts
			fAdjPts = m_numHCPoints + nBonusPts - (nMinTrumpLength - numTrumps)/2.0; // A Q&D approximation

			strTemp.Format("2REVAL70: Dummy's hand is revalued for a suit contract from %.1f %sto %.1f points.\n", 
							   m_numTotalPoints, ((fAdjPts < m_numTotalPoints)? "down " : ""), fAdjPts);
			strMessage += strTemp;
		}  // end changed else clause for NCR-695 

		//
		if (bTrace)
			status << strMessage;
		m_numAdjustedPoints = fAdjPts;
		m_nAdjPointsBySuit[nTrumpSuit] = fAdjPts; // NCR-468
		return fAdjPts;
	}
	else
	{
		ASSERT(false);  // NCR this should never happen, but let's make sure
		return fAdjPts;
	}

}





//
// RankSuits()
//
// compare up to 4 suits and select the best one
//
// nArbitrate: what to return if suits are equal
//   SP_NONE: return NONE
//   SP_FIRST: return first suit
//   SP_SECOND: return second suit
//   SP_THIRD: return third suit
//   SP_FOURTH: return fourth suit
//   SP_LAST: return last suit
// nOrder: array in which to place the suit order
//
int CHandHoldings::RankSuits(int numSuits, int nArbitrate, int nSuit1, int nSuit2, 
					   int nSuit3, int nSuit4, int* pOrder)
{
	// first sort the suits by length
	int nSuit[4];
	int i,j,nTemp;
	nSuit[0] = nSuit1;
	nSuit[1] = nSuit2;
	nSuit[2] = nSuit3;
	nSuit[3] = nSuit4;
	//
	for(i=0;i<numSuits;i++) 
	{
		for(j=0;j<numSuits-1;j++) 
		{
			if (m_suit[nSuit[j+1]].GetLength() > m_suit[nSuit[j]].GetLength()) 
			{
				nTemp = nSuit[j];
				nSuit[j] = nSuit[j+1];
				nSuit[j+1] = nTemp;
			}
		}		
	}
	// and then by high cards
	// a suit must have at least as many cards & more HCPs
	// in order to be superior, OR
	// four HCPs per card that it's short.
	for(i=0;i<numSuits;i++) 
	{
		for(j=0;j<numSuits-1;j++) 
		{
			int nLen1 = m_suit[nSuit[j]].GetLength();
			int nLen2 = m_suit[nSuit[j+1]].GetLength();
			int nLD = nLen1 - nLen2; 
			double fPts1 = m_suit[nSuit[j]].GetHCPoints();
			double fPts2 = m_suit[nSuit[j+1]].GetHCPoints();
			// see if the second suit is superior
			if ( ((fPts2 > fPts1) && (nLen2 >= nLen1)) ||
			      (fPts2 > fPts1 + nLD*4) ) 
			{
				nTemp = nSuit[j];
				nSuit[j] = nSuit[j+1];
				nSuit[j+1] = nTemp;
			} 
			else if ( ((fPts2 == fPts1) && (nLen2 == nLen1)) ||
			      	     (fPts2 == fPts1 + nLD*4) ) 
			{
				// suits identical; sort by suit order
				if (nSuit[j+1] > nSuit[j]) 
				{
					nTemp = nSuit[j];
					nSuit[j] = nSuit[j+1];
					nSuit[j+1] = nTemp;
				}
			}
		}		
	}
	// done sorting; now copy	
	if (pOrder) 
	{
		for(i=0;i<numSuits;i++)
			pOrder[i] = nSuit[i];
		for(i=numSuits;i<4;i++)
			pOrder[i] = NONE;
	}
	//
	return nSuit[0];
}



//
// AllSuitsStopped()
//
// determines whether all suits are stopped in the hand
//
BOOL CHandHoldings::AllSuitsStopped(BOOL bIncludeProbStoppers) const
{
	//
	if (bIncludeProbStoppers) 
	{
		if (m_numSuitsProbStopped < 4)
			return FALSE;
	} 
	else 
	{
		if (m_numSuitsStopped < 4)
			return FALSE;
	}
	//
	return TRUE;
}




//
// AllOtherSuitsStopped()
//
// determines whether all suits other than the ones given are
// stopped.
// useful when deciding whether to go for a NT contract
//
BOOL CHandHoldings::AllOtherSuitsStopped(int nSuit1, int nSuit2, int nSuit3, BOOL bIncludeProbStoppers) const
{
	int numSuits = 0;

	//
	if (nSuit1 > NONE)
		numSuits++;
	if (nSuit2 > NONE)
		numSuits++;
	if (nSuit3 > NONE)
		numSuits++;

	//
	if (bIncludeProbStoppers) 
	{
		if (m_numSuitsProbStopped < (4 - numSuits))
			return FALSE;
	} 
	else 
	{
		if (m_numSuitsStopped < (4 - numSuits))
			return FALSE;
	}

	//
	for(int i=0;i<4;i++) 
	{
		if ((i == nSuit1) || (i == nSuit2) || (i == nSuit3))
			continue;
		if (bIncludeProbStoppers) 
		{
			if (!m_suit[i].IsSuitProbablyStopped())
				return FALSE;
		} 
		else 
		{
			if (!m_suit[i].IsSuitStopped())
				return FALSE;
		}
	}
	//
	return TRUE;
}


// NCR Test for a worthless doubleton - no King or Ace
bool CHandHoldings::HasWorthlessDoubleton(int exceptSuit) const // NCR-304 added exceptSuit
{
	for(int i = 0; i < m_numDoubletons; i++) {
		int dSuit = m_nDoubletonSuits[i];
		if (dSuit == exceptSuit)
			continue; // NCR-304 ignore this suit
	    const CSuitHoldings& suit = m_suit[dSuit];
		if(suit.GetTopCard()->GetFaceValue() < KING)
			return true;		// Worthless if not Ace or King
	}
	return false;
}

// Balanced hand test
BOOL CHandHoldings::IsBalanced() const
{ 
	// a hand is considered balanced if it has no voids, 
	// no singletons, and no more than one doubleton.
	// e.g., the following qualify:
	// 4/3/3/3, 4/4/3/2, and 5/3/3/2
	if ((m_nBalanceValue <= 2) && (m_numDoubletons <= 1))
		return TRUE;
	else
		return FALSE; 
}


// Semi-balanced test
BOOL CHandHoldings::IsSemiBalanced() const
{ 
	// the semi-balanced test is used to see if a hand can
	// support a NT contract opposite a balanced hand.
	// the basic rqmt is no voids, no more than one singleton,
	// and no 7-card suits.
	// e.g., the following qualify, although they are not truly
	// "balanced" in the strictest sense:
	// 6/4/2/1, 5/4/2/2, and 4/4/4/1
	if ((m_numSuitsOfAtLeast[7] == 0) && 
					(m_numVoids == 0) && (m_numSingletons <= 1))
		return TRUE;
	else
		return FALSE; 
}


//
// GetLongestSuit()
//
int	CHandHoldings::GetLongestSuit(int nType) const
{
	int i;
	switch(nType)
	{
		case SUIT_ANY:
			return m_nLongestSuit;

		case SUIT_MAJOR:
			for(i=0;i<4;i++)
				if (ISMAJOR(m_nSuitsByLength[i]))
					return i;
			break;

		case SUIT_MINOR:
			for(i=0;i<4;i++)
				if (ISMINOR(m_nSuitsByLength[i]))
					return i;
			break;

        // NCR-336 Find longest non-trump suit
		case SUIT_NOTTRUMP:
			int nTrumpSuit = pDOC->GetTrumpSuit();
			for(i=0;i<4;i++) {
				if (m_nSuitsByLength[i] == nTrumpSuit)
					continue;  // skip trump suit
				if (ISSUIT(m_nSuitsByLength[i]))
					return m_nSuitsByLength[i];  // if a suit, return it
			}
			return NOSUIT;  //  NCR-336 Indicate no suit found
	}
	// should never get here!
	ASSERT(FALSE);
	return NONE;
}




//
// GetDiscard()
//
CCard* CHandHoldings::GetDiscard()
{
	CSuitHoldings* pDiscardSuit = NULL;
	CCard* pCard;

	// determine the lead suit and the trump suit
	CCard* pLeadCard = pDOC->GetCurrentTrickCardLed();
	ASSERT(pLeadCard);
	int nSuitLed = pLeadCard->GetSuit();
	int nTrumpSuit = pDOC->GetTrumpSuit();

	// see if we have cards in the suit led
	CSuitHoldings& suit = m_suit[nSuitLed];
	if (suit.GetNumCards() > 0)
	{
		// we have no choice but to follow suit
		// so return the lowest card in the suit
		pCard = suit.GetBottomCard();
		return pCard;
	}

	// here we have no more cards in the suit, so discard from another suit
	// but first see whether we're playing with a trump suit (and have trumps left)
	if (ISSUIT(nTrumpSuit)) 
	{
		// playing with a trump suit
		// see if we have any trumps left
		if (GetNumTrumps() > 0)
		{
			// yes indeed, we do have at least one trump left
			// so return the shortest suit with losers in it
			// first find the shortest suit with no winners, if there is one
			int i; // NCR-FFS added here, removed below
			for(/*int*/ i=3;i>=0;i--)
			{
				int nSuit = m_nSuitsByLength[i];
				// special code -- try to protect high honors, 
				// even if they're not sure winners
				if ( (m_suit[nSuit].HasKing()) ||
					 (m_suit[nSuit].HasQueen() && (m_suit[nSuit].GetNumCards() >= 2)) )
					 continue;
				// else discard from the suit if it has no winners
				if ((nSuit != nTrumpSuit) && 
					(GetNumCardsInSuit(nSuit) > 0) && 
					(GetNumWinnersInSuit(nSuit) == 0))
				{
					// NCR-202 set this suit conditionally
					if(pDiscardSuit == NULL)
					{
						pDiscardSuit = &m_suit[nSuit];
						// NCR test if this suit is hopeless
						if(pDiscardSuit->GetTopMissingSequence().GetNumCards() > 2)
							break;   // use this one
					}
					else if(pDiscardSuit->GetNumCards() < (&m_suit[nSuit])->GetNumCards())
							pDiscardSuit = &m_suit[nSuit];  // change to use the longer suit
				}
			} // end for(i) thru suits

			// see if we found one
			if (pDiscardSuit == NULL)
			{
				// here, each of the remaining suits has at least one winner, 
				// or else have no cards in them
				// so just pick the shortest non-void, non-trump suit that has 
				// at least one loser in it
				for(i=3;i>=0;i--)
				{
					int nSuit = m_nSuitsByLength[i];
					int nSuitLength = GetNumCardsInSuit(nSuit);
					int numLosers = GetNumLosersInSuit(nSuit);
					if ((nSuitLength > 0) && (numLosers > 0) && (nSuit != nTrumpSuit))
					{
						pDiscardSuit = &m_suit[nSuit];
						break;
					}
				}
			}

			// see if we still failed to find a suit
			if (pDiscardSuit == NULL)
			{
				// no suit found -- meaning that there are no suits with losers,
				// or that the only suit left is the trump suit
				if (GetNumCards() == (GetNumCardsInSuit(nTrumpSuit)))
				{
					// no cards outside the trump suit? then we have no
					// choice -- "discard" a trump
					pDiscardSuit = &m_suit[nTrumpSuit];
				}
				else
				{
					// all suits have only winners, so pick the shortest non-trump suit
					for(int i=3;i>=0;i--)
					{
						int nSuit = m_nSuitsByLength[i];
						if ((m_suit[nSuit].GetNumCards() > 0) && (nSuit != nTrumpSuit))
						{
							pDiscardSuit = &m_suit[nSuit];
							break;
						}
					}
				}
			}

			// return the bottom card of the discard suit
			pCard = pDiscardSuit->GetBottomCard();

		}
		else
		{
			// here, we're out of trumps
			// so just return the bottom card of the worst suit
			int nSuit;
			int i; // NCR-FFS added here, removed below
			for(/*int*/ i=3;i>=0;i--)
			{
				nSuit = m_nSuitsByPreference[i];
				// avoid letting go of winners!
				if ((m_suit[nSuit].GetNumCards() > 0) 
					&& (m_suit[nSuit].GetNumCards() > m_suit[nSuit].GetNumWinners()))
				{
					// NCR-202 set this suit conditionally
					if(pDiscardSuit == NULL)
					{
						pDiscardSuit = &m_suit[nSuit];
						// NCR test if this suit is hopeless
						if((pDiscardSuit->GetNumMissingSequences() > 0) 
							&& (pDiscardSuit->GetTopMissingSequence().GetNumCards() > 2))
							break;   // use this one
					}
					else if(pDiscardSuit->GetNumCards() < (&m_suit[nSuit])->GetNumCards())
							pDiscardSuit = &m_suit[nSuit];  // change to use the longer suit
				}
					
			} // end for(i) thru suits  by preference

			// if we failed above, try again
			if (pDiscardSuit == NULL)
			{
				for(i=3;i>=0;i--)
				{
					nSuit = m_nSuitsByPreference[i];
					if (m_suit[nSuit].GetNumCards() > 0)
					{
						pDiscardSuit = &m_suit[nSuit];
						break;
					}
						
				}  // end for(i) thru suits by preference
			}

			// NCR-426 Test if suit is singleton and the card is a winner
			if(pDiscardSuit->IsSingleton() && (pDiscardSuit->GetNumWinners() == 1)) 
			{
				pDiscardSuit = &m_suit[GetSuitsByLength(0)]; // NCR-426 use longest suit
			}  // end NCR-426 Not discarding singleton winner

			//
			pCard = pDiscardSuit->GetBottomCard();
		} // end We're out of trumps
	}
	else
	{
		// playing no trumps
		// generally return the bottom card of the longest suit
		int nSuit = m_nSuitsByLength[0];
		// this is a horrible, horrible way of getting the priority information
		int nPrioritySuit = m_pPlayer->GetPlayEngine()->GetPrioritySuit();
		int nPartnersPrioritySuit = m_pPlayer->GetPlayEngine()->GetPartnersPrioritySuit();
		// see if this is the priority suit, or the suit has zero losers
		if ( (ISSUIT(nSuit) && ((nSuit == nPrioritySuit) || (nSuit == nPartnersPrioritySuit))) || 
		 	 (m_suit[nSuit].GetNumLosers() == 0) )
		{
			// don't discard from this suit if possible;
			// try the other suits in descending length
			int i; // NCR-FFS added here, removed below
			for(/*int*/ i=1;i<4;i++)
			{
				// in order of descending length, find a suit that has losers
				nSuit = m_nSuitsByLength[i];
				if (!ISSUIT(nSuit))
					break;		// no more non-void suits
				if (m_suit[nSuit].GetNumLosers() > 0)
					break;	   // found one
			} // end forI9I thru suits by length

			// if no other suits were found to discard from, return to the first one
			if (!ISSUIT(nSuit) || (i == 4))
				nSuit = m_nSuitsByLength[0];
		}

		//NCR-448 If dummy, need to consider the combined hands
		if(m_pPlayer->IsDummy()) 
		{
			CHandHoldings& dclrHand = m_pPlayer->GetPartner()->GetHand();
		    CDeclarerPlayEngine* dclrPE = dclrHand.m_pPlayer->GetDeclarerEngine();
			CCombinedHoldings& combinedHands = dclrPE->GetCombinedHand();
			int x = 0; // 
		} // NCR-448 end looking at partner's hand
		else if (m_pPlayer->IsDefending()) {
			int x = 0; // for debug break point  NEVER CALLED ???
		}


		// return the bottom card of the suit to discard
		pDiscardSuit = &m_suit[nSuit];
		pCard = pDiscardSuit->GetBottomCard();
	}  //end playing no trumps

	// verify the selected card is OK
	ASSERT(pCard->IsValid());
	ASSERT(HasCard(pCard));
	return pCard;
}




//
int CHandHoldings::GetNumTrumps() const
{
	int nTrumpSuit = pDOC->GetTrumpSuit();
	if (!ISSUIT(nTrumpSuit))
		return 0;
	//
	return GetNumCardsInSuit(nTrumpSuit);
}








//====================================================================
//
// Value Set/Retrieval
//
//====================================================================

//
LPVOID CHandHoldings::GetValuePV(int nItem, int nIndex1, int nIndex2, int nIndex3) const
{
	switch (nItem)
	{
		//
		// card info
		//
		case tInitialHand:	// [13]
			return (LPVOID) m_initialHand[nIndex1];
		case tHand:			// [13]
			return (LPVOID) m_cards[nIndex1];
		case tDisplayHand:	// [13]
			return (LPVOID) m_displayHand[nIndex1];
		case tstrHoldings:		
			return (LPVOID) (LPCTSTR) m_strHolding;
		case tstrScreenHoldings:	
			return (LPVOID) (LPCTSTR) m_strScreenHolding;
		case tbHandSorted:
			return (LPVOID) m_bSorted;
		case tbExposeCards:
			return (LPVOID) m_bCardsExposed;
		case tnumCardsHeld:
			return (LPVOID) m_numCards;
		case tSuitHoldings:			// [4][13]
			return (LPVOID) m_suit[nIndex1][nIndex2];
		case tnumCardsInSuit:		// [4]
			return (LPVOID) m_suit[nIndex1].GetNumCards();
		// basic hand statistics
		case tnumAcesHeld:	
			return (LPVOID) m_numAces;
		case tnumKingsHeld:	
			return (LPVOID) m_numKings;
		case tnumQueensHeld:	
			return (LPVOID) m_numQueens;
		case tnumJacksHeld:	
			return (LPVOID) m_numJacks;
		case tnumTensHeld:	
			return (LPVOID) m_numTens;
		case tnBalanceValue:
			return (LPVOID) m_nBalanceValue;
		// more detailed hand analysis
		case tnumSuitHonors:		// [5]
			return (LPVOID) m_suit[nIndex1].GetNumHonors();
		case tbSuitStopped:		// [4]
			return (LPVOID) m_suit[nIndex1].IsSuitStopped();
		case tbSuitProbStopped:	// [4]
			return (LPVOID) m_suit[nIndex1].IsSuitProbablyStopped();
		case tnumSuitsStopped:		
			return (LPVOID) m_numSuitsStopped;
		case tnumSuitsProbStopped:	
			return (LPVOID) m_numSuitsProbStopped;
		case tnumStoppersInSuit:
			return (LPVOID) m_suit[nIndex1].GetNumStoppers();
		case tnumSuitsUnstopped:
			return (LPVOID) m_numSuitsUnstopped;
		case tnStoppedSuits:		// [4]
			return (LPVOID) m_nSuitsStopped[nIndex1];
		case tnProbStoppedSuits:	// [4]
			return (LPVOID) m_nSuitsProbStopped[nIndex1];
		case tnUnstoppedSuits:	// [4]
			return (LPVOID) m_nSuitsUnstopped[nIndex1];
		case tstrSuitsStopped:
			return (LPVOID) (LPCTSTR) m_strSuitsStopped;
		case tstrSuitsUnstopped:
			return (LPVOID) (LPCTSTR) m_strSuitsUnstopped;
		//
		case tnumRebiddableSuits:
			return (LPVOID) m_numRebiddableSuits;
		case tbSuitIsRebiddable:	// [4]
			return (LPVOID) m_suit[nIndex1].IsRebiddable();
		//
		case tnSuitStrength:		// [5] 
			return (LPVOID) m_suit[nIndex1].GetStrength();
		case tbSuitIsMarginal:	// [5] 
			return (LPVOID) m_suit[nIndex1].IsMarginal();
		case tbSuitIsOpenable:	// [5]  
			return (LPVOID) m_suit[nIndex1].IsOpenable();
		case tbSuitIsStrong:		// [5]  
			return (LPVOID) m_suit[nIndex1].IsStrong();
		case tbSuitIsPreferred:	// [5]  
			return (LPVOID) m_suit[nIndex1].IsPreferred();
		case tbSuitIsAbsolute:	// [5]  
			return (LPVOID) m_suit[nIndex1].IsAbsolute();
		case tbSuitIsSolid:		// [5]  
			return (LPVOID) m_suit[nIndex1].IsSolid();
		case tnumMarginalSuits:
			return (LPVOID) m_numMarginalSuits;
		case tnumOpenableSuits:
			return (LPVOID) m_numOpenableSuits;
		case tnumStrongSuits:
			return (LPVOID) m_numStrongSuits;
		case tnumPreferredSuits:
			return (LPVOID) m_numPreferredSuits;
		case tnumAbsoluteSuits:
			return (LPVOID) m_numAbsoluteSuits;
		case tnumSolidSuits:
			return (LPVOID) m_numSolidSuits;
		case tnLowestMarginalSuit:
			return (LPVOID) m_nLowestMarginalSuit;
		case tnLowestOpenableSuit:	
			return (LPVOID) m_nLowestOpenableSuit;
		case tnLowestStrongSuit:
			return (LPVOID) m_nLowestStrongSuit;
		case tnLowestPreferredSuit:
			return (LPVOID) m_nLowestPreferredSuit;
		case tnLowestAbsoluteSuit:
			return (LPVOID) m_nLowestAbsoluteSuit;
		case tnLowestSolidSuit:
			return (LPVOID) m_nLowestSolidSuit;
		case tnMarginalSuitList: 	// [5]
			return (LPVOID) m_nMarginalSuitList[nIndex1];
		case tnOpenableSuitList:		// [5]
			return (LPVOID) m_nOpenableSuitList[nIndex1];
		case tnStrongSuitList:		// [5]
			return (LPVOID) m_nStrongSuitList[nIndex1];
		case tnPreferredSuitList:	// [5]
			return (LPVOID) m_nPreferredSuitList[nIndex1];
		case tnAbsoluteSuitList:		// [5]
			return (LPVOID) m_nAbsoluteSuitList[nIndex1];
		case tnSolidSuitList:		// [5]
			return (LPVOID) m_nSolidSuitList[nIndex1];
		case tnHighestMarginalSuit:
			return (LPVOID) m_nHighestMarginalSuit;
		case tnHighestOpenableSuit:
			return (LPVOID) m_nHighestOpenableSuit;
		case tnHighestStrongSuit:
			return (LPVOID) m_nHighestStrongSuit;
		case tnHighestPreferredSuit:
			return (LPVOID) m_nHighestPreferredSuit;
		case tnHighestAbsoluteSuit:
			return (LPVOID) m_nHighestAbsoluteSuit;
		case tnHighestSolidSuit:
			return (LPVOID) m_nHighestSolidSuit;
		case tnPreferredSuit:	
			return (LPVOID) m_nPreferredSuit;
		case tnSuitsByPreference:	// [4]
			return (LPVOID) m_nSuitsByPreference[nIndex1];
		case tnSuitRank:				// [4]
			return (LPVOID) m_suit[nIndex1].GetRank();
		case tnumVoidSuits:
			return (LPVOID) m_numVoids;
		case tnVoidSuits:				// [4]
			return (LPVOID) m_nVoidSuits[nIndex1];
		case tnumSingletonSuits:
			return (LPVOID) m_numSingletons;
		case tnSingletonSuits:			// [4]
			return (LPVOID) m_nSingletonSuits[nIndex1];
		case tnumDoubletonSuits:
			return (LPVOID) m_numDoubletons;
		case tnDoubletonSuits:			// [4]
			return (LPVOID) m_nDoubletonSuits[nIndex1];
		case tnSuitsOfAtLeast:		// [14]
			return (LPVOID) m_numSuitsOfAtLeast[nIndex1];
		case tnumWinners:
			return (LPVOID) m_numLikelyWinners;
		case tnumLosers:
			return (LPVOID) m_numLikelyLosers;
		case tnumSuitWinners:		// [4]
			return (LPVOID) m_suit[nIndex1].GetNumLikelyWinners();
		case tnumSuitLosers:			// [4]
			return (LPVOID) m_suit[nIndex1].GetNumLikelyLosers();
		//
		default:
			AfxMessageBox("Unhandled Call to CHandHoldings::GetValue()");
			return NULL;
	}
	return NULL;
}

//
double CHandHoldings::GetValueDouble(int nItem, int nIndex1, int nIndex2, int nIndex3) const
{
	switch (nItem)
	{
		case tfQuickTricks:	
			return m_numQuickTricks;
		case tnumHighCardPoints:
			return m_numHCPoints;
		case tnumSuitPoints:	// [4]
			return m_suit[nIndex1].GetHCPoints();
		case tnumShortPoints:
			return m_numShortPoints;
		case tnumLongPoints:
			return m_numLongPoints;
		case tnumBonusPoints:
			return m_numBonusPoints;
		case tnumPenaltyPoints:
			return m_numPenaltyPoints;
		case tnumTotalPoints:
			return m_numTotalPoints;
		case tnumAdjustedPoints:
			return m_numAdjustedPoints;
		default:
			AfxMessageBox("Unhandled Call to CHandHoldings::GetValue");
			return 1;
	}
	return 0;
}

//
// SetValuePV()
//
int CHandHoldings::SetValuePV(int nItem, LPVOID value, int nIndex1, int nIndex2, int nIndex3)
{
	int nVal = (int) value;
	BOOL bVal = (BOOL) value;
	LPCTSTR sVal = (LPCTSTR) value;
	//
	switch (nItem)
	{
		// card info
		case tInitialHand:	// [13]
//			m_initialHand[nIndex1] = (CCard*) value;
			break;
		case tHand:			// [13]
//			m_hand[nIndex1] = (CCard*) value;
			break;
		case tDisplayHand:	// [13]
//			m_displayHand[nIndex1] = (CCard*) value;
			break;
		case tstrHoldings:		
			m_strHolding = sVal;
			break;
		case tstrScreenHoldings:	
			m_strScreenHolding = sVal;
			break;
		case tbHandSorted:
			m_bSorted = bVal;
			break;
		case tbExposeCards:
//			m_bCardsExposed = bVal;
			break;
		case tnumCardsHeld:
			m_numCards = nVal;
			break;
		case tSuitHoldings:			// [4][13]
		case tnumCardsInSuit:		// [4]
			break;
		// basic hand statistics
		case tnumAcesHeld:	
			m_numAces = nVal;
			break;
		case tnumKingsHeld:	
			m_numKings = nVal;
			break;
		case tnumQueensHeld:	
			m_numQueens = nVal;
			break;
		case tnumJacksHeld:	
			m_numJacks = nVal;
			break;
		case tnumTensHeld:	
			m_numTens = nVal;
			break;
		case tnBalanceValue:
			m_nBalanceValue = nVal;
			break;
		// more detailed hand analysis
		case tnumSuitHonors:		// [5]
		case tbSuitStopped:		// [4]
		case tbSuitProbStopped:	// [4]
			break;
		case tnumSuitsStopped:		
			m_numSuitsStopped = nVal;
			break;
		case tnumSuitsProbStopped:	
			m_numSuitsProbStopped = nVal;
			break;
		case tnumStoppersInSuit:
			break;
		case tnumSuitsUnstopped:
			m_numSuitsUnstopped = nVal;
			break;
		case tnStoppedSuits:		// [4]
			m_nSuitsStopped[nIndex1] = nVal;
			break;
		case tnProbStoppedSuits:	// [4]
			m_nSuitsProbStopped[nIndex1] = nVal;
			break;
		case tnUnstoppedSuits:	// [4]
			m_nSuitsUnstopped[nIndex1] = nVal;
			break;
		case tstrSuitsStopped:
			m_strSuitsStopped = "" + nVal;  // NCR force to String
			break;
		case tstrSuitsUnstopped:
			m_strSuitsUnstopped = "" + nVal; // NCR force to String
			break;
		//
		case tnumRebiddableSuits:
			m_numRebiddableSuits = nVal;
			break;
		case tbSuitIsRebiddable:	// [4]
			break;
		//
		case tnSuitStrength:		// [5] 
		case tbSuitIsMarginal:	// [5] 
		case tbSuitIsOpenable:	// [5]  
		case tbSuitIsStrong:	// [5]  
			break;
		case tbSuitIsPreferred:	// [5]  
			m_suit[nIndex1].SetPreferred();
			break;
		case tbSuitIsAbsolute:	// [5]  
		case tbSuitIsSolid:		// [5]  
		case tnumMarginalSuits:
			m_numMarginalSuits = nVal;
			break;
		case tnumOpenableSuits:
			m_numOpenableSuits = nVal;
			break;
		case tnumStrongSuits:
			m_numStrongSuits = nVal;
			break;
		case tnumPreferredSuits:
			m_numPreferredSuits = nVal;
			break;
		case tnumAbsoluteSuits:
			m_numAbsoluteSuits = nVal;
			break;
		case tnumSolidSuits:
			m_numSolidSuits = nVal;
			break;
		case tnLowestMarginalSuit:
			m_nLowestMarginalSuit = nVal;
			break;
		case tnLowestOpenableSuit:	
			m_nLowestOpenableSuit = nVal;
			break;
		case tnLowestStrongSuit:
			m_nLowestStrongSuit = nVal;
			break;
		case tnLowestPreferredSuit:
			m_nLowestPreferredSuit = nVal;
			break;
		case tnLowestAbsoluteSuit:
			m_nLowestAbsoluteSuit = nVal;
			break;
		case tnLowestSolidSuit:
			m_nLowestSolidSuit = nVal;
			break;
		case tnMarginalSuitList: 	// [5]
			m_nMarginalSuitList[nIndex1] = nVal;
			break;
		case tnOpenableSuitList:		// [5]
			m_nOpenableSuitList[nIndex1] = nVal;
			break;
		case tnStrongSuitList:		// [5]
			m_nStrongSuitList[nIndex1] = nVal;
			break;
		case tnPreferredSuitList:	// [5]
			m_nPreferredSuitList[nIndex1] = nVal;
			break;
		case tnAbsoluteSuitList:		// [5]
			m_nAbsoluteSuitList[nIndex1] = nVal;
			break;
		case tnSolidSuitList:		// [5]
			m_nSolidSuitList[nIndex1] = nVal;
			break;
		case tnHighestMarginalSuit:
			m_nHighestMarginalSuit = nVal;
			break;
		case tnHighestOpenableSuit:
			m_nHighestOpenableSuit = nVal;
			break;
		case tnHighestStrongSuit:
			m_nHighestStrongSuit = nVal;
			break;
		case tnHighestPreferredSuit:
			m_nHighestPreferredSuit = nVal;
			break;
		case tnHighestAbsoluteSuit:
			m_nHighestAbsoluteSuit = nVal;
			break;
		case tnHighestSolidSuit:
			m_nHighestSolidSuit = nVal;
			break;
		case tnPreferredSuit:	
			m_nPreferredSuit = nVal;
			break;
		case tnSuitsByPreference:	// [4]
			m_nSuitsByPreference[nIndex1] = nVal;
			break;
		case tnSuitRank:				// [4]
			m_suit[nIndex1].SetRank(nVal);
			break;
		case tnumVoidSuits:
			m_numVoids = nVal;
			break;
		case tnVoidSuits:				// [4]
			m_nVoidSuits[nIndex1] = nVal;
			break;
		case tnumSingletonSuits:
			m_numSingletons = nVal;
			break;
		case tnSingletonSuits:			// [4]
			m_nSingletonSuits[nIndex1] = nVal;
			break;
		case tnumDoubletonSuits:
			m_numDoubletons = nVal;
			break;
		case tnDoubletonSuits:			// [4]
			m_nDoubletonSuits[nIndex1] = nVal;
			break;
		case tnSuitsOfAtLeast:		// [14]
			m_numSuitsOfAtLeast[nIndex1] = nVal;
			break;
		case tnumWinners:
			m_numLikelyWinners = nVal;
			break;
		case tnumLosers:
			m_numLikelyLosers = nVal;
			break;
		case tnumSuitWinners:		// [4]
		case tnumSuitLosers:			// [4]
			break;
		//
		default:
			AfxMessageBox("Unhandled Call to CHandHoldings::SetValue()");
			return 1;
	}
	return 0;
}

// "double" version
int CHandHoldings::SetValue(int nItem, double fValue, int nIndex1, int nIndex2, int nIndex3)
{
	switch (nItem)
	{
		case tnumHighCardPoints:
			m_numHCPoints = fValue;
			break;
		case tnumSuitPoints:	// [4]
			break;
		case tnumShortPoints:
			m_numShortPoints = fValue;
			break;
		case tnumLongPoints:
			m_numLongPoints = fValue;
			break;
		case tnumBonusPoints:
			m_numBonusPoints = fValue;
			break;
		case tnumPenaltyPoints:
			m_numPenaltyPoints = fValue;
			break;
		case tnumTotalPoints:
			m_numTotalPoints = fValue;
			break;
		case tnumAdjustedPoints:
			m_numAdjustedPoints = fValue;
			break;
		default:
			AfxMessageBox("Unhandled Call to CHandHoldings::SetValue()");
			return 1;
	}
	return 0;
}

//
int CHandHoldings::GetValue(int nItem, int nIndex1, int nIndex2, int nIndex3) const
{
	return (int) GetValuePV(nItem, nIndex1, nIndex2, nIndex3);
}

int CHandHoldings::SetValue(int nItem, int nValue, int nIndex1, int nIndex2, int nIndex3)
{
	return SetValuePV(nItem, (LPVOID) nValue, nIndex1, nIndex2, nIndex3);
}

