//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
#if !defined(AFX_DISPOPTIONSCARDBACKSPAGE_H__BBF47353_F590_11D1_8F2C_00609777FAF1__INCLUDED_)
#define AFX_DISPOPTIONSCARDBACKSPAGE_H__BBF47353_F590_11D1_8F2C_00609777FAF1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DispOptsCardBacksPage.h : header file
//
class CObjectWithProperties;

/////////////////////////////////////////////////////////////////////////////
// CDispOptsCardBacksPage dialog

class CDispOptsCardBacksPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDispOptsCardBacksPage )

// operations
public:
	BOOL Update();

// data
private:
	CObjectWithProperties&	m_app;
	CObjectWithProperties&	m_frame;
	CObjectWithProperties&	m_view;
	CObjectWithProperties&	m_deck;
	//
	int		m_nIndex;


// Construction
public:
	CDispOptsCardBacksPage(CObjectWithProperties* pApp=NULL, CObjectWithProperties* pFrame=NULL, CObjectWithProperties* pView=NULL, CObjectWithProperties* pDeck=NULL);
	~CDispOptsCardBacksPage();

public:
// Dialog Data
	//{{AFX_DATA(CDispOptsCardBacksPage)
	enum { IDD = IDP_DISP_CARDBACKS };
	CListCtrl	m_listCardBacks;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDispOptsCardBacksPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDispOptsCardBacksPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPOPTIONSCARDBACKSPAGE_H__BBF47353_F590_11D1_8F2C_00609777FAF1__INCLUDED_)
