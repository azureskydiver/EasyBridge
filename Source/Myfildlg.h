//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// myfildlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyFileDialog dialog

class CMyFileDialog : public CFileDialog
{
// Construction
public:
//	CMyFileDialog(CWnd* pParent = NULL);   // standard constructor
	CMyFileDialog(BOOL bOpenFileDialog, 
				  LPCTSTR lpszDefExt = NULL, 
				  LPCTSTR lpszFileName = NULL, 
				  DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
				  LPCTSTR lpszFilter = NULL, 
				  CWnd* pParentWnd = NULL );

// Dialog Data
	//{{AFX_DATA(CMyFileDialog)
	enum { IDD = IDD_FILE_MYOPEN };
	//}}AFX_DATA

public:
	BOOL	m_bIsSaving;
	int		m_nFileType;

// routines
protected:
	void GetFileDescription(CString& strFileName);
	virtual void OnLBSelChangedNotify(UINT nIDBox, UINT iCurSel, UINT nCode);
	virtual void OnFileNameChange();
//	virtual void OnTypeChange();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyFileDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMyFileDialog)
	afx_msg void OnChangeFileDescription();
	virtual BOOL OnInitDialog();
	afx_msg void OnOptions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
