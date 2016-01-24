//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CCard
//
#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "EasyBvw.h"
#include "cardopts.h"
#include "card.h"
#include "deck.h"
#include "viewopts.h"
#include "math.h"


// class static data
int CCard::m_nCardWidth = -1;
int CCard::m_nCardHeight = -1;


// constructor
CCard::CCard()
{	
	m_pBitmap = NULL;
	m_nFaceValue = NONE;  // NCR init some more 
	m_nDeckValue = NONE;
}

// destructor
CCard::~CCard()
{
	// destroy the card's prev bitmap
	m_prevBitmap.DeleteObject();
	m_hlPrevBitmap.DeleteObject();
}


//
void CCard::Clear()
{
	m_bAssigned = FALSE;
	m_bBackgroundSet = FALSE;
	m_bHLBackgroundSet = FALSE;
	m_bFaceUp = FALSE;
	m_nOwner = UNKNOWN;  //NCR-OWN2POS
	m_nDisplayPosition = -1;
	m_nSuitPosition = -1;
	m_nHandIndex = -1;
	m_nPosX = m_nPosY = 0;
}


//
void CCard::ClearBackground()
{
	m_bBackgroundSet = FALSE;
	m_bHLBackgroundSet = FALSE;
}


//
// GetProgramOption() and SetProgramOption() are 
// generic property get/set routines
//

//
LPVOID CCard::GetValuePV(int nItem, int nIndex1, int nIndex2, int nIndex3) const
{
	switch(nItem)
	{
		case tcCard:
			return (LPVOID) cCard[nIndex1];
		case tcFaceCard:
			return (LPVOID) cFaceCard[nIndex1];
		case tcSuit:
			return (LPVOID) cSuit[nIndex1];
		default:
			AfxMessageBox("Unhandled Call to CCard::GetValuePV()");
			return (LPVOID) NULL;
	}
	return NULL;
}

//
int CCard::SetValuePV(int nItem, LPVOID value, int nIndex1, int nIndex2, int nIndex3)
{
	int nVal = (int) value;
	BOOL bVal = (BOOL) value;
	LPCTSTR sVal = (LPCTSTR) value;
	//
	//
	switch(nItem)
	{
		// card chars are constants
		case tcCard:
			break;
		case tcFaceCard:
			break;
		case tcSuit:
			break;
		default:
			AfxMessageBox("Unhandled Call to CCard::SetValuePV()");
			return 1;
	}
	return 0;
}

// conversion functions
int CCard::GetValueInt(int nItem, int nIndex1, int nIndex2, int nIndex3) const
{
	return (int) GetValuePV(nItem, nIndex1, nIndex2, nIndex3);
}

LPCTSTR CCard::GetValueString(int nItem, int nIndex1, int nIndex2, int nIndex3) const
{
	return (LPCTSTR) GetValuePV(nItem, nIndex1, nIndex2, nIndex3);
}

int CCard::GetValue(int nItem, int nIndex1, int nIndex2, int nIndex3) const
{
	return (int) GetValuePV(nItem, nIndex1, nIndex2, nIndex3);
}

int CCard::SetValueInt(int nItem, int nValue, int nIndex1, int nIndex2, int nIndex3)
{
	return SetValuePV(nItem, (LPVOID)nValue, nIndex1, nIndex2, nIndex3);
}

int CCard::SetValueString(int nItem, LPCTSTR szValue, int nIndex1, int nIndex2, int nIndex3)
{
	return SetValuePV(nItem, (LPVOID)szValue, nIndex1, nIndex2, nIndex3);
}

int CCard::SetValue(int nItem, int nValue, int nIndex1, int nIndex2, int nIndex3)
{
	return SetValuePV(nItem, (LPVOID)nValue, nIndex1, nIndex2, nIndex3);
}


//
//===============================================================
//



//
void CCard::Initialize(int nSuit, int nValue, CBitmap* pBitmap, CDC* pDC)
{
	CString strTemp;
	// check limits
	if ((nSuit < CLUBS) || (nSuit > SPADES) || 
			(nValue < 2) || (nValue > ACE)) 
	{
		strTemp.Format("Illegal call to card's initialize routine; suit = %d, value = %d",
						nSuit,nValue);
		AfxMessageBox(strTemp);
		return;
	}

	// init the card's bitmap
	SetBitmap(pBitmap, pDC);

	//
	m_nSuit = nSuit;
	m_nFaceValue = nValue;
	m_nDeckValue = MAKEDECKVALUE(m_nSuit,m_nFaceValue);
	m_strName = CardToShortString(m_nDeckValue);
	m_strFaceName = GetCardName(m_nFaceValue);
	m_strFullName = CardToString(m_nDeckValue);
	m_strReverseFullName = CardToReverseString(m_nDeckValue);
	m_bAssigned = FALSE;
}


//
void CCard::SetBitmap(CBitmap* pBitmap, CDC* pDC)
{
	// if reassigning bitmap, clear old info
	if (m_pBitmap)
	{
		m_prevBitmap.DeleteObject();
		m_hlPrevBitmap.DeleteObject();
	}

	// get dimensions for the new bitmap
	m_nCardWidth = deck.GetCardWidth();
	m_nCardHeight = deck.GetCardHeight();

	// set the new bitmap
	m_pBitmap = pBitmap;
	if (m_pBitmap == NULL) 
	{
		AfxMessageBox(FormString("Failed to find card bitmap, suit %d, value %d", m_nSuit, m_nFaceValue));
	}
	else
	{
		// create background bitmap
		m_prevBitmap.CreateCompatibleBitmap(pDC, m_nCardWidth, m_nCardHeight);
		m_hlPrevBitmap.CreateCompatibleBitmap(pDC, m_nCardWidth, m_nCardHeight);
	}

	// and clear flags
	m_bBackgroundSet = FALSE;
	m_bHLBackgroundSet = FALSE;
}




// duplicate
void CCard::operator=(CCard* pSource) 
{
	//
	m_nSuit = pSource->m_nSuit;
	m_nFaceValue = pSource->m_nFaceValue;
	m_pBitmap = pSource->m_pBitmap;
}

void CCard::operator=(CCard& cSource) 
{
	//
	m_nSuit = cSource.m_nSuit;
	m_nFaceValue = cSource.m_nFaceValue;
	m_pBitmap = cSource.m_pBitmap;
}


//
BOOL CCard::IsValid() const
{
	if ((m_nDeckValue < 0) || (m_nDeckValue >= 52))
		return FALSE;
	if ((m_nSuit < CLUBS) || (m_nSuit > SPADES))
		return FALSE;
	//
	return TRUE;
}





//
//----------------------------------------------------------------
//
// utility functions
//

//
// GetDisplayValue()
//
// the card's display order onscreen
// the highest card (the one that appears first) has the lowest code
//
int CCard::GetDisplayValue() const
{
	CEasyBView* pView = CEasyBView::GetView();
	// get the suit's actual onscreen suit order
	int nSuitIndex = pView->GetSuitToScreenIndex(m_nSuit);
	return (nSuitIndex*13 + (14 - m_nFaceValue));
}


//
// GetDummyDisplayValue()
//
int CCard::GetDummyDisplayValue() const
{
	CEasyBView* pView = CEasyBView::GetView();
	// get the suit's actual (dummy) onscreen suit order
	int nSuitIndex = pView->GetDummySuitToScreenIndex(m_nSuit);
	return (nSuitIndex*13 + (14 - m_nFaceValue));
}


//
void CCard::GetRect(RECT& rect) const
{
	rect.left = m_nPosX;
	rect.top = m_nPosY;
	rect.right = m_nPosX + deck.GetCardWidth();
	rect.bottom = m_nPosY + deck.GetCardHeight();
}
  

//
TCHAR CCard::GetCardLetter() const
{
	return cCard[m_nFaceValue];
}

//
TCHAR CCard::GetSuitLetter() const
{
	return cSuit[m_nSuit];
}



//
//----------------------------------------------------------------
//
// draw operations
//

// draw the card at the current location
void CCard::Draw(CDC* pDC)
{
#ifdef _DEBUG   // NCR DEBUG CODE
	if((m_nPosY > 500) 
	   || ((m_nPosX > 700) && (m_nPosX < 730))
	   || (m_nDeckValue == 21)  // show Diamond 10
	   ) 
	{  // NCR DEBUG CODE
		printf("X=%d, Y=%d for card=%s, dv=%d\n", m_nPosX, m_nPosY, LPCTSTR(m_strFullName), m_nDeckValue);
	}
	if((m_nPosX == 507) && (m_nDeckValue == 21)) {  // Trap DT
		printf("misplaced DT\n");
	}
#endif

	CDC cardDC,cacheDC,maskDC;
	// create DCs
	cardDC.CreateCompatibleDC(pDC);
	cacheDC.CreateCompatibleDC(pDC);
	maskDC.CreateCompatibleDC(pDC);

	// load bitmaps
	CBitmap* pOldBitmap1;
//	if ((m_bFaceUp) || (theApp.m_bDebugMode) || (theApp.m_bShowCardsFaceUp))
	if ((m_bFaceUp) || (theApp.AreCardsFaceUp())) 
	{
		pOldBitmap1 = (CBitmap*) cardDC.SelectObject(m_pBitmap);
	} 
	else 
	{
		pOldBitmap1 = (CBitmap*) cardDC.SelectObject(deck.GetCardBackBitmap());
	}
	CBitmap* pOldBitmap2 = (CBitmap*) maskDC.SelectObject(deck.GetMaskBitmap());
    CBitmap tempBitmap;
	//
    tempBitmap.CreateCompatibleBitmap(pDC, m_nCardWidth, m_nCardHeight);
	CBitmap* pOldBitmap3 = (CBitmap*) cacheDC.SelectObject(&tempBitmap);
	// first copy screen background to cacheDC
	cacheDC.BitBlt(0,0,m_nCardWidth,m_nCardHeight,pDC,m_nPosX,m_nPosY,SRCCOPY);
	// and save it for later use
	CDC saveDC;
	saveDC.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap4 = (CBitmap*) saveDC.SelectObject(&m_prevBitmap);
	saveDC.BitBlt(0,0,m_nCardWidth,m_nCardHeight,&cacheDC,0,0,SRCCOPY);
	saveDC.SelectObject(pOldBitmap4);
	saveDC.DeleteDC();

	// now mask the cacheDC
	cacheDC.BitBlt(0,0,m_nCardWidth,m_nCardHeight,&maskDC,0,0,SRCAND);
	// then copy the bitmap onto the cacheDC
	cacheDC.BitBlt(0,0,m_nCardWidth,m_nCardHeight,&cardDC,0,0,SRCPAINT);
	// and finally copy the cacheDC onto the screen
	pDC->BitBlt(m_nPosX,m_nPosY,m_nCardWidth,m_nCardHeight,&cacheDC,0,0,SRCCOPY);
	// all done
	(void)cardDC.SelectObject(pOldBitmap1);
	(void)maskDC.SelectObject(pOldBitmap2);
	(void)cacheDC.SelectObject(pOldBitmap3);
	cardDC.DeleteDC();
	maskDC.DeleteDC();
	cacheDC.DeleteDC();
	tempBitmap.DeleteObject();
	return;
}



// draw the card highlighted  at the current location
void CCard::DrawHighlighted(CDC* pDC, BOOL bVisible)
{
	//
	if (bVisible) 
	{
		//
		CDC cardDC,cacheDC,maskDC;
		// create DCs
		cardDC.CreateCompatibleDC(pDC);
		cacheDC.CreateCompatibleDC(pDC);
		maskDC.CreateCompatibleDC(pDC);

		// load bitmaps
		CBitmap* pOldBitmap1;
		pOldBitmap1 = (CBitmap*) cardDC.SelectObject(m_pBitmap);
		CBitmap* pOldBitmap2 = (CBitmap*) maskDC.SelectObject(deck.GetMaskBitmap());
	    CBitmap tempBitmap;
    	tempBitmap.CreateCompatibleBitmap(pDC, m_nCardWidth, m_nCardHeight);
		CBitmap* pOldBitmap3 = (CBitmap*) cacheDC.SelectObject(&tempBitmap);
		// first copy screen background to the cacheDC
		cacheDC.BitBlt(0,0,m_nCardWidth,m_nCardHeight,pDC,m_nPosX,m_nPosY,SRCCOPY);
		// and save it for later use
		CDC saveDC;
		saveDC.CreateCompatibleDC(pDC);
		CBitmap* pOldBitmap4 = (CBitmap*) saveDC.SelectObject(&m_hlPrevBitmap);
		saveDC.BitBlt(0,0,m_nCardWidth,m_nCardHeight,&cacheDC,0,0,SRCCOPY);
		saveDC.SelectObject(pOldBitmap4);
		saveDC.DeleteDC();
		m_bHLBackgroundSet = TRUE;

		// now mask the cacheDC
		cacheDC.BitBlt(0,0,m_nCardWidth,m_nCardHeight,&maskDC,0,0,SRCAND);
		// then copy the bitmap onto the cacheDC, INVERTED
		cacheDC.BitBlt(0,0,m_nCardWidth,m_nCardHeight,&cardDC,0,0,NOTSRCCOPY);
		// and finally copy the cacheDC onto the screen
		pDC->BitBlt(m_nPosX,m_nPosY,m_nCardWidth,m_nCardHeight,&cacheDC,0,0,SRCCOPY);
		// all done
		(void)cardDC.SelectObject(pOldBitmap1);
		(void)maskDC.SelectObject(pOldBitmap2);
		(void)cacheDC.SelectObject(pOldBitmap3);
		cardDC.DeleteDC();
		maskDC.DeleteDC();
		cacheDC.DeleteDC();
		tempBitmap.DeleteObject();
		return;

	} 
	else 
	{
	
		//
		if (!m_bHLBackgroundSet)
			return;
		//
		CDC cacheDC;
		// create the DC
		cacheDC.CreateCompatibleDC(pDC);
		// load bitmap
		CBitmap* pOldBitmap = (CBitmap*) cacheDC.SelectObject(&m_hlPrevBitmap);
		// copy the cacheDC onto the screen
		pDC->BitBlt(m_nPosX,m_nPosY,m_nCardWidth,m_nCardHeight,&cacheDC,0,0,SRCCOPY);
		// all done
		(void)cacheDC.SelectObject(pOldBitmap);
		cacheDC.DeleteDC();
		m_bHLBackgroundSet = FALSE;
		return;

	}
}



