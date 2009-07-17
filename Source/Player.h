//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CPlayer
//

#ifndef __CPLAYER__
#define __CPLAYER__

class CCard;
class CHandHoldings;
class CCombinedHoldings;
class CDeclarerPlayEngine;
class CDummyPlayEngine;
class CDefenderPlayEngine;
class CBidEngine;
class CPlayEngine;
class CCardLocation;
class CGuessedHandHoldings;
class CConvention;
class CPlayerStatusDialog;

//
class CPlayer {

	friend class CConvention;

// public routines
public:
	LPVOID GetValuePV(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const; // NCR added const
	double GetValueDouble(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	LPCTSTR GetValueString(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int GetValue(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const; // NCR added const
 	int SetValuePV(int nItem, LPVOID value, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, double fValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, int nValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	void SetValueString(int nItem, LPCTSTR szValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	void InitializePlayer(Position nPos, CPlayer* pPartner, CPlayer* pLHOpponent, CPlayer* pRHOpponent);
	void InitializeEngines();

	//
	// Hand-related routines
	// (delegated to the m_pHand object)
	//
	CHandHoldings& GetHand() { return *m_pHand; }
	CGuessedHandHoldings* GetGuessedHand(int nPlayer) { return m_pGuessedHands[nPlayer]; }
	CGuessedHandHoldings** GetGuessedHands() { return m_pGuessedHands; }
	void ClearHand();
	void ExposeCards(BOOL bCode=TRUE, BOOL bRedraw=TRUE);
	void RedrawHand() const;
	void InitializeHand();
	void RestoreInitialHand();
	void InitializeRestoredHand();
	void InitializeSwappedHand();
	void AddCardToHand(CCard* pCard, BOOL bSort=FALSE);
	void AddCardToInitialHand(CCard* pCard, BOOL bSort=FALSE);
	void RemoveCardFromHand(CCard* pCard);
	CCard* RemoveCardFromHand(int nIndex);
	void RemoveCardFromInitialHand(CCard* pCard);
	CCard* RemoveCardFromInitialHand(int nIndex);
	void RemoveAllCardsFromHand();
	void SetCard(int nIndex, CCard* pCard);
	void SetInitialHandCard(int nIndex, CCard* pCard);
	void SortHand();
	void InitialEvaluateHand();
	void CountCards();
	double CountCardPoints(BOOL bForceCount=FALSE);
	int	GetNumCards() const;
	int GetNumCardsInSuit(int nSuit) const;
	int GetSuitLength(int nSuit) const;
	BOOL HasCard(const CCard* pCard) const;
	BOOL HasCard(int nDeckValue) const;
	CCard* GetCardByValue(int nVal) const;
	CCard* GetCardByPosition(int nIndex) const;
	CCard* GetCardInSuit(int nSuit, int nIndex) const;
	CCard* GetDisplayedCard(int nIndex) const;
	CCard* GetInitialHandCard(int nIndex) const;
	BOOL AreCardsExposed() const;
	int GetNumCardsOf(int nValue) const;
	BOOL SuitHasCard(int nSuit, int nCardFaceValue) const;
	double GetHCPoints() const;
	double GetTotalPoints() const;
	int GetBalanceValue() const;
	double GetNumQuickTricks() const;
	int GetNumSuitsStopped() const;

	//
	// Bidding-related routines
	// (delegated to the m_pBidder object)
	//
	int GetBiddingHint(BOOL bAutoHintMode=FALSE);
	BOOL IsGameBid(int nBid);
	int GetGameBid(int nSuit);
	int Bid();
	int BidInternal();
	BOOL EnterHumanBid(int nBid);
	void CancelBidding();
	void ClearBiddingInfo();
	void ClearConventionStatus(CConvention* pConvention=NULL);
	void RecordBid(int nPos, int nBid);
	void RestartBidding();
	void BiddingFinished();
	int InquireLastBid();
	int GetNumBidsMade() const;  // NCR added const
	int GetNumBidTurns();
	BOOL OpenedBiddingForTeam();
	int GetPriorBid(int nIndex=1);
	int GetPriorSuit(int nIndex=1);

	//
	// play-related routines
	// (mostly delegated to the m_pPlayEngine object)
	//
	CCard* PlayCard();
	CCard* PlayForDummy();
	CCard* PlayForDummy(int nPosition);
	CCard* GetPlayHint(BOOL bAutoHintMode=FALSE);
	CCard* GetPlayHintForDummy();
	void RestartPlay();
	void RecordCardPlay(int nPos, CCard* pCard);
	void RecordCardUndo(int nPos, CCard* pCard);
	void RecordTrickUndo();
	void RecordRoundComplete(int nPos, CCard* pCard);
	void RecordHandComplete(int nResult);
	void RecordSpecialEvent(int nCode, int nParam1=0, int nParam2=0, int nParam3=0);
	int GetNumClaimableTricks();
	BOOL TestForAutoPlayLastCard();

	//
	// feeback routines (using m_statusDlg)
	//
	void Trace(const CString& strText);
	void AddTrace(CString& strMessage, CString& strTemp);
	void AddTrace(CString& strMessage, LPCTSTR szTemp);
	void SuspendTrace();
	void ResumeTrace();
	void ClearAnalysis();
	void ShowAnalysis(BOOL bShow=TRUE);
	CPlayerStatusDialog& GetStatusDialog() { return *m_pStatusDlg; }
	//
	void SetTestBiddingMode(bool bMode);
	bool GetTestBiddingMode();

	//
	// misc other routines
	//
	BOOL IsDeclarer() const { return m_bDeclarer; }
	BOOL IsDummy() const { return m_bDummy; }
	BOOL IsDefending() const { return (!m_bDeclarer && !m_bDummy); }
	BOOL IsHumanPlayer() const;
	int GetTeam() const { return ((m_nPosition==SOUTH) || (m_nPosition==NORTH))? NORTH_SOUTH : EAST_WEST; }
	void SetDeclarerFlag(BOOL bValue=TRUE) { m_bDeclarer = bValue; }
	void SetDummyFlag(BOOL bValue=TRUE);
	Position GetPosition() const { return m_nPosition; }
	Position GetOpeningPosition();
	CPlayer* GetPartner() const { return m_pPartner; }
	CPlayer* GetLHOpponent() const { return m_pLHOpponent; }
	CPlayer* GetRHOpponent() const { return m_pRHOpponent; }

	//
	CBidEngine*	GetBidEngine() { return m_pBidder; }
	CPlayEngine* GetPlayEngine() { return m_pPlayEngine; }
	CDeclarerPlayEngine* GetDeclarerEngine() { return m_pDeclarer; }
	CDummyPlayEngine* GetDummyEngine() { return m_pDummy; }
	CDefenderPlayEngine* GetDefenderEngine() { return m_pDefender; }

private:
	LPCTSTR SuitHoldingsToString(int nSuit);	// shortcut
	void SortSuits();

// data
private:
	// player info
	Position	m_nPosition;
	int 		m_nSkillLevel;
	BOOL 		m_bDeclarer;
	BOOL 		m_bDummy;
	BOOL		m_bTeamIsVulnerable;
	CPlayer*	m_pPartner;
	CPlayer*	m_pLHOpponent;
	CPlayer*	m_pRHOpponent;
	//
	CHandHoldings*			m_pHand;
	CBidEngine*				m_pBidder;
	CPlayEngine*			m_pPlayEngine;
	CDeclarerPlayEngine*	m_pDeclarer;
	CDummyPlayEngine*		m_pDummy;
	CDefenderPlayEngine*	m_pDefender;
	// derived information
	CCardLocation*			m_pCardLocation;
	CGuessedHandHoldings*	m_pGuessedHands[4];
	//
	CPlayerStatusDialog*	m_pStatusDlg;


// construction/destruction 
public:
	CPlayer();
	~CPlayer();
};

#endif