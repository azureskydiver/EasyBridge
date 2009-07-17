//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------
/* NCR Changes:
	19 Apr 09 - Make WeakTwo and Artifical2Club the same
	30 Apr 09 - Added Roman Key Card Blackwood flag read
*/

//
// ConventionSet.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "4thSuitForcingConvention.h"
#include "StrongTwoBidsConvention.h"
#include "WeakTwoBidsConvention.h"
#include "ShutoutBidsConvention.h"
#include "Artificial2ClubConvention.h"
#include "StaymanConvention.h"
#include "JacobyTransferConvention.h"
#include "BlackwoodConvention.h"
#include "GerberConvention.h"
#include "CueBidConvention.h"
#include "SplinterBidsConvention.h"
#include "MichaelsCueBidConvention.h"
#include "UnusualNTConvention.h"
#include "Jacoby2NTConvention.h"
#include "Gambling3NTConvention.h"
#include "DruryConvention.h"
#include "NegativeDoublesConvention.h"
#include "TakeoutDoublesConvention.h"
#include "OvercallsConvention.h"
#include "ConventionSet.h"
#include "Bidparams.h"
#include "CardLocation.h"
#include "GuessedHandHoldings.h"



// default bidding settings
MINMAX	tnNTRangeTable[3][3] = { 
	{{ 12,14 }, { 15,17 }, { 16,18 }},
	{{ 20,21 }, { 21,22 }, { 22,24 }},
//	{{ 25,27 }, { -1,-1 }, { -1,-1 }}
	{{ 25,27 }, { 99,99 }, { 99,99 }}
};
// default bidding settings
int tn2ClubOpenTable[4] = { 20, 21, 22, 23, };

// NCR-177 Minimum pts for Strong Two opening
int tnStrongTwoPts = 21;


// central repository for global convention objects
// only one copy of each convention object exists
//CNaturalConvention			naturalConvention;
//CNoTrumpConvention			noTrumpConvention;
//CFiveCardMajorsConvention	fiveCardMajorsConvention;
CStrongTwoBidsConvention	strongTwoBidsConvention;
CWeakTwoBidsConvention		weakTwoBidsConvention;
CShutoutBidsConvention		shutoutBidsConvention;
CArtificial2ClubConvention	artificial2ClubConvention;
C4thSuitForcingConvention	fourthSuitForcingConvention;
CStaymanConvention			staymanConvention;
CJacobyTransferConvention	jacobyTransferConvention;
CBlackwoodConvention		blackwoodConvention;
CGerberConvention			gerberConvention;
CCueBidConvention			cueBidConvention;
CSplinterBidsConvention		splinterBidsConvention;
CMichaelsCueBidConvention	michaelsCueBidConvention;
CJacoby2NTConvention		jacoby2NTConvention;
CUnusualNTConvention		unusualNTConvention;
CGambling3NTConvention		gambling3NTConvention;
CDruryConvention			druryConvention;
COvercallsConvention		overcallsConvention;
CNegativeDoublesConvention	negativeDoublesConvention;
CTakeoutDoublesConvention	takeoutDoublesConvention;




//
//==================================================================
//
// Main Operations
//
	

