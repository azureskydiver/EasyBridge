//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
#if !defined(AFX_DAILYTIPDIALOG_H__3923DD44_270E_11D2_8F60_00609777FAF1__INCLUDED_)
#define AFX_DAILYTIPDIALOG_H__3923DD44_270E_11D2_8F60_00609777FAF1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DailyTipDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDailyTipDialog dialog

class AFX_EXT_CLASS CDailyTipDialog : public CDialog
{
// Construction
public:
	CDailyTipDialog();
	
// Operations
public:
	virtual BOOL Create(CWnd* pParentWnd);
	void LoadRandomTip();
	void DrawTip();

public:
	CString		m_strTip;
	int			m_nTipStart, m_nTipEnd, m_nTipsSkip;
	int			m_nTipIndex;
	BOOL		m_bLoadSpecifiedTip;
	CRect		m_rectTip;

// Dialog Data
	//{{AFX_DATA(CDailyTipDialog)
//	enum { IDD = IDD_TIP_OF_THE_DAY };
	CStatic	m_paneMessage;
	BOOL	m_bShowAtStartup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDailyTipDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDailyTipDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnNext();
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnPrev();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DAILYTIPDIALOG_H__3923DD44_270E_11D2_8F60_00609777FAF1__INCLUDED_)
