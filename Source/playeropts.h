//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Playeropts.h
//
// constants used in generic document property get/set calls
//

#ifndef __PLAYEROPTS__
#define __PLAYEROPTS__


enum playeropts {
	//
	tnPosition,
	tnSkillLevel,
	tbDummy,
	tbTeamIsVulnerable,
	// Game info
//	tnumTricksMade,
	// misc
	tszAnalysis,
	tpPartner,

/*
 * declared in CBidStatus.h
 * 
	// team pos
	tnMinimumTPPoints,		
	tnMinimumTPCPoints,	
	tnMaximumTPPoints,	
	tnMaximumTPCPoints,
	tnPartnersMinimum,
	tnPartnersMaximum,
	//
	tnBid,
	tnumBidTurns,
	tnumBidsMadeByPlayer,
	tnPartnersLastBid,
	tnNextIntendedBid,
	tnIntendedSuit,
	tnIntendedContract,
	//
	tnOpeningPosition,
	tnPartnersSuit,
	tnAgreedSuit,
	tbRoundForceActive,
	tbGameForceActive,
	// convention status
	tnBlackwoodStatus,
	tnGerberStatus,
	tnJacobyStatus,
	tnStaymanStatus,
	// partner's hand info
	tnumPartnersAces,
	tnumPartnersKings,
 */
};


#endif