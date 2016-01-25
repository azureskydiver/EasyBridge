//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------
/*   NCR Changes:
    Added def for CommandLine and code to parse a /U: parm to id registry for a user
*/

//
// EasyB.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "EasyB.h"
#include "mainfrm.h"
#include "EasyBdoc.h"
#include "EasyBvw.h"
#include "Player.h"
#include "progopts.h"
#include "playeropts.h"
#include "bidopts.h"
#include "bidparams.h"
#include "dealparm.h"
#include "ConventionSet.h"
#include "deck.h"
#include "card.h"
#include "SplashWnd.h"
#include "GIB.h"
//#include "NeuralNet.h"
#include "play.h"
#include <mmsystem.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


// default order in which suits get displayed, left to right
const int defSuitDisplaySequence[5][4] = {
	{ SPADES, HEARTS, CLUBS, DIAMONDS },
	{ SPADES, HEARTS, DIAMONDS, CLUBS },
	{ DIAMONDS, CLUBS, HEARTS, SPADES },
	{ CLUBS, DIAMONDS, HEARTS, SPADES },
	{ HEARTS, SPADES, DIAMONDS, CLUBS },
};
const int tnumSuitSequences = sizeof(defSuitDisplaySequence) / sizeof(int[4]);

const LPCTSTR szPosition[4] = { "South", "West", "North", "East" };

//const LPCTSTR tszAppAboutWAVFile = _T("Ocean.wav");

// default counting options
const int tnDefaultAceValue			= 4;
const int tnDefaultKingValue		= 3;
const int tnDefaultQueenValue		= 2;
const int tnDefaultJackValue		= 1;
LPCTSTR tszDefaultAceValue			= _T("4");
LPCTSTR tszDefaultKingValue			= _T("3");
LPCTSTR tszDefaultQueenValue		= _T("2");
LPCTSTR tszDefaultJackValue			= _T("1");
//
const double tfGinsburgAceValue = 4.5;
const double tfGinsburgKingValue = 3.0;
const double tfGinsburgQueenValue = 1.5;
const double tfGinsburgJackValue = 0.8;

// global pointer to the current convention set
CConventionSet* pCurrConvSet = NULL;

// neural net settings
const int tnumNNetInputs				= 350;
//const int tnumNNetOutputs				= 38;
const int tnumNNetOutputs				= 608;
const int tnumNNetHiddenLayers			= 1;
const int tnumNNetNodesPerHiddenLayer	= 100;

//
const double c_tfDefaultMajorSuitGamePts	= 26;
const double c_tfDefaultMinorSuitGamePts	= 29;
const double c_tfDefaultNTGamePts		= 26;
const double c_tfDefault4LevelPts		= 26;
const double c_tfDefault3LevelPts		= 23;
const double c_tfDefault2LevelPts		= 20;
const double c_tfDefaultSlamPts			= 33;
const double c_tfDefaultGrandSlamPts	= 37;

// static data
double	CEasyBApp::m_fDefaultMajorSuitGamePts	= c_tfDefaultMajorSuitGamePts;
double	CEasyBApp::m_fDefaultMinorSuitGamePts	= c_tfDefaultMinorSuitGamePts;
double	CEasyBApp::m_fDefaultNTGamePts			= c_tfDefaultNTGamePts;
double	CEasyBApp::m_fDefault4LevelPts			= c_tfDefault4LevelPts;
double	CEasyBApp::m_fDefault3LevelPts			= c_tfDefault3LevelPts;
double	CEasyBApp::m_fDefault2LevelPts			= c_tfDefault2LevelPts;
double	CEasyBApp::m_fDefaultSlamPts			= c_tfDefaultSlamPts;
double	CEasyBApp::m_fDefaultGrandSlamPts		= c_tfDefaultGrandSlamPts;
//
double	CEasyBApp::m_fMajorSuitGamePts	= c_tfDefaultMajorSuitGamePts;
double	CEasyBApp::m_fMinorSuitGamePts	= c_tfDefaultMinorSuitGamePts;
double	CEasyBApp::m_fNTGamePts			= c_tfDefaultNTGamePts;
double	CEasyBApp::m_f4LevelPts			= c_tfDefault4LevelPts;
double	CEasyBApp::m_f3LevelPts			= c_tfDefault3LevelPts;
double	CEasyBApp::m_f2LevelPts			= c_tfDefault2LevelPts;
double	CEasyBApp::m_fSlamPts			= c_tfDefaultSlamPts;
double	CEasyBApp::m_fGrandSlamPts		= c_tfDefaultGrandSlamPts;

