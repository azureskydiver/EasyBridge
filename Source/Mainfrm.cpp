//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// mainfrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "mainfrm.h"
//#include "DockingTextWnd.h"
#include "MyStatusBar.h"
#include "HistoryWnd.h"
#include "MainFrameopts.h"
#include "EasyBvw.h"
#include "Player.h"
#include "Deck.h"
#include "progopts.h"
#include "docopts.h"
#include "viewopts.h"
#include "CardLayout.h"
#include "FileComments.h"
#include "AnalysisDialog.h"
#include "GameReviewDialog.h"
#include "BidDialog.h"
//#include "scoredialog.h"
#include "help\helpcode.h"
#include "BidOptionsPropSheet.h"
#include "DealOptionsPropSheet.h"
#include "DispOptionsPropSheet.h"
#include "GameOptionsPropSheet.h"
#include "StatusWnd.h"
#include "StatusAnalysesPage.h"
#include "BidDlgLarge.h"
#include "BidDlgSmall.h"
#include "DailyTipDialog.h"
#include "PlayerStatusDialog.h"
#include "AutoHintDialog.h"
#include "WelcomeWnd.h"
#include "NNetOutputDialog.h"
#include "ProgramConfigWizard.h"

//
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const int tnDefaultWidth = 644;
const int tnDefaultHeight = 518;

const int DEF_ANALYSIS_WIDTH	= 140;
const int DEF_ANALYSIS_HEIGHT	= 118;

extern POINT defDrawPoint[];

const POINT	nAnalysisDlgOffset[4] = {
	// south,      west,       north,     east
	 { 70, 56 }, { 15, 108 }, { 22, 56 }, { -90, 108 },
};
const POINT	nAnalysisSize[4] = {
	// south,       west,         north,       east
	 { 234, 88 }, { 160, 140 }, { 220, 88 }, { 166, 140 }, 
};
const POINT	nDefBHDlgPoint = { 446, 160 };
const POINT	nDefPHDlgPoint = {  24, 160 };


static void GetProfileFont(LPCTSTR szSec, LOGFONT* plf);
static void WriteProfileFont(LPCTSTR szSec, const LOGFONT* plf);


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CCJFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CCJFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_PALETTECHANGED()
	ON_WM_QUERYNEWPALETTE()
	ON_COMMAND(ID_DISPLAY_OPTIONS, OnDisplayOptions)
	ON_WM_DESTROY()
	ON_COMMAND(ID_HELP_INTRODUCTION, OnHelpIntroduction)
	ON_COMMAND(ID_HELP_MENUS, OnHelpMenus)
	ON_COMMAND(ID_HELP_RULES, OnHelpRules)
	ON_COMMAND(ID_HELP_GLOSSARY, OnHelpGlossary)
	ON_COMMAND(ID_HELP_SEARCH, OnHelpSearch)
	ON_COMMAND(ID_HELP_MISC, OnHelpMisc)
	ON_WM_RBUTTONDOWN()
	ON_UPDATE_COMMAND_UI(ID_DEAL_OPTIONS, OnUpdateDealOptions)
	ON_COMMAND(ID_DEAL_OPTIONS, OnDealOptions)
	ON_COMMAND(ID_SHOW_ANALYSIS_ALL, OnShowAnalysisAll)
	ON_UPDATE_COMMAND_UI(ID_SHOW_ANALYSIS_ALL, OnUpdateShowAnalysis)
	ON_COMMAND(ID_GAME_OPTIONS, OnGameOptions)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILE_COMMENTS, OnUpdateViewFileComments)
	ON_COMMAND(ID_VIEW_FILE_COMMENTS, OnViewFileComments)
	ON_UPDATE_COMMAND_UI(ID_BIDDING_OPTIONS, OnUpdateBiddingOptions)
	ON_COMMAND(ID_BIDDING_OPTIONS, OnBiddingOptions)
	ON_WM_SIZE()
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	ON_WM_HELPINFO()
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_HELP_QUICK_START, OnHelpQuickStart)
	ON_COMMAND(ID_HELP_FAQ, OnHelpFaq)
	ON_COMMAND(ID_TOGGLE_ANALYSIS_TRACING, OnToggleAnalysisTracing)
	ON_UPDATE_COMMAND_UI(ID_TOGGLE_ANALYSIS_TRACING, OnUpdateToggleAnalysisTracing)
	ON_WM_CLOSE()
	ON_COMMAND(ID_SWAP_CARDS, OnSwapCards)
	ON_COMMAND(ID_HELP_TIP_OF_THE_DAY, OnHelpTipOfTheDay)
	ON_COMMAND(ID_HELP_README, OnHelpReadme)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NEURAL_NET, OnUpdateViewNeuralNet)
	ON_COMMAND(ID_VIEW_NEURAL_NET, OnViewNeuralNet)
	ON_UPDATE_COMMAND_UI(ID_EXPOSE_ALL_CARDS, OnUpdateExposeAllCards)
	ON_COMMAND(ID_EXPOSE_ALL_CARDS, OnExposeAllCards)
	ON_UPDATE_COMMAND_UI(ID_GAME_AUTOHINT, OnUpdateGameAutoHint)
	ON_COMMAND(ID_GAME_AUTOHINT, OnGameAutoHint)
	ON_UPDATE_COMMAND_UI(ID_TRAINING_MODE, OnUpdateTrainingMode)
	ON_COMMAND(ID_TRAINING_MODE, OnTrainingMode)
	ON_UPDATE_COMMAND_UI(ID_PLAY_MODE_NORMAL, OnUpdatePlayModeNormal)
	ON_COMMAND(ID_PLAY_MODE_NORMAL, OnPlayModeNormal)
	ON_COMMAND(ID_PLAY_MODE_MANUAL, OnPlayModeManual)
	ON_UPDATE_COMMAND_UI(ID_PLAY_MODE_MANUAL, OnUpdatePlayModeManual)
	ON_UPDATE_COMMAND_UI(ID_PLAY_MODE_MANUAL_DEFEND, OnUpdatePlayModeManualDefend)
	ON_COMMAND(ID_PLAY_MODE_MANUAL_DEFEND, OnPlayModeManualDefend)
	ON_UPDATE_COMMAND_UI(ID_PLAY_MODE_FULL_AUTO, OnUpdatePlayModeFullAuto)
	ON_COMMAND(ID_PLAY_MODE_FULL_AUTO, OnPlayModeFullAuto)
	ON_UPDATE_COMMAND_UI(ID_PLAY_MODE_LOCK, OnUpdatePlayModeLock)
	ON_COMMAND(ID_PLAY_MODE_LOCK, OnPlayModeLock)
	ON_UPDATE_COMMAND_UI(ID_PROG_CONFIG_WIZARD, OnUpdateProgConfigWizard)
	ON_COMMAND(ID_PROG_CONFIG_WIZARD, OnProgConfigWizard)
	ON_UPDATE_COMMAND_UI(ID_SHOW_COMMENT_IDENTIFIERS, OnUpdateShowCommentIdentifiers)
	ON_COMMAND(ID_SHOW_COMMENT_IDENTIFIERS, OnShowCommentIdentifiers)
	ON_UPDATE_COMMAND_UI(ID_MANUAL_BIDDING, OnUpdateManualBidding)
	ON_COMMAND(ID_MANUAL_BIDDING, OnManualBidding)
	ON_UPDATE_COMMAND_UI(ID_MANUAL_PLAY, OnUpdateManualPlay)
	ON_COMMAND(ID_MANUAL_PLAY, OnManualPlay)
	//}}AFX_MSG_MAP
//	ON_UPDATE_COMMAND_UI(ID_VIEW_HISTORY, OnUpdateViewHistory)
//	ON_COMMAND(ID_VIEW_HISTORY, OnViewHistory)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HISTORY, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_HISTORY, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_SHOW_STATUS, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_SHOW_STATUS, OnBarCheck)
	ON_UPDATE_COMMAND_UI(IDR_SECONDARY_TOOLBAR, OnUpdateControlBarMenu)
	ON_COMMAND_EX(IDR_SECONDARY_TOOLBAR, OnBarCheck)
	ON_UPDATE_COMMAND_UI(IDR_TEST_TOOLBAR, OnUpdateControlBarMenu)
	ON_COMMAND_EX(IDR_TEST_TOOLBAR, OnBarCheck)
	//
	ON_COMMAND(ID_HELP, OnHelpIndex)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TRICKS, OnUpdatePane)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_CONTRACT, OnUpdatePane)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_DECLARER, OnUpdatePane)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_VULNERABLE, OnUpdatePane)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_MODE, OnUpdatePane)
/*
	ON_COMMAND(ID_SHOW_ANALYSIS_EAST, OnShowAnalysisEast)
	ON_COMMAND(ID_SHOW_ANALYSIS_NORTH, OnShowAnalysisNorth)
	ON_COMMAND(ID_SHOW_ANALYSIS_SOUTH, OnShowAnalysisSouth)
	ON_COMMAND(ID_SHOW_ANALYSIS_WEST, OnShowAnalysisWest)
	ON_UPDATE_COMMAND_UI(ID_SHOW_ANALYSIS_EAST, OnUpdateShowAnalysisEast)
	ON_UPDATE_COMMAND_UI(ID_SHOW_ANALYSIS_NORTH, OnUpdateShowAnalysisNorth)
	ON_UPDATE_COMMAND_UI(ID_SHOW_ANALYSIS_SOUTH, OnUpdateShowAnalysisSouth)
	ON_UPDATE_COMMAND_UI(ID_SHOW_ANALYSIS_WEST, OnUpdateShowAnalysisWest)
*/
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// member initialization
//	m_pWndBiddingHistory = NULL;
//	m_pWndPlayHistory = NULL;
	m_pWndHistory = NULL;
	m_pDailyTipDialog = NULL;
	m_nDefaultCardSwapOperation = ID_SWAP_POSITION_EAST;
	//
	m_bStatusDialogActive = FALSE;
	m_bCommentsDialogActive = FALSE;
	m_bHistoryDialogActive = FALSE;
	m_bBHDialogActive = FALSE;
	m_bPHDialogActive = FALSE;
	m_pWndStatus = NULL;
	m_pAutoHintDialog = NULL;
	m_pBidDlg = NULL;
	m_nStatusBarLock = 0;
	m_bIsClosing = FALSE;
	for(int i=0;i<4;i++)
		m_bAnalysisDialogActive[i] = FALSE;
}

CMainFrame::~CMainFrame()
{
}




//
// Settings load/save code
//
static TCHAR BASED_CODE szDockingRegistryKey[] = _T("Control Bars\\Position");

//static TCHAR BASED_CODE szDisplayOptions[] = _T("Display Options");
static TCHAR BASED_CODE szAnalysisFont[] = _T("Analysis Dialog Font");
static TCHAR BASED_CODE szHistoryFont[] = _T("History Dialog Font");
static TCHAR BASED_CODE szAutoHintFont[] = _T("Auto Hint Window Font");
static TCHAR BASED_CODE szHeight[] = _T("Height");
static TCHAR BASED_CODE szWeight[] = _T("Weight");
static TCHAR BASED_CODE szItalic[] = _T("Italic");
static TCHAR BASED_CODE szUnderline[] = _T("Underline");
static TCHAR BASED_CODE szPitchAndFamily[] = _T("Pitch And Family");
static TCHAR BASED_CODE szFaceName[] = _T("Face Name");
static TCHAR BASED_CODE szSansSerif[] = _T("MS Sans Serif");
//
static TCHAR BASED_CODE szDialogSettings[] = _T("Dialog Settings");
static TCHAR BASED_CODE szHistoryDialogWidth[] = _T("History Dialog Width");
static TCHAR BASED_CODE szHistoryDialogHeight[] = _T("History Dialog Height");
static TCHAR BASED_CODE szStatusDialogWidth[] = _T("Status Dialog Width");
static TCHAR BASED_CODE szStatusDialogHeight[] = _T("Status Dialog Height");
static TCHAR* BASED_CODE szAnalysisDialogOpen[4] = {
	_T("Dialog 1 Open"), _T("Dialog 2 Open"), _T("Dialog 3 Open"), _T("Dialog 4 Open")
};
//
static TCHAR BASED_CODE szStatusDialogActive[] = _T("Status Dialog Active");
//
static TCHAR BASED_CODE szGameReviewDlgX[] = _T("Game Review Dialog Pos X");
static TCHAR BASED_CODE szGameReviewDlgY[] = _T("Game Review Dialog Pos Y");
static TCHAR BASED_CODE szNNetOutputDlgX[] = _T("Neural Network Output Dialog Pos X");
static TCHAR BASED_CODE szNNetOutputDlgY[] = _T("Neural Network Output Dialog Pos Y");
static TCHAR BASED_CODE szCommentsDlgX[] = _T("File Comments Dialog Pos Left");
static TCHAR BASED_CODE szCommentsDlgY[] = _T("File Comments Dialog Pos Top");
static TCHAR BASED_CODE szCommentsDlgCX[] = _T("File Comments Dialog Pos Right");
static TCHAR BASED_CODE szCommentsDlgCY[] = _T("File Comments Dialog Pos Bottom");
static TCHAR BASED_CODE szAutoHintDlgX[] = _T("AutoHint Dialog Pos Left");
static TCHAR BASED_CODE szAutoHintDlgY[] = _T("AutoHint Dialog Pos Top");
static TCHAR BASED_CODE szAutoHintDlgCX[] = _T("AutoHint Dialog Pos Right");
static TCHAR BASED_CODE szAutoHintDlgCY[] = _T("AutoHint Dialog Pos Bottom");
static TCHAR BASED_CODE szDockingWndWidth[] = _T("Docking Window Width");
static TCHAR BASED_CODE szDockingWndHeight[] = _T("Docking Window Height");

