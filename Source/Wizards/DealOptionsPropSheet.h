//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DealOptionsPropSheet.h : header file
//
class CObjectWithProperties;
class CDealOptsPointsPage;
class CDealOptsMinorsPage;
class CDealOptsMajorsPage;
class CDealOptsNoTrumpPage;	
class CDealOptsSlamPage;
class CDealOptsMiscPage;


/////////////////////////////////////////////////////////////////////////////
// CDealOptionsPropSheet

class AFX_EXT_CLASS CDealOptionsPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CDealOptionsPropSheet)

// Construction
public:
	CDealOptionsPropSheet(CObjectWithProperties* pApp=NULL, CWnd* pParentWnd = NULL, UINT iSelectPage=0);
	virtual ~CDealOptionsPropSheet();

// Attributes
public:
	CDealOptsPointsPage* 	m_pDealPointsPage;
	CDealOptsMinorsPage* 	m_pDealMinorsPage;
	CDealOptsMajorsPage* 	m_pDealMajorsPage;
	CDealOptsNoTrumpPage* 	m_pDealNoTrumpsPage;	
	CDealOptsSlamPage* 		m_pDealSlamPage;
	CDealOptsMiscPage*		m_pDealMiscPage;

// Operations
public:
	void UpdateAllPages();
	void OnHelp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDealOptionsPropSheet)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CDealOptionsPropSheet)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
