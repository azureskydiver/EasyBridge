//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// ProgConfigIntroPage.cpp : implementation file
//

#include "stdafx.h"
#include "ProgConfigIntroPage.h"
#include "ProgramConfigWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgConfigIntroPage property page

IMPLEMENT_DYNCREATE(CProgConfigIntroPage, CPropertyPage)

CProgConfigIntroPage::CProgConfigIntroPage(CProgramConfigWizard* pSheet) : 
		CPropertyPage(CProgConfigIntroPage::IDD),
		m_sheet(*pSheet),
		m_data(pSheet->GetData())
{
	//{{AFX_DATA_INIT(CProgConfigIntroPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CProgConfigIntroPage::~CProgConfigIntroPage()
{
}

void CProgConfigIntroPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgConfigIntroPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgConfigIntroPage, CPropertyPage)
	//{{AFX_MSG_MAP(CProgConfigIntroPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgConfigIntroPage message handlers


//
BOOL CProgConfigIntroPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
BOOL CProgConfigIntroPage::OnSetActive() 
{
	m_sheet.SetWizardButtons( PSWIZB_NEXT );
	//
	return CPropertyPage::OnSetActive();
}


//
LRESULT CProgConfigIntroPage::OnWizardBack() 
{
	/// this shouldn't get called
	return -1;
//	return CPropertyPage::OnWizardBack();
}


//
LRESULT CProgConfigIntroPage::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnWizardNext();
}