//
static TCHAR BASED_CODE szWindowPlacement[] = _T("Main Program Window Placement");
static TCHAR BASED_CODE szShowCommand[] = _T("Main Window State");
static TCHAR BASED_CODE szShowFlags[] = _T("Main Window Display Flags");
static TCHAR BASED_CODE szWinRectTop[] = _T("Main Window Top");
static TCHAR BASED_CODE szWinRectLeft[] = _T("Main Window Left");
static TCHAR BASED_CODE szWinRectBottom[] = _T("Main Window Bottom");
static TCHAR BASED_CODE szWinRectRight[] = _T("Main Window Right");

//
static void GetProfileFont(LPCTSTR szSec, LOGFONT* plf)
{
	CWinApp* pApp = AfxGetApp();
	memset(plf, 0, sizeof(LOGFONT));
	plf->lfHeight = pApp->GetProfileInt(szSec, szHeight, 0);
	plf->lfWeight = pApp->GetProfileInt(szSec, szWeight, 400);
	plf->lfItalic = (BYTE)pApp->GetProfileInt(szSec, szItalic, 0);
	plf->lfUnderline = (BYTE)pApp->GetProfileInt(szSec, szUnderline, 0);
	plf->lfPitchAndFamily = (BYTE)pApp->GetProfileInt(szSec, szPitchAndFamily, 49);
	CString strFont = pApp->GetProfileString(szSec, szFaceName, _T(""));
	lstrcpyn((TCHAR*)plf->lfFaceName, strFont, sizeof plf->lfFaceName);
	plf->lfFaceName[sizeof plf->lfFaceName-1] = 0;
}

static void WriteProfileFont(LPCTSTR szSec, const LOGFONT* plf)
{
	CWinApp* pApp = AfxGetApp();

	pApp->WriteProfileInt(szSec, szHeight, plf->lfHeight);
	pApp->WriteProfileInt(szSec, szWeight, plf->lfWeight);
	pApp->WriteProfileInt(szSec, szItalic, plf->lfItalic);
	pApp->WriteProfileInt(szSec, szUnderline, plf->lfUnderline);
	pApp->WriteProfileInt(szSec, szPitchAndFamily, plf->lfPitchAndFamily);
	pApp->WriteProfileString(szSec, szFaceName, (LPCTSTR)plf->lfFaceName);
}

//
void CMainFrame::Initialize() 
{
	// called after the document and frame have been created

	// read in the history dialog font
	GetProfileFont(szHistoryFont, &m_lfHistory);
	// read in analysis dialog font
	GetProfileFont(szAnalysisFont, &m_lfAnalysis);
	// read in hint dialog font
	GetProfileFont(szAutoHintFont, &m_lfAutoHint);

	// read in analysis dialog info
	for(int i=0;i<4;i++)
		m_bAnalysisDialogActive[i] = theApp.GetProfileInt(szDialogSettings, szAnalysisDialogOpen[i], FALSE);
	m_bStatusDialogActive = theApp.GetProfileInt(szDialogSettings, szStatusDialogActive, FALSE);

	// and other dialog positions
	m_pointHistoryDialog.x = theApp.GetProfileInt(szDialogSettings, szHistoryDialogWidth, -1);
	m_pointHistoryDialog.y = theApp.GetProfileInt(szDialogSettings, szHistoryDialogHeight, -1);
	m_pointStatusDialog.x = theApp.GetProfileInt(szDialogSettings, szStatusDialogWidth, -1);
	m_pointStatusDialog.y = theApp.GetProfileInt(szDialogSettings, szStatusDialogHeight, -1);
	m_pointGameReviewDialog.x = theApp.GetProfileInt(szDialogSettings, szGameReviewDlgX, 20);
	m_pointGameReviewDialog.y = theApp.GetProfileInt(szDialogSettings, szGameReviewDlgY, 20);
	m_pointNNetOutputDialog.x = theApp.GetProfileInt(szDialogSettings, szNNetOutputDlgX, -1);
	m_pointNNetOutputDialog.y = theApp.GetProfileInt(szDialogSettings, szNNetOutputDlgY, -1);
	m_rectCommentsDialog.left = theApp.GetProfileInt(szDialogSettings, szCommentsDlgX, 20);
	m_rectCommentsDialog.top = theApp.GetProfileInt(szDialogSettings, szCommentsDlgY, 40);
	m_rectCommentsDialog.right = theApp.GetProfileInt(szDialogSettings, szCommentsDlgCX, 420);
	m_rectCommentsDialog.bottom = theApp.GetProfileInt(szDialogSettings, szCommentsDlgCY, 180);
	m_rectAutoHintDialog.left = theApp.GetProfileInt(szDialogSettings, szAutoHintDlgX, 20);
	m_rectAutoHintDialog.top = theApp.GetProfileInt(szDialogSettings, szAutoHintDlgY, 100);
	m_rectAutoHintDialog.right = theApp.GetProfileInt(szDialogSettings, szAutoHintDlgCX, 220);
	m_rectAutoHintDialog.bottom = theApp.GetProfileInt(szDialogSettings, szAutoHintDlgCY, 300);
	//
	m_nDockingWndWidth = theApp.GetProfileInt(szDialogSettings, szDockingWndWidth, 208);
	m_nDockingWndHeight = theApp.GetProfileInt(szDialogSettings, szDockingWndHeight, 180);

	// load save options
	// get basic settings
	m_winPlacement.showCmd = theApp.GetProfileInt(szWindowPlacement, szShowCommand, SW_SHOWMAXIMIZED);
	m_winPlacement.flags = theApp.GetProfileInt(szWindowPlacement, szShowFlags, 0);
	m_winPlacement.rcNormalPosition.top = theApp.GetProfileInt(szWindowPlacement, szWinRectTop, -999);
	m_winPlacement.rcNormalPosition.left = theApp.GetProfileInt(szWindowPlacement, szWinRectLeft, -999);
	m_winPlacement.rcNormalPosition.bottom = theApp.GetProfileInt(szWindowPlacement, szWinRectBottom, tnDefaultHeight);
	m_winPlacement.rcNormalPosition.right = theApp.GetProfileInt(szWindowPlacement, szWinRectRight, tnDefaultWidth);
	m_winPlacement.ptMaxPosition.x = m_winPlacement.ptMaxPosition.y = -1;
	m_winPlacement.ptMinPosition.x = m_winPlacement.ptMinPosition.y = -1;
	m_winPlacement.length = sizeof(m_winPlacement);
	if ((m_winPlacement.rcNormalPosition.left > -100) && (m_winPlacement.rcNormalPosition.top > -100) &&
		(m_winPlacement.rcNormalPosition.bottom > -100) && (m_winPlacement.rcNormalPosition.right > -100) &&
		(m_winPlacement.rcNormalPosition.left < 10000) && (m_winPlacement.rcNormalPosition.top < 10000) &&
		(m_winPlacement.rcNormalPosition.bottom < 20000) && (m_winPlacement.rcNormalPosition.right < 20000))
	{
		// SetWindowPlacement never seems to work right, particularly when the window is maximized
//		SetWindowPlacement(&m_winPlacement);
		SetWindowPos(&wndTop, 
					  m_winPlacement.rcNormalPosition.left, 
					  m_winPlacement.rcNormalPosition.top,
					  m_winPlacement.rcNormalPosition.right - m_winPlacement.rcNormalPosition.left,
					  m_winPlacement.rcNormalPosition.bottom - m_winPlacement.rcNormalPosition.top,
					  SWP_NOACTIVATE | SWP_NOZORDER);
		theApp.m_nCmdShow = m_winPlacement.showCmd;
	}
	else
	{
		// if size is not initialized, show maximized
		theApp.m_nCmdShow = SW_SHOWMAXIMIZED;
	}
}


//
void CMainFrame::Terminate() 
{
	// save settings
	if (m_pDailyTipDialog)
		theApp.SetValue(tbShowDailyTipDialog, m_pDailyTipDialog->m_bShowAtStartup);

	// save history dialog font
	WriteProfileFont(szHistoryFont, &m_lfHistory);
	// save analysis dialog font
	WriteProfileFont(szAnalysisFont, &m_lfAnalysis);
	// save read in hint dialog font
	WriteProfileFont(szAutoHintFont, &m_lfAutoHint);

	// and analysis dialog info
	for(int i=0;i<4;i++)
		theApp.WriteProfileInt(szDialogSettings, szAnalysisDialogOpen[i], m_bAnalysisDialogActive[i]);
	theApp.WriteProfileInt(szDialogSettings, szStatusDialogActive, m_bStatusDialogActive);

	// and other dialog positions
	theApp.WriteProfileInt(szDialogSettings, szHistoryDialogWidth, m_pointHistoryDialog.x);
	theApp.WriteProfileInt(szDialogSettings, szHistoryDialogHeight, m_pointHistoryDialog.y);
	theApp.WriteProfileInt(szDialogSettings, szStatusDialogWidth, m_pointStatusDialog.x);
	theApp.WriteProfileInt(szDialogSettings, szStatusDialogHeight, m_pointStatusDialog.y);
	theApp.WriteProfileInt(szDialogSettings, szGameReviewDlgX, m_pointGameReviewDialog.x);
	theApp.WriteProfileInt(szDialogSettings, szGameReviewDlgY, m_pointGameReviewDialog.y);
	theApp.WriteProfileInt(szDialogSettings, szNNetOutputDlgX, m_pointNNetOutputDialog.x);
	theApp.WriteProfileInt(szDialogSettings, szNNetOutputDlgY, m_pointNNetOutputDialog.y);
	theApp.WriteProfileInt(szDialogSettings, szCommentsDlgX, m_rectCommentsDialog.left);
	theApp.WriteProfileInt(szDialogSettings, szCommentsDlgY, m_rectCommentsDialog.top);
	theApp.WriteProfileInt(szDialogSettings, szCommentsDlgCX, m_rectCommentsDialog.right);
	theApp.WriteProfileInt(szDialogSettings, szCommentsDlgCY, m_rectCommentsDialog.bottom);
	theApp.WriteProfileInt(szDialogSettings, szAutoHintDlgX, m_rectAutoHintDialog.left);
	theApp.WriteProfileInt(szDialogSettings, szAutoHintDlgY, m_rectAutoHintDialog.top);
	theApp.WriteProfileInt(szDialogSettings, szAutoHintDlgCX, m_rectAutoHintDialog.right);
	theApp.WriteProfileInt(szDialogSettings, szAutoHintDlgCY, m_rectAutoHintDialog.bottom);
	//
	theApp.WriteProfileInt(szDialogSettings, szDockingWndWidth, m_nDockingWndWidth);
	theApp.WriteProfileInt(szDialogSettings, szDockingWndHeight, m_nDockingWndHeight);

	// save window placement
	GetWindowPlacement(&m_winPlacement);
	theApp.WriteProfileInt(szWindowPlacement, szShowCommand, m_winPlacement.showCmd);
	theApp.WriteProfileInt(szWindowPlacement, szShowFlags, m_winPlacement.flags);
	theApp.WriteProfileInt(szWindowPlacement, szWinRectTop, m_winPlacement.rcNormalPosition.top);
	theApp.WriteProfileInt(szWindowPlacement, szWinRectLeft, m_winPlacement.rcNormalPosition.left);
	theApp.WriteProfileInt(szWindowPlacement, szWinRectBottom, m_winPlacement.rcNormalPosition.bottom);
	theApp.WriteProfileInt(szWindowPlacement, szWinRectRight, m_winPlacement.rcNormalPosition.right);
}


//
void CMainFrame::OnClose() 
{
	// mark that we're closing
	m_bIsClosing = TRUE;

	// save docking control bar status
	SaveBarState(szDockingRegistryKey);
	//
	CCJFrameWnd::OnClose();
}


//
void CMainFrame::OnDestroy() 
{
	// save program settings
	Terminate();

	// destroy status bar
	m_pWndStatusBar->DestroyWindow();
	delete m_pWndStatusBar;

	// delete the fonts
	m_fixedFont.DeleteObject();
	m_standardFont.DeleteObject();
	m_smallFont.DeleteObject();
	m_largeFont.DeleteObject();
	m_tricksFont.DeleteObject();
	m_analysisFont.DeleteObject();
	m_historyFont.DeleteObject();
	m_dialogFont.DeleteObject();
//	m_dialogBoldFont.DeleteObject();

	// and the dialogs
	m_pLayoutDlg->DestroyWindow();
	delete m_pLayoutDlg;
	//
	if (m_pDailyTipDialog)
	{
		m_pDailyTipDialog->DestroyWindow();
		delete m_pDailyTipDialog;
	}
	
	//
	m_pFileCommentsDlg->DestroyWindow();
	delete m_pFileCommentsDlg;
	//
	m_pGameReviewDlg->DestroyWindow();
	delete m_pGameReviewDlg;
	// 
	m_pBidDlg->DestroyWindow();
	delete m_pBidDlg;
	//
	m_pAutoHintDialog->DestroyWindow();
	delete m_pAutoHintDialog;
	//
	m_pWndStatus->DestroyWindow();
	delete m_pWndStatus;

	//
//	m_pScoreDialog->DestroyWindow();
//	delete m_pScoreDialog;

	m_pNNetOutputDialog->DestroyWindow();
	delete m_pNNetOutputDialog;

	// the following will auto-destruct (in 5 seconds...)
//	m_biddingHistoryDlg.DestroyWindow();
//	m_playHistoryDlg.DestroyWindow();

/*
	// and the players' analysis windows
	int i;
	for(i=0;i<4;i++)
	{
		m_pDlgAnalysis[i]->DestroyWindow();
		delete m_pDlgAnalysis[i];
	}
*/

	// delete control bars
/*
	m_pWndBiddingHistory->DestroyWindow();
	delete m_pWndBiddingHistory;
	//
	m_pWndPlayHistory->DestroyWindow();
	delete m_pWndPlayHistory;
*/
	m_pWndHistory->DestroyWindow();
	delete m_pWndHistory;

	// delete the poput menu
	m_popupMenu.DestroyMenu();

	//
	CCJFrameWnd::OnDestroy();
}


