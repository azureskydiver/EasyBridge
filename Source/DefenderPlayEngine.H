//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CDefenderPlayEngine
//
#ifndef __CDEFENDERPLAYENGINE__
#define __CDEFENDERPLAYENGINE__

#include "PlayEngine.h"


//
class CDefenderPlayEngine : public CPlayEngine {

	friend class CPlayer;

//
// public routines
//
public:
	// virtual functions
	virtual void Initialize(CPlayer* pPlayer, CPlayer* pPartner, CPlayer* pLHOpponent, CPlayer* pRHOpponent, CHandHoldings* pHoldings, CCardLocation* pCardLocation, CGuessedHandHoldings** pGuessedHands, CBidEngine* pBidder, CPlayerStatusDialog* pStatusDlg);
	virtual void InitNewHand();
	virtual void Clear();
	//
	virtual CCard*  PlayCard();
	virtual CCard*	PlayFirst();
	virtual CCard*	PlaySecond();
	virtual CCard*	PlayThird();
	virtual CCard*	PlayFourth();
	//
	virtual CCard*  GetLeadCard();
	virtual CCard*  GetDiscard();
	virtual CCard*	GetPlayHint();
	//
	virtual int  GetNumClaimableTricks();
	//
	virtual void RecordCardPlay(int nPos, CCard* pCard);
	virtual void RecordCardUndo(int nPos, CCard* pCard);
	virtual void RecordTrickUndo();
	virtual void RecordRoundComplete(int nPos, CCard* pCard);
	virtual void RestartPlay();

	// class-specific functions
	virtual int		ReviewBiddingHistory();
	virtual CCard*	FindLeadCardFromPartnerPreference();
	virtual CCard*	Get4thBestLeadCard();
	virtual CCard*  GetHighDiscard(CSuitHoldings& suit);
	virtual CCard*	CashWinners();

//
// private routines
//
protected:
	//
	virtual void	AssessPosition();
	virtual BOOL	IsPartnerVoidInSuit(int nSuit);


// public data
public:

// private data
private:
	// class-specific data
	int		m_numRequiredTricks;
	int		m_nPartnerSuitPreference[4];
	int		m_nSignalledSuit;
	int		m_nSuitSignalStatus[4];
	int		m_nEchoedSuit;
	BOOL	m_bSuitEchoed[4];
	BOOL	m_bSuitLed[4];
	BOOL	m_bUsingGIB;

//
// private routines
//
private:
	CDefenderPlayEngine();
	virtual ~CDefenderPlayEngine();
};

#endif