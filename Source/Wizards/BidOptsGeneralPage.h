//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// BidOptsGeneralPage.h : header file
//
class CObjectWithProperties;


/////////////////////////////////////////////////////////////////////////////
// CBidOptsGeneralPage dialog

class CBidOptsGeneralPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CBidOptsGeneralPage)

	friend class CBidOptionsPropSheet;

// routines
private:
	void Update();

// Construction
public:
	CBidOptsGeneralPage(CObjectWithProperties* pApp=NULL, CObjectWithProperties* pConventionSet=NULL);
	~CBidOptsGeneralPage();

// data
private:
	CObjectWithProperties&	m_app;
	CObjectWithProperties&	m_conventionSet;


// Dialog Data
public:
	//{{AFX_DATA(CBidOptsGeneralPage)
	enum { IDD = IDP_BID_GENERAL };
	BOOL	m_bBlackwood;
	BOOL	m_bCueBids;
	int		m_nMajorsMode;
	BOOL	m_bStayman;
	BOOL	m_bLimitRaises;
	BOOL	m_bNegativeDoubles;
	BOOL	m_bTakeoutDoubles;
	BOOL	m_bSplinterBids;
	BOOL	m_bJacobyTransfers;
	BOOL	m_b4SuitTransfers;
	BOOL	m_bGerber;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBidOptsGeneralPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBidOptsGeneralPage)
	afx_msg void OnClickJacoby();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
