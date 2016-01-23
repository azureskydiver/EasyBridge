//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DispOptionsPropSheet.cpp : implementation file
//

#include "stdafx.h"
#include "DispOptsSuitsPage.h"
#include "DispOptsCardsPage.h"
#include "DispOptsMiscPage.h"
#include "DispOptsCardBacksPage.h"
#include "DispOptsDialogsPage.h"
#include "DispOptsFontsPage.h"
#include "DispOptionsPropSheet.h"
#include "HelpCode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDispOptionsPropSheet

IMPLEMENT_DYNAMIC(CDispOptionsPropSheet, CPropertySheet)

CDispOptionsPropSheet::CDispOptionsPropSheet(CObjectWithProperties* pApp, CObjectWithProperties* pFrame, CObjectWithProperties* pView, CObjectWithProperties* pDeck, CWnd* pParentWnd, UINT iSelectPage) :
		CPropertySheet(_T("Display Options"), pParentWnd, iSelectPage)
{
	// create the pages
	m_pSuitsPage = new CDispOptsSuitsPage(pApp, pFrame, pView);
	m_pCardsPage = new CDispOptsCardsPage(pApp, pFrame, pView);
	m_pCardBacksPage = new CDispOptsCardBacksPage(pApp, pFrame, pView, pDeck);
	m_pDialogsPage = new CDispOptsDialogsPage(pApp, pFrame, pView);
	m_pFontsPage = new CDispOptsFontsPage(pApp, pFrame, pView);
	m_pMiscPage = new CDispOptsMiscPage(pApp, pFrame, pView);

	// then add the pages
	AddPage(m_pSuitsPage);
	AddPage(m_pCardsPage);
	AddPage(m_pCardBacksPage);
	AddPage(m_pDialogsPage);
	AddPage(m_pFontsPage);
	AddPage(m_pMiscPage);
	//
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

CDispOptionsPropSheet::~CDispOptionsPropSheet()
{
	// delete all pages
	delete m_pSuitsPage;
	delete m_pCardsPage;
	delete m_pCardBacksPage;
	delete m_pDialogsPage;
	delete m_pFontsPage;
	delete m_pMiscPage;
}


BEGIN_MESSAGE_MAP(CDispOptionsPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CDispOptionsPropSheet)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDispOptionsPropSheet message handlers


//
void CDispOptionsPropSheet::UpdateAllPages()
{
	// update all data pages
	m_bDisplayAffected = FALSE;
	m_bGlobalDisplayAffected = FALSE;
	//
	m_bDisplayAffected |= m_pSuitsPage->Update();
	m_bDisplayAffected |= m_pCardsPage->Update();
	m_bDisplayAffected |= m_pCardBacksPage->Update();
	m_bGlobalDisplayAffected |= m_pDialogsPage->Update();
	m_bGlobalDisplayAffected |= m_pFontsPage->Update();
	m_bGlobalDisplayAffected |= m_pMiscPage->Update();
}


//
void CDispOptionsPropSheet::OnHelp()
{
	int nPageIndex = GetActiveIndex();
	int nHelpContextID = DIALOG_DISPLAY_OPTIONS_SUITS_PAGE + nPageIndex;
	WinHelp(nHelpContextID);
}

//
BOOL CDispOptionsPropSheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;	
}

