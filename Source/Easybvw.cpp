//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// EasyBvw.cpp : implementation of the CEasyBView class
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "Player.h"
#include "mainfrm.h"
#include "deck.h"
#include "card.h"
#include "EasyBvw.h"
#include "progopts.h"
#include "viewopts.h"
#include "docopts.h"
#include "BidDialog.h"
#include "GameReviewDialog.h"
#include "MainframeOpts.h"
#include "CardLayout.h"
#include "SelectHandDialog.h"
#include "DrawParameters.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// constants
char* szLayoutDonePrompt = "Reassign cards or press F9 when done";
int numCardsProcessed = 0;

// static data
CEasyBView* CEasyBView::m_pView = NULL;
BOOL	CEasyBView::m_bResourcesInitialized = FALSE;
BOOL	CEasyBView::m_bViewInitialized = FALSE;


/////////////////////////////////////////////////////////////////////////////
// CEasyBView

IMPLEMENT_DYNCREATE(CEasyBView, CView)

BEGIN_MESSAGE_MAP(CEasyBView, CView)
	//{{AFX_MSG_MAP(CEasyBView)
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_EDIT_UNDO_CARD, OnUndoCard)
	ON_COMMAND(ID_EDIT_UNDO_TRICK, OnUndoTrick)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO_CARD, OnUpdateUndoCard)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO_TRICK, OnUpdateUndoTrick)
	ON_WM_LBUTTONUP()
	ON_UPDATE_COMMAND_UI(ID_LAYOUT_CARDS, OnUpdateLayoutCards)
	ON_COMMAND(ID_LAYOUT_CARDS, OnLayoutCards)
	ON_WM_LBUTTONDBLCLK()
	ON_UPDATE_COMMAND_UI(ID_DISTRIBUTE_RANDOM, OnUpdateDistributeRandom)
	ON_COMMAND(ID_DISTRIBUTE_RANDOM, OnDistributeRandom)
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_EDIT_EXISTING_HANDS, OnUpdateEditExistingHands)
	ON_COMMAND(ID_EDIT_EXISTING_HANDS, OnEditExistingHands)
	ON_UPDATE_COMMAND_UI(ID_BID_CURRENT_HAND, OnUpdateBidCurrentHand)
	ON_COMMAND(ID_BID_CURRENT_HAND, OnBidCurrentHand)
	ON_COMMAND(ID_REFRESH_SCREEN, OnRefreshScreen)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_WM_SETCURSOR()
	ON_UPDATE_COMMAND_UI(ID_GAME_REVIEW, OnUpdateGameReview)
	ON_COMMAND(ID_GAME_REVIEW, OnGameReview)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()



static BOOL bStart = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CEasyBView construction/destruction

CEasyBView::CEasyBView()
{
	m_pView = this;

	// read in the saved variables from the .INI file
	Initialize();

	// clear suits draw area
	for(int i=0;i<4;i++) 
	{
		for(int j=0;j<4;j++) 
		{
			m_rectSuitOffset[i][j].left = -1;
			m_rectSuitOffset[i][j].top = -1;
			m_rectSuitOffset[i][j].right = -1;
			m_rectSuitOffset[i][j].bottom = -1;
		}
	}

	// screen-oriented inits flags
	m_bOpeningSequenceDone = FALSE;
	m_pIntroWnd = NULL;
	m_nSuppressRefresh = 0;
	//
	m_nCurrMode = MODE_NONE;
	m_nOldMode = MODE_NONE;
	//
	m_bLayoutCardSelected = FALSE;
	m_nLayoutCardDest = UNKNOWN;
	m_nLayoutCardOrigin = UNKNOWN;
	m_pLayoutCard = NULL;
	m_numCardsUnassigned = 52;
	m_pointLayout.x = -1;
	m_pointLayout.y = -1;
	m_bLayoutDlgActive = FALSE;
	//
	m_bExchanging = FALSE;
	m_nExchangeSrcPlayer = NONE;
	m_pExchangeCard = NULL;

	// load the default background bitmap
	m_defaultBitmap.LoadBitmap(IDB_DEFAULT_BACKGROUND);
	// load the mini-card bitmaps
	BOOL bSmallCards = theApp.GetValue(tbLowResOption);
	if (bSmallCards)
	{
		m_winnersBitmap.LoadBitmap(IDBS_WINNERS);
		m_losersBitmap.LoadBitmap(IDBS_LOSERS);
	}
	else
	{
		m_winnersBitmap.LoadBitmap(IDB_WINNERS);
		m_losersBitmap.LoadBitmap(IDB_LOSERS);
	}

	// check # of color planes
	HDC hDC = ::GetDC(NULL);
	int nPlanes = ::GetDeviceCaps(hDC, PLANES);
	int nBits = ::GetDeviceCaps(hDC, BITSPIXEL);
	int nXRes = GetDeviceCaps(hDC, HORZRES);
	int nYRes = GetDeviceCaps(hDC, VERTRES);
	int nDIBitmap = ::GetDeviceCaps(hDC, RC_DI_BITMAP);
	int nDIBtoDev = ::GetDeviceCaps(hDC, RC_DIBTODEV);
	int nRasterCaps = ::GetDeviceCaps(hDC, RASTERCAPS);
	::ReleaseDC(NULL,hDC);
	//
	m_nColorDepth = nBits;
	m_nScreenWidth = nXRes;
	m_nScreenHeight = nYRes;

	//
	if ((nBits < 8) && (nPlanes == 1))
	{
		// can't draw any fancy bitmaps
		m_bCanDisplayBitmap = FALSE;
		m_bBitmapAvailable = FALSE;
	} 
	else 
	{
		// we're physically able to draw bitmaps
		m_bCanDisplayBitmap = TRUE;

		// load custom bitmap file
		if (!m_strBackgroundBitmap.IsEmpty())
			m_bBitmapAvailable = m_customDIB.Load(m_strBackgroundBitmap);
		else
			m_bBitmapAvailable = FALSE;
	}
}

// destructor
CEasyBView::~CEasyBView()
{
	// delete table region
	m_tableRegion.DeleteObject();

	// delete default background bitmap
	m_defaultBitmap.DeleteObject();
	m_winnersBitmap.DeleteObject();
	m_losersBitmap.DeleteObject();

	// delete cursors
	// (not necessary)

	// and the intro window, if necessary
//	if (theApp.GetValue(tbFirstTimeRunning))
//		delete m_pIntroWnd;

	//
	m_bResourcesInitialized = FALSE;
	m_bViewInitialized = FALSE;
}


