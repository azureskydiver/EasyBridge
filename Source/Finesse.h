//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Finesse.h
//

#ifndef __CFINESSE__
#define __CFINESSE__

#include "Play.h"

//
// The CFinesse class
//
class CFinesse : public CPlay {

// public data
public:
	// finesse target
	enum { AGAINST_LHO=0, 
		   AGAINST_RHO=1, 
		   AGAINST_BOTH=2,
	};
	// finesse type
	enum { TYPE_I=0, 
		   TYPE_II, 
		   TYPE_III, 
		   TYPE_IV, 
		   TYPE_A, 
		   TYPE_B, 
	};

// public routines
public:
	// overloadable functions
	virtual void	Clear();
	virtual void	Init();
	virtual CString GetFullDescription() = 0;
	virtual PlayResult	Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
								CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, 
								CPlayerStatusDialog& status, CCard*& pPlayCard) = 0;
	//
	virtual int			GetTargetPos() { return m_nTargetPos; }
	virtual int			GetDepth() { return m_nGapSize; }
	virtual CCardList*	GetGapCards() { return m_pGapCards; }
	virtual int			GetSubType() { return m_nSubType; }


// protected routines
protected:


// protected data
protected:
	int		m_nSubType;
	int		m_nPlayerPosition;
	int		m_nTarget;			// AGAINST_LHO or AGAINST_RHO
	int		m_nTargetPos;		// North, West, etc...
	int		m_nGapSize;
	int		m_nCardVal;	// 2..14 (deuce through Ace)
	CCardList*	m_pGapCards;

// construction/destruction
public:
	CFinesse(int nSubType, int nTargetHand, int nPlayerPosition, CCardList* pGapCards, int nSuit, int nCardVal, BOOL bOpportunistic=FALSE);
	CFinesse(int nSubType, int nTargetHand, int nPlayerPosition, CCardList* pGapCards, CCard* pCard, BOOL bOpportunistic=FALSE);
	virtual ~CFinesse();
};


#endif
