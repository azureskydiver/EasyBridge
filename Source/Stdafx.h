//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxmt.h>			// MFC multithreading support
#include <afxtempl.h>		// MFC templates
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#include <afxdisp.h>        // MFC OLE automation classes

// MFCXLib support
#define MFCX_PROJ
//#include <CJ60Lib.h>
#include <CJLibrary.h>

#pragma warning( disable : 4786) 
