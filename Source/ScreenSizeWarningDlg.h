//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

#if !defined(AFX_SCREENSIZEWARNINGDLG_H__A94D9181_8730_11D4_BDC7_44F1FFC00000__INCLUDED_)
#define AFX_SCREENSIZEWARNINGDLG_H__A94D9181_8730_11D4_BDC7_44F1FFC00000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScreenSizeWarningDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScreenSizeWarningDlg dialog

class CScreenSizeWarningDlg : public CDialog
{
// Construction
public:
	CScreenSizeWarningDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScreenSizeWarningDlg)
	enum { IDD = IDD_SCREEN_TOO_SMALL };
	BOOL	m_bDontShowDialog;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScreenSizeWarningDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScreenSizeWarningDlg)
	afx_msg void OnClose();
	afx_msg void OnTellMeMore();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCREENSIZEWARNINGDLG_H__A94D9181_8730_11D4_BDC7_44F1FFC00000__INCLUDED_)
