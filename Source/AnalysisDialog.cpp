//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// anlysdlg.cpp : implementation file
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "mainfrm.h"
#include "MainFrameOpts.h"
#include "subclass.h"
#include "progopts.h"
#include "player.h"
#include "playeropts.h"
#include "analysisDialog.h"
#include "Help\HelpCode.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//const int EDIT_MARGIN = 2;
const int EDIT_MARGIN = 0;

/////////////////////////////////////////////////////////////////////////////
// CAnalysisDialog dialog


CAnalysisDialog::CAnalysisDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAnalysisDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAnalysisDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bShow = FALSE;
}


void CAnalysisDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnalysisDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnalysisDialog, CDialog)
	//{{AFX_MSG_MAP(CAnalysisDialog)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_MOVE()
	ON_WM_SHOWWINDOW()
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAnalysisDialog message handlers

//
int CAnalysisDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	//
	// create the RTFEdit control
	//
	CRect rect;
	GetClientRect(&rect);
	m_pEdit = new CRichEditCtrl;
	if (!m_pEdit->Create(ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL, 
						 rect, this, IDC_EDIT))
	{
		// oops!!!
		ASSERT(FALSE);
		delete m_pEdit;
		m_pEdit = NULL;
		return -1;
	}

	// set window icon
	m_hIcon = theApp.LoadIcon(IDR_ANALYSIS);
	SetIcon(m_hIcon, FALSE);

	//
	return 0;
}

//
void CAnalysisDialog::OnClose() 
{
	pMAINFRAME->InformChildFrameClosed(twAnalysisDialog, m_nPlayer)	;
	CDialog::OnClose();
}

//
void CAnalysisDialog::OnDestroy() 
{
	CDialog::OnDestroy();
	delete m_pEdit;
}

//
BOOL CAnalysisDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// subclass the RTF edit control
	wpOrigRichEditProc = (WNDPROC) SetWindowLong(m_pEdit->m_hWnd, GWL_WNDPROC, (LONG) RichEditSubclassProc);
	// load the icon
//	m_hIcon = LoadIcon(theApp.m_hInstance,
//					   MAKEINTRESOURCE(IDR_ANALYSIS_SMALL));
	// set the edit control's font
	SendDlgItemMessage(IDC_EDIT,
		 			   WM_SETFONT, 
					   (LONG)m_pFont->m_hObject,
					   FALSE);
	// and set window info
	CString strTemp;
	strTemp.Format("%s Analysis", PositionToString(m_nPlayer));
	SetWindowText(strTemp);
//	pEdit->LimitText(30000);
	//
	CMenu* pMenu = GetSystemMenu(FALSE);
	pMenu->AppendMenu(MF_SEPARATOR,0,"");
	pMenu->AppendMenu(MF_STRING,ID_CLEAR_ANALYSIS, "Clear");
	pMenu->AppendMenu(MF_STRING,ID_SAVE_ANALYSIS, "Save...");
	pMenu->AppendMenu(MF_STRING,ID_SET_ANALYSIS_FONT, "Set Font...");
	// set window placement information
	m_placement.length = sizeof(WINDOWPLACEMENT);
	m_placement.flags = WPF_SETMINPOSITION;
	m_placement.showCmd = SW_HIDE;
	m_placement.rcNormalPosition = m_rect;
	switch(m_nPlayer) 
	{
		case SOUTH:
			m_placement.ptMinPosition.x = 50;
			break;
		case WEST:
			m_placement.ptMinPosition.x = 6;
			break;
		case NORTH:
			m_placement.ptMinPosition.x = 558;
			break;
		case EAST:
			m_placement.ptMinPosition.x = 598;
			break;
	}
	m_placement.ptMinPosition.y = 394;
	m_placement.ptMaxPosition.x = 0;
	m_placement.ptMaxPosition.y = 0;
	SetWindowPlacement(&m_placement);
	SizeTextBox();
	SetDefID(IDC_BID_PASS);
	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CAnalysisDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
	GetParent()->PostMessage(WM_KEYDOWN, nChar, (nRepCnt&0xFFFF) | ((nFlags&0xFFFF)<<16));
}



//
void CAnalysisDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	SizeTextBox();
	if (m_bShow) 
	{
		RECT rect;
		GetWindowRect(&rect);
		// make sure rect is non-empty (such as when closing)
		if ((rect.right - rect.left) > 0)
		{
			// update position info
			pMAINFRAME->SetValue(tnAnalysisDialogRectLeft, rect.left, m_nPlayer);
			pMAINFRAME->SetValue(tnAnalysisDialogRectTop, rect.top, m_nPlayer);
			pMAINFRAME->SetValue(tnAnalysisDialogRectRight, rect.right, m_nPlayer);
			pMAINFRAME->SetValue(tnAnalysisDialogRectBottom, rect.bottom, m_nPlayer);
		}
		//
		GetParent()->SetFocus();
	}
}

void CAnalysisDialog::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	if (m_bShow) 
	{
		RECT rect;
		GetWindowRect(&rect);
		// update position info
		pMAINFRAME->SetValue(tnAnalysisDialogRectLeft, rect.left, m_nPlayer);
		pMAINFRAME->SetValue(tnAnalysisDialogRectTop, rect.top, m_nPlayer);
		pMAINFRAME->SetValue(tnAnalysisDialogRectRight, rect.right, m_nPlayer);
		pMAINFRAME->SetValue(tnAnalysisDialogRectBottom, rect.bottom, m_nPlayer);
		//
		GetParent()->SetFocus();
	}
}


