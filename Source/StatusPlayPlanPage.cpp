//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// StatusPlayPlanPage.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "easybDoc.h"
#include "mainfrm.h"
#include "StatusPlayPlanPage.h"
#include "subclass.h"
#include "card.h"
#include "cardlist.h"
#include "player.h"
#include "play.h"
#include "playlist.h"
#include "DeclarerPlayEngine.h"
#include "MyToolTipWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusPlayPlanPage property page

IMPLEMENT_DYNCREATE(CStatusPlayPlanPage, CPropertyPage)

CStatusPlayPlanPage::CStatusPlayPlanPage() : CPropertyPage(CStatusPlayPlanPage::IDD)
{
	//{{AFX_DATA_INIT(CStatusPlayPlanPage)
	m_strDeclarer = _T("");
	m_strNumPlays = _T("");
	m_strCurrentPlay = _T("");
	//}}AFX_DATA_INIT
	m_bInitialized = FALSE;
	m_nPrevTooltipIndex = -1;
}

CStatusPlayPlanPage::~CStatusPlayPlanPage()
{
}

void CStatusPlayPlanPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatusPlayPlanPage)
	DDX_Control(pDX, IDC_PLAY_PLAN, m_listPlayPlan);
	DDX_Text(pDX, IDC_DECLARER_LABEL, m_strDeclarer);
	DDX_Text(pDX, IDC_NUM_PLAYS, m_strNumPlays);
	DDX_Text(pDX, IDC_CURRENT_PLAY, m_strCurrentPlay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatusPlayPlanPage, CPropertyPage)
	//{{AFX_MSG_MAP(CStatusPlayPlanPage)
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusPlayPlanPage message handlers

static LPCTSTR tszColumnName[] = {
	"#", 
	"Type        ", 
	"Description                            ", 
	"Winner?  ", 
	"Uses", 
	"Tgt Cd", 
	"Key Cds", 
	"Alt Key Cds",
	"Alt K/C 2",
	"Opp. Key Cds", 
	"Opp. Alt Key Cds",
	"Req. Played Cds",
};

static LPCSTR szProspectDescription[] = {
	"???",
	"No",
	"Maybe",
	"Probable",
	"Yes",
};


//
BOOL CStatusPlayPlanPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	const int tnColSpacer = 14;

	// init the list control
	int numCols = sizeof(tszColumnName) / sizeof(LPCTSTR);
	for(int i=0;i<numCols;i++)
		m_listPlayPlan.InsertColumn(i, tszColumnName[i], LVCFMT_LEFT, m_listPlayPlan.GetStringWidth(tszColumnName[i]) + tnColSpacer, i);

	// subclass the list control
	wpOrigListCtrlProc = (WNDPROC) SetWindowLong(m_listPlayPlan.GetSafeHwnd(), GWL_WNDPROC, (LONG) ListCtrlSubclassProc);

	// record initial window characterisitcs
	CRect winRect, listRect;
	GetClientRect(&winRect);
	m_listPlayPlan.GetWindowRect(&listRect);
	ScreenToClient(&listRect);
	m_nInitialX = listRect.left;
	m_nInitialY = listRect.top;
	m_nInitialXMargin = listRect.left - winRect.left;
	m_nInitialYMargin = winRect.bottom - listRect.bottom;

/*
 * code for standard tooltip control
 *
	// init and enable tool tips
	static TOOLINFO toolInfo;
	m_toolTip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX);
	m_toolTip.AddTool(&m_listPlayPlan, "");
	m_toolTip.SetToolInfo(&toolInfo);
	m_toolTip.SetDelayTime(200);
	m_toolTip.Activate(TRUE);
*/
	// init custom tooltip wnd
//	CString strClassName = AfxRegisterWndClass(CS_SAVEBITS);
/*
	m_toolTip.Create(WS_EX_LEFT | WS_EX_NOPARENTNOTIFY | WS_EX_TOOLWINDOW,
					 WS_BORDER | WS_POPUP,
					 strClassName,
					 "My Tooltip Window",
					 0, 0, 0, 0 ,
					 m_hWnd, NULL);
	m_toolTip.Show(SW_HIDE);
*/

	// create the custom tooltip window
	m_pToolTip = new CMyToolTipWnd;
	m_pToolTip->Create(this);

	// done with the inits, so show data
	m_bInitialized = TRUE;
	Update();

	// all done
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CStatusPlayPlanPage::OnDestroy() 
{
	delete m_pToolTip;
//	m_toolTip.DelTool(&m_listPlayPlan);
	m_listPlayPlan.DeleteAllItems();
	CPropertyPage::OnDestroy();
}


//
BOOL CStatusPlayPlanPage::PreTranslateMessage(MSG* pMsg) 
{
/*
 * standard tooltip code
	m_toolTip.RelayEvent(pMsg);
 */
	return CPropertyPage::PreTranslateMessage(pMsg);
}


//
BOOL CStatusPlayPlanPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch(wParam)
	{
		case WMS_UPDATE_TOOLTIP_TEXT:
			// update tooltip text
			CPoint point(LOWORD(lParam),HIWORD(lParam));
/*
 * standard tooltip code
 *
			TOOLINFO toolInfo;
			if (m_toolTip.HitTest(&m_listPlayPlan, point, &toolInfo))
				UpdateToolTipText(point);
 */
			m_listPlayPlan.ClientToScreen(&point);
			UpdateToolTipText(point);
			return TRUE;
	}

	// call default
	return CPropertyPage::OnCommand(wParam, lParam);
}


//
void CStatusPlayPlanPage::OnMouseMove(UINT nFlags, CPoint point) 
{
	// see if we should disable the tooltips window
	CRect rect;
	m_listPlayPlan.GetWindowRect(&rect);
	ScreenToClient(&point);
	if (!rect.PtInRect(point))
		m_pToolTip->ShowWindow(SW_HIDE);
	//
	CPropertyPage::OnMouseMove(nFlags, point);
}


//
void CStatusPlayPlanPage::UpdateToolTipText(CPoint point)
{
/*
 * code for standard tooltip control
 *
	// get the declarer & his play engine
	if (!theApp.IsGameInProgress())
		return;
	CPlayer* pPlayer = pDOC->GetDeclarer();
	CDeclarerPlayEngine* pPlayEngine = pPlayer->GetDeclarerEngine();
	CPlayList& playPlan = pPlayEngine->GetPlayPlan();
	// get number of available plays
	int nCount = playPlan.GetSize();
	if (nCount == 0)
	{
	    m_toolTip.UpdateTipText("", &m_listPlayPlan);
		return;
	}
	// get the index of the play under the cursor
	int nIndex = -1;
	int nFirstVisible = m_listPlayPlan.GetTopIndex();
	int numVisible = m_listPlayPlan.GetCountPerPage();
	for(int i=nFirstVisible;i<nFirstVisible+numVisible;i++)
	{
		CRect rect;
		if (m_listPlayPlan.GetItemRect(i, &rect, LVIR_BOUNDS))
		{
			if (rect.PtInRect(point))
			{
				nIndex = i;
				break;
			}
		}
	}
	// update text 
	if ((nIndex >= 0) && (nIndex < nCount))
	{
		if (nIndex != m_nPrevTooltipIndex)
		{
			CPlay* pPlay = playPlan[nIndex];
			m_toolTip.UpdateTipText(pPlay->GetFullDescription(), &m_listPlayPlan);
			m_nPrevTooltipIndex = nIndex;
		}
	}
	else
    {
		m_toolTip.UpdateTipText("", &m_listPlayPlan);
	}
*/
	//
	// new code
	//
	if (!m_bInitialized)
		return;

	// get client position
	CPoint clientPoint = point;
	m_listPlayPlan.ScreenToClient(&clientPoint);
	// get the declarer & his play engine
	if (!theApp.IsGameInProgress())
		return;
	CPlayer* pPlayer = pDOC->GetDeclarer();
	CDeclarerPlayEngine* pPlayEngine = pPlayer->GetDeclarerEngine();
	CPlayList& playPlan = pPlayEngine->GetPlayPlan();
	// get number of available plays
	int nCount = playPlan.GetSize();
	if (nCount == 0)
	{
	    m_pToolTip->SetText("", &point);
		return;
	}
	// get the index of the play under the cursor
	int nIndex = -1;
	int nFirstVisible = m_listPlayPlan.GetTopIndex();
	int numVisible = m_listPlayPlan.GetCountPerPage();
	CRect itemRect;
	for(int i=nFirstVisible;i<nFirstVisible+numVisible;i++)
	{
		if (m_listPlayPlan.GetItemRect(i, &itemRect, LVIR_BOUNDS))
		{
			if (itemRect.PtInRect(clientPoint))
			{
				nIndex = i;
				break;
			}
		}
	}
	// update text, if the cursor is on the right column
	if ((nIndex >= 0) && (nIndex < nCount))
	{
		const int tnDescColumn = 2;
		BOOL bShow = FALSE;
		BOOL bInPosition = FALSE;
		// check if the cursor is within the description col
		CRect targetRect;
		m_listPlayPlan.ClientToScreen(&itemRect);
		targetRect.left = itemRect.left + m_listPlayPlan.GetColumnWidth(0) + m_listPlayPlan.GetColumnWidth(1);
		targetRect.right = targetRect.left + m_listPlayPlan.GetColumnWidth(tnDescColumn);
		if ((point.x >= targetRect.left) && (point.x <= targetRect.right))
		{
			// check if we moved to a new line
			if (nIndex != m_nPrevTooltipIndex)
			{
				CPlay* pPlay = playPlan[nIndex];
				// align the tooltip with list control entry's description
				point.x = targetRect.left;	
				point.y = itemRect.top;	
				// account for column padding
				point.x += 5;
				point.y -= 1;
				//
				m_pToolTip->SetText(pPlay->GetFullDescription(), &point);
				m_nPrevTooltipIndex = nIndex;
				m_pToolTip->Show();
			}
			else
			{
				// on the same line; else leave the mesage as it is
			}
		}
		else
		{
			// not within the desc column
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
	// and show the tooltip
}


//
void CStatusPlayPlanPage::Clear()
{
	if (!m_bInitialized)
		return;
	//
	m_strDeclarer = "Declarer: None";
	m_listPlayPlan.DeleteAllItems();
	UpdateData(FALSE);
	m_nPrevTooltipIndex = -1;
}


//
void CStatusPlayPlanPage::Update()
{
	// return if not all inits are complete
	if ((!m_bInitialized) || (pDOC == NULL))
		return;
	if (!theApp.IsGameInProgress())
	{
		m_strDeclarer = "None";
		UpdateData(FALSE);
		return;
	}

	// clear existing items
	m_listPlayPlan.DeleteAllItems();

	// get the declarer & his play engine
	int nDeclarer = pDOC->GetDeclarerPosition();
	if (!ISPLAYER(nDeclarer))
		return;
	CPlayer* pPlayer = pDOC->GetDeclarer();
	CDeclarerPlayEngine* pPlayEngine = pPlayer->GetDeclarerEngine();
	CPlayList& playPlan = pPlayEngine->GetPlayPlan();

	// list declarer
	m_strDeclarer.Format("%s", PositionToString(pPlayer->GetPosition()));

	// & number of available plays
	int nCount = playPlan.GetSize();
	m_strNumPlays.Format("%d",nCount);

	// also list the current play
	const CPlay* pPlay = pPlayEngine->GetCurrentPlay();
	CString strFailedPlay = pPlayEngine->GetFailedPlayName();
	if (pPlay)
		m_strCurrentPlay = (const_cast<CPlay*>(pPlay))->GetDescription();
	else if (!strFailedPlay.IsEmpty())
		m_strCurrentPlay = strFailedPlay;
	else
		m_strCurrentPlay = "None";

	// now list the plays
	for(int i=0;i<nCount;i++)
	{
		CPlay* pPlay = playPlan[i];
		// set number
		m_listPlayPlan.InsertItem(i, FormString(i+1));
		// set name
		m_listPlayPlan.SetItem(i, 1, LVIF_TEXT, pPlay->GetPlayTypeName(), 0, 0, 0, 0L);
		// add description
		m_listPlayPlan.SetItem(i, 2, LVIF_TEXT, pPlay->GetDescription(), 0, 0, 0, 0L);
		// add winner coded
		int nProspect = pPlay->GetPlayProspect();
		m_listPlayPlan.SetItem(i, 3, LVIF_TEXT, szProspectDescription[nProspect], 0, 0, 0, 0L);
		// add consumed card
		CCard* pConsumedCard = pPlay->GetConsumedCard();
		m_listPlayPlan.SetItem(i, 4, LVIF_TEXT, (pConsumedCard? pConsumedCard->GetName() : ""), 0, 0, 0, 0L);
		// add target card
		CCard* pTargetCard = pPlay->GetTargetCard();
		m_listPlayPlan.SetItem(i, 5, LVIF_TEXT, (pTargetCard? pTargetCard->GetName() : ""), 0, 0, 0, 0L);
		// add key cards
		CCardList* pKeyCards = pPlay->GetKeyCardsList();
		m_listPlayPlan.SetItem(i, 6, LVIF_TEXT, (pKeyCards? pKeyCards->GetHoldingsString() : ""), 0, 0, 0, 0L);
		// add OR-Key cards
		CCardList* pORKeyCards = pPlay->GetOrKeyCardsList();
		m_listPlayPlan.SetItem(i, 7, LVIF_TEXT, (pORKeyCards? pORKeyCards->GetHoldingsString() : ""), 0, 0, 0, 0L);
		// add OR-Key cards 2
		CCardList* pORKeyCards2 = pPlay->GetOrKeyCardsList2();
		m_listPlayPlan.SetItem(i, 8, LVIF_TEXT, (pORKeyCards2? pORKeyCards2->GetHoldingsString() : ""), 0, 0, 0, 0L);
		// add opponents' key cards
		CCardList* pOppKeyCards = pPlay->GetEnemyKeyCardsList();
		m_listPlayPlan.SetItem(i, 9, LVIF_TEXT, (pOppKeyCards? pOppKeyCards->GetHoldingsString() : ""), 0, 0, 0, 0L);
		// add opponents' OR-Key cards
		CCardList* pOppORKeyCards = pPlay->GetEnemyOrKeyCardsList();
		m_listPlayPlan.SetItem(i, 10, LVIF_TEXT, (pOppORKeyCards? pOppORKeyCards->GetHoldingsString() : ""), 0, 0, 0, 0L);
		// add required played cards
		CCardList* pReqPlayedCards = pPlay->GetRequiredPlayedCardsList();
		m_listPlayPlan.SetItem(i, 11, LVIF_TEXT, (pReqPlayedCards? pReqPlayedCards->GetHoldingsString() : ""), 0, 0, 0, 0L);
	}

	// done
	UpdateData(FALSE);
	m_nPrevTooltipIndex = -1;
}


//
void CStatusPlayPlanPage::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//	CPropertyPage::OnKeyDown(nChar, nRepCnt, nFlags);
//	GetParent()->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
	pMAINFRAME->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
}


//
void CStatusPlayPlanPage::OnSize(UINT nType, int cx, int cy) 
{
	if (!m_bInitialized)
		return;
	CPropertyPage::OnSize(nType, cx, cy);

	// resize the list control
	CRect rect;
	GetClientRect(&rect);
	m_listPlayPlan.MoveWindow(m_nInitialX, m_nInitialY, 
							  rect.Width() - m_nInitialXMargin*2,
							  rect.Height() - m_nInitialY - m_nInitialYMargin);

	// resize the static text
	CWnd* pStatic = GetDlgItem(IDC_CURRENT_PLAY);
	pStatic->GetWindowRect(&rect);
	ScreenToClient(&rect);
	int dx = cx - rect.right;
	rect.InflateRect(0, 0, dx-4, 0);
	pStatic->MoveWindow(&rect);
}


//
void CStatusPlayPlanPage::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// we don't do anything special here, so send it up to the parent
	ClientToScreen(&point);
	CWnd* pParent = GetParent();
	pParent->ScreenToClient(&point);
	pParent->SendMessage(WM_RBUTTONDOWN, nFlags, MAKELONG(point.x, point.y));
}
