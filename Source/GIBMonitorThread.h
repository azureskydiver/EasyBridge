//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// GIBMonitorThread.h : header file
//

#ifndef __GIBMONITORTHREAD__
#define __GIBMONITORTHREAD__

#include "GIBDialog.h"
class CGIBDialog;

/////////////////////////////////////////////////////////////////////////////
// CGIBMonitorThread thread

class CGIBMonitorThread : public CWinThread
{
	DECLARE_DYNCREATE(CGIBMonitorThread)

public:
	CGIBMonitorThread(HANDLE m_hEventFinished, HANDLE m_hEventCancel, int nProcessingTime, CWnd* pParent);
	CGIBMonitorThread();
	virtual ~CGIBMonitorThread();

// operations
public:
	void Initialize();

protected:

// Attributes
public:

private:
	HANDLE	m_hEventFinished;
	HANDLE	m_hEventCancel;
	HANDLE	m_hEventUserCancel;
	int		m_nProcessingTime;
	//
	CWnd*			m_pParent;
	CGIBDialog*		m_pDialog;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGIBMonitorThread)
	public:
	virtual BOOL InitInstance();
	virtual int Run();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	// Generated message map functions
	//{{AFX_MSG(CGIBMonitorThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
