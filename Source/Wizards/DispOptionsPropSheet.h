//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DispOptionsPropSheet.h : header file
//
class CObjectWithProperties;
class CDispOptsSuitsPage;
class CDispOptsCardsPage;
class CDispOptsCardBacksPage;
class CDispOptsDialogsPage;
class CDispOptsFontsPage;
class CDispOptsMiscPage;


/////////////////////////////////////////////////////////////////////////////
// CDispOptionsPropSheet

class AFX_EXT_CLASS CDispOptionsPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CDispOptionsPropSheet)

// Construction
public:
	CDispOptionsPropSheet(CObjectWithProperties* pApp=NULL, CObjectWithProperties* pFrame=NULL, CObjectWithProperties* pView=NULL, CObjectWithProperties* pDeck=NULL, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CDispOptsSuitsPage*			m_pSuitsPage;
	CDispOptsCardsPage*			m_pCardsPage;
	CDispOptsCardBacksPage*		m_pCardBacksPage;
	CDispOptsDialogsPage*		m_pDialogsPage;
	CDispOptsFontsPage*			m_pFontsPage;
	CDispOptsMiscPage*			m_pMiscPage;
	//
	BOOL	m_bDisplayAffected;
	BOOL	m_bGlobalDisplayAffected;

// Operations
public:
	void UpdateAllPages();
	void OnHelp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDispOptionsPropSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDispOptionsPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDispOptionsPropSheet)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
