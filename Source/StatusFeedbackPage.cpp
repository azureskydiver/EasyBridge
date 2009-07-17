//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// StatusFeedbackPage.cpp : implementation file
//

#include "stdafx.h"
#include "EasyB.h"
#include "mainfrm.h"
#include "mainframeopts.h"
#include "StatusFeedbackPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusFeedbackPage property page

IMPLEMENT_DYNCREATE(CStatusFeedbackPage, CPropertyPage)

CStatusFeedbackPage::CStatusFeedbackPage() : CPropertyPage(CStatusFeedbackPage::IDD)
{
	//{{AFX_DATA_INIT(CStatusFeedbackPage)
	m_strText = _T("");
	//}}AFX_DATA_INIT
	m_bInitialized = FALSE;
}

CStatusFeedbackPage::~CStatusFeedbackPage()
{
}

void CStatusFeedbackPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatusFeedbackPage)
	DDX_Text(pDX, IDC_STATIC1, m_strText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatusFeedbackPage, CPropertyPage)
	//{{AFX_MSG_MAP(CStatusFeedbackPage)
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusFeedbackPage message handlers


//
BOOL CStatusFeedbackPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_bInitialized = TRUE;
	Update();
	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



//
BOOL CStatusFeedbackPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch(wParam) 
	{
		case WMS_SET_FEEDBACK_TEXT:
			m_strText = (LPCTSTR)lParam;	
			UpdateData(FALSE);
			return TRUE;
	}
	//			
	return CPropertyPage::OnCommand(wParam, lParam);
}


//
void CStatusFeedbackPage::Update()
{
	if (!m_bInitialized)
		return;
	m_strText = (char *)pMAINFRAME->GetValue(tszFeedbackText);  // NCR added (char *)
	UpdateData(FALSE);
}


//
void CStatusFeedbackPage::Clear()
{
	if (!m_bInitialized)
		return;
	m_strText.Empty();	
	UpdateData(FALSE);
}


//
// OnKeyDown()
//
// pass along to the parent
//
void CStatusFeedbackPage::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//	CPropertyPage::OnKeyDown(nChar, nRepCnt, nFlags);
//	GetParent()->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
	pMAINFRAME->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
}


//
void CStatusFeedbackPage::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// we don't do anything special here, so send it up to the parent
	ClientToScreen(&point);
	CWnd* pParent = GetParent();
	pParent->ScreenToClient(&point);
	pParent->SendMessage(WM_RBUTTONDOWN, nFlags, MAKELONG(point.x, point.y));
}
