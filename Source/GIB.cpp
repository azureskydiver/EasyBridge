//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// GIB.cpp : implementation of the CGIB class
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "Player.h"
#include "GIB.h"
#include "HandHoldings.h"
#include "progopts.h"
#include "mainfrm.h"
#include "GIBDialog.h"
#include "MyException.h"
//#include "GIBMonitorThread.h"


const int BUFSIZE = 4096;
const LPCTSTR tszGIBTempFilename = "GIBtemp.txt";

// static member variables
CRITICAL_SECTION		CGIB::m_csGIBPending;
BOOL					CGIB::m_bGIBPending = FALSE;

//
typedef struct {
	HANDLE		hReadHandle;
	CGIBDialog* pGIBDialog;
} GIBStruct;


/////////////////////////////////////////////////////////////////////////////
//
// CGIB
//
/////////////////////////////////////////////////////////////////////////////



//
// Invoke()
//
// invoke the GIB program and return the recommended play
//
int CGIB::Invoke(CPlayer* pPlayer, CHandHoldings* pHand, CHandHoldings* pDummyHand, CPlayerStatusDialog* pStatusDlg)
{
	SECURITY_ATTRIBUTES saAttr; 
	
	//
	// create the GIB monitor dialog
	//
	CGIBDialog	gibDialog(pMAINFRAME);
	int nProcessingTime = theApp.GetValue(tnGIBAnalysisTime);
	gibDialog.m_nProcessTime = nProcessingTime;
//	gibDialog.m_hEventCancel = m_hEventCancel;

	
	// Set the bInheritHandle flag so pipe handles are inherited. 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 
	
	//
	// create input and output pipes for the child process
	//

	// Create a pipe for the child process's STDOUT. 
	if (!CreatePipe(&m_hChildStdoutRd,	// returns the pipe's input handle
				    &m_hChildStdoutWr, 	// returns the pipe's output handle
					&saAttr, 
					0)) 
	{
		CString strError = "Stdout pipe creation failed\n"; 
		TRACE(strError); 
		pMAINFRAME->SetGIBMonitorText(strError);
		return ExitGracefully(-5);
	}
	// then create a pipe for the child process's STDIN. 
	if (!CreatePipe(&m_hChildStdinRd, 
					&m_hChildStdinWr, 
					&saAttr, 
					0)) 
	{
		CString strError = "Stdin pipe creation failed\n"; 
		TRACE(strError); 
		pMAINFRAME->SetGIBMonitorText(strError);
		return ExitGracefully(-5);
	}

	//
	// Now create the child process (GIB)
	//
	PROCESS_INFORMATION piProcInfo; 
	if (!LaunchProgram(piProcInfo)) 
	{
		TRACE("Create process failed"); 
		return ExitGracefully(-1);
	}
	HANDLE hGIBProcess = piProcInfo.hProcess;
	DWORD nGIBProcessID = piProcInfo.dwProcessId;

	// now close the readable handle to the child's stdin
	SafeCloseHandle(m_hChildStdinRd);
	// and the writable handle to the child's stdout
	SafeCloseHandle(m_hChildStdoutWr);

	//
	//------------------------------------------------------------------
	//
	// create the GIB input file
	//
	CFile file;
	CFileException fileException;
	CString strTempFile, strTempPath;
	GetTempPath(1024, strTempPath.GetBuffer(1024));
	strTempPath.ReleaseBuffer();
	GetTempFileName(strTempPath, "ezb", 0, strTempFile.GetBuffer(2048));
	strTempFile.ReleaseBuffer();
//	strTempFile.Format("%s\\%s", theApp.GetValueString(tszProgramDirectory), tszGIBTempFilename);
/*
	LPTSTR szBuffer = strTempFile.GetBuffer(MAX_PATH);
	GetTempFileName(theApp.GetValueString(tszProgramDirectory), "ezb", 0, szBuffer);
	strTempFile.ReleaseBuffer();
*/
//	CString strInput;
//	strInput.Format("-T %d %s\n",theApp.GetValue(tnGIBAnalysisTime),strTempFile);
	int nCode = file.Open(strTempFile, 
			  			  CFile::modeWrite | CFile::modeCreate | CFile::shareDenyWrite, 
						  &fileException);
	if (nCode == 0) 
	{
		CString strError = "Error opening temporary input file for GIB"; 
		TRACE(strError); 
		pMAINFRAME->SetGIBMonitorText(strError);
		return ExitGracefully(-2);
	}
	//
	CString strFileContents;
	CreateGIBInputFile(file, pPlayer, pHand, pDummyHand, strFileContents);
	file.Close();

	// then send the parameters line
	CString strParameters, strShortParameters;
	strParameters.Format("-T %d %s\n",nProcessingTime,strTempFile);
	strShortParameters.Format("-T %d",nProcessingTime);
	DWORD dwWritten;
	int nErrCode;
	if (!WriteFile(m_hChildStdinWr, (LPCTSTR)strParameters, strParameters.GetLength(), &dwWritten, NULL)) 
	{
		CString strError = "Error providing parameters to GIB"; 
		TRACE(strError); 
		pMAINFRAME->SetGIBMonitorText(strError);
		nErrCode = GetLastError();
		return ExitGracefully(-3);
	}

	//
	// update the GIB monitor window
	//
	CString strGIBText = "========================================\n";
	strGIBText += FormString("Launching %s %s\n",
							 theApp.GetValueString(tszGIBPath),
							 strShortParameters);
//	strGIBText += FormString("Input file contents:\n%s", strFileContents);
	strGIBText += "Awaiting Responses...\n";
	strGIBText += "----------------------------------------\n";
//	pMAINFRAME->SetGIBMonitorText(strGIBText);
	pMAINFRAME->AppendGIBMonitorText(strGIBText);
	

	//
	//------------------------------------------------------------
	//
	// now set up the wait loop and the cancel dialog,
	// then sit and wait for the process to run or for a cancel message
	//

/*
	//
	// create the "Cancel GIB" dialog thread
	// (this is a user interface thread)
	//
	CGIBMonitorThread* pMonitorThread = new CGIBMonitorThread(m_hEventFinished, m_hEventCancel, nProcessingTime);
	pMonitorThread->CreateThread(CREATE_SUSPENDED);
	pMonitorThread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	pMonitorThread->ResumeThread();

	// wait for the monitor thread to initialize
	DWORD nCode0 = WaitForSingleObject(m_hEventFinished,
									   INFINITE);		
*/

	//
	// create the wait thread
	// (this is a worker thread)
	//
	GIBStruct gibData;
	gibData.hReadHandle = m_hChildStdoutRd;
	gibData.pGIBDialog = &gibDialog;
	CWinThread* pWaitThread = AfxBeginThread(CGIB::ReadGIBOutput, &gibData, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);

	// copy its handle se that we can check its exit code later
  	HANDLE hWaitThread;
	BOOL bCode = ::DuplicateHandle(GetCurrentProcess(), pWaitThread->m_hThread, 
								   GetCurrentProcess(), &hWaitThread, 
								   0, FALSE, DUPLICATE_SAME_ACCESS);
	// and launch the threads
//	MonitorThread->ResumeThread();
	pWaitThread->ResumeThread();


	//
	// Show the Wait/Cancel dialog
	//
	m_bGIBPending = TRUE;		// mark dialog as active
	bCode = gibDialog.DoModal();

	// see if the user cancelled
	if (!bCode) 
	{
/*
		// lock out the wait thread and cancel operations
		if (ClearGIBPending())
		{
*/
		//
		pMAINFRAME->SetStatusText("GIB cancelled.");
		//
		TerminateProcess(hGIBProcess, 0);
		TerminateThread(hWaitThread, 0);
		// wait for the read thread to end
		WaitForSingleObject(hWaitThread, INFINITE);
		// close the wait thread handle
		CloseHandle(hWaitThread);
		CloseHandle(hGIBProcess);
		// and delete the thread object
		delete pWaitThread;
		// close pipe handles 
		SafeCloseHandle(m_hChildStdinWr);
		SafeCloseHandle(m_hChildStdoutRd);
		// and throw an exception
		throw CGIBException();
//		}
	}

/*
	// set up events
	HANDLE eventArray[2];
	eventArray[0] = m_hEventCancel;
	eventArray[1] = pWaitThread->m_hThread;

	//
	// then sit back and wait for the thread(s)
	//
	for(;;)
	{
		// wait for the cancelled or finished messages
		DWORD nCode = WaitForMultipleObjects(2,				// 2 events to wait for
											 eventArray,	// events array
											 FALSE,			// not all at once
											 INFINITE);		// wait 4-ever
		//
		if (nCode == WAIT_FAILED)
		{
			ASSERT(FALSE);
			break;
		}
		else if (nCode == WAIT_OBJECT_0) 
		{
			// got the cancel message, so kill GIB & the wait thread
			// the following is very dangersous --
			// so kids, don't try this at home
			TerminateThread(pWaitThread, 0);
			TerminateProcess(hGIBProcess, 0);
			return GIB_CANCEL;
		}
		else if (nCode == WAIT_OBJECT_0 + 1)
		{
			// GIB finished message
			// signal the GIB monitor that GIB has finished
			SetEvent(m_hEventFinished);
			break;
		}
	}

*/

	//
	//------------------------------------------------------------
	//
	// presumably, GIB has finished running
	//

	// wait for the GIB thread to exit, then get the card code
	DWORD nCardPlayed, nErrorCode;
	bCode = WaitForSingleObject(hWaitThread, INFINITE);
	bCode = GetExitCodeThread(hWaitThread, &nCardPlayed);
	if (!bCode)
		nErrorCode = GetLastError();

	// close the wait thread handle
	CloseHandle(hWaitThread);

	// delete the temporary file
	DeleteFile(strTempFile);
 
	// and kill the child process
	// first send a Ctrl-C to the app 
	// (this doesn't seem to do anything)
	CString strInput = "\03";	// Ctrl-C
	if (!WriteFile(m_hChildStdinWr, (LPCTSTR)strInput, strInput.GetLength(), &dwWritten, NULL)) 
	{
		CString strError = "Error stopping GIB"; 
		TRACE(strError); 
		pMAINFRAME->SetGIBMonitorText(strError);
		nErrCode = GetLastError();
		return ExitGracefully(-4);
	}

	// close the writable handle to the child's stdin
	SafeCloseHandle(m_hChildStdinWr);

	// then call terminateProcess
	TerminateProcess(hGIBProcess, 0);
	CloseHandle(hGIBProcess);

	// then close the readable handle to the child's stdout
	SafeCloseHandle(m_hChildStdoutRd);

	//
	// done
	//
	return nCardPlayed; 
} 
 


