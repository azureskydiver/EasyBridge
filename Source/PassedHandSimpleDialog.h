//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

#if !defined(AFX_PASSEDHANDSIMPLEDIALOG_H__5A68B3E1_9D1D_11D3_BDC6_444553540000__INCLUDED_)
#define AFX_PASSEDHANDSIMPLEDIALOG_H__5A68B3E1_9D1D_11D3_BDC6_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PassedHandSimpleDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPassedHandSimpleDialog dialog

class CPassedHandSimpleDialog : public CDialog
{
// Construction
public:
	CPassedHandSimpleDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPassedHandSimpleDialog)
	enum { IDD = IDD_PASSED_HAND_SIMPLE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPassedHandSimpleDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPassedHandSimpleDialog)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSEDHANDSIMPLEDIALOG_H__5A68B3E1_9D1D_11D3_BDC6_444553540000__INCLUDED_)
