//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// dealmnpg.h : header file
//
class CObjectWithProperties;

/////////////////////////////////////////////////////////////////////////////
// CDealOptsMinorsPage dialog

class CDealOptsMinorsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDealOptsMinorsPage)

	friend class CDealOptionsPropSheet;

// routines
private:
	void Update();

// data
private:
	CObjectWithProperties&	m_app;


// Construction
public:
	CDealOptsMinorsPage(CObjectWithProperties* pApp=NULL);
	~CDealOptsMinorsPage();

// Dialog Data
	//{{AFX_DATA(CDealOptsMinorsPage)
	enum { IDD = IDP_DEAL_MINORS };
	int		m_nMinCardsInMinor;
	int		m_nMinSuitTopper;
	int		m_nMinTeamDistribution;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDealOptsMinorsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDealOptsMinorsPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
