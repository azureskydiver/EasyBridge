//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// GameOptsScoringPage.h : header file
//
class CObjectWithProperties;

/////////////////////////////////////////////////////////////////////////////
// CGameOptsScoringPage dialog

class CGameOptsScoringPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGameOptsScoringPage)

// operations
public:
	void Update();

// data
private:
	CObjectWithProperties&	m_app;


// Construction
public:
	CGameOptsScoringPage(CObjectWithProperties* pApp=NULL);
	~CGameOptsScoringPage();

// Dialog Data
	//{{AFX_DATA(CGameOptsScoringPage)
	enum { IDD = IDP_GAME_SCORING };
	BOOL	m_bScoreHonorsBonuses;
	BOOL	m_bUseDuplicateScoring;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGameOptsScoringPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGameOptsScoringPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
