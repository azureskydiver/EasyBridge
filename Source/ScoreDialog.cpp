//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// ScoreDialog.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "ScoreDialog.h"
#include "subclass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScoreDialog dialog


CScoreDialog::CScoreDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CScoreDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScoreDialog)
	m_strBonusPoints = _T("");
	m_strTrickPoints = _T("");
	m_strTotalPoints = _T("");
	//}}AFX_DATA_INIT
//	m_bDisableGameControls = FALSE;
}


void CScoreDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScoreDialog)
	DDX_Text(pDX, IDC_BONUS_POINTS, m_strBonusPoints);
	DDX_Text(pDX, IDC_TRICK_POINTS, m_strTrickPoints);
	DDX_Text(pDX, IDC_TOTAL_POINTS, m_strTotalPoints);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScoreDialog, CDialog)
	//{{AFX_MSG_MAP(CScoreDialog)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScoreDialog message handlers


//
BOOL CScoreDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CenterWindow();	

	// subclass edit boxes to make them read-only without graying them
	// (kind of a hack)
	wpOrigEditROProc = (WNDPROC) SetWindowLong(GetDlgItem(IDC_BONUS_POINTS)->m_hWnd, GWL_WNDPROC, (LONG) EditROSubclassProc);
	SetWindowLong(GetDlgItem(IDC_TRICK_POINTS)->m_hWnd, GWL_WNDPROC, (LONG) EditROSubclassProc);
	SetWindowLong(GetDlgItem(IDC_TOTAL_POINTS)->m_hWnd, GWL_WNDPROC, (LONG) EditROSubclassProc);

/*
	//
	if (m_bDisableGameControls)
	{
		GetDlgItem(ID_REBID_HAND)->EnableWindow(FALSE);
		GetDlgItem(ID_REPLAY_HAND)->EnableWindow(FALSE);
	}
*/

	//
	ScrollToBottom((CRichEditCtrl&) *(GetDlgItem(IDC_BONUS_POINTS)));
	ScrollToBottom((CRichEditCtrl&) *(GetDlgItem(IDC_TRICK_POINTS)));
	
	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//
void CScoreDialog::OnOK() 
{
	EndDialog(TRUE);
}

/*
//
void CScoreDialog::OnReplayHand() 
{
	m_nCode = 1;
	EndDialog(FALSE);
}

//
void CScoreDialog::OnRebidHand() 
{
	m_nCode = 2;
	EndDialog(FALSE);
}
*/

//
void CScoreDialog::OnClose() 
{
	EndDialog(TRUE);
}


//
void CScoreDialog::Clear()
{
	m_strBonusPoints.Empty();
	m_strTrickPoints.Empty();
	UpdateData(FALSE);
}


//
void CScoreDialog::StartNewHand()
{
	m_strTrickPoints += "--------------------\n";
	UpdateData(FALSE);
}


//
void CScoreDialog::SetBonusPoints(CStringArray& strArrayText)
{
	int numLines = strArrayText.GetSize();
	for(int i=0;i<numLines;i++)
	{
		m_strBonusPoints += strArrayText.GetAt(i);
		if (i < numLines-1)
			m_strBonusPoints += _T("\r\n");
	}
}


//
void CScoreDialog::SetTrickPoints(CStringArray& strArrayText)
{
	int numLines = strArrayText.GetSize();
	for(int i=0;i<numLines;i++)
	{
		m_strTrickPoints += strArrayText.GetAt(i);
		if (i < numLines-1)
			m_strTrickPoints += _T("\r\n");
	}
}


//
void CScoreDialog::SetTotalPoints(CString& strText)
{
	m_strTotalPoints = strText;
}


//
void CScoreDialog::ScrollToBottom(CRichEditCtrl& edit) 
{
	RECT rect;
	edit.GetRect(&rect);
	int nSize = rect.bottom - rect.top;

	//
	CWindowDC dc(this);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);

	// scroll to bottom
	int nHeight = tm.tmHeight;
	int nVisibleRows = nSize / nHeight;
	int numLines = edit.GetLineCount();
	int nFirstLine = edit.GetFirstVisibleLine();
	if (numLines > nVisibleRows) 
	{
		int nScrollCount = numLines - nVisibleRows - nFirstLine;
		edit.LineScroll(nScrollCount);
	}

	// and set selection at the end of the text
	int nTextLength = edit.GetTextLength();
	edit.SetSel(nTextLength,nTextLength);
}
