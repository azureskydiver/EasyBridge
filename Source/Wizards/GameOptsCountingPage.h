//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// GameOptsCountingPage.h : header file
//
class CObjectWithProperties;

/////////////////////////////////////////////////////////////////////////////
// CGameOptsCountingPage dialog

class CGameOptsCountingPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGameOptsCountingPage)

// operations
public:
	void Update();

// data
private:
	CObjectWithProperties&	m_app;


// Construction
public:
	CGameOptsCountingPage(CObjectWithProperties* pApp=NULL);
	~CGameOptsCountingPage();

// Dialog Data
	//{{AFX_DATA(CGameOptsCountingPage)
	enum { IDD = IDP_GAME_COUNTING };
	BOOL	m_bAcelessPenalty;
	BOOL	m_b4AceBonus;
	BOOL	m_bPenalizeUGHonors;
	BOOL	m_bCountShortSuits;
	int		m_nHonorsValuationMode;
	double	m_fAceValue;
	double	m_fKingValue;
	double	m_fQueenValue;
	double	m_fJackValue;
	double	m_fTenValue;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGameOptsCountingPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGameOptsCountingPage)
	afx_msg void OnValuationModeChange();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
