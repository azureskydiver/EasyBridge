//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DialogInfo.h -- helpers structure
//
#ifndef __DIALOGINFO__
#define __DIALOGINFO__


//
typedef struct {
	int			nControlID;
	int			nIconID;
	LPTSTR		szToolTipText;
} DialogControlInfo;


//
typedef struct {
	int			nControlID;
	LPTSTR		szToolTipText;
} DialogToolTipInfo;



#endif