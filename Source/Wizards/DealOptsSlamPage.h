//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DealOptsSlamPage.h : header file
//
class CObjectWithProperties;

/////////////////////////////////////////////////////////////////////////////
// CDealOptsSlamPage dialog

class CDealOptsSlamPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDealOptsSlamPage)

	friend class CDealOptionsPropSheet;

// operations
private:
	void Update();

// data
private:
	CObjectWithProperties&	m_app;


// Construction
public:
	CDealOptsSlamPage(CObjectWithProperties* pApp=NULL);
	~CDealOptsSlamPage();

// Dialog Data
	//{{AFX_DATA(CDealOptsSlamPage)
	enum { IDD = IDP_DEAL_SLAM };
	int		m_numAcesForGenericSlam;
	int		m_numAcesForSmallSlam;
	int		m_numKingsForGenericSlam;
	int		m_numKingsForSmallSlam;
	int		m_numKingsForGrandSlam;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDealOptsSlamPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDealOptsSlamPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
