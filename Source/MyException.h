//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CMyException.h
//
#ifndef __MYEXCEPTION__
#define __MYEXCEPTION__


//
// class CMyExceptionclass 
//
// base class for EasyBridge exceptions
//
class CMyException
{
//
public:
	int GetErrorCode() { return m_nErrorCode; }
	LPCTSTR GetErrorDescription() { return m_strErrorDescription; }
	//
	CMyException(int nError, LPCTSTR szDescription) : m_nErrorCode(nError), m_strErrorDescription(szDescription) {}
	CMyException(int nError) : m_nErrorCode(nError) {}
	CMyException(LPCTSTR szDescription) : m_nErrorCode(0), m_strErrorDescription(szDescription) {}
	CMyException() : m_nErrorCode(0) {}
	~CMyException() {}

//
private:
	int		m_nErrorCode;
	CString m_strErrorDescription;
};



//
// class CGIBException
//
// will eventually extend this in some way
//
class CGIBException : public CMyException
{
//
public:
	CGIBException(int nError, LPCTSTR szDescription) : CMyException(nError, szDescription) {}
	CGIBException(int nError) : CMyException(nError) {}
	CGIBException(LPCTSTR szDescription) : CMyException(szDescription) {}
	CGIBException() : CMyException() {}

//
private:
};

#endif