//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// SplashWnd.h : header file
//
#ifndef __SPLASH_H__
#define __SPLASH_H___

#include "MyBitmap.h"

/////////////////////////////////////////////////////////////////////////////
// CSplashWnd dialog

class AFX_EXT_CLASS CSplashWnd : public CDialog
{
// Construction
public:
	BOOL Create(CWnd* pParent);
// Dialog Data
	//{{AFX_DATA(CSplashWnd)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashWnd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
//	CBitmap m_bitmap;
	CMyBitmap m_bitmap;
	UINT 	m_timer;

protected:
	HINSTANCE	m_hAppInstance;
	CFont 		m_font;   // light version of dialog font

	// Generated message map functions
	//{{AFX_MSG(CSplashWnd)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif