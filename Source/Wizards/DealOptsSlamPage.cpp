//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DealOptsSlamPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ObjectWithProperties.h"
#include "DealOptsSlamPage.h"
#include "progopts.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDealOptsSlamPage property page

IMPLEMENT_DYNCREATE(CDealOptsSlamPage, CPropertyPage)

CDealOptsSlamPage::CDealOptsSlamPage(CObjectWithProperties* pApp) : 
		CPropertyPage(CDealOptsSlamPage::IDD),
		m_app(*pApp)
{
	//{{AFX_DATA_INIT(CDealOptsSlamPage)
	m_numAcesForGenericSlam = -1;
	m_numAcesForSmallSlam = -1;
	m_numKingsForGenericSlam = -1;
	m_numKingsForSmallSlam = -1;
	m_numKingsForGrandSlam = -1;
	//}}AFX_DATA_INIT
	m_numAcesForGenericSlam = 4 - m_app.GetValue(tnumAcesForSlam, 0);
	m_numAcesForSmallSlam = 4 - m_app.GetValue(tnumAcesForSlam, 1);
//	m_numAcesForGrandSlam = 4 - m_app.GetValue(tnumAcesForSlam, 2);
	m_numKingsForGenericSlam =  4 - m_app.GetValue(tnumKingsForSlam, 0);
	m_numKingsForSmallSlam = 4 - m_app.GetValue(tnumKingsForSlam, 1);
	m_numKingsForGrandSlam = 4 - m_app.GetValue(tnumKingsForSlam, 2);
}

CDealOptsSlamPage::~CDealOptsSlamPage()
{
}

void CDealOptsSlamPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDealOptsSlamPage)
	DDX_Radio(pDX, IDC_RADIO1, m_numAcesForGenericSlam);
	DDX_Radio(pDX, IDC_RADIO4, m_numAcesForSmallSlam);
	DDX_Radio(pDX, IDC_RADIO9, m_numKingsForGenericSlam);
	DDX_Radio(pDX, IDC_RADIO12, m_numKingsForSmallSlam);
	DDX_Radio(pDX, IDC_RADIO15, m_numKingsForGrandSlam);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDealOptsSlamPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDealOptsSlamPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDealOptsSlamPage message handlers



void CDealOptsSlamPage::Update()
{
	// store results
	m_app.SetValue(tnumAcesForSlam, (4 - m_numAcesForGenericSlam), 0);
	m_app.SetValue(tnumAcesForSlam, (4 - m_numAcesForSmallSlam), 1);
//	m_app.SetValue(tnumAcesForSlam, (4 - m_numAcesForGrandSlam), 2);
	m_app.SetValue(tnumKingsForSlam, (4 - m_numKingsForGenericSlam), 0);
	m_app.SetValue(tnumKingsForSlam, (4 - m_numKingsForSmallSlam), 1);
	m_app.SetValue(tnumKingsForSlam, (4 - m_numKingsForGrandSlam), 2);
}
