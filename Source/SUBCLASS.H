//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
//
// Subclass.h
//
////////////////////////////////////////////////////////////////////////////

extern WNDPROC wpOrigButtonProc;
extern WNDPROC wpOrigFlatButtonProc;
extern WNDPROC wpOrigEditProc;
extern WNDPROC wpOrigEditROProc;
extern WNDPROC wpOrigRichEditProc;
extern WNDPROC wpOrigRadioProc;
extern WNDPROC wpOrigCheckBoxProc;
extern WNDPROC wpOrigListBoxProc;
extern WNDPROC wpOrigListCtrlProc;
//
extern LRESULT APIENTRY ButtonSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern LRESULT APIENTRY FlatButtonSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern LRESULT APIENTRY EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern LRESULT APIENTRY EditROSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern LRESULT APIENTRY RichEditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern LRESULT APIENTRY RadioSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern LRESULT APIENTRY CheckBoxSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern LRESULT APIENTRY ListBoxSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern LRESULT APIENTRY ListCtrlSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
