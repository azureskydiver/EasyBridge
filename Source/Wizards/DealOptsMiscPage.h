//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DealOptsMiscPage.h : header file
//
class CObjectWithProperties;

/////////////////////////////////////////////////////////////////////////////
// CDealOptsMiscPage dialog

class CDealOptsMiscPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDealOptsMiscPage)

	friend class CDealOptionsPropSheet;

// routines
private:
	void Update();

// data
private:
	CObjectWithProperties&	m_app;


// Construction
public:
	CDealOptsMiscPage(CObjectWithProperties* pApp=NULL);
	~CDealOptsMiscPage();

// Dialog Data
	//{{AFX_DATA(CDealOptsMiscPage)
	enum { IDD = IDP_DEAL_MISC };
	BOOL	m_bBalanceHands;
	BOOL	m_bGiveSouthBestHand;
	BOOL	m_bEnableDealNumbering;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDealOptsMiscPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDealOptsMiscPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
