//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//===========================================================
//
// Defines.h
//
//===========================================================

#ifndef __DEFINES_H__
#define __DEFINES_H__

// forward declaration
class CCard;

// constant suit symbol
const unsigned char tSuitLetter = _T('§');

// basic alias
typedef int BID;
typedef CArray<int,int>			CIntArray;
typedef CList<int,int>			CIntList;
typedef CMap<int,int,int,int>	CIntMap;

// limits
const int MAX_CARDBACKS	= 32;
//const int MAXHOLDING	= 13;
const int MAXHOLDING	= 26;

// commonly used values
const int NONE = -99;  // NCR changed from -1

// NCR NONE used for MANY enums as undefined value so until all changed, need to have undefined
//     values set to SAME value as NONE !!!
// card suits
enum Suit { NOSUIT=-99, ANY=-1, CLUBS=0, DIAMONDS=1, HEARTS=2, SPADES=3, NOTRUMP=4 }; // NCR added NOSUIT

#define GETNEXTSUIT(X) X = Suit(((int)X)+1)  // NCR Converting int to Suit

// majors vs. minors selection
const int SUITS_ANY	   = 0;
const int SUITS_MINORS = 1;
const int SUITS_MAJORS = 2;

// honor card values
const int TEN =   10;
const int JACK =  11;
const int QUEEN = 12;
const int KING =  13;
const int ACE =   14;

const int NOTHING = 9;  //NCR define max mothing card

// positions
enum Position { UNKNOWN=-1, ANY_POS=-1, SOUTH=0, WEST=1, NORTH=2, EAST=3, TABLE=4, };

// teams
enum Team { NEITHER=-1, NORTH_SOUTH=0, EAST_WEST=1, BOTH=2 };

// bids
enum Bids {
	BID_NONE=-99,  // NCR changed from -1 (same as NONE for now)
	BID_PASS=0, 
	BID_1C= 1, BID_1D= 2, BID_1H= 3, BID_1S= 4, BID_1NT= 5,
	BID_2C= 6, BID_2D= 7, BID_2H= 8, BID_2S= 9, BID_2NT=10,
	BID_3C=11, BID_3D=12, BID_3H=13, BID_3S=14, BID_3NT=15,
	BID_4C=16, BID_4D=17, BID_4H=18, BID_4S=19, BID_4NT=20,
	BID_5C=21, BID_5D=22, BID_5H=23, BID_5S=24, BID_5NT=25,
	BID_6C=26, BID_6D=27, BID_6H=28, BID_6S=29, BID_6NT=30,
	BID_7C=31, BID_7D=32, BID_7H=33, BID_7S=34, BID_7NT=35,
	BID_DOUBLE=36, BID_REDOUBLE=37
};


// Shortcuts (Macros)
#define pMAINFRAME		 ((CMainFrame*) theApp.GetFrame())
#define pDOC			 (CEasyBDoc::m_pDoc)
#define pVIEW			 (CEasyBView::m_pView)
#define DECK			 theApp.m_deck
#define PLAYER(x)		 (*(CEasyBDoc::GetDoc()->GetPlayer(x)))
#define DUMMY			 (*(CEasyBDoc::GetDoc()->GetPlayer(GetDoc()->GetDummyPlayer())))
#define	ABS(x)			 ((x>=0)?x:-x)
#define	DECKVALUE(x)		((x->GetSuit()*13) + (x->GetFaceValue() - 2))
#define	MAKEDECKVALUE(x,y)	((x*13) + (y - 2))
#define	CARDSUIT(x)		 (x / 13)
#define	FACEVAL(x)		 (x % 13) + 2

#define SETSTATUSMSG(x)		((CMainFrame*)theApp.GetFrame())->SetStatusText(x)
#define RESETSTATUSMSG()	((CMainFrame*)theApp.GetFrame())->SetStatusText(AFX_IDS_IDLEMESSAGE)
#define CLEARSTATUSMSG()	((CMainFrame*)theApp.GetFrame())->SetStatusText("")