//
// operator=()
//
// Assignment operator
//
CConventionSet& CConventionSet::operator=(CConventionSet& src)
{
	// do a shallow copy of the convention objects
	m_listConventions.RemoveAll();
	POSITION pos = src.m_listConventions.GetHeadPosition();
	while(pos)
		m_listConventions.AddTail(src.m_listConventions.GetNext(pos));
	
	// and set the opter variables
	m_strName						= src.m_strName;
	m_strDescription				= src.m_strDescription;
	m_bFiveCardMajors				= src.m_bFiveCardMajors;
	m_bArtificial2ClubConvention	= src.m_bArtificial2ClubConvention;
	m_bWeakTwoBids					= src.m_bWeakTwoBids;
	m_bShutoutBids					= src.m_bShutoutBids;
	m_bOgust						= src.m_bOgust;
	m_bLimitRaises					= src.m_bLimitRaises;
	m_bSplinterBids					= src.m_bSplinterBids;
	m_bMichaels						= src.m_bMichaels;
	m_bUnusualNT					= src.m_bUnusualNT;
	m_bJacoby2NT					= src.m_bJacoby2NT;
	m_bGambling3NT					= src.m_bGambling3NT;
	m_nGambling3NTVersion			= src.m_nGambling3NTVersion;
	m_bDrury						= src.m_bDrury;
	m_bLebensohl					= src.m_bLebensohl;
	m_bDONT							= src.m_bDONT;
	m_bTakeoutDoubles				= src.m_bTakeoutDoubles;
	m_bNegativeDoubles				= src.m_bNegativeDoubles;
	m_nAllowable1Openings			= src.m_nAllowable1Openings;
	m_n2ClubOpenRange				= src.m_n2ClubOpenRange;
	m_nNTRange[0]					= src.m_nNTRange[0];
	m_nNTRange[1]					= src.m_nNTRange[1];
	m_nNTRange[2]					= src.m_nNTRange[2];
	m_bStayman						= src.m_bStayman;
	m_bJacobyTransfers				= src.m_bJacobyTransfers;
	m_b4SuitTransfers				= src.m_b4SuitTransfers;
	m_b4thSuitForcing				= src.m_b4thSuitForcing;
	m_bStructuredReverses			= src.m_bStructuredReverses;
	m_b3LevelTakeouts				= src.m_b3LevelTakeouts;
	m_bWeakJumpOvercalls			= src.m_bWeakJumpOvercalls;
	m_bBlackwood					= src.m_bBlackwood;
	m_bRKCB							= src.m_bRKCB;
	m_bCueBids						= src.m_bCueBids;
	m_bGerber						= src.m_bGerber;

	// done
	return *this;
}
	
	

	
//
// ApplyConventionTests()
//
BOOL CConventionSet::ApplyConventionTests(const CPlayer& player, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status) 
{
	if (m_listConventions.IsEmpty()) 
		return FALSE;
	//
	POSITION pos = m_listConventions.GetHeadPosition();
	CConvention* pConvention = m_listConventions.GetNext(pos);
	while(pConvention)
	{
		if (pConvention->ApplyTest(player, *this, hand, cardLocation, ppGuessedHands, bidState, status)) { // NCR-SCU
#ifdef _DEBUG
			CEasyBDoc::m_pDoc->AddSCU(pConvention->m_nID); // NCR-SCU add this convention's id
#endif
			return TRUE;
		} // end NCR-SCU
		if (pos)
			pConvention = m_listConventions.GetNext(pos);
		else
			pConvention = NULL;
	}
	//
	return FALSE;
}



// 
// ClearState()
//
// clears the internal state of all conventions in use
//
void CConventionSet::ClearState()
{
	if (m_listConventions.IsEmpty()) 
		return;
	//
	POSITION pos = m_listConventions.GetHeadPosition();
	CConvention* pConvention = m_listConventions.GetNext(pos);
	while(pConvention)
	{
		pConvention->ClearState();
		pConvention = m_listConventions.GetNext(pos);
	}
}



//
// GetConvention()
//
CConvention* CConventionSet::GetNextConvention(POSITION& pos)
{
	if (this == NULL)	// just in case
		return NULL;
	if (pos == NULL)
		pos = m_listConventions.GetHeadPosition();
	if (pos)
		return m_listConventions.GetNext(pos);
	else
		return NULL;
}

//
//==================================================================
//
// constructon & destruction
CConventionSet::CConventionSet() 
{
}

CConventionSet::~CConventionSet() 
{
}



