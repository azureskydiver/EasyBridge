//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Print.cpp : printing routines for CEasyBView
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "DocOpts.h"
#include "mainfrm.h"
#include "EasyBvw.h"
#include "progopts.h"






/////////////////////////////////////////////////////////////////////////////
// CEasyBView printing


//
BOOL CEasyBView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	BOOL bCode = DoPreparePrinting(pInfo);
	//
	m_strPrintText.Empty();
	pInfo->SetMaxPage(1);		// defaults to 1 page only
	return bCode;
}



//
void CEasyBView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CView::OnBeginPrinting(pDC, pInfo);
}



//
void CEasyBView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CView::OnEndPrinting(pDC, pInfo);
}



//
void CEasyBView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
//	pInfo->m_bContinuePrinting = FALSE;		// only needed if doing real-time pagination
//	CView::OnPrepareDC(pDC, pInfo);
}



//
// PrintTextBlock()
//
int CEasyBView::PrintTextBlock(CDC& dc, const int nCharHeight, CRect& rectPage, int& nPageLine, CString strSource)
{
	// save print record
	m_strPrintText += strSource;

	// fit lines of text on page
	CString strText;
	while ( ((rectPage.bottom - rectPage.top) >= nCharHeight) && !strSource.IsEmpty())
	{
		int nIndex = 0;
		int nLength = strSource.GetLength();

		// retrieve the next line of the hand string
		while((nIndex < nLength) && (strSource[nIndex] != '\n') && (strSource[nIndex] != '\r'))
			nIndex++;
		strText = strSource.Left(nIndex);

		// output the line
		dc.TextOut(rectPage.left, rectPage.top, strText, strText.GetLength());

		// and chop off the hands string before the CR/LF
/*
		while ((nIndex < strSource.GetLength()) && 
			  ((strSource[nIndex] == '\r') || (strSource[nIndex] == '\n')) )
			  nIndex++;
*/
		// trim both chars of a "\r\n" pair
		if ( (strSource.GetLength() > nIndex+1) && 
		     (strSource[nIndex] == '\r') && (strSource[nIndex+1] == '\n') )
			  nIndex++;
		if (strSource.GetLength() > nIndex+1)
			strSource = strSource.Mid(nIndex+1);
		else
			strSource = _T("");

		// adjust the draw area
		rectPage.top += nCharHeight;
		nPageLine++;
	}

	// done
	return 0;
}




//
// OnPrint()
// 
void CEasyBView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	// format header
	CTime time = CTime::GetCurrentTime();
	CString strHeaderLeft = FormString("Easy Bridge %s", theApp.GetProgramVersionString());
	CString strHeaderRight = theApp.GetValueString(tstrProgramCopyright);
	CString strFooterLeft = FormString("Page 1 of 1");
	CString strFooterRight = FormString("%s", time.Format("%B %d, %Y  %I:%M %p"));

	//
	// setup print font
	//
	CFont printFont;
	CString strDefaultFont;
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	logFont.lfHeight = -13;
	// scale the font to the printer
	int nPrinterPixPerInchX = pDC->GetDeviceCaps(LOGPIXELSX);
	int nPrinterPixPerInchY = pDC->GetDeviceCaps(LOGPIXELSY);
	CWindowDC dc(this);
	int nScreenPixPerInchX = dc.GetDeviceCaps(LOGPIXELSX);
	int nScreenPixPerInchY = dc.GetDeviceCaps(LOGPIXELSY);
	double fRatio = (double)nPrinterPixPerInchX / (double)nScreenPixPerInchX;
	logFont.lfHeight = (long)(logFont.lfHeight * fRatio);
	//
	logFont.lfWeight = FW_NORMAL;
	logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	strDefaultFont.LoadString(IDS_DEFAULT_PRINTFONT);
	lstrcpy(logFont.lfFaceName, strDefaultFont);
	printFont.CreateFontIndirect(&logFont);
	//	
	CFont* pOldFont = pDC->SelectObject(&printFont);
	pDC->SetBkMode(TRANSPARENT);

	//
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	int cyChar = tm.tmHeight;
	CRect rectPage = pInfo->m_rectDraw;

	// deflate page rect by .75" on left & right, 1/2" top & bottom
	rectPage.left += (nPrinterPixPerInchX * 3) / 4;
	rectPage.right -= (nPrinterPixPerInchX * 3) / 4;
	rectPage.top += (nPrinterPixPerInchY / 2);
	rectPage.bottom -= (nPrinterPixPerInchY / 2);

	// draw and exclude space for header
	if (!strHeaderLeft.IsEmpty() || !strHeaderRight.IsEmpty()) 
	{
		// draw left header
		pDC->TextOut(rectPage.left, rectPage.top, strHeaderLeft);
		// draw right header
		CSize size = pDC->GetTextExtent(strHeaderRight, strHeaderRight.GetLength());
		pDC->TextOut(rectPage.right - size.cx, rectPage.top, strHeaderRight);
		//
		rectPage.top += cyChar + cyChar / 2;
		pDC->MoveTo(rectPage.left, rectPage.top);
		pDC->LineTo(rectPage.right, rectPage.top);
//		rectPage.top += cyChar / 2;
		rectPage.top += cyChar;
	}

	// allow space for footer
	if (!strFooterLeft.IsEmpty() || !strFooterRight.IsEmpty())