double CEasyBApp::m_fHonorValue[] = {
	// 0  1  2  3  4  5  6  7  8  9  T  J  Q  K  A
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


// NCR begin addition
class NormsCommandLineInfo : public CCommandLineInfo
{
public:
	NormsCommandLineInfo() {m_bHaveRegKey = FALSE;}
	BOOL m_bHaveRegKey;
	CString m_RegKey;
	virtual void ParseParam(const char* pszParam,BOOL bFlag,BOOL bLast);
};
void NormsCommandLineInfo::ParseParam(const char* pszParam,BOOL bFlag,BOOL bLast){
	if (bFlag)	{
		if (pszParam[0] == 'U' && pszParam[1] == ':')		{
			CString param = pszParam;
//			AfxMessageBox("param is >" + param + "<");
			if(param.GetLength() > 2) {
               // strip off the leading U: and add a leading blank
			   m_RegKey = " " + param.Right(param.GetLength() -2); // leading blank
			   m_bHaveRegKey = TRUE;
			}
			return;
		}
	}
	CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
}
// NCE end addition

/////////////////////////////////////////////////////////////////////////////
//
// Global Variables
//
/////////////////////////////////////////////////////////////////////////////

//
CEasyBApp			theApp;
CConventionSet*		pConventionSet[100];





/////////////////////////////////////////////////////////////////////////////
// CEasyBApp

BEGIN_MESSAGE_MAP(CEasyBApp, CWinApp)
	//{{AFX_MSG_MAP(CEasyBApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
//	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()





/////////////////////////////////////////////////////////////////////////////
// CEasyBApp construction

CEasyBApp::CEasyBApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// display options
static TCHAR BASED_CODE szDisplayOptions[] = _T("Display Options");
static TCHAR BASED_CODE szSuitSequence[] = _T("Suit Display Order");
static TCHAR BASED_CODE szLowResOption[] = _T("Low Resolution Option");
static TCHAR BASED_CODE szLayoutFollowsDisplayOrder[] = _T("Card Layout Dialog follows Display Suit Order");
static TCHAR BASED_CODE szShowDummyTrumpsOnLeft[] = _T("Show Dummy Trumps on Left");
static TCHAR BASED_CODE szShowStartupAnimation[] = _T("Show Startup Animation");
static TCHAR BASED_CODE szShowSplashWindow[] = _T("Show Splash Window");
static TCHAR BASED_CODE szShowDailyTips[] = _T("Show Tip of the Day");
static TCHAR BASED_CODE szShowScreenSizeWarning[] = _T("Show Screen Size Warning");
static TCHAR BASED_CODE szShowBackgroundBitmap[] = _T("Show Background Bitmap Display");
static TCHAR BASED_CODE szBitmapDisplayMode[] = _T("Background Bitmap Display Mode (0=Tile, 1=Center)");
static TCHAR BASED_CODE szScaleLargeBitmaps[] = _T("Scale Large Background Bitmaps");
static TCHAR BASED_CODE szBackgroundColor[] = _T("Window Background Color");
static TCHAR BASED_CODE szUseSuitSymbols[] = _T("Use Suit Symbols Instead of Letters");
static TCHAR BASED_CODE szShowLayoutOnEdit[] = _T("Automatically Show Card Layout Dialog When Editing Existing Hands");
static TCHAR BASED_CODE szCollapseGameReviewDialog[] = _T("Hide tags list in Game Review dialog");

// game options
static TCHAR BASED_CODE szGameOptions[] = _T("Game Options");
static TCHAR BASED_CODE szAutoBidStart[] = _T("Auto Start Bidding after Deal");
static TCHAR BASED_CODE szEnableAnalysisTracing[] = _T("Enable Computer Player Analysis Tracing");
static TCHAR BASED_CODE szEnableAnalysisDuringHints[] = _T("Enable Analysis Tracing When Obtaining Hints");
static TCHAR BASED_CODE szAutoHintMode[] = _T("Auto Hint Level");
static TCHAR BASED_CODE szAutoHintTraceLevel[] = _T("Auto Hint Trace Level");
static TCHAR BASED_CODE szAnalysisTraceLevel[] = _T("Computer Player Analysis Trace Level");
//static TCHAR BASED_CODE szShowCommentIdentifiers[] = _T("Suppress Comment Identifier Codes");
static TCHAR BASED_CODE szShowCommentIdentifiers[] = _T("Show Comment Identifier Codes");
static TCHAR BASED_CODE szInsertBiddingPause[] = _T("Insert Pause between Bids");
static TCHAR BASED_CODE szBiddingPauseLength[] = _T("Pause Length between Bids (tenths of a second)");
static TCHAR BASED_CODE szInsertPlayPause[] = _T("Insert Pause between Plays");
static TCHAR BASED_CODE szPlayPauseLength[] = _T("Pause Length between Plays (tenths of a second)");
static TCHAR BASED_CODE szComputerCanClaim[] = _T("Computer Player Can Claim Remaining Tricks");
static TCHAR BASED_CODE szShowPassedHands[] = _T("Show All Four Hands When a Hand Is Passed Out");
static TCHAR BASED_CODE szAllowRebidPassedHands[] = _T("Allow Rebid On a Passed Hand");
static TCHAR BASED_CODE szPassedHandWaitInterval[] = _T("Time to Wait After Passed Hand to Deal New Hand");
static TCHAR BASED_CODE szAutoShowBidHistory[] = _T("Automatically Show Bid History When Biddding");
static TCHAR BASED_CODE szAutoShowPlayHistory[] = _T("Automatically Show Play History During Play");
static TCHAR BASED_CODE szAutoHideBidHistory[] = _T("Automatically Hide Bid History During Play");
static TCHAR BASED_CODE szAutoHidePlayHistory[] = _T("Automatically Hide Play History When Bidding");
//static TCHAR BASED_CODE szAutoShowNNetOutputWhenTraining[] = _T("Automatically show Neural Net Output Dialog When in Training Mode");
static TCHAR BASED_CODE szPlayMode[] = _T("Game Play Mode");
static TCHAR BASED_CODE szPlayModeLocked[] = _T("Game Play Mode Locked");
static TCHAR BASED_CODE szAutoJumpCursor[] = _T("Automatically Jump the Cursor");
static TCHAR BASED_CODE szAutoPlayLastCard[] = _T("Automatically play the last card in the suit led");
static TCHAR BASED_CODE szEnableSpokenBids[] = _T("Enable Spoken Bids");
static TCHAR BASED_CODE szSaveIntermediatePositions[] = _T("Save Intermediate Positions When Saving Hands");
static TCHAR BASED_CODE szExposePBNGameCards[] = _T("Expose All Cards When a PBN Game Is Loaded");

// GIB options
static TCHAR BASED_CODE szGIBOptions[] = _T("GIB Options");
static TCHAR BASED_CODE szGIBPath[] = _T("GIB Executable Path");
static TCHAR BASED_CODE szEnableGIBForDeclarer[] = _T("Enable GIB for Declarer Play");
static TCHAR BASED_CODE szEnableGIBForDefender[] = _T("Enable GIB for Defender Play");
static TCHAR BASED_CODE szGIBAnalysisTime[] = _T("GIB Max Analysis Time");
static TCHAR BASED_CODE szGIBSampleSize[] = _T("GIB Monte Carlo Deal Sample Size");
static TCHAR BASED_CODE szShowGIBOutput[] = _T("Show GIB Program Output");

// counting options
static TCHAR BASED_CODE szCountingOptions[] = _T("Counting Options");
static TCHAR BASED_CODE szAggressivenessRating[] = _T("Bidding Aggressiveness Rating (-2 to + 2)");
static TCHAR BASED_CODE szHonorsValuationMode[] = _T("Honors Valuation Mode");
static TCHAR BASED_CODE szCustomAceValue[] = _T("Custom Value of an Ace");
static TCHAR BASED_CODE szCustomKingValue[] = _T("Custom value of a King");
static TCHAR BASED_CODE szCustomQueenValue[] = _T("Custom value of a Queen");
static TCHAR BASED_CODE szCustomJackValue[] = _T("Custom value of a Jack");
static TCHAR BASED_CODE szMajorSuitGamePts[] = _T("Default Points needed for Game in a Major Suit");
static TCHAR BASED_CODE szMinorSuitGamePts[] = _T("Default Points needed for Game in a Minor Suit");
static TCHAR BASED_CODE szNTGamePts[] = _T("Default Points needed for Game in No Trump");
static TCHAR BASED_CODE sz4LevelPts[] = _T("Default Points needed for a Level 4 Countract");
static TCHAR BASED_CODE sz3LevelPts[] = _T("Default Points needed for a Level 3 Countract");
static TCHAR BASED_CODE sz2LevelPts[] = _T("Default Points needed for a Level 2 Countract");
static TCHAR BASED_CODE szSlamPts[] = _T("Default Points needed for a Small Slam bid");
static TCHAR BASED_CODE szGrandSlamPts[] = _T("Default Points needed for a Grand Slam bid");
static TCHAR BASED_CODE szAcelessPenalty[] = _T("Deduct 1 pt for Aceless Hand");
static TCHAR BASED_CODE sz4AceBonus[] = _T("Add 1 pt for Holding All 4 Aces");
static TCHAR BASED_CODE szPenalizeUGHonors[] = _T("Penalize Unguarded/Poorly Guarded Honors");
static TCHAR BASED_CODE szCountShortSuits[] = _T("Count Points for Short Suits When Opening");

// deal options
static TCHAR BASED_CODE szDealOptions[] = _T("Deal Options");
static TCHAR BASED_CODE szMinGamePts[] = _T("Min Points for Game Hand");
static TCHAR BASED_CODE szMaxGamePts[] = _T("Max Points for Game Hand");
static TCHAR BASED_CODE szMinMinorGamePts[] = _T("Min Points for Minor Game Hand");
static TCHAR BASED_CODE szMaxMinorGamePts[] = _T("Max Points for Minor Game Hand");
static TCHAR BASED_CODE szMinMajorGamePts[] = _T("Min Points for Major Game Hand");
static TCHAR BASED_CODE szMaxMajorGamePts[] = _T("Max Points for Major Game Hand");
static TCHAR BASED_CODE szMinNoTrumpGamePts[] = _T("Min Points for No Trump Game Hand");
static TCHAR BASED_CODE szMaxNoTrumpGamePts[] = _T("Max Points for No Trump Game Hand");
static TCHAR BASED_CODE szMinSlamPts[] = _T("Min Points for Slam Hand");
static TCHAR BASED_CODE szMaxSlamPts[] = _T("Max Points for Slam Hand");
static TCHAR BASED_CODE szMinSmallSlamPts[] = _T("Min Points for Small Slam Hand");
static TCHAR BASED_CODE szMaxSmallSlamPts[] = _T("Max Points for Small Slam Hand");
static TCHAR BASED_CODE szMinGrandSlamPts[] = _T("Min Points for Grand Slam Hand");
static TCHAR BASED_CODE szMaxGrandSlamPts[] = _T("Max Points for Grand Slam Hand");
static TCHAR BASED_CODE szBalanceTeamHands[] = _T("Try to Balance Partnership Hands Points");
static TCHAR BASED_CODE szMinCardsInMajor[] = _T("Min # Cards in Major Suit");
static TCHAR BASED_CODE szMinCardsInMinor[] = _T("Min # Cards in Minor Suit");
static TCHAR BASED_CODE szMinSuitDistMinor[] = _T("Min Distribution for Minor Suit");
static TCHAR BASED_CODE szMinSuitDistMajor[] = _T("Min Distribution for Major Suit");
static TCHAR BASED_CODE szMinTopMajorCard[] = _T("Min Top Cards in Major Suit");
static TCHAR BASED_CODE szMinTopMinorCard[] = _T("Min Top Cards in Minor Suit");
static TCHAR BASED_CODE szMaxImbalanceForNT[] = _T("Max Imbalance for NT Game");
static TCHAR BASED_CODE szNeedTwoBalancedHandsForNT[] = _T("Need 2 Balanced Hands for NT Game");
static TCHAR BASED_CODE szAcesNeededForSlam[] = _T("Number of Aces to Deal for Slam Hand");
static TCHAR BASED_CODE szAcesNeededForSmallSlam[] = _T("Min Number of Aces to Deal for Small Slam Hand");
static TCHAR BASED_CODE szAcesNeededForGrandSlam[] = _T("Min Number of Aces to Deal for Grand Slam Hand");
static TCHAR BASED_CODE szKingsNeededForSlam[] = _T("Min Number of Kings to Deal for Slam Hand");
static TCHAR BASED_CODE szKingsNeededForSmallSlam[] = _T("Min Number of Kings to Deal for Small Slam Hand");
static TCHAR BASED_CODE szKingsNeededForGrandSlam[] = _T("Min Number of Kings to Deal for Grand Slam Hand");
static TCHAR BASED_CODE szEnableDealNumbering[] = _T("Enable Deal Numbering and Recall");

// scoring options
static TCHAR BASED_CODE szScoringOptions[] = _T("Scoring Options");
static TCHAR BASED_CODE szUseDuplicateScoring[] = _T("Use Duplicate Scoring");
static TCHAR BASED_CODE szScoreHonorsBonuses[] = _T("Score Honors Bonuses");

// convention settings
static TCHAR BASED_CODE szCurrConventionSet[] = _T("Current Convention Set");

// bidding config 
static TCHAR BASED_CODE szBiddingConfigOptions[] = _T("Bidding Configuration");
//static TCHAR BASED_CODE szBiddingEngine[] = _T("Bidding Engine (0 = Rule-Based, 1 = Neural Net)");
static TCHAR BASED_CODE szBiddingAggressiveness[] = _T("Bidding Aggressiveness (-2 to +2)");
/*
static TCHAR BASED_CODE szNumNNetHiddenLayers[] = _T("Default Neural Hidden Layers");
static TCHAR BASED_CODE szNumNNetNodesPerHiddenLayer[] = _T("Default Neural Nodes Per Hidden Layer");
static TCHAR BASED_CODE szNeuralNetFile[] = _T("Nueral Net File");
*/

// play options
static TCHAR BASED_CODE szPlayOptions[] = _T("Game Play Options");

// debug options
static TCHAR BASED_CODE szDebugOptions[] = _T("Debug Options");
static TCHAR BASED_CODE szEnableDebugMode[] = _T("Enable Debug Mode");
static TCHAR BASED_CODE szExposeAllCards[] = _T("Expose All Cards");


//
void CEasyBApp::Initialize()
{
	// read in display options
	m_nSuitSeqOption = GetProfileInt(szDisplayOptions, szSuitSequence, 0);
	m_bLayoutFollowsDisplayOrder = GetProfileInt(szDisplayOptions, szLayoutFollowsDisplayOrder, TRUE);
	m_bLowResOption = GetProfileInt(szDisplayOptions, szLowResOption, FALSE);
	m_bShowDummyTrumpsOnLeft = GetProfileInt(szDisplayOptions, szShowDummyTrumpsOnLeft, TRUE);
	m_bShowStartupAnimation = GetProfileInt(szDisplayOptions, szShowStartupAnimation, FALSE);
	m_bShowSplashWindow = GetProfileInt(szDisplayOptions, szShowSplashWindow, TRUE);
	m_bShowDailyTipDialog = GetProfileInt(szDisplayOptions, szShowDailyTips, TRUE);
	m_bShowScreenSizeWarning = GetProfileInt(szDisplayOptions, szShowScreenSizeWarning, TRUE);
	m_bShowBackgroundBitmap = GetProfileInt(szDisplayOptions, szShowBackgroundBitmap, TRUE);
	m_nBitmapDisplayMode = GetProfileInt(szDisplayOptions, szBitmapDisplayMode, 1);
	m_bScaleLargeBitmaps = GetProfileInt(szDisplayOptions, szScaleLargeBitmaps, FALSE);
	m_nBackgroundColor = GetProfileInt(szDisplayOptions, szBackgroundColor, RGB(0,128,0));
	m_bUseSuitSymbols = GetProfileInt(szDisplayOptions, szUseSuitSymbols, FALSE);
	m_bShowLayoutOnEdit = GetProfileInt(szDisplayOptions, szShowLayoutOnEdit, FALSE);
	m_bCollapseGameReviewDialog = GetProfileInt(szDisplayOptions, szCollapseGameReviewDialog, FALSE);

	// read in game options
	m_bAutoBidStart = GetProfileInt(szGameOptions, szAutoBidStart, TRUE);
	m_bEnableAnalysisTracing = GetProfileInt(szGameOptions, szEnableAnalysisTracing, FALSE);
	m_bEnableAnalysisDuringHints = GetProfileInt(szGameOptions, szEnableAnalysisDuringHints, TRUE);
	m_nAnalysisTraceLevel = GetProfileInt(szGameOptions, szAnalysisTraceLevel, 3);
#ifdef _DEBUG
	m_bShowCommentIdentifiers = GetProfileInt(szGameOptions, szShowCommentIdentifiers, TRUE);
#else
	m_bShowCommentIdentifiers = GetProfileInt(szGameOptions, szShowCommentIdentifiers, FALSE);
#endif
	m_nAutoHintMode = GetProfileInt(szGameOptions, szAutoHintMode, 0);
	m_nAutoHintTraceLevel = GetProfileInt(szGameOptions, szAutoHintTraceLevel, 2);
	m_bInsertBiddingPause = GetProfileInt(szGameOptions, szInsertBiddingPause, TRUE);
	m_nBiddingPauseLength = GetProfileInt(szGameOptions, szBiddingPauseLength, 3);
	m_bInsertPlayPause = GetProfileInt(szGameOptions, szInsertPlayPause, TRUE);
	m_nPlayPauseLength = GetProfileInt(szGameOptions, szPlayPauseLength, 10);
	m_bComputerCanClaim = GetProfileInt(szGameOptions, szComputerCanClaim, FALSE);
	m_bShowPassedHands = GetProfileInt(szGameOptions, szShowPassedHands, TRUE);
	m_bAllowRebidPassedHands = GetProfileInt(szGameOptions, szAllowRebidPassedHands, FALSE);
	m_nPassedHandWaitInterval = GetProfileInt(szGameOptions, szPassedHandWaitInterval, 1100);
	m_bAutoShowBidHistory = GetProfileInt(szGameOptions, szAutoShowBidHistory, TRUE);
	m_bAutoShowPlayHistory = GetProfileInt(szGameOptions, szAutoShowPlayHistory, TRUE);
	m_bAutoHideBidHistory = GetProfileInt(szGameOptions, szAutoHideBidHistory, FALSE);
	m_bAutoHidePlayHistory = GetProfileInt(szGameOptions, szAutoHidePlayHistory, FALSE);
//	m_bAutoShowNNetOutputWhenTraining = GetProfileInt(szGameOptions, szAutoShowNNetOutputWhenTraining, TRUE);
	m_bAutoJumpCursor = GetProfileInt(szGameOptions, szAutoJumpCursor, FALSE);
	m_bAutoPlayLastCard = GetProfileInt(szGameOptions, szAutoPlayLastCard, TRUE);
//	m_bEnableSpokenBids = GetProfileInt(szGameOptions, szEnableSpokenBids, FALSE);
	m_bSaveIntermediatePositions = GetProfileInt(szGameOptions, szSaveIntermediatePositions, TRUE);
	m_bExposePBNGameCards = GetProfileInt(szGameOptions, szExposePBNGameCards, TRUE);
#ifdef _DEBUG
	m_nPlayMode = GetProfileInt(szGameOptions, szPlayMode, PLAY_NORMAL);
	m_bPlayModeLocked = GetProfileInt(szGameOptions, szPlayModeLocked, FALSE);
#else
	m_nPlayMode = m_bPlayModeLocked = FALSE;
#endif

	// counting parameters
	m_nHonorsValuationMode = GetProfileInt(szCountingOptions, szHonorsValuationMode, 0);
	m_fCustomAceValue = atof(GetProfileString(szCountingOptions, szCustomAceValue, tszDefaultAceValue));
	m_fCustomKingValue = atof(GetProfileString(szCountingOptions, szCustomKingValue, tszDefaultKingValue));
	m_fCustomQueenValue = atof(GetProfileString(szCountingOptions, szCustomQueenValue, tszDefaultQueenValue));
	m_fCustomJackValue = atof(GetProfileString(szCountingOptions, szCustomJackValue, tszDefaultJackValue));
	m_fDefaultMajorSuitGamePts = atof(GetProfileString(szCountingOptions, szMajorSuitGamePts, FormString("%f", c_tfDefaultMajorSuitGamePts)));
	m_fDefaultMinorSuitGamePts = atof(GetProfileString(szCountingOptions, szMinorSuitGamePts, FormString("%f", c_tfDefaultMinorSuitGamePts)));
	m_fDefaultNTGamePts = atof(GetProfileString(szCountingOptions, szNTGamePts, FormString("%f", c_tfDefaultNTGamePts)));
	m_fDefault4LevelPts = atof(GetProfileString(szCountingOptions, sz4LevelPts, FormString("%f", c_tfDefault4LevelPts)));
	m_fDefault3LevelPts = atof(GetProfileString(szCountingOptions, sz3LevelPts, FormString("%f", c_tfDefault3LevelPts)));
	m_fDefault2LevelPts = atof(GetProfileString(szCountingOptions, sz2LevelPts, FormString("%f", c_tfDefault2LevelPts)));
	m_fDefaultSlamPts = atof(GetProfileString(szCountingOptions, szSlamPts, FormString("%f", c_tfDefaultSlamPts)));
	m_fDefaultGrandSlamPts = atof(GetProfileString(szCountingOptions, szGrandSlamPts, FormString("%f", c_tfDefaultGrandSlamPts)));
// NCR-249 Reinstated Aceless Penalty
	m_bAcelessPenalty = GetProfileInt(szCountingOptions, szAcelessPenalty, TRUE);
/*
	m_b4AceBonus = GetProfileInt(szCountingOptions, sz4AceBonus, TRUE);
	m_bPenalizeUGHonors = GetProfileInt(szCountingOptions, szPenalizeUGHonors, FALSE);
	m_bCountShortSuits = GetProfileInt(szCountingOptions, szCountShortSuits, TRUE);
*/
//NCR	m_bAcelessPenalty = TRUE;
	m_b4AceBonus = TRUE;
	m_bPenalizeUGHonors = TRUE;
	m_bCountShortSuits = TRUE;

	// get GIB options
	m_strGIBPath = GetProfileString(szGIBOptions, szGIBPath, "C:\\");
	m_bEnableGIBForDeclarer = GetProfileInt(szGIBOptions, szEnableGIBForDeclarer, FALSE);
//	m_bEnableGIBForDefender = GetProfileInt(szGIBOptions, szEnableGIBForDefender, FALSE);
	m_bEnableGIBForDefender = FALSE;
	m_nGIBAnalysisTime = GetProfileInt(szGIBOptions, szGIBAnalysisTime, 30);
	m_nGIBSampleSize = GetProfileInt(szGIBOptions, szGIBSampleSize, 100);
	m_bShowGIBOutput = GetProfileInt(szGIBOptions, szShowGIBOutput, FALSE);

//	m_nCurrConventionSet = GetProfileInt(szBiddingConventions, szCurrConvSet, 0);
	m_nCurrConventionSet = 0;	

	// get deal options
	m_nReqPointsGame[0][0] = GetProfileInt(szDealOptions, szMinGamePts, nDefReqPointsGame[0][0]);
	m_nReqPointsGame[0][1] = GetProfileInt(szDealOptions, szMaxGamePts, nDefReqPointsGame[0][1]);
	m_nReqPointsGame[1][0] = GetProfileInt(szDealOptions, szMinMinorGamePts, nDefReqPointsGame[1][0]);
	m_nReqPointsGame[1][1] = GetProfileInt(szDealOptions, szMaxMinorGamePts, nDefReqPointsGame[1][1]);
	m_nReqPointsGame[2][0] = GetProfileInt(szDealOptions, szMinMajorGamePts, nDefReqPointsGame[2][0]);
	m_nReqPointsGame[2][1] = GetProfileInt(szDealOptions, szMaxMajorGamePts, nDefReqPointsGame[2][1]);
	m_nReqPointsGame[3][0] = GetProfileInt(szDealOptions, szMinNoTrumpGamePts, nDefReqPointsGame[3][0]);
	m_nReqPointsGame[3][1] = GetProfileInt(szDealOptions, szMaxNoTrumpGamePts, nDefReqPointsGame[3][1]);
	m_nReqPointsSlam[0][0] = GetProfileInt(szDealOptions, szMinSlamPts, nDefReqPointsSlam[0][0]);
	m_nReqPointsSlam[0][1] = GetProfileInt(szDealOptions, szMaxSlamPts, nDefReqPointsSlam[0][1]);
	m_nReqPointsSlam[1][0] = GetProfileInt(szDealOptions, szMinSmallSlamPts, nDefReqPointsSlam[1][0]);
	m_nReqPointsSlam[1][1] = GetProfileInt(szDealOptions, szMaxSmallSlamPts, nDefReqPointsSlam[1][1]);
	m_nReqPointsSlam[2][0] = GetProfileInt(szDealOptions, szMinGrandSlamPts, nDefReqPointsSlam[2][0]);
	m_nReqPointsSlam[2][1] = GetProfileInt(szDealOptions, szMaxGrandSlamPts, nDefReqPointsSlam[2][1]);
	m_bBalanceTeamHands = GetProfileInt(szDealOptions, szBalanceTeamHands, bDefBalanceTeamHands);
	//
	m_nMinCardsInMajor = GetProfileInt(szDealOptions, szMinCardsInMajor, nDefMinCardsInMajor);
	m_nMinCardsInMinor = GetProfileInt(szDealOptions, szMinCardsInMinor, nDefMinCardsInMinor);
	m_nMinSuitDist[0] = GetProfileInt(szDealOptions, szMinSuitDistMinor, nDefMinSuitDist[0]);
	m_nMinSuitDist[1] = GetProfileInt(szDealOptions, szMinSuitDistMajor, nDefMinSuitDist[1]);
	m_nMinTopMajorCard = GetProfileInt(szDealOptions, szMinTopMajorCard, nDefMinTopMajorCard);
	m_nMinTopMinorCard = GetProfileInt(szDealOptions, szMinTopMinorCard, nDefMinTopMinorCard);
//	m_nMaxImbalanceForNT = GetProfileInt(szDealOptions, szMaxImbalanceForNT, nDefMaxBalancedForNT);
	m_nMaxImbalanceForNT = nDefMaxBalancedForNT;
	m_bNeedTwoBalancedTrumpHands = GetProfileInt(szDealOptions, szNeedTwoBalancedHandsForNT, bDefNeedTwoBalancedTrumpHands);
	m_numAcesForSlam[0] = GetProfileInt(szDealOptions, szAcesNeededForSlam, nDefNumAcesForSlam[0]);
	m_numAcesForSlam[1] = GetProfileInt(szDealOptions, szAcesNeededForSmallSlam, nDefNumAcesForSlam[1]);
	m_numAcesForSlam[2] = GetProfileInt(szDealOptions, szAcesNeededForGrandSlam, nDefNumAcesForSlam[2]);
	m_numKingsForSlam[0] = GetProfileInt(szDealOptions, szAcesNeededForSlam, nDefNumKingsForSlam[0]);
	m_numKingsForSlam[1] = GetProfileInt(szDealOptions, szAcesNeededForSmallSlam, nDefNumKingsForSlam[1]);
	m_numKingsForSlam[2] = GetProfileInt(szDealOptions, szAcesNeededForGrandSlam, nDefNumKingsForSlam[2]);
	m_bEnableDealNumbering = GetProfileInt(szDealOptions, szEnableDealNumbering, TRUE);

	// get scoring options
	m_bDuplicateScoring = GetProfileInt(szScoringOptions, szUseDuplicateScoring, FALSE);
	m_bScoreHonorsBonuses = GetProfileInt(szScoringOptions, szScoreHonorsBonuses, FALSE);

	// get bidding config options
	m_fBiddingAggressiveness = atof(GetProfileString(szBiddingConfigOptions, szBiddingAggressiveness, 0));
/*
	m_nBiddingEngine = GetProfileInt(szBiddingConfigOptions, szBiddingEngine, 0);
	m_numNNetHiddenLayers = GetProfileInt(szBiddingConfigOptions, szNumNNetHiddenLayers, tnumNNetHiddenLayers);
	m_numNNetNodesPerHiddenLayer = GetProfileInt(szBiddingConfigOptions, szNumNNetNodesPerHiddenLayer, tnumNNetNodesPerHiddenLayer);
	m_strNeuralNetFile = GetProfileString(szBiddingConfigOptions, szNeuralNetFile, _T(""));
*/
	// get debug options
	m_bDebugMode = GetProfileInt(szDebugOptions, szEnableDebugMode, TRUE);
	m_bShowCardsFaceUp = GetProfileInt(szDebugOptions, szExposeAllCards, FALSE);
}

//
void CEasyBApp::Terminate()
{
	// read in display options
	WriteProfileInt(szDisplayOptions, szSuitSequence, m_nSuitSeqOption);
	WriteProfileInt(szDisplayOptions, szLayoutFollowsDisplayOrder, m_bLayoutFollowsDisplayOrder);
	WriteProfileInt(szDisplayOptions, szLowResOption, m_bToggleResolutionMode? !m_bLowResOption : m_bLowResOption);
	WriteProfileInt(szDisplayOptions, szShowDummyTrumpsOnLeft, m_bShowDummyTrumpsOnLeft);
	WriteProfileInt(szDisplayOptions, szShowStartupAnimation, m_bShowStartupAnimation);
	WriteProfileInt(szDisplayOptions, szShowSplashWindow, m_bShowSplashWindow);
	WriteProfileInt(szDisplayOptions, szShowDailyTips, m_bShowDailyTipDialog);
	WriteProfileInt(szDisplayOptions, szShowScreenSizeWarning, m_bShowScreenSizeWarning);
	WriteProfileInt(szDisplayOptions, szShowBackgroundBitmap, m_bShowBackgroundBitmap);
	WriteProfileInt(szDisplayOptions, szBitmapDisplayMode, m_nBitmapDisplayMode);
	WriteProfileInt(szDisplayOptions, szScaleLargeBitmaps, m_bScaleLargeBitmaps);
	WriteProfileInt(szDisplayOptions, szBackgroundColor, m_nBackgroundColor);
	WriteProfileInt(szDisplayOptions, szUseSuitSymbols, m_bUseSuitSymbols);
	WriteProfileInt(szDisplayOptions, szShowLayoutOnEdit, m_bShowLayoutOnEdit);
	WriteProfileInt(szDisplayOptions, szCollapseGameReviewDialog, m_bCollapseGameReviewDialog);

	// write out game options
	WriteProfileInt(szGameOptions, szAutoBidStart, m_bAutoBidStart);
	WriteProfileInt(szGameOptions, szEnableAnalysisTracing, m_bEnableAnalysisTracing);
	WriteProfileInt(szGameOptions, szEnableAnalysisDuringHints, m_bEnableAnalysisDuringHints);
	WriteProfileInt(szGameOptions, szAutoHintMode, m_nAutoHintMode);
	WriteProfileInt(szGameOptions, szAutoHintTraceLevel, m_nAutoHintTraceLevel);
	WriteProfileInt(szGameOptions, szAnalysisTraceLevel, m_nAnalysisTraceLevel);
	WriteProfileInt(szGameOptions, szShowCommentIdentifiers, m_bShowCommentIdentifiers);
	WriteProfileInt(szGameOptions, szInsertBiddingPause, m_bInsertBiddingPause);
	WriteProfileInt(szGameOptions, szBiddingPauseLength, m_nBiddingPauseLength);
	WriteProfileInt(szGameOptions, szInsertPlayPause, m_bInsertPlayPause);
	WriteProfileInt(szGameOptions, szPlayPauseLength, m_nPlayPauseLength);
	WriteProfileInt(szGameOptions, szComputerCanClaim, m_bComputerCanClaim);
	WriteProfileInt(szGameOptions, szShowPassedHands, m_bShowPassedHands);
	WriteProfileInt(szGameOptions, szAllowRebidPassedHands, m_bAllowRebidPassedHands);
	WriteProfileInt(szGameOptions, szPassedHandWaitInterval, m_nPassedHandWaitInterval);
	WriteProfileInt(szGameOptions, szAutoShowBidHistory, m_bAutoShowBidHistory);
	WriteProfileInt(szGameOptions, szAutoShowPlayHistory, m_bAutoShowPlayHistory);
	WriteProfileInt(szGameOptions, szAutoHideBidHistory, m_bAutoHideBidHistory);
	WriteProfileInt(szGameOptions, szAutoHidePlayHistory, m_bAutoHidePlayHistory);
//	WriteProfileInt(szGameOptions, szAutoShowNNetOutputWhenTraining, m_bAutoShowNNetOutputWhenTraining);
	WriteProfileInt(szGameOptions, szAutoJumpCursor, m_bAutoJumpCursor);
	WriteProfileInt(szGameOptions, szAutoPlayLastCard, m_bAutoPlayLastCard);
//	WriteProfileInt(szGameOptions, szEnableSpokenBids, m_bEnableSpokenBids);
	WriteProfileInt(szGameOptions, szSaveIntermediatePositions, m_bSaveIntermediatePositions);
	WriteProfileInt(szGameOptions, szExposePBNGameCards, m_bExposePBNGameCards);
#ifdef _DEBUG
	WriteProfileInt(szGameOptions, szPlayMode, m_nPlayMode);
	WriteProfileInt(szGameOptions, szPlayModeLocked, m_bPlayModeLocked);
#endif

	// write out counting parameters
	WriteProfileInt(szCountingOptions, szHonorsValuationMode, m_nHonorsValuationMode);
	if (m_nHonorsValuationMode == 2)
	{
		WriteProfileString(szCountingOptions, szCustomAceValue, FormString(_T("%.4f"),m_fCustomAceValue));
		WriteProfileString(szCountingOptions, szCustomKingValue, FormString(_T("%.4f"),m_fCustomKingValue));
		WriteProfileString(szCountingOptions, szCustomQueenValue, FormString(_T("%.4f"),m_fCustomQueenValue));
		WriteProfileString(szCountingOptions, szCustomJackValue, FormString(_T("%.4f"),m_fCustomJackValue));
	}
	WriteProfileString(szCountingOptions, szMajorSuitGamePts, FormString("%f", m_fDefaultMajorSuitGamePts));
	WriteProfileString(szCountingOptions, szMinorSuitGamePts, FormString("%f", m_fDefaultMinorSuitGamePts));
	WriteProfileString(szCountingOptions, szNTGamePts, FormString("%f", m_fDefaultNTGamePts));
	WriteProfileString(szCountingOptions, sz4LevelPts, FormString("%f", m_fDefault4LevelPts));
	WriteProfileString(szCountingOptions, sz3LevelPts, FormString("%f", m_fDefault3LevelPts));
	WriteProfileString(szCountingOptions, sz2LevelPts, FormString("%f", m_fDefault2LevelPts));
	WriteProfileString(szCountingOptions, szSlamPts, FormString("%f", m_fDefaultSlamPts));
	WriteProfileString(szCountingOptions, szGrandSlamPts, FormString("%f", m_fDefaultGrandSlamPts));
// NCR-249 reinstated AcelessPenalty
	WriteProfileInt(szCountingOptions, szAcelessPenalty, m_bAcelessPenalty);
/*
	WriteProfileInt(szCountingOptions, sz4AceBonus, m_b4AceBonus);
	WriteProfileInt(szCountingOptions, szPenalizeUGHonors, m_bPenalizeUGHonors);
	WriteProfileInt(szCountingOptions, szCountShortSuits, m_bCountShortSuits);
*/

	// write out GIB options
	WriteProfileString(szGIBOptions, szGIBPath, m_strGIBPath);
	WriteProfileInt(szGIBOptions, szEnableGIBForDeclarer, m_bEnableGIBForDeclarer);
//	WriteProfileInt(szGIBOptions, szEnableGIBForDefender, m_bEnableGIBForDefender);
	WriteProfileInt(szGIBOptions, szGIBAnalysisTime, m_nGIBAnalysisTime);
	WriteProfileInt(szGIBOptions, szGIBSampleSize, m_nGIBSampleSize);
	WriteProfileInt(szGIBOptions, szShowGIBOutput, m_bShowGIBOutput);


	// write out deal options
	WriteProfileInt(szDealOptions, szMinGamePts, m_nReqPointsGame[0][0]);
	WriteProfileInt(szDealOptions, szMaxGamePts, m_nReqPointsGame[0][1]);
	WriteProfileInt(szDealOptions, szMinMinorGamePts, m_nReqPointsGame[1][0]);
	WriteProfileInt(szDealOptions, szMaxMinorGamePts, m_nReqPointsGame[1][1]);
	WriteProfileInt(szDealOptions, szMinMajorGamePts, m_nReqPointsGame[2][0]);
	WriteProfileInt(szDealOptions, szMaxMajorGamePts, m_nReqPointsGame[2][1]);
	WriteProfileInt(szDealOptions, szMinNoTrumpGamePts, m_nReqPointsGame[3][0]);
	WriteProfileInt(szDealOptions, szMaxNoTrumpGamePts, m_nReqPointsGame[3][1]);
	WriteProfileInt(szDealOptions, szMinSlamPts, m_nReqPointsSlam[0][0]);
	WriteProfileInt(szDealOptions, szMaxSlamPts, m_nReqPointsSlam[0][1]);
	WriteProfileInt(szDealOptions, szMinSmallSlamPts, m_nReqPointsSlam[1][0]);
	WriteProfileInt(szDealOptions, szMaxSmallSlamPts, m_nReqPointsSlam[1][1]);
	WriteProfileInt(szDealOptions, szMinGrandSlamPts, m_nReqPointsSlam[2][0]);
	WriteProfileInt(szDealOptions, szMaxGrandSlamPts, m_nReqPointsSlam[2][1]);
	WriteProfileInt(szDealOptions, szBalanceTeamHands, m_bBalanceTeamHands);
	WriteProfileInt(szDealOptions, szMinCardsInMajor, m_nMinCardsInMajor);
	WriteProfileInt(szDealOptions, szMinCardsInMinor, m_nMinCardsInMinor);
	WriteProfileInt(szDealOptions, szMinSuitDistMinor, m_nMinSuitDist[0]);
	WriteProfileInt(szDealOptions, szMinSuitDistMajor, m_nMinSuitDist[1]);
	WriteProfileInt(szDealOptions, szMinTopMajorCard, m_nMinTopMajorCard);
	WriteProfileInt(szDealOptions, szMinTopMinorCard, m_nMinTopMinorCard);
//	WriteProfileInt(szDealOptions, szMaxImbalanceForNT, m_nMaxImbalanceForNT);
	WriteProfileInt(szDealOptions, szNeedTwoBalancedHandsForNT, m_bNeedTwoBalancedTrumpHands);
	WriteProfileInt(szDealOptions, szAcesNeededForSlam, m_numAcesForSlam[0]);
	WriteProfileInt(szDealOptions, szAcesNeededForSmallSlam, m_numAcesForSlam[1]);
	WriteProfileInt(szDealOptions, szAcesNeededForGrandSlam, m_numAcesForSlam[2]);
	WriteProfileInt(szDealOptions, szKingsNeededForSlam, m_numKingsForSlam[0]);
	WriteProfileInt(szDealOptions, szKingsNeededForSmallSlam, m_numKingsForSlam[1]);
	WriteProfileInt(szDealOptions, szKingsNeededForGrandSlam, m_numKingsForSlam[2]);
	WriteProfileInt(szDealOptions, szEnableDealNumbering, m_bEnableDealNumbering);

	// write out bidding config options
//	WriteProfileInt(szBiddingConfigOptions, szBiddingEngine, m_nBiddingEngine);
	WriteProfileString(szBiddingConfigOptions, szBiddingAggressiveness, FormString("%f", m_fBiddingAggressiveness));
/*
	WriteProfileInt(szBiddingConfigOptions, szNumNNetHiddenLayers, m_numNNetHiddenLayers);
	WriteProfileInt(szBiddingConfigOptions, szNumNNetNodesPerHiddenLayer, m_numNNetNodesPerHiddenLayer);
	WriteProfileString(szBiddingConfigOptions, szNeuralNetFile, m_strNeuralNetFile );
*/
	// save scoring options
	WriteProfileInt(szScoringOptions, szUseDuplicateScoring, m_bDuplicateScoring);
	WriteProfileInt(szScoringOptions, szScoreHonorsBonuses, m_bScoreHonorsBonuses);

	//
	//	WriteProfileInt(szBiddingConventions, szCurrConvSet, m_nCurrConvSet);
	// write out play options

	// write out debug options
	// get debug options
	WriteProfileInt(szDebugOptions, szEnableDebugMode, m_bDebugMode);
	WriteProfileInt(szDebugOptions, szExposeAllCards, m_bShowCardsFaceUp);
}


//
// GetProgramOption() and SetProgramOption() are 
// generic property get/set routines
//

//
LPVOID CEasyBApp::GetValuePV(int nItem, int nIndex1, int nIndex2, int nIndex3)
{
	switch(nItem)
	{
		case tbAutoBidStart:
			return (LPVOID) m_bAutoBidStart;
		case tbAutoJumpCursor:
			return (LPVOID) m_bAutoJumpCursor;
		case tbAutoPlayLastCard:
			return (LPVOID) m_bAutoPlayLastCard;
		case tbAutoShowBidHistory:
			return (LPVOID) m_bAutoShowBidHistory;
		case tbAutoShowPlayHistory:
			return (LPVOID) m_bAutoShowPlayHistory;
		case tbAutoHideBidHistory:
			return (LPVOID) m_bAutoHideBidHistory;
		case tbAutoHidePlayHistory:
			return (LPVOID) m_bAutoHidePlayHistory;
		case tbAutoShowNNetOutputWhenTraining:
			return (LPVOID) m_bAutoShowNNetOutputWhenTraining;
		case tbCollapseGameReviewDialog:
			return (LPVOID) m_bCollapseGameReviewDialog;
		case tbBiddingInProgress:
			return (LPVOID) m_bBiddingInProgress;
		case tbGameInProgress:
			return (LPVOID) m_bGameInProgress;
		case tbRubberInProgress:
			return (LPVOID) m_bRubberInProgress;
		case tbUsingDuplicateScoring:
			return (LPVOID) m_bDuplicateScoring;
			break;
		case tbDebugMode:
			return (LPVOID) m_bDebugMode;
		case tbManualBidding:
			return (LPVOID) m_bManualBidding;
		case tbAutoTestMode:
			return (LPVOID) m_bAutoTestMode;
		case tnAutoHintMode:
			return (LPVOID) m_nAutoHintMode;
		case tnAutoHintTraceLevel:
			return (LPVOID) m_nAutoHintTraceLevel;
		case tbEnableAnalysisTracing:
			return (LPVOID) m_bEnableAnalysisTracing;
		case tbEnableAnalysisDuringHints:
			return (LPVOID) m_bEnableAnalysisDuringHints;
		case tnAnalysisTraceLevel:
			return (LPVOID) m_nAnalysisTraceLevel;
		case tbShowCommentIdentifiers:
			return (LPVOID) m_bShowCommentIdentifiers;
		case tbInsertBiddingPause:
			return (LPVOID) m_bInsertBiddingPause;
		case tnBiddingPauseLength:
			return (LPVOID) m_nBiddingPauseLength;
		case tbInsertPlayPause:
			return (LPVOID) m_bInsertPlayPause;
		case tnPlayPauseLength:
			return (LPVOID) m_nPlayPauseLength;
		case tbComputerCanClaim:
			return (LPVOID) m_bComputerCanClaim;
		case tbShowPassedHands:
			return (LPVOID) m_bShowPassedHands;
		case tbAllowRebidPassedHands:
			return (LPVOID) m_bAllowRebidPassedHands;
		case tnPassedHandWaitInterval:
			return (LPVOID) m_nPassedHandWaitInterval;
		case tbManualPlayMode:
			return (LPVOID) m_bManualPlayMode;
		case tnCardPlayMode:
			if (m_bManualPlayMode)
				return (LPVOID) PLAY_MANUAL;
			else
				return (LPVOID) m_nPlayMode;
		case tbPlayModeLocked:
			return (LPVOID) m_bPlayModeLocked;
		case tbShowCardsFaceUp:
			return (LPVOID) m_bShowCardsFaceUp;
		case tbShowLayoutOnEdit:
			return (LPVOID) m_bShowLayoutOnEdit;
		case tbAutoAlignDialogs:
			return (LPVOID) m_bAutoAlignDialogs;
		case tbShowStartupAnimation:
			return (LPVOID) m_bShowStartupAnimation;
		case tbShowBackgroundBitmap:
			return (LPVOID) m_bShowBackgroundBitmap;
		case tnBackgroundColor:
			return (LPVOID) m_nBackgroundColor;
		case tbUseSuitSymbols:
			return (LPVOID) m_bUseSuitSymbols;
		case tnBitmapDisplayMode:
			return (LPVOID) m_nBitmapDisplayMode;
		case tbScaleLargeBitmaps:
			return (LPVOID) m_bScaleLargeBitmaps;
		case tbShowSplashWindow:
			return (LPVOID) m_bShowSplashWindow;
		case tbEnableSpokenBids:
			return (LPVOID) m_bEnableSpokenBids;
		case tbSaveIntermediatePositions:
			return (LPVOID) m_bSaveIntermediatePositions;
		case tbExposePBNGameCards:
			return (LPVOID) m_bExposePBNGameCards;
		case tbShowDailyTipDialog:
			return (LPVOID) m_bShowDailyTipDialog;
		case tbShowScreenSizeWarning:
			return (LPVOID) m_bShowScreenSizeWarning;
		//
		case tnSplashTime:
			return (LPVOID) m_dwSplashTime;
		case tnumConventionSets:
			return (LPVOID) m_numConventionSets;
		case tnCurrentConventionSet:
			return (LPVOID) m_nCurrConventionSet;
		case tbFirstTimeRunning:
			return (LPVOID) m_bFirstTimeRunning;
		case tnGameMode:
			return (LPVOID) m_nGameMode;
		case tszProgramPath:
			return (LPVOID) (LPCTSTR) m_strProgPath;
		case tszProgramDirectory:
			return (LPVOID) (LPCTSTR) m_strProgDirectory;
//		case tszAboutDialogWAV:
//			return (LPVOID) (LPCTSTR) tszAppAboutWAVFile;
		case tstrProgramCopyright:
			return (LPVOID) (LPCTSTR) m_strProgCopyright;
		case tnProgramMajorVersion:
			return (LPVOID) m_nProgMajorVersion;
		case tnProgramMinorVersion:
			return (LPVOID) m_nProgMinorVersion;
		case tnProgramIncrementVersion:
			return (LPVOID) m_nProgIncrementVersion;
		case tnProgramBuildNumber:
			return (LPVOID) m_nProgBuildNumber;
		case tstrProgramBuildDate:
			return (LPVOID) (LPCTSTR) m_strProgBuildDate;
		case tstrSpecialBuildCode:
			return (LPVOID) (LPCTSTR) m_strSpecialBuildCode;
		case tstrProgramVersionString:
			return (LPVOID) (LPCTSTR) GetProgramVersionString();
		case tnSuitSequenceList:
			return (LPVOID) m_nSuitSeqList[nIndex1][nIndex2];
		case tnSuitSequenceOption:
			return (LPVOID) m_nSuitSeqOption;
		case tbLowResOption:
			return (LPVOID) m_bLowResOption;
		case tbToggleResolutionMode:
			return (LPVOID) m_bToggleResolutionMode;
		case tbLayoutFollowsDisplayOrder:
			return (LPVOID) m_bLayoutFollowsDisplayOrder;
		case tbShowDummyTrumpsOnLeft:
			return (LPVOID) m_bShowDummyTrumpsOnLeft;
		case tnSuitSequence:
			return (LPVOID) m_nSuitSequence[nIndex1];
		case tnDummySuitSequence:
			return (LPVOID) m_nDummySuitSequence[nIndex1];
		case tpvActiveDocument:
			return (LPVOID) m_pActiveDocument;
		case tpvSplashWindow:
			return (LPVOID) NULL;	//
		case tstrProgramTitle:
			return (LPVOID) (LPCTSTR) m_strProgTitle;
		case tszPlayerPosition:
			return (LPVOID) szPosition[nIndex1];
		case tszSuitName:
			return (LPVOID) szSuitName[nIndex1];

		// GIB Info
		case tszGIBPath:
			return (LPVOID) (LPCTSTR) m_strGIBPath;
		case tbEnableGIBForDeclarer:
			return (LPVOID) m_bEnableGIBForDeclarer;
		case tbEnableGIBForDefender:
			return (LPVOID) m_bEnableGIBForDefender;
		case tnGIBAnalysisTime:
			return (LPVOID) m_nGIBAnalysisTime;
		case tnGIBSampleSize:
			return (LPVOID) m_nGIBSampleSize;

		// deal parameters
		case tnRequiredPointsForGame:	// [4][2]
			return (LPVOID) m_nReqPointsGame[nIndex1][nIndex2];
		case tnRequiredPointsForSlam:	// [3][2]
			return (LPVOID) m_nReqPointsSlam[nIndex1][nIndex2];
		case tnPointsAbsoluteGameLimits:
			return (LPVOID) m_nPointsAbsGameLimits[nIndex1][nIndex2];
		case tnPointsAbsoluteSlamLimits:
			return (LPVOID) m_nPointsAbsSlamLimits[nIndex1][nIndex2];
		case tbBalanceTeamHands:
			return (LPVOID) m_bBalanceTeamHands;
		case tbGiveSouthBestHandInPartnership:
			return (LPVOID) m_bGiveSouthBestHandInPartnership;
		case tbGiveSouthBestHandInGame:
			return (LPVOID) m_bGiveSouthBestHandInGame;
		case tnMinCardsInMajor:
			return (LPVOID) m_nMinCardsInMajor;
		case tnMinCardsInMinor:
			return (LPVOID) m_nMinCardsInMinor;
		case tnMinSuitDistributionTable:
			return (LPVOID) m_nMinSuitDistTable[nIndex1][nIndex2][nIndex3];
		case tnMinSuitDistributions:
			return (LPVOID) m_nMinSuitDist[nIndex1];
		case tnMinTopMajorCard:
			return (LPVOID) m_nMinTopMajorCard;
		case tnMinTopMinorCard:
			return (LPVOID) m_nMinTopMinorCard;
		case tnMaxImbalanceForNT:
			return (LPVOID) m_nMaxImbalanceForNT;
		case tbNeedTwoBalancedTrumpHands:
			return (LPVOID) m_bNeedTwoBalancedTrumpHands;
		case tnumAcesForSlam:
			return (LPVOID) m_numAcesForSlam[nIndex1];
		case tnumKingsForSlam:
			return (LPVOID) m_numKingsForSlam[nIndex1];
		case tbEnableDealNumbering:
			return (LPVOID) m_bEnableDealNumbering;

/*
		case tnBiddingEngine:
			return (LPVOID) m_nBiddingEngine;
		case tbNNetTrainingMode:
			return (LPVOID) m_bNNetTrainingMode;
		case tszNeuralNetFile:
			return (LPVOID) (LPCTSTR) m_strNeuralNetFile;
		case tnumNeuralNetInputs:
			return (LPVOID) tnumNNetInputs;
		case tnumNeuralNetOutputs:
			return (LPVOID) tnumNNetOutputs;
		case tnumNeuralNetHiddenLayers:
			return (LPVOID) m_numNNetHiddenLayers;
		case tnumNeuralNetUnitsPerHiddenLayer:
			return (LPVOID) m_numNNetNodesPerHiddenLayer;
*/
		// scoring options
		case tbScoreHonorsBonuses:
			return (LPVOID) m_bScoreHonorsBonuses;

		// counting options
		case tnHonorsValuationMode:
			return (LPVOID) m_nHonorsValuationMode;
		case tb4AceBonus:
			return (LPVOID) m_b4AceBonus;
		case tbAcelessPenalty:
			return (LPVOID) m_bAcelessPenalty;
		case tbCountShortSuits:
			return (LPVOID) m_bCountShortSuits;
		case tbPenalizeUGHonors:
			return (LPVOID) m_bPenalizeUGHonors;
		//
		default:
			AfxMessageBox("Unhandled Call to CEasyBApp::GetValuePV()");
			return (LPVOID) NULL;
	}
	return NULL;
}

//
double CEasyBApp::GetValueDouble(int nItem, int nIndex1, int nIndex2, int nIndex3)
{
	switch(nItem)
	{
		case tfAceValue:
			return m_fAceValue;
		case tfKingValue:
			return m_fKingValue;
		case tfQueenValue:
			return m_fQueenValue;
		case tfJackValue:
			return m_fJackValue;
		case tfTenValue:
			return m_fTenValue;
		//
		case tfDefaultMajorSuitGamePts:
			return m_fDefaultMajorSuitGamePts;
		case tfDefaultMinorSuitGamePts:
			return m_fDefaultMinorSuitGamePts;
		case tfDefaultNTGamePts:
			return m_fDefaultNTGamePts;
		case tfDefault4LevelPts:
			return m_fDefault4LevelPts;
		case tfDefault3LevelPts:
			return m_fDefault3LevelPts;
		case tfDefault2LevelPts:
			return m_fDefault2LevelPts;
		case tfDefaultSlamPts:
			return m_fDefaultSlamPts;
		case tfDefaultGrandSlamPts:
			return m_fDefaultGrandSlamPts;
		//
		case tfMajorSuitGamePts:
			return m_fMajorSuitGamePts;
		case tfMinorSuitGamePts:
			return m_fMinorSuitGamePts;
		case tfNTGamePts:
			return m_fNTGamePts;
		case tf4LevelPts:
			return m_f4LevelPts;
		case tf3LevelPts:
			return m_f3LevelPts;
		case tf2LevelPts:
			return m_f2LevelPts;
		case tfSlamPts:
			return m_fSlamPts;
		case tfGrandSlamPts:
			return m_fGrandSlamPts;
		// 
		case tfBiddingAggressiveness:
			return m_fBiddingAggressiveness;
		//
		case tfCustomAceValue:
			return m_fCustomAceValue;
		case tfCustomKingValue:
			return m_fCustomKingValue;
		case tfCustomQueenValue:
			return m_fCustomQueenValue;
		case tfCustomJackValue:
			return m_fCustomJackValue;
		case tfCustomTenValue:
			return m_fCustomTenValue;
		default:
			AfxMessageBox("Unhandled Call to CEasyBApp::GetValueDouble()");
			return 0;
	}
	return 0;
}

//
int CEasyBApp::SetValuePV(int nItem, LPVOID value, int nIndex1, int nIndex2, int nIndex3)
{
	int nVal = (int) value;
	BOOL bVal = (BOOL) value;
	LPCTSTR sVal = (LPCTSTR) value;
	//
	//
	switch(nItem)
	{
		case tbAutoBidStart:
			m_bAutoBidStart = bVal;
			break;
		case tbAutoJumpCursor:
			m_bAutoJumpCursor = bVal;
			break;
		case tbAutoPlayLastCard:
			m_bAutoPlayLastCard = bVal;
			break;
		case tbAutoShowBidHistory:
			m_bAutoShowBidHistory = bVal;
			break;
		case tbAutoShowPlayHistory:
			m_bAutoShowPlayHistory = bVal;
			break;
		case tbAutoHideBidHistory:
			m_bAutoHideBidHistory = bVal;
			break;
		case tbAutoHidePlayHistory:
			m_bAutoHidePlayHistory = bVal;
			break;
		case tbAutoShowNNetOutputWhenTraining:
			m_bAutoShowNNetOutputWhenTraining = bVal;
			break;
		case tbCollapseGameReviewDialog:
			m_bCollapseGameReviewDialog = bVal;
			break;
		case tbBiddingInProgress:
			m_bBiddingInProgress = bVal;
			break;
		case tbGameInProgress:
			m_bGameInProgress = bVal;
			break;
		case tbRubberInProgress:
			m_bRubberInProgress = bVal;
			break;
		case tbUsingDuplicateScoring:
			m_bDuplicateScoring = bVal;
			break;
		case tbDebugMode:
			m_bDebugMode = bVal;
			break;
		case tbManualBidding:
			m_bManualBidding = bVal;
			break;
		case tbAutoTestMode:
			m_bAutoTestMode = bVal;
			break;
		case tnAutoHintMode:
			m_nAutoHintMode = nVal;
			break;
		case tnAutoHintTraceLevel:
			m_nAutoHintTraceLevel = nVal;
			break;
		case tbEnableAnalysisTracing:
			m_bEnableAnalysisTracing = bVal;
			break;
		case tbEnableAnalysisDuringHints:
			m_bEnableAnalysisDuringHints = bVal;
			break;
		case tnAnalysisTraceLevel:
			m_nAnalysisTraceLevel = nVal;
			break;
		case tbShowCommentIdentifiers:
			m_bShowCommentIdentifiers = bVal;
			break;
		case tbInsertBiddingPause:
			m_bInsertBiddingPause = bVal;
			break;
		case tnBiddingPauseLength:
			m_nBiddingPauseLength = nVal;
			break;
		case tbInsertPlayPause:
			m_bInsertPlayPause = bVal;
			break;
		case tnPlayPauseLength:
			m_nPlayPauseLength = nVal;
			break;
		case tbComputerCanClaim:
			m_bComputerCanClaim = bVal;
			break;
		case tbShowPassedHands:
			m_bShowPassedHands = bVal;
			break;
		case tbAllowRebidPassedHands:
			m_bAllowRebidPassedHands = bVal;
			break;
		case tnPassedHandWaitInterval:
			m_nPassedHandWaitInterval = nVal;
			break;
		case tbManualPlayMode:
			m_bManualPlayMode = bVal;
			break;
		case tnCardPlayMode:
			if (!m_bPlayModeLocked || (nIndex1 > 0))
				m_nPlayMode = nVal;
			break;
		case tbPlayModeLocked:
			m_bPlayModeLocked = bVal;
			break;
		case tbShowCardsFaceUp:
			m_bShowCardsFaceUp = bVal;
			break;
		case tbShowLayoutOnEdit:
			m_bShowLayoutOnEdit = bVal;
			break;
		case tbAutoAlignDialogs:
			m_bAutoAlignDialogs = bVal;
			break;
		case tbShowStartupAnimation:
			m_bShowStartupAnimation = bVal;
			break;
		case tbShowBackgroundBitmap:
			m_bShowBackgroundBitmap = bVal;
			break;
		case tnBitmapDisplayMode:
			m_nBitmapDisplayMode = nVal;
			break;
		case tbScaleLargeBitmaps:
			m_bScaleLargeBitmaps = bVal;
			break;
		case tnBackgroundColor:
			m_nBackgroundColor = nVal;
			break;
		case tbUseSuitSymbols:
			m_bUseSuitSymbols = bVal;
			break;
		case tbShowSplashWindow:
			m_bShowSplashWindow = bVal;
			break;
		case tbEnableSpokenBids:
			m_bEnableSpokenBids = bVal;
			break;
		case tbSaveIntermediatePositions:
			m_bSaveIntermediatePositions = bVal;
			break;
		case tbExposePBNGameCards:
			m_bExposePBNGameCards = bVal;
			break;
		case tbShowDailyTipDialog:
			m_bShowDailyTipDialog = bVal;
			break;
		case tbShowScreenSizeWarning:
			m_bShowScreenSizeWarning = bVal;
			break;
		//
		case tnSplashTime:
			m_dwSplashTime = nVal;
			break;
		case tnumConventionSets:
			m_numConventionSets = nVal;
			break;
		case tnCurrentConventionSet:
			m_nCurrConventionSet = nVal;
			break;
		case tnGameMode:
			m_nGameMode = nVal;
			break;
		case tbFirstTimeRunning:
			m_bFirstTimeRunning = bVal;
			break;
		case tszProgramPath:
//			m_strProgPath = sVal;
			break;
		case tszProgramDirectory:
//			m_strProgDirectory = sVal;
			break;
		case tszAboutDialogWAV:
			break;
		case tstrProgramCopyright:
			m_strProgCopyright = sVal;
			break;
		case tnProgramMajorVersion:
//			m_nProgMajorVersion = nVal;
			break;
		case tnProgramMinorVersion:
//			m_nProgMinorVersion = nVal;
			break;
		case tnProgramIncrementVersion:
//			m_nProgIncrementVersion = nVal;
			break;
		case tnProgramBuildNumber:
//			m_nProgBuildNumber = nVal;
			break;
		case tstrProgramBuildDate:
			m_strProgBuildDate = sVal;
			break;
		case tstrSpecialBuildCode:
			m_strSpecialBuildCode = sVal;
			break;
		case tstrProgramVersionString:
			break;
		case tnSuitSequence:
			m_nSuitSequence[nIndex1] = nVal;
			break;
		case tnDummySuitSequence:
			m_nDummySuitSequence[nIndex1] = nVal;
			break;
		case tnSuitSequenceList:
			m_nSuitSeqList[nIndex1][nIndex2] = nVal;
			break;
		case tnSuitSequenceOption:
			m_nSuitSeqOption = nVal;
			SetSuitSequence(m_nSuitSeqOption);
			break;
		case tbLowResOption:
			m_bLowResOption = bVal;
			break;
		case tbToggleResolutionMode:
			m_bToggleResolutionMode = bVal;
			break;
		case tbLayoutFollowsDisplayOrder:
			m_bLayoutFollowsDisplayOrder = bVal;
			break;
		case tbShowDummyTrumpsOnLeft:
			m_bShowDummyTrumpsOnLeft = bVal;
			break;
		case tpvActiveDocument:
			m_pActiveDocument = (CDocument*) value;
			break;
		case tpvSplashWindow:
			break;
		case tstrProgramTitle:
			m_strProgTitle = sVal;
			break;
		case tszPlayerPosition:
			break;
		case tszSuitName:
			break;

		// GIB Info
		case tszGIBPath:
			m_strGIBPath = sVal;
			break;
		case tbEnableGIBForDeclarer:
			m_bEnableGIBForDeclarer = bVal;
			break;
		case tbEnableGIBForDefender:
			m_bEnableGIBForDefender = bVal;
			break;
		case tnGIBAnalysisTime:
			m_nGIBAnalysisTime = nVal;
			break;
		case tnGIBSampleSize:
			m_nGIBSampleSize = nVal;
			break;

		// deal parameters
		case tnRequiredPointsForGame:	// [4][2]
			m_nReqPointsGame[nIndex1][nIndex2] = nVal;
			break;
		case tnRequiredPointsForSlam:	// [3][2]
			m_nReqPointsSlam[nIndex1][nIndex2] = nVal;
			break;
		case tnPointsAbsoluteGameLimits:
			m_nPointsAbsGameLimits[nIndex1][nIndex2] = nVal;
			break;
		case tnPointsAbsoluteSlamLimits:
			m_nPointsAbsSlamLimits[nIndex1][nIndex2] = nVal;
			break;
		case tbBalanceTeamHands:
			m_bBalanceTeamHands = bVal;
			break;
		case tbGiveSouthBestHandInPartnership:
			m_bGiveSouthBestHandInPartnership = bVal;
			break;
		case tbGiveSouthBestHandInGame:
			m_bGiveSouthBestHandInGame = bVal;
			break;
		case tnMinCardsInMajor:
			m_nMinCardsInMajor = nVal;
			break;
		case tnMinCardsInMinor:
			m_nMinCardsInMinor = nVal;
			break;
		case tnMinSuitDistributionTable:
			m_nMinSuitDistTable[nIndex1][nIndex2][nIndex3] = nVal;
			break;
		case tnMinSuitDistributions:
			m_nMinSuitDist[nIndex1] = nVal;
			break;
		case tnMinTopMajorCard:
			m_nMinTopMajorCard = nVal;
			break;
		case tnMinTopMinorCard:
			m_nMinTopMinorCard = nVal;
			break;
		case tnMaxImbalanceForNT:
			m_nMaxImbalanceForNT = nVal;
			break;
		case tbNeedTwoBalancedTrumpHands:
			m_bNeedTwoBalancedTrumpHands = bVal;
			break;
		case tnumAcesForSlam:
			m_numAcesForSlam[nIndex1] = nVal;
			break;
		case tnumKingsForSlam:
			m_numKingsForSlam[nIndex1] = nVal;
			break;
		case tbEnableDealNumbering:
			m_bEnableDealNumbering = bVal;
			break;

/*
		case tnBiddingEngine:
			m_nBiddingEngine = nVal;
			break;
		case tbNNetTrainingMode:
			m_bNNetTrainingMode = bVal;
			break;
		case tszNeuralNetFile:
			CloseNeuralNet();		// save out old file
			m_strNeuralNetFile = sVal;
			InitNeuralNet();		// load/create new file
			break;
		case tnumNeuralNetInputs:
		case tnumNeuralNetOutputs:
		case tnumNeuralNetHiddenLayers:
			m_numNNetHiddenLayers = nVal;
			break;
		case tnumNeuralNetUnitsPerHiddenLayer:
			m_numNNetNodesPerHiddenLayer = nVal;
			break;
*/

		// scoring options
		case tbScoreHonorsBonuses:
			m_bScoreHonorsBonuses = bVal;
			break;

		// counting options
		case tnHonorsValuationMode:
			m_nHonorsValuationMode = nVal;
			break;
		case tb4AceBonus:
			m_b4AceBonus = bVal;
			break;
		case tbAcelessPenalty:
			m_bAcelessPenalty = bVal;
			break;
		case tbCountShortSuits:
			m_bCountShortSuits = bVal;
			break;
		case tbPenalizeUGHonors:
			m_bPenalizeUGHonors = bVal;
			break;

		//
		default:
			AfxMessageBox("Unhandled Call to CEasyBApp::SetValuePV()");
			return 1;
	}
	return 0;
}

//
int CEasyBApp::SetValue(int nItem, double fValue, int nIndex1, int nIndex2, int nIndex3)
{
/*
	switch(nItem)
	{
		default:
			AfxMessageBox("Unhandled Call to CEasyBApp::SetValueDouble()");
			return 1;
	}
*/
	return 0;
}

//
int CEasyBApp::SetValueDouble(int nItem, double fValue, int nIndex1, int nIndex2, int nIndex3)
{
	switch(nItem)
	{
		case tfAceValue:
			m_fAceValue = fValue;
			break;
		case tfKingValue:
			m_fKingValue = fValue;
			break;
		case tfQueenValue:
			m_fQueenValue = fValue;
			break;
		case tfJackValue:
			m_fJackValue = fValue;
			break;
		case tfTenValue:
			m_fTenValue = fValue;
			break;
		//
		case tfDefaultMajorSuitGamePts:
			m_fDefaultMajorSuitGamePts = fValue;
			m_fDefault4LevelPts = fValue;
			m_fDefault3LevelPts = m_fDefault4LevelPts - 3;
			m_fDefault2LevelPts = m_fDefault3LevelPts - 3;
			break;
		case tfDefaultMinorSuitGamePts:
			m_fDefaultMinorSuitGamePts = fValue;
			break;
		case tfDefaultNTGamePts:
			m_fDefaultNTGamePts = fValue;
			break;
		case tfDefaultSlamPts:
			m_fDefaultSlamPts = fValue;
			break;
		case tfDefaultGrandSlamPts:
			m_fDefaultGrandSlamPts = fValue;
			break;
		//
		case tfMajorSuitGamePts:
			m_fMajorSuitGamePts = fValue;
			m_f4LevelPts = fValue;
			m_f3LevelPts = m_f4LevelPts - 3;
			m_f2LevelPts = m_f3LevelPts - 3;
			break;
		case tfMinorSuitGamePts:
			m_fMinorSuitGamePts = fValue;
			break;
		case tfNTGamePts:
			m_fNTGamePts = fValue;
			break;
		case tfSlamPts:
			m_fSlamPts = fValue;
			break;
		case tfGrandSlamPts:
			m_fGrandSlamPts = fValue;
			break;
		// 
		case tfBiddingAggressiveness:
			m_fBiddingAggressiveness = fValue;
			InitPointCountRequirements();
			break;
		//
		case tfCustomAceValue:
			m_fCustomAceValue = fValue;
			break;
		case tfCustomKingValue:
			m_fCustomKingValue = fValue;
			break;
		case tfCustomQueenValue:
			m_fCustomQueenValue = fValue;
			break;
		case tfCustomJackValue:
			m_fCustomJackValue = fValue;
			break;
		case tfCustomTenValue:
			m_fCustomTenValue = fValue;
			break;
		default:
			AfxMessageBox("Unhandled Call to CEasyBApp::SetValueDouble()");
			return 0;
	}
	return 0;
}

// conversion functions
LPCTSTR CEasyBApp::GetValueString(int nItem, int nIndex1, int nIndex2, int nIndex3)
{
	return (LPCTSTR) GetValuePV(nItem, nIndex1, nIndex2, nIndex3);
}

int CEasyBApp::GetValue(int nItem, int nIndex1, int nIndex2, int nIndex3)
{
	return (int) GetValuePV(nItem, nIndex1, nIndex2, nIndex3);
}

int CEasyBApp::SetValue(int nItem, LPCTSTR szValue, int nIndex1, int nIndex2, int nIndex3)
{
	return SetValuePV(nItem, (LPVOID)szValue, nIndex1, nIndex2, nIndex3);
}

int CEasyBApp::SetValue(int nItem, int nValue, int nIndex1, int nIndex2, int nIndex3)
{
	return SetValuePV(nItem, (LPVOID)nValue, nIndex1, nIndex2, nIndex3);
}

//
CString CEasyBApp::GetProgramVersionString()
{
	return FormString("Version %d.%d.%d%s",
					  theApp.GetValue(tnProgramMajorVersion),
					  theApp.GetValue(tnProgramMinorVersion),
					  theApp.GetValue(tnProgramIncrementVersion),
					  theApp.GetValueString(tstrSpecialBuildCode));
}

//
CString CEasyBApp::GetFullProgramVersionString()
{
	CString strVersion, strSpecialBuildCode = theApp.GetValueString(tstrSpecialBuildCode);
	strVersion.Format("Version %d.%d.%d%s",
					  theApp.GetValue(tnProgramMajorVersion),
					  theApp.GetValue(tnProgramMinorVersion),
					  theApp.GetValue(tnProgramIncrementVersion),
					  strSpecialBuildCode);
	//
	int nBuild = theApp.GetValue(tnProgramBuildNumber);
	if (nBuild > 0)
		strVersion += FormString(_T(", Build #%d"), nBuild);
	//
	return strVersion;
}


//
// RefreshProperties()
//
// - refreshes info after properties have been changed
//
void CEasyBApp::RefreshProperties()
{
	//
	InitHonorsValuations();
}



/////////////////////////////////////////////////////////////////////////////
//
// Utilities
//
/////////////////////////////////////////////////////////////////////////////

//
BOOL CEasyBApp::AreCardsFaceUp() const 
{ 
	// see if the face-up flag is set
	if (m_bShowCardsFaceUp)
		return TRUE;

	// else see if we're doing something that causes cards to be face up
	return pVIEW->AreCardsFaceUp(); 
}

//
void CEasyBApp::SetCardsFaceUp(BOOL bFaceUp) 
{ 
	m_bShowCardsFaceUp = bFaceUp; 
	pVIEW->Notify(WM_COMMAND, WMS_RESET_DISPLAY, TRUE);
}




/////////////////////////////////////////////////////////////////////////////
//
// CEasyBApp initialization
//
/////////////////////////////////////////////////////////////////////////////

BOOL CEasyBApp::InitInstance()
{
	// obtain program version info
	LPTSTR szProgPath = m_strProgPath.GetBuffer(1024);
	GetModuleFileName(m_hInstance, szProgPath, 1023);
	DWORD dummy;
	DWORD nFileVersionInfoSize = ::GetFileVersionInfoSize(szProgPath, &dummy);
	LPVOID pVersionBuffer = malloc(nFileVersionInfoSize);
	VERIFY(pVersionBuffer);
	GetFileVersionInfo(szProgPath, (DWORD)0, nFileVersionInfoSize, pVersionBuffer);
	m_strProgPath.ReleaseBuffer();
	UINT nInfoLength;

	// get version # string
	LPVOID pVersionData;
	VerQueryValue(pVersionBuffer, TEXT("\\StringFileInfo\\040904b0\\ProductVersion"), &pVersionData, &nInfoLength);
	sscanf((LPCTSTR)pVersionData,"%d.%d.%d",&m_nProgMajorVersion,&m_nProgMinorVersion,&m_nProgIncrementVersion);

	// get copyright string
	LPVOID pCopyrightData;
	VerQueryValue(pVersionBuffer, TEXT("\\StringFileInfo\\040904b0\\LegalCopyright"), &pCopyrightData, &nInfoLength);
	m_strProgCopyright = (LPCTSTR) pCopyrightData;

	// get build #
	LPVOID pBuildNumData;
	VerQueryValue(pVersionBuffer, TEXT("\\StringFileInfo\\040904b0\\PrivateBuild"), &pBuildNumData, &nInfoLength);
	sscanf((LPCTSTR)pBuildNumData, "Build #%d", &m_nProgBuildNumber);

	// get build date	
	LPVOID pBuildDateData;
	VerQueryValue(pVersionBuffer, TEXT("\\StringFileInfo\\040904b0\\Comments"), &pBuildDateData, &nInfoLength);
	m_strProgBuildDate = (LPCTSTR) pBuildDateData;

	// get special code
	LPVOID pSpecialBuildData;
	if (VerQueryValue(pVersionBuffer, TEXT("\\StringFileInfo\\040904b0\\SpecialBuild"), &pSpecialBuildData, &nInfoLength))
		m_strSpecialBuildCode = (LPCTSTR) pSpecialBuildData;
	else
		m_strSpecialBuildCode = _T("");

	// free the version memory
	free(pVersionBuffer);
	
	// extract the program startup directory
	int nIndex = m_strProgPath.ReverseFind('\\');
	m_strProgDirectory = m_strProgPath.Left(nIndex);

	// NCR added following 2 lines
	NormsCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// set registry info
#ifdef RDEBUG
		SetRegistryKey("Steve's Software (RDebug)");	// ReleaseDebug
#elif defined _DEBUG
		SetRegistryKey("Steve's Software (Debug)");		// Debug
//		freopen("EasyBridge.out", "a+", stdout); // NCR append debug output to a file
//		printf("\n>>>>>>> Starting EasyBridge <<<<<<\n");  // NCR separator line
#else
		// NCR changed following lines to append suffix to registry entry
		CString regKey = "Steve's Software";
		if(cmdInfo.m_bHaveRegKey) {
			regKey += cmdInfo.m_RegKey; // add on param value
        }
		SetRegistryKey(regKey);				// Release
//		AfxMessageBox("Regkey is " + regKey); // DEBUG >>>>>>>>>><<<
#endif
	//

	// load main program settings from the registry
	Initialize();

	// show splash window
	if ((m_lpCmdLine[0] == 0) && (m_bShowSplashWindow) && !m_bShowStartupAnimation) 
//			&& (!m_bFirstTimeRunning)) 
	{
		m_pSplash = new CSplashWnd;
		m_pSplash->Create(NULL);
		m_pSplash->ShowWindow(SW_SHOW);
		m_pSplash->UpdateWindow();
		m_pSplash->m_timer = m_pSplash->SetTimer(1, 2000, NULL);
		m_dwSplashTime = ::GetCurrentTime();
	}

	// load other registry settings
//	Initialize();
	LoadStdProfileSettings(5);  // Load standard INI file options (including MRU)

	// and do other inits
	InitSettings();

	//
	// primary inits done, ready to proceed with creating the doc/view
	//

	// check for the registry key indicating first time running
	static TCHAR BASED_CODE szFirstTime[] = _T("First Time Running");
	if (GetProfileInt(szGameOptions, szFirstTime, 0) > 0)
	{
		m_bFirstTimeRunning = TRUE;
		WriteProfileInt(szGameOptions, szFirstTime, 0);
	}
	else
	{
		m_bFirstTimeRunning = FALSE;
	}

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CEasyBDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CEasyBView));
	AddDocTemplate(pDocTemplate);
