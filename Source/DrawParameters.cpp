//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Draw Parameters.cpp
//
#include "stdafx.h"
#include "DrawParameters.h"


// Screen defaults

// playing card destination for tricks on table
// order: South, West, North, East
const POINT defCardDest[4] = {
   // south,         west,         north,         east
//   { 280, 210 }, {  190, 158 }, { 280,  110 }, { 372, 158 },
   { 282, 211 }, {  192, 158 }, { 282,  109 }, { 371, 158 },
};
const POINT smallCardDest[4] = {
   // south,         west,         north,         east
   { 208, 192 }, {  141, 150 }, { 208, 108 }, { 275, 150 },
};

// onscreen drawing coordinates (x,y origins) for each hand
POINT defDrawPoint[4] = {
   // south,        west,      north,       east
   { 84, 316 }, {  3, 29 }, { 154,  2 }, { 553, 29 },
};
POINT smallDrawPoint[4] = {
   // south,        west,      north,       east
   { 66, 332 }, {  1, 26 }, { 138,  2 }, { 566, 26 },
};


// offsets to draw area when displaying dummy
// order: South, West, North, East
const POINT defDummyDrawOffset[4] = {
    { 80, 0 }, { 0, 76 }, { 0, 0 }, { 0, 76 }
};
const POINT smallDummyDrawOffset[4] = {
    { 80, 0 }, { 0, 58 }, { 0, 0 }, { 0, 58 }
};
const POINT defDummyLabelOffset[4] = {
    { 32, -17 }, { 0, 74 }, { 0, 0 }, { 0, 74 }
};
const POINT smallDummyLabelOffset[4] = {
    { 32, -17 }, { 0, 74 }, { 0, 0 }, { 0, 74 }
};

// winning & losing tricks display coordinates
const POINT tricksDisplayDest[2] = {
	// winners      losers
	{ 568, 366 }, { 580, 378 },
};
const POINT smallTricksDisplayDest[2] = {
	// winners      losers
	{ 417, 338 }, { 429, 350 },
};

//
const SIZE winnerBitmapSize = { 32, 44 };
const SIZE loserBitmapSize = { 44, 32 };
//
const SIZE smallWinnerBitmapSize = { 28, 40 };
const SIZE smallLoserBitmapSize = { 40, 28 };