//
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	// no thick border on this frame
	cs.style &= ~FWS_ADDTOTITLE;
	//
	return CCJFrameWnd::PreCreateWindow(cs);
}



//
void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	//
	CCJFrameWnd::OnSize(nType, cx, cy);

	// resposition the bid dialog if active
	if (m_pBidDlg && m_pBidDlg->IsWindowVisible())
		m_pBidDlg->RepositionWindow();
}


//
void CMainFrame::DockControlBarLeftOf(CToolBar* Bar, CToolBar* LeftOf)
{
    CRect rect;
    DWORD dw;
    UINT n;

    // get MFC to adjust the dimensions of all docked ToolBars
    // so that GetWindowRect will be accurate
    RecalcLayout(TRUE);

    LeftOf->GetWindowRect(&rect);
    rect.OffsetRect(1,0);
    dw=LeftOf->GetBarStyle();
    n = 0;
    n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
    n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
    n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
    n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;

    // When we take the default parameters on rect, DockControlBar will dock
    // each Toolbar on a seperate line. By calculating a rectangle, we
    // are simulating a Toolbar being dragged to that location and docked.
    DockControlBar(Bar,n,&rect);
}


/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

//
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
//	ID_INDICATOR_TRICKS,
	ID_INDICATOR_CONTRACT,
	ID_INDICATOR_DECLARER,
	ID_INDICATOR_VULNERABLE,
	ID_INDICATOR_MODE,
//	ID_INDICATOR_CAPS,
//	ID_INDICATOR_NUM,
//	ID_INDICATOR_SCRL,
};



