//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// ProgramConfigWizard.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ProgramConfigWizard.h"
#include "ProgConfigWizardData.h"
#include "ProgConfigIntroPage.h"
#include "ProgConfigHelpLevelPage.h"
#include "ProgConfigViewSettingsPage.h"
#include "ProgConfigSuitsDisplayPage.h"
#include "ProgConfigGameMechanicsPage.h"
#include "ProgConfigPausesPage.h"
#include "ProgConfigBiddingPage.h"
#include "ProgConfigFinishPage.h"
#include "ObjectWithProperties.h"
#include "progopts.h"
#include "ConvCodes.h"
#include "HelpCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgramConfigWizard

IMPLEMENT_DYNAMIC(CProgramConfigWizard, CPropertySheet)

CProgramConfigWizard::CProgramConfigWizard(CObjectWithProperties* pApp, CObjectWithProperties* pDoc, CObjectWithProperties* pFrame, 
										   CObjectWithProperties* pView,  CObjectWithProperties* pConventionSet,
										   CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(_T("Program Configuration Wizard"), pParentWnd, iSelectPage),
	  m_app(*pApp), m_doc(*pDoc), m_frame(*pFrame), m_view(*pView), m_conventionSet(*pConventionSet)
{
	// create the data obejct
	m_pData				= new CProgConfigWizardData;

	// create the pages
	m_pIntroPage		= new CProgConfigIntroPage(this);
	m_pHelpLevelPage	= new CProgConfigHelpLevelPage(this);
	m_pViewPage			= new CProgConfigViewSettingsPage(this);
	m_pSuitsPage		= new CProgConfigSuitsDisplayPage(this);
	m_pMechanicsPage	= new CProgConfigGameMechanicsPage(this);
	m_pPausesPage		= new CProgConfigPausesPage(this);
	m_pBiddingPage		= new CProgConfigBiddingPage(this);
	m_pFinishPage		= new CProgConfigFinishPage(this);

	// add them to the property sheet
	AddPage(m_pIntroPage);
	AddPage(m_pHelpLevelPage);
	AddPage(m_pViewPage);
	AddPage(m_pSuitsPage);
	AddPage(m_pMechanicsPage);
	AddPage(m_pPausesPage);
	AddPage(m_pBiddingPage);
	AddPage(m_pFinishPage);

	// and we're off
	SetWizardMode();
}

CProgramConfigWizard::~CProgramConfigWizard()
{
	// delete the data object
	delete m_pData;

	// then delete all pages
	delete m_pIntroPage;
	delete m_pHelpLevelPage;
	delete m_pViewPage;
	delete m_pSuitsPage;
	delete m_pMechanicsPage;
	delete m_pBiddingPage;
	delete m_pPausesPage;
	delete m_pFinishPage;
}


BEGIN_MESSAGE_MAP(CProgramConfigWizard, CPropertySheet)
	//{{AFX_MSG_MAP(CProgramConfigWizard)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgramConfigWizard message handlers



//
// InitOptions()
//
void CProgramConfigWizard::InitOptions(BOOL bFirstTime)
{
	//
	// load or set primary optiosn
	//

	// Help Level Page
	m_pData->m_nHelpLevel = 2 - m_app.GetValue(tnAutoHintMode);
	if (m_pData->m_nHelpLevel < 0)
		m_pData->m_nHelpLevel = 0;
	if (m_pData->m_nHelpLevel > 2)
		m_pData->m_nHelpLevel = 2;
	m_pData->m_bShowAnalysis = m_app.GetValue(tbEnableAnalysisTracing);

	// View Page
	m_pData->m_bShowBidHistory = m_app.GetValue(tbAutoShowBidHistory);
	m_pData->m_bShowPlayHistory = m_app.GetValue(tbAutoShowPlayHistory);
	m_pData->m_bHideBanner = !m_app.GetValue(tbShowSplashWindow);
	m_pData->m_bDisableBitmap = !m_app.GetValue(tbShowBackgroundBitmap);
	m_pData->m_bUseSuitSymbols = m_app.GetValue(tbUseSuitSymbols);

	// Game Mechanics Page
	m_pData->m_bScoreDuplicate = m_app.GetValue(tbUsingDuplicateScoring);
	m_pData->m_bIgnoreHonorsBonuses = !m_app.GetValue(tbScoreHonorsBonuses);

	// Suits Display Page
	m_pData->m_nSuitDisplayOrder = m_app.GetValue(tnSuitSequenceOption);
	m_pData->m_bShowDummyTrumpsOnLeft = m_app.GetValue(tbShowDummyTrumpsOnLeft);

	// Bidding Pause Page
	if (m_app.GetValue(tbInsertBiddingPause))
		m_pData->m_nBiddingPauseSetting = 1;
	else
		m_pData->m_nBiddingPauseSetting = 0;
	//
	if (m_app.GetValue(tbInsertPlayPause))
		m_pData->m_nPlayPauseSetting = 1;
	else
		m_pData->m_nPlayPauseSetting = 0;

	// Bidding Preference Page
	m_pData->m_nBiddingPreference = bFirstTime? 0 : m_conventionSet.GetValue(tnBiddingStyle);
}




//
// SaveOptions()
//
void CProgramConfigWizard::SaveOptions()
{
	//
	// save primary optiosn
	//

	// Help Level Page
	m_app.SetValue(tnAutoHintMode, -(m_pData->m_nHelpLevel - 2));
	m_app.SetValue(tbEnableAnalysisTracing, m_pData->m_bShowAnalysis);

	// View Page
	m_app.SetValue(tbAutoShowBidHistory, m_pData->m_bShowBidHistory);
	m_app.SetValue(tbAutoShowPlayHistory, m_pData->m_bShowPlayHistory);
	m_app.SetValue(tbShowSplashWindow, !m_pData->m_bHideBanner);
	m_app.SetValue(tbShowBackgroundBitmap, !m_pData->m_bDisableBitmap);
	m_app.SetValue(tbUseSuitSymbols, m_pData->m_bUseSuitSymbols);

	// Game Mechanics Page
	m_app.SetValue(tbUsingDuplicateScoring, m_pData->m_bScoreDuplicate);
	m_app.SetValue(tbScoreHonorsBonuses, !m_pData->m_bIgnoreHonorsBonuses);

	// Suits Display Page
	m_app.SetValue(tnSuitSequenceOption, m_pData->m_nSuitDisplayOrder);
	m_app.SetValue(tbShowDummyTrumpsOnLeft, m_pData->m_bShowDummyTrumpsOnLeft);

	// Pauses Page
	switch(m_pData->m_nBiddingPauseSetting)
	{
		case 0:
			m_app.SetValue(tbInsertBiddingPause, FALSE);
			m_app.SetValue(tnBiddingPauseLength, 0);
			break;
		case 1:
			m_app.SetValue(tbInsertBiddingPause, TRUE);
			m_app.SetValue(tnBiddingPauseLength, 3);
			break;
		case 2:
			m_app.SetValue(tbInsertBiddingPause, TRUE);
			m_app.SetValue(tnBiddingPauseLength, 5);
			break;
		case 3:
			m_app.SetValue(tbInsertBiddingPause, TRUE);
			m_app.SetValue(tnBiddingPauseLength, 10);
			break;
		case 4:
			m_app.SetValue(tbInsertBiddingPause, TRUE);
			m_app.SetValue(tnBiddingPauseLength, 20);
			break;
	}
	//
	switch(m_pData->m_nPlayPauseSetting)
	{
		case 0:
			m_app.SetValue(tbInsertPlayPause, FALSE);
			m_app.SetValue(tnPlayPauseLength, 0);
			break;
		case 1:
			m_app.SetValue(tbInsertPlayPause, TRUE);
			m_app.SetValue(tnPlayPauseLength, 3);
			break;
		case 2:
			m_app.SetValue(tbInsertPlayPause, TRUE);
			m_app.SetValue(tnPlayPauseLength, 5);
			break;
		case 3:
			m_app.SetValue(tbInsertPlayPause, TRUE);
			m_app.SetValue(tnPlayPauseLength, 10);
			break;
		case 4:
			m_app.SetValue(tbInsertPlayPause, TRUE);
			m_app.SetValue(tnPlayPauseLength, 20);
			break;
	}

	//
	// save biding options
	//
	m_conventionSet.SetValue(tnBiddingStyle, m_pData->m_nBiddingPreference);
	switch (m_pData->m_nBiddingPreference)
	{
		// 0 - no conventions
		case 0:
		{
			m_pData->m_bFiveCardMajors			  = FALSE;
			m_pData->m_bArtificial2ClubConvention = TRUE;	// always on!
			m_pData->m_bWeakTwoBids				  = FALSE;
			m_pData->m_bShutoutBids				  = FALSE;
			m_pData->m_bLimitRaises				  = FALSE;
			m_pData->m_bSplinterBids			  = FALSE;
			m_pData->m_bJacoby2NT				  = FALSE;
			m_pData->m_bUnusualNoTrump			= FALSE;
			m_pData->m_bMichaels				  = FALSE;
			m_pData->m_bGambling3NT				  = FALSE;
			m_pData->m_nGambling3NTVersion		  = 0;
			m_pData->m_bDrury					  = FALSE;
			m_pData->m_bTakeoutDoubles			  = FALSE;
			m_pData->m_bNegativeDoubles			  = FALSE;
			m_pData->m_n2ClubOpenRange			  = 3;	// 23+ for 2C Open
			m_pData->m_nNTRange[0]				  = 2;	// 16-18 1NT
			m_pData->m_nNTRange[1]				  = 2;	// 22-24 2NT
			m_pData->m_nNTRange[2]				  = 0;	// 25-27 3NT
			m_pData->m_bStayman					  = FALSE;
			m_pData->m_bJacobyTransfers			  = FALSE;
			m_pData->m_b4SuitTransfers			  = FALSE;
			m_pData->m_b4thSuitForcing			  = FALSE;
			m_pData->m_bStructuredReverses		  = FALSE;
			m_pData->m_bWeakJumpOvercalls		  = FALSE;
			m_pData->m_bBlackwood				  = FALSE;
			m_pData->m_bRKCB					  = FALSE;
			m_pData->m_bCueBids					  = FALSE;
			m_pData->m_bGerber					  = FALSE;
			//
//			m_pData->m_bOgust					  = FALSE;
//			m_pData->m_nAllowable1Openings		  = 0;
			break;
		}

		// 1- Goren
		case 1:
		{
			m_pData->m_bFiveCardMajors			  = FALSE;
			m_pData->m_bArtificial2ClubConvention = TRUE;
			m_pData->m_bWeakTwoBids				  = FALSE;
			m_pData->m_bShutoutBids				  = TRUE;
			m_pData->m_bLimitRaises				  = FALSE;
			m_pData->m_bSplinterBids			  = FALSE;
			m_pData->m_bJacoby2NT				  = FALSE;
			m_pData->m_bMichaels				  = FALSE;
			m_pData->m_bGambling3NT				  = FALSE;
			m_pData->m_nGambling3NTVersion		  = 0;
			m_pData->m_bDrury					  = FALSE;
			m_pData->m_bUnusualNoTrump			  = FALSE;
			m_pData->m_bTakeoutDoubles			  = TRUE;
			m_pData->m_bNegativeDoubles			  = FALSE;
			m_pData->m_n2ClubOpenRange			  = 3;	// 23+ pts for 2C Open
			m_pData->m_nNTRange[0]				  = 2;	// 16-18 1NT
			m_pData->m_nNTRange[1]				  = 2;	// 22-24 2NT
			m_pData->m_nNTRange[2]				  = 0;	// 25-27 3NT
			m_pData->m_bStayman					  = TRUE;
			m_pData->m_bJacobyTransfers			  = FALSE;
			m_pData->m_b4SuitTransfers			  = FALSE;
			m_pData->m_b4thSuitForcing			  = FALSE;
			m_pData->m_bStructuredReverses		  = FALSE;
			m_pData->m_bWeakJumpOvercalls		  = FALSE;
			m_pData->m_bBlackwood				  = TRUE;
			m_pData->m_bRKCB					  = FALSE;
			m_pData->m_bCueBids					  = TRUE;
			m_pData->m_bGerber					  = TRUE;
			//
//			m_pData->m_bOgust					  = FALSE;
//			m_pData->m_nAllowable1Openings		  = 0;
			break;
		}

		// 2 - Standard American
		case 2:
		{
			m_pData->m_bFiveCardMajors			  = TRUE;
			m_pData->m_bArtificial2ClubConvention = TRUE;
			m_pData->m_bWeakTwoBids				  = TRUE;
			m_pData->m_bShutoutBids				  = TRUE;
			m_pData->m_bLimitRaises				  = FALSE;
			m_pData->m_bSplinterBids			  = FALSE;
			m_pData->m_bJacoby2NT				  = FALSE;
			m_pData->m_bUnusualNoTrump			  = FALSE;
			m_pData->m_bMichaels				  = FALSE;
			m_pData->m_bGambling3NT				  = FALSE;
			m_pData->m_nGambling3NTVersion		  = 0;
			m_pData->m_bDrury					  = FALSE;
			m_pData->m_bTakeoutDoubles			  = TRUE;
			m_pData->m_bNegativeDoubles			  = TRUE;
			m_pData->m_n2ClubOpenRange			  = 2;	// 22+ pts 2C Open
			m_pData->m_nNTRange[0]				  = 1;	// 15-17 1NT
			m_pData->m_nNTRange[1]				  = 1;	// 21-22 2NT
			m_pData->m_nNTRange[2]				  = 1;	// don't open 3NT
			m_pData->m_bStayman					  = TRUE;
			m_pData->m_bJacobyTransfers			  = TRUE;
			m_pData->m_b4SuitTransfers			  = FALSE;
			m_pData->m_b4thSuitForcing			  = FALSE;
			m_pData->m_bStructuredReverses		  = TRUE;
			m_pData->m_bWeakJumpOvercalls		  = TRUE;
			m_pData->m_bBlackwood				  = TRUE;
			m_pData->m_bRKCB					  = FALSE;
			m_pData->m_bCueBids					  = TRUE;
			m_pData->m_bGerber					  = TRUE;
			//
//			m_pData->m_bOgust					  = FALSE;
//			m_pData->m_nAllowable1Openings		  = 0;
			break;
		}

		// 3 - Standard American + Options
		case 3:
		{
			m_pData->m_bFiveCardMajors			  = TRUE;
			m_pData->m_bArtificial2ClubConvention = TRUE;
			m_pData->m_bWeakTwoBids				  = TRUE;
			m_pData->m_bShutoutBids				  = TRUE;
			m_pData->m_bLimitRaises				  = TRUE;
			m_pData->m_bSplinterBids			  = TRUE;
			m_pData->m_bJacoby2NT				  = TRUE;
			m_pData->m_bUnusualNoTrump			  = TRUE;
			m_pData->m_bMichaels				  = TRUE;
			m_pData->m_bGambling3NT				  = TRUE;
			m_pData->m_nGambling3NTVersion		  = 1;	// ACOL Gambling
			m_pData->m_bDrury					  = TRUE;
			m_pData->m_bTakeoutDoubles			  = TRUE;
			m_pData->m_bNegativeDoubles			  = TRUE;
			m_pData->m_n2ClubOpenRange			  = 2;	// 22+ pts 2C Open
			m_pData->m_nNTRange[0]				  = 1;	// 15-17 1NT
			m_pData->m_nNTRange[1]				  = 1;	// 21-22 2NT
			m_pData->m_nNTRange[2]				  = 1;	// don't open 3NT
			m_pData->m_bStayman					  = TRUE;
			m_pData->m_bJacobyTransfers			  = TRUE;
			m_pData->m_b4SuitTransfers			  = TRUE;
			m_pData->m_b4thSuitForcing			  = TRUE;
			m_pData->m_bStructuredReverses		  = TRUE;
			m_pData->m_bWeakJumpOvercalls		  = TRUE;
			m_pData->m_bBlackwood				  = TRUE;
			m_pData->m_bRKCB					  = FALSE;
			m_pData->m_bCueBids					  = TRUE;
			m_pData->m_bGerber					  = TRUE;
			//
//			m_pData->m_bOgust					  = FALSE;
//			m_pData->m_nAllowable1Openings		  = 0;
			break;
		}

		// 4 - ACOL
		case 4:
		{
			m_pData->m_bFiveCardMajors			  = FALSE;
			m_pData->m_bArtificial2ClubConvention = TRUE;
			m_pData->m_bWeakTwoBids				  = FALSE;
			m_pData->m_bShutoutBids				  = TRUE;
			m_pData->m_bLimitRaises				  = FALSE;
			m_pData->m_bSplinterBids			  = TRUE; 
			m_pData->m_bJacoby2NT				  = FALSE;
			m_pData->m_bUnusualNoTrump			  = TRUE;
			m_pData->m_bMichaels				  = TRUE;
			m_pData->m_bGambling3NT				  = TRUE;
			m_pData->m_nGambling3NTVersion		  = 2;	// ACOL Gambling
			m_pData->m_bDrury					  = TRUE;
			m_pData->m_bTakeoutDoubles			  = TRUE;
			m_pData->m_bNegativeDoubles			  = FALSE;
			m_pData->m_n2ClubOpenRange			  = 3;	// 23+ pts for 2C Open
			m_pData->m_nNTRange[0]				  = 0;	// 12-14 1NT
			m_pData->m_nNTRange[1]				  = 2;	// 22-24 2NT
			m_pData->m_nNTRange[2]				  = 0;	// 25-27 3NT
			m_pData->m_bStayman					  = TRUE;
			m_pData->m_bJacobyTransfers			  = FALSE;
			m_pData->m_b4SuitTransfers			  = FALSE;
			m_pData->m_b4thSuitForcing			  = TRUE;
			m_pData->m_bStructuredReverses		  = FALSE;
			m_pData->m_bWeakJumpOvercalls		  = FALSE;
			m_pData->m_bBlackwood				  = TRUE;
			m_pData->m_bRKCB					  = FALSE;
			m_pData->m_bCueBids					  = TRUE;
			m_pData->m_bGerber					  = TRUE;
			//
//			m_pData->m_bOgust					  = FALSE;
//			m_pData->m_nAllowable1Openings		  = 0;
			break;
		}
	}

	// and save
	m_conventionSet.SetValue(tid5CardMajors, m_pData->m_bFiveCardMajors);
	m_conventionSet.SetValue(tidArtificial2ClubConvention, m_pData->m_bArtificial2ClubConvention);
	m_conventionSet.SetValue(tidWeakTwoBids, m_pData->m_bWeakTwoBids);
	m_conventionSet.SetValue(tidShutoutBids, m_pData->m_bShutoutBids);
	m_conventionSet.SetValue(tidLimitRaises, m_pData->m_bLimitRaises);
	m_conventionSet.SetValue(tidSplinterBids, m_pData->m_bSplinterBids);
	m_conventionSet.SetValue(tidJacoby2NT, m_pData->m_bJacoby2NT);
	m_conventionSet.SetValue(tidUnusualNT, m_pData->m_bUnusualNoTrump);
	m_conventionSet.SetValue(tidMichaels, m_pData->m_bMichaels);
	m_conventionSet.SetValue(tidGambling3NT, m_pData->m_bGambling3NT);
	m_conventionSet.SetValue(tnGambling3NTVersion, m_pData->m_nGambling3NTVersion);
	m_conventionSet.SetValue(tidTakeoutDoubles, m_pData->m_bTakeoutDoubles);
	m_conventionSet.SetValue(tidNegativeDoubles, m_pData->m_bNegativeDoubles);
	m_conventionSet.SetValue(tn2ClubOpenRange, m_pData->m_n2ClubOpenRange);
	m_conventionSet.SetValue(tn1NTRange, m_pData->m_nNTRange[0]);
	m_conventionSet.SetValue(tn2NTRange, m_pData->m_nNTRange[1]);
	m_conventionSet.SetValue(tn3NTRange, m_pData->m_nNTRange[2]);
	m_conventionSet.SetValue(tidStayman, m_pData->m_bStayman);
	m_conventionSet.SetValue(tidJacobyTransfers, m_pData->m_bJacobyTransfers);
	m_conventionSet.SetValue(tb4SuitTransfers, m_pData->m_b4SuitTransfers);
	m_conventionSet.SetValue(tid4thSuitForcing, m_pData->m_b4thSuitForcing);
	m_conventionSet.SetValue(tbStructuredReverses, m_pData->m_bStructuredReverses);
	m_conventionSet.SetValue(tidWeakJumpOvercalls, m_pData->m_bWeakJumpOvercalls);
	m_conventionSet.SetValue(tidBlackwood, m_pData->m_bBlackwood);
	m_conventionSet.SetValue(tidGerber, m_pData->m_bGerber);
	m_conventionSet.SetValue(tidRKCB, m_pData->m_bRKCB);
	m_conventionSet.SetValue(tidCueBids, m_pData->m_bCueBids);
	//
//	m_conventionSet.SetValue(tidOgust, m_pData->m_bOgust);
//	m_conventionSet.SetValue(tnAllowable1Openings, m_pData->m_nAllowable1Openings);
}




//
void CProgramConfigWizard::OnHelp()
{
	int nPageIndex = GetActiveIndex();
	int nHelpContextID = WIZARD_PROG_CONFIG_WELCOME_PAGE + nPageIndex;
	WinHelp(nHelpContextID);
}

//
BOOL CProgramConfigWizard::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;	
}