//
BOOL CEasyBView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{

	// Register a custom WndClass so that our custom icons
	// won't revert back to the standard arrow icon every time
	// the cursor moves
	LPCTSTR lpszMyClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,
											  NULL, // no default cursor
											  (HBRUSH)(COLOR_WINDOW+1),
											  NULL);
	return CWnd::Create(lpszMyClass, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
//	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}



static TCHAR BASED_CODE szDisplayOptions[] = _T("Display Options");
static TCHAR BASED_CODE szTripleBuffer[] = _T("Triple Buffer");
static TCHAR BASED_CODE szAnimateCards[] = _T("Animate Cards");
static TCHAR BASED_CODE szAnimationGranularity[] = _T("Animation Granularity");
static TCHAR BASED_CODE szOffsetVerticalSuits[] = _T("Offset Vertical Suits");
static TCHAR BASED_CODE szBackgroundBitmap[] = _T("Background Bitmap");

//
void CEasyBView::Initialize() 
{
	m_bTripleBuffer = theApp.GetProfileInt(szDisplayOptions, szTripleBuffer, FALSE);
	m_bAnimateCards = theApp.GetProfileInt(szDisplayOptions, szAnimateCards, TRUE);
	m_nAnimationGranularity = theApp.GetProfileInt(szDisplayOptions, szAnimationGranularity, 9);
//	m_bOffsetVertSuits = theApp.GetProfileInt(szDisplayOptions, szOffsetVerticalSuits, TRUE);
	m_bOffsetVertSuits = TRUE;

	m_strBackgroundBitmap = theApp.GetProfileString(szDisplayOptions, szBackgroundBitmap, "bridge.bmp");
	//
	ResetSuitSequence();
}


//
void CEasyBView::Terminate() 
{
	theApp.WriteProfileInt(szDisplayOptions, szTripleBuffer, m_bTripleBuffer);
	theApp.WriteProfileInt(szDisplayOptions, szAnimateCards, m_bAnimateCards);
	theApp.WriteProfileInt(szDisplayOptions, szAnimationGranularity, m_nAnimationGranularity);
//	theApp.WriteProfileInt(szDisplayOptions, szOffsetVerticalSuits, m_bOffsetVertSuits);
	theApp.WriteProfileString(szDisplayOptions, szBackgroundBitmap, m_strBackgroundBitmap);
}


//
void CEasyBView::OnDestroy() 
{
	Terminate();	
	//
	CView::OnDestroy();
}


// static function to get the current view
CEasyBView* CEasyBView::GetView()
{
	return m_pView;
/*
	CFrameWnd* pFrame = (CFrameWnd*)(AfxGetApp()->m_pMainWnd);
	if (!pFrame)
		return NULL;
	CView* pView = pFrame->GetActiveView();
	if (!pView)
		return NULL;
	// Fail if view is of wrong kind
	// (this could occur with splitter windows, or additional
	// views on a single document
	if (!pView->IsKindOf(RUNTIME_CLASS(CEasyBView)))
		return NULL;
	return (CEasyBView*) pView;
*/
}
 

/////////////////////////////////////////////////////////////////////////////
//
// EasyBView Routines
//
/////////////////////////////////////////////////////////////////////////////


//
// generic property operators
//

//
LPVOID CEasyBView::GetValuePV(int nItem, int nIndex1, int nIndex2, int nIndex3)
{
	switch(nItem)
	{
		case tbAnimateCards:
			return (LPVOID) m_bAnimateCards;
		case tbAutoRestackCards:
			return (LPVOID) m_bAutoRestackCards;
		case tbBitmapAvailable:
			return (LPVOID) m_bBitmapAvailable;
		case tbCanDisplayBitmap:
			return (LPVOID) m_bCanDisplayBitmap;
		case tbExchanging:
			return (LPVOID) m_bExchanging;
		case tbLayoutCardSelected:
			return (LPVOID) m_bLayoutCardSelected;
		case tbLayoutDialogActive:
			return (LPVOID) m_bLayoutDlgActive;
		case tbOffsetVerticalSuits:
			return (LPVOID) m_bOffsetVertSuits;
		case tbTripleBuffer:
			return (LPVOID) m_bTripleBuffer;
		case tbViewInitialized:
			return (LPVOID) m_bViewInitialized;
		case tbOpeningSequenceDone:
			return (LPVOID) m_bOpeningSequenceDone;
		case tpCardDest:
			return (nIndex2==0)? (LPVOID) m_cardDest[nIndex1].x :
				   (nIndex2==1)? (LPVOID) m_cardDest[nIndex1].y : NULL;
		case tpvCustomDIB:
			return (LPVOID) &m_customDIB;
		case tpvDefaultBitmap:
			return (LPVOID) m_defaultBitmap;
		case tpDrawPoint:
			return (nIndex2==0)? (LPVOID) m_drawPoint[nIndex1].x :
				   (nIndex2==1)? (LPVOID) m_drawPoint[nIndex1].y : NULL;
		case tpDummyDrawOffset:
			return (nIndex2==0)? (LPVOID) m_dummyDrawOffset[nIndex1].x :
				   (nIndex2==1)? (LPVOID) m_dummyDrawOffset[nIndex1].y : NULL;
		case tpDummyLabelOffset:
			return (nIndex2==0)? (LPVOID) m_dummyLabelOffset[nIndex1].x :
				   (nIndex2==1)? (LPVOID) m_dummyLabelOffset[nIndex1].y : NULL;
		case tnCardWidth:
			return (LPVOID) m_nCardWidth;
		case tnCardHeight:
			return (LPVOID) m_nCardHeight;
		case tnCardXGap:
			return (LPVOID) m_nCardXGap;
		case tnCardYGap:
			return (LPVOID) m_nCardYGap;
		case tnOverlapYOffset:
			return (LPVOID) m_nOverlapYOffset;
		case tnSuitSpacer:
			return (LPVOID) m_nSuitSpacer;
		case tnDummySuitSpacer:
			return (LPVOID) m_nDummySuitSpacer;
		case tpvCursor:
			return (LPVOID) m_hCursor[nIndex1];
		case tpvCursorExchange:
			return (LPVOID) m_hCursorExchange;
		case tpvCursorGrab:
			return (LPVOID) m_hCursorGrab;
		case tpvCursorInvalid:
			return (LPVOID) m_hCursorInvalid[nIndex1];
		case tpvLayoutDeck:
			return (LPVOID) m_layoutDeck;
		case tnAnimationGranularity:
			return (LPVOID) m_nAnimationGranularity;
		case tnCurrentMode:
			return (LPVOID) m_nCurrMode;
		case tnExchangeSrcPlayer:
			return (LPVOID) m_nExchangeSrcPlayer;
		case tnLayoutCardDestination:
			return (LPVOID) m_nLayoutCardDest;
		case tnLayoutCardOrigin:
			return (LPVOID) m_nLayoutCardOrigin;
		case tnPlayerCardLimit:
			return (LPVOID) m_nPlayerCardLimit;
		case tnSuitToScreenIndex:
			return (LPVOID) m_nSuitToScreenIndex[nIndex1];
		case tnDummySuitToScreenIndex:
			return (LPVOID) m_nDummySuitToScreenIndex[nIndex1];
		case tnumCardsUnassigned:
			return (LPVOID) m_numCardsUnassigned;
		case tpvExchangeCard:
			return (LPVOID) m_pExchangeCard;
		case tpvLayoutCard:
			return (LPVOID) m_pLayoutCard;
		case tpLayout:
			return (nIndex2==0)? (LPVOID) m_pointLayout.x :
				   (nIndex2==1)? (LPVOID) m_pointLayout.y : NULL;
		case tpLayoutCardOffset:
			return (nIndex2==0)? (LPVOID) m_pointLayoutCardOffset.x :
				   (nIndex2==1)? (LPVOID) m_pointLayoutCardOffset.y : NULL;
		case trCardDest:
			return (nIndex2==0)? (LPVOID) m_rectCardDest[nIndex1].left :
				   (nIndex2==1)? (LPVOID) m_rectCardDest[nIndex1].top :
				   (nIndex2==2)? (LPVOID) m_rectCardDest[nIndex1].right :
				   (nIndex2==3)? (LPVOID) m_rectCardDest[nIndex1].bottom : NULL;
		case trLayout:
			return (nIndex2==0)? (LPVOID) m_rectLayout.left :
				   (nIndex2==1)? (LPVOID) m_rectLayout.top :
				   (nIndex2==2)? (LPVOID) m_rectLayout.right :
				   (nIndex2==3)? (LPVOID) m_rectLayout.bottom : NULL;
		case trSuitOffset:
			return (nIndex3==0)? (LPVOID) m_rectSuitOffset[nIndex1][nIndex2].left :
				   (nIndex3==1)? (LPVOID) m_rectSuitOffset[nIndex1][nIndex2].top :
				   (nIndex3==2)? (LPVOID) m_rectSuitOffset[nIndex1][nIndex2].right :
				   (nIndex3==3)? (LPVOID) m_rectSuitOffset[nIndex1][nIndex2].bottom : NULL;
		case trTable:
			return (nIndex2==0)? (LPVOID) m_rectTable.left :
				   (nIndex2==1)? (LPVOID) m_rectTable.top :
				   (nIndex2==2)? (LPVOID) m_rectTable.right :
				   (nIndex2==3)? (LPVOID) m_rectTable.bottom : NULL;
		case tszBackgroundBitmap:
			return (LPVOID) (LPCTSTR) m_strBackgroundBitmap;
		case tpvTableRegion:
			return (LPVOID) &m_tableRegion;
		case tnColorDepth:
			return (LPVOID) &m_nColorDepth;
		case tnScreenWidth:
			return (LPVOID) &m_nScreenWidth;
		case tnScreenHeight:
			return (LPVOID) &m_nScreenHeight;
		default:
			AfxMessageBox("Unhandled Call to CEasyBView::GetValuePV()");
			return NULL;
	}
	return NULL;
}

//
int CEasyBView::SetValuePV(int nItem, LPVOID value, int nIndex1, int nIndex2, int nIndex3)
{
	int nVal = (int) value;
	BOOL bVal = (BOOL) value;
	LPCTSTR sVal = (LPCTSTR) value;
	//
	switch(nItem)
	{
		case tbAnimateCards:
			m_bAnimateCards = bVal;
			break;
		case tbAutoRestackCards:
			m_bAutoRestackCards = bVal;
			break;
		case tbBitmapAvailable:
			m_bBitmapAvailable = bVal;
			break;
		case tbCanDisplayBitmap:
			m_bCanDisplayBitmap = bVal;
			break;
		case tbExchanging:
			m_bExchanging = bVal;
			break;
		case tbLayoutCardSelected:
			m_bLayoutCardSelected = bVal;
			break;
		case tbLayoutDialogActive:
			m_bLayoutDlgActive = bVal;
			break;
		case tbOffsetVerticalSuits:
			m_bOffsetVertSuits = bVal;
			break;
		case tbTripleBuffer:
			m_bTripleBuffer = bVal;
			break;
		case tbViewInitialized:
			m_bViewInitialized = bVal;
			break;
		case tbOpeningSequenceDone:
			m_bOpeningSequenceDone = bVal;
			break;
		case tpCardDest:
			if (nIndex2 == 0)
				m_cardDest[nIndex1].x = nVal;
			else if (nIndex2 == 1)
				m_cardDest[nIndex1].y = nVal;
			break;
		case tpvCustomDIB:
		case tpvDefaultBitmap:
			break;
		case tpDrawPoint:
			if (nIndex2 == 0)
				m_drawPoint[nIndex1].x = nVal;
			else if (nIndex2 == 1)
				m_drawPoint[nIndex1].y = nVal;
			break;
		case tpDummyDrawOffset:
			if (nIndex2 == 0)
				m_dummyDrawOffset[nIndex1].x = nVal;
			else if (nIndex2 == 1)
				m_dummyDrawOffset[nIndex1].y = nVal;
			break;
		case tpDummyLabelOffset:
			if (nIndex2 == 0)
				m_dummyLabelOffset[nIndex1].x = nVal;
			else if (nIndex2 == 1)
				m_dummyLabelOffset[nIndex1].y = nVal;
			break;
		case tnCardWidth:
			m_nCardWidth = nVal;
			break;
		case tnCardHeight:
			m_nCardHeight = nVal;
			break;
		case tnCardXGap:
			m_nCardXGap = nVal;
			break;
		case tnCardYGap:
			m_nCardYGap = nVal;
			break;
		case tnOverlapYOffset:
			m_nOverlapYOffset = nVal;
			break;
		case tnSuitSpacer:
			m_nSuitSpacer = nVal;
			break;
		case tnDummySuitSpacer:
			m_nDummySuitSpacer = nVal;
			break;
		case tpvCursor:
		case tpvCursorExchange:
		case tpvCursorGrab:
		case tpvCursorInvalid:
		case tpvLayoutDeck:
			break;
		case tnAnimationGranularity:
			m_nAnimationGranularity = nVal;
			break;
		case tnCurrentMode:
			SetCurrentMode((ScreenMode) nVal);
			break;
		case tnExchangeSrcPlayer:
			m_nExchangeSrcPlayer = nVal;
			break;
		case tnLayoutCardDestination:
			m_nLayoutCardDest = (Position) nVal;
			break;
		case tnLayoutCardOrigin:
			m_nLayoutCardOrigin = (Position) nVal;
			break;
		case tnPlayerCardLimit:
			m_nPlayerCardLimit = nVal;
			break;
		case tnSuitToScreenIndex:
			m_nSuitToScreenIndex[nIndex1] = nVal;
			break;
		case tnDummySuitToScreenIndex:
			m_nDummySuitToScreenIndex[nIndex1] = nVal;
			break;
		case tnumCardsUnassigned:
			m_numCardsUnassigned = nVal;
			break;
		case tpvExchangeCard:
			break;
		case tpLayout:
			if (nIndex2 == 0)
				m_pointLayout.x = nVal;
			else if (nIndex2 == 1)
				m_pointLayout.y = nVal;
			break;
		case tpLayoutCardOffset:
			if (nIndex2 == 0)
				m_pointLayoutCardOffset.x = nVal;
			else if (nIndex2 == 1)
				m_pointLayoutCardOffset.y = nVal;
			break;
		case trCardDest:
			if (nIndex2 == 0)
				m_rectCardDest[nIndex1].left = nVal;
			else if (nIndex2 == 1)
				m_rectCardDest[nIndex1].top = nVal;
			else if (nIndex2 == 2)
				m_rectCardDest[nIndex1].right = nVal;
			else if (nIndex2 == 3)
				m_rectCardDest[nIndex1].bottom = nVal;
			break;
		case trLayout:
			if (nIndex2 == 0)
				m_rectLayout.left = nVal;
			else if (nIndex2 == 1)
				m_rectLayout.top = nVal;
			else if (nIndex2 == 2)
				m_rectLayout.right = nVal;
			else if (nIndex2 == 3)
				m_rectLayout.bottom = nVal;
			break;
		case trSuitOffset:
			if (nIndex3 == 0)
				m_rectSuitOffset[nIndex1][nIndex2].left = nVal;
			else if (nIndex3 == 1)
				m_rectSuitOffset[nIndex1][nIndex2].top = nVal;
			else if (nIndex3 == 2)
				m_rectSuitOffset[nIndex1][nIndex2].right = nVal;
			else if (nIndex3 == 3)
				m_rectSuitOffset[nIndex1][nIndex2].bottom = nVal;
			break;
		case trTable:
			if (nIndex2 == 0)
				m_rectTable.left = nVal;
			else if (nIndex2 == 1)
				m_rectTable.top = nVal;
			else if (nIndex2 == 2)
				m_rectTable.right = nVal;
			else if (nIndex2 == 3)
				m_rectTable.bottom = nVal;
			break;
		case tszBackgroundBitmap:
		{
			// see if this is a test
			if (!SetBackgroundBitmap(sVal, (BOOL) nIndex1))
				return -1;
			break;
		}
		case tpvTableRegion:
			break;
		case tnColorDepth:
			break;
		case tnScreenWidth:
			break;
		case tnScreenHeight:
			break;
		default:
			AfxMessageBox("Unhandled Call to CEasyBView::SetValuePV()");
			return 1;
	}
	return 0;
}

//
int CEasyBView::GetValue(int nItem, int nIndex1, int nIndex2, int nIndex3)
{
	return (int) GetValuePV(nItem, nIndex1, nIndex2, nIndex3);
}

// 
LPCTSTR CEasyBView::GetValueString(int nItem, int nIndex1, int nIndex2, int nIndex3)
{
	return (LPCTSTR) GetValuePV(nItem, nIndex1, nIndex2, nIndex3);
}

//
int CEasyBView::SetValue(int nItem, int nValue, int nIndex1, int nIndex2, int nIndex3)
{
	return SetValuePV(nItem, (LPVOID)nValue, nIndex1, nIndex2, nIndex3);
}

//
//
//
void CEasyBView::OnInitialUpdate() 
{
	// though this function may be called many times (once per game),
	// teh below initialization should only be performed once
	if (!m_bResourcesInitialized) 
	{
		// Load cursors
		m_hCursor[SOUTH] = theApp.LoadCursor(IDC_CURSOR_SOUTH);
		m_hCursor[WEST] = theApp.LoadCursor(IDC_CURSOR_WEST);
		m_hCursor[NORTH] = theApp.LoadCursor(IDC_CURSOR_NORTH);
		m_hCursor[EAST] = theApp.LoadCursor(IDC_CURSOR_EAST);
		//
		m_hCursorInvalid[SOUTH] = theApp.LoadCursor(IDC_CURSOR_SOUTH_INVALID);
		m_hCursorInvalid[WEST] = theApp.LoadCursor(IDC_CURSOR_WEST_INVALID);
		m_hCursorInvalid[NORTH] = theApp.LoadCursor(IDC_CURSOR_NORTH_INVALID);
		m_hCursorInvalid[EAST] = theApp.LoadCursor(IDC_CURSOR_EAST_INVALID);
		//
		m_hCursorPlay[SOUTH] = theApp.LoadCursor(IDC_CURSOR_PLAY_SOUTH);
		m_hCursorPlay[WEST] = theApp.LoadCursor(IDC_CURSOR_PLAY_WEST);
		m_hCursorPlay[NORTH] = theApp.LoadCursor(IDC_CURSOR_PLAY_NORTH);
		m_hCursorPlay[EAST] = theApp.LoadCursor(IDC_CURSOR_PLAY_EAST);
		//
		m_hCursorClick = theApp.LoadCursor(IDC_CURSOR_CLICK);
		m_hCursorGrab = theApp.LoadCursor(IDC_CURSOR_GRAB);
		m_hCursorExchange = theApp.LoadCursor(IDC_CURSOR_EXCHANGE);
		m_hCursorHand = theApp.LoadCursor(IDC_CURSOR_HAND);
		m_hCursorHandIllegal = theApp.LoadCursor(IDC_CURSOR_HAND_ILLEGAL);
		m_hCursorBusy = theApp.LoadStandardCursor(IDC_WAIT);
		m_hDefaultCursor = theApp.LoadStandardCursor(IDC_ARROW);
		//
		m_bResourcesInitialized = TRUE;
	}
	// define standard cursor
	SetWindowCursor(m_hDefaultCursor);
	//
	CView::OnInitialUpdate();
}



/////////////////////////////////////////////////////////////////////////////
// CEasyBView diagnostics

#ifdef _DEBUG
void CEasyBView::AssertValid() const
{
	CView::AssertValid();
}

void CEasyBView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEasyBDoc* CEasyBView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEasyBDoc)));
	return (CEasyBDoc*)m_pDocument;
}
#endif //_DEBUG








////////////////////////////////////////////////////////////////////////////
//
//
// Routines
//
//
//
////////////////////////////////////////////////////////////////////////////




//
// SetCurrentMode()
//
// called both from iniude & outside 
//
void CEasyBView::SetCurrentMode(int nMode) 
{
	// sanity check
	if (!this || !m_bViewInitialized)
		return;

	// special test -- if we're clearing the mode, and we were 
	// in the middle of editing, restore dialogs
	if ((nMode == MODE_NONE) && 
		(m_nCurrMode == MODE_CARDLAYOUT) || (m_nCurrMode == MODE_EDITHANDS))
		pMAINFRAME->RestoreAllDialogs();

	//
	m_nCurrMode = (ScreenMode)nMode;
	SetWindowCursor();
}

//
// SetCurrentModeTemp()
//
void CEasyBView::SetCurrentModeTemp(int nMode) 
{
	m_nOldMode = m_nCurrMode;
	SetCurrentMode(nMode);
}

//
// RestoreMode()
//
void CEasyBView::RestoreMode() 
{
	SetCurrentMode(m_nOldMode);
}




//
// ClearMode()
//
// called to clear the current view mode
//
void CEasyBView::ClearMode(BOOL bRedraw) 
{
	if (!this || !m_bViewInitialized)
		return;

	// special test -- if we're clearing the mode, and we were in the middle 
	// of editing, restore dialogs
	if (!bRedraw)
		SuppressRefresh();
	if ((m_nCurrMode == MODE_CARDLAYOUT) || (m_nCurrMode == MODE_EDITHANDS))
		pMAINFRAME->RestoreAllDialogs();
	if (!bRedraw)
		EnableRefresh();
	//
	if (bRedraw)
	{
		pMAINFRAME->GetBidDialog()->ShowWindow(SW_HIDE);
		Invalidate();
	}
	//
	SetCurrentMode(MODE_NONE);
	m_nOldMode = MODE_NONE;
}






//
//---------------------------------------------------------
//



void CEasyBView::OnActivateView(BOOL bActivate, CView* pActivateView,
									CView* pDeactiveView)
{
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	//
	if (bActivate)
	{
		ASSERT(pActivateView == this);
		OnDoRealize();
		// make sure the bidding dialog is showing
		if (theApp.IsBiddingInProgress()) 
			pMAINFRAME->MakeDialogVisible(twBidDialog);
	}
}


//
// Command processing
//
BOOL CEasyBView::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// this mechanism is outdated, but keep it around for now
	switch(wParam) 
	{
		case WMS_RESET_DISPLAY:
			// user has changed display options
			ResetSuitSequence();
			ResetSuitOffsets();
			SetViewParameters();
			if (lParam)
			{
				m_nSuppressRefresh = 0;
				Invalidate();
				UpdateWindow();
			}
			pMAINFRAME->GetDialog(twLayoutDialog)->SendMessage(WM_COMMAND,WMS_RESET_DISPLAY);
			return TRUE;

		case WMS_UPDATE_DISPLAY:
			Invalidate();
			return TRUE;

		case WMS_REFRESH_DISPLAY:
			OnRefreshScreen();
			return TRUE;

		case WMS_CARD_PLAY:
			// a computer(?) player has picked a card to play
			HandleCardPlay((CCard*)lParam);
			return TRUE;

		case WMS_CARD_PLAY + 1000:
			// a computer(?) player has picked a card to play
			HandleCardPlay((CCard*)lParam);
			return TRUE;

		case WMS_CARD_PLAY + 1001:
			// a computer(?) player has picked a card to play
			HandleCardPlay((CCard*)lParam);
			return TRUE;

		case WMS_CARD_PLAY + 1002:
			// a computer(?) player has picked a card to play
			HandleCardPlay((CCard*)lParam);
			return TRUE;

		case WMS_CARD_PLAY + 1003:
			// a computer(?) player has picked a card to play
			HandleCardPlay((CCard*)lParam);
			return TRUE;

		case WMS_PLAY_RESTART:
			// the play has been restarted
			RestartPlay();
			return TRUE;

		case WMS_LAYOUT_DONE:
			// done with layout dialog; show table if cards left
			m_bLayoutDlgActive = FALSE;
			if (m_numCardsUnassigned > 0) 
			{
				CDC* pDC = GetDC();
				DrawCardLayoutMode(pDC,TRUE,FALSE);
				ReleaseDC(pDC);
			}
			else
			{
				// if in layout mode, toggle 
				// else if in edit mode, continue
				if (m_nCurrMode == MODE_CARDLAYOUT)
					OnLayoutCards();	
			}
			return TRUE;

		case WMS_INITNEWDEAL:
			/// force immediate update
			// ### TEMP ###
//			if (lParam)
//				UpdateWindow();
			// then update status display
//			pMAINFRAME->ClearStatusWindow();
			// hide the card layout dialog, just in case
			((CCardLayoutDialog*) pMAINFRAME->GetDialog(twLayoutDialog))->ShowWindow(SW_HIDE);
			// and bid immediately if so ordered
			if (theApp.GetValue(tbAutoBidStart)) 
			{
				OnBidCurrentHand();
			} 
			else 
			{
				SetCurrentMode(MODE_WAITTOBID);
				pMAINFRAME->SetStatusText(IDS_WAITTOBID);
			}
			return TRUE;

		case WMS_BIDDING_DONE:
			BiddingComplete(TRUE);
			break;

		case WMS_BIDDING_CANCELLED:
			BiddingComplete(FALSE);
			break;

		case WMS_FILE_LOADED:
			GameLoaded();
			break;

		case WMS_RESUME_GAME:
			ResumeLoadedGame();
			break;

		case WMS_BIDDING_RESTART: 
			pMAINFRAME->GetDialog(twBidDialog)->PostMessage(WM_COMMAND, WMS_BIDDING_RESTART, 0L);
			return TRUE;

		case WMS_UPDATE:
			Invalidate();
			return TRUE;

		case WMS_DOREALIZE:
			OnDoRealize();
			return TRUE;

		case WMS_FLUSH_MESSAGES:
			return TRUE;
	}

	// call default handler
	return CView::OnCommand(wParam, lParam);
}



//
// Message processing
//
bool CEasyBView::Notify(long lCode, long param1, long param2, long param3) 
{
	//
	if (lCode != WM_COMMAND)
		return false;

	//
	switch(param1) 
	{
		case WMS_RESET_DISPLAY:
			// user has changed display options
			ResetSuitSequence();
			ResetSuitOffsets();
			SetViewParameters();
			if (param2)
			{
				m_nSuppressRefresh = 0;
				Invalidate();
				UpdateWindow();
			}
			pMAINFRAME->GetDialog(twLayoutDialog)->SendMessage(WM_COMMAND,WMS_RESET_DISPLAY);
			return true;

		case WMS_UPDATE_DISPLAY:
			Invalidate();
			return true;

		case WMS_REFRESH_DISPLAY:
			OnRefreshScreen();
			return true;

		case WMS_CARD_PLAY:
			// a computer(?) player has picked a card to play
			HandleCardPlay((CCard*)param2);
			return true;

		case WMS_CARD_PLAY + 1000:
			// a computer(?) player has picked a card to play
			HandleCardPlay((CCard*)param2);
			return true;

		case WMS_CARD_PLAY + 1001:
			// a computer(?) player has picked a card to play
			HandleCardPlay((CCard*)param2);
			return true;

		case WMS_CARD_PLAY + 1002:
			// a computer(?) player has picked a card to play
			HandleCardPlay((CCard*)param2);
			return true;

		case WMS_CARD_PLAY + 1003:
			// a computer(?) player has picked a card to play
			HandleCardPlay((CCard*)param2);
			return true;

		case WMS_PLAY_RESTART:
			// the play has been restarted
			RestartPlay();
			return true;

		case WMS_LAYOUT_DONE:
			// done with layout dialog; show table if cards left
			m_bLayoutDlgActive = FALSE;
			if (m_numCardsUnassigned > 0) 
			{
				CDC* pDC = GetDC();
				DrawCardLayoutMode(pDC,TRUE,FALSE);
				ReleaseDC(pDC);
			}
			else
			{
				// if in layout mode, toggle 
				// else if in edit mode, continue
				if (m_nCurrMode == MODE_CARDLAYOUT)
					OnLayoutCards();	
			}
			return true;

		case WMS_INITNEWDEAL:
			/// force immediate update
			// ### TEMP ###
//			if (param2)
//				UpdateWindow();
			// then update status display
//			pMAINFRAME->ClearStatusWindow();
			// hide the card layout dialog, just in case
			((CCardLayoutDialog*) pMAINFRAME->GetDialog(twLayoutDialog))->ShowWindow(SW_HIDE);
			// and bid immediately if so ordered
			if (theApp.GetValue(tbAutoBidStart)) 
			{
				OnBidCurrentHand();
			} 
			else 
			{
				SetCurrentMode(MODE_WAITTOBID);
				pMAINFRAME->SetStatusText(IDS_WAITTOBID);
			}
			return true;

		case WMS_BIDDING_DONE:
			BiddingComplete(TRUE);
			return true;

		case WMS_BIDDING_CANCELLED:
			BiddingComplete(FALSE);
			return true;

		case WMS_FILE_LOADED:
			GameLoaded();
			return true;

		case WMS_RESUME_GAME:
			ResumeLoadedGame();
			return true;

		case WMS_BIDDING_RESTART: 
			pMAINFRAME->GetDialog(twBidDialog)->PostMessage(WM_COMMAND, WMS_BIDDING_RESTART, 0L);
			return true;

		case WMS_UPDATE:
			Invalidate();
			return true;

		case WMS_DOREALIZE:
			OnDoRealize();
			return true;

		case WMS_FLUSH_MESSAGES:
			return true;
	}

	// not handled
	return false;
}




/////////////////////////////////////////////////////////////////////////////
// CEasyBView message handlers



//
void CEasyBView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// clear exchange card, is active
	if ((nChar == VK_ESCAPE) && (m_bExchanging)) 
	{
		// found a card to exchange
		ASSERT(m_pExchangeCard != NULL);
		// clear the highlighted card
		CDC *pDC = GetDC();
		m_pExchangeCard->DrawHighlighted(pDC,FALSE);
		ReleaseDC(pDC);
		SetWindowCursor();	// reset cursor
		// done
		ClearCardExchangeMode();
		// done
//		ClearPrompt();
		ReleaseCapture();
		SetPrompt(szLayoutDonePrompt);
	}
	//	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}




//
BOOL inline CEasyBView::InRect(CPoint point, CCard* pCard)
{
	CRect rect(pCard->GetXPosition(),pCard->GetYPosition(),
				pCard->GetXPosition() + m_nCardWidth,
				pCard->GetYPosition() + m_nCardHeight);
	return rect.PtInRect(point);
}


