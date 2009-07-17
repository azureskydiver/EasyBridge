//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// myfildlg.cpp : implementation file
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "myfildlg.h"
#include "saveopts.h"
#include "progopts.h"
#include "docopts.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMyFileDialog dialog

const int MAX_BUFLEN = 255;
static char szRawBuffer[MAX_BUFLEN+1],szBuffer[MAX_BUFLEN+1];


//CMyFileDialog::CMyFileDialog(CWnd* pParent /*=NULL*/)
//	: CFileDialog(CMyFileDialog::IDD, pParent)
CMyFileDialog::CMyFileDialog(BOOL bOpenFileDialog, 
				  			 LPCTSTR lpszDefExt, 
				  			 LPCTSTR lpszFileName, 
							 DWORD dwFlags, 
							 LPCTSTR lpszFilter, 
				  			 CWnd* pParentWnd)
	: CFileDialog(bOpenFileDialog, 
				  			 lpszDefExt, 
				  			 lpszFileName, 
							 dwFlags, 
							 lpszFilter, 
				  			 pParentWnd)
{
	//{{AFX_DATA_INIT(CMyFileDialog)
	//}}AFX_DATA_INIT
	
	// customize the dialog appropriately
	m_ofn.Flags |= OFN_ENABLETEMPLATE;
	if (theApp.GetValue(tnWindowsMajorVersion) < 4)
	{
		// old-style customization
		m_ofn.Flags &= ~OFN_EXPLORER;
		m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_FILE_MYOPEN);
	}
	else
	{
		// Win95-style customization
		m_ofn.Flags |= OFN_EXPLORER;
		m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_COMMOPEN_EXTENSION);
	}
	if (bOpenFileDialog)
		m_bIsSaving = FALSE;
	else
		m_bIsSaving = TRUE;
}


void CMyFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyFileDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMyFileDialog, CFileDialog)
	//{{AFX_MSG_MAP(CMyFileDialog)
	ON_EN_CHANGE(IDC_FILE_DESCRIPTION, OnChangeFileDescription)
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMyFileDialog message handlers


