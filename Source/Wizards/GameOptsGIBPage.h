//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// GameOptsGIBPage.h : header file
//
class CObjectWithProperties;

/////////////////////////////////////////////////////////////////////////////
// CGameOptsGIBPage dialog

class CGameOptsGIBPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGameOptsGIBPage)

// operations
public:
	void Update();

// data
private:
	CObjectWithProperties&	m_app;


// Construction
public:
	CGameOptsGIBPage(CObjectWithProperties* pApp=NULL);
	~CGameOptsGIBPage();

// Dialog Data
	//{{AFX_DATA(CGameOptsGIBPage)
	enum { IDD = IDP_GAME_GIB };
	CSpinButtonCtrl	m_sbAnalysisTime;
	int		m_nAnalysisTime;
	CString	m_strGIBPath;
	BOOL	m_bEnableGIBDefenderPlay;
	BOOL	m_bEnableGIBDeclarerPlay;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGameOptsGIBPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGameOptsGIBPage)
	afx_msg void OnBrowse();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
