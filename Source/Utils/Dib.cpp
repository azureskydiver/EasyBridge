//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// dib.cpp : implementation file
//
//

#include "stdafx.h"
#include "dib.h"
#include "malloc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDIB

IMPLEMENT_SERIAL(CDIB, CObject, 0 /* Schema number */ )

// Create a small DIB here so m_pBMI and m_pBits are always valid.
CDIB::CDIB()
{
    m_pBMI = NULL;
    m_pBits = NULL;
	m_palDIB = NULL;
    Create(16, 16);
}

CDIB::~CDIB()
{
    // Free the memory.
    if (m_pBMI != NULL) free(m_pBMI);
    if (m_pBits != NULL) free(m_pBits);
	if (m_palDIB) delete m_palDIB;
}

/////////////////////////////////////////////////////////////////////////////
// CDIB serialization

// We don't support this yet.  
void CDIB::Serialize(CArchive& ar)
{
    ASSERT(1);
}

/////////////////////////////////////////////////////////////////////////////
// Private functions

static BOOL IsWinDIB(BITMAPINFOHEADER* pBIH)
{
    ASSERT(pBIH);
    if (((BITMAPCOREHEADER*)pBIH)->bcSize == sizeof(BITMAPCOREHEADER))
        return FALSE;
    return TRUE;
}

static int NumDIBColorEntries(LPBITMAPINFO pBmpInfo) 
{
    BITMAPINFOHEADER* pBIH;
    BITMAPCOREHEADER* pBCH;
    int iColors, iBitCount;

    ASSERT(pBmpInfo);

    pBIH = &(pBmpInfo->bmiHeader);
    pBCH = (BITMAPCOREHEADER*) pBIH;

    // Start off by assuming the color table size from
    // the bit-per-pixel field.
    if (IsWinDIB(pBIH))
        iBitCount = pBIH->biBitCount;
    else
        iBitCount = pBCH->bcBitCount;

    switch (iBitCount) 
	{
		case 1:
			iColors = 2;
			break;
		case 4:
			iColors = 16;
			break;
		case 8:
			iColors = 256;
			break;
		default:
			iColors = 0;
			break;
    }

    // If this is a Windows DIB, then the color table length
    // is determined by the biClrUsed field if the value in
    // the field is nonzero.
    if (IsWinDIB(pBIH) && (pBIH->biClrUsed != 0))
        iColors = pBIH->biClrUsed;

    return iColors;
}


/////////////////////////////////////////////////////////////////////////////
// CDIB commands

// Create a new empty 8bpp DIB with a 256 entry color table.
BOOL CDIB::Create(int iWidth, int iHeight)
{
    // Delete any existing stuff.
    if (m_pBMI != NULL) free(m_pBMI);
    if (m_pBits != NULL) free(m_pBits);

    // Allocate memory for the header.
    m_pBMI = (BITMAPINFO*) malloc(sizeof(BITMAPINFOHEADER)
                                  + 256 * sizeof(RGBQUAD));
    if (!m_pBMI)
	{
        TRACE("Out of memory for DIB header");
        return FALSE;
    }

    // Allocate memory for the bits (DWORD aligned).
    int iBitsSize = ((iWidth + 3) & ~3) * iHeight;
    m_pBits = (BYTE*)malloc(iBitsSize);
    if (!m_pBits) 
	{
        TRACE("Out of memory for DIB bits");
        free(m_pBMI);
        m_pBMI = NULL;
        return FALSE;
    }

    // Fill in the header info.
    BITMAPINFOHEADER* pBI = (BITMAPINFOHEADER*) m_pBMI;
    pBI->biSize = sizeof(BITMAPINFOHEADER);
    pBI->biWidth = iWidth;
    pBI->biHeight = iHeight;
    pBI->biPlanes = 1;
    pBI->biBitCount = 8;
    pBI->biCompression = BI_RGB;
    pBI->biSizeImage = 0;
    pBI->biXPelsPerMeter = 0;
    pBI->biYPelsPerMeter = 0;
    pBI->biClrUsed = 0;
    pBI->biClrImportant = 0;

    // Create an arbitrary color table (gray scale).
    RGBQUAD* prgb = GetClrTabAddress();
    for (int i = 0; i < 256; i++) 
	{
        prgb->rgbBlue = prgb->rgbGreen = prgb->rgbRed = (BYTE) i;
        prgb->rgbReserved = 0;
        prgb++;
    }

    // Set all the bits to a known state (black).
    memset(m_pBits, 0, iBitsSize);

    return TRUE;
}


