//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CCardLocation
//
#ifndef __CCARDLOCATION__
#define __CCARDLOCATION__

#include "GuessedCard.h"

// 
class CCardLocation {

// public operations
public:
	void Clear();
	CGuessedCard& operator[](int nIndex) { VERIFY(ISDECKVAL(nIndex)); return m_cards[nIndex]; }

// private data
private:
	CGuessedCard	m_cards[52];

// construction / destruction
public:
	CCardLocation();
	~CCardLocation();
};


#endif