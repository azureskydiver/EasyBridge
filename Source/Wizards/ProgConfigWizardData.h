//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// ProgConfigWizardData.h: interface for the CProgConfigWizardData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROGCONFIGWIZARDDATA_H__26420ED3_D7F7_11D2_9097_00609777FAF1__INCLUDED_)
#define AFX_PROGCONFIGWIZARDDATA_H__26420ED3_D7F7_11D2_9097_00609777FAF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// the Program Config Wizard Data class
//

//
class CProgConfigWizardData  
{
// data
public:
	// Help Level Page
	int		m_nHelpLevel;
	BOOL	m_bShowAnalysis;

	// View Page
	BOOL	m_bShowBidHistory;
	BOOL	m_bShowPlayHistory;
	BOOL	m_bHideBanner;
	BOOL	m_bDisableBitmap;
	BOOL	m_bUseSuitSymbols;

	// Game Mechanics Page
	BOOL	m_bScoreDuplicate;
	BOOL	m_bIgnoreHonorsBonuses;

	// Suits Display Page
	int		m_nSuitDisplayOrder;
	BOOL	m_bShowDummyTrumpsOnLeft;

	// Pauses Page
	int		m_nBiddingPauseSetting;
	int		m_nPlayPauseSetting;

	// Bidding Preference Page
	int		m_nBiddingPreference;
	//
	BOOL	m_bFiveCardMajors;
	BOOL	m_bArtificial2ClubConvention;
	BOOL	m_bWeakTwoBids;
	BOOL	m_bShutoutBids;
	BOOL	m_bOgust;
	BOOL	m_bLimitRaises;
	BOOL	m_bSplinterBids;
	BOOL	m_bJacoby2NT;
	BOOL	m_bUnusualNoTrump;
	BOOL	m_bMichaels;
	BOOL	m_bGambling3NT;
	int		m_nGambling3NTVersion;
	BOOL	m_bDrury;
	BOOL	m_bTakeoutDoubles;
	BOOL	m_bNegativeDoubles;
	int		m_nAllowable1Openings;
	int	    m_n2ClubOpenRange;
	int		m_nNTRange[3];			
	BOOL	m_bStayman;
	BOOL	m_bJacobyTransfers;
	BOOL 	m_b4SuitTransfers;
	BOOL	m_b4thSuitForcing;
	BOOL	m_bStructuredReverses;
	BOOL	m_bWeakJumpOvercalls;
	BOOL	m_bBlackwood;
	BOOL	m_bRKCB;
	BOOL	m_bCueBids;
	BOOL	m_bGerber;


// operations
public:
	void	Clear();


// construction / destruction
public:
	CProgConfigWizardData();
	virtual ~CProgConfigWizardData();
};

#endif // !defined(AFX_PROGCONFIGWIZARDDATA_H__26420ED3_D7F7_11D2_9097_00609777FAF1__INCLUDED_)
