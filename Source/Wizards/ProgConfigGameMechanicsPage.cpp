//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// ProgConfigGameMechanicsPage.cpp : implementation file
//

#include "stdafx.h"
#include "ProgramConfigWizard.h"
#include "ProgConfigGameMechanicsPage.h"
#include "ProgConfigWizardData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgConfigGameMechanicsPage property page

IMPLEMENT_DYNCREATE(CProgConfigGameMechanicsPage, CPropertyPage)

CProgConfigGameMechanicsPage::CProgConfigGameMechanicsPage(CProgramConfigWizard* pSheet) : 
		CPropertyPage(CProgConfigGameMechanicsPage::IDD),
		m_sheet(*pSheet),
		m_data(pSheet->GetData())
{
	//{{AFX_DATA_INIT(CProgConfigGameMechanicsPage)
	m_bScoreDuplicate = FALSE;
	m_bIgnoreHonorsBonuses = FALSE;
	//}}AFX_DATA_INIT
}

CProgConfigGameMechanicsPage::~CProgConfigGameMechanicsPage()
{
}

void CProgConfigGameMechanicsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgConfigGameMechanicsPage)
	DDX_Check(pDX, IDC_SCORE_DUPLICATE, m_bScoreDuplicate);
	DDX_Check(pDX, IDC_IGNORE_HONORS_BONUS, m_bIgnoreHonorsBonuses);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgConfigGameMechanicsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CProgConfigGameMechanicsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgConfigGameMechanicsPage message handlers



//
BOOL CProgConfigGameMechanicsPage::OnInitDialog() 
{
	// load data
	m_bScoreDuplicate = m_data.m_bScoreDuplicate;
	m_bIgnoreHonorsBonuses = m_data.m_bIgnoreHonorsBonuses;
	//
	CPropertyPage::OnInitDialog();
	
	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
BOOL CProgConfigGameMechanicsPage::OnSetActive() 
{
	m_sheet.SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	return CPropertyPage::OnSetActive();
}


//
LRESULT CProgConfigGameMechanicsPage::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnWizardBack();
}


//
LRESULT CProgConfigGameMechanicsPage::OnWizardNext() 
{
	// save data
	UpdateData(TRUE);
	m_data.m_bScoreDuplicate = m_bScoreDuplicate;
	m_data.m_bIgnoreHonorsBonuses = m_bIgnoreHonorsBonuses;
	//	
	return CPropertyPage::OnWizardNext();
}
