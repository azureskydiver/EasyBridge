//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// TrumpPull.h
//

#ifndef __CTRUPMPULL__
#define __CTRUPMPULL__

#include "Play.h"


//
// The CTrumpPull class
//
class CTrumpPull : virtual public CPlay {

// public data
public:

// public routines
public:
	// overloadable functions
	virtual void	Clear();
	virtual void	Init();
	virtual CString GetFullDescription();
	virtual PlayResult	Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
								CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, 
								CPlayerStatusDialog& status, CCard*& pPlayCard);
	//
	virtual int		UsesUpEntry();
	//
	virtual int		IsOptional() { return m_bOptional; }


// protected routines
protected:

// protected data
protected:
	int		m_nCardVal;	// 2..14 (deuce through Ace)
	BOOL	m_bOptional;
	CCardList*	m_pOutstandingCards;

// construction/destruction
public:
	CTrumpPull(int nTargetHand, int nSuit, int nCardVal, CCardList* pRequiredPlayedCards, CCardList* pOutstandingCards, BOOL bWinner=TRUE, BOOL bOptional=TRUE);
	CTrumpPull(int nTargetHand, CCard* pCard, CCardList* pRequiredPlayedCards, CCardList* pOutstandingCards, BOOL bWinner=TRUE, BOOL bOptional=TRUE);
	virtual ~CTrumpPull();
};


#endif
