//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// BiddingFinishedDialog.h : header file
//

#ifndef __BIDDINGFINIHSEDDLG__
#define __BIDDINGFINIHSEDDLG__

#include "FlatButton.h"

/////////////////////////////////////////////////////////////////////////////
// CBiddingFinishedDialog dialog

class CBiddingFinishedDialog : public CDialog
{
// Construction
public:
	CBiddingFinishedDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBiddingFinishedDialog)
	enum { IDD = IDD_BIDDING_FINISHED };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	void SetText(CString strText) { m_strText = strText; }

// public data
public:
	int m_nResultCode;

// private data
private:
	CString		m_strText;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBiddingFinishedDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBiddingFinishedDialog)
	virtual void OnOK();
	afx_msg void OnRebidHand();
	virtual BOOL OnInitDialog();
	afx_msg void OnRedeal();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif