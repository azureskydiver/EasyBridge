//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CStatusWnd.h
//
#ifndef __STATUSWND__
#define __STATUSWND__

class CStatusSheet;

//
class CStatusWnd : public CCJControlBar
{
	friend class CMainFrame;

// enums
public:
	enum { 
		SP_ANALYSES			= 0x01,
		SP_CARD_LOCATIONS	= 0x02,
		SP_PLAY_PLAN		= 0x04,
		SP_SUIT_STATUS		= 0x08,
		SP_GIB_MONITOR		= 0x10,
		SP_FEEDBACK			= 0x20,
		SP_ALL				= 0x3F, 
	} Pages;

//
public:
	void Update(int nCode=SP_ALL);
	void Clear();
	void SetActiveTab(int nTab);
	BOOL IsTabActive(int nTab);

	
// Data
public:
	static CStatusWnd*	m_pWnd;

private:
	CStatusSheet*	m_pStatusSheet;
	BOOL			m_bInitialized;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusWnd)
    public:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL


// Implementation
public:
	CStatusWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStatusWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif 