//		rectPage.bottom -= (cyChar + cyChar/2 + cyChar/2);
//		rectPage.bottom -= (cyChar + cyChar/2 + cyChar);
		rectPage.bottom -= (cyChar + cyChar/2);
	pInfo->m_rectDraw = rectPage;

	//
	//----------------------------------------------------------------------
	// draw body text
	//
	rectPage = pInfo->m_rectDraw;
	int nPageLine = 0;

	// draw dealt hands
	PrintTextBlock(*pDC, cyChar, rectPage, nPageLine, FormString(_T("[Dealt Hands -- #%s]\n"), pDOC->GetDealIDString()));
	CString strHands = pDOC->FormatOriginalHands();
	PrintTextBlock(*pDC, cyChar, rectPage, nPageLine, strHands);

	// draw bidding history
	PrintTextBlock(*pDC, cyChar, rectPage, nPageLine, "\n\n[Bidding History]\n");
	const CString strBiddingHistory = pMAINFRAME->GetPlainBiddingHistory();
	PrintTextBlock(*pDC, cyChar, rectPage, nPageLine, strBiddingHistory);
	if (ISBID(pDOC->GetContract()))
	{
		int nDeclarer = pDOC->GetDeclarerPosition();
		CString strContract = FormString("Contract: %s by %s; %s leads", pDOC->GetContractString(), PositionToString(nDeclarer), PositionToString(GetNextPlayer(nDeclarer)));
		PrintTextBlock(*pDC, cyChar, rectPage, nPageLine, strContract);
	}

	// draw play history
	PrintTextBlock(*pDC, cyChar, rectPage, nPageLine, "\n\n[Play History]\n");
	const CString strPlayHistory = pMAINFRAME->GetPlainPlayHistory();
	PrintTextBlock(*pDC, cyChar, rectPage, nPageLine, strPlayHistory);
	CString strTricks = FormString("Tricks -- N/S: %d,  E/W: %d", 
									pDOC->GetNumTricksWonByTeam(NORTH_SOUTH),
									pDOC->GetNumTricksWonByTeam(EAST_WEST));
	PrintTextBlock(*pDC, cyChar, rectPage, nPageLine, strTricks);

	// draw current hands
	if (pDOC->GetNumTricksPlayed() > 0)
	{
		PrintTextBlock(*pDC, cyChar, rectPage, nPageLine, "\n\n[Ending Hands]\n");
		CString strHands = pDOC->FormatCurrentHands();
		PrintTextBlock(*pDC, cyChar, rectPage, nPageLine, strHands);
	}

	// draw footer
	if (!strFooterLeft.IsEmpty() || !strFooterRight.IsEmpty()) 
	{
		// first restore page rect
//		rectPage.bottom += cyChar + cyChar/2 + cyChar/2;
		rectPage.bottom += cyChar + cyChar/2 + cyChar;
		rectPage.bottom -= cyChar;
		// draw left footer
		pDC->TextOut(rectPage.left, rectPage.bottom, strFooterLeft);
		// draw right footer
		CSize size = pDC->GetTextExtent(strFooterRight, strFooterRight.GetLength());
		pDC->TextOut(rectPage.right - size.cx, rectPage.bottom, strFooterRight);
		//
		rectPage.bottom -= cyChar / 2;
		pDC->MoveTo(rectPage.left, rectPage.bottom);
		pDC->LineTo(rectPage.right, rectPage.bottom);
	}

	//
	// All done
	//
	(void)pDC->SelectObject(pOldFont);
	printFont.DeleteObject();
	pInfo->SetMaxPage(pInfo->m_nCurPage); 
	pInfo->m_bContinuePrinting = FALSE;

	//
