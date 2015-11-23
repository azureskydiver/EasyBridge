//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CDeclarerPlayEngine
//
#ifndef __CDECLARERPLAYENGINE__
#define __CDECLARERPLAYENGINE__

#include "PlayEngine.h"
#include "PlayList.h"
class CCombinedHoldings;
class CCombinedSuitHoldings;
class CDummyPlayEngine;

//
class CDeclarerPlayEngine : public CPlayEngine {

	friend class CPlayer;

//
// public routines
//
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

	virtual CCard*	GetPlayHint();
	virtual CCard*  GetPlayHintForDummy();
	//
	virtual void RecordCardPlay(int nPos, CCard* pCard);
	virtual void RecordCardUndo(int nPos, CCard* pCard);
	virtual void RecordTrickUndo();
	virtual void RecordRoundComplete(int nPos, CCard* pCard);
	virtual void AdjustCardCountFromPlay(int nPos, CCard* pCard);
	virtual void AdjustCardCountFromUndo(int nPos, CCard* pCard);
	virtual void AdjustHoldingsCount(CCard* pCard);
	virtual void RestartPlay();
	virtual int  GetNumClaimableTricks();
	//
	virtual void AddCardToCombinedHand(CCard* pCard, BOOL bPlayerCard, BOOL bSort=FALSE);
	virtual void RemoveCardFromCombinedHand(CCard* pCard);

	// analysis routines
	virtual int	 GetOutstandingCards(int nSuit, CCardList& cardList) const;
	//
	virtual int	 GetNumDeclarerEntries() { return m_numDeclarerEntries; }
	virtual int	 GetNumDummyEntries() { return m_numDummyEntries; }
	virtual int	 GetNumDeclarerPossibleEntries() { return m_numDeclarerPossibleEntries; }  // NCR-563
	virtual int	 GetNumDummyPossibleEntries() { return m_numDummyPossibleEntries; }  // NCR-563
	virtual int	 GetNumDeclarerRuffingEntries() { return m_numDeclarerRuffingEntries; }
	virtual int	 GetNumDummyRuffingEntries() { return m_numDummyRuffingEntries; }
	virtual int	 GetNumDeclarerEntriesInSuit(int nSuit) { return m_numDeclarerEntriesInSuit[nSuit]; }
	virtual int	 GetNumDummyEntriesInSuit(int nSuit) { return m_numDummyEntriesInSuit[nSuit]; }
	
	//
	virtual CCombinedHoldings& GetCombinedHand() { return *m_pCombinedHand; }
	virtual CPlayList&	 GetPlayPlan() { return m_playPlan; }
	virtual CPlayList&	 GetUsedPlays() { return m_usedPlays; }
	virtual CPlayList&	 GetDiscardedPlays() { return m_discardedPlays; }
	virtual const CPlay* GetCurrentPlay();

	// class-specific functions
	CCard* PlayForDummy();
	LPCTSTR GetFailedPlayName() const { return m_strFailedPlay; }

