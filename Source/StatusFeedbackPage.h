//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// StatusFeedbackPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStatusFeedbackPage dialog

class CStatusFeedbackPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CStatusFeedbackPage)

// Construction
public:
	CStatusFeedbackPage();
	~CStatusFeedbackPage();

//
public:
	void Update();
	void Clear();
	BOOL IsInitialized() { return m_bInitialized; }

private:
	BOOL m_bInitialized;

// Dialog Data
	//{{AFX_DATA(CStatusFeedbackPage)
	enum { IDD = IDP_STATUS_FEEDBACK };
	CString	m_strText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStatusFeedbackPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CStatusFeedbackPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
