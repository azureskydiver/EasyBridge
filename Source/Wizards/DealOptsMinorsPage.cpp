//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DealOptsMinorsPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ObjectWithProperties.h"
#include "DealOptsMinorsPage.h"
#include "progopts.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define ACE	14

/////////////////////////////////////////////////////////////////////////////
// CDealOptsMinorsPage property page

IMPLEMENT_DYNCREATE(CDealOptsMinorsPage, CPropertyPage)

CDealOptsMinorsPage::CDealOptsMinorsPage(CObjectWithProperties* pApp) : 
		CPropertyPage(CDealOptsMinorsPage::IDD),
		m_app(*pApp)
{
	//{{AFX_DATA_INIT(CDealOptsMinorsPage)
	m_nMinCardsInMinor = -1;
	m_nMinSuitTopper = -1;
	m_nMinTeamDistribution = -1;
	//}}AFX_DATA_INIT
	m_nMinCardsInMinor = m_app.GetValue(tnMinCardsInMinor) - 7;
	m_nMinSuitTopper = ACE - m_app.GetValue(tnMinTopMinorCard);
	m_nMinTeamDistribution = m_app.GetValue(tnMinSuitDistributions,0);
}

CDealOptsMinorsPage::~CDealOptsMinorsPage()
{
}

void CDealOptsMinorsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDealOptsMinorsPage)
	DDX_Radio(pDX, IDC_RADIO1, m_nMinCardsInMinor);
	DDX_Radio(pDX, IDC_RADIO4, m_nMinSuitTopper);
	DDX_Radio(pDX, IDC_RADIO7, m_nMinTeamDistribution);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDealOptsMinorsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDealOptsMinorsPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDealOptsMinorsPage message handlers


//
void CDealOptsMinorsPage::Update()
{
	// store results
	m_app.SetValue(tnMinCardsInMinor, m_nMinCardsInMinor + 7);
	m_app.SetValue(tnMinTopMinorCard, ACE - m_nMinSuitTopper);
	m_app.SetValue(tnMinSuitDistributions, m_nMinTeamDistribution, 0);
}

