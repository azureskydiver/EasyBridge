//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// bidprop.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
//#include "BidOptsParamsPage.h"
#include "BidOptsConfigPage.h"
#include "BidOptsGeneralPage.h"
#include "BidOptsTwoBidsPage.h"
#include "BidOptsNoTrumpsPage.h"
#include "BidOptsMiscPage.h"
#include "BidOptsOpenPage.h"
#include "BidOptionsPropSheet.h"
#include "HelpCode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBidOptionsPropSheet

IMPLEMENT_DYNAMIC(CBidOptionsPropSheet, CPropertySheet)

CBidOptionsPropSheet::CBidOptionsPropSheet(CObjectWithProperties* pApp, CObjectWithProperties* pConventionSet, CWnd* pParentWnd, UINT iSelectPage) :
		CPropertySheet(_T("Bidding Conventions"), pParentWnd, iSelectPage)
{
	// create the pages
	// m_pParamsPage = new CBidParamsPage(pApp, pConventionSet);
	m_pBidConfigPage = new CBidOptsConfigPage(pApp, pConventionSet);
	m_pBidGeneralPage = new CBidOptsGeneralPage(pApp, pConventionSet);
	m_pBidTwoBidsPage = new CBidOptsTwoBidsPage(pApp, pConventionSet);
	m_pBidNoTrumpsPage = new CBidOptsNoTrumpsPage(pApp, pConventionSet);
	m_pBidMiscPage = new CBidOptsMiscPage(pApp, pConventionSet);
	m_pBidOpenPage = new CBidOptsOpenPage(pApp, pConventionSet);

	// add the pages
//	AddPage(m_pBidParamsPage);
	AddPage(m_pBidConfigPage);
	AddPage(m_pBidGeneralPage);
	AddPage(m_pBidTwoBidsPage);
	AddPage(m_pBidNoTrumpsPage);
	AddPage(m_pBidMiscPage);
	AddPage(m_pBidOpenPage);
	//
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

CBidOptionsPropSheet::~CBidOptionsPropSheet()
{
	// delete all pages
	// delete m_pParamsPage;
	delete m_pBidConfigPage;
	delete m_pBidGeneralPage;
	delete m_pBidTwoBidsPage;
	delete m_pBidNoTrumpsPage;
	delete m_pBidMiscPage;
	delete m_pBidOpenPage;
}


BEGIN_MESSAGE_MAP(CBidOptionsPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CBidOptionsPropSheet)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBidOptionsPropSheet message handlers


//
void CBidOptionsPropSheet::UpdateAllPages()
{
	// update all data pages
//	m_pBidParamsPage.Update();
	m_pBidConfigPage->Update();
	m_pBidGeneralPage->Update();
	m_pBidTwoBidsPage->Update();
	m_pBidNoTrumpsPage->Update();
	m_pBidMiscPage->Update();
	m_pBidOpenPage->Update();
}


//
void CBidOptionsPropSheet::OnHelp()
{
	int nPageIndex = GetActiveIndex();
	int nHelpContextID = DIALOG_BID_OPTIONS_CONFIG_PAGE + nPageIndex;
	WinHelp(nHelpContextID);
}

//
BOOL CBidOptionsPropSheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;	
}