//
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//
	if (CCJFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// set pointer to self
	theApp.m_pMainWnd = this;

	// get system caps
	HDC hDC = ::GetDC(NULL);
	m_nSysScreenWidth = ::GetDeviceCaps(hDC,HORZRES);
	m_nSysScreenHeight = ::GetDeviceCaps(hDC,VERTRES);
	m_nSysNumColors = ::GetDeviceCaps(hDC,NUMCOLORS);
	m_nSysColorPlanes = ::GetDeviceCaps(hDC,PLANES);
	m_nSysBitsPerPixel = ::GetDeviceCaps(hDC,BITSPIXEL);
	m_bSysRCDIBitmap = ::GetDeviceCaps(hDC,RC_DI_BITMAP);
	m_bSysRCDIBtoDev = ::GetDeviceCaps(hDC,RC_DIBTODEV);
	m_nSysClipCaps = ::GetDeviceCaps(hDC,CLIPCAPS);
	m_nSysRasterCaps = ::GetDeviceCaps(hDC,RASTERCAPS);
	::ReleaseDC(NULL,hDC);

	// mark whether the screen is high-res (i.e., >= 800x600
	if ((m_nSysScreenWidth >= 800) && (m_nSysScreenWidth >= 600))
		m_bHighResDisplay = TRUE;
	else
		m_bHighResDisplay = FALSE;

	// check the low-res option
	BOOL bLowResOption = theApp.GetValue(tbLowResOption);

	// call init routine to read in saved params
	Initialize();

	// create the toolbar
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.SetWindowText(_T("Toolbar"));

/*
	if (!m_wndMenuBar.Create(this) ||
		!m_wndMenuBar.LoadMenu(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndMenuBar.SetWindowText(_T("Menu"));
*/

/*
	//
	UINT nToolbars[] = { IDR_MAINFRAME, IDR_MAINFRAME_TOOLBAR, IDR_SECONDARY_TOOLBAR, IDR_TEST_TOOLBAR, IDR_TOOLBAR_HIDDEN_BUTTONS };
	m_menu.LoadToolbars(nToolbars, sizeof(nToolbars) / sizeof(UINT));
*/

/*
	//
	m_wndMenuBar.GetMenu()->AddFromToolBar((CToolBar*)&m_wndToolBar, IDR_MAINFRAME_TOOLBAR);
	m_wndMenuBar.GetMenu()->AddFromToolBar((CToolBar*)&m_wndSecondaryToolBar, IDR_SECONDARY_TOOLBAR);
	m_wndMenuBar.GetMenu()->AddFromToolBar((CToolBar*)&m_wndTestToolBar, IDR_TEST_TOOLBAR);
*/

	// create secondary toolbar
	if (!m_wndSecondaryToolBar.Create(this) ||
		!m_wndSecondaryToolBar.LoadToolBar(IDR_SECONDARY_TOOLBAR))
	{
		TRACE0("Failed to create secondary toolbar\n");
		return -1;      // fail to create
	}
	m_wndSecondaryToolBar.SetWindowText(_T("Toolbar 2"));
	m_wndSecondaryToolBar.SetDlgCtrlID(IDR_SECONDARY_TOOLBAR);

	// create testing toolbar
#ifdef _DEBUG
	if (!m_wndTestToolBar.Create(this) ||
		!m_wndTestToolBar.LoadToolBar(IDR_TESTING_TOOLBAR))
	{
		TRACE0("Failed to create play toolbar\n");
		return -1;      // fail to create
	}
	m_wndTestToolBar.SetWindowText(_T("Play Toolbar"));
	m_wndTestToolBar.SetDlgCtrlID(IDR_TEST_TOOLBAR);
#endif

	// tweak the toolbar
	// set checkbox style buttons
	m_wndToolBar.SetButtonStyle(m_wndToolBar.CommandToIndex(ID_PLAY_RUBBER), TBBS_CHECKBOX);
	m_wndToolBar.SetButtonStyle(m_wndToolBar.CommandToIndex(ID_MANUAL_BIDDING), TBBS_CHECKBOX);
	m_wndToolBar.SetButtonStyle(m_wndToolBar.CommandToIndex(ID_MANUAL_PLAY), TBBS_CHECKBOX);
	m_wndToolBar.SetButtonStyle(m_wndToolBar.CommandToIndex(ID_GAME_AUTOHINT), TBBS_CHECKBOX);
	m_wndToolBar.SetButtonStyle(m_wndToolBar.CommandToIndex(ID_GAME_AUTO_PLAY_ALL), TBBS_CHECKBOX);
	m_wndToolBar.SetButtonStyle(m_wndToolBar.CommandToIndex(ID_GAME_AUTO_PLAY_EXPRESS), TBBS_CHECKBOX);
	m_wndToolBar.SetButtonStyle(m_wndToolBar.CommandToIndex(ID_EXPOSE_ALL_CARDS), TBBS_CHECKBOX);
	m_wndToolBar.SetButtonStyle(m_wndToolBar.CommandToIndex(ID_VIEW_HISTORY), TBBS_CHECKBOX);
	m_wndToolBar.SetButtonStyle(m_wndToolBar.CommandToIndex(ID_VIEW_FILE_COMMENTS), TBBS_CHECKBOX);
//	m_wndToolBar.SetButtonStyle(m_wndToolBar.CommandToIndex(ID_VIEW_BIDDING_HISTORY), TBBS_CHECKBOX);
//	m_wndToolBar.SetButtonStyle(m_wndToolBar.CommandToIndex(ID_VIEW_PLAY_HISTORY), TBBS_CHECKBOX);
	m_wndToolBar.SetButtonStyle(m_wndToolBar.CommandToIndex(ID_SHOW_STATUS), TBBS_CHECKBOX);
	m_wndToolBar.SetButtonStyle(m_wndToolBar.CommandToIndex(ID_TOGGLE_ANALYSIS_TRACING), TBBS_CHECKBOX);

	// tweak the secondary toolbar
	// set checkbox style buttons
//	m_wndSecondaryToolBar.SetButtonStyle(m_wndSecondaryToolBar.CommandToIndex(ID_SHOW_ANALYSIS_ALL), TBBS_CHECKBOX);
	m_wndSecondaryToolBar.SetButtonStyle(m_wndSecondaryToolBar.CommandToIndex(ID_LAYOUT_CARDS), TBBS_CHECKBOX);
	m_wndSecondaryToolBar.SetButtonStyle(m_wndSecondaryToolBar.CommandToIndex(ID_EDIT_EXISTING_HANDS), TBBS_CHECKBOX);

	// tweak the test toolbar
#ifdef _DEBUG
	m_wndTestToolBar.SetButtonStyle(m_wndTestToolBar.CommandToIndex(ID_GAME_AUTO_PLAY_ALL), TBBS_CHECKBOX);
	m_wndTestToolBar.SetButtonStyle(m_wndTestToolBar.CommandToIndex(ID_GAME_AUTO_PLAY_EXPRESS), TBBS_CHECKBOX);
	m_wndTestToolBar.SetButtonStyle(m_wndTestToolBar.CommandToIndex(ID_PLAY_MODE_NORMAL), TBBS_CHECKBOX | TBBS_CHECKGROUP);
	m_wndTestToolBar.SetButtonStyle(m_wndTestToolBar.CommandToIndex(ID_PLAY_MODE_MANUAL), TBBS_CHECKBOX);
	m_wndTestToolBar.SetButtonStyle(m_wndTestToolBar.CommandToIndex(ID_PLAY_MODE_MANUAL_DEFEND), TBBS_CHECKBOX);
	m_wndTestToolBar.SetButtonStyle(m_wndTestToolBar.CommandToIndex(ID_PLAY_MODE_FULL_AUTO), TBBS_CHECKBOX);
	m_wndTestToolBar.SetButtonStyle(m_wndTestToolBar.CommandToIndex(ID_PLAY_MODE_LOCK), TBBS_CHECKBOX);
#endif

	// add dropdowns to appropriate buttons
	m_wndSecondaryToolBar.AddDropDownButton(ID_DEAL_GAME_HAND, IDR_DEAL_GAME_HAND, TRUE);
	m_wndSecondaryToolBar.AddDropDownButton(ID_DEAL_SLAM, IDR_DEAL_SLAM, TRUE);
	m_wndSecondaryToolBar.AddDropDownButton(ID_SWAP_CARDS, IDR_SWAP_CARDS, TRUE);

	// create the status bar
	// make sure to keep this ahead of the code that docks the toolbars!
	// else it won't work
	m_pWndStatusBar = new CMyStatusBar;
	if (!m_pWndStatusBar->Create(this) ||
		!m_pWndStatusBar->SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
//	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndSecondaryToolBar.EnableDocking(CBRS_ALIGN_ANY);
#ifdef _DEBUG
	m_wndTestToolBar.EnableDocking(CBRS_ALIGN_ANY);
#endif
	EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar(&m_wndMenuBar);
	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_wndSecondaryToolBar);
#ifdef _DEBUG
	DockControlBar(&m_wndTestToolBar);
#endif
//	DockControlBarLeftOf(&m_wndSecondaryToolBar, &m_wndToolBar);

	// TODO: Remove this if you don't want tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndSecondaryToolBar.SetBarStyle(m_wndSecondaryToolBar.GetBarStyle() | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY);
#ifdef _DEBUG
	m_wndTestToolBar.SetBarStyle(m_wndTestToolBar.GetBarStyle() | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY);
#endif
//	m_wndMenuBar.SetBarStyle(m_wndMenuBar.GetBarStyle() | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// init the menu manager
/*
	const UINT nToolbars[] = { IDR_MAINFRAME_TOOLBAR, IDR_SECONDARY_TOOLBAR, IDR_TEST_TOOLBAR, IDR_TOOLBAR_HIDDEN_BUTTONS };
	m_menuManager.Install(this);
	m_menuManager.LoadToolbars(nToolbars, sizeof(nToolbars)/sizeof(UINT));
	m_menuManager.m_bAutoAccel = FALSE;
*/
	
	//
	// create logical fonts for screen displays
	//
	CString strDefaultFont;
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	if (!::GetSystemMetrics(SM_DBCSENABLED)) 
	{
		// fixed-pitch font
		logFont.lfHeight = -12;
		logFont.lfWeight = FW_NORMAL;
		logFont.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
		strDefaultFont.LoadString(IDS_DEFAULT_FIXEDFONT);
		lstrcpy(logFont.lfFaceName, strDefaultFont);
		m_fixedFont.CreateFontIndirect(&logFont);

		// standard font for text 
		logFont.lfHeight = -14;
		logFont.lfWeight = FW_NORMAL;
		logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
		strDefaultFont.LoadString(IDS_DEFAULT_TEXTFONT);
		lstrcpy(logFont.lfFaceName, strDefaultFont);
		m_standardFont.CreateFontIndirect(&logFont);

		// small font
		logFont.lfHeight = -10;
		logFont.lfWeight = FW_NORMAL;
		logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
		strDefaultFont.LoadString(IDS_DEFAULT_TEXTFONT);
		lstrcpy(logFont.lfFaceName, strDefaultFont);
		m_smallFont.CreateFontIndirect(&logFont);

		// large font
		logFont.lfHeight = -14;
		logFont.lfWeight = FW_BOLD;
		logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
		strDefaultFont.LoadString(IDS_DEFAULT_SYSFONT);
		lstrcpy(logFont.lfFaceName, strDefaultFont);
		m_largeFont.CreateFontIndirect(&logFont);

		// tricks count font
		logFont.lfHeight = -16;
		logFont.lfWeight = FW_BOLD;
		logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
		strDefaultFont.LoadString(IDS_DEFAULT_SYSFONT);
		lstrcpy(logFont.lfFaceName, strDefaultFont);
		m_tricksFont.CreateFontIndirect(&logFont);

		// create analysis font from logical font
		if ((m_lfAnalysis.lfFaceName[0] == _T('\0')) || 
		    !m_analysisFont.CreateFontIndirect(&m_lfAnalysis))
		{
			logFont.lfHeight = -11;
			logFont.lfWeight = FW_NORMAL;
			logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
			strDefaultFont.LoadString(IDS_DEFAULT_TEXTFONT);
			lstrcpy(logFont.lfFaceName, strDefaultFont);
			if (!m_analysisFont.CreateFontIndirect(&logFont))
			{
				// default to Courier
				logFont.lfPitchAndFamily = FIXED_PITCH | FF_SWISS;
				strDefaultFont.LoadString(IDS_DEFAULT_FIXEDFONT);
				lstrcpy(logFont.lfFaceName, strDefaultFont);
				m_analysisFont.CreateFontIndirect(&logFont);
			}
		}

		// create history font from logical font
		if ((m_lfHistory.lfFaceName[0] == _T('\0')) || 
			!m_historyFont.CreateFontIndirect(&m_lfHistory)) 
		{
			// try loading the Monotype typeface
			logFont.lfHeight = bLowResOption? -10 : -11;
			logFont.lfWeight = FW_NORMAL;
			logFont.lfPitchAndFamily = FIXED_PITCH | FF_SWISS;
			strDefaultFont.LoadString(IDS_DEFAULT_HISTORYFONT);
			lstrcpy(logFont.lfFaceName, strDefaultFont);
			if (!m_historyFont.CreateFontIndirect(&logFont))
			{
				// default to Courier
				logFont.lfPitchAndFamily = FIXED_PITCH | FF_SWISS;
				strDefaultFont.LoadString(IDS_DEFAULT_FIXEDFONT);
				lstrcpy(logFont.lfFaceName, strDefaultFont);
				m_historyFont.CreateFontIndirect(&logFont);
			}
		}

		// create symbol font using same point size as history font
		m_lfSymbol = m_lfHistory;
		m_lfSymbol.lfPitchAndFamily = FIXED_PITCH | FF_SWISS;
		strDefaultFont.LoadString(IDS_DEFAULT_SYMBOLFONT);
		lstrcpy(m_lfSymbol.lfFaceName, strDefaultFont);
		if (!m_symbolFont.CreateFontIndirect(&m_lfSymbol)) 
		{
			// symbol font is unavailable!
			theApp.SetValue(tbUseSuitSymbols, FALSE);
		}

		// create autohint font from logical font
		if ((m_lfAutoHint.lfFaceName[0] == _T('\0')) || 
			!m_autoHintFont.CreateFontIndirect(&m_lfAutoHint)) 
		{
			// try loading the Monotype typeface
			logFont.lfHeight = bLowResOption? -10 : -11;
			logFont.lfWeight = FW_NORMAL;
			logFont.lfPitchAndFamily = FIXED_PITCH | FF_SWISS;
			strDefaultFont.LoadString(IDS_DEFAULT_AUTOHINTFONT);
			lstrcpy(logFont.lfFaceName, strDefaultFont);
			if (!m_autoHintFont.CreateFontIndirect(&logFont))
			{
				// default to Courier
				logFont.lfPitchAndFamily = FIXED_PITCH | FF_SWISS;
				strDefaultFont.LoadString(IDS_DEFAULT_FIXEDFONT);
				lstrcpy(logFont.lfFaceName, strDefaultFont);
				m_autoHintFont.CreateFontIndirect(&logFont);
			}
		}
		
		// create the default dialog font
		logFont.lfHeight = 13; 
		logFont.lfWidth = 0;
		logFont.lfEscapement = 0; 
		logFont.lfOrientation = 0; 
		logFont.lfWeight = 200; // non-bold font weight 
		logFont.lfItalic = 0; 
		logFont.lfUnderline = 0; 
		logFont.lfStrikeOut = 0; 
		logFont.lfCharSet = ANSI_CHARSET; 
		logFont.lfOutPrecision = OUT_DEFAULT_PRECIS; 
		logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS; 
		logFont.lfQuality = DEFAULT_QUALITY; 
		logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS; 
		logFont.lfFaceName[0] = NULL; 
		m_dialogFont.CreateFontIndirect(&logFont); 
/*
		// and its bold version
		logFont.lfWeight = FW_BOLD;
		m_dialogBoldFont.CreateFontIndirect(&logFont); 
*/
	}

	// create the popup menu
	if (m_popupMenu.CreatePopupMenu()) 
	{
		m_popupMenu.AppendMenu(0, ID_GAME_HINT, "Hint");
		m_popupMenu.AppendMenu(0, ID_GAME_AUTO_PLAY, "Auto Play Card");
		m_popupMenu.AppendMenu(0, ID_GAME_AUTO_PLAY_ALL, "Auto Play All");
		m_popupMenu.AppendMenu(0, ID_EDIT_UNDO_TRICK, "Undo Trick");
		m_popupMenu.AppendMenu(MF_SEPARATOR, 0, "");
		m_popupMenu.AppendMenu(0, ID_BID_CURRENT_HAND, "Bid/Rebid");
		m_popupMenu.AppendMenu(0, ID_RESTART_CURRENT_HAND, "Restart Play");
		m_popupMenu.AppendMenu(MF_SEPARATOR, 0, "");
		m_popupMenu.AppendMenu(0, ID_LAYOUT_CARDS, "Lay Out Cards");
		m_popupMenu.AppendMenu(0, ID_EDIT_EXISTING_HANDS, "Edit Existing Hands");
		m_popupMenu.AppendMenu(MF_SEPARATOR, 0, "");
		m_popupMenu.AppendMenu(0, ID_EXPOSE_ALL_CARDS, "Expose Cards");
		m_popupMenu.AppendMenu(0, ID_REFRESH_SCREEN, "Refresh");
	}

	// create card layout dialog
	m_pLayoutDlg = new CCardLayoutDialog;
	m_pLayoutDlg->Create(CCardLayoutDialog::IDD, this);
	m_pLayoutDlg->ShowWindow(SW_HIDE);
	m_pLayoutDlg->UpdateWindow();

	// create bidding dialog, in either low or high res versions
	if (bLowResOption)
	{
		m_pBidDlg = new CBidDialogSmall;
		m_pBidDlg->Create(CBidDialogSmall::IDD, this);
	}
	else
	{
		m_pBidDlg = new CBidDialogLarge;
		m_pBidDlg->Create(CBidDialogLarge::IDD, this);
	}
	m_pBidDlg->ShowWindow(SW_HIDE);
	m_pBidDlg->UpdateWindow();

	// create the scoring dialog
/*
	m_pScoreDialog = new CScoreDialog;
	m_pScoreDialog->Create(CScoreDialog::IDD, this);
	m_pScoreDialog->ShowWindow(SW_HIDE);
	m_pScoreDialog->UpdateWindow();
*/

	//
	//	init docking window sizes
	//
//	int nWidth = Max(m_pointHistoryDialog.x, m_pointStatusDialog.x);
	int nWidth = m_nDockingWndWidth;
	if (nWidth <= 0)
		nWidth = bLowResOption? 158: 207;
	//
//	int nHeight = Max(m_pointHistoryDialog.y, m_pointStatusDialog.y);
	int nHeight = m_nDockingWndHeight;
	if (nHeight <= 0)
		nHeight = 400;
	//
//	SetInitialSize(nHeight,nHeight,nWidth,nWidth);
	EnableDockingSizeBar(CBRS_ALIGN_ANY);

	//
	// create the bidding / play history docking window
	//
	m_pWndHistory = new CHistoryWnd;
	m_pWndHistory->Create(this, ID_VIEW_HISTORY, _T("History"), CSize(nWidth, nHeight), CBRS_LEFT);
	m_pWndHistory->EnableDockingOnSizeBar(CBRS_ALIGN_ANY);
	DockSizeBar(m_pWndHistory);
	m_pWndHistory->SetMenuID(IDR_HISTORY_CONTROLBAR);
	m_pWndHistory->ShowWindow(SW_HIDE);

	//
	// create & init the docking status window
	//
	m_pWndStatus = new CStatusWnd;
	m_pWndStatus->Create(this, ID_SHOW_STATUS, _T("Status"), CSize(nWidth ,nHeight), CBRS_BOTTOM);
	m_pWndStatus->EnableDockingOnSizeBar(CBRS_ALIGN_ANY);
	DockSizeBar(m_pWndStatus);
	m_pWndStatus->SetMenuID(IDR_STATUS_CONTROLBAR);
	m_pWndStatus->ShowWindow(SW_HIDE);

	// update the player status dialog's hint trace level
	CPlayerStatusDialog::SetHintTraceLevel(theApp.GetValue(tnAutoHintTraceLevel));


	// Define the image list to use with the tab control
//	m_pWndStatus.CreateImageList( IDB_IL_TAB, 16, RGB(0,128,128));

	//
	// create the players' analyses windows
	//
/*
	CEasyBDoc* pDoc = CEasyBDoc::GetDoc();
	for(int i=0;i<4;i++) 
	{
		m_pDlgAnalysis[i] = new CAnalysisDialog;
		m_pDlgAnalysis[i]->m_nPlayer = i;
		m_pDlgAnalysis[i]->m_pFont = &m_analysisFont;
		m_pDlgAnalysis[i]->m_rect = m_rectAnalysisDialogs[i];
		if ((m_pDlgAnalysis[i]->m_rect.right < 0) || (m_pDlgAnalysis[i]->m_rect.bottom < 0)) 
		{
			m_pDlgAnalysis[i]->m_rect.left = defDrawPoint[i].x + nAnalysisDlgOffset[i].x;
			m_pDlgAnalysis[i]->m_rect.top = defDrawPoint[i].y + nAnalysisDlgOffset[i].y;
			m_pDlgAnalysis[i]->m_rect.right = m_pDlgAnalysis[i]->m_rect.left+nAnalysisSize[i].x;
			m_pDlgAnalysis[i]->m_rect.bottom = m_pDlgAnalysis[i]->m_rect.top+nAnalysisSize[i].y;
		}
		m_pDlgAnalysis[i]->Create(CAnalysisDialog::IDD, this);
		if (m_bAnalysisDialogActive[i])
//			m_pDlgAnalysis[i]->ShowWindow(SW_SHOW);
			PostMessage(WM_COMMAND, ID_SHOW_ANALYSIS_SOUTH+i);	// delay showing dialog
		else
			m_pDlgAnalysis[i]->ShowWindow(SW_HIDE);
	}
*/

	// create the auto hint dialog
	m_pAutoHintDialog = new CAutoHintDialog;
	m_pAutoHintDialog->m_rect = m_rectAutoHintDialog;
	m_pAutoHintDialog->Create(CAutoHintDialog::IDD, this);
//	m_pAutoHintDialog->ShowWindow((theApp.GetValue(tnAutoHintMode) > 0)? SW_SHOW : SW_HIDE);
	m_pAutoHintDialog->ShowWindow(SW_HIDE);
	m_pAutoHintDialog->UpdateWindow();

	// create and position the file comments window
	m_pFileCommentsDlg = new CFileCommentsDialog;
	m_pFileCommentsDlg->m_rect = m_rectCommentsDialog;
	m_pFileCommentsDlg->Create(CFileCommentsDialog::IDD,this);
	m_pFileCommentsDlg->ShowWindow(SW_HIDE);
	m_pFileCommentsDlg->UpdateWindow();

	// create the game review dialog
	CRect rect;
	m_pGameReviewDlg = new CGameReviewDialog;
	m_pGameReviewDlg->Create(CGameReviewDialog::IDD, this);
	m_pGameReviewDlg->GetWindowRect(&rect);
	m_pGameReviewDlg->MoveWindow(m_pointGameReviewDialog.x, 
								 m_pointGameReviewDialog.y,
								 rect.Width(), rect.Height(), FALSE);
	m_pGameReviewDlg->ShowWindow(SW_HIDE);
	m_pGameReviewDlg->UpdateWindow();

	// create the Neural Net output Dialog
	m_pNNetOutputDialog = new CNNetOutputDialog;
	m_pNNetOutputDialog->Create(CNNetOutputDialog::IDD, this);
	m_pNNetOutputDialog->GetWindowRect(rect);
	m_pNNetOutputDialog->MoveWindow(m_pointNNetOutputDialog.x, 
									m_pointNNetOutputDialog.y,
									rect.Width(), rect.Height(), FALSE);
	m_pNNetOutputDialog->ShowWindow(SW_HIDE);
	m_pNNetOutputDialog->UpdateWindow();

	// delete certain menu items if not in debug mode
#ifndef _DEBUG
	GetMenu()->DeleteMenu(ID_FILE_PRINT_PREVIEW, MF_BYCOMMAND);
	GetMenu()->DeleteMenu(ID_VIEW_STATUS_BAR, MF_BYCOMMAND);
	GetMenu()->DeleteMenu(IDR_TEST_TOOLBAR, MF_BYCOMMAND);
	GetMenu()->DeleteMenu(ID_GAME_AUTO_TEST, MF_BYCOMMAND);
//	GetMenu()->DeleteMenu(ID_PLAY_MODE_LOCK, MF_BYCOMMAND);
//	GetMenu()->DeleteMenu(ID_SHOW_COMMENT_IDENTIFIERS, MF_BYCOMMAND);
	GetMenu()->DeleteMenu(8, MF_BYPOSITION);
#endif

	// read in saved control bar settings
	LoadBarState(szDockingRegistryKey);

	// whew! we're finally done
	// if this is teh first time running, show the config wizard
	if (theApp.GetValue(tbFirstTimeRunning))
	{
		PostMessage(WM_COMMAND, ID_PROG_CONFIG_WIZARD, 0);
	}
	else
	{
		// else show the daily tip dialog
		if (theApp.GetValue(tbShowDailyTipDialog))
			PostMessage(WM_COMMAND, ID_HELP_TIP_OF_THE_DAY);
	}

	// done
	return 0;
}



/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CCJFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CCJFrameWnd::Dump(dc);
}

#endif //_DEBUG




/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


//
BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int nCode = lParam;

	switch (wParam) 
	{
		case WMS_F1DOWN:
			// F1 pressed somewhere
/*
			if (nCode == MSGF_DIALOGBOX) {
				CWnd* pWnd = GetActiveWindow();
				if (pWnd)
					pWnd->SendMessage(IDHELP);
			}
*/
			return TRUE;	
	}
	//	
	return CCJFrameWnd::OnCommand(wParam, lParam);
}



