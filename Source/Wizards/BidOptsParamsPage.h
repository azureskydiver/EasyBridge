//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// BidOptsParamsPage.h : header file
//
class CObjectWithProperties;


/////////////////////////////////////////////////////////////////////////////
// CBidOptsParamsPage dialog

class CBidOptsParamsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CBidOptsParamsPage)

	friend class CBidOptionsPropSheet;

// operations
public:
	void Update();

// data
private:
	CObjectWithProperties&	m_app;
	CObjectWithProperties&	m_conventionSet;


// Construction
public:
	CBidOptsParamsPage(CObjectWithProperties* pApp=NULL, CObjectWithProperties* pConventionSet=NULL);
	~CBidOptsParamsPage();

// Dialog Data
	//{{AFX_DATA(CBidOptsParamsPage)
	enum { IDD = IDP_BID_CONFIG };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBidOptsParamsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBidOptsParamsPage)
	afx_msg void OnSelchangeConfig();
	afx_msg void OnSaveConfig();
	afx_msg void OnRemoveConfig();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