//
void CEasyBView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int i,j,nSuit;
	CString strTemp;
	CCard* pCard = NULL;
	Position nNext,nOrigin;
	int nState = GetKeyState(VK_CONTROL);
	BOOL bFound = FALSE;
	BOOL bCtrlKeyPressed = (nState & 0x8000)? TRUE : FALSE;

	// see what mode we're in
	switch (m_nCurrMode) 
	{
		case MODE_TITLESCREEN:
/*
			// see if this is the first time running --
			// if so, show the intro screen
			if (theApp.GetValue(tbFirstTimeRunning))
			{
				::Sleep(500);
				SetCurrentMode(MODE_INTROSCREEN); 
				m_pIntroWnd = new CIntroWnd;
				m_pIntroWnd->Create(CIntroWnd::IDD, this);
				m_pIntroWnd->ShowWindow(SW_SHOW);
				m_pIntroWnd->UpdateWindow();
			}
			else
			{
				// go ahead and mark the view as ready
				m_bViewInitialized = TRUE;
			}
			Invalidate();
			ResetPrompt();
			SetCurrentMode(MODE_NONE);
*/
			return;

		case MODE_INTROSCREEN:
			// intro screen  --
			// do nothing; let the dialog handle events
			return;

		case MODE_CLICKFORNEXTTRICK:
			// trick is over
			ClearTable();
			pDOC->ClearTrick();
			return;

		case MODE_CLICKTOBEGINPLAY:
			// ready to begin play
			BeginPlay();
			return;

		case MODE_CLICKTORESUMEPLAY:
			// ready to resume play
			AdvanceToNextPlayer();
			return;

		case MODE_CLICKTORESTARTTRICK:
			// ready to proceed afer taking trick back
			AdvanceToNextPlayer();
			return;

		case MODE_CLICKFORNEXTGAME:
			// let the doc move on to the next game
			pDOC->PostProcessGame();
			return;

		case MODE_GAMEREVIEW:
		case MODE_GAMERESTORE:
		case MODE_NONE:
			return;

	}

	// if here, we should be waiting for a card seleciton
	if ((m_nCurrMode != MODE_WAITCARDPLAY) &&
		(m_nCurrMode != MODE_CARDLAYOUT) &&
		(m_nCurrMode != MODE_EDITHANDS))
		return;

	// handle if we're selecting a layout from the table
	if ((m_nCurrMode == MODE_CARDLAYOUT) ||
						(m_nCurrMode == MODE_EDITHANDS)) 
	{
		// look for cards on the table
		int nIndex;
		for(i=3;i>=0;i--) 
		{
			nSuit = theApp.GetSuitSequence(i);
			for(j=0;j<13;j++) 
			{
				nIndex = nSuit*13 + j;
				pCard = m_layoutDeck[nIndex];
				if (pCard == NULL)
					continue;
				if ((!pCard->IsCardAssigned()) && (InRect(point, pCard))) 
				{
					bFound = TRUE;
					nOrigin = TABLE;
					break;
				}
			}
			if (bFound)
				break;
		}
	}

	// keep looking if an owner was not found
	if (!bFound)
	{
		// if playing a card, set card selection restrictions
		if (m_nCurrMode == MODE_WAITCARDPLAY) 
		{
			nNext = (Position) pDOC->GetCurrentPlayerPosition();
		} 
		else if ((m_nCurrMode == MODE_CARDLAYOUT) ||
							(m_nCurrMode == MODE_EDITHANDS)) 
		{
			// else picking a player's card to move
			nNext = ANY_POS;
		}

		// the following can change

		// see if a player card has been selected
		int nPlayer;
		bFound = GetCardUnderPoint(nPlayer, pCard, &point);
		nOrigin = (Position) nPlayer;
	}

	//
	// Found a card that the user selected
	//
	if (bFound) 
	{
		// handling depends on mode
		if (m_nCurrMode == MODE_WAITCARDPLAY) 
		{
			// selected a card to play
			// see if it's a legitemate play
			if (!pDOC->TestPlayValidity(nOrigin,pCard))
				return;
			// else handle the card play
			HandleCardPlay(pCard);

		} 
		else if ((m_nCurrMode == MODE_CARDLAYOUT) ||
						(m_nCurrMode == MODE_EDITHANDS)) 
		{

			// laying out or editing hands
			// see if exchange mode is on
			if (m_bExchanging) 
			{
				// found a card to exchange
				ASSERT(m_pExchangeCard != NULL);
				// clear the highlighted card
				CDC *pDC = GetDC();
				m_pExchangeCard->DrawHighlighted(pDC,FALSE);
				int nOwner2 = pCard->GetOwner();
				int nOwner1 = m_pExchangeCard->GetOwner();
				int nSuit2 = m_nSuitToScreenIndex[pCard->GetSuit()];
				int nSuit1 = m_nSuitToScreenIndex[m_pExchangeCard->GetSuit()];
				// and animate the cards to be exchanged
				// insert animate sequence here
				if (nOwner1 != nOwner2) 
				{
					// first remove then cards
					PLAYER(nOwner1).RemoveCardFromHand(m_pExchangeCard);
					PLAYER(nOwner2).RemoveCardFromHand(pCard);
					// then handle differently based on display mode
					if (m_bAnimateCards) 
					{
//						DisplayHand((Position) nOwner1);
//						DisplayHand((Position) nOwner2);

						// temporarily speed up animation
						int nOldAG = m_nAnimationGranularity;
						m_nAnimationGranularity *= 2;
						if (m_nAnimationGranularity <= 0)
							m_nAnimationGranularity = 0;

						// and animate from the 1st hand to the 2nd
						m_pExchangeCard->Animate(pDC,m_rectSuitOffset[nOwner2][nSuit1].left,
									   			     m_rectSuitOffset[nOwner2][nSuit1].top, TRUE);
						// and from the second hand to the first
						pCard->Animate(pDC,m_rectSuitOffset[nOwner1][nSuit2].left,
									   	   m_rectSuitOffset[nOwner1][nSuit2].top, TRUE);
						// and finally exchange the cards			   	
						PLAYER(nOwner1).AddCardToHand(pCard);
						PLAYER(nOwner2).AddCardToHand(m_pExchangeCard);
						DisplayHand((Position) nOwner1);
						DisplayHand((Position) nOwner2);

						// restore animation speed
						m_nAnimationGranularity = nOldAG;
					} 
					else 
					{
						// else use a simpler approach
						PLAYER(nOwner1).AddCardToHand(pCard);
						PLAYER(nOwner2).AddCardToHand(m_pExchangeCard);
						DisplayHand((Position) nOwner1);
						DisplayHand((Position) nOwner2);
					}
				}
				ReleaseDC(pDC);
				ReleaseCapture();
				SetWindowCursor();	// reset cursor
				// done
				m_bExchanging = FALSE;
				m_nExchangeSrcPlayer = NONE;
				m_pExchangeCard = NULL;
				SetPrompt(szLayoutDonePrompt);
			} 
			else if (bCtrlKeyPressed && (nOrigin != TABLE)) 
			{
				// exchanging a card
				SetCapture();
				SetWindowCursor(m_hCursorExchange);
				m_bExchanging = TRUE;
				m_nExchangeSrcPlayer = pCard->GetOwner();
				m_pExchangeCard = pCard;
				int nPos = pCard->GetOwner();
				CDC *pDC = GetDC();
				pCard->DrawHighlighted(pDC,TRUE);
				ReleaseDC(pDC);
				SetPrompt("Click on another card to complete the exchange");
			} 
			else 
			{
				// selected a card to drag
				SetCapture();
				SetWindowCursor(m_hCursorGrab);
				m_bLayoutCardSelected = TRUE;
				m_pLayoutCard = pCard;
				m_pointLayout.x = point.x;
				m_pointLayout.y = point.y;
				CDC *pDC = GetDC();
				m_nLayoutCardOrigin = nOrigin;
				// set default destination
				m_nLayoutCardDest = m_nLayoutCardOrigin;
				if ((nOrigin == TABLE) && (m_bTripleBuffer)) 
				{
					// comment out the following to eliminate
					// main stack update for more speed
					DrawCardLayoutMode(pDC, TRUE, FALSE);
				}
				// see if we grabbed a player's card
				if (pCard->IsCardAssigned()) 
				{
					Position nPos = (Position) pCard->GetOwner();
					PLAYER(nPos).RemoveCardFromHand(pCard);
					DisplayHand(nPos);
				}
				// first adjust for card/point offset
				m_pointLayoutCardOffset.x = point.x - pCard->GetXPosition();
				m_pointLayoutCardOffset.y = point.y - pCard->GetYPosition();
				// now display the card
//				pCard->MoveTo(pDC,point.x,point.y);
				pCard->MoveTo(pDC,point.x - m_pointLayoutCardOffset.x,
								  point.y - m_pointLayoutCardOffset.y);
				ReleaseDC(pDC);
				SetPrompt("Drag a card in a player's direction");
			}
		}
	}

	//
	CView::OnLButtonDown(nFlags, point);
}



//
// GetCardUnderPoint()
//
BOOL CEasyBView::GetCardUnderPoint(int& nPlayer, CCard*& pCard, CPoint* pTargetPoint)
{
	BOOL bFound = FALSE;
	//
	CPoint point;
	if (pTargetPoint == NULL)
		GetCursorPos(&point);
	else
		point = *pTargetPoint;

	//
	for(nPlayer=SOUTH;nPlayer<=EAST;nPlayer++) 
	{
		// check the current player's cards
		for(int i=3;i>=0;i--) 
		{
			// get the proper suit sequence
			int nSuit;
			if (PLAYER(nPlayer).IsDummy())
				nSuit = theApp.GetDummySuitSequence(i);
			else
				nSuit = theApp.GetSuitSequence(i);
			//
			if (((nPlayer == EAST) || (nPlayer == SOUTH))
							 && (PLAYER(nPlayer).IsDummy())) 
			{
				// if E/S dummy, search from high card down
				for(int j=0;j<PLAYER(nPlayer).GetNumCardsInSuit(nSuit);j++) 
				{
					pCard = PLAYER(nPlayer).GetCardInSuit(nSuit,j);
					if (InRect(point, pCard)) 
					{
						bFound = TRUE;
						break;
					}
				}
			} 
			else 
			{
				// else search from low card up
				for(int j=PLAYER(nPlayer).GetNumCardsInSuit(nSuit)-1;j>=0;j--) 
				{
					pCard = PLAYER(nPlayer).GetCardInSuit(nSuit,j);
					if (InRect(point, pCard)) 
					{
						bFound = TRUE;
						break;
					}
				}
			}
			//
			if (bFound)
				return TRUE;
		}
		//
		if (bFound) 
				return TRUE;
	}

	// no card found
	return FALSE;
}



//
// OnMouseMove()
//
void CEasyBView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// see if we're dragging a card
	if ((m_bLayoutCardSelected) && (m_pLayoutCard)) 
	{

		//
		CCard* pCard = m_pLayoutCard;
		CDC *pDC = GetDC();
		BOOL bDirectionSet = FALSE;
		// first see if we're returning a card to the stack
		if ((m_nLayoutCardOrigin > UNKNOWN) &&
					(m_nLayoutCardOrigin < TABLE) &&
								(!m_bLayoutDlgActive)) 
		{
			CRect rect(&m_rectLayout);
			if (rect.PtInRect(point)) 
			{
				m_nLayoutCardDest = TABLE;				
				bDirectionSet = TRUE;
			}
		} 
		if (!bDirectionSet) 
		{
			// else determine drag direction
			int dx = point.x - m_pointLayout.x;
			int dy = point.y - m_pointLayout.y;
			if (ABS(dx) > ABS(dy)) 
			{
				// horiz displacement
				if (dx < 0)
					m_nLayoutCardDest = WEST;		
				else if (dx > 0)
					m_nLayoutCardDest = EAST;		
				else
					m_nLayoutCardDest = UNKNOWN;		
//					m_nLayoutCardDest = TABLE;		
			} 
			else 
			{
				// vert displacement
				if (dy < 0)
					m_nLayoutCardDest = NORTH;		
				else if (dy > 0)
					m_nLayoutCardDest = SOUTH;		
				else
					m_nLayoutCardDest = UNKNOWN;		
//					m_nLayoutCardDest = TABLE;		
			}
		}
		m_pointLayout = point;
//		pCard->DragTo(pDC,point.x,point.y);
		pCard->DragTo(pDC,point.x - m_pointLayoutCardOffset.x,
						  point.y - m_pointLayoutCardOffset.y);
		// now set cursor
		switch (m_nLayoutCardDest) 
		{
			case NORTH:
				if (PLAYER(NORTH).GetNumCards() < m_nPlayerCardLimit)
					SetWindowCursor(m_hCursor[NORTH]);
				else
					SetWindowCursor(m_hCursorInvalid[NORTH]);
				break;
			case EAST:
				if (PLAYER(EAST).GetNumCards() < m_nPlayerCardLimit)
					SetWindowCursor(m_hCursor[EAST]);
				else
					SetWindowCursor(m_hCursorInvalid[EAST]);
				break;
			case WEST:
				if (PLAYER(WEST).GetNumCards() < m_nPlayerCardLimit)
					SetWindowCursor(m_hCursor[WEST]);
				else
					SetWindowCursor(m_hCursorInvalid[WEST]);
				break;
			case SOUTH:
				if (PLAYER(SOUTH).GetNumCards() < m_nPlayerCardLimit)
					SetWindowCursor(m_hCursor[SOUTH]);
				else
					SetWindowCursor(m_hCursorInvalid[SOUTH]);
				break;
			case UNKNOWN:
			case TABLE:
				SetWindowCursor(m_hCursorGrab);
				break;
		}
		// and drag card
//		pCard->DragTo(pDC,point.x,point.y);
		ReleaseDC(pDC);
		SetPrompt("Release mouse button to assign card");
	}
	else
	{
		// else we're not dragging a card
		// see if we're waiting for a mouse click from the user
		if (!theApp.IsGameInProgress())
			return;
		CPlayer* pPlayer = pDOC->GetCurrentPlayer();
		int nCurrPlayerPos = pPlayer->GetPosition();
		int nPlayMode = theApp.GetValue(tnCardPlayMode);
		BOOL bManualPlay;
		if ( (pPlayer && pPlayer->IsHumanPlayer()) ||
			 (nPlayMode == CEasyBApp::PLAY_MANUAL) ||
		     ((nPlayMode == CEasyBApp::PLAY_MANUAL_DEFEND) && (pDOC->GetCurrentPlayer()->IsDefending())) )
			bManualPlay = TRUE;
		else
			bManualPlay = FALSE;

		// see if we're playing waiting for a card to be played
		if ((m_nCurrMode == MODE_WAITCARDPLAY) && bManualPlay)
		{
			// set mouse cursor depending on position
			int nPos = pPlayer->GetPosition();
			CRect rect;
			int nSuitLed = NONE;
			GetHandBoundsRect((Position)nPos, rect);
			if (rect.PtInRect(point))
			{
				// in the right hand; see if it's on the right suit
				BOOL bAnySuit = FALSE;
				// check if we're leading or following suit
				if (pDOC->GetNumCardsPlayedInRound() == 0)
				{
					// player can lead any suit
					bAnySuit = TRUE;
				}
				else
				{
					// player must follow suit if possible
					CCard* pLeadCard = pDOC->GetCurrentTrickCardLed();
					ASSERT(pLeadCard);
					nSuitLed = pLeadCard->GetSuit();				
					if (pPlayer->GetNumCardsInSuit(nSuitLed) == 0)
						bAnySuit = TRUE;
				}
				//
				if (bAnySuit)
				{
					// any suit is OK
					SetWindowCursor(m_hCursorHand);
				}
				else
				{
					// only the suit led is OK
					// search for the card the cursor is currently over
					CCard* pCard;
					int nPlayer;
					if (GetCardUnderPoint(nPlayer, pCard, &point))
					{
						if (pCard->GetSuit() == nSuitLed)
							SetWindowCursor(m_hCursorHand);
						else
							SetWindowCursor(m_hCursorHandIllegal);
					}
					else
					{
						// not over a card (maybe a gap), so reset cursor
						SetWindowCursor();
					}
				}
			}
			else
			{
				// not in the player's hand
//				SetWindowCursor(m_hDefaultCursor);
				SetWindowCursor(m_hCursorPlay[nCurrPlayerPos]);
			}
		}
		else
		{
			// reset cursor
			SetWindowCursor();
		}
	}

	//done
	CView::OnMouseMove(nFlags, point);
}