//
//==================================================================
//
// bidding convention registry strings
//
static TCHAR BASED_CODE szBiddingOptions[] = _T("Bidding Conventions");
static TCHAR BASED_CODE szBiddingStyle[] = _T("Bidding Style");
static TCHAR BASED_CODE szBlackwood[] = _T("Blackwood");
static TCHAR BASED_CODE szRKCB[] = _T("Roman Key Card Blackwood"); //NCR
static TCHAR BASED_CODE szCueBids[] = _T("Cue Bids");
static TCHAR BASED_CODE szGerber[] = _T("Gerber");
static TCHAR BASED_CODE sz1NTRange[] = _T("Range of Points for 1NT Bid (0:12-14, 1:15-17, 2:16-18)");
static TCHAR BASED_CODE sz2NTRange[] = _T("Range of Points for 2NT Bid (0:21-22, 1:22-24)");
static TCHAR BASED_CODE sz3NTRange[] = _T("Range of Points for 3NT Bid (0:25-27, 1:Bid 2C Instead)");
static TCHAR BASED_CODE szFiveCardMajors[] = _T("Five-Card Majors");
static TCHAR BASED_CODE sz2ClubConvention[] = _T("Two Club Opening Convention");
static TCHAR BASED_CODE szWeakTwoBids[] = _T("Weak Two Bids");
static TCHAR BASED_CODE szWeakJumpOvercalls[] = _T("Jump Overcalls Mode (0:Strong, 1:Weak)");
static TCHAR BASED_CODE szShutoutBids[] = _T("Shutout 3 and 4 Bids");
static TCHAR BASED_CODE szOgustConvention[] = _T("Ogust Convention");
static TCHAR BASED_CODE szLimitRaises[] = _T("Limit Raises");
static TCHAR BASED_CODE szTakeoutDoubles[] = _T("Takeout Doubles");
static TCHAR BASED_CODE szNegativeDoubles[] = _T("Negative Doubles");
static TCHAR BASED_CODE szSplinters[] = _T("Splinter Bids");
static TCHAR BASED_CODE szMichaels[] = _T("Michaels Cue Bids");
static TCHAR BASED_CODE szUnusualNT[] = _T("Unusual No Trump");
static TCHAR BASED_CODE szJacoby2NT[] = _T("Jacoby 2No Trump");
static TCHAR BASED_CODE szGambling3NT[] = _T("Gambling 3NT");
static TCHAR BASED_CODE szGambling3NTVersion[] = _T("Gambling 3NT Version (0=Standard, 1=ACOL)");
static TCHAR BASED_CODE szDrury[] = _T("Drury");
static TCHAR BASED_CODE szLebensohl[] = _T("Lebensohl");
static TCHAR BASED_CODE szDONT[] = _T("DONT (Double Over No Trump)");
static TCHAR BASED_CODE szStayman[] = _T("Stayman");
static TCHAR BASED_CODE szJacobyTransfers[] = _T("Jacoby Transfers");
static TCHAR BASED_CODE sz4SuitTransfers[] = _T("Four-Suit Transfers");
static TCHAR BASED_CODE szStructuredReverses[] = _T("Structured Reverses");
static TCHAR BASED_CODE sz3LevelTakeouts[] = _T("3-Level Takeouts");
static TCHAR BASED_CODE szAllowable1Openings[] = _T("Allowable Holdings for 1-Level Opening Bidding");
static TCHAR BASED_CODE sz2ClubOpenRange[] = _T("Artificial 2 Club Opening Range (0=20+, 1=21+, 2=22+, 3=23+)");
static TCHAR BASED_CODE sz4thSuitForcing[] = _T("4th Suit is Forcing");
		


