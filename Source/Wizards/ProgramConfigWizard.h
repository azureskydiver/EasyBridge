//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
#if !defined(AFX_PROGRAMCONFIGWIZARD_H__89D86431_D747_11D2_9096_00609777FAF1__INCLUDED_)
#define AFX_PROGRAMCONFIGWIZARD_H__89D86431_D747_11D2_9096_00609777FAF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//
// ProgramConfigWizard.h : header file
//
class CObjectWithProperties;
class CProgConfigWizardData;
class CProgConfigIntroPage;
class CProgConfigHelpLevelPage;
class CProgConfigViewSettingsPage;
class CProgConfigSuitsDisplayPage;
class CProgConfigGameMechanicsPage;
class CProgConfigPausesPage;
class CProgConfigBiddingPage;
class CProgConfigFinishPage;


/////////////////////////////////////////////////////////////////////////////
// CProgramConfigWizard

class AFX_EXT_CLASS CProgramConfigWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(CProgramConfigWizard)

// Operations
public:
	CProgConfigWizardData&	GetData() { return *m_pData; }
	void InitOptions(BOOL bFirstTime=TRUE);
	void SaveOptions();
	void OnHelp();

// Data
public:

private:
	CProgConfigWizardData*		m_pData;
	//
	CObjectWithProperties&		m_app;
	CObjectWithProperties&		m_doc;
	CObjectWithProperties&		m_frame;
	CObjectWithProperties&		m_view;
	CObjectWithProperties&		m_conventionSet;
	//
	CProgConfigIntroPage*			m_pIntroPage;
	CProgConfigHelpLevelPage*		m_pHelpLevelPage;
	CProgConfigViewSettingsPage*	m_pViewPage;
	CProgConfigSuitsDisplayPage*	m_pSuitsPage;
	CProgConfigGameMechanicsPage*	m_pMechanicsPage;
	CProgConfigPausesPage*			m_pPausesPage;
	CProgConfigBiddingPage*			m_pBiddingPage;
	CProgConfigFinishPage*			m_pFinishPage;


// Construction
public:
	CProgramConfigWizard(CObjectWithProperties* pApp, CObjectWithProperties* pDoc, CObjectWithProperties* pFrame, 
						 CObjectWithProperties* pView,  CObjectWithProperties* pConventionSet,
						 CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgramConfigWizard)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CProgramConfigWizard();

	// Generated message map functions
protected:
	//{{AFX_MSG(CProgramConfigWizard)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRAMCONFIGWIZARD_H__89D86431_D747_11D2_9096_00609777FAF1__INCLUDED_)
