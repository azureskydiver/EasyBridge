//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// BidOptsNoTrumpsPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ObjectWithProperties.h"
#include "BidOptsNoTrumpsPage.h"
#include "ConvCodes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBidOptsNoTrumpsPage property page

IMPLEMENT_DYNCREATE(CBidOptsNoTrumpsPage, CPropertyPage)

CBidOptsNoTrumpsPage::CBidOptsNoTrumpsPage(CObjectWithProperties* pApp, CObjectWithProperties* pConventionSet) : 
		CPropertyPage(CBidOptsNoTrumpsPage::IDD),
		m_app(*pApp), m_conventionSet(*pConventionSet)
{
	//{{AFX_DATA_INIT(CBidOptsNoTrumpsPage)
	m_n1NTRange = -1;
	m_n2NTRange = -1;
	m_n3NTRange = -1;
	m_bGambling3NT = FALSE;
	//}}AFX_DATA_INIT
	m_n1NTRange = m_conventionSet.GetValue(tn1NTRange);
	m_n2NTRange = m_conventionSet.GetValue(tn2NTRange);
	m_n3NTRange = m_conventionSet.GetValue(tn3NTRange);
	m_bGambling3NT = m_conventionSet.GetValue(tidGambling3NT);
	m_nGambling3NTVersion = m_conventionSet.GetValue(tnGambling3NTVersion);
}

CBidOptsNoTrumpsPage::~CBidOptsNoTrumpsPage()
{
}

void CBidOptsNoTrumpsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBidOptsNoTrumpsPage)
	DDX_Radio(pDX, IDC_RADIO_1NT_RANGE, m_n1NTRange);
	DDX_Radio(pDX, IDC_RADIO_2NT_RANGE, m_n2NTRange);
	DDX_Radio(pDX, IDC_RADIO_3NT_RANGE, m_n3NTRange);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBidOptsNoTrumpsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBidOptsNoTrumpsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBidOptsNoTrumpsPage message handlers


//
BOOL CBidOptsNoTrumpsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// done
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CBidOptsNoTrumpsPage::Update()
{
	// set values
	if (m_n3NTRange == 0)
	{
		m_bGambling3NT = FALSE;
	}
	else
	{
		m_bGambling3NT = TRUE;
		m_nGambling3NTVersion = m_n3NTRange - 1;
	}

	// and store results
	m_conventionSet.SetValue(tn1NTRange, m_n1NTRange);
	m_conventionSet.SetValue(tn2NTRange, m_n2NTRange);
	m_conventionSet.SetValue(tn3NTRange, m_n3NTRange);
	m_conventionSet.SetValue(tidGambling3NT, m_bGambling3NT);
	m_conventionSet.SetValue(tnGambling3NTVersion, m_nGambling3NTVersion);
}