	virtual void ShowPlayList(CPlayList& playList);   // NCR-760


//
// protected routines
//
protected:
	//
	virtual CCard*	PlayBestCard(int nPosition);
	virtual CCard*  GetFinalDiscard();


//
// private routines
//
private:
	//
	virtual void	AssessPosition();
	//
	void	InitCombinedHand();
	void	PlanPlay();
	void	PickPrioritySuit();
	void	ReviewPlayPlan();
	void	EvaluateEntries();
	void	SequencePlays(BOOL bInitialPlan=FALSE);
	void	InterleavePlays();
	void	TestPlaysAgainstBiddingHistory();
	void	TestPlaysAgainstPlayHistory();
	int		CreateNoTrumpPlayPlan();
	int		CreateSuitContractPlayPlan();
	int		FindHoldUpPlays(CPlayList& playList, BOOL bExcludeTrumpSuit=FALSE);
	int		FindCashingPlays(CPlayList& playList, int& numReqdCards, BOOL bExcludeTrumpSuit=FALSE);
//	int		FindForcePlays(CPlayList& playList);
	int		FindRuffingPlays(CPlayList& playList);
	int		FindDropPlays(CPlayList& playList);
	int		FindFinessePlays(CPlayList& playList);
	int		FindSuitDevelopmentPlays(CPlayList& forcePlayList, CPlayList& secondaryCashList);
	int		FindFinessesInSuit(CCombinedSuitHoldings& suit, CPlayList& playList);
	int		FormTrumpPullingPlan(CPlayList& mainPlayList, CPlayList& trumpPlayList);
	int		FormTrumpFinessePlan(CPlayList& mainPlayList, CPlayList& trumpPlayList, bool bSafeOnly=true); // NCR-425 added bool
	int		FilterPlays(CPlayList& playList);
	int		ScreenIneligiblePlays();
	void	AdjustPlayCountForDeletedPlay(CPlay* pPlay);
	int		GetNumGapCardsOutstanding(int nSuit, CCardList*& pGapCards) const;
	CCard*	TrySelectedPlays();
	BOOL	IsPlayUsable(CPlay& play);
	void	TestForClaim();
	//
	int		GetCardOwner(CCard* pCard) const;


// public data
public:

// private data
private:
	// class-specific data
	CHandHoldings*			m_pPartnersHand;
	CCombinedHoldings*		m_pCombinedHand;
	CCombinedHoldings*		m_pInitialCombinedHand;
	CDummyPlayEngine*		m_pDummy;
	BOOL					m_bUsingGIB;
	CPlay*					m_pCurrentPlay;
	CString					m_strFailedPlay;
	BOOL					m_bClaimDisabled;
	BOOL					m_bCrossedOnLastPlay;

	//
	CPlayList		m_playPlan;
	CPlayList		m_usedPlays;
	CPlayList		m_discardedPlays;
	CCardList		m_dummyPriorityDiscards;
	CCardList		m_declarerPriorityDiscards;
	CCardList		m_declarerDiscardedPriorityDiscards;
	CCardList		m_dummyDiscardedPriorityDiscards;
	CCardList		m_declarerEntries;
	CCardList		m_dummyEntries;
	CCardList		m_declarerRuffingEntries;
	CCardList		m_dummyRuffingEntries;


	// data initialized once per hand
	int		m_numRequiredTricks;
	int		m_numTricksLeftToBeMade;
	int		m_numSureTricks;
	int		m_numTricksShort;
	int		m_numTricksMade;
	int		m_numDeclarerTrumps;
	int		m_numDummyTrumps;
	int		m_numTotalTrumps;

	// data initialized every round
	int		m_numOutstandingTrumps;
	//
	int		m_numDeclarerEntries;
	int		m_numDummyEntries;
	int     m_numDeclarerPossibleEntries;  // NCR-563 Entries after forcing higher card
	int     m_numDummyPossibleEntries;     // NCR-563
	int		m_numDeclarerRuffingEntries;
	int		m_numDummyRuffingEntries;
	int		m_numDeclarerEntriesInSuit[4];
	int		m_numDummyEntriesInSuit[4];

	// play planning
	int		m_numPlannedHoldUps;
	int		m_numPlannedCashingTricks;
	int		m_numPlannedDummyRuffingTricks;
	int		m_numPlannedDeclarerRuffingTricks;
	int		m_numPlannedDummyRuffsInSuit[4];
	int		m_numPlannedDeclarerRuffsInSuit[4];
	int		m_numPlannedTotalRuffsInSuit[4];
	int		m_numPlannedRuffingTricks;
	int		m_numPlannedDropPlays;
	int		m_numPlannedDropPlaysInSuit[4];
//	int		m_numPlannedFinesseTricks;
	int		m_numPlannedFinesses;
	int		m_numPlannedFinessesInSuit[4];
//	int		m_numPlannedSpeculativeTricks;
	int		m_numPlannedForcePlays;
	int		m_numPlannedForcePlaysInSuit[4];
	int		m_numPlannedSecondaryCashPlays;
	int		m_numPlannedSecondaryCashPlaysInSuit[4];
	int		m_numPlannedSluffTricks;
	int		m_numPlannedTrumpPulls;
	int		m_numTotalPlannedTricks;
	int		m_numPlannedRounds;

//
// private routines
//
private:
	CDeclarerPlayEngine();
	virtual ~CDeclarerPlayEngine();
};

#endif