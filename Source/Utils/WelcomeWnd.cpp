//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Welcome.cpp : implementation file
//
#include "stdafx.h"
#include "WelcomeWnd.h"
#include "..\help\helpcode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWelcomeWnd dialog

//BOOL CWelcomeWnd::Create(CWinApp* pApp, CWnd* pParent) :
//	m_hAppInstance(pApp->m_hInstance)
BOOL CWelcomeWnd::Create(CWnd* pParent)
{
	if (!CDialog::Create(IDD_WELCOME, pParent))
	{
		TRACE0("Warning: creation of CWelcomeWnd dialog failed\n");
		return FALSE;
	}
	return TRUE;
}


/*
CWelcomeWnd::CWelcomeWnd(CWnd* pParent)
	: CDialog(CWelcomeWnd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWelcomeWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
*/

void CWelcomeWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWelcomeWnd)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWelcomeWnd, CDialog)
	//{{AFX_MSG_MAP(CWelcomeWnd)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWelcomeWnd message handlers

BOOL CWelcomeWnd::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// check device caps
	HDC hDC = ::GetDC(NULL);
	int nPlanes = ::GetDeviceCaps(hDC, PLANES);
	int nBits = ::GetDeviceCaps(hDC, BITSPIXEL);
	::ReleaseDC(NULL, hDC);

	// resize depending on bitmap
	// load either the 16-color or 256-color Welcome screens
	HMODULE hModule = GetModuleHandle(_T("EasyBUtils.DLL"));
	if (nBits <= 8)
		m_bitmap.LoadBitmap(hModule, IDB_WELCOME_BITMAP_16);
	else
		m_bitmap.LoadBitmap(hModule, IDB_WELCOME_BITMAP);
	ASSERT(m_bitmap.m_hObject != NULL);
	BITMAP bmInfo;
	m_bitmap.GetObject(sizeof(BITMAP),&bmInfo);
//	SetWindowPos(&wndTopMost, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, SWP_NOMOVE);
	SetWindowPos(&wndTop, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, SWP_NOMOVE);

	//
	CenterWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CWelcomeWnd::OnClose() 
{
	m_bitmap.DeleteObject();
	//	
	CDialog::OnClose();
}


//
void CWelcomeWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// get our own DC instead, with no restrictions
	CDC* pDC = GetDC();

	// realize the palette
	CPalette* pOldPalette = pDC->SelectPalette(m_bitmap.GetPalette(), FALSE);
	pDC->RealizePalette();

	// And draw the bitmap
	BITMAP bmInfo;
	if (m_bitmap.m_hObject == NULL)
		return;
	m_bitmap.GetObject(sizeof(BITMAP),&bmInfo);
	CDC newDC;
	newDC.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = newDC.SelectObject(&m_bitmap);
	pDC->BitBlt(0,0,bmInfo.bmWidth,bmInfo.bmHeight,&newDC,0,0,SRCCOPY);

	//
	(void)newDC.SelectObject(pOldBitmap);
	newDC.DeleteDC();
	pDC->SelectPalette(pOldPalette, FALSE);
	ReleaseDC(pDC);

	// draw the 'OK' button
	CRect winRect, rect;
	GetClientRect(&winRect);
	CButton* pBtn = (CButton*) GetDlgItem(IDOK);
	pBtn->GetWindowRect(&rect);
	ScreenToClient(&rect);
	int nWidth = rect.Width();
	int nHeight = rect.Height();
	rect.bottom = winRect.bottom - 24;
	rect.top = rect.bottom - nHeight;
	rect.left = (winRect.right - nWidth) / 2;
	rect.right = rect.left + nWidth;
	pBtn->MoveWindow(&rect);
	pBtn->ShowWindow(SW_SHOW);
}

//
void CWelcomeWnd::OnOK() 
{
	EndDialog(TRUE);
	//
	if (m_bShowRulesHelp)
		WinHelp(HIDT_INTRODUCTION);
}

//
void CWelcomeWnd::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	//
	delete this;
}

