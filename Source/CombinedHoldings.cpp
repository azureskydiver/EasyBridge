//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//===========================================================
//
// CCombinedHoldings
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "Card.h"
#include "CombinedSuitHoldings.h"
#include "CombinedHoldings.h"
#include "PlayerStatusDialog.h"
#include "handopts.h"
#include "GuessedHandHoldings.h"   // NCR-11



//
//=================================================================
//
// Construction/Destruction
//

// constructor
CCombinedHoldings::CCombinedHoldings()
{
	// alloc memory
	for(int i=0;i<4;i++)
		m_pSuit[i] = new CCombinedSuitHoldings;
	// clear all variables
}

// destructor
CCombinedHoldings::~CCombinedHoldings()
{
	// free memory
	for(int i=0;i<4;i++)
		delete m_pSuit[i];
}

//
void CCombinedHoldings::Initialize(CPlayer* pPlayer, CPlayerStatusDialog* pStatusDlg, CHandHoldings* pHand, CHandHoldings* pPartnersHand)
{ 
	CCardHoldings::Init();
	for(int i=0;i<4;i++)
	{
		m_pSuit[i]->Init();
		m_pSuit[i]->SetSuit(i);
	}
	//
	m_pPlayer = pPlayer; 
	m_pStatusDlg = pStatusDlg;
	m_pPlayerHand = pHand;
	m_pPartnerHand = pPartnersHand;
}


//
//======================================================================
//
//
// Misc routines
//

//
CCard* CCombinedHoldings::GetCard(int nSuit, int nPos) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->GetAt(nPos); }

CCard* CCombinedHoldings::GetCard(int nCardValue) const 
	{ return GetCard(CARDSUIT(nCardValue), FACEVAL(nCardValue)); }

CCombinedSuitHoldings& CCombinedHoldings::GetSuit(int nSuit) 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return *(m_pSuit[nSuit]); }

int CCombinedHoldings::GetNumCardsInSuit(int nSuit) const
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->GetNumCards(); }

int CCombinedHoldings::GetNumTrumps() const
	{	int nTrumpSuit = pDOC->GetTrumpSuit();
		if (!ISSUIT(nTrumpSuit))
			return 0;
		return GetNumCardsInSuit(nTrumpSuit);
	}

int CCombinedHoldings::GetNumDeclarerTrumps() const
	{	int nTrumpSuit = pDOC->GetTrumpSuit();
		if (!ISSUIT(nTrumpSuit))
			return 0;
		return m_pPlayerHand->GetNumCardsInSuit(nTrumpSuit);
	}

int CCombinedHoldings::GetNumDummyTrumps() const
	{	int nTrumpSuit = pDOC->GetTrumpSuit();
		if (!ISSUIT(nTrumpSuit))
			return 0;
		return m_pPartnerHand->GetNumCardsInSuit(nTrumpSuit);
	}

int CCombinedHoldings::GetNumPlayersCardsInSuit(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->GetDeclarerLength(); }

int CCombinedHoldings::GetNumPartnersCardsInSuit(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->GetDummyLength(); }
	
//
int	CCombinedHoldings::GetNumWinnersInSuit(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->GetNumWinners(); }	

int	CCombinedHoldings::GetNumLosersInSuit(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->GetNumLosers(); }

int	CCombinedHoldings::GetNumLikelyWinnersInSuit(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->GetNumLikelyWinners(); }

int	CCombinedHoldings::GetNumLikelyLosersInSuit(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->GetNumLikelyLosers(); }

int	CCombinedHoldings::GetNumDeclarerWinnersInSuit(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->GetNumDeclarerWinners(); }

int	CCombinedHoldings::GetNumDeclarerLosersInSuit(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->GetNumDeclarerLosers(); }

int	CCombinedHoldings::GetNumDummyWinnersInSuit(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->GetNumDummyWinners(); }

int	CCombinedHoldings::GetNumDummyLosersInSuit(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->GetNumDummyLosers(); }

//
int	CCombinedHoldings::GetSuitStrength(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->GetStrength(); }

int	CCombinedHoldings::GetSuitLength(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->GetNumCards(); }	

double CCombinedHoldings::GetSuitHCPoints(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->GetHCPoints(); }

