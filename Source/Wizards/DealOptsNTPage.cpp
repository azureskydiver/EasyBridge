//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DealOptsNoTrumpPage.cpp : implementation file
//

#include "stdafx.h"
#include "ObjectWithProperties.h"
#include "DealOptsNTPage.h"
#include "progopts.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDealOptsNoTrumpPage property page

IMPLEMENT_DYNCREATE(CDealOptsNoTrumpPage, CPropertyPage)

CDealOptsNoTrumpPage::CDealOptsNoTrumpPage(CObjectWithProperties* pApp) : 
		CPropertyPage(CDealOptsNoTrumpPage::IDD),
		m_app(*pApp)
{
	//{{AFX_DATA_INIT(CDealOptsNoTrumpPage)
	m_bNeedTwoBalancedHands = FALSE;
	//}}AFX_DATA_INIT
//	m_nMaxImbalanceForNT = m_app.m_nMaxImbalanceForNT;
	m_bNeedTwoBalancedHands = m_app.GetValue(tbNeedTwoBalancedTrumpHands);
}

CDealOptsNoTrumpPage::~CDealOptsNoTrumpPage()
{
}

void CDealOptsNoTrumpPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDealOptsNoTrumpPage)
	DDX_Check(pDX, IDC_CHECK1, m_bNeedTwoBalancedHands);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDealOptsNoTrumpPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDealOptsNoTrumpPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDealOptsNoTrumpPage message handlers

void CDealOptsNoTrumpPage::Update()
{
	// store results
//	m_app.m_nMaxImbalanceForNT = m_nMaxImbalanceForNT;
	m_app.SetValue(tbNeedTwoBalancedTrumpHands, m_bNeedTwoBalancedHands);
}
