//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// dib.h : header file
//
// CDIB class
//

#ifndef __DIB__
#define __DIB__

class AFX_EXT_CLASS CDIB : public CObject
{
    DECLARE_SERIAL(CDIB)

public:
	// construction/destruction
    CDIB();
    ~CDIB();
	//
    virtual BOOL Create(int iWidth, int iHeight);	// Create a new DIB
    virtual BOOL Load(const CString& strFile);			// Load from file
    virtual void Serialize(CArchive& ar);
    virtual void Draw(CDC* pDC, RECT& rect, int x=0, int y=0, BOOL bShrinkToFit=FALSE);
	CPalette* CreateDIBPalette();
	// inline functions
    BITMAPINFO* GetBitmapInfoAddress()
        { return m_pBMI; }                        // Pointer to bitmap info
    BYTE* GetBitsAddress()
        { return m_pBits; }                       // Pointer to the bits
    RGBQUAD* GetClrTabAddress()
        { return (LPRGBQUAD)(((BYTE *)(m_pBMI)) 
            + sizeof(BITMAPINFOHEADER)); }       // Pointer to color table
	CPalette* GetPalette() { return m_palDIB; };
    int DibWidth() const { return m_pBMI->bmiHeader.biWidth; }
    int DibHeight() const { return m_pBMI->bmiHeader.biHeight; }

protected:
    BITMAPINFO* m_pBMI;         // Pointer to BITMAPINFO struct
    BYTE* m_pBits;              // Pointer to the bits
	CPalette* m_palDIB;
	int	m_numColors;

};

#endif // __DIB__
