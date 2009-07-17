//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// MyCustomDialog.cpp : implementation file
//
// - a value-added dialog base class
//

#include "stdafx.h"
#include "EasyB.h"
#include "MyCustomDialog.h"
#include "MyToolTipWnd.h"
#include "globals.h"
#include "subclass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMyCustomDialog dialog


CMyCustomDialog::CMyCustomDialog(const int nID, const DialogControlInfo* pControlInfo, const int numControls, const int numButtons, CWnd* pParent)
	: CDialog(nID, pParent)
{
	//{{AFX_DATA_INIT(CMyCustomDialog)
	//}}AFX_DATA_INIT

	// init
	m_bInitialized = FALSE;
	m_nPrevTooltipIndex = -1;
	m_pListControl = NULL;

	// copy controls info
	if (pControlInfo)
	{
		m_numControls = numControls;
		m_numButtons = numButtons;
		m_pControlInfo = new DialogControlInfo[m_numControls];
		for(int i=0;i<m_numControls;i++)
			*(m_pControlInfo+i) = *(pControlInfo+i);
	}
	else
	{
		m_numControls = 0;
		m_numButtons = 0;
		m_pControlInfo = NULL;
	}
}


void CMyCustomDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyCustomDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMyCustomDialog, CDialog)
	//{{AFX_MSG_MAP(CMyCustomDialog)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_MOVE()
	ON_WM_MOUSEMOVE()
	ON_WM_VSCROLL()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
//
// CMyCustomDialog message handlers
//
/////////////////////////////////////////////////////////////////////////////




//
int CMyCustomDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// load button bitmaps
	for(int i=0;i<m_numButtons;i++)
	{
		if (m_pControlInfo[i].nIconID != 0)
		{	
			// load icon
			HICON* phIcon = new HICON;
			*phIcon = (HICON) LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(m_pControlInfo[i].nIconID),
										IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
			m_buttonIcons.SetAtGrow(i, phIcon);
		}
		else
		{
			m_buttonIcons.SetAtGrow(i, NULL);
		}

	}
	//
	return 0;
}


//
void CMyCustomDialog::OnDestroy() 
{
	CDialog::OnDestroy();

	// delete the tooltip
	delete m_pToolTip;

	// delete the icons
	for(int i=0;i<m_numButtons;i++)
	{
		// delete bitmaps
		HICON* phIcon = m_buttonIcons[i];
		if (phIcon)
		{
			// this seems unnecessary for some reason
			// indeed, it causes a crash if I uncomment it
			// so just how do icons get released???
//			CloseHandle(*phIcon);
			delete phIcon;
		}
	}

	// clear the lists
	m_buttonIcons.RemoveAll();

	// and finally delete the control info structure
	delete[] m_pControlInfo;
}




//
BOOL CMyCustomDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// enable standard tooltips
	EnableToolTips(TRUE);

	// assign bitmaps
	CButton* pButton;
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i=0;i<m_numButtons;i++)
	{
		// set icon
		if (m_pControlInfo[i].nIconID != 0)
		{
			pButton = (CButton*) GetDlgItem(m_pControlInfo[i].nControlID);
			pButton->ModifyStyle(NULL, BS_ICON);
		}

		// subclass button
		wpOrigButtonProc = (WNDPROC) SetWindowLong(GetDlgItem(m_pControlInfo[i].nControlID)->m_hWnd, GWL_WNDPROC, (LONG) ButtonSubclassProc);
	}

	// subclass listbox
	if (m_pListControl)
		wpOrigListCtrlProc = (WNDPROC) SetWindowLong(m_pListControl->m_hWnd, GWL_WNDPROC, (LONG) ListCtrlSubclassProc);

	// populate the control ID -> index map
	for(i=0;i<m_numControls;i++)
		m_mapIDtoIndex.SetAt(m_pControlInfo[i].nControlID, i);

	// and create custom tooltip for the tags list
	m_pToolTip = new CMyToolTipWnd;
	m_pToolTip->Create(this);

	//	done
	m_bInitialized = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}





//
BOOL CMyCustomDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch(wParam)
	{
		case WMS_UPDATE_TOOLTIP_TEXT:
			// update tooltip text
			if (m_pListControl)
			{
				CPoint point(LOWORD(lParam), HIWORD(lParam));
				m_pListControl->ClientToScreen(&point);
				UpdateToolTipText(point);
			}
			return TRUE;
	}
	
	return CDialog::OnCommand(wParam, lParam);
}


//
void CMyCustomDialog::UpdateToolTipText(CPoint point)
{
	// do the work in the derived class
//	int nIndex = m_pToolTip->ListControlHitTest(point, m_listTags, 1);
}



//
void CMyCustomDialog::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	//
/*
	if (IsWindowVisible() && m_bInitialized)
	{
		RECT rect;
		GetWindowRect(&rect);
		pMAINFRAME->SetValue(tnReviewDialogPosLeft, rect.left);
		pMAINFRAME->SetValue(tnReviewDialogPosTop, rect.top);
	}
*/
}


//
void CMyCustomDialog::OnMouseMove(UINT nFlags, CPoint point) 
{
	// see if we should disable the tooltips window
	CRect rect;
	if (m_pListControl)
	{
		m_pListControl->GetWindowRect(&rect);
		if (!rect.PtInRect(point))
			m_pToolTip->ShowWindow(SW_HIDE);
	}
	//
	CDialog::OnMouseMove(nFlags, point);
}


//
void CMyCustomDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
	// assume scroll was from listbox
	if (m_pListControl)
		m_pToolTip->ShowWindow(SW_HIDE);
}


  
//
void CMyCustomDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
	GetParent()->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
}



//
BOOL CMyCustomDialog::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
    UINT nID = pNMHDR->idFrom;
	int nIndex;
	//
    if (pTTT->uFlags & TTF_IDISHWND)
    {
        // idFrom is actually the HWND of the tool
        nID = ::GetDlgCtrlID((HWND)nID);
        if(nID)
        {
			// look up the internal ordinal of the control
			if (!m_mapIDtoIndex.Lookup(nID, nIndex))
			{
				return FALSE;
			}
            pTTT->lpszText = m_pControlInfo[nIndex].szToolTipText;
            pTTT->hinst = AfxGetResourceHandle();
            return(TRUE);
        }
    }
    return(FALSE);
}





/////////////////////////////////////////////////////////////////////////////
//
//
// Utilities
//
//
/////////////////////////////////////////////////////////////////////////////



//
void CMyCustomDialog::Initialize()
{
	// enable all controls by default
	for(int i=0;i<m_numControls;i++)
		EnableControl(i, TRUE, TRUE);
}

static HICON hU;


//
void CMyCustomDialog::EnableControl(int nIndex, BOOL bEnable, BOOL bSetImage) 
{
	if ((nIndex < 0) || (nIndex > m_numControls))
		return;

	// enable or disable control as appropriate
	GetDlgItem(m_pControlInfo[nIndex].nControlID)->EnableWindow(bEnable);

	// set proper bitmaps if desired
	if (bSetImage)
	{
		CButton* pButton = (CButton*) GetDlgItem(m_pControlInfo[nIndex].nControlID);
//		if (m_buttonIcons[nIndex] != NULL)
		if (m_pControlInfo[nIndex].nIconID != 0)
			pButton->SetIcon(*(m_buttonIcons[nIndex]));
	}
}



//
void CMyCustomDialog::EnableControlByID(int nControlID, BOOL bEnable, BOOL bSetImage) 
{
	// first look up the index
	int nIndex;
	if (!m_mapIDtoIndex.Lookup(nControlID, nIndex))
		return;

	// then enable or disable control index as appropriate
	EnableControlByID(nIndex, bEnable, bSetImage);
}
