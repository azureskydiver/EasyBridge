//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// HistoryWnd.h : header file
//
class CEditDialog;

/////////////////////////////////////////////////////////////////////////////
// CHistoryWnd frame

class CHistoryWnd : public CCJControlBar
{
	friend class CMainFrame;

//	DECLARE_DYNCREATE(CHistoryWnd)
protected:
	CHistoryWnd();           // protected constructor used by dynamic creation


// Operations
public:
	//
	void ClearBiddingHistory() { m_editBidding.SetWindowText(""); }
	void ClearPlayHistory() { m_editPlay.SetWindowText(""); }
	void ClearAll() { ClearBiddingHistory(); ClearPlayHistory(); }
	//
	void SetBiddingHistory(LPCTSTR szText);
	void SetPlayHistory(LPCTSTR szText);
	void ShowBiddingHistory(BOOL bShow=TRUE);
	void ShowPlayHistory(BOOL bShow=TRUE);

// internal routines
protected:
	void ScrollToEnd(CRichEditCtrl& edit, int numVisibleRows);
	void ReplaceSuitSymbols(CRichEditCtrl& edit);
	void ResizeControls();


// Data
public:
	int		m_nRows;
	int		m_nCols;

private:	
	CMenu			m_menuPopup;
	int 			m_nWndType;
	int				m_nFontHeight;
	int				m_nCurrentChild;
	int				m_numVisibleBiddingRows, m_numVisiblePlayRows;
	CRichEditCtrl	m_editBidding, m_editPlay;
	CStatic			m_lblBidding, m_lblPlay;
	CButton			m_btnBidding, m_btnPlay;
	BOOL			m_bShowBidding, m_bShowPlay;
	bool			m_bInitialized;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistoryWnd)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CHistoryWnd();

	// Generated message map functions
	//{{AFX_MSG(CHistoryWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMove(int x, int y);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnClickWindowShow();
	afx_msg void OnHide();
	afx_msg void OnUpdateMenuItem(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
