//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// BidOptsTwoBidsPage.h : header file
//
class CObjectWithProperties;


/////////////////////////////////////////////////////////////////////////////
// CBidOptsTwoBidsPage dialog

class CBidOptsTwoBidsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CBidOptsTwoBidsPage)

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
	CBidOptsTwoBidsPage(CObjectWithProperties* pApp=NULL, CObjectWithProperties* pConventionSet=NULL);
	~CBidOptsTwoBidsPage();

// Dialog Data
	//{{AFX_DATA(CBidOptsTwoBidsPage)
	enum { IDD = IDP_BID_2BIDS };
	int		m_nTwoBidsMode;
	int		m_n2NTAfterWeak2;
	int		m_n2ClubOpenValues;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBidOptsTwoBidsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBidOptsTwoBidsPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
