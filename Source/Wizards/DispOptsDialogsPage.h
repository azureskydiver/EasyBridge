//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
#if !defined(AFX_DisplayOptsDialogsPage_H__D8A35B21_2735_11D2_BDC6_444553540000__INCLUDED_)
#define AFX_DisplayOptsDialogsPage_H__D8A35B21_2735_11D2_BDC6_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DisplayOptsDialogsPage.h : header file
//
class CObjectWithProperties;


/////////////////////////////////////////////////////////////////////////////
// CDispOptsDialogsPage dialog

class CDispOptsDialogsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDispOptsDialogsPage)

// operations
public:
	BOOL Update();

// data
private:
	CObjectWithProperties&	m_app;
	CObjectWithProperties&	m_frame;
	CObjectWithProperties&	m_view;


// Construction
public:
	CDispOptsDialogsPage(CObjectWithProperties* pApp=NULL, CObjectWithProperties* pFrame=NULL, CObjectWithProperties* pView=NULL);
	~CDispOptsDialogsPage();

// Dialog Data
	//{{AFX_DATA(CDispOptsDialogsPage)
	enum { IDD = IDP_DISP_DIALOGS };
	BOOL	m_bAutoShowBidHistory;
	BOOL	m_bAutoShowPlayHistory;
	BOOL	m_bAutoHideBidHistory;
	BOOL	m_bAutoHidePlayHistory;
	BOOL	m_bShowLayoutOnEdit;
	BOOL	m_bAutoShowNNetOutputWhenTraining;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDispOptsDialogsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDispOptsDialogsPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DisplayOptsDialogsPage_H__D8A35B21_2735_11D2_BDC6_444553540000__INCLUDED_)
