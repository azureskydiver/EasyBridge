//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Docopts.h
//
// constants used in generic document property get/set calls
//

enum docopts {
	//
	tppPlayer,				// [4]	
	tstrFileProgTitle,
	tnFileProgramMajorVersion,
	tnFileProgramMinorVersion,
	tnFileProgramIncrementVersion,
	tnFileProgramBuildNumber,
	tstrFileProgramBuildStatus,
	tstrFileProgramBuildDate,
	tstrFileDate,
	tstrFileDescription,
	tstrFileComments,
	tstrDocTitle,
	// match info
	tbReviewingGame,
	tbGameReviewAvailable,
	tnGameScore,			// [30][2]
	tnCurrGame,
	tnBonusScore,			// [2]
	tnTotalScore,			// [2]
	tnumGamesWon,
	tnVulnerableTeam,
	tbVulnerable,			// [2]
	tstrTrickPointsRecord,
	tstrBonusPointsRecord,
	tstrTotalPointsRecord,
	// game (contract) info
	tnContract,
	tnContractLevel,
	tnContractSuit,
	tnContractTeam,
	tnDefendingTeam,
	tnBiddingStartPos,
	tnumBidsMade,
	tnumValidBidsMade,
	tnOpeningBid,
	tnOpeningBidder,
	tnBiddingRound,
	tbDoubled,	
	tnDoubler,	
	tbRedoubled,
	tnRedoubler,
	tnContractModifier,
	tnLastValidBid,
	tnLastValidBidTeam,
	tnPartnershipSuit,	// [2]
	tnPartnershipLead,	// [2][5]
	tnumPasses,
	tnBiddingHistory,	// [100]
	tnValidBidHistory,
	tnBidsByPlayer,		// [4][50]
	// hand info
	tnDealer,
	tnCurrentPlayer,
	tnPreviousDealer,
	tnDeclarer,
	tnDummy,
	tbExposeDummy,
	tnTrumpSuit,
	tnGameLead,
	tnRoundLead,
	tnPlayRecord,		// [52]
	tpcGameTrick,		// [13][4]
	tnTrickLead,		// [13]
	tnTrickWinner,		// [13]
	tpcCurrentTrick,	// [4] 
	tnumTricksPlayed,
	tnumActualTricksPlayed,
	tnumCardsPlayedInRound,
	tnumCardsPlayedInGame,
	tnumTricksWon,		// [2]
	//
	tnDealNumber,
	tnSpecialDealCode,
	tbDealNumberAvailable,
	// results info
	tnSuitLed,
	tnHighVal,
	tnHighTrumpVal,
	tpvHighCard,
	tnHighPos,
	tnRoundWinner,
	tnRoundWinningTeam,
	// flags
	tbHandsDealt,
	tbShowCommentsUponOpen,
	tbShowBidHistoryUponOpen,
	tbShowPlayHistoryUponOpen,
	tbShowAnalysesUponOpen,
	tbSavePlayerAnalysis,
	tbDocSaveIntermediatePositions,
	// other stuff
	tnCardBack,
	tbExpressPlayMode,
	tbAutoReplayMode,
};
