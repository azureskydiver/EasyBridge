//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// dealgmpg.h : header file
//
class CObjectWithProperties;

/////////////////////////////////////////////////////////////////////////////
// CDealOptsPointsPage dialog

class CDealOptsPointsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDealOptsPointsPage)
	
	friend class CDealOptionsPropSheet;

// routines
private:
	void OnPressSpinButton();
	void InitSpinButtons();
	void Update();

// data
private:
	CObjectWithProperties&	m_app;


// Construction
public:
	CDealOptsPointsPage(CObjectWithProperties* pApp=NULL);
	~CDealOptsPointsPage();

// Dialog Data
	//{{AFX_DATA(CDealOptsPointsPage)
	enum { IDD = IDP_DEAL_POINTS };
	CSpinButtonCtrl	m_sbMaxGrandSlamPts;
	CSpinButtonCtrl	m_sbMinGrandSlamPts;
	CSpinButtonCtrl	m_sbMaxSmallSlamPts;
	CSpinButtonCtrl	m_sbMinSmallSlamPts;
	CSpinButtonCtrl	m_sbMaxSlamPts;
	CSpinButtonCtrl	m_sbMinSlamPts;
	CSpinButtonCtrl	m_sbMaxNTGamePts;
	CSpinButtonCtrl	m_sbMinNTGamePts;
	CSpinButtonCtrl	m_sbMaxMinorGamePts;
	CSpinButtonCtrl	m_sbMinMinorGamePts;
	CSpinButtonCtrl	m_sbMaxMajorGamePts;
	CSpinButtonCtrl	m_sbMinMajorGamePts;
	CSpinButtonCtrl	m_sbMaxGamePts;
	CSpinButtonCtrl	m_sbMinGamePts;
	int		m_nMinGamePts;
	int		m_nMinMinorGamePts;
	int		m_nMaxMinorGamePts;
	int		m_nMaxGamePts;
	int		m_nMinMajorGamePts;
	int		m_nMaxMajorGamePts;
	int		m_nMinNoTrumpGamePts;
	int		m_nMaxNoTrumpGamePts;
	int		m_nMinSlamPts;
	int		m_nMaxSlamPts;
	int		m_nMaxSmallSlamPts;
	int		m_nMinSmallSlamPts;
	int		m_nMinGrandSlamPts;
	int		m_nMaxGrandSlamPts;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDealOptsPointsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDealOptsPointsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeSpinValue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
