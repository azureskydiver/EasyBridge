//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Bidopts.h
//
// constants used in bidding property get/set calls
//

enum bidopts {
	//
	tnOpeningPosition = 4000,
	tnPartnersSuit,
	tnAgreedSuit,
	tbRoundForceActive,
	tbGameForceActive,
	//
	tnBid,
	tnumBidTurns,
	tnumBidsMadeByPlayer,
	tnNextIntendedBid,
	tnIntendedSuit,
	tnIntendedContract,
	// team points
	tnMinimumTPPoints,		
	tnMinimumTPCPoints,	
	tnMaximumTPPoints,	
	tnMaximumTPCPoints,
	tnPartnersMinimum,
	tnPartnersMaximum,
	tnumPartnersAces,
	tnumPartnersKings,
	// convention status
	tnBlackwoodStatus,
	tnCueBidStatus,
	tnGerberStatus,
	tnJacobyTransferStatus,
	tnStaymanStatus,
	// state info
	tbInTestBiddingMode,
};