/*
	// add another template for PBN files
	CSingleDocTemplate* pDocTemplatePBN;
	pDocTemplatePBN = new CSingleDocTemplate(
		IDR_PBN_TYPE,
		RUNTIME_CLASS(CEasyBDoc),
		RUNTIME_CLASS(CMainFrame), 
		RUNTIME_CLASS(CEasyBView));
	AddDocTemplate(pDocTemplatePBN);
*/

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes();

	// create a new (empty) document
	OnFileNew();

//	UINT nToolbars[] = { IDR_MAINFRAME_TOOLBAR, IDR_SECONDARY_TOOLBAR, IDR_TEST_TOOLBAR, IDR_TOOLBAR_HIDDEN_BUTTONS };
//	((CMainFrame*)m_pMainWnd)->InitializeMenu(IDR_MAINFRAME, nToolbars, sizeof(nToolbars) / sizeof(UINT));

	// now that the main window has been created, we can create the cards
	deck.Initialize();
	// and shuffle the deck
	deck.Shuffle();

	// initialize the neural net engine
//	m_pNeuralNet = NULL;
//	InitNeuralNet();

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// initialize main frame and document
//	pMAINFRAME->Initialize();

	// now we can safely open an existing document, if so specified
//	if (m_lpCmdLine[0] != '\0')	
//		OpenDocumentFile(m_lpCmdLine);
	if(!cmdInfo.m_strFileName.IsEmpty()) // NCR replaced above with this
		OpenDocumentFile(cmdInfo.m_strFileName);

	// all done
	return TRUE;
}