//	CView::OnPrint(pDC, pInfo);

	
/*
	// get string to show as "filename" in header/footer
	LPCTSTR pszFileName = GetDocument()->GetPathName();
	if (pszFileName[0] == 0)
		pszFileName = GetDocument()->GetTitle();
	// go thru global CPageSetupDlg to format the header and footer
	CString strHeader;
	dlgPageSetup.FormatHeader(strHeader, m_timeHeader, pszFileName,
							  pInfo->m_nCurPage);
	CString strFooter;
	dlgPageSetup.FormatFooter(strFooter, m_timeFooter, pszFileName,
							  pInfo->m_nCurPage);

	//
	// setup print font
	//
	CFont* pOldFont = pDC->SelectObject(&m_fontSpecial);
	//
	pDC->SetBkMode(TRANSPARENT);
	//
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	int cyChar = tm.tmHeight;
	CRect rectPage = pInfo->m_rectDraw;

	// draw and exclude space for header
	if (!strHeader.IsEmpty()) 
	{
		pDC->TextOut(rectPage.left, rectPage.top, strHeader);
		rectPage.top += cyChar + cyChar / 4;
		pDC->MoveTo(rectPage.left, rectPage.top);
		pDC->LineTo(rectPage.right, rectPage.top);
		rectPage.top += cyChar / 4;
	}

	// allow space for footer
	if (!strFooter.IsEmpty())
		rectPage.bottom -= cyChar + cyChar/4 + cyChar/4;
	pInfo->m_rectDraw = rectPage;

	//
	// draw body text
	//
	//--------------------------------------------------------------------
	//
	// Custom print routine
	//
	char szText[256];
	LPCTSTR pText, pBase, pEnd;
	UINT nLen = 0;
	UINT nBufLen = GetBufferLength();
	pText = LockBuffer();
	// reset pointers
	pBase = pText;
	pEnd = pBase;
	m_nTextIndex = 0;
	// advance to the current page
	UINT nTargetPage = pInfo->m_nCurPage-1;
	UINT i;
	int	nPageLine = 0;
	//
	for(i=0;i<=nTargetPage;i++) 
	{
		// quit if exceeded the end of the file
		if (m_nTextIndex >= nBufLen)
			break;
		// reset page rect and fit lines of text on page
		rectPage = pInfo->m_rectDraw;
		while ( ((rectPage.bottom - rectPage.top) >= cyChar) && 
											(m_nTextIndex< nBufLen-1) ) 
		{
			nLen = 0;
			while((*(pEnd) != '\r') && (++nLen < 256) && 
  									   ((m_nTextIndex + nLen) < nBufLen)) 
			{
				// check for form feed character
				if ((*pEnd == '\f') && (nPageLine > 0)) 
				{
					nLen--;
					nPageLine = 0;
					break;
				}
				pEnd++;
			}

			if (i == nTargetPage) 
			{
				strncpy(szText, pBase, nLen);
				szText[nLen] = '\0';
				// check for special code
				if ((i == 0) && (m_nTextIndex == 0) && 
							((strncmp(szText,"[PRINT MODE",11) == 0) ||
							 (strncmp(szText,"[Print Mode",11) == 0)) ) 
				{
					// this part skips the print info line
				} else {
					// do normal print
					pDC->TextOut(rectPage.left, rectPage.top, szText, nLen);
				}
			}
			rectPage.top += cyChar;
			nPageLine++;
			m_nPrintLine++;
			// 
			m_nTextIndex += nLen + 2;
			// skip over CR&LF
			if (*pEnd == '\f') 
			{
				// we're at a page break
				pBase += nLen + 1;
				pEnd++;
				break;
			} 
			else 
			{
				pBase += nLen + 2;
				pEnd += 2;
			}
		}
		// check for full page
		if ((rectPage.bottom - rectPage.top) < cyChar)
			nPageLine = 0;
	}
	UnlockBuffer();
	if (m_nTextIndex >= nBufLen) 
	{
		pInfo->SetMaxPage(pInfo->m_nCurPage); 
		pInfo->m_bContinuePrinting = FALSE;
	} 
	//
	//--------------------------------------------------------------------
	//
	// draw footer
	if (!strFooter.IsEmpty()) 
	{
		// first restore page rect
		rectPage.bottom += cyChar + cyChar/4 + cyChar/4;
		//
		rectPage.bottom -= cyChar;
		pDC->TextOut(rectPage.left, rectPage.bottom, strFooter);
		rectPage.bottom -= cyChar / 4;
		pDC->MoveTo(rectPage.left, rectPage.bottom);
		pDC->LineTo(rectPage.right, rectPage.bottom);
//		rectPage.bottom -= cyChar / 4;
	}
	//
	// All done
	//
	(void)pDC->SelectObject(pOldFont);
	pInfo->SetMaxPage(1); 
	pInfo->m_bContinuePrinting = FALSE;
	//
//	CView::OnPrint(pDC, pInfo);
*/
}