void CEasyBView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// see if we were dragging a card
	int nIndex;
	if (m_bLayoutCardSelected) 
	{
		CDC *pDC = GetDC();
		// first restore cursor
		SetWindowCursor();
		// assign card
		CCard* pCard = m_pLayoutCard;
		ASSERT(pCard != NULL);
		pCard->RestoreBackground(pDC);
		// clear the following now for correct operation of 
		// DrawCardLayoutMode()
		m_pLayoutCard = NULL;
		//
		BOOL bDestValid = TRUE;
		// check limits
		if ( ((m_nLayoutCardDest == NORTH) && (PLAYER(NORTH).GetNumCards() >= 13)) ||
		     ((m_nLayoutCardDest == EAST) && (PLAYER(EAST).GetNumCards() >= 13)) ||
		     ((m_nLayoutCardDest == SOUTH) && (PLAYER(SOUTH).GetNumCards() >= 13)) ||
		     ((m_nLayoutCardDest == WEST) && (PLAYER(WEST).GetNumCards() >= 13)) ||
			 (m_nLayoutCardDest == UNKNOWN) )
			 bDestValid = FALSE;
		// check holdings
		if (bDestValid)
		{
			if (m_nLayoutCardDest < TABLE)
			{
				// stick in a player's hand
				PLAYER(m_nLayoutCardDest).AddCardToHand(pCard);
				nIndex = pCard->GetSuit()*13 + pCard->GetFaceValue()-2;
				m_layoutDeck[nIndex] = NULL;
				if (m_nLayoutCardOrigin == TABLE)
					m_numCardsUnassigned--;
				// insert animate sequence here
				int nSuit = m_nSuitToScreenIndex[pCard->GetSuit()];
				int x = m_rectSuitOffset[m_nLayoutCardDest][nSuit].left;
				int y = m_rectSuitOffset[m_nLayoutCardDest][nSuit].top;
				if ((x < 0) || (y < 0)) 
				{
					x = m_drawPoint[m_nLayoutCardDest].x;
					y = m_drawPoint[m_nLayoutCardDest].y;
				}
				if ((m_nLayoutCardDest != m_nLayoutCardOrigin) && 
											     (m_bAnimateCards)) 
				{
					pCard->Animate(pDC,x,y,TRUE);
				}
				// and draw destination hand
				DisplayHand((Position)m_nLayoutCardDest);
				// and draw table if needed
				if ((m_nLayoutCardOrigin == TABLE) && (!m_bTripleBuffer))
					DrawCardLayoutMode(pDC, TRUE, FALSE);
			} 
			else 
			{
				// stick card back in the table pile
				nIndex = pCard->GetSuit()*13 + pCard->GetFaceValue()-2;
				m_layoutDeck[nIndex] = pCard;
				pCard->ClearAssignment();
				if (m_nLayoutCardOrigin != TABLE)
					m_numCardsUnassigned++;
				// and draw table only if necessary
				if ((m_nLayoutCardOrigin != TABLE) ||
				    ((m_nLayoutCardOrigin == TABLE) && (m_bTripleBuffer)) )
					DrawCardLayoutMode(pDC, TRUE, FALSE);
				// and update assign dialog if necessary
				if (m_bLayoutDlgActive)
					pMAINFRAME->GetDialog(twLayoutDialog)->PostMessage(WM_COMMAND,WMS_RESET_DISPLAY);
			}
		} 
		else 
		{
			// restore source
			if (m_nLayoutCardOrigin == TABLE) 
			{
				DrawCardLayoutMode(pDC, TRUE, FALSE);
			} 
			else if (m_nLayoutCardOrigin > UNKNOWN) 
			{
				PLAYER(m_nLayoutCardOrigin).AddCardToHand(pCard);
				DisplayHand((Position)m_nLayoutCardOrigin);
			}
		}
		// and reset
		m_bLayoutCardSelected = FALSE;
		m_nLayoutCardDest = UNKNOWN;
		m_nLayoutCardOrigin = UNKNOWN;
		m_pointLayout.x = -1;
		m_pointLayout.y = -1;
		ReleaseDC(pDC);
		SetPrompt(szLayoutDonePrompt);
	}
	ReleaseCapture();
	//
	CView::OnLButtonUp(nFlags, point);
}


//
void CEasyBView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	AfxMessageBox("Double-Clicked");
	CView::OnLButtonDblClk(nFlags, point);
}


//
void CEasyBView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (m_nCurrMode == MODE_TITLESCREEN) 
	{
		Invalidate();
		return;
	}

	// see if Ctrl key is down
	int nState = GetKeyState(VK_CONTROL);
	if (!(nState & 0x8000)) 
	{
		// if not, call MainFrame handler
		pMAINFRAME->PostMessage(WM_RBUTTONDOWN, 0L, 0L);
//		pMAINFRAME->OnRButtonDown(nFlags, point);
		return;
	}

	int i,j,nSuit;
	CString strTemp;
	CCard* pCard = NULL;
	BOOL bFound = FALSE;
	int minX,maxX,minY,maxY;
	Position nNext,nPlayer,nDummy,nEast,nWest,nOrigin;

	//
	// #######################
	//   TEMP 
	// #######################
	// the following can change
	nPlayer = SOUTH;
	nDummy = NORTH;
	nWest = WEST;
	nEast = EAST;
	nNext = (Position) pDOC->GetCurrentPlayerPosition();

	// see if a player card has been selected
	minX = m_drawPoint[nPlayer].x;
	maxX = 640;
	minY = m_drawPoint[nPlayer].y;
	maxY = m_drawPoint[nPlayer].y + m_nCardHeight;
	//
//	if (nNext == SOUTH) {
	 	// check player's cards
		for(i=3;i>=0;i--) 
		{
			if (PLAYER(nPlayer).IsDummy())
				nSuit = theApp.GetDummySuitSequence(i);
			else
				nSuit = theApp.GetSuitSequence(i);
			for(j=PLAYER(nPlayer).GetNumCardsInSuit(nSuit)-1;j>=0;j--) 
			{
				pCard = PLAYER(nPlayer).GetCardInSuit(nSuit,j);
				if (InRect(point, pCard)) 
				{
					bFound = TRUE;
					break;
				}
			}
			if (bFound)
				break;
		}
		//
		if (bFound) 
		{
			nOrigin = nPlayer;
//			strTemp.Format("Selected %s",pCard->GetName());
//			AfxMessageBox(strTemp);
		}
//	}

	// now check north's cards
	minX = m_drawPoint[nDummy].x;
	maxX = 640;
	minY = m_drawPoint[nDummy].y;
	maxY = 480;
	//
//	if ((!bFound) && (point.x >= minX) && (point.x < maxX) &&
//				(point.y >= minY) && (point.y < maxY)) {
	if (!bFound) 
	{
	 	// check dummy's cards
		for(i=3;i>=0;i--) 
		{
			if (PLAYER(nPlayer).IsDummy())
				nSuit = theApp.GetDummySuitSequence(i);
			else
				nSuit = theApp.GetSuitSequence(i);
			for(j=PLAYER(nDummy).GetNumCardsInSuit(nSuit)-1;j>=0;j--) 
			{
 				pCard = PLAYER(nDummy).GetCardInSuit(nSuit,j);
				if (InRect(point, pCard)) 
				{
					bFound = TRUE;
					break;
				}
			}
			if (bFound)
				break;
		}
		//
		if (bFound) 
		{
			nOrigin = nDummy;
//			strTemp.Format("Selected North's %s",CardToString(pCard->GetName());
//			AfxMessageBox(strTemp);
		}
	}

	// see if a West card has been selected
	minX = m_drawPoint[nWest].x;
	maxX = m_drawPoint[nWest].x + m_nCardWidth;
	minY = m_drawPoint[nWest].y;
	maxY = 480;
	//
//	if ((!bFound) && (point.x >= minX) && (point.x < maxX) &&
//				(point.y >= minY) && (point.y < maxY)) {
	if (!bFound) 
	{
	 	// check west's cards
		for(i=3;i>=0;i--) 
		{
			if (PLAYER(nPlayer).IsDummy())
				nSuit = theApp.GetDummySuitSequence(i);
			else
				nSuit = theApp.GetSuitSequence(i);
			for(j=PLAYER(nWest).GetNumCardsInSuit(nSuit)-1;j>=0;j--) 
			{
				pCard = PLAYER(nWest).GetCardInSuit(nSuit,j);
				if (InRect(point, pCard)) 
				{
					bFound = TRUE;
					break;
				}
			}
			if (bFound)
				break;
		}
		//
		if (bFound) 
		{
			nOrigin = nWest;
//			strTemp.Format("Selected West's %s",pCard->GetName());
//			AfxMessageBox(strTemp);
		}
	}

	// see if an East card has been selected
	minX = m_drawPoint[nEast].x;
	maxX = m_drawPoint[nEast].x + m_nCardWidth;
	minY = m_drawPoint[nEast].y;
	maxY = 480;
	//
//	if ((!bFound) && (point.x >= minX) && (point.x < maxX) &&
//				(point.y >= minY) && (point.y < maxY)) {
	if (!bFound) 
	{
	 	// check east's cards
		for(i=3;i>=0;i--) {
			if (PLAYER(nPlayer).IsDummy())
				nSuit = theApp.GetDummySuitSequence(i);
			else
				nSuit = theApp.GetSuitSequence(i);
			for(j=PLAYER(nEast).GetNumCardsInSuit(nSuit)-1;j>=0;j--) 
			{
				pCard = PLAYER(nEast).GetCardInSuit(nSuit,j);
				if (InRect(point, pCard)) 
				{
					bFound = TRUE;
					break;
				}
			}
			if (bFound)
				break;
		}
		//
		if (bFound) 
		{
			nOrigin = nEast;
//			strTemp.Format("Selected East's %s",theApp.pCard->GetName());
//			AfxMessageBox(strTemp);
		}
	}

	//
	// Found a card that the user selected
	//
	// this is just debugging code, of course
	if (bFound)  
	{
		pDOC->SetCurrentPlayer(nOrigin);
		ThrowCard(nOrigin,pCard);
	}
	//
//	CView::OnRButtonDown(nFlags, point);
}




//
BOOL CEasyBView::OnEraseBkgnd(CDC* pDC) 
{
	// avoid multiple updates
	if (m_nSuppressRefresh > 0)
		return TRUE;

	//
	CEasyBDoc* pDoc = GetDocument();
	BOOL bUseBitmap = ((m_bCanDisplayBitmap) && (m_bBitmapAvailable) && 
						(m_bViewInitialized) && theApp.GetValue(tbShowBackgroundBitmap) );

	// see if the background bitmap is big enough to cover the screen
	CRect rect;
	GetClientRect(&rect);
	int nWidth = bUseBitmap? m_customDIB.DibWidth() : 0;
	int nHeight = bUseBitmap? m_customDIB.DibHeight() : 0;
	
	// erase the background if the bitmap is smaller than the window
	if ((nWidth < rect.Width()) || (nHeight < rect.Height()))
		EraseBackground(*pDC);

	// then draw bitmap if appropriate
	if (bUseBitmap) 
	{
		CRect rect;
		GetClientRect(&rect);

		// center bitmap if requested
		int nX = (rect.Width() - m_customDIB.DibWidth()) / 2;
		int nY = (rect.Height() - m_customDIB.DibHeight()) / 2;
		if (nX < 0)
			nX = 0;
		if (nY < 0)
			nY = 0;
		//
		if ((m_customDIB.DibHeight() < rect.Height()) && (m_customDIB.DibWidth() < rect.Width()))
		{
			if (theApp.GetValue(tnBitmapDisplayMode) == 0)
			{
				// tile
				m_customDIB.Draw(pDC, rect);
			}
			else
			{
				// draw centered
				m_customDIB.Draw(pDC, rect, nX, nY, theApp.GetValue(tbScaleLargeBitmaps));
			}
		}
		else
		{
			// bitmap is bigger than the window, so just draw (centered)
			m_customDIB.Draw(pDC, rect, nX, nY, theApp.GetValue(tbScaleLargeBitmaps));
		}
	} 

	return TRUE;
	//
//	return CView::OnEraseBkgnd(pDC);
}


//
void CEasyBView::EraseBackground(CDC& dc)
{
	if (m_nSuppressRefresh > 0)
		return;
	//
	CBrush *pOldBrush,newBrush;
	newBrush.CreateSolidBrush(theApp.GetValue(tnBackgroundColor));
	pOldBrush = (CBrush*) dc.SelectObject(&newBrush);
	CRect rect;
	dc.GetClipBox(&rect);     // Erase the area needed
	dc.PatBlt(rect.left, rect.top, rect.Width(),
			  rect.Height(), PATCOPY);
	(void)dc.SelectObject(pOldBrush);
	newBrush.DeleteObject();
}



//
// CEasyBView drawing
//
void CEasyBView::OnDraw(CDC* pDC)
{
	//
	if (m_nSuppressRefresh > 0)
		return;

	// first init the view & show animation, if called for
	if (!m_bViewInitialized) 
	{
/*
		if (theApp.GetValue(tbShowStartupAnimation) ||
			theApp.GetValue(tbFirstTimeRunning))
*/

//
// disable startup animation for now
//		if (theApp.GetValue(tbShowStartupAnimation))
		if (FALSE)
		{
			// show opening sequence
			SetCurrentMode(MODE_TITLESCREEN);
			DoOpeningSequence();
			::Sleep(2500);
/*
			// see if this is the first time running --
			// if so, show the intro screen
			if (theApp.GetValue(tbFirstTimeRunning))
			{
				SetCurrentMode(MODE_INTROSCREEN); 
				m_pIntroWnd = new CIntroWnd;
				m_pIntroWnd->Create(CIntroWnd::IDD, this);
				m_pIntroWnd->ShowWindow(SW_SHOW);
				m_pIntroWnd->UpdateWindow();
			}
*/

			// go ahead and mark the view as ready
			m_bViewInitialized = TRUE;

			// initialize draw parameters
			SetViewParameters();

			// and redraw
			Invalidate();
			ResetPrompt();
			SetCurrentMode(MODE_NONE);
			//
			return;
		}
		else
		{
/*
			// see if this is the first time running --
			// if so, show the intro screen
			if (theApp.GetValue(tbFirstTimeRunning))
			{
				UpdateWindow();
				m_pIntroWnd = new CIntroWnd;
				m_pIntroWnd->Create(CIntroWnd::IDD, this);
				m_pIntroWnd->ShowWindow(SW_SHOW);
				m_pIntroWnd->UpdateWindow();
				::Sleep(1000);
			}
*/

			// no opening sequence here
			m_bViewInitialized = TRUE;
			SetCurrentMode(MODE_NONE);

			// initialize draw parameters
			SetViewParameters();

			// force a repaint to show a bitmap
			if ((m_bCanDisplayBitmap) && (m_bBitmapAvailable) &&
				(theApp.GetValue(tbShowBackgroundBitmap)))
				Invalidate();
		}
	}
	// now do normal drawing
	// first see what mode we're in
	switch (m_nCurrMode) 
	{
		case MODE_CARDLAYOUT:
		case MODE_EDITHANDS:
			DrawCardLayoutMode(pDC, !m_bLayoutDlgActive);
			break;
		default:
			// normal play mode
			DrawPlayingField(pDC);
			if (theApp.GetValue(tbGameInProgress))
				DisplayTricks();
			break;
	}
}



//
void CEasyBView::DisplayTricks(CDC* pSentDC) 
{
	CDC* pDC;
	//
	if (pSentDC == NULL)
		pDC = GetDC();
	else
		pDC = pSentDC;
	//
	CString strCount;
	int x,y,cx,cy;
	CSize size;
	CDC tempDC;
	tempDC.CreateCompatibleDC(pDC);
	CFont* pOldFont = pDC->SelectObject(&pMAINFRAME->m_tricksFont);
	COLORREF oldColor = pDC->SetTextColor(RGB(64,64,255));
	int nPrevMode = pDC->SetBkMode(TRANSPARENT);

	// new version -- draw cards criss-crossed
	BOOL bSmallCards = theApp.GetValue(tbLowResOption);
	int nWinnerWidth = bSmallCards? smallWinnerBitmapSize.cx : winnerBitmapSize.cx;
	int nWinnerHeight = bSmallCards? smallWinnerBitmapSize.cy : winnerBitmapSize.cy;
	int nLoserWidth = bSmallCards? smallLoserBitmapSize.cx : loserBitmapSize.cx;
	int nLoserHeight = bSmallCards? smallLoserBitmapSize.cy : loserBitmapSize.cy;

	// display tricks lost
	CBitmap* pOldBitmap = tempDC.SelectObject(&m_losersBitmap);
	x = m_ptTricksDisplayDest[1].x;
	y = m_ptTricksDisplayDest[1].y;
	pDC->BitBlt(x, y, nLoserWidth, nLoserHeight, &tempDC, 0, 0, SRCCOPY);
	tempDC.SelectObject(pOldBitmap);
	strCount.Format("%d",pDOC->GetValue(tnumTricksWon,1));
	size = pDC->GetTextExtent((LPCTSTR)strCount,strCount.GetLength());
	int nOffset = (m_ptTricksDisplayDest[1].x + nLoserWidth) - (m_ptTricksDisplayDest[0].x + nWinnerWidth);
	cx = (nOffset - size.cx) / 2;
	x = m_ptTricksDisplayDest[0].x + nWinnerWidth;
	cy = (nLoserHeight - size.cy) / 2;
	pDC->TextOut(x+cx, y+cy, (LPCTSTR)strCount);

	// display tricks won
	pOldBitmap = tempDC.SelectObject(&m_winnersBitmap);
	x = m_ptTricksDisplayDest[0].x;
	y = m_ptTricksDisplayDest[0].y;
	pDC->BitBlt(x, y, nWinnerWidth, nWinnerHeight, &tempDC, 0, 0, SRCCOPY);
	tempDC.SelectObject(pOldBitmap);
	strCount.Format("%d",pDOC->GetValue(tnumTricksWon,0));
	size = pDC->GetTextExtent((LPCTSTR)strCount,strCount.GetLength());
	cx = (nWinnerWidth - size.cx) / 2;
	cy = (nWinnerHeight - size.cy) / 2;
	pDC->TextOut(x+cx, y+cy, (LPCTSTR)strCount);

	// done
	pDC->SetBkMode(nPrevMode);
	pDC->SelectObject(pOldFont);
	pDC->SetTextColor(oldColor);
	//
	if (pSentDC == NULL)
		ReleaseDC(pDC);
}


//
void CEasyBView::OnRefreshScreen() 
{
	OnDoRealize();
//	ResetSuitOffsets();	// ??? necessary ???
	Invalidate();	
	UpdateWindow();	
}

//
void CEasyBView::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
	if (pDOC->GetValue(tbHandsDealt))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CEasyBView::OnFilePrint() 
{
	CView::OnFilePrint();
}

//
void CEasyBView::OnUpdateFilePrintPreview(CCmdUI* pCmdUI) 
{
	if (pDOC->GetValue(tbHandsDealt))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CEasyBView::OnFilePrintPreview() 
{
	CView::OnFilePrintPreview();
}



//
///////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////
//





//
// HandleCardPlay() 
//
// called after a card is selected for play in the curent round, either 
// manually or by a computer player
//
void CEasyBView::HandleCardPlay(CCard* pCard) 
{
	numCardsProcessed++;
	//
	Position nPosition = (Position) pDOC->GetCurrentPlayerPosition();
	ASSERT((Position)pCard->GetOwner() == nPosition);
//	if ((Position)pCard->GetOwner() != nPosition)
//		return;		// oops
	//
	ThrowCard(nPosition, pCard);

	// return at this point if in batch mode
//	if (pDOC->GetValue(tbBatchMode))
//		return;

	// advance to the next player
	pDOC->SetCurrentPlayer(GetNextPlayer(nPosition));
	if (pDOC->GetNumCardsPlayedInRound() < 4) 
	{
		// and move on, if more cards need to be played
		AdvanceToNextPlayer();
	} 
	else 
	{
		// else all four players have played, so the trick is over
		pDOC->EvaluateTrick();
	}
}




//
// Undo last card played
//
void CEasyBView::OnUpdateUndoCard(CCmdUI* pCmdUI) 
{
	if (pDOC->GetNumCardsPlayedInRound() == 0)
		pCmdUI->Enable(FALSE);
}

//
void CEasyBView::OnUndoCard() 
{
	int nPos = GetPrevPlayer(pDOC->GetCurrentPlayerPosition());
	//
	CCard* pCard = pDOC->GetCurrentTrickCard(nPos);
	if (pCard == NULL)
		return;

	// restore count
	pDOC->UndoLastCardPlayed();

	CDC* pDC = GetDC();
	// if the card came from dummy, the cards on the table should be redrawn
	if (PLAYER(nPos).IsDummy())
	{
		// first clear the table
		ClearTableRegion();
		// then draw any remaining table cards
		DrawTableCards(pDC);
	}
	else
	{
		// else just clear the table card and redraw the hand
		pCard->RestoreBackground(pDC);
	}
	DisplayHand((Position)nPos);
	ReleaseDC(pDC);

	// correct the prompt and invoke the (previous) player 
	AdvanceToNextPlayer();
}



//
// Undo the current trick
//
void CEasyBView::OnUpdateUndoTrick(CCmdUI* pCmdUI) 
{
//	if (!theApp.IsGameInProgress() || (pDOC->GetNumCardsPlayedInRound() == 0))
	if (!theApp.IsGameInProgress() || 
		((pDOC->GetNumTricksPlayed() == 0) && (pDOC->GetNumCardsPlayedInRound() == 0)) )
		pCmdUI->Enable(FALSE);
}

//
void CEasyBView::OnUndoTrick() 
{
	int numCardsPlayed = pDOC->GetNumCardsPlayedInRound();	

	// first clear the cards display
	BOOL bImmediateRestart = FALSE;
	if (numCardsPlayed > 0)
	{

		// clearing the trick being played
		CDC *pDC = GetDC();
		// start with the most recent card and work backwards
		int nPos = GetPrevPlayer(pDOC->GetCurrentPlayerPosition());
/*
		for(int i=0;i<numCardsPlayed;i++) 
		{
			// get the card played
			CCard* pCard = pDOC->GetCurrentTrickCard(nPos);
			ASSERT(pCard != NULL);
			// clear its background
			pCard->RestoreBackground(pDC);
			// return the card to the player
			pDOC->UndoLastCardPlayed();
			// then redraw the hand
			DisplayHand((Position)nPos);
			// and get the next (actually, previous) position
			nPos = GetPrevPlayer(nPos); 
		}
*/
		// un-draw the cards 
		for(int i=0;i<numCardsPlayed;i++) 
		{
			// get the card played
			CCard* pCard = pDOC->GetCurrentTrickCard(nPos);
			ASSERT(pCard != NULL);
			// clear its background
			pCard->RestoreBackground(pDC);
			nPos = GetPrevPlayer(nPos); 
		}
		ReleaseDC(pDC);
		// and reset info
		pDOC->UndoTrick();
		// then redraw the hands
		if (pDOC->GetNumTricksPlayed() == 0)
		{
			// recalc dummy suit offsets
			ResetSuitOffsets();
		}
		Invalidate();

		// see if it's the human player's turn
		if (pDOC->GetRoundLeadPlayer()->IsHumanPlayer())
			bImmediateRestart = TRUE;
	}
	else
	{
		// no cards played in this round -- move to the previous trick
		if (pDOC->GetNumTricksPlayed() == 0)
			return;
		// 
		pDOC->UndoPreviousTrick();
		ResetSuitOffsets();
		Invalidate();	
//		DisplayTricks();
	}

/*
	// now invoke the first player 
	// but prompt the user if the current player is not human
	if (!pDOC->GetRoundLeadPlayer()->IsHumanPlayer())
	{
		if (pDOC->GetNumTricksPlayed() > 0)
			SetPrompt("Click to play the trick.");
		else
			SetPrompt("Click to begin play.");
		SetCurrentMode(MODE_CLICKTORESTARTTRICK);
	}
	else
	{
		AdvanceToNextPlayer();
	}
*/
	//
	if (bImmediateRestart && (theApp.GetValue(tnCardPlayMode) != CEasyBApp::PLAY_FULL_AUTO) &&
				(theApp.GetValue(tnCardPlayMode) != CEasyBApp::PLAY_FULL_AUTO_EXPRESS))
	{
		AdvanceToNextPlayer();
	}
	else
	{
		if (pDOC->GetNumTricksPlayed() > 0)
			SetPrompt(FormString("Click to play the trick -- %s leads.", PositionToString(pDOC->GetRoundLead())));
		else
			SetPrompt(FormString("Click to begin play -- %s leads.", PositionToString(pDOC->GetRoundLead())));
		//
		SetCurrentMode(MODE_CLICKTORESTARTTRICK);
	}
}


void CEasyBView::OnUpdateLayoutCards(CCmdUI* pCmdUI) 
{
//	if (!pDOC->GetValue(tbHandsDealt))
//		pCmdUI->Enable(FALSE);
	//
	if (m_nCurrMode == MODE_CARDLAYOUT)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
	//
	if (m_nCurrMode == MODE_CARDLAYOUT)
		pCmdUI->SetText("End Layout Mode\tF9");
	else if (m_nCurrMode == MODE_EDITHANDS)
		pCmdUI->SetText("End Edit Mode\tF9");
	else
		pCmdUI->SetText("Lay Out Cards\tF9");
}


//
void CEasyBView::OnLayoutCards() 
{
	// may be a toggle on or off
	BOOL bCancelled = FALSE;
	//
	if ((m_nCurrMode == MODE_CARDLAYOUT) ||
						(m_nCurrMode == MODE_EDITHANDS)) 
	{
		// see if we're done assigning cards
		if (m_numCardsUnassigned > 0) 
		{
			// ask to cancel or distribute random
			int nCode = AfxMessageBox("The cards left unassigned will be distributed randomly to the players.  Do you wish to proceed?", MB_OKCANCEL);
			if (nCode == IDOK)
			{
				// clicked on yes, so distribute
				// but suppress refresh
				SuppressRefresh();
				OnDistributeRandom();
				EnableRefresh();
				// clear the table area
				ClearDisplayArea(&m_rectLayout);
			}
			else if (nCode == IDCANCEL)
			{
				// clicked on cancel, so exit & do nothing (stay in mode)
				return;
			}
		}

		// okay, now turn off mode
//		SetCurrentMode(MODE_NONE);
		m_bExchanging = FALSE;
		m_nExchangeSrcPlayer = NONE;
		m_pExchangeCard = NULL;
		ClearPrompt();

		// suspend refreshes
		SuppressRefresh();

		// turn off dialog if it's still on
		m_bLayoutDlgActive = FALSE;
		pMAINFRAME->GetDialog(twLayoutDialog)->ShowWindow(SW_HIDE);
		pMAINFRAME->RestoreAllDialogs();
		pMAINFRAME->ClearModeIndicator();
		pMAINFRAME->UpdateWindow();

		// restore draw flag
		EnableRefresh();

		//
		if (!bCancelled)
		{
			// now redraw the cards
			Invalidate();
			UpdateWindow();

			// and ask for dealer position
			CSelectHandDialog handDialog(pMAINFRAME);
			handDialog.m_strTitle = "Select Dealer";
			handDialog.m_nMode = CSelectHandDialog::SH_MODE_DEALER;
			int nDealer = pDOC->GetDealer();
			if (!ISPLAYER(nDealer))
				nDealer = SOUTH;
			handDialog.m_nPosition = nDealer;
			BOOL bDuplicate = theApp.IsUsingDuplicateScoring();
			if (bDuplicate)
				handDialog.m_bCollapsed = FALSE;

			// preset dealer
			if (bDuplicate && (m_nCurrMode == MODE_EDITHANDS))
			{
				int nVul = pDOC->GetValue(tnVulnerableTeam);
				switch(nVul)
				{
					case NEITHER:
						handDialog.m_nVulnerability = 1;
						break;
					case BOTH:
						handDialog.m_nVulnerability = 2;
						break;
					case NORTH_SOUTH:
						handDialog.m_nVulnerability = 3;
						break;
					case EAST_WEST:
						handDialog.m_nVulnerability = 4;
						break;
				}
			}

			//
			handDialog.DoModal();

			// set dealer
			pDOC->SetValue(tnDealer, handDialog.m_nPosition);
			pDOC->SetValue(tnCurrentPlayer, handDialog.m_nPosition);

			// set vulnerable
			if (bDuplicate)
			{
				int nVulnTeam;
				switch(handDialog.m_nVulnerability)
				{
					case 0:
						nVulnTeam = GetRandomValue(3) - 1;
						break;
					case 1:
						nVulnTeam = NEITHER;
						break;
					case 2:
						nVulnTeam = BOTH;
						break;
					case 3:
						nVulnTeam = NORTH_SOUTH;
						break;
					case 4:
						nVulnTeam = EAST_WEST;
						break;
				}
				pDOC->SetValue(tnVulnerableTeam, nVulnTeam);
			}
/*
			// note that for new deals, the dealer is automatically 
			// advanced 1 player each time, so compensate
			if (m_nCurrMode == MODE_CARDLAYOUT)
				pDOC->SetValue(tnDealer, GetPrevPlayer(handDialog.m_nPosition));
			else
				pDOC->SetValue(tnDealer, handDialog.m_nPosition);
*/
			// turn off cards exposed flag
			for(int i=0;i<4;i++) 
				PLAYER(i).ExposeCards(FALSE, FALSE);
			// but show South's hand
			PLAYER(SOUTH).ExposeCards(TRUE, FALSE);

			// turn off card layout mode flag
			SetCurrentMode(MODE_NONE);

			// and start play
			pDOC->InitPlay(TRUE, TRUE);
		}
		else
		{
			// simply clear screen & return to normal mode
			SetCurrentMode(MODE_NONE);
			Invalidate();
		}
	} 
	else 
	{
		// starting layout
		// confirm
		if (theApp.GetValue(tbGameInProgress) && (pDOC->GetNumCardsPlayedInGame() > 0))
		{
			if (AfxMessageBox("This will cancel the game in progress.  Do you wish to continue?", MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL)
				return;
		}

		// set mode ahead of the refresh
		SetCurrentMode(MODE_CARDLAYOUT);
		PrepareCardLayout();

		// hide supplementary dialogs
		SuppressRefresh();
		pMAINFRAME->HideAllDialogs();
		if (theApp.IsBiddingInProgress())
			pMAINFRAME->GetBidDialog()->CancelImmediate();
//		UpdateWindow();

		// mark that the current hand cannot be reproduced by ID
		pDOC->SetValue(tbDealNumberAvailable, FALSE);

		// if in the course of play, restore initial hands 
		if (theApp.GetValue(tbGameInProgress))
		{
			pDOC->RestoreInitialHands();
			theApp.SetValue(tbGameInProgress, FALSE);
		}

		// clear display
		if (pDOC->GetValue(tbHandsDealt)) 
		{
			CDC* pDC = GetDC();
			OnEraseBkgnd(pDC);
			ReleaseDC(pDC);
		}

		// and prepare
		m_bLayoutDlgActive = TRUE;
		CCardLayoutDialog* pDlg = (CCardLayoutDialog*) pMAINFRAME->GetDialog(twLayoutDialog);
		pDlg->ForceRefresh();
		pDlg->ShowWindow(SW_SHOW);
		pMAINFRAME->SetModeIndicator(" Layout ");
		SetPrompt("Click on checkboxes to assign cards to players");
		//
		EnableRefresh();
		Invalidate();
	}
}


//
void CEasyBView::PrepareCardLayout() 
{
	// init card layout mode
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<4;i++) 
		PLAYER(i).ClearHand();
	pDOC->PrepForCardLayout();
	pMAINFRAME->ClearAllIndicators();
	SetCurrentMode(MODE_CARDLAYOUT);

	// turn all cards & hands face up
	for(i=0;i<52;i++) 
		deck[i]->SetFaceUp();
	for(i=0;i<4;i++) 
		PLAYER(i).ExposeCards(TRUE, FALSE);

	// assign cards to layout deck
	for(i=0;i<52;i++) 
	{
		m_layoutDeck[i] = deck.GetSortedCard(i);
		m_layoutDeck[i]->ClearAssignment();
		m_layoutDeck[i]->SetXPosition();
		m_layoutDeck[i]->SetYPosition();
	}
	m_numCardsUnassigned = 52;
	m_nPlayerCardLimit = 13;
	pMAINFRAME->GetDialog(twLayoutDialog)->SendMessage(WM_COMMAND, WMS_SET_CARD_LIMIT, 13);

	//
	ClearCardExchangeMode();
	ResetSuitOffsets();
}



//
void CEasyBView::ClearCardExchangeMode()
{
	//
	m_bExchanging = FALSE;
	m_nExchangeSrcPlayer = NONE;
	m_pExchangeCard = NULL;
}



//
// OnUpdateEditExistingHands() 
//
void CEasyBView::OnUpdateEditExistingHands(CCmdUI* pCmdUI) 
{
	if (!pDOC->GetValue(tbHandsDealt) || (m_nCurrMode == MODE_CARDLAYOUT))
		pCmdUI->Enable(FALSE);
	//
	if (m_nCurrMode == MODE_EDITHANDS)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}


//
// OnEditExistingHands() 
//
void CEasyBView::OnEditExistingHands() 
{
	// if already in this mode, exit
	if (m_nCurrMode == MODE_EDITHANDS)
	{
		// use the common function
		OnLayoutCards();
		return;
	}
	// confirm
	if (theApp.GetValue(tbGameInProgress) && (pDOC->GetNumCardsPlayedInGame() > 0))
	{
		if (AfxMessageBox("This will cancel the game in progress.  Do you wish to continue?", MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL)
			return;
	}

	// set mode ahead of the refresh
	SetCurrentMode(MODE_EDITHANDS);

	// hide supplementary dialogs
	SuppressRefresh();
	pMAINFRAME->HideAllDialogs();
	if (theApp.IsBiddingInProgress())
		pMAINFRAME->GetBidDialog()->CancelImmediate();

	//
	SetWindowCursor();
	UpdateWindow();

	// mark that the current hand cannot be reproduced by ID
	pDOC->SetValue(tbDealNumberAvailable, FALSE);

	// if in the course of play, restore initial hands 
	if (theApp.GetValue(tbGameInProgress))
	{
		pDOC->RestoreInitialHands();
		theApp.SetValue(tbGameInProgress, FALSE);
	}

	// and turn on card layout mode
	pMAINFRAME->DisplayTricks(FALSE);
	pMAINFRAME->DisplayContract(FALSE);
	if (theApp.IsRubberInProgress())
		pMAINFRAME->DisplayVulnerable(TRUE);
	else
		pMAINFRAME->DisplayVulnerable(FALSE);

	// turn all cards face up
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<52;i++) 
		deck[i]->SetFaceUp();
	for(i=0;i<4;i++) 
		PLAYER(i).ExposeCards(TRUE, FALSE);

	// clear layout deck
	for(i=0;i<52;i++)
		m_layoutDeck[i] = NULL;
	m_numCardsUnassigned = 0;

	//
	BOOL bUpdated = FALSE;
	CDC* pDC = GetDC();
	if (theApp.IsGameInProgress()) 
	{
		OnEraseBkgnd(pDC);
		ResetSuitOffsets();
		DrawCardLayoutMode(pDC, FALSE, TRUE);
		UpdateWindow();
		bUpdated = TRUE;
	}
	ReleaseDC(pDC);
	//
	m_bExchanging = FALSE;
	m_nExchangeSrcPlayer = NONE;
	m_pExchangeCard = NULL;
	//
	m_nPlayerCardLimit = 13;
	pMAINFRAME->GetDialog(twLayoutDialog)->SendMessage(WM_COMMAND, WMS_SET_CARD_LIMIT, 13);
	if (theApp.GetValue(tbShowLayoutOnEdit)) 
	{
		m_bLayoutDlgActive = TRUE;
		CCardLayoutDialog* pDlg = (CCardLayoutDialog*) pMAINFRAME->GetDialog(twLayoutDialog);
		pDlg->ForceRefresh();
		pDlg->ShowWindow(SW_SHOW);
	}
	pMAINFRAME->SetModeIndicator(" Edit ");
	SetPrompt(szLayoutDonePrompt);

	// only update once
	EnableRefresh();
	if (!bUpdated)
		Invalidate();
}



//
// OnUpdateDistributeRandom() 
//
void CEasyBView::OnUpdateDistributeRandom(CCmdUI* pCmdUI) 
{
	if ( ((m_nCurrMode == MODE_CARDLAYOUT) || 
		  (m_nCurrMode == MODE_EDITHANDS)) &&
							(m_numCardsUnassigned > 0))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

//
void CEasyBView::OnDistributeRandom() 
{
	if ((m_nCurrMode != MODE_CARDLAYOUT) &&
						(m_nCurrMode != MODE_EDITHANDS))
		return;
	// first look for any remaining cards
	int i,j,numCardsRemaining = 0;
	CCard *pCard, *remainingStack[52];
	for(i=0;i<52;i++) 
	{
		if (m_layoutDeck[i]) 
		{
			pCard = m_layoutDeck[i];
			m_layoutDeck[i] = NULL;
			remainingStack[numCardsRemaining] = pCard;
			numCardsRemaining++;
		}
	}
	// now assign the remaining cards
	int nPos,nIndex,numRequired;
	for(nPos=0;nPos<4;nPos++) 
	{
		if(PLAYER(nPos).GetNumCards() == 13)
			continue;
		// fill remainder of player's hand
		numRequired = 13 - PLAYER(nPos).GetNumCards();
		for(j=0;j<numRequired;j++) 
		{
			// get random index into layout deck
			nIndex = GetRandomValue(numCardsRemaining-1);
			// and search for a non-empty card slot
			while(remainingStack[nIndex] == NULL) 
			{
				nIndex++;
				if (nIndex >= numCardsRemaining)
					nIndex = 0;
			}
			// found a card; place it in the player's hand
			PLAYER(nPos).AddCardToHand(remainingStack[nIndex]);
			remainingStack[nIndex] = NULL;
		}
	}
	m_numCardsUnassigned = 0;
	//
	ResetSuitOffsets();
	// redraw cards
	if (m_nSuppressRefresh == 0)
	{
		CDC* pDC = GetDC();
		OnEraseBkgnd(pDC);
		DrawCardLayoutMode(pDC);
		ReleaseDC(pDC);
	}
	//
	pMAINFRAME->GetDialog(twLayoutDialog)->SendMessage(WM_COMMAND,WMS_RESET_DISPLAY);
	//
	SetPrompt(szLayoutDonePrompt);
}



//
void CEasyBView::OnUpdateBidCurrentHand(CCmdUI* pCmdUI) 
{
	if (theApp.IsGameInProgress()) 
	{
		pCmdUI->SetText("Rebid Current Hand\tF3");	
	} 
	else 
	{
		pCmdUI->SetText("Bid\tF3");	
	}
	//
	BOOL foo = pDOC->GetValue(tbHandsDealt);
	BOOL ack = theApp.GetValue(tbBiddingInProgress);
	//
	if ((!pDOC->GetValue(tbHandsDealt)) ||
//				(theApp.GetValue(tbBiddingInProgress)) ||
							(m_nCurrMode == MODE_CARDLAYOUT) ||
							(m_nCurrMode == MODE_EDITHANDS))
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
/*
	if ((pDOC->m_bHandsDealt) &&
					(m_nCurrMode == MODE_WAITTOBID))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
*/
}




//
// OnBidCurrentHand() 
//
// called to bid the current hand
//
void CEasyBView::OnBidCurrentHand() 
{
	// see if we're restarting bidding
	if (theApp.IsBiddingInProgress())
	{
		pDOC->RestartBidding();
		pMAINFRAME->GetBidDialog()->InitBiddingSequence();
		return;
	}
/*  Following causes skipped bidding ???
	// NCR-GUI1 Need to refresh when rebidding a fully bid hand
	if(pDOC->GetPlayRound() == 13) {  // value set in ReadFile
		pDOC->RestartBidding();
		pMAINFRAME->GetBidDialog()->InitBiddingSequence();
	} // end NCR-GUI1
*/
	// suppressh multiple updates
	SuppressRefresh();

	// hide the play history dialog if auto mode is on
	if (theApp.GetValue(tbAutoHidePlayHistory))
		pMAINFRAME->HideDialog(twPlayHistoryDialog);
	//
	pMAINFRAME->ClearStatusMessage();

	// clear layout mode
	if ((m_nCurrMode == MODE_CARDLAYOUT) ||
					(m_nCurrMode == MODE_EDITHANDS)) 
	{
		if (m_bLayoutDlgActive) 
		{
			pMAINFRAME->GetDialog(twLayoutDialog)->ShowWindow(SW_HIDE);
			m_bLayoutDlgActive = FALSE;
		}
		ClearCardExchangeMode();
	}

	// verify that the cards have been dealt
	if (!pDOC->GetValue(tbHandsDealt))
	{
		EnableRefresh();
		return;
	}

	// rebidding the hand after game is underway?
	if (theApp.IsGameInProgress() || (m_nCurrMode == MODE_GAMEREVIEW)
		// NCR-GUI1  Need this for Rebid on Hand Finished 
		|| (m_nCurrMode == MODE_GAMERESTORE))
	{
		// if in the midst of game review, hide the game review dialog
		if (m_nCurrMode == MODE_GAMEREVIEW)
		{
			pMAINFRAME->HideDialog(twGameReviewDialog);
			SetCurrentMode(MODE_WAITTOBID);
		}
		// reset hands to initial cards dealt
		for(int i=0;i<4;i++) 
			PLAYER(i).RestoreInitialHand();
		// notify doc (which will notify the players, which will notify...)
		pDOC->RestartBidding();
		// then clear card display locations and redisplay
		ResetSuitOffsets();
		Invalidate();
		UpdateWindow();
	}

	// show bidding history
	if (theApp.GetValue(tbAutoShowBidHistory))
		pMAINFRAME->MakeDialogVisible(twBiddingHistoryDialog);

	// make sure south's hand is exposed
	PLAYER(SOUTH).ExposeCards(TRUE, FALSE);

	// set internal mode, although it may have already been set
	SetCurrentMode(MODE_WAITTOBID);

	// restore refresh
	EnableRefresh();
	Refresh(TRUE);	// make sure to invalidate

	//
	theApp.SetValue(tbBiddingInProgress, TRUE);
//	pMAINFRAME->SetModeIndicator(" Bid ");
	pMAINFRAME->SetAllIndicators();
	CBidDialog* pBidDlg = pMAINFRAME->GetBidDialog();
	pBidDlg->ShowWindow(SW_SHOW);
	pBidDlg->InitBiddingSequence();
//	pBidDlg->ShowWindow(SW_SHOW);

	// and make sure the focus is on the bidding dialog
	pBidDlg->SetFocus();

/*
	// restore refresh
	EnableRefresh();
	Refresh(TRUE);	// make sure to invalidate
*/
}



//
// GameLoaded()
//
// called after a game has been loaded
//
void CEasyBView::GameLoaded() 
{
	// show/hide dialogs
	if (theApp.GetValue(tbAutoHideBidHistory))
		pMAINFRAME->HideDialog(twBiddingHistoryDialog);
	if (theApp.GetValue(tbAutoShowPlayHistory))
		pMAINFRAME->MakeDialogVisible(twPlayHistoryDialog);

	// set mode to "click to begin"
	CString strPrompt;
	strPrompt.Format("Contract is %s by %s; %s leads.  Click to begin.",
					 pDOC->GetFullContractString(), PositionToString(pDOC->GetDeclarerPosition()),
					 PositionToString(pDOC->GetRoundLead()));
	SetPrompt(strPrompt);
	SetCurrentMode(MODE_CLICKTOBEGINPLAY);
}



//
// ResumeLoadedGame()
//
// called after a game position has been loaded
//
void CEasyBView::ResumeLoadedGame() 
{
//	ResetSuitSequence();
//	ResetSuitOffsets();
	//
	CString strPrompt;
	strPrompt.Format("%s ", PositionToString(pDOC->GetCurrentPlayerPosition()));
	if (pDOC->GetNumCardsPlayedInRound() == 0)
		strPrompt += "leads.";
	else
		strPrompt += "plays next.";
/*
	if (pDOC->GetCurrentPlayer()->IsHumanPlayer())
	{
		// human player -- no need to prompt twice
		SetPrompt(strPrompt);
		AdvanceToNextPlayer();
	}
	else
	{
		strPrompt += "  Click to begin.";
		SetPrompt(strPrompt);
		SetCurrentMode(MODE_CLICKTORESUMEPLAY);
	}
	*/
	strPrompt += "  Click to begin.";
	SetPrompt(strPrompt);
	SetCurrentMode(MODE_CLICKTORESUMEPLAY);
}






//
// BiddingComplete()
//
// called when bidding is done
//
void CEasyBView::BiddingComplete(BOOL bSuccess) 
{
	if (!bSuccess) 
	{
		// close the bidding dialog if it's still visible
		pMAINFRAME->GetBidDialog()->ShowWindow(SW_HIDE);
//		AfxMessageBox("Game Was Cancelled!");
		int i;
		for(i=0;i<4;i++) 
		{
			PLAYER(i).CancelBidding();
		}

		// don't mark as waiting to bid if canceling bidding to edit hands
		if ((m_nCurrMode != MODE_CARDLAYOUT) && (m_nCurrMode != MODE_EDITHANDS))
			SetCurrentMode(MODE_WAITTOBID);

		//
		CMainFrame::SetStatusMessage("Bidding cancelled.");
		return;
	}

	// else completed bidding
	pDOC->SetBiddingComplete();
	// make sure to clear the bidding dialog
	UpdateWindow();
	//
	BeginPlay();
}



//
// BeginPlay()
//
// called to begin the game play
//
void CEasyBView::BeginPlay() 
{
	// make sure to reset the mode & the cursor 
	SetCurrentMode(MODE_NONE);

	// suppress multiple updates
	SuppressRefresh();

	// record dialog states
	BOOL bBidDlgWasVisible = pMAINFRAME->IsDialogVisible(twBiddingHistoryDialog);
	BOOL bPlayDlgWasVisible = pMAINFRAME->IsDialogVisible(twPlayHistoryDialog);
	
	// show/hide dialogs
	if (theApp.GetValue(tbAutoHideBidHistory))
		pMAINFRAME->HideDialog(twBiddingHistoryDialog);
	if (theApp.GetValue(tbAutoShowPlayHistory))
		pMAINFRAME->MakeDialogVisible(twPlayHistoryDialog);

	//
	if (pDOC->GetValue(tbAutoReplayMode))
		pMAINFRAME->SetStatusText("Performing computer replay...");
	else
		PromptLead();

	//
	SetCurrentMode(MODE_WAITCARDPLAY);
	theApp.SetValue(tbGameInProgress, TRUE);
	pMAINFRAME->SetAllIndicators();
	//
	if ((pDOC->GetCurrentPlayer()->IsHumanPlayer()) &&
		(theApp.GetValue(tbAutoJumpCursor)))
		JumpCursor();

	// get the ball rolling
	if (pDOC->GetValue(tbAutoReplayMode))
		AdvanceToNextPlayer();
	else
		pDOC->BeginRound();

	// restore refreshes
	EnableRefresh();
	
	// may need to invalidate (redraw cards) if showing/hidings dialogs
	if ((theApp.GetValue(tbAutoHideBidHistory) && bBidDlgWasVisible) || 
		(theApp.GetValue(tbAutoShowPlayHistory) && !bPlayDlgWasVisible) )
		Refresh(TRUE);
	else
		Refresh(FALSE);
}




//
// RestartPlay()
//
// called when play is restarted
//
void CEasyBView::RestartPlay() 
{
	//
	// redraw cards and hide dummy's hand again
	//
	int nDummy = pDOC->GetDummyPosition();
	if (nDummy != SOUTH)
		PLAYER(nDummy).ExposeCards(FALSE,FALSE);
	ResetSuitSequence();
	ResetSuitOffsets();
	Invalidate();
	UpdateWindow();

	//
	// if in computer replay, or the lead is human, we might begin play immediately;
	// else wait for a click to begin play again
	//
	if (pDOC->GetValue(tbAutoReplayMode))
	{
		if (theApp.InExpressAutoPlay())
		{
			BeginPlay();
		}
		else
		{
			SetCurrentMode(MODE_CLICKTOBEGINPLAY);
			SetPrompt("Click to begin computer replay.");
		}
	}
	else if (pDOC->GetRoundLeadPlayer()->IsHumanPlayer())
	{
		BeginPlay();
	}
	else
	{
		SetCurrentMode(MODE_CLICKTOBEGINPLAY);
		SetPrompt("Click mouse button to begin play.");
	}
}



//
// GameFinished()
//
// called just after a game has been finished and the user
// elected not to proceed immediately to the next game
//
void CEasyBView::GameFinished()
{
	SetPrompt("Click to wrap up the game, or press <backspace>, <F3>, or <F4> to back up");
	SetCurrentMode(MODE_CLICKFORNEXTGAME);
}



//
void CEasyBView::OnUpdateGameReview(CCmdUI* pCmdUI) 
{
	if (pDOC->IsGameReviewAvailable() && !pDOC->IsReviewingGame())
		pCmdUI->Enable(TRUE);	
	else
		pCmdUI->Enable(FALSE);	
}


//
void CEasyBView::OnGameReview() 
{
	pDOC->InitGameReview();
	pMAINFRAME->GetBidDialog()->ShowWindow(SW_HIDE);
	BeginGameReview(FALSE);
}


//
void CEasyBView::BeginGameReview(BOOL bReset)
{
	SetCurrentMode(MODE_GAMEREVIEW);
	CGameReviewDialog* pReviewDlg = (CGameReviewDialog*) pMAINFRAME->GetDialog(twGameReviewDialog);
	// depending on the mode, init the dialog or show the cards
	if (bReset)
	{
		// starting game review anew
		pReviewDlg->Initialize(FALSE);
	}
	else
	{
		// returning to game review mode
		pReviewDlg->Reset();
	}
	pReviewDlg->ShowWindow(SW_SHOW);
}


//
void CEasyBView::RestoreGameReview()
{
	SetCurrentMode(MODE_GAMEREVIEW);
	CGameReviewDialog* pReviewDlg = (CGameReviewDialog*) pMAINFRAME->GetDialog(twGameReviewDialog);
	pReviewDlg->Reset(TRUE);
	pReviewDlg->ShowWindow(SW_SHOW);
}


//
void CEasyBView::EndGameReview() 
{
	SetCurrentMode(MODE_NONE);
	pMAINFRAME->HideDialog(twGameReviewDialog);
}




//
BOOL CEasyBView::SetBackgroundBitmap(LPCTSTR szFileName, BOOL bTest) 
{
	// see if this is a test only
	if (bTest)
	{
		CDIB testDib;
		return testDib.Load(szFileName);
	}

	// else load the specified bitmap
	CString strMessage;
	m_bBitmapAvailable = m_customDIB.Load(szFileName);
	if (!m_bBitmapAvailable) 
	{
//		strMessage.Format("Error reading file %s.\nIt may not be a Windows Bitmap file.",strFileName);
//		AfxMessageBox(strMessage);
		return FALSE;
	} 
	else 
	{
		m_strBackgroundBitmap = szFileName;
		OnDoRealize();
		Invalidate();
		return TRUE;
	}
}



//
// OnSize()
//
void CEasyBView::OnSize(UINT nType, int cx, int cy) 
{
	//
	CView::OnSize(nType, cx, cy);

	//
	// adjust draw params
	//
	SetViewParameters(cx, cy);

	//
	// and reset the display
	//
	ResetSuitOffsets();
	Invalidate();
	UpdateWindow();
}
