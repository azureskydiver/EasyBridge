//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// cdlayout.h : header file
//
#include "FlatButton.h"


/////////////////////////////////////////////////////////////////////////////
// CCardLayoutDialog dialog

class CCardLayoutDialog : public CDialog
{
// Construction
public:
	CCardLayoutDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCardLayoutDialog)
	enum { IDD = IDD_CARD_LAYOUT };
	int		m_nPlayer;
	BOOL	m_bUseSuitDisplayOrder;
	//}}AFX_DATA

public:
	int			m_nCardLimit;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardLayoutDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	void Reset(BOOL bForceReset=FALSE);
	void EnableOrDisable(BOOL bForceReset=FALSE);
	void ForceRefresh();

protected:

	// Generated message map functions
	//{{AFX_MSG(CCardLayoutDialog)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClear();
	afx_msg void OnClose();
	afx_msg void OnPaint();
	virtual void OnOK();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual BOOL OnInitDialog();
	afx_msg void OnDisplayOrder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
