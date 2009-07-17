//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CHandHoldings
//
#ifndef __HANDHOLDINGS__
#define __HANDHOLDINGS__

#include "SuitHoldings.h"
#include "DisplayHoldings.h"

class CPlayerStatusDialog;

//
class CHandHoldings : public CCardHoldings {

	friend class CBidEngine;

// public routines
public:
	//
	virtual void InitNewHand();
	virtual void InitSwappedHand();
	virtual void ClearHand(BOOL bClearInitialHand=TRUE);
	virtual void Add(CCard* pCard, const BOOL bSort=FALSE);
	virtual void AddToInitialHand(CCard* pCard, const BOOL bSort=FALSE);
	virtual CCard* RemoveByIndex(const int nIndex);
	virtual void Sort();
	//
	virtual double CountPoints(const BOOL bForceCount=FALSE);
	virtual void EvaluateHoldings();
	virtual void ReevaluateHoldings(const CCard* pCard=NULL);
	virtual void FormatHoldingsString();
	//
	virtual void CountCards();
	//
	void Initialize(CPlayer* pPlayer, CPlayerStatusDialog* pStatusDlg);
	LPVOID GetValuePV(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	double GetValueDouble(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	int GetValue(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	int SetValuePV(int nItem, LPVOID value, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, double fValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, int nValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	void SetInitialHand(const CCardHoldings& srcCards) { m_initialHand.Clear(); m_initialHand << srcCards; };
	void SetInitialHandCard(int nIndex, CCard* pCard) { m_initialHand.SetCard(nIndex, pCard); };
	void RestoreInitialHand();
	void RestartBidding();
	//
	CCard* GetCardInSuit(int nSuit, int nIndex) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit][nIndex]; }
	CCard* GetInitialHandCard(int nIndex) const { return m_initialHand[nIndex]; }
	CCard* GetDisplayedCard(int nIndex) const { return m_displayHand[nIndex]; }
	CCardHoldings& GetInitialHand() { return m_initialHand; }
	double RevalueHand(int nMode, int nSuit, BOOL bTrace, BOOL bForceRevalue=FALSE);
	//
	BOOL AllSuitsStopped(BOOL bIncludeProbStoppers=FALSE) const;
	BOOL AllOtherSuitsStopped(int nSuit1, int nSuit2=NONE, int nSuit3=NONE, BOOL bIncludeProbStoppers=FALSE) const;
	BOOL IsBalanced() const;
	BOOL IsSemiBalanced() const;
	bool HasWorthlessDoubleton(int exceptSuit=NONE) const; // NCR NCR-304
	//
	int RankSuits(int numSuits, int nArbitrate, int nSuit1, int nSuit2, 
								int nSuit3=NONE, int nSuit4=NONE, int* pOrder=NULL);
	CCard* GetDiscard();

	//
	// inline functions
	//
	CCard* GetCardByPosition(int nSuit, int nPos) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit][nPos]; }
	CCard* GetCardByValue(int nSuit, int nFaceValue) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].FindCardByFaceValue(nFaceValue); }
	CCard* GetCardByValue(int nCardValue) const { return GetCardByValue(CARDSUIT(nCardValue), FACEVAL(nCardValue)); }
	CSuitHoldings& GetSuit(int nSuit) { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit]; }
	BOOL AreCardsExposed() const { return m_bCardsExposed; }
	void ExposeCards(BOOL bExposed);
	int GetNumCardsInSuit(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].GetNumCards(); }
	int GetNumTrumps() const;
	double GetNumQuickTricks() const { return m_numQuickTricks; }
	int GetNumHonorsInSuit(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].GetNumHonors(); }
	int GetNumPseudoHonorsInSuit(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].GetNumPseudoHonors(); }
	int	GetBalanceValue() const { return m_nBalanceValue; }
	int GetNumSuitsStopped() const { return m_numSuitsStopped; }
	int GetNumSuitsProbStopped() const { return m_numSuitsProbStopped; }
	int GetNumOpenableSuits() const { return m_numOpenableSuits; }
	int GetNumPreferredSuits() const { return m_numPreferredSuits; }
	int GetNumSolidSuits() const { return m_numSolidSuits; }
	int GetNumAbsoluteSuits() const { return m_numAbsoluteSuits; }
	int GetNumRebiddableSuits() const { return m_numRebiddableSuits; }
	int GetSuitRank(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].GetRank(); }
	int	GetPreferredSuit() const { return m_nPreferredSuit; }
	int	GetSuitsByPreference(int nIndex) const { return m_nSuitsByPreference[nIndex]; }
	int	GetLongestSuit(int nType=SUIT_ANY) const;
	int	GetSuitsByLength(int nIndex) const { return m_nSuitsByLength[nIndex]; }
	//
	int GetLowestMarginalSuit() const { return m_nLowestMarginalSuit; }
	int GetLowestOpenableSuit() const { return m_nLowestOpenableSuit; }
	int GetLowestStrongSuit() const { return m_nLowestStrongSuit; }
	int GetLowestPreferredSuit() const { return m_nLowestPreferredSuit; }
	int GetLowestSolidSuit() const { return m_nLowestSolidSuit; }
	int GetLowestAbsoluteSuit() const { return m_nLowestAbsoluteSuit; }
	int GetHighestMarginalSuit() const { return m_nHighestMarginalSuit; }
	int GetHighestOpenableSuit() const { return m_nHighestOpenableSuit; }
	int GetHighestStrongSuit() const { return m_nHighestStrongSuit; }
	int GetHighestPreferredSuit() const { return m_nHighestPreferredSuit; }
	int GetHighestSolidSuit() const { return m_nHighestSolidSuit; }
	int GetHighestAbsoluteSuit() const { return m_nHighestAbsoluteSuit; }
	//
	int GetNumVoids() const { return m_numVoids; }
	int GetNumSingletons() const { return m_numSingletons; }
	int GetNumDoubletons() const { return m_numDoubletons; }
	//
	double GetLengthPoints() const { return m_numLongPoints; }
	double GetShortPoints() const { return m_numShortPoints; }
	double GetDistPoints() const { return m_numDistPoints; }
	double GetBonusPoints() const { return m_numBonusPoints; }
	double GetPenaltyPoints() const { return m_numPenaltyPoints; }
	double GetTotalPoints() const { return m_numTotalPoints; }
	double GetTotalAdjustedPoints() const { return m_numAdjustedPoints; }
	int	GetNumLikelyWinners() const { return m_numLikelyWinners; }
	int	GetNumLikelyLosers() const { return m_numLikelyLosers; }
	int	GetNumWinners() const { return m_numWinners; }
	int	GetNumLosers() const { return m_numLosers; }
	int	GetNumWinnersInSuit(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].GetNumWinners(); }
	int	GetNumLosersInSuit(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].GetNumLosers(); }
	int	GetNumTopCards() const { return m_numTopCards; }
	//
	int	GetSuitStrength(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].GetStrength(); }
	int	GetSuitLength(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].GetNumCards(); }
	double GetSuitHCPoints(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].GetHCPoints(); }
	double GetSuitTotalPoints(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].GetTotalPoints(); }
	int GetNumSuitsOfAtLeast(int nLength) const { return m_numSuitsOfAtLeast[nLength]; }
	//
	int GetNumAces() const { return m_numAces; }
	int GetNumKings() const { return m_numKings; }
	int GetNumQueens() const { return m_numQueens; }
	int GetNumJacks() const { return m_numJacks; }
	int GetNumTens() const { return m_numTens; }
	//
	BOOL SuitHasCard(int nSuit, int nCardFaceValue) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].HasCardOfFaceValue(nCardFaceValue); }
	BOOL SuitHasAce(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].HasAce(); }
	BOOL SuitHasKing(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].HasKing(); }
	BOOL SuitHasQueen(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].HasQueen(); }
	BOOL SuitHasJack(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].HasJack(); }
	BOOL SuitHasTen(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].HasTen(); }
	//
	BOOL IsSuitVoid(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].IsVoid(); }
	BOOL IsSuitSingleton(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].IsSingleton(); }
	BOOL IsSuitDoubleton(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].IsDoubleton(); }
	//
	BOOL IsSuitMarginal(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].IsMarginal(); }
	BOOL IsSuitOpenable(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].IsOpenable(); }
	BOOL IsSuitPreferred(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].IsPreferred(); }
	BOOL IsSuitStrong(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].IsStrong(); }
	BOOL IsSuitAbsolute(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].IsAbsolute(); }
	BOOL IsSuitSelfSupporting(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].IsSelfSupporting(); }
	BOOL IsSuitSolid(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].IsSolid(); }
	BOOL IsSuitRebiddable(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].IsRebiddable(); }
	BOOL IsSuitStopped(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].IsSuitStopped(); }
	BOOL IsSuitProbablyStopped(int nSuit) const { VERIFY((nSuit>=0)&&(nSuit<=3)); return m_suit[nSuit].IsSuitProbablyStopped(); }
	//
	LPCTSTR GetStoppedSuitNames() const { return m_strSuitsStopped; }
	LPCTSTR GetProbStoppedSuitNames() const { return m_strSuitsProbStopped; }
	LPCTSTR GetUnstoppedSuitNames() const { return m_strSuitsUnstopped; }
	//
	void SetDummyFlag(BOOL bValue=TRUE) { m_displayHand.SetDummyFlag(bValue); }


