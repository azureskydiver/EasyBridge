//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// saveopts.cpp : implementation file
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "docopts.h"
#include "saveopts.h"
#include "progopts.h"
#include "Help\HelpCode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileSaveOptionsDlg dialog


CFileSaveOptionsDlg::CFileSaveOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileSaveOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileSaveOptionsDlg)
	m_bShowComments = FALSE;
	m_bShowBiddingHistory = FALSE;
	m_bShowPlayHistory = FALSE;
	m_bShowAnalyses = FALSE;
	m_bSaveWestAnalysis = FALSE;
	m_bSaveEastAnalysis = FALSE;
	m_bSaveSouthAnalysis = FALSE;
	m_bSaveNorthAnalysis = FALSE;
	m_bSaveIntermediatePositions = FALSE;
	//}}AFX_DATA_INIT
	CEasyBDoc* pDoc = CEasyBDoc::GetDoc();
	m_bShowComments = pDoc->GetValue(tbShowCommentsUponOpen);
	m_bShowBiddingHistory = pDoc->GetValue(tbShowBidHistoryUponOpen);
	m_bShowPlayHistory = pDoc->GetValue(tbShowPlayHistoryUponOpen);
	m_bShowAnalyses = pDoc->GetValue(tbShowAnalysesUponOpen);
	m_bSaveSouthAnalysis = pDoc->GetValue(tbSavePlayerAnalysis, SOUTH);
	m_bSaveWestAnalysis = pDoc->GetValue(tbSavePlayerAnalysis, WEST);
	m_bSaveNorthAnalysis = pDoc->GetValue(tbSavePlayerAnalysis, NORTH);
	m_bSaveEastAnalysis = pDoc->GetValue(tbSavePlayerAnalysis, EAST);
	m_bSaveIntermediatePositions = pDoc->GetValue(tbDocSaveIntermediatePositions);
}


void CFileSaveOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileSaveOptionsDlg)
	DDX_Check(pDX, IDC_SHOW_FILE_COMMENTS, m_bShowComments);
	DDX_Check(pDX, IDC_SHOW_BID_HISTORY, m_bShowBiddingHistory);
	DDX_Check(pDX, IDC_SHOW_PLAY_HISTORY, m_bShowPlayHistory);
	DDX_Check(pDX, IDC_CHECK4, m_bShowAnalyses);
	DDX_Check(pDX, IDC_CHECK5, m_bSaveWestAnalysis);
	DDX_Check(pDX, IDC_CHECK7, m_bSaveEastAnalysis);
	DDX_Check(pDX, IDC_CHECK8, m_bSaveSouthAnalysis);
	DDX_Check(pDX, IDC_CHECK6, m_bSaveNorthAnalysis);
	DDX_Check(pDX, IDC_SAVE_POSITIONS, m_bSaveIntermediatePositions);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileSaveOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CFileSaveOptionsDlg)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFileSaveOptionsDlg message handlers

void CFileSaveOptionsDlg::OnOk() 
{
	UpdateData(TRUE);
	CEasyBDoc* pDoc = CEasyBDoc::GetDoc();
	pDoc->SetValue(tbShowCommentsUponOpen, m_bShowComments);
	pDoc->SetValue(tbShowBidHistoryUponOpen, m_bShowBiddingHistory);
	pDoc->SetValue(tbShowPlayHistoryUponOpen, m_bShowPlayHistory);
	pDoc->SetValue(tbShowAnalysesUponOpen, m_bShowAnalyses);
	pDoc->SetValue(tbSavePlayerAnalysis, m_bSaveSouthAnalysis, SOUTH);
	pDoc->SetValue(tbSavePlayerAnalysis, m_bSaveWestAnalysis, WEST);
	pDoc->SetValue(tbSavePlayerAnalysis, m_bSaveNorthAnalysis, NORTH);
	pDoc->SetValue(tbSavePlayerAnalysis, m_bSaveEastAnalysis, EAST);
	pDoc->SetValue(tbDocSaveIntermediatePositions, m_bSaveIntermediatePositions, EAST);
	EndDialog(TRUE);	
}

//
void CFileSaveOptionsDlg::OnCancel() 
{
	EndDialog(FALSE);	
//	CDialog::OnCancel();
}

//
void CFileSaveOptionsDlg::OnHelp() 
{
	WinHelp(DIALOG_FILE_SAVE_OPTIONS);
}

//
BOOL CFileSaveOptionsDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

