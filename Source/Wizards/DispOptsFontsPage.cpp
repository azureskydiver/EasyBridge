//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DispOptsFontsPage.cpp : implementation file
//

#include "stdafx.h"
#include "ObjectWithProperties.h"
#include "DispOptsFontsPage.h"
#include "MainFrameOpts.h"
#include "progopts.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDispOptsFontsPage property page

IMPLEMENT_DYNCREATE(CDispOptsFontsPage, CPropertyPage)

CDispOptsFontsPage::CDispOptsFontsPage(CObjectWithProperties* pApp, CObjectWithProperties* pFrame, CObjectWithProperties* pView) : 
		CPropertyPage(CDispOptsFontsPage::IDD),
		m_app(*pApp), m_frame(*pFrame), m_view(*pView) 
{
	//{{AFX_DATA_INIT(CDispOptsFontsPage)
	m_strAutoHintDialogFont = _T("");
	m_strHistoryDialogFont = _T("");
	m_strAnalysisDialogFont = _T("");
	m_bUseSuitSymbols = FALSE;
	//}}AFX_DATA_INIT

	// copy logical font info
	m_lfAutoHint = *((LOGFONT*) m_frame.GetValuePV(tpLogFontAutoHint));
	m_lfHistory = *((LOGFONT*) m_frame.GetValuePV(tpLogFontHistory));
	m_lfAnalysis = *((LOGFONT*) m_frame.GetValue(tpLogFontAnalysis));
	m_bUseSuitSymbols = (BOOL) m_app.GetValue(tbUseSuitSymbols);
	//
	m_bFontModified = false;
	m_bWarned = false;
}

CDispOptsFontsPage::~CDispOptsFontsPage()
{
}

void CDispOptsFontsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDispOptsFontsPage)
	DDX_Text(pDX, IDC_AUTOHINT_DIALOG_FONT, m_strAutoHintDialogFont);
	DDX_Text(pDX, IDC_HISTORY_DIALOG_FONT, m_strHistoryDialogFont);
	DDX_Text(pDX, IDC_ANALYSIS_DIALOG_FONT, m_strAnalysisDialogFont);
	DDX_Check(pDX, IDC_USE_SUIT_SYMBOLS, m_bUseSuitSymbols);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDispOptsFontsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDispOptsFontsPage)
	ON_BN_CLICKED(IDC_SET_AUTOHINT_DIALOG_FONT, OnSetAutohintDialogFont)
	ON_BN_CLICKED(IDC_SET_HISTORY_DIALOG_FONT, OnSetHistoryDialogFont)
	ON_BN_CLICKED(IDC_SET_ANALYSIS_DIALOG_FONT, OnSetAnalysisDialogFont)
	ON_BN_CLICKED(IDC_RESET_AUTOHINT_DIALOG_FONT, OnResetAutohintDialogFont)
	ON_BN_CLICKED(IDC_RESET_HISTORY_DIALOG_FONT, OnResetHistoryDialogFont)
	ON_BN_CLICKED(IDC_RESET_ANALYSIS_DIALOG_FONT, OnResetAnalysisDialogFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDispOptsFontsPage message handlers


//
BOOL CDispOptsFontsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	//
	UpdateFontNames();
	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CDispOptsFontsPage::OnSetAutohintDialogFont() 
{
	CFontDialog dlg(&m_lfAutoHint, CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT);
	if (dlg.DoModal() == IDOK)
	{
		m_bFontModified = true;
		UpdateFontNames();
		if (!m_bWarned)
		{
			AfxMessageBox(_T("The changes you make here will not take effect until you restart the program."));
			m_bWarned = true;
		}
	}
}

//
void CDispOptsFontsPage::OnSetHistoryDialogFont() 
{
	CFontDialog dlg(&m_lfHistory, CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT);
	if (dlg.DoModal() == IDOK)
	{
		m_bFontModified = true;
		UpdateFontNames();
		if (!m_bWarned)
		{
			AfxMessageBox(_T("The changes you make here will not take effect until you restart the program."));
			m_bWarned = true;
		}
	}
}

//
void CDispOptsFontsPage::OnSetAnalysisDialogFont() 
{
	CFontDialog dlg(&m_lfAnalysis, CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT);
	if (dlg.DoModal() == IDOK)
	{
		m_bFontModified = true;
		UpdateFontNames();
		if (!m_bWarned)
		{
			AfxMessageBox(_T("The changes you make here will not take effect until you restart the program."));
			m_bWarned = true;
		}
	}
}


//
void CDispOptsFontsPage::OnResetAutohintDialogFont() 
{
	m_lfAutoHint.lfFaceName[0] = _T('\0');
	m_bFontModified = true;
	UpdateFontNames();
	if (!m_bWarned)
	{
		AfxMessageBox(_T("The changes you make here will not take effect until you restart the program."));
		m_bWarned = true;
	}
}


//
void CDispOptsFontsPage::OnResetHistoryDialogFont() 
{
	m_lfHistory.lfFaceName[0] = _T('\0');
	m_bFontModified = true;
	UpdateFontNames();
	if (!m_bWarned)
	{
		AfxMessageBox(_T("The changes you make here will not take effect until you restart the program."));
		m_bWarned = true;
	}
}


//
void CDispOptsFontsPage::OnResetAnalysisDialogFont() 
{
	m_lfAnalysis.lfFaceName[0] = _T('\0');
	m_bFontModified = true;
	UpdateFontNames();
	if (!m_bWarned)
	{
		AfxMessageBox(_T("The changes you make here will not take effect until you restart the program."));
		m_bWarned = true;
	}
}


//
void CDispOptsFontsPage::UpdateFontNames()
{
	if (m_lfAutoHint.lfFaceName[0] != _T('\0'))
		m_strAutoHintDialogFont.Format(_T("%s %d point"), m_lfAutoHint.lfFaceName, LogicalSizeToPoints(m_lfAutoHint.lfHeight));
	else
		m_strAutoHintDialogFont = _T("System Default");
	//
	if (m_lfHistory.lfFaceName[0] != _T('\0'))
		m_strHistoryDialogFont.Format(_T("%s %d point"), m_lfHistory.lfFaceName, LogicalSizeToPoints(m_lfHistory.lfHeight));
	else
		m_strHistoryDialogFont = _T("System Default");
	//
	if (m_lfAnalysis.lfFaceName[0] != _T('\0'))
		m_strAnalysisDialogFont.Format(_T("%s %d point"), m_lfAnalysis.lfFaceName, LogicalSizeToPoints(m_lfAnalysis.lfHeight));
	else
		m_strAnalysisDialogFont = _T("System Default");
	//
	UpdateData(FALSE);
}


//
int CDispOptsFontsPage::LogicalSizeToPoints(int nLogSize)
{
	CWindowDC dc(this);
	int nPoints = Round((Abs(nLogSize) / (float)::GetDeviceCaps(dc, LOGPIXELSY)) * 72);
	return nPoints;
}

//
BOOL CDispOptsFontsPage::Update()
{
	// store results & return if changes affect the display
	m_frame.SetValuePV(tpLogFontAutoHint, &m_lfAutoHint);
	m_frame.SetValuePV(tpLogFontHistory,  &m_lfHistory);
	m_frame.SetValuePV(tpLogFontAnalysis, &m_lfAnalysis);
	m_app.SetValue(tbUseSuitSymbols, m_bUseSuitSymbols);
	return m_bFontModified;
}


