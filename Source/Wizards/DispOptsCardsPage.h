//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// dis_card.h : header file
//
class CObjectWithProperties;

/////////////////////////////////////////////////////////////////////////////
// CDispOptsCardsPage dialog

class CDispOptsCardsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDispOptsCardsPage)

// operations
public:
	BOOL Update();

// data
private:
	CObjectWithProperties&	m_app;
	CObjectWithProperties&	m_frame;
	CObjectWithProperties&	m_view;
	//
	int		m_nAnimationGranularity;

// Construction
public:
	CDispOptsCardsPage(CObjectWithProperties* pApp=NULL, CObjectWithProperties* pFrame=NULL, CObjectWithProperties* pView=NULL);
	~CDispOptsCardsPage();

// Dialog Data
	//{{AFX_DATA(CDispOptsCardsPage)
	enum { IDD = IDP_DISP_CARDS };
	CSliderCtrl	m_sliderGranularity;
//	CSpinButtonCtrl	m_spinGranularity;
	BOOL	m_bOffsetVertSuits;
	BOOL	m_bTripleBuffer;
	BOOL	m_bAnimateCards;
	CString	m_strGranularity;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDispOptsCardsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDispOptsCardsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