//
void CAnalysisDialog::SizeTextBox() 
{
	CRect textRect;
	GetClientRect(&textRect);
	m_pEdit->SetWindowPos(&wndTop, textRect.left, textRect.top,
					      textRect.Width(), textRect.Height(),SWP_NOZORDER);
	m_pEdit->ShowWindow(SW_SHOW);
}



//
BOOL CAnalysisDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch (wParam)
	{
		case WMS_SET_ANALYSIS_FONT:
			SendDlgItemMessage(IDC_EDIT,
			 				   WM_SETFONT, 
							   (LONG)pMAINFRAME->m_analysisFont.m_hObject,
							   FALSE);
			return TRUE;
		case WMS_SETTEXT:
			SetText((LPCTSTR)lParam);
			return TRUE;
	}
	//
	return CDialog::OnCommand(wParam, lParam);
}


//
void CAnalysisDialog::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if (nID == ID_SAVE_ANALYSIS) 
	{
		CFileDialog fileDlg(FALSE, "txt", "*.txt",
						    OF_READ | OFN_OVERWRITEPROMPT,
				  			"Analysis Files (*.txt)|*.txt|All Files (*.*)|*.*||",
  							theApp.m_pMainWnd);
		fileDlg.m_ofn.lpstrTitle = "Save Analysis Record";
		if (fileDlg.DoModal() == IDCANCEL) 
		{
			int nCode = ::CommDlgExtendedError();
			return;
		}
		// get selected file and path information
		CString strPath = fileDlg.GetPathName();
		//
		BeginWaitCursor();
		CFile file;
		CFileException fileException;
		int nCode = file.Open(strPath, 
				  			  CFile::modeWrite | CFile::modeCreate | CFile::shareDenyWrite, 
							  &fileException);
		if (nCode == 0) 
		{
			AfxMessageBox("Error opening file.");
			return;
		}
		// write
		CString strTemp;
		strTemp.Format("EasyBridge Analysis record\r\nProgram version %s",
							theApp.GetProgramVersionString());
		file.Write((LPCTSTR)strTemp,strTemp.GetLength());
		//
		CTime time = CTime::GetCurrentTime();
		strTemp.Format("Time: %s\r\n",(LPCTSTR)time.Format("%c"));
		file.Write((LPCTSTR)strTemp,strTemp.GetLength());
		//
		strTemp.Format("Player is %s\r\n", PositionToString(m_nPlayer));
		file.Write((LPCTSTR)strTemp,strTemp.GetLength());
		//
		strTemp = "-------------------------\r\n";
		file.Write((LPCTSTR)strTemp,strTemp.GetLength());
		//
		CRichEditCtrl* pEdit = (CRichEditCtrl*) GetDlgItem(IDC_EDIT);
		pEdit->GetWindowText(strTemp);
		file.Write((LPCTSTR)strTemp,strTemp.GetLength());
		// done
		file.Close();
		EndWaitCursor();
		return;
	} 
	else if (nID == ID_CLEAR_ANALYSIS) 
	{
		pDOC->GetPlayer(m_nPlayer)->SetValueString(tszAnalysis, "");
	}
	else if (nID == ID_SET_ANALYSIS_FONT) 
	{
		SetFont();
		return;
	} 
	else 
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


//
void CAnalysisDialog::Clear() 
{
	m_pEdit->SetWindowText("");
}

//
void CAnalysisDialog::SetText(LPCTSTR szText) 
{
	m_pEdit->SetWindowText(szText);
	ScrollToBottom();
}


//
void CAnalysisDialog::ScrollToBottom() 
{
	RECT rect;
	m_pEdit->GetRect(&rect);
	int nSize = rect.bottom - rect.top;
	//
	CDC* pDC = GetDC();
	CFont* pOldFont = (CFont*) pDC->SelectObject(m_pFont);
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);
	//
	// scroll to bottom
//	int nHeight = tm.tmHeight - tm.tmInternalLeading;
	int nHeight = tm.tmHeight;
	int nVisibleRows = nSize / nHeight;
	int numLines = m_pEdit->GetLineCount();
	int nFirstLine = m_pEdit->GetFirstVisibleLine();
	if (numLines > nVisibleRows) 
	{
//		int nScrollCount = numLines - nVisibleRows - nFirstLine - 1;
		int nScrollCount = numLines - nVisibleRows - nFirstLine;
		m_pEdit->LineScroll(nScrollCount);
	}
	// and set selection at the end of the text
	int nTextLength = m_pEdit->GetTextLength();
	m_pEdit->SetSel(nTextLength,nTextLength);
}


//
void CAnalysisDialog::SetFont() 
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


//
void CAnalysisDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if (IsIconic()) 
	{
		// Erase the icon background when placed over other app window 
		DefWindowProc(WM_ICONERASEBKGND, (WORD)dc.m_hDC, 0L); 
		// Center the icon 
		CRect rc; 
		GetClientRect(&rc); 
		rc.left = (rc.right  - ::GetSystemMetrics(SM_CXICON)) >> 1; 
		rc.top  = (rc.bottom - ::GetSystemMetrics(SM_CYICON)) >> 1; 
		// Draw the icon 
		HICON hIcon = AfxGetApp()->LoadIcon(AFX_IDI_STD_FRAME); 
		dc.DrawIcon(rc.left, rc.top, m_hIcon); 
	} 
	else 
	{
		CString strTemp;
		strTemp.Format("%s Analysis", PositionToString(m_nPlayer));
		SetWindowText(strTemp);
	}
	//	
	GetParent()->SetFocus();
}


//
void CAnalysisDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	m_bShow = bShow;
}


//
BOOL CAnalysisDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
//	WinHelp(DIALOG_ANALYSIS_WINDOW);
	return TRUE;	
//	return CDialog::OnHelpInfo(pHelpInfo);
}


