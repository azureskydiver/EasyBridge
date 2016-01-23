//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DealOptsPointsPage.cpp : implementation file
//

#include "stdafx.h"
#include "ObjectWithProperties.h"
#include "DealOptsPointsPage.h"
#include "progopts.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CDealOptsPointsPage property page

IMPLEMENT_DYNCREATE(CDealOptsPointsPage, CPropertyPage)

CDealOptsPointsPage::CDealOptsPointsPage(CObjectWithProperties* pApp) : 
		CPropertyPage(CDealOptsPointsPage::IDD),
		m_app(*pApp)
{
	//{{AFX_DATA_INIT(CDealOptsPointsPage)
	m_nMinGamePts = 0;
	m_nMinMinorGamePts = 0;
	m_nMaxMinorGamePts = 0;
	m_nMaxGamePts = 0;
	m_nMinMajorGamePts = 0;
	m_nMaxMajorGamePts = 0;
	m_nMinNoTrumpGamePts = 0;
	m_nMaxNoTrumpGamePts = 0;
	m_nMinSlamPts = 0;
	m_nMaxSlamPts = 0;
	m_nMaxSmallSlamPts = 0;
	m_nMinSmallSlamPts = 0;
	m_nMinGrandSlamPts = 0;
	m_nMaxGrandSlamPts = 0;
	//}}AFX_DATA_INIT
}

CDealOptsPointsPage::~CDealOptsPointsPage()
{
}


void CDealOptsPointsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDealOptsPointsPage)
	DDX_Control(pDX, IDC_SPIN13, m_sbMaxGrandSlamPts);
	DDX_Control(pDX, IDC_SPIN12, m_sbMinGrandSlamPts);
	DDX_Control(pDX, IDC_SPIN11, m_sbMaxSmallSlamPts);
	DDX_Control(pDX, IDC_SPIN10, m_sbMinSmallSlamPts);
	DDX_Control(pDX, IDC_SPIN9, m_sbMaxSlamPts);
	DDX_Control(pDX, IDC_SPIN8, m_sbMinSlamPts);
	DDX_Control(pDX, IDC_SPIN7, m_sbMaxNTGamePts);
	DDX_Control(pDX, IDC_SPIN6, m_sbMinNTGamePts);
	DDX_Control(pDX, IDC_SPIN5, m_sbMaxMinorGamePts);
	DDX_Control(pDX, IDC_SPIN4, m_sbMinMinorGamePts);
	DDX_Control(pDX, IDC_SPIN3, m_sbMaxMajorGamePts);
	DDX_Control(pDX, IDC_SPIN2, m_sbMinMajorGamePts);
	DDX_Control(pDX, IDC_SPIN1, m_sbMaxGamePts);
	DDX_Control(pDX, IDC_SPIN0, m_sbMinGamePts);
	DDX_Text(pDX, IDC_MIN_GAME_PTS, m_nMinGamePts);
	DDV_MinMaxInt(pDX, m_nMinGamePts, 0, 40);
	DDX_Text(pDX, IDC_MIN_MINOR_GAME_PTS, m_nMinMinorGamePts);
	DDV_MinMaxInt(pDX, m_nMinMinorGamePts, 0, 40);
	DDX_Text(pDX, IDC_MAX_MINOR_GAME_PTS, m_nMaxMinorGamePts);
	DDV_MinMaxInt(pDX, m_nMaxMinorGamePts, 0, 40);
	DDX_Text(pDX, IDC_MAX_GAME_PTS, m_nMaxGamePts);
	DDV_MinMaxInt(pDX, m_nMaxGamePts, 0, 40);
	DDX_Text(pDX, IDC_MIN_MAJOR_GAME_PTS, m_nMinMajorGamePts);
	DDV_MinMaxInt(pDX, m_nMinMajorGamePts, 0, 40);
	DDX_Text(pDX, IDC_MAX_MAJOR_GAME_PTS, m_nMaxMajorGamePts);
	DDV_MinMaxInt(pDX, m_nMaxMajorGamePts, 0, 40);
	DDX_Text(pDX, IDC_MIN_NT_GAME_PTS, m_nMinNoTrumpGamePts);
	DDV_MinMaxInt(pDX, m_nMinNoTrumpGamePts, 0, 40);
	DDX_Text(pDX, IDC_MAX_NT_GAME_PTS, m_nMaxNoTrumpGamePts);
	DDX_Text(pDX, IDC_MIN_SLAM_PTS, m_nMinSlamPts);
	DDV_MinMaxInt(pDX, m_nMinSlamPts, 0, 40);
	DDX_Text(pDX, IDC_MAX_SLAM_PTS, m_nMaxSlamPts);
	DDV_MinMaxInt(pDX, m_nMaxSlamPts, 0, 40);
	DDX_Text(pDX, IDC_MAX_SMALL_SLAM_PTS, m_nMaxSmallSlamPts);
	DDV_MinMaxInt(pDX, m_nMaxSmallSlamPts, 0, 40);
	DDX_Text(pDX, IDC_MIN_SMALL_SLAM_PTS, m_nMinSmallSlamPts);
	DDV_MinMaxInt(pDX, m_nMinSmallSlamPts, 0, 40);
	DDX_Text(pDX, IDC_MIN_GRAND_SLAM_PTS, m_nMinGrandSlamPts);
	DDV_MinMaxInt(pDX, m_nMinGrandSlamPts, 0, 40);
	DDX_Text(pDX, IDC_MAX_GRAND_SLAM_PTS, m_nMaxGrandSlamPts);
	DDV_MinMaxInt(pDX, m_nMaxGrandSlamPts, 0, 40);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDealOptsPointsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDealOptsPointsPage)
	ON_NOTIFY(NM_CLICK, IDC_SPIN0, OnClickSpin)
	ON_EN_CHANGE(IDC_MAX_GAME_PTS, OnChangeSpinValue)
	ON_NOTIFY(NM_CLICK, IDC_SPIN1, OnClickSpin)
	ON_EN_CHANGE(IDC_MAX_GRAND_SLAM_PTS, OnChangeSpinValue)
	ON_EN_CHANGE(IDC_MAX_MAJOR_GAME_PTS, OnChangeSpinValue)
	ON_EN_CHANGE(IDC_MAX_MINOR_GAME_PTS, OnChangeSpinValue)
	ON_EN_CHANGE(IDC_MAX_NT_GAME_PTS, OnChangeSpinValue)
	ON_EN_CHANGE(IDC_MAX_SLAM_PTS, OnChangeSpinValue)
	ON_EN_CHANGE(IDC_MAX_SMALL_SLAM_PTS, OnChangeSpinValue)
	ON_EN_CHANGE(IDC_MIN_GAME_PTS, OnChangeSpinValue)
	ON_EN_CHANGE(IDC_MIN_GRAND_SLAM_PTS, OnChangeSpinValue)
	ON_EN_CHANGE(IDC_MIN_MAJOR_GAME_PTS, OnChangeSpinValue)
	ON_EN_CHANGE(IDC_MIN_MINOR_GAME_PTS, OnChangeSpinValue)
	ON_EN_CHANGE(IDC_MIN_NT_GAME_PTS, OnChangeSpinValue)
	ON_EN_CHANGE(IDC_MIN_SLAM_PTS, OnChangeSpinValue)
	ON_EN_CHANGE(IDC_MIN_SMALL_SLAM_PTS, OnChangeSpinValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDealOptsPointsPage message handlers


//
BOOL CDealOptsPointsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// inti spin buttons & link them to the edit controls
	InitSpinButtons();
	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDealOptsPointsPage::InitSpinButtons()
{
	m_nMinGamePts = m_app.GetValue(tnRequiredPointsForGame,0,0);
	m_nMaxGamePts = m_app.GetValue(tnRequiredPointsForGame,0,1);
	m_nMinMinorGamePts = m_app.GetValue(tnRequiredPointsForGame,1,0);
	m_nMaxMinorGamePts = m_app.GetValue(tnRequiredPointsForGame,1,1);
	m_nMinMajorGamePts = m_app.GetValue(tnRequiredPointsForGame,2,0);
	m_nMaxMajorGamePts = m_app.GetValue(tnRequiredPointsForGame,2,1);
	m_nMinNoTrumpGamePts = m_app.GetValue(tnRequiredPointsForGame,3,0);
	m_nMaxNoTrumpGamePts = m_app.GetValue(tnRequiredPointsForGame,3,1);
	m_nMinSlamPts = m_app.GetValue(tnRequiredPointsForSlam,0,0);
	m_nMaxSlamPts = m_app.GetValue(tnRequiredPointsForSlam,0,1);
	m_nMinSmallSlamPts = m_app.GetValue(tnRequiredPointsForSlam,1,0);
	m_nMaxSmallSlamPts = m_app.GetValue(tnRequiredPointsForSlam,1,1);
	m_nMinGrandSlamPts = m_app.GetValue(tnRequiredPointsForSlam,2,0);
	m_nMaxGrandSlamPts = m_app.GetValue(tnRequiredPointsForSlam,2,1);
	//
	m_sbMinGamePts.SetRange(m_app.GetValue(tnPointsAbsoluteGameLimits,0,0),
							m_app.GetValue(tnPointsAbsoluteGameLimits,0,1));
	m_sbMinGamePts.SetPos(m_nMinGamePts);
	m_sbMaxGamePts.SetRange(m_app.GetValue(tnPointsAbsoluteGameLimits,0,0),
						    m_app.GetValue(tnPointsAbsoluteGameLimits,0,1));
	m_sbMaxGamePts.SetPos(m_nMaxGamePts);
	//
	m_sbMinMinorGamePts.SetRange(m_app.GetValue(tnPointsAbsoluteGameLimits,1,0),
								  m_app.GetValue(tnPointsAbsoluteGameLimits,1,1));
	m_sbMinMinorGamePts.SetPos(m_nMinMinorGamePts);
	m_sbMaxMinorGamePts.SetRange(m_app.GetValue(tnPointsAbsoluteGameLimits,1,0),
								 m_app.GetValue(tnPointsAbsoluteGameLimits,1,1));
	m_sbMaxMinorGamePts.SetPos(m_nMaxMinorGamePts);
	//
	m_sbMinMajorGamePts.SetRange(m_app.GetValue(tnPointsAbsoluteGameLimits,2,0),
								 m_app.GetValue(tnPointsAbsoluteGameLimits,2,1));
	m_sbMinMajorGamePts.SetPos(m_nMinMajorGamePts);
	m_sbMaxMajorGamePts.SetRange(m_app.GetValue(tnPointsAbsoluteGameLimits,2,0),
							     m_app.GetValue(tnPointsAbsoluteGameLimits,2,1));
	m_sbMaxMajorGamePts.SetPos(m_nMaxMajorGamePts);
	//
	m_sbMinNTGamePts.SetRange(m_app.GetValue(tnPointsAbsoluteGameLimits,3,0),
							  m_app.GetValue(tnPointsAbsoluteGameLimits,3,1));
	m_sbMinNTGamePts.SetPos(m_nMinNoTrumpGamePts);
	m_sbMaxNTGamePts.SetRange(m_app.GetValue(tnPointsAbsoluteGameLimits,3,0),
						     m_app.GetValue(tnPointsAbsoluteGameLimits,3,1));
	m_sbMaxNTGamePts.SetPos(m_nMaxNoTrumpGamePts);
	//
	m_sbMinSlamPts.SetRange(m_app.GetValue(tnPointsAbsoluteSlamLimits,0,0),
							m_app.GetValue(tnPointsAbsoluteSlamLimits,0,1));
	m_sbMinSlamPts.SetPos(m_nMinSlamPts);
	m_sbMaxSlamPts.SetRange(m_app.GetValue(tnPointsAbsoluteSlamLimits,0,0),
							m_app.GetValue(tnPointsAbsoluteSlamLimits,0,1));
	m_sbMaxSlamPts.SetPos(m_nMaxSlamPts);
	//
	m_sbMinSmallSlamPts.SetRange(m_app.GetValue(tnPointsAbsoluteSlamLimits,1,0),
			 				     m_app.GetValue(tnPointsAbsoluteSlamLimits,1,1));
	m_sbMinSmallSlamPts.SetPos(m_nMinSmallSlamPts);
	m_sbMaxSmallSlamPts.SetRange(m_app.GetValue(tnPointsAbsoluteSlamLimits,1,0),
								 m_app.GetValue(tnPointsAbsoluteSlamLimits,1,1));
	m_sbMaxSmallSlamPts.SetPos(m_nMaxSmallSlamPts);
	//
	m_sbMinGrandSlamPts.SetRange(m_app.GetValue(tnPointsAbsoluteSlamLimits,2,0),
								 m_app.GetValue(tnPointsAbsoluteSlamLimits,2,1));
	m_sbMinGrandSlamPts.SetPos(m_nMinGrandSlamPts);
	m_sbMaxGrandSlamPts.SetRange(m_app.GetValue(tnPointsAbsoluteSlamLimits,2,0),
								 m_app.GetValue(tnPointsAbsoluteSlamLimits,2,1));
	m_sbMaxGrandSlamPts.SetPos(m_nMaxGrandSlamPts);
	// done
	UpdateData(FALSE);
}


//
void CDealOptsPointsPage::OnChangeSpinValue() 
{
	// need not do anything for now
}


//
void CDealOptsPointsPage::OnClickSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// 
	*pResult = 0;
}



