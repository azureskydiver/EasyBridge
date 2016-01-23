//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// dis_card.cpp : implementation file
//

#include "stdafx.h"
#include "ObjectWithProperties.h"
#include "DispOptsCardsPage.h"
#include "viewopts.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDispOptsCardsPage property page

IMPLEMENT_DYNCREATE(CDispOptsCardsPage, CPropertyPage)

CDispOptsCardsPage::CDispOptsCardsPage(CObjectWithProperties* pApp, CObjectWithProperties* pFrame, CObjectWithProperties* pView) : 
		CPropertyPage(CDispOptsCardsPage::IDD),
		m_app(*pApp), m_frame(*pFrame), m_view(*pView)
{
	//{{AFX_DATA_INIT(CDispOptsCardsPage)
	m_bOffsetVertSuits = FALSE;
	m_bTripleBuffer = FALSE;
	m_bAnimateCards = FALSE;
	m_strGranularity = _T("");
	//}}AFX_DATA_INIT
	m_bTripleBuffer = m_view.GetValue(tbTripleBuffer);
	m_bAnimateCards = m_view.GetValue(tbAnimateCards);
	m_bOffsetVertSuits = m_view.GetValue(tbOffsetVerticalSuits);
	m_nAnimationGranularity = m_view.GetValue(tnAnimationGranularity);
	m_strGranularity.Format("%d", m_nAnimationGranularity);
}

CDispOptsCardsPage::~CDispOptsCardsPage()
{
}

void CDispOptsCardsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDispOptsCardsPage)
	DDX_Control(pDX, IDC_GRANULARITY, m_sliderGranularity);
//	DDX_Control(pDX, IDC_SPIN_GRANULARITY, m_spinGranularity);
	DDX_Check(pDX, IDC_OFFSET_EW_DISPLAY, m_bOffsetVertSuits);
	DDX_Check(pDX, IDC_TRIPLE_BUFFER, m_bTripleBuffer);
	DDX_Check(pDX, IDC_ANIMATE_CARDS, m_bAnimateCards);
//	DDX_Text(pDX, IDC_EDIT_GRANULARITY, m_strGranularity);
	DDX_Text(pDX, IDC_TEXT_GRANULARITY, m_strGranularity);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDispOptsCardsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDispOptsCardsPage)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDispOptsCardsPage message handlers


//
BOOL CDispOptsCardsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	//
//	m_spinGranularity.SetRange(1,64);
//	m_spinGranularity.SetPos(16);
	m_sliderGranularity.SetRange(0,60);
	m_sliderGranularity.SetTicFreq(5);
	m_sliderGranularity.SetPos(m_nAnimationGranularity);
	UpdateData(FALSE);
	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CDispOptsCardsPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CSliderCtrl* pSlider = (CSliderCtrl*) GetDlgItem(IDC_GRANULARITY);
	if ((CSliderCtrl*)pScrollBar == pSlider)
	{
		UpdateData(TRUE);
		m_nAnimationGranularity = pSlider->GetPos();
		m_strGranularity.Format("%d", m_nAnimationGranularity);
		UpdateData(FALSE);
	}

// CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}


//
BOOL CDispOptsCardsPage::Update()
{
	// store results & return if changes affect the display
	BOOL bChanged = FALSE;
	m_view.SetValue(tbTripleBuffer, m_bTripleBuffer);
	m_view.SetValue(tbAnimateCards, m_bAnimateCards);
	m_view.SetValue(tnAnimationGranularity,atoi(m_strGranularity));
	if (m_bOffsetVertSuits != m_view.GetValue(tbOffsetVerticalSuits)) 
	{
		m_view.SetValue(tbOffsetVerticalSuits, m_bOffsetVertSuits);
		bChanged = TRUE;
	}
	return bChanged;
}


