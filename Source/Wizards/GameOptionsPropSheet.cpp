//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// GameOptionsPropSheet.cpp : implementation file
//
#include "stdafx.h"
#include "GameOptsMechanicsPage.h"
#include "GameOptsInterfacePage.h"
#include "GameOptsCountingPage.h"
#include "GameOptsScoringPage.h"
#include "GameOptsFilesPage.h"
#include "GameOptsGIBPage.h"
#include "GameOptionsPropSheet.h"
#include "HelpCode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameOptionsPropSheet

IMPLEMENT_DYNAMIC(CGameOptionsPropSheet, CPropertySheet)

CGameOptionsPropSheet::CGameOptionsPropSheet(CObjectWithProperties* pApp, CWnd* pParentWnd, UINT iSelectPage) :
		CPropertySheet(_T("Game Options"), pParentWnd, iSelectPage)
{
	// create all pages
	m_pMechanicsPage = new CGameOptsMechanicsPage(pApp);
	m_pInterfacePage = new CGameOptsInterfacePage(pApp);
	m_pCountingPage = new CGameOptsCountingPage(pApp);
	m_pScoringPage = new CGameOptsScoringPage(pApp);
	m_pFilesPage = new CGameOptsFilesPage(pApp);
#ifndef DISABLE_GIB
	m_pGibPage = new CGameOptsGIBPage(pApp);
#endif

	// then add the pages
	AddPage(m_pInterfacePage);
	AddPage(m_pMechanicsPage);
	AddPage(m_pCountingPage);
	AddPage(m_pScoringPage);
	AddPage(m_pFilesPage);
#ifndef DISABLE_GIB
	AddPage(m_pGibPage);
#endif
	//
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

CGameOptionsPropSheet::~CGameOptionsPropSheet()
{
	// delete all pages
	delete m_pMechanicsPage;
	delete m_pInterfacePage;
	delete m_pCountingPage;
	delete m_pScoringPage;
	delete m_pFilesPage;
#ifndef DISABLE_GIB
	delete m_pGibPage;
#endif
}


BEGIN_MESSAGE_MAP(CGameOptionsPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CGameOptionsPropSheet)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGameOptionsPropSheet message handlers



//
void CGameOptionsPropSheet::UpdateAllPages()
{
	// update all data pages
	m_pInterfacePage->Update();
	m_pMechanicsPage->Update();
	m_pCountingPage->Update();
	m_pScoringPage->Update();
	m_pFilesPage->Update();
	m_pGibPage->Update();
}


//
void CGameOptionsPropSheet::OnHelp()
{
	int nPageIndex = GetActiveIndex();
	int nHelpContextID = DIALOG_GAME_OPTIONS_INTERFACE_PAGE + nPageIndex;
	WinHelp(nHelpContextID);
}

//
BOOL CGameOptionsPropSheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;	
}