//
void CDealOptsPointsPage::Update()
{
	// validate inputs
	if (m_nMaxGamePts < m_nMinGamePts)
		m_nMaxGamePts = m_nMinGamePts;
	if (m_nMaxMinorGamePts < m_nMinMinorGamePts)
		m_nMaxMinorGamePts = m_nMinMinorGamePts;
	if (m_nMaxMajorGamePts < m_nMinMajorGamePts)
		m_nMaxMajorGamePts = m_nMinMajorGamePts;
	if (m_nMaxNoTrumpGamePts < m_nMinNoTrumpGamePts)
		m_nMaxNoTrumpGamePts = m_nMinNoTrumpGamePts;
	if (m_nMaxSlamPts < m_nMinSlamPts)
		m_nMaxSlamPts = m_nMinSlamPts;
	if (m_nMaxSmallSlamPts < m_nMinSmallSlamPts)
		m_nMaxSmallSlamPts = m_nMinSmallSlamPts;
	if (m_nMaxGrandSlamPts < m_nMinGrandSlamPts)
		m_nMaxGrandSlamPts = m_nMinGrandSlamPts;
	// store results
	m_app.SetValue(tnRequiredPointsForGame,m_nMinGamePts,0,0);
	m_app.SetValue(tnRequiredPointsForGame,m_nMaxGamePts,0,1);
	m_app.SetValue(tnRequiredPointsForGame,m_nMinMinorGamePts,1,0);
	m_app.SetValue(tnRequiredPointsForGame,m_nMaxMinorGamePts,1,1);
	m_app.SetValue(tnRequiredPointsForGame,m_nMinMajorGamePts,2,0);
	m_app.SetValue(tnRequiredPointsForGame,m_nMaxMajorGamePts,2,1);
	m_app.SetValue(tnRequiredPointsForGame,m_nMinNoTrumpGamePts,3,0);
	m_app.SetValue(tnRequiredPointsForGame,m_nMaxNoTrumpGamePts,3,1);
	m_app.SetValue(tnRequiredPointsForSlam,m_nMinSlamPts,0,0);
	m_app.SetValue(tnRequiredPointsForSlam,m_nMaxSlamPts,0,1);
	m_app.SetValue(tnRequiredPointsForSlam,m_nMinSmallSlamPts,1,0);
	m_app.SetValue(tnRequiredPointsForSlam,m_nMaxSmallSlamPts,1,1);
	m_app.SetValue(tnRequiredPointsForSlam,m_nMinGrandSlamPts,2,0);
	m_app.SetValue(tnRequiredPointsForSlam,m_nMaxGrandSlamPts,2,1);
}

