//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CCardHoldings
//
// a collection of cards
//
#ifndef __CARDHOLDINGS__
#define __CARDHOLDINGS__

#include "CardList.h"
class CCard;
class CPlayer;

//
class CCardHoldings : public CCardList {

// public routines
public:
	virtual void Init();
	virtual void Clear();
	virtual void ClearPointCount();
	//
	virtual double CountPoints(const BOOL bForceCount=FALSE);
	virtual void EvaluateHoldings();
	virtual void Sort();
	virtual void FormatHoldingsString();
	virtual void FormatGIBHoldingsString();
	//
	virtual int  GetCardsOfSuit(const int nSuit, CCardList* pSuitList=NULL) const;
	virtual int  GetNumCardsOfSuit(const int nSuit) const { return GetCardsOfSuit(nSuit); }
	
	//
	// inline functions
	double  GetHCPoints() const { return m_numHCPoints; }
	double  GetCardPoints() const { return GetHCPoints(); }
	LPCTSTR GetGIBFormatHoldingsString() const { return m_strGIBFormatHoldings; }
	
// public data
public:

// private data
protected:
	double 	m_numHCPoints;
	BOOL	m_bCountPerformed;
	CString m_strGIBFormatHoldings;

// construction/destruction
public:
	CCardHoldings();
	~CCardHoldings();
};

#endif