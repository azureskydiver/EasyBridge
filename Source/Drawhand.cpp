//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////
//
// Drawhand.cpp : implementation of CEasyBView's hand drawing
//
// Routines:
// ---------
//   DisplayHand() 				show hand

//   DrawHoriz()			    draw hand in horiz row
//   PartialDrawHoriz()         
//   DrawHorizGrouped()			draw in horizontal goups
//   PartialDrawHorizGrouped()
//   DrawHorizDummy()           draw horizontal dummy
//   PartialDrawHorizDummy()
//   DrawVert()                 draw in vertical row
//   PartialDrawVert()
//   DrawVertDummy()            draw vertical dummy
//   PartialDrawVertDummy()
//
//   GetPlayerBottomCard(CPlayer& player);
//   CheckForTrickCardsOverlap(CDC* pDC, RECT& eraseRect);
//   DrawCardBack(CDC* pDC, int x, int y);
//
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "EasyBvw.h"
#include "player.h"
#include "card.h"
#include "deck.h"
#include "DrawParameters.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


#define DEBUG_MODE		1

//
// local functions
//
CCard* GetPlayerBottomCard(CPlayer& player);
void DrawCardBack(CDC* pDC, int x, int y);





//
//===========================================================
//
// DisplayHand
//
// parameters:
//	  nPos: player positions
//    nDisplaySuit: specific suit to display; display 
//                  all suits if left blank
//    bClearBackground: clear background before drawing
//    bCardPlayed:      a card has just been removed
//				        signals partial redraw
//
//===========================================================
//
void CEasyBView::DisplayHand(Position nPos, int nDisplaySuit, 
						BOOL bClearBackground, BOOL bCardPlayed) 
{
	int nStartSuit,nEndSuit;
	CDC *pDC = GetDC();
	CEasyBDoc* pDoc = CEasyBDoc::GetDoc();
	if (!pDOC || !pDOC->IsInitialized())
		return;
	CPlayer* pPlayer = pDOC->GetPlayer(nPos);
	if (pPlayer == NULL)
		return;
	BOOL bDummy = pPlayer->IsDummy();
	CDC cacheDC,*pOldDC;
   	CBitmap tempBitmap,*pOldBitmap;
	RECT clipRect;
	int nWidth, nHeight;

	// see if we're using triple buffering
	if (m_bTripleBuffer) 
	{
		cacheDC.CreateCompatibleDC(pDC);
		RECT rect;
		GetClientRect(&rect);
	    tempBitmap.CreateCompatibleBitmap(pDC, rect.right, rect.bottom);
		pOldBitmap = (CBitmap*) cacheDC.SelectObject(&tempBitmap);
		pDC->GetClipBox(&clipRect);
		nWidth = clipRect.right - clipRect.left;
		nHeight = clipRect.bottom - clipRect.top;
		cacheDC.BitBlt(clipRect.left,clipRect.top,nWidth,nHeight,pDC,clipRect.left,clipRect.top,SRCCOPY);
		// pull a switcheroo here
		pOldDC = pDC;
		pDC = &cacheDC;
	}
	//
//	if (!pDOC->m_bHandsDealt)
//		return;
	//
	if (nDisplaySuit >= 0) 
	{
		// reset the suit to index in suit display table
		if (nPos == pDOC->GetDummyPosition())
			nStartSuit = m_nDummySuitToScreenIndex[nDisplaySuit];
		else
			nStartSuit = m_nSuitToScreenIndex[nDisplaySuit];
		nEndSuit = nStartSuit;
	} 
	else 
	{
		nStartSuit = 0;
		nEndSuit = 3;
	}

	//
	// select subroutine according to position and draw mode
	//
	// 1/28 -- horrible hack -- the last card in a face-down hand doesn't
	//         clear properly, so force a complete redraw afterwards
	int numCardsLeft = PLAYER(nPos).GetNumCards();

	//
	if (nPos == SOUTH) 
	{
		//
		// player's hand (south); always display face up
		//
//		if ((bDummy) && (pDOC->IsDummyExposed()) && (theApp.IsGameInProgress())) 
		if ((bDummy) && (pDOC->IsDummyExposed()) && (m_nCurrMode != MODE_CARDLAYOUT)) 
		{
			//
			// Dummy player
			//
			if (!bCardPlayed) 
			{
				// dummy player, face up, full draw
				DrawHorizDummy(pDC,nPos,nDisplaySuit,bClearBackground,nStartSuit,nEndSuit);
			} 
			else 
			{
				// dummy player, face up, partial draw	
				PartialDrawHorizDummy(pDC,nPos);
			}
		} 
		else 
		{
			//
			// Non-dummy player, face up
			//
			if (!bCardPlayed) 
			{
				// non-dummy player, face up, full draw
				DrawHorizGrouped(pDC,nPos,nDisplaySuit,bClearBackground,nStartSuit,nEndSuit);
			} 
			else 
			{
				// non-dummy player, face up, partial draw	
				PartialDrawHorizGrouped(pDC,nPos);
			}
		}

	} 
	else if ((nPos == EAST) || (nPos == WEST)) 
	{

		//
		// Opponents hands
		//
//		if ((bDummy) && (pDOC->IsDummyExposed()) && (theApp.IsGameInProgress())) 
		if ((bDummy) && (pDOC->IsDummyExposed()) && (m_nCurrMode != MODE_CARDLAYOUT)) 
		{
			//
			// Dummy east-west here
			//
			if (!bCardPlayed) 
			{
				// dummy player, face up, full draw
				DrawVertDummy(pDC,nPos,nDisplaySuit,bClearBackground,nStartSuit,nEndSuit);
			} 
			else 
			{
				// dummy opponent, face up, partial draw	
				PartialDrawVertDummy(pDC,nPos);
			}
		} 
		else 
		{
			//
			// non-dummy east-west opponents here
			//
			if ((!bCardPlayed) || (numCardsLeft == 0))
			{
				// non-dummy player, face up/down, full draw
				DrawVert(pDC,nPos,nDisplaySuit,bClearBackground,nStartSuit,nEndSuit);
			} 
			else 
			{
				// non-dummy opponent, face up/down, partial draw	
				PartialDrawVert(pDC,nPos);
			}
		}

	} 
	else 
	{

		//
		//--------------------------------------------------
		// north (partner)
		//
//		if ((bDummy) && (pDOC->IsDummyExposed()) && (theApp.IsGameInProgress())) 
		if ((bDummy) && (pDOC->IsDummyExposed()) && (m_nCurrMode != MODE_CARDLAYOUT)) 
		{
			//
			// North, Dummy format
			//
			if (!bCardPlayed) 
			{
				// dummy north, face up, full draw
				DrawHorizDummy(pDC,nPos,nDisplaySuit,bClearBackground,nStartSuit,nEndSuit);
			} 
			else 
			{
				// dummy north, face up, partial draw	
				PartialDrawHorizDummy(pDC,nPos);
			}
		} 
		else 
		{
			//
			// North, non-dummy format
			//
			if ( (m_nCurrMode == MODE_CARDLAYOUT) || (m_nCurrMode == MODE_EDITHANDS) ||
				 (pDOC->GetDeclarerPosition() == NORTH) ||
				 (theApp.AreCardsFaceUp()) ) 
			{
				// show grouped cards
				if (!bCardPlayed)
				{
					DrawHorizGrouped(pDC,nPos,bClearBackground,nDisplaySuit,nStartSuit,nEndSuit);
				} 
				else 
				{
					PartialDrawHorizGrouped(pDC,nPos);
				}
			} 
			else if ((!bCardPlayed) || (numCardsLeft == 0))
			{
				// non-dummy north, face up or down, full draw
				DrawHoriz(pDC,nPos,bClearBackground,nDisplaySuit,nStartSuit,nEndSuit);
			} 
			else 
			{
				// non-dummy north, face up or down, partial draw	
				PartialDrawHoriz(pDC,nPos);
			}
		}
	}
	// finish up
	if (m_bTripleBuffer) 
	{
		// restore pointer
		pDC = pOldDC;
		// copy results of offscreen blitting to the screen
		RECT rect;
		GetHandBoundsRect(nPos,rect);
		pDC->BitBlt(clipRect.left,clipRect.top,nWidth,nHeight,&cacheDC,clipRect.left,clipRect.top,SRCCOPY);
		// done with triple buffering
		(void)cacheDC.SelectObject(pOldBitmap);
		cacheDC.DeleteDC();
		tempBitmap.DeleteObject();
	}
	// all done
	ReleaseDC(pDC);
}



