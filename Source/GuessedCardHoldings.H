//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CGuessedCardHoldings
//
//
#ifndef __CGUESSEDCARDHOLDINGS__
#define __CGUESSEDCARDHOLDINGS__

#include "GuessedCard.h"
class CCardList;

//
class CGuessedCardHoldings {

// public routines
public:
	virtual void Init();
	virtual void Clear(BOOL bDelete=TRUE);
	virtual void Add(CGuessedCard* pCard, BOOL bSort=FALSE);
	virtual void Merge(CGuessedCardHoldings* pList);
	virtual void operator<<(CGuessedCard* pCard) { Add(pCard); }
	virtual void operator<<(CGuessedCardHoldings* pList) { Merge(pList); }
	virtual CGuessedCard* RemoveByIndex(int nIndex);	// the main routine
	virtual CGuessedCard* RemoveByValue(int nDeckValue);
	virtual void Remove(CGuessedCard* pCard);
	virtual CGuessedCard* Remove(CCard* pCard);
	virtual void DeleteByIndex(int nIndex);
	virtual void DeleteByValue(int nDeckValue);
	virtual void Delete(CGuessedCard* pCard);
	virtual BOOL Delete(CCard* pCard);
	virtual void Sort();
	virtual void ReverseSortOrder(BOOL bReverse) { m_bReverseSort = bReverse; }
	virtual BOOL HasCard(CGuessedCard* pCard) const;
	virtual BOOL HasCard(int nDeckValue) const;
	virtual BOOL HasCardOfFaceValue(int nFaceValue) const;
	virtual int GetCardIndex(int nDeckValue) const;
	virtual int GetCardIndex(CGuessedCard* pCard) const;
	virtual int GetCardIndex(CCard* pCard) const;
	//
	virtual void FormatHoldingsString();

	//
	// inline functions
	//
	CGuessedCard* GetAt(int nIndex) const { VERIFY(nIndex < m_numTotalCards); return m_cards[nIndex]; }
	CGuessedCard* operator[](int nIndex) const { VERIFY(nIndex < m_numTotalCards); return m_cards[nIndex]; }
	CGuessedCard* GetTopCard() const { VERIFY(m_numTotalCards > 0); VERIFY(m_bSorted); return m_cards[0]; }
	CGuessedCard* GetBottomCard() const { VERIFY(m_numTotalCards > 0); VERIFY(m_bSorted); return m_cards[m_numTotalCards-1]; }
	CGuessedCard* GetSecondHighestCard() const { VERIFY(m_numTotalCards > 1); VERIFY(m_bSorted); return m_cards[1]; }
	CGuessedCard* GetSecondLowestCard() const { VERIFY(m_numTotalCards > 1); VERIFY(m_bSorted); return m_cards[m_numTotalCards-2]; }
	int GetTopCardVal() const { VERIFY(m_numTotalCards > 0); VERIFY(m_bSorted); return m_cards[0]->GetFaceValue(); }
	int GetBottomCardVal() const { VERIFY(m_numTotalCards > 0); VERIFY(m_bSorted); return m_cards[m_numTotalCards-1]->GetFaceValue(); }
	//
	int GetNumTotalCards() const { return m_numTotalCards; }
//	int SetNumTotalCards(int nVal) { m_numTotalCards = nVal; }
	int GetNumCardsPlayed() const { return m_numPlayedCards; }
//	int SetNumCardsPlayed(int nVal) { m_numPlayedCards = nVal; }
	int GetNumDefiniteCards() const { return m_numDefiniteCards; }
	void SetNumDefiniteCards(int nVal) { m_numDefiniteCards = nVal; }
	int GetNumLikelyCards() const { return m_numLikelyCards; }
	void SetNumLikelyCards(int nVal) { m_numLikelyCards = nVal; }
	//
	int GetNumRemainingCards() const { return m_numRemainingCards; }
	void SetNumRemainingCards(int nVal) { m_numRemainingCards = m_numMinRemainingCards = m_numMaxRemainingCards = nVal; }
	int GetNumMinRemainingCards() const { return m_numMinRemainingCards; }
	void SetNumMinRemainingCards(int nVal) { m_numMinRemainingCards = nVal; }
	int GetNumMaxRemainingCards() const { return m_numMaxRemainingCards; }
	void SetNumMaxRemainingCards(int nVal) { m_numMaxRemainingCards = nVal; }
	//
	int GetNumOriginalCards() const { return m_numOriginalCards; }
	void SetNumOriginalCards(int nVal) { m_numOriginalCards = m_numMinOriginalCards = m_numMaxOriginalCards = nVal; }
	int GetNumMinOriginalCards() const { return m_numMinOriginalCards; }
	void SetNumMinOriginalCards(int nVal) { m_numMinOriginalCards = nVal; }
	int GetNumMaxOriginalCards() const { return m_numMaxOriginalCards; }
	void SetNumMaxOriginalCards(int nVal) { m_numMaxOriginalCards = nVal; }
	//
	virtual int GetPlayedCardsList(CGuessedCardHoldings& cardList);
	virtual int GetIdentifiedCardsList(CGuessedCardHoldings& cardList);
	virtual int GetRemainingCardsList(CGuessedCardHoldings& cardList);
	//
	BOOL IsSorted() const { return m_bSorted; }
	void EnableAutoSort(BOOL bMode) { m_bAutoSort = bMode; }
	LPCTSTR GetHoldingsString() const { return m_strHoldings; }
	
// public data
public:

// protected data
protected:
	CGuessedCard* m_cards[MAXHOLDING];
	int		m_numDefiniteCards;		// definitely ID'd cards
	int		m_numLikelyCards;		// cards the player prob'ly holds
	int		m_numTotalCards;		// cards currently in the list
	int		m_numPlayedCards;		// cards that have been played
	int		m_numRemainingCards;	// cards the player has remaining
	int		m_numMinRemainingCards;	// min remaining cards
	int		m_numMaxRemainingCards;	// max remaining cards
	int		m_numOriginalCards;		// probalbe original cards
	int		m_numMinOriginalCards;	// max prob original cards
	int		m_numMaxOriginalCards;	// min prob original cards
	//
	int		m_bSorted;
	int		m_bAutoSort;
	BOOL	m_bReverseSort;
	CString m_strHoldings;
	//

// construction/destruction
public:
	CGuessedCardHoldings();
	~CGuessedCardHoldings();
};

#endif