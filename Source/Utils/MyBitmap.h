//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
//
// CMyBitmap.h
//
#ifndef __MY_BITMAP__
#define __MY_BITMAP__


class AFX_EXT_CLASS CMyBitmap : public CBitmap
{
// constructon
public:
	CMyBitmap(int nResourceID);
	CMyBitmap() {};
	~CMyBitmap();

// data
private:
	CPalette*	m_pPalette;

// operations
public:
	BOOL		LoadBitmap(int nResourceID);
	BOOL		LoadBitmap(HINSTANCE hInstance, int nResourceID);
	CPalette*	GetPalette() { return m_pPalette; }

//
private:
	HBITMAP LoadResourceBitmap(HINSTANCE hInstance, LPCTSTR lpString);
	CPalette* CreateDIBPalette(LPBITMAPINFO lpbmi, LPINT lpiNumColors);
};


#endif