//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Bidparms.h
//
// parms used for bidding info
//
#ifndef __BIDPARMS___
#define __BIDPARMS___


#include "ConvCodes.h"	// need the convention codes

// shortcuts (Macros)
#define BTS(x) BidToFullString(x)
#define STS(x) SuitToString(x)
#define STSS(x) SuitToSingularString(x)
#define SHTS(x) SuitHoldingsToString(x)
#define SSTS(x) SuitStrengthToString(x)
#define SLTS(x) SupportLevelToString(x)
#define LEN(x) numCardsInSuit[x]
//
#define SINGLE_RAISE(x) (x + 5)
//
#define SUITS_TOUCHING(x,y) ((x == y+1) || (y == x+1))? TRUE: FALSE


// honors values
#define HONOR_VALUE(x)	theApp.m_fHonorValue[x]
#define ACE_VALUE		theApp.m_fHonorValue[ACE]
#define KING_VALUE		theApp.m_fHonorValue[KING]
#define QUEEN_VALUE		theApp.m_fHonorValue[QUEEN]
#define JACK_VALUE		theApp.m_fHonorValue[JACK]
#define TEN_VALUE		theApp.m_fHonorValue[TEN]

//
/*
#define MAJORGAMEPTS	(theApp.GetValue(tfMajorGamePts) - theApp.GetValue(tfAggressivenessRating))
#define MINORGAMEPTS	(theApp.GetValue(tfMajorGamePts) - theApp.GetValue(tfAggressivenessRating))
#define NTGAMEPTS		(theApp.GetValue(tfMajorGamePts) - theApp.GetValue(tfAggressivenessRating))
#define SMALLSLAMPTS	(theApp.GetValue(tfMajorGamePts) - theApp.GetValue(tfAggressivenessRating))
#define GRANDSLAMPTS	(theApp.GetValue(tfMajorGamePts) - theApp.GetValue(tfAggressivenessRating))
*/

// game points macros
#define PTS_GAME			CEasyBApp::m_fNTGamePts
#define PTS_MAJOR_GAME		CEasyBApp::m_fMajorSuitGamePts
#define PTS_MINOR_GAME		CEasyBApp::m_fMinorSuitGamePts
#define PTS_NT_GAME			CEasyBApp::m_fNTGamePts
#define PTS_4LEVEL			CEasyBApp::m_f4LevelPts
#define PTS_3LEVEL			CEasyBApp::m_f3LevelPts
#define PTS_2LEVEL			CEasyBApp::m_f2LevelPts
//
#define PTS_SLAM			CEasyBApp::m_fSlamPts
#define PTS_SMALL_SLAM		CEasyBApp::m_fSlamPts
#define PTS_GRAND_SLAM		CEasyBApp::m_fGrandSlamPts

// opening points macros
// deduct 0.5 pts for each aggressiveness tick
#define OPEN_PTS(x)			((double)(x) - (double)theApp.GetBiddingAgressiveness()*0.5)

// more generic macro
#define PT_COUNT(x)  ((double)(x) - (double)theApp.GetBiddingAgressiveness()*0.5)



//
//----------------------------------------------------------
//
// codes used to convey the strength of a suit
//
// suit is void
const int	SS_VOID						= 0;
const int	SS_NONE						= 0;
const int	SS_UNUSABLE					= 0;
const int	STRENGTH_VOID				= 0;
//
const int	SS_ANY						= 0;
const int	STRENGTH_ANY				= 0;
// a poor holding is xx or worse
const int	SS_POOR						= 1;
const int	STRENGTH_POOR				= 1;
// suit can be used as weak support for partner's suit
// (3 small cards or 2 cards with an honor)
const int	SS_WEAK_SUPPORT				= 2;
const int	STRENGTH_WEAK_SUPPORT		= 2;
// suit can be used as moderate support for partner's suit
// (4 small cards or 3 cards with an honor)
const int	SS_MODERATE_SUPPORT			= 3;
const int	STRENGTH_MODERATE_SUPPORT	= 3;
// suit is strong support for partner's suit 
// (or a marginal opener)
// (5 cards with 0-2 pts or 4 cards with 0-3 pts)
const int	SS_GOOD_SUPPORT				= 4;
const int	STRENGTH_GOOD_SUPPORT		= 4;
const int	SS_MARGINAL_OPENER			= 4;
const int	STRENGTH_MARGINAL_OPENER	= 4;
// suit is openable (4 cards with 4+ honor points or 5 cards with 3+ pts)
const int	SS_OPENABLE					= 5;
const int	STRENGTH_OPENABLE			= 5;
const int	SS_STRONG_SUPPORT			= 5;
const int	STRENGTH_STRONG_SUPPORT		= 5;
// suit is strong
const int	SS_STRONG					= 6;
const int	STRENGTH_STRONG				= 6;
// absolute suit to play in, regardless of partner's wishes
const int	SS_ABSOLUTE					= 7;
const int	STRENGTH_ABSOLUTE			= 7;

// modifier to suit strength
const int	SS_SOLID					= 0x4000;
const int	STRENGTH_SOLID				= 0x4000;
const int	SS_SELFSUPPORTING			= 0x8000;
const int	STRENGTH_SELFSUPPORTING		= 0x8000;


//
//----------------------------------------------------------
//
// codes used in raising partner or rebidding/shifting
//
typedef enum {
	RAISE_ONE			= 1,
	RAISE_TWO			= 2,
	RAISE_THREE			= 3,
	RAISE_FOUR			= 4,
	RAISE_FIVE			= 5,
	RAISE_SIX			= 6,
	RAISE_TO_TWO		= -2,
	RAISE_TO_THREE		= -3,
	RAISE_TO_FOUR		= -4,
	RAISE_TO_FIVE		= -5,
	RAISE_TO_SIX		= -6,
	RAISE_TO_SEVEN		= -7,
//
	RAISE_BY_ONE		= 1,
	RAISE_BY_TWO		= 2,
	RAISE_BY_THREE		= 3,
	RAISE_BY_FOUR		= 4,
	RAISE_BY_FIVE		= 5,
	RAISE_BY_SIX		= 6,
//
	RAISE_1				= 1,
	RAISE_2				= 2,
	RAISE_3				= 3,
	RAISE_4				= 4,
	RAISE_5				= 5,
	RAISE_6				= 6,
	RAISE_TO_2			= -2,
	RAISE_TO_3			= -3,
	RAISE_TO_4			= -4,
	RAISE_TO_5			= -5,
	RAISE_TO_6			= -6,
	RAISE_TO_7			= -7,
//
	RAISE_TO_NO_MORE_THAN_GAME	= -9,
	RAISE_TO_GAME				= -10,
} RaiseLevel;


// bid level codes
typedef enum
{
	LEVEL_1				= 1,
	LEVEL_2				= 2,
	LEVEL_3				= 3,
	LEVEL_4				= 4,
	LEVEL_5				= 5,
	LEVEL_6				= 6,
	LEVEL_7				= 7,
} BidLevel;


// notrump codes
typedef enum
{
	BALANCED_DONTCARE	= 0,
	BALANCED			= 1,
} BalancedCode;


// # of minimum cards cards for a bid/rebid
typedef enum {
	LENGTH_ANY			= 0,
	LENGTH_ONE			= 1,
	LENGTH_TWO			= 2,
	LENGTH_THREE		= 3,
	LENGTH_FOUR			= 4,
	LENGTH_FIVE			= 5,
	LENGTH_SIX			= 6,
	LENGTH_SEVEN		= 7,
	LENGTH_EIGHT		= 8,
	LENGTH_NINE			= 9,
	LENGTH_TEN			= 10,
//
	LENGTH_1			= 1,
	LENGTH_2			= 2,
	LENGTH_3			= 3,
	LENGTH_4			= 4,
	LENGTH_5			= 5,
	LENGTH_6			= 6,
	LENGTH_7			= 7,
	LENGTH_8			= 8,
	LENGTH_9			= 9,
	LENGTH_10			= 10,
//
	LEN_1				= 1,
	LEN_2				= 2,
	LEN_3				= 3,
	LEN_4				= 4,
	LEN_5				= 5,
	LEN_6				= 6,
	LEN_7				= 7,
	LEN_8				= 8,
	LEN_9				= 9,
	LEN_10				= 10,
} SuitLength;


// # of minimum support cards for a raise
typedef enum {
	SUPLEN_ANY			= 0,
	SUPLEN_ONE			= 1,
	SUPLEN_TWO			= 2,
	SUPLEN_THREE		= 3,
	SUPLEN_FOUR			= 4,
	SUPLEN_FIVE			= 5,
	SUPLEN_SIX			= 6,
	SUPLEN_SEVEN		= 7,
	//
	SUPLEN_1			= 1,
	SUPLEN_2			= 2,
	SUPLEN_3			= 3,
	SUPLEN_4			= 4,
	SUPLEN_5			= 5,
	SUPLEN_6			= 6,
	SUPLEN_7			= 7,
} SupportLength;


// # of honors
typedef enum {
	HONORS_ANY			= 0,
	HONORS_ONE			= 1,
	HONORS_TWO			= 2,
	HONORS_THREE		= 3,
	HONORS_FOUR			= 4,
	HONORS_1			= 1,
	HONORS_2			= 2,
	HONORS_3			= 3,
	HONORS_4			= 4,
} HonorsCount;


//
//------------------------------------------------------------
// Codes used in shifting or rebidding a suit
//
typedef enum {
	REBID_CHEAPEST		= 0,
	JUMP_REBID_ONE		= 1,
	JUMP_REBID_TWO		= 2,
	JUMP_REBID_THREE	= 3,
	JUMP_REBID_FOUR		= 4,
	JUMP_REBID_FIVE		= 5,
	JUMP_REBID_SIX		= 6,
	REBID_AT_ONE		= -1,
	REBID_AT_TWO		= -2,
	REBID_AT_THREE		= -3,
	REBID_AT_FOUR		= -4,
	REBID_AT_FIVE		= -5,
	REBID_AT_SIX		= -6,
	REBID_AT_SEVEN		= -7,
//
	JUMP_REBID_1		= 1,
	JUMP_REBID_2		= 2,
	JUMP_REBID_3		= 3,
	JUMP_REBID_4		= 4,
	JUMP_REBID_5		= 5,
	JUMP_REBID_6		= 6,
	REBID_AT_1			= -1,
	REBID_AT_2			= -2,
	REBID_AT_3			= -3,
	REBID_AT_4			= -4,
	REBID_AT_5			= -5,
	REBID_AT_6			= -6,
	REBID_AT_7			= -7,

// suit shift codes
	SHIFT_CHEAPEST		= 0,
	JUMP_SHIFT_ONE		= 1,
	JUMP_SHIFT_TWO		= 2,
	JUMP_SHIFT_THREE	= 3,
	JUMP_SHIFT_FOUR		= 4,
	JUMP_SHIFT_FIVE		= 5,
	JUMP_SHIFT_SIX		= 6,
	JUMP_SHIFT_TO_ONE	= -1,
	JUMP_SHIFT_TO_TWO	= -2,
	JUMP_SHIFT_TO_THREE	= -3,
	JUMP_SHIFT_TO_FOUR	= -4,
	JUMP_SHIFT_TO_FIVE	= -5,
	JUMP_SHIFT_TO_SIX	= -6,
	JUMP_SHIFT_TO_SEVEN	= -7,
//
	SHIFT_0				= 0,
	JUMP_SHIFT_1		= 1,
	JUMP_SHIFT_2		= 2,
	JUMP_SHIFT_3		= 3,
	JUMP_SHIFT_4		= 4,
	JUMP_SHIFT_5		= 5,
	JUMP_SHIFT_6		= 6,
	JUMP_SHIFT_TO_1		= -1,
	JUMP_SHIFT_TO_2		= -2,
	JUMP_SHIFT_TO_3		= -3,
	JUMP_SHIFT_TO_4		= -4,
	JUMP_SHIFT_TO_5		= -5,
	JUMP_SHIFT_TO_6		= -6,
	JUMP_SHIFT_TO_7		= -7,
} RebidLevel;


// suits stopped codes for NT contract
typedef enum {
	STOPPED_DONTCARE	= 0,
	STOPPED_ALL			= 1,
	STOPPED_ALLOTHER	= 2,
	STOPPED_ONE			= 3,
	STOPPED_UNBID		= 4,
} StoppedCode;


//
//----------------------------------------------------------
//
// codes used for type of opening desired
//
const int	OT_ANY			= 0;
const int	OT_OPENER		= 1;
const int	OT_STRONG		= 2;
const int	OT_ABSOLUTE		= 3;


//
//----------------------------------------------------------
//
// codes used to indicate next intended bid
//
const int	NIB_NONE		= 0;
const int	NIB_NT			= 5;
const int	NIB_JUMP_NT		= 10;



//
//----------------------------------------------------------
//
// codes used to categorize cards
//
const int	CV_LOSER		= 0;
const int	CV_PROBABLE		= 1;
const int	CV_WINNER		= 2;


//
//----------------------------------------------------------
//
// codes used for selection superior suits
//
const int	SP_NONE			= 0;
const int	SP_FIRST		= 1;
const int	SP_SECOND		= 2;
const int	SP_THIRD		= 3;
const int	SP_FOURTH		= 4;
const int	SP_LAST			= 5;



//
//----------------------------------------------------------
//
// codes used for opening options
//
const int	OB_11_HCPS_RBS_LM	= 0x0001;
const int	OB_11_HCPS_6CS      = 0x0002;
const int	OB_14_TCPS_GS       = 0x0004;
const int	OB_14_TCPS_LS       = 0x0008;
const int	OB_13_TCPS_LS		= 0x0010;


//
// params used in determining whether a suit can be opened
//
const int	POS_ANY			= 0;
const int	POS_12			= 1;
const int	POS_34			= 2;
const int	POS_4			= 3;


// suit type
const int	SUIT_ANY		= 0;
const int	SUIT_MINOR		= 1;
const int	SUIT_MAJOR		= 2;
const int	SUIT_PREV		= 3;
const int	SUIT_PREV_MAJOR	= 4;
const int	SUIT_PREV_MINOR	= 5;
const int	SUIT_PREFERRED	= 6;
const int	SUIT_DIRECT		= -10;
const int	SUIT_CLUBS		= -10;
const int	SUIT_DIAMONDS	= -11;
const int	SUIT_HEARTS		= -12;
const int	SUIT_SPADES		= -13;
const int	SUIT_NOTRUMP	= -14;
inline int MakeDirectSuit(int nSuit) { return SUIT_DIRECT - nSuit; }

// hand revaluation codes
const int	REVALUE_DUMMY					= 0;
const int	REVALUE_DECLARER				= 1;
const int	REVALUE_NOPENALTY_SHORTTRUMP	= 2;

// Blackwood codes
const int	BLACKWOOD_ACES		= 0;
const int	BLACKWOOD_KINGS		= 1;


#endif