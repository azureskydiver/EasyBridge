//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// ConvCodes.h
//
#ifndef __CCONVCODES__
#define __CCONVCODES__


//
// Standard Bidding Convention Codes
//
enum bidconvcodes {
	//
	tnBiddingStyle,
	// unique convention IDs
	tid5CardMajors,
	tidArtificial2ClubConvention,
	tidWeakTwoBids,
	tidWeakJumpOvercalls,
	tidStrongTwoBids,
	tidShutoutBids,
	tid4thSuitForcing,
	tidOgust,
	tidBlackwood,
	tidRKCB,
	tidCueBids,
	tidGerber,
	tidStayman,
	tidJacobyTransfers,
	tidLimitRaises,
	tidTakeoutDoubles,
	tidNegativeDoubles,
	tidSplinterBids,
	tidMichaels,
	tidUnusualNT,
	tidJacoby2NT,
	tidGambling3NT,
	tidDrury,
	tidLebensohl,
	tidDONT,
	tidOvercalls,
	// generic "conventions"
	tidNoTrumpBids,
	tidNaturalBids,
	// convention mode switches & options
	tnJumpOvercallsMode,
	tb4SuitTransfers,
	tbStructuredReverses,
	tb3LevelTakeouts,
	// convention info
	tn1NTRange,
	tn2NTRange,
	tn3NTRange,
	tn2ClubOpenRange,		// index for pts
	tn2ClubOpeningPoints,
	tnGambling3NTVersion,
	tnAllowable1Openings,
	// misc info
	tn1NTRangeMinPts,
	tn1NTRangeMaxPts,
	tn2NTRangeMinPts,
	tn2NTRangeMaxPts,
	tn3NTRangeMinPts,
	tn3NTRangeMaxPts,
};


#endif

