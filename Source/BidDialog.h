//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// BidDialog.h : header file
//
#ifndef __BIDDIALOG__
#define __BIDDIALOG__

#include "DialogInfo.h"
#include "BiddingFinishedDialog.h"
#include "FlatButton.h"

/////////////////////////////////////////////////////////////////////////////
// CBidDialog dialog

class CBidDialog : public CDialog
{
// info
public:
	enum {	
		BD_MODE_NONE		= 0,
		BD_MODE_WAITFORBID	= 1,
		BD_MODE_EVALUATE	= 2,
		BD_MODE_TRAIN		= 3,
		BD_MODE_AUTO_TRAIN	= 4,
		BD_MODE_MANUAL_BID	= 5,
		BD_MODE_DONE		= 99,
	};

// Construction
public:
	CBidDialog(int nDialogID, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBidDialog)
	BOOL	m_bManualBidding;
	//}}AFX_DATA
public:
	int	m_nCurrMode;

public:
	// virtual operations
	virtual void InitBiddingSequence();
	virtual void RegisterBid(int nBid, BOOL bShowButtonPress=FALSE);
	// non-virtual operations
	void CancelImmediate();
	void RepositionWindow();
	void EnableManualBidding(BOOL bEnable=TRUE);
	void EnableTrainingMode(BOOL bEnable) { m_bTrainingMode = bEnable; }

protected:
	// pure virtual functions
	virtual void EnableControls() = 0;
	virtual void EnableButtonImages(BOOL bEnable) = 0;
	virtual void DisableControls();
	virtual void FlashButton(int nBid) = 0;
	virtual void PressBidButton(int nBid) = 0;

	// non-pure virtual functions
	virtual void UpdateBidDisplay(int nPos, int nBid, BOOL bPrompt=FALSE, BOOL bTentative=FALSE);

	// non-virtual operations
	BOOL UsingSuitSymbols() { return m_bUseSuitSymbols; }
	void CloseSecondaryWindows();
	int EnterPlayerBid(int nBid);
	void ProcessBidReturnCode(int nCode);
	int GetComputerBids(int nStart=WEST);
	void DealNewHands();
	BOOL BiddingDone();
	void FlashWindow(CWnd* pWnd1, CWnd* pWnd2=NULL);
	void HighlightRect(CDC* pDC, const CRect& rect);
	BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);

//
protected:
	int			m_nBiddingEngine;
	BOOL		m_bSpeechEnabled;
	BOOL		m_bUseSuitSymbols;
	BOOL		m_bTrainingMode;
	BOOL		m_bInsertBiddingPause;
	int			m_nBiddingPauseLength;
	int			m_nComputerBid;
	CIntMap		m_mapIDtoIndex;
	CFlatButton*			m_pFlatBaseButtons;
	CBiddingFinishedDialog	m_bidFinishedDialog;
	//
	HICON		m_buttonIcons[5][7];
	HICON		m_hIconPlayerPrompt;
	HICON		m_hIconComputerPrompt;

//Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBidDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
//protected:
public:
	// Generated message map functions
	//{{AFX_MSG(CBidDialog)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBidHint();
	afx_msg void OnBidBackUp();
	afx_msg void OnHelp();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnBidRedeal();
	afx_msg void OnBidRestart();
	afx_msg void OnAutotrain();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