//
BOOL CMainFrame::OnBarCheck(UINT nID)
{
	// call base class
	BOOL bCode = CCJFrameWnd::OnBarCheck(nID);

	// reposition bid dialog if active
	CBidDialog* pBidDlg = pMAINFRAME->GetBidDialog();
	if (pBidDlg->IsWindowVisible())
	{
		pBidDlg->RepositionWindow();
		pBidDlg->UpdateWindow();
	}
	
	// set flags
	if (m_pWndStatus->IsWindowVisible())
		m_bStatusDialogActive = TRUE;
	else
		m_bStatusDialogActive = FALSE;

	if (m_pWndHistory->IsWindowVisible())
		m_bHistoryDialogActive = TRUE;
	else
		m_bHistoryDialogActive = FALSE;

	//
	return bCode;
}


//
void CMainFrame::OnSwapCards() 
{
	// translate the swap button to a valid command
	PostMessage(WM_COMMAND, m_nDefaultCardSwapOperation, 0);
}





//
//===========================================================
//
// Misc operations
//



//
CWnd* CMainFrame::GetDialog(int nWindow, int nIndex)
{
	switch (nWindow)
	{
		case twLayoutDialog:
			return m_pLayoutDlg;
		case twStatusDialog:
			return m_pWndStatus;
		case twFileCommentsDialog:
			return m_pFileCommentsDlg;
		case twHistoryDialog:
			return m_pWndHistory;
/*
		case twBiddingHistoryDialog:
			return m_pWndBiddingHistory;
		case twPlayHistoryDialog:
			return m_pWndPlayHistory;
*/
		case twBidDialog:
			return m_pBidDlg;
		case twAutoHintDialog:
			return m_pAutoHintDialog;
		case twScoreDialog:
//			return m_pScoreDialog;
		case twAnalysisDialog:
//			return m_pDlgAnalysis[nIndex];
		case twGameReviewDialog:
			return m_pGameReviewDlg;
		case twNNetOutputDialog:
			return m_pNNetOutputDialog;
		default:
			AfxMessageBox("Unknown index in call to CMainFrame::GetDialog()", MB_ICONHAND);
			break;
	}
	return NULL;
}


//
// call this function to show a mini-frame dialog
// it will be created if necessary
//
void CMainFrame::MakeDialogVisible(int nWindow, int nIndex)
{
	switch (nWindow)
	{
		case twStatusDialog:
			if (!m_pWndStatus->IsWindowVisible())
			{
				SendMessage(WM_COMMAND, ID_SHOW_STATUS);
				UpdateWindow();
			}
			break;
		case twFileCommentsDialog:
			m_pFileCommentsDlg->ShowWindow(SW_SHOW);
			break;
		case twHistoryDialog:
			if (!m_pWndHistory->IsWindowVisible())
			{
				SendMessage(WM_COMMAND, ID_VIEW_HISTORY);
				UpdateWindow();
			}
			break;
		case twBiddingHistoryDialog:
//			if (!m_pWndBiddingHistory->IsWindowVisible())
			if (!m_pWndHistory->IsWindowVisible())
			{
//				SendMessage(WM_COMMAND, ID_VIEW_BIDDING_HISTORY);
				SendMessage(WM_COMMAND, ID_VIEW_HISTORY);
				m_pWndHistory->UpdateWindow();
			}
			m_pWndHistory->ShowBiddingHistory();
			break;
		case twPlayHistoryDialog:
//			if (!m_pWndPlayHistory->IsWindowVisible())
			if (!m_pWndHistory->IsWindowVisible())
			{
//				SendMessage(WM_COMMAND, ID_VIEW_PLAY_HISTORY);
				SendMessage(WM_COMMAND, ID_VIEW_HISTORY);
				m_pWndHistory->UpdateWindow();
			}
			m_pWndHistory->ShowPlayHistory();
			break;
		case twBidDialog:
			m_pBidDlg->ShowWindow(SW_SHOW);
			break;
		case twAutoHintDialog:
			m_pAutoHintDialog->ShowWindow(SW_SHOW);
			break;
		case twScoreDialog:
//			m_pScoreDialog->ShowWindow(SW_SHOW);
			break;
		case twAnalysisDialog:
			if (!m_pWndStatus->IsWindowVisible())
				SendMessage(WM_COMMAND, ID_SHOW_STATUS);
			CStatusAnalysesPage::m_pPage->ShowAnalysis(nIndex);
			m_pWndStatus->SetActiveTab(CStatusWnd::SP_ANALYSES);
			m_bAnalysisDialogActive[nIndex] = TRUE;
			break;
		case twGameReviewDialog:
			m_pGameReviewDlg->ShowWindow(SW_SHOW);
			break;
		case twNNetOutputDialog:
			m_pNNetOutputDialog->ShowWindow(SW_SHOW);
			break;
		default:
			AfxMessageBox("Unknown index in call to CMainFrame::MakeDialogVisible()", MB_ICONHAND);
			break;
	}
	return;
}


//
// call this function to enable or disable a dialog
//
void CMainFrame::EnableDialog(int nWindow, BOOL bEnable, int nIndex)
{
	switch (nWindow)
	{
		case twStatusDialog:
			m_pWndStatus->EnableWindow(bEnable);
			break;
		case twFileCommentsDialog:
			m_pFileCommentsDlg->EnableWindow(bEnable);
			break;
		case twHistoryDialog:
			m_pWndHistory->EnableWindow(bEnable);
			break;
		case twBiddingHistoryDialog:
//			m_pWndBiddingHistory->EnableWindow(bEnable);
			m_pWndHistory->EnableWindow(bEnable);
			break;
		case twPlayHistoryDialog:
//			m_pWndPlayHistory->EnableWindow(bEnable);
			m_pWndHistory->EnableWindow(bEnable);
			break;
		case twBidDialog:
			m_pBidDlg->EnableWindow(bEnable);
			break;
		case twAutoHintDialog:
			m_pAutoHintDialog->EnableWindow(bEnable);
			break;
		case twScoreDialog:
//			m_pScoreDialog->EnableWindow(bEnable);
			break;
		case twAnalysisDialog:
			CStatusAnalysesPage::m_pPage->EnableWindow(bEnable);
			break;
		case twGameReviewDialog:
			m_pGameReviewDlg->EnableWindow(bEnable);
			break;
		case twNNetOutputDialog:
			m_pNNetOutputDialog->EnableWindow(bEnable);
			break;
		default:
			AfxMessageBox("Unknown index in call to CMainFrame::EnableDialog()", MB_ICONHAND);
			break;
	}
	return;
}


//
// call this function to hide a mini-frame dialog
//
void CMainFrame::HideDialog(int nWindow, int nIndex)
{
	switch (nWindow)
	{
		case twStatusDialog:
			if (m_pWndStatus->IsWindowVisible())
			{
				SendMessage(WM_COMMAND, ID_SHOW_STATUS);
				UpdateWindow();
			}
			break;
		case twFileCommentsDialog:
			m_pFileCommentsDlg->ShowWindow(SW_HIDE);
			m_pFileCommentsDlg->UpdateWindow();
			break;
		case twHistoryDialog:
			if (m_pWndHistory->IsWindowVisible())
			{
				SendMessage(WM_COMMAND, ID_VIEW_HISTORY);
				UpdateWindow();
			}
			break;

		case twBiddingHistoryDialog:
//			if (m_pWndBiddingHistory->IsWindowVisible())
			if (m_pWndHistory->IsWindowVisible())
				m_pWndHistory->ShowBiddingHistory(FALSE);
			break;

		case twPlayHistoryDialog:
//			if (m_pWndPlayHistory->IsWindowVisible())
			if (m_pWndHistory->IsWindowVisible())
				m_pWndHistory->ShowPlayHistory(FALSE);
			break;

		case twBidDialog:
			m_pBidDlg->ShowWindow(SW_HIDE);
			m_pBidDlg->UpdateWindow();
			break;
		case twAutoHintDialog:
			m_pAutoHintDialog->ShowWindow(SW_HIDE);
			m_pBidDlg->UpdateWindow();
			break;
		case twScoreDialog:
//			m_pScoreDialog->ShowWindow(SW_HIDE);
//			m_pScoreDialog->UpdateWindow();
			break;
		case twAnalysisDialog:
			CStatusAnalysesPage::m_pPage->ShowAnalysis(nIndex, FALSE);
			m_bAnalysisDialogActive[nIndex] = FALSE;
			break;
		case twGameReviewDialog:
			m_pGameReviewDlg->ShowWindow(SW_HIDE);
			m_pGameReviewDlg->UpdateWindow();
			break;
		case twNNetOutputDialog:
			m_pNNetOutputDialog->ShowWindow(SW_HIDE);
			m_pNNetOutputDialog->UpdateWindow();
			break;
		default:
			AfxMessageBox("Unknown index in call to CMainFrame::HideDialog()", MB_ICONHAND);
			break;
	}
	return;
}


//
void CMainFrame::InformChildFrameOpened(int nWindow, int nIndex)
{	
	switch (nWindow)
	{
		case twLayoutDialog:
			break;
		case twStatusDialog:
			break;
		case twFileCommentsDialog:
			break;
		case twHistoryDialog:
			break;
/*
		case twBiddingHistoryDialog:
			break;
		case twPlayHistoryDialog:
			break;
*/
		case twBidDialog:
			break;
		case twAutoHintDialog:
			break;
		case twAnalysisDialog:
			m_bAnalysisDialogActive[nIndex] = TRUE;
			break;
		case twNNetOutputDialog:
			break;
		default:
			AfxMessageBox("Unknown index in call to CMainFrame::InformChildFrameOpened()", MB_ICONHAND);
			break;
	}
	return;
}


//
void CMainFrame::InformChildFrameClosed(int nWindow, int nIndex)
{	
	switch (nWindow)
	{
		case twLayoutDialog:
			break;
		case twStatusDialog:
			break;
		case twFileCommentsDialog:
			break;
		case twHistoryDialog:
			break;
/*
		case twBiddingHistoryDialog:
			break;
		case twPlayHistoryDialog:
			break;
*/
		case twBidDialog:
			break;
		case twAutoHintDialog:
			OnGameAutoHint();
			break;
		case twAnalysisDialog:
			m_bAnalysisDialogActive[nIndex] = FALSE;
			break;
		case twNNetOutputDialog:
			break;
		default:
			AfxMessageBox("Unknown index in call to CMainFrame::InformChildFrameClosed()", MB_ICONHAND);
			break;
	}
	return;
}


//
// call this function to find if a dialog is currently visible 
//
BOOL CMainFrame::IsDialogVisible(int nWindow, int nIndex)
{
	switch (nWindow)
	{
		case twStatusDialog:
			return m_pWndStatus->IsWindowVisible();
		case twFileCommentsDialog:
			return m_pFileCommentsDlg->IsWindowVisible();
		case twHistoryDialog:
			return m_pWndHistory->IsWindowVisible();
		case twBiddingHistoryDialog:
//			return m_pWndBiddingHistory->IsWindowVisible();
			return m_pWndHistory->IsWindowVisible();
		case twPlayHistoryDialog:
//			return m_pWndPlayHistory->IsWindowVisible();
			return m_pWndHistory->IsWindowVisible();
		case twBidDialog:
			return m_pBidDlg->IsWindowVisible();
		case twAutoHintDialog:
			return m_pAutoHintDialog->IsWindowVisible();
		case twScoreDialog:
//			return m_pScoreDialog->IsWindowVisible();
			break;
		case twAnalysisDialog:
			return m_pWndStatus->IsTabActive(CStatusWnd::SP_ANALYSES);
		case twGameReviewDialog:
			return m_pGameReviewDlg->IsWindowVisible();
		case twNNetOutputDialog:
			return m_pNNetOutputDialog->IsWindowVisible();
		default:
			AfxMessageBox("Unknown index in call to CMainFrame::IsWindowVisible()", MB_ICONHAND);
			break;
	}
	return FALSE;
}



//
void CMainFrame::HideAllDialogs()
{	
	// save dialog states for possible recovery
	m_bStatusDialogActiveSave = m_bStatusDialogActive;
	m_bHistoryDialogActiveSave = m_bHistoryDialogActive;
	for(int i=0;i<4;i++)
		m_bAnalysisDialogActiveSave[i] = m_bAnalysisDialogActive[i];
	//
	HideDialog(twBidDialog);
	UpdateWindow();
	HideDialog(twAutoHintDialog);
	HideDialog(twStatusDialog);
	HideDialog(twFileCommentsDialog);
	HideDialog(twHistoryDialog);
//	HideDialog(twBiddingHistoryDialog);
//	HideDialog(twPlayHistoryDialog);
	HideDialog(twNNetOutputDialog);
//	HideDialog(twScoreDialog);
//	for(i=0;i<4;i++) 
//		HideDialog(twAnalysisDialog, i);
	HideDialog(twGameReviewDialog);
}



//
void CMainFrame::RestoreAllDialogs()
{
	// restore all the dialogs that were previously active
	if (m_bStatusDialogActiveSave)
		MakeDialogVisible(twStatusDialog);
//	if (m_bCommentsDialogActive)
//		MakeDialogVisible(twFileCommentsDialog);
	if (m_bHistoryDialogActiveSave)
		MakeDialogVisible(twHistoryDialog);
//	if (m_bBHDialogActive)
//		MakeDialogVisible(twBiddingHistoryDialog);
//	if (m_bPHDialogActive)
//		MakeDialogVisible(twPlayHistoryDialog);
//	if (m_bBidDialogActive)
//		MakeDialogVisible(twBidDialog);
//	if (m_bNNetOutputDialogActive)
//		MakeDialogVisible(twNNetOutputDialog);
/*
	for(int i=0;i<4;i++)
	{
		if (m_bAnalysisDialogActiveSave[i])
			MakeDialogVisible(twAnalysisDialog,i);
	}
*/
}




//
void CMainFrame::SetNonBoldDialogFont(CDialog* pDialog)
{
	//
	if ((!pDialog) || (theApp.GetValue(tbWin32)))
		return;
	// Send WM_SETFONT message to child controls 
	pDialog->SendMessageToDescendants(WM_SETFONT, (UINT) m_dialogFont.m_hObject, 0L, FALSE); 
}



