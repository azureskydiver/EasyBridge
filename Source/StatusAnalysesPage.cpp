//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// StatusAnalysesPage.cpp : implementation file
//

#include "stdafx.h"
#include "easyb.h"
#include "easybdoc.h"
#include "mainfrm.h"
#include "Mainframeopts.h"
#include "StatusAnalysesPage.h"
#include "StatusWnd.h"
#include "Player.h"
#include "subclass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
CStatusAnalysesPage* CStatusAnalysesPage::m_pPage = NULL;


/////////////////////////////////////////////////////////////////////////////
// CStatusAnalysesPage property page

IMPLEMENT_DYNCREATE(CStatusAnalysesPage, CPropertyPage)

CStatusAnalysesPage::CStatusAnalysesPage() : CPropertyPage(CStatusAnalysesPage::IDD)
{
	//{{AFX_DATA_INIT(CStatusAnalysesPage)
	m_bShowWest = FALSE;
	m_bShowNorth = FALSE;
	m_bShowEast = FALSE;
	m_bShowSouth = FALSE;
	m_bShowAll = FALSE;
	//}}AFX_DATA_INIT

	//
	m_bShowWest = TRUE;
	m_bShowNorth = TRUE;
	m_bShowEast = TRUE;
	m_bShowSouth = TRUE;
	m_bShowAll = TRUE;
	//
	m_nCurrentChild = -1;
	m_bInitialized = FALSE;
}

CStatusAnalysesPage::~CStatusAnalysesPage()
{
}

void CStatusAnalysesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatusAnalysesPage)
	DDX_Check(pDX, IDC_WEST, m_bShowWest);
	DDX_Check(pDX, IDC_NORTH, m_bShowNorth);
	DDX_Check(pDX, IDC_EAST, m_bShowEast);
	DDX_Check(pDX, IDC_SOUTH, m_bShowSouth);
	DDX_Check(pDX, IDC_ALL, m_bShowAll);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatusAnalysesPage, CPropertyPage)
	//{{AFX_MSG_MAP(CStatusAnalysesPage)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_WEST, OnClickWindowShow)
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_NORTH, OnClickWindowShow)
	ON_BN_CLICKED(IDC_EAST, OnClickWindowShow)
	ON_BN_CLICKED(IDC_SOUTH, OnClickWindowShow)
	ON_BN_CLICKED(IDC_ALL, OnClickWindowShow)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_HIDE, OnHide)
	ON_COMMAND(ID_CLEAR_PANE, OnClearPane)
	ON_COMMAND(ID_CLEAR_ALL_PANES, OnClearAllPanes)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusAnalysesPage message handlers


//
BOOL CStatusAnalysesPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// init the window display order
	m_nWindowOrder[0] = WEST;
	m_nWindowOrder[1] = NORTH;
	m_nWindowOrder[2] = EAST;
	m_nWindowOrder[3] = SOUTH;
	//
	m_nPlayerToWindow[SOUTH] = 3;
	m_nPlayerToWindow[WEST] = 0;
	m_nPlayerToWindow[NORTH] = 1;
	m_nPlayerToWindow[EAST] = 2;

	// create the rich edit controls
	m_pFont = (CFont*) pMAINFRAME->GetValuePV(tpFontAnalysis);
	RECT rect = { 0, 0, 0, 0 };
	for(int i=0;i<4;i++)
	{
		m_edit[i].Create(ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL, rect, this, IDC_EDIT1+i);
		m_edit[i].HideSelection(TRUE, TRUE);
		// subclass
		if (i == 0)
			wpOrigRichEditProc = (WNDPROC) SetWindowLong(m_edit[i].GetSafeHwnd(), GWL_WNDPROC, (LONG) RichEditSubclassProc);
		else
			SetWindowLong(m_edit[i].GetSafeHwnd(), GWL_WNDPROC, (LONG) RichEditSubclassProc);
		// and set font
		m_edit[i].SendMessage(WM_SETFONT, (LONG)m_pFont->m_hObject, FALSE);
	}

	// get text metrics
	CWindowDC dc(&m_edit[0]);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	m_nFontHeight = tm.tmHeight;
	//
	CRect editRect;
	m_edit[0].GetWindowRect(&editRect);
	m_numVisibleRows = editRect.Height() / m_nFontHeight;

	// and subclass the buttons
	wpOrigCheckBoxProc = (WNDPROC) SetWindowLong(GetDlgItem(IDC_WEST)->GetSafeHwnd(), GWL_WNDPROC, (LONG) ButtonSubclassProc);
	SetWindowLong(GetDlgItem(IDC_NORTH)->GetSafeHwnd(), GWL_WNDPROC, (LONG) ButtonSubclassProc);
	SetWindowLong(GetDlgItem(IDC_EAST)->GetSafeHwnd(), GWL_WNDPROC, (LONG) ButtonSubclassProc);
	SetWindowLong(GetDlgItem(IDC_SOUTH)->GetSafeHwnd(), GWL_WNDPROC, (LONG) ButtonSubclassProc);
	SetWindowLong(GetDlgItem(IDC_ALL)->GetSafeHwnd(), GWL_WNDPROC, (LONG) ButtonSubclassProc);

	// record initial window characterisitcs
	CRect winRect, itemRect;
	GetClientRect(&winRect);
	// use the top button to get top left coords
