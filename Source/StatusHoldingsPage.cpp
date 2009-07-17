//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// StatusHoldingsPage.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "easybDoc.h"
#include "mainfrm.h"
#include "StatusHoldingsPage.h"
#include "card.h"
#include "cardlist.h"
#include "player.h"
#include "DeclarerPlayEngine.h"
#include "CombinedHoldings.h"
#include "CombinedSuitHoldings.h"
#include "StatusSheet.h"
#include "Subclass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusHoldingsPage property page

IMPLEMENT_DYNCREATE(CStatusHoldingsPage, CPropertyPage)

CStatusHoldingsPage::CStatusHoldingsPage() : CPropertyPage(CStatusHoldingsPage::IDD)
{
	//{{AFX_DATA_INIT(CStatusHoldingsPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bInitialized = FALSE;
}

CStatusHoldingsPage::~CStatusHoldingsPage()
{
}

void CStatusHoldingsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatusHoldingsPage)
	DDX_Control(pDX, IDC_SUIT_STATUS, m_listSuitStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatusHoldingsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CStatusHoldingsPage)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusHoldingsPage message handlers



//
static LPCTSTR tszColumnName[] = {
	"Decl  Winners", 
	"Dummy Winners", 
	"Decl Entries", 
	"Dummy Entries", 
};


//
BOOL CStatusHoldingsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	const int tnColSpacer = 14;  // NCR added int

	// init the list control
	m_listSuitStatus.SetImageList(&CStatusSheet::m_imageList, LVSIL_SMALL);
	m_listSuitStatus.InsertColumn(0, "", LVCFMT_LEFT, 16, 0);	// icon column

	// add columns
	int numCols = sizeof(tszColumnName) / sizeof(LPCTSTR);
	for(int i=0;i<numCols;i++)
		m_listSuitStatus.InsertColumn(i+1, tszColumnName[i], LVCFMT_LEFT, m_listSuitStatus.GetStringWidth(tszColumnName[i]) + tnColSpacer, i+1);

	// subclass the list control
//	wpOrigListCtrlProc = (WNDPROC) SetWindowLong(m_listSuitStatus.GetSafeHwnd(), GWL_WNDPROC, (LONG) ListCtrlSubclassProc);

	// record initial window characterisitcs
	CRect winRect, listRect;
	GetClientRect(&winRect);
	m_listSuitStatus.GetWindowRect(&listRect);
	ScreenToClient(&listRect);
	m_nInitialX = listRect.left;
	m_nInitialY = listRect.top;
	m_nInitialXMargin = listRect.left - winRect.left;
	m_nInitialYMargin = winRect.bottom - listRect.bottom;

	// done with the inits, so show data
	m_bInitialized = TRUE;
	Update();

	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CStatusHoldingsPage::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//	CPropertyPage::OnKeyDown(nChar, nRepCnt, nFlags);
//	GetParent()->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
	pMAINFRAME->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
}



//
void CStatusHoldingsPage::OnSize(UINT nType, int cx, int cy) 
{
	if (!m_bInitialized)
		return;
	CPropertyPage::OnSize(nType, cx, cy);
	// resize the list control
	CRect rect;
	GetClientRect(&rect);
	//
	m_listSuitStatus.MoveWindow(m_nInitialX, m_nInitialY, 
							    rect.Width() - m_nInitialXMargin*2,
							    rect.Height() - m_nInitialY - m_nInitialYMargin);
}



//
void CStatusHoldingsPage::OnDestroy() 
{
	// clean up
	m_listSuitStatus.DeleteAllItems();
	CPropertyPage::OnDestroy();
}




//
void CStatusHoldingsPage::Clear()
{
	if (!m_bInitialized)
		return;
	//
	m_listSuitStatus.DeleteAllItems();
	UpdateData(FALSE);
}



//
void CStatusHoldingsPage::Update()
{
	// return if not all inits are complete
	if ((!m_bInitialized) || (pDOC == NULL))
		return;
	if (!theApp.IsGameInProgress())
		return;

	// clear existing items
	m_listSuitStatus.DeleteAllItems();

	// get the declarer & his play engine
	int nDeclarer = pDOC->GetDeclarerPosition();
	if (!ISPLAYER(nDeclarer))
		return;
	CPlayer* pPlayer = pDOC->GetDeclarer();
	CDeclarerPlayEngine* pPlayEngine = pPlayer->GetDeclarerEngine();
	CCombinedHoldings& combinedHand = pPlayEngine->GetCombinedHand();
	CPlayList& playPlan = pPlayEngine->GetPlayPlan();

	// now list information for each suit
	for(int i=0;i<4;i++)
	{
		int nSuit = theApp.GetSuitSequence(i);
		CCombinedSuitHoldings& suit = combinedHand.GetSuit(nSuit);

		// insert dummy column
		m_listSuitStatus.InsertItem(i, "", nSuit);

		// then insert # of declarer winners
		m_listSuitStatus.SetItem(i, 1, LVIF_TEXT, FormString(suit.GetNumDeclarerWinners()), 0, 0, 0, 0L);
		// then insert # of dummy winners
		m_listSuitStatus.SetItem(i, 2, LVIF_TEXT, FormString(suit.GetNumDummyWinners()), 0, 0, 0, 0L);

		// then insert # of declarer entries
		int numDeclarerEntries = pPlayEngine->GetNumDeclarerEntriesInSuit(nSuit);
		if (nSuit == pDOC->GetTrumpSuit())
		{
			int numDeclarerRuffingEntries = pPlayEngine->GetNumDeclarerRuffingEntries();
			m_listSuitStatus.SetItem(i, 3, LVIF_TEXT, FormString("%d + %d", numDeclarerEntries-numDeclarerRuffingEntries, numDeclarerRuffingEntries), 0, 0, 0, 0L);
		}
		else
		{
			m_listSuitStatus.SetItem(i, 3, LVIF_TEXT, FormString(numDeclarerEntries), 0, 0, 0, 0L);
		}

		// then insert # of dummy entries
		int numDummyEntries = pPlayEngine->GetNumDummyEntriesInSuit(nSuit);
		if (nSuit == pDOC->GetTrumpSuit())
		{
			int numDummyRuffingEntries = pPlayEngine->GetNumDummyRuffingEntries();
			m_listSuitStatus.SetItem(i, 4, LVIF_TEXT, FormString("%d + %d", numDummyEntries-numDummyRuffingEntries, numDummyRuffingEntries), 0, 0, 0, 0L);
		}
		else
		{
			m_listSuitStatus.SetItem(i, 4, LVIF_TEXT, FormString(numDummyEntries), 0, 0, 0, 0L);
		}
	}

	// done
	UpdateData(FALSE);
}


//
void CStatusHoldingsPage::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// we don't do anything special here, so send it up to the parent
	ClientToScreen(&point);
	CWnd* pParent = GetParent();
	pParent->ScreenToClient(&point);
	pParent->SendMessage(WM_RBUTTONDOWN, nFlags, MAKELONG(point.x, point.y));
}
