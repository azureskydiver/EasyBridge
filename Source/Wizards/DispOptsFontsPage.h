//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
#if !defined(AFX_DISPOPTSFONTSPAGE_H__F4301361_5D4E_11D3_BDC6_444553540000__INCLUDED_)
#define AFX_DISPOPTSFONTSPAGE_H__F4301361_5D4E_11D3_BDC6_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DispOptsFontsPage.h : header file
//
class CObjectWithProperties;

/////////////////////////////////////////////////////////////////////////////
// CDispOptsFontsPage dialog

class CDispOptsFontsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDispOptsFontsPage)

// operations
public:
	BOOL Update();

private:
	void UpdateFontNames();
	int LogicalSizeToPoints(int nLogSize);


// data
private:
	CObjectWithProperties&	m_app;
	CObjectWithProperties&	m_frame;
	CObjectWithProperties&	m_view;
	//
	LOGFONT		m_lfAutoHint;
	LOGFONT		m_lfHistory;
	LOGFONT		m_lfAnalysis;
	//
	bool		m_bFontModified;
	bool 		m_bWarned;


// Construction
public:
	CDispOptsFontsPage(CObjectWithProperties* pApp=NULL, CObjectWithProperties* pFrame=NULL, CObjectWithProperties* pView=NULL);
	~CDispOptsFontsPage();

// Dialog Data
	//{{AFX_DATA(CDispOptsFontsPage)
	enum { IDD = IDP_DISP_FONTS };
	CString	m_strAutoHintDialogFont;
	CString	m_strHistoryDialogFont;
	CString	m_strAnalysisDialogFont;
	BOOL	m_bUseSuitSymbols;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDispOptsFontsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDispOptsFontsPage)
	afx_msg void OnSetAutohintDialogFont();
	afx_msg void OnSetHistoryDialogFont();
	afx_msg void OnSetAnalysisDialogFont();
	virtual BOOL OnInitDialog();
	afx_msg void OnResetAutohintDialogFont();
	afx_msg void OnResetHistoryDialogFont();
	afx_msg void OnResetAnalysisDialogFont();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPOPTSFONTSPAGE_H__F4301361_5D4E_11D3_BDC6_444553540000__INCLUDED_)