//
void CMainFrame::DisplayTricks(BOOL bClear)
{
/*
	CString strMessage;
	if ((bClear) || (!theApp.IsGameInProgress())) 
	{
		strMessage = "";
	} 
	else 
	{
		strMessage.Format(" T: N/S %d, E/W %d",
			pDOC->GetValue(tnumTricksWon,0),pDOC->GetValue(tnumTricksWon,1));
	}
	m_pWndStatusBar->SetPaneText(1,strMessage);
	m_pWndStatusBar->UpdateWindow();
*/
}


//
void CMainFrame::DisplayContract(BOOL bClear)
{
	CString strMessage;
	if ((bClear) || (!theApp.IsGameInProgress() && !pDOC->IsReviewingGame())) 
	{
		strMessage = "";
	} 
	else 
	{
		// get bid info
		int nSuit = pDOC->GetContractSuit();
		int nValue = pDOC->GetContractLevel();
		if ((nValue < 1) || (nValue > 7) ||
					(nSuit < CLUBS) || (nSuit > NOTRUMP)) 
		{
			strMessage = "??";
		} 
		else 
		{
/*
			if (nSuit < NOTRUMP)
				strMessage.Format(" C: %d%c ",
						nValue, GetSuitLetter(nSuit));
			else
				strMessage.Format(" C: %dNT ",nValue);
*/
			strMessage = "C: " + ContractToString(pDOC->GetContract(), pDOC->GetContractModifier());
		}
	}
	m_pWndStatusBar->SetPaneText(1,strMessage);
	m_pWndStatusBar->UpdateWindow();
}


//
void CMainFrame::DisplayDeclarer(BOOL bClear)
{
	CString strMessage;
	if ((bClear) || (!theApp.IsGameInProgress() && !pDOC->IsReviewingGame())) 
	{
		strMessage = "";
	} 
	else 
	{
		strMessage.Format("D: %s", PositionToString(pDOC->GetDeclarerPosition()));
	}
	m_pWndStatusBar->SetPaneText(2,strMessage);
	m_pWndStatusBar->UpdateWindow();
}


//
void CMainFrame::DisplayVulnerable(BOOL bClear) 
{
	CString strMessage;
	if (theApp.IsRubberInProgress() || pDOC->IsReviewingGame() || 
				theApp.IsUsingDuplicateScoring())
	{
		switch(pDOC->GetValue(tnVulnerableTeam)) 
		{
			case NEITHER:
				strMessage = "V: None";
				break;
			case NORTH_SOUTH:
				strMessage = "V: N/S";
				break;
			case EAST_WEST:
				strMessage = "V: E/W";
				break;
			case BOTH:
				strMessage = "V: Both";
				break;
		}
	}
	else 
	{
		strMessage = "";
	} 
	m_pWndStatusBar->SetPaneText(3, strMessage);
	m_pWndStatusBar->UpdateWindow();
}



//
void CMainFrame::SetAnalysisText(int nPosition, LPCTSTR szText)
{
	CStatusAnalysesPage::m_pPage->SetAnalysisText(nPosition, szText);
}


//
void CMainFrame::LockStatusBar(BOOL bLock)
{
	if (bLock)
		m_nStatusBarLock++;
	else
		m_nStatusBarLock--;
	if (m_nStatusBarLock < 0)
		m_nStatusBarLock = 0;
}


//
void CMainFrame::SetStatusText(const char* szText, int nPane, BOOL bLockPane) 
{
	if (m_nStatusBarLock > 0)
		return;
	//
	if (nPane == 0)
		m_pWndStatusBar->UnlockFirstPane();	
	m_pWndStatusBar->SetPaneText(nPane,szText);
	m_pWndStatusBar->UpdateWindow();
	//
	if (nPane == 0) 
	{
		if (bLockPane)
			m_pWndStatusBar->LockFirstPane();
		else
			m_pWndStatusBar->UnlockFirstPane();
	}
}

void CMainFrame::SetStatusText(int nIndex, BOOL bLockPane) 
{
	if (m_nStatusBarLock > 0)
		return;
	//
	m_pWndStatusBar->UnlockFirstPane();	
	SetMessageText(nIndex);
    UpdateWindow();
	//
	if (bLockPane)
		m_pWndStatusBar->LockFirstPane();
	else
		m_pWndStatusBar->UnlockFirstPane();
}

void CMainFrame::ClearStatusText(int nPane, BOOL bClearToEmpty) 
{
	if (m_nStatusBarLock > 0)
		return;
	//
	if (nPane == 0) 
	{
		m_pWndStatusBar->UnlockFirstPane();	
		if (bClearToEmpty)
			m_pWndStatusBar->SetPaneText(0,"");
		else
			SetMessageText(AFX_IDS_IDLEMESSAGE);
	} 
	else 
	{
		m_pWndStatusBar->SetPaneText(nPane,"");
		m_pWndStatusBar->UpdateWindow();
	}
}

//
void CMainFrame::SetModeIndicator(LPCTSTR szText)
{
	// use the last pane as a mode indicator
	if (szText == NULL) 
	{
		if (pDOC->GetValue(tbAutoReplayMode))
			m_pWndStatusBar->SetPaneText(4,"Replay");
		else if (theApp.GetValue(tbRubberInProgress))
			m_pWndStatusBar->SetPaneText(4,"Match");
		else if (pDOC->GetValue(tbReviewingGame))
			m_pWndStatusBar->SetPaneText(4,"Review");
		else
			m_pWndStatusBar->SetPaneText(4,"Practice");
	} 
	else 
	{
		m_pWndStatusBar->SetPaneText(4,szText);
	}
	m_pWndStatusBar->UpdateWindow();
}

//
void CMainFrame::ClearModeIndicator()
{
	m_pWndStatusBar->SetPaneText(4,"");
	m_pWndStatusBar->UpdateWindow();
}

//
void CMainFrame::SetAllIndicators()
{
	DisplayTricks();
	DisplayContract();
	DisplayDeclarer();
	DisplayVulnerable();
	SetModeIndicator();
}

void CMainFrame::ClearAllIndicators()
{
	m_pWndStatusBar->UnlockFirstPane();	
	int i;
	for(i=0;i<5;i++)
		m_pWndStatusBar->SetPaneText(i,"");
}

void CMainFrame::UpdateStatusWindow(int nPage, BOOL bShow)
{
	// see if the frame is visible & whether we want it to be
	if (m_pWndStatus) 
	{
		if (nPage == -1)
			nPage = CStatusWnd::SP_ALL;
		m_pWndStatus->Update(nPage);
		if (bShow)
			m_pWndStatus->ShowWindow(SW_SHOW);
	}
}

void CMainFrame::ClearStatusWindow()
{
	if (m_pWndStatus)
	{
		m_pWndStatus->Clear();
		m_pWndStatus->UpdateWindow();
	}
}


void CMainFrame::MakeStatusDialogVisible() 
{
	MakeDialogVisible(twStatusDialog);
}

void CMainFrame::SetFeedbackText(LPCTSTR szText) 
{
	m_strFeedback = szText;
	if (m_pWndStatus)
	{
		m_pWndStatus->SendMessage(WM_COMMAND, WMS_SET_FEEDBACK_TEXT, (long)szText);
		m_pWndStatus->UpdateWindow();
	}
}

void CMainFrame::SetGIBMonitorText(LPCTSTR szText)
{
	m_strGIBMonitor = szText;
	if (m_pWndStatus)
	{
		m_pWndStatus->SendMessage(WM_COMMAND, WMS_SET_GIB_TEXT, (long)(LPCTSTR)m_strGIBMonitor);
		m_pWndStatus->UpdateWindow();
	}
}

void CMainFrame::AppendGIBMonitorText(LPCTSTR szText)
{
	m_strGIBMonitor += szText;
	if (m_pWndStatus)
	{
		m_pWndStatus->SendMessage(WM_COMMAND, WMS_SET_GIB_TEXT, (long)(LPCTSTR)m_strGIBMonitor);
		m_pWndStatus->UpdateWindow();
	}
}

void CMainFrame::SuspendHints()
{
	CPlayerStatusDialog::SuspendHintOutput();
}

void CMainFrame::ResumeHints()
{
	CPlayerStatusDialog::ResumeHintOutput();
}

void CMainFrame::SetBiddingHistory(LPCTSTR szText, BOOL bUseSuitSymbols)
{
	m_strBiddingHistory = szText;
	if (m_pWndHistory)
		m_pWndHistory->SetBiddingHistory(szText);
/*
	if (m_pWndBiddingHistory)
	{
		m_pWndBiddingHistory->SendMessage(WM_COMMAND, WMS_SETTEXT, (long)szText);
		m_pWndBiddingHistory->UpdateWindow();
	}
*/
}


void CMainFrame::SetPlayHistory(LPCTSTR szText, BOOL bUseSuitSymbols)
{
	m_strPlayHistory = szText;
	if (m_pWndHistory)
		m_pWndHistory->SetPlayHistory(szText);
/*
	if (m_pWndPlayHistory)
	{
		m_pWndPlayHistory->SendMessage(WM_COMMAND, WMS_SETTEXT, (long)szText);
		m_pWndPlayHistory->UpdateWindow();
	}
*/
}



//
//==========================================================
//
// static functions to set the status bar message
//

void CMainFrame::SetStatusMessage(LPCTSTR szMessage, int nPane)
{
	pMAINFRAME->SetStatusText(szMessage, nPane, TRUE);
}

void CMainFrame::ClearStatusMessage()
{
	pMAINFRAME->ClearStatusText();
}

void CMainFrame::ResetStatusMessage()
{
	pMAINFRAME->SetStatusText(AFX_IDS_IDLEMESSAGE, FALSE);
}


//
// ClearAutoHints()
//
void CMainFrame::ClearAutoHints()
{
	m_pAutoHintDialog->Clear();
}




//
//==========================================================
//
// CMainFrame menu handlers
//
//




//
//
void CMainFrame::OnPaletteChanged(CWnd* pFocusWnd) 
{
	CCJFrameWnd::OnPaletteChanged(pFocusWnd);
	CView* pView = GetActiveView();
	ASSERT(pView != NULL);
	// notify all child windows that the palette has changed
	SendMessageToDescendants(WMS_DOREALIZE, (WPARAM)pView->m_hWnd);
}


//
BOOL CMainFrame::OnQueryNewPalette() 
{
	// always realize the palette for the active view
	CView* pView = GetActiveView();
	ASSERT(pView != NULL);
	// just notify the target view
	if (pView)
		pVIEW->Notify(WMS_DOREALIZE, (WPARAM)pView->m_hWnd);
	return TRUE;
}


void CMainFrame::OnUpdatePane(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(); 
}


//
void CMainFrame::OnUpdateProgConfigWizard(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

//
void CMainFrame::OnProgConfigWizard() 
{
	// why is the cast necessary? (VC++ won't acceept it otherwise!)
//	CProgramConfigWizard configWizard(&theApp, pDOC, pMAINFRAME, pVIEW, pCurrConvSet);
	CProgramConfigWizard configWizard(&theApp, pDOC, pMAINFRAME, pVIEW, (CObjectWithProperties*)pCurrConvSet);
	configWizard.InitOptions(FALSE);

	//
	if (configWizard.DoModal() == IDOK)
		configWizard.SaveOptions();

	// 
	if (theApp.GetValue(tbFirstTimeRunning))
	{
		// refresh and wait
		UpdateWindow();
		::Sleep(100);
		// show welcome window
		CWelcomeWnd* pWelcomeDialog = new CWelcomeWnd;
		pWelcomeDialog->Create(this);
		// set to show help rules if necesary
		pWelcomeDialog->SetShowRulesHelp((theApp.GetValue(tnAutoHintMode) == 2));
		//
		pWelcomeDialog->ShowWindow(SW_SHOW);
		// and turn off the first time flag
		theApp.SetValue(tbFirstTimeRunning, FALSE);
	}
}

//
void CMainFrame::OnHelp() 
{
	// this gets called after the other handlers (e.g., the
	// currently active window) have been called to handle
	// the help -- so do nothing
//	WinHelp(0, HELP_FINDER);
}

BOOL CMainFrame::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	WinHelp(0, HELP_FINDER);
	return TRUE;	
//	return CCJFrameWnd::OnHelpInfo(pHelpInfo);
}

void CMainFrame::OnHelpContents() 
{
	WinHelp(0, HELP_FINDER);
}

void CMainFrame::OnHelpQuickStart() 
{
	WinHelp(HIDT_QUICK_START);
}

void CMainFrame::OnHelpFaq() 
{
	WinHelp(FAQ_INDEX);
}

void CMainFrame::OnHelpTipOfTheDay() 
{
	if (!m_pDailyTipDialog)
	{
		m_pDailyTipDialog = new CDailyTipDialog;
		m_pDailyTipDialog->Create(this);
	}
	//
	m_pDailyTipDialog->m_bShowAtStartup = theApp.GetValue(tbShowDailyTipDialog);
	m_pDailyTipDialog->UpdateData(FALSE);
	m_pDailyTipDialog->LoadRandomTip();
	m_pDailyTipDialog->ShowWindow(SW_SHOW);
	m_pDailyTipDialog->UpdateWindow();
}

void CMainFrame::OnHelpIntroduction() 
{
	WinHelp(HIDT_INTRODUCTION);
}

void CMainFrame::OnHelpMenus() 
{
	WinHelp(MENU_INDEX);
}

void CMainFrame::OnHelpRules() 
{
	WinHelp(RULES_INDEX);
}

void CMainFrame::OnHelpGlossary() 
{
	WinHelp(GLOSSARY_INDEX);
}

void CMainFrame::OnHelpSearch() 
{
	WinHelp((UINT)"",HELP_PARTIALKEY);
}

void CMainFrame::OnHelpMisc() 
{
	WinHelp(MISC_TOPICS_INDEX);
}

