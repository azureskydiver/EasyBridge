//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// GameOptsCountingPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ObjectWithProperties.h"
#include "GameOptsCountingPage.h"
#include "progopts.h"
#include "globals.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameOptsCountingPage property page

IMPLEMENT_DYNCREATE(CGameOptsCountingPage, CPropertyPage)

CGameOptsCountingPage::CGameOptsCountingPage(CObjectWithProperties* pApp) : 
		CPropertyPage(CGameOptsCountingPage::IDD),
		m_app(*pApp)
{
	//{{AFX_DATA_INIT(CGameOptsCountingPage)
	m_bAcelessPenalty = FALSE;
	m_b4AceBonus = FALSE;
	m_bPenalizeUGHonors = FALSE;
	m_bCountShortSuits = FALSE;
	m_nHonorsValuationMode = -1;
	m_fAceValue = 0.0;
	m_fKingValue = 0.0;
	m_fQueenValue = 0.0;
	m_fJackValue = 0.0;
	m_fTenValue = 0.0;
	//}}AFX_DATA_INIT
	m_nHonorsValuationMode = m_app.GetValue(tnHonorsValuationMode);
	m_fAceValue = m_app.GetValueDouble(tfCustomAceValue);
	m_fKingValue = m_app.GetValueDouble(tfCustomKingValue);
	m_fQueenValue = m_app.GetValueDouble(tfCustomQueenValue);
	m_fJackValue = m_app.GetValueDouble(tfCustomJackValue);
	m_fTenValue = m_app.GetValueDouble(tfCustomTenValue);
	//
/*
	m_bAcelessPenalty = m_app.GetValue(tbAcelessPenalty);
	m_b4AceBonus = m_app.GetValue(tb4AceBonus);
	m_bPenalizeUGHonors = m_app.GetValue(tbPenalizeUGHonors);
	m_bCountShortSuits = m_app.GetValue(tbCountShortSuits);
*/
}

CGameOptsCountingPage::~CGameOptsCountingPage()
{
}

void CGameOptsCountingPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGameOptsCountingPage)
	DDX_Check(pDX, IDC_ACELESS_PENALTY, m_bAcelessPenalty);
	DDX_Check(pDX, IDC_4ACE_BONUS, m_b4AceBonus);
	DDX_Check(pDX, IDC_PENALIZE_UNGUARDED_HONORS, m_bPenalizeUGHonors);
	DDX_Check(pDX, IDC_COUNT_SHORT_SUITS, m_bCountShortSuits);
	DDX_Radio(pDX, IDC_VALUATION1, m_nHonorsValuationMode);
	DDX_Text(pDX, IDC_VALUE_ACE, m_fAceValue);
	DDV_MinMaxDouble(pDX, m_fAceValue, 0., 10.);
	DDX_Text(pDX, IDC_VALUE_KING, m_fKingValue);
	DDV_MinMaxDouble(pDX, m_fKingValue, 0., 10.);
	DDX_Text(pDX, IDC_VALUE_QUEEN, m_fQueenValue);
	DDV_MinMaxDouble(pDX, m_fQueenValue, 0., 10.);
	DDX_Text(pDX, IDC_VALUE_JACK, m_fJackValue);
	DDV_MinMaxDouble(pDX, m_fJackValue, 0., 10.);
	DDX_Text(pDX, IDC_VALUE_TEN, m_fTenValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGameOptsCountingPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGameOptsCountingPage)
	ON_BN_CLICKED(IDC_VALUATION1, OnValuationModeChange)
	ON_BN_CLICKED(IDC_VALUATION2, OnValuationModeChange)
	ON_BN_CLICKED(IDC_VALUATION3, OnValuationModeChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGameOptsCountingPage message handlers


//
BOOL CGameOptsCountingPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// 
	OnValuationModeChange();

	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
void CGameOptsCountingPage::OnValuationModeChange() 
{
	UpdateData(TRUE);
	if (m_nHonorsValuationMode == 2)
	{
		GetDlgItem(IDC_VALUE_ACE)->EnableWindow(TRUE);
		GetDlgItem(IDC_VALUE_KING)->EnableWindow(TRUE);
		GetDlgItem(IDC_VALUE_QUEEN)->EnableWindow(TRUE);
		GetDlgItem(IDC_VALUE_JACK)->EnableWindow(TRUE);
		GetDlgItem(IDC_VALUE_TEN)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_VALUE_ACE)->EnableWindow(FALSE);
		GetDlgItem(IDC_VALUE_KING)->EnableWindow(FALSE);
		GetDlgItem(IDC_VALUE_QUEEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_VALUE_JACK)->EnableWindow(FALSE);
		GetDlgItem(IDC_VALUE_TEN)->EnableWindow(FALSE);
	}
}


//
void CGameOptsCountingPage::Update()
{
	// store results
	m_app.SetValue(tnHonorsValuationMode, m_nHonorsValuationMode);
	if (m_nHonorsValuationMode == 2)
	{
		// warn if total is not 10 pts
		double fTotal = m_fAceValue + m_fKingValue + m_fQueenValue + m_fJackValue + m_fTenValue;
		if ((fTotal < 9.9) || (fTotal >= 10.1))
			AfxMessageBox(FormString(_T("Warning -- your honors values total to %.2f.  If they do not add up to 10, the bidding engine may not function properly."), fTotal));
		//
		m_app.SetValueDouble(tfCustomAceValue, m_fAceValue);
		m_app.SetValueDouble(tfCustomKingValue, m_fKingValue);
		m_app.SetValueDouble(tfCustomQueenValue, m_fQueenValue);
		m_app.SetValueDouble(tfCustomJackValue, m_fJackValue);
		m_app.SetValueDouble(tfCustomTenValue, m_fTenValue);
	}
	//
/*
	m_app.SetValue(tbAcelessPenalty, m_bAcelessPenalty);
	m_app.SetValue(tb4AceBonus, m_b4AceBonus);
	m_app.SetValue(tbPenalizeUGHonors, m_bPenalizeUGHonors);
	m_app.SetValue(tbCountShortSuits, m_bCountShortSuits);
*/
	//
	m_app.RefreshProperties();	// ####!!!!!#####
}