/*
	CWnd* pWnd = GetDlgItem(IDC_WEST);
	pWnd->GetWindowRect(&itemRect);
	ScreenToClient(&itemRect);
	m_nInitialX = itemRect.left;
	m_nInitialY = itemRect.top;
*/

	// use the bottom button to get margins
	CWnd* pWnd = GetDlgItem(IDC_ALL);
	pWnd->GetWindowRect(&itemRect);
	ScreenToClient(&itemRect);
	m_nInitialXMargin = itemRect.left - winRect.left;
	m_nInitialYMargin = winRect.bottom - itemRect.bottom;

	// get label height
	pWnd = GetDlgItem(IDC_LABEL_WEST);
	pWnd->GetWindowRect(&itemRect);
	m_nLabelHeight = itemRect.Height();

	// load the popup menu
	m_menuPopup.LoadMenu(IDR_ANALYSIS_POPUP);

	// enable each window as appropriate
	m_bShowWest = pMAINFRAME->GetValue(tbAnalysisDialogActive, WEST);
	m_bShowNorth = pMAINFRAME->GetValue(tbAnalysisDialogActive, NORTH);
	m_bShowEast = pMAINFRAME->GetValue(tbAnalysisDialogActive, EAST);
	m_bShowSouth = pMAINFRAME->GetValue(tbAnalysisDialogActive, SOUTH);
	m_bShowAll = m_bShowWest && m_bShowNorth && m_bShowEast && m_bShowSouth;
	//
	m_edit[m_nPlayerToWindow[WEST]].ShowWindow(m_bShowWest? SW_SHOW : SW_HIDE);
	m_edit[m_nPlayerToWindow[NORTH]].ShowWindow(m_bShowNorth? SW_SHOW : SW_HIDE);
	m_edit[m_nPlayerToWindow[EAST]].ShowWindow(m_bShowEast? SW_SHOW : SW_HIDE);
	m_edit[m_nPlayerToWindow[SOUTH]].ShowWindow(m_bShowSouth? SW_SHOW : SW_HIDE);
	//
	UpdateData(FALSE);

	// done
	m_pPage = this;
	m_bInitialized = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



//
// OnClose() 
//
void CStatusAnalysesPage::OnClose() 
{
	//
	m_menuPopup.DestroyMenu();
	//
	CPropertyPage::OnClose();
}



//
// OnRButtonDown()
//
void CStatusAnalysesPage::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// see which child this is
	CRect rect;
	m_nCurrentChild = -1;
	for(int i=0;i<4;i++)
	{
		if (!m_edit[i].IsWindowVisible())
			continue;
		m_edit[i].GetWindowRect(&rect);
		ScreenToClient(&rect);
		if (rect.PtInRect(point))
			m_nCurrentChild = m_nWindowOrder[i];
	}

	// get actual screen coordinates and show menu
		ClientToScreen(&point);
	if (m_nCurrentChild >= 0)
	{
		CMenu* pPopup = m_menuPopup.GetSubMenu(0);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
							   point.x, 
							   point.y,
							   this, NULL);
	}
	else
	{
		// send it up to the parent
		CWnd* pParent = GetParent();
		pParent->ScreenToClient(&point);
		pParent->SendMessage(WM_RBUTTONDOWN, nFlags, MAKELONG(point.x, point.y));
	}
}




