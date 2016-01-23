//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// ProgConfigHelpLevelPage.cpp : implementation file
//

#include "stdafx.h"
#include "ProgConfigHelpLevelPage.h"
#include "ProgramConfigWizard.h"
#include "ProgConfigWizardData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgConfigHelpLevelPage property page

IMPLEMENT_DYNCREATE(CProgConfigHelpLevelPage, CPropertyPage)

CProgConfigHelpLevelPage::CProgConfigHelpLevelPage(CProgramConfigWizard* pSheet) : 
		CPropertyPage(CProgConfigHelpLevelPage::IDD),
		m_sheet(*pSheet),
		m_data(pSheet->GetData())
{
	//{{AFX_DATA_INIT(CProgConfigHelpLevelPage)
	m_nHelpLevel = -1;
	m_bShowAnalyses = FALSE;
	//}}AFX_DATA_INIT
}

CProgConfigHelpLevelPage::~CProgConfigHelpLevelPage()
{
}

void CProgConfigHelpLevelPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgConfigHelpLevelPage)
	DDX_Radio(pDX, IDC_HELP_LEVEL, m_nHelpLevel);
	DDX_Check(pDX, IDC_SHOW_ANALYSIS, m_bShowAnalyses);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgConfigHelpLevelPage, CPropertyPage)
	//{{AFX_MSG_MAP(CProgConfigHelpLevelPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgConfigHelpLevelPage message handlers



//
BOOL CProgConfigHelpLevelPage::OnInitDialog() 
{
	// load data
	m_nHelpLevel = m_data.m_nHelpLevel;
	m_bShowAnalyses = m_data.m_bShowAnalysis;

	//
	CPropertyPage::OnInitDialog();
	
	// 
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
BOOL CProgConfigHelpLevelPage::OnSetActive() 
{
	m_sheet.SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	return CPropertyPage::OnSetActive();
}


//
LRESULT CProgConfigHelpLevelPage::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnWizardBack();
}


//
LRESULT CProgConfigHelpLevelPage::OnWizardNext() 
{
	// save data
	UpdateData(TRUE);
	m_data.m_nHelpLevel = m_nHelpLevel;
	m_data.m_bShowAnalysis = m_bShowAnalyses;
	//	
	return CPropertyPage::OnWizardNext();
}
