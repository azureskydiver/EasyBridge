//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// GIBMonitorThread.cpp : implementation file
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBvw.h"
#include "mainfrm.h"
#include "GIBDialog.h"
#include "GIBMonitorThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CGIBMonitorThread

IMPLEMENT_DYNCREATE(CGIBMonitorThread, CWinThread)

//
BEGIN_MESSAGE_MAP(CGIBMonitorThread, CWinThread)
	//{{AFX_MSG_MAP(CGIBMonitorThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
CGIBMonitorThread::CGIBMonitorThread(HANDLE hEventFinished, HANDLE hEventCancel, int nProcessingTime, CWnd* pParent) :
			m_hEventFinished(hEventFinished),
			m_hEventCancel(hEventCancel),
			m_nProcessingTime(nProcessingTime),
			m_pParent(pParent)
{
	m_hEventUserCancel = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CGIBMonitorThread::CGIBMonitorThread()
{
}

CGIBMonitorThread::~CGIBMonitorThread()
{
	CloseHandle(m_hEventUserCancel);
}


//
/////////////////////////////////////////////////////////////////////////
//


//
// InitInstance()
//
// 
//
BOOL CGIBMonitorThread::InitInstance()
{
	CWnd* pParent = CWnd::FromHandle(m_pParent->m_hWnd);
	m_pDialog = new CGIBDialog;
	m_pDialog->Create(CGIBDialog::IDD, pParent);
	m_pMainWnd = m_pDialog;
//	m_pDialog->m_hEventCancel = m_hEventUserCancel;
	m_pDialog->m_nProcessTime = m_nProcessingTime;
	m_pDialog->CenterWindow();
	m_pDialog->ShowWindow(SW_SHOW);
	m_pDialog->UpdateWindow();
	//
/*
	CWnd* pParent = CWnd::FromHandle(m_pParent->m_hWnd);
	m_gibDlg.Create(CGIBDialog::IDD, pParent);
	m_pMainWnd = &m_gibDlg;
	m_gibDlg.m_hEventCancel = m_hEventUserCancel;
	m_gibDlg.m_nProcessTime = m_nProcessingTime;
	m_gibDlg.CenterWindow();
	m_gibDlg.ShowWindow(SW_SHOW);
	m_gibDlg.UpdateWindow();
*/
/*
	//
	CGIBDialog gibDlg;
	gibDlg.Create(CGIBDialog::IDD, NULL);
	gibDlg.m_hEventCancel = m_hEventUserCancel;
	gibDlg.m_nProcessTime = m_nProcessingTime;
	gibDlg.CenterWindow();
	gibDlg.ShowWindow(SW_SHOW);
	gibDlg.UpdateWindow();

	// set up events array
	HANDLE eventArray[2];
	eventArray[0] = m_hEventFinished;
	eventArray[1] = m_hEventUserCancel;
	//
	int nWaitInterval = 5000;		// set timeouts to 5 seconds

	// loop and wait for one of two things:
	// 1) the user to press "Cancel", or
	// 2) a signal from the controlling function that GIB has finished,
	//    and that the dialog should close
	// periodically, this routine will time out and update the dialog display
	//
	for(;;)
	{
		// wait for the draw or kill messages
		DWORD nCode = WaitForMultipleObjects(2,					// 2 events to wait for
											 eventArray,		// events array
											 FALSE,				// not all at once
											 nWaitInterval);	// 
		//
		if (nCode == WAIT_FAILED)
		{
			ASSERT(FALSE);
			break;
		}
		else if (nCode == WAIT_OBJECT_0) 
		{
			// GIB is finished
			break;
		}
		else if (nCode == WAIT_OBJECT_0 + 1)
		{
			// User pressed the "Cancel" button
			SetEvent(m_hEventCancel);
			break;
		}
		else if (nCode == WAIT_TIMEOUT)
		{
			// update display...
		}
	}

	// avoid entering standard message loop by returning FALSE
	return FALSE;
*/
	return TRUE;
}


//
int CGIBMonitorThread::ExitInstance() 
{
	delete m_pDialog;
	//
	return CWinThread::ExitInstance();
}




/////////////////////////////////////////////////////////////////////////////
//
// CGIBMonitorThread message handlers
//
// Note: these functions are called from oustide the thread,
//       and not from this thread itself.
//
/////////////////////////////////////////////////////////////////////////////






/////////////////////////////////////////////////////////////////////////////
//
// CGIBMonitorThread internal routines
//
/////////////////////////////////////////////////////////////////////////////







//
// Initialize() 
//
// 
//
void CGIBMonitorThread::Initialize() 
{
/*
	//
	m_gibDlg.Create(CGIBDialog::IDD, pMAINFRAME);
	m_gibDlg.m_hEventCancel = m_hEventUserCancel;
	m_gibDlg.m_nProcessTime = m_nProcessingTime;
	m_gibDlg.CenterWindow();
	m_gibDlg.ShowWindow(SW_SHOW);
	m_gibDlg.UpdateWindow();

	// #### TEMP ####
	// inform main thread that we've finished initializing
	SetEvent(m_hEventFinished);
*/
}





//
// Run() 
//
// overridden thread function
//
int CGIBMonitorThread::Run() 
{
/*
 * for some reason, the following Create() function causes the thread
 * to hang until GIB finished running
 * figure this out later
 *
	//
	CGIBDialog gibDlg;
	gibDlg.Create(CGIBDialog::IDD, m_pParent);
	gibDlg.m_hEventCancel = m_hEventUserCancel;
	gibDlg.m_nProcessTime = m_nProcessingTime;
	gibDlg.CenterWindow();
	gibDlg.ShowWindow(SW_SHOW);
	gibDlg.UpdateWindow();

	// #### TEMP ####
	// inform main thread that we've finished initializing
	SetEvent(m_hEventFinished);

	m_gibDlg.DoModal();
	SetEvent(m_hEventCancel);
	return 0;
*/

/*
	// set up events array
	HANDLE eventArray[2];
	eventArray[0] = m_hEventFinished;
	eventArray[1] = m_hEventUserCancel;
	//
	int nWaitInterval = 5000;		// set timeouts to 5 seconds

	// loop and wait for one of two things:
	// 1) the user to press "Cancel", or
	// 2) a signal from the controlling function that GIB has finished,
	//    and that the dialog should close
	// periodically, this routine will time out and update the dialog display
	//
	for(;;)
	{
		// wait for the draw or kill messages
		DWORD nCode = WaitForMultipleObjects(2,					// 2 events to wait for
											 eventArray,		// events array
											 FALSE,				// not all at once
											 nWaitInterval);	// 
		//
		if (nCode == WAIT_FAILED)
		{
			ASSERT(FALSE);
			break;
		}
		else if (nCode == WAIT_OBJECT_0) 
		{
			// GIB is finished
			break;
		}
		else if (nCode == WAIT_OBJECT_0 + 1)
		{
			// User pressed the "Cancel" button
			SetEvent(m_hEventCancel);
			break;
		}
		else if (nCode == WAIT_TIMEOUT)
		{
			// update display...
		}
	}
	// done
	return TRUE;
	
*/	
/*
	m_gibDlg.Create(CGIBDialog::IDD, pMAINFRAME);
	m_gibDlg.m_hEventCancel = m_hEventUserCancel;
	m_gibDlg.m_nProcessTime = m_nProcessingTime;
	m_gibDlg.CenterWindow();
	m_gibDlg.ShowWindow(SW_SHOW);
	m_gibDlg.UpdateWindow();
*/

	// TODO: Add your specialized code here and/or call the base class
	return CWinThread::Run();
}



