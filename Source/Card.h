//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Card.h
//

#ifndef __CARD__
#define __CARD__


// file constants
extern const TCHAR cSuit[];
extern const TCHAR cCard[];
extern const TCHAR cFaceCard[];
extern const LPCTSTR szFaceValueName[];

class CDeck;


class CCard {

	friend class CDeck;

// public functions
public:
	// one-time only init operation
	void Initialize(int nSuit, int nValue, CBitmap* pBitmap, CDC* pDC);
	void SetBitmap(CBitmap* pBitmap, CDC* pDC);
	//
	void Clear();
	void ClearBackground();
	LPVOID GetValuePV(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	int GetValueInt(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	LPCTSTR GetValueString(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	char GetValueChar(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	int GetValue(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	int SetValuePV(int nItem, LPVOID value, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValueInt(int nItem, int nValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValueString(int nItem, LPCTSTR szValue=NULL, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValueChar(int nItem, char cValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, int nValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	BOOL IsValid() const;

	// GetName() -- "D7"
	LPCTSTR GetName()  const { return m_strName; }
	// GetFullName() -- "7 of Diamonds"
	LPCTSTR GetFullName()  const { return m_strFullName; }
	// GetReverseFullName() -- "Diamond 7"
	LPCTSTR GetReverseFullName()  const { return m_strReverseFullName; }
	// GetFaceName() -- "seven"
	LPCTSTR GetFaceName()  const { return m_strFaceName; }
	// GetCardLetter() -- "7"
	TCHAR GetCardLetter() const;
	// GetSuitLetter() -- "D"
	TCHAR GetSuitLetter() const;

	//
	void GetRect(RECT& rect) const;
	void Draw(CDC *pDC);
	void DrawHighlighted(CDC* pDC, BOOL bVisible);
	void FlashCard(CDC* pDC, int numTimes=2);
	void RestoreBackground(CDC* pDC);
	void MoveTo(CDC *pDC, int destX, int destY, BOOL bRedraw=TRUE);
	void DragTo(CDC* pDC, int destX, int destY);
	void Animate(CDC* pDC, int destx, int desty, BOOL bClearAtEnd=FALSE, int nGranularity=-99);
	int GetDisplayValue() const;
	int GetDummyDisplayValue() const;

	// overloaded operators
	BOOL operator>(CCard& compareCard) { return (m_nDeckValue > compareCard.GetDeckValue())? TRUE : FALSE; }
	BOOL operator<(CCard& compareCard) { return (m_nDeckValue < compareCard.GetDeckValue())? TRUE : FALSE; }
	BOOL operator==(CCard& compareCard) { return (m_nDeckValue == compareCard.GetDeckValue())? TRUE : FALSE; }
	BOOL operator>=(CCard& compareCard) { return (m_nDeckValue >= compareCard.GetDeckValue())? TRUE : FALSE; }
	BOOL operator<=(CCard& compareCard) { return (m_nDeckValue <= compareCard.GetDeckValue())? TRUE : FALSE; }
	BOOL operator>(int nFaceValue) { VERIFY(nFaceValue <= ACE); return (m_nFaceValue > nFaceValue)? TRUE : FALSE; }
	BOOL operator<(int nFaceValue) { VERIFY(nFaceValue <= ACE); return (m_nFaceValue < nFaceValue)? TRUE : FALSE; }
	BOOL operator==(int nFaceValue) { VERIFY(nFaceValue <= ACE); return (m_nFaceValue == nFaceValue)? TRUE : FALSE; }
	BOOL operator>=(int nFaceValue) { VERIFY(nFaceValue <= ACE); return (m_nFaceValue >= nFaceValue)? TRUE : FALSE; }
	BOOL operator<=(int nFaceValue) { VERIFY(nFaceValue <= ACE); return (m_nFaceValue <= nFaceValue)? TRUE : FALSE; }
	int operator+(int nVal) { return (m_nFaceValue + nVal); }
	int operator-(int nVal) { return (m_nFaceValue - nVal); }
	int operator+(CCard& card) { return (m_nFaceValue + card.GetFaceValue()); }
	int operator-(CCard& card) { return (m_nFaceValue - card.GetFaceValue()); }

	// inline functions
	int GetFaceValue() const { return m_nFaceValue; }
	int GetDeckValue() const { return m_nDeckValue; }
	int GetSortedDeckIndex() const { return (m_nSuit*13 + m_nFaceValue-2); }
	int GetDisplayPosition() const { return m_nDisplayPosition; }
	void SetDisplayPosition(int nPos) { ASSERT(nPos >= 0); m_nDisplayPosition = nPos; }
	void DecrementDisplayPosition() { ASSERT(m_nDisplayPosition > 0); m_nDisplayPosition--; }
	int GetSuitPosition() const { return m_nSuitPosition; }
	void SetSuitPosition(int nPos) { ASSERT(nPos >= 0); m_nSuitPosition = nPos; }
	void DecrementSuitPosition() { ASSERT(m_nSuitPosition > 0); m_nSuitPosition--; }
	int GetXPosition() const { return m_nPosX; }
	int GetYPosition() const { return m_nPosY; }
	void SetXPosition(int nPos=-1) { m_nPosX = nPos; }
	void SetYPosition(int nPos=-1) { m_nPosY = nPos; }
	int GetSuit() const { return m_nSuit; }
	int GetOwner() const { return m_nOwner; }
	void SetOwner(int nPlayer) { m_nOwner = nPlayer; 
								 if (m_nOwner == -1) m_bAssigned = FALSE; }
	int GetHandIndex() const { return m_nHandIndex; }
	void SetHandIndex(int nIndex) { m_nHandIndex = nIndex; }
	void DecrementHandIndex() { m_nHandIndex--; }
	BOOL IsCardAssigned() const { return m_bAssigned; }
	void SetAssigned(BOOL bAssign=TRUE) { m_bAssigned = bAssign; }
	void ClearAssignment() { m_bAssigned = FALSE; }
	BOOL IsCardFaceUp() const { return m_bFaceUp; }
	void SetFaceUp(BOOL bFaceUp=TRUE) { m_bFaceUp = bFaceUp; }
	void SetFaceDown() { m_bFaceUp = FALSE; }

// data
private:
	char	m_szValue[3];
	CString	m_strName,m_strFaceName,m_strFullName,m_strReverseFullName;
	int 	m_nPosX,m_nPosY,m_nOwner;
	int 	m_nSuit,m_nFaceValue, m_nDeckValue;
	int		m_nFaceValueCode;	// 0=loser, 1=maybe, 2=winner
	int		m_nHandIndex;
	int		m_nDisplayPosition;
	int		m_nSuitPosition;
	CBitmap *m_pBitmap;
	CBitmap m_prevBitmap,m_hlPrevBitmap;
	BOOL	m_bFaceUp,m_bBackgroundSet,m_bHLBackgroundSet;
	BOOL	m_bAssigned;
	//
	static int  m_nCardWidth;
	static int	m_nCardHeight;

// routines
private:
	CCard();
	~CCard();

private:
	void operator=(CCard* pSource);
	void operator=(CCard& cSource);
};

#endif