//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// dis_misc.cpp : implementation file
//

#include "stdafx.h"
#include "ObjectWithProperties.h"
#include "DispOptsMiscPage.h"
#include "progopts.h"
#include "viewopts.h"
#include "globals.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDispOptsMiscPage property page

IMPLEMENT_DYNCREATE(CDispOptsMiscPage, CPropertyPage)

CDispOptsMiscPage::CDispOptsMiscPage(CObjectWithProperties* pApp, CObjectWithProperties* pFrame, CObjectWithProperties* pView) : 
		CPropertyPage(CDispOptsMiscPage::IDD),
		m_app(*pApp), m_frame(*pFrame), m_view(*pView)
{
	//{{AFX_DATA_INIT(CDispOptsMiscPage)
	m_bShowSplashWindow = FALSE;
	m_bShowStartupAnimation = FALSE;
	m_nBitmapMode = -1;
	m_bShowBackgroundBitmap = FALSE;
	m_bScaleLargeBitmaps = FALSE;
	m_strFilePath = _T("");
	m_bLowResOption = FALSE;
	//}}AFX_DATA_INIT
//	m_bAutoAlignDialogs = m_app.GetValue(tbAutoAlignDialogs);
//	m_bShowStartupAnimation = m_app.GetValue(tbShowStartupAnimation);
	m_bShowBackgroundBitmap = m_app.GetValue(tbShowBackgroundBitmap);
	m_nBitmapMode = m_app.GetValue(tnBitmapDisplayMode);
	m_bScaleLargeBitmaps = m_app.GetValue(tbScaleLargeBitmaps);
	m_bShowSplashWindow = m_app.GetValue(tbShowSplashWindow);
	m_nBackgroundColor = m_app.GetValue(tnBackgroundColor);
	m_bLowResOption = m_app.GetValue(tbLowResOption);
	m_strFilePath = m_view.GetValueString(tszBackgroundBitmap);
}

CDispOptsMiscPage::~CDispOptsMiscPage()
{
}

void CDispOptsMiscPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDispOptsMiscPage)
	DDX_Control(pDX, IDC_COLOR_SWATCH, m_wndColorSwatch);
	DDX_Check(pDX, IDC_SHOW_SPLASH_WINDOW, m_bShowSplashWindow);
	DDX_Radio(pDX, IDC_BITMAP_MODE, m_nBitmapMode);
	DDX_Check(pDX, IDC_SHOW_BACKGROUND_BITMAP, m_bShowBackgroundBitmap);
	DDX_Check(pDX, IDC_SCALE_LARGE_BITMAPS, m_bScaleLargeBitmaps);
	DDX_Text(pDX, IDC_BITMAP_PATH, m_strFilePath);
	DDX_Check(pDX, IDC_SHOW_LOW_RES_OPTION, m_bLowResOption);
	//}}AFX_DATA_MAP
//	DDX_Check(pDX, IDC_SHOW_STARTUP_ANIMATION, m_bShowStartupAnimation);
}


BEGIN_MESSAGE_MAP(CDispOptsMiscPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDispOptsMiscPage)
	ON_BN_CLICKED(IDC_BACKGROUND_COLOR, OnBackgroundColor)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_SHOW_BACKGROUND_BITMAP, OnShowBackgroundBitmap)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_SHOW_LOW_RES_OPTION, OnShowLowResOption)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDispOptsMiscPage message handlers


//
BOOL CDispOptsMiscPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// see if we can display a bitmap
	if (!m_view.GetValue(tbCanDisplayBitmap))
	{
		m_bShowBackgroundBitmap = FALSE;
		GetDlgItem(IDC_SHOW_BACKGROUND_BITMAP)->EnableWindow(FALSE);
	}
	
	//
	OnShowBackgroundBitmap();
	UpdateColorSwatch();
	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CDispOptsMiscPage::OnShowBackgroundBitmap() 
{
	UpdateData(TRUE);
	if (m_bShowBackgroundBitmap)
	{
		GetDlgItem(IDC_BITMAP_MODE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BITMAP_MODE2)->EnableWindow(TRUE);
		GetDlgItem(IDC_SCALE_LARGE_BITMAPS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BITMAP_PATH)->EnableWindow(TRUE);
		GetDlgItem(IDC_BROWSE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BITMAP_MODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BITMAP_MODE2)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCALE_LARGE_BITMAPS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BITMAP_PATH)->EnableWindow(FALSE);
		GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);
	}
}



