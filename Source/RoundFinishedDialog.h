//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// RoundFinishedDialog.h : header file
//

#ifndef __ROUNDFINISHED__
#define __ROUNDFINISHED__


/////////////////////////////////////////////////////////////////////////////
// CRoundFinishedDialog dialog

class CRoundFinishedDialog : public CDialog
{
// return codes
public:
	enum { RF_NONE=0, RF_REBID, RF_REPLAY, RF_AUTOPLAY, RF_AUTOPLAY_FULL };

// Construction
public:
	CRoundFinishedDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRoundFinishedDialog)
	enum { IDD = IDD_ROUND_FINISHED };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	void SetMessage(CString strText) { m_strText = strText; }
	void SetMessage(CString strText, CString strOldText) { m_strText = strText; m_strOldText = strOldText; }

public:
	int		m_nCode;
	BOOL	m_bReplayMode;
	BOOL	m_bDisableCancel;

private:
	static CString m_strOldText;
	CString	m_strText;
	BOOL	m_bCollapsed;
	int		m_nFullHeight, m_nCollapsedHeight;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRoundFinishedDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRoundFinishedDialog)
	virtual void OnOK();
	afx_msg void OnReplayHand();
	afx_msg void OnRebidHand();
	virtual BOOL OnInitDialog();
	afx_msg void OnSaveGameRecord();
	afx_msg void OnCancel();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnMore();
	afx_msg void OnComputerReplay();
	afx_msg void OnComputerReplayAuto();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
