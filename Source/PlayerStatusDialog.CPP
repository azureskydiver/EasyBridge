//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//===========================================================
//
// CPlayerStatusDialog
//
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "mainfrm.h"
#include "MainFrameOpts.h"
#include "progopts.h"
#include "PlayerStatusDialog.h"
#include "AutoHintDialog.h"

//#define _STRIP_HEADERS_

//#define _TESTING


//
//const int tnDefaultTraceLevel = 3;
const int tnDefaultTraceLevel = 1;

//const int tnDefaultRequiredHintTraceLevel = 1;
const int tnDefaultRequiredHintTraceLevel = 3;

//
BOOL	CPlayerStatusDialog::m_bAutoHintMode = FALSE;
BOOL	CPlayerStatusDialog::m_bCopyHintBlock = FALSE;
int		CPlayerStatusDialog::m_nRequiredHintTraceLevel = tnDefaultRequiredHintTraceLevel;
int		CPlayerStatusDialog::m_nSuspendHints = 0;
CString	CPlayerStatusDialog::m_strHintBlock;


// constructor
CPlayerStatusDialog::CPlayerStatusDialog()
{
	m_nOverrideOutputLevel = 0;
	m_nSuppressCode = 1;
	m_bSuppressOutput = FALSE;
	m_bSuppressStream = FALSE;
	m_bIgnoreStreamLevel = FALSE;
	//
/*
	m_bAutoHintMode = FALSE;
	m_bCopyHintBlock = FALSE;
	m_nRequiredHintTraceLevel = tnDefaultRequiredHintTraceLevel;
*/
}

// destructor
CPlayerStatusDialog::~CPlayerStatusDialog()
{
}


//
void CPlayerStatusDialog::EnableTrace(BOOL bEnable) 
{ 
	// now semaphore based
	if (bEnable)
		m_nSuppressCode++;
	else
		m_nSuppressCode--;
//	if (m_nSuppressCode < 0)
//		m_nSuppressCode = 0;
	
	//
	if (m_nSuppressCode == 0)
		m_bSuppressOutput = TRUE; 
	else
		m_bSuppressOutput = FALSE; 
}


//
void CPlayerStatusDialog::OutputText()
{
	// show all text in the mainframe status window
	pMAINFRAME->SetAnalysisText(m_nPosition, m_strAnalysis);
}


//
void CPlayerStatusDialog::Clear()
{
	m_strAnalysis.Empty();
	m_bAutoHintMode = FALSE;
	m_nCurrentOutputLevel = 0;
	m_nOverrideOutputLevel = 0;
	//
	m_bCopyHintBlock = FALSE;
	m_strHintBlock.Empty();
	//
	OutputText();
}


//
void CPlayerStatusDialog::ClearHint()
{
	m_bCopyHintBlock = FALSE;
	m_strHintBlock.Empty();
	CAutoHintDialog* pHintDlg = (CAutoHintDialog*) pMAINFRAME->GetDialog(twAutoHintDialog);
	pHintDlg->SetHintText(m_strHintBlock);
}


//
void CPlayerStatusDialog::ResetRequiredHintTraceLevel()
{
	m_nRequiredHintTraceLevel = tnDefaultRequiredHintTraceLevel;
}

//
void CPlayerStatusDialog::Show(BOOL bShow)
{
	if (bShow)
		pMAINFRAME->MakeDialogVisible(twAnalysisDialog, m_nPosition);
	else
		pMAINFRAME->HideDialog(twAnalysisDialog, m_nPosition);
}


//
//==========================================================
//
// operator << is the same as Trace()
//
// outputs a complete line to the player status dialog
//
CPlayerStatusDialog& CPlayerStatusDialog::operator<<(const CString& strText)
{
	Trace(strText);
	return *this;
}
CPlayerStatusDialog& CPlayerStatusDialog::operator<<(char cText)
{
	CString strText = cText;
	Trace(strText);
	return *this;
}
CPlayerStatusDialog& CPlayerStatusDialog::operator<<(int nValue)
{
	CString strTemp;
	strTemp.Format("%d",nValue);
	Trace(strTemp);
	return *this;
}
CPlayerStatusDialog& CPlayerStatusDialog::operator<<(double fValue)
{
	CString strTemp;
	strTemp.Format("%.1f",fValue);
	Trace(strTemp);
	return *this;
}


