//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

#if !defined(AFX_FILEPROPERTIESDIALOG_H__D43F8221_54A3_11D2_BDC6_444553540000__INCLUDED_)
#define AFX_FILEPROPERTIESDIALOG_H__D43F8221_54A3_11D2_BDC6_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FilePropertiesDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFilePropertiesDialog dialog

class CFilePropertiesDialog : public CDialog
{
// Construction
public:
	CFilePropertiesDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFilePropertiesDialog)
	enum { IDD = IDD_FILE_PROPERTIES };
	CString	m_strDealNumber;
	CString	m_strFileFormat;
	CString	m_strFileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilePropertiesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilePropertiesDialog)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEPROPERTIESDIALOG_H__D43F8221_54A3_11D2_BDC6_444553540000__INCLUDED_)
