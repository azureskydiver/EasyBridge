//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
#if !defined(AFX_PROGCONFIGGAMEMECHANICSPAGE_H__48241AA1_DA20_11D2_BDC6_444553540000__INCLUDED_)
#define AFX_PROGCONFIGGAMEMECHANICSPAGE_H__48241AA1_DA20_11D2_BDC6_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgConfigGameMechanicsPage.h : header file
//
class CProgramConfigWizard;
class CProgConfigWizardData;


/////////////////////////////////////////////////////////////////////////////
// CProgConfigGameMechanicsPage dialog

class CProgConfigGameMechanicsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CProgConfigGameMechanicsPage)

// Construction
public:
	CProgConfigGameMechanicsPage(CProgramConfigWizard* pSheet=NULL);
	~CProgConfigGameMechanicsPage();

// Dialog Data
private:
	CProgramConfigWizard&	m_sheet;
	CProgConfigWizardData&	m_data;

public:
	//{{AFX_DATA(CProgConfigGameMechanicsPage)
	enum { IDD = IDW_PROG_CONFIG_GAME_MECHANICS };
	BOOL	m_bScoreDuplicate;
	BOOL	m_bIgnoreHonorsBonuses;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProgConfigGameMechanicsPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CProgConfigGameMechanicsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGCONFIGGAMEMECHANICSPAGE_H__48241AA1_DA20_11D2_BDC6_444553540000__INCLUDED_)
