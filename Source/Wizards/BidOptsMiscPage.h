//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// BidOptsMiscPage.h : header file
//
class CObjectWithProperties;


/////////////////////////////////////////////////////////////////////////////
// CBidOptsMiscPage dialog

class CBidOptsMiscPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CBidOptsMiscPage)

	friend class CBidOptionsPropSheet;

	// routines
private:
	void Update();

// data
private:
	CObjectWithProperties&	m_app;
	CObjectWithProperties&	m_conventionSet;


// Construction
public:
	CBidOptsMiscPage(CObjectWithProperties* pApp=NULL, CObjectWithProperties* pConventionSet=NULL);
	~CBidOptsMiscPage();

// Dialog Data
	//{{AFX_DATA(CBidOptsMiscPage)
	enum { IDD = IDP_BID_MISC };
	int		m_nJumpOvercalls;
	BOOL	m_b4thSuitForcing;
	BOOL	m_bStructuredReverses;
	BOOL	m_bJacoby2NT;
	BOOL	m_bMichaelsCueBid;
	BOOL	m_bUnusualNT;
	BOOL	m_bDrury;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBidOptsMiscPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBidOptsMiscPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
