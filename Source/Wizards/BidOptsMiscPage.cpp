//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CBidOptsMiscPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ObjectWithProperties.h"
#include "BidOptsMiscPage.h"
#include "ConvCodes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBidOptsMiscPage property page

IMPLEMENT_DYNCREATE(CBidOptsMiscPage, CPropertyPage)

CBidOptsMiscPage::CBidOptsMiscPage(CObjectWithProperties* pApp, CObjectWithProperties* pm_conventionSet) : 
		CPropertyPage(CBidOptsMiscPage::IDD),
		m_app(*pApp), m_conventionSet(*pm_conventionSet)
{
	//{{AFX_DATA_INIT(CBidOptsMiscPage)
	m_nJumpOvercalls = -1;
	m_b4thSuitForcing = FALSE;
	m_bStructuredReverses = FALSE;
	m_bJacoby2NT = FALSE;
	m_bMichaelsCueBid = FALSE;
	m_bUnusualNT = FALSE;
	m_bDrury = FALSE;
	//}}AFX_DATA_INIT
	m_bJacoby2NT  = m_conventionSet.GetValue(tidJacoby2NT);
	m_bMichaelsCueBid= m_conventionSet.GetValue(tidMichaels);
	m_bUnusualNT = m_conventionSet.GetValue(tidUnusualNT);
	m_bDrury = m_conventionSet.GetValue(tidDrury);
	m_b4thSuitForcing = m_conventionSet.GetValue(tid4thSuitForcing);
	m_bStructuredReverses = m_conventionSet.GetValue(tbStructuredReverses);
	m_nJumpOvercalls = m_conventionSet.GetValue(tidWeakJumpOvercalls);
}

CBidOptsMiscPage::~CBidOptsMiscPage()
{
}

void CBidOptsMiscPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBidOptsMiscPage)
	DDX_Radio(pDX, IDC_RADIO_OVERCALLS, m_nJumpOvercalls);
	DDX_Check(pDX, IDC_4THSUIT_FORCING, m_b4thSuitForcing);
	DDX_Check(pDX, IDC_STRUCTURED_REVERSES, m_bStructuredReverses);
	DDX_Check(pDX, IDC_JACOBY_2NT, m_bJacoby2NT);
	DDX_Check(pDX, IDC_MICHAELS, m_bMichaelsCueBid);
	DDX_Check(pDX, IDC_UNUSUAL_NT, m_bUnusualNT);
	DDX_Check(pDX, IDC_DRURY, m_bDrury);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBidOptsMiscPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBidOptsMiscPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBidOptsMiscPage message handlers




//
void CBidOptsMiscPage::Update()
{
	// store results
	m_conventionSet.SetValue(tidJacoby2NT, m_bJacoby2NT);
	m_conventionSet.SetValue(tidMichaels, m_bMichaelsCueBid);
	m_conventionSet.SetValue(tidUnusualNT, m_bUnusualNT);
	m_conventionSet.SetValue(tidDrury, m_bDrury);
	m_conventionSet.SetValue(tid4thSuitForcing, m_b4thSuitForcing);
	m_conventionSet.SetValue(tbStructuredReverses, m_bStructuredReverses);
	m_conventionSet.SetValue(tidWeakJumpOvercalls, m_nJumpOvercalls);
}
