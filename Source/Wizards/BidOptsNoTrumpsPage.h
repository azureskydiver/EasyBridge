//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// BidOptsNoTrumpsPage.h : header file
//
class CObjectWithProperties;


/////////////////////////////////////////////////////////////////////////////
// CBidOptsNoTrumpsPage dialog

class CBidOptsNoTrumpsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CBidOptsNoTrumpsPage)

	friend class CBidOptionsPropSheet;

// routines
private:
	void Update();

// data
private:
	CObjectWithProperties&	m_app;
	CObjectWithProperties&	m_conventionSet;
	int						m_nGambling3NTVersion;
	
// Construction
public:
	CBidOptsNoTrumpsPage(CObjectWithProperties* pApp=NULL, CObjectWithProperties* pConventionSet=NULL);
	~CBidOptsNoTrumpsPage();

// Dialog Data
	//{{AFX_DATA(CBidOptsNoTrumpsPage)
	enum { IDD = IDP_BID_NOTRUMPS };
	int		m_n1NTRange;
	int		m_n2NTRange;
	int		m_n3NTRange;
	BOOL	m_bGambling3NT;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBidOptsNoTrumpsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBidOptsNoTrumpsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnGambling3nt();
	afx_msg void OnRadio3ntRange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
