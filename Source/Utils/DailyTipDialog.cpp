//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DailyTipDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DailyTipDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDailyTipDialog dialog


//
//
// Round()
//
static inline int Round(float fValue)
{
	if ( ((int)(fValue + 0.5f)) > ((int) fValue) )
		return ((int) fValue) + 1;
	else
		return ((int) fValue);
}




/////////////////////////////////////////////////////////////////////////////
// CDailyTipDialog dialog


CDailyTipDialog::CDailyTipDialog()
	: CDialog(IDD_TIP_OF_THE_DAY)
{
	//{{AFX_DATA_INIT(CDailyTipDialog)
	m_bShowAtStartup = FALSE;
	//}}AFX_DATA_INIT
	m_bShowAtStartup = TRUE;
	m_bLoadSpecifiedTip = FALSE;
}

//
BOOL CDailyTipDialog::Create(CWnd* pParent) 
{
	return CDialog::Create(IDD_TIP_OF_THE_DAY, pParent);
}


void CDailyTipDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDailyTipDialog)
	DDX_Control(pDX, IDC_MESSAGE, m_paneMessage);
	DDX_Check(pDX, IDC_SHOW_AT_STARTUP, m_bShowAtStartup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDailyTipDialog, CDialog)
	//{{AFX_MSG_MAP(CDailyTipDialog)
	ON_BN_CLICKED(ID_NEXT, OnNext)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_BN_CLICKED(ID_PREV, OnPrev)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDailyTipDialog message handlers



//
BOOL CDailyTipDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// get tip indices
	CString strTipStart, strTipEnd, strTipsSkip;
	strTipStart.LoadString(IDS_TIP_START);
	strTipEnd.LoadString(IDS_TIP_END);
	strTipsSkip.LoadString(IDS_TIPS_SKIP);
	m_nTipStart = _ttoi(strTipStart);
	m_nTipEnd = _ttoi(strTipEnd);
	m_nTipsSkip = _ttoi(strTipsSkip);

	// hide the "Prev" button if 
#ifdef _DEBUG
//	GetDlgItem(ID_PREV)->ShowWindow(SW_HIDE);
#endif

	// get a random index
	srand((unsigned)time(NULL));
	LoadRandomTip();
	SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	CenterWindow();

	// done
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



//
void CDailyTipDialog::OnNext() 
{
	// load the next non-empty tip
	m_strTip.Empty();
	while (m_strTip.IsEmpty() || (m_strTip[0] == _T('!')))
	{
		m_nTipIndex++;
		if (m_nTipIndex > (m_nTipEnd - m_nTipsSkip))
			m_nTipIndex = m_nTipStart;
		//
		if (!m_strTip.LoadString(m_nTipIndex))
		{
			// oops, an error -- load default tip
			m_strTip.LoadString(m_nTipStart);
			break;
		}
		// 
		m_strTip.TrimLeft();
	}
	//
	DrawTip();
}


//
void CDailyTipDialog::OnPrev() 
{
	// load the prev non-empty tip
	m_strTip.Empty();
	while (m_strTip.IsEmpty() || (m_strTip[0] == _T('!')))
	{
		m_nTipIndex--;
		if (m_nTipIndex <= m_nTipStart)
			m_nTipIndex = m_nTipEnd - m_nTipsSkip;
		//
		if (!m_strTip.LoadString(m_nTipIndex))
		{
			// oops, an error -- load default tip
			m_strTip.LoadString(m_nTipStart);
			break;
		}
		// 
		m_strTip.TrimLeft();
	}
	//
	DrawTip();
}



//
void CDailyTipDialog::LoadRandomTip() 
{
	if (!m_bLoadSpecifiedTip)
	{
		int nMax = m_nTipEnd - m_nTipStart - m_nTipsSkip;
		int nValue = Round(((rand() / (float)RAND_MAX) * (nMax+1)) - 0.4999f);
		m_nTipIndex = m_nTipStart + nValue;
	}

	// load a non-empty tip
	m_strTip.Empty();
	while (m_strTip.IsEmpty() || (m_strTip[0] == _T('!')))
	{
		// load the next tip
		if (!m_strTip.LoadString(m_nTipIndex))
		{
			// oops, an error -- load default tip
			m_strTip.LoadString(m_nTipStart);
			break;
		}
		// see if the current tip is empty
		m_strTip.TrimLeft();
		if (m_strTip.IsEmpty() || (m_strTip[0] == _T('!')))
		{
			// advance to the next tip
			m_nTipIndex++;
			if (m_nTipIndex > (m_nTipEnd - m_nTipsSkip))
				m_nTipIndex = m_nTipStart;
		}
	}
}



