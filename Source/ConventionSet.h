//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// ConventionSet.h
//
#ifndef __CCONVENTIONSET__
#define __CCONVENTIONSET__

#include "ConvCodes.h"

class CConvention;
class CBidEngine;
class CHandHoldings;
class CPlayerStatusDialog;
class CCardLocation;
class CGuessedHandHoldings;


//
typedef struct {
	int min;
	int max;
} MINMAX;


//
// The ConventionSet class
//
class CConventionSet : public CObjectWithProperties
{

	friend class CConvention;

// public routines
public:
	// property get/set operations
	int  GetValue(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int  SetValue(int nItem, int nValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	void RefreshProperties() { InitConventions(); }
	//
	void Initialize(LPCTSTR szName=NULL);
	void Terminate();
	void ClearState();
	static void InitCommonSettings();
	void InitConventions();
	BOOL IsConventionEnabled(int nConventionID) const;
	BOOL IsOptionEnabled(int nOptionID) const;
	BOOL ApplyConventionTests(const CPlayer& player, CHandHoldings& hand, CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine& bidState, CPlayerStatusDialog& status);
//	int GetOpeningRangeMin(int nBid);
//	int GetOpeningRangeMax(int nBid);
	int GetNTRangeMin(int nLevel);
	int GetNTRangeMax(int nLevel);
	CConvention* GetNextConvention(POSITION& pos);
	//
	CConventionSet& operator=(CConventionSet& src);

// private routines
private:

// data
private:
	// list of convention objects
	CTypedPtrList<CPtrList, CConvention*> m_listConventions;

	// Bidding convention info
	// description
	CString m_strName;
	CString m_strDescription;
	int		m_nBiddingStyle;
	// misc
	BOOL	m_bFiveCardMajors;
	BOOL	m_bArtificial2ClubConvention;
	BOOL	m_bWeakTwoBids;
	BOOL	m_bShutoutBids;
	BOOL	m_bOgust;
	BOOL	m_bLimitRaises;
	BOOL	m_bSplinterBids;
	BOOL	m_bUnusualNT;
	BOOL	m_bJacoby2NT;
	BOOL	m_bMichaels;
	BOOL	m_bGambling3NT;
	int		m_nGambling3NTVersion;
	BOOL	m_bDrury;
	BOOL	m_bLebensohl;
	BOOL	m_bDONT;
	BOOL	m_bTakeoutDoubles;
	BOOL	m_bNegativeDoubles;
	// allowable 1-level openings
	int		m_nAllowable1Openings;
	// 2C opening range -- 20+, 21+, 22+, 23+
	int	    m_n2ClubOpenRange;
	// NT-related
	// 0=12-14, 0=15-17, 1=16-18, 
	// 0=21-22, 1=22-24
	// 0=25-27, 1=Bid 2C instead
	int		m_nNTRange[3];			
	// after NT
	BOOL	m_bStayman;
	BOOL	m_bJacobyTransfers;
	BOOL 	m_b4SuitTransfers;
	//
	//
	BOOL	m_b4thSuitForcing;
	BOOL	m_bStructuredReverses;
	BOOL	m_b3LevelTakeouts;
	// misc
	BOOL	m_bWeakJumpOvercalls;
	// slam-related
	BOOL	m_bBlackwood;
	BOOL	m_bRKCB;
	BOOL	m_bCueBids;
	BOOL	m_bGerber;

//
public:
	CConventionSet();
	~CConventionSet();
};

#endif