void CMainFrame::OnHelpReadme() 
{
	CString strFile = CString(theApp.GetValueString(tszProgramDirectory)) + _T('\\') + _T("README.Doc");
	ShellExecute(pMAINFRAME->GetSafeHwnd(), _T("open"), strFile, NULL, theApp.GetValueString(tszProgramDirectory), SW_SHOWNORMAL);
}

void CMainFrame::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// check the state of each menu item
//	SendMessage(WM_COMMAND, WMS_QUERY_CAN_UNDO, NULL);	
	// might have been sent by a child window, so confirm cursor pos
	POINT actualPoint;
	::GetCursorPos(&actualPoint);
	// now show the menu
	m_popupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
							   actualPoint.x, 
							   actualPoint.y,
							   this, NULL);
//	CCJFrameWnd::OnRButtonDown(nFlags, point);
}


//
void CMainFrame::OnDisplayOptions() 
{
	CDispOptionsPropSheet dispOptsDialog(&theApp, pMAINFRAME, pVIEW, &deck, this);
	//
	if (dispOptsDialog.DoModal() == IDOK)
	{
		dispOptsDialog.UpdateAllPages();
		if ( dispOptsDialog.m_bGlobalDisplayAffected ||
			 ((dispOptsDialog.m_bDisplayAffected) && pDOC->GetValue(tbHandsDealt)) )
		{
			theApp.InitDummySuitSequence(pDOC->GetTrumpSuit(), pDOC->GetDummyPosition());	
			for(int i=0;i<4;i++)
				PLAYER(i).SortHand();
			pVIEW->Notify(WM_COMMAND, WMS_RESET_DISPLAY, TRUE);
		}
	}
}