//
void CStatusAnalysesPage::OnSize(UINT nType, int cx, int cy) 
{
	if (!m_bInitialized)
		return;
	CPropertyPage::OnSize(nType, cx, cy);

	// resize the buttons & edit controls
	CRect rectClient;
	GetClientRect(&rectClient);

	// return if the sizes are not valid
	if (rectClient.left == 0 && rectClient.top == 0 && rectClient.right == 0 && rectClient.bottom == 0)
		return;
	if ((rectClient.right <= rectClient.left) || (rectClient.bottom <= rectClient.top))
		return;

	// count the # of panels
	int numPanels = 0;
	for(int i=0;i<4;i++)
	{
		if (((CButton*)GetDlgItem(IDC_WEST+i))->GetCheck())
			numPanels++;
	}

	// we need the parent docking bar's current orientation
	if (CStatusWnd::m_pWnd == NULL)
		return;
	DWORD nParentStyle = CStatusWnd::m_pWnd->GetBarStyle();
	
	//
	if ((nParentStyle & CBRS_ORIENT_HORZ) || (nParentStyle& CBRS_FLOATING))
	{
		// docked horizontal or floating 

		//
		const int tnTopMargin = 0;
		const int tnBottomMargin = 2;
		const int tnSpacer = 3;
		const int tnButtonWidth = 40;
		int nTotalWidth = rectClient.Width();

		//
		int nPanelWidth = (numPanels > 0)? ((nTotalWidth - numPanels*tnSpacer - tnButtonWidth - m_nInitialXMargin*2) / numPanels) : 0;
		int nPanelHeight = rectClient.Height() - tnTopMargin - tnBottomMargin - m_nLabelHeight;

		// size the buttons
		int nX = m_nInitialXMargin;
		int nY = tnTopMargin;
		//
		int nCurrY = nY + m_nLabelHeight;
		int nBtnHeight = nPanelHeight / 5;
		//
		for(i=0;i<5;i++)
		{
			CButton* pButton = (CButton*) GetDlgItem(IDC_WEST+i);
			pButton->MoveWindow(nX, nCurrY, tnButtonWidth, nBtnHeight);
			nCurrY += nBtnHeight;
		}
		nX += tnButtonWidth + tnSpacer + 3;

		// relocate the edit controls & labels
		CRect rect;
		for(i=0;i<4;i++)
		{
			CStatic* pLabel = (CStatic*) GetDlgItem(IDC_LABEL_WEST+i);
			if (((CButton*)GetDlgItem(IDC_WEST+i))->GetCheck())
			{
				// first show the label and set its text
				pLabel->ShowWindow(SW_SHOW);
				pLabel->SetWindowText(PositionToString(m_nWindowOrder[i]));

				// then get the label's width and move it
				pLabel->GetWindowRect(&rect);
				pLabel->MoveWindow(nX+2, nY, rect.Width(), m_nLabelHeight);

				// move & size the edit box
				m_edit[i].MoveWindow(nX, nY+m_nLabelHeight, nPanelWidth, nPanelHeight);

				// save metrics
				m_edit[i].GetWindowRect(&rect);
				m_numVisibleRows = rect.Height() / m_nFontHeight;

				// and move right
				nX += nPanelWidth + tnSpacer;
			}
			else
			{
				// hide the label
				pLabel->ShowWindow(SW_HIDE);
			}
		}
	}
	else
	{
		// docked vertically

		//
		const int tnTopMargin = 4;
		const int tnBottomMargin = 0;
		const int tnSpacer = 3;
		const int tnButtonHeight = 20;
		int nTotalHeight = rectClient.Height();

		//
		int nPanelWidth = rectClient.Width();
		int nPanelHeight = (numPanels > 0)? ((nTotalHeight - numPanels*(m_nLabelHeight+tnSpacer) - tnButtonHeight - tnTopMargin - tnBottomMargin) / numPanels) : 0;

		// size the buttons
		int nX = 0;
		int nY = tnTopMargin;
		//
		int nCurrX = nX;
		int nBtnWidth = rectClient.Width() / 5;
		//
		for(i=0;i<5;i++)
		{
			CButton* pButton = (CButton*) GetDlgItem(IDC_WEST+i);
			pButton->MoveWindow(nCurrX, nY, nBtnWidth, tnButtonHeight);
			nCurrX += nBtnWidth;
		}
		nY += tnButtonHeight+ tnSpacer;

		// relocate the edit controls & labels
		CRect rect;
		for(i=0;i<4;i++)
		{
			CStatic* pLabel = (CStatic*) GetDlgItem(IDC_LABEL_WEST+i);
			if (((CButton*)GetDlgItem(IDC_WEST+i))->GetCheck())
			{
				// first show the label and set its text
				pLabel->ShowWindow(SW_SHOW);
				pLabel->SetWindowText(PositionToString(m_nWindowOrder[i]));

				// then get the label's width and move it
				pLabel->GetWindowRect(&rect);
				pLabel->MoveWindow(nX+2, nY, rect.Width(), m_nLabelHeight);

				// move & size the edit box
				m_edit[i].MoveWindow(nX, nY+m_nLabelHeight, nPanelWidth, nPanelHeight);

				// save metrics
				m_edit[i].GetWindowRect(&rect);
				m_numVisibleRows = rect.Height() / m_nFontHeight;

				// and move down
				nY += nPanelHeight + m_nLabelHeight + tnSpacer;
			}
			else
			{
				// hide the label
				pLabel->ShowWindow(SW_HIDE);
			}
		}
	}
}