//
// Initialize(()
//
// reads in the settings for this convention
//
void CConventionSet::Initialize(LPCTSTR szName) 
{
	if (szName)
		m_strName = szName;
	CString strSettingsKey = szBiddingOptions + m_strName;

	// read in bidding conventions settings
	m_nBiddingStyle = theApp.GetProfileInt(strSettingsKey, szBiddingStyle, 2);
	//
	m_bFiveCardMajors = theApp.GetProfileInt(strSettingsKey, szFiveCardMajors, 1);
//NCR	m_bArtificial2ClubConvention = theApp.GetProfileInt(strSettingsKey, sz2ClubConvention, 1);
	m_bWeakTwoBids = theApp.GetProfileInt(strSettingsKey, szWeakTwoBids, 1);
    m_bArtificial2ClubConvention = m_bWeakTwoBids; // NCR Must be the same
	m_bShutoutBids = theApp.GetProfileInt(strSettingsKey, szShutoutBids, TRUE);
	m_bOgust = FALSE;
	m_bLimitRaises = theApp.GetProfileInt(strSettingsKey, szLimitRaises, FALSE);
	m_bSplinterBids = theApp.GetProfileInt(strSettingsKey, szSplinters, FALSE);
	m_bMichaels = theApp.GetProfileInt(strSettingsKey, szMichaels, FALSE);
	m_bUnusualNT = theApp.GetProfileInt(strSettingsKey, szUnusualNT, FALSE);
	m_bJacoby2NT = theApp.GetProfileInt(strSettingsKey, szJacoby2NT, FALSE);
	m_bGambling3NT = theApp.GetProfileInt(strSettingsKey, szGambling3NT, FALSE);
	m_nGambling3NTVersion = theApp.GetProfileInt(strSettingsKey, szGambling3NTVersion, 0);
	m_bDrury = theApp.GetProfileInt(strSettingsKey, szDrury, FALSE);
	m_bLebensohl = theApp.GetProfileInt(strSettingsKey, szLebensohl, FALSE);
	m_bDONT = theApp.GetProfileInt(strSettingsKey, szDONT, FALSE);
	m_bTakeoutDoubles = theApp.GetProfileInt(strSettingsKey, szTakeoutDoubles, TRUE);
	m_bNegativeDoubles = theApp.GetProfileInt(strSettingsKey, szNegativeDoubles, TRUE);
	m_nAllowable1Openings = theApp.GetProfileInt(strSettingsKey, szAllowable1Openings, 0);
	m_n2ClubOpenRange = theApp.GetProfileInt(strSettingsKey, sz2ClubOpenRange, 0);
	m_nNTRange[0] = theApp.GetProfileInt(strSettingsKey, sz1NTRange, 1);
	m_nNTRange[1] = theApp.GetProfileInt(strSettingsKey, sz2NTRange, 2);
	m_nNTRange[2] = theApp.GetProfileInt(strSettingsKey, sz3NTRange, 0);
	m_bStayman = theApp.GetProfileInt(strSettingsKey, szStayman, TRUE);
	m_bJacobyTransfers = theApp.GetProfileInt(strSettingsKey, szJacobyTransfers, FALSE);
	m_b4SuitTransfers = theApp.GetProfileInt(strSettingsKey, sz4SuitTransfers, TRUE);
	m_b4thSuitForcing = theApp.GetProfileInt(strSettingsKey, sz4thSuitForcing, FALSE);
	m_bStructuredReverses = theApp.GetProfileInt(strSettingsKey, szStructuredReverses, FALSE);
	m_b3LevelTakeouts = theApp.GetProfileInt(strSettingsKey, sz3LevelTakeouts, FALSE);
	m_bWeakJumpOvercalls = theApp.GetProfileInt(strSettingsKey, szWeakJumpOvercalls, 0);
	m_bBlackwood = theApp.GetProfileInt(strSettingsKey, szBlackwood, 1);
	m_bRKCB = theApp.GetProfileInt(strSettingsKey, szRKCB, 0); // NCR
	m_bCueBids = theApp.GetProfileInt(strSettingsKey, szCueBids, TRUE);
	m_bGerber = theApp.GetProfileInt(strSettingsKey, szGerber, FALSE);
}



