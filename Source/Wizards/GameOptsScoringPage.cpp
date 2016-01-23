//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// GameOptsScoringPage.cpp : implementation file
//

#include "stdafx.h"
#include "ObjectWithProperties.h"
#include "GameOptsScoringPage.h"
#include "progopts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameOptsScoringPage property page

IMPLEMENT_DYNCREATE(CGameOptsScoringPage, CPropertyPage)

CGameOptsScoringPage::CGameOptsScoringPage(CObjectWithProperties* pApp) : 
		CPropertyPage(CGameOptsScoringPage::IDD),
		m_app(*pApp)
{
	//{{AFX_DATA_INIT(CGameOptsScoringPage)
	m_bScoreHonorsBonuses = FALSE;
	m_bUseDuplicateScoring = FALSE;
	//}}AFX_DATA_INIT
	m_bScoreHonorsBonuses = m_app.GetValue(tbScoreHonorsBonuses);
	m_bUseDuplicateScoring = m_app.GetValue(tbUsingDuplicateScoring);
}

CGameOptsScoringPage::~CGameOptsScoringPage()
{
}

void CGameOptsScoringPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGameOptsScoringPage)
	DDX_Check(pDX, IDC_SCORE_HONORS, m_bScoreHonorsBonuses);
	DDX_Check(pDX, IDC_DUPLICATE_SCORING, m_bUseDuplicateScoring);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGameOptsScoringPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGameOptsScoringPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameOptsScoringPage message handlers



//
void CGameOptsScoringPage::Update()
{
	// store results
	m_app.SetValue(tbScoreHonorsBonuses, m_bScoreHonorsBonuses);
	m_app.SetValue(tbUsingDuplicateScoring, m_bUseDuplicateScoring);
}

