//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// ScoreDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScoreDialog dialog

class CScoreDialog : public CDialog
{
// Construction
public:
	CScoreDialog(CWnd* pParent = NULL);   // standard constructor
	//
	void Clear();
	void StartNewHand();
	void SetBonusPoints(CStringArray& strArrayText);
	void SetTrickPoints(CStringArray& strArrayText);
	void SetTotalPoints(CString& strText);
//	void DisableGameControls() { m_bDisableGameControls = TRUE; }

private:
	void ScrollToBottom(CRichEditCtrl& edit);

public:
//	int		m_nCode;
//	BOOL	m_bDisableGameControls;

// Dialog Data
	//{{AFX_DATA(CScoreDialog)
	enum { IDD = IDD_SCORE };
	CString	m_strBonusPoints;
	CString	m_strTrickPoints;
	CString	m_strTotalPoints;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScoreDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScoreDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
