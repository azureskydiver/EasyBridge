//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// anlysdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnalysisDialog dialog

class CAnalysisDialog : public CDialog
{
// Construction
public:
	CAnalysisDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAnalysisDialog)
	enum { IDD = IDD_ANALYSIS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
public:
	int		m_nPlayer;
	HICON 	m_hIcon;
	BOOL 	m_bShow;
	CFont*	m_pFont;
	RECT	m_rect;
	WINDOWPLACEMENT	m_placement;

private:
	CRichEditCtrl*	m_pEdit;

public:
	void Clear();
	void SetText(LPCTSTR szText);
	void ScrollToBottom();
	void SizeTextBox();

private:
	void SetFont();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnalysisDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

protected:

	// Generated message map functions
	//{{AFX_MSG(CAnalysisDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