//
void CPlayerStatusDialog::Trace(const CString& strText)
{
	// sanity check
	CString strOutput = strText;
	if (strOutput.IsEmpty() || m_bSuppressStream)
		return;

	// no analysis if in express autoplay or output is suppressed
#ifdef _TESTING
	if (m_bSuppressOutput)
		return;
#else
	if (theApp.InExpressAutoPlay() || m_bSuppressOutput)
		return;
#endif

	// need to be either printing analysis or providing a hint
	if (!theApp.GetValue(tbEnableAnalysisTracing) && !m_bCopyHintBlock)
		return;

	// get and test the trace level
	int nTraceLevelLimit = theApp.GetValue(tnAnalysisTraceLevel);
	m_nCurrentOutputLevel = 0;

	// see if a verbosity escape code is present
	if (strOutput[0] == '!')
	{
		m_bIgnoreStreamLevel = TRUE;
		strOutput = strOutput.Mid(1);
	}
	else
	{
		m_bIgnoreStreamLevel = FALSE;
	}

	// see if a verbosity level code is present
	if (isdigit(strOutput[0]))
		m_nCurrentOutputLevel = strOutput[0] - '0';
	else 
		m_nCurrentOutputLevel = tnDefaultTraceLevel;

	// look for an autohint bypass (not so informative information)
	BOOL bSkipAutoHint = FALSE;
	if (strOutput[0] == _T('$'))
	{
		bSkipAutoHint = TRUE;
		strOutput = strOutput.Mid(1);
	}

	// check for temporary stream level override
	if ((m_nOverrideOutputLevel > 0) && (!m_bIgnoreStreamLevel))
		m_nCurrentOutputLevel = m_nOverrideOutputLevel;

	// skip if the verbosity in the stream is too high
	if (m_nCurrentOutputLevel > nTraceLevelLimit)
	{
		// if this string doesn't contain a CR, suppress remainder of output stream
		if (strOutput.Find('\n') < 0)
			m_bSuppressStream = TRUE;
		return;
	}

	// strip the verbosity level digit
	if (isdigit(strOutput[0]))
		strOutput = strOutput.Mid(1);

	// convert newlines
	CString strNewText;
	BOOL bOutputText = ConvertNewlines(strNewText, strOutput);

	// show analysis notes
	if (theApp.GetValue(tbEnableAnalysisTracing))
	{
		m_strAnalysis += strNewText;
		if (bOutputText)
			OutputText();
	}

	// and show hints
//	if (m_bAutoHintMode && m_bCopyHintBlock && !bSkipAutoHint && (m_nSuspendHints == 0) &&
	if (m_bCopyHintBlock && !bSkipAutoHint && (m_nSuspendHints == 0) &&
						(m_nCurrentOutputLevel <= m_nRequiredHintTraceLevel))
	{
		// route to the autohint window
		m_strHintBlock += strNewText;
		CAutoHintDialog* pHintDlg = (CAutoHintDialog*) pMAINFRAME->GetDialog(twAutoHintDialog);
		pHintDlg->SetHintText(m_strHintBlock);
	}
}


//
// AddTrace()
//
// - adds text to the status dialog text buffer.  The window is
// updated with the new text only if there's a newline character 
// in the text.
//
void CPlayerStatusDialog::AddTrace(const CString& strText)
{
	// no analysis if in express autoplay or output is suppressed
#ifdef _TESTING
	if (m_bSuppressOutput)
		return;
#else
	if (theApp.InExpressAutoPlay() || m_bSuppressOutput)
		return;
#endif

	// need to be either printing analysis or providing a hint
	if (!theApp.GetValue(tbEnableAnalysisTracing) && !m_bCopyHintBlock)
		return;

	//
	CString strNewText;
	BOOL bOutputText = ConvertNewlines(strNewText, strText);
	if (m_bSuppressStream)
	{
		if (bOutputText)
			m_bSuppressStream = FALSE;	// reset
	}
	else
	{
		// show analysis notes if enabled
		if (theApp.GetValue(tbEnableAnalysisTracing))
		{
			m_strAnalysis += strNewText;
			if (bOutputText)
				OutputText();
		}

		// and update hint window
//		if (m_bAutoHintMode && m_bCopyHintBlock && (m_nSuspendHints == 0) &&
		if (m_bCopyHintBlock && (m_nSuspendHints == 0) &&
						(m_nCurrentOutputLevel <= m_nRequiredHintTraceLevel))
		{
			// route to the autohint window
			m_strHintBlock += strNewText;
			CAutoHintDialog* pHintDlg = (CAutoHintDialog*) pMAINFRAME->GetDialog(twAutoHintDialog);
			pHintDlg->SetHintText(m_strHintBlock);
		}
	}
}