double CCombinedHoldings::GetSuitTotalPoints(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->GetTotalPoints(); }

int	CCombinedHoldings::GetNumSuitsOfAtLeast(int nLength) const 
	{ return m_numSuitsOfAtLeast[nLength]; }

//
BOOL CCombinedHoldings::SuitHasCard(int nSuit, int nCardFaceValue) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->HasCardOfFaceValue(nCardFaceValue); }

BOOL CCombinedHoldings::SuitHasAce(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->HasAce(); }

BOOL CCombinedHoldings::SuitHasKing(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->HasKing(); }

BOOL CCombinedHoldings::SuitHasQueen(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->HasQueen(); }

BOOL CCombinedHoldings::SuitHasJack(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->HasJack(); }

BOOL CCombinedHoldings::SuitHasTen(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->HasTen(); }

BOOL CCombinedHoldings::IsSuitStopped(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->IsSuitStopped(); }

BOOL CCombinedHoldings::IsSuitProbablyStopped(int nSuit) const 
	{ VERIFY((nSuit>=0)&&(nSuit<=3)); return m_pSuit[nSuit]->IsSuitProbablyStopped(); }





//
//======================================================================
//
// Main routines
//
//======================================================================
//



//
// InitNewHand()
//
// initialized the combined holdings of declarer & dummy
// called right after dummy exposes his hand
//
void CCombinedHoldings::InitNewHand()
{
	// first clear the current hand
	ClearHand();

	// then copy the cards from both source hands
	int i;
	for(i=0;i<m_pPlayerHand->GetNumCards();i++)
		AddFromSource((*m_pPlayerHand)[i], TRUE);
	for(i=0;i<m_pPartnerHand->GetNumCards();i++)
		AddFromSource((*m_pPartnerHand)[i], FALSE);

	// now sort hand & suits
	Sort();
	for(i=0;i<4;i++)
		m_pSuit[i]->Sort();

	// then count the card points
	CountPoints();
}


//
void CCombinedHoldings::ClearHand()
{
	CCardHoldings::Clear();
	int i;
	for(i=0;i<4;i++) 
		m_pSuit[i]->Clear();

	// clear hand holdings variables
	m_numAces = 0;
	m_numKings = 0;
	m_numDistPoints = 0;
	m_numTotalPoints = 0;
	m_numAdjustedPoints = 0;	// this is modified later
	//
	m_numQuickTricks = 0;
	m_numSuitsStopped = 0;
	m_numSuitsProbStopped = 0;
	m_numSuitsUnstopped = 0;
	for(i=0;i<4;i++)
	{
		m_nSuitsStopped[i];
		m_nSuitsProbStopped[i];
		m_nSuitsUnstopped[i];
	}
	//
	m_strSuitsStopped.Empty();
	m_strSuitsProbStopped.Empty();
	m_strSuitsUnstopped.Empty();
	//
	m_nContractSuit = NONE;			
	m_nPreferredSuit = NONE;
	m_nLongestSuit = NONE;			
	for(i=0;i<5;i++)
		m_nSuitsByPreference[i] = NONE;
	for(i=0;i<4;i++)
		m_nSuitsByLength[i] = NONE;
	for(i=0;i<14;i++)
		m_numSuitsOfAtLeast[i] = 0;
	//
	m_numMaxWinners = 0;
	m_numMaxLikelyWinners = 0;
	m_numSureWinners = 0; // NCR-587
	m_numMaxTopCards = 0;
	m_numDeclarerWinners = 0;
	m_numDummyWinners = 0;
	m_numDeclarerTopCards = 0;
	m_numDummyTopCards = 0;
	m_numLikelyWinners = 0;
	m_numLikelyLosers = 0;
	m_numWinners = 0;
	m_numLosers = 0;
	//
	m_numTopCards = 0;
	m_numTopHonors = 0;
	m_numSecondaryHonors = 0;
	m_numMissingHonors = 0;
	m_numMissingTopHonors = 0;
	m_numMissingInternalHonors = 0;
	m_numMissingBottomHonors = 0;
}



