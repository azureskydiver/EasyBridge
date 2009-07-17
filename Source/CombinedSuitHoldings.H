//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CCombinedSuitHoldings
//
#ifndef __COMBINEDSUITHOLDINGS__
#define __COMBINEDSUITHOLDINGS__

#include "SuitHoldings.h"

class CCard;

//
class CCombinedSuitHoldings : public CSuitHoldings {

// public routines
public:
	virtual void Init();
	virtual void Clear();
	virtual void Add(CCard* pCard, const BOOL bSort=FALSE);
	virtual void AddFromSource(CCard* pCard, const BOOL bPlayerCard, const BOOL bSort=FALSE);
	virtual void RemoveFromSource(CCard* pCard, const BOOL bPlayerCard);
	virtual CCard* RemoveByIndex(const int nIndex);
	virtual void Sort();
	//
	virtual double CountPoints(const BOOL bForceCount=FALSE);
	virtual int CheckKeyHoldings();
	virtual void EvaluateHoldings();
	virtual void ReevaluateHoldings(const CCard* pCard=NULL);
	virtual void FormatHoldingsString();
	virtual void SetSuit(int nSuit);
	//
	virtual int GetNumDeclarerEntries() { return m_numDeclarerEntries; }
	virtual int GetNumDummyEntries() { return m_numDummyEntries; }

	// inline functions
	virtual CCard* GetDeclarerCard(int nIndex) const { return m_declarerCards[nIndex]; }
	virtual CCard* GetDummyCard(int nIndex) const { return m_dummyCards[nIndex]; }
	CSuitHoldings& GetDeclarerSuit() { return m_declarerCards; }
	CSuitHoldings& GetDummySuit() { return m_dummyCards; }
	virtual int GetNumDeclarerCards() const { return m_nDeclarerLength; }
	virtual int GetNumDummyCards() const { return m_nDummyLength; }
	virtual int GetDeclarerLength() const { return m_nDeclarerLength; }
	virtual int GetDummyLength() const { return m_nDummyLength; }
	virtual int GetMinLength() const { return m_nMinLength; }
	virtual int GetMaxLength() const { return m_nMaxLength; }
	virtual int GetLengthDiff() const { return m_nDeclarerLength - m_nDummyLength; }
	//
	virtual int GetNumOutstandingCards() const { return m_numOutstandingCards; }
	virtual int GetNumMaxWinners() const { return m_numMaxWinners; }
	virtual int GetNumMaxLikelyWinners() const { return m_numMaxLikelyWinners; }
	virtual int GetNumSureWinners() const { return m_numSureWinners; } // NCR-587
	virtual int GetNumMaxTopCards() const { return m_numMaxTopCards; }
	virtual int GetNumDeclarerWinners() const { return m_numDeclarerWinners; }
	virtual int GetNumDummyWinners() const { return m_numDummyWinners; }
	virtual int GetNumExcessWinners() const { return m_numExcessWinners; }
	virtual int GetNumDeclarerExcessWinners() const { return m_numDeclarerExcessWinners; }
	virtual int GetNumDummyExcessWinners() const { return m_numDummyExcessWinners; }
	virtual int GetNumDeclarerLosers() const { return m_numDeclarerLosers; }
	virtual int GetNumDummyLosers() const { return m_numDummyLosers; }
	virtual int GetNumDeclarerTopCards() const { return m_numDeclarerTopCards; }
	virtual int GetNumDummyTopCards() const { return m_numDummyTopCards; }
	virtual int GetTopCardsDiff() const { return m_numDeclarerTopCards - m_numDummyTopCards; }
	virtual int GetWinnersDiff() const { return m_numDeclarerWinners - m_numDummyLosers; }
	//
	virtual const CCardList& GetWinners() const { return m_winners; }
	virtual const CCardList& GetLosers() const { return m_losers; }
	virtual const CCardList& GetDeclarerWinners() const { return m_declarerWinners; }
	virtual const CCardList& GetDummyWinners() const { return m_dummyWinners; }
	virtual const CCardList& GetDeclarerLosers() const { return m_declarerLosers; }
	virtual const CCardList& GetDummyLosers() const { return m_dummyLosers; }
	//
	int	GetMissingCardVal(int nIndex) { VERIFY(nIndex <= m_numOutstandingCards); return m_nMissingCards[nIndex]; }

private:
	void CompareHands();


// private data
private:
	CSuitHoldings	m_declarerCards;
	CSuitHoldings	m_dummyCards;
	//
	int		m_nDeclarerLength;
	int		m_nDummyLength;
	int		m_nMinLength;
	int		m_nMaxLength;
	int		m_nLongHand;		// 0 = declarer, 1 = dummy
	CSuitHoldings* m_pLongHand;
	int		m_numMaxWinners;
	int		m_numMaxLikelyWinners;
	int		m_numSureWinners; // NCR-587 These win from the top
	int		m_numMaxTopCards;
	int		m_numDeclarerWinners;
	int		m_numDummyWinners;
	int		m_numExcessWinners;
	int		m_numDeclarerExcessWinners;
	int		m_numDummyExcessWinners;
	int		m_numDeclarerLosers;
	int		m_numDummyLosers;
	int		m_numDeclarerTopCards;
	int		m_numDummyTopCards;
	int		m_numOutstandingCards;
	int		m_nMissingCards[13];
	//
	int		m_numDeclarerEntries;
	int		m_numDummyEntries;
	//
	CCardList	m_winners;
	CCardList	m_losers;
	CCardList	m_declarerWinners;
	CCardList	m_declarerLosers;
	CCardList	m_dummyWinners;
	CCardList	m_dummyLosers;

// construction/destruction
public:
	CCombinedSuitHoldings();
	~CCombinedSuitHoldings();
};

#endif