//
// LaunchProgram()
//
BOOL CGIB::LaunchProgram(PROCESS_INFORMATION& piProcInfo) const
{ 
	// get app path
	CString strPath = theApp.GetValueString(tszGIBPath);
	if (strPath.IsEmpty())
		return FALSE;	// error!

	// 
	// Set up members of STARTUPINFO structure. 
	//
	STARTUPINFO siStartInfo; 
 	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO); 
	siStartInfo.dwFlags = STARTF_USESTDHANDLES;
	siStartInfo.hStdInput = m_hChildStdinRd;
	siStartInfo.hStdOutput = m_hChildStdoutWr;
 
	// Create the child process. 
	DWORD dwFlags = DETACHED_PROCESS;		// hide the GIB window
    return CreateProcess(NULL, 
						 const_cast<char*>((LPCTSTR)strPath),   // command line 
						 NULL,				// process security attributes 
						 NULL,			    // primary thread security attributes 
						 TRUE,				// handles are inherited 
						 dwFlags,	        // creation flags 
						 NULL,		        // use parent's environment 
						 NULL,			    // use parent's current directory 
						 &siStartInfo,		// STARTUPINFO pointer 
						 &piProcInfo);		// receives PROCESS_INFORMATION 
}






//
// ReadGIBOutput() 
// 
// used as the GIB wait thread procecure
//
UINT CGIB::ReadGIBOutput(LPVOID pParam) 
{ 
	DWORD dwRead; 
	TCHAR chBuf[BUFSIZE+1]; 
	//
	GIBStruct* pGibData = (GIBStruct*) pParam;
	HANDLE hReadHandle = pGibData->hReadHandle;
	CString strOutput, strCard;
	int nCardPlayed = NONE;
 
	// Read output from the child process and save to the passed string
	for (;;) 
	{ 
		if (!ReadFile(hReadHandle, chBuf, BUFSIZE, &dwRead, NULL) || dwRead == 0) 
			break; 
		chBuf[dwRead] = '\0';
		strOutput = chBuf;
		// update monitor
		pMAINFRAME->AppendGIBMonitorText(strOutput);
		// check for output
		int nIndex = strOutput.Find("I play ");
		if (nIndex >= 0)
		{
			strCard = strOutput.Mid(nIndex+7,2);
			nCardPlayed = StringToDeckValue(strCard);
			break;
		}
		// check for errors
		if (strOutput.Find("Huh?") >= 0)
			break;
	} 

	//
//	if (ClearGIBPending())

	// wait till it's OK to call a method on the dialog
	pGibData->pGIBDialog->WaitForDialogInit();
	// then close it
	pGibData->pGIBDialog->GIBFinished();

	// sone
	return nCardPlayed;
} 

 