// flash the card
void CCard::FlashCard(CDC* pDC, int numTimes)
{
	for(int i=0;i<numTimes;i++)
	{
		DrawHighlighted(pDC, TRUE);
		Sleep(200);		// wait a fraction of a second
		DrawHighlighted(pDC, FALSE);
		// wait again if desired
		if (i < numTimes-1)
			Sleep(200);
	}
}



// restore the card's background image
void CCard::RestoreBackground(CDC* pDC)
{
	CDC cacheDC;
	// create the DC
	cacheDC.CreateCompatibleDC(pDC);
	// load bitmap
	CBitmap* pOldBitmap = (CBitmap*) cacheDC.SelectObject(&m_prevBitmap);
	// copy the cacheDC onto the screen
	pDC->BitBlt(m_nPosX,m_nPosY,deck.GetCardWidth(),deck.GetCardHeight(),&cacheDC,0,0,SRCCOPY);
	// all done
	(void)cacheDC.SelectObject(pOldBitmap);
	cacheDC.DeleteDC();
	return;
}



// move card
void CCard::MoveTo(CDC *pDC, int destX, int destY, BOOL bRedraw) {
	//
	m_nPosX = destX;
	m_nPosY = destY;
	if (bRedraw)
		Draw(pDC);
}



// drag card
void CCard::DragTo(CDC* pDC, int destX, int destY)
{
	int dx = m_nPosX - destX;
	int dy = m_nPosY - destY;
	CDC oldBkDC,newBkDC,cacheDC,cardDC,maskDC;

	// create DCs
	oldBkDC.CreateCompatibleDC(pDC);
	newBkDC.CreateCompatibleDC(pDC);
	cacheDC.CreateCompatibleDC(pDC);
	cardDC.CreateCompatibleDC(pDC);
	maskDC.CreateCompatibleDC(pDC);

	// load bitmaps
	CBitmap* pOldBitmapOldBk = (CBitmap*) oldBkDC.SelectObject(&m_prevBitmap);
	CBitmap* pOldBitmapCard;
//	if ((m_bFaceUp) || (theApp.m_bDebugMode) || (theApp.m_bShowCardsFaceUp))
	if ((m_bFaceUp) || (theApp.AreCardsFaceUp()))
		pOldBitmapCard = (CBitmap*) cardDC.SelectObject(m_pBitmap);
	else
		pOldBitmapCard = (CBitmap*) cardDC.SelectObject(deck.GetCardBackBitmap());
	CBitmap* pOldBitmapMask = (CBitmap*) maskDC.SelectObject(deck.GetMaskBitmap());
    CBitmap tempBitmapNewBk,tempBitmapCache;
    tempBitmapNewBk.CreateCompatibleBitmap(pDC, m_nCardWidth, m_nCardHeight);
    tempBitmapCache.CreateCompatibleBitmap(pDC, m_nCardWidth, m_nCardHeight);
	CBitmap* pOldBitmapNewBk = (CBitmap*) newBkDC.SelectObject(&tempBitmapNewBk);
	CBitmap* pOldBitmapCache = (CBitmap*) cacheDC.SelectObject(&tempBitmapCache);

	// #### TEMP ####
//	pDC->BitBlt(destX+m_nCardWidth,destY,m_nCardWidth,m_nCardHeight,&oldBkDC,0,0,SRCCOPY);

	// first copy screen background to the new background DC
	newBkDC.BitBlt(0,0,m_nCardWidth,m_nCardHeight,pDC,destX,destY,SRCCOPY);

	// now restore the old background onto the area of the new 
	// background that overlaps with the old background
	newBkDC.BitBlt(dx,dy,m_nCardWidth,m_nCardHeight,&oldBkDC,0,0,SRCCOPY);
	// newBkDC is saved for later; we work with cacheDC for now
	cacheDC.BitBlt(0,0,m_nCardWidth,m_nCardHeight,&newBkDC,0,0,SRCCOPY);

	// Copy a portion of the card bitmap to the old background
	oldBkDC.BitBlt(-dx,-dy,m_nCardWidth,m_nCardHeight,&maskDC,0,0,SRCAND);
	oldBkDC.BitBlt(-dx,-dy,m_nCardWidth,m_nCardHeight,&cardDC,0,0,SRCPAINT);

	// now mask & copy the card bitmap to the cacheDC (new background)
	cacheDC.BitBlt(0,0,m_nCardWidth,m_nCardHeight,&maskDC,0,0,SRCAND);
	cacheDC.BitBlt(0,0,m_nCardWidth,m_nCardHeight,&cardDC,0,0,SRCPAINT);
	// and then copy the cacheDC onto the screen
	pDC->BitBlt(destX,destY,m_nCardWidth,m_nCardHeight,&cacheDC,0,0,SRCCOPY);

	// and then copy the old background to the screen
	pDC->BitBlt(m_nPosX,m_nPosY,m_nCardWidth,m_nCardHeight,&oldBkDC,0,0,SRCCOPY);

	// all done
	(void)oldBkDC.SelectObject(pOldBitmapOldBk);
	(void)cardDC.SelectObject(pOldBitmapCard);
	(void)maskDC.SelectObject(pOldBitmapMask);
	(void)cacheDC.SelectObject(pOldBitmapCache);
	oldBkDC.DeleteDC();
	cardDC.DeleteDC();
	maskDC.DeleteDC();
	cacheDC.DeleteDC();
	tempBitmapCache.DeleteObject();
	// finally, we save the new background bitmap
	CDC saveDC;
	saveDC.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmapSave = (CBitmap*) saveDC.SelectObject(&m_prevBitmap);
	saveDC.BitBlt(0,0,m_nCardWidth,m_nCardHeight,&newBkDC,0,0,SRCCOPY);
	saveDC.SelectObject(pOldBitmapSave);
	saveDC.DeleteDC();
	// we're done with the newBkDC now, and can delete it
	(void)newBkDC.SelectObject(pOldBitmapNewBk);
	newBkDC.DeleteDC();
	tempBitmapNewBk.DeleteObject();
	// and update the new card position
	m_nPosX = destX;
	m_nPosY = destY;
	//
	return;
}