//
void CEasyBView::GetHandBoundsRect(Position nPos, RECT& rect)
{
	int i;
	rect.left = m_rectSuitOffset[nPos][0].left;
	rect.top = m_rectSuitOffset[nPos][0].top;
	rect.right = m_rectSuitOffset[nPos][0].right;
	rect.bottom = m_rectSuitOffset[nPos][0].bottom;
	for(i=1;i<4;i++) 
	{
		if (m_rectSuitOffset[nPos][i].left < rect.left)
			rect.left = m_rectSuitOffset[nPos][i].left;
		if (m_rectSuitOffset[nPos][i].top < rect.top)
			rect.top = m_rectSuitOffset[nPos][i].top;
		if (m_rectSuitOffset[nPos][i].right > rect.right)
			rect.right = m_rectSuitOffset[nPos][i].right;
		if (m_rectSuitOffset[nPos][i].bottom > rect.bottom)
			rect.bottom = m_rectSuitOffset[nPos][i].bottom;
	}
}



//
//-------------------------------------------------------------
//
// Draw cards horizontally in an unbroken row
//
void CEasyBView::DrawHoriz(CDC* pDC, Position nPos, int nDisplaySuit,
					BOOL bClearBackground, int nStartSuit,  int nEndSuit)
{
	//
	// non-dummy north/south, face up or down, full draw	
	//
	int i,j,nSuit;
	int x = m_drawPoint[nPos].x;
	int y = m_drawPoint[nPos].y;

	// first clear background if requested
	// also clear if the last card has been played 
//	if ((bClearBackground) && (OffsetRectValid(nPos))) 
	if ( (bClearBackground || (PLAYER(nPos).GetNumCards() == 0)) && 
		 (OffsetRectValid(nPos)) ) 
	{
		ClearDisplayArea(m_rectSuitOffset[nPos][0].left,
						 m_rectSuitOffset[nPos][0].top,
						 m_rectSuitOffset[nPos][3].right,
						 m_rectSuitOffset[nPos][3].bottom,
						 pDC);
	}
	// then check to see if we have to do anything more
	if (PLAYER(nPos).GetNumCards() == 0)
		return;
	// display in traditional format
	for(i=nStartSuit;i<=nEndSuit;i++) 
	{
		nSuit = theApp.GetSuitSequence(i);
		m_rectSuitOffset[nPos][i].left = x;
		m_rectSuitOffset[nPos][i].top = y;
		//
		for(j=0;j<PLAYER(nPos).GetNumCardsInSuit(nSuit);j++) 
		{
			CCard* pCard = PLAYER(nPos).GetCardInSuit(nSuit,j);
			ASSERT(pCard != NULL);
			pCard->MoveTo(pDC, x, y);
			x += m_nCardXGap;
		}
		m_rectSuitOffset[nPos][i].right = x + m_nCardWidth - m_nCardXGap;
		m_rectSuitOffset[nPos][i].bottom = y + m_nCardHeight;
	}
}



