//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// StatusCardLocationsPage.h : header file
//
#ifndef __STATUS_CARD_LOCATIONS__
#define __STATUS_CARD_LOCATIONS__


/////////////////////////////////////////////////////////////////////////////
// CStatusCardLocationsPage dialog

class CStatusCardLocationsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CStatusCardLocationsPage)

// Construction
public:
	CStatusCardLocationsPage();
	~CStatusCardLocationsPage();

public:
	void Update();
	void Clear();

private:
	BOOL		m_bInitialized;
	int			m_nInitialX, m_nInitialY;
	int			m_nInitialXMargin, m_nInitialYMargin;

// Dialog Data
	//{{AFX_DATA(CStatusCardLocationsPage)
	enum { IDD = IDP_STATUS_CARD_LOCATIONS };
	CComboBox	m_cbTarget;
	CComboBox	m_cbObserver;
	CListCtrl	m_listHoldings;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStatusCardLocationsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CStatusCardLocationsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeComboObserver();
	afx_msg void OnSelchangeComboTarget();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


#endif
