//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//
// Subclass.cpp
//
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EasyB.h"
#include "mainfrm.h"


WNDPROC wpOrigButtonProc;
WNDPROC wpOrigFlatButtonProc;
WNDPROC wpOrigEditProc;
WNDPROC wpOrigEditROProc;
WNDPROC wpOrigRichEditProc;
WNDPROC wpOrigRadioProc;
WNDPROC wpOrigCheckBoxProc;
WNDPROC wpOrigListBoxProc;
WNDPROC wpOrigListCtrlProc;


//
//--------------------------------------------------------------------------
// routine used to subclass button controls
//
LRESULT APIENTRY ButtonSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hWndParent;
//	short nState;

	switch(uMsg) 
	{

		case WM_KEYDOWN:
			// send message to parent
			hWndParent = ::GetParent(hwnd);
			PostMessage(hWndParent, uMsg, wParam, lParam);
			return TRUE;
	}
    return CallWindowProc(wpOrigButtonProc, hwnd, uMsg, wParam, lParam);
}






//
//--------------------------------------------------------------------------
// routine used to subclass button controls to give a flat appearance
//
static bool bColorsInitialized = FALSE;  // NCR added bool
static COLORREF clrHilite;
static COLORREF m_clrShadow;
static COLORREF m_clrButton;
//
LRESULT APIENTRY FlatButtonSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hWndParent;

	switch(uMsg) 
	{
		case WM_CREATE:
/*
			// init colors
			if (!bColorsInitialized)
			clrHilite  = GetSysColor(COLOR_BTNHIGHLIGHT);
			clrShadow  = GetSysColor(COLOR_BTNSHADOW);
			clrButton  = GetSysColor(COLOR_BTNFACE);
*/
			break;

		case WM_KEYDOWN:
			// send message to parent
			hWndParent = ::GetParent(hwnd);
			PostMessage(hWndParent, uMsg, wParam, lParam);
			return TRUE;

		case WM_MOUSEMOVE:
//			SetTimer(m_hWnd, 1, 10, NULL);
			break;

		case WM_TIMER:
			break;
//			return TRUE;

	}
    return CallWindowProc(wpOrigButtonProc, hwnd, uMsg, wParam, lParam);
}





//
//--------------------------------------------------------------------------
// routine used to subclass edit controls
//
LRESULT APIENTRY EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hWndParent;
//	short nState;

	switch(uMsg) 
	{

		case WM_KEYDOWN:
			// send any non-cursor key to parent
			if ((wParam == VK_DOWN) || (wParam == VK_UP) ||
			    (wParam == VK_LEFT) || (wParam == VK_RIGHT) ||
				(wParam == VK_PRIOR) || (wParam == VK_NEXT) ||
			    (wParam == VK_HOME) || (wParam == VK_END) ||
				(wParam == VK_RETURN))
				break;
			hWndParent = ::GetParent(hwnd);
			PostMessage(hWndParent, uMsg, wParam, lParam);
			return TRUE;
/*			
			if (wParam == VK_TAB)  
			{
				hWndParent = ::GetParent(hwnd);
//				::PostMessage(hWndParent, WM_COMMAND, WMS_KEYSTROKE, (LONG)wParam);
				nState = GetKeyState(VK_SHIFT);
				if (nState & 0x8000)
					::PostMessage(hWndParent, WM_COMMAND, WMS_KEYSTROKE, (LONG) VK_BACK);
				else
					::PostMessage(hWndParent, WM_COMMAND, WMS_KEYSTROKE, (LONG)wParam);
				return TRUE;
			} 
			else if ((wParam == VK_LEFT) || (wParam == VK_RIGHT) ||
					   (wParam == VK_HOME) || (wParam == VK_END)) 
			{
				nState = GetKeyState(VK_CONTROL);
				if (nState & 0x8000) 
				{
					hWndParent = ::GetParent(hwnd);
					::PostMessage(hWndParent, WM_COMMAND, WMS_KEYSTROKE, (LONG)wParam);
					return TRUE;
				}
				break;
			} 
			else if ((wParam == VK_DOWN) || (wParam == VK_UP) ||
					   (wParam == VK_PRIOR) || (wParam == VK_NEXT) ||
					   (wParam == VK_LEFT) || (wParam == VK_RIGHT) ||
					   (wParam == VK_RETURN)) 
			{
				hWndParent = ::GetParent(hwnd);
				::PostMessage(hWndParent, WM_COMMAND, WMS_KEYSTROKE, (LONG)wParam);
				return TRUE;
			}
*/

/*
		case WM_SETFOCUS:
			::PostMessage(hwnd, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
			hWndParent = ::GetParent(hwnd);
			::SendMessage(hWndParent, 
						  WM_COMMAND, 
						  MAKELONG(::GetDlgCtrlID(hwnd),WMS_SETFOCUS),
						  (LONG)hwnd);
			break;
			
		case WM_KILLFOCUS:
			hWndParent = ::GetParent(hwnd);
			::SendMessage(hWndParent, WM_COMMAND, WMS_RESETFOCUS, 0L);
			break;

		case WM_RBUTTONDOWN:
			hWndParent = ::GetParent(hwnd);
			::SendMessage(hWndParent, uMsg, wParam, lParam);
			return TRUE;

*/
	}
    return CallWindowProc(wpOrigEditProc, hwnd, uMsg, wParam, lParam);
}





//
//--------------------------------------------------------------------------
// a hack used to force read-only status on edit controls
// (why not just set the read-only bit? cuz that makes the window 
// ugly and gray)
//
LRESULT APIENTRY EditROSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if ((uMsg == WM_KEYDOWN) || (uMsg == WM_CHAR))
	{
		// ignore keystrokes
//		HWND hWndParent = ::GetParent(hwnd);
//		PostMessage(hWndParent, uMsg, wParam, lParam);
		return TRUE;
	}
    return CallWindowProc(wpOrigEditROProc, hwnd, uMsg, wParam, lParam);
}





//
//--------------------------------------------------------------------------
// routine used to subclass rich text format edit controls
//
LRESULT APIENTRY RichEditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hWndParent;
//	short nState;

	switch(uMsg) 
	{

		case WM_KEYDOWN:
			pMAINFRAME->PostMessage(uMsg, wParam, lParam);
			return TRUE;
//			break;
/*
			// send any non-cursor key to parent
			if ((wParam == VK_DOWN) || (wParam == VK_UP) ||
			    (wParam == VK_LEFT) || (wParam == VK_RIGHT) ||
				(wParam == VK_PRIOR) || (wParam == VK_NEXT) ||
			    (wParam == VK_HOME) || (wParam == VK_END) ||
				(wParam == VK_RETURN))
				break;
			hWndParent = ::GetParent(hwnd);
			PostMessage(hWndParent, uMsg, wParam, lParam);
			return TRUE;
*/

		case WM_RBUTTONDOWN:
			// allow parent to handle popup menus and such
			hWndParent = ::GetParent(hwnd);
			// convert client coordinates to parent coordinates
			POINT point;
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);
			::ClientToScreen(hwnd, &point);
			::ScreenToClient(hWndParent, &point);
			lParam = MAKELONG(point.x, point.y);
			// and send
			PostMessage(hWndParent, uMsg, wParam, lParam);
			return TRUE;
/*			
			if (wParam == VK_TAB)  
			{
				hWndParent = ::GetParent(hwnd);
//				::PostMessage(hWndParent, WM_COMMAND, WMS_KEYSTROKE, (LONG)wParam);
				nState = GetKeyState(VK_SHIFT);
				if (nState & 0x8000)
					::PostMessage(hWndParent, WM_COMMAND, WMS_KEYSTROKE, (LONG) VK_BACK);
				else
					::PostMessage(hWndParent, WM_COMMAND, WMS_KEYSTROKE, (LONG)wParam);
				return TRUE;
			} 
			else if ((wParam == VK_LEFT) || (wParam == VK_RIGHT) ||
					   (wParam == VK_HOME) || (wParam == VK_END)) 
			{
				nState = GetKeyState(VK_CONTROL);
				if (nState & 0x8000) 
				{
					hWndParent = ::GetParent(hwnd);
					::PostMessage(hWndParent, WM_COMMAND, WMS_KEYSTROKE, (LONG)wParam);
					return TRUE;
				}
				break;
			} 
			else if ((wParam == VK_DOWN) || (wParam == VK_UP) ||
					   (wParam == VK_PRIOR) || (wParam == VK_NEXT) ||
					   (wParam == VK_LEFT) || (wParam == VK_RIGHT) ||
					   (wParam == VK_RETURN)) 
			{
				hWndParent = ::GetParent(hwnd);
				::PostMessage(hWndParent, WM_COMMAND, WMS_KEYSTROKE, (LONG)wParam);
				return TRUE;
			}
*/

/*
		case WM_SETFOCUS:
			::PostMessage(hwnd, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
			hWndParent = ::GetParent(hwnd);
			::SendMessage(hWndParent, 
						  WM_COMMAND, 
						  MAKELONG(::GetDlgCtrlID(hwnd),WMS_SETFOCUS),
						  (LONG)hwnd);
			break;
			
		case WM_KILLFOCUS:
			hWndParent = ::GetParent(hwnd);
			::SendMessage(hWndParent, WM_COMMAND, WMS_RESETFOCUS, 0L);
			break;

		case WM_RBUTTONDOWN:
			hWndParent = ::GetParent(hwnd);
			::SendMessage(hWndParent, uMsg, wParam, lParam);
			return TRUE;

*/
	}
    return CallWindowProc(wpOrigRichEditProc, hwnd, uMsg, wParam, lParam);
}




//
//--------------------------------------------------------------------------
//
// routine used to subclass radio controls
//
LRESULT APIENTRY RadioSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hWndParent;

	switch(uMsg) 
	{

		case WM_KEYDOWN:
			// send any non-cursor key to parent
			if ((wParam == VK_DOWN) || (wParam == VK_UP) ||
			    (wParam == VK_LEFT) || (wParam == VK_RIGHT) ||
				(wParam == VK_PRIOR) || (wParam == VK_NEXT) ||
			    (wParam == VK_HOME) || (wParam == VK_END) ||
				(wParam == VK_RETURN))
				break;
			hWndParent = ::GetParent(hwnd);
			PostMessage(hWndParent, uMsg, wParam, lParam);
			return TRUE;

	}
    return CallWindowProc(wpOrigRadioProc, hwnd, uMsg, wParam, lParam);
}




//
//--------------------------------------------------------------------------
//
// routine used to subclass check box controls
//
LRESULT APIENTRY CheckBoxSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hWndParent;

	switch(uMsg) 
	{

		case WM_KEYDOWN:
			// send any non-cursor key to parent
			if ((wParam == VK_DOWN) || (wParam == VK_UP) ||
			    (wParam == VK_LEFT) || (wParam == VK_RIGHT) ||
				(wParam == VK_PRIOR) || (wParam == VK_NEXT) ||
			    (wParam == VK_HOME) || (wParam == VK_END) ||
				(wParam == VK_RETURN))
				break;
			hWndParent = ::GetParent(hwnd);
			PostMessage(hWndParent, uMsg, wParam, lParam);
			return TRUE;

	}
    return CallWindowProc(wpOrigCheckBoxProc, hwnd, uMsg, wParam, lParam);
}





//
//--------------------------------------------------------------------------
//
// routine used to subclass list boxes
//
LRESULT APIENTRY ListBoxSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) 
	{

		case WM_KEYDOWN:
			pMAINFRAME->PostMessage(uMsg, wParam, lParam);
			break;
/*
			// send any non-cursor key to parent
			if ((wParam == VK_DOWN) || (wParam == VK_UP) ||
			    (wParam == VK_LEFT) || (wParam == VK_RIGHT) ||
				(wParam == VK_PRIOR) || (wParam == VK_NEXT) ||
			    (wParam == VK_HOME) || (wParam == VK_END) ||
				(wParam == VK_RETURN))
				break;
			hWndParent = ::GetParent(hwnd);
			PostMessage(hWndParent, uMsg, wParam, lParam);
			return TRUE;
*/
	}
    return CallWindowProc(wpOrigListBoxProc, hwnd, uMsg, wParam, lParam);
}







//
//--------------------------------------------------------------------------
//
// routine used to subclass list view controls
//
LRESULT APIENTRY ListCtrlSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hWndParent;

	switch(uMsg) 
	{

		case WM_KEYDOWN:
			pMAINFRAME->PostMessage(uMsg, wParam, lParam);
			break;
/*
			// send any non-cursor key to parent
			if ((wParam == VK_DOWN) || (wParam == VK_UP) ||
			    (wParam == VK_LEFT) || (wParam == VK_RIGHT) ||
				(wParam == VK_PRIOR) || (wParam == VK_NEXT) ||
			    (wParam == VK_HOME) || (wParam == VK_END) ||
				(wParam == VK_RETURN))
				break;
			hWndParent = ::GetParent(hwnd);
			PostMessage(hWndParent, uMsg, wParam, lParam);
			return TRUE;
*/

		case WM_MOUSEMOVE:
			// notrify the parent to update tooltips
			hWndParent = ::GetParent(hwnd);
			PostMessage(hWndParent, WM_COMMAND, WMS_UPDATE_TOOLTIP_TEXT, lParam);
			break;

		case WM_VSCROLL:
			// notrify the parent to update tooltip state
			hWndParent = ::GetParent(hwnd);
			PostMessage(hWndParent, uMsg, wParam, lParam);
			break;

	}
    return CallWindowProc(wpOrigListCtrlProc, hwnd, uMsg, wParam, lParam);
}


