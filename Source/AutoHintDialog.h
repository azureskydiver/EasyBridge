//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

#if !defined(AFX_AUTOHINTDIALOG_H__028099A1_B488_11D2_BDC6_444553540000__INCLUDED_)
#define AFX_AUTOHINTDIALOG_H__028099A1_B488_11D2_BDC6_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoHintDialog.h : header file
//
#include "FlatButton.h"
//#include "CJFlatComboBox.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoHintDialog dialog

class CAutoHintDialog : public CDialog
{
// Construction
public:
	CAutoHintDialog(CWnd* pParent = NULL);   // standard constructor

// operations
public:
	void SetHintText(LPCTSTR szText);
	void Clear();
	//
	void EnableHintAccept(BOOL bEnable=TRUE);

//
private:
	void	ResizeWindows();


// data
public:
	CRect			m_rect;

protected:
	BOOL			m_bInitialized;
	int				m_numVisibleRows;
	CString			m_strText;
	CFont*			m_pFont;
	CRichEditCtrl	m_edit;
	CFlatButton		m_flatButtons[2];

public:
// Dialog Data
	//{{AFX_DATA(CAutoHintDialog)
	enum { IDD = IDD_AUTO_HINT };
	CCJFlatComboBox		m_cbLevel;
	int		m_nHintLevel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoHintDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutoHintDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChangeHintLevel();
	afx_msg void OnAcceptHint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOHINTDIALOG_H__028099A1_B488_11D2_BDC6_444553540000__INCLUDED_)
