//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// BidConfigPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ObjectWithProperties.h"
#include "BidOptsConfigPage.h"
#include "progopts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBidOptsConfigPage dialog

IMPLEMENT_DYNCREATE(CBidOptsConfigPage, CPropertyPage)

CBidOptsConfigPage::CBidOptsConfigPage(CObjectWithProperties* pApp, CObjectWithProperties* pConventionSet) : 
		CPropertyPage(CBidOptsConfigPage::IDD),
		m_app(*pApp), m_conventionSet(*pConventionSet)
{
	//{{AFX_DATA_INIT(CBidOptsConfigPage)
	m_nBiddingEngine = -1;
	m_nAggressiveness = 0;
	m_strNeuralNetFilePath = _T("");
	//}}AFX_DATA_INIT
	//
	m_nAggressiveness = (int) m_app.GetValueDouble(tfBiddingAggressiveness);
/*
	m_nBiddingEngine = m_app.GetValue(tnBiddingEngine);
	m_strNeuralNetFilePath = m_app.GetValueString(tszNeuralNetFile);
	m_numNNetHiddenLayers = m_app.GetValue(tnumNeuralNetHiddenLayers);
	m_numNNetNodesPerHiddenLayer = m_app.GetValue(tnumNeuralNetUnitsPerHiddenLayer);
*/
}

CBidOptsConfigPage::~CBidOptsConfigPage()
{
}

void CBidOptsConfigPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBidOptsConfigPage)
	DDX_Control(pDX, IDC_SLIDER_AGGRESSIVENESS, m_sliderAggressiveness);
	DDX_Radio(pDX, IDC_BIDDING_ENGINE, m_nBiddingEngine);
	DDX_Slider(pDX, IDC_SLIDER_AGGRESSIVENESS, m_nAggressiveness);
	DDX_Text(pDX, IDC_NET_FILE_PATH, m_strNeuralNetFilePath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBidOptsConfigPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBidOptsConfigPage)
	ON_BN_CLICKED(IDC_BIDDING_ENGINE, OnBiddingEngine)
	ON_BN_CLICKED(IDC_BIDDING_ENGINE2, OnBiddingEngine)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBidOptsConfigPage message handlers


//
BOOL CBidOptsConfigPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	//
	m_sliderAggressiveness.SetRange(-3, 3);
	m_sliderAggressiveness.SetPos(m_nAggressiveness);
	m_sliderAggressiveness.SetTicFreq(1);

	//
//	OnBiddingEngine();
	
	// 
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CBidOptsConfigPage::OnBiddingEngine() 
{
	UpdateData(TRUE);
	if (m_nBiddingEngine == 0)
	{
		// rule-based engine enabled
		GetDlgItem(IDC_SLIDER_AGGRESSIVENESS)->EnableWindow(TRUE);
		//
		GetDlgItem(IDC_NET_FILE_PATH)->EnableWindow(FALSE);
		GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);
		GetDlgItem(IDC_NEW)->EnableWindow(FALSE);
	}
	else
	{
		// neural net enabled
		GetDlgItem(IDC_NET_FILE_PATH)->EnableWindow(TRUE);
		GetDlgItem(IDC_BROWSE)->EnableWindow(TRUE);
		GetDlgItem(IDC_NEW)->EnableWindow(TRUE);
		//
		GetDlgItem(IDC_SLIDER_AGGRESSIVENESS)->EnableWindow(FALSE);
	}
}


//
void CBidOptsConfigPage::OnBrowse() 
{
	SetNeuralNetFile(FALSE);
}


//
void CBidOptsConfigPage::OnNew() 
{
	SetNeuralNetFile(TRUE);
}



//
void CBidOptsConfigPage::SetNeuralNetFile(BOOL bNew)
{
/*	
	// get neural net file path
	// first retrieve the directory of the existing file, if any
	char szName[256], szPath[256];
	if (!m_strNeuralNetFilePath.IsEmpty())
	{
		int nPos = m_strNeuralNetFilePath.ReverseFind('\\');
		if (nPos >= 0)
		{
			strcpy(szName, m_strNeuralNetFilePath.Mid(nPos+1));
			strcpy(szPath, m_strNeuralNetFilePath.Mid(0, nPos));
		}
	}

	// create the file dialog
	CFileDialog fileDlg(TRUE, "bmp", (bNew? "" : szName), (bNew? 0 : OFN_FILEMUSTEXIST),
			  			"Neural Net Files (*.net)|*.net|All Files (*.*)|*.*||",
		   				this);
	fileDlg.m_ofn.lpstrTitle = "Select Neural Net file";
	fileDlg.m_ofn.lpstrInitialDir = szPath;

	// then show
	if (fileDlg.DoModal() == IDCANCEL)
		return;

	// and get size
	CNNetConfigDialog netConfigDlg;
	netConfigDlg.m_numLayersSel = m_numNNetHiddenLayers - 1;
	netConfigDlg.m_numNNetNodesPerHiddenLayer = m_numNNetNodesPerHiddenLayer;
	if (netConfigDlg.DoModal() == IDCANCEL)
		return;

	// save
	m_strNeuralNetFilePath = fileDlg.GetPathName();
	m_numNNetHiddenLayers = netConfigDlg.m_numNNetHiddenLayers;
	m_numNNetNodesPerHiddenLayer = netConfigDlg.m_numNNetNodesPerHiddenLayer;

	UpdateData(FALSE);
*/
}



//
void CBidOptsConfigPage::Update()
{
	m_app.SetValueDouble(tfBiddingAggressiveness, (double) m_nAggressiveness);
/*
//	m_app.SetValue(tnBiddingEngine, m_nBiddingEngine);
//	m_app.SetValue(tszNeuralNetFile, m_strNeuralNetFilePath);

	// set neural net file path if changed
	if (m_strNeuralNetFilePath.CompareNoCase(m_app.GetValueString(tszNeuralNetFile)) != 0)
	{
		m_app.SetValue(tnumNeuralNetHiddenLayers, m_numNNetHiddenLayers);
		m_app.SetValue(tnumNeuralNetUnitsPerHiddenLayer, m_numNNetNodesPerHiddenLayer);
		m_app.SetValue(tszNeuralNetFile, m_strNeuralNetFilePath);
	}
*/
}