//
// Terminate()
//
// write out settings for this convention set
//
void CConventionSet::Terminate() 
{
	CString strSettingsKey = szBiddingOptions + m_strName;

	// write out current bidding conventions settings
	theApp.WriteProfileInt(strSettingsKey, szBiddingStyle, m_nBiddingStyle);
	//
	theApp.WriteProfileInt(strSettingsKey, szFiveCardMajors, m_bFiveCardMajors);
//NCR	theApp.WriteProfileInt(strSettingsKey, sz2ClubConvention, m_bArtificial2ClubConvention);
	theApp.WriteProfileInt(strSettingsKey, sz2ClubConvention, m_bWeakTwoBids); //NCR make same
	theApp.WriteProfileInt(strSettingsKey, szWeakTwoBids, m_bWeakTwoBids);
	theApp.WriteProfileInt(strSettingsKey, szShutoutBids, m_bShutoutBids);
	theApp.WriteProfileInt(strSettingsKey, szOgustConvention, m_bOgust);
	theApp.WriteProfileInt(strSettingsKey, szLimitRaises, m_bLimitRaises);
	theApp.WriteProfileInt(strSettingsKey, szSplinters, m_bSplinterBids);
	theApp.WriteProfileInt(strSettingsKey, szMichaels, m_bMichaels);
	theApp.WriteProfileInt(strSettingsKey, szUnusualNT, m_bUnusualNT);
	theApp.WriteProfileInt(strSettingsKey, szJacoby2NT, m_bJacoby2NT);
	theApp.WriteProfileInt(strSettingsKey, szGambling3NT, m_bGambling3NT);
	theApp.WriteProfileInt(strSettingsKey, szGambling3NTVersion, m_nGambling3NTVersion);
	theApp.WriteProfileInt(strSettingsKey, szDrury, m_bDrury);
	theApp.WriteProfileInt(strSettingsKey, szLebensohl, m_bLebensohl);
	theApp.WriteProfileInt(strSettingsKey, szDONT, m_bDONT);
	theApp.WriteProfileInt(strSettingsKey, szTakeoutDoubles, m_bTakeoutDoubles);
	theApp.WriteProfileInt(strSettingsKey, szNegativeDoubles, m_bNegativeDoubles);
	theApp.WriteProfileInt(strSettingsKey, szAllowable1Openings, m_nAllowable1Openings);
	theApp.WriteProfileInt(strSettingsKey, sz2ClubOpenRange, m_n2ClubOpenRange);
	theApp.WriteProfileInt(strSettingsKey, sz1NTRange, m_nNTRange[0]);
	theApp.WriteProfileInt(strSettingsKey, sz2NTRange, m_nNTRange[1]);
	theApp.WriteProfileInt(strSettingsKey, sz3NTRange, m_nNTRange[2]);
	theApp.WriteProfileInt(strSettingsKey, szStayman, m_bStayman);
	theApp.WriteProfileInt(strSettingsKey, szJacobyTransfers, m_bJacobyTransfers);
	theApp.WriteProfileInt(strSettingsKey, sz4SuitTransfers, m_b4SuitTransfers);
	theApp.WriteProfileInt(strSettingsKey, sz4thSuitForcing, m_b4thSuitForcing);
	theApp.WriteProfileInt(strSettingsKey, szStructuredReverses, m_bStructuredReverses);
	theApp.WriteProfileInt(strSettingsKey, sz3LevelTakeouts, m_b3LevelTakeouts);
	theApp.WriteProfileInt(strSettingsKey, szWeakJumpOvercalls, m_bWeakJumpOvercalls);
	theApp.WriteProfileInt(strSettingsKey, szBlackwood, m_bBlackwood);
	theApp.WriteProfileInt(strSettingsKey, szCueBids, m_bCueBids);
	theApp.WriteProfileInt(strSettingsKey, szGerber, m_bGerber);
}




// 
// InitCommonSettings()
//
// static function to initialize data common to all convention sets
//
void CConventionSet::InitCommonSettings()
{
}