//
void CCombinedHoldings::FormatHoldingsString()
{
	m_strHoldings.Format("S:%s H:%s D:%s C:%s",m_pSuit[3]->GetHoldingsString(),
											   m_pSuit[2]->GetHoldingsString(),
											   m_pSuit[1]->GetHoldingsString(),
											   m_pSuit[0]->GetHoldingsString());
	CCardHoldings::FormatGIBHoldingsString();
}


//
void CCombinedHoldings::Add(CCard* pCard, const BOOL bSort)
{
	// shouldn't call this function on this class
	ASSERT(FALSE);
}


//
void CCombinedHoldings::AddFromSource(CCard* pCard, const BOOL bPlayerCard, const BOOL bSort)
{
	CCardHoldings::Add(pCard, bSort);
	m_pSuit[pCard->GetSuit()]->AddFromSource(pCard, bPlayerCard, bSort);
	FormatHoldingsString();
}


//
void CCombinedHoldings::RemoveFromSource(CCard* pCard, const BOOL bPlayerCard)
{
	CCardHoldings::Remove(pCard);
	m_pSuit[pCard->GetSuit()]->RemoveFromSource(pCard, bPlayerCard);
	FormatHoldingsString();
}



//
CCard* CCombinedHoldings::RemoveByIndex(const int nIndex)
{
	CCard* pCard = CCardHoldings::RemoveByIndex(nIndex);

	// reorganize ordered suit holding
	int i;
	for(i=nIndex;i<m_numCards;i++) 
		m_cards[i]->DecrementHandIndex();

	// then update the suit
	m_pSuit[pCard->GetSuit()]->Remove(pCard);

	// and update card info
	pCard->ClearAssignment();
	FormatHoldingsString();

	// done
	return pCard;
}