//
//-------------------------------------------------------------
//
// Partial draw cards horizontally in an unbroken row
//
void CEasyBView::PartialDrawHoriz(CDC* pDC, Position nPos)
{
	//
	// non-dummy north/south, redrawing after a card was pulled
	//
	int cx,cy;
	int x = m_drawPoint[nPos].x;
	int y = m_drawPoint[nPos].y;

	if (!theApp.AreCardsFaceUp()) 
	{
		//
		// non-dummy north, face down, 
		// partial draw	
		//
		// clear area under bottom card
		int numCards = PLAYER(nPos).GetNumCards();
		CCard* pPlayedCard = pDOC->GetCurrentTrickCard(nPos);
		cy = y + m_nCardHeight;
		if (numCards > 0) 
		{
			x += m_nCardWidth + m_nCardXGap*(numCards-1);
			cx = x + m_nCardXGap;
			ClearDisplayArea(x, y, cx, cy, pDC);
			x -= m_nCardWidth;
			DrawCardBack(pDC, x, y);
		} 
		else 
		{
			cx = m_nCardWidth;
			ClearDisplayArea(x, y, cx, cy, pDC);
		}
		// and thazzit
		return;
	} 
	else 
	{
		//
		// non-dummy north, face up, partial draw	
		//
		// since cards are face up, redraw starting 
		// from card after the one last played
		CCard* pOldCard = pDOC->GetCurrentTrickCard(nPos);
		// restore the played card's background
		pOldCard->RestoreBackground(pDC);
		// and then restore remaining cards
		int nStart = pOldCard->GetDisplayPosition();
		int nEnd = PLAYER(nPos).GetNumCards();
		// if this was the last card on the 
		// stack, then we're done
		if ((nStart == nEnd) || (nEnd == 0))
			return;
		// else clear the area to the right of the 
		// played card -- grab the card right 
		// after the card played
		CCard* pCard1 = PLAYER(nPos).GetDisplayedCard(nStart);
		CCard* pCard2 = PLAYER(nPos).GetDisplayedCard(nEnd-1);
		RECT eraseRect;
		eraseRect.left = pCard1->GetXPosition() + m_nCardWidth - m_nCardXGap;
		eraseRect.top = pCard1->GetYPosition();
		eraseRect.right = pCard2->GetXPosition() + m_nCardWidth;
		eraseRect.bottom = pCard1->GetYPosition() + m_nCardHeight;
		ClearDisplayArea(&eraseRect,pDC);
		// and draw remaining cards starting from
		// the lcoation of the old card
		x = pOldCard->GetXPosition();
		y = pOldCard->GetYPosition();
		int i;
		for(i=nStart;i<nEnd;i++) 
		{
			CCard* pCard = PLAYER(nPos).GetDisplayedCard(i);
			ASSERT(pCard != NULL);
			pCard->MoveTo(pDC, x, y);
			x += m_nCardXGap;
		}
	}
}




//
//-------------------------------------------------------------
//
// Draw cards horizontally, grouped by suit
//
void CEasyBView::DrawHorizGrouped(CDC* pDC, Position nPos, int nDisplaySuit,
				BOOL bClearBackground, int nStartSuit,  int nEndSuit)
{
	//
	// grouped north/south, draw face up, full draw
	//
	int i,j,nSuit;
	int x = m_drawPoint[nPos].x;
	int y = m_drawPoint[nPos].y;
	
	// make an x-coord adjustment if this is north
	if (nPos == NORTH) 
	{
		// make an exception; draw north using South's x-coord
		x = m_drawPoint[SOUTH].x;
	}

	// first clear background if requested
	if ((bClearBackground) && (OffsetRectValid(nPos))) 
	{
		RECT rect;
		GetHandBoundsRect(nPos,rect);
		ClearDisplayArea(&rect,pDC);
		ResetSuitOffsets(nPos);	// ??? necessary ???
	}
	// then check to see if we have to do anything more
	if (PLAYER(nPos).GetNumCards() == 0)
		return;
	// else draw
	for(i=nStartSuit;i<=nEndSuit;i++) 
	{
		nSuit = theApp.GetSuitSequence(i);
		// init the suit locations or read from it
		if (m_rectSuitOffset[nPos][i].left == -1) 
		{
			m_rectSuitOffset[nPos][i].left = x;
			m_rectSuitOffset[nPos][i].top = y;
		} 
		else 
		{
			x = m_rectSuitOffset[nPos][i].left;
			y = m_rectSuitOffset[nPos][i].top;
		}
		//
		if (PLAYER(nPos).GetNumCardsInSuit(nSuit) > 0) 
		{
			for(j=0;j<PLAYER(nPos).GetNumCardsInSuit(nSuit);j++) 
			{
				CCard* pCard = PLAYER(nPos).GetCardInSuit(nSuit,j);
				ASSERT(pCard != NULL);
				pCard->MoveTo(pDC, x, y);
				x += m_nCardXGap;
			}
			m_rectSuitOffset[nPos][i].right = x + m_nCardWidth - m_nCardXGap;
			x += m_nCardWidth - m_nCardXGap;
		} 
		else 
		{
			m_rectSuitOffset[nPos][i].right = 0;
		}
		x += m_nSuitSpacer;
		m_rectSuitOffset[nPos][i].bottom = y + m_nCardHeight;
	}
}




//
//-------------------------------------------------------------
//
// Partially draw cards horizontally, grouped by suit
//
void CEasyBView::PartialDrawHorizGrouped(CDC* pDC, Position nPos)
{
	//
	// non-dummy north/south, face up, partial draw	
	//
	// since cards are face up, redraw starting 
	// from card after the one last played
	CCard* pOldCard = pDOC->GetCurrentTrickCard(nPos);

	// restore the played card's background
	pOldCard->RestoreBackground(pDC);
	// and then restore remaining cards
	int nSuit = pOldCard->GetSuit();
	int nStart = pOldCard->GetSuitPosition();
	int nEnd = PLAYER(nPos).GetNumCardsInSuit(nSuit);

	// if this was the last card in the suit, then clear and we're done
	if (nEnd == 0)
	{
		int nSuitIndex = GetSuitToScreenIndex(nSuit);
		ClearDisplayArea(&m_rectSuitOffset[nPos][nSuitIndex], pDC);
		return;
	}

	// else if this was the bottom card on the stack, then we're done
	if (nStart >= nEnd)
		return;

	// clear the suit area to the right of the played card -- 
	// grab the card right after the card played
	CCard* pCard1 = PLAYER(nPos).GetCardInSuit(nSuit,nStart);
	CCard* pCard2 = PLAYER(nPos).GetCardInSuit(nSuit,nEnd-1);
	RECT eraseRect;
	eraseRect.left = pCard1->GetXPosition() + m_nCardWidth - m_nCardXGap;
	eraseRect.top = pCard1->GetYPosition();
	eraseRect.right = pCard2->GetXPosition() + m_nCardWidth;
	eraseRect.bottom = pCard1->GetYPosition() + m_nCardHeight;
	ClearDisplayArea(&eraseRect, pDC);
	// and draw remaining cards starting from
	// the location of the old card
	int i,x,y;
	x = pOldCard->GetXPosition();
	y = pOldCard->GetYPosition();
	for(i=nStart;i<nEnd;i++) 
	{
		CCard* pCard = PLAYER(nPos).GetCardInSuit(nSuit,i);
		ASSERT(pCard != NULL);
		pCard->MoveTo(pDC, x, y);
		x += m_nCardXGap;
	}
}




//
//-------------------------------------------------------------
//
// Draw cards horizontally, in dummy column format
//
void CEasyBView::DrawHorizDummy(CDC* pDC, Position nPos, int nDisplaySuit,
				BOOL bClearBackground, int nStartSuit,  int nEndSuit)
{
	//
	// dummy north/south, face up, full draw
	//
	int i,j,nSuit;
	int x = m_drawPoint[nPos].x + m_dummyDrawOffset[nPos].x;
	int y = m_drawPoint[nPos].y + m_dummyDrawOffset[nPos].y;

	// first clear background if requested
	if ((bClearBackground) && (OffsetRectValid(nPos))) 
	{
		RECT rect;
		// see if we're only doing one suit
		if (nDisplaySuit >= 0) 
		{
			ClearDisplayArea(&m_rectSuitOffset[nPos][nStartSuit],pDC);
		} 
		else 
		{
			// else clear entire hand area
			GetHandBoundsRect(nPos,rect);
			ClearDisplayArea(&rect,pDC);
		}
	}
	// then check to see if we have to do anything more
	if (PLAYER(nPos).GetNumCards() == 0)
		return;

	// insert offset if drawing a single suit
	if (nDisplaySuit >= 0) 
	{
		x = m_rectSuitOffset[nPos][nStartSuit].left;
	}

	//
	for(i=nStartSuit;i<=nEndSuit;i++) 
	{
		// use the DUMMY suit sequence
		nSuit = theApp.GetDummySuitSequence(i);
		m_rectSuitOffset[nPos][i].left = x;
		m_rectSuitOffset[nPos][i].top = y;
		int numInSuit = PLAYER(nPos).GetNumCardsInSuit(nSuit);
		//
		if (nPos == NORTH) 
		{
			for(j=0;j<numInSuit;j++) 
			{
				CCard* pCard = PLAYER(nPos).GetCardInSuit(nSuit,j);
				ASSERT(pCard != NULL);
				pCard->MoveTo(pDC, x, y);
				y += m_nCardYGap;
			}
		} 
		else 
		{
			// south as dummy
			// draw low card on top
			y = m_rectSuitOffset[nPos][i].top - 
								((numInSuit-1) * m_nCardYGap);
			m_rectSuitOffset[nPos][i].top = y;
			for(j=numInSuit-1;j>=0;j--) 
			{
				CCard* pCard = PLAYER(nPos).GetCardInSuit(nSuit,j);
				ASSERT(pCard != NULL);
				pCard->MoveTo(pDC, x, y);
				y += m_nCardYGap;
			}
		}		
		m_rectSuitOffset[nPos][i].right = x + m_nCardWidth;
		m_rectSuitOffset[nPos][i].bottom = y + m_nCardHeight - m_nCardYGap;
		x += m_nCardWidth + m_nDummySuitSpacer;
		y = m_drawPoint[nPos].y;
	}
}




//
//-------------------------------------------------------------
//
// Partial draw cards horizontally, in dummy column format
//
void CEasyBView::PartialDrawHorizDummy(CDC* pDC, Position nPos)
{
	//
	// dummy north/south, face up, partial draw
	//
	int i,j,x,y,cx,cy,nSuit,nStart,nEnd;
	//
	RECT eraseRect;

	// first grab played card
	CCard* pPlayedCard = pDOC->GetCurrentTrickCard(nPos);
	nSuit = pPlayedCard->GetSuit();

	// first get the played card's posititon
	x = pPlayedCard->GetXPosition();
	y = pPlayedCard->GetYPosition();

	// handling then depends on whether this is north or south
	if (nPos == NORTH) 
	{
		//
		nStart = pPlayedCard->GetSuitPosition();
		nEnd = PLAYER(nPos).GetNumCardsInSuit(nSuit);
		// then establish redraw area
		if (nEnd == 0) 
		{
			// last card in suit; the erase rect is simply 
			// the card iteself
			eraseRect.left = x;
			eraseRect.top = y;
			eraseRect.right = x + m_nCardWidth;
			eraseRect.bottom = y + m_nCardHeight;
		} 
		else if (nStart == nEnd) 
		{
			// area to be erased is just bottom portion of
			// the played card	
			eraseRect.left = pPlayedCard->GetXPosition();
			eraseRect.top = pPlayedCard->GetYPosition() + m_nCardHeight - m_nCardYGap;
			eraseRect.right = pPlayedCard->GetXPosition() + m_nCardWidth;
			eraseRect.bottom = pPlayedCard->GetYPosition() + m_nCardHeight;
		} 
		else 
		{
			// else need to clear area under the bottom card
			CCard* pCard2 = PLAYER(nPos).GetCardInSuit(nSuit,nEnd-1);
			eraseRect.left = pPlayedCard->GetXPosition();
			eraseRect.top = pPlayedCard->GetYPosition() + m_nCardHeight - m_nCardYGap;
			eraseRect.right = pPlayedCard->GetXPosition() + m_nCardWidth;
			eraseRect.bottom = pCard2->GetYPosition() + m_nCardHeight;
		}
		// see if the area to be erased intersects with a 
		// card on the table, and if so, erase the card
		CheckForTrickCardsOverlap(pDC, eraseRect, pPlayedCard, FALSE);
		// now finish erasing and redrawing cards 
		pPlayedCard->RestoreBackground(pDC);
		if (nStart == nEnd) 
		{
			// last card on stack, so nothing more to do
		} 
		else 
		{
			// first clear the affected card stack area
			ClearDisplayArea(&eraseRect,pDC);
			// then redraw remaining suit stack
			for(i=nStart;i<nEnd;i++) 
			{
				CCard* pCard = PLAYER(nPos).GetCardInSuit(nSuit,i);
				ASSERT(pCard != NULL);
				pCard->MoveTo(pDC, x, y);
				y += m_nCardYGap;
			}
		}

	} 
	else 
	{

		//
		nStart = pPlayedCard->GetSuitPosition();
		nEnd = 0;
		// draw the high card on the bottom
		// since we're drawing high card on bottom, we have 
		// to always redraw the entire stack
		// first convert screen suit position to ordinal pos
		int nDisplayCol = m_nDummySuitToScreenIndex[pPlayedCard->GetSuit()];
		x = m_rectSuitOffset[nPos][nDisplayCol].left;
		y = m_rectSuitOffset[nPos][nDisplayCol].top;
		cx = m_rectSuitOffset[nPos][nDisplayCol].right;
		cy = m_rectSuitOffset[nPos][nDisplayCol].bottom;
		// see if the area to be erased intersects with a 
		// card on the table, and if so, erase the card
		eraseRect = m_rectSuitOffset[nPos][nDisplayCol];
		CheckForTrickCardsOverlap(pDC, eraseRect, pPlayedCard, FALSE);
		// then erase the suit area
		ClearDisplayArea(x, y, cx, cy, pDC);
		//
		y += m_nCardYGap;
		m_rectSuitOffset[nPos][nDisplayCol].top = y;
		//
		for(j=PLAYER(nPos).GetNumCardsInSuit(nSuit)-1;j>=0;j--) 
		{
			CCard* pCard = PLAYER(nPos).GetCardInSuit(nSuit,j);
			ASSERT(pCard != NULL);
			pCard->MoveTo(pDC, x, y);
			y += m_nCardYGap;
		}
	}
	// now redraw any table cards that may have been erased
	CheckForTrickCardsOverlap(pDC, eraseRect, pPlayedCard, TRUE);
}




//
//-------------------------------------------------------------
//
// Draw cards vertically in overlapped line format
//
void CEasyBView::DrawVert(CDC* pDC, Position nPos, int nDisplaySuit,
				BOOL bClearBackground, int nStartSuit,  int nEndSuit)
{
	//
	// non-dummy east-west, face up or down, full draw
	//
	int i,j,xx,cx,nSuit;
	int x = m_drawPoint[nPos].x;
	int y = m_drawPoint[nPos].y;

	// first clear background if requested
	if ((bClearBackground) && (OffsetRectValid(nPos))) 
	{
		RECT rect;
		GetHandBoundsRect(nPos,rect);
		ClearDisplayArea(&rect,pDC);
	}
	// then check to see if we have to do anything more
	if (PLAYER(nPos).GetNumCards() == 0)
		return;
	// else draw
	int nSuitsShown = 0;
	BOOL bFaceUp = theApp.AreCardsFaceUp() || PLAYER(nPos).AreCardsExposed();
	for(i=nStartSuit=0;i<=nEndSuit;i++) 
	{
		if (bFaceUp && m_bOffsetVertSuits) 
		{
			// cards are face up -- offset display of alternate suits
		 	if ((nSuitsShown == 1) || (nSuitsShown == 3))  
			{
				if (nPos == WEST)
					xx = x + m_nHorizSuitOffset;
				else	// East
					xx = x - m_nHorizSuitOffset;
			}
			else
			{
				xx = x;
			}
			cx = x + m_nCardWidth + m_nHorizSuitOffset;
		} 
		else 
		{
			// cards are face down -- draw flush left or right
			xx = x;
			cx = x + m_nCardWidth;
		}
		nSuit = theApp.GetSuitSequence(i);
		m_rectSuitOffset[nPos][i].left = xx;
		m_rectSuitOffset[nPos][i].right = cx;
		m_rectSuitOffset[nPos][i].top = y;
		//
		for(j=0;j<PLAYER(nPos).GetNumCardsInSuit(nSuit);j++) 
		{
			CCard* pCard = PLAYER(nPos).GetCardInSuit(nSuit,j);
			ASSERT(pCard != NULL);
			pCard->MoveTo(pDC, xx, y);
			y += m_nCardYGap;
		}
		if (j > 0)
			nSuitsShown++;
		m_rectSuitOffset[nPos][i].bottom = y + m_nCardHeight - m_nCardYGap;
	}
}



