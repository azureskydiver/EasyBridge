//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DealOptionsPropSheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DealOptsPointsPage.h"
#include "DealOptsMinorsPage.h"
#include "DealOptsMajorsPage.h"
#include "DealOptsNTPage.h"
#include "DealOptsSlamPage.h"
#include "DealOptsMiscPage.h"
#include "DealOptionsPropSheet.h"
#include "HelpCode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDealOptionsPropSheet


IMPLEMENT_DYNAMIC(CDealOptionsPropSheet, CPropertySheet)

CDealOptionsPropSheet::CDealOptionsPropSheet(CObjectWithProperties* pApp, CWnd* pParentWnd, UINT iSelectPage) :
		CPropertySheet(_T("Deal Options"), pParentWnd, iSelectPage)
{
	// first create the pages
	m_pDealPointsPage = new CDealOptsPointsPage(pApp);
	m_pDealMinorsPage = new CDealOptsMinorsPage(pApp);
	m_pDealMajorsPage = new CDealOptsMajorsPage(pApp);
	m_pDealNoTrumpsPage = new CDealOptsNoTrumpPage(pApp);
	m_pDealSlamPage = new CDealOptsSlamPage(pApp);
	m_pDealMiscPage = new CDealOptsMiscPage(pApp);

	// then add the agaes
	AddPage(m_pDealPointsPage);
	AddPage(m_pDealMinorsPage);
	AddPage(m_pDealMajorsPage);
	AddPage(m_pDealNoTrumpsPage);
	AddPage(m_pDealSlamPage);
	AddPage(m_pDealMiscPage);
	//
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

CDealOptionsPropSheet::~CDealOptionsPropSheet()
{
	// delete all pages
	delete m_pDealPointsPage;
	delete m_pDealMinorsPage;
	delete m_pDealMajorsPage;
	delete m_pDealNoTrumpsPage;
	delete m_pDealSlamPage;
	delete m_pDealMiscPage;
}


BEGIN_MESSAGE_MAP(CDealOptionsPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CDealOptionsPropSheet)
	ON_WM_SETFOCUS()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDealOptionsPropSheet message handlers

//
void CDealOptionsPropSheet::UpdateAllPages()
{
	// update all data pages
	m_pDealPointsPage->Update();
	m_pDealMinorsPage->Update();
	m_pDealMajorsPage->Update();
	m_pDealNoTrumpsPage->Update();
	m_pDealSlamPage->Update();
	m_pDealMiscPage->Update();
}


//
void CDealOptionsPropSheet::OnHelp()
{
	int nPageIndex = GetActiveIndex();
	int nHelpContextID = DIALOG_DEAL_OPTIONS_POINTS_PAGE + nPageIndex;
	WinHelp(nHelpContextID);
}

//
BOOL CDealOptionsPropSheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;	
}


