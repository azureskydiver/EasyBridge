//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
//
// CMyBitmap
//
//
#include "stdafx.h"
#include "MyBitmap.h"



/////////////////////////////////////////////////////////////////////
//
// Construction
//
/////////////////////////////////////////////////////////////////////

//
CMyBitmap::CMyBitmap(int nResourceID)
{
	LoadBitmap(nResourceID);
}

//
CMyBitmap::~CMyBitmap()
{
	if (m_pPalette)
		m_pPalette->DeleteObject();
	delete m_pPalette;
}




/////////////////////////////////////////////////////////////////////
//
// Operations
//
/////////////////////////////////////////////////////////////////////


//
// LoadBitmap()
//
// - this version uses the current App's module
//
BOOL CMyBitmap::LoadBitmap(int nResourceID)
{
	// load bitmap from current app's res file as a DIB
	return LoadBitmap(AfxGetApp()->m_hInstance, nResourceID);
}



//
BOOL CMyBitmap::LoadBitmap(HINSTANCE hInstance, int nResourceID)
{
	BITMAP bm;
	CString strResource;
	strResource.Format(_T("#%d"), nResourceID);

	// load bitmap from res file as a DIB
	HBITMAP hBitmap = LoadResourceBitmap(hInstance, strResource);
	if (!hBitmap)
		return FALSE;

	// then bitblt to our internal DDB object
	// first get bitmap dimensions
	::GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);

	//
	CWindowDC  screenDC(AfxGetMainWnd());
	CDC		memDC, cacheDC;
	CreateCompatibleBitmap(&screenDC, bm.bmWidth, bm.bmHeight);
	memDC.CreateCompatibleDC(&screenDC);
	cacheDC.CreateCompatibleDC(&screenDC);
	CPalette* pOldPalette1 = memDC.SelectPalette(m_pPalette, FALSE);
	memDC.RealizePalette();
	CPalette* pOldPalette2 = cacheDC.SelectPalette(m_pPalette, FALSE);
	cacheDC.RealizePalette();
	HBITMAP hOldBitmap1 = (HBITMAP) ::SelectObject(memDC.m_hDC, m_hObject);
	HBITMAP hOldBitmap2 = (HBITMAP) ::SelectObject(cacheDC.m_hDC, hBitmap);
	//
	memDC.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &cacheDC, 0, 0, SRCCOPY);

	// release resources
	::SelectObject(memDC.m_hDC, hOldBitmap1);
	::SelectObject(cacheDC.m_hDC, hOldBitmap2);
	memDC.SelectPalette(pOldPalette1, FALSE);
	cacheDC.SelectPalette(pOldPalette2, FALSE);
	memDC.DeleteDC();
	cacheDC.DeleteDC();
	::DeleteObject(hBitmap);
	//
	return TRUE;
}



//
// LoadResourceBitmap()
//
// loads a bitmap from the resource file and returns it as a 
// handle to a DIB + a new CPalette object
//
HBITMAP CMyBitmap::LoadResourceBitmap(HINSTANCE hInstance, LPCTSTR lpString)
{
    HRSRC  hRsrc;
    HGLOBAL hGlobal;
    HBITMAP hBitmapFinal = NULL;
    LPBITMAPINFOHEADER  lpbi;
    int iNumColors;

    if (hRsrc = FindResource(hInstance, lpString, RT_BITMAP))
	{
		hGlobal = LoadResource(hInstance, hRsrc);
		lpbi = (LPBITMAPINFOHEADER)LockResource(hGlobal);

		CWindowDC dc(AfxGetMainWnd());
		m_pPalette =  CreateDIBPalette ((LPBITMAPINFO)lpbi, &iNumColors);
		CPalette* pOldPalette = NULL;
		if (m_pPalette)
		{
			pOldPalette = dc.SelectPalette(m_pPalette, FALSE);
			dc.RealizePalette();
		}
		//
		hBitmapFinal = CreateDIBitmap(dc.m_hDC,
									  (LPBITMAPINFOHEADER)lpbi,
									  (LONG)CBM_INIT,
									  (LPSTR)lpbi + lpbi->biSize + iNumColors * sizeof(RGBQUAD), 
									  (LPBITMAPINFO)lpbi,
									  DIB_RGB_COLORS );
		UnlockResource(hGlobal);
		FreeResource(hGlobal);
		if (m_pPalette)
			dc.SelectPalette(pOldPalette, FALSE);
	}
	return (hBitmapFinal);
} 


//
CPalette* CMyBitmap::CreateDIBPalette(LPBITMAPINFO lpbmi, LPINT lpiNumColors) 
{ 
	LPBITMAPINFOHEADER  lpbi;
	LPLOGPALETTE     lpPal;
	HANDLE           hLogPal;
	CPalette*		 pPal;
	int              i;

	lpbi = (LPBITMAPINFOHEADER)lpbmi;
	if (lpbi->biBitCount <= 8)
		*lpiNumColors = (1 << lpbi->biBitCount);
	else
		*lpiNumColors = 0;  // No palette needed for 24 BPP DIB

	if (lpbi->biClrUsed > 0)
		*lpiNumColors = lpbi->biClrUsed;  // Use biClrUsed

	if (*lpiNumColors)
	{
		hLogPal = GlobalAlloc (GHND, sizeof (LOGPALETTE) +
							 sizeof (PALETTEENTRY) * (*lpiNumColors));
		lpPal = (LPLOGPALETTE) GlobalLock (hLogPal);
		lpPal->palVersion    = 0x300;
		lpPal->palNumEntries = *lpiNumColors;

		for (i = 0;  i < *lpiNumColors;  i++)
		{
			 lpPal->palPalEntry[i].peRed   = lpbmi->bmiColors[i].rgbRed;
			 lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
			 lpPal->palPalEntry[i].peBlue  = lpbmi->bmiColors[i].rgbBlue;
			 lpPal->palPalEntry[i].peFlags = 0;
		}
		pPal = new CPalette();
		pPal->CreatePalette(lpPal);
		GlobalUnlock (hLogPal);
		GlobalFree   (hLogPal);
	}
	return pPal;
} 

