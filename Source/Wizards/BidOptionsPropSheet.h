//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// bidprop.h : header file
//
//class BidOptsParamsPage;
class CBidOptsConfigPage;
class CBidOptsGeneralPage;
class CBidOptsTwoBidsPage;
class CBidOptsNoTrumpsPage;
class CBidOptsMiscPage;
class CBidOptsOpenPage;
class CObjectWithProperties;


/////////////////////////////////////////////////////////////////////////////
// CBidOptionsPropSheet

class AFX_EXT_CLASS CBidOptionsPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CBidOptionsPropSheet)

// Construction
public:
	CBidOptionsPropSheet(CObjectWithProperties* pApp=NULL, CObjectWithProperties* pConventionSet=NULL, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CBidOptionsPropSheet();

// Attributes
public:
	//
	//CBidOptsParamsPage*	m_pParamsPage;
	CBidOptsConfigPage* 	m_pBidConfigPage;
	CBidOptsGeneralPage*	m_pBidGeneralPage;
	CBidOptsTwoBidsPage* 	m_pBidTwoBidsPage;
	CBidOptsNoTrumpsPage*	m_pBidNoTrumpsPage;
	CBidOptsMiscPage*		m_pBidMiscPage;
	CBidOptsOpenPage*		m_pBidOpenPage;

// Operations
public:
	void UpdateAllPages();
	void OnHelp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBidOptionsPropSheet)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CBidOptionsPropSheet)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
