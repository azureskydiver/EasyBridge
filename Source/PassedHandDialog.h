//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

#if !defined(AFX_PASSEDHANDDIALOG_H__C232C1A1_3450_11D2_BDC6_444553540000__INCLUDED_)
#define AFX_PASSEDHANDDIALOG_H__C232C1A1_3450_11D2_BDC6_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PassedHandDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPassedHandDialog dialog

class CPassedHandDialog : public CDialog
{
// Construction
public:
	CPassedHandDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPassedHandDialog)
	enum { IDD = IDD_PASSED_HAND };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPassedHandDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPassedHandDialog)
	virtual void OnOK();
	afx_msg void OnRebidHand();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSEDHANDDIALOG_H__C232C1A1_3450_11D2_BDC6_444553540000__INCLUDED_)
