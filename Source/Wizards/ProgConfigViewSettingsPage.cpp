//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// ProgConfigViewSettingsPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ProgConfigViewSettingsPage.h"
#include "ProgramConfigWizard.h"
#include "ProgConfigWizardData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgConfigViewSettingsPage property page

IMPLEMENT_DYNCREATE(CProgConfigViewSettingsPage, CPropertyPage)

CProgConfigViewSettingsPage::CProgConfigViewSettingsPage(CProgramConfigWizard* pSheet) : 
		CPropertyPage(CProgConfigViewSettingsPage::IDD),
		m_sheet(*pSheet),
		m_data(pSheet->GetData())
{
	//{{AFX_DATA_INIT(CProgConfigViewSettingsPage)
	m_bShowBiddingHistory = FALSE;
	m_bShowPlayHistory = FALSE;
	m_bUseSuitSymbols = FALSE;
	//}}AFX_DATA_INIT
//	m_bDisableSplashScreen = FALSE;
//	m_bDisableBackgroundBitmap = FALSE;
}

CProgConfigViewSettingsPage::~CProgConfigViewSettingsPage()
{
}

void CProgConfigViewSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgConfigViewSettingsPage)
	DDX_Check(pDX, IDC_SHOW_BIDDING_HISTORY, m_bShowBiddingHistory);
	DDX_Check(pDX, IDC_SHOW_PLAY_HISTORY, m_bShowPlayHistory);
	DDX_Check(pDX, IDC_USE_SUIT_SYMBOLS, m_bUseSuitSymbols);
	//}}AFX_DATA_MAP
//	DDX_Check(pDX, IDC_DISABLE_SPLASH_SCREEN, m_bDisableSplashScreen);
//	DDX_Check(pDX, IDC_DISABLE_BITMAP, m_bDisableBackgroundBitmap);
}


BEGIN_MESSAGE_MAP(CProgConfigViewSettingsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CProgConfigViewSettingsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgConfigViewSettingsPage message handlers


//
BOOL CProgConfigViewSettingsPage::OnInitDialog() 
{
	// load data
	m_bShowBiddingHistory = m_data.m_bShowBidHistory;
	m_bShowPlayHistory = m_data.m_bShowPlayHistory;
	m_bUseSuitSymbols = m_data.m_bUseSuitSymbols;
//	m_bDisableSplashScreen = m_data.m_bHideBanner;
//	m_bDisableBackgroundBitmap = m_data.m_bDisableBitmap;
	//
	CPropertyPage::OnInitDialog();

	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
BOOL CProgConfigViewSettingsPage::OnSetActive() 
{
	m_sheet.SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	return CPropertyPage::OnSetActive();
}


//
LRESULT CProgConfigViewSettingsPage::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnWizardBack();
}


//
LRESULT CProgConfigViewSettingsPage::OnWizardNext() 
{
	// save data
	UpdateData(TRUE);
	m_data.m_bShowBidHistory = m_bShowBiddingHistory;
	m_data.m_bShowPlayHistory =  m_bShowPlayHistory;
	m_data.m_bUseSuitSymbols = m_bUseSuitSymbols;
//	m_data.m_bHideBanner = m_bDisableSplashScreen;
//	m_data.m_bDisableBitmap = m_bDisableBackgroundBitmap;
	//	
	return CPropertyPage::OnWizardNext();
}
