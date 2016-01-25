//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// EasyB.h : main header file for the EASYB application
//
#ifndef __EASYB_H__
#define __EASYB_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "defines.h"        // other global info
#include "globals.h"
#include "ObjectWithProperties.h"
#include "EventProcessor.h"

class CPlayer;
class CGIB;
class CConventionSet;
class CSplashWnd;
class CNeuralNet;
extern const LPCTSTR szSuitName[];


/////////////////////////////////////////////////////////////////////////////
// CEasyBApp:
// See EasyB.cpp for the implementation of this class
//

#define DllImport   __declspec( dllimport )


class CEasyBApp : public CWinApp, public CObjectWithProperties, public CEventProcessor
{
public:
	enum { PLAY_NORMAL, PLAY_MANUAL, PLAY_MANUAL_DEFEND, PLAY_FULL_AUTO, PLAY_FULL_AUTO_EXPRESS };

// public routines
public:
	// property get/set operations
	LPVOID GetValuePV(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	LPCTSTR GetValueString(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	double GetValueDouble(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int GetValue(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValuePV(int nItem, LPVOID value, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, double fValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, LPCTSTR szValue=NULL, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, int nValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValueDouble(int nItem, double fValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	void RefreshProperties();
	// Event Processor
	bool Notify(long lCode, long param1=0, long param2=0, long param3=0) { return false; }
	//
	CString GetProgramTitle() { return m_strProgTitle; }
	CString GetProgramVersionString();
	CString GetFullProgramVersionString();
	CWnd* GetFrame() { return m_pMainWnd; }
	int GetMinimumOpeningValue(CPlayer* pPlayer=NULL) const;
	// inline functions
	BOOL IsDebugModeActive() const { return m_bDebugMode; }
	BOOL AreCardsFaceUp() const;
	void SetCardsFaceUp(BOOL bFaceUp); 
	int GetLHO(int nPos) const { return GetNextPlayer(nPos); }
	int GetRHO(int nPos) const { return GetPrevPlayer(nPos); }
	int	GetCurrentConvention() const { return m_nCurrConventionSet; }
	int	GetSuitSequence(int nIndex) const { return m_nSuitSequence[nIndex]; }
	void SetSuitSequence(int nIndex, int nValue) { m_nSuitSequence[nIndex] = nValue; }
	int	GetDummySuitSequence(int nIndex) const { return m_nDummySuitSequence[nIndex]; }
	void InitDummySuitSequence(int nTrumpSuit, int nDummyPosition=NONE);
	void InitPointCountRequirements();
	void InitHonorsValuations();
	double GetBiddingAgressiveness() const { return m_fBiddingAggressiveness; }
	int	GetSuitSequenceOption() const { return m_nSuitSeqOption; }
	LPCTSTR GetSuitName(int nSuit) const { return szSuitName[nSuit]; }
	BOOL InExpressAutoPlay() const { return (m_nPlayMode == PLAY_FULL_AUTO_EXPRESS); }
	BOOL IsBiddingInProgress() const { return m_bBiddingInProgress; }
	BOOL IsGameInProgress() const { return m_bGameInProgress; }
	BOOL IsRubberInProgress() const { return m_bRubberInProgress; }
	BOOL IsUsingDuplicateScoring() const { return m_bDuplicateScoring; }
	BOOL IsAutoHintEnabled() const { return (m_nAutoHintMode > 0); }
	int GetAutoHintLevel() const { return m_nAutoHintMode; }
	int GetNumAcesRequiredForSlam(int nIndex) const { return m_numAcesForSlam[nIndex]; }
	int GetNumKingsRequiredForSlam(int nIndex) const { return m_numKingsForSlam[nIndex]; }
	void SetNumAcesRequiredForSlam(int nIndex, int nCount) { m_numAcesForSlam[nIndex] = nCount; }
	void SetNumKingsRequiredForSlam(int nIndex, int nCount) { m_numKingsForSlam[nIndex] = nCount; }
	double GetHonorValue() const;
	CGIB* GetGIB() { return m_pGIBWrapper; }
//	CNeuralNet* GetNeuralNet() { return m_pNeuralNet; }

// static data
public:
	//
	static double m_fDefaultMajorSuitGamePts;
	static double m_fDefaultMinorSuitGamePts;
	static double m_fDefaultNTGamePts;
	static double m_fDefault4LevelPts;
	static double m_fDefault3LevelPts;
	static double m_fDefault2LevelPts;
	static double m_fDefaultSlamPts;
	static double m_fDefaultGrandSlamPts;
	//
	static double m_fMajorSuitGamePts;
	static double m_fMinorSuitGamePts;
	static double m_fNTGamePts;
	static double m_f4LevelPts;
	static double m_f3LevelPts;
	static double m_f2LevelPts;
	static double m_fSlamPts;
	static double m_fGrandSlamPts;
	//
	static double m_fHonorValue[15];

// pricate data
private:
	// prog info
	CString		m_strProgTitle;
	CString		m_strProgPath;
	CString		m_strProgDirectory;
	int			m_nProgVersion[4];
	int			m_nProgMajorVersion;
	int			m_nProgMinorVersion;
	int			m_nProgIncrementVersion;
	int			m_nProgBuildNumber;
	CString	    m_strProgCopyright;
	CString		m_strProgBuildDate;
	CString		m_strSpecialBuildCode;
	//
	CDocument* m_pActiveDocument;
	//
	int		m_nCurrConventionSet;
	int		m_numConventionSets;
	int		m_nSuitSeqList[5][4];
	int		m_nSuitSequence[4];
	int		m_nDummySuitSequence[4];
	int		m_nSuitSeqOption;
	BOOL	m_bLowResOption;
	BOOL	m_bToggleResolutionMode;
	BOOL	m_bLayoutFollowsDisplayOrder;
	BOOL	m_bShowDummyTrumpsOnLeft;

	//
	// program status
	//
	int		m_nGameMode;			// 0=practice, 1=rubber
	BOOL	m_bBiddingInProgress;	// bidding
	BOOL	m_bGameInProgress;		// playing tricks
	BOOL	m_bRubberInProgress;	// playing rubber?
	BOOL	m_bDuplicateScoring;	// duplicate scoring?
	int		m_nAutoHintMode;		// Auto Hint in effect?
	int		m_nAutoHintTraceLevel;  // Auto Hint trace level
	BOOL	m_bFirstTimeRunning;	// first time running?

	// debug options
	BOOL	m_bDebugMode;			// debug mode
	int		m_nTestMode;
	BOOL	m_bShowCardsFaceUp;
	BOOL	m_bManualPlayMode;
	int		m_nPlayMode;
	BOOL	m_bPlayModeLocked;
	BOOL	m_bAutoTestMode;

	// display options
	BOOL	m_bAutoAlignDialogs;
	BOOL	m_bShowStartupAnimation;
	BOOL	m_bShowSplashWindow;
	BOOL	m_bEnableSpokenBids;
	BOOL	m_bSaveIntermediatePositions;
	BOOL	m_bExposePBNGameCards;
	BOOL    m_bShowBackgroundBitmap;
	BOOL	m_bShowDailyTipDialog;
	BOOL	m_bShowScreenSizeWarning;
	int		m_nBitmapDisplayMode;
	BOOL	m_bScaleLargeBitmaps;
	int		m_nBackgroundColor;
	BOOL	m_bUseSuitSymbols;
	BOOL	m_bCollapseGameReviewDialog;
	BOOL	m_bCollapseRoundFinishedDialog;

	// game options
	BOOL	m_bAutoBidStart;
	BOOL	m_bAutoJumpCursor;
	BOOL	m_bAutoPlayLastCard;
	BOOL	m_bEnableAnalysisTracing;
	BOOL	m_bEnableAnalysisDuringHints;
	int		m_nAnalysisTraceLevel;
	BOOL	m_bShowCommentIdentifiers;
	BOOL	m_bInsertBiddingPause;
	int		m_nBiddingPauseLength;
	BOOL	m_bInsertPlayPause;
	int		m_nPlayPauseLength;
	BOOL	m_bComputerCanClaim;
	BOOL	m_bShowPassedHands;
	BOOL	m_bAllowRebidPassedHands;
	int		m_nPassedHandWaitInterval;
	BOOL	m_bAutoShowBidHistory;
	BOOL	m_bAutoShowPlayHistory;
	BOOL	m_bAutoHideBidHistory;
	BOOL	m_bAutoHidePlayHistory;
	BOOL	m_bAutoShowNNetOutputWhenTraining;
	BOOL 	m_bShowLayoutOnEdit;

	// bidding config
	double		m_fBiddingAggressiveness;
	BOOL		m_bManualBidding;		// transient
/*
	int			m_nBiddingEngine;
	BOOL		m_bNNetTrainingMode;
	int			m_numNNetHiddenLayers;
	int			m_numNNetNodesPerHiddenLayer;
	CString		m_strNeuralNetFile;
	CNeuralNet*	m_pNeuralNet;
*/

	// counting options
	int		m_nHonorsValuationMode;
	double	m_fAceValue;
	double	m_fKingValue;
	double	m_fQueenValue;
	double	m_fJackValue;
	double  m_fTenValue;
	double	m_fCustomAceValue;
	double	m_fCustomKingValue;
	double	m_fCustomQueenValue;
	double	m_fCustomJackValue;
	double	m_fCustomTenValue;
	BOOL	m_bAcelessPenalty;
	BOOL	m_b4AceBonus;
	BOOL	m_bPenalizeUGHonors;
	BOOL	m_bCountShortSuits;

	// deal options
	int 	m_nReqPointsGame[4][2];
	int 	m_nReqPointsSlam[3][2];
	int 	m_nPointsAbsGameLimits[4][2];
	int 	m_nPointsAbsSlamLimits[3][2];
	BOOL	m_bBalanceTeamHands;
	BOOL	m_bGiveSouthBestHandInPartnership;
	BOOL	m_bGiveSouthBestHandInGame;
	int 	m_nMinCardsInMajor;
	int 	m_nMinCardsInMinor;
	int		m_nMinSuitDistTable[2][4][2];
	int		m_nMinSuitDist[2];
	int 	m_nMinTopMajorCard;
	int 	m_nMinTopMinorCard;
	int 	m_nMaxImbalanceForNT;
	BOOL	m_bNeedTwoBalancedTrumpHands;
	int		m_numAcesForSlam[3];
	int		m_numKingsForSlam[3];
	BOOL	m_bEnableDealNumbering;
	// scoring options
	BOOL	m_bScoreHonorsBonuses;

	// GIB options
	CGIB*	m_pGIBWrapper;
	CString	m_strGIBPath;
	BOOL	m_bEnableGIBForDeclarer;
	BOOL	m_bEnableGIBForDefender;
	int		m_nGIBAnalysisTime;
	int		m_nGIBSampleSize;
	BOOL	m_bShowGIBOutput;

	//
	CSplashWnd*	m_pSplash;
	DWORD 		m_dwSplashTime;


// internal routines
public:
	CEasyBApp();
private:
	void Initialize();
	void Terminate();
	void InitSettings();
	void InitNeuralNet();
	void CloseNeuralNet();
	void SetSuitSequence(int nSeq);
	// helpful functions
	int	MinimumOpeningValue(CPlayer* pPlayer=NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEasyBApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CEasyBApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

extern CEasyBApp theApp;
extern CConventionSet* pConventionSet[];
extern CConventionSet* pCurrConvSet;


#endif