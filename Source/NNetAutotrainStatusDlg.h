//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

#if !defined(AFX_AUTOTRAINSTATUSDLG_H__AAF8D441_AE01_11D2_BDC6_444553540000__INCLUDED_)
#define AFX_AUTOTRAINSTATUSDLG_H__AAF8D441_AE01_11D2_BDC6_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutotrainStatusDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNNetAutotrainStatusDlg dialog

class CNNetAutotrainStatusDlg : public CDialog
{
// Construction
public:
	CNNetAutotrainStatusDlg(CWnd* pParent = NULL);   // standard constructor

//
public:
	BOOL Update(int numHands, int numBids, int numCorrections, int numNonPasBids, int numNonPassCorrections, int numTotalCyles, double fError);


// Dialog Data
protected:
	BOOL	m_bCancel;


public:
	//{{AFX_DATA(CAutotrainStatusDlg)
	enum { IDD = IDD_AUTOTRAIN_STATUS };
	CString	m_strNumHands;
	CString	m_strNumBids;
	CString	m_strNumCorrections;
	CString	m_strCorrectionRate;
	CString	m_strNumNPBids;
	CString	m_strNumNPCorrections;
	CString	m_strNPCorrectionRate;
	CString	m_strErrorValue;
	CString	m_strNumTotalCycles;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutotrainStatusDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutotrainStatusDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOTRAINSTATUSDLG_H__AAF8D441_AE01_11D2_BDC6_444553540000__INCLUDED_)
