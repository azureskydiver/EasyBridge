//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// SelectHandDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectHandDialog dialog

class CSelectHandDialog : public CDialog
{
// Construction
public:
	CSelectHandDialog(CWnd* pParent = NULL);   // standard constructor
	void CollapseWindow(BOOL bCollapse=TRUE);
	BOOL IsCollapsed() { return m_bCollapsed; }

private:
	void ScrollToBottom(CEdit& edit);


public:
	enum { SH_MODE_HAND, SH_MODE_DEALER };
	int		 m_nMode;
	BOOL	 m_bCollapsed;
	CString	 m_strTitle;

private:
	int		m_nCollapsedHeight;
	int		m_nFullHeight;

public:
// Dialog Data
	//{{AFX_DATA(CSelectHandDialog)
	enum { IDD = IDD_SELECT_HAND };
	int		m_nPosition;
	int		m_nVulnerability;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectHandDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectHandDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelectPosition();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