//
// ClearGIBPending()
//
// attempts to clear the "GIB Pending" flag and return
// returns FALSE upon failure (i.e., if the dialog was already closed)
//
BOOL CGIB::ClearGIBPending() 
{
	// wait for the critical section
	BOOL bRtnCode = FALSE;
	EnterCriticalSection(&m_csGIBPending);
	{
		// if the GIB pending flag is active, clear it
		if (m_bGIBPending)
		{
			// now clear it
			m_bGIBPending = FALSE;
			bRtnCode = TRUE;
		}
	}
	LeaveCriticalSection(&m_csGIBPending);

	// done
	return bRtnCode;
}





/////////////////////////////////////////////////////////////////////////////


//
BOOL CGIB::CreateGIBInputFile(CFile& file, CPlayer* pPlayer, CHandHoldings* pHand, CHandHoldings* pDummyHand, CString& strContents) const
{
	// format the input
	CString strInput;
	CEasyBDoc* pDoc = pDOC;

	// indicate player
	strInput.Format("%c\n",PositionToChar(pPlayer->GetPosition()));
	strContents += strInput;
	file.Write((LPCTSTR)strInput, strInput.GetLength());

	// indicate hand
	strInput = pHand->GetInitialHand().GetGIBFormatHoldingsString();
	strInput += '\n';
	strContents += strInput;
	file.Write((LPCTSTR)strInput, strInput.GetLength());

	// indicate dealer
	strInput.Format("%c\n",PositionToChar(pDoc->GetDealer()));
	strContents += strInput;
	file.Write((LPCTSTR)strInput, strInput.GetLength());

	// indicate vulnerability
	strInput = "n\n";	// TEMP
	strContents += strInput;
	file.Write((LPCTSTR)strInput, strInput.GetLength());

	// indicate auction
	strInput.Empty();
	int numBids = pDoc->GetNumBidsMade();
	for(int i=0;i<numBids;i++)
	{
		strInput += BidToBriefString(pDoc->GetBidByIndex(i));
		if (i < numBids)
			strInput += ' ';
	}
	strInput += '\n';
	strContents += strInput;
	file.Write((LPCTSTR)strInput, strInput.GetLength());

	// indicate opening lead
	int nLeadCard = pDoc->GetPlayRecord(0);
	strInput.Format("%s\n",CardToShortString(nLeadCard));
	strContents += strInput;
	file.Write((LPCTSTR)strInput, strInput.GetLength());
	
	// indicate dummy's (ORIGINAL) hand
	strInput = pDummyHand->GetInitialHand().GetGIBFormatHoldingsString();
	strInput += '\n';
	strContents += strInput;
	file.Write((LPCTSTR)strInput, strInput.GetLength());

	// and enter any plays so far
	strInput.Empty();
	int numCardsPlayed = pDoc->GetNumCardsPlayedInGame();
	for(i=1;i<numCardsPlayed;i++)
	{
		strInput += CardToShortString(pDoc->GetPlayRecord(i));
		strInput += ' ';
		if (((i % 3) == 0) || (i == numCardsPlayed-1))
		{
			// flush the line
			strInput += '\n';
			file.Write((LPCTSTR)strInput, strInput.GetLength());
			strInput.Empty();
		}
	}

	// end the file with a # sign
	strInput = "#\n";
	strContents += strInput;
	file.Write((LPCTSTR)strInput, strInput.GetLength());

	// done
	return TRUE;
}





//
inline void CGIB::SafeCloseHandle(HANDLE& handle)
{
	if (handle)
	{
		CloseHandle(handle);
		handle = NULL;		
	}
}

//
int CGIB::ExitGracefully(int nCode)
{
	SafeCloseHandle(m_hChildStdinRd);
//	SafeCloseHandle(m_hChildStdoutRdDup);
	SafeCloseHandle(m_hChildStdinWr);
	SafeCloseHandle(m_hChildStdoutWr);
//	SafeCloseHandle(m_hChildStdinWrDup);
	SafeCloseHandle(m_hChildStdoutRd);
	SafeCloseHandle(m_hSaveStdin);
	SafeCloseHandle(m_hSaveStdout);
	SafeCloseHandle(m_hInputFile);
	SafeCloseHandle(m_hOutputFile);

	// check exit code
	if (nCode)
	{
		throw CGIBException(nCode);
		return -1;	// to shut up the complier
	}
	else
	{
		return 0;
	}
}




/////////////////////////////////////////////////////////////////////////////
// construction/destruction
CGIB::CGIB()
{
	// clear all handles
	m_hChildStdinRd = m_hChildStdoutRdDup = m_hChildStdinWr = NULL;
	m_hChildStdoutWr = m_hChildStdinWrDup = m_hChildStdoutRd = NULL;
	m_hSaveStdin = m_hSaveStdout = NULL; 
	m_hInputFile = m_hOutputFile = NULL;
	// 
//	m_hEventCancel = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hEventFinished = CreateEvent(NULL, FALSE, FALSE, NULL);
	//
	InitializeCriticalSection(&m_csGIBPending);
}

CGIB::~CGIB()
{
	// 
//	CloseHandle(m_hEventCancel);
	CloseHandle(m_hEventFinished);
	DeleteCriticalSection(&m_csGIBPending);
}

