//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// GIBDialog.h : header file
//
#ifndef __GIBDIALOG__
#define __GIBDIALOG__

/////////////////////////////////////////////////////////////////////////////
// CGIBDialog dialog

//
class CGIBDialog : public CDialog
{
// Construction
public:
	CGIBDialog(CWnd* pParent = NULL);   // standard constructor
	~CGIBDialog();
	//
	void WaitForDialogInit();
	void GIBFinished();
//	void CreateThread(CWnd* pParent, HANDLE hEventFinished, HANDLE hEventCancel);

private:
	BOOL MarkDialogFinished();

public:
	BOOL	m_bFinished;
//	HANDLE	m_hEventCancel;
	int		m_nProcessTime;
	HANDLE	m_hDialogReady;

private:
	int		m_nProgressIncrement, m_nProgressUnits;
	CRITICAL_SECTION	m_csDialogFinished;

public:

// Dialog Data
	//{{AFX_DATA(CGIBDialog)
	enum { IDD = IDD_GIB_ACTIVE };
	CProgressCtrl	m_progress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGIBDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGIBDialog)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