//
void CCombinedHoldings::Sort()
{
	int i,j;

	// first call base class to sort the cards in S-H-D-C order
	CCardHoldings::Sort();

	// and then be sure to sort each of the suits individually
	for(i=0;i<4;i++) 
		m_pSuit[i]->Sort();

	// and set the literal hand description
	// possible bug in MFC 4.0 causes the following to crash
	m_strHolding = "";
	CString strTemp,strTemp2;
	for(i=3;i>=0;i--) 
	{
		// first set the S-H-D-C order holding
		strTemp.Format("%c:",GetSuitLetter(i));
		if (m_pSuit[i]->GetLength() == 0) 
		{
			strTemp += "void";
			if (i > 0)
				strTemp += " ";
		} 
		else 
		{
			for(j=0;j<m_pSuit[i]->GetLength();j++) 
			{
				strTemp2.Format("%c",m_pSuit[i]->GetAt(j)->GetCardLetter());
				strTemp += strTemp2;
			}
			if (i > 0)
				strTemp += " ";
		}
		m_strHolding += strTemp;
		// then the screen-order holding
		int nSuit = theApp.GetSuitSequence(i);
		strTemp.Format("%c:",GetSuitLetter(nSuit));
		if (m_pSuit[i]->GetLength() == 0) 
		{
			strTemp += "void ";
			if (i > 0)
				strTemp += " ";
		} 
		else 
		{
			for(j=0;j<m_pSuit[i]->GetLength();j++) 
			{
				strTemp2.Format("%c",GetCardLetter((*m_pSuit[i])[j]->GetFaceValue()));
				strTemp += strTemp2;
			}
			if (i > 0)
				strTemp += " ";
		}
	}

	// done
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
//----------------------------------------------------------
//
// CountPoints()
//
// called right after the deal to count points in the hand
//
double CCombinedHoldings::CountPoints(const BOOL bForceCount)
{
	//
	// first call the base class to count HC points
	//
	CCardHoldings::CountPoints(bForceCount);
		
	//
	// sort the hand
	//
	if (!m_bSorted)
		Sort();

	//
	// count the points for each suit
	//
	m_numTotalPoints = 0;
	int i; // NCR-FFS added here and removed below
	for(/*int*/ i=0;i<4;i++) 
	{
		m_pSuit[i]->CountPoints(bForceCount);
		m_numTotalPoints += m_pSuit[i]->GetHCPoints();
	}
	m_numDistPoints = m_pPlayerHand->GetDistPoints() + m_pPartnerHand->GetDistPoints();
	m_numTotalPoints += m_numDistPoints; 
	m_numAdjustedPoints = m_numTotalPoints;

	// count aces and kings, and top/secondary honors
	m_numAces = 0;
	m_numKings = 0;
	m_numTopCards = 0;
	m_numTopHonors = 0;
	m_numSecondaryHonors = 0;
	m_numMissingHonors = 0;
	m_numMissingTopHonors = 0;
	m_numMissingInternalHonors = 0;
	m_numMissingBottomHonors = 0;
	//
	for(i=0;i<4;i++)
	{
		if (m_pSuit[i]->HasAce())
			m_numAces++;
		if (m_pSuit[i]->HasKing())
			m_numKings++;
		m_numTopCards += m_pSuit[i]->GetNumTopCards();
		m_numTopHonors += m_pSuit[i]->GetNumTopHonors();
		m_numSecondaryHonors += m_pSuit[i]->GetNumSecondaryHonors ();
		m_numMissingHonors += m_pSuit[i]->GetNumMissingHonors();
		m_numMissingTopHonors += m_pSuit[i]->GetNumMissingTopHonors();
		m_numMissingInternalHonors += m_pSuit[i]->GetNumMissingInternalHonors();
		m_numMissingBottomHonors += m_pSuit[i]->GetNumMissingBottomHonors();
	}

	// mark indicators of how many suits have at least 
	// x # of cards in them
	for(i=13;i>=0;i--) 
	{
		// first clear, then add up
		m_numSuitsOfAtLeast[i] = 0;
		for(int j=0;j<4;j++)
			if (m_pSuit[j]->GetLength() >= i)
				m_numSuitsOfAtLeast[i]++;
	}

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
		m_numQuickTricks += m_pSuit[i]->GetNumQuickTricks();
		if (m_pSuit[i]->IsSuitStopped())
		{
			m_nSuitsStopped[m_numSuitsStopped] = i;
			m_numSuitsStopped++;
			// record the name of the suit stopped
			m_strSuitsStopped += SuitToString(i);
			m_strSuitsStopped += ", ";
		}
		else if (m_pSuit[i]->IsSuitProbablyStopped())
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
void CCombinedHoldings::EvaluateHoldings()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;

	//
	//-------------------------------------------------------
	//
	// first report the hand
	//
	CString strLine,str1,str2;
	status.SetStreamOutputLevel(2);
	status << "Combined hand (Dummy/Hand):\n" & 
				    "  S:" & m_pSuit[3]->GetHoldingsString() & "\n" &
					"  H:" & m_pSuit[2]->GetHoldingsString() & "\n" &
					"  D:" & m_pSuit[1]->GetHoldingsString() & "\n" &
					"  C:" & m_pSuit[0]->GetHoldingsString() & "\n";


	//
	//-------------------------------------------------------
	//
	// then analyze strength of hand
	//

	// first evaluate the individual suits
	int i;
	for(i=0;i<4;i++) 
		m_pSuit[i]->EvaluateHoldings();

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
			if (m_pSuit[m_nSuitsByLength[j]]->GetLength() < m_pSuit[m_nSuitsByLength[j+1]]->GetLength())
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
	// count winners among the suits
	//
	m_numWinners = 0;
	m_numLikelyWinners = 0;
	m_numMaxWinners = 0;
	m_numMaxTopCards = 0;
	m_numMaxLikelyWinners = 0;
	m_numSureWinners = 0;  // NCR-587
	for(i=0;i<4;i++) 
	{
		m_numWinners += m_pSuit[i]->GetNumWinners();
		m_numLikelyWinners += m_pSuit[i]->GetNumLikelyWinners();
		m_numMaxWinners += m_pSuit[i]->GetNumMaxWinners();
		m_numMaxLikelyWinners += m_pSuit[i]->GetNumMaxLikelyWinners();
		m_numMaxTopCards += m_pSuit[i]->GetNumMaxTopCards();
		m_numSureWinners += m_pSuit[i]->GetNumSureWinners(); // NCR-587
	}
	if (m_numWinners > 13)
		m_numWinners = 13;
	if (m_numLikelyWinners > 13)
		m_numLikelyWinners = 13;
	m_numLosers = 13 - m_numWinners;
	m_numLikelyLosers = 13 - m_numLikelyWinners;

	// determine the declarer/dummy split of winners
	m_numDeclarerWinners = 0;
	m_numDummyWinners = 0;
	m_numDeclarerTopCards = 0;
	m_numDummyTopCards = 0;
	for(i=0;i<4;i++) 
	{
		m_numDeclarerWinners += m_pSuit[i]->GetNumDeclarerWinners();
		m_numDummyWinners += m_pSuit[i]->GetNumDummyWinners();
		m_numDeclarerTopCards += m_pSuit[i]->GetNumDeclarerTopCards();
		m_numDummyTopCards += m_pSuit[i]->GetNumDummyTopCards();
	}

	// it's possible we might have > 13 combined winners
	if ((m_numDeclarerWinners+m_numDummyWinners) <= 13)
		ASSERT((m_numDeclarerWinners+m_numDummyWinners) == m_numWinners);


	//
	//------------------------------------------------
	//
	// and finally report on the quality of the hand
	//
	CString strTemp;
	strLine = "Combined hand evaluation:\n";
	strTemp.Format("  HCPs = %.1f, Distn pts = %.1f;\r\n  Total pts = %.1f.\n",
					  m_numHCPoints,m_numDistPoints,m_numTotalPoints);
	strLine += strTemp;
	strTemp.Format("  QT's = %3.1f,\n",m_numQuickTricks);
	strLine += strTemp;
	if (m_numSuitsStopped > 0) 
	{
		int nCount = 0;
		strTemp = "  Stoppers in: ";
		for(i=3;i>=0;i--) 
		{
			if (m_pSuit[i]->IsSuitStopped()) 
			{
//				if (m_pSuit[i]->GetNumStoppers() == 1)
					str1.Format("%c",GetSuitLetter(i));
//				else
//					str1.Format("%c(%d)",GetSuitLetter(i),m_pSuit[i]->GetNumStoppers());
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
			if ((m_pSuit[i]->IsSuitProbablyStopped()) &&
								(!m_pSuit[i]->IsSuitStopped())) 
			{
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
		strLine += "  Probable Stoppers in:\n";
	}
	//
	strTemp.Format("  %d Sure Winner%s(%d/%d/%d/%d)\n",
					 m_numMaxTopCards,
					 (m_numMaxTopCards > 1)? "s " : " ",
					 m_pSuit[3]->GetNumMaxTopCards(),
					 m_pSuit[2]->GetNumMaxTopCards(),
					 m_pSuit[1]->GetNumMaxTopCards(),
					 m_pSuit[0]->GetNumMaxTopCards());
	strLine += strTemp;		 
	strTemp.Format("  %d Eventual Winner%s(%d/%d/%d/%d)\n",
					 m_numMaxWinners,
					 (m_numMaxWinners > 1)? "s " : " ",
					 m_pSuit[3]->GetNumMaxWinners(),
					 m_pSuit[2]->GetNumMaxWinners(),
					 m_pSuit[1]->GetNumMaxWinners(),
					 m_pSuit[0]->GetNumMaxWinners());
/*
	strTemp.Format("  %d Sure Winner%s(%d/%d/%d/%d)\n",
					 m_numWinners,
					 (m_numWinners> 1)? "s " : " ",
					 m_pSuit[3]->GetNumWinners(),
					 m_pSuit[2]->GetNumWinners(),
					 m_pSuit[1]->GetNumWinners(),
					 m_pSuit[0]->GetNumWinners());
	strLine += strTemp;		 
	strTemp.Format("  %d Eventual Winner%s(%d/%d/%d/%d)\n",
					 m_numLikelyWinners,
					 (m_numLikelyWinners> 1)? "s " : " ",
					 m_pSuit[3]->GetNumLikelyWinners(),
					 m_pSuit[2]->GetNumLikelyWinners(),
					 m_pSuit[1]->GetNumLikelyWinners(),
					 m_pSuit[0]->GetNumLikelyWinners());
*/
	strLine += strTemp;		 
/*
	strTemp.Format("  %d Loser%s(%d/%d/%d/%d) vs. %d allowable\n",
					 m_numLosers,
					 ((m_numLosers > 1)? "s " : " "),
					 m_pSuit[3]->GetNumLosers(),
					 m_pSuit[2]->GetNumLosers(),
					 m_pSuit[1]->GetNumLosers(),
					 m_pSuit[0]->GetNumLosers(),
					 (7 - pDOC->GetContractLevel()));
*/
	strTemp.Format("  %d Possible Loser%s(vs. %d allowable)\n",
				   m_numLosers,
				   (m_numLosers > 1)? "s " : " ",
					7 - pDOC->GetContractLevel());
	strLine += strTemp;		 

	//	strLine += "\n====================\n";
	//
	// done
	//
	status << strLine;
	status.ClearStreamOutputLevel();
}





//
//----------------------------------------------------------
//
// ReevaluateHoldings()
//
// called after a card has been played in the round
//
void CCombinedHoldings::ReevaluateHoldings(const CCard* pCard)
{
	// re-evaluate the suit
	if (pCard)
	{
		m_pSuit[pCard->GetSuit()]->ReevaluateHoldings(pCard);
	}
	else
	{
		for(int i=0;i<4;i++)
			m_pSuit[i]->ReevaluateHoldings(pCard);
	}

	// re-count the points
	CountPoints(TRUE);

	//
	// re-sort suits by length (and also by suit rank)
	//
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<4;i++) 
		m_nSuitsByLength[i] = 3-i;	// init, high suit to low suit
	// 
	for(i=0;i<4;i++) 
	{
		for(int j=0;j<3;j++) 
		{
			// move the suit back if it's shorter
			if (m_pSuit[m_nSuitsByLength[j]]->GetLength() < m_pSuit[m_nSuitsByLength[j+1]]->GetLength())
			{
				// swap suits and move this one back
				int nTemp = m_nSuitsByLength[j];
				m_nSuitsByLength[j] = m_nSuitsByLength[j+1];
				m_nSuitsByLength[j+1] = nTemp;
			}
		}
	}
	m_nLongestSuit = m_nSuitsByLength[0];

	// adjust winners count
	m_numWinners = 0;
	m_numLikelyWinners = 0;
	m_numMaxWinners = 0;
	m_numMaxTopCards = 0;
	m_numMaxLikelyWinners = 0;
	m_numSureWinners = 0; // NCR-587
	for(i=0;i<4;i++) 
	{
		m_numWinners += m_pSuit[i]->GetNumWinners();
		m_numLikelyWinners += m_pSuit[i]->GetNumLikelyWinners();
		m_numMaxWinners += m_pSuit[i]->GetNumMaxWinners();
		m_numMaxLikelyWinners += m_pSuit[i]->GetNumMaxLikelyWinners();
		m_numMaxTopCards += m_pSuit[i]->GetNumMaxTopCards();
		m_numSureWinners += m_pSuit[i]->GetNumSureWinners(); // NCR-587
	}

	// adjust max winners
	// e.g., AK/Q Q/AK = 3 cards in each hand, 2+2 max winners? no!
	m_numMaxWinners = Min(m_numMaxWinners, m_numCards);

	//
	// NCR-11 Losers need to be computed looking at both hands, not individually
	if((m_numCards > 26))   // We'll do this incrementally starting a the last 12 cards
	{
		CCombinedSuitHoldings* pSpades = m_pSuit[SPADES]; // DEBUG CODE
		CCombinedSuitHoldings* pHearts = m_pSuit[HEARTS]; // DEBUG CODE
		CCombinedSuitHoldings* pDiamonds = m_pSuit[DIAMONDS]; // DEBUG CODE
		CCombinedSuitHoldings* pClubs = m_pSuit[CLUBS]; // DEBUG CODE
		int nSW = pSpades->GetNumWinners();
		int nSS = pSpades->GetNumSequences(); 

		CGuessedCardHoldings playedCardsList;
		int numPlayed = pSpades->GetCardsPlayedInSuit(playedCardsList);

		int nHS = pHearts->GetNumSequences(); 
		int nDS = pDiamonds->GetNumSequences(); 
		int nCS = pClubs->GetNumSequences(); 
		int nHW = pHearts->GetNumWinners();
		playedCardsList.Clear(FALSE);
	} // NCR-11 end computing number of losers

	int numTricksLeft = (m_numCards+1)/2;   // NCR-11 there are cards for two hands here
	m_numLosers = numTricksLeft - m_numWinners;
	m_numLikelyLosers = numTricksLeft - m_numLikelyWinners;

	// determine the declarer/dummy split of winners
	m_numDeclarerWinners = 0;
	m_numDummyWinners = 0;
	m_numDeclarerTopCards = 0;
	m_numDummyTopCards = 0;
	for(i=0;i<4;i++) 
	{
		m_numDeclarerWinners += m_pSuit[i]->GetNumDeclarerWinners();
		m_numDummyWinners += m_pSuit[i]->GetNumDummyWinners();
		m_numDeclarerTopCards += m_pSuit[i]->GetNumDeclarerTopCards();
		m_numDummyTopCards += m_pSuit[i]->GetNumDummyTopCards();
	}
	ASSERT((m_numDeclarerWinners+m_numDummyWinners) == m_numSureWinners); // NCR-613 Sure winners
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
int CCombinedHoldings::RankSuits(int numSuits, int nArbitrate, int nSuit1, int nSuit2, 
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
			if (m_pSuit[nSuit[j+1]]->GetLength() > m_pSuit[nSuit[j]]->GetLength()) 
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
			int nLen1 = m_pSuit[nSuit[j]]->GetLength();
			int nLen2 = m_pSuit[nSuit[j+1]]->GetLength();
			int nLD = nLen1 - nLen2; 
			double fPts1 = m_pSuit[nSuit[j]]->GetHCPoints();
			double fPts2 = m_pSuit[nSuit[j+1]]->GetHCPoints();
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
BOOL CCombinedHoldings::AllSuitsStopped(BOOL bIncludeProbStoppers) const
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
// GetLongestSuit()
//
int	CCombinedHoldings::GetLongestSuit(int nType) const
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
	}
	// should never get here!
	ASSERT(FALSE);
	return NONE;
}





//====================================================================
//
// Value Set/Retrieval
//
//====================================================================

//
LPVOID CCombinedHoldings::GetValuePV(int nItem, int nIndex1, int nIndex2, int nIndex3) const
{
   // NCR-38 Uncommented and Added items to this switch

	switch (nItem)
	{
		case tnumSuitsStopped:		
			return (LPVOID) m_numSuitsStopped;
		case tnumSuitsProbStopped:	
			return (LPVOID) m_numSuitsProbStopped;
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
		default:
			AfxMessageBox("Unhandled Call to CCombinedHoldings::GetValuePV() ");
			return NULL;
	}

	return NULL;
}

//
double CCombinedHoldings::GetValueDouble(int nItem, int nIndex1, int nIndex2, int nIndex3) const
{
	switch (nItem)
	{
		case tfQuickTricks:	
			return m_numQuickTricks;
		default:
			AfxMessageBox("Unhandled Call to CCombinedHoldings::GetValue");
			return 1;
	}
	return 0;
}

//
// SetValuePV()
//
int CCombinedHoldings::SetValuePV(int nItem, LPVOID value, int nIndex1, int nIndex2, int nIndex3)
{
	int nVal = (int) value;
	BOOL bVal = (BOOL) value;
	LPCTSTR sVal = (LPCTSTR) value;
/*
	//
	switch (nItem)
	{
		//
		default:
			AfxMessageBox("Unhandled Call to CCombinedHoldings::SetValue()");
			return 1;
	}
*/
	return 0;
}

// "double" version
int CCombinedHoldings::SetValue(int nItem, double fValue, int nIndex1, int nIndex2, int nIndex3)
{
//	switch (nItem)
//	{
//		default:
			AfxMessageBox("Unhandled Call to CCombinedHoldings::SetValue()");
			return 1;
//	}
//	return 0;
}

//
int CCombinedHoldings::GetValue(int nItem, int nIndex1, int nIndex2, int nIndex3) const
{
	return (int) GetValuePV(nItem, nIndex1, nIndex2, nIndex3);
}

int CCombinedHoldings::SetValue(int nItem, int nValue, int nIndex1, int nIndex2, int nIndex3)
{
	return SetValuePV(nItem, (LPVOID) nValue, nIndex1, nIndex2, nIndex3);
}

