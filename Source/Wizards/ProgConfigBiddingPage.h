//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
#if !defined(AFX_PROGCONFIGBIDDINGPAGE_H__89D86437_D747_11D2_9096_00609777FAF1__INCLUDED_)
#define AFX_PROGCONFIGBIDDINGPAGE_H__89D86437_D747_11D2_9096_00609777FAF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//
// ProgConfigBiddingPage.h : header file
//
class CProgramConfigWizard;
class CProgConfigWizardData;


/////////////////////////////////////////////////////////////////////////////
// CProgConfigBiddingPage dialog

class CProgConfigBiddingPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CProgConfigBiddingPage)

// Construction
public:
	CProgConfigBiddingPage(CProgramConfigWizard* pSheet=NULL);
	~CProgConfigBiddingPage();

// operations
private:


// Dialog Data
private:
	CProgramConfigWizard&	m_sheet;
	CProgConfigWizardData&	m_data;

public:
	//{{AFX_DATA(CProgConfigBiddingPage)
	enum { IDD = IDW_PROG_CONFIG_BIDDING_STYLE };
	int		m_nBiddingStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProgConfigBiddingPage)
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
	//{{AFX_MSG(CProgConfigBiddingPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGCONFIGBIDDINGPAGE_H__89D86437_D747_11D2_9096_00609777FAF1__INCLUDED_)
