//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CPlayerStatusDialog
//
// wrapper around the CAnalysisDialog class
//

#ifndef __CPLAYERSTATUSDLG__
#define __CPLAYERSTATUSDLG__


//
class CPlayerStatusDialog {
//
public:
	void Clear();
	void ClearHint();
	void Show(BOOL bShow=TRUE);
	void EnableTrace(BOOL bEnable=TRUE);
	CPlayerStatusDialog& operator<<(const CString& strText);
	CPlayerStatusDialog& operator<<(char cText);
	CPlayerStatusDialog& operator<<(int nValue);
	CPlayerStatusDialog& operator<<(double fValue);
	//
	CPlayerStatusDialog& operator<(const CString& strText);
	CPlayerStatusDialog& operator<(char cText);
	CPlayerStatusDialog& operator<(int nValue);
	CPlayerStatusDialog& operator<(double fValue);
	CPlayerStatusDialog& operator&(const CString& strText);
	CPlayerStatusDialog& operator&(char cText);
	CPlayerStatusDialog& operator&(int nValue);
	CPlayerStatusDialog& operator&(double fValue);
	//
	void SetPosition(Position nPos) { m_nPosition = nPos; }
	LPCTSTR GetText() { return (LPCTSTR) m_strAnalysis; }

	// routines to temporarily assign a verbosity level to a stream
	void SetStreamOutputLevel(int nLevel) { m_nOverrideOutputLevel = nLevel; }
	void ClearStreamOutputLevel() { m_nOverrideOutputLevel = 0; }
	//
	void SetRequiredHintTraceLevel(int nLevel) { m_nRequiredHintTraceLevel = nLevel; } 
	void ResetRequiredHintTraceLevel();
	static void SuspendHintOutput() { m_nSuspendHints++; }
	static void ResumeHintOutput() { if (--m_nSuspendHints < 0) m_nSuspendHints = 0; }

	// Auto Hint routines
	void SetAutoHintMode(BOOL bMode) { m_bAutoHintMode = bMode; }
	void BeginHintBlock() { /*m_strHintBlock = _T("");*/ m_bCopyHintBlock = TRUE;  }
	void EndHintBlock() { m_bCopyHintBlock = FALSE; }
	void ClearHints(BOOL bRefresh=FALSE);
	//
	static int  GetHintTraceLevel() { return m_nRequiredHintTraceLevel; }
	static void SetHintTraceLevel(int nLevel) { m_nRequiredHintTraceLevel = nLevel; }


//
private:
	void OutputText();
	void Trace(const CString& strText);
	void AddTrace(const CString& strText);
	BOOL ConvertNewlines(CString& strOutput, LPCTSTR szInput);

// data
private:
	Position	m_nPosition;
	CString		m_strAnalysis;
	int			m_nCurrentOutputLevel;
	int			m_nOverrideOutputLevel;
	int			m_nSuppressCode;
	BOOL		m_bSuppressOutput;
	BOOL		m_bSuppressStream;
	BOOL		m_bIgnoreStreamLevel;

	// hint info is per-class
	static BOOL		m_bAutoHintMode;
	static BOOL		m_bCopyHintBlock;
	static int		m_nRequiredHintTraceLevel;
	static int		m_nSuspendHints;
	static CString	m_strHintBlock;

// construction/destruction 
public:
	CPlayerStatusDialog();
	~CPlayerStatusDialog();
};

#endif
