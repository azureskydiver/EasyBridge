//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

#if !defined(AFX_NNETOUTPUTDIALOG_H__2BF09A44_985B_11D2_BDC6_444553540000__INCLUDED_)
#define AFX_NNETOUTPUTDIALOG_H__2BF09A44_985B_11D2_BDC6_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NNetOutputDialog.h : header file
//
typedef float NVALUE;


/////////////////////////////////////////////////////////////////////////////
// CNNetOutputDialog dialog

class CNNetOutputDialog : public CDialog
{
// public routines
public:
	void ShowResults(NVALUE* fOutputs, int nWinner);
	void ShowError(double fError);
	void Clear();
	void SetTrainingCyclesCount(int nCount) { m_numTrainingCycles = nCount; }


// private data
private:
	BOOL	m_bInitialized;
	double	m_fOutputs[38];
	int		m_nWinner;
	int		m_numTrainingCycles;


// Construction
public:
	CNNetOutputDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNNetOutputDialog)
	enum { IDD = IDD_NEURAL_NET_OUTPUT };
	CString	m_strError;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNNetOutputDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNNetOutputDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NNETOUTPUTDIALOG_H__2BF09A44_985B_11D2_BDC6_444553540000__INCLUDED_)
