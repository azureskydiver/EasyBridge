//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

#if !defined(AFX_BIDDIALOGSMALL_H__C42ABC13_686A_11D2_8FC0_00609777FAF1__INCLUDED_)
#define AFX_BIDDIALOGSMALL_H__C42ABC13_686A_11D2_8FC0_00609777FAF1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BidDlgSmall.h : header file
//
#include "BidDialog.h"


/////////////////////////////////////////////////////////////////////////////
// CBidDialogSmall dialog

class CBidDialogSmall : public CBidDialog
{
// Construction
public:
	CBidDialogSmall(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBidDialogSmall)
	enum { IDD = IDD_BID_SMALL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

//
public:
	// overloaded virtual operations
	virtual void RegisterBid(int nBid, BOOL bShowButtonPress=FALSE);

protected:
	// overloaded virtual operations
	virtual void InitBiddingSequence();
	virtual void EnableControls();
	virtual void EnableButtonImages(BOOL bEnable);
	virtual void DisableControls();
	virtual void FlashButton(int nBid);
	virtual void PressBidButton(int nBid);
	//
	virtual void UpdateBidDisplay(int nPos, int nBid, BOOL bPrompt=FALSE, BOOL bTentative=FALSE);

//
private:
	int			m_nSelectedLevel;
	HICON		m_buttonLevelIcons[7];
	HICON		m_buttonSuitIcons[5];
	CFlatButton	m_flatLevelButtons[7];
	CFlatButton	m_flatSuitButtons[5];


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBidDialogSmall)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBidDialogSmall)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BIDDIALOGSMALL_H__C42ABC13_686A_11D2_8FC0_00609777FAF1__INCLUDED_)
