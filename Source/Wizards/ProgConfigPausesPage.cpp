//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// ProgConfigPausesPage.cpp : implementation file
//

#include "stdafx.h"
#include "ProgConfigPausesPage.h"
#include "ProgramConfigWizard.h"
#include "ProgConfigWizardData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgConfigPausesPage property page

IMPLEMENT_DYNCREATE(CProgConfigPausesPage, CPropertyPage)

CProgConfigPausesPage::CProgConfigPausesPage(CProgramConfigWizard* pSheet) : 
		CPropertyPage(CProgConfigPausesPage::IDD),
		m_sheet(*pSheet),
		m_data(pSheet->GetData())
{
	//{{AFX_DATA_INIT(CProgConfigPausesPage)
	m_nBiddingPauseSetting = -1;
	m_nPlayPauseSetting = -1;
	//}}AFX_DATA_INIT
}

CProgConfigPausesPage::~CProgConfigPausesPage()
{
}

void CProgConfigPausesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgConfigPausesPage)
	DDX_Radio(pDX, IDC_BID_PAUSE_0, m_nBiddingPauseSetting);
	DDX_Radio(pDX, IDC_PLAY_PAUSE_0, m_nPlayPauseSetting);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgConfigPausesPage, CPropertyPage)
	//{{AFX_MSG_MAP(CProgConfigPausesPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgConfigPausesPage message handlers



//
BOOL CProgConfigPausesPage::OnInitDialog() 
{
	// load data
	m_nBiddingPauseSetting = m_data.m_nBiddingPauseSetting;
	m_nPlayPauseSetting = m_data.m_nPlayPauseSetting;
	//
	CPropertyPage::OnInitDialog();

	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
BOOL CProgConfigPausesPage::OnSetActive() 
{
	m_sheet.SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	return CPropertyPage::OnSetActive();
}


//
LRESULT CProgConfigPausesPage::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnWizardBack();
}


//
LRESULT CProgConfigPausesPage::OnWizardNext() 
{
	// save data
	UpdateData(TRUE);
	m_data.m_nBiddingPauseSetting = m_nBiddingPauseSetting;
	m_data.m_nPlayPauseSetting = m_nPlayPauseSetting;
	//
	return CPropertyPage::OnWizardNext();
}