//
//-------------------------------------------------------------
//
// Partial draw cards vertically in overlapped line format
//
void CEasyBView::PartialDrawVert(CDC* pDC, Position nPos)
{
	int cx,cy;
	int x = m_drawPoint[nPos].x;
	int y = m_drawPoint[nPos].y;
	//
	if (!theApp.AreCardsFaceUp()) 
	{
		//
		// non-dummy east-west, face DOWN, partial draw
		//

		// adjust clear area
		BOOL bFaceUp = theApp.AreCardsFaceUp() || PLAYER(nPos).AreCardsExposed();
		if (bFaceUp && !m_bOffsetVertSuits) 
		{
			if (nPos == WEST)
				x += m_nHorizSuitOffset;
			else
				x -= m_nHorizSuitOffset;
		}

		// and clear area under bottom card
		int numCards = PLAYER(nPos).GetNumCards();
		cx = x + m_nCardWidth;
		if (numCards > 0) 
		{
			y += m_nCardHeight + m_nCardYGap*(numCards-1);
			cy = y + m_nCardYGap;
			ClearDisplayArea(x,y,cx,cy,pDC);
			y -= m_nCardHeight;
			DrawCardBack(pDC,x,y);
		} 
		else 
		{
			cy = m_nCardHeight;
			ClearDisplayArea(x,y,cx,cy,pDC);
		}
		// and thazzit
		return;
	} 
	else 
	{
		//
		// cards are forced to be face up
		// non-dummy east-west, face up, partial draw
		//
		// redraw starting from card after the one last played
		CCard* pOldCard = pDOC->GetCurrentTrickCard(nPos);
		// restore the played card's background
		pOldCard->RestoreBackground(pDC);
		// and then restore remaining cards
		int nStart = pOldCard->GetDisplayPosition();
		int nEnd = PLAYER(nPos).GetNumCards();
		// if we're drawing with vert suit offsets, and we run out of a suit,
		// we have to redraw the whole hand
		if ((PLAYER(nPos).GetNumCardsInSuit(pOldCard->GetSuit()) == 0) &&
														(m_bOffsetVertSuits)) 
		{
			DrawVert(pDC, nPos, 0, TRUE, 0, 3);
			return;
		}
		// if this was the bottom card on the 
		// stack, then we're done
		if ((nStart == nEnd) || (nEnd == 0))
			return;
		// else clear the area under the bottom card
		// grab the card right after the card played
		CCard* pCard1 = PLAYER(nPos).GetDisplayedCard(nStart);
		CCard* pCard2 = PLAYER(nPos).GetDisplayedCard(nEnd-1);
		RECT eraseRect;
		eraseRect.left = pCard1->GetXPosition();
		eraseRect.top = pCard1->GetYPosition() + m_nCardHeight - m_nCardYGap;
		eraseRect.right = pCard1->GetXPosition() + m_nCardWidth;
		eraseRect.bottom = pCard2->GetYPosition() + m_nCardHeight;
		// expand rect if drawing with vert suit offsets
		if ( (theApp.AreCardsFaceUp() || PLAYER(nPos).AreCardsExposed()) && m_bOffsetVertSuits) 
		{
			eraseRect.left = m_drawPoint[nPos].x - m_nHorizSuitOffset;
			eraseRect.right = m_drawPoint[nPos].x + m_nCardWidth + m_nHorizSuitOffset;
		}
		ClearDisplayArea(&eraseRect,pDC);
		// and draw remaining cards starting from
		// the location of the old card
		x = pOldCard->GetXPosition();
		y = pOldCard->GetYPosition();
		int i;
		for(i=nStart;i<nEnd;i++) 
		{
			CCard* pCard = PLAYER(nPos).GetDisplayedCard(i);
			ASSERT(pCard != NULL);
			x = pCard->GetXPosition();
			pCard->MoveTo(pDC, x, y);
			y += m_nCardYGap;
		}
	}
}



//
//-------------------------------------------------------------
//
// Draw cards vertically in dummy format
//
void CEasyBView::DrawVertDummy(CDC* pDC, Position nPos, int nDisplaySuit,
				BOOL bClearBackground, int nStartSuit,  int nEndSuit)
{
	//
	// dummy east-west, face up, full draw
	//
	int i,j,nSuit;
	int xx = m_drawPoint[nPos].x + m_dummyDrawOffset[nPos].x;
	int yy = m_drawPoint[nPos].y + m_dummyDrawOffset[nPos].y;
	int x = xx;
	int y = yy;

	// first clear background if requested
	if ((bClearBackground) && (OffsetRectValid(nPos))) 
	{
		RECT rect;
		GetHandBoundsRect(nPos,rect);
		ClearDisplayArea(&rect,pDC);
	}
	//
	for(i=nStartSuit;i<=nEndSuit;i++) 
	{
		// use the DUMMY suit sequence
		nSuit = theApp.GetDummySuitSequence(i);
		m_rectSuitOffset[nPos][i].left = x;
		m_rectSuitOffset[nPos][i].top = y;
		int numInSuit = PLAYER(nPos).GetNumCardsInSuit(nSuit);
		//
		if (nPos == WEST) 
		{
			for(j=0;j<numInSuit;j++) 
			{
				CCard* pCard = PLAYER(nPos).GetCardInSuit(nSuit,j);
				ASSERT(pCard != NULL);
				pCard->MoveTo(pDC, x, y);
				x += m_nCardXGap;
			}		
			m_rectSuitOffset[nPos][i].right = x + m_nCardWidth;
		} 
		else 
		{
			// draw left to right, high card on top
			x -= (numInSuit-1) * m_nCardXGap;
			m_rectSuitOffset[nPos][i].left = x;
			for(j=numInSuit-1;j>=0;j--) 
			{
				CCard* pCard = PLAYER(nPos).GetCardInSuit(nSuit,j);
				ASSERT(pCard != NULL);
				pCard->MoveTo(pDC, x, y);
				x += m_nCardXGap;
			}		
			m_rectSuitOffset[nPos][i].right = x + m_nCardWidth;
		}
//		m_rectSuitOffset[nPos][i].right = x + m_nCardWidth;
		m_rectSuitOffset[nPos][i].bottom = y + m_nCardHeight;
		y += m_nOverlapYOffset;
		x = xx;
	}
}



//
//-------------------------------------------------------------
//
// Partial draw cards vertically in dummy format
//
void CEasyBView::PartialDrawVertDummy(CDC* pDC, Position nPos)
{
	//
	// dummy east-west, face up, partial redraw
	//

	// redrawing after a card was pulled from a stack
	CCard* pPlayedCard = pDOC->GetCurrentTrickCard(nPos);
	int nSuit = pPlayedCard->GetSuit();
	int nStartSuit = m_nDummySuitToScreenIndex[nSuit];

	// first restore the suit stack's background
	int i,j,x,y,numInSuit,nDisplaySuit;

	// set erase area
	RECT eraseRect;

	// see if E/W dummy suits overlap
	int nStart, nEnd;
	if (m_bHorizDummySuitsOverlap)
	{
		// the suits do overlap, so redraw the entire stack
		nStart = 0;
		nEnd = 3;
		// check for erase any table cards overlapping with the hand
		GetHandBoundsRect(nPos,eraseRect);
		CheckForTrickCardsOverlap(pDC, eraseRect, pPlayedCard, FALSE);
	}
	else
	{
		// no overlap, so just redraw the affected suit
		nStart = nEnd = nStartSuit;
		// check for erase any table cards overlapping with the hand
		eraseRect = m_rectSuitOffset[nPos][nStart];
		CheckForTrickCardsOverlap(pDC, eraseRect, pPlayedCard, FALSE);
	}
	// erase hand area
	ClearDisplayArea(&eraseRect,pDC);

	// then redraw all affected card stacks 
	for(i=nStart;i<=nEnd;i++) 
	{					
		// use DUMMY's suit sequence
		nDisplaySuit = theApp.GetDummySuitSequence(i);
		numInSuit = PLAYER(nPos).GetNumCardsInSuit(nDisplaySuit);
		// move cards over on suit just played
		if (nDisplaySuit == nSuit) 
		{
			if (nPos == EAST)
				m_rectSuitOffset[nPos][i].left += m_nCardXGap; 
			else
				m_rectSuitOffset[nPos][i].right -= m_nCardXGap; 
		}
		y = m_rectSuitOffset[nPos][i].top;
		if (nPos == WEST) 
		{
			// west -- draw cards left to right, low card on top
			x = m_rectSuitOffset[nPos][i].left;
			for(j=0;j<numInSuit;j++) 
			{
				CCard* pCard = PLAYER(nPos).GetCardInSuit(nDisplaySuit,j);
				ASSERT(pCard != NULL);
				pCard->MoveTo(pDC, x, y);
				x += m_nCardXGap;
			}
		} 
		else 
		{
			// draw cards left to right, high card on top
			x = m_drawPoint[nPos].x - ((numInSuit-1)*m_nCardXGap);
			for(j=numInSuit-1;j>=0;j--) 
			{
				CCard* pCard = PLAYER(nPos).GetCardInSuit(nDisplaySuit,j);
				ASSERT(pCard != NULL);
				pCard->MoveTo(pDC, x, y);
				x += m_nCardXGap;
			}
		}
	}

	// re-draw any table cards that got erased on our redraw
	CheckForTrickCardsOverlap(pDC, eraseRect, pPlayedCard, TRUE);
}




//
BOOL CEasyBView::OffsetRectValid(Position nPos)
{
	int i;
	for(i=0;i<4;i++)
		if ((m_rectSuitOffset[nPos][i].left < 0) ||
			(m_rectSuitOffset[nPos][i].top < 0) ||
			(m_rectSuitOffset[nPos][i].right < 0) ||
			(m_rectSuitOffset[nPos][i].bottom < 0))
			return FALSE;
	return TRUE;
}




