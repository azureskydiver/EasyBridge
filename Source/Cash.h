//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Cash.h
//

#ifndef __CCASH__
#define __CCASH__

#include "Play.h"


//
// The CCash class
//
class CCash : public CPlay {

	// public data
public:
	// NCR-707 add this enum
	// cash type
	typedef enum {
		ORDINARY = 1,
		FOR_PROMOTION = 2,  // these cashes are for setting up a promotion
	} Property;

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
	virtual int		GetProperties() const { return m_Properties; }  // NCR-707
	virtual void    SetProperites(Property prop) {m_Properties = prop;}  // NCR-707


// protected routines
protected:


// protected data
protected:
	int		m_nCardVal;	// 2..14 (deuce through Ace)
	Property m_Properties;  // NCR-707

// construction/destruction
public:
	CCash(int nTargetHand, int nStartingHand, CCardList* pRequiredPlayedCards, int nSuit=NONE, int nCardVal=NONE, PlayProspect nProspect=PP_LIKELY_WINNER, BOOL bOpportunistic=FALSE);
	CCash(int nTargetHand, int nStartingHand, CCardList* pRequiredPlayedCards, CCard* pCard, PlayProspect nProspect=PP_LIKELY_WINNER, BOOL bOpportunistic=FALSE);
	CCash(CCash& srcPlay);
	virtual ~CCash();
};


#endif