//
int CEasyBApp::ExitInstance() 
{
	// Destroy the cards
	deck.Terminate();

	// and delete the conventions
	int i;
	for(i=0;i<m_numConventionSets;i++)
	{
		pConventionSet[i]->Terminate();	// save settings
		delete pConventionSet[i];		// then delete
	}
	
	// delete the GIB wrapper
	delete m_pGIBWrapper;

	// cose out the neural net
//	CloseNeuralNet();

	// perform play class static uninitialization
	CPlay::ClassTerminate();
	
	// and save application settings
	Terminate();
	
	//
	return CWinApp::ExitInstance();
}




//
// InitSetttings()
//
// - performs primary program initialization tasks
//
void CEasyBApp::InitSettings()
{
	// init flags
	m_nTestMode = 0;
	m_bBiddingInProgress = FALSE;
	m_bGameInProgress = FALSE;
	m_bRubberInProgress = FALSE;
	m_bAutoTestMode = FALSE;

	//
	m_bToggleResolutionMode = FALSE;

	//
	InitPointCountRequirements();
	InitHonorsValuations();

	// #### TEMP ####
	m_nGameMode = 0;
	m_bManualPlayMode = FALSE;

	//
	// set default point limits
	//
	int i,j,k;
	for(i=0;i<2;i++)
		for(j=0;j<4;j++)
			for(k=0;k<2;k++)
				m_nMinSuitDistTable[i][j][k] = nDefMinSuitDistTable[i][j][k];
	for(i=0;i<4;i++)
		for(j=0;j<2;j++)
			m_nPointsAbsGameLimits[i][j] = nDefPointsAbsGameLimits[i][j];
	for(i=0;i<3;i++)
		for(j=0;j<2;j++)
			m_nPointsAbsSlamLimits[i][j] = nDefPointsSlamLimits[i][j];

	// set suit sequence
	for(i=0;i<tnumSuitSequences;i++)
		for(j=0;j<4;j++)
			m_nSuitSeqList[i][j] = defSuitDisplaySequence[i][j];
	//
	SetSuitSequence(m_nSuitSeqOption);

	// load program title string
	m_strProgTitle.LoadString(IDS_APPTITLE);

	// create the GIB Wrapper
	m_pGIBWrapper = new CGIB();

	// init the GIB object
//	m_gibWrapper.Initialize();

	// initialize conventions
	m_nCurrConventionSet = 0;
	m_numConventionSets = 1;
	pConventionSet[0] = new CConventionSet;
	pConventionSet[0]->Initialize("Default");	// read in settings
	pConventionSet[0]->InitConventions();		// and prepare conventions
	pCurrConvSet = pConventionSet[0];

	// perform play class static initialization
	CPlay::ClassInitialize();

	// done
}



