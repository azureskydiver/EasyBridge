//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// BidOptsOpenPage.h : header file
//
class CObjectWithProperties;

/////////////////////////////////////////////////////////////////////////////
// CBidOptsOpenPage dialog

class CBidOptsOpenPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CBidOptsOpenPage)

	friend class CBidOptionsPropSheet;

// routines
private:
	void Update();

// data
private:
	CObjectWithProperties&	m_app;
	CObjectWithProperties&	m_conventionSet;
	//
	int	m_nAllowableOpenings;

// Construction
public:
	CBidOptsOpenPage(CObjectWithProperties* pApp=NULL, CObjectWithProperties* pConventionSet=NULL);
	~CBidOptsOpenPage();

// Dialog Data
	//{{AFX_DATA(CBidOptsOpenPage)
	enum { IDD = IDP_BID_OPEN1_VALUES };
	BOOL	m_b13TPsLs;
	BOOL	m_b14TPsGs;
	BOOL	m_b14TPsLs;
	BOOL	m_b11HCPsRbsLm;
	BOOL	m_b11HCPs6Cs;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBidOptsOpenPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBidOptsOpenPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
