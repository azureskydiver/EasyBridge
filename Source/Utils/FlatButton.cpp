//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// FlatButton.cpp : implementation file
//

#include "stdafx.h"
#include "FlatButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlatButton

CFlatButton::CFlatButton()
{
  m_MouseOnButton = FALSE;

  m_hIconIn = NULL;
  m_hIconOut = NULL;
  m_cxIcon = 0;
  m_cyIcon = 0;
  m_bIsFlat = TRUE; // Default type is 'flat' button
  m_bDrawBorder = TRUE; // By default draw border in 'flat' button 
  m_nAlign = ST_ALIGN_HORIZ; // By default icon is aligned horizontally
  m_bShowText = TRUE; // By default show the text button
	
  SetDefaultInactiveBgColor();
  SetDefaultInactiveFgColor();
  SetDefaultActiveBgColor();
  SetDefaultActiveFgColor();
} // End of CFlatButton


BEGIN_MESSAGE_MAP(CFlatButton, CButton)
    //{{AFX_MSG_MAP(CFlatButton)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
  ON_WM_MOUSEMOVE()
  ON_WM_TIMER()
  ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////


//
void CFlatButton::SetIcon(int nIconInId, int nIconOutId, BYTE cx, BYTE cy)
{
  // Set icon when the mouse is IN the button
  m_hIconIn = (HICON)::LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(nIconInId), IMAGE_ICON, 0, 0, 0);
  
  // Set icon when the mouse is OUT the button
  m_hIconOut = (nIconOutId == NULL) ? m_hIconIn : (HICON)::LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(nIconOutId), IMAGE_ICON, 0, 0, 0);
  
  // if the icon size was not specified, try to determine it
  if ((cx == 0) || (cy == 0))
  {
	  /// get information about the icon
	  ICONINFO iconInfo;
	  GetIconInfo(m_hIconIn, &iconInfo);
	  BITMAP bitmapInfo;
	  // get information about the icon's mask bitmap
	  ::GetObject(iconInfo.hbmMask, sizeof(bitmapInfo), &bitmapInfo);
	  m_cxIcon = (UCHAR) bitmapInfo.bmWidth;
	  // height is 1/2 of the mask bitmap for monochrome icons
	  if (iconInfo.hbmColor == NULL)
		  m_cyIcon = (UCHAR) bitmapInfo.bmHeight / 2;
	  else
		  m_cyIcon = (UCHAR) bitmapInfo.bmHeight;
	  // clean up
	  ::DeleteObject(iconInfo.hbmColor);
	  ::DeleteObject(iconInfo.hbmMask);
  }
  else
  {
	  m_cxIcon = cx;
	  m_cyIcon = cy;
  }
} // End of SetIcon


void CFlatButton::SetFlat(BOOL bState)
{
  m_bIsFlat = bState;
  Invalidate();
} // End of SetFlat


BOOL CFlatButton::GetFlat()
{
  return m_bIsFlat;
} // End of GetFlat


void CFlatButton::SetAlign(int nAlign)
{
  switch (nAlign)
  {    
    case ST_ALIGN_HORIZ:
         m_nAlign = ST_ALIGN_HORIZ;
         break;
    case ST_ALIGN_VERT:
         m_nAlign = ST_ALIGN_VERT;
         break;
  }
  Invalidate();
} // End of SetAlign


int CFlatButton::GetAlign()
{
  return m_nAlign;
} // End of GetAlign


void CFlatButton::DrawBorder(BOOL bEnable)
{
  m_bDrawBorder = bEnable;
} // End of DrawBorder


const char* CFlatButton::GetVersionC()
{
  return "2.2";
} // End of GetVersionC


const short CFlatButton::GetVersionI()
{
  return 22; // Divide by 10 to get actual version
} // End of GetVersionI


void CFlatButton::SetShowText(BOOL bShow)
{
  m_bShowText = bShow;
  Invalidate();
} // End of SetShowText


BOOL CFlatButton::GetShowText()
{
  return m_bShowText;
} // End of GetShowText


void CFlatButton::OnTimer(UINT nIDEvent) 
{
	CRect rcItem;
	GetWindowRect(rcItem);

	CPoint ptCursor;
	GetCursorPos(&ptCursor);

	if (!rcItem.PtInRect(ptCursor))
	{
		KillTimer(1);
		m_MouseOnButton = FALSE;
		InvalidateRect (NULL);
		ReleaseCapture();
		return;
	}
	//
	CButton::OnTimer(nIDEvent);
}

