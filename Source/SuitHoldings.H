//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CSuitHoldings
//
#ifndef __SUITHOLDINGS__
#define __SUITHOLDINGS__

#include "cardholdings.h"
#include "bidparams.h"

class CCard;
class CGuessedCardHoldings;

//
class CSuitHoldings : public CCardHoldings {

// public routines
public:
	virtual void Init();
	virtual void Clear();
	virtual void Add(CCard* pCard, const BOOL bSort=FALSE);
	virtual CCard* RemoveByIndex(const int nIndex);
	virtual void Sort();
	//
	virtual double CountPoints(const BOOL bForceCount=FALSE);
	virtual void EvaluateHoldings();
	virtual void ReevaluateHoldings(const CCard* pCard=NULL);
	virtual void FormatHoldingsString();
	//
	virtual void CountCards();

	// class-specific functions
	virtual int	   CheckKeyHoldings();
	virtual CCard* GetLowestCardAbove(const int nFaceValue) const;
	virtual CCard* GetLowestCardAbove(CCard* pCard) const;
	virtual void   EnableSuitSorting(BOOL bValue=TRUE) { m_bEnableSuitSorting = bValue; }
	//
	// inline functions
	int GetSuit() const { return m_nSuit; }
	void SetSuit(int nSuit) { m_nSuit = nSuit; }
	int GetRank() const { return m_nRank; }
	void SetRank(int nRank) { m_nRank = nRank; }
	int GetQualityTest();
	LPCTSTR GetCompressedHoldingsString() { return m_strCompressedHoldings; }
	//
	int GetLength() const { return m_numCards; } // same as GetNumCards()
	int GetStrength() const { return m_nStrength; }
	double GetLengthPoints() const { return m_numLengthPoints; }
	double GetShortPoints() const { return m_numShortPoints; }
	double GetBonusPoints() const { return m_numBonusPoints; }
	double GetPenaltyPoints() const { return m_numPenaltyPoints; }
	double GetTotalPoints() const { return m_numTotalPoints; }
	//
	int GetNumHonors() const { return m_numHonors; }
	int GetNumPseudoHonors() const { return m_numPseudoHonors; }
	int GetNumBodyCards() const { return m_numBodyCards; }
	int GetNumTopCards() const { return m_numTopCards; }
	int GetNumTopHonors() const { return m_numTopHonors; }
	int GetNumSecondaryHonors() const { return m_numSecondaryHonors; }
	int GetNumMissingHonors() const { return m_numMissingHonors; }
	int GetNumMissingTopHonors() const { return m_numMissingTopHonors; }
	int GetNumMissingInternalHonors() const { return m_numMissingInternalHonors; }
	int GetNumMissingBottomHonors() const { return m_numMissingBottomHonors; }
	CCard* GetLowestTopHonor() const { return (m_numTopHonors > 0)? m_cards[m_numTopHonors-1] : NULL; }
	CCard* GetHighestSecondaryHonor() const { return (m_numTopHonors > 0)? m_cards[m_numTopHonors] : NULL; }

	// sequence operations
	virtual int GetNumSequences() const { return m_numSequences; }
	virtual CCardList& GetSequence(int nIndex) const { VERIFY(nIndex < m_numSequences); return *(m_sequenceList[nIndex]); }
	virtual CCardList& GetTopSequence() const { VERIFY(m_numSequences >= 1); return *(m_sequenceList[0]); }
	virtual CCardList& GetSecondSequence() const { VERIFY(m_numSequences > 1); return *(m_sequenceList[1]); }
	//
	virtual int GetNumMissingSequences() const { return m_numMissingSequences; }
	virtual CCardList& GetMissingSequence(int nIndex) const { VERIFY(nIndex < m_numMissingSequences); return *(m_missingSequenceList[nIndex]); }
	virtual CCardList& GetTopMissingSequence() const { VERIFY(m_numMissingSequences >= 1); return *(m_missingSequenceList[0]); }
	virtual CCardList& GetSecondMissingSequence() const { VERIFY(m_numMissingSequences > 1); return *(m_missingSequenceList[1]); }
	//
	virtual const CCardHoldings& GetLikelyWinners() const { return m_likelyWinners; }
	virtual const CCardHoldings& GetLikelyLosers() const { return m_likelyLosers; }

	//
	double GetNumQuickTricks() const { return m_numQuickTricks; }
	int	GetNumLikelyWinners() const { return m_numLikelyWinners; }
	int	GetNumLikelyLosers() const { return m_numLikelyLosers; }
	int	GetNumWinners() const { return m_numWinners; }
	int	GetNumLosers() const { return m_numLosers; }
//	int	GetNumWinners() const { return m_numLikelyWinners; }
//	int	GetNumLosers() const { return m_numLikelyLosers; }

	//
	BOOL HasAce() const { return m_bHaveAce; }
	BOOL HasKing() const { return m_bHaveKing; }
	BOOL HasQueen() const { return HasCard(QUEEN); }
	BOOL HasJack() const { return HasCard(JACK); }
	BOOL HasTen() const { return HasCard(TEN); }
	//
	BOOL IsVoid() const { return (m_numCards == 0)? TRUE : FALSE; }
	BOOL IsSingleton() const { return (m_numCards == 1)? TRUE : FALSE; }
	BOOL IsDoubleton() const { return (m_numCards == 2)? TRUE : FALSE; }
	//
	BOOL IsMarginal() const { return (m_nStrength >= SS_MARGINAL_OPENER)? TRUE : FALSE; }
	BOOL IsOpenable() const { return (m_nStrength >= SS_OPENABLE)? TRUE : FALSE; }
	BOOL IsStrong() const { return (m_nStrength >= SS_ABSOLUTE)? TRUE : FALSE; }
	BOOL IsAbsolute() const { return (m_nStrength >= SS_ABSOLUTE)? TRUE : FALSE; }
	BOOL IsSelfSupporting() const { return m_bSelfSupporting; }
	BOOL IsSolid() const { return m_bSolid; }
	BOOL IsPreferred() const { return m_bPreferred; }
	void SetPreferred() { m_bPreferred = TRUE; }

	BOOL IsRebiddable() const { return m_bRebiddable; }
	BOOL IsSuitStopped() const { return m_bSuitStopped; }
	BOOL IsSuitProbablyStopped() const { return m_bSuitProbablyStopped; }
	int  GetNumStoppers() const { return m_numStoppers; }

	//
	int  GetCardsPlayedInSuit(CGuessedCardHoldings& playedCardsList);
	

// public data
public:

// private data
protected:
	int m_nSuit;
	int m_nRank;
	int m_nStrength;
	double m_numLengthPoints;
	double m_numShortPoints;
	double m_numBonusPoints;
	double m_numPenaltyPoints;
	double m_numTotalPoints;
	// honors & tenace info
	int m_numHonors;
	// tenace info
	int m_numPseudoHonors;
	int m_numBodyCards;
	int	m_nHighestHonor;
	int	m_nLowestHonor;
	int	m_nLowestPseudoHonor;
	int	m_nMissingHonors[5];
	int	m_numMissingHonors;			// e.g., in AQx, 3 (counting the Jack)
	int	m_numMissingTopHonors;		// e.g., in QJT, 2
	int	m_numMissingInternalHonors; // e.g., in AKT, 2
	int	m_numMissingBottomHonors;	// e.g., in AQx, 2 
	int	m_numTopHonors;				// e.g., in AKxx, 2 (the AK)
	int	m_numSecondaryHonors;		// e.g., in AQTx, 2 (the QT)
	int	m_numTopCards;				// e.g., in AKQJT9, 6
	int m_numSequences;
	int m_numMissingSequences;
	CCardHoldings m_honors;
	CCardHoldings m_bodyCards;
	CCardHoldings m_likelyWinners;
	CCardHoldings m_likelyLosers;
	CTypedPtrArray<CPtrArray, CCardList*> m_sequenceList;
	CTypedPtrArray<CPtrArray, CCardList*> m_missingSequenceList;
	//
	BOOL m_bHaveAce;
	BOOL m_bHaveKing;
	double m_numQuickTricks;
	int m_numStoppers;
	int m_numLikelyWinners;
	int m_numLikelyLosers;
	int	m_numWinners;		// likely winning cards
	int	m_numLosers;		// likely losers
	//
	BOOL m_bEnableSuitSorting;	// sorting within a suit
	//
	BOOL m_bSolid;
	BOOL m_bPreferred;
	BOOL m_bRebiddable;
	BOOL m_bSelfSupporting;
	BOOL m_bSuitStopped;
	BOOL m_bSuitProbablyStopped;
	//
	CString m_strCompressedHoldings;

// construction/destruction
public:
	CSuitHoldings();
	~CSuitHoldings();
};

#endif