//
BOOL CMyFileDialog::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_FILE_DESCRIPTION);
	if (m_bIsSaving) 
	{
		ASSERT(pEdit != NULL);
		pEdit->SetReadOnly(FALSE);
		pEdit->LimitText(255);
		pEdit->SetWindowText((LPCTSTR)pDOC->GetValuePV(tstrFileDescription));
	} 
	else 
	{
		pEdit->SetReadOnly(TRUE);
		CWnd* pWnd = GetDlgItem(IDC_OPTIONS);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



const int LISTBOX_FILENAMES	= 1120;
const int STATIC_FILENAME	= 1088;

//
// OnLBSelChangedNotify()
//
// use with old-style common dialogs
//
void CMyFileDialog::OnLBSelChangedNotify(UINT nIDBox, UINT iCurSel, UINT nCode)
{
	CString strFileName;
	//
	if ((nIDBox == LISTBOX_FILENAMES) && (nCode == CD_LBSELCHANGE)) 
	{
		// file selected
		if (!m_bIsSaving)
		{
			CListBox* pListBox = (CListBox*) GetDlgItem(LISTBOX_FILENAMES);
			pListBox->GetText(iCurSel,strFileName);
			GetFileDescription(strFileName);
		}
	} 
	else 
	{
		// call default routine
		CFileDialog::OnLBSelChangedNotify(nIDBox, iCurSel, nCode);
	}
}


//
// OnFileNameChange()
//
// use with Win95 (Explorer) style common dialogs
//
void CMyFileDialog::OnFileNameChange()
{
	if (!m_bIsSaving)
	{
		CString strFileName = GetPathName();
		GetFileDescription(strFileName);
	}
}

/*
//
// OnTypeChange()
//
void CMyFileDialog::OnTypeChange()
{
	if 
}
*/

//
void CMyFileDialog::GetFileDescription(CString& strFileName) 
{
	CStdioFile file;
	CFileException fileException;
	char* pszRtn;
	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_FILE_DESCRIPTION);
	CString strDescription;
	//
	pEdit->SetWindowText("");
	//
	int nCode = file.Open(strFileName, 
				  		  CFile::modeRead | CFile::shareDenyWrite, 
						  &fileException);
	// return if error
	if (nCode == 0)
		return;

	// else check file type 
	CString strSuffix = strFileName.Mid(strFileName.GetLength()-3);
	CString strKey;
	if (strSuffix.CompareNoCase("brd") == 0)
	{
		m_nFileType = CEasyBDoc::tnEasyBridgeFormat;
		strKey = "[[File Description]]";
	}
	else if(strSuffix.CompareNoCase("ppl") == 0)    // NCR adding PPL
	{
		m_nFileType = CEasyBDoc::tnPPLFormat;
		strKey = "";  // NCR None, file is binary
		file.Close();
		return;			// exit early and hope for the best
	}
	else
	{
		m_nFileType = CEasyBDoc::tnPBNFormat;
		strKey = "[Description ";
	}

	// and scan for the file description block
	do 
	{
		pszRtn = file.ReadString(szRawBuffer,MAX_BUFLEN);
	} while	((pszRtn) && (strncmp(szRawBuffer, (LPCTSTR)strKey, strKey.GetLength()) != 0));

	// check for EOF
	if (pszRtn ==NULL) 
	{
		file.Close();
		return;
	}

	// else read file description
	int nEnd;
	if (m_nFileType == CEasyBDoc::tnEasyBridgeFormat)
	{
		BOOL bBreak = FALSE;
		do {
			pszRtn = file.ReadString(szRawBuffer,MAX_BUFLEN);
			if ((pszRtn) && (pszRtn[0] != '\0') && (strncmp(pszRtn,"[[",2))) 
			{
				strDescription += pszRtn;
				// replace the \n with \r\n
				nEnd = strDescription.GetLength() - 1;
				if (strDescription[nEnd] == '\n')
				{
					strDescription.SetAt(nEnd,'\r');
					strDescription += '\n';
				}
			}
			else
			{
				bBreak = TRUE;
			}
		} while	(!bBreak);

		// trim ending CR/LF
		int nLen = strDescription.GetLength();
		if ((nLen >= 2) && (strDescription[nLen-1] == _T('\n')))
			strDescription = strDescription.Left(nLen-2);
	}
	else
	{
		strDescription = (char*)(&szRawBuffer[14]);
		// trim end of line chars
		strDescription = strDescription.Left(strDescription.GetLength() - 4);
	}

	// done reading
	file.Close();
	pEdit->SetWindowText((LPCTSTR)strDescription);
}



//
void CMyFileDialog::OnChangeFileDescription() 
{
	//
	CString strTemp;
	char* pszBuf = strTemp.GetBuffer(256);
	GetDlgItemText(IDC_FILE_DESCRIPTION,pszBuf,255);
	strTemp.ReleaseBuffer();	
	pDOC->SetValue(tstrFileDescription,(LPCTSTR)strTemp);
}


//
void CMyFileDialog::OnOptions() 
{
	CFileSaveOptionsDlg	optionsDlg;
	//
	optionsDlg.DoModal();	
}



BOOL CMyFileDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{			  
	OFNOTIFY* lpon = (LPOFNOTIFY) lParam; 
	if (lpon->hdr.code == CDN_TYPECHANGE)
	{
		LPOPENFILENAME lpOFN = lpon->lpOFN;
		if (lpOFN->nFilterIndex == 3)
			m_nFileType = CEasyBDoc::tnTextFormat;
		else if (lpOFN->nFilterIndex == 2)
			m_nFileType = CEasyBDoc::tnPBNFormat;
		else if(lpOFN->nFilterIndex == 4)    // NCR adding PLL
			m_nFileType = CEasyBDoc::tnPPLFormat;
		else
			m_nFileType = CEasyBDoc::tnEasyBridgeFormat;
		return TRUE;
	}
	//
	if (m_bIsSaving)
	{
		if (m_nFileType == CEasyBDoc::tnTextFormat)
			GetDlgItem(IDC_FILE_DESCRIPTION)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_FILE_DESCRIPTION)->EnableWindow(TRUE);
		//
		if (m_nFileType != CEasyBDoc::tnEasyBridgeFormat)
			GetDlgItem(IDC_OPTIONS)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_OPTIONS)->EnableWindow(TRUE);
	}
	//
	return CFileDialog::OnNotify(wParam, lParam, pResult);
}