void CFlatButton::OnMouseMove(UINT nFlags, CPoint point)
{
  CWnd* pWnd;  // Finestra attiva
  CWnd* pParent; // Finestra che contiene il bottone

  CButton::OnMouseMove(nFlags, point);

  // If the mouse enter the button with the left button pressed
  // then do nothing
  if (nFlags & MK_LBUTTON && m_MouseOnButton == FALSE) return;

  // If our button is not flat then do nothing
  if (m_bIsFlat == FALSE) return;

  pWnd = GetActiveWindow();
  pParent = GetOwner();

  if ((GetCapture() != this) &&
//      ((pWnd != NULL) && (pWnd->m_hWnd == pParent->m_hWnd))) 
      ((pWnd != NULL) && (pParent != NULL))) 
  {
    m_MouseOnButton = TRUE;
//    SetFocus();	// Thanks Ralph!
    SetCapture();
    Invalidate();
  }
  else
  {
    CRect rc;
    GetClientRect(&rc);
    if (!rc.PtInRect(point))
    {
      // Redraw only if mouse goes out
      if (m_MouseOnButton == TRUE)
      {
        m_MouseOnButton = FALSE;
        Invalidate();
      }
      // If user is NOT pressing left button then release capture!
      if (!(nFlags & MK_LBUTTON)) ReleaseCapture();
    }
  }
  SetTimer(1, 10, NULL);
} // End of OnMouseMove


void CFlatButton::OnKillFocus(CWnd * pNewWnd)
{
  CButton::OnKillFocus(pNewWnd);

  // If our button is not flat then do nothing
  if (m_bIsFlat == FALSE) return;

  if (m_MouseOnButton == TRUE)
  {
    m_MouseOnButton = FALSE;
    Invalidate();
  }
} // End of OnKillFocus


void CFlatButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
#ifdef ST_USE_MEMDC
  CDC  *pdrawDC = CDC::FromHandle(lpDIS->hDC);
  CMemDC memDC(pdrawDC);
  CDC  *pDC = &memDC;
#else	
  CDC* pDC = CDC::FromHandle(lpDIS->hDC);
#endif

  CPen *pOldPen;
  BOOL bIsPressed  = (lpDIS->itemState & ODS_SELECTED);
  BOOL bIsFocused  = (lpDIS->itemState & ODS_FOCUS);
  BOOL bIsDisabled = (lpDIS->itemState & ODS_DISABLED);

  CRect itemRect = lpDIS->rcItem;

  if (m_bIsFlat == FALSE)
  {
    if (bIsFocused)
    {
      CBrush br(RGB(0,0,0));  
      pDC->FrameRect(&itemRect, &br);
      itemRect.DeflateRect(1, 1);
    }
  }

  // Prepare draw... paint button's area with background color
  COLORREF bgColor;
  if ( ((m_MouseOnButton == TRUE) && !bIsDisabled) || bIsPressed)
    bgColor = GetActiveBgColor();
  else
    bgColor = GetInactiveBgColor();

  CBrush br(bgColor);
  pDC->FillRect(&itemRect, &br);

	// Disegno lo sfondo del bottone
//CBrush br(GetSysColor(COLOR_BTNFACE));  
//pDC->FillRect(&itemRect, &br);

  // Draw pressed button
  if (bIsPressed)
  {
    if (m_bIsFlat == TRUE)
    {
      if (m_bDrawBorder == TRUE)
      {
	    CPen penBtnHiLight(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT)); // Bianco
        CPen penBtnShadow(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));   // Grigio scuro

        // Disegno i bordi a sinistra e in alto
        // Dark gray line
        pOldPen = pDC->SelectObject(&penBtnShadow);
        pDC->MoveTo(itemRect.left, itemRect.bottom-1);
        pDC->LineTo(itemRect.left, itemRect.top);
        pDC->LineTo(itemRect.right, itemRect.top);
        // Disegno i bordi a destra e in basso
        // White line
        pDC->SelectObject(penBtnHiLight);
        pDC->MoveTo(itemRect.left, itemRect.bottom-1);
        pDC->LineTo(itemRect.right-1, itemRect.bottom-1);
        pDC->LineTo(itemRect.right-1, itemRect.top-1);
        //
        pDC->SelectObject(pOldPen);
      }
    }
    else    
    {
      CBrush brBtnShadow(GetSysColor(COLOR_BTNSHADOW));
      pDC->FrameRect(&itemRect, &brBtnShadow);
    }
  }
  else // ...else draw non pressed button
  {
    CPen penBtnHiLight(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT)); // White
    CPen pen3DLight(PS_SOLID, 0, GetSysColor(COLOR_3DLIGHT));       // Light gray
    CPen penBtnShadow(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));   // Dark gray
    CPen pen3DDKShadow(PS_SOLID, 0, GetSysColor(COLOR_3DDKSHADOW)); // Black

    if (m_bIsFlat == TRUE)
    {
      if (m_MouseOnButton == TRUE && !bIsDisabled && m_bDrawBorder == TRUE)
      {
  	    // Disegno i bordi a sinistra e in alto
        // White line
        pOldPen = pDC->SelectObject(&penBtnHiLight);
        pDC->MoveTo(itemRect.left, itemRect.bottom-1);
        pDC->LineTo(itemRect.left, itemRect.top);
        pDC->LineTo(itemRect.right, itemRect.top);
        // Disegno i bordi a destra e in basso
        // Dark gray line
        pDC->SelectObject(penBtnShadow);
        pDC->MoveTo(itemRect.left, itemRect.bottom-1);
        pDC->LineTo(itemRect.right-1, itemRect.bottom-1);
        pDC->LineTo(itemRect.right-1, itemRect.top-1);
        //
        pDC->SelectObject(pOldPen);
      }
    }
    else
    {
      // Disegno i bordi a sinistra e in alto
      // White line
      pOldPen = pDC->SelectObject(&penBtnHiLight);
      pDC->MoveTo(itemRect.left, itemRect.bottom-1);
      pDC->LineTo(itemRect.left, itemRect.top);
      pDC->LineTo(itemRect.right, itemRect.top);
      // Light gray line
      pDC->SelectObject(pen3DLight);
      pDC->MoveTo(itemRect.left+1, itemRect.bottom-1);
      pDC->LineTo(itemRect.left+1, itemRect.top+1);
      pDC->LineTo(itemRect.right, itemRect.top+1);
      // Disegno i bordi a destra e in basso
      // Black line
      pDC->SelectObject(pen3DDKShadow);
      pDC->MoveTo(itemRect.left, itemRect.bottom-1);
      pDC->LineTo(itemRect.right-1, itemRect.bottom-1);
      pDC->LineTo(itemRect.right-1, itemRect.top-1);
      // Dark gray line
      pDC->SelectObject(penBtnShadow);
      pDC->MoveTo(itemRect.left+1, itemRect.bottom-2);
      pDC->LineTo(itemRect.right-2, itemRect.bottom-2);
      pDC->LineTo(itemRect.right-2, itemRect.top);
      //
      pDC->SelectObject(pOldPen);
    }
  }

  if (m_bIsFlat == FALSE)
  {
    // Draw the focus rect
    if (bIsFocused)
    {
      CRect focusRect = itemRect;
      focusRect.DeflateRect(3, 3);
      pDC->DrawFocusRect(&focusRect);
    }
  }

  // Read the button title
  CString sTitle;
  GetWindowText(sTitle);

  // If we don't want the title displayed
  if (m_bShowText == FALSE) sTitle.Empty();

  CRect captionRect = lpDIS->rcItem;

  // Draw the icon
  if (m_hIconIn != NULL)
  {
    DrawTheIcon(pDC, &sTitle, &lpDIS->rcItem, &captionRect, bIsPressed, bIsDisabled);
  }

  // Write the button title (if any)
  if (sTitle.IsEmpty() == FALSE)
  {
    // Disegno la caption del bottone
    // Se il bottone e' premuto muovo la captionRect di conseguenza
    if (bIsPressed)
      captionRect.OffsetRect(1, 1);
    
    // ONLY FOR DEBUG 
    // Evidenzia il rettangolo in cui verra' centrata la caption 
    //CBrush brBtnShadow(RGB(255, 0, 0));
    //pDC->FrameRect(&captionRect, &brBtnShadow);

#ifdef ST_USE_MEMDC
	// Get dialog's font
    CFont *pCurrentFont = GetFont(); 
    CFont *pOldFont = pDC->SelectObject(pCurrentFont);
#endif
    if ( ((m_MouseOnButton == TRUE) && !bIsDisabled) || (bIsPressed)) 
	{
      pDC->SetTextColor(GetActiveFgColor());
      pDC->SetBkColor(GetActiveBgColor());
    } 
	else 
	{
      pDC->SetTextColor(GetInactiveFgColor());
      pDC->SetBkColor(GetInactiveBgColor());
    }
    // Center text
    CRect centerRect = captionRect;
    pDC->DrawText(sTitle, -1, captionRect, DT_SINGLELINE|DT_CALCRECT);
    captionRect.OffsetRect((centerRect.Width() - captionRect.Width())/2, (centerRect.Height() - captionRect.Height())/2);

    pDC->DrawState(captionRect.TopLeft(), captionRect.Size(), (LPCTSTR)sTitle, (bIsDisabled ? DSS_DISABLED : DSS_NORMAL), 
                   TRUE, 0, (CBrush*)NULL);
#ifdef ST_USE_MEMDC
    pDC->SelectObject(pOldFont);
#endif
  }
} // End of DrawItem


