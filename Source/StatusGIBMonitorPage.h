//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// StatusGIBMonitorPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStatusGIBMonitorPage dialog

class CStatusGIBMonitorPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CStatusGIBMonitorPage)

// Construction
public:
	CStatusGIBMonitorPage();
	~CStatusGIBMonitorPage();

public:
	void Update();
	void Clear();
	BOOL IsInitialized() { return m_bInitialized; }

private:
	void ScrollToEnd();

private:
	CRichEditCtrl	m_edit;
	BOOL			m_bInitialized;

// Dialog Data
	//{{AFX_DATA(CStatusGIBMonitorPage)
	enum { IDD = IDP_STATUS_GIB_MONITOR };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStatusGIBMonitorPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CStatusGIBMonitorPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
