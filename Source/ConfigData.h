//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// ConfigData.h: interface for the CConfigData class.
//
// - used to hold data for the configuration wizard
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGDATA_H__6A673724_D715_11D2_9095_00609777FAF1__INCLUDED_)
#define AFX_CONFIGDATA_H__6A673724_D715_11D2_9095_00609777FAF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ConventionSet.h"


//
class CConfigData  
{
// operations
public:

private:


// data
public:

private:
	CConventionSet	m_conventionSet;


// construction/destruction
public:
	CConfigData();
	virtual ~CConfigData();

};


#endif // !defined(AFX_CONFIGDATA_H__6A673724_D715_11D2_9095_00609777FAF1__INCLUDED_)
