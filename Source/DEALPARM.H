//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Default deal parameters file
//


// default min and max partnership point totals for various hands
int nDefReqPointsGame[4][2] = {
	{ 26, 32 },	// min, max for "any" game	
	{ 29, 32 },	// min, max for minor suit game	
	{ 26, 32 },	// min, max for major suit game	
	{ 26, 32 },	// min, max for notrump game	
};
// absolute limits
int nDefPointsAbsGameLimits[4][2] = {
	{ 20, 40 },	// min, max for "any" game	
	{ 20, 40 },	// min, max for minor suit game	
	{ 20, 40 },	// min, max for major suit game	
	{ 20, 40 },	// min, max for notrump game	
};

// now the same for slam hands
int nDefReqPointsSlam[3][2] = {
	{ 33, 38 },	// min, max for "any" slam game	
	{ 33, 36 },	// min, max for small slam game	
	{ 37, 38 },	// min, max for grand slam game	
};

int nDefPointsSlamLimits[3][2] = {
	{ 30, 40 },	// min, max for "any" slam game	
	{ 30, 40 },	// min, max for small slam game	
	{ 30, 40 },	// min, max for grand slam game	
};

BOOL bDefBalanceTeamHands = TRUE;

int nDefMinCardsInMajor = 8;
int nDefMinCardsInMinor = 8;

//
int nDefMinSuitDistTable[2][4][2] = {
	{ { 4, 3 }, { 4, 4 }, { 5, 3 }, { 5, 4 }, }, // minors
	{ { 4, 3 }, { 4, 4 }, { 5, 3 }, { 5, 4 }, }, // majors
};
int nDefMinSuitDist[2] =  { 1, 0 };	// 4/4 minors, 4/3 majors

//
int nDefMinTopMajorCard = KING;
int nDefMinTopMinorCard = KING;
//
int nDefMaxBalancedForNT = 2;
BOOL	bDefNeedTwoBalancedTrumpHands = TRUE;

//
int nDefNumAcesForSlam[3] = {
	3, 3, 4	
};
int nDefNumKingsForSlam[3] = {
	3, 3, 4	
};

