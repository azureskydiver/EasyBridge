//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// ProgConfigFinishPage.cpp : implementation file
//

#include "stdafx.h"
#include "ProgConfigFinishPage.h"
#include "ProgramConfigWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgConfigFinishPage property page

IMPLEMENT_DYNCREATE(CProgConfigFinishPage, CPropertyPage)

CProgConfigFinishPage::CProgConfigFinishPage(CProgramConfigWizard* pSheet) : 
		CPropertyPage(CProgConfigFinishPage::IDD),
		m_sheet(*pSheet),
		m_data(pSheet->GetData())
{
	//{{AFX_DATA_INIT(CProgConfigFinishPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CProgConfigFinishPage::~CProgConfigFinishPage()
{
}

void CProgConfigFinishPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgConfigFinishPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgConfigFinishPage, CPropertyPage)
	//{{AFX_MSG_MAP(CProgConfigFinishPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgConfigFinishPage message handlers


//
BOOL CProgConfigFinishPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
BOOL CProgConfigFinishPage::OnSetActive() 
{
	m_sheet.SetWizardButtons( PSWIZB_BACK | PSWIZB_FINISH );
	return CPropertyPage::OnSetActive();
}


//
LRESULT CProgConfigFinishPage::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnWizardBack();
}


//
LRESULT CProgConfigFinishPage::OnWizardNext() 
{
	// this shouldn't get called!
	return -1;
//	return CPropertyPage::OnWizardNext();
}


//
BOOL CProgConfigFinishPage::OnWizardFinish() 
{
	// set exit code
	m_sheet.EndDialog(IDOK);
	//
	return CPropertyPage::OnWizardFinish();
}
