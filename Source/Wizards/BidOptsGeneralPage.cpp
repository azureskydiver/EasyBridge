//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// BidOptsGeneralPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ObjectWithProperties.h"
#include "BidOptsGeneralPage.h"
#include "ConvCodes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBidOptsGeneralPage property page

IMPLEMENT_DYNCREATE(CBidOptsGeneralPage, CPropertyPage)

CBidOptsGeneralPage::CBidOptsGeneralPage(CObjectWithProperties* pApp, CObjectWithProperties* pConventionSet) : 
		CPropertyPage(CBidOptsGeneralPage::IDD),
		m_app(*pApp), m_conventionSet(*pConventionSet)
{
	//{{AFX_DATA_INIT(CBidOptsGeneralPage)
	m_bBlackwood = FALSE;
	m_bCueBids = FALSE;
	m_nMajorsMode = -1;
	m_bStayman = FALSE;
	m_bTakeoutDoubles = FALSE;
	m_bSplinterBids = FALSE;
	m_bJacobyTransfers = FALSE;
	m_b4SuitTransfers = FALSE;
	m_bGerber = FALSE;
	//}}AFX_DATA_INIT
	m_bBlackwood = m_conventionSet.GetValue(tidBlackwood);
	m_bCueBids = m_conventionSet.GetValue(tidCueBids);
	m_bGerber = m_conventionSet.GetValue(tidGerber);
	m_bStayman = m_conventionSet.GetValue(tidStayman);
	m_bJacobyTransfers = m_conventionSet.GetValue(tidJacobyTransfers);
	m_b4SuitTransfers = m_conventionSet.GetValue(tb4SuitTransfers);
	m_bLimitRaises = m_conventionSet.GetValue(tidLimitRaises);
	m_bSplinterBids = m_conventionSet.GetValue(tidSplinterBids);
	m_bTakeoutDoubles = m_conventionSet.GetValue(tidTakeoutDoubles);
	m_bNegativeDoubles = m_conventionSet.GetValue(tidNegativeDoubles);
	m_nMajorsMode = m_conventionSet.GetValue(tid5CardMajors);
}

CBidOptsGeneralPage::~CBidOptsGeneralPage()
{
}

void CBidOptsGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBidOptsGeneralPage)
	DDX_Check(pDX, IDC_BLACKWOOD, m_bBlackwood);
	DDX_Check(pDX, IDC_CUE_BIDS, m_bCueBids);
	DDX_Radio(pDX, IDC_RADIO_MAJORS, m_nMajorsMode);
	DDX_Check(pDX, IDC_STAYMAN, m_bStayman);
	DDX_Check(pDX, IDC_LIMIT_RAISES, m_bLimitRaises);
	DDX_Check(pDX, IDC_NEGATIVE_DOUBLES, m_bNegativeDoubles);
	DDX_Check(pDX, IDC_TAKEOUT_DOUBLES, m_bTakeoutDoubles);
	DDX_Check(pDX, IDC_SPLINTER_BIDS, m_bSplinterBids);
	DDX_Check(pDX, IDC_JACOBY, m_bJacobyTransfers);
	DDX_Check(pDX, IDC_4SUIT_TRANSFERS, m_b4SuitTransfers);
	DDX_Check(pDX, IDC_GERBER, m_bGerber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBidOptsGeneralPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBidOptsGeneralPage)
	ON_BN_CLICKED(IDC_JACOBY, OnClickJacoby)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBidOptsGeneralPage message handlers


//
BOOL CBidOptsGeneralPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// set staus of 4-suit transfers checkbox
	OnClickJacoby() ;
	//	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CBidOptsGeneralPage::Update()
{
	// store results
	m_conventionSet.SetValue(tidBlackwood, m_bBlackwood);
	m_conventionSet.SetValue(tidCueBids, m_bCueBids);
	m_conventionSet.SetValue(tidGerber, m_bGerber);
	m_conventionSet.SetValue(tidStayman, m_bStayman);
	m_conventionSet.SetValue(tidJacobyTransfers, m_bJacobyTransfers);
	m_conventionSet.SetValue(tb4SuitTransfers, m_b4SuitTransfers);
	m_conventionSet.SetValue(tidLimitRaises, m_bLimitRaises);
	m_conventionSet.SetValue(tidSplinterBids, m_bSplinterBids);
	m_conventionSet.SetValue(tidTakeoutDoubles, m_bTakeoutDoubles);
	m_conventionSet.SetValue(tidNegativeDoubles, m_bNegativeDoubles);
	m_conventionSet.SetValue(tid5CardMajors, (m_nMajorsMode == 1)? TRUE: FALSE);
}


//
void CBidOptsGeneralPage::OnClickJacoby() 
{
	UpdateData();
	if (m_bJacobyTransfers)
		GetDlgItem(IDC_4SUIT_TRANSFERS)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_4SUIT_TRANSFERS)->EnableWindow(FALSE);
}