//
// InitConventions()
//
// gather up all the conventions used in this convention set and put
// pointers to them in the list 
//
void CConventionSet::InitConventions() 
{
	// clear existing set
//	while(m_listConventions.GetHead())
	m_listConventions.RemoveAll();

	//
	// add the conventions to the list
	// each convention will be applied in the order in which it is
	// placed in the list
	//

	// in general, put the most esoteric conventions first
	if (m_bCueBids)
		m_listConventions.AddTail(&cueBidConvention);
	//
	if (m_bBlackwood && !m_bRKCB)
		m_listConventions.AddTail(&blackwoodConvention);

	if (m_bGerber)
		m_listConventions.AddTail(&gerberConvention);

	//
	if (m_bSplinterBids)
		m_listConventions.AddTail(&splinterBidsConvention);
	//
	if (m_bUnusualNT)
		m_listConventions.AddTail(&unusualNTConvention);
	//
	if (m_bJacoby2NT)
		m_listConventions.AddTail(&jacoby2NTConvention);
	//
	if (m_bGambling3NT)
		m_listConventions.AddTail(&gambling3NTConvention);
	//
	if (m_bJacobyTransfers)
		m_listConventions.AddTail(&jacobyTransferConvention);
	//
	if (m_bStayman)
		m_listConventions.AddTail(&staymanConvention);
	//
	if (m_bArtificial2ClubConvention && m_bWeakTwoBids)  //NCR need weak 2 with 2 club
		m_listConventions.AddTail(&artificial2ClubConvention);
	//
	if (m_bShutoutBids)
		m_listConventions.AddTail(&shutoutBidsConvention);
	//
	if (m_b4thSuitForcing)
		m_listConventions.AddTail(&fourthSuitForcingConvention);
	//
	if (m_bMichaels)
		m_listConventions.AddTail(&michaelsCueBidConvention);
	//
	if (m_bDrury)
		m_listConventions.AddTail(&druryConvention);

	// simple overcalls are always used
	m_listConventions.AddTail(&overcallsConvention);

	//
	if (m_bNegativeDoubles)
		m_listConventions.AddTail(&negativeDoublesConvention);
	//
	if (m_bTakeoutDoubles)
		m_listConventions.AddTail(&takeoutDoublesConvention);

	// only one of weak twos or strong twos can be active
	if (m_bWeakTwoBids)
		m_listConventions.AddTail(&weakTwoBidsConvention); // weak twos
	else
		m_listConventions.AddTail(&strongTwoBidsConvention); // strong twos

	//
//	if (m_bFiveCardMajors)
//		m_listConventions.AddTail(&fiveCardMajorsConvention);

	// add the most basic conventions at the end
//	m_listConventions.AddTail(&noTrumpConvention);
//	m_listConventions.AddTail(&naturalConvention);


	// instruct all players to clear their convention status
	if (pDOC && pDOC->IsInitialized())
	{
		for(int i=0;i<4;i++)
			PLAYER(i).ClearConventionStatus();
	}
}




//
// IsConventionEnabled()
//
// replaces the old PLAYING_XXX macros to test whether 
// a convention is being played in this convention set
//
BOOL CConventionSet::IsConventionEnabled(int nConventionID) const
{
	// look through the list of conventions and see if it's in the list
	// this is slower than, say, a switch/case expression to check
	// individual member variables (i.e., case tbBlackwood: return m_bBlackwood),
	// but is simpler and doesn't need to change when new conventions
	// are added
	if (m_listConventions.IsEmpty()) return FALSE;

	// look in the conventions list
	POSITION pos = m_listConventions.GetHeadPosition();
	CConvention* pConvention = m_listConventions.GetNext(pos);
	while(pConvention)
	{
//		ASSERT(pConvention->GetID() != -1);	// make sure it's initialized
		if (pConvention->GetID() == nConventionID)
			return TRUE;
		if (pos)
			pConvention = m_listConventions.GetNext(pos);
		else
			pConvention = NULL;
	}

	// see if it's a convention that's currently not implemented in the list
	switch(nConventionID)
	{
		case tid5CardMajors:
			return m_bFiveCardMajors;
		case tidLimitRaises:
			return m_bLimitRaises;
		case tidWeakJumpOvercalls:
			return m_bWeakJumpOvercalls;
		case tb4SuitTransfers:
			return m_b4SuitTransfers;
		case tidOgust:
			return m_bOgust;
		case tbStructuredReverses:
			return m_bStructuredReverses;
		case tb3LevelTakeouts:
			return m_b3LevelTakeouts;
		//
//		case tbGerber:
//			return m_bGerber;
	}
	

	//
	return FALSE;
}





//
// IsOptionEnabled()
//
// reports on whether a convention option is enabled
// more flexible than the call above, but is less general
//
BOOL CConventionSet::IsOptionEnabled(int nOptionID) const
{
	switch(nOptionID)
	{
		case tb4SuitTransfers:
			return m_b4SuitTransfers;
		case tbStructuredReverses:
			return m_bStructuredReverses;
		case tb3LevelTakeouts:
			return m_b3LevelTakeouts;
	}
	//
	return FALSE;
}





