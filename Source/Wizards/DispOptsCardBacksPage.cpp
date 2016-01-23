//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DispOptsCardBacksPage.cpp : implementation file
//

#include "stdafx.h"
#include "ObjectWithProperties.h"
#include "DispOptsCardBacksPage.h"
#include "deckopts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDispOptsCardBacksPage dialog

IMPLEMENT_DYNCREATE(CDispOptsCardBacksPage, CPropertyPage)

CDispOptsCardBacksPage::CDispOptsCardBacksPage(CObjectWithProperties* pApp, CObjectWithProperties* pFrame, CObjectWithProperties* pView, CObjectWithProperties* pDeck) :
		CPropertyPage(CDispOptsCardBacksPage::IDD),
		m_app(*pApp), m_frame(*pFrame), m_view(*pView), m_deck(*pDeck)
{
	//{{AFX_DATA_INIT(CDispOptsCardBacksPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nIndex = m_deck.GetValue(tnCardBackIndex);
}

CDispOptsCardBacksPage::~CDispOptsCardBacksPage()
{
}

void CDispOptsCardBacksPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDispOptsCardBacksPage)
	DDX_Control(pDX, IDC_LIST, m_listCardBacks);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDispOptsCardBacksPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDispOptsCardBacksPage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDispOptsCardBacksPage message handlers


//
BOOL CDispOptsCardBacksPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// insert cardback images
	m_listCardBacks.SetImageList((CImageList*)m_deck.GetValuePV(tpvCardBacksImageList), LVSIL_NORMAL);
	int numCardBacks = m_deck.GetValue(tnumCardBacks);
//	m_listCardBacks.InsertColumn(0, "", LVCFMT_LEFT, 32);
	for(int i=0;i<numCardBacks;i++)
		m_listCardBacks.InsertItem(i, "", i);

	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



//
BOOL CDispOptsCardBacksPage::Update()
{
	if ((m_nIndex >= 0) && (m_nIndex != m_deck.GetValue(tnCardBackIndex)))
	{
		m_deck.SetValue(tnCardBackIndex, m_nIndex);
		return TRUE;
	}
	//
	return FALSE;	// no changes
}


//
void CDispOptsCardBacksPage::OnDestroy() 
{
	// store results & return if changes affect the display
	m_nIndex = m_listCardBacks.GetNextItem(-1, LVNI_SELECTED);

	//
	CPropertyPage::OnDestroy();
}
