//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// GameReviewDialog.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "easyBDoc.h"
#include "easyBVw.h"
#include "mainfrm.h"
#include "mainFrameopts.h"
#include "GameReviewDialog.h"
#include "GameRecord.h"
#include "MyToolTipWnd.h"
#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "Subclass.h"
#include "progopts.h"
#include "Help\helpcode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CGameReviewDialog dialog


//
// constants
//
const DialogControlInfo  tControlInfo[] = {
	{ 	IDC_NEXT,		IDI_REVIEW_NEXT,		"Next trick"			},
	{	IDC_PREV,		IDI_REVIEW_PREV,		"Previous trick",		},
	{	IDC_LAST,		IDI_REVIEW_LAST,		"Go to end of game",	},
	{	IDC_FIRST,		IDI_REVIEW_FIRST,		"Go to start of game",	},
	{ 	IDC_NEXT_GAME,	IDI_REVIEW_NEXT_GAME,	"Next game"				},
	{	IDC_PREV_GAME,	IDI_REVIEW_PREV_GAME,	"Previous game",		},
	{	IDC_LAST_GAME,	IDI_REVIEW_LAST_GAME,	"Go to last game",		},
	{	IDC_FIRST_GAME,	IDI_REVIEW_FIRST_GAME,	"Go to first game",		},
	//
	{	IDC_EXPAND_COLLAPSE,	0, 	"Expand/Collapse tags list",	},
	{	IDC_PLAY,		0, 	"Play this game",		},
	{	IDC_SAVE_OUT,	0, 	"Save out this game",	},
	//
	{	IDC_GAME_INDEX, 0, 	"Select game",			},
	{	IDC_NOTE,		0, 	"Note",					},
//	{	IDC_TAGS_LIST,	0, 	"Game information",		},
};
const int tnumControls = sizeof(tControlInfo) / sizeof(DialogControlInfo);
const int tnumButtons = 11;
const int tnExpandCollapseIndex = 8;

//
// construction/destruction
//
CGameReviewDialog::CGameReviewDialog(CWnd* pParent /*=NULL*/)
	: CMyCustomDialog(CGameReviewDialog::IDD, tControlInfo, tnumControls, tnumButtons, pParent)
{
	//{{AFX_DATA_INIT(CGameReviewDialog)
	m_nGameIndex = -1;
	m_strGameInfo = _T("");
	m_strNote = _T("");
	//}}AFX_DATA_INIT
	m_bInitialized = FALSE;
	m_nPrevTooltipIndex = -1;
	m_nGameIndex = 0;
	// set optional info in base class
	m_pListControl = &m_listTags;
}


void CGameReviewDialog::DoDataExchange(CDataExchange* pDX)
{
	CMyCustomDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGameReviewDialog)
	DDX_Control(pDX, IDC_TAGS_LIST, m_listTags);
	DDX_Control(pDX, IDC_GAME_INDEX, m_listGameIndex);
	DDX_CBIndex(pDX, IDC_GAME_INDEX, m_nGameIndex);
	DDX_Text(pDX, IDC_GAMEINFO, m_strGameInfo);
	DDX_Text(pDX, IDC_NOTE, m_strNote);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGameReviewDialog, CMyCustomDialog)
	//{{AFX_MSG_MAP(CGameReviewDialog)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_PREV, OnPrev)
	ON_BN_CLICKED(IDC_LAST, OnLast)
	ON_BN_CLICKED(IDC_FIRST, OnFirst)
	ON_BN_CLICKED(IDC_NEXT_GAME, OnNextGame)
	ON_BN_CLICKED(IDC_PREV_GAME, OnPrevGame)
	ON_BN_CLICKED(IDC_LAST_GAME, OnLastGame)
	ON_BN_CLICKED(IDC_FIRST_GAME, OnFirstGame)
	ON_WM_CLOSE()
	ON_WM_MOVE()
	ON_CBN_SELCHANGE(IDC_GAME_INDEX, OnSelchangeGameIndex)
	ON_BN_CLICKED(IDC_EXPAND_COLLAPSE, OnExpandCollapse)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_SAVE_OUT, OnSaveOut)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()






/////////////////////////////////////////////////////////////////////////////
//
// CGameReviewDialog message handlers
//
/////////////////////////////////////////////////////////////////////////////




//
void CGameReviewDialog::Initialize(BOOL bRefresh)
{
	// first call base class
	CMyCustomDialog::Initialize();

	// then operate
	m_nGameIndex = 0;
	CTypedPtrArray<CPtrArray, CGameRecord*>& gamesArray = pDOC->GetGameRecords();
	m_numGamesAvailable = gamesArray.GetSize();
	BOOL bEnable = (m_numGamesAvailable > 0);

	// fill the games listbox
	m_listGameIndex.ResetContent();
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<m_numGamesAvailable;i++)
		m_listGameIndex.AddString(FormString(i+1));
	m_nGameIndex = 0;
	m_strGameInfo.Format("of %d", m_numGamesAvailable);
	UpdateData(FALSE);

	// enable/disable all controls
	for(i=0;i<tnumControls;i++)
		EnableControl(i, bEnable, TRUE);

	// disable the back buttons
	EnableControlByID(IDC_PREV, FALSE);
	EnableControlByID(IDC_FIRST, FALSE);
	EnableControlByID(IDC_PREV_GAME, FALSE);
	EnableControlByID(IDC_FIRST_GAME, FALSE);

	// expose all cards
//	theApp.SetValue(tbShowCardsFaceUp, TRUE);

	// and set current game
	SetGameIndex(0, bRefresh);
}



//
void CGameReviewDialog::Reset(BOOL bRefresh)
{
	// expose all cards
//	theApp.SetValue(tbShowCardsFaceUp, TRUE);

	// and set current game
	SetGameIndex(m_nGameIndex, bRefresh);

	// and expose all the player hands
//	for(int i=0;i<4;i++)
//		PLAYER(i).ExposeCards(TRUE, FALSE);
}




//
BOOL CGameReviewDialog::OnInitDialog() 
{
	// first call base class
	CMyCustomDialog::OnInitDialog();

	// init the tags list control
	const int tnColSpacer = 14;
	m_listTags.InsertColumn(0, "Tag", LVCFMT_LEFT, m_listTags.GetStringWidth("Tag               ") + tnColSpacer, 0);
	m_listTags.InsertColumn(1, "Value", LVCFMT_LEFT, m_listTags.GetStringWidth("Value                                  ") + tnColSpacer, 1);

	// subclass buttons
	for(int i=0;i<tnumButtons;i++)
	{
		m_flatButtons[i].SubclassDlgItem(tControlInfo[i].nControlID, this);
		if (tControlInfo[i].nIconID != 0)
			m_flatButtons[i].SetIcon(tControlInfo[i].nIconID);
		if (i < 9)
			m_flatButtons[i].SetShowText(FALSE);
	}

	// subclass listbox
	CListBox* pList = (CListBox*) GetDlgItem(IDC_GAME_INDEX);
	wpOrigListBoxProc = (WNDPROC) SetWindowLong(pList->GetSafeHwnd(), GWL_WNDPROC, (LONG) ListBoxSubclassProc);

	// get the full width
	CRect windowRect;
	GetWindowRect(&windowRect);
	m_nFullWidth = windowRect.Width();

	// and the collapsed width
	CRect btnRect;
	GetDlgItem(IDC_FIRST)->GetWindowRect(&btnRect);
	int nSpace = btnRect.left - windowRect.left;
	GetDlgItem(IDC_PLAY)->GetWindowRect(&btnRect);
	m_nCollapsedWidth = btnRect.right + nSpace - windowRect.left;

	// load expand.collapsed icons
	m_hIconExpand = (HICON) LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDI_TAGS_EXPAND),
										IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	m_hIconCollapse = (HICON) LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(IDI_TAGS_COLLAPSE),
										IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);

	// collapse if desired
	m_bCollapsed = theApp.GetValue(tbCollapseGameReviewDialog);
	CollapseWindow(m_bCollapsed);

	// done
//	m_bInitialized = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




//
void CGameReviewDialog::UpdateToolTipText(CPoint point)
{
	//
	if (!pDOC->IsReviewingGame())
		return;

	// first call base class
	CMyCustomDialog::UpdateToolTipText(point);

	// bail out if no data
	if (m_numGamesAvailable == 0)
		return;

	// then operate locally 
	int nIndex = m_pToolTip->ListControlHitTest(point, m_listTags, 1);

	// update text if the cursor is on the right column
	CGameRecord* pGameRecord = pDOC->GetGameRecord(m_nGameIndex);
	if (!pGameRecord)
		return;
	int numTags = pGameRecord->m_mapTagValues.size();
	//
	if ((nIndex >= 0) && (nIndex < numTags))
	{
		CString strValue, strTag = m_listTags.GetItemText(nIndex, 0);
		std::map<CString,CString>::const_iterator iter = pGameRecord->m_mapTagValues.find(strTag);
		if (iter != pGameRecord->m_mapTagValues.end())
		{
			strValue = (*iter).second;
//			m_pToolTip->SetText(strValue, &point);
			m_pToolTip->SetText(strValue);
			m_nPrevTooltipIndex = nIndex;
			m_pToolTip->Show();
		}
		else
		{
			// tag/value not found
			m_pToolTip->Hide();
			m_nPrevTooltipIndex = -1;
		}
	}
	else
    {
		// not on a valid line
		m_pToolTip->Hide();
		m_nPrevTooltipIndex = -1;
	}
}



//
void CGameReviewDialog::OnMove(int x, int y) 
{
	CMyCustomDialog::OnMove(x, y);
	//
	if (IsWindowVisible() && m_bInitialized)
	{
		RECT rect;
		GetWindowRect(&rect);
		pMAINFRAME->SetValue(tnReviewDialogPosLeft, rect.left);
		pMAINFRAME->SetValue(tnReviewDialogPosTop, rect.top);
	}
}



//
void CGameReviewDialog::OnClose() 
{
	pVIEW->EndGameReview();
	CMyCustomDialog::OnClose();
}




////////////////////////////////////////////////////////////////////////
//
// Navigation
//
////////////////////////////////////////////////////////////////////////


