//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CDisplayHoldings
//
#ifndef __DISPLAYHOLDINGS__
#define __DISPLAYHOLDINGS__

#include "cardholdings.h"


//
class CDisplayHoldings : public CCardHoldings {

// public routines
public:
	virtual void Init();
	virtual void Clear();
	virtual void Add(CCard* pCard, const BOOL bSort=FALSE);
	virtual CCard* RemoveByIndex(const int nIndex);
	virtual void Sort();

	//
	void	SetDummyFlag(BOOL bValue=TRUE) { m_bDummy = bValue; }

// public data
public:

// private data
private:
	BOOL	m_bDummy;

public:
	CDisplayHoldings::CDisplayHoldings();
	CDisplayHoldings::~CDisplayHoldings();
};

#endif