//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DealOptsNoTrumpPage.h : header file
//
class CObjectWithProperties;

/////////////////////////////////////////////////////////////////////////////
// CDealOptsNoTrumpPage dialog

class CDealOptsNoTrumpPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDealOptsNoTrumpPage)

	friend class CDealOptionsPropSheet;

// operations
private:
	void Update();

// data
private:
	CObjectWithProperties&	m_app;


// Construction
public:
	CDealOptsNoTrumpPage(CObjectWithProperties* pApp=NULL);
	~CDealOptsNoTrumpPage();

// Dialog Data
	//{{AFX_DATA(CDealOptsNoTrumpPage)
	enum { IDD = IDP_DEAL_NOTRUMP };
	BOOL	m_bNeedTwoBalancedHands;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDealOptsNoTrumpPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDealOptsNoTrumpPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
