//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// dis_suit.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ObjectWithProperties.h"
#include "DispOptsSuitsPage.h"
#include "progopts.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDispOptsSuitsPage property page

IMPLEMENT_DYNCREATE(CDispOptsSuitsPage, CPropertyPage)

CDispOptsSuitsPage::CDispOptsSuitsPage(CObjectWithProperties* pApp, CObjectWithProperties* pFrame, CObjectWithProperties* pView) : 
		CPropertyPage(CDispOptsSuitsPage::IDD),
		m_app(*pApp), m_frame(*pFrame), m_view(*pView)
{
	//{{AFX_DATA_INIT(CDispOptsSuitsPage)
	m_nSuitSeqOption = -1;
	m_bShowDummyTrumpsOnLeft = FALSE;
	//}}AFX_DATA_INIT
	m_nSuitSeqOption = m_app.GetValue(tnSuitSequenceOption);
	m_bShowDummyTrumpsOnLeft = m_app.GetValue(tbShowDummyTrumpsOnLeft);
}

CDispOptsSuitsPage::~CDispOptsSuitsPage()
{
}

void CDispOptsSuitsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDispOptsSuitsPage)
	DDX_Radio(pDX, IDC_SUIT_DISPLAY_ORDER, m_nSuitSeqOption);
	DDX_Check(pDX, IDC_SHOW_DUMMY_TRUMPS_ON_LEFT, m_bShowDummyTrumpsOnLeft);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDispOptsSuitsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDispOptsSuitsPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDispOptsSuitsPage message handlers



//
BOOL CDispOptsSuitsPage::Update()
{
	// store results & return if changes affect the display
	BOOL bChanged = FALSE;
	if ( (m_nSuitSeqOption != m_app.GetValue(tnSuitSequenceOption)) ||
		 (m_bShowDummyTrumpsOnLeft != m_app.GetValue(tbShowDummyTrumpsOnLeft)) )
		bChanged = TRUE;
	m_app.SetValue(tnSuitSequenceOption, m_nSuitSeqOption);
	m_app.SetValue(tbShowDummyTrumpsOnLeft, m_bShowDummyTrumpsOnLeft);
	//
	return bChanged;
}