//
// CheckForTrickCardsOverlap()
//
// - check for overlap of the specified rect with the current trick cards
//
void CEasyBView::CheckForTrickCardsOverlap(CDC* pDC, RECT& eraseRect, CCard* pPlayedCard, BOOL bRedraw)
{
	CRect overlapRect;
	//
	int i;
	CRgn region;

#ifdef DEBUG_MODE
	for(i=0;i<4;i++) 
	{
		// check that we're not comparing the card being played against itself
		// before it's moved onto the table
		CCard* pCard = pDOC->GetCurrentTrickCard(i); 
		if (!pCard || (pCard == pPlayedCard))
			continue;
		//
		if (overlapRect.IntersectRect(&m_rectCardDest[i],&eraseRect)) 
		{
			if (bRedraw) 
			{
				pCard->Draw(pDC);
			} 
			else 
			{
				// erase stil visible portion of card
				pCard->RestoreBackground(pDC);
			}
		}
	}
#else
	for(i=0;i<pDOC->m_numCardsPlayed;i++) 
	{
		// 
		CCard* pCard = pDOC->GetCurrentTrickCard(i); 
		if (!pCard || (pCard == pPlayedCard))
			continue;
		//
		if (overlapRect.IntersectRect(&m_rectCardDest[i],&eraseRect)) 
		{
			pCard = pDOC->m_currTrick[i];
			pCard->Draw(pDC);
		}
	}
#endif
}



//
//
//
void CEasyBView::AnimateTrick(int nWinner)
{
	int i;
	CCard* pCard;
	CDC* pDC = GetDC();
/*
	// start animation with first card to winner's left
	int nNext = nWinner + 1;
	int nIndex = nNext - pDOC->m_nRoundLead;
	// account for negative values
	int nPos = (nIndex + 4) % 4;
	for(i=0;i<3;i++) 
	{
		pCard = pDOC->m_currTrick[nPos];
		ASSERT(pCard != NULL);
		pCard->RestoreBackground(pDC);
		pCard->Animate(pDC, m_cardDest[nWinner].x, m_cardDest[nWinner].y, FALSE);
		nPos = theApp.GetNextPlayer(nPos);
	}
*/
	int nPos = nWinner;
	// slow down card sweep display
	int nGranularity = (int)((float)m_nAnimationGranularity * 0.75);
	for(i=0;i<3;i++) 
	{
		nPos = GetNextPlayer(nPos);
		pCard = pDOC->GetCurrentTrickCard(nPos);
		ASSERT(pCard != NULL);
		pCard->RestoreBackground(pDC);
		pCard->Animate(pDC, m_cardDest[nWinner].x, m_cardDest[nWinner].y, FALSE, nGranularity);
	}
	// then clear and anmiate the winning card
	CCard* pWinningCard = pDOC->GetCurrentTrickCard(nWinner);
/*
	ClearDisplayArea(pWinningCard->GetXPosition(),
					 pWinningCard->GetYPosition(),
					 pWinningCard->GetXPosition()+m_nCardWidth,
					 pWinningCard->GetYPosition()+m_nCardHeight,
					 pDC);
	pWinningCard->Draw(pDC);
*/
	pWinningCard->RestoreBackground(pDC);
	// animate the card 
	pWinningCard->Animate(pDC, m_drawPoint[nWinner].x, m_drawPoint[nWinner].y, TRUE);
	// all done
	ReleaseDC(pDC);
}




//
// ClearPartialTrickCards()
//
void CEasyBView::ClearPartialTrickCards()
{
	int i;
	CCard* pCard;
	CDC* pDC = GetDC();

	int nPos = pDOC->GetRoundLead();
	int numCards = pDOC->GetNumCardsPlayedInRound();

	// 
	for(i=0;i<numCards;i++) 
	{
		pCard = pDOC->GetCurrentTrickCardByOrder(i);
		ASSERT(pCard != NULL);
		pCard->RestoreBackground(pDC);
	}

	// all done
	ReleaseDC(pDC);
}




//
//----------------------------------------------------------
//
// return the bottommost card in the player's current
// displayed hand
//
CCard* CEasyBView::GetPlayerBottomCard(CPlayer& player)
{
	int i,nSuit,nBottomSuit;
	// first find bottom suit
	for(i=3;i>=0;i--) 
	{
		if (player.IsDummy())
			nSuit = theApp.GetDummySuitSequence(i);
		else
			nSuit = theApp.GetSuitSequence(i);
		if(player.GetNumCardsInSuit(nSuit) > 0)
		break;
	}
	nBottomSuit = nSuit;
	//
	i = player.GetNumCardsInSuit(nBottomSuit);
	CCard* pCard = player.GetCardInSuit(nBottomSuit,i-1);
	return pCard;
}




//
//----------------------------------------------------------
// draw a cardback
//
void CEasyBView::DrawCardBack(CDC* pDC, int x, int y)
{
	CDC cardDC,cacheDC,maskDC;
	// create DCs
	cardDC.CreateCompatibleDC(pDC);
	cacheDC.CreateCompatibleDC(pDC);
	maskDC.CreateCompatibleDC(pDC);

	// load bitmaps
	CBitmap* pOldBitmap1 = (CBitmap*) cardDC.SelectObject(deck.GetCardBackBitmap());
	CBitmap* pOldBitmap2 = (CBitmap*) maskDC.SelectObject(deck.GetMaskBitmap());
    CBitmap tempBitmap;
    tempBitmap.CreateCompatibleBitmap(pDC, m_nCardWidth, m_nCardHeight);
	CBitmap* pOldBitmap3 = (CBitmap*) cacheDC.SelectObject(&tempBitmap);
	// first copy screen background to cacheDC
	cacheDC.BitBlt(0,0,m_nCardWidth,m_nCardHeight,pDC,x,y,SRCCOPY);
	// then copy the mask bitmap to the cacheDC
	cacheDC.BitBlt(0,0,m_nCardWidth,m_nCardHeight,&maskDC,0,0,SRCAND);
	// then copy the bitmap onto the cacheDC
	cacheDC.BitBlt(0,0,m_nCardWidth,m_nCardHeight,&cardDC,0,0,SRCPAINT);
	// and finally copy the cacheDC onto the screen
	pDC->BitBlt(x,y,m_nCardWidth,m_nCardHeight,&cacheDC,0,0,SRCCOPY);
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

