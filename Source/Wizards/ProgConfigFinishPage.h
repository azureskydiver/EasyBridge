//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
#if !defined(AFX_PROGCONFIGFINISHPAGE_H__89D86438_D747_11D2_9096_00609777FAF1__INCLUDED_)
#define AFX_PROGCONFIGFINISHPAGE_H__89D86438_D747_11D2_9096_00609777FAF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//
// ProgConfigFinishPage.h : header file
//
class CProgramConfigWizard;
class CProgConfigWizardData;


/////////////////////////////////////////////////////////////////////////////
// CProgConfigFinishPage dialog

class CProgConfigFinishPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CProgConfigFinishPage)

// Construction
public:
	CProgConfigFinishPage(CProgramConfigWizard* pSheet=NULL);
	~CProgConfigFinishPage();


// Dialog Data
private:
	CProgramConfigWizard&	m_sheet;
	CProgConfigWizardData&	m_data;

public:
	//{{AFX_DATA(CProgConfigFinishPage)
	enum { IDD = IDW_PROG_CONFIG_FINISH };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProgConfigFinishPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CProgConfigFinishPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGCONFIGFINISHPAGE_H__89D86438_D747_11D2_9096_00609777FAF1__INCLUDED_)