//
//==========================================================================
//
// Value Get/Set functions

//
//
int CConventionSet::GetValue(int nItem, int nIndex1, int nIndex2, int nIndex3)
{
	switch(nItem)
	{
		case tnBiddingStyle:
			return m_nBiddingStyle;
		case tid5CardMajors:
			return m_bFiveCardMajors;
		case tidArtificial2ClubConvention:
			return m_bArtificial2ClubConvention;
		case tidWeakTwoBids:
			return m_bWeakTwoBids;
		case tidWeakJumpOvercalls:
			return m_bWeakJumpOvercalls;
		case tidStrongTwoBids:
			return !m_bWeakTwoBids;
		case tidShutoutBids:
			return m_bShutoutBids;
		case tid4thSuitForcing:
			return m_b4thSuitForcing;
		case tidOgust:
			return m_bOgust;
		case tidBlackwood:
			return m_bBlackwood;
		case tidCueBids:
			return m_bCueBids;
		case tidGerber:
			return m_bGerber;
		case tidStayman:
			return m_bStayman;
		case tidJacobyTransfers:
			return m_bJacobyTransfers;
		case tidLimitRaises:
			return m_bLimitRaises;
		case tidTakeoutDoubles:
			return m_bTakeoutDoubles;
		case tidNegativeDoubles:
			return m_bNegativeDoubles;
		case tidSplinterBids:
			return m_bSplinterBids;
		case tidMichaels:
			return m_bMichaels;
		case tidUnusualNT:
			return m_bUnusualNT;
		case tidJacoby2NT:
			return m_bJacoby2NT;
		case tidGambling3NT:
			return m_bGambling3NT;
		case tnGambling3NTVersion:
			return m_nGambling3NTVersion;
		case tidDrury:
			return m_bDrury;
		case tidLebensohl:
			return m_bLebensohl;
		case tidDONT:
			return m_bDONT;
		//
		case tb4SuitTransfers:
			return m_b4SuitTransfers;
		case tbStructuredReverses:
			return m_bStructuredReverses;
		case tb3LevelTakeouts:
			return m_b3LevelTakeouts;
		//
		case tn1NTRange:
			return m_nNTRange[0];
		case tn2NTRange:
			return m_nNTRange[1];
		case tn3NTRange:
			return m_nNTRange[2];
		case tn2ClubOpenRange:
			return m_n2ClubOpenRange;
		//
		case tnAllowable1Openings:
			return m_nAllowable1Openings;

		// misc info
		case tn1NTRangeMinPts:
			return tnNTRangeTable[0][m_nNTRange[0]].min;
		case tn1NTRangeMaxPts:
			return tnNTRangeTable[0][m_nNTRange[0]].max;
		case tn2NTRangeMinPts:
			return tnNTRangeTable[1][m_nNTRange[1]].min;
		case tn2NTRangeMaxPts:
			return tnNTRangeTable[1][m_nNTRange[1]].max;
		case tn3NTRangeMinPts:
			return tnNTRangeTable[2][m_nNTRange[2]].min;
		case tn3NTRangeMaxPts:
			return tnNTRangeTable[2][m_nNTRange[2]].max;
		case tn2ClubOpeningPoints:
			return tn2ClubOpenTable[m_n2ClubOpenRange];
		case tnStrong2OpeningPts:   // NCR-177
			return tnStrongTwoPts;
		//
		default:
			return 0;
	}
	return 0;
}


