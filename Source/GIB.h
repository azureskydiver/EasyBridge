//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
//
// GIB.h
//
// wrapper for the GIB executable
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __GIBWRAPPER__
#define __GIBWRAPPER__

class CPlayer;
class CHandHoldings;
class CPlayerStatusDialog;

const int GIB_CANCEL = -99;


class CGIB {

// public routines
public:
//	void	Initialize();
	int		Invoke(CPlayer* pPlayer, CHandHoldings* pHand, CHandHoldings* pDummyHand, CPlayerStatusDialog* pStatusDlg);
	static UINT ReadGIBOutput(LPVOID pParam);
	static BOOL ClearGIBPending();

// private routines
private:
	BOOL	CreateGIBInputFile(CFile& file, CPlayer* pPlayer, CHandHoldings* pHand, CHandHoldings* pDummyHand, CString& strContents) const;
	BOOL	LaunchProgram(PROCESS_INFORMATION& piProcInfo) const;
//	UINT	ReadGIBOutput(LPVOID pParam);
	void	SafeCloseHandle(HANDLE& handle);
	int		ExitGracefully(int nCode=0);

// private data
private:
	HANDLE	m_hChildStdinRd, m_hChildStdoutRdDup, m_hChildStdinWr;
	HANDLE	m_hChildStdoutWr, m_hChildStdinWrDup, m_hChildStdoutRd;
	HANDLE	m_hSaveStdin, m_hSaveStdout; 
	HANDLE	m_hInputFile, m_hOutputFile;
	HANDLE	m_hEventCancel, m_hEventFinished;
	static CRITICAL_SECTION	m_csGIBPending;
	static BOOL		m_bGIBPending;


// construction/destruction
public:
	CGIB();
	~CGIB();
};


#endif