//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// ProgConfigSuitsDisplayPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ProgConfigSuitsDisplayPage.h"
#include "ProgramConfigWizard.h"
#include "ProgConfigWizardData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgConfigSuitsDisplayPage property page

IMPLEMENT_DYNCREATE(CProgConfigSuitsDisplayPage, CPropertyPage)

CProgConfigSuitsDisplayPage::CProgConfigSuitsDisplayPage(CProgramConfigWizard* pSheet) : 
		CPropertyPage(CProgConfigSuitsDisplayPage::IDD),
		m_sheet(*pSheet),
		m_data(pSheet->GetData())
{
	//{{AFX_DATA_INIT(CProgConfigSuitsDisplayPage)
	m_nSuitDisplayOrder = -1;
	m_bDummyTrumpsOnLeft = FALSE;
	//}}AFX_DATA_INIT
}

CProgConfigSuitsDisplayPage::~CProgConfigSuitsDisplayPage()
{
}

void CProgConfigSuitsDisplayPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgConfigSuitsDisplayPage)
	DDX_Radio(pDX, IDC_SUIT_ORDER1, m_nSuitDisplayOrder);
	DDX_Check(pDX, IDC_DUMMY_TRUMPS_ON_LEFT, m_bDummyTrumpsOnLeft);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgConfigSuitsDisplayPage, CPropertyPage)
	//{{AFX_MSG_MAP(CProgConfigSuitsDisplayPage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgConfigSuitsDisplayPage message handlers



//
BOOL CProgConfigSuitsDisplayPage::OnInitDialog() 
{
	// load data
	m_nSuitDisplayOrder = m_data.m_nSuitDisplayOrder;
	m_bDummyTrumpsOnLeft = m_data.m_bShowDummyTrumpsOnLeft;
	//
	CPropertyPage::OnInitDialog();
	
	// 
	for(int i=0;i<5;i++)
	{
		m_radioBitmaps[i].LoadBitmap(IDB_SUIT_ORDER_1+i);
		((CButton*)GetDlgItem(IDC_SUIT_ORDER1+i))->SetBitmap(m_radioBitmaps[i]);
	}

	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CProgConfigSuitsDisplayPage::OnDestroy() 
{
	// empty out the bitmaps
	for(int i=0;i<5;i++)
		m_radioBitmaps[i].DeleteObject();

	//
	CPropertyPage::OnDestroy();
}


//
BOOL CProgConfigSuitsDisplayPage::OnSetActive() 
{
	m_sheet.SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	return CPropertyPage::OnSetActive();
}


//
LRESULT CProgConfigSuitsDisplayPage::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnWizardBack();
}


//
LRESULT CProgConfigSuitsDisplayPage::OnWizardNext() 
{
	// save data
	UpdateData(TRUE);
	m_data.m_nSuitDisplayOrder = m_nSuitDisplayOrder;
	m_data.m_bShowDummyTrumpsOnLeft = m_bDummyTrumpsOnLeft;
	//
	return CPropertyPage::OnWizardNext();
}
