//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
#if !defined(AFX_BIDCONFIGPAGE_H__3C1A5D41_981D_11D2_BDC6_444553540000__INCLUDED_)
#define AFX_BIDCONFIGPAGE_H__3C1A5D41_981D_11D2_BDC6_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BidConfigPage.h : header file
//
class CObjectWithProperties;


/////////////////////////////////////////////////////////////////////////////
// CBidOptsConfigPage dialog

class CBidOptsConfigPage : public CPropertyPage
{
// Construction
public:
	DECLARE_DYNCREATE(CBidOptsConfigPage)

	friend class CBidOptionsPropSheet;

// routines
private:
	void SetNeuralNetFile(BOOL bNew=TRUE);
	void Update();

// data
private:
	CObjectWithProperties&	m_app;
	CObjectWithProperties&	m_conventionSet;
	//
	int		m_numNNetHiddenLayers;
	int		m_numNNetNodesPerHiddenLayer;


// Construction
public:
	CBidOptsConfigPage(CObjectWithProperties* pApp=NULL, CObjectWithProperties* pConventionSet=NULL);
	~CBidOptsConfigPage();

// Dialog Data
	//{{AFX_DATA(CBidOptsConfigPage)
	enum { IDD = IDP_BID_CONFIG };
	CSliderCtrl	m_sliderAggressiveness;
	int		m_nBiddingEngine;
	int		m_nAggressiveness;
	CString	m_strNeuralNetFilePath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBidOptsConfigPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBidOptsConfigPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBiddingEngine();
	afx_msg void OnBrowse();
	afx_msg void OnNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BIDCONFIGPAGE_H__3C1A5D41_981D_11D2_BDC6_444553540000__INCLUDED_)
