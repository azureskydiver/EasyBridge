//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// CNNetconfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "NNetConfigDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//
const int tnumNodesPerLayer[] = {
	50, 100, 200, 300, 400,
};
const int nodeArraySize = sizeof(tnumNodesPerLayer) / sizeof(int);



/////////////////////////////////////////////////////////////////////////////
// CNNetConfigDialog dialog


CNNetConfigDialog::CNNetConfigDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNNetConfigDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNNetConfigDialog)
	m_numLayersSel = 1;
	m_numNodesPerLayerSel = 1;
	//}}AFX_DATA_INIT

	//
	m_numNNetHiddenLayers = m_numLayersSel + 1;
	for(int i=0;i<nodeArraySize;i++)
	{
		if (tnumNodesPerLayer[i] == m_numNNetNodesPerHiddenLayer)
			break;
	}
	if (i < nodeArraySize)
		m_numNodesPerLayerSel = i;
	else
		m_numNodesPerLayerSel = 0;
}


void CNNetConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNNetConfigDialog)
	DDX_Radio(pDX, IDC_LAYERS0, m_numLayersSel);
	DDX_Radio(pDX, IDC_NODES_PER_LAYER, m_numNodesPerLayerSel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNNetConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CNNetConfigDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNNetConfigDialog message handlers

BOOL CNNetConfigDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



//
void CNNetConfigDialog::OnOK() 
{
	UpdateData(TRUE);
	m_numNNetHiddenLayers = m_numLayersSel + 1;
	m_numNNetNodesPerHiddenLayer = tnumNodesPerLayer[m_numNodesPerLayerSel];
	//
	EndDialog(IDOK);
}



//
void CNNetConfigDialog::OnCancel() 
{
	EndDialog(IDCANCEL);
}