//
void CDailyTipDialog::OnOK() 
{
	UpdateData(TRUE);
	EndDialog(TRUE);
}



//
void CDailyTipDialog::OnClose() 
{
	// save state
	UpdateData(TRUE);
	EndDialog(TRUE);
}



//
void CDailyTipDialog::OnPaint() 
{
	// make sure the entire dialog gets redrawn
	m_paneMessage.Invalidate(NULL);

	//
	CPaintDC dc(this); // device context for painting

	//
	CRect rect;
	m_paneMessage.GetClientRect(&rect);
	CWindowDC paneDC(&m_paneMessage);

	// load & draw the bitmap
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_TIP_LEFT_PANE);
	BITMAP bmInfo;
	bitmap.GetObject(sizeof(BITMAP), &bmInfo);
	CDC newDC;
	newDC.CreateCompatibleDC(&paneDC);
	CBitmap* pOldBitmap = newDC.SelectObject(&bitmap);
	paneDC.BitBlt(0, 0, bmInfo.bmWidth, bmInfo.bmHeight, &newDC, 0, 0, SRCCOPY);
	newDC.SelectObject(pOldBitmap);
	newDC.DeleteDC();

	// fill the rect with white
	rect.DeflateRect(bmInfo.bmWidth, 0, 0, 0);
	paneDC.FillSolidRect(&rect, RGB(255,255,255));

	// deflate the rect some more
	const RECT rectAdj = { 8, 12, 4, 4 };
	rect.DeflateRect(&rectAdj);

	// draw the banner
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	CFont bigFont;
	logFont.lfHeight = -20;
	logFont.lfWeight = FW_BOLD;
	logFont.lfPitchAndFamily = FIXED_PITCH | FF_ROMAN;
	lstrcpy(logFont.lfFaceName, _T("Times New Roman"));
	bigFont.CreateFontIndirect(&logFont);

	// 
	CFont* pOldFont = (CFont*) paneDC.SelectObject(&bigFont);
	paneDC.DrawText("Did you know?", &rect, DT_WORDBREAK);
	
	// draw line
	TEXTMETRIC tm;
	paneDC.GetTextMetrics(&tm);
	rect.top += tm.tmHeight + 8;
	paneDC.MoveTo(rect.left - rectAdj.left, rect.top);
	paneDC.LineTo(rect.right + rectAdj.right, rect.top);
	rect.top += 8;

	// clean up
	paneDC.SelectObject(pOldFont);
	bigFont.DeleteObject();

	// mark the text area
	m_rectTip = rect;

	// now draw the tip text
	DrawTip();
}



//
void CDailyTipDialog::DrawTip()
{
	CWindowDC paneDC(&m_paneMessage);
	paneDC.FillSolidRect(&m_rectTip, RGB(255,255,255));

	// create font
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	CFont smallFont;
	logFont.lfHeight = -12;
	logFont.lfWeight = FW_NORMAL;
	logFont.lfPitchAndFamily = FIXED_PITCH | FF_SWISS;
	lstrcpy(logFont.lfFaceName, _T("Arial"));
	smallFont.CreateFontIndirect(&logFont);

	// draw remaining text
	CFont* pOldFont = paneDC.SelectObject(&smallFont);
	CRect rect = m_rectTip;
	paneDC.DrawText(m_strTip, &m_rectTip, DT_NOPREFIX | DT_WORDBREAK);

	// clean up
	paneDC.SelectObject(pOldFont);
	smallFont.DeleteObject();
//	m_paneMessage.ValidateRect(NULL);

#ifdef _DEBUG
	// show tip number
	CString strTitle;
	strTitle.Format("Tip of the Day (#%d)", m_nTipIndex);
	SetWindowText(strTitle);
#endif
}


