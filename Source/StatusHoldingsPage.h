//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// StatusHoldingsPage.h : header file
//
#ifndef __STATUS_HOLDINGS_PAGE__
#define __STATUS_HOLDINGS_PAGE__


/////////////////////////////////////////////////////////////////////////////
// CStatusHoldingsPage dialog

class CStatusHoldingsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CStatusHoldingsPage)

// Construction
public:
	CStatusHoldingsPage();
	~CStatusHoldingsPage();

public:
	void Clear();
	void Update();
 
private:
	BOOL		m_bInitialized;
	int			m_nInitialX, m_nInitialY;
	int			m_nInitialXMargin, m_nInitialYMargin;

// Dialog Data
	//{{AFX_DATA(CStatusHoldingsPage)
	enum { IDD = IDP_STATUS_HOLDINGS };
	CListCtrl	m_listSuitStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStatusHoldingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CStatusHoldingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif