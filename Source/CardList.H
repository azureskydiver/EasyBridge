//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CCardList
//
// base class -- a collection of cards
//
#ifndef __CARDLIST__
#define __CARDLIST__

class CCard;
class CPlayer;

//
class CCardList {

// public routines
public:
	virtual void Init();
	virtual void Clear();
	//
	virtual void Add(CCard* pCard, const BOOL bSort=FALSE);
	virtual void Merge(const CCardList& srcList);
	virtual void operator<<(CCard* pCard) { Add(pCard); }
	virtual void operator<<(const CCardList& srcList) { Merge(srcList); }
	virtual CCard* RemoveByIndex(const int nIndex);	// the main routine
	virtual CCard* RemoveByValue(const int nDeckValue);
	virtual void Remove(const CCard* pCard);
	virtual void Sort();
	virtual void ReverseSortOrder(const BOOL bReverse=TRUE) { m_bReverseSort = bReverse; }
	virtual void FormatHoldingsString();
	//
	virtual CCard* FindCard(const int nSuit, const int nFaceValue) const;
	virtual CCard* FindCardByDeckValue(const int nDeckValue) const;
	virtual CCard* FindCardByFaceValue(const int nFaceValue) const;
	virtual BOOL HasCard(const CCard* pCard) const;
	virtual BOOL HasCard(const int nDeckValue) const;
	virtual BOOL HasCardOfFaceValue(const int nFaceValue) const;
	virtual BOOL ContainsCards(const CCardList& cards) const;
	virtual BOOL ContainsAtLeastOneOf(const CCardList& cCards) const;
	virtual BOOL RangeCovers(const int nFaceValue) const;
	virtual BOOL RangeCovers(const CCard* pCard) const;
	virtual int GetCardIndex(const int nDeckValue) const;
	virtual int GetCardIndex(const CCard* pCard) const;
	virtual void SetCard(int nIndex, CCard* pCard);
	virtual int GetNumCardsAbove(const int nFaceValue) const;
	virtual int GetNumCardsBelow(const int nFaceValue) const;
	virtual int GetNumCardsAbove(const CCard* pCard) const;
	virtual int GetNumCardsBelow(const CCard* pCard) const;
	virtual int GetAllCardsAbove(const int nFaceValue, CCardList& cards) const;
	virtual int GetAllCardsBelow(const int nFaceValue, CCardList& cards) const;
	virtual int GetAllCardsAbove(const CCard* pCard, CCardList& cards) const;
	virtual int GetAllCardsBelow(const CCard* pCard, CCardList& cards) const;
	virtual CCard* GetLowestCardAbove(const int nFaceValue) const;
	virtual CCard* GetHighestCardBelow(const int nFaceValue) const;
	virtual CCard* GetLowestCardAbove(const CCard* pCard) const;
	virtual CCard* GetHighestCardBelow(const CCard* pCard) const;
	virtual int	   GetEquivalentCards(CCard* pCard, CCardList& cardList, const BOOL bIncludeCard=FALSE);
	virtual BOOL   AreEquivalentCards(CCard* pCard1, CCard* pCard2);
	virtual CCard* GetHighestEquivalentCard(CCard* pCard);
	virtual CCard* GetLowestEquivalentCard(CCard* pCard);

	//
	BOOL   operator==(const CCardList& srcList) const;
	BOOL   operator!=(const CCardList& srcList) const;
	//
	CCard* operator[](int nIndex) const;
	CCard* GetAt(int nIndex) const;
	CCard* GetTopCard() const;
	CCard* GetBottomCard() const;
	CCard* GetSecondHighestCard() const;
	CCard* GetSecondLowestCard() const;
	int GetTopCardVal() const;
	int GetBottomCardVal() const;
	int GetNumCards() const { return m_numCards; }
	BOOL IsEmpty() { return (m_numCards == 0); }
	BOOL IsSorted() const { return m_bSorted; }
	void EnableAutoSort(BOOL bMode) { m_bAutoSort = bMode; }
	LPCTSTR GetHoldingsString() const { return m_strHoldings; }
	CString GetHoldingsCommaDelimited() const;

// protected routines
protected:
	virtual void TurnCardsFaceUp(const BOOL bCode);


// public data
public:

// private data
protected:
	CCard* m_cards[MAXHOLDING];
	int	m_numCards;
	int	m_bSorted;
	int	m_bAutoSort;
	BOOL m_bReverseSort;
	CString m_strHoldings;

// construction/destruction
public:
	CCardList();
	CCardList(const CCardList& srcList);
	~CCardList();
};

#endif