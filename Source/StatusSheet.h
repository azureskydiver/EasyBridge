//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// StatusSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStatusSheet

#include "StatusFeedbackPage.h"
#include "StatusAnalysesPage.h"
#include "StatusPlayPlanPage.h"
#include "StatusCardLocationsPage.h"
#include "StatusHoldingsPage.h"
#include "StatusGIBMonitorPage.h"


class CStatusSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CStatusSheet)

// Operations
public:
	void Update(int nPage);
	void Clear();


// Attributes
public:
	CStatusAnalysesPage			m_analysesPage;
	CStatusPlayPlanPage			m_playPlanPage;
	CStatusCardLocationsPage	m_cardLocationsPage;
	CStatusHoldingsPage			m_holdingsPage;
	CStatusGIBMonitorPage		m_gibMonitorPage;
	CStatusFeedbackPage			m_feedbackPage;
	//
	static 	CImageList			m_imageList;


private:
	BOOL	m_bInitialized;
	CRect	m_sheetMargins;

// Construction
public:
	CStatusSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusSheet)
	public:
	virtual BOOL OnInitDialog();
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStatusSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStatusSheet)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
