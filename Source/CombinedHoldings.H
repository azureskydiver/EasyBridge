//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CCombinedHoldings
//
#ifndef __COMBINEDHOLDINGS__
#define __COMBINEDHOLDINGS__

#include "HandHoldings.h"

class CPlayerStatusDialog;
class CCombinedSuitHoldings;


//
class CCombinedHoldings : public CCardHoldings {

	friend class CPlayEngine;

// public routines
public:
	//
	virtual void InitNewHand();
	virtual void ClearHand();
	virtual void Add(CCard* pCard, const BOOL bSort=FALSE);
	virtual void AddFromSource(CCard* pCard, const BOOL bPlayerCard, const BOOL bSort=FALSE);
	virtual void RemoveFromSource(CCard* pCard, const BOOL bPlayerCard);
	virtual CCard* RemoveByIndex(const int nIndex);
	virtual void Sort();
	//
	virtual double CountPoints(const BOOL bForceCount=FALSE);
	virtual void EvaluateHoldings();
	virtual void ReevaluateHoldings(const CCard* pCard=NULL);
	virtual void FormatHoldingsString();
	//
	void Initialize(CPlayer* pPlayer, CPlayerStatusDialog* pStatusDlg, CHandHoldings* pHand, CHandHoldings* pPartnersHand);
	LPVOID GetValuePV(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	double GetValueDouble(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	int GetValue(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	int SetValuePV(int nItem, LPVOID value, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, double fValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, int nValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);

	// general utility functions
	int RankSuits(int numSuits, int nArbitrate, int nSuit1, int nSuit2, 
					   int nSuit3, int nSuit4, int* pOrder);
	BOOL AllSuitsStopped(BOOL bIncludeProbStoppers=FALSE) const;
	int	GetLongestSuit(int nType=SUIT_ANY) const;
	int	GetSuitsByLength(int nIndex) const { return m_nSuitsByLength[nIndex]; }

	//
	virtual CCard* GetCard(int nCardValue) const;
	virtual CCard* GetCard(int nSuit, int nPos) const;
	CCombinedSuitHoldings& GetSuit(int nSuit);
	int GetNumCardsInSuit(int nSuit) const;
	int GetNumTrumps() const;
	int GetNumDeclarerTrumps() const;
	int GetNumDummyTrumps() const;
	int GetNumPlayersCardsInSuit(int nSuit) const;
	int GetNumPartnersCardsInSuit(int nSuit) const;
	//
	CHandHoldings*	GetPlayerHand() { return m_pPlayerHand; }
	CHandHoldings*	GetPartnerHand() { return m_pPartnerHand; }
	//
	double GetTotalPoints() const { return m_numTotalPoints; }
	double GetTotalAdjustedPoints() const { return m_numAdjustedPoints; }
	double GetNumQuickTricks() const { return m_numQuickTricks; }
	int	GetNumLikelyWinners() const { return m_numLikelyWinners; }
	int	GetNumLikelyLosers() const { return m_numLikelyLosers; }
	int	GetNumWinners() const { return m_numWinners; }
	int	GetNumLosers() const { return m_numLosers; }
	int	GetNumMaxWinners() const { return m_numMaxWinners; }
	int GetNumMaxLikelyWinners() const { return m_numMaxLikelyWinners; }
	int GetNumSureWinners() const { return m_numSureWinners; } // NCR-587
	int	GetNumMaxTopCards() const { return m_numMaxTopCards; }
	int	GetNumDeclarerWinners() const { return m_numDeclarerWinners; }
	int	GetNumDummyWinners() const { return m_numDummyWinners; }
	int	GetNumDeclarerTopCards() const { return m_numDeclarerTopCards; }
	int	GetNumDummyTopCards() const { return m_numDummyTopCards; }
	int GetNumSuitsStopped() const { return m_numSuitsStopped; }   // NCR-38
	//
	int	GetNumWinnersInSuit(int nSuit) const;
	int	GetNumLosersInSuit(int nSuit) const;
	int	GetNumLikelyWinnersInSuit(int nSuit) const;
	int	GetNumLikelyLosersInSuit(int nSuit) const;
	int	GetNumDeclarerWinnersInSuit(int nSuit) const;
	int	GetNumDeclarerLosersInSuit(int nSuit) const;
	int	GetNumDummyWinnersInSuit(int nSuit) const;
	int	GetNumDummyLosersInSuit(int nSuit) const;

	//
	int	GetSuitStrength(int nSuit) const;
	int	GetSuitLength(int nSuit) const;
	double GetSuitHCPoints(int nSuit) const;
	double GetSuitTotalPoints(int nSuit) const;
	int GetNumSuitsOfAtLeast(int nLength) const;
	//
	int GetNumAces() const { return m_numAces; }
	int GetNumKings() const { return m_numKings; }
	int GetNumTopCards() const { return m_numTopCards; }
	int GetNumTopHonors() const { return m_numTopHonors; }
	int GetNumSecondaryHonors() const { return m_numSecondaryHonors; }
	int GetNumMissingHonors() const { return m_numMissingHonors; }
	int GetNumMissingTopHonors() const { return m_numMissingTopHonors; }
	int GetNumMissingInternalHonors() const { return m_numMissingInternalHonors; }
	int GetNumMissingBottomHonors() const { return m_numMissingBottomHonors; }
	//
	BOOL SuitHasCard(int nSuit, int nCardFaceValue) const;
	BOOL SuitHasAce(int nSuit) const;
	BOOL SuitHasKing(int nSuit) const;
	BOOL SuitHasQueen(int nSuit) const;
	BOOL SuitHasJack(int nSuit) const;
	BOOL SuitHasTen(int nSuit) const;
	BOOL IsSuitStopped(int nSuit) const;
	BOOL IsSuitProbablyStopped(int nSuit) const;
	//
	LPCTSTR GetStoppedSuitNames() const { return m_strSuitsStopped; }
	LPCTSTR GetProbStoppedSuitNames() const { return m_strSuitsProbStopped; }
	LPCTSTR GetUnstoppedSuitNames() const { return m_strSuitsUnstopped; }

// public data
public:

// private data
private:
	//
	CPlayer*				m_pPlayer;
	CPlayerStatusDialog*	m_pStatusDlg;
	CHandHoldings*			m_pPlayerHand;
	CHandHoldings*			m_pPartnerHand;
	//
	CString					m_strHolding;		// description of hand
	CCombinedSuitHoldings*	m_pSuit[4];

	// basic hand statistics
	int			m_numAces;			// # of Aces in hand
	int			m_numKings;			// # of Kings in hand
	double		m_numDistPoints;	// distribution pts 
	double		m_numTotalPoints;	// # of total points in hand
	double		m_numAdjustedPoints;// adjusted points for suit contract

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
	int			m_nContractSuit;			
	int			m_nPreferredSuit;			// always have a preferred suit
	int			m_nLongestSuit;			
	int			m_nSuitsByPreference[5];	// ranked in order of preference
	int			m_nSuitsByLength[4];
	int			m_numSuitsOfAtLeast[14]; // # of suits of >=x # cards
										// i.e., x[2] = # of suits with >= 2 cards
	//
	int			m_numMaxWinners;
	int			m_numMaxLikelyWinners;
	int			m_numSureWinners;  // NCR-587 These win from the top
	int			m_numMaxTopCards;
	int			m_numDeclarerWinners;
	int			m_numDummyWinners;
	int			m_numDeclarerTopCards;
	int			m_numDummyTopCards;
	int			m_numWinners;			// sure winning cards
	int			m_numLosers;			// sure losers
	int			m_numLikelyWinners;		// likely winning cards
	int			m_numLikelyLosers;		// likely losers
	//
	int			m_numTopCards;			
	int			m_numTopHonors;			
	int			m_numSecondaryHonors;			
	int			m_numMissingHonors;			
	int			m_numMissingTopHonors;			
	int			m_numMissingInternalHonors;
	int			m_numMissingBottomHonors;

public:
	CCombinedHoldings();
	~CCombinedHoldings();
};

#endif