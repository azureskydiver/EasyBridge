//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// DrawParameters.h
//

// display constants
const int STANDARD_X_GAP	= 18;  // gap between cards laid out horizontally
const int STANDARD_Y_GAP	= 18;  // gap between cards laid out vertically
const int OVERLAP_Y_OFFSET	= 36;  // vertical offset between east/west dummy suits
const int PLAYER_SPACER		= 5;   // gap between suits for horizontal (south) layout
const int DUMMY_SPACER	    = 10;  // gap between suits for vertical (north) layout

// small cards version
const int SMALL_X_GAP			 = 16;	// gap between cards laid out horizontally
const int SMALL_Y_GAP			 = 16;	// gap between cards laid out vertically
const int SMALL_OVERLAP_Y_OFFSET = 43;	// vertical offset between east/west dummy suits
const int SMALL_PLAYER_SPACER	 = 3;	// gap between suits for horizontal (south) layout
const int SMALL_DUMMY_SPACER	 = 10;	// gap between suits for vertical (north) layout

// east/west suit offsets
const int HORIZ_SUIT_OFFSET			= 8;
const int SMALL_HORIZ_SUIT_OFFSET	= 4;

// layout table location
const int LAYOUT_TOP	= 140;
const int LAYOUT_LEFT	= 100;
const int LAYOUT_X_GAP	= 28;
const int LAYOUT_Y_GAP	= 24;

// small cards version
const int SMALL_LAYOUT_TOP		= 140;
const int SMALL_LAYOUT_LEFT		= 100;
const int SMALL_LAYOUT_X_GAP	= 20;
const int SMALL_LAYOUT_Y_GAP	= 18;



//
extern const POINT defCardDest[];
extern const POINT smallCardDest[];
 
// 
extern POINT defDrawPoint[];
extern POINT smallDrawPoint[];

//
extern const POINT defDummyDrawOffset[];
extern const POINT smallDummyDrawOffset[];
extern const POINT defDummyLabelOffset[];
extern const POINT smallDummyLabelOffset[];

//
extern const POINT tricksDisplayDest[];
extern const POINT smallTricksDisplayDest[];

//
extern const SIZE winnerBitmapSize;
extern const SIZE loserBitmapSize;
extern const SIZE smallWinnerBitmapSize;
extern const SIZE smallLoserBitmapSize;
