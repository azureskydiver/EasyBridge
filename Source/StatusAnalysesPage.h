//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

#if !defined(AFX_STATUSANALYSESPAGE_H__D1ABEA23_8A4A_11D2_8FF6_00609777FAF1__INCLUDED_)
#define AFX_STATUSANALYSESPAGE_H__D1ABEA23_8A4A_11D2_8FF6_00609777FAF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StatusAnalysesPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStatusAnalysesPage dialog

class CStatusAnalysesPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CStatusAnalysesPage)

// Construction
public:
	CStatusAnalysesPage();
	~CStatusAnalysesPage();

public:
	void Update();
	void Clear(int nPosition=-1);
	void SetAnalysisText(int nPosition, LPCTSTR szText);
	void ShowAnalysis(int nPosition=-1, BOOL bShow=TRUE);
	BOOL IsAnalysisShown(int nPosition);
	void SetFont(CFont& font) { m_pFont = &font; }

private:
	void ScrollToEnd(CRichEditCtrl& edit);

public:
	static CStatusAnalysesPage*		m_pPage;

private:
	CMenu			m_menuPopup;
	CRichEditCtrl	m_edit[4];
	CFont*			m_pFont;
	int				m_nFontHeight;
	int				m_numVisibleRows;
	BOOL			m_bInitialized;
//	int				m_nInitialX, m_nInitialY;
	int				m_nInitialXMargin, m_nInitialYMargin;
	int				m_nLabelHeight;
	int				m_nWindowOrder[4];
	int				m_nPlayerToWindow[4];
	int				m_nCurrentChild;

// Dialog Data
	//{{AFX_DATA(CStatusAnalysesPage)
	enum { IDD = IDP_STATUS_ANALYSES };
	BOOL	m_bShowWest;
	BOOL	m_bShowNorth;
	BOOL	m_bShowEast;
	BOOL	m_bShowSouth;
	BOOL	m_bShowAll;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStatusAnalysesPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CStatusAnalysesPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickWindowShow();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnClearPane();
	afx_msg void OnClearAllPanes();
	afx_msg void OnHide();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATUSANALYSESPAGE_H__D1ABEA23_8A4A_11D2_8FF6_00609777FAF1__INCLUDED_)
