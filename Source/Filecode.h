//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Filecode.h
//
// - contains codes for reading & writing EasyBridge files
//

// data blocks
const int	BLOCK_UNKNOWN			= 0;
const int	BLOCK_FILEINFO			= 1; 
const int	BLOCK_FILEDESC			= 2;
const int	BLOCK_HANDINFO			= 3;
const int	BLOCK_ROUNDINFO			= 4;
const int	BLOCK_GAMEINFO			= 5;
const int	BLOCK_GAMERECORD		= 6;
const int	BLOCK_MATCHINFO			= 7;
const int	BLOCK_MISCINFO			= 8;
const int	BLOCK_COMMENTS			= 9;
const int	BLOCK_PLAYER_ANALYSIS	= 10;
const int	BLOCK_SOUTH_ANALYSIS	= 10;
const int	BLOCK_WEST_ANALYSIS		= 11;
const int	BLOCK_NORTH_ANALYSIS	= 12;
const int	BLOCK_EAST_ANALYSIS		= 13;
const int	NUM_BLOCKS				= 14;

extern const char* tszBlockName[];


// file identification items, 0..9
const int	ITEM_NONE					= 0;
const int	ITEM_UNKNOWN			 	= 0;
const int	ITEM_PROGRAM_ID				= 1;
const int	ITEM_MAJOR_VERSIONNO	 	= 2;
const int	ITEM_MINOR_VERSIONNO	 	= 3;
const int	ITEM_INCREMENT_VERSIONNO	= 4;
const int	ITEM_BUILD_NUMBER			= 5;
const int	ITEM_BUILD_DATE		 		= 6;
const int	ITEM_FILE_DATE		 	 	= 7;
// hand info, 10..19
const int	ITEM_CURRHAND_NORTH		 	= 10;
const int	ITEM_CURRHAND_EAST		 	= 11;
const int	ITEM_CURRHAND_SOUTH		 	= 12;
const int	ITEM_CURRHAND_WEST		 	= 13;
const int	ITEM_ORIGHAND_NORTH		 	= 14;
const int	ITEM_ORIGHAND_EAST			= 15;
const int	ITEM_ORIGHAND_SOUTH			= 16;
const int	ITEM_ORIGHAND_WEST			= 17;
// current round info, 20..29
const int	ITEM_CURR_ROUND_LEAD			= 20;
const int	ITEM_NUM_CARDS_PLAYED_IN_ROUND	= 21;
const int	ITEM_TRICK_CARD_1				= 22;
const int	ITEM_TRICK_CARD_2				= 23;
const int	ITEM_TRICK_CARD_3				= 24;
const int	ITEM_TRICK_CARD_4				= 25;
// game status info, 30..49
const int	ITEM_VIEW_STATUS_CODE		= 30;
const int	ITEM_RUBBER_IN_PROGRESS		= 31;
const int	ITEM_GAME_IN_PROGRESS		= 32;
const int	ITEM_BIDDING_IN_PROGRESS	= 33;
const int	ITEM_HANDS_DEALT			= 34;
const int	ITEM_DEALER					= 35;
const int	ITEM_CONTRACT_SUIT			= 36;
const int	ITEM_CONTRACT_LEVEL			= 37;
const int	ITEM_CONTRACT_MODIFIER		= 38;
const int	ITEM_NUM_BIDS				= 39;
const int	ITEM_BIDDING_HISTORY		= 40;
const int	ITEM_DECLARER				= 41;
// game record, 50..69
const int	ITEM_NUM_TRICKS_PLAYED		= 50;
const int	ITEM_NUM_TRICKS_WON_NS		= 51;
const int	ITEM_NUM_TRICKS_WON_EW		= 52;
const int	ITEM_GAME_LEAD				= 60;
const int	ITEM_GAME_TRICK_1			= 61;
const int	ITEM_GAME_TRICK_2			= 62;
const int	ITEM_GAME_TRICK_3			= 63;
const int	ITEM_GAME_TRICK_4			= 64;
const int	ITEM_GAME_TRICK_5			= 65;
const int	ITEM_GAME_TRICK_6			= 66;
const int	ITEM_GAME_TRICK_7			= 67;
const int	ITEM_GAME_TRICK_8			= 68;
const int	ITEM_GAME_TRICK_9			= 69;
const int	ITEM_GAME_TRICK_10			= 70;
const int	ITEM_GAME_TRICK_11			= 71;
const int	ITEM_GAME_TRICK_12			= 72;
const int	ITEM_GAME_TRICK_13			= 73;
// rubber score info, 80..99
const int	ITEM_SCORE_NS_BONUS			= 80;
const int	ITEM_SCORE_NS_GAME0			= 81;
const int	ITEM_SCORE_NS_GAME1			= 82;
const int	ITEM_SCORE_NS_GAME2			= 83;
const int	ITEM_SCORE_NS_GAMES_WON		= 84;
const int	ITEM_SCORE_EW_BONUS			= 85;
const int	ITEM_SCORE_EW_GAME0			= 86;
const int	ITEM_SCORE_EW_GAME1			= 87;
const int	ITEM_SCORE_EW_GAME2			= 88;
const int	ITEM_SCORE_EW_GAMES_WON		= 89;
const int	ITEM_CURRENT_GAME_INDEX		= 90;
const int	ITEM_BONUS_SCORE_RECORD		= 91;
const int	ITEM_GAME_SCORE_RECORD		= 92;
// misc info, 100..109
const int	ITEM_AUTOSHOW_COMMENTS		= 100;
const int	ITEM_AUTOSHOW_BID_HISTORY	= 101;
const int	ITEM_AUTOSHOW_PLAY_HISTORY	= 102;
const int	ITEM_AUTOSHOW_ANALYSES		= 103;

// totals
const int	NUM_ITEMS				    = 110;

extern const char* tszItemName[];


