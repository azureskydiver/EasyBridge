//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DispOptsDialogsPage.cpp : implementation file
//

#include "stdafx.h"
#include "ObjectWithProperties.h"
#include "DispOptsDialogsPage.h"
#include "progopts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDispOptsDialogsPage property page

IMPLEMENT_DYNCREATE(CDispOptsDialogsPage, CPropertyPage)

CDispOptsDialogsPage::CDispOptsDialogsPage(CObjectWithProperties* pApp, CObjectWithProperties* pFrame, CObjectWithProperties* pView) : 
		CPropertyPage(CDispOptsDialogsPage::IDD),
		m_app(*pApp), m_frame(*pFrame), m_view(*pView)
{
	//{{AFX_DATA_INIT(CDispOptsDialogsPage)
	m_bAutoShowBidHistory = FALSE;
	m_bAutoShowPlayHistory = FALSE;
	m_bAutoHideBidHistory = FALSE;
	m_bAutoHidePlayHistory = FALSE;
	m_bShowLayoutOnEdit = FALSE;
	m_bAutoShowNNetOutputWhenTraining = FALSE;
	//}}AFX_DATA_INIT
	m_bAutoShowBidHistory = m_app.GetValue(tbAutoShowBidHistory);
	m_bAutoShowPlayHistory = m_app.GetValue(tbAutoShowPlayHistory);
	m_bAutoHideBidHistory = m_app.GetValue(tbAutoHideBidHistory);
	m_bAutoHidePlayHistory = m_app.GetValue(tbAutoHidePlayHistory);
	m_bAutoShowNNetOutputWhenTraining = m_app.GetValue(tbAutoShowNNetOutputWhenTraining);
	m_bShowLayoutOnEdit = m_app.GetValue(tbShowLayoutOnEdit);
}

CDispOptsDialogsPage::~CDispOptsDialogsPage()
{
}

void CDispOptsDialogsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDispOptsDialogsPage)
	DDX_Check(pDX, IDC_AUTO_SHOW_BID_HISTORY, m_bAutoShowBidHistory);
	DDX_Check(pDX, IDC_AUTO_SHOW_PLAY_HISTORY, m_bAutoShowPlayHistory);
	DDX_Check(pDX, IDC_AUTO_HIDE_BID_HISTORY, m_bAutoHideBidHistory);
	DDX_Check(pDX, IDC_AUTO_HIDE_PLAY_HISTORY, m_bAutoHidePlayHistory);
	DDX_Check(pDX, IDC_SHOW_LAYOUT_ON_EDIT, m_bShowLayoutOnEdit);
	DDX_Check(pDX, IDC_AUTOSHOW_NNET, m_bAutoShowNNetOutputWhenTraining);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDispOptsDialogsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDispOptsDialogsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDispOptsDialogsPage message handlers
/////////////////////////////////////////////////////////////////////////////



//
BOOL CDispOptsDialogsPage::Update()
{
	// store results
	BOOL bModified = FALSE;
/*
	if ( (m_bAutoShowBidHistory != m_app.GetValue(tbAutoShowBidHistory)) ||
		 (m_bAutoShowPlayHistory != m_app.GetValue(tbAutoShowPlayHistory)) ||
		 (m_bAutoHideBidHistory != m_app.GetValue(tbAutoHideBidHistory)) ||
		 (m_bAutoHidePlayHistory != m_app.GetValue(tbAutoHidePlayHistory)) ||
		 (m_bShowLayoutOnEdit != m_app.GetValue(tbShowLayoutOnEdit) )
		 bModified = TRUE;
*/
	//
	m_app.SetValue(tbAutoShowBidHistory, m_bAutoShowBidHistory);
	m_app.SetValue(tbAutoShowPlayHistory, m_bAutoShowPlayHistory);
	m_app.SetValue(tbAutoHideBidHistory, m_bAutoHideBidHistory);
	m_app.SetValue(tbAutoHidePlayHistory, m_bAutoHidePlayHistory);
	m_app.SetValue(tbAutoShowNNetOutputWhenTraining, m_bAutoShowNNetOutputWhenTraining);
	m_app.SetValue(tbShowLayoutOnEdit, m_bShowLayoutOnEdit);
	//
	return bModified;
}




