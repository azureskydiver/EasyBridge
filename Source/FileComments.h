//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// filecmts.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileCommentsDialog dialog

class CFileCommentsDialog : public CDialog
{
// Construction
public:
	CFileCommentsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileCommentsDialog)
	enum { IDD = IDD_FILE_COMMENTS };
	BOOL	m_bAutoShow;
	//}}AFX_DATA

public:
	RECT m_rect;

public:
	void SizeTextBox();
	void UpdateText(BOOL bUpdateVariable);
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileCommentsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileCommentsDialog)
	afx_msg void OnAutoShow();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnMove(int x, int y);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