// Load a DIB from an open file.
BOOL CDIB::Load(const CString& strFile)
{
    BOOL bIsPM = FALSE;
    BITMAPINFO* pBmpInfo = NULL;
    BYTE* pBits = NULL;
	CFile file;
	CFileException fileException;

	// try to open specified file
	if (!file.Open(strFile, 
				  CFile::modeRead | CFile::shareDenyWrite, 
				  &fileException))
		return FALSE;

    // Get the current file position.
    ULONGLONG ullFileStart = file.GetPosition();

    // Read the file header to get the file size and to
    // find out where the bits start in the file.
    BITMAPFILEHEADER BmpFileHdr;
    int iBytes;
    iBytes = file.Read(&BmpFileHdr, sizeof(BmpFileHdr));
    if (iBytes != sizeof(BmpFileHdr)) 
	{
        TRACE("Failed to read file header");
        goto $abort;
    }

    // Check that we have the magic 'BM' at the start.
    if (BmpFileHdr.bfType != 0x4D42) 
	{
        TRACE("Not a bitmap file");
        goto $abort;
    }

    // Make a wild guess that the file is in Windows DIB
    // format and read the BITMAPINFOHEADER. If the file turns
    // out to be a PM DIB file we'll convert it later.
    BITMAPINFOHEADER BmpInfoHdr;
    iBytes = file.Read(&BmpInfoHdr, sizeof(BmpInfoHdr)); 
    if (iBytes != sizeof(BmpInfoHdr)) 
	{
        TRACE("Failed to read BITMAPINFOHEADER");
        goto $abort;
    }

    // Check that we got a real Windows DIB file.
    if (BmpInfoHdr.biSize != sizeof(BITMAPINFOHEADER)) 
	{
        if (BmpInfoHdr.biSize != sizeof(BITMAPCOREHEADER)) 
		{
            TRACE(" File is not Windows or PM DIB format");
            goto $abort;
        }

        // Set a flag to convert PM file to Win format later.
        bIsPM = TRUE;

        // Back up the file pointer and read the BITMAPCOREHEADER
        // and create the BITMAPINFOHEADER from it.
        file.Seek(ullFileStart + sizeof(BITMAPFILEHEADER), CFile::begin);
        BITMAPCOREHEADER BmpCoreHdr;
        iBytes = file.Read(&BmpCoreHdr, sizeof(BmpCoreHdr)); 
        if (iBytes != sizeof(BmpCoreHdr)) 
		{
            TRACE("Failed to read BITMAPCOREHEADER");
            goto $abort;
        }

        BmpInfoHdr.biSize = sizeof(BITMAPINFOHEADER);
        BmpInfoHdr.biWidth = (int) BmpCoreHdr.bcWidth;
        BmpInfoHdr.biHeight = (int) BmpCoreHdr.bcHeight;
        BmpInfoHdr.biPlanes = BmpCoreHdr.bcPlanes;
        BmpInfoHdr.biBitCount = BmpCoreHdr.bcBitCount;
        BmpInfoHdr.biCompression = BI_RGB;
        BmpInfoHdr.biSizeImage = 0;
        BmpInfoHdr.biXPelsPerMeter = 0;
        BmpInfoHdr.biYPelsPerMeter = 0;
        BmpInfoHdr.biClrUsed = 0;
        BmpInfoHdr.biClrImportant = 0;
    }

    // Work out how much memory we need for the BITMAPINFO
    // structure, color table and then for the bits.  
    // Allocate the memory blocks.
    // Copy the BmpInfoHdr we have so far,
    // and then read in the color table from the file.
    int iColors;
    int iColorTableSize;
    iColors = NumDIBColorEntries((LPBITMAPINFO) &BmpInfoHdr);
	//
	m_numColors = iColors;
	//
    iColorTableSize = iColors * sizeof(RGBQUAD);
    int iBitsSize;
    int iBISize;
    // always allocate enough room for 256 entries
    iBISize = sizeof(BITMAPINFOHEADER)    
           + 256 * sizeof(RGBQUAD);
    iBitsSize = BmpFileHdr.bfSize - 
                BmpFileHdr.bfOffBits;

    // Allocate the memory for the header.
    pBmpInfo = (LPBITMAPINFO) malloc(iBISize);
    if (!pBmpInfo) 
	{
        TRACE("Out of memory for DIB header");
        goto $abort;
    }
    memset(pBmpInfo, 0, iBISize);

    // Copy the header we already have.
    memcpy(pBmpInfo, &BmpInfoHdr, sizeof(BITMAPINFOHEADER));

    // Now read the color table in from the file.
    if (bIsPM == FALSE) 
	{
        // Read the color table from the file.
        iBytes = file.Read(((LPBYTE) pBmpInfo) + sizeof(BITMAPINFOHEADER),
                             iColorTableSize);
        if (iBytes != iColorTableSize) 
		{
            TRACE("Failed to read color table");
            goto $abort;
        }
    } 
	else 
	{
        // Read each PM color table entry in turn and convert it
        // to Win DIB format as we go.
        LPRGBQUAD lpRGB;
        lpRGB = (LPRGBQUAD) ((LPBYTE) pBmpInfo + sizeof(BITMAPINFOHEADER));
        int i;
        RGBTRIPLE rgbt;
        for (i=0; i<iColors; i++) 
		{
            iBytes = file.Read(&rgbt, sizeof(RGBTRIPLE));
            if (iBytes != sizeof(RGBTRIPLE)) 
			{
                TRACE("Failed to read RGBTRIPLE");
                goto $abort;
            }
            lpRGB->rgbBlue = rgbt.rgbtBlue;
            lpRGB->rgbGreen = rgbt.rgbtGreen;
            lpRGB->rgbRed = rgbt.rgbtRed;
            lpRGB->rgbReserved = 0;
            lpRGB++;
        }
    }

    // Allocate the memory for the bits
    // and read the bits from the file.
    pBits = (BYTE*) malloc(iBitsSize);
    if (!pBits) 
	{
        TRACE("Out of memory for DIB bits");
        goto $abort;
    }

    // Seek to the bits in the file.
    file.Seek(ullFileStart + BmpFileHdr.bfOffBits, CFile::begin);

    // read the bits
    iBytes = file.Read(pBits, iBitsSize);
    if (iBytes != iBitsSize) 
	{
        TRACE("Failed to read bits");
        goto $abort;
    }

    // Everything went OK.
    if (m_pBMI != NULL) free(m_pBMI);
    m_pBMI = pBmpInfo; 
    if (m_pBits != NULL) free(m_pBits);
    m_pBits = pBits;
	// now close the file
	file.Close();
	// and create a palette
	if (m_palDIB)
		delete m_palDIB;
	m_palDIB = CreateDIBPalette();
	// all done
    return TRUE;
                
$abort: // Something went wrong.
    if (pBmpInfo) free(pBmpInfo);
    if (pBits) free(pBits);
    return FALSE;    
}