void CFlatButton::DrawTheIcon(CDC* pDC, CString* title, RECT* rcItem, CRect* captionRect, BOOL IsPressed, BOOL IsDisabled)
{
  CRect iconRect = rcItem;

  switch (m_nAlign)
  {
    case ST_ALIGN_HORIZ:
         if (title->IsEmpty())
         {
           // Center the icon horizontally
           iconRect.left += ((iconRect.Width() - m_cxIcon)/2);
         }
         else
         {
           // L'icona deve vedersi subito dentro il focus rect
           iconRect.left += 3;  
           captionRect->left += m_cxIcon + 3;
         }
         // Center the icon vertically
         iconRect.top += ((iconRect.Height() - m_cyIcon)/2);
         break;
    case ST_ALIGN_VERT:
         // Center the icon horizontally
         iconRect.left += ((iconRect.Width() - m_cxIcon)/2);
         if (title->IsEmpty())
         {
           // Center the icon vertically
           iconRect.top += ((iconRect.Height() - m_cyIcon)/2);           
         }
         else
         {
           captionRect->top += m_cyIcon;
         }
         break;
  }
    
  // If button is pressed then press the icon also
  if (IsPressed) iconRect.OffsetRect(1, 1);
  // Ole'!
  pDC->DrawState(iconRect.TopLeft(), 
	               iconRect.Size(), 
				         (m_MouseOnButton == TRUE || IsPressed) ? m_hIconIn : m_hIconOut, 
				         (IsDisabled ? DSS_DISABLED : DSS_NORMAL), 
                 (CBrush*)NULL);
} // End of DrawTheIcon


void CFlatButton::PreSubclassWindow() 
{
  // Add BS_OWNERDRAW style
  SetButtonStyle(GetButtonStyle() | BS_OWNERDRAW);
  CButton::PreSubclassWindow();
} // End of PreSubclassWindow


void CFlatButton::SetDefaultInactiveBgColor(BOOL bRepaint)
{
	m_crInactiveBg = ::GetSysColor(COLOR_BTNFACE); 
	if (bRepaint == TRUE) Invalidate();
} // End of SetDefaultInactiveBgColor


void CFlatButton::SetInactiveBgColor(COLORREF crNew, BOOL bRepaint)
{
	m_crInactiveBg = crNew; 
	if (bRepaint == TRUE) Invalidate();
} // End of SetInactiveBgColor


const COLORREF CFlatButton::GetInactiveBgColor()
{
	return m_crInactiveBg;
} // End of GetInactiveBgColor


void CFlatButton::SetDefaultInactiveFgColor(BOOL bRepaint)
{
	m_crInactiveFg = ::GetSysColor(COLOR_BTNTEXT); 
	if (bRepaint == TRUE) Invalidate();
} // End of SetDefaultInactiveFgColor


void CFlatButton::SetInactiveFgColor(COLORREF crNew, BOOL bRepaint)
{
	m_crInactiveFg = crNew; 
	if (bRepaint == TRUE) Invalidate();
} // End of SetInactiveFgColor


const COLORREF CFlatButton::GetInactiveFgColor()
{
	return m_crInactiveFg;
} // End of GetInactiveFgColor


void CFlatButton::SetDefaultActiveBgColor(BOOL bRepaint)
{
	m_crActiveBg = ::GetSysColor(COLOR_BTNFACE); 
	if (bRepaint == TRUE) Invalidate();
} // End of SetDefaultActiveBgColor


void CFlatButton::SetActiveBgColor(COLORREF crNew, BOOL bRepaint)
{
	m_crActiveBg = crNew; 
	if (bRepaint == TRUE) Invalidate();
} // End of SetActiveBgColor


const COLORREF CFlatButton::GetActiveBgColor()
{
	return m_crActiveBg;
} // End of GetActiveBgColor


void CFlatButton::SetDefaultActiveFgColor(BOOL bRepaint)
{
	m_crActiveFg = ::GetSysColor(COLOR_BTNTEXT); 
	if (bRepaint == TRUE) Invalidate();
} // End of SetDefaultActiveFgColor


void CFlatButton::SetActiveFgColor(COLORREF crNew, BOOL bRepaint)
{
	m_crActiveFg = crNew; 
	if (bRepaint == TRUE) Invalidate();
} // End of SetActiveFgColor


const COLORREF CFlatButton::GetActiveFgColor()
{
	return m_crActiveFg;
} // End of GetActiveFgColor


// SSH - custom behavior
void CFlatButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    m_MouseOnButton = FALSE;
    ReleaseCapture();
	GetParent()->PostMessage(WM_KEYDOWN, nChar, MAKELONG(nRepCnt, nFlags));
	//	
//	CButton::OnKeyDown(nChar, nRepCnt, nFlags);
}
