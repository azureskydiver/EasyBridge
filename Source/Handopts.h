//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Handopts.h
//

enum handopts {
	// card info
	tInitialHand=2000,
	tHand,		
	tDisplayHand,
	tstrHoldings,		
	tstrScreenHoldings,	
	tbHandSorted,
	tbExposeCards,
	tnumCardsHeld,
	tSuitHoldings,
	tnumCardsInSuit,	
	// basic hand statistics
	tnumAcesHeld,	
	tnumKingsHeld,	
	tnumQueensHeld,	
	tnumJacksHeld,	
	tnumTensHeld,	
	tnumHighCardPoints,
	tnumSuitPoints,	// [4]
	tnumShortPoints,
	tnumLongPoints,
	tnumBonusPoints,
	tnumPenaltyPoints,
	tnumTotalPoints,
	tnumAdjustedPoints,
	tnBalanceValue,
	// more detailed hand analysis
	tfQuickTricks,	
	tnumSuitHonors,		// [5]
	tbSuitStopped,		// [4]
	tbSuitProbStopped,	// [4]
	tnumSuitsStopped,		
	tnumSuitsProbStopped,	
	tnumStoppersInSuit,	// [4]
	tnumSuitsUnstopped,
	tnStoppedSuits,		// [4]
	tnProbStoppedSuits,	// [4]
	tnUnstoppedSuits,	// [4]
	tstrSuitsStopped,
	tstrSuitsUnstopped,
	//
	tnumRebiddableSuits,
	tbSuitIsRebiddable,	// [4]
	//
	tnSuitStrength,		// [5] 
	tbSuitIsMarginal,	// [5] 
	tbSuitIsOpenable,	// [5]  
	tbSuitIsStrong,		// [5]  
	tbSuitIsPreferred,	// [5]  
	tbSuitIsAbsolute,	// [5]  
	tbSuitIsSolid,		// [5]  
	tnumMarginalSuits,
	tnumOpenableSuits,
	tnumStrongSuits,
	tnumPreferredSuits,
	tnumAbsoluteSuits,
	tnumSolidSuits,
	tnLowestMarginalSuit,
	tnLowestOpenableSuit,	
	tnLowestStrongSuit,
	tnLowestPreferredSuit,
	tnLowestAbsoluteSuit,
	tnLowestSolidSuit,
	tnMarginalSuitList, 	// [5]
	tnOpenableSuitList,		// [5]
	tnStrongSuitList,		// [5]
	tnPreferredSuitList,	// [5]
	tnAbsoluteSuitList,		// [5]
	tnSolidSuitList,		// [5]
	tnHighestMarginalSuit,
	tnHighestOpenableSuit,
	tnHighestStrongSuit,
	tnHighestPreferredSuit,
	tnHighestAbsoluteSuit,
	tnHighestSolidSuit,
	tnPreferredSuit,	
	tnSuitsByPreference,	// [4]
	tnSuitRank,				// [4]
	tnumVoidSuits,
	tnVoidSuits,			// [4]
	tnumSingletonSuits,
	tnSingletonSuits,		// [4]
	tnumDoubletonSuits,
	tnDoubletonSuits,		// [4]
	tnSuitsOfAtLeast,		// [14]
	tnumLikelyWinners,
	tnumLikelyLosers,
	tnumWinners,
	tnumLosers,
	tnumLikelySuitWinners,	// [4]
	tnumLikelySuitLosers,	// [4]
	tnumSuitWinners,		// [4]
	tnumSuitLosers,			// [4]
};
