//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// EasyBdoc.h : interface of the CEasyBDoc class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __EASYBDOC__
#define __EASYBDOC__

#include "ObjectWithProperties.h"
#include "EventProcessor.h"
class CPlayer;
class CCard;
class CGameRecord;


class CEasyBDoc : public CDocument, public CObjectWithProperties, public CEventProcessor
{
public:
	// game-related event codes
	enum { EVENT_NONE, EVENT_CLAIMED, EVENT_CONCEDED };

// public routines
public:
	// property get/set operations
	LPVOID GetValuePV(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	LPCTSTR GetValueString(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	double GetValueDouble(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	int GetValue(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0) const;
	int SetValuePV(int nItem, LPVOID value, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, double fValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, LPCTSTR szValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, int nValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	// Event Processor
	bool Notify(long lCode, long param1=0, long param2=0, long param3=0) { return false; }
	//
	static CEasyBDoc* GetDoc();
	//
	void ClearAllInfo();
	void ClearMatchInfo();
	void ClearFileParameters();
	void InitNewMatch();
	void InitNewHand(BOOL bRestarting=FALSE);
	int  DealCards();
	void DealHands(BOOL bUseDealNumber=FALSE, int nDealNumber=0);
	void InitGameReview();
	void LoadGameRecord(const CGameRecord& game);
	void PlayGameRecord(int nGameIndex);
	void ClearHands();
	void ClearBiddingInfo();
	void PrepForCardLayout();
	void InitPlay(BOOL bRedrawScreen=TRUE, BOOL bRestarting=FALSE);
	BOOL IsBidValid(int nBid);
	int	 EnterBid(int nPos, int nBid);
	int	 UndoBid();
	void RestartBidding();
	int  WasSuitBid(int nSuit) const;
	int  GetSuitsBid(CArray<int,int>& suits) const;
	int  GetSuitsUnbid(CArray<int,int>& suits) const;
	void SetBiddingComplete();
	void UpdateBiddingHistory();
	void UpdatePlayHistory();
	void BeginRound();
	BOOL TestPlayValidity(Position nOrigin, CCard* pCard, BOOL bAlert=TRUE);
	void EnterCardPlay(Position nPos, CCard* pCard);
	void ClaimTricks(int nPos, int numTricks=0);
	void ConcedeTricks(int nPos);
	void InvokeNextPlayer();
	void EvaluateTrick(BOOL bQuietMode=FALSE);
	void ClearTrick();
	void UndoLastCardPlayed();
	void UndoTrick();
	void UndoPreviousTrick();
	void RestoreInitialHands();
	void OnGameComplete();
	void PostProcessGame();
	void ComputerReplay(BOOL bFullAuto);
	void UpdateDisplay();
	void RefreshDisplay();
	void ResetDisplay();
	void ShowAutoHint();
	BOOL IsHintAvailable();
	CString FormatOriginalHands();
	CString FormatCurrentHands();
	CString GetDealIDString();

	// inline functions
	BOOL IsInitialized() { return m_bInitialized; }
	CPlayer* GetPlayer(int nIndex) const { return m_pPlayer[nIndex]; }
	CPlayer* GetHumanPlayer() const { return m_pPlayer[SOUTH]; }
	int GetHumanPlayerPos() const { return SOUTH; }
	int	GetContract() const { return m_nContract; }
	const CString GetContractString() const;
	const CString GetFullContractString() const;
	int	GetContractSuit() const { return m_nContractSuit; }
	int	GetTrumpSuit() const { return m_nTrumpSuit; }
	int	GetContractLevel() const { return m_nContractLevel; }
	int GetContractModifier() const { return m_nContractModifier; }
	BOOL IsContractDoubled() const { return m_bDoubled; }
	BOOL IsContractRedoubled() const { return m_bRedoubled; }
	int	GetDealer() const { return m_nDealer; }
	CPlayer* GetDeclarer() const { return ISPLAYER(m_nDeclarer)? m_pPlayer[m_nDeclarer] : NULL; }
	int	GetDeclarerPosition() const { return m_nDeclarer; }
	int GetDeclaringTeam() const { return GetPlayerTeam(m_nDeclarer); }
	int	GetRoundLead() const { return m_nRoundLead; }
	CPlayer* GetRoundLeadPlayer() const { return m_pPlayer[m_nRoundLead]; }
	int	GetBiddingRound() const { return m_nBiddingRound; }
	int	GetNumBidsMade() const { return m_numBidsMade; }
	int GetOpeningBid() const { return m_nOpeningBid; }
	int GetOpeningBidder() const { return m_nOpeningBidder; }
	int	GetNumValidBidsMade() const { return m_numValidBidsMade; }
	int	GetLastValidBid() const { return m_nLastValidBid; }
	int	GetLastValidBidTeam() const { return m_nLastValidBidTeam; }
	int GetBidByPlayer(int nPlayer, int nRound) const { return m_nBidsByPlayer[nPlayer][nRound]; }
	int GetBidByPlayer(CPlayer* pPlayer, int nRound) const;
	int GetBidByIndex(int nIndex) const { return m_nBiddingHistory[nIndex]; }
	int GetValidBidRecord(int nIndex) const  { return m_nValidBidHistory[nIndex]; }
	int GetVulnerableTeam() const { return m_nVulnerableTeam; }
	BOOL IsTeamVulnerable(int nTeam) { return m_bVulnerable[nTeam]; }
//	int	GetPlayRound() const { return m_nPlayRound; }
	int	GetPlayRound() const { return m_numTricksPlayed; }
	int	GetNumCardsPlayedInRound() const { return m_numCardsPlayedInRound; }
	int	GetNumCardsPlayedInGame() const { return m_numCardsPlayedInGame; }
	int	GetNumTricksPlayed() const { return m_numTricksPlayed; }
	int	GetNumTricksRemaining() const { return 13 - m_numTricksPlayed; }
	int	GetPlayRecord(int nIndex) const { return m_nPlayRecord[nIndex]; }
	BOOL WasTrumpPlayed() const;
	int    GetGameTrickLead(int nRound) const { return m_nTrickLead[nRound]; }
	int    GetGameTrickWinner(int nRound) const { return m_nTrickWinner[nRound]; }
	CCard* GetGameTrickCard(int nRound, int nPlayer) const { return m_pGameTrick[nRound][nPlayer]; }
	CCard* GetCurrentTrickCardByOrder(int nOrder) const;
	CCard* GetCurrentTrickCard(int nPos) const { return m_pCurrTrick[nPos]; }
	CCard* GetCurrentTrickCardLed() const { return GetCurrentTrickCardByOrder(0); }
	CCard* GetCurrentTrickHighCard(int* nPos=NULL)  const;
	void SetCurrentTrickCard(int nPos, CCard* pCard) { m_pCurrTrick[nPos] = pCard; }
	int	GetCurrentPlayerPosition() const { return m_nCurrPlayer; }
	CPlayer* GetCurrentPlayer() const { return m_pPlayer[m_nCurrPlayer]; }
	void SetCurrentPlayer(int nPlayer) { m_nCurrPlayer = nPlayer; }
	int GetRoundWinner() const { return m_nRoundWinner; }
	int GetRoundWinningTeam() { return m_nRoundWinningTeam; }
	int GetNumTricksWonByTeam(int nTeam) { return m_numTricksWon[nTeam]; }
	int GetDummyPosition() const { return m_nDummy; }
	CPlayer* GetDummyPlayer() const { return m_pPlayer[m_nDummy]; }
	BOOL IsDummyExposed() const { return m_bExposeDummy; }
	void ExposeDummy(BOOL bExpose=TRUE, BOOL bRedraw=FALSE);
	void GetGameHint(BOOL bAutoHintRequest=TRUE);
	CTypedPtrArray<CPtrArray, CGameRecord*>& GetGameRecords() { return m_gameRecords; }
	CGameRecord* GetGameRecord(int nIndex) { if ((nIndex < 0) || (nIndex > m_gameRecords.GetSize())) return NULL; return m_gameRecords[nIndex]; }
	BOOL IsReviewingGame() { return m_bReviewingGame; }
	BOOL IsGameReviewAvailable() { return m_bGameReviewAvailable; }
	void SuppressBidHistoryUpdate(BOOL bCode=TRUE) { m_bSuppressBidHistoryUpdate = bCode; }
	void SuppressPlayHistoryUpdate(BOOL bCode=TRUE) { m_bSuppressPlayHistoryUpdate = bCode; }
	// public access points
	void SaveFile() { OnFileSave(); }
	void SaveFileAs() { OnFileSaveAs(); }

//
public:
	static CEasyBDoc* m_pDoc;

protected: // create from serialization only
	CEasyBDoc();
	DECLARE_DYNCREATE(CEasyBDoc)

// Attributes
public:
	enum { tnEasyBridgeFormat=0, tnPBNFormat=1, tnTextFormat=2 };

private:
	static BOOL	m_bInitialized;
	CPlayer*	m_pPlayer[4];
	// file info
	CString		m_strFileProgTitle;
	int			m_nFileProgMajorVersion;
	int			m_nFileProgMinorVersion;
	int			m_nFileProgIncrementVersion;
	int			m_nFileProgBuildNumber;
	CString		m_strFileProgBuildDate;
	CString		m_strFileDate;
	CString		m_strFileDescription;
	CString		m_strFileComments;
	int			m_nFileFormat;
	int			m_nPrevFileFormat;
	//
	CString		m_strDocTitle;
	// match info
	int			m_nGameScore[3][2];	// current game score
	int			m_nCurrGame;
	int			m_nBonusScore[2];	// score above the line
	int			m_nTotalScore[2];	// total game score
	int	    	m_numGamesWon[2];	// num games won
	Team		m_nVulnerableTeam;	// which team is vunerable
	BOOL		m_bVulnerable[2];	// team is vulnerable flag
	CStringArray	m_strArrayTrickPointsRecord;
	CStringArray	m_strArrayBonusPointsRecord;
	CString			m_strTotalPointsRecord;
	// game (contract) info
	int			m_nContract;
	int			m_nContractLevel;
	int			m_nContractSuit;
	int			m_nContractTeam;		// team with the contract
	int			m_nDefendingTeam;
	int			m_numBidsMade;
	int			m_nBiddingRound;
	BOOL		m_bDoubled;				// double in efect
	int			m_nDoubler;				// player who doubled
	BOOL		m_bRedoubled;			// redouble in effect
	int			m_nRedoubler;			// player who redoubled
	int			m_nContractModifier;	// doubled/redoubled status
	int			m_nLastValidBid;		// last non-pass bid
	int			m_nLastValidBidTeam;	// team to make that bid
	int			m_nPartnershipSuit[2];	// selected suit
	int			m_nPartnershipLead[2][5];	// player who called the suit first
	int			m_numPasses;
	int			m_nBiddingHistory[100];
	int			m_nValidBidHistory[100];	// record of non-pass bids
	int			m_numValidBidsMade;
	int			m_nOpeningBid;
	int			m_nOpeningBidder;
	int			m_nBidsByPlayer[4][50];
	// game (play) info
	// hand info
	int			m_nDealer;			// dealer for current hand
	int			m_nPrevDealer;		// dealer for previous hand
	int			m_nDeclarer;		// declarer/driver for hand
	int			m_nDummy;			// current dummy
	BOOL		m_bExposeDummy;		// dummy show flag
	int			m_nTrumpSuit;		//
	//
	int			m_nDealNumber;				// hand seed number
	int			m_nSpecialDealCode;
	BOOL		m_bDealNumberAvailable;		// hand seed avaialble?
	// game (play) info
	int			m_nGameLead;		// player to lead off the game
	int			m_nRoundLead;		// player to lead this round
	int			m_nPlayRound;		// current play round
	int			m_nCurrPlayer;		// next person to play
	int			m_nPlayRecord[52];	// record of cards played
	CCard*		m_pGameTrick[13][4]; // history of tricks
	int			m_nTrickLead[13];	// history of lead players
	int			m_nTrickWinner[13];	// history of winners
	CCard*		m_pCurrTrick[4];			// cards so far in trick
	int			m_numTricksPlayed;			// total # tricks played
	int			m_numActualTricksPlayed;	// actual # tricks played (w/ claim/concede)
	int			m_numCardsPlayedInRound; // # played in current trick
	int			m_numCardsPlayedInGame;	 // # played in current game
	int			m_numTricksWon[2];	// # won by each team
	int			m_numActualTricksWon;	// saves # tricks won before auto replay
	//
	int			m_nLastBiddingHint;
	CCard*		m_pLastPlayHint;
	BOOL		m_bHintFollowed;
	// results info
	int			m_nSuitLed;
	int			m_nHighVal;
	int			m_nHighTrumpVal;
	CCard*		m_pHighCard;
	int			m_nHighPos;
	int			m_nRoundWinner;
	Team 		m_nRoundWinningTeam;		
	// flags
	BOOL		m_bHandsDealt;		// hand dealt yet?
	BOOL		m_bExpressPlayMode;
	BOOL		m_bAutoReplayMode;
	BOOL		m_bBatchMode;
	BOOL		m_bHintMode;
	BOOL		m_bSuppressBidHistoryUpdate;
	BOOL		m_bSuppressPlayHistoryUpdate;
	// auto window open triggers
	BOOL		m_bShowCommentsUponOpen;
	BOOL		m_bShowBidHistoryUponOpen;
	BOOL		m_bShowPlayHistoryUponOpen;
	BOOL		m_bShowAnalysesUponOpen;
	// save options
	BOOL		m_bSavePlayerAnalysis[4];
	BOOL		m_bSaveIntermediatePositions;
	// file I/O
	BOOL		m_bReviewingGame;
	BOOL		m_bGameReviewAvailable;
	int			m_nLineNumber;

	//
	// misc info
	//
	CTypedPtrArray<CPtrArray, CGameRecord*>	 m_gameRecords;


// Operations
private:
	//
	void Initialize();
	void Terminate();
	void InitNewGame();
	void PrepForNewDeal();
	void RestartCurrentHand(BOOL bUpdateView);
	void InitializeVulnerability();
	void ClearPlayInfo();
	void UpdateScore();
	void DisplayScore();
	void UpdateDuplicateScore();
	void DisplayDuplicateScore();

	//
	void DealSpecial(int nGameCode, int nSuitCode, int nSlamCode, int nTeam=NORTH_SOUTH, int nDealNumber=0);
	void DealSpecial(int nDealNumber, int nSpecialDealCode);
	double SwapPoints(int nDest, int nSource, double fMax, int nGameCode, int nSuitCode, int nSlamCode);
	BOOL SwapPlayersCards(int nPlayer1, int nPlayer2,int nSuit1, int nSuit2,int nCard1, int nCard2, BOOL bResetCounts=FALSE);
	void SwapPartialHands(int nPos1, int nPos2);

	//
	BOOL SwapPlayersHands(Position player1, Position player2, BOOL bRefresh=TRUE, BOOL bRestartBidding=TRUE);
	void RotatePlayersHands(int nDirection, BOOL bRefresh=TRUE, BOOL bRestartBidding=TRUE);
	void RotatePartialHands(int numPositions);

	// I/O routines
	BOOL ReadFile(CArchive& ar);
	int ParseLine(CString& string, int nLineLength);
	BOOL ReadFilePBN(CArchive& ar);
	int ParseLinePBN(CString& string, CString& strTag, CString& strValue, int nLineLength);
	int PreloadPBNFile(CArchive& ar, CStringArray& strLines);
	int ReadLine(CArchive&ar, CString& strDest);
	void AssignCards(CString& str, int nPosition, BOOL bInitialHand=FALSE);
	int  ParseBidsPBN(CArchive& ar, const CString& strValue);
	int  ParsePlaysPBN(CArchive& ar, const CString& strValue);
	void AssignCardsPBN(const CString& str);
	void SaveCurrentGameRecord(BOOL bAllocNew=FALSE);
	//
	BOOL WriteFile(CArchive& ar);
	BOOL WriteFilePBN(CArchive& ar);
	BOOL ExportGameInfo(CArchive& ar);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEasyBDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEasyBDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
	//{{AFX_MSG(CEasyBDoc)
	afx_msg void OnNewGame();
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAs();
	afx_msg void OnDealGameHand();
	afx_msg void OnUpdateDealSpecial(CCmdUI* pCmdUI);
	afx_msg void OnDealGrandSlam();
	afx_msg void OnDealMajorGame();
	afx_msg void OnDealMinorGame();
	afx_msg void OnDealSlam();
	afx_msg void OnDealSmallSlam();
	afx_msg void OnDealNotrumpGame();
	afx_msg void OnSwapNS_EWCards();
	afx_msg void OnSwapPositionEast();
	afx_msg void OnSwapPositionNorth();
	afx_msg void OnSwapPositionWest();
	afx_msg void OnUpdateRestartCurrentHand(CCmdUI* pCmdUI);
	afx_msg void OnRestartCurrentHand();
	afx_msg void OnSwapCardsClockwise();
	afx_msg void OnSwapCardsCounterclockwise();
	afx_msg void OnUpdateDealNewHand(CCmdUI* pCmdUI);
	afx_msg void OnDealNewHand();
	afx_msg void OnViewScore();
	afx_msg void OnUpdateViewScore(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePlayRubber(CCmdUI* pCmdUI);
	afx_msg void OnPlayRubber();
	afx_msg void OnUpdatePlayClaimTricks(CCmdUI* pCmdUI);
	afx_msg void OnPlayClaimTricks();
	afx_msg void OnUpdatePlayConcedeTricks(CCmdUI* pCmdUI);
	afx_msg void OnPlayConcedeTricks();
	afx_msg void OnUpdateGameAutoPlay(CCmdUI* pCmdUI);
	afx_msg void OnGameAutoPlay();
	afx_msg void OnUpdateGameAutoPlayAll(CCmdUI* pCmdUI);
	afx_msg void OnGameAutoPlayAll();
	afx_msg void OnUpdateClearAll(CCmdUI* pCmdUI);
	afx_msg void OnClearAll();
	afx_msg void OnUpdateSwapCards(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGameHint(CCmdUI* pCmdUI);
	afx_msg void OnGameHint();
	afx_msg void OnUpdatePlayClaimContract(CCmdUI* pCmdUI);
	afx_msg void OnPlayClaimContract();
	afx_msg void OnDealGameHandEastWest();
	afx_msg void OnDealMajorGameEastWest();
	afx_msg void OnDealMinorGameEastWest();
	afx_msg void OnDealNotrumpGameEastWest();
	afx_msg void OnDealSlamEastWest();
	afx_msg void OnDealSmallSlamEastWest();
	afx_msg void OnDealGrandSlamEastWest();
	afx_msg void OnUpdateGameAutoPlayExpress(CCmdUI* pCmdUI);
	afx_msg void OnGameAutoPlayExpress();
	afx_msg void OnUpdateFileProperties(CCmdUI* pCmdUI);
	afx_msg void OnFileProperties();
	afx_msg void OnUpdateDealNumberedHand(CCmdUI* pCmdUI);
	afx_msg void OnDealNumberedHand();
	afx_msg void OnUpdateGameAutoTest(CCmdUI* pCmdUI);
	afx_msg void OnGameAutoTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
