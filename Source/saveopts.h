//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// saveopts.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileSaveOptionsDlg dialog

class CFileSaveOptionsDlg : public CDialog
{
// Construction
public:
	CFileSaveOptionsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileSaveOptionsDlg)
	enum { IDD = IDD_FILE_SAVE_OPTIONS };
	BOOL	m_bShowComments;
	BOOL	m_bShowBiddingHistory;
	BOOL	m_bShowPlayHistory;
	BOOL	m_bShowAnalyses;
	BOOL	m_bSaveWestAnalysis;
	BOOL	m_bSaveEastAnalysis;
	BOOL	m_bSaveSouthAnalysis;
	BOOL	m_bSaveNorthAnalysis;
	BOOL	m_bSaveIntermediatePositions;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileSaveOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileSaveOptionsDlg)
	afx_msg void OnOk();
	virtual void OnCancel();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