// public data
public:

// private data
private:
	//
	CPlayer*				m_pPlayer;
	CPlayerStatusDialog*	m_pStatusDlg;
	//
	// card holdings
	// m_cards[] is inherited from CCardHoldings
	CSuitHoldings	 m_suit[4];
	CCardHoldings	 m_initialHand;		// original hand (saved)
	CDisplayHoldings m_displayHand;		// hand sorted in display order
	CString			 m_strHolding;		// description of hand
	CString			 m_strScreenHolding;	// as above, sorted by display order
	BOOL			 m_bCardsExposed;

	// basic hand statistics
	int			m_numAces;			// # of Aces in hand
	int			m_numKings;			// # of Kings in hand
	int			m_numQueens;		// # of Queens in hand
	int			m_numJacks;			// # of Jacks in hand
	int			m_numTens;			// # of Tens in hand
//	double		m_numHCPoints;	    // this is present in the base class
	double		m_numShortPoints;	// # of short suit points in hand
	double		m_numLongPoints;	// # of long suit points in hand
	double		m_numDistPoints;	// short points + long points
	double		m_numBonusPoints;	// bonuese (e.g., long trump suit)
	double		m_numPenaltyPoints;	// negatives (e.g., aceless hand)
	double		m_numTotalPoints;	// # of total points in hand
	double		m_numAdjustedPoints;// adjusted points for suit contract
	int 		m_nBalanceValue;	// A measure of hand balance
									// 0=3/3/3/4, 1=4/4/3/2, 2=5/3/3/2

	// more detailed hand analysis
	double		m_numQuickTricks;		// # of quick tricks
	int 		m_numSuitsStopped;		// # of stoppers
	int 		m_numSuitsProbStopped;	// # of probable stoppers
	int 		m_numSuitsUnstopped;
	int 		m_nSuitsStopped[4];
	int 		m_nSuitsProbStopped[4];
	int 		m_nSuitsUnstopped[4];
	CString		m_strSuitsStopped;
	CString		m_strSuitsProbStopped;
	CString		m_strSuitsUnstopped;
	//
	int			m_numRebiddableSuits;	//
	int			m_numMarginalSuits;		// marginally openable suits
	int			m_numOpenableSuits;		// openable suits
	int			m_numStrongSuits;		// strong openable suits
	int			m_numAbsoluteSuits;		// absolute opener suits
	int			m_numPreferredSuits;
	int			m_numSolidSuits;		// solid openable suits
	int			m_nLowestMarginalSuit;
	int			m_nLowestOpenableSuit;	
	int			m_nLowestStrongSuit;
	int			m_nLowestAbsoluteSuit;
	int			m_nLowestPreferredSuit;
	int			m_nLowestSolidSuit;
	int			m_nMarginalSuitList[5];
	int			m_nOpenableSuitList[5];
	int			m_nStrongSuitList[5];
	int			m_nAbsoluteSuitList[5];
	int			m_nPreferredSuitList[5];
	int			m_nSolidSuitList[5];
	//
	int			m_nPreferredSuit;			// always have a preferred suit
	int			m_nSuitsByPreference[5];	// ranked in order of preference
	int			m_nLongestSuit;			
	int			m_nSuitsByLength[4];
	//
	int			m_nHighestMarginalSuit;
	int			m_nHighestOpenableSuit;
	int			m_nHighestStrongSuit;
	int			m_nHighestAbsoluteSuit;
	int			m_nHighestPreferredSuit;
	int			m_nHighestSolidSuit;
	//
	int			m_numVoids;				// # of void suits
	int			m_nVoidSuits[4];
	int			m_numSingletons;		// # of singleton suits
	int			m_nSingletonSuits[4];
	int			m_numDoubletons;		// # of doubleton suits
	int			m_nDoubletonSuits[4];
	int			m_numSuitsOfAtLeast[14]; // # of suits of >=x # cards
										// i.e., x[2] = # of suits with >= 2 cards
	int			m_numLikelyWinners;		// likely winning cards
	int			m_numLikelyLosers;		// likely losers
	int			m_numWinners;			// sure winning cards
	int			m_numLosers;			// sure losers
	int			m_numTopCards;			// top cards
	//
	BOOL		m_bRevaluedForSuit[4];
	int         m_nRevaluedForPlayer[4]; // NCR-268 dummy or declarer?
	double      m_nAdjPointsBySuit[4];   // NCR-468 Adjusted points for each suit

public:
	CHandHoldings();
	~CHandHoldings();
};

#endif