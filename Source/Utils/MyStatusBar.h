//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// MyStatusBar.h : header file
//
#ifndef __MY_STATUS_BAR__
#define __MY_STATUS_BAR__


/////////////////////////////////////////////////////////////////////////////
// CMyStatusBar window

class AFX_EXT_CLASS CMyStatusBar : public CStatusBar
{
// Construction
public:
	CMyStatusBar();

// Attributes
public:
	BOOL	m_bIgnoreSetText;

// Operations
public:
	void LockFirstPane();
	void UnlockFirstPane();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyStatusBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyStatusBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyStatusBar)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg LRESULT OnSetText(WPARAM, LPARAM); 
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////


#endif