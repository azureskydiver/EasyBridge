//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// GameOptsFilesPage.cpp : implementation file
//

#include "stdafx.h"
#include "ObjectWithProperties.h"
#include "GameOptsFilesPage.h"
#include "progopts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameOptsFilesPage property page

IMPLEMENT_DYNCREATE(CGameOptsFilesPage, CPropertyPage)

CGameOptsFilesPage::CGameOptsFilesPage(CObjectWithProperties* pApp) : 
		CPropertyPage(CGameOptsFilesPage::IDD),
		m_app(*pApp)
{
	//{{AFX_DATA_INIT(CGameOptsFilesPage)
	m_bSaveIntermediatePositions = FALSE;
	m_bExposePBNGameCards = FALSE;
	//}}AFX_DATA_INIT
	m_bSaveIntermediatePositions = m_app.GetValue(tbSaveIntermediatePositions);
	m_bExposePBNGameCards = m_app.GetValue(tbExposePBNGameCards);
}

CGameOptsFilesPage::~CGameOptsFilesPage()
{
}

void CGameOptsFilesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGameOptsFilesPage)
	DDX_Check(pDX, IDC_SAVE_POSITIONS, m_bSaveIntermediatePositions);
	DDX_Check(pDX, IDC_SHOW_PBN_CARDS_FACEUP, m_bExposePBNGameCards);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGameOptsFilesPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGameOptsFilesPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameOptsFilesPage message handlers


//
void CGameOptsFilesPage::Update()
{
	// store results
	m_app.SetValue(tbSaveIntermediatePositions, m_bSaveIntermediatePositions);
	m_app.SetValue(tbExposePBNGameCards, m_bExposePBNGameCards);
}

