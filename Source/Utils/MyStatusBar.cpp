//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// MyStatusBar.cpp : implementation file
//

#include "stdafx.h"
#include "MyStatusBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyStatusBar

CMyStatusBar::CMyStatusBar()
{
	m_bIgnoreSetText = FALSE;
}

CMyStatusBar::~CMyStatusBar()
{
}


BEGIN_MESSAGE_MAP(CMyStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CMyStatusBar)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETTEXT, OnSetText) 
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMyStatusBar message handlers



//
//-------------------------------------------------------
//


LRESULT CMyStatusBar::OnSetText(WPARAM wParam, LPARAM lParam) 
{ 
	if (!m_bIgnoreSetText) 
		return CStatusBar::OnSetText( wParam, lParam ); 
	return 0;  // Same as CStatusBar::OnSetText success 
} 


void CMyStatusBar::LockFirstPane()
{
	m_bIgnoreSetText = TRUE;
}

void CMyStatusBar::UnlockFirstPane()
{
	m_bIgnoreSetText = FALSE;
}

