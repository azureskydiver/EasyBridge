//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

#if !defined(AFX_DEALNUMBERDIALOG_H__9CAE64C5_54D4_11D2_8FA4_00609777FAF1__INCLUDED_)
#define AFX_DEALNUMBERDIALOG_H__9CAE64C5_54D4_11D2_8FA4_00609777FAF1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DealNumberDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDealNumberDialog dialog

class CDealNumberDialog : public CDialog
{
// Construction
public:
	CDealNumberDialog(CWnd* pParent = NULL);   // standard constructor

public:
	int		m_nDealNumber;
	int		m_nDealer;
	int		m_nVulnerability;
	int		m_nSpecialDealCode;


// Dialog Data
	//{{AFX_DATA(CDealNumberDialog)
	enum { IDD = IDD_DEAL_NUMBER };
	CString	m_strDealNumber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDealNumberDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDealNumberDialog)
	afx_msg void OnOk();
	virtual void OnCancel();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEALNUMBERDIALOG_H__9CAE64C5_54D4_11D2_8FA4_00609777FAF1__INCLUDED_)