//
// InitPointCountRequirements()
//
void CEasyBApp::InitPointCountRequirements()
{
	// adjust point count requirements
	m_fMajorSuitGamePts = m_fDefaultMajorSuitGamePts - m_fBiddingAggressiveness;
	m_fMinorSuitGamePts = m_fDefaultMinorSuitGamePts - m_fBiddingAggressiveness;
	m_fNTGamePts = m_fDefaultNTGamePts - m_fBiddingAggressiveness;
	m_f4LevelPts = m_fDefault4LevelPts - m_fBiddingAggressiveness;
	m_f3LevelPts = m_fDefault3LevelPts - m_fBiddingAggressiveness;
	m_f2LevelPts = m_fDefault2LevelPts - m_fBiddingAggressiveness;
	m_fSlamPts = m_fDefaultSlamPts - m_fBiddingAggressiveness;
	m_fGrandSlamPts = m_fDefaultGrandSlamPts - m_fBiddingAggressiveness;
}




//
// InitHonorsValuations()
//
void CEasyBApp::InitHonorsValuations()
{
	// set honors values
	switch(m_nHonorsValuationMode)
	{
		case 1:		// Ginsburg
			m_fAceValue = tfGinsburgAceValue;
			m_fKingValue = tfGinsburgKingValue;
			m_fQueenValue = tfGinsburgQueenValue;
			m_fJackValue = tfGinsburgJackValue;
			m_fTenValue = 0;
			break;
		case 2:		// custom
			m_fAceValue = m_fCustomAceValue;
			m_fKingValue = m_fCustomKingValue;
			m_fQueenValue = m_fCustomQueenValue;
			m_fJackValue = m_fCustomJackValue;
			m_fTenValue = 0;
			break;
		default:	
			m_fAceValue = tnDefaultAceValue;
			m_fKingValue = tnDefaultKingValue;
			m_fQueenValue = tnDefaultQueenValue;
			m_fJackValue = tnDefaultJackValue;
			m_fTenValue = 0;
			break;
	}

	// fill the static array
	m_fHonorValue[ACE] = m_fAceValue;
	m_fHonorValue[KING] = m_fKingValue;
	m_fHonorValue[QUEEN] = m_fQueenValue;
	m_fHonorValue[JACK] = m_fJackValue;
	m_fHonorValue[TEN] = m_fTenValue;
}



