//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

#if !defined(AFX_TESTPLAYDIALOG_H__D24618E1_4281_11D3_BDC6_444553540000__INCLUDED_)
#define AFX_TESTPLAYDIALOG_H__D24618E1_4281_11D3_BDC6_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestPlayDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTestPlayDialog dialog

class CTestPlayDialog : public CDialog
{
// Construction
public:
	CTestPlayDialog(CWnd* pParent = NULL);   // standard constructor

//
private:
	void	Update();

// Dialog Data
private:
	BOOL	m_bPlayActive;
	BOOL	m_bStopFlag;
	//
	int		m_numMade[8][8], m_numContracts[8][8];


//
public:
	//{{AFX_DATA(CTestPlayDialog)
	enum { IDD = IDD_TEST_PLAY };
	CListCtrl	m_listResults;
	CString	m_strStatus;
	CString	m_strPercentMade;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestPlayDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTestPlayDialog)
	afx_msg void OnStart();
	afx_msg void OnStop();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTPLAYDIALOG_H__D24618E1_4281_11D3_BDC6_444553540000__INCLUDED_)