// Draw the DIB to a given DC.
void CDIB::Draw(CDC* pDC, RECT& destRect, int x, int y, BOOL bShrinkToFit)
{
	CPalette* pOldPal;
	if (m_palDIB)
		pOldPal = (CPalette*) pDC->SelectPalette(m_palDIB, TRUE);
	//
	int nSrcWidth = DibWidth();
	int nSrcHeight = DibHeight();
	if ((nSrcWidth >= destRect.right) || (nSrcHeight >= destRect.bottom)) 
	{
		// scale if desired
		if (bShrinkToFit)
		{
			double fScale = 1.0;
			if (nSrcWidth >= destRect.right)
			{
				fScale = (double) destRect.right / nSrcWidth;
			}
			if (nSrcHeight >= destRect.bottom)
			{
				double fVScale = (double) destRect.bottom / nSrcHeight;
				if (fVScale < fScale)
					fScale = fVScale;
			}
			int nCX = (int) (nSrcWidth * fScale);
			int nCY = (int) (nSrcHeight * fScale);
			int nXPos = (destRect.right - nCX) / 2;
			int nYPos = (destRect.bottom - nCY) / 2;
			::StretchDIBits(pDC->GetSafeHdc(),
    						nXPos,                    // Destination x
        					nYPos,                    // Destination y
            				nCX,					  // scaled width
                			nCY,					  // scaled height
                    		0,                        // Source x
							0,                        // Source y
    						nSrcWidth,                // Source width
        					nSrcHeight,               // Source height
            				GetBitsAddress(),         // Pointer to bits
                			GetBitmapInfoAddress(),   // BITMAPINFO
        					DIB_RGB_COLORS,           // Options
    						SRCCOPY);                 // Raster operator code (ROP)
		}
		else
		{
			::StretchDIBits(pDC->GetSafeHdc(),
    						x,                        // Destination x
        					y,                        // Destination y
	            	        nSrcWidth,                // Destination width
	                	    nSrcHeight,               // Destination height
                    		0,                        // Source x
							0,                        // Source y
    						nSrcWidth,                // Source width
        					nSrcHeight,               // Source height
            				GetBitsAddress(),         // Pointer to bits
                			GetBitmapInfoAddress(),   // BITMAPINFO
        					DIB_RGB_COLORS,           // Options
    						SRCCOPY);                 // Raster operator code (ROP)
		}
	} 
	else 
	{
		if ((x != 0) || (y != 0))
		{
			// draw at the specified offset
			::StretchDIBits(pDC->GetSafeHdc(),
    						x,				          // Destination x
        	    			y,		                  // Destination y
		            		nSrcWidth,                // Destination width
        		        	nSrcHeight,               // Destination height
                		    0,                        // Source x
	                    	0,                        // Source y
		    				nSrcWidth,                // Source width
        					nSrcHeight,               // Source height
            				GetBitsAddress(),         // Pointer to bits
                	    	GetBitmapInfoAddress(),   // BITMAPINFO
		        			DIB_RGB_COLORS,           // Options
    						SRCCOPY);                 // Raster operator code (ROP)
		}
		else
		{
			// tile bitmap
			int nOffsetX,nOffsetY;
			int i,j;
			nOffsetY = 0;
			for(i=0;i<(destRect.bottom/nSrcHeight)+1;i++) 
			{
				nOffsetX = 0;
				for(j=0;j<(destRect.right/nSrcWidth)+1;j++) 
				{
					::StretchDIBits(pDC->GetSafeHdc(),
    								nOffsetX,                 // Destination x
        	    					nOffsetY,                 // Destination y
		            				nSrcWidth,                // Destination width
        		        			nSrcHeight,               // Destination height
                		    		0,                        // Source x
	                    			0,                        // Source y
		    						nSrcWidth,                // Source width
        							nSrcHeight,               // Source height
            						GetBitsAddress(),         // Pointer to bits
                	    			GetBitmapInfoAddress(),   // BITMAPINFO
		        					DIB_RGB_COLORS,           // Options
    								SRCCOPY);                 // Raster operator code (ROP)
					nOffsetX += nSrcWidth;
				}
				nOffsetY += nSrcHeight;
			}
		}
	}
	//
	if (m_palDIB)
		pDC->SelectPalette(pOldPal, TRUE);
}