/*
//
// InitNeuralNet()
//
// - initialize the neural net engine
//
void CEasyBApp::InitNeuralNet()
{
	// close any existing
	CloseNeuralNet();

	// see if we have a valid neural net file path
	BOOL bExisting = !m_strNeuralNetFile.IsEmpty();
	if (bExisting)
	{
		// load the existing neural net file
		CFile file;
		CFileException error;
		if (file.Open(m_strNeuralNetFile, CFile::modeRead, &error))
		{
			m_pNeuralNet = new CNeuralNet;
			CArchive ar(&file, CArchive::load);
			m_pNeuralNet->Serialize(ar);
			file.Close();
		}
		else
		{
			// no existing net fopund
			bExisting = FALSE;
		}
	}

	//
	if (!bExisting)
	{
		// the neural net file was not specified or does not exist
		// so create a new neural net
		m_pNeuralNet = new CNeuralNet(tnumNNetInputs, tnumNNetOutputs, m_numNNetHiddenLayers, m_numNNetNodesPerHiddenLayer);
		m_pNeuralNet->RandomizeWeights();
	}
}



//
// CloseNeuralNet()
//
// - close out the neural net file
//
void CEasyBApp::CloseNeuralNet()
{
	if (m_pNeuralNet == NULL)
		return;

	// check filename
	if (m_strNeuralNetFile.IsEmpty())
	{
		// prompt to see if the user wants to save
		if (AfxMessageBox("Do you wish to save the neural net file?", MB_ICONQUESTION | MB_YESNO) == IDNO)
			return;

		// get the filename
		CFileDialog fileDlg(TRUE, "net", "", 0,
			  				"Neural Net Files (*.net)|*.net|All Files (*.*)|*.*||",
		   					theApp.m_pMainWnd);
		fileDlg.m_ofn.lpstrTitle = "Enter Neural Net Output file";
		fileDlg.m_ofn.lpstrInitialDir = m_strProgDirectory;

		// then show
		if (fileDlg.DoModal() == IDCANCEL)
			return;

		// and get filenmae
		m_strNeuralNetFile = fileDlg.GetPathName();
		if (m_strNeuralNetFile.IsEmpty())
			return;
	}

	// save the neural net file
	CFile file;
	CFileException error;
	if (file.Open(m_strNeuralNetFile, CFile::modeWrite | CFile::modeCreate, &error))
	{
		CArchive ar(&file, CArchive::store);
		m_pNeuralNet->Serialize(ar);
	}
	file.Close();
	
	// and delete 
	delete m_pNeuralNet;
	m_pNeuralNet = NULL;
}
*/





