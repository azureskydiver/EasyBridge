//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CCardLocation
//
#include "stdafx.h"
#include "EasyB.h"
#include "CardLocation.h"



//
// construction / destruction
//
CCardLocation::CCardLocation() 
{ 
}

CCardLocation::~CCardLocation()
{
}






//========================================================================
//
//
// Utils
//
//
//========================================================================

	

//
// Clear() 
//
void CCardLocation::Clear() 
{
	for(int i=0;i<52;i++) m_cards[i].Clear(); 
}

