//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// NNetOutputDialog.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "easybDoc.h"
#include "Mainfrm.h"
#include "Mainframeopts.h"
#include "NNetOutputDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//
const int tnumDefaultBiddingOutputs = 38;



/////////////////////////////////////////////////////////////////////////////
// CNNetOutputDialog dialog


CNNetOutputDialog::CNNetOutputDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNNetOutputDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNNetOutputDialog)
	m_strError = _T("");
	//}}AFX_DATA_INIT

	// clear data
	for(int i=0;i<38;i++)
		m_fOutputs[i] = 0;
	m_nWinner = -1;
	m_numTrainingCycles = -1;
	m_bInitialized = FALSE;
}


void CNNetOutputDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNNetOutputDialog)
	DDX_Text(pDX, IDC_ERROR, m_strError);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNNetOutputDialog, CDialog)
	//{{AFX_MSG_MAP(CNNetOutputDialog)
	ON_WM_MOVE()
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNNetOutputDialog message handlers




//
// OnInitDialog() 
//
BOOL CNNetOutputDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// 
	m_bInitialized = TRUE;
	Clear();

	// done
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




//
// ShowResults()
//
void CNNetOutputDialog::ShowResults(NVALUE* fOutputs, int nWinner)
{
	if (!m_bInitialized || !IsWindowVisible())
		return;

	// copy data
	for(int i=0;i<38;i++)
		m_fOutputs[i] = fOutputs[i];
	m_nWinner = nWinner;

	// and refresh
	Invalidate();
}



//
// ShowError()
//
void CNNetOutputDialog::ShowError(double fError)
{
	m_strError.Format("%.8f", fError);
	UpdateData(FALSE);
}

//
// Clear()
//
void CNNetOutputDialog::Clear()
{
	if (!m_bInitialized || !IsWindowVisible())
		return;

	// copy data
	for(int i=0;i<38;i++)
		m_fOutputs[i] = 0;
	m_nWinner = -1;

	// clear error
	m_strError = _T("");
	UpdateData(FALSE);

	// and refresh
	Invalidate();

/*
	//
	CRect rect;
	// iterate
	for(int i=0;i<tnumDefaultBiddingOutputs;i++)
	{
		// get a dc into the static control
		CWnd* pWnd = GetDlgItem(IDC_OUTPUT0 + i);
		ASSERT(pWnd != NULL);
		CWindowDC dc(pWnd);
		pWnd->GetClientRect(&rect);

		// draw a rect filed with white
		dc.Rectangle(&rect);
	}
*/
}



void CNNetOutputDialog::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	
	//
	if (IsWindowVisible() && m_bInitialized)
	{
		RECT rect;
		GetWindowRect(&rect);
		pMAINFRAME->SetValue(tnNNetOutputDialogLeft, x);
		pMAINFRAME->SetValue(tnNNetOutputDialogTop, y);
	}
}


//
void CNNetOutputDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	//
	if (!IsWindowVisible() || !m_bInitialized)
		return;

	// prepare for drawing
	CRect rect, rectDefault;
	GetDlgItem(IDC_OUTPUT0)->GetClientRect(rectDefault);
	int nWidth = rectDefault.Width();
	nWidth -= 2;	// adjust for the border
	int nHeight = rectDefault.Height();
	int nLastValidBid = pDOC->GetLastValidBid();

	// iterate
	for(int i=0;i<tnumDefaultBiddingOutputs;i++)
	{
		// get a dc into the static control
		CWnd* pWnd = GetDlgItem(IDC_OUTPUT0 + i);
		ASSERT(pWnd != NULL);
		CWindowDC dc(pWnd);
		pWnd->GetClientRect(&rect);

		// draw a rect around the whole things
		dc.Rectangle(&rect);
		rect.DeflateRect(1, 1, 1, 1);

		// fill value filled in with dark grey
		int nVal = (int) (nWidth * m_fOutputs[i]);
		CRect valRect = rect;
		valRect.right = valRect.left + nVal;
		if (i == m_nWinner)
			dc.FillSolidRect(&valRect, RGB(0, 0, 255));
		else if ((i <= nLastValidBid) && (nLastValidBid != BID_PASS))
			dc.FillSolidRect(&valRect, RGB(192, 192, 192));
		else
			dc.FillSolidRect(&valRect, RGB(128, 128, 128));
	}

	// update training count
	if (m_numTrainingCycles >= 0)
		SetWindowText(FormString(_T("NNet Output (%d Training Cycles)"), m_numTrainingCycles));
	else
		SetWindowText("NNet Output");
}



//
void CNNetOutputDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	//
	if (bShow)
		Invalidate();
}



//
void CNNetOutputDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	pMAINFRAME->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
//	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}