//
int CConventionSet::SetValue(int nItem, int nValue, int nIndex1, int nIndex2, int nIndex3)
{
	int nVal = (int) nValue;
	BOOL bVal = (BOOL) nValue;
	//
	switch(nItem)
	{
		case tnBiddingStyle:
			m_nBiddingStyle = nVal;
			break;
		case tid5CardMajors:
			m_bFiveCardMajors = bVal;
			InitConventions();
			break;
		case tidArtificial2ClubConvention:
			m_bArtificial2ClubConvention = bVal;
			InitConventions();
			break;
		case tidWeakTwoBids:
			m_bWeakTwoBids = bVal;
			InitConventions();
			break;
		case tidWeakJumpOvercalls:
			m_bWeakJumpOvercalls = bVal;
			break;
		case tidStrongTwoBids:
			m_bWeakTwoBids = !bVal;
			InitConventions();
			break;
		case tidShutoutBids:
			m_bShutoutBids = bVal;
			InitConventions();
			break;
		case tid4thSuitForcing:
			m_b4thSuitForcing = bVal;
			break;
		case tidOgust:
			m_bOgust = bVal;
			InitConventions();
			break;
		case tidBlackwood:
			m_bBlackwood = bVal;
			InitConventions();
			break;
		case tidCueBids:
			m_bCueBids = bVal;
			InitConventions();
			break;
		case tidGerber:
			m_bGerber = bVal;
			InitConventions();
			break;
		case tidStayman:
			m_bStayman = bVal;
			InitConventions();
			break;
		case tidJacobyTransfers:
			m_bJacobyTransfers = bVal;
			InitConventions();
			break;
		case tidLimitRaises:
			m_bLimitRaises = bVal;
			InitConventions();
			break;
		case tidTakeoutDoubles:
			m_bTakeoutDoubles = bVal;
			InitConventions();
			break;
		case tidNegativeDoubles:
			m_bNegativeDoubles = bVal;
			InitConventions();
			break;
		case tidSplinterBids:
			m_bSplinterBids = bVal;
			InitConventions();
			break;
		case tidMichaels:
			m_bMichaels = bVal;
			InitConventions();
			break;
		case tidUnusualNT:
			m_bUnusualNT = bVal;
			InitConventions();
			break;
		case tidJacoby2NT:
			m_bJacoby2NT = bVal;
			InitConventions();
			break;
		case tidGambling3NT:
			m_bGambling3NT = bVal;
			InitConventions();
			break;
		case tnGambling3NTVersion:
			m_nGambling3NTVersion = nVal;
//			InitConventions();
			break;
		case tidDrury:
			m_bDrury = bVal;
			InitConventions();
			break;
		case tidLebensohl:
			m_bLebensohl = bVal;
			InitConventions();
			break;
		case tidDONT:
			m_bDONT = bVal;
			InitConventions();
			break;
		//
		case tb4SuitTransfers:
			m_b4SuitTransfers = bVal;
			break;
		case tbStructuredReverses:
			m_bStructuredReverses = bVal;
			break;
		case tb3LevelTakeouts:
			m_b3LevelTakeouts = bVal;
			break;
		//
		case tn1NTRange:
			m_nNTRange[0] = nVal;
			break;
		case tn2NTRange:
			m_nNTRange[1] = nVal;
			break;
		case tn3NTRange:
			m_nNTRange[2] = nVal;
			break;
		case tn2ClubOpenRange:
			m_n2ClubOpenRange = nVal;
			break;
		//
		case tnAllowable1Openings:
			m_nAllowable1Openings = nVal;
			break;
		//
		case tn1NTRangeMinPts:
		case tn1NTRangeMaxPts:
		case tn2NTRangeMinPts:
		case tn2NTRangeMaxPts:
		case tn3NTRangeMinPts:
		case tn3NTRangeMaxPts:
		case tn2ClubOpeningPoints:
		//
		default:
			return -1;
	}
	return 0;
}



//
// GetNTRangeMin()
//
// returns the min opening point count for opening 1NT, 2NT, or 3NT
//
int CConventionSet::GetNTRangeMin(int nLevel) 
{
	if ((nLevel < 1) || (nLevel > 3))
		return 0;
 	return tnNTRangeTable[nLevel-1][m_nNTRange[nLevel-1]].min;
}



//
// GetNTRangeMax()
//
// returns the max opening point count for opening 1NT, 2NT, or 3NT
//
int CConventionSet::GetNTRangeMax(int nLevel) 
{
	if ((nLevel < 1) || (nLevel > 3))
		return 0;
 	return tnNTRangeTable[nLevel-1][m_nNTRange[nLevel-1]].max;
}
