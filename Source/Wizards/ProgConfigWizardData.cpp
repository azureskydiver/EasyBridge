//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// ProgConfigWizardData.cpp: implementation of the CProgConfigWizardData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProgConfigWizardData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProgConfigWizardData::CProgConfigWizardData()
{
	Clear();
}

CProgConfigWizardData::~CProgConfigWizardData()
{
}


//////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////



//
// Clear()
//
void CProgConfigWizardData::Clear()
{
	m_nHelpLevel				= FALSE;
	m_bShowBidHistory			= FALSE;
	m_bShowPlayHistory			= FALSE;
	m_bShowDummyTrumpsOnLeft	= FALSE;

	// bidding related settings
	m_bFiveCardMajors				= FALSE;
	m_bArtificial2ClubConvention	= FALSE;
	m_bWeakTwoBids					= FALSE;
	m_bShutoutBids					= FALSE;
	m_bOgust						= FALSE;
	m_bLimitRaises					= FALSE;
	m_bSplinterBids					= FALSE;
	m_bJacoby2NT					= FALSE;
	m_bUnusualNoTrump				= FALSE;
	m_bGambling3NT					= FALSE;
	m_bMichaels						= FALSE;
	m_nGambling3NTVersion			= 0;
	m_bDrury						= FALSE;
	m_bTakeoutDoubles				= FALSE;
	m_bNegativeDoubles				= FALSE;
	m_nAllowable1Openings			= FALSE;
	m_n2ClubOpenRange				= FALSE;
	m_nNTRange[3]					= FALSE;
	m_bStayman						= FALSE;
	m_bJacobyTransfers				= FALSE;
	m_b4SuitTransfers				= FALSE;
	m_b4thSuitForcing				= FALSE;
	m_bStructuredReverses			= FALSE;
	m_bWeakJumpOvercalls			= FALSE;
	m_bBlackwood					= FALSE;
	m_bRKCB							= FALSE;
	m_bCueBids						= FALSE;
	m_bGerber						= FALSE;
}
