//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// FileCodePGN.h
//
// - contains codes for reading & writing PGN format files
//

// file identification tags, 1..16
const int	TAG_NONE			= 0;
const int	TAG_EVENT			= 1;
const int	TAG_SITE			= 2;
const int	TAG_DATE			= 3;
const int	TAG_ROUND	 		= 4;
const int	TAG_BOARD		 	= 5;
const int	TAG_WEST			= 6;
const int	TAG_NORTH			= 7;
const int	TAG_EAST		 	= 8;
const int	TAG_SOUTH		 	= 9;
const int	TAG_DEALER		 	= 10;
const int	TAG_VULNERABLE		= 11;
const int	TAG_DEAL			= 12;
const int	TAG_SCORING			= 13;
const int	TAG_DECLARER		= 14;
const int	TAG_CONTRACT		= 15;
const int	TAG_RESULT			= 16;

// auction tags, 20..
const int	TAG_AUCTION			= 20;

// played cards tags, 25..
const int	TAG_PLAY			= 25;

// misc tags, 30..
const int	TAG_NOTE			= 30;
const int	TAG_DESCRIPTION		= 31;
const int	TAG_GENERATOR		= 32;

// special tags
const int	TAG_TERMINATOR		= 39;

//
const int	NUM_TAGS		    = 40;


