//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// dis_misc.h : header file
//
class CObjectWithProperties;

/////////////////////////////////////////////////////////////////////////////
// CDispOptsMiscPage dialog

class CDispOptsMiscPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDispOptsMiscPage)

// operations
public:
	BOOL Update();

private:
	void UpdateColorSwatch();

// data
private:
	CObjectWithProperties&	m_app;
	CObjectWithProperties&	m_frame;
	CObjectWithProperties&	m_view;
	//
	int	 m_nBackgroundColor;


// Construction
public:
	CDispOptsMiscPage(CObjectWithProperties* pApp=NULL, CObjectWithProperties* pFrame=NULL, CObjectWithProperties* pView=NULL);
	~CDispOptsMiscPage();

// Dialog Data
	//{{AFX_DATA(CDispOptsMiscPage)
	enum { IDD = IDP_DISP_MISC };
	CStatic	m_wndColorSwatch;
	BOOL	m_bShowSplashWindow;
	BOOL	m_bShowStartupAnimation;
	int		m_nBitmapMode;
	BOOL	m_bShowBackgroundBitmap;
	BOOL	m_bScaleLargeBitmaps;
	CString	m_strFilePath;
	BOOL	m_bLowResOption;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDispOptsMiscPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDispOptsMiscPage)
	afx_msg void OnBackgroundColor();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnShowBackgroundBitmap();
	afx_msg void OnBrowse();
	afx_msg void OnShowLowResOption();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
