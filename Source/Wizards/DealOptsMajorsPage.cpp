//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DealOptsMajorsPage.cpp : implementation file
//

#include "stdafx.h"
#include "ObjectWithProperties.h"
#include "DealOptsMajorsPage.h"
#include "progopts.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define ACE	14

/////////////////////////////////////////////////////////////////////////////
// CDealOptsMajorsPage property page

IMPLEMENT_DYNCREATE(CDealOptsMajorsPage, CPropertyPage)

CDealOptsMajorsPage::CDealOptsMajorsPage(CObjectWithProperties* pApp) : 
		CPropertyPage(CDealOptsMajorsPage::IDD),
		m_app(*pApp)
{
	//{{AFX_DATA_INIT(CDealOptsMajorsPage)
	m_nMinCardsInSuit = -1;
	m_nMinSuitTopper = -1;
	m_nMinTeamDistribution = -1;
	//}}AFX_DATA_INIT
	m_nMinCardsInSuit = m_app.GetValue(tnMinCardsInMajor) - 7;
	m_nMinSuitTopper = ACE - m_app.GetValue(tnMinTopMajorCard);
	m_nMinTeamDistribution = m_app.GetValue(tnMinSuitDistributions,1);
}

CDealOptsMajorsPage::~CDealOptsMajorsPage()
{
}

void CDealOptsMajorsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDealOptsMajorsPage)
	DDX_Radio(pDX, IDC_RADIO1, m_nMinCardsInSuit);
	DDX_Radio(pDX, IDC_RADIO4, m_nMinSuitTopper);
	DDX_Radio(pDX, IDC_RADIO7, m_nMinTeamDistribution);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDealOptsMajorsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDealOptsMajorsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDealOptsMajorsPage message handlers



//
void CDealOptsMajorsPage::Update()
{
	// store results
	m_app.SetValue(tnMinCardsInMajor, m_nMinCardsInSuit + 7);
	m_app.SetValue(tnMinTopMajorCard, ACE - m_nMinSuitTopper);
	m_app.SetValue(tnMinSuitDistributions, m_nMinTeamDistribution, 1);
}
