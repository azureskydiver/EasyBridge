//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// MyToolTipWnd.h : header file
//
#ifndef __MYTOOLTIPWND__
#define __MYTOOLTIPWND__

/////////////////////////////////////////////////////////////////////////////
// CMyToolTipWnd window

class AFX_EXT_CLASS CMyToolTipWnd : public CWnd
{
// Construction
public:
	CMyToolTipWnd();

// Attributes
public:
	BOOL Create(CWnd* pParentWnd);
	BOOL SetText(LPCTSTR szText, CPoint* pPoint=NULL);
	int	 ListControlHitTest(CPoint point, CListCtrl& listCtl, int nColumn=-1);
	void Show() { ShowWindow(SW_SHOWNOACTIVATE); }
	void Hide() { ShowWindow(SW_HIDE); }

// Operations
public:

private:
	HFONT	m_hFont;
	CString	m_strText;
	CPoint	m_location;
	CWnd*	m_pParentWnd;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyToolTipWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyToolTipWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyToolTipWnd)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