#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))

//
CPalette* CDIB::CreateDIBPalette()
{
	HANDLE hLogPal;
	LPLOGPALETTE lpPal;
	CPalette* pPalette = new CPalette;
	LPBITMAPINFO lpbmi = (LPBITMAPINFO) m_pBMI;
	LPBITMAPCOREINFO lpbmc = (LPBITMAPCOREINFO) m_pBMI;
	//
	if (m_numColors != 0) 
	{
		// allocate memory block for logical palette
		hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE)
						+ sizeof(PALETTEENTRY)*m_numColors);
		// if not enough memory, clean up and return NULL
		if (hLogPal == 0)
			return NULL;
		//
		lpPal = (LPLOGPALETTE)::GlobalLock((HGLOBAL) hLogPal);
		lpPal->palVersion = 0x300;
		lpPal->palNumEntries = (WORD)m_numColors;

		// is this a Win 3.0 DIB?
		BOOL bWinStyleDIB = IS_WIN30_DIB(m_pBMI);
		int i;
		for (i=0;i<(int)m_numColors;i++) 
		{
			if (bWinStyleDIB) 
			{
				lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
				lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
				lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
				lpPal->palPalEntry[i].peFlags = 0;
			} 
			else 
			{
				lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
				lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
				lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
				lpPal->palPalEntry[i].peFlags = 0;
			}
		}
		// finally create the palette and get handle to it
		BOOL bResult = pPalette->CreatePalette(lpPal);
		::GlobalUnlock((HGLOBAL) hLogPal);
		::GlobalFree((HGLOBAL) hLogPal);
	}
	return pPalette;
}

