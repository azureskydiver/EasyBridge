//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// StatusPlayPlanPage.h : header file
//
class CMyToolTipWnd;

/////////////////////////////////////////////////////////////////////////////
// CStatusPlayPlanPage dialog

class CStatusPlayPlanPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CStatusPlayPlanPage)

// Construction
public:
	CStatusPlayPlanPage();
	~CStatusPlayPlanPage();

public:
	void Clear();
	void Update();
	void UpdateToolTipText(CPoint point);
 
private:
	CMyToolTipWnd*	m_pToolTip;
//	CToolTipCtrl	m_toolTip;
	int				m_nPrevTooltipIndex;
	BOOL	m_bInitialized;
	int		m_nInitialX, m_nInitialY;
	int		m_nInitialXMargin, m_nInitialYMargin;

// Dialog Data
	//{{AFX_DATA(CStatusPlayPlanPage)
	enum { IDD = IDP_STATUS_PLAY_PLAN };
	CListCtrl	m_listPlayPlan;
	CString	m_strDeclarer;
	CString	m_strNumPlays;
	CString	m_strCurrentPlay;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStatusPlayPlanPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CStatusPlayPlanPage)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
