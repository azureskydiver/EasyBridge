//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// WelcomeWnd.h : header file
//
#ifndef __WELCOMEWND_H__
#define __WELCOMEWND_H__

#include "MyBitmap.h"

/////////////////////////////////////////////////////////////////////////////
// CWelcomeWnd dialog

class AFX_EXT_CLASS CWelcomeWnd : public CDialog
{
// Construction
public:
	BOOL Create(CWnd* pParent);
// Dialog Data
	//{{AFX_DATA(CWelcomeWnd)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWelcomeWnd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetShowRulesHelp(BOOL bShow) { m_bShowRulesHelp = bShow; }

public:
//	CBitmap m_bitmap;
	CMyBitmap m_bitmap;
	UINT 	m_timer;
	BOOL	m_bShowRulesHelp;

protected:
	HINSTANCE	m_hAppInstance;
	CFont 		m_font;   // light version of dialog font

	// Generated message map functions
	//{{AFX_MSG(CWelcomeWnd)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual void OnOK();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif