//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "EasyBvw.h"
#include "mainfrm.h"
#include "Player.h"
#include "deck.h"
#include "card.h"
#include "progopts.h"
#include "docopts.h"
#include "DrawParameters.h"
#include "ScreenSizeWarningDlg.h"
#include <limits.h>
#include <math.h>




//
void CEasyBView::AdvanceToNextPlayer() 
{
	// see whether this is a human or computer player
	BOOL bManualPlay = FALSE;
	int nPlayMode = theApp.GetValue(tnCardPlayMode);
	if ((pDOC->GetCurrentPlayer()->IsHumanPlayer()) &&
					(nPlayMode != CEasyBApp::PLAY_FULL_AUTO) &&
					(nPlayMode != CEasyBApp::PLAY_FULL_AUTO_EXPRESS))
		bManualPlay = TRUE;
	else if ((theApp.GetValue(tbManualPlayMode)) ||
			 (nPlayMode == CEasyBApp::PLAY_MANUAL) ||
		     ((nPlayMode == CEasyBApp::PLAY_MANUAL_DEFEND) && 
			  (pDOC->GetCurrentPlayer()->IsDefending())) ) 
		bManualPlay = TRUE;
	// it's not manual if computer is replaying
	if (pDOC->GetValue(tbAutoReplayMode))
		bManualPlay = FALSE;

	//
	if (bManualPlay)
	{
		// this is a human player
		// first see if autoplay last card option is enabled
		if (theApp.GetValue(tbAutoPlayLastCard))
		{
			CPlayer* pPlayer = pDOC->GetCurrentPlayer();
			if (pPlayer->TestForAutoPlayLastCard())
				return;
		}

		// jump the cursor if appropriate
		if (theApp.GetValue(tbAutoJumpCursor))
			JumpCursor();

		// set the prompt
		CString strMessage;
		strMessage.Format("%s's turn -- select a card to play.",
					PositionToString(pDOC->GetCurrentPlayerPosition()));
		pMAINFRAME->SetStatusText(strMessage);

		// and set status code
		SetCurrentMode(MODE_WAITCARDPLAY);

		// finally, show auto hint if appropriate
		pDOC->ShowAutoHint();
	}
	else
	{
		// this is a computer player
		SetCurrentMode(MODE_NONE);	// clear up loose ends
		BOOL bExpressMode = theApp.InExpressAutoPlay();
		// prompt if not in express mode
		if (!bExpressMode && !theApp.GetValue(tbAutoTestMode))
		{
			CWaitCursor wait;
			CString strMessage;
			if ( (!pDOC->GetCurrentPlayer()->IsDefending() && theApp.GetValue(tbEnableGIBForDeclarer)) || 
				 (pDOC->GetCurrentPlayer()->IsDefending() && theApp.GetValue(tbEnableGIBForDefender)) )
				strMessage.Format("%s is playing (GIB)...",
							PositionToString(pDOC->GetCurrentPlayerPosition()));
			else
				strMessage.Format("%s is playing...",
							PositionToString(pDOC->GetCurrentPlayerPosition()));
			SetPrompt(strMessage);
		}

		// and move to the next player
		// don't pop up wait cursor if in auto mode!
		if (!bExpressMode)
		{
			CWaitCursor wait;
			pDOC->InvokeNextPlayer();
		}
		else
		{
			pDOC->InvokeNextPlayer();
		}
	}
}


//
void CEasyBView::PromptLead() 
{
	//
	if ((!pDOC->GetCurrentPlayer()->IsHumanPlayer()) &&
		(!theApp.GetValue(tbManualPlayMode)))
		return;
	//
	CString strMessage;
	strMessage.Format("%s leads -- select a card to play.",
				PositionToString(pDOC->GetCurrentPlayerPosition()));
	pMAINFRAME->SetStatusText(strMessage);
}

//
void CEasyBView::PromptGameOver() 
{
	CString strMessage;
	strMessage.Format("Hand is over.  Press F2 for another hand.");
	pMAINFRAME->SetStatusText(strMessage);
}


//
void CEasyBView::SetPrompt(LPCSTR szText, BOOL bLockPane) 
{
	pMAINFRAME->SetStatusText(szText, 0, bLockPane);
}


//
void CEasyBView::ResetPrompt(BOOL bLockPane) 
{
	pMAINFRAME->SetStatusText(AFX_IDS_IDLEMESSAGE, bLockPane);
}


//
void CEasyBView::ClearPrompt(BOOL bLockPane) 
{
	pMAINFRAME->SetStatusText("",0,bLockPane);
}


//
BOOL CEasyBView::CanDealNewHand() 
{
	switch(m_nCurrMode)
	{
		// new deal illegal in the following modes
		case MODE_TITLESCREEN:  
		case MODE_INTROSCREEN:
		case MODE_WAITTIMER:
		case MODE_GAMERESTORE:
//		case MODE_THINKING:
			return FALSE;

		// new deal OK in the following modes
		case MODE_NONE:  
		case MODE_WAITCARDPLAY:		
		case MODE_CLICKTOBEGINPLAY:
		case MODE_CLICKFORNEXTTRICK:		
		case MODE_CLICKTORESTARTTRICK:
		case MODE_CLICKTOCONTINUE:
		case MODE_CLICKFORNEXTGAME:
		case MODE_WAITKEYPRESS:
		case MODE_CARDLAYOUT:
		case MODE_EDITHANDS:
		case MODE_WAITTOBID:
		case MODE_GAMEREVIEW:
		case MODE_THINKING:
			return TRUE;

		// otherwise return false (should never get here!)
		default:
			return FALSE;	// don't know
	}
}




//
// CanSaveFile() 
//
BOOL CEasyBView::CanSaveFile() 
{
	switch(m_nCurrMode)
	{
		//
		case MODE_CARDLAYOUT:
		case MODE_EDITHANDS:
//			if (m_numCardsUnassigned > 0)
				return FALSE;
			break;

		// other restrictions?
		case MODE_THINKING:
			return FALSE;

		default:
			break;
	}

	// default is TRUE
	return TRUE;
}




//
// DrawPlayerLabels()
//
// draw player position labels
//
void CEasyBView::DrawPlayerLabels(CDC* pDC)
{
/*
	// first draw labels
	TEXTMETRIC tm;
	int x,y;
	CFont* pOldFont = pDC->SelectObject(&m_largeFont);
	pDC->GetTextMetrics(&tm);
	int cyChar = tm.tmHeight;
	CSize size;

	// do north
	// skip if laying out cards (cards will obscure label)
	if ((m_nCurrMode != MODE_CARDLAYOUT) &&
					(m_nCurrMode != MODE_EDITHANDS)) 
					{
		x = m_drawPoint[NORTH].x;
		y = m_drawPoint[NORTH].y;
		if (pDOC->m_nDummy == NORTH) 
		{
			x += m_dummyLabelOffset[NORTH].x;
			y += m_dummyLabelOffset[NORTH].y;
		}
		size = pDC->GetTextExtent("North",5);
		pDC->TextOut(x - size.cx - 2, y, "North");
	}

	// do East
	size = pDC->GetTextExtent("East",4);
	x = m_drawPoint[EAST].x;
	int dx = (m_nCardWidth - size.cx) / 2;
	if (dx < 0)
		dx = 0;
	y = m_drawPoint[EAST].y - cyChar-2;
	if (pDOC->m_nDummy == EAST) 
	{
		x += m_dummyLabelOffset[EAST].x;
		y += m_dummyLabelOffset[EAST].y;
	}
	pDC->TextOut(x + dx, y, "East");

	// do south
	x = m_drawPoint[SOUTH].x;
	y = m_drawPoint[SOUTH].y;
	if (pDOC->m_nDummy == SOUTH) 
	{
		x += m_dummyLabelOffset[SOUTH].x;
		y += m_dummyLabelOffset[SOUTH].y;
	}
	pDC->TextOut(x, y-cyChar-2, "South");

	// do west
	size = pDC->GetTextExtent("West",4);
	x = m_drawPoint[WEST].x;
	dx = (m_nCardWidth - size.cx) / 2;
	if (dx < 0)
		dx = 0;
	y = m_drawPoint[WEST].y-cyChar-2;
	if (pDOC->m_nDummy == WEST) 
	{
		x += m_dummyLabelOffset[WEST].x;
		y += m_dummyLabelOffset[WEST].y;
	}
	pDC->TextOut(x + dx, y, "West");

	//
	(void)pDC->SelectObject(pOldFont);
*/
}



//
// DrawPlayingField()
//
// draw labels, hands, & trick cards on table
//
void CEasyBView::DrawPlayingField(CDC* pDC)
{
	CEasyBDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc->IsInitialized())
		return;

	// first draw labels
	DrawPlayerLabels(pDC);

	// then draw cards
	int i;
	RECT clipRect,clientRect;
	GetClientRect(&clientRect);
	pDC->GetClipBox(&clipRect);
	BOOL bFullDraw = FALSE;

	// check for draw mode
	// full redraw under the following circumstances
	if ((clipRect.left == 0) && (clipRect.top == 0) &&
	    (clipRect.right == 0) && (clipRect.bottom == 0))
		bFullDraw = TRUE;
	if ((clipRect.left == clientRect.left) &&
		(clipRect.top == clientRect.top) &&
		(clipRect.right == clientRect.right) &&
		(clipRect.bottom == clientRect.bottom)) 
		bFullDraw = TRUE;

	if (bFullDraw) 
	{
		// do a full redraw
		for(i=0;i<4;i++) 
			DisplayHand((Position)i, -1, FALSE);
	} 
	else 
	{
		// only doing a partial redraw
		// but need to redraw a hand fully even if 
		// it was only partially obscured
		CRect newRect;
		for(i=0;i<4;i++) 
		{
			if ( ((newRect.IntersectRect(&clipRect,&m_rectSuitOffset[i][0])) ||
			      (newRect.IntersectRect(&clipRect,&m_rectSuitOffset[i][1])) ||
			      (newRect.IntersectRect(&clipRect,&m_rectSuitOffset[i][2])) ||
			      (newRect.IntersectRect(&clipRect,&m_rectSuitOffset[i][3])) ) ||
				  (m_rectSuitOffset[i][0].left == -1) )
				DisplayHand((Position)i);
		}		
	}

	// and any trick cards on the table
	if (theApp.GetValue(tbGameInProgress) || pDOC->IsReviewingGame() || 
					(m_nCurrMode == MODE_CARDLAYOUT))
		DrawTableCards(pDC, bFullDraw);
}



//
// DrawTableCards()
//
// draw cards played on the table in the current trick
//
void CEasyBView::DrawTableCards(CDC* pDC, BOOL bFullDraw)
{
	CEasyBDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// and any trick cards on the table
	int numCards = pDOC->GetNumCardsPlayedInRound();
	CCard* pCard;
	// gotta stretch the clipping region to make sure table 
	// cards are visible
	CDC* pNewDC = GetDC();
	int nPos = pDOC->GetRoundLead();
	for(int i=0;i<numCards;i++) 
	{
		pCard = pDOC->GetCurrentTrickCard(nPos);
		// it's possible the cards may be null 
		// (e.g., after the end of play, when reviewing hands)
		if (pCard)
		{
			if (!bFullDraw)
				pCard->RestoreBackground(pNewDC);
			pCard->Draw(pNewDC);
		}
		nPos = GetNextPlayer(nPos);
	}
	ReleaseDC(pNewDC);
}



//
// DrawCardLayoutMode()
//
// draw cards in manual layout mode
//
void CEasyBView::DrawCardLayoutMode(CDC* pDC, BOOL bDrawPile, BOOL bDrawHands)
{
	CEasyBDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int i,j,nSuit;
	CCard* pCard;
	CDC cacheDC;
   	CBitmap tempBitmap,*pOldBitmap;

	// check draw mode
//	if ((bDrawPile) && (!m_bLayoutDlgActive)) {
	if (bDrawPile) 
	{

		// first draw labels
		DrawPlayerLabels(pDC);
		int x,y;
		int nWidth = 12*m_nLayoutXGap  + m_nCardWidth;
		int nHeight = 3*m_nLayoutYGap  + m_nCardHeight;
		// then clear
		ClearDisplayArea(&m_rectLayout,pDC);
		// then draw
		if (m_bTripleBuffer) 
		{
			// use triple buffering
			cacheDC.CreateCompatibleDC(pDC);
			RECT rect;
			GetClientRect(&rect);
		    tempBitmap.CreateCompatibleBitmap(pDC, rect.right, rect.bottom);
			pOldBitmap = (CBitmap*) cacheDC.SelectObject(&tempBitmap);
			cacheDC.BitBlt(m_rectLayout.left,m_rectLayout.top,
						   nWidth,nHeight,
						   pDC,
						   m_rectLayout.left,m_rectLayout.top,
						   SRCCOPY);
		}
		//
		y = m_rectLayout.top;
		for(i=0;i<4;i++) 
		{
			nSuit = theApp.GetSuitSequence(i);
			x = m_rectLayout.left;
			for(j=12;j>=0;j--) 
			{
				pCard = m_layoutDeck[nSuit*13+j];
				if (pCard != NULL) 
				{
					if ((!pCard->IsCardAssigned()) &&
						(pCard != m_pLayoutCard)) 
					{
						if (m_bTripleBuffer)
							pCard->MoveTo(&cacheDC,x,y);
						else
							pCard->MoveTo(pDC,x,y);
					}
				}
				x += m_nLayoutXGap;
			}
			y += m_nLayoutYGap;
		}
		if (m_bTripleBuffer) 
		{
			// copy results of offscreen blitting to the screen
			pDC->BitBlt(m_rectLayout.left,m_rectLayout.top,
						nWidth,nHeight,
						&cacheDC,
						m_rectLayout.left,m_rectLayout.top,
						SRCCOPY);
			// done with triple buffering
			(void)cacheDC.SelectObject(pOldBitmap);
			cacheDC.DeleteDC();
			tempBitmap.DeleteObject();
		}
	}

	//
	if (bDrawHands) 
	{

		// now draw hands
		for(i=0;i<4;i++) 
			DisplayHand((Position)i);

		// draw any highlighted (exchange) cards
		if ((m_bExchanging) && (m_pExchangeCard)) 
		{
			m_pExchangeCard->DrawHighlighted(pDC,TRUE);
		}

	}

	// all done
}



//
// EnableCardAnimation()
//
BOOL CEasyBView::EnableCardAnimation(BOOL bEnable) 
{ 
	BOOL bOldMode = m_bAnimateCards; 
	m_bAnimateCards = bEnable; 
	return bOldMode; 
}



//
// Throw a card down -- i.e., play a card on the table
//
void CEasyBView::ThrowCard(Position nPos, CCard* pCard) 
{
	CDC *pDC = GetDC();
	int x,y;

	// record the card played
	pDOC->EnterCardPlay(nPos, pCard);

	// provide feedback
	CString strLine,strTemp;
	strLine = "Tricks: ";
	for(int i=0;i<pDOC->GetNumCardsPlayedInRound();i++) 
	{
		CCard* pCard = pDOC->GetCurrentTrickCard(i);
		if (pCard)
			strTemp = pCard->GetName();
		strLine += strTemp;
		strLine += " ";
	}
	FEEDBACK(strLine);

	// skip drawing if in express play mode, OR if updates are disabled
	if (theApp.InExpressAutoPlay() || (m_nSuppressRefresh > 0))
	{
		if ((pDOC->GetNumTricksPlayed() == 0) &&
				(pDOC->GetNumCardsPlayedInRound() == 1)) 
			pDOC->ExposeDummy(TRUE);
		return;
	}

	//
	// now then redraw affected suit
	//
	DisplayHand(nPos, pCard->GetSuit(), TRUE, TRUE);

	// see if this was the lead card of the hand -- and if so,
	// turn up dummy's cards _IF_ it's being played by a human
	// if it's a computer player, it will be automatically 
	// redrawn after its cards is played (avoid redrawing the
	// dummy twice)
	if ((pDOC->GetNumTricksPlayed() == 0) &&
			(pDOC->GetNumCardsPlayedInRound() == 1)) 
	{
		pDOC->ExposeDummy(TRUE);
		int nextPos = GetNextPlayer(nPos);
		PLAYER(nextPos).ExposeCards(TRUE);
	}

	// now draw the played card on the table
	// if the hand is hidden, make sure the draw starts at the hand origin
	if ( (!pCard->IsCardFaceUp()) && (!theApp.AreCardsFaceUp()) )
		pCard->MoveTo(pDC, 	m_drawPoint[nPos].x, m_drawPoint[nPos].y, FALSE);

	// then expose the card
	pCard->SetFaceUp();

	// and animate the card if desired 
	x = m_cardDest[nPos].x;
	y = m_cardDest[nPos].y;
	if (m_bAnimateCards) 
	{
		pCard->Draw(pDC);
		pCard->Animate(pDC, x, y);
	} 
	else 
	{
		pCard->MoveTo(pDC, x, y);
	}
	// all done
	//
	ReleaseDC(pDC);
}



//
// Draw a played card without doing anything else
//
void CEasyBView::DrawPlayedCard(Position nPos, CCard* pCard, BOOL bShowAtEnd) 
{
	CDC *pDC = GetDC();
	int x,y;

	// expose the card
	pCard->SetFaceUp();

	// and draw at the new location
	x = m_cardDest[nPos].x;
	y = m_cardDest[nPos].y;
	pCard->MoveTo(pDC, x, y, bShowAtEnd);

	// all done
	ReleaseDC(pDC);
}




//
// JumpCursor()
//
void CEasyBView::JumpCursor(int nPlayer)
{
	if (nPlayer == NONE)
		nPlayer = pDOC->GetCurrentPlayerPosition();
	POINT pt;
	pt.x = m_drawPoint[nPlayer].x + (m_nCardWidth/2);
	pt.y = m_drawPoint[nPlayer].y + (m_nCardHeight/2);
	ClientToScreen(&pt);
	SetCursorPos(pt.x,pt.y);
}




//
void CEasyBView::ClearDisplayArea(RECT* pRect, CDC* pSentDC) 
{
	if (pRect && 
		((pRect->left == -1) || (pRect->top == -1) ||
		 (pRect->right == -1) || (pRect->bottom == -1)) )
	{
//		AfxMessageBox("Illegal ClearDisplayArea call");
		return; 
	}
	CDC* pDC;
	if (pSentDC == NULL)
		pDC = GetDC();
	else
		pDC = pSentDC;
	CRgn region;
	// save old clipping box
	RECT rect,origRect;
	pDC->GetClipBox(&origRect);
	if (pRect)  
	{
		region.CreateRectRgn(pRect->left,pRect->top,
							 pRect->right,pRect->bottom);
	} 
	else 
	{
		GetClientRect(&rect);
		region.CreateRectRgn(rect.left,rect.top,
							 rect.right,rect.bottom);
	}
	pDC->SelectClipRgn(&region);    // Erase the area needed
	OnEraseBkgnd(pDC);
	// restore original clip region/rect
	region.SetRectRgn(origRect.left,origRect.top,
						 origRect.right,origRect.bottom);
	pDC->SelectClipRgn(&region);
	//
	region.DeleteObject();
	if (pSentDC == NULL)
		ReleaseDC(pDC);
}



//
void CEasyBView::ClearDisplayArea(int nLeft, int nTop, 
								  int nRight, int nBottom, 
								  CDC* pSentDC) 
{
	if ((nLeft == -1) || (nTop == -1) ||
		(nRight == -1) || (nBottom == -1)) 
	{
//		AfxMessageBox("Illegal ClearDisplayArea call");
		return; 
	}
	CDC* pDC;
	if (pSentDC == NULL)
		pDC = GetDC();
	else
		pDC = pSentDC;
	// save old clipping box
	RECT origRect;
	pDC->GetClipBox(&origRect);
	CRgn region;
	region.CreateRectRgn(nLeft,nTop,nRight,nBottom);
	pDC->SelectClipRgn(&region);    // Erase the area needed
	OnEraseBkgnd(pDC);
	// restore original clip region/rect
	region.SetRectRgn(origRect.left,origRect.top,
						 origRect.right,origRect.bottom);
	pDC->SelectClipRgn(&region);
	//
	region.DeleteObject();
	if (pSentDC == NULL)
		ReleaseDC(pDC);
}




//
// clear the table after a trick has been played
//
void CEasyBView::ClearTable() 
{
	// make sure the mode & cursor is updated immediately
	SetCurrentMode(MODE_NONE);
	//
	if (m_bAnimateCards) 
	{
		AnimateTrick(pDOC->GetRoundWinner());
	} 
	else 
	{
		// no animation?  simply restore each table card's background
		CDC *pDC = GetDC();
		for(int i=0;i<4;i++) 
		{
			CCard* pCard = pDOC->GetCurrentTrickCard(i);
			ASSERT(pCard != NULL);
			pCard->RestoreBackground(pDC);
		}
		ReleaseDC(pDC);
	}
}


//
// clear the table without doing any animation
// and redraw card stacks if necessary
//
void CEasyBView::ClearTableRegion(CDC* pSentDC) 
{
	// clear the table region
	CDC* pDC;
	if (pSentDC == NULL)
		pDC = GetDC();
	else
		pDC = pSentDC;
	pDC->SelectClipRgn(&m_tableRegion);  // Erase the play area
	OnEraseBkgnd(pDC);

	// see if we need to redraw anyone's card stacks
	int i,nPlayer,nSuit;
	CRect overlapRect;
	RECT rect;
	//
	for(nPlayer=0;nPlayer<4;nPlayer++) 
	{
		for(i=0;i<4;i++) {
			rect = m_rectSuitOffset[nPlayer][i];
			if (m_tableRegion.RectInRegion(&rect)) 
			{
				// first convert screen pos to suit #
				if ((m_nCurrMode == MODE_CARDLAYOUT) || (m_nCurrMode == MODE_EDITHANDS))
				{
					nSuit = theApp.GetSuitSequence(i);
				}
				else 
				{
					if (PLAYER(nPlayer).IsDummy())
						nSuit = theApp.GetDummySuitSequence(i);
					else
						nSuit = theApp.GetSuitSequence(i);
				}
				DisplayHand((Position)nPlayer, nSuit);
			}
		}
	}
	//
	if (pSentDC == NULL)
		ReleaseDC(pDC);
}



//
void CEasyBView::ResetSuitSequence() 
{
	for(int i=0;i<4;i++) 
	{
		for(int j=0;j<4;j++) 
		{
			if (theApp.GetSuitSequence(j) == i) 
				m_nSuitToScreenIndex[i] = j;
		}
	}
	//
	ResetDummySuitSequence();
}



//
void CEasyBView::ResetDummySuitSequence() 
{
	for(int i=0;i<4;i++) 
	{
		for(int j=0;j<4;j++) 
		{
			if (theApp.GetDummySuitSequence(j) == i) 
				m_nDummySuitToScreenIndex[i] = j;
		}
	}
}



//
void CEasyBView::InitNewRound() 
{
	// clear card display locations
	ResetSuitOffsets();
	Invalidate();
}


//
void CEasyBView::ResetSuitOffsets(Position nPos) 
{
	int nStartPos,nEndPos;
	if ((nPos >= SOUTH) && (nPos <= EAST)) 
	{
		nStartPos = nEndPos = nPos;
	} 
	else 
	{
		nStartPos = SOUTH;
		nEndPos = EAST;
	}
	int i,j;
	for(i=nStartPos;i<=nEndPos;i++) 
	{
		for(j=0;j<4;j++) 
		{
			m_rectSuitOffset[i][j].left = -1;
			m_rectSuitOffset[i][j].top = -1;
			m_rectSuitOffset[i][j].right = -1;
			m_rectSuitOffset[i][j].bottom = -1;
		}	
	}
		
}


// list of destinations for animated cards
POINT nAnimCardDest[] = {
	// letter 'E'
	{  30,  10 },
	{  30,  60 },
	{  30, 110 },
	{  30, 160 },
	{  30, 210 },
	{  30, 260 },
	{  30, 310 },
	{  80,  10 },
	{ 130,  10 },
	{ 180,  10 },
	{ 230,  10 },
	{  80, 160 },
	{ 130, 160 },
	{ 180, 160 },
	{  80, 310 },
	{ 130, 310 },
	{ 180, 310 },
	{ 230, 310 },
	// letter 'Z'
	{ 340,  10 },
	{ 390,  10 },
	{ 440,  10 },
	{ 490,  10 },
	{ 540,  10 },
	{ 507,  60 },
	{ 473, 110 },
	{ 440, 160 },
	{ 407, 210 },
	{ 373, 260 },
	{ 340, 310 },
	{ 390, 310 },
	{ 440, 310 },
	{ 490, 310 },
	{ 540, 310 },
	// done
	{  -1,  -1 },
};

const int MIN_GRANULARITY = 10;

//
void CEasyBView::DoOpeningSequence()
{
	// avoid repetition of the opening sequence
	if (m_bOpeningSequenceDone)
		return;
	CCard *pCard;
	CDC* pDC = GetDC();
	RECT rect;
	GetClientRect(&rect);
	int bx = rect.left;
	int by = rect.bottom - m_nCardHeight;
	int distance,nGranularity;

	// need to turn cards face up
	BOOL bCardsFaceUpMode = theApp.AreCardsFaceUp();
	theApp.SetValue(tbShowCardsFaceUp, TRUE);

	//
	ClearPrompt();
	//
	POINT nextPoint;
	int nIndex = 0;
	nextPoint = nAnimCardDest[nIndex];
	do {
		pCard = deck[nIndex];
		pCard->MoveTo(pDC, bx, by);
		pCard->RestoreBackground(pDC);
		// try to equalize out animation time
		distance = (int) sqrt(pow(nextPoint.x-bx,2) + pow(nextPoint.y-by,2));
		nGranularity = distance / 5;
		if (nGranularity < MIN_GRANULARITY)
			nGranularity = MIN_GRANULARITY;
		//
		pCard->Animate(pDC, nextPoint.x, nextPoint.y, FALSE, nGranularity);
		nIndex++;
		nextPoint = nAnimCardDest[nIndex];
	} while (nextPoint.x > 0);

	// 
	// now draw the title letters
	//
	CDC titleDC,cacheDC,maskDC;
	// create DCs
	titleDC.CreateCompatibleDC(pDC);
	cacheDC.CreateCompatibleDC(pDC);
	maskDC.CreateCompatibleDC(pDC);
	// set background color for the cache bitmap
	titleDC.SetBkColor(RGB(0, 0, 0));
	// load bitmaps
	CBitmap bmTitle1;
//	CBitmap bmTitle2;
	bmTitle1.LoadBitmap(IDB_TITLE);
//	bmTitle2.LoadBitmap(IDB_TITLE1);
	// get bitmap info
	BITMAP bitmapInfo;
	bmTitle1.GetObject(sizeof(BITMAP),&bitmapInfo);
	int nBMWidth = bitmapInfo.bmWidth;
	int nBMHeight = bitmapInfo.bmHeight;
	// and center onscreen
	int titleX = (rect.right - nBMWidth) / 2;
	int titleY = (rect.bottom - nBMHeight) / 2;
	//
	CBitmap* pOldBitmap0 = (CBitmap*) titleDC.SelectObject(&bmTitle1);
	// Create the bitmap mask
    CBitmap tempBitmap1;
    tempBitmap1.CreateCompatibleBitmap(&maskDC, nBMWidth, nBMHeight);
	CBitmap* pOldBitmap1 = (CBitmap*) maskDC.SelectObject(&tempBitmap1);
	maskDC.BitBlt(0,0,nBMWidth,nBMHeight,&titleDC,0,0,SRCCOPY);
	// and the cache bitmap
    CBitmap tempBitmap2;
    tempBitmap2.CreateCompatibleBitmap(pDC, nBMWidth, nBMHeight);
	CBitmap* pOldBitmap2 = (CBitmap*) cacheDC.SelectObject(&tempBitmap2);
	// first copy screen background to cacheDC
	cacheDC.BitBlt(0,0,nBMWidth,nBMHeight,pDC,titleX,titleY,SRCCOPY);
	// now mask the cacheDC
	cacheDC.BitBlt(0,0,nBMWidth,nBMHeight,&maskDC,0,0,SRCAND);
	// then copy the bitmap onto the cacheDC
	cacheDC.BitBlt(0,0,nBMWidth,nBMHeight,&titleDC,0,0,SRCPAINT);
	// and finally copy the cacheDC onto the screen
	pDC->BitBlt(titleX,titleY,nBMWidth,nBMHeight,&cacheDC,0,0,SRCCOPY);
	// all done
	(void)titleDC.SelectObject(pOldBitmap0);
	(void)maskDC.SelectObject(pOldBitmap1);
	(void)cacheDC.SelectObject(pOldBitmap2);
	titleDC.DeleteDC();
	maskDC.DeleteDC();
	cacheDC.DeleteDC();
	tempBitmap1.DeleteObject();
	tempBitmap2.DeleteObject();
	bmTitle1.DeleteObject();
	//
	pMAINFRAME->SetStatusText("Welcome to Easy Bridge!");
	// done
	ReleaseDC(pDC);

	// restore cards face up status
	theApp.SetValue(tbShowCardsFaceUp, bCardsFaceUpMode);

	//
	m_bOpeningSequenceDone = TRUE;
}



//
void CEasyBView::OnDoRealize()
{
	CEasyBDoc* pDoc = GetDocument();
	if (m_bCanDisplayBitmap == NULL)
		return;  // must be a new document

	//
	CPalette* pPal = m_customDIB.GetPalette();
	if (pPal != NULL) 
	{
		CMainFrame* pAppFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
		ASSERT(pAppFrame->IsKindOf(RUNTIME_CLASS( CMainFrame )));

		CClientDC appDC(pAppFrame);
		// All views but one should be a background palette.
		// wParam contains a handle to the active view, so the SelectPalette
		// bForceBackground flag is FALSE only if wParam == m_hWnd (this view)
		CPalette* oldPalette = appDC.SelectPalette(pPal, FALSE);

		if (oldPalette != NULL) 
		{
			UINT nColorsChanged = appDC.RealizePalette();
			if (nColorsChanged > 0)
				pDoc->UpdateAllViews(NULL);
			appDC.SelectPalette(oldPalette, TRUE);
		} 
		else 
		{
			TRACE0("\tSelectPalette failed in CDibView::OnPaletteChanged\n");
		}
	}
	return;
}


//
BOOL CEasyBView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
/*
	// check the current mode
	switch (m_nCurrMode)
	{
		case MODE_WAITCARDPLAY:
			// use a hand cursor
//			::SetCursor(m_hCursorHand);
			::SetCursor(m_hCurrentCursor);
			break;

		case MODE_CLICKTOBEGINPLAY:
		case MODE_CLICKFORNEXTTRICK:
		case MODE_CLICKTORESTARTTRICK:
		case MODE_CLICKTOCONTINUE:
		case MODE_CLICKFORNEXTGAME:
			// show the click prompt cursor
			::SetCursor(m_hCursorClick);
			break;

		default:
			// use the current defined cursor
			::SetCursor(m_hCurrentCursor);
			break;
	}
*/
	//
	::SetCursor(m_hCurrentCursor);
	return TRUE;
//	return CView::OnSetCursor(pWnd, nHitTest, message);
}


//
void CEasyBView::SetWindowCursor(HCURSOR hCursor)
{
	//
	if (!m_bResourcesInitialized)
	{
		m_hCurrentCursor = NULL;
		return;
	}

	// override if in special mode
	if (hCursor == NULL)
	{
		switch (m_nCurrMode)
		{
			case MODE_CLICKTOBEGINPLAY:
			case MODE_CLICKTORESUMEPLAY:
			case MODE_CLICKFORNEXTTRICK:
			case MODE_CLICKTORESTARTTRICK:
			case MODE_CLICKTOCONTINUE:
			case MODE_CLICKFORNEXTGAME:
				// show the click prompt cursor
				m_hCurrentCursor = m_hCursorClick;
				break;

			case MODE_WAITCARDPLAY:
				// show the hand 
				m_hCurrentCursor = m_hCursorHand;
				break;

			case MODE_GAMEREVIEW:
			case MODE_GAMERESTORE:
			case MODE_THINKING:
			case MODE_WAITSPECIALDEAL:
				m_hCurrentCursor = m_hCursorBusy;
				break;

			default:
				// use the default cursor
				m_hCurrentCursor = m_hDefaultCursor;
				break;
		}
	}
	else
	{
		// use the specified cursor
		m_hCurrentCursor = hCursor;
	}

	//	
	::SetCursor(m_hCurrentCursor);
}


//
void CEasyBView::SetViewParameters(int cx, int cy)
{
	// skip if view is not yet initialized
	if (!m_bViewInitialized)
		return;
	
	BOOL bSmallCards = theApp.GetValue(tbLowResOption);
	m_nCardWidth = deck.GetCardWidth();
	m_nCardHeight = deck.GetCardHeight();
	//
	int nDefaultWidth, nDefaultHeight;
	int dx, dy;

	// see if we're using small cards
	if (bSmallCards)
	{
		// using small cards
		nDefaultWidth = 472;
		nDefaultHeight = 380;

		//
		if ((cx == 0) || (cy == 0))
		{
			CRect rect;
			GetClientRect(&rect);
			cx = rect.Width();
			cy = rect.Height();
		}
		//
		dx = cx - nDefaultWidth;
		dy = cy - nDefaultHeight;

		// space between suits in horiz grouped display
		if (dx >= 0)
			m_nSuitSpacer = SMALL_PLAYER_SPACER + (dx / 15);
		else
			m_nSuitSpacer = SMALL_PLAYER_SPACER + (dx / 4);
		//
		if (m_nSuitSpacer <= 0)
			m_nSuitSpacer = 1;

		// space between suits in N/S dummy display
		m_nDummySuitSpacer = SMALL_DUMMY_SPACER + (dx / 16);

		// offsets between E/W alternate face-up suits 
		if (dx > 0)
			m_nHorizSuitOffset = SMALL_HORIZ_SUIT_OFFSET + (dx / 24);
		else
			m_nHorizSuitOffset = SMALL_HORIZ_SUIT_OFFSET + (dx / 40);

		//
		int nSpaceUsed;
		// spacers = edges to left/right of E/W cards, plus gapes between S and E/W
		if (theApp.AreCardsFaceUp())
			nSpaceUsed = m_nCardWidth*6 + m_nSuitSpacer*3 + m_nHorizSuitOffset*2 + 2 + 2;
		else
			nSpaceUsed = m_nCardWidth*6 + m_nSuitSpacer*3 + 2 + 2;
		m_nCardXGap = (cx - nSpaceUsed) / 9;

		// gap between vertically laid out cards
		if (dy >= 0)
			m_nCardYGap = SMALL_Y_GAP + (dy / 12);
		else
			m_nCardYGap = SMALL_Y_GAP + (dy / 20);

		// overlap between suits on E/W dummy display
		m_nOverlapYOffset = SMALL_OVERLAP_Y_OFFSET + (dy / 3);
		m_bHorizDummySuitsOverlap = (m_nOverlapYOffset < m_nCardHeight);
	}
	else
	{
		// using normal sized cards
		nDefaultWidth = 632;
		nDefaultHeight = 418;
		//
		if ((cx == 0) || (cy == 0))
		{
			CRect rect;
			GetClientRect(&rect);
			cx = rect.Width();
			cy = rect.Height();
		}
		//
		dx = cx - nDefaultWidth;
		dy = cy - nDefaultHeight;

		// space between suits in horiz grouped display
		if (dx >= 0)
			m_nSuitSpacer = PLAYER_SPACER + (dx / 15);
		else
			m_nSuitSpacer = PLAYER_SPACER + (dx / 18);

		//
		if (m_nSuitSpacer <= 0)
			m_nSuitSpacer = 1;

		// space between suits in N/S dummy display
		m_nDummySuitSpacer = DUMMY_SPACER + (dx / 30);

		// offsets between E/W alternate face-up suits 
		if (dx > 0)
			m_nHorizSuitOffset = HORIZ_SUIT_OFFSET + (dx / 24);
		else
			m_nHorizSuitOffset = HORIZ_SUIT_OFFSET + (dx / 40);

		// gap between horizontally laid out cards
		int nSpaceUsed;
		int nSeparation = 4 + (dx / 20);
		if (theApp.AreCardsFaceUp())
			nSpaceUsed = m_nCardWidth*6 + m_nSuitSpacer*3 + m_nHorizSuitOffset*2 + 6 + nSeparation*2;
		else
			nSpaceUsed = m_nCardWidth*6 + m_nSuitSpacer*3 + 6 + nSeparation*2;
		m_nCardXGap = (cx - nSpaceUsed) / 9;

		// warn if the screen is too small
		if ((m_nCardXGap < 9) && theApp.GetValue(tbShowScreenSizeWarning))
		{
			CScreenSizeWarningDlg dlg;
			dlg.DoModal();
		}

		// gap between vertically laid out cards
		m_nCardYGap = STANDARD_Y_GAP + (dy / 15);

		// overlap between suits on E/W dummy display
		m_nOverlapYOffset = OVERLAP_Y_OFFSET + (dy / 3);
		m_bHorizDummySuitsOverlap = (m_nOverlapYOffset < m_nCardHeight);
	}
	
	//
	// adjust hand display positions
	//
	int nCardWidth = deck.GetCardWidth();
	int nCardHeight = deck.GetCardHeight();

	int nLeftOffset = (cx - m_nCardXGap*9 - m_nSuitSpacer*3 - nCardWidth*4) / 2;
	m_drawPoint[SOUTH].x = nLeftOffset;
	m_drawPoint[SOUTH].y = cy - nCardHeight - (bSmallCards? 2 : 3);
	int nNorthLeftOffset = (cx - m_nCardXGap*12 - nCardWidth) / 2;
	m_drawPoint[NORTH].x = nNorthLeftOffset;
	m_drawPoint[NORTH].y = bSmallCards? 1 : 2;
	//
	int nTopOffset = ((cy - m_nCardYGap*12 - nCardHeight) / 2) - 2;
	m_drawPoint[WEST].y = m_drawPoint[EAST].y = nTopOffset;
	m_drawPoint[WEST].x = bSmallCards? smallDrawPoint[WEST].x : defDrawPoint[WEST].x;
	m_drawPoint[EAST].x = cx - nCardWidth - (bSmallCards? 1 : 3);

	//
	// adjust dummy draw offsets
	//
	// position E/W dummy to to be at least x pixels below North's cards
	int nVertDummyOffset, nNorthBottom = m_drawPoint[NORTH].y + m_nCardHeight;
	if (nNorthBottom > m_drawPoint[WEST].y)
		nVertDummyOffset = (nNorthBottom - m_drawPoint[WEST].y) + (bSmallCards? 8 : 10);
	else
		nVertDummyOffset = Max(m_drawPoint[WEST].y - nNorthBottom, 12);
	m_dummyDrawOffset[WEST].y = m_dummyDrawOffset[EAST].y = nVertDummyOffset;
	m_dummyDrawOffset[WEST].x = m_dummyDrawOffset[EAST].x =0;
	//
	int nDummyWidth = m_nCardWidth*4 + m_nDummySuitSpacer*3;
	int nDummyXPos = (cx - nDummyWidth) / 2;
	m_dummyDrawOffset[SOUTH].x = nDummyXPos - m_drawPoint[SOUTH].x;
	m_dummyDrawOffset[NORTH].x = nDummyXPos - m_drawPoint[NORTH].x;
	m_dummyDrawOffset[NORTH].y = m_dummyDrawOffset[SOUTH].y = 0;

	// adjust layout table area
	if (bSmallCards)
	{
		m_nLayoutXGap = SMALL_LAYOUT_X_GAP + ((dx - m_nSuitSpacer*3) / 11);
		m_nLayoutYGap = SMALL_LAYOUT_Y_GAP + (dy / 5);
	}
	else
	{
		m_nLayoutXGap = LAYOUT_X_GAP + ((dx - m_nSuitSpacer*3) / 12);
		m_nLayoutYGap = LAYOUT_Y_GAP + (dy / 5);
	}
	int nWidth = 12*m_nLayoutXGap + m_nCardWidth;
	int nHeight = 3*m_nLayoutYGap + m_nCardHeight;
	m_rectLayout.left = (cx - nWidth) / 2;
	m_rectLayout.top = (cy - nHeight) / 2;
	m_rectLayout.right = m_rectLayout.left + nWidth;
	m_rectLayout.bottom = m_rectLayout.top + nHeight;

	// adjust table card destinations
	int nXOffset = dx / 2;
	int nYOffset = dy / 2;
	for(int i=0;i<4;i++)
	{
		if (bSmallCards)
		{
			m_cardDest[i].x = smallCardDest[i].x + nXOffset;
			m_cardDest[i].y = smallCardDest[i].y + nYOffset;
		}
		else
		{
			m_cardDest[i].x = defCardDest[i].x + nXOffset;
			m_cardDest[i].y = defCardDest[i].y + nYOffset;
		}
		//
		m_rectCardDest[i].left = m_cardDest[i].x;
		m_rectCardDest[i].top = m_cardDest[i].y;
		m_rectCardDest[i].right = m_cardDest[i].x + m_nCardWidth;
		m_rectCardDest[i].bottom = m_cardDest[i].y + m_nCardHeight;
	}

	// adjsts card play table area
	m_rectTable.left = m_cardDest[WEST].x;
	m_rectTable.top = m_cardDest[NORTH].y;
	m_rectTable.right = m_cardDest[EAST].x + m_nCardWidth;
	m_rectTable.bottom = m_cardDest[SOUTH].y + m_nCardHeight;

	// adjust any table cards
	// and any trick cards on the table
	if (theApp.IsGameInProgress())
	{
		int numCards = pDOC->GetNumCardsPlayedInRound();
		int nPos = pDOC->GetRoundLead();
		for(i=0;i<numCards;i++) 
		{
			CCard* pCard = pDOC->GetCurrentTrickCard(nPos);
			if (pCard)
				pCard->MoveTo(NULL, m_cardDest[nPos].x, m_cardDest[nPos].y, FALSE);
			nPos = GetNextPlayer(nPos);
		}
	}

	//
	// and tricks display
	//
	for(i=0;i<2;i++)
	{
		if (bSmallCards)
		{
			m_ptTricksDisplayDest[i].x = smallTricksDisplayDest[i].x + dx;
			m_ptTricksDisplayDest[i].y = smallTricksDisplayDest[i].y + dy;
		}
		else
		{
			m_ptTricksDisplayDest[i].x = tricksDisplayDest[i].x + dx;
			m_ptTricksDisplayDest[i].y = tricksDisplayDest[i].y + dy;
		}
	}

	//
	// create table region
	//
	m_tableRegion.DeleteObject();
	CRgn region1, region2;
	region1.CreateRectRgn(m_cardDest[NORTH].x,m_cardDest[NORTH].y,
						  m_cardDest[NORTH].x+m_nCardWidth,
						  m_cardDest[SOUTH].y+m_nCardHeight);
	region2.CreateRectRgn(m_cardDest[WEST].x,m_cardDest[WEST].y,
						  m_cardDest[EAST].x+m_nCardWidth,
						  m_cardDest[WEST].y+m_nCardHeight);
	m_tableRegion.CreateRectRgn(0,0,0,0);
	(void)m_tableRegion.CombineRgn(&region1,&region2,RGN_OR);
	region1.DeleteObject();
	region2.DeleteObject();
}





