//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
#if !defined(AFX_GAMEOPTSFILESPAGE_H__579DD881_3FB1_11D3_BDC6_444553540000__INCLUDED_)
#define AFX_GAMEOPTSFILESPAGE_H__579DD881_3FB1_11D3_BDC6_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GameOptsFilesPage.h : header file
//
class CObjectWithProperties;

/////////////////////////////////////////////////////////////////////////////
// CGameOptsFilesPage dialog

class CGameOptsFilesPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGameOptsFilesPage)

// operations
public:
	void Update();

// data
private:
	CObjectWithProperties&	m_app;


// Construction
public:
	CGameOptsFilesPage(CObjectWithProperties* pApp=NULL);
	~CGameOptsFilesPage();

// Dialog Data
	//{{AFX_DATA(CGameOptsFilesPage)
	enum { IDD = IDP_GAME_FILES };
	BOOL	m_bSaveIntermediatePositions;
	BOOL	m_bExposePBNGameCards;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGameOptsFilesPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGameOptsFilesPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMEOPTSFILESPAGE_H__579DD881_3FB1_11D3_BDC6_444553540000__INCLUDED_)
