//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// GameOptsGIBPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ObjectWithProperties.h"
#include "GameOptsGIBPage.h"
#include "progopts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameOptsGIBPage property page

IMPLEMENT_DYNCREATE(CGameOptsGIBPage, CPropertyPage)

CGameOptsGIBPage::CGameOptsGIBPage(CObjectWithProperties* pApp) : 
		CPropertyPage(CGameOptsGIBPage::IDD),
		m_app(*pApp)
{
	//{{AFX_DATA_INIT(CGameOptsGIBPage)
	m_nAnalysisTime = 0;
	m_strGIBPath = _T("");
	m_bEnableGIBDefenderPlay = FALSE;
	m_bEnableGIBDeclarerPlay = FALSE;
	//}}AFX_DATA_INIT
	//
	m_strGIBPath = m_app.GetValueString(tszGIBPath);
	m_bEnableGIBDeclarerPlay = m_app.GetValue(tbEnableGIBForDeclarer);
	m_bEnableGIBDefenderPlay = m_app.GetValue(tbEnableGIBForDefender);
	m_nAnalysisTime = m_app.GetValue(tnGIBAnalysisTime);
//	m_nSampleSize = m_app.GetValue(tnGIBSampleSize);
}

CGameOptsGIBPage::~CGameOptsGIBPage()
{
}

void CGameOptsGIBPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGameOptsGIBPage)
	DDX_Control(pDX, IDC_SPIN_ANALYSIS_TIME, m_sbAnalysisTime);
	DDX_Text(pDX, IDC_ANALYSIS_TIME, m_nAnalysisTime);
	DDV_MinMaxInt(pDX, m_nAnalysisTime, 1, 1000);
	DDX_Text(pDX, IDC_EDIT_GIB_PATH, m_strGIBPath);
	DDX_Check(pDX, IDC_ENABLE_GIB_DEFENDER, m_bEnableGIBDefenderPlay);
	DDX_Check(pDX, IDC_ENABLE_GIB, m_bEnableGIBDeclarerPlay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGameOptsGIBPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGameOptsGIBPage)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameOptsGIBPage message handlers


BOOL CGameOptsGIBPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// init the spin button
	m_sbAnalysisTime.SetRange(0, 999);
	m_sbAnalysisTime.SetPos(m_nAnalysisTime);

	//	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CGameOptsGIBPage::OnBrowse() 
{
	// first retrieve the current filename & path
	CString strFilePath = m_strGIBPath;
	char szName[256], szPath[256];
	if (!strFilePath.IsEmpty())
	{
		int nPos = strFilePath.ReverseFind('\\');
		if (nPos >= 0)
		{
			strcpy(szName, strFilePath.Mid(nPos+1));
			strcpy(szPath, strFilePath.Mid(0, nPos));
		}
	}

	// create the file dialog
	CFileDialog fileDlg(TRUE, "bmp", "gib.exe", OF_READ | OFN_HIDEREADONLY,
				  		"Programs Files (*.exe)|*.exe|All Files (*.*)|*.*||",
  						this);
	fileDlg.m_ofn.lpstrTitle = "Find GIB Program";
	fileDlg.m_ofn.lpstrInitialDir = szPath;

	//
	if (fileDlg.DoModal() == IDCANCEL) 
	{
		int nCode = ::CommDlgExtendedError();
		return;
	}

	// get selected filename
	CString strPath = fileDlg.GetPathName();
	if (!strPath.IsEmpty())
	{
		m_strGIBPath = strPath;
		UpdateData(FALSE);
	}
}


//
void CGameOptsGIBPage::Update()
{
	// store results
	m_app.SetValue(tszGIBPath, m_strGIBPath);
	m_app.SetValue(tbEnableGIBForDeclarer, m_bEnableGIBDeclarerPlay);
	m_app.SetValue(tbEnableGIBForDefender, m_bEnableGIBDefenderPlay);
	m_app.SetValue(tnGIBAnalysisTime, m_nAnalysisTime);
//	m_app.SetValue(tnGIBSampleSize, m_nSampleSize);
}