//
void CDispOptsMiscPage::OnBrowse() 
{
	// first retrieve the current filename & path
	CString strFilePath = (LPCTSTR) m_view.GetValuePV(tszBackgroundBitmap);
	char szName[256], szPath[256];
	szName[0] = _T('\0');
	szPath[0] = _T('\0');
	if (!strFilePath.IsEmpty())
	{
		int nPos = strFilePath.ReverseFind('\\');
		if (nPos >= 0)
		{
			strcpy(szName, strFilePath.Mid(nPos+1));
			strcpy(szPath, strFilePath.Mid(0, nPos));
		}
	}

	// create the file dialog
	CFileDialog fileDlg(TRUE, "bmp", szName, OFN_FILEMUSTEXIST,
			  			"Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*||",
		   				this);
	fileDlg.m_ofn.lpstrTitle = "Select Bitmap file";
	fileDlg.m_ofn.lpstrInitialDir = szPath;

	// then show
	if (fileDlg.DoModal() == IDCANCEL)
		return;

	// try loading the bitmap
	CString strFileName = fileDlg.GetPathName();
	if (m_view.SetValuePV(tszBackgroundBitmap, (LPVOID)(LPCTSTR)strFileName, 1) != 0) 
	{
		AfxMessageBox(FormString("Error reading file %s.\nIt may not be a Windows Bitmap file.",strFileName));
	} 
	else
	{
		m_strFilePath = strFileName;
		UpdateData(FALSE);
	}
}




//
void CDispOptsMiscPage::OnBackgroundColor() 
{
	CColorDialog colorDlg(m_nBackgroundColor, CC_FULLOPEN | CC_RGBINIT, this);
	if (colorDlg.DoModal() == IDOK)
	{
		m_nBackgroundColor = colorDlg.m_cc.rgbResult;
		UpdateColorSwatch();	
	}
}


//
void CDispOptsMiscPage::UpdateColorSwatch()
{
	CRect rect;
	m_wndColorSwatch.GetClientRect(&rect);
	CWindowDC dc(&m_wndColorSwatch);
	dc.FillSolidRect(&rect, m_nBackgroundColor);
//	dc.Draw3dRect(rect, RGB(128,128,128), RGB(255,255,255));
}


//
void CDispOptsMiscPage::OnShowLowResOption() 
{
	UpdateData(TRUE);
	//
	if (m_bLowResOption != m_app.GetValue(tbLowResOption))
		AfxMessageBox("This change will not take effect until you restart the program.", MB_ICONINFORMATION | MB_OK);
}


//
BOOL CDispOptsMiscPage::Update()
{
	// store results & return if changes affect the display
	BOOL bChanged = FALSE;
	if ( (m_bShowBackgroundBitmap != m_app.GetValue(tbShowBackgroundBitmap)) ||
		 (m_nBitmapMode != m_app.GetValue(tnBitmapDisplayMode)) ||
		 (m_bScaleLargeBitmaps != m_app.GetValue(tbScaleLargeBitmaps)) ||
	 	 (m_nBackgroundColor != m_app.GetValue(tnBackgroundColor)) ||
		 (m_strFilePath != m_view.GetValue(tszBackgroundBitmap)) )
		bChanged = TRUE;

	// update resolution setting in deferred mode
	if (m_bLowResOption != m_app.GetValue(tbLowResOption))
	{
		m_app.SetValue(tbToggleResolutionMode, TRUE);
//		deck.InitializeBitmaps();
	}

	//
//	m_app.SetValue(tbAutoAlignDialogs, m_bAutoAlignDialogs);
//	m_app.SetValue(tbShowStartupAnimation, m_bShowStartupAnimation);
	m_view.SetValuePV(tszBackgroundBitmap, (LPVOID)(LPCTSTR)m_strFilePath);
	m_app.SetValue(tbShowBackgroundBitmap, m_bShowBackgroundBitmap);
	m_app.SetValue(tnBitmapDisplayMode, m_nBitmapMode);
	m_app.SetValue(tbScaleLargeBitmaps, m_bScaleLargeBitmaps);
	m_app.SetValue(tnBackgroundColor, m_nBackgroundColor);
	m_app.SetValue(tbShowSplashWindow, m_bShowSplashWindow);
	//
	return bChanged;
}




void CDispOptsMiscPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	UpdateColorSwatch();
	
	// Do not call CPropertyPage::OnPaint() for painting messages
}



