//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// BidOptsParamsPage.cpp : implementation file
//

#include "stdafx.h"
#include "ObjectWithProperties.h"
#include "BidOptsParamsPage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBidOptsParamsPage property page

IMPLEMENT_DYNCREATE(CBidOptsParamsPage, CPropertyPage)

CBidOptsParamsPage::CBidOptsParamsPage(CObjectWithProperties* pApp, CObjectWithProperties* pConventionSet) : 
		CPropertyPage(CBidOptsParamsPage::IDD),
		m_app(*pApp), m_conventionSet(*pConventionSet)
{
	//{{AFX_DATA_INIT(CBidOptsParamsPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CBidOptsParamsPage::~CBidOptsParamsPage()
{
}

void CBidOptsParamsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBidOptsParamsPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBidOptsParamsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBidOptsParamsPage)
	ON_CBN_SELCHANGE(IDC_BIDDING_CONFIGURATION, OnSelchangeConfig)
	ON_BN_CLICKED(IDC_SAVE_CONFIG, OnSaveConfig)
	ON_BN_CLICKED(IDC_REMOVE_CONFIG, OnRemoveConfig)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBidOptsParamsPage message handlers


//
void CBidOptsParamsPage::OnSelchangeConfig() 
{
	// TODO: Add your control notification handler code here
	
}


//
void CBidOptsParamsPage::OnSaveConfig() 
{
	// TODO: Add your control notification handler code here
	
}


//
void CBidOptsParamsPage::OnRemoveConfig() 
{
	// TODO: Add your control notification handler code here
	
}



//
void CBidOptsParamsPage::Update()
{
	// store results
}

