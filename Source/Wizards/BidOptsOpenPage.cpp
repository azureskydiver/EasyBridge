//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// BidOptsOpenPage.cpp : implementation file
//

#include "stdafx.h"
#include "ObjectWithProperties.h"
#include "BidOptsOpenPage.h"
#include "Bidparams.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBidOptsOpenPage property page

IMPLEMENT_DYNCREATE(CBidOptsOpenPage, CPropertyPage)

CBidOptsOpenPage::CBidOptsOpenPage(CObjectWithProperties* pApp, CObjectWithProperties* pConventionSet) : 
		CPropertyPage(CBidOptsOpenPage::IDD),
		m_app(*pApp), m_conventionSet(*pConventionSet)
{
	//{{AFX_DATA_INIT(CBidOptsOpenPage)
	m_b13TPsLs = FALSE;
	m_b14TPsGs = FALSE;
	m_b14TPsLs = FALSE;
	m_b11HCPsRbsLm = FALSE;
	m_b11HCPs6Cs = FALSE;
	//}}AFX_DATA_INIT
	m_nAllowableOpenings = m_conventionSet.GetValue(tnAllowable1Openings);
	m_b11HCPsRbsLm = (m_nAllowableOpenings & OB_11_HCPS_RBS_LM)? TRUE : FALSE;
	m_b11HCPs6Cs = (m_nAllowableOpenings & OB_11_HCPS_6CS)? TRUE : FALSE;
	m_b14TPsGs = (m_nAllowableOpenings & OB_14_TCPS_GS)? TRUE : FALSE;
	m_b14TPsLs = (m_nAllowableOpenings & OB_14_TCPS_LS)? TRUE : FALSE;
	m_b13TPsLs = (m_nAllowableOpenings & OB_13_TCPS_LS)? TRUE : FALSE;
}

CBidOptsOpenPage::~CBidOptsOpenPage()
{
}

void CBidOptsOpenPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBidOptsOpenPage)
	DDX_Check(pDX, IDC_13_TPS_LS, m_b13TPsLs);
	DDX_Check(pDX, IDC_14_TPS_GS, m_b14TPsGs);
	DDX_Check(pDX, IDC_14_TPS_LS, m_b14TPsLs);
	DDX_Check(pDX, IDC_10_HCPS_RBS_LM, m_b11HCPsRbsLm);
	DDX_Check(pDX, IDC_10_HCPS_6CS, m_b11HCPs6Cs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBidOptsOpenPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBidOptsOpenPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBidOptsOpenPage message handlers



//
void CBidOptsOpenPage::Update()
{
	// store results
	if (m_b11HCPsRbsLm)
		m_nAllowableOpenings |= OB_11_HCPS_RBS_LM;
	else
		m_nAllowableOpenings &= ~OB_11_HCPS_RBS_LM;

	if (m_b11HCPs6Cs)
		m_nAllowableOpenings |= OB_11_HCPS_6CS;
	else
		m_nAllowableOpenings &= ~OB_11_HCPS_6CS;

	if (m_b14TPsGs)
	 	m_nAllowableOpenings |= OB_14_TCPS_GS;
	else
	 	m_nAllowableOpenings &= ~OB_14_TCPS_GS;

	if (m_b14TPsLs)
		m_nAllowableOpenings |= OB_14_TCPS_LS;
	else
		m_nAllowableOpenings &= ~OB_14_TCPS_LS;

	if (m_b13TPsLs)
		m_nAllowableOpenings |= OB_13_TCPS_LS;
	else
		m_nAllowableOpenings &= ~OB_13_TCPS_LS;
	//
	m_conventionSet.SetValue(tnAllowable1Openings, m_nAllowableOpenings);
}
