//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// ProgConfigBiddingPage.cpp : implementation file
//

#include "stdafx.h"
#include "ProgConfigBiddingPage.h"
#include "ProgramConfigWizard.h"
#include "ProgConfigWizardData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgConfigBiddingPage property page

IMPLEMENT_DYNCREATE(CProgConfigBiddingPage, CPropertyPage)

CProgConfigBiddingPage::CProgConfigBiddingPage(CProgramConfigWizard* pSheet) : 
		CPropertyPage(CProgConfigBiddingPage::IDD),
		m_sheet(*pSheet),
		m_data(pSheet->GetData())
{
	//{{AFX_DATA_INIT(CProgConfigBiddingPage)
	m_nBiddingStyle = -1;
	//}}AFX_DATA_INIT
}

CProgConfigBiddingPage::~CProgConfigBiddingPage()
{
}

void CProgConfigBiddingPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgConfigBiddingPage)
	DDX_Radio(pDX, IDC_BIDDING1, m_nBiddingStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgConfigBiddingPage, CPropertyPage)
	//{{AFX_MSG_MAP(CProgConfigBiddingPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgConfigBiddingPage message handlers


//
BOOL CProgConfigBiddingPage::OnInitDialog() 
{
	// load data
	m_nBiddingStyle = m_data.m_nBiddingPreference;

	//
	CPropertyPage::OnInitDialog();
	
	// 
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
BOOL CProgConfigBiddingPage::OnSetActive() 
{
	m_sheet.SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	return CPropertyPage::OnSetActive();
}


//
LRESULT CProgConfigBiddingPage::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnWizardBack();
}


//
LRESULT CProgConfigBiddingPage::OnWizardNext() 
{
	// save data
	UpdateData(TRUE);
	m_data.m_nBiddingPreference = m_nBiddingStyle;
	//	
	return CPropertyPage::OnWizardNext();
}


