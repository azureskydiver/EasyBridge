//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
#if !defined(AFX_PROGCONFIGBIDDINGPAUSEPAGE_H__4FC4A3C5_DCA4_11D2_BDC6_444553540000__INCLUDED_)
#define AFX_PROGCONFIGBIDDINGPAUSEPAGE_H__4FC4A3C5_DCA4_11D2_BDC6_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgConfigBiddingPausePage.h : header file
//
class CProgramConfigWizard;
class CProgConfigWizardData;


/////////////////////////////////////////////////////////////////////////////
// CProgConfigPausesPage dialog

class CProgConfigPausesPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CProgConfigPausesPage)

// Construction
public:
	CProgConfigPausesPage(CProgramConfigWizard* pSheet=NULL);
	~CProgConfigPausesPage();


// Dialog Data
private:
	CProgramConfigWizard&	m_sheet;
	CProgConfigWizardData&	m_data;

public:
	//{{AFX_DATA(CProgConfigPausesPage)
	enum { IDD = IDW_PROG_CONFIG_PAUSES };
	int		m_nBiddingPauseSetting;
	int		m_nPlayPauseSetting;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProgConfigPausesPage)
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
	//{{AFX_MSG(CProgConfigPausesPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGCONFIGBIDDINGPAUSEPAGE_H__4FC4A3C5_DCA4_11D2_BDC6_444553540000__INCLUDED_)