#define ISMAJOR(x)	(((x == HEARTS) || (x == SPADES))? TRUE: FALSE)
#define ISMINOR(x)	(((x == CLUBS) || (x == DIAMONDS))? TRUE: FALSE)
#define ISNOTRUMP(x)((x == NOTRUMP)? TRUE: FALSE)

#define BID_SUIT(x)  ( ((x <= BID_PASS) || (x >= BID_DOUBLE))? NOSUIT : ((x-1) % 5) )  // NCR NOSUIT vs -1
#define BID_LEVEL(x) ( ((x <= BID_PASS) || (x >= BID_DOUBLE))? BID_NONE : (((x-1) / 5) + 1) ) // NCE BID_NONE vs -1
#define MAKEBID(s,l) (BID_PASS + (l-1)*5 + s + 1)

#define ISBID(x) ((x >= BID_1C) && (x <= BID_7NT))
#define ISSUIT(x) ((x >= CLUBS) && (x <= SPADES))
#define ISCARD(x) ((x >= 0) && (x <= 51))
#define ISDECKVAL(x) ((x >= 0) && (x <= 51))
#define ISFACEVAL(x) ((x >= 2) && (x <= ACE))
#define ISPOSITION(x) ((x >= SOUTH) && (x <= EAST))
#define ISPLAYER(x) ((x >= SOUTH) && (x <= EAST))
#define ISTEAM(x) ((x == NORTH_SOUTH) || (x == EAST_WEST))

//
#define OTHER_MAJOR(x) 	((x == HEARTS)? SPADES:HEARTS)
#define OTHER_MINOR(x) 	((x == CLUBS)? DIAMONDS:CLUBS)

// misc
#define SHOW_STATUSDLG() ((CMainFrame*)theApp.GetFrame())->MakeStatusDialogVisible()
#define SHOW_FEEDBACK() ((CMainFrame*)theApp.GetFrame())->MakeStatusDialogVisible()
#define FEEDBACK(s)  ((CMainFrame*)theApp.GetFrame())->SetFeedbackText(s)

//
#define MIN(a,b) ( ((a) <= (b))? (a): (b) )
#define MAX(a,b) ( ((a) >= (b))? (a): (b) )


// Windoze user messages
#define WMS_UPDATE				WM_USER + 100
#define WMS_DOREALIZE   		WM_USER + 120
#define WMS_RESET_DISPLAY		WM_USER + 140
#define WMS_UPDATE_DISPLAY		WM_USER + 160
#define WMS_REFRESH_DISPLAY		WM_USER + 180
#define WMS_FLASH_BUTTON		WM_USER + 190
#define WMS_UPDATE_TEXT			WM_USER + 200
#define WMS_UPDATE_TOOLTIP_TEXT	WM_USER + 210
#define WMS_SET_ANALYSIS_FONT	WM_USER + 220
#define WMS_SETTEXT				WM_USER + 240
#define WMS_SET_FEEDBACK_TEXT	WM_USER + 242
#define WMS_SET_GIB_TEXT		WM_USER + 244
//
#define WMS_FILE_LOADED			WM_USER + 300
#define WMS_INITNEWDEAL			WM_USER + 320
#define WMS_BIDDING_DONE		WM_USER + 340
#define WMS_BIDDING_CANCELLED	WM_USER + 360
#define WMS_BIDDING_RESTART		WM_USER + 380
#define WMS_CARD_PLAY			WM_USER + 400
#define WMS_PLAY_RESTART		WM_USER + 420
#define WMS_SET_CARD_LIMIT		WM_USER + 520
#define WMS_LAYOUT_DONE			WM_USER + 540
#define WMS_RESUME_GAME			WM_USER + 560
//
#define WMS_FLUSH_MESSAGES		WM_USER + 590
//
#define WMS_F1DOWN				WM_USER + 600



// Win95 support
#define OFN_EXPLORER                 0x00080000     // new look commdlg
#define OFN_NODEREFERENCELINKS       0x00100000
#define OFN_LONGNAMES                0x00200000     // force long names for 3.x modules


#endif