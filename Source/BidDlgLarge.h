//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// BidDlgLarge.h : header file
//
#ifndef __BIDDIALOGLARGE__
#define __BIDDIALOGLARGE__

#include "BidDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CBidDialogLarge dialog

class CBidDialogLarge : public CBidDialog
{
// Construction
public:
	CBidDialogLarge(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBidDialogLarge)
	enum { IDD = IDD_BID };
	//}}AFX_DATA
public:
	// overridden virtual operations
	virtual void RegisterBid(int nBid, BOOL bShowButtonPress=FALSE);

protected:
	// overloaded virtual operations
	virtual void EnableControls();
	virtual void EnableButtonImages(BOOL bEnable);
	virtual void DisableControls();
	virtual void FlashButton(int nBid);
	virtual void PressBidButton(int nBid);
	//
	virtual void UpdateBidDisplay(int nPos, int nBid, BOOL bPrompt=FALSE, BOOL bTentative=FALSE);

//
private:
	CFlatButton	m_flatButtons[35];


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBidDialogLarge)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
//protected:
public:
	// Generated message map functions
	//{{AFX_MSG(CBidDialogLarge)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
