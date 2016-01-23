//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// GameOptsInterfacePage.cpp : implementation file
//

#include "stdafx.h"
#include "ObjectWithProperties.h"
#include "GameOptsInterfacePage.h"
#include "progopts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameOptsInterfacePage property page

IMPLEMENT_DYNCREATE(CGameOptsInterfacePage, CPropertyPage)

CGameOptsInterfacePage::CGameOptsInterfacePage(CObjectWithProperties* pApp) : 
		CPropertyPage(CGameOptsInterfacePage::IDD),
		m_app(*pApp)
{
	//{{AFX_DATA_INIT(CGameOptsInterfacePage)
	m_bAutoPlayLastCard = FALSE;
	m_bAutoJumpCursor = FALSE;
	m_bInsertBiddingPause = FALSE;
	m_bInsertPlayPause = FALSE;
	m_strBiddingPause = _T("");
	m_strPlayPause = _T("");
	m_bEnableSpokenBids = FALSE;
	m_nBiddingPause = 0;
	m_nPlayPause = 0;
	//}}AFX_DATA_INIT
	m_bAutoPlayLastCard = m_app.GetValue(tbAutoPlayLastCard);
	m_bAutoJumpCursor = m_app.GetValue(tbAutoJumpCursor);
	m_bInsertBiddingPause = m_app.GetValue(tbInsertBiddingPause);
	m_bInsertPlayPause = m_app.GetValue(tbInsertPlayPause);
	m_bEnableSpokenBids = m_app.GetValue(tbEnableSpokenBids);
	m_nBiddingPause = m_app.GetValue(tnBiddingPauseLength);
	m_nPlayPause = m_app.GetValue(tnPlayPauseLength);
}

CGameOptsInterfacePage::~CGameOptsInterfacePage()
{
}

void CGameOptsInterfacePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGameOptsInterfacePage)
	DDX_Control(pDX, IDC_BID_PAUSE, m_sliderBiddingPause);
	DDX_Control(pDX, IDC_PLAY_PAUSE, m_sliderPlayPause);
	DDX_Check(pDX, IDC_AUTOPLAY_LAST_CARD, m_bAutoPlayLastCard);
	DDX_Check(pDX, IDC_AUTO_JUMP_CURSOR, m_bAutoJumpCursor);
	DDX_Check(pDX, IDC_ENABLE_BIDDING_PAUSE, m_bInsertBiddingPause);
	DDX_Check(pDX, IDC_ENABLE_PLAY_PAUSE, m_bInsertPlayPause);
	DDX_Text(pDX, IDC_TEXT_BID_PAUSE, m_strBiddingPause);
	DDX_Text(pDX, IDC_TEXT_PLAY_PAUSE, m_strPlayPause);
	DDX_Check(pDX, IDC_SPOKEN_BIDS, m_bEnableSpokenBids);
	DDX_Slider(pDX, IDC_BID_PAUSE, m_nBiddingPause);
	DDX_Slider(pDX, IDC_PLAY_PAUSE, m_nPlayPause);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGameOptsInterfacePage, CPropertyPage)
	//{{AFX_MSG_MAP(CGameOptsInterfacePage)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_ENABLE_BIDDING_PAUSE, OnEnablePause)
	ON_BN_CLICKED(IDC_ENABLE_PLAY_PAUSE, OnEnablePause)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameOptsInterfacePage message handlers


//
BOOL CGameOptsInterfacePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	//
	m_sliderBiddingPause.SetRange(0, 20);
	m_sliderBiddingPause.SetTicFreq(2);
	m_sliderBiddingPause.SetPos(m_nBiddingPause);
	//
	m_sliderPlayPause.SetRange(0, 20);
	m_sliderPlayPause.SetTicFreq(2);
	m_sliderPlayPause.SetPos(m_nPlayPause);
	UpdateData(FALSE);
	//
	UpdatePauseLabels();
	EnableOrDisablePauseSliders();

	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



//
void CGameOptsInterfacePage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateData(TRUE);
	UpdatePauseLabels();
}


//
void CGameOptsInterfacePage::UpdatePauseLabels()
{
	double fBiddingPause = m_nBiddingPause / 10.0;
	m_strBiddingPause.Format(_T("%.1f second%s"), fBiddingPause, ((fBiddingPause > 1)? "s" : ""));
	//
	double fPlayPause = m_nPlayPause / 10.0;
	m_strPlayPause.Format(_T("%.1f second%s"), fPlayPause, ((fPlayPause > 1)? "s" : ""));
	//
	UpdateData(FALSE);
}



//
void CGameOptsInterfacePage::Update()
{
	// store results
	m_app.SetValue(tbAutoPlayLastCard, m_bAutoPlayLastCard);
	m_app.SetValue(tbAutoJumpCursor, m_bAutoJumpCursor);
	m_app.SetValue(tbInsertBiddingPause, m_bInsertBiddingPause);
	m_app.SetValue(tnBiddingPauseLength, m_nBiddingPause);
	m_app.SetValue(tbInsertPlayPause, m_bInsertPlayPause);
	m_app.SetValue(tnPlayPauseLength, m_nPlayPause);
	m_app.SetValue(tbEnableSpokenBids, m_bEnableSpokenBids);
}


//
void CGameOptsInterfacePage::OnEnablePause() 
{
	UpdateData(TRUE);
	EnableOrDisablePauseSliders();
}


//
void CGameOptsInterfacePage::EnableOrDisablePauseSliders() 
{
	if (m_bInsertBiddingPause)
		GetDlgItem(IDC_BID_PAUSE)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_BID_PAUSE)->EnableWindow(FALSE);
	if (m_bInsertPlayPause)
		GetDlgItem(IDC_PLAY_PAUSE)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_PLAY_PAUSE)->EnableWindow(FALSE);
}
