//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// StatusSheet.cpp : implementation file
//

#include "stdafx.h"
#include "EasyB.h"
#include "Mainfrm.h"
#include "StatusSheet.h"
#include "StatusWnd.h"
#include "progopts.h"
#include "Help\helpcode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define FEEDBACK_PAGE

const RECT tnRectTabMargin = {
	4, 5, 3, 3,
};

CImageList	CStatusSheet::m_imageList;


/////////////////////////////////////////////////////////////////////////////
// CStatusSheet

IMPLEMENT_DYNAMIC(CStatusSheet, CPropertySheet)


CStatusSheet::CStatusSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_analysesPage);
	AddPage(&m_playPlanPage);
	AddPage(&m_cardLocationsPage);
	AddPage(&m_holdingsPage);
#ifndef DISABLE_GIB
	AddPage(&m_gibMonitorPage);
#endif
#ifdef FEEDBACK_PAGE
	AddPage(&m_feedbackPage);
#endif
	//
	m_bInitialized = FALSE;
}

CStatusSheet::~CStatusSheet()
{
}


BEGIN_MESSAGE_MAP(CStatusSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CStatusSheet)
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusSheet message handlers


//
BOOL CStatusSheet::OnInitDialog() 
{
	BOOL bCode = CPropertySheet::OnInitDialog();

	// create the suits imagelist
	// init the image list
	m_imageList.Create(IDB_SUITS_IMAGELIST, 11, 0, RGB(255,255,255));

	// record the margins associated with a property sheet/page
	CRect clientRect, pageRect;
	GetClientRect(&clientRect);
	GetPage(0)->GetWindowRect(&pageRect);
	ScreenToClient(&pageRect);
	m_sheetMargins.left = pageRect.left - clientRect.left;
	m_sheetMargins.top = pageRect.top - clientRect.top;
	m_sheetMargins.right = clientRect.right - pageRect.right;
	m_sheetMargins.bottom = clientRect.bottom - pageRect.bottom;
	// resize the tab control
	clientRect.DeflateRect(&tnRectTabMargin);
	GetTabControl()->SetWindowPos(NULL, clientRect.left, clientRect.top,
								  clientRect.Width(), clientRect.Height(),
								  SWP_NOZORDER | SWP_NOACTIVATE);
	// done
	m_bInitialized = TRUE;
	return bCode;
}


//
BOOL CStatusSheet::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch(wParam)
	{
		case WMS_SETTEXT:
		case WMS_SET_FEEDBACK_TEXT:
#ifdef FEEDBACK_PAGE
			if (m_feedbackPage.IsInitialized())
				return m_feedbackPage.SendMessage(WM_COMMAND, wParam, lParam);
			else
#endif
				return TRUE;

		case WMS_SET_GIB_TEXT:
#ifndef DISABLE_GIB
			if (m_gibMonitorPage.IsInitialized())
				return m_gibMonitorPage.SendMessage(WM_COMMAND, wParam, lParam);
			else
#endif
				return TRUE;

	}
	//
	return CPropertySheet::OnCommand(wParam, lParam);
}



//
void CStatusSheet::Update(int nPage)
{
	// don't update if in express play mode
	if (theApp.InExpressAutoPlay())
		return;

	//
	if (nPage < 0)
		nPage = CStatusWnd::SP_ALL;

	// update appropriate pages
	if (nPage & CStatusWnd::SP_ANALYSES)
		m_analysesPage.Update();
	
	if (nPage & CStatusWnd::SP_CARD_LOCATIONS)
		m_cardLocationsPage.Update();
	//
	if (nPage & CStatusWnd::SP_PLAY_PLAN)
		m_playPlanPage.Update();
	//
	if (nPage & CStatusWnd::SP_SUIT_STATUS)
		m_holdingsPage.Update();
	//
#ifndef DISABLE_GIB
	if (nPage & CStatusWnd::SP_FEEDBACK)
		m_gibMonitorPage.Update();
#endif
	//
#ifdef FEEDBACK_PAGE
	if (nPage & CStatusWnd::SP_FEEDBACK)
		m_feedbackPage.Update();
#endif
}



//
void CStatusSheet::Clear()
{
	// clear all
	m_analysesPage.Clear();
	m_cardLocationsPage.Clear();
	m_playPlanPage.Clear();
	m_holdingsPage.Clear();
#ifndef DISABLE_GIB
	m_gibMonitorPage.Clear();
#endif
#ifdef FEEDBACK_PAGE
	m_feedbackPage.Clear();
#endif
}


//
void CStatusSheet::OnSize(UINT nType, int cx, int cy) 
{
	CPropertySheet::OnSize(nType, cx, cy);
	//
	if (!m_bInitialized)
		return;
	// first resize the tab control
	CRect clientRect;
	GetClientRect(&clientRect);
	clientRect.DeflateRect(&tnRectTabMargin);
	CTabCtrl* pTabCtrl = GetTabControl();
	pTabCtrl->SetWindowPos(NULL, clientRect.left, clientRect.top,
						   clientRect.Width(), clientRect.Height(),
						   SWP_NOZORDER | SWP_NOACTIVATE);
	// now resize the property pages themselves
	// determine margins around the property pages
	const int tnMargin = 4;
	pTabCtrl->GetClientRect(&clientRect);
	pTabCtrl->AdjustRect(FALSE, &clientRect);
	CRect childRect;
	CPropertyPage* pFirstPage = GetPage(0);
	for(int i=0;i<(int)m_psh.nPages;i++)
	{
		CPropertyPage* pPage = GetPage(i);
		if (pPage->GetSafeHwnd())
		{
			pPage->SetWindowPos(NULL, 
							    clientRect.left + tnMargin, clientRect.top + tnMargin,
								clientRect.Width() - 1, 
								clientRect.Height() - 1,
								SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}
}


//
void CStatusSheet::OnDestroy() 
{
	m_imageList.DeleteImageList();
	CPropertySheet::OnDestroy();
}

	
//
void CStatusSheet::PostNcDestroy() 
{
	CPropertySheet::PostNcDestroy();
	delete this;
}


// 
// OnKeyDown()
//
// pass along to the parent
//
void CStatusSheet::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//	CPropertySheet::OnKeyDown(nChar, nRepCnt, nFlags);
//	GetParent()->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
	pMAINFRAME->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
}




BOOL CStatusSheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	int nPageIndex = GetActiveIndex();
	int nHelpContextID = DIALOG_STATUS_PLAY_PLAN_PAGE + nPageIndex;
	WinHelp(nHelpContextID);
	return TRUE;	
//	return CPropertySheet::OnHelpInfo(pHelpInfo);
}


//
void CStatusSheet::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// we don't do anything special here, so send it up to the parent
	ClientToScreen(&point);
	CWnd* pParent = GetParent();
	pParent->ScreenToClient(&point);
	pParent->SendMessage(WM_RBUTTONDOWN, nFlags, MAKELONG(point.x, point.y));
//	CPropertySheet::OnRButtonDown(nFlags, point);
}
