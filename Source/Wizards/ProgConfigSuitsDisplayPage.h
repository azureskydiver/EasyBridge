//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
#if !defined(AFX_PROGCONFIGSUITSDISPLAYPAGE_H__856E7801_D931_11D2_BDC6_444553540000__INCLUDED_)
#define AFX_PROGCONFIGSUITSDISPLAYPAGE_H__856E7801_D931_11D2_BDC6_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgConfigSuitsDisplayPage.h : header file
//
class CProgramConfigWizard;
class CProgConfigWizardData;


/////////////////////////////////////////////////////////////////////////////
// CProgConfigSuitsDisplayPage dialog

class CProgConfigSuitsDisplayPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CProgConfigSuitsDisplayPage)

// Dialog Data
private:
	CProgramConfigWizard&	m_sheet;
	CProgConfigWizardData&	m_data;
	//
	CBitmap					m_radioBitmaps[5];


// Construction
public:
	CProgConfigSuitsDisplayPage(CProgramConfigWizard* pSheet=NULL);
	~CProgConfigSuitsDisplayPage();

// Dialog Data
	//{{AFX_DATA(CProgConfigSuitsDisplayPage)
	enum { IDD = IDW_PROG_CONFIG_SUITS_DISPLAY };
	int		m_nSuitDisplayOrder;
	BOOL	m_bDummyTrumpsOnLeft;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProgConfigSuitsDisplayPage)
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
	//{{AFX_MSG(CProgConfigSuitsDisplayPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGCONFIGSUITSDISPLAYPAGE_H__856E7801_D931_11D2_BDC6_444553540000__INCLUDED_)
