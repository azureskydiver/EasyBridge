//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Deck.h
//
// Encapsulates a deck of cards
//
#ifndef __DECK__
#define __DECK__

#include "ObjectWithProperties.h"
class CCard;

// default card bitmap dimensions
const int DEFCARDWIDTH 		= 71;
const int DEFCARDHEIGHT		= 96;
const int SMALLCARDWIDTH 	= 57;
const int SMALLCARDHEIGHT	= 78;


class CDeck : public CObjectWithProperties {

// public routines
public:
	// property get/set operations
	int		GetValue(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	LPVOID	GetValuePV(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int		SetValue(int nItem, int nValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);

	// one-time only init/termination operations
	void	Initialize();
	void	InitializeBitmaps();
	void	Terminate();
	//
	void	Clear();
	void	ClearCardBackgrounds();
	int		Shuffle(int nSeed=0, bool bSuppressSeed=false);
	//
	CCard*	GetCard(int nDeckValue);
	CCard*	GetCard(LPCTSTR pszName);
	CCard*	GetCard(int nSuit, int nFaceValue);
	// inline operations
	CCard*	  operator[](int nIndex) { return m_cards[nIndex]; }
	CCard*    GetSortedCard(int nIndex) { return m_sortedCards[nIndex]; }
	int		  GetCardWidth() const;
	int		  GetCardHeight() const;
	CBitmap*  GetMaskBitmap() { return &m_maskBitmap; }
	CBitmap*  GetCardBitmap(int nIndex) { return &m_cardBitmap[nIndex]; }
	int		  GetNumCardBacks() { return m_numCardBacks; }
	void	  SetCardBackIndex(int nIndex) { if ((nIndex >= 0) && (nIndex < m_numCardBacks)) m_nCurrCardBack = nIndex; }
	int		  GetCardBackIndex() { return m_nCurrCardBack; }
	CBitmap*  GetCardBackBitmap(int nIndex=-1) { 
						if (nIndex == -1) nIndex = m_nCurrCardBack;
						return &m_cardBackBitmap[nIndex]; }
	CImageList& GetCardBackImageList() { return m_imageListCardBacks; }

// data
private:
	CCard*		m_cards[52];
	CCard*		m_sortedCards[52];
	CBitmap		m_cardBitmap[52];
	CImageList	m_imageListCardBacks;
	CBitmap		m_cardBackImageListBitmap[MAX_CARDBACKS];
	CBitmap		m_cardBackBitmap[MAX_CARDBACKS];
	CBitmap		m_maskBitmap;
	int			m_nCardWidth;
	int			m_nCardHeight;
	int			m_numCardBacks;
	int			m_nCurrCardBack;
	int 		m_nDeckIndex[4][15];
	BOOL		m_bInitialized;
	int			m_nPrevTime;
	
// routines
public:
	CDeck();
	~CDeck();

};

extern CDeck deck;

#endif