//
void CStatusAnalysesPage::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CPropertyPage::OnKeyDown(nChar, nRepCnt, nFlags);
	GetParent()->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
}



//
void CStatusAnalysesPage::Clear(int nPosition)
{
	if (!m_bInitialized)
		return;
	//
	if ((nPosition == WEST) || (nPosition == -1))
	{
		PLAYER(WEST).ClearAnalysis();
		m_edit[0].SetWindowText(_T(""));
	}
	if ((nPosition == NORTH) || (nPosition == -1))
	{
		PLAYER(NORTH).ClearAnalysis();
		m_edit[1].SetWindowText(_T(""));
	}
	if ((nPosition == EAST) || (nPosition == -1))
	{
		PLAYER(EAST).ClearAnalysis();
		m_edit[2].SetWindowText(_T(""));
	}
	if ((nPosition == SOUTH) || (nPosition == -1))
	{
		PLAYER(SOUTH).ClearAnalysis();
		m_edit[3].SetWindowText(_T(""));
	}
}


//
void CStatusAnalysesPage::SetAnalysisText(int nPosition, LPCTSTR szAnalysisText)
{
	if (!m_bInitialized)
		return;
	//
	switch(nPosition)
	{
		case WEST:
			m_edit[0].SetWindowText(szAnalysisText);
			ScrollToEnd(m_edit[WEST]);
			break;
		case NORTH:
			m_edit[1].SetWindowText(szAnalysisText);
			ScrollToEnd(m_edit[NORTH]);
			break;
		case EAST:
			m_edit[2].SetWindowText(szAnalysisText);
			ScrollToEnd(m_edit[EAST]);
			break;
		case SOUTH:
			m_edit[3].SetWindowText(szAnalysisText);
			ScrollToEnd(m_edit[SOUTH]);
			break;
	}
}





//
void CStatusAnalysesPage::ScrollToEnd(CRichEditCtrl& edit)
{
	int numLines = edit.GetLineCount();
	int nFirstLine = edit.GetFirstVisibleLine();
	if (numLines > m_numVisibleRows) 
	{
		int nScrollCount = numLines - m_numVisibleRows - nFirstLine - 1;
//		int nScrollCount = numLines - m_numVisibleRows - nFirstLine;
		if (nScrollCount > 0)
			edit.LineScroll(nScrollCount);
	}
}



/*
//
void CStatusAnalysesPage::SetFont()
{
	// get current font description
	CFont* pFont = m_pEdit->GetFont();
	LOGFONT lf;
	if (pFont != NULL)
		pFont->GetObject(sizeof(LOGFONT), &lf);
	else
		::GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &lf);
	//
	CFontDialog dlg(&lf, CF_SCREENFONTS|CF_INITTOLOGFONTSTRUCT);
	// switch to new font.
	if (dlg.DoModal() == IDOK) 
		pMAINFRAME->SetFont(tpFontAnalysis, lf);
}
*/


//
void CStatusAnalysesPage::Update()
{
	// return if not all inits are complete
	if (!m_bInitialized)
		return;
}


//
void CStatusAnalysesPage::OnClickWindowShow() 
{
	//
	if (!m_bInitialized)
		return;

	//
	BOOL bOldShowWest = m_bShowWest;
	BOOL bOldShowNorth = m_bShowNorth;
	BOOL bOldShowEast = m_bShowEast;
	BOOL bOldShowSouth = m_bShowSouth;
	BOOL bOldShowAll = m_bShowAll;
	UpdateData(TRUE);

	// check if "All" was enabled
	if (m_bShowAll && !bOldShowAll)
	{
		m_bShowWest = m_bShowEast = m_bShowNorth = m_bShowSouth = TRUE;
		UpdateData(FALSE);
	}
	// check if "All was disabled
	else if (!m_bShowAll && bOldShowAll)
	{
		m_bShowWest = m_bShowEast = m_bShowNorth = m_bShowSouth = FALSE;
		UpdateData(FALSE);
	}
	// see if "All" buton should be unchecked
	else if (!m_bShowWest || !m_bShowNorth || !m_bShowEast || !m_bShowSouth)
	{
		m_bShowAll = FALSE;
		UpdateData(FALSE);
	}

	// if closing a window, inform the main frame
	if (m_bShowWest && !bOldShowWest)
		pMAINFRAME->InformChildFrameOpened(twAnalysisDialog, WEST);
	else if (!m_bShowWest && bOldShowWest)
		pMAINFRAME->InformChildFrameClosed(twAnalysisDialog, WEST);
	//
	if (m_bShowNorth && !bOldShowNorth)
		pMAINFRAME->InformChildFrameOpened(twAnalysisDialog, NORTH);
	else if (!m_bShowNorth && bOldShowNorth)
		pMAINFRAME->InformChildFrameClosed(twAnalysisDialog, NORTH);
	//
	if (m_bShowEast && !bOldShowEast)
		pMAINFRAME->InformChildFrameOpened(twAnalysisDialog, EAST);
	else if (!m_bShowEast && bOldShowEast)
		pMAINFRAME->InformChildFrameClosed(twAnalysisDialog, EAST);
	//
	if (m_bShowSouth && !bOldShowSouth)
		pMAINFRAME->InformChildFrameOpened(twAnalysisDialog, SOUTH);
	else if (!m_bShowSouth && bOldShowSouth)
		pMAINFRAME->InformChildFrameClosed(twAnalysisDialog, SOUTH);

	// then enable each window
	m_edit[m_nPlayerToWindow[WEST]].ShowWindow(m_bShowWest? SW_SHOW : SW_HIDE);
	m_edit[m_nPlayerToWindow[NORTH]].ShowWindow(m_bShowNorth? SW_SHOW : SW_HIDE);
	m_edit[m_nPlayerToWindow[EAST]].ShowWindow(m_bShowEast? SW_SHOW : SW_HIDE);
	m_edit[m_nPlayerToWindow[SOUTH]].ShowWindow(m_bShowSouth? SW_SHOW : SW_HIDE);
	
	// and resize
	CRect rect;
	GetClientRect(&rect);
	OnSize(0, rect.Width(), rect.Height());
}



//
// ShowAnalysis()
//
// - called from outside
//
void CStatusAnalysesPage::ShowAnalysis(int nPosition, BOOL bShow)
{
	if (!m_bInitialized)
		return;
	//
	switch(nPosition)
	{
		case WEST:
			m_bShowWest = bShow;
			break;
		case NORTH:
			m_bShowNorth = bShow;
			break;
		case EAST:
			m_bShowEast = bShow;
			break;
		case SOUTH:
			m_bShowSouth = bShow;
			break;
		case -1:
			m_bShowWest = m_bShowNorth = m_bShowSouth = m_bShowSouth = bShow;
			break;
	}
	UpdateData(FALSE);
	OnClickWindowShow();
}



//
// IsAnalysisShown()
//
// - called from outside
//
BOOL CStatusAnalysesPage::IsAnalysisShown(int nPosition)
{
	switch(nPosition)
	{
		case WEST:
			return m_bShowWest;
			break;
		case NORTH:
			return m_bShowNorth;
			break;
		case EAST:
			return m_bShowEast;
			break;
		case SOUTH:
			return m_bShowSouth;
			break;
	}
	//
	return FALSE;
}


//
void CStatusAnalysesPage::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	if ((m_nCurrentChild >= 0) && (m_nCurrentChild <= 3))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

//
// OnEditCopy() 
// 
void CStatusAnalysesPage::OnEditCopy() 
{
	if ((m_nCurrentChild < 0) || (m_nCurrentChild > 3))
		return;
	//
	m_edit[m_nCurrentChild].SetSel(0, -1);
	m_edit[m_nCurrentChild].Copy();
	m_edit[m_nCurrentChild].SetSel(-1, -1);
}



//
// OnClearPane() 
//
void CStatusAnalysesPage::OnClearPane() 
{
	if ((m_nCurrentChild < 0) || (m_nCurrentChild > 3))
		return;
	Clear(m_nCurrentChild);
}



//
// OnClearAllPanes() 
//
void CStatusAnalysesPage::OnClearAllPanes() 
{
	Clear();
}


//
// OnHide() 
//
void CStatusAnalysesPage::OnHide() 
{
	if ((m_nCurrentChild < 0) || (m_nCurrentChild > 3))
		return;

	// simulate a button click to hide a window
	((CButton*)GetDlgItem(IDC_WEST+m_nCurrentChild))->SetCheck(0);
	OnClickWindowShow();
}
