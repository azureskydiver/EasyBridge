//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// GIBDialog.cpp : implementation file
//

#include "stdafx.h"
#include "EasyB.h"
#include "GIBDialog.h"
#include "GIBMonitorThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// default progress bar time increment
const int DEFAULT_INCREMENT = 250;	// milliseconds between progress ctl updates  NCR added int


/////////////////////////////////////////////////////////////////////////////
// CGIBDialog dialog


CGIBDialog::CGIBDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CGIBDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGIBDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bFinished = FALSE;
	InitializeCriticalSection(&m_csDialogFinished);
	m_hDialogReady = CreateEvent(NULL, FALSE, FALSE, NULL);
}

//
CGIBDialog::~CGIBDialog()
{
	DeleteCriticalSection(&m_csDialogFinished);
	CloseHandle(m_hDialogReady);
}

void CGIBDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGIBDialog)
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGIBDialog, CDialog)
	//{{AFX_MSG_MAP(CGIBDialog)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGIBDialog message handlers


//
BOOL CGIBDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CenterWindow();
	m_nProgressIncrement = DEFAULT_INCREMENT;
	m_nProgressUnits = (m_nProcessTime * 1000) / m_nProgressIncrement;
	m_progress.SetRange(0, m_nProgressUnits);
	m_progress.SetStep(1);
	m_progress.SetPos(1);
	//
	int nTimeIncrement = (m_nProcessTime * 1000) / m_nProgressUnits;
	SetTimer(0, nTimeIncrement, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//
void CGIBDialog::WaitForDialogInit() 
{
	WaitForSingleObject(m_hDialogReady, INFINITE);
}


//
void CGIBDialog::OnTimer(UINT nIDEvent) 
{
/*
	// check for messages
    MSG msg;
    while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) 
    { 
        if (!AfxGetApp()->PumpMessage()) 
        { 
            ::PostQuitMessage(0); 
            return; 
        } 
    } 
*/
	// update the progress bar
	m_progress.StepIt();
}


//
void CGIBDialog::OnClose() 
{
	KillTimer(0);
	CDialog::OnClose();
}


//
// OnCancel() 
//
// user pressed the "Cancel" button
//
void CGIBDialog::OnCancel() 
{
	// deal with a simultaneous "Cancel" press & GIB finish
	if (!MarkDialogFinished())
		return;
	//
//	if (AfxMessageBox("Cancelling GIB will cause the game to be restarted.\nDo you wish to continue with the cancel operation?", MB_YESNO) != IDYES)
//		return;
//	SetEvent(m_hEventCancel);
	EndDialog(FALSE);
}


//				 
// GIBFinished() 
//
// called by the GIB read thread when GIB is finished
//
void CGIBDialog::GIBFinished() 
{
	// don't do anything if the user has already pressed 
	// the "Cancel" button 
	if (!MarkDialogFinished())
		return;

	// show the progress bar as finished and close
	m_progress.SetPos(m_nProgressUnits);
	m_progress.UpdateWindow();
	Sleep(200);	// make sure the progress display is seen as finished
	EndDialog(TRUE);
}


//
// MarkDialogFinished()
//
// called to mark the dialog as "finished" so that 
// no one will try to use it after either GIB has finished
// or the user has pressed "Cancel"
//
// this handles a potential simultaneous press of the 
// "Cancel" button and a "Finished" call from GIB
//
BOOL CGIBDialog::MarkDialogFinished() 
{
	BOOL bRtnCode = FALSE;
	//
	EnterCriticalSection(&m_csDialogFinished);
	{
		//
		if (!m_bFinished)
		{
			// now mark as finished
			m_bFinished = TRUE;
			bRtnCode = TRUE;
		}
	}
	LeaveCriticalSection(&m_csDialogFinished);
	// done
	return bRtnCode;
}




void CGIBDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	// mark that the dialog is now ready for external calls
	if (bShow)
		SetEvent(m_hDialogReady);
	//
	CDialog::OnShowWindow(bShow, nStatus);
}