/////////////////////////////////////////////////////////////////////////////
//
//
//
// CAboutDlg dialog used for App About
//
//
//
/////////////////////////////////////////////////////////////////////////////

#include "CJHyperLink.h"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CCJHyperLink	m_hyperLinkEMail;
	CCJHyperLink	m_hyperLinkEMailNorms;
	CCJHyperLink	m_hyperLinkURL;
	CString	m_strSecondaryCopyright;
	CString	m_strEmailAddress;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	virtual void OnOK();
	afx_msg void OnImageClicked();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//
private:
	bool	m_bPortraitShown;
	CBitmap m_bitmapPortrait;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_strSecondaryCopyright = _T("");
	m_strEmailAddress = _T("");
	//}}AFX_DATA_INIT
	m_bPortraitShown = false;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_SECONDARY_COPYRIGHT, m_strSecondaryCopyright);
	DDX_Control(pDX, IDC_EMAIL_ADDRESS, m_hyperLinkEMail);
	DDX_Control(pDX, IDC_EMAIL_NORMS, m_hyperLinkEMailNorms);
	DDX_Control(pDX, IDC_HYPERLINK, m_hyperLinkURL);
	//}}AFX_DATA_MAP
}


BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString strTemp;

	// init hyperlinks
	m_hyperLinkEMail.SetURL(_T("mailto:shan@nyx.net"));
	m_hyperLinkEMailNorms.SetURL(_T("mailto:radder@hotmail.com")); // NCR add our email address
	m_hyperLinkURL.SetURL(_T("http://www.nyx.net/~shan/EasyBridge.html"));
	// show copyright info
	SetDlgItemText(IDC_STATIC_COPYRIGHT, theApp.GetValueString(tstrProgramCopyright));	
	// show version number		  
	strTemp = theApp.GetFullProgramVersionString();
	SetDlgItemText(IDC_STATIC_VERSION, (LPCTSTR)strTemp);	
	// show misc info
	strTemp.Format("Build Date: %s", theApp.GetValueString(tstrProgramBuildDate));
	SetDlgItemText(IDC_STATIC_DATE, (LPCTSTR)strTemp);	

	// show e-mail address
	m_strEmailAddress.LoadString(IDS_AUTHOR_EMAIL);

	// show secondary copyright
