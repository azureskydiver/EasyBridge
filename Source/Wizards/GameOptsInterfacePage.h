//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
#if !defined(AFX_GAMEOPTSINTERFACEPAGE_H__E04E6101_9F41_11D3_BDC6_444553540000__INCLUDED_)
#define AFX_GAMEOPTSINTERFACEPAGE_H__E04E6101_9F41_11D3_BDC6_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GameOptsInterfacePage.h : header file
//
class CObjectWithProperties;


/////////////////////////////////////////////////////////////////////////////
// CGameOptsInterfacePage dialog

class CGameOptsInterfacePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGameOptsInterfacePage)

// operations
public:
	void Update();

// data
private:
	CObjectWithProperties&	m_app;

// Construction
public:
	CGameOptsInterfacePage(CObjectWithProperties* pApp=NULL);
	~CGameOptsInterfacePage();

private:
	void UpdatePauseLabels();
	void EnableOrDisablePauseSliders();

public:
// Dialog Data
	//{{AFX_DATA(CGameOptsInterfacePage)
	enum { IDD = IDP_GAME_INTERFACE };
	CSliderCtrl	m_sliderBiddingPause;
	CSliderCtrl	m_sliderPlayPause;
	BOOL	m_bAutoPlayLastCard;
	BOOL	m_bAutoJumpCursor;
	BOOL	m_bInsertBiddingPause;
	BOOL	m_bInsertPlayPause;
	CString	m_strBiddingPause;
	CString	m_strPlayPause;
	BOOL	m_bEnableSpokenBids;
	int		m_nBiddingPause;
	int		m_nPlayPause;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGameOptsInterfacePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGameOptsInterfacePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnablePause();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMEOPTSINTERFACEPAGE_H__E04E6101_9F41_11D3_BDC6_444553540000__INCLUDED_)