//
// SetGameIndex()
//
void CGameReviewDialog::SetGameIndex(int nGame, BOOL bRefresh)
{
	if ((nGame < 0) || (nGame >= m_numGamesAvailable))
		return;

	// init
	m_nGameIndex = nGame;
	m_nBidIndex = 0;
	m_nPlayIndex = 0;
	m_nPlayRound = 0;

	// fill tags list
	// first clear existing items
	m_listTags.DeleteAllItems();
	// then add entries
	CGameRecord* pGameRecord = pDOC->GetGameRecord(m_nGameIndex);
	if (!pGameRecord)
		return;

	// fill the tags list
	int nIndex = 0;
	int nMaxWidth = 0;
	const int tnColSpacer = 14;
	CString strTag, strValue;
	std::map<CString,CString>::const_iterator iter;
	for(iter=pGameRecord->m_mapTagValues.begin();iter!=pGameRecord->m_mapTagValues.end();iter++)
	{
		// get the next tag & value
		strTag = (*iter).first;
		strValue = (*iter).second;

		// skip certain tags
		if ((strTag.CompareNoCase("Auction") == 0) ||
						(strTag.CompareNoCase("Play") == 0))
			continue;

		// set tag column
		m_listTags.InsertItem(nIndex, strTag);

		// set value column
		m_listTags.SetItem(nIndex, 1, LVIF_TEXT, strValue, 0, 0, 0, 0L);
		int nWidth = m_listTags.GetStringWidth(strValue) + tnColSpacer;
		if (nWidth > nMaxWidth)
			nMaxWidth = nWidth;
		nIndex++;
	}

	// set value column width 
	m_listTags.SetColumnWidth(1, nMaxWidth);

	//
	m_nGameIndex = nGame;
	m_nPlayerPosition = pGameRecord->m_nRoundLead[0];
	UpdateData(FALSE);

	// now update the display
	iter = pGameRecord->m_mapTagValues.find(_T("DEAL"));
	if (iter != pGameRecord->m_mapTagValues.end())
		strValue = (*iter).second;
	pDOC->LoadGameRecord(*pGameRecord);
	if (bRefresh)
		pDOC->ResetDisplay();
	pDOC->UpdateBiddingHistory();
	pDOC->UpdatePlayHistory();
	pMAINFRAME->SetAllIndicators();

	// NCR-759 Check if this hand has data to show
	if(pDOC->GetDeclarerPosition() == NONE) {
		return;   // No plays to make
	}  // end NCR-759

	// set prompt
	pMAINFRAME->SetMessageText(FormString("Contract is %s.  Declarer is %s; %s leads.",
										   pDOC->GetFullContractString(),
										   PositionToString(pDOC->GetDeclarerPosition()),
										   PositionToString(pDOC->GetRoundLead())));
	// set game info
	m_numPlaysAvailable = pGameRecord->m_numCardsPlayed;
	if (m_numPlaysAvailable > 0)
		m_numTricksAvailable = ((m_numPlaysAvailable-1) / 4) + 1;
	else
		m_numTricksAvailable = 0;

	// reset suit sequence
	theApp.InitDummySuitSequence(BID_SUIT(pGameRecord->m_nContract), GetPartner(pGameRecord->m_nDeclarer));

	// enable/disable play controls
	GetDlgItem(IDC_PREV)->EnableWindow(FALSE);
	GetDlgItem(IDC_FIRST)->EnableWindow(FALSE);
	if (m_numPlaysAvailable <= 1)
	{
		GetDlgItem(IDC_NEXT)->EnableWindow(FALSE);
		GetDlgItem(IDC_LAST)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_NEXT)->EnableWindow(TRUE);
		GetDlgItem(IDC_LAST)->EnableWindow(TRUE);
	}

	// and enable/disable game nav controls
	if (m_nGameIndex == 0)
	{
		GetDlgItem(IDC_PREV_GAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_FIRST_GAME)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_PREV_GAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_FIRST_GAME)->EnableWindow(TRUE);
	}
	//
	if (m_nGameIndex >= m_numGamesAvailable-1)
	{
		GetDlgItem(IDC_NEXT_GAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_LAST_GAME)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_NEXT_GAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_LAST_GAME)->EnableWindow(TRUE);
	}
}



//
// SetPlayRound()
//
void CGameReviewDialog::SetPlayRound(int nRound)
{
	if ((m_nGameIndex < 0) || (nRound < 0))
		return;
	// 
	m_nPlayRound = nRound;
	if (m_nPlayRound == 0)
		m_nPlayIndex = 0;

	//
	if (m_nPlayRound >= m_numTricksAvailable)
	{
		GetDlgItem(IDC_NEXT)->EnableWindow(FALSE);
		GetDlgItem(IDC_LAST)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_NEXT)->EnableWindow(TRUE);
		GetDlgItem(IDC_LAST)->EnableWindow(TRUE);
	}
	//
	if (m_nPlayRound == 0)
	{
		GetDlgItem(IDC_PREV)->EnableWindow(FALSE);
		GetDlgItem(IDC_FIRST)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_PREV)->EnableWindow(TRUE);
		GetDlgItem(IDC_FIRST)->EnableWindow(TRUE);
	}
}


//
void CGameReviewDialog::OnSelchangeGameIndex() 
{
	UpdateData(TRUE);
	SetGameIndex(m_nGameIndex);
}


// 
void CGameReviewDialog::ShowTrick(int nTrick)
{
}




//
void CGameReviewDialog::OnPrev() 
{
	if (m_nPlayRound == 0)
		return;

	// get the game record
	CGameRecord* pGameRecord = pDOC->GetGameRecord(m_nGameIndex);

	// undo the last trick
	CEasyBDoc* pDoc = pDOC;
	int numCardsPlayed = pDoc->GetNumCardsPlayedInRound();

	// return if there's nothing to undo
	if ((m_nPlayRound == 0) && (numCardsPlayed == 0))
		return;

	// decrement the play round one extra if at the end of play record
	// play round = # of rounds played so far
	// undo cards from the current trick, if it has cards
	if (numCardsPlayed > 0)
	{
		pDoc->UndoTrick();
		m_nPlayRound--;
		m_nPlayIndex -= numCardsPlayed;
	}

	// see if we're at the start
	if (m_nPlayRound == 0)
	{
		// can't go back any further
		SetPlayRound(0);
		// hide dummy
		pDOC->ExposeDummy(FALSE);
		//
		pVIEW->PostMessage(WM_COMMAND, WMS_RESET_DISPLAY, 1L);
		pMAINFRAME->SetStatusMessage("Start of play record.");
		pDOC->UpdatePlayHistory();
		return;
	}

	// if past the first round, clear the previous trick as well
	pDoc->UndoPreviousTrick();
	m_nPlayRound--;
	m_nPlayIndex -= 4;
	if (m_nPlayIndex < 0)
		m_nPlayIndex = 0;
	//
	pDoc->ResetDisplay();
	pVIEW->DisplayTricks();

	// then re-display the trick
	OnNext();
}



//
void CGameReviewDialog::OnNext() 
{
	if (m_nPlayRound >= m_numTricksAvailable)
		return;

	// get the game record
	CGameRecord* pGameRecord = pDOC->GetGameRecord(m_nGameIndex);
	pMAINFRAME->ClearStatusMessage();

	// and show the next trick -- this is a bit tricky
	// first clear previous trick if appropriate
	// play round = # of rounds played so far
	if (m_nPlayRound > 0)
	{
		if (pDOC->GetNumCardsPlayedInRound() == 4)
		{
			pVIEW->ClearTable();
			pDOC->ClearTrick();
		}
		else
		{
			// play record ended prematurely
			pVIEW->ClearPartialTrickCards();
		}
	}

	// see if we've come to the end of hand or end of play record
	if (m_nPlayRound == 13)
	{
		// done with hand
		pMAINFRAME->SetStatusMessage("End of play record.");
		return;
	}
	else if (m_nPlayRound >= m_numTricksAvailable)
	{
		pMAINFRAME->SetStatusMessage("End of play record.");
		return;
	}

	//	
	int nPos = pGameRecord->m_nRoundLead[m_nPlayRound];
	int nPosInPlay = m_nPlayIndex % 4;
	for(int i=0;i<4;i++)
	{
		//
		m_nPlayerPosition = nPos;
		int nCardPlayed = pGameRecord->m_nGameTrick[m_nPlayRound][m_nPlayerPosition];
		CCard* pCard = deck.GetSortedCard(nCardPlayed);
		if (!pCard)
		{
			AfxMessageBox("Invalid Play Record!");
			GetDlgItem(IDC_NEXT)->EnableWindow(FALSE);
			GetDlgItem(IDC_LAST)->EnableWindow(FALSE);
			return;
		}

		// sanity check
		ASSERT(PLAYER(m_nPlayerPosition).HasCard(pCard));

		// see if we should expose dummy here
		if ((m_nPlayRound == 0) && (i == 1))
		{
			pDOC->ExposeDummy();
//			pDOC->RefreshDisplay();
		}

		//
		pVIEW->ThrowCard((Position)m_nPlayerPosition, pCard);

		// delay between cards
		if (i == 0)
			::Sleep(250);
		// NCR use the Options delays for these plays also
		if (theApp.GetValue(tbInsertPlayPause))
			::Sleep(theApp.GetValue(tnPlayPauseLength)*100);


		// advance to the next player
		nPos = GetNextPlayer(nPos);
		m_nPlayIndex++;
		if (m_nPlayIndex >= m_numPlaysAvailable)
		{
			pMAINFRAME->SetStatusMessage("End of play record.");
			break;
		}
	}

	// evaluate the trick results
	if (pDOC->GetNumCardsPlayedInRound() == 4)
		pDOC->EvaluateTrick();
	//
	pDOC->UpdatePlayHistory();

	// and update the index
	SetPlayRound(++m_nPlayRound);
}




//
void CGameReviewDialog::OnLast() 
{
	// fast forward to the end
	if (m_nPlayRound >= m_numTricksAvailable)
		return;

	// show wait cursor
	CWaitCursor wait;
//	pMAINFRAME->SetStatusMessage(_T("Advancing to the end of the play record..."));

	// get the game record
	CGameRecord* pGameRecord = pDOC->GetGameRecord(m_nGameIndex);
	pMAINFRAME->ClearStatusMessage();

	// first clear previous trick if appropriate
	if (m_nPlayRound > 0)
	{
		if (pDOC->GetNumCardsPlayedInRound() == 4)
		{
			pVIEW->ClearTable();
			pDOC->ClearTrick();
		}
		else
		{
			// play record ended prematurely
			pVIEW->ClearPartialTrickCards();
		}
	}

	// see if we've come to the end of hand or end of play record
	while (m_nPlayRound < m_numTricksAvailable)
	{
		//	
		int nPos = pGameRecord->m_nRoundLead[m_nPlayRound];
		int nPosInPlay = m_nPlayIndex % 4;
		for(int i=0;i<4;i++)
		{
			//
			m_nPlayerPosition = nPos;
			int nCardPlayed = pGameRecord->m_nGameTrick[m_nPlayRound][m_nPlayerPosition];
			CCard* pCard = deck.GetSortedCard(nCardPlayed);
			if (!pCard)
			{
				AfxMessageBox("Invalid Play Record!");
				GetDlgItem(IDC_NEXT)->EnableWindow(FALSE);
				GetDlgItem(IDC_LAST)->EnableWindow(FALSE);
				return;
			}

			// sanity check
			ASSERT(PLAYER(m_nPlayerPosition).HasCard(pCard));

			// see if we should expose dummy here
			if ((m_nPlayRound == 0) && (i == 1))
			{
				PLAYER(pDOC->GetDummyPosition()).SetDummyFlag(TRUE);
				pDOC->ExposeDummy();
			}

			// play the card
			pDOC->EnterCardPlay((Position)nPos, pCard);

			// if this is the last round, show the card
			if (m_nPlayRound == m_numTricksAvailable-1)
				pVIEW->DrawPlayedCard((Position)nPos, pCard, FALSE);

			// advance to the next player
			nPos = GetNextPlayer(nPos);
			m_nPlayIndex++;
			if (m_nPlayIndex >= m_numPlaysAvailable)
				break;
		}

		// evaluate the trick results
		if (pDOC->GetNumCardsPlayedInRound() == 4)
		{
			pDOC->EvaluateTrick(TRUE);
			// but don't clear the final trick; leave it to show
			if (m_nPlayIndex < m_numPlaysAvailable)
				pDOC->ClearTrick();
		}
		//
		m_nPlayRound++;
		if (m_nPlayIndex >= m_numPlaysAvailable)
			break;
	}

	// update status
	pDOC->ResetDisplay();
	pVIEW->DisplayTricks();
	pDOC->UpdatePlayHistory();
	pMAINFRAME->SetStatusMessage("End of play record.");
	SetPlayRound(m_nPlayRound);
}




//
void CGameReviewDialog::OnFirst() 
{
	// undo the last trick
	CEasyBDoc* pDoc = pDOC;
	int numCardsPlayed = pDoc->GetNumCardsPlayedInRound();

	// return if there's nothing to undo
	if ((m_nPlayRound == 0) && (numCardsPlayed == 0))
		return;

	// suppress flashing
	CWaitCursor wait;
	pDoc->SuppressPlayHistoryUpdate(TRUE);
	pMAINFRAME->SetStatusMessage(_T("Backing up to the beginning of play..."));

	// decrement the play round one extra if at the end of play record
	if (m_nPlayRound > m_numTricksAvailable)
		m_nPlayRound--;

	// undo cards from the current trick
	if (numCardsPlayed > 0)
	{
		pDoc->UndoTrick();
		m_nPlayRound--;
		m_nPlayIndex -= numCardsPlayed;
	}

	// and all previous tricks
	while(m_nPlayRound > 0)
	{
		pDoc->UndoPreviousTrick();
		m_nPlayRound--;
		m_nPlayIndex -= 4;
	}

	// and reset
	SetPlayRound(0);

	// hide dummy
	pDOC->ExposeDummy(FALSE);

	// and reset display
	pDoc->ResetDisplay();
	pMAINFRAME->ClearStatusMessage();
	pDoc->SuppressPlayHistoryUpdate(FALSE);
	pDOC->UpdatePlayHistory();

	// done
	return;
}



//
void CGameReviewDialog::OnNextGame() 
{
	if (m_nGameIndex >= m_numGamesAvailable)
		return;
	//
	SetGameIndex(m_nGameIndex+1);
}



//
void CGameReviewDialog::OnPrevGame() 
{
	if (m_nGameIndex == 0)
		return;
	//
	SetGameIndex(m_nGameIndex-1);
}


//
void CGameReviewDialog::OnLastGame() 
{
	SetGameIndex(m_numGamesAvailable-1);
}


//
void CGameReviewDialog::OnFirstGame() 
{
	SetGameIndex(0);
}


//
void CGameReviewDialog::CollapseWindow(BOOL bCollapse) 
{
	CRect rect;
	GetWindowRect(&rect);
	//
	if (bCollapse)
	{
		// collapsing window
		rect.right = rect.left + m_nCollapsedWidth; 
		m_bCollapsed = TRUE;
//		((CButton*)GetDlgItem(IDC_EXPAND_COLLAPSE))->SetIcon(m_hIconExpand);
		m_flatButtons[tnExpandCollapseIndex].SetIcon(IDI_TAGS_EXPAND);
		m_pControlInfo[8].szToolTipText = "Expand tags list";
	}
	else
	{
		// expanding window
		rect.right = rect.left + m_nFullWidth;
		m_bCollapsed = FALSE;
//		((CButton*)GetDlgItem(IDC_EXPAND_COLLAPSE))->SetIcon(m_hIconCollapse);
		m_flatButtons[tnExpandCollapseIndex].SetIcon(IDI_TAGS_COLLAPSE);
		m_pControlInfo[8].szToolTipText = "Collapse tags list";
	}
	//
	MoveWindow(&rect, TRUE);
}


//
void CGameReviewDialog::OnExpandCollapse() 
{
	if (m_bCollapsed)
	{
		// expand window
		CollapseWindow(FALSE);
	}
	else
	{
		// collapse window window
		CollapseWindow(TRUE);
	}
	//
	theApp.SetValue(tbCollapseGameReviewDialog, m_bCollapsed);
}


//
void CGameReviewDialog::OnPlay() 
{
	// clear previous trick
	if (m_nPlayRound > 0)
	{
		if (pDOC->GetNumCardsPlayedInRound() == 4)
		{
			pVIEW->ClearTable();
			pDOC->ClearTrick();
		}
/*
		else
		{
			// play record ended prematurely
			pVIEW->ClearPartialTrickCards();
		}
*/
	}
	//
	pDOC->PlayGameRecord(m_nGameIndex);	
}



//
void CGameReviewDialog::OnSaveOut() 
{
	pMAINFRAME->PostMessage(WM_COMMAND, ID_FILE_SAVE_AS);	
}



//
BOOL CGameReviewDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	WinHelp(DIALOG_GAME_REVIEW);
	return TRUE;	
//	return CMyCustomDialog::OnHelpInfo(pHelpInfo);
}