//	m_strSecondaryCopyright.Empty();
	m_strSecondaryCopyright.LoadString(IDS_SECONDARY_COPYRIGHT);

	// and update
	UpdateData(FALSE);

	//
	CenterWindow();

	// done
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// play sound if showing window
	if (bShow)
	{
/*
		// see if the WAV file exists
		CString strPath = FormString(_T("%s\\%s"), theApp.GetValue(tszProgramDirectory), theApp.GetValue(tszAboutDialogWAV));
		CFile file;
		if (file.Open(strPath, CFile::modeRead))
		{
			file.Close();
			PlaySound(strPath, NULL, SND_ASYNC | SND_NOSTOP | SND_NOWAIT);
		}
*/
	}
}

void CAboutDlg::OnImageClicked() 
{
	// show bitmap
	if (!m_bPortraitShown)
	{
		// need Ctrl key pressed
		if (GetKeyState(VK_CONTROL) & 0x8000) 
		{
			CStatic* pStaticImage = (CStatic*) GetDlgItem(IDC_IMAGE);
			m_bitmapPortrait.LoadBitmap(IDB_PORTRAIT);
			pStaticImage->SetBitmap(m_bitmapPortrait);
			m_bPortraitShown = true;

			// and also change the text around
			CStatic* pText1 = (CStatic*) GetDlgItem(IDC_COPYRIGHT1);
			CStatic* pText2 = (CStatic*) GetDlgItem(IDC_COPYRIGHT2);
			pText1->SetWindowText(_T("Hi, I'm Steven Han.  I hope you enjoy this program as much as I enjoyed writing it."));
			pText2->SetWindowText(_T(""));
		}
	}
}

void CAboutDlg::OnClose() 
{
	// stop the sound
	PlaySound(NULL, NULL, SND_ASYNC | SND_PURGE | SND_NOSTOP | SND_NOWAIT);

	// free image
	if (m_bPortraitShown)
	{
		CStatic* pStaticImage = (CStatic*) GetDlgItem(IDC_IMAGE);
		pStaticImage->SetBitmap(NULL);
		m_bitmapPortrait.DeleteObject();
	}

	//	
	CDialog::OnClose();
}

void CAboutDlg::OnOK() 
{
	// stop the sound
	PlaySound(NULL, NULL, SND_ASYNC | SND_PURGE | SND_NOSTOP | SND_NOWAIT);
	//	
	CDialog::OnOK();
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_IMAGE, OnImageClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CEasyBApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CEasyBApp commands



//
void CEasyBApp::SetSuitSequence(int nSeq)
{
	if ((nSeq < 0) || (nSeq >= tnumSuitSequences))
		return;
	//
	m_nSuitSeqOption = nSeq;
	for(int i=0;i<4;i++)
		m_nSuitSequence[i] = m_nSuitSeqList[nSeq][i];

	// init dummy's suit sequence
	InitDummySuitSequence(NONE);
}


//
void CEasyBApp::InitDummySuitSequence(int nTrumpSuit, int nDummyPosition)
{
	// set special dummy sequence, if desired
	if (ISSUIT(nTrumpSuit) && ISPLAYER(nDummyPosition) && m_bShowDummyTrumpsOnLeft)
	{
		// check who's dummy
		if (nDummyPosition == WEST)
		{
			// if dummy is West, the trump suit goes LAST (on the left)
			m_nDummySuitSequence[3] = nTrumpSuit;
			for(int i=0,nIndex=0;i<3;i++)
			{
				if (m_nSuitSequence[nIndex] != nTrumpSuit)
					m_nDummySuitSequence[i] = m_nSuitSequence[nIndex];
				else
					i--;
				nIndex++;
			}
		}
		else
		{
			// otherwise the trump suit goes first
			m_nDummySuitSequence[0] = nTrumpSuit;
			for(int i=1,nIndex=0;i<4;i++)
			{
				if (m_nSuitSequence[nIndex] != nTrumpSuit)
					m_nDummySuitSequence[i] = m_nSuitSequence[nIndex];
				else
					i--;
				nIndex++;
			}
		}
	}
	else
	{
		// dummy's sequence is the same as everyone else
		for(int i=0;i<4;i++)
			m_nDummySuitSequence[i] = m_nSuitSequence[i];
	}
}



//
int CEasyBApp::GetMinimumOpeningValue(CPlayer* pPlayer) const
{
	//
	int nOpeningPos;
	if (pPlayer)
		nOpeningPos = pPlayer->GetValue(tnOpeningPosition);
	if ((pPlayer == NULL) || (nOpeningPos < 0) || (nOpeningPos > 3))
		return 10;
	BOOL b3rd4thPos = FALSE;
	if ((nOpeningPos == 2) || (nOpeningPos == 3))
		b3rd4thPos = TRUE;
	//
	if (b3rd4thPos)
		return 10;
//	else if ((CURR_CONVENTIONSET.nAllowable1Openings & OB_11_HCPS_RBS_LM) ||
//			 (CURR_CONVENTIONSET.nAllowable1Openings & OB_11_HCPS_6CS))
//		return 11;
	else
		return 12;
}


