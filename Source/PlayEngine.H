//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CPlayEngine
//
#ifndef __CPLAYENGINE__
#define __CPLAYENGINE__

class CCardLocation;
class CGuessedHandHoldings;
class CPlay;
class CPlayer;
class CHandHoldings;
class CSuitHoldings;
class CPlayerStatusDialog;
class CBidEngine;


//
class CPlayEngine {

	friend class CPlayer;

// public routines
public:
	// virtual functions
	virtual void Initialize(CPlayer* pPlayer, CPlayer* pPartner, CPlayer* pLHOpponent, CPlayer* pRHOpponent, CHandHoldings* pHoldings, CCardLocation* pCardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine* pBidder, CPlayerStatusDialog* pStatusDlg);
	virtual void InitNewHand();
	virtual void InitSwappedHand();
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
	virtual int		PickFinalDiscardSuit(CHandHoldings& hand);
	virtual CCard*	GetPlayHint() { return NULL; }

	// basic routines, overridable
	virtual void RecordCardPlay(int nPos, CCard* pCard);
	virtual void RecordCardUndo(int nPos, CCard* pCard);
	virtual void RecordTrickUndo();
	virtual void RecordRoundComplete(int nPos, CCard* pCard);
	virtual void RecordHandComplete(int nResult);
	virtual void RecordSpecialEvent(int nCode, int nParam1=0, int nParam2=0, int nParam3=0);
	//
	virtual void AdjustCardCountFromPlay(int nPos, CCard* pCard);
	virtual void AdjustCardCountFromUndo(int nPos, CCard* pCard);
	virtual void AdjustHoldingsCount(CCard* pCard);
	virtual void RestartPlay();
	virtual int  GetNumClaimableTricks() { return 0; }
	virtual int  GetPrioritySuit() { return m_nPrioritySuit; }
	virtual int  GetPartnersPrioritySuit() { return m_nPartnersPrioritySuit; }
	virtual int  GetNthPrioritySuit(int nIndex) { ASSERT((nIndex >= 0) && (nIndex < 4)); return m_nSuitPriorityList[nIndex]; }
	//
	virtual void RecordCardsPlayed();

	// analysis & counting routines
	virtual int		GetOutstandingCards(int nSuit, CCardList& cardList, bool bCountDummy=false) const;
	virtual CCard*	GetHighestOutstandingCard(int nSuit) const;
	virtual int		GetNumOutstandingCards(int nSuit) const;
	virtual BOOL	IsCardOutstanding(CCard* pCard) const;
	virtual BOOL	IsCardOutstanding(int nSuit, int nFaceValue) const;
	virtual int		GetNumCardsPlayedInSuit(int nPlayer, int nSuit) const;
	virtual int		GetMinCardsInSuit(int nPlayer, int nSuit) const;
	virtual int		GetMaxCardsInSuit(int nPlayer, int nSuit) const;
	virtual int		GetMinStartingCardsInSuit(int nPlayer, int nSuit) const;
	virtual int		GetMaxStartingCardsInSuit(int nPlayer, int nSuit) const;
	
	//
	virtual void	SetHintMode(bool bMode) { m_bInHintMode = bMode; }
	virtual bool	GetHintMode() { return m_bInHintMode; }

	//
	CPlayer* GetPlayer() { return m_pPlayer; }
	CPlayer* GetPartner() { return m_pPartner; }
	int		 GetPlayerPosition() const;
	int		 GetPartnerPosition() const;
	CPlayer* GetLHOpponent() const { return m_pLHOpponent; }
	CPlayer* GetRHOpponent() const { return m_pRHOpponent; }

	// value get/set functions
	LPVOID GetValuePV(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	double GetValueDouble(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	int GetValue(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	int SetValuePV(int nItem, LPVOID value, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, double fValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, int nValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);

//
// protected routines
//
protected:
	//
	virtual void	AssessPosition();
	virtual CCard*	PlayBestCard(int nPosition);
	// 
	virtual CHandHoldings&  GetDummyHand();
	virtual CSuitHoldings&  GetDummySuit(int nSuit);


// public data
public:
	// shortcuts
	char	szLHO[6], szRHO[6];

// protected data
protected:
	//
	CPlayer*				m_pPlayer;
	CPlayer*				m_pPartner;
	CPlayer*				m_pLHOpponent;
	CPlayer*				m_pRHOpponent;
	Position				m_nPosition;
	Position				m_nPartnerPosition;
	CHandHoldings*			m_pHand;
	CHandHoldings*			m_pPartnersHand;
	CBidEngine*				m_pBidder;
	CCardLocation*			m_pCardLocation;
	CGuessedHandHoldings**	m_ppGuessedHands;
	CPlayerStatusDialog*	m_pStatusDlg;

	//
	bool	m_bInHintMode;

	// data initialized once per hand
	BOOL	m_bVulnerable;
	BOOL	m_bOpponentsVulnerable;
	BOOL	m_bLHDefender;
	BOOL	m_bRHDefender;
	int		m_nContract;
	int		m_nContractModifier;
	int		m_nTrumpSuit;
	//
	int		m_nPrioritySuit;
	int		m_nPartnersPrioritySuit;
	int		m_nSuitPriorityList[4];

	// data initialized every round
	int		m_nRound;
	int		m_numCardsPlayed;
	int		m_nRoundLead;
	CCard*	m_pLeadCard;
	int		m_nLeadSuit;
	int		m_nLeadFaceValue;
	CCard*	m_pPreviousCard;	// card played last round
	CCard*	m_pPartnersCard;	// pard's card this round
	CCard*	m_pPartnersPrevCard;// pard's card last round

//
// construction/destruction
//
protected:
	CPlayEngine();
	virtual ~CPlayEngine();
};

#endif