//
void CMainFrame::OnUpdateBiddingOptions(CCmdUI* pCmdUI) 
{
	if (theApp.IsBiddingInProgress())
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

//
void CMainFrame::OnBiddingOptions() 
{
	CBidOptionsPropSheet biddingOptsDialog(&theApp, (CObjectWithProperties*)pCurrConvSet, this);
	if (biddingOptsDialog.DoModal() == IDOK)
		biddingOptsDialog.UpdateAllPages();
}


//
void CMainFrame::OnGameOptions() 
{
	CGameOptionsPropSheet gameOptsDlg(&theApp, this);		
	if (gameOptsDlg.DoModal() == IDOK)
		gameOptsDlg.UpdateAllPages();
}

//
void CMainFrame::OnUpdateToggleAnalysisTracing(CCmdUI* pCmdUI) 
{
	if (theApp.GetValue(tbEnableAnalysisTracing))
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

//
void CMainFrame::OnToggleAnalysisTracing() 
{
	BOOL bEnable = !theApp.GetValue(tbEnableAnalysisTracing);
	theApp.SetValue(tbEnableAnalysisTracing, bEnable);
	if (bEnable)
		SetStatusMessage("Computer analysis tracing enabled.");
	else
		SetStatusMessage("Computer analysis tracing disabled.");
}


//
void CMainFrame::OnUpdateDealOptions(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnDealOptions() 
{
	CDealOptionsPropSheet dealDialog(&theApp, this);
	if (dealDialog.DoModal() == IDOK)
		dealDialog.UpdateAllPages();
}


//
void CMainFrame::OnShowAnalysisWest() 
{
	if (IsDialogVisible(twAnalysisDialog, WEST))
		HideDialog(twAnalysisDialog, WEST);
	else
		MakeDialogVisible(twAnalysisDialog, WEST);
}

void CMainFrame::OnShowAnalysisNorth() 
{
	if (IsDialogVisible(twAnalysisDialog, NORTH))
		HideDialog(twAnalysisDialog, NORTH);
	else
		MakeDialogVisible(twAnalysisDialog, NORTH);
}

void CMainFrame::OnShowAnalysisEast() 
{
	if (IsDialogVisible(twAnalysisDialog, EAST))
		HideDialog(twAnalysisDialog, EAST);
	else
		MakeDialogVisible(twAnalysisDialog, EAST);
}

void CMainFrame::OnShowAnalysisSouth() 
{
	if (IsDialogVisible(twAnalysisDialog, SOUTH))
		HideDialog(twAnalysisDialog, SOUTH);
	else
		MakeDialogVisible(twAnalysisDialog, SOUTH);
}

void CMainFrame::OnUpdateShowAnalysis(CCmdUI* pCmdUI) 
{
	if (m_pWndStatus->IsWindowVisible())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
/*
	if (m_pDlgAnalysis[WEST]->IsWindowVisible() && m_pDlgAnalysis[NORTH]->IsWindowVisible() &&
	    m_pDlgAnalysis[EAST]->IsWindowVisible() && m_pDlgAnalysis[SOUTH]->IsWindowVisible())
			pCmdUI->SetCheck(1);
		else
			pCmdUI->SetCheck(0);
	if (pVIEW && pVIEW->GetCurrentMode() == CEasyBView::MODE_CARDLAYOUT)
		pCmdUI->Enable(FALSE);
*/
}

void CMainFrame::OnShowAnalysisAll() 
{
	if (!IsDialogVisible(twAnalysisDialog))
		MakeDialogVisible(twAnalysisDialog);
	else
//		HideDialog(twAnalysisDialog);
		HideDialog(twStatusDialog);

/*
	if ( (!m_pDlgAnalysis[0]->IsWindowVisible()) || (!m_pDlgAnalysis[1]->IsWindowVisible()) ||
		 (!m_pDlgAnalysis[2]->IsWindowVisible()) || (!m_pDlgAnalysis[3]->IsWindowVisible()) ) 
	{
		for(int i=0;i<4;i++)
			MakeDialogVisible(twAnalysisDialog, i);	
	} else 
	{
		for(int i=0;i<4;i++)
			HideDialog(twAnalysisDialog, i);	
	}
*/
}

//
void CMainFrame::OnUpdateShowAnalysisEast(CCmdUI* pCmdUI) 
{
/*
	if (m_pDlgAnalysis[EAST]->IsWindowVisible())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
*/
}
void CMainFrame::OnUpdateShowAnalysisNorth(CCmdUI* pCmdUI) 
{
/*
	if (m_pDlgAnalysis[NORTH]->IsWindowVisible())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
*/
}
void CMainFrame::OnUpdateShowAnalysisSouth(CCmdUI* pCmdUI) 
{
/*
	if (m_pDlgAnalysis[SOUTH]->IsWindowVisible())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
*/
}
void CMainFrame::OnUpdateShowAnalysisWest(CCmdUI* pCmdUI) 
{
/*
	if (m_pDlgAnalysis[WEST]->IsWindowVisible())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
*/
}

//
void CMainFrame::OnUpdateViewFileComments(CCmdUI* pCmdUI) 
{	    
	if (m_pFileCommentsDlg->IsWindowVisible())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
	pCmdUI->Enable(TRUE);
}

//
void CMainFrame::OnViewFileComments() 
{
	if (m_pFileCommentsDlg->IsWindowVisible())
		m_pFileCommentsDlg->ShowWindow(SW_HIDE);
	else
		m_pFileCommentsDlg->ShowWindow(SW_SHOW);
}

//
void CMainFrame::ShowCommentsDialog(BOOL bShow) 
{
	if (bShow)
		m_pFileCommentsDlg->ShowWindow(SW_SHOW);	
	else
		m_pFileCommentsDlg->ShowWindow(SW_HIDE);	
}

//
void CMainFrame::OnUpdateViewNeuralNet(CCmdUI* pCmdUI) 
{
	if (m_pNNetOutputDialog->IsWindowVisible())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
	pCmdUI->Enable(TRUE);
}

//
void CMainFrame::OnViewNeuralNet() 
{
	if (m_pNNetOutputDialog->IsWindowVisible())
		m_pNNetOutputDialog->ShowWindow(SW_HIDE);
	else
		m_pNNetOutputDialog->ShowWindow(SW_SHOW);
}


/*
//
void CMainFrame::OnViewHistory() 
{
	PostMessage(ID_VIEW_HISTORY);
}


//
void CMainFrame::OnUpdateShowStatus(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}
*/


//
void CMainFrame::OnShowStatus() 
{
	if (m_pWndStatus)
	{
		if (m_pWndStatus->IsWindowVisible())
			m_pWndStatus->ShowWindow(SW_HIDE);
		else
			m_pWndStatus->ShowWindow(SW_SHOW);
	}
}


void CMainFrame::OnUpdateExposeAllCards(CCmdUI* pCmdUI) 
{
	BOOL bShowCardsUp = theApp.AreCardsFaceUp();
	pCmdUI->SetCheck(bShowCardsUp);
	if (pVIEW && pVIEW->GetCurrentMode() == CEasyBView::MODE_CARDLAYOUT)
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnExposeAllCards() 
{
	theApp.SetCardsFaceUp(!theApp.AreCardsFaceUp());
}


/*
//
void CMainFrame::OnUpdateDebugMode(CCmdUI* pCmdUI) 
{
	BOOL bDebugMode = !theApp.GetValue(tbDebugMode);
	pCmdUI->SetCheck(bDebugMode? 1 : 0);
	pCmdUI->Enable(TRUE);
}

//
void CMainFrame::OnDebugMode() 
{
	BOOL bDebugMode = !theApp.GetValue(tbDebugMode);
	theApp.SetValue(tbDebugMode, !bDebugMode);
	if (bDebugMode)
		m_bShowCardsFaceUp = TRUE;
}
*/


//
void CMainFrame::OnUpdateGameAutoHint(CCmdUI* pCmdUI) 
{
	int nMode = theApp.GetValue(tnAutoHintMode);
	pCmdUI->SetCheck(nMode? 1 : 0);
	pCmdUI->Enable(TRUE);
}

//
void CMainFrame::OnGameAutoHint() 
{
	int nMode = !theApp.GetValue(tnAutoHintMode);
	theApp.SetValue(tnAutoHintMode, nMode);
	if (nMode)
	{
		// show autohint dialog and hint if necessary
		if (!pDOC->IsHintAvailable())
			pDOC->ShowAutoHint();
	}
	else
	{
		HideDialog(twAutoHintDialog);
	}
}

//
void CMainFrame::OnUpdateTrainingMode(CCmdUI* pCmdUI) 
{
/*	
	pCmdUI->SetCheck(m_bNNetTrainingMode);
	//
	if (theApp.GetValue(tnBiddingEngine) != 0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
*/
}

//
void CMainFrame::OnTrainingMode() 
{
//	theApp.SetValue(tbNNetTrainingMode, !theApp.SetValue(tbNNetTrainingMode));
}


//
void CMainFrame::OnUpdatePlayModeNormal(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((theApp.GetValue(tnCardPlayMode) == CEasyBApp::PLAY_NORMAL)? 1 : 0);
}
//
void CMainFrame::OnUpdatePlayModeManual(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((theApp.GetValue(tnCardPlayMode) == CEasyBApp::PLAY_MANUAL)? 1 : 0);
}
//
void CMainFrame::OnUpdatePlayModeManualDefend(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((theApp.GetValue(tnCardPlayMode) == CEasyBApp::PLAY_MANUAL_DEFEND)? 1 : 0);
}
//
void CMainFrame::OnUpdatePlayModeFullAuto(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((theApp.GetValue(tnCardPlayMode) == CEasyBApp::PLAY_FULL_AUTO)? 1 : 0);
}
//
void CMainFrame::OnUpdatePlayModeLock(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(theApp.GetValue(tbPlayModeLocked));
}

//
void CMainFrame::OnPlayModeNormal() 
{
	theApp.SetValue(tnCardPlayMode, CEasyBApp::PLAY_NORMAL, 1);	// override lock
	if ((theApp.IsGameInProgress()) && (pVIEW->GetCurrentMode() == CEasyBView::MODE_WAITCARDPLAY))
		pVIEW->AdvanceToNextPlayer();
}
//
void CMainFrame::OnPlayModeManual() 
{
	theApp.SetValue(tnCardPlayMode, CEasyBApp::PLAY_MANUAL, 1);
	if ((theApp.IsGameInProgress()) && (pVIEW->GetCurrentMode() == CEasyBView::MODE_WAITCARDPLAY))
		pVIEW->AdvanceToNextPlayer();
}
//
void CMainFrame::OnPlayModeManualDefend() 
{
	theApp.SetValue(tnCardPlayMode, CEasyBApp::PLAY_MANUAL_DEFEND, 1);
	if ((theApp.IsGameInProgress()) && (pVIEW->GetCurrentMode() == CEasyBView::MODE_WAITCARDPLAY))
		pVIEW->AdvanceToNextPlayer();
}
//
void CMainFrame::OnPlayModeFullAuto() 
{
	theApp.SetValue(tnCardPlayMode, CEasyBApp::PLAY_FULL_AUTO, 1);
	HideDialog(twAutoHintDialog);
	if ((theApp.IsGameInProgress()) && 
		(pVIEW->GetCurrentMode() == CEasyBView::MODE_WAITCARDPLAY))
		pVIEW->AdvanceToNextPlayer();
}
//
void CMainFrame::OnPlayModeLock() 
{
	theApp.SetValue(tbPlayModeLocked, !theApp.GetValue(tbPlayModeLocked));
}

//
void CMainFrame::OnUpdateManualBidding(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(theApp.GetValue(tbManualBidding));
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnManualBidding() 
{
	theApp.SetValue(tbManualBidding, !theApp.GetValue(tbManualBidding));
	if (m_pBidDlg && m_pBidDlg->IsWindowVisible())
		m_pBidDlg->EnableManualBidding(theApp.GetValue(tbManualBidding));
}

//
void CMainFrame::OnUpdateManualPlay(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((theApp.GetValue(tnCardPlayMode) == CEasyBApp::PLAY_MANUAL)? 1 : 0);
}

void CMainFrame::OnManualPlay() 
{
	if (theApp.GetValue(tnCardPlayMode) == CEasyBApp::PLAY_MANUAL)
	{
		// is manual, set to normal
		theApp.SetValue(tnCardPlayMode, CEasyBApp::PLAY_NORMAL, 1);
	}
	else
	{
		// is NOT manual, set to manual
		theApp.SetValue(tnCardPlayMode, CEasyBApp::PLAY_MANUAL, 1);
	}
	//
	if ((theApp.IsGameInProgress()) && (pVIEW->GetCurrentMode() == CEasyBView::MODE_WAITCARDPLAY))
		pVIEW->AdvanceToNextPlayer();
}

//
void CMainFrame::OnUpdateShowCommentIdentifiers(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(theApp.GetValue(tbShowCommentIdentifiers));
	pCmdUI->Enable(TRUE);
}

//
void CMainFrame::OnShowCommentIdentifiers() 
{
	theApp.SetValue(tbShowCommentIdentifiers, !theApp.GetValue(tbShowCommentIdentifiers));
}







//
//==============================================================================
//
//
// Property Set/Put operations
//
//
//==============================================================================
//

// 
LPVOID CMainFrame::GetValuePV(int nItem, int nIndex1, int nIndex2, int nIndex3)
{
	switch(nItem)
	{
		// dialog window codes
		case twLayoutDialog:
			return (LPVOID) m_pLayoutDlg;
		case twStatusDialog:
			return (LPVOID) m_pWndStatus;
		case twFileCommentsDialog:
			return (LPVOID) m_pFileCommentsDlg;
		case twBiddingHistoryDialog:
//			return (LPVOID) m_pWndBiddingHistory;
			return (LPVOID) m_pWndHistory;
		case twPlayHistoryDialog:
//			return (LPVOID) m_pWndPlayHistory;
			return (LPVOID) m_pWndHistory;
		case twBidDialog:
			return (LPVOID) m_pBidDlg;
		case twScoreDialog:
//			return (LPVOID) m_pScoreDialog;
		case twAnalysisDialog:
			return (LPVOID) NULL;
//			return (LPVOID) m_pDlgAnalysis[nIndex1];
			// property get/set codes
		case tnAnalysisDialogRectLeft:
			return (LPVOID) m_rectAnalysisDialogs[nIndex1].left;
		case tnAnalysisDialogRectTop:
			return (LPVOID) m_rectAnalysisDialogs[nIndex1].top;
		case tnAnalysisDialogRectRight:
			return (LPVOID) m_rectAnalysisDialogs[nIndex1].right;
		case tnAnalysisDialogRectBottom:
			return (LPVOID) m_rectAnalysisDialogs[nIndex1].bottom;
		case tnHistoryDialogWidth:
			return (LPVOID) m_pointHistoryDialog.x;
		case tnHistoryDialogHeight:
			return (LPVOID) m_pointHistoryDialog.y;
		case tnStatusDialogWidth:
			return (LPVOID) m_pointStatusDialog.x;
		case tnStatusDialogHeight:
			return (LPVOID) m_pointStatusDialog.y;
		case tnReviewDialogPosLeft:
			return (LPVOID) m_pointGameReviewDialog.x;
		case tnReviewDialogPosTop:
			return (LPVOID) m_pointGameReviewDialog.y;
		case tnNNetOutputDialogLeft:
			return (LPVOID) m_pointNNetOutputDialog.x;
		case tnNNetOutputDialogTop:
			return (LPVOID) m_pointNNetOutputDialog.y;
		case tnStatusDialogRectLeft:
			return (LPVOID) m_rectStatusDialog.left;
		case tnStatusDialogRectTop:
			return (LPVOID) m_rectStatusDialog.top;
		case tnStatusDialogRectRight:
			return (LPVOID) m_rectStatusDialog.right;
		case tnStatusDialogRectBottom:
			return (LPVOID) m_rectStatusDialog.bottom;
		case tnCommentsDialogRectLeft:
			return (LPVOID) m_rectCommentsDialog.left;
		case tnCommentsDialogRectTop:
			return (LPVOID) m_rectCommentsDialog.top;
		case tnCommentsDialogRectRight:
			return (LPVOID) m_rectCommentsDialog.right;
		case tnCommentsDialogRectBottom:
			return (LPVOID) m_rectCommentsDialog.bottom;
		case tnAutoHintDialogRectLeft:
			return (LPVOID) m_rectAutoHintDialog.left;
		case tnAutoHintDialogRectTop:
			return (LPVOID) m_rectAutoHintDialog.top;
		case tnAutoHintDialogRectRight:
			return (LPVOID) m_rectAutoHintDialog.right;
		case tnAutoHintDialogRectBottom:
			return (LPVOID) m_rectAutoHintDialog.bottom;
		case tbAnalysisDialogActive:
			return (LPVOID) m_bAnalysisDialogActive[nIndex1];
		//
		case tnDockingWndWidth:
		case tnDockingWndHeight:
			break;
		//
		case tszFeedbackText:
			return (LPVOID) (LPCTSTR) m_strFeedback;
		case tszGIBMonitorText:
			return (LPVOID) (LPCTSTR) m_strGIBMonitor;
		//
		case tpFontFixed:
			return (LPVOID) &m_fixedFont;
		case tpFontStandard:
			return (LPVOID) &m_standardFont;
		case tpFontSmall:
			return (LPVOID) &m_smallFont;
		case tpFontLarge:
			return (LPVOID) &m_largeFont;
		case tpFontAnalysis:
			return (LPVOID) &m_analysisFont;
		case tpFontHistory:
			return (LPVOID) &m_historyFont;
		case tpFontAutoHint:
			return (LPVOID) &m_autoHintFont;
		case tpFontDialog:
			return (LPVOID) &m_dialogFont;
		case tpFontDialogBold:
			return (LPVOID) &m_dialogBoldFont;
		//
		case tpLogFontAnalysis:
			return (LPVOID) &m_lfAnalysis;
		case tpLogFontHistory:
			return (LPVOID) &m_lfHistory;
		case tpLogFontAutoHint:
			return (LPVOID) &m_lfAutoHint;
		// sys caps
		case tnSysScreenWidth:
			return (LPVOID) m_nSysScreenWidth;
		case tnSysScreenHeight:
			return (LPVOID) m_nSysScreenHeight;
		case tnSysNumColors:
			return (LPVOID) m_nSysNumColors;
		case tnSysColorPlanes:
			return (LPVOID) m_nSysColorPlanes;
		case tnSysBitsPerPixel:
			return (LPVOID) m_nSysBitsPerPixel;
		case tbSysRCDIBitmap:
			return (LPVOID) m_bSysRCDIBitmap;
		case tbSysRCDIBtoDev:
			return (LPVOID) m_bSysRCDIBtoDev;
		case tnSysClipCaps:
			return (LPVOID) m_nSysClipCaps;
		case tnSysRasterCaps:
			return (LPVOID) m_nSysRasterCaps;
		//
		case tbHighResDisplay:
			return (LPVOID) m_bHighResDisplay;
		case tnDefaultWindowWidth:
			return (LPVOID) tnDefaultWidth;
		case tnDefaultWindowHeight:
			return (LPVOID) tnDefaultHeight;
		//
		default:
			AfxMessageBox("Unhandled Call to CMainFrame::GetValuePV()");
			return NULL;
	}
	return NULL;
}


//
int CMainFrame::SetValuePV(int nItem, LPVOID value, int nIndex1, int nIndex2, int nIndex3)
{
	int nVal = (int) value;
	LPCTSTR szVal = (LPCTSTR) value;
	//
	switch(nItem)
	{
		// dialog window codes
		case twLayoutDialog:
		case twStatusDialog:
		case twFileCommentsDialog:
		case twBiddingHistoryDialog:
		case twPlayHistoryDialog:
		case twBidDialog:
		case twScoreDialog:
		case twAnalysisDialog:
			break;
			// property get/set codes
		case tnAnalysisDialogRectLeft:
			m_rectAnalysisDialogs[nIndex1].left = nVal;
			break;
		case tnAnalysisDialogRectTop:
			m_rectAnalysisDialogs[nIndex1].top = nVal;
			break;
		case tnAnalysisDialogRectRight:
			m_rectAnalysisDialogs[nIndex1].right = nVal;
			break;
		case tnAnalysisDialogRectBottom:
			m_rectAnalysisDialogs[nIndex1].bottom = nVal;
			break;
		case tnHistoryDialogWidth:
			m_pointHistoryDialog.x = nVal;
			break;
		case tnHistoryDialogHeight:
			m_pointHistoryDialog.y = nVal;
			break;
		case tnStatusDialogWidth:
			m_pointStatusDialog.x = nVal;
			break;
		case tnStatusDialogHeight:
			m_pointStatusDialog.y = nVal;
			break;
		case tnReviewDialogPosLeft:
			m_pointGameReviewDialog.x = nVal;
			break;
		case tnReviewDialogPosTop:
			m_pointGameReviewDialog.y = nVal;
			break;
		case tnNNetOutputDialogLeft:
			m_pointNNetOutputDialog.x = nVal;
			break;
		case tnNNetOutputDialogTop:
			m_pointNNetOutputDialog.y = nVal;
			break;
		case tnStatusDialogRectLeft:
			m_rectStatusDialog.left = nVal;
			break;
		case tnStatusDialogRectTop:
			m_rectStatusDialog.top = nVal;
			break;
		case tnStatusDialogRectRight:
			m_rectStatusDialog.right = nVal;
			break;
		case tnStatusDialogRectBottom:
			m_rectStatusDialog.bottom = nVal;
			break;
		case tnCommentsDialogRectLeft:
			m_rectCommentsDialog.left = nVal;
			break;
		case tnCommentsDialogRectTop:
			m_rectCommentsDialog.top = nVal;
			break;
		case tnCommentsDialogRectRight:
			m_rectCommentsDialog.right = nVal;
			break;
		case tnCommentsDialogRectBottom:
			m_rectCommentsDialog.bottom = nVal;
			break;
		case tnAutoHintDialogRectLeft:
			m_rectAutoHintDialog.left = nVal;
			break;
		case tnAutoHintDialogRectTop:
			m_rectAutoHintDialog.top = nVal;
			break;
		case tnAutoHintDialogRectRight:
			m_rectAutoHintDialog.right = nVal;
			break;
		case tnAutoHintDialogRectBottom:
			m_rectAutoHintDialog.bottom = nVal;
			break;
		case tbAnalysisDialogActive:
			break;
		//
		case tnDockingWndWidth:
			m_nDockingWndWidth = nVal;
			break;
		case tnDockingWndHeight:
			m_nDockingWndHeight = nVal;
			break;
		//
		case tszFeedbackText:
			m_strFeedback = szVal;
			break;
		case tszGIBMonitorText:
			m_strGIBMonitor = szVal;
			break;
		//
		case tpFontFixed:
		case tpFontStandard:
		case tpFontSmall:
		case tpFontLarge:
		case tpFontAnalysis:
		case tpFontHistory:
		case tpFontDialog:
		case tpFontDialogBold:
			break;
		//
		case tpLogFontAnalysis:
			SetFont(tpLogFontAnalysis, *((LOGFONT*)value));
			break;
		case tpLogFontHistory:
			SetFont(tpLogFontHistory, *((LOGFONT*)value));
			break;
		case tpLogFontAutoHint:
			SetFont(tpLogFontAutoHint, *((LOGFONT*)value));
			break;
		// sys caps
		case tnSysScreenWidth:
		case tnSysScreenHeight:
		case tnSysNumColors:
		case tnSysColorPlanes:
		case tnSysBitsPerPixel:
		case tbSysRCDIBitmap:
		case tbSysRCDIBtoDev:
		case tnSysClipCaps:
		case tnSysRasterCaps:
			break;
		//
		case tbHighResDisplay:
		case tnDefaultWindowWidth:
		case tnDefaultWindowHeight:
			break;
		//
		default:
			AfxMessageBox("Unhandled Call to CMainFrame::SetValuePV()");
			return 1;
	}
	return NULL;
}

// conversion functions
LPCTSTR CMainFrame::GetValueString(int nItem, int nIndex1, int nIndex2, int nIndex3)
{
	return (LPCTSTR) GetValue(nItem, nIndex1, nIndex2, nIndex3);
}

int CMainFrame::GetValue(int nItem, int nIndex1, int nIndex2, int nIndex3)
{
	return (int) GetValuePV(nItem, nIndex1, nIndex2, nIndex3);
}

int CMainFrame::SetValue(int nItem, LPCTSTR szValue, int nIndex1, int nIndex2, int nIndex3)
{
	return SetValuePV(nItem, (LPVOID) szValue, nIndex1, nIndex2, nIndex3);
}

int CMainFrame::SetValue(int nItem, int nValue, int nIndex1, int nIndex2, int nIndex3)
{
	return SetValuePV(nItem, (LPVOID)nValue, nIndex1, nIndex2, nIndex3);
}


//
int CMainFrame::SetFont(int nFont, LOGFONT& lf)
{
	// switch to new font
	// but first delete old font
	CFont* pFont = NULL;
	switch (nFont)
	{
		case tpLogFontAnalysis:
			pFont = &m_analysisFont;
			break;

		case tpLogFontHistory:
			pFont = &m_historyFont;
			break;

		case tpLogFontAutoHint:
			pFont = &m_autoHintFont;
			break;
	}
	//
	if (!pFont)
		return -1;

	// delete old cfont object and create with new lf
	pFont->DeleteObject();
	if (!pFont->CreateFontIndirect(&lf)) 
		return -1;	// oops!

	// now do some additional processing
	// copy logical fonts into our own member variables
	switch(nFont)
	{
		case tpLogFontAnalysis:
		{
			CWaitCursor wait;
			m_lfAnalysis = lf;
//			for(int i=0;i<4;i++)
//				GetDialog(twAnalysisDialog,i)->PostMessage(WM_COMMAND,WMS_SET_ANALYSIS_FONT,0L);
			break;
		}

		case tpLogFontHistory:
			m_lfHistory = lf;
			break;

		case tpLogFontAutoHint:
			m_lfAutoHint = lf;
			break;
	}

	// done
	return 0;
}


