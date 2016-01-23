//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// splash.cpp : implementation file
//
#include "stdafx.h"
#include "SplashWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplashWnd dialog

//BOOL CSplashWnd::Create(CWinApp* pApp, CWnd* pParent) :
//	m_hAppInstance(pApp->m_hInstance)
BOOL CSplashWnd::Create(CWnd* pParent)
{
	if (!CDialog::Create(IDD_SPLASH, pParent))
	{
		TRACE0("Warning: creation of CSplashWnd dialog failed\n");
		return FALSE;
	}
	return TRUE;
}


/*
CSplashWnd::CSplashWnd(CWnd* pParent)
	: CDialog(CSplashWnd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSplashWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
*/

void CSplashWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSplashWnd)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSplashWnd, CDialog)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_TIMER()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSplashWnd message handlers

BOOL CSplashWnd::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// check device caps
	HDC hDC = ::GetDC(NULL);
	int nPlanes = ::GetDeviceCaps(hDC, PLANES);
	int nBits = ::GetDeviceCaps(hDC, BITSPIXEL);
	::ReleaseDC(NULL, hDC);

	// resize depending on bitmap
	// load either the 16-color or 256-color splash screens
	HMODULE hModule = GetModuleHandle(_T("EasyBUtils.DLL"));
	if (nBits <= 8)
		m_bitmap.LoadBitmap(hModule, IDB_SPLASH16);
	else
		m_bitmap.LoadBitmap(hModule, IDB_SPLASH);
	ASSERT(m_bitmap.m_hObject != NULL);
	BITMAP bmInfo;
	m_bitmap.GetObject(sizeof(BITMAP),&bmInfo);
	SetWindowPos(&wndTopMost, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, SWP_NOMOVE);
								
	CenterWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CSplashWnd::OnTimer(UINT nIDEvent) 
{
	CDialog::OnTimer(nIDEvent);
	if (nIDEvent == m_timer) 
	{
		KillTimer(m_timer);	
		EndDialog(TRUE);
	}
}




void CSplashWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// get our own DC instead, with no restrictions
	CDC* pDC = GetDC();
/*
	// First draw 3-D border	
	CPen ltGrayPen4, dkGrayPen4,ltGrayPen6, dkGrayPen6;;
	ltGrayPen4.CreatePen(PS_SOLID, 4, RGB(255,255,255));
	dkGrayPen4.CreatePen(PS_SOLID, 4, GetSysColor(COLOR_BTNSHADOW));
	ltGrayPen6.CreatePen(PS_SOLID, 6, RGB(255,255,255));
	dkGrayPen6.CreatePen(PS_SOLID, 6, GetSysColor(COLOR_BTNSHADOW));
	RECT rect;
	GetWindowRect(&rect);
	ScreenToClient(&rect);
	CPen* pOldPen = pDC->SelectObject(&ltGrayPen4);
	//
	pDC->MoveTo(rect.right, rect.top);
	pDC->LineTo(rect.left, rect.top);
	pDC->LineTo(rect.left, rect.bottom);
	(void) pDC->SelectObject(&dkGrayPen4);
	pDC->MoveTo(rect.left, rect.bottom);
	pDC->LineTo(rect.right, rect.bottom);
	pDC->LineTo(rect.right, rect.top);
	//
	(void) pDC->SelectObject(&ltGrayPen6);
	pDC->MoveTo(rect.right-4, rect.top+4);
	pDC->LineTo(rect.right-4, rect.bottom-4);
	pDC->LineTo(rect.left+4, rect.bottom-4);
	(void) pDC->SelectObject(&dkGrayPen6);
	pDC->LineTo(rect.left+4, rect.top+4);
	pDC->LineTo(rect.right-4, rect.top+4);
	//
	pDC->SelectObject(pOldPen);
	ltGrayPen4.DeleteObject();		
	dkGrayPen4.DeleteObject();		
	ltGrayPen6.DeleteObject();		
	dkGrayPen6.DeleteObject();		
*/
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
	m_bitmap.DeleteObject();
	newDC.DeleteDC();
	pDC->SelectPalette(pOldPalette, FALSE);
	ReleaseDC(pDC);
}

//
void CSplashWnd::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	//
	delete this;
}