//
// Animate()
//
void CCard::Animate(CDC* pDC, int destx, int desty, BOOL bClearAtEnd, int nGranularity) 
{
	int x = m_nPosX;
	int y = m_nPosY;
	int diffx = destx - x;
	int diffy = desty - y;
	int dx = ABS(diffx);
	int dy = ABS(diffy);
	// use pythagorean theorem here
	int max = (int) sqrt((double)dx*dx + dy*dy);

	//
	if (nGranularity == -99)
		nGranularity = pVIEW->GetValue(tnAnimationGranularity);
	int nSteps = (nGranularity > 0)? ABS(max / nGranularity) : max;
	if (nSteps == 0) 
	{
		if ((dx > 0) || (dy > 0))
			nSteps = 1;
		else
			return;
	}

	//
	int nWaitTime = 16;	// 1/60th of a second
	if (nGranularity < 0)
		nWaitTime *= -nGranularity;

	//
	double sx = (destx - x) / (double)nSteps;
	double sy = (desty - y) / (double)nSteps;
	double posX = x;
	double posY = y;
	
	// 
	for(int i=0;i<nSteps-1;i++) 
	{
		posX += sx;
		posY += sy;
		//
		DragTo(pDC, (int)posX, (int)posY);
		//
		if (nGranularity < 0)
			Sleep(nWaitTime);
	}

	// final step is made only if we want to show card at end
	// else the card disappears from view
	RestoreBackground(pDC);
	if (!bClearAtEnd)
		MoveTo(pDC, destx, desty);
}





