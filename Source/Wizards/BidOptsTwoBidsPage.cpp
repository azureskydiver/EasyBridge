//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// BidOptsTwoBidsPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ObjectWithProperties.h"
#include "BidOptsTwoBidsPage.h"
#include "ConvCodes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBidOptsTwoBidsPage property page

IMPLEMENT_DYNCREATE(CBidOptsTwoBidsPage, CPropertyPage)

CBidOptsTwoBidsPage::CBidOptsTwoBidsPage(CObjectWithProperties* pApp, CObjectWithProperties* pm_conventionSet) : 
		CPropertyPage(CBidOptsTwoBidsPage::IDD),
		m_app(*pApp), m_conventionSet(*pm_conventionSet)
{
	//{{AFX_DATA_INIT(CBidOptsTwoBidsPage)
	m_nTwoBidsMode = -1;
	m_n2NTAfterWeak2 = -1;
	m_n2ClubOpenValues = -1;
	//}}AFX_DATA_INIT
	m_nTwoBidsMode = (m_conventionSet.GetValue(tidWeakTwoBids) == 1)? TRUE : FALSE;
	m_n2NTAfterWeak2 = (m_conventionSet.GetValue(tidOgust) == 1)? TRUE : FALSE;
	m_n2ClubOpenValues = m_conventionSet.GetValue(tn2ClubOpenRange);
}

CBidOptsTwoBidsPage::~CBidOptsTwoBidsPage()
{
}

void CBidOptsTwoBidsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBidOptsTwoBidsPage)
	DDX_Radio(pDX, IDC_RADIO_TWOBIDS, m_nTwoBidsMode);
	DDX_Radio(pDX, IDC_RADIO_2NT_AFTER_WEAK_2, m_n2NTAfterWeak2);
	DDX_Radio(pDX, IDC_RADIO_2COPEN20, m_n2ClubOpenValues);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBidOptsTwoBidsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBidOptsTwoBidsPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBidOptsTwoBidsPage message handlers



//
void CBidOptsTwoBidsPage::Update()
{
	// store results
	m_conventionSet.SetValue(tidWeakTwoBids, m_nTwoBidsMode);
	m_conventionSet.SetValue(tidOgust, m_n2NTAfterWeak2);
	m_conventionSet.SetValue(tn2ClubOpenRange, m_n2ClubOpenValues);
}