//
// ClearHints()
//
void CPlayerStatusDialog::ClearHints(BOOL bRefresh)
{
	m_strHintBlock = _T("");
//	if (bRefresh)
	{
		CAutoHintDialog* pHintDlg = (CAutoHintDialog*) pMAINFRAME->GetDialog(twAutoHintDialog);
		pHintDlg->SetHintText(m_strHintBlock);
	}
}




//
//================================================================
//
// the operators < and & are the same as <<, but without 
// the check for the debug mode / header stripping
//
CPlayerStatusDialog& CPlayerStatusDialog::operator<(const CString& strText)
{
	AddTrace(strText);
	return *this;
}
CPlayerStatusDialog& CPlayerStatusDialog::operator<(char cText)
{
	CString strText = cText;
	AddTrace(strText);
	return *this;
}
CPlayerStatusDialog& CPlayerStatusDialog::operator<(int nValue)
{
	CString strTemp;
	strTemp.Format("%d",nValue);
	AddTrace(strTemp);
	return *this;
}
CPlayerStatusDialog& CPlayerStatusDialog::operator<(double fValue)
{
	CString strTemp;
	strTemp.Format("%.1f",fValue);
	AddTrace(strTemp);
	return *this;
}
CPlayerStatusDialog& CPlayerStatusDialog::operator&(const CString& strText)
{
	AddTrace(strText);
	return *this;
}
CPlayerStatusDialog& CPlayerStatusDialog::operator&(char cText)
{
	CString strText = cText;
	AddTrace(strText);
	return *this;
}
CPlayerStatusDialog& CPlayerStatusDialog::operator&(int nValue)
{
	CString strTemp;
	strTemp.Format("%d",nValue);
	AddTrace(strTemp);
	return *this;
}
CPlayerStatusDialog& CPlayerStatusDialog::operator&(double fValue)
{
	CString strTemp;
	strTemp.Format("%.1f",fValue);
	AddTrace(strTemp);
	return *this;
}



//
// ConvertNewlines()
//
// - replaces all instances of the '\n' character in the source string
//   with the "\r\n" pair.
//
BOOL CPlayerStatusDialog::ConvertNewlines(CString& strOutput, LPCTSTR szInput)
{
	int nSrcLen = _tcslen(szInput);

/*
	// first bang characters
	TCHAR* pcToken = NULL;
	if ((pcToken = _tcschr(szInput, _T('!'))) != NULL)
	{
		// chop off the header
		int nOffset = (pcToken - szInput) + 2;
		if (nSrcLen > nOffset)
		{
			szInput += nOffset;
			nSrcLen -= nOffset;
		}
	}
*/
	// if in debug, strip header if desired
	if (!theApp.GetValue(tbShowCommentIdentifiers))
	{
		TCHAR* pcToken = NULL;
		if ((pcToken = _tcschr(szInput, _T('!'))) != NULL)
		{
			// chop off the header
			int nOffset = (pcToken - szInput) + 2;
			if (nSrcLen >= nOffset)
			{
				szInput += nOffset;
				nSrcLen -= nOffset;
			}
		}
	}

	//	
	int nActualLen = nSrcLen;
	TCHAR tChar;
	LPCTSTR pSrc = szInput;
	LPTSTR pDest = strOutput.GetBuffer(nSrcLen*2);	// max possible length
	BOOL bNewlineFound = FALSE;
	// 
	for(int i=0;i<nSrcLen;i++)
	{
		//
		if ((tChar = *pSrc++) == '\n')
		{
			// perform the substitution
			*pDest++ = '\r';
			*pDest++ = '\n';
			nActualLen++;
			bNewlineFound = TRUE;
		}
		else
		{
			// replace any exclamation points
			if (tChar == _T('!'))
				*pDest++ = _T(':');
			else
				*pDest++ = tChar;
		}
	}
	strOutput.ReleaseBuffer(nActualLen);
	return bNewlineFound;
}
