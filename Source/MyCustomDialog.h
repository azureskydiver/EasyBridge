//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// MyCustomDialog.h : header file
//
// - a value-added dialog base class
//

#include "DialogInfo.h"
class CMyToolTipWnd;


/////////////////////////////////////////////////////////////////////////////
// CMyCustomDialog dialog

class CMyCustomDialog : public CDialog
{
// Construction
public:
	CMyCustomDialog(const int nID, const DialogControlInfo* pControlInfo, const int numControls, const int numButtons, CWnd* pParent = NULL);   // standard constructor
	//
	virtual void	Initialize();
	BOOL			OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);

//
protected:
	//
	virtual void	UpdateToolTipText(CPoint point);
	//
	void	EnableControl(int nIndex, BOOL bEnable=TRUE, BOOL bSetImage=TRUE);
	void	EnableControlByID(int nControlID, BOOL bEnable=TRUE, BOOL bSetImage=TRUE);


// data
protected:
	BOOL				m_bInitialized;
	DialogControlInfo*	m_pControlInfo;
	CIntMap				m_mapIDtoIndex;
	int					m_numControls;
	int					m_numButtons;
	CMyToolTipWnd*		m_pToolTip;
	int					m_nPrevTooltipIndex;
	//
	CTypedPtrArray<CPtrArray, HICON*>	m_buttonIcons;
	//
	CListCtrl*			m_pListControl;


public:
// Dialog Data
	//{{AFX_DATA(CMyCustomDialog)
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyCustomDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMyCustomDialog)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
