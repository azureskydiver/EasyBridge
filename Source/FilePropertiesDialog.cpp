//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// FilePropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "FilePropertiesDialog.h"
#include "help\helpcode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilePropertiesDialog dialog


CFilePropertiesDialog::CFilePropertiesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFilePropertiesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilePropertiesDialog)
	m_strDealNumber = _T("");
	m_strFileFormat = _T("");
	m_strFileName = _T("");
	//}}AFX_DATA_INIT
}


void CFilePropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilePropertiesDialog)
	DDX_Text(pDX, IDC_DEAL_NUMBER, m_strDealNumber);
	DDX_Text(pDX, IDC_FILE_FORMAT, m_strFileFormat);
	DDX_Text(pDX, IDC_FILENAME, m_strFileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilePropertiesDialog, CDialog)
	//{{AFX_MSG_MAP(CFilePropertiesDialog)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFilePropertiesDialog message handlers



//
BOOL CFilePropertiesDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_DEAL_NUMBER);
	pEdit->SetFocus();
	pEdit->SetSel(0, -1);
	
	// 
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




//
BOOL CFilePropertiesDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	WinHelp(DIALOG_FILE_PROPERTIES);
	return TRUE;
}



