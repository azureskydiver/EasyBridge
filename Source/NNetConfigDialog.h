//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

#if !defined(AFX_NEURALNETSIZEDLG_H__159F9481_AF2B_11D2_BDC6_444553540000__INCLUDED_)
#define AFX_NEURALNETSIZEDLG_H__159F9481_AF2B_11D2_BDC6_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NeuralNetSizeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNNetConfigDialog dialog

class CNNetConfigDialog : public CDialog
{
// Construction
public:
	CNNetConfigDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNNetConfigDialog)
	enum { IDD = IDD_NNET_SIZE };
	int		m_numLayersSel;
	int		m_numNodesPerLayerSel;
	//}}AFX_DATA

//
public:
	int		m_numNNetHiddenLayers;
	int		m_numNNetNodesPerHiddenLayer;



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNNetConfigDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNNetConfigDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEURALNETSIZEDLG_H__159F9481_AF2B_11D2_BDC6_444553540000__INCLUDED_)
