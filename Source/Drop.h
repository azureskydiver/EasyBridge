//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Drop.h
//

#ifndef __CDROP__
#define __CDROP__

#include "Play.h"


//
// The CDrop class
//
class CDrop : public CPlay {

// public data
public:
	enum { 
		AGAINST_LHO=0, 
		AGAINST_RHO=1, 
		AGAINST_EITHER=2,
	};

// public routines
public:
	// overloadable functions
	virtual void	Clear();
	virtual void	Init();
	virtual CString GetFullDescription();
	virtual PlayResult	Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
								CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, 
								CPlayerStatusDialog& status, CCard*& pPlayCard);

// protected routines
protected:


// protected data
protected:
	int			m_nCardVal;	// 2..14 (deuce through Ace)
	int			m_numTargetCards;
	int			m_nTargetOpponent;
	CString		m_strDropCards;
	CString		m_strDropMessage;

// construction/destruction
public:
	CDrop(int nTargetHand, int nTargetOpponent, CCardList* m_pEnemyCards, int nSuit, int nCardVal);
	CDrop(int nTargetHand, int nTargetOpponent, CCardList* m_pEnemyCards, CCard* pCard);
	CDrop(CDrop& srcPlay);
	virtual ~CDrop();
};


#endif
