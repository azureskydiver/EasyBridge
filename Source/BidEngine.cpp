//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//===========================================================
//
// CBidEngine
//
// - encapsulates status variables relating to 
// a player's bidding status
//
//=========================================================


#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "PlayerStatusDialog.h"
#include "MainFrm.h"
#include "MainFrameOpts.h"
#include "Player.h"
#include "Card.h"
#include "bidparams.h"
#include "bidengine.h"
#include "progopts.h"
#include "bidopts.h"
#include "ConventionSet.h"
//#include "NeuralNet.h"
//#include "NNetOutputDialog.h"


// static data
int CBidEngine::m_numDefaultBiddingInputs = 350;
int CBidEngine::m_numDefaultBiddingOutputs = 608;


//
//--------------------------------------------------------------
//
// Construction/Destruction
//

// constructor
CBidEngine::CBidEngine()
{
	// clear all variables
	m_pHand = NULL;
}

// destructor
CBidEngine::~CBidEngine()
{
	m_mapConventionStatus.RemoveAll();
	m_mapConventionParameters.RemoveAll();
}


// 
// Initialize()
//
void CBidEngine::Initialize(CPlayer* pPlayer, CPlayer* pPartner, CPlayer* pLHOpponent, CPlayer* pRHOpponent, CHandHoldings* pHoldings, CCardLocation* pCardLocation, CGuessedHandHoldings** ppGuessedHands, CPlayerStatusDialog* pStatusDlg)
{ 
	//
	m_pPlayer = pPlayer; 
	m_pPartner = pPartner; 
	m_pLHOpponent = pLHOpponent;
	m_pRHOpponent = pRHOpponent;
	//
	m_pHand = pHoldings; 
	m_pCardLocation = pCardLocation; 
	m_ppGuessedHands = ppGuessedHands;
	m_pStatusDlg = pStatusDlg; 
}




//
//--------------------------------------------------------------
//
// Misc routines
//




//
// Clear()
//
void CBidEngine::Clear()
{
	//
	// clear all internal variables
	//
	m_nBid = BID_NONE;  // NCR changed to BID_NONE vs NONE
	m_numBidsMade = 0;
	m_numBidTurns = 0;
	m_nNextIntendedBid = NIB_NONE;
//	m_nPartnersSuit = NONE;
	m_numPartnersAces = 0;
	m_numPartnersKings = 0;
	//
	m_fMinTPPoints = 0;
	m_fMaxTPPoints = 0;
	m_fMinTPCPoints = 0;
	m_fMaxTPCPoints = 0;
	m_fPartnersMin = 0;
	m_fPartnersMax = 0;
	//
	nPartnersOpeningBid = BID_NONE;  // NCR changed to BID_NONE vs NONE
	nPartnersOpeningBidLevel = 0;
	nPartnersOpeningSuit = NOSUIT;   // NCR changed to NOSUIT vs NONE 
 	//
	m_nOpeningPosition = -1;
	m_bOpenedBiddingForTeam = FALSE;
	m_bPartnerOpenedForTeam = FALSE;
	m_bPartnerOvercalled = FALSE;
	m_nAgreedSuit = NOSUIT; // NCR changed to NOSUIT vs NONE
	m_nIntendedSuit = NOSUIT; // NCR changed to NOSUIT vs NONE
	m_nIntendedContract = NONE;
	//
	m_bRoundForceActive = FALSE;
	m_bGameForceActive = FALSE;
	m_bInvitedToGame = FALSE;
	m_bInvitedToSlam = FALSE;
	m_bHintMode = FALSE;
	m_bInTestBiddingMode = false;

	//
	// clear and rebuild the convention status map
	//
	m_pActiveConvention = NULL;
	m_mapConventionStatus.RemoveAll();
	m_mapConventionParameters.RemoveAll();
	POSITION pos = NULL;
	do 
	{
		CConvention* pConvention = pCurrConvSet->GetNextConvention(pos);
		if (pConvention)
		{
			m_mapConventionStatus.SetAt(pConvention, CONV_INACTIVE);
			m_mapConventionParameters.SetAt(pConvention, 0);
		}
	} while (pos);
}



//
// SuitHoldingsToString()
//
// - also called by STS()
//
LPCTSTR CBidEngine::SuitHoldingsToString(int nSuit)
{
	if (ISSUIT(nSuit))
		return m_pHand->GetSuit(nSuit).GetCompressedHoldingsString(); 
	else
		return "";
}


//
// SuitStrengthToString()
//
// - also called by SSTS()
//
LPCTSTR CBidEngine::SuitStrengthToString(int nSuit)
{
	if (!ISSUIT(nSuit))
		return ("N/A");
	//
	int nStrength = nSuitStrength[nSuit];
	switch (nStrength)
	{
		case SS_VOID:
			return "void";

		case SS_POOR:
			return "very poor";

		case SS_WEAK_SUPPORT:
		case SS_MODERATE_SUPPORT:
			return "weak";

		case SS_MARGINAL_OPENER:
			return "decent";

		case SS_OPENABLE:
			return "openable";

		case SS_STRONG:
			return "strong";

		case SS_ABSOLUTE:
			return "powerful";

		default:
			return "unknown";
	}
}



//
//
// SupportLevelToString()
//
// - also called by SLTS()
//
LPCTSTR CBidEngine::SupportLevelToString(int nSuit)
{
	if (!ISSUIT(nSuit))
		return ("N/A");
	//
	int nSupportLevel = nSuitStrength[nSuit];
	if (nSupportLevel < SS_WEAK_SUPPORT)
	{
		return "poor";
	}
	else if (nSupportLevel > SS_STRONG_SUPPORT)
	{
		return "excellent";
	}
	else
	{
		// somewhere in between
		switch (nSupportLevel)
		{
			case SS_WEAK_SUPPORT:
				return "weak";

			case SS_MODERATE_SUPPORT:
				return "moderate";

			case SS_GOOD_SUPPORT:
				return "good";

			case SS_STRONG_SUPPORT:
				return "strong";
			
			default:
				return "unknown";
		}
	}
}



//
// Trace()
//
void CBidEngine::Trace(const CString& strText) 
{ 
	m_pPlayer->Trace(strText); 
}



//
// TraceNH()
//
void CBidEngine::TraceNH(const CString& strText) 
{ 
	pMAINFRAME->SuspendHints();
	m_pPlayer->Trace(strText); 
	pMAINFRAME->ResumeHints();
}



//
// AddTrace()
//
// shortcut routine to add a string to a base string and trace
// (print) the result
//
void CBidEngine::AddTrace(CString& strMessage, CString& strTemp)
{
	// return if trace disabled
	if (!theApp.GetValue(tbEnableAnalysisTracing))
		return;
	//
	strMessage += strTemp;
	Trace(strMessage);	
}


//
// AddTrace()
//
void CBidEngine::AddTrace(CString& strMessage, LPCTSTR szTemp)
{
	// return if trace disabled
	if (!theApp.GetValue(tbEnableAnalysisTracing))
		return;
	//
	strMessage += szTemp;
	Trace(strMessage);	
}





//
//==========================================================================
//
// Main routines
//
//==========================================================================
//



//
//==========================================================
//

int CBidEngine::Bid()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;

	// build up the bidding info working data structures
	AssessPosition();

	//
	int nBid = BID_NONE;

	// first time bidding?
	if (nRound == 0) 
	{						   
		// first round -- has partner bid?
		if (m_pPartner->GetNumBidsMade() == 0) 
			// partner has not bid yet, so make opening bid
			nBid = MakeOpeningBid();
		else 
			// else partner has bid, so respond
			nBid = MakeRespondingBid();
	} 
	else 
	{
		// second or later round -- check if we bid last time
		if (m_nBid == BID_PASS) 
		{
			// we passed -- did partner bid last time?
			if (m_pPartner->InquireLastBid() != BID_PASS) 
			{
				// we passed but (s)he bid, so respond
				if (m_pPartner->GetNumBidsMade() == 2) 
				{
					// parter bid, we passed, and partner bid again
					nBid = MakeRebidAsResponder();
				}
				else if (m_numBidsMade == 0) 
				{
					// we passed and partner bid, so respond
					nBid = MakeRespondingBid();
				}
				else if ((m_numBidsMade == 1) && (nPreviousBid != BID_PASS)) // NCR-349 Not response if no bid
				{
					// partner bid, we responded, and partner rebid
					nBid = MakeRebidAsResponder();
				}
				else 
				{
/*#ifdef _DEBUG   // NCR DEBUG CODE
					if(m_numBidsMade == 1) { //NCR-369 Debug code
					     AfxMessageBox("Calling MakeRebidExtended with numBidsMade == 1");
					}
#endif */
					// rebid/respond after the second round
					nBid = MakeRebidExtended();
				}
			} 
			else 
			{
				// else he passed also, so pass
				status << "We passed and partner passed, so pass again.\n";
				nBid = BID_PASS;
			}
		} 
		else 
		{
			// we bid last time, so rebid
			// but as opener or responder?
			if (m_pPartner->GetNumBidsMade() == 0) 
			{
				// partner passed, so rebid as opener
				nBid = MakeRebidAsOpener();
			}                                             // NCR-588 Test if partner openned
			else if (m_pPartner->GetNumBidsMade() == 1 && !m_bPartnerOpenedForTeam) 
			{
				// partner reponded, so rebid as opener
				nBid = MakeRebidAsOpener();
			}
			else if (m_pPartner->GetNumBidsMade() == 2) 
			{
				// partner made two bids, so rebid as responder
				// if we've only made 1 response, else
				// rebid after the 3rd round 
				if (m_numBidsMade <= 1) 
					nBid = MakeRebidAsResponder();
				else 
					nBid = MakeRebidExtended();
			} 
			else 
			{
				// rebid/respond after the second round
				nBid = MakeRebidExtended();
			}
		}
	}

	// if the bid is PASS, see if we should double for penalties
	if (nBid == BID_PASS)
	{
		if (TestForPenaltyDouble())
		{
			// double!!!
			nBid = BID_DOUBLE;
		}
	}

	// done
	return nBid;

/*
  // see if we're using a neural net
	if (theApp.GetValue(tnBiddingEngine) == 1)
		return GetNeuralNetBid();
	else
		return GetRuleBasedBid();
*/
}



//
// GetRuleBasedBid()
//
int CBidEngine::GetRuleBasedBid()
{
	// 
	//============================================================
	//
	// Using standard bidding engine
	// First bid on the player's part; determine actions by points
	// need to select opening bids from the strongest on down
	//

	// first time bidding?
	if (nRound == 0) 
	{						   
		// first round -- has partner bid?
		if (m_pPartner->GetNumBidsMade() == 0) 
			// partner has not bid yet, so make opening bid
			nBid = MakeOpeningBid();
		else 
			// else partner has bid, so respond
			nBid = MakeRespondingBid();
	} 
	else 
	{
		// second or later round -- check if we bid last time
		if (m_nBid == BID_PASS) 
		{
			// we passed -- did partner bid last time?
			if (m_pPartner->InquireLastBid() != BID_PASS) 
			{
				// we passed but (s)he bid, so respond
				if (m_pPartner->GetNumBidsMade() == 2) 
				{
					// parter bid, we passed, and partner bid again
					nBid = MakeRebidAsResponder();
				}
				else if (m_numBidsMade == 0) 
				{
					// we passed and partner bid, so respond
					nBid = MakeRespondingBid();
				}
				else if (m_numBidsMade == 1) 
				{
					// partner bid, we responded, and partner rebid
					nBid = MakeRebidAsResponder();
				}
				else 
				{
					// rebid/respond after the second round
					nBid = MakeRebidExtended();
				}
			} 
			else 
			{
				// else he passed also, so pass
				nBid = BID_PASS;
			}
		} 
		else 
		{
			// we bid last time, so rebid
			// but as opener or responder?
			if (m_pPartner->GetNumBidsMade() == 0) 
			{
				// partner passed, so rebid as opener
				nBid = MakeRebidAsOpener();
			}
			else if (m_pPartner->GetNumBidsMade() == 1) 
			{
				// partner reponded, so rebid as opener
				nBid = MakeRebidAsOpener();
			}
			else if (m_pPartner->GetNumBidsMade() == 2) 
			{
				// partner made two bids, so rebid as responder
				// if we've only made 1 response, else
				// rebid after the 3rd round 
				if (m_numBidsMade <= 1) 
					nBid = MakeRebidAsResponder();
				else 
					nBid = MakeRebidExtended();
			} 
			else 
			{
				// rebid/respond after the second round
				nBid = MakeRebidExtended();
			}
		}
	}

	// if the bid is PASS, see if we should double for penalties
	if (nBid == BID_PASS)
	{
		if (TestForPenaltyDouble())
		{
			// double!!!
			nBid = BID_DOUBLE;
		}
	}

	// done
	return nBid;
}




//
// GetBiddingHint()
//
int CBidEngine::GetBiddingHint()
{
	m_bHintMode = TRUE;
	// save old bid
	int nOldBid = m_nBid;
	int nBid = Bid();
	// restor eold bid
	m_nBid = nOldBid;
	m_bHintMode = FALSE;
	//
	return nBid;
}





//
// AssessPosition()
//
// fill in some shortcut variables
//
void CBidEngine::AssessPosition()
{
	// 
	nRound = pDOC->GetBiddingRound();
	nLastBid = pDOC->GetLastValidBid();
	nPosition = m_pPlayer->GetPosition();
	nBiddingOrder = pDOC->GetNumBidsMade();
	nLastValidRecordedBid = pDOC->GetLastValidBid();

	// NCR moved this forward so it could be used to check for overcall vs jump
	// record opponents' info shortcuts
	nLHOBid = m_pLHOpponent->InquireLastBid();
	if (nLHOBid != BID_PASS)
	{
		nLHOBidLevel = BID_LEVEL(nLHOBid);
		nLHOSuit = BID_SUIT(nLHOBid);
	}
	else
	{
		nLHOBidLevel = 0;
		nLHOSuit = NOSUIT;  // NCR changed NONE to NOSUIT
	}
	bLHOPassed = (nLHOBid == BID_PASS)? TRUE: FALSE;
	bLHOInterfered = !bLHOPassed;
	nLHONumBidsMade = m_pLHOpponent->GetNumBidsMade();
	nLHONumBidTurns = m_pLHOpponent->GetNumBidTurns();
	//
	nRHOBid = m_pRHOpponent->InquireLastBid();
	if (nRHOBid != BID_PASS)
	{
		nRHOBidLevel = BID_LEVEL(nRHOBid);
		nRHOSuit = BID_SUIT(nRHOBid);
	}
	else
	{
		nRHOBidLevel = 0;
		nRHOSuit = NOSUIT;  // NCR changed NONE to NOSUIT
	}
	bRHOPassed = (nRHOBid == BID_PASS)? TRUE: FALSE;
	bRHOInterfered = !bRHOPassed;
	nRHONumBidsMade = m_pRHOpponent->GetNumBidsMade();
	nRHONumBidTurns = m_pRHOpponent->GetNumBidTurns();
	//
	if (nRHOBid > nLHOBid) 
	{
		nOpponentsBid = nRHOBid;
		nOpponentsBidLevel = nRHOBidLevel;
		nOpponentsBidSuit = nRHOSuit;
	}
	else
	{
		nOpponentsBid = nLHOBid;
		nOpponentsBidLevel = nLHOBidLevel;
		nOpponentsBidSuit = nLHOSuit;
	}
	// NCR end of moved code

	//
	// retrieve bidding history info
	//
	nPreviousBid = m_nBid;
	if (ISBID(nPreviousBid))
	{
		nPreviousBidLevel = BID_LEVEL(nPreviousBid);
		nPreviousSuit = BID_SUIT(nPreviousBid);
	}
	else
	{
		nPreviousBidLevel = NONE;
		nPreviousSuit = NOSUIT;  // NCR changed NONE to NOSUIT
	}
	if (ISSUIT(nPreviousSuit))
	{
		nPreviousSuitStrength = m_pHand->GetSuitStrength(nPreviousSuit);
		numPreviousSuitCards = m_pHand->GetSuitLength(nPreviousSuit);
		numPreviousSuitPoints = m_pHand->GetSuitHCPoints(nPreviousSuit);
		numHonorsInPreviousSuit = m_pHand->GetNumHonorsInSuit(nPreviousSuit);
		bPreviousSuitIsSolid = m_pHand->IsSuitSolid(nPreviousSuit);
		bPreviousSuitIsRebiddable = m_pHand->IsSuitRebiddable(nPreviousSuit);
		bPreviousSuitIsSelfSupporting = m_pHand->IsSuitSelfSupporting(nPreviousSuit);
	}
	else
	{
		nPreviousSuitStrength = 0;
		numPreviousSuitCards = 0;
		numPreviousSuitPoints = 0;
		numHonorsInPreviousSuit = 0;
		bPreviousSuitIsSolid = FALSE;
		bPreviousSuitIsRebiddable = FALSE;
		bPreviousSuitIsSelfSupporting = FALSE;
	}

	//
	if (nRound >= 2)
	{
		nNextPrevBid = pDOC->GetBidByPlayer(m_pPlayer->GetPosition(), nRound-2);
		if (ISBID(nNextPrevBid))
			nNextPrevSuit = BID_SUIT(nNextPrevBid);
		else
			nNextPrevSuit = NOSUIT;  // NCR changed NONE to NOSUIT
	}
	else
	{
		nNextPrevBid = BID_NONE;  // NCR changed NONE to BID_NONE
		nNextPrevSuit = NOSUIT;  // NCR changed NONE to NOSUIT
	}


	// get first round bid info
	nFirstRoundBid = pDOC->GetBidByPlayer(m_pPlayer->GetPosition(), 0);
	if (ISBID(nFirstRoundBid))
	{
		nFirstRoundBidLevel = BID_LEVEL(nFirstRoundBid);
		nFirstRoundSuit = BID_SUIT(nFirstRoundBid);
	}
	else
	{
		nFirstRoundBidLevel = NONE;
		nFirstRoundSuit = NOSUIT;  // NCR changed NONE to NOSUIT
	}
	if (ISSUIT(nFirstRoundSuit))
	{
		nFirstRoundSuitStrength = m_pHand->GetSuitStrength(nFirstRoundSuit);
		numFirstRoundSuitCards = m_pHand->GetSuitLength(nFirstRoundSuit);
		bFirstRoundSuitIsSolid = m_pHand->IsSuitSolid(nFirstRoundSuit);
		bFirstRoundSuitIsSelfSupporting = m_pHand->IsSuitSelfSupporting(nFirstRoundSuit);
	}
	else
	{
		nFirstRoundSuitStrength = 0;
		numFirstRoundSuitCards = 0;
		bFirstRoundSuitIsSolid = FALSE;
		bFirstRoundSuitIsSelfSupporting = FALSE;
	}

	//
	// note our preferred suit
	//
	nPrefSuit = m_pHand->GetPreferredSuit();
	nPrefSuitStrength = m_pHand->GetSuitStrength(nPrefSuit);
	numPrefSuitCards = m_pHand->GetSuitLength(nPrefSuit);
	numPrefSuitPoints = m_pHand->GetSuitHCPoints(nPrefSuit);
	numHonorsInPrefSuit = m_pHand->GetNumHonorsInSuit(nPrefSuit);
	bPrefSuitIsSolid = m_pHand->IsSuitSolid(nPrefSuit);
	bPrefSuitIsSelfSupporting = m_pHand->IsSuitSelfSupporting(nPrefSuit);
	numPreferredSuits = m_pHand->GetNumPreferredSuits();
	
	//
	// note partner's suit
	//
	nPartnerPos = m_pPartner->GetPosition();
	m_numPartnerBidsMade = m_pPartner->GetNumBidsMade();
	m_numPartnerBidTurns = m_pPartner->GetNumBidTurns();

	nPartnersBid = m_pPartner->InquireLastBid();
	nPartnersBidLevel = BID_LEVEL(nPartnersBid);
	nPartnersSuit = BID_SUIT(nPartnersBid);
	if (ISSUIT(nPartnersSuit))
	{
		nPartnersSuitSupport = m_pHand->GetSuitStrength(nPartnersSuit);
		numSupportCards = m_pHand->GetSuitLength(nPartnersSuit);
		numHonorsInPartnersSuit = m_pHand->GetNumHonorsInSuit(nPartnersSuit);
		// NCR test for intervening bid. If so, pard's bid was an overcall, NOT a Jump!
		int nTheBidBeforePards = ISBID(nLHOBid) ? nLHOBid : nPreviousBid;  //NCR use last bid before pards
		bJumpResponseByPartner = ((nPartnersBid - nTheBidBeforePards) > 5)? TRUE :  FALSE;
	}
	else
	{
		// partner may have doubled, passed, or just hasn't had his turn yet
		nPartnersSuitSupport = 0;
		numSupportCards = 0;
		numHonorsInPartnersSuit = 0;
		bJumpResponseByPartner = FALSE;
	}

	// see if this is partner's opening bid
	if (nPartnersOpeningBid == BID_NONE)  // NCR changed NONE to BID_NONE
	{
		nPartnersOpeningBid = nPartnersBid;
		nPartnersOpeningBidLevel = nPartnersBidLevel;
		nPartnersOpeningSuit = nPartnersSuit;
	}

	// record partner's previous bid as well
	nPartnersPrevBid = m_pPartner->GetPriorBid(1);
	nPartnersPrevBidLevel = BID_LEVEL(nPartnersPrevBid);
	nPartnersPrevSuit = BID_SUIT(nPartnersPrevBid);
	if (ISSUIT(nPartnersPrevSuit))
	{
		nPPrevSuitSupport = m_pHand->GetSuitStrength(nPartnersPrevSuit);
		numPPrevSuitSupportCards = m_pHand->GetSuitLength(nPartnersPrevSuit);
	}
	else
	{
		nPPrevSuitSupport = 0;
		numPPrevSuitSupportCards = 0;
	}
	// special handling!!! see if partner opened a strong 2C
	if ((nPartnersPrevBid == BID_2C) &&
		(pCurrConvSet->IsConventionEnabled(tidArtificial2ClubConvention)) &&
		(GetConventionStatus(tidArtificial2ClubConvention) != CONV_INACTIVE))
	{
		// replace suit with a fake to prevent responder from trying to raise it
		nPartnersPrevSuit = NOSUIT;  // NCR changed NONE to NOSUIT
		nPPrevSuitSupport = SS_NONE;
		numPPrevSuitSupportCards = 0;
	}
	// NCR-36 also don't save previous bid if partner responding to Jacoby Transfer
	if(((nFirstRoundBid == BID_1NT) || (nFirstRoundBid == BID_2NT))  // ??? NEED WORK HERE
		&& (pCurrConvSet->IsConventionEnabled(tidJacobyTransfers)) &&
		(GetConventionStatus(tidJacobyTransfers) != CONV_INACTIVE))
	{
		nPartnersPrevSuit = NOSUIT;  // NCR changed NONE to NOSUIT  // this bid over a NT is not a real bid
		nPPrevSuitSupport = SS_NONE;
		numPPrevSuitSupportCards = 0;
	} // NCR-36 end

	//
	numSuitsStopped = m_pHand->GetNumSuitsStopped();
	numSuitsProbStopped = m_pHand->GetNumSuitsProbStopped();
	numVoids = m_pHand->GetNumVoids();
	numSingletons = m_pHand->GetNumSingletons();
	numDoubletons = m_pHand->GetNumDoubletons();
	numLikelyWinners = m_pHand->GetNumLikelyWinners();
	numLikelyLosers = m_pHand->GetNumLikelyLosers(); // NCR-260 changed from ...Winners();
	numWinners = m_pHand->GetNumWinners();
	numLosers = m_pHand->GetNumLosers(); // NCR-260 changed from ...Winners();
	numQuickTricks = m_pHand->GetNumQuickTricks();
	//
	numOpenableSuits = m_pHand->GetNumOpenableSuits();
	numRebiddableSuits = m_pHand->GetNumRebiddableSuits();
	numSolidSuits = m_pHand->GetNumSolidSuits();
	numAbsoluteSuits = m_pHand->GetNumAbsoluteSuits();
	int i;
	for(i=0;i<4;i++)
	{
		nSuitStrength[i] = m_pHand->GetSuitStrength(i);
		numCardsInSuit[i] = m_pHand->GetSuitLength(i);
		numHonorsInSuit[i] = m_pHand->GetNumHonorsInSuit(i);
		numSuitPoints[i] = m_pHand->GetSuitHCPoints(i);
	}
	for(i=0;i<5;i++)
		nPrefSuitList[i] = m_pHand->GetSuitsByPreference(i);

	//
	numAces = m_pHand->GetNumAces();
	numKings = m_pHand->GetNumKings();
	numQueens = m_pHand->GetNumQueens();
	numJacks = m_pHand->GetNumJacks();
	numTens = m_pHand->GetNumTens();

	//
	// record text shortcuts
	//
	strcpy(szLHO, PositionToString(m_pLHOpponent->GetPosition()));
	strcpy(szRHO, PositionToString(m_pRHOpponent->GetPosition()));

	//
	strcpy(szPB,BTS(nPartnersBid));			// partner's bid
	strcpy(szPS,STS(nPartnersSuit));		// partner's suit
	strcpy(szPSS,STSS(nPartnersSuit));		// partner's suit, singular
	strcpy(szHP,SHTS(nPartnersSuit));		// our holding in partner's suit
/*
	if (m_numPartnerBidsMade > 0)
	{
		if (nPartnersBid != BID_PASS)
		{
			strcpy(szPB,BTS(nPartnersBid));			// partner's bid
			strcpy(szPS,STS(nPartnersSuit));		// partner's suit
			strcpy(szPSS,STSS(nPartnersSuit));		// partner's suit, singular
			if ((nPartnersBid != BID_PASS) && (nPartnersSuit != NOTRUMP))
				strcpy(szHP,SHTS(nPartnersSuit));	// our holding in partner's suit
			else
				strcpy(szHP,"");
		}
	}
	else
	{
		szPB[0] = szPS[0] = szPSS[0] = szHP[0] = '\0';
	}
*/

	strcpy(szPPB,BTS(nPartnersPrevBid));	// partner's prev bid
	strcpy(szPPS,STS(nPartnersPrevSuit));	// partner's prev suit
	strcpy(szPPSS,STSS(nPartnersPrevSuit));	// partner's prev suit, singular
	strcpy(szHPP,SHTS(nPartnersPrevSuit));	// our holding in partner's prev suit
/*
	//
	if ((m_numPartnerBidsMade > 1) && (nPartnersPrevBid != BID_PASS))
	{
		strcpy(szPPB,BTS(nPartnersPrevBid));		// partner's prev bid
		strcpy(szPPS,STS(nPartnersPrevSuit));	// partner's prev suit
		strcpy(szPPSS,STSS(nPartnersPrevSuit));	// partner's prev suit, singular
		if ((nPartnersPrevBid != BID_PASS) && (nPartnersPrevSuit != NOTRUMP))
			strcpy(szHPP,SHTS(nPartnersPrevSuit));	// our holding in partner's prev suit
		else
			strcpy(szHPP,"");
	}
	else
	{
		szPPB[0] = szPPS[0] = szPPSS[0] = szHPP[0] = '\0';
	}
*/
	//
	strcpy(szPrefS,STS(nPrefSuit));		// our preferred suit
	strcpy(szPrefSS,STSS(nPrefSuit));	// our preferred suit, singular
	strcpy(szHPref,SHTS(nPrefSuit));	// holdings in our preferred suit

	// record our own bid info in strings

//	if ((m_numBidsMade > 0) && (nPreviousSuit != NONE))
//	{
		strcpy(szPVB,BTS(nPreviousBid));		// our previous bid 
		strcpy(szPVS,STS(nPreviousSuit));		// our previously bid suit
		strcpy(szPVSS,STSS(nPreviousSuit));		// prev bid suit, singular
		if (nPreviousSuit != NOTRUMP)
			strcpy(szHPV,SHTS(nPreviousSuit));	// holdings in our prev bid suit
		else
			szHPV[0] = '\0';
/*
	}
	else
	{
		szPVB[0] = szPVS[0] = szPVSS[0] = szHPV[0] = '\0';
	}
*/

//	if (nNextPrevSuit != NONE)
//	{
		strcpy(szNPB,BTS(nNextPrevBid));		// our bid 2 turns ago
		strcpy(szNPS,STS(nNextPrevSuit));	
		strcpy(szNPSS,STSS(nNextPrevSuit));	
		if (nNextPrevSuit != NOTRUMP)
			strcpy(szHNPS,SHTS(nNextPrevSuit));
		else
			szHNPS[0] = '\0';
/*
	}
	else
	{
		szNPB[0] = szNPS[0] = szNPSS[0] = '\0';
	}
*/

	// set some other shortcuts
	fPts = m_pHand->GetTotalPoints();
	fAdjPts = fPts;
	fCardPts = m_pHand->GetHCPoints();
	fDistPts = m_pHand->GetLengthPoints() + m_pHand->GetShortPoints();
	fPenPts = m_pHand->GetPenaltyPoints();
	bBalanced = m_pHand->IsBalanced();
	bSemiBalanced = m_pHand->IsBalanced() || m_pHand->IsSemiBalanced();
	bAllSuitsStopped = m_pHand->AllSuitsStopped();
/*  Move following to front of method so can be used to check for overcall
	// record opponents' info shortcuts
	nLHOBid = m_pLHOpponent->InquireLastBid();
	if (nLHOBid != BID_PASS)
	{
		nLHOBidLevel = BID_LEVEL(nLHOBid);
		nLHOSuit = BID_SUIT(nLHOBid);
	}
	else
	{
		nLHOBidLevel = 0;
		nLHOSuit = NONE;
	}
	bLHOPassed = (nLHOBid == BID_PASS)? TRUE: FALSE;
	bLHOInterfered = !bLHOPassed;
	nLHONumBidsMade = m_pLHOpponent->GetNumBidsMade();
	nLHONumBidTurns = m_pLHOpponent->GetNumBidTurns();
	//
	nRHOBid = m_pRHOpponent->InquireLastBid();
	if (nRHOBid != BID_PASS)
	{
		nRHOBidLevel = BID_LEVEL(nRHOBid);
		nRHOSuit = BID_SUIT(nRHOBid);
	}
	else
	{
		nRHOBidLevel = 0;
		nRHOSuit = NONE;
	}
	bRHOPassed = (nRHOBid == BID_PASS)? TRUE: FALSE;
	bRHOInterfered = !bRHOPassed;
	nRHONumBidsMade = m_pRHOpponent->GetNumBidsMade();
	nRHONumBidTurns = m_pRHOpponent->GetNumBidTurns();
	//
	if (nRHOBid > nLHOBid) 
	{
		nOpponentsBid = nRHOBid;
		nOpponentsBidLevel = nRHOBidLevel;
		nOpponentsBidSuit = nRHOSuit;
	}
	else
	{
		nOpponentsBid = nLHOBid;
		nOpponentsBidLevel = nLHOBidLevel;
		nOpponentsBidSuit = nLHOSuit;
	}
*/
} // end AssessPosition()




//
// AdjustPartnershipPoints()
//
void CBidEngine::AdjustPartnershipPoints(double fPartnersMin, double fPartnersMax, bool bAdjustPoints)
{
	if ((fPartnersMin > 0) && (fPartnersMax > 0))
	{
		if (bAdjustPoints)
		{
			m_fPartnersMin = OPEN_PTS(fPartnersMin);
			m_fPartnersMax = OPEN_PTS(fPartnersMax);
		}
		else
		{
			m_fPartnersMin = fPartnersMin;
			m_fPartnersMax = fPartnersMax;
		}
	}

	// see if default parameters were passed in
	if (fPartnersMin < 0)
		fPartnersMin = m_fPartnersMin;
	if (fPartnersMax < 0)
		fPartnersMax = m_fPartnersMax;

	//
	m_fMinTPPoints = fAdjPts + fPartnersMin;
	m_fMaxTPPoints = fAdjPts + fPartnersMax;
	m_fMinTPCPoints = fCardPts + fPartnersMin;
	m_fMaxTPCPoints = fCardPts + fPartnersMax;
}




///////////////////////////////////////////////////////////////////////////
//
//
// Neural Net routines
//
//
///////////////////////////////////////////////////////////////////////////



//
// GetNeuralNetBid()
//
int CBidEngine::GetNeuralNetBid()
{
/*
	CNeuralNet* pNet = theApp.GetNeuralNet();
	CNNetOutputDialog* pNetDlg = (CNNetOutputDialog*) pMAINFRAME->GetDialog(twNNetOutputDialog);
	
	//
	int numInputs = pNet->GetNumInputs();
	NVALUE* pfInputs = new NVALUE[numInputs];
	ASSERT(numInputs == m_numDefaultBiddingInputs);
	//
	int numOutputs = pNet->GetNumOutputs();
	NVALUE* pfOutputs = new NVALUE[numOutputs];
	ASSERT(numOutputs == m_numDefaultBiddingOutputs);


	//
	// first feed the neural net
	//
	FillNeuralNetInputs(pfInputs, numInputs);

	//
	// then get the output
	//
	pNet->SaveWeights();
	pNet->SimulateNet(pfInputs, pfOutputs);

	//
	// determine the winners
	//

	//
	BOOL bDoubled = pDOC->IsContractDoubled();
	BOOL bRedoubled = pDOC->IsContractRedoubled();

	// get the right group of outputs
	int nOutputGroupSize = 38;
	int tnumGroups = numOutputs / nOutputGroupSize;
	int nOutputGroup = pDOC->GetNumBidsMade();
	if (nOutputGroup >= tnumGroups)
		nOutputGroup = tnumGroups - 1;
	int nOffset = nOutputGroupSize * nOutputGroup;
	//
	double fHigh = pfOutputs[nOffset];	// BID_PASS value
	int nBidIndex = 0;

	//
	for(int i=nOffset;i<nOffset+nOutputGroupSize;i++)
	{
		// see if this bid is acceptable
		BOOL bValid = FALSE;
		int nBid = i - nOffset;
		// a pass is always OK
		if (nBid == 0)
			bValid = TRUE;
		// a bid is ok as long as it's higher than the last one
		if ((nBid > nLastValidRecordedBid) && (nBid < BID_DOUBLE))
			bValid = TRUE;
		// a double of a valid bid is ok if not doubled or redoubled
		if ((nBid == BID_DOUBLE) && (nLastValidRecordedBid > BID_PASS) && !bDoubled && !bRedoubled)
			bValid = TRUE;
		// a redouble is ok if doubled and not already redoubled
		if ((nBid == BID_REDOUBLE) && bDoubled && !bRedoubled)
			bValid = TRUE;
		//
//		if (bValid && (pfOutputs[i] > fHigh))
		if (pfOutputs[i] > fHigh)
		{
			fHigh = pfOutputs[i];
			if (bValid)
				nBidIndex = nBid;
		}
	}

	// normalize all values to between 0 and 1.0
	ASSERT(fHigh != 0);
	for(i=nOffset;i<nOffset+nOutputGroupSize;i++)
		pfOutputs[i] /= (NVALUE) fHigh;

	// show results
	NVALUE* pfResults = &pfOutputs[nOffset];
	pNetDlg->SetTrainingCyclesCount(pNet->GetNumTrainingCycles());
	pNetDlg->ShowResults(pfResults, nBidIndex);

	// done
	delete[] pfInputs;
	delete[] pfOutputs;
	return nBidIndex;
*/
	return 0;
}



//
// GetNeuralNetError()
//
double CBidEngine::GetNeuralNetError()
{
/*
	CNeuralNet* pNet = theApp.GetNeuralNet();
	return pNet->GetError();
*/
	return 0;
}



//
// GetNeuralNetTrainingCycles()
//
int CBidEngine::GetNeuralNetTrainingCycles()
{
/*
	CNeuralNet* pNet = theApp.GetNeuralNet();
	return pNet->GetNumTrainingCycles();
*/
	return 0;
}



//
// SetNeuralNetBid()
//
void CBidEngine::SetNeuralNetBid(int nCorrectBid)
{
/*
	CNeuralNet* pNet = theApp.GetNeuralNet();
	//
	int numOutputs = pNet->GetNumOutputs();
	NVALUE* pfOutputs = new NVALUE[numOutputs];
	ASSERT(numOutputs == m_numDefaultBiddingOutputs);

	// get the right group of outputs
	int nOutputGroupSize = 38;
	int tnumGroups = numOutputs / nOutputGroupSize;
	int nOutputGroup = pDOC->GetNumBidsMade();
	if (nOutputGroup >= tnumGroups)
		nOutputGroup = tnumGroups - 1;
	int nOffset = nOutputGroupSize * nOutputGroup;
	int i;

	// fill the output array, before, inside, and after the apprpriate group
//	for(i=0;i<nOffset;i++)
//		pfOutputs[i] = 0;

	//
	int nBidIndex;
	for(i=nOffset,nBidIndex=0;i<nOffset+nOutputGroupSize;i++,nBidIndex++)
	{
		if (nBidIndex == nCorrectBid)
			pfOutputs[i] = 1;
		else
			pfOutputs[i] = 0;
	}

	//
//	for(;i<numOutputs;i++)
//		pfOutputs[i] = 0;

	// and set 
	pNet->TrainNet(pfOutputs, nOffset, nOutputGroupSize);

	// mark the error
	CNNetOutputDialog* pNetDlg = (CNNetOutputDialog*) pMAINFRAME->GetDialog(twNNetOutputDialog);
	pNetDlg->ShowError(pNet->GetError());

	// done
	delete[] pfOutputs;
*/
}



//
// SetNeuralNetBid()
//
void CBidEngine::FillNeuralNetInputs(NVALUE* fInputs, int numInputs)
{
	int nIndex = 0;

	// inputs 0.. 2: basic information
	fInputs[nIndex++]		= (NVALUE) 1;		// # 0 = bias
	fInputs[nIndex++]		= (NVALUE) nRound;
	fInputs[nIndex++]		= (NVALUE) nBiddingOrder;

	// bid history
	// 3..12
	fInputs[nIndex++]		= (NVALUE) m_numBidTurns;
	fInputs[nIndex++]		= (NVALUE) m_numBidsMade;
	fInputs[nIndex++]		= (NVALUE) m_numPartnerBidTurns;
	fInputs[nIndex++]		= (NVALUE) m_numPartnerBidsMade;
	fInputs[nIndex++]		= (NVALUE) m_nOpeningPosition;
	fInputs[nIndex++]		= (NVALUE) m_bOpenedBiddingForTeam;
	fInputs[nIndex++]		= (NVALUE) m_bPartnerOpenedForTeam;
	fInputs[nIndex++]		= (NVALUE) m_bPartnerOvercalled;
	fInputs[nIndex++]		= (NVALUE) nLastBid;			// last bid made by anybody
	fInputs[nIndex++]		= (NVALUE) nLastValidRecordedBid;	// last recorded valid bid

	// 13..22
	fInputs[nIndex++]		= (NVALUE) nPreviousBid;			// my previous bid
	fInputs[nIndex++]		= (NVALUE) nPreviousBidLevel;		
	fInputs[nIndex++]		= (NVALUE) nPreviousSuit;
	fInputs[nIndex++]		= (NVALUE) nPreviousSuitStrength;
	fInputs[nIndex++]		= (NVALUE) numPreviousSuitCards;
	fInputs[nIndex++]		= (NVALUE) numPreviousSuitPoints;
	fInputs[nIndex++]		= (NVALUE) numHonorsInPreviousSuit;
	fInputs[nIndex++]		= (NVALUE) bPreviousSuitIsSolid;
	fInputs[nIndex++]		= (NVALUE) bPreviousSuitIsRebiddable;
	fInputs[nIndex++]		= (NVALUE) bPreviousSuitIsSelfSupporting;

	// 23..24
	fInputs[nIndex++]		= (NVALUE) nNextPrevBid;		// from 2 rounds ago
	fInputs[nIndex++]		= (NVALUE) nNextPrevSuit;

	// 25..31
	fInputs[nIndex++]		= (NVALUE) nFirstRoundBid;
	fInputs[nIndex++]		= (NVALUE) nFirstRoundBidLevel;
	fInputs[nIndex++]		= (NVALUE) nFirstRoundSuit;
	fInputs[nIndex++]		= (NVALUE) nFirstRoundSuitStrength;
	fInputs[nIndex++]		= (NVALUE) numFirstRoundSuitCards;
	fInputs[nIndex++]		= (NVALUE) bFirstRoundSuitIsSolid;
	fInputs[nIndex++]		= (NVALUE) bFirstRoundSuitIsSelfSupporting;

	// 32..39
	fInputs[nIndex++]		= (NVALUE) nPrefSuit;
	fInputs[nIndex++]		= (NVALUE) nPrefSuitStrength;
	fInputs[nIndex++]		= (NVALUE) numPrefSuitCards;
	fInputs[nIndex++]		= (NVALUE) numPrefSuitPoints;
	fInputs[nIndex++]		= (NVALUE) numHonorsInPrefSuit;
	fInputs[nIndex++]		= (NVALUE) bPrefSuitIsSolid;
	fInputs[nIndex++]		= (NVALUE) bPrefSuitIsSelfSupporting;
	fInputs[nIndex++]		= (NVALUE) numPreferredSuits;

	// partner's bids & holdings
	// 40..46
	fInputs[nIndex++]		= (NVALUE) nPartnersBid;
	fInputs[nIndex++]		= (NVALUE) nPartnersBidLevel;
	fInputs[nIndex++]		= (NVALUE) nPartnersSuit;
	fInputs[nIndex++]		= (NVALUE) nPartnersSuitSupport;
	fInputs[nIndex++]		= (NVALUE) numSupportCards;
	fInputs[nIndex++]		= (NVALUE) numHonorsInPartnersSuit;
	fInputs[nIndex++]		= (NVALUE) bJumpResponseByPartner;

	// 47..50
	fInputs[nIndex++]		= (NVALUE) nPartnersOpeningBid;
	fInputs[nIndex++]		= (NVALUE) nPartnersOpeningBidLevel;
	fInputs[nIndex++]		= (NVALUE) nPartnersOpeningSuit;

	// 51..55
	fInputs[nIndex++]		= (NVALUE) nPartnersPrevBid;
	fInputs[nIndex++]		= (NVALUE) nPartnersPrevBidLevel;
	fInputs[nIndex++]		= (NVALUE) nPartnersPrevSuit;
	fInputs[nIndex++]		= (NVALUE) nPPrevSuitSupport;
	fInputs[nIndex++]		= (NVALUE) numPPrevSuitSupportCards;

	// 56..59 (dummy)
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;

	// opponents' bidding info
	// 60..65
	fInputs[nIndex++]		= (NVALUE) nLHOBid;
	fInputs[nIndex++]		= (NVALUE) nLHOBidLevel;
	fInputs[nIndex++]		= (NVALUE) nLHOSuit;
	fInputs[nIndex++]		= (NVALUE) bLHOPassed;
	fInputs[nIndex++]		= (NVALUE) nLHONumBidsMade;
	fInputs[nIndex++]		= (NVALUE) nLHONumBidTurns;

	// 66..71
	fInputs[nIndex++]		= (NVALUE) nRHOBid;
	fInputs[nIndex++]		= (NVALUE) nRHOBidLevel;
	fInputs[nIndex++]		= (NVALUE) nRHOSuit;
	fInputs[nIndex++]		= (NVALUE) bRHOPassed;
	fInputs[nIndex++]		= (NVALUE) nRHONumBidsMade;
	fInputs[nIndex++]		= (NVALUE) nRHONumBidTurns;

	// 72..74
	fInputs[nIndex++]		= (NVALUE) nOpponentsBid;		// most recent opp bid
	fInputs[nIndex++]		= (NVALUE) nOpponentsBidLevel;
	fInputs[nIndex++]		= (NVALUE) nOpponentsBidSuit;

	// 75..79 - dummy
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;

	//
	// detailed hand info
	//

	// point counts
	// 80..84
	fInputs[nIndex++]		= (NVALUE) fPts;
	fInputs[nIndex++]		= (NVALUE) fAdjPts;
	fInputs[nIndex++]		= (NVALUE) fCardPts;
	fInputs[nIndex++]		= (NVALUE) fDistPts;
	fInputs[nIndex++]		= (NVALUE) fPenPts;

	// partnership points
	// 85..90
	fInputs[nIndex++]		= (NVALUE) m_fMinTPPoints;	// min total partnership points
	fInputs[nIndex++]		= (NVALUE) m_fMaxTPPoints;	//
	fInputs[nIndex++]		= (NVALUE) m_fMinTPCPoints;	// min total partnership HCPs
	fInputs[nIndex++]		= (NVALUE) m_fMaxTPCPoints;
	fInputs[nIndex++]		= (NVALUE) m_fPartnersMin;	// min holding by partner
	fInputs[nIndex++]		= (NVALUE) m_fPartnersMax;

	// 91..100
	fInputs[nIndex++]		= (NVALUE) numSuitsStopped;
	fInputs[nIndex++]		= (NVALUE) numSuitsProbStopped;
	fInputs[nIndex++]		= (NVALUE) numVoids;
	fInputs[nIndex++]		= (NVALUE) numSingletons;
	fInputs[nIndex++]		= (NVALUE) numDoubletons;
	fInputs[nIndex++]		= (NVALUE) numLikelyWinners;
	fInputs[nIndex++]		= (NVALUE) numLikelyLosers;
	fInputs[nIndex++]		= (NVALUE) numWinners;
	fInputs[nIndex++]		= (NVALUE) numLosers;
	fInputs[nIndex++]		= (NVALUE) numQuickTricks;
	
	// 101..104
	fInputs[nIndex++]		= (NVALUE) numOpenableSuits;
	fInputs[nIndex++]		= (NVALUE) numRebiddableSuits;
	fInputs[nIndex++]		= (NVALUE) numSolidSuits;
	fInputs[nIndex++]		= (NVALUE) numAbsoluteSuits;

	// 105..111
	fInputs[nIndex++]		= (NVALUE) bBalanced;
	fInputs[nIndex++]		= (NVALUE) bSemiBalanced;
	fInputs[nIndex++]		= (NVALUE) bAllSuitsStopped;
	fInputs[nIndex++]		= (NVALUE) numAces;
	fInputs[nIndex++]		= (NVALUE) numKings;
	fInputs[nIndex++]		= (NVALUE) numQueens;
	fInputs[nIndex++]		= (NVALUE) numJacks;

	// 112..131
	for(nSuit=0;nSuit<4;nSuit++)
	{
		fInputs[nIndex++]		= (NVALUE) nSuitStrength[nSuit];
		fInputs[nIndex++]		= (NVALUE) numCardsInSuit[nSuit];
		fInputs[nIndex++]		= (NVALUE) numHonorsInSuit[nSuit];
		fInputs[nIndex++]		= (NVALUE) numSuitPoints[nSuit];
		fInputs[nIndex++]		= (NVALUE) nPrefSuitList[nSuit];
	}

	// 131..139 - dummy
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;

	// detailed hand info
	// 140..141
	fInputs[nIndex++]		= (NVALUE) m_pHand->GetNumTopCards();
	fInputs[nIndex++]		= (NVALUE) m_pHand->GetNumTrumps();

	// detailed suits info
	// 142..154
	fInputs[nIndex++]		= (NVALUE) m_pHand->GetBalanceValue();
	fInputs[nIndex++]		= (NVALUE) m_pHand->GetHighestAbsoluteSuit();
	fInputs[nIndex++]		= (NVALUE) m_pHand->GetHighestMarginalSuit();
	fInputs[nIndex++]		= (NVALUE) m_pHand->GetHighestOpenableSuit();
	fInputs[nIndex++]		= (NVALUE) m_pHand->GetHighestPreferredSuit();
	fInputs[nIndex++]		= (NVALUE) m_pHand->GetHighestSolidSuit();
	fInputs[nIndex++]		= (NVALUE) m_pHand->GetHighestStrongSuit();
	fInputs[nIndex++]		= (NVALUE) m_pHand->GetLowestAbsoluteSuit();
	fInputs[nIndex++]		= (NVALUE) m_pHand->GetLowestMarginalSuit();
	fInputs[nIndex++]		= (NVALUE) m_pHand->GetLowestOpenableSuit();
	fInputs[nIndex++]		= (NVALUE) m_pHand->GetLowestPreferredSuit();
	fInputs[nIndex++]		= (NVALUE) m_pHand->GetLowestSolidSuit();
	fInputs[nIndex++]		= (NVALUE) m_pHand->GetLowestStrongSuit();

	// 155..162
	for(int nRank=0;nRank<4;nRank++)
	{
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuitsByLength(nRank);
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuitsByPreference(nRank);
	}

	// 163..175
	for(int nLength=13;nLength<=0;nLength--)
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetNumSuitsOfAtLeast(nLength);

	// 176..179 - dummy
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;
	fInputs[nIndex++]		= (NVALUE) 0;
	
	// detailed info about each suit
	// 180..246
	for(nSuit=0;nSuit<4;nSuit++)
	{
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuit(nSuit).GetNumTopCards();
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuit(nSuit).GetNumTopHonors();
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuit(nSuit).GetNumHonors();
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuit(nSuit).GetNumSecondaryHonors();
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuit(nSuit).GetNumPseudoHonors();
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuit(nSuit).GetNumBodyCards();
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuit(nSuit).GetNumWinners();
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuit(nSuit).GetNumLosers();
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuit(nSuit).GetNumLikelyWinners();
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuit(nSuit).GetNumLikelyLosers();
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuit(nSuit).GetNumMissingHonors();
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuit(nSuit).GetNumMissingTopHonors();
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuit(nSuit).GetNumMissingInternalHonors();
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuit(nSuit).GetNumMissingBottomHonors();
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuit(nSuit).GetNumSequences();
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuit(nSuit).GetNumMissingSequences();
		fInputs[nIndex++]		= (NVALUE) m_pHand->GetSuit(nSuit).GetNumQuickTricks();
	}

	// 247: dummy
	fInputs[nIndex++]		= (NVALUE) 0;

	//
	// actual suit contents
	//

	// 248..260: Clubs holdings
	int nPos; // NCR-FFS added here, removed below
	for(/*int*/ nPos=0;nPos<13;nPos++)
		fInputs[nIndex++]		= (NVALUE) ((m_pHand->GetNumCardsInSuit(0) > nPos)? m_pHand->GetSuit(0).GetAt(nPos)->GetFaceValue() : 0);

	// 261-273: Diamonds holdings
	for(nPos=0;nPos<13;nPos++)
		fInputs[nIndex++]		= (NVALUE) ((m_pHand->GetNumCardsInSuit(1) > nPos)? m_pHand->GetSuit(1).GetAt(nPos)->GetFaceValue() : 0);

	// 274-286: Hearts holdings
	for(nPos=0;nPos<13;nPos++)
		fInputs[nIndex++]		= (NVALUE) ((m_pHand->GetNumCardsInSuit(2) > nPos)? m_pHand->GetSuit(2).GetAt(nPos)->GetFaceValue() : 0);

	// 287-299: Spade holdings
	for(nPos=0;nPos<13;nPos++)
		fInputs[nIndex++]		= (NVALUE) ((m_pHand->GetNumCardsInSuit(3) > nPos)? m_pHand->GetSuit(3).GetAt(nPos)->GetFaceValue() : 0);

	// additional inputs
	// bidding history for each position
	for(int nRound=0;nRound<6;nRound++)
	{
		int nPos = pDOC->GetDealer();
		for(int j=0;j<4;j++)
		{
			fInputs[nIndex++]		= (NVALUE) pDOC->GetBidByPlayer(nPos, nRound);
			nPos = GetNextPlayer(nPos);
		}
	}


	//
	// clear remaining inputs
	//
	while(nIndex < numInputs)
		fInputs[nIndex++] = (NVALUE) 0;

	// done
}








///////////////////////////////////////////////////////////////////////////
//
//
// Hand evaluation algorithms
//
//
///////////////////////////////////////////////////////////////////////////






//
// IsSuitOpenable()
//
BOOL CBidEngine::IsSuitOpenable(int nSuit, int nMinStrength)
{
	// sanity check
	if (!ISSUIT(nSuit))
		return FALSE;	
	//
	int numCardsRequired;
	if (pCurrConvSet->IsConventionEnabled(tid5CardMajors))
		numCardsRequired = 5;
	else
		numCardsRequired = 4;
	//
	if ((numCardsInSuit[nSuit] < numCardsRequired) ||
						(nSuitStrength[nSuit] < nMinStrength))
		return FALSE;

	// else all's OK
	return TRUE;
}




//
// IsSuitShiftable()
//
BOOL CBidEngine::IsSuitShiftable(int nSuit, int nMinStrength, int nMinCards) // NCR MinCards/MinStrength reversed in definition
{
	// sanity check
	if (!ISSUIT(nSuit))
		return FALSE;	
	//
	if ((numCardsInSuit[nSuit] < nMinCards) ||
						(nSuitStrength[nSuit] < nMinStrength))
		return FALSE;

	// else all's OK
	return TRUE;
}





//
// ValidateBid()
//
// make sure the proffered bid is higher than the last valid bid
// if not, make overcall if possible
//
int CBidEngine::ValidateBid(int& nBid, int nOvercallBid)
{
	CString strTemp;
	int nLVBid = pDOC->GetLastValidBid();
	int nSuit = (m_nBid-1) % 5;
	int nPartnersBid = m_pPartner->InquireLastBid();
	int nPartnersSuit = (nPartnersBid-1) % 5;

	// see if this really isn't a bid but rather advice
	if (m_bHintMode)
	{
		if (pDOC->IsBidValid(nBid)) 
		{
			return nBid;
		}
		else
		{
			strTemp.Format("But the bid is too low vs. the last bid of of %s, so we have to pass.",
							BTS(pDOC->GetLastValidBid()));
			Trace(strTemp);
			return BID_PASS;
		}
	}

	// else this is a real bid
	if (pDOC->IsBidValid(nBid)) 
	{
		// bid is okay; record count
		if ((nBid > BID_PASS) & (nBid < BID_DOUBLE))
			m_numBidsMade++;
	} 
	else if (nOvercallBid > nLVBid) 
	{
		//
		strTemp.Format("But opponent bid %s, so overcall at %s.",
						nBid,nOvercallBid);
		nBid = nOvercallBid;
		m_numBidsMade++;
	} 
	else 
	{	
		//
		if (nBid == nLVBid)
			strTemp.Format("But %s has already been bid, so we have to pass.",
							BTS(nBid));
		else if (nBid != BID_PASS)
			strTemp.Format("But our bid (%s) is too low vs. the last bid of of %s, so we have to pass.",
							BTS(nBid), BTS(pDOC->GetLastValidBid()));  // NCR showed our bid
		Trace(strTemp);
		nBid = BID_PASS;
	}
	m_numBidTurns++;

	// see if this is the opening bid for team
	if ((nBid > BID_PASS) && (nBid < BID_DOUBLE) && 
		(m_pPlayer->GetNumBidsMade() == 1) && (m_pPartner->GetNumBidsMade() == 0))  
	{
		m_nOpeningPosition = pDOC->GetNumBidsMade();
		m_bOpenedBiddingForTeam = TRUE;
	}
	//
	return nBid;
}


// NCR GetBidType() 
// Scan back thru history and determine what kind of bid this is
//
BidType CBidEngine::GetBidType(int nBid) {
	BidType bidType = BT_Open;    // default to open
	// Search back thru history to nBid
	int i; // NCR-FFS added here, removed below
	for(/*int*/ i = pDOC->GetNumBidsMade(); i >= 0; i--)
	{
		int aBid = pDOC->GetBidByIndex(i);
		if(aBid == nBid)
			break;  // found bid in question
	}
	if(i < 1)
		return BT_Open;					// nothing prior
	// have a bid by RHO, get it and check
	int aBid = pDOC->GetBidByIndex(i-1);
	if((aBid != BID_PASS) && (aBid != BID_DOUBLE)) // NCR-627 added double
	{
		bidType = BT_Overcall;
	    int gap = nBid - aBid;
		if(gap > 5 && gap < 10)
			bidType = (BidType)(bidType | BT_Jump); // add on Jump flag
		if(gap > 10)
			bidType = (BidType)(bidType | BT_Leap); // add on Leap flag
		return bidType;
	}
	if(i < 2)			// NCR-92 changed from <= to <		
		return BT_Open;					// nothing prior
	aBid = pDOC->GetBidByIndex(i-2);    // get partner's bid
	if(aBid != BID_PASS)
	{
		bidType = BT_Response;
	    int gap = nBid - aBid;
		if(gap > 5 && gap < 10)
			bidType = (BidType)(bidType | BT_Jump);
		if(gap > 10)
			bidType = (BidType)(bidType | BT_Leap);
		return bidType;
	}
	// Did LHO bid?
	if(i < 3)					
		return BT_Open;					// nothing prior
	aBid = pDOC->GetBidByIndex(i-3);    // get LHO's bid
	// NCR-590 Did pard double at 4 level?
	if((nBid == BID_DOUBLE) && (BID_LEVEL(aBid) >= 4))
		return BT_PenaltyDouble; // NCR-590
	if(aBid != BID_PASS)
	{
		bidType = BT_Overcall;
	    int gap = nBid - aBid;
		if(gap > 5 && gap < 10)
			bidType = (BidType)(bidType | BT_Jump);
		if(gap > 10)
			bidType = (BidType)(bidType | BT_Leap);
		return bidType;
	}
	return bidType;
}  // NCR end GetBidType()


//
BOOL CBidEngine::IsGameBid(int nBid)
{
	int nSuit = BID_SUIT(nBid);
	int nLevel = BID_LEVEL(nBid);
	//
	// modify this later to take partial scores into account
	//
	if ( (ISMAJOR(nSuit) && (nLevel >= 4)) ||
		 (ISMINOR(nSuit) && (nLevel >= 5)) ||
		 ((nSuit == NOTRUMP) && (nLevel >= 3)) )
		return TRUE;
	else
		return FALSE;
}


//
//
//
int CBidEngine::GetGameBid(int nSuit)
{
	if (ISMAJOR(nSuit))
		return MAKEBID(nSuit, 4);
	else if (ISMINOR(nSuit))
		return MAKEBID(nSuit, 5);
	else
		return BID_3NT;
}


//
// PlayerOpenedSuit()
//
// returns TRUE is player opened the suit, or FALSE if partner did
//
BOOL CBidEngine::PlayerOpenedSuit(int nSuit)
{
	if (!ISSUIT(nSuit))
		return FALSE;
	//
	int nPlayerPos = m_pPlayer->GetPosition();
	int nPartnerPos = m_pPartner->GetPosition();
	int numBidsMade = pDOC->GetNumBidsMade();
	int nPos = pDOC->GetDealer();
	for(int i=0;i<numBidsMade;i++)
	{
		int nBid = pDOC->GetBidByIndex(i);
		if (ISBID(nBid) && (BID_SUIT(nBid) == nSuit))
		{
			if (nPos == nPlayerPos)
				return TRUE;
			else if (nPos == nPartnerPos)
				return FALSE;
		}
		nPos = GetNextPlayer(nPos);
	}
	//
	return FALSE;
}


//
double CBidEngine::GetGamePoints(int nSuit)
{
	if (ISMAJOR(nSuit))
		return PTS_MAJOR_GAME;
	else if (ISMINOR(nSuit))
		return PTS_MINOR_GAME;
	else
		return PTS_NT_GAME;
}


//
void CBidEngine::CancelBidding()
{
	TraceNH("***Bidding Cancelled***");
	Clear();
}



//
void CBidEngine::RestartBidding()
{
	CString strLine;
	TraceNH("***Bidding restarted***");
	Clear();
}



//
void CBidEngine::RecordBid(int nPos, int nBid)
{
	CString strTemp;
	if (nPos != m_pPlayer->GetPosition()) 
	{
		if (nBid != BID_PASS)
		{
			if ((nBid == BID_DOUBLE) || (nBid == BID_REDOUBLE))
				strTemp.Format("%s %s.",PositionToString(nPos),
				((nBid == BID_DOUBLE)? "doubles" : "redoubles"));
			else
				strTemp.Format("%s bids %s.",PositionToString(nPos),
										   BidToShortString(nBid));
			// see if this is partner opening the bidding for the team
			if ((&PLAYER(nPos) == m_pPartner) && (!m_bOpenedBiddingForTeam))
				m_bPartnerOpenedForTeam = TRUE;
		}
		else
		{
			strTemp.Format("%s passes.",PositionToString(nPos));
		}
		TraceNH(strTemp);
	} 
	else 
	{
		if (nBid == BID_PASS) 
		{
			strTemp.Format("We pass.");
			TraceNH(strTemp);
		}
		else  // NCR Show our bid
		{
			strTemp.Format("We bid %s.",BidToShortString(nBid)); 
			TraceNH(strTemp);
		}
	}
	TraceNH("--------------------");
}



//
void CBidEngine::BiddingFinished()
{
	pMAINFRAME->SuspendHints();
	CString strLine;
	strLine.Format("Bidding finished; contract is %s.  Declarer is %s; %s leads.",
					ContractToFullString(pDOC->GetContract(), pDOC->GetContractModifier()),
					PositionToString(pDOC->GetDeclarerPosition()),
					PositionToString(pDOC->GetRoundLead()));
	strLine += "\n=======================";
	TraceNH(strLine);
	pMAINFRAME->ResumeHints();
}








//
// GetCheapestShiftBid()
//
// returns the lowest level suit shift bid
//
int CBidEngine::GetCheapestShiftBid(int nTargetSuit, int nOldBid)
{
//	if(!ISSUIT(nTargetSuit)) // NCR-138 removed by NCR-189 THIS disables NT ???
//		return BID_PASS; // NCR can't compare to non suit

	// sanity check
	if ((nOldBid == BID_DOUBLE) || (nOldBid == BID_REDOUBLE))
		return BID_PASS;
	//
	if (nOldBid == BID_NONE)
		nOldBid = nPartnersBid;
	//
	// NCR need to test if RHO has bid!!
	if(ISBID(nRHOBid)) // (nRHOBid != BID_PASS) && (nRHOBid != BID_DOUBLE) && (nRHOBid != BID_REDOUBLE))
		nOldBid = nRHOBid;    // NCR this is the bid (by RHO) to beat
	int nOldLevel = ((nOldBid-1) / 5) + 1;
	int nOldSuit = (nOldBid-1) % 5;

	// make sure we don't run out of room!
	if ((nTargetSuit < nOldSuit) && (nOldLevel == 7))
		return BID_PASS;	// oops

	// else get the bid
	if (nTargetSuit > nOldSuit)
		return MAKEBID(nTargetSuit, nOldLevel);
	else
		return MAKEBID(nTargetSuit, nOldLevel+1);
}





//
// GetJumpShiftBid()
//
// returns the proper jump bid
//
int CBidEngine::GetJumpShiftBid(int nTargetSuit, int nOldBid, int nJumpLevel)
{
	int nOldLevel = ((nOldBid-1) / 5) + 1;
	int nOldSuit = (nOldBid-1) % 5;

	// the jump shift in a new suit is forcing to game
	int nPreviousSuit = (m_nBid-1) % 5;
	if (nTargetSuit != nPreviousSuit)
		m_bGameForceActive = TRUE;		

	// determine the correct bid level and return it
	int nLevel;
	if (nTargetSuit > nOldSuit)
		nLevel = nOldLevel+nJumpLevel;
	else
		nLevel = nOldLevel+nJumpLevel+1;

	// but make sure we don't run out of room!
	if (nLevel > 7)
		nLevel = 7;

	//	
	if (nTargetSuit > nOldSuit)
		return MAKEBID(nTargetSuit, nOldLevel+nJumpLevel);
	else
		return MAKEBID(nTargetSuit, nOldLevel+nJumpLevel+1);
}







//
// PickSuperiorSuit()
//
// compare two suits and select the one that is superior
//
// nArbitrate:  what to return if suits are equal
//   SP_NONE:  return NONE
//   SP_FIRST:  return first suit
//   SP_SECOND:  return second suit
//
int CBidEngine::PickSuperiorSuit(int nSuit1, int nSuit2, int nArbitrate)
{
	// sanity check
/*
	ASSERT(ISSUIT(nSuit1) && ISSUIT(nSuit2));
	VERIFY(ISSUIT(nSuit1) || ISSUIT(nSuit2));
*/
	if (ISSUIT(nSuit1) && !ISSUIT(nSuit2))
		return nSuit1;
	if (ISSUIT(nSuit2) && !ISSUIT(nSuit1))
		return nSuit2;

	// better suits = lower rank in the sorted quality list
	if (m_pHand->GetSuitRank(nSuit1) < m_pHand->GetSuitRank(nSuit2))
		return nSuit1;
	else
		return nSuit2;
//	return RankSuits(2, nArbitrate, nSuit1, nSuit2);
/*
	if (numCardsInSuit[nSuit1] > numCardsInSuit[nSuit2]) 
	{
		// return suit 1 if it's longer 
		return nSuit1;
	} 
	else if (numCardsInSuit[nSuit2] > numCardsInSuit[nSuit1]) 
	{
		// else suit 2 if it's longer 
		return nSuit2;
	} 
	else 
	{
		// suits are of the same length; check high cards
		if (m_numSuitPoints[nSuit1] > m_numSuitPoints[nSuit2]) 
		{
			return nSuit1;
		} 
		else if (m_numSuitPoints[nSuit2] > m_numSuitPoints[nSuit1]) 
		{
			return nSuit2;
		} 
		else 
		{
			// same length & same # of HCPs; see if
			// one is a minor and one a major
			if ((ISMAJOR(nSuit1)) && (ISMINOR(nSuit2)))
				return nSuit1;
			if ((ISMAJOR(nSuit2)) && (ISMINOR(nSuit1)))
				return nSuit2;
			// else use arbitration
			if (nArbitrate == SP_FIRST)
				return nSuit1;
			if (nArbitrate == SP_SECOND)
				return nSuit2;
			else
				return NONE;
		}
	}
*/
}





//
// GetLongerSuit()
//
// get the longer suit of the two
//
// nArbitrate:  what to return if suits are equal in length
//   SP_NONE:  return NONE
//   SP_FIRST:  return first suit
//   SP_SECOND:  return second suit
//
int CBidEngine::GetLongerSuit(int nSuit1, int nSuit2, int nArbitrate)
{
	// sanity check
	if (ISSUIT(nSuit1) && !ISSUIT(nSuit2))
		return nSuit1;
	if (ISSUIT(nSuit2) && !ISSUIT(nSuit1))
		return nSuit2;

	// 
	if (m_pHand->GetNumCardsInSuit(nSuit1) >= m_pHand->GetNumCardsInSuit(nSuit2))
		return nSuit1;
	else
		return nSuit2;
}




//
// GetBestSuitofAtLeast()
//
// return the best suit of length at least nLen
//
int CBidEngine::GetBestSuitofAtLeast(int nSuit1, int nSuit2, int nLen, int nArbitrate)
{
	if ((nLen < 1) || (nLen > 13) || (m_pHand->GetNumSuitsOfAtLeast(nLen) == 0)) 
	{
		CString strTemp;
		strTemp.Format("Error in GBSOAL(%d,%d,%d)",nSuit1,nSuit2,nLen);
		AfxMessageBox(strTemp);
	}
	// first check if one suit doesn't meet the length requirement
	if ((numCardsInSuit[nSuit1] >= nLen) && (numCardsInSuit[nSuit2] < nLen))
		return nSuit1;
	if ((numCardsInSuit[nSuit2] >= nLen) && (numCardsInSuit[nSuit1] < nLen))
		return nSuit2;
	// else return the better suit
	return (PickSuperiorSuit(nSuit1,nSuit2,nArbitrate));
}




//
// GetLowestOpenableBid()
//
// returns the lowest openable suit in the given category
//
// arguments:
// ----------
//   nSuit:  0=any, 1=minor, 2=majors
//   nType:  0=marginal opener or higher, 1=openable suit, 
//			2=solid suit, 3=preferred suit, 4=absolute
//   nLevel = bid level (1-7)
//
int CBidEngine::GetLowestOpenableBid(int nSuitType, int nOpenType, int nLevel)
{
	if (nSuitType == SUITS_ANY) 
	{

		// return lowest suit (any)
		if (nOpenType == OT_ANY)
			return(MAKEBID(m_pHand->GetLowestMarginalSuit(),nLevel));
		else if (nOpenType == OT_OPENER)
			return(MAKEBID(m_pHand->GetLowestOpenableSuit(),nLevel));
		else if (nOpenType == OT_STRONG)
			return(MAKEBID(m_pHand->GetLowestSolidSuit(),nLevel));
		else if (nOpenType == OT_ABSOLUTE)
			return(MAKEBID(m_pHand->GetLowestAbsoluteSuit(),nLevel));
		else
			return BID_PASS;

	} 
	else if (nSuitType == SUITS_MINORS) 
	{

		// return openable minor if possible
		if (nOpenType == OT_ANY)
			return(MAKEBID(m_pHand->GetLowestMarginalSuit(),nLevel));
		else if (nOpenType == OT_OPENER)
			return(MAKEBID(m_pHand->GetLowestOpenableSuit(),nLevel));
		else if (nOpenType == OT_STRONG)
			return(MAKEBID(m_pHand->GetLowestSolidSuit(),nLevel));
		else if (nOpenType == OT_ABSOLUTE)
			return(MAKEBID(m_pHand->GetLowestAbsoluteSuit(),nLevel));
		else
			return BID_PASS;

	} 
	else if (nSuitType == SUITS_MAJORS) 
	{

		// return lowest openable major
		if (nOpenType == OT_ANY) 
		{
			//
			if (nSuitStrength[HEARTS] >= SS_MARGINAL_OPENER)
				return MAKEBID(HEARTS,nLevel);
			else if (nSuitStrength[SPADES] >= SS_MARGINAL_OPENER)
				return MAKEBID(SPADES,nLevel);
			// NCR-326 Open stronger minor vs doing it in reverse order: clubs then diamonds
			else if ((nSuitStrength[DIAMONDS] >= SS_MARGINAL_OPENER)
				     && (nSuitStrength[DIAMONDS] >= nSuitStrength[CLUBS])) 
				return MAKEBID(DIAMONDS, nLevel);
			else if (nSuitStrength[CLUBS] >= SS_MARGINAL_OPENER) // NCR-326 changed these to CLUBS vs DIAMONDS
				return MAKEBID(CLUBS, nLevel);
			else
				return BID_PASS;
		} 
		else if (nOpenType == OT_OPENER) 
		{
			//
			if (nSuitStrength[HEARTS] >= SS_OPENABLE)
				return MAKEBID(HEARTS,nLevel);
			else if (nSuitStrength[SPADES] >= SS_OPENABLE)
				return MAKEBID(SPADES,nLevel);
			else if (nSuitStrength[CLUBS] >= SS_OPENABLE)
				return MAKEBID(CLUBS,nLevel);
			else if (nSuitStrength[DIAMONDS] >= SS_OPENABLE)
				return MAKEBID(DIAMONDS,nLevel);
			else
				return BID_PASS;
		} 
		else if (nOpenType == OT_STRONG) 
		{
			//
			if (nSuitStrength[HEARTS] >= SS_STRONG)
				return MAKEBID(HEARTS,nLevel);
			else if (nSuitStrength[SPADES] >= SS_STRONG)
				return MAKEBID(SPADES,nLevel);
			else if (nSuitStrength[CLUBS] >= SS_STRONG)
				return MAKEBID(CLUBS,nLevel);
			else if (nSuitStrength[DIAMONDS] >= SS_STRONG)
				return MAKEBID(DIAMONDS,nLevel);
			else
				return BID_PASS;
		} 
		else if (nOpenType == OT_ABSOLUTE) 
		{
			//
			if (nSuitStrength[HEARTS] >= SS_ABSOLUTE)
				return MAKEBID(HEARTS,nLevel);
			else if (nSuitStrength[SPADES] >= SS_ABSOLUTE)
				return MAKEBID(SPADES,nLevel);
			else if (nSuitStrength[CLUBS] >= SS_ABSOLUTE)
				return MAKEBID(CLUBS,nLevel);
			else if (nSuitStrength[DIAMONDS] >= SS_ABSOLUTE)
				return MAKEBID(DIAMONDS,nLevel);
			else
				return BID_PASS;
		} 
		else 
		{
			return BID_PASS;
		}
	} 
	else 
	{
		return BID_PASS;
	}
}





//
// GetBestOpeningSuit()
//
// returns the best opening suit
//
//
int CBidEngine::GetBestOpeningSuit()
{
	CString strTemp;
	// set the default suit
	int nSuit = nPrefSuit;

	// see if we're playing 5-card majors
	if (pCurrConvSet->IsConventionEnabled(tid5CardMajors)) 
	{

		// a major suit needs 5 cards to be openable
		// see if the preferred suit is a 5-card major
		if ((ISMAJOR(nSuit)) && (numCardsInSuit[nSuit] >= 5)) 
		{
			// see if there are actually two 5-card majors
			if ((numCardsInSuit[HEARTS] >= 5) &&
							(numCardsInSuit[SPADES] >= 5)) 
			{
				return GetBestSuitofAtLeast(SPADES,HEARTS,5,SP_FIRST);
			} 
			else 
			{
				// only one 5-card major; return it
				return nSuit;
			}
		}
		// or can return a 6-card preferred minor suit
		if ((ISMINOR(nSuit)) && (numCardsInSuit[nSuit] >= 6)) 
		{
			return nSuit;
		}
		// else seek a 5-card major
		if ((numCardsInSuit[HEARTS] >= 5) && (numCardsInSuit[SPADES] >= 5)) 
		{
			// two 5-card majors; return the better one
			// pick the higher suit if they're equal
			return PickSuperiorSuit(HEARTS,SPADES,SP_SECOND);
		} 
		else if (numCardsInSuit[SPADES] >= 5) 
		{
			// Spades has 5 cards
			return SPADES;
		}  
		else if (numCardsInSuit[HEARTS] >= 5) 
		{
			// Hearts has 5 cards
			return HEARTS;
		} 
		else 
		{
			// return a minor suit
			nSuit = PickSuperiorSuit(CLUBS,DIAMONDS,SP_SECOND);
			// NCR-650 make sure at least 3 cards
			if(numCardsInSuit[nSuit] > 2)
				return nSuit;
            if(numCardsInSuit[CLUBS] > numCardsInSuit[DIAMONDS])  // NCR-650 Assume 2 & 3 cards in minors
				return CLUBS;
			else
				return DIAMONDS;
			// NCR-650 End ensuring 3 cards

		}

	} else {
		
		// playing normal (4-card) majors
		// see if we have > 1 preferred suit
		if (m_pHand->GetNumPreferredSuits() >= 3) 
		{
			// 3 (but not likely 4) good suits
			// bid the middle suit
			strTemp.Format("BPK1! With %d good suits (%s, %s, and %s), bid the middle suit.",
							m_pHand->GetNumPreferredSuits(),
							STS(m_pHand->GetSuitsByPreference(2)),
							STS(m_pHand->GetSuitsByPreference(1)),
							STS(m_pHand->GetSuitsByPreference(0)));
			Trace(strTemp);
			return m_pHand->GetSuitsByPreference(1);
		} 
		else if (m_pHand->GetNumPreferredSuits() == 2) 
		{
			// bid the higher of 2 preferred suits
			strTemp.Format("BPK2! With 2 good suits (%s and %s), bid the higher suit.",
							STS(m_pHand->GetSuitsByPreference(1)),
							STS(m_pHand->GetSuitsByPreference(0)));
			Trace(strTemp);
			return m_pHand->GetSuitsByPreference(0);
		} 
		else 
		{
			// else just return the lowest preferred suit
			// NCR-693 Make sure a major suit has 4 cards
			if(ISMAJOR(nSuit) && (numCardsInSuit[nSuit] > 3) )
				return nSuit;
			// NCR-694 Check if 3 cards and second preferred is 4card major
			if(numCardsInSuit[nSuit] < 4) {
				nSuit = nPrefSuitList[1];
				if(ISMAJOR(nSuit) && (numCardsInSuit[nSuit] > 3) )
					return nSuit;  // NCR-694 use it
			} // end NCR-694
			// NCR-693 find another one
			for(int i=0; i < 4; i++) {
				nSuit = m_pHand->GetSuitsByPreference(i);
				if(ISMAJOR(nSuit) ) {
					if(numCardsInSuit[nSuit] > 3)
						return nSuit;
					continue;    // go to next suit
				}
				return nSuit; // NCR-693 return any minor
			}
			ASSERT(false);   //  TEST if this ever happens
			return nSuit;   // Compiler needs this one
		}

	}

}





//
// GetRebidSuit()
//
// returns the best rebid suit
//
//
int CBidEngine::GetRebidSuit(int nPrevSuit)
{
	// first see if the prev suit is an absolute one
	if (nSuitStrength[nPrevSuit] >= SS_ABSOLUTE)
		return nPrevSuit;

	// else see if we have another preferred suit
	if (m_pHand->GetNumPreferredSuits() > 1) 
	{
		if (nPrevSuit == m_pHand->GetSuitsByPreference(0))
			return m_pHand->GetSuitsByPreference(1);
		else
			return m_pHand->GetSuitsByPreference(0);
	}

	// else see if we have another decent suit we can bid
	// i.e., 4+ cards and 3+ HCPs
	int i,nSuit;
	for(i=0;i<4;i++) 
	{
		nSuit = m_pHand->GetSuitsByPreference(i);
		if (nSuit == nPrevSuit)
			continue;
		if ((m_pHand->GetSuitLength(nSuit) >= 4) && (m_pHand->GetSuitHCPoints(nSuit) >= 3))
			break;
	}
	if (i < 4)
		return nSuit;

	// otherwise return the prev suit if it's rebiddable
	if (bPreviousSuitIsRebiddable)
		// rebid the previous suit
		return nPrevSuit;

	// else we have no available rebids
	return NONE;

}





//
// GetNextBestSuit()
//
// returns a suit that is next best to the specified suit,
// AND which is not the same as the partner's suit
// (if one is specified)
//
//
int CBidEngine::GetNextBestSuit(int nPrevSuit, int nPartnersSuit)
{
	int i,nSuit;
	//
	for(i=0;i<4;i++) 
	{
		nSuit = m_pHand->GetSuitsByPreference(i);
		if ((nSuit != nPrevSuit) && (nSuit != nPartnersSuit))
			break;
	}
	return nSuit;
}





//
// GetFourthSuit()
//
// returns the fourth suit (the one not named in the list)
//
int CBidEngine::GetFourthSuit(int nSuit1, int nSuit2, int nSuit3)
{
	int i;
	//
	for(i=0;i<4;i++) 
	{
		if ((i == nSuit1) || (i == nSuit2) || (i == nSuit3))
			continue;
		else
			break;
	}
	ASSERT(i < 4);
	return i;
}




//
// TestReverseRules()
//
// - called by the opener while making a rebid to 
// see if we might be violating rules on a reverse
// return code:
//    TRUE:  suit is okay to bid
//    FALSE: suit cannot be bid, and another one was chosen
//
BOOL CBidEngine::TestReverseRules(int& nSuit)
{
	// are structured reversed enabled?
	if (pCurrConvSet->IsOptionEnabled(tbStructuredReverses))
	{
		// if so, we need 17+ pts to reverse
		if ((nSuit > nPreviousSuit) && (fPts < 17))
		{
			// oops, reversing violation -- gotta find another suit (the 3rd one) 
			*m_pStatusDlg << "2RevV0! We'd like to bid the " & STSS(nSuit) &
							 " suit, but we can't reverse with less than 17 points.\n";
			int newSuit = GetNextBestSuit(nSuit, nPreviousSuit);
			// if this suit is also higher than the original suit, 
			// then try the fourth suit
//			if ((newSuit > nPreviousSuit) || (nSuitStrength[newSuit] < SS_MARGINAL_OPENER))
			if (newSuit > nPreviousSuit)
			{
				newSuit = GetFourthSuit(newSuit, nSuit, nPreviousSuit);
//				*m_pStatusDlg << "2RevV1! So with no other good suits available, we may have to rebid the original suit.\n";
			}
			else
			{
//				*m_pStatusDlg << "2RevV2! So we'll have to try another suit.\n";
			}
			if (nSuitStrength[newSuit] < SS_OPENABLE)
			{
				// new suit not biddable? have to rebid original suit
				*m_pStatusDlg << "2RevV2! And since the 3rd best suit (" & STS(newSuit) &
								 ") is not strong enough to bid, we have to rebid the original " & 
								 STSS(nPreviousSuit) & " suit.\n";
				newSuit = nPreviousSuit;
			}
			//
			nSuit = newSuit;
			return FALSE;
		}
		else
		{
			// no problemo
			return TRUE;
		}
	}
	// no restrictions in effect
	return TRUE;
}




//
// CountModifiedPlayingTricks()
//
// counts playing tricks with the named suit as trump
// this count is more precise than the initial count performed
// after the deal
//
//
int CBidEngine::CountModifiedPlayingTricks(int nSuit)
{
	int i;
	double fWinners = 0;

	//
	for(i=0;i<4;i++) {

		int nCount = numCardsInSuit[i];
		int nCard0 = 0;
		int nCard1 = 0;
		int nCard2 = 0;
		if (nCount >= 3)
			nCard2 = m_pHand->GetCardInSuit(i,2)->GetFaceValue();
		if (nCount >= 2)
			nCard1 = m_pHand->GetCardInSuit(i,1)->GetFaceValue();
		if (nCount >= 1)
			nCard0 = m_pHand->GetCardInSuit(i,0)->GetFaceValue();
		
		//
		if (nCount == 0) 
		{
			// no winners or losers here
		}

		// 1-card suit
		if (nCount == 1) 
		{
			// Ace == 1 winner
			if (nCard0 == ACE)
				fWinners++;
		}

		// 2-card suit
		if (nCount == 2) 
		{
			// AK == 2 winners
			if ((nCard0 == ACE) && (nCard1 == KING))
				fWinners += 2.0;
			// AQ == 1.5 winners
			else if ((nCard0 == ACE) && (nCard1 == QUEEN))
				fWinners += 1.5;
			// KQ == 1.0 winners
			else if ((nCard0 == KING) && (nCard1 == QUEEN))
				fWinners += 1.0;
			// Kx == 0.5 winners
			else if ((nCard0 == KING) && (nCard1 < QUEEN))
				fWinners += 0.5;
		} 
		
		// 3+ card suit -- count high card winners
		if (nCount >= 3) 
		{
			// AKQ == 3 winners
			if ((nCard0 == ACE) && (nCard1 == KING) && (nCard2 == QUEEN))
				fWinners += 3.0;
			// AKJ, AQJ == 2.5 winners
			else if (((nCard0 == ACE) && (nCard1 == KING) && (nCard2 == JACK)) ||
				 	 ((nCard0 == ACE) && (nCard1 == QUEEN) && (nCard2 == JACK)) )
				fWinners += 2.5;
			// AKx, AQT, AJT, KQJ = 2.0 winners
			else if (((nCard0 == ACE) && (nCard1 == KING)) ||
					 ((nCard0 == ACE) && (nCard1 == QUEEN) && (nCard2 == 10)) ||
					 ((nCard0 == ACE) && (nCard1 == JACK) && (nCard2 == 10)) ||
					 ((nCard0 == KING) && (nCard1 == QUEEN) && (nCard2 == JACK)) )
				fWinners += 2.0;
			// AQx, KQT, KJT == 1.5 winners
			else if (((nCard0 == ACE) && (nCard1 == QUEEN)) ||
					 ((nCard0 == KING) && (nCard1 == QUEEN) && (nCard2 == 10)) ||
				 	 ((nCard0 == KING) && (nCard1 == JACK) && (nCard2 == 10)) )
				fWinners += 1.5;
			// Axx, KQx, KJx, QJT == 1.0 winners
			else if ((nCard0 == ACE) ||
					 ((nCard0 == KING) && (nCard1 == QUEEN)) ||
					 ((nCard0 == KING) && (nCard1 == JACK)) ||
				 	 ((nCard0 == QUEEN) && (nCard1 == JACK) && (nCard2 == 10)) )
				fWinners += 1.0;
			// Kxx, QJx == 0.5 winners
			else if ((nCard0 == KING) ||
					 ((nCard0 == QUEEN) && (nCard1 == JACK) && (nCard2 < JACK)) )
				fWinners += 0.5;
		}

		// now cound long suits, as appropriate
		if (nCount >= 4) 
		{
			// 
			if (i == nSuit) 
			{
				// 1 winner for each card over 3 in the trump suit
				fWinners += (nCount - 3);
			} 
			else 
			{
				// count 0.5 trick for 4th card in a side suit, 
				// or 1 pt for each card over 5 in a side suit
				if (nCount == 4)
					fWinners += 0.5;
				else
					fWinners += (nCount - 4) + 0.5;
			}
		}

	}
	//
//	return (int) ROUND(fWinners);
	return (int) fWinners;
}





//
// TestForPenaltyDouble()
//
// see if we should double for penalties
//
BOOL CBidEngine::TestForPenaltyDouble()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;

	// see if contract is already doubled or redoubled
	if (pDOC->GetContractModifier() != 0)
		return FALSE;

	// see which side made the last bid
	if (pDOC->GetLastValidBidTeam() == GetPlayerTeam(m_pPlayer->GetPosition()))
		return FALSE;	// can't double our own contract

	// make sure a valid bid was indeed made
	int nBid = pDOC->GetLastValidBid();
	if (!ISBID(nBid))
		return FALSE;

	// OK, here's the nitty-gritty
	// estimate how many tricks we can win vs. how many we need to win
	int numTricksRequired = 8 - BID_LEVEL(nBid);
	int numLikelyTricks = 0;
	int nTrumpSuit = BID_SUIT(nBid);

	// is this a suit contract?
	if (ISSUIT(nTrumpSuit))
	{
		// add 1 likely trick for each winner in the trump suit card
		CSuitHoldings& trumpSuit = m_pHand->GetSuit(nTrumpSuit);
		// do a specific calculation to estimate winners in the trump suit
		int numTrumps = trumpSuit.GetNumCards();
		// an Ace is 1 winner
		if (trumpSuit.HasCardOfFaceValue(ACE))
			numLikelyTricks++;
		// a Kx is a winner
		if ((numTrumps >= 2) && trumpSuit.HasCardOfFaceValue(KING))
			numLikelyTricks++;
		// a Qxx is a winner
		if ((numTrumps >= 3) && trumpSuit.HasCardOfFaceValue(QUEEN))
			numLikelyTricks++;
		// a Jxxx is a winner
		if ((numTrumps >= 4) && trumpSuit.HasCardOfFaceValue(JACK))
			numLikelyTricks++;
		// and each trump over 5 is also a likely winner
		numLikelyTricks += Max(0, numTrumps-5);
	}

	// add the # of quick tricks in the other suits (rounded down)
	for(int i=0;i<4;i++)
	{
		if (i == nTrumpSuit)
			continue;
		CSuitHoldings& suit = m_pHand->GetSuit(i);
		numLikelyTricks += (int)suit.GetNumQuickTricks();
	}

	// test -- need n+1 tricks (i.e., can set by 2 tricks) to double
	if (numLikelyTricks >= (numTricksRequired+1))
	{
		// double!
		status << "2TSTDBL4! But upon reconsideration, we have at least " & numLikelyTricks & 
				  " likely tricks in the hand, so we can double for penalties.\n";
		return TRUE;
	}
	else
	{
		// can't double
		return FALSE;
	}
}



	
//
// TestForRedouble();
//
BOOL CBidEngine::TestForRedouble()
{
	// check to make sure the contract is doubled 
	if (pDOC->GetContractModifier() != 1)
		return FALSE;

	// temp
	return FALSE;
}






//
//---------------------------------------------------------------
//
// CalcNoTrumpPoints()
//
// Calculate No Trump point holdings in the partnership
//
//	 
void CBidEngine::CalcNoTrumpPoints()
{
	CPlayerStatusDialog& status = *m_pStatusDlg;
	//
	double fMin, fMax;
	int nPreviousBid = m_nBid;
	int nPreviousBidLevel = ((nPreviousBid-1) / 5) + 1;
	int nPreviousSuit = (nPreviousBid-1) % 5;
	int nPartnersBid = m_pPartner->InquireLastBid();
	//
	char szPB[12],szPVB[12];
	strcpy(szPB,BTS(nPartnersBid));			// partner's bid
	strcpy(szPVB,BTS(nPreviousBid));		// our previous bid 
	//
	double fPts = m_pHand->m_numTotalPoints;
	double fCardPts = m_pHand->m_numHCPoints;
	//
/*
	if (nPreviousSuit != NOTRUMP)
		ASSERT(nPreviousBidLevel < 4);
	else
		ASSERT(nPreviousBidLevel < 6);
*/
	// NCR-671 Test if NT was to keep the bidding open
	if(((GetBidType(nPreviousBid) & BT_Jump) != 0) && (nPartnersOpeningBid == BID_PASS) ) {
		m_fPartnersMin = 7;
		m_fPartnersMax = 9;
		m_fMinTPPoints = fPts + m_fPartnersMin;
		m_fMaxTPPoints = fPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;
		status << "2NT05! Partner's response of " & szPB & " to our jump to " & szPVB & 
				  " keeping the bidding open indicates " & m_fPartnersMin &
				  "+ points, for a total of " & m_fMinTPPoints & 
				  "+ points in the partnership.\n";
		return;     // Done
	} // end NCR-671

	// see if this NT bid was in response to an opening or
	// responding bid
	BOOL bInitialResponse;
	if (m_pPartner->GetNumBidsMade() == 1)
		bInitialResponse = TRUE;
	else
		bInitialResponse = FALSE;
		
	//
	// after a raise of a NT bid, we can estimate partner's strength by 
	// determining what partner expects us to be holding.
	// e.g., if partner responds 3NT to our 1NT opening, then partner
	// knows we have 15+ HCPs, and the total must add up to 26+ HCPs, so
	// he must have at least 10 HCPs himself.
	//
	// here, fMin,fMax are the min & max HCPs that partner must assume we have
	//
	// but this only works if we opened the bidding, and with a NT bid -- i.e., 
	// in cases like 1x -> 2NT -> 3NT, the 2NT bid may not necessarily indicate 
	// 2NT opening value points in hand.
	//
	if ((nPreviousSuit == NOTRUMP) && (m_bOpenedBiddingForTeam))
	{
		switch(nPreviousBid) 
		{
			case BID_1NT:
				fMin = pCurrConvSet->GetNTRangeMin(1);
				fMax = pCurrConvSet->GetNTRangeMax(1);
				break;
			case BID_2NT:
				fMin = pCurrConvSet->GetNTRangeMin(2);
				fMax = pCurrConvSet->GetNTRangeMax(2);
				break;
			case BID_3NT:
				fMin = pCurrConvSet->GetNTRangeMin(3);
				fMax = pCurrConvSet->GetNTRangeMin(3);
				break;
			case BID_4NT:
			case BID_5NT:
			case BID_6NT:
				fMin = fMax = fCardPts;
				break;
		}
	} 
	else 
	{
		//
		// else we can't expect partner to assume anything
		//
		fMin = fCardPts;
		fMax = fCardPts;
	}

	//
	// check point count ranges for a NT raise
	//
	if (nPreviousSuit == NOTRUMP) 
	{
		switch (nPartnersBid) 
		{
			case BID_2NT:
				// invitational towards game, 24 HCPs min.
				m_fPartnersMin = (PTS_NT_GAME-2) - fMin;
				m_fPartnersMax = (PTS_NT_GAME-1) - fMin;
				m_fMinTPPoints = m_fMinTPCPoints = fCardPts + m_fPartnersMin;
				m_fMaxTPPoints = m_fMaxTPCPoints = fCardPts + m_fPartnersMax;;
				break;

			case BID_3NT:
				// game bid, 25 HCPs min
				m_fPartnersMin = PTS_NT_GAME - fMin;
				m_fPartnersMax = 30 - fMin;
				m_fMinTPPoints = m_fMinTPCPoints = fCardPts + m_fPartnersMin;
				m_fMaxTPPoints = m_fMaxTPCPoints = fCardPts + m_fPartnersMax;;
				break;

			case BID_4NT:
				// invitational towards a small slam
				m_bInvitedToSlam = TRUE;
				m_fPartnersMin = (PTS_SMALL_SLAM-2) - fMin;
				m_fPartnersMax = (PTS_SMALL_SLAM-1) - fMin;
				m_fMinTPPoints = m_fMinTPCPoints = fCardPts + m_fPartnersMin;
				m_fMaxTPPoints = m_fMaxTPCPoints = fCardPts + m_fPartnersMax;;
				break;

			case BID_5NT:
				// invitational towards a grand slam
				m_bInvitedToSlam = TRUE;
				m_fPartnersMin = (PTS_GRAND_SLAM-2) - fMin;
				m_fPartnersMax = (PTS_GRAND_SLAM-1) - fMin;
				m_fMinTPPoints = m_fMinTPCPoints = fCardPts + m_fPartnersMin;
				m_fMaxTPPoints = m_fMaxTPCPoints = fCardPts + m_fPartnersMax;;
				break;

			case BID_6NT:
				m_fPartnersMin = PTS_SMALL_SLAM - fMin;
				m_fPartnersMax = (PTS_SMALL_SLAM+1) - fMin;
				m_fMinTPPoints = m_fMinTPCPoints = fCardPts + m_fPartnersMin;
				m_fMaxTPPoints = m_fMaxTPCPoints = fCardPts + m_fPartnersMax;
				break;

			case BID_7NT:
				m_fPartnersMin = PTS_GRAND_SLAM - fMin;
				m_fPartnersMax = 40 - fMin;
				m_fMinTPPoints = m_fMinTPCPoints = fCardPts + m_fPartnersMin;
				m_fMaxTPPoints = m_fMaxTPCPoints = fCardPts + m_fPartnersMax;
				break;
		}
		//
		status << "2NT10! Partner's raise from our " & szPVB & " to " & szPB & 
				  " indicates a balanced hand and " & 
				  m_fPartnersMin & "-" & m_fPartnersMax & 
				  " HCPs, for a total of " & m_fMinTPCPoints & 
				  "+ HCPs in the partnership" &
				  ((m_bInvitedToSlam)? " and is invitational to slam." : ".") & "\n";
	} 
	else if ((nPartnersBid == BID_2NT) && (nPreviousBidLevel == 1)
			  && ((GetBidType(nPartnersBid) & BT_Overcall) == 0) ) // NCR test NOT overcall
	{

		// 1x -> 2NT jump shift
		if (bInitialResponse) 
		{
			// adjust for pt count variation
/*
			if ((m_nOpeningPosition == 2) || (m_nOpeningPosition == 3)) 
			{
				m_fPartnersMin = 13;
				m_fPartnersMax = 15;
			} 
			else 
			{
				m_fPartnersMin = 11;
				m_fPartnersMax = 13;
			}
*/
			// as per SAYC
			m_fPartnersMin = 13;
			m_fPartnersMax = 14;
		} 
		else 
		{
			m_fPartnersMin = 17;
			m_fPartnersMax = 19;
		}
		m_fMinTPPoints = fPts + m_fPartnersMin;
		m_fMaxTPPoints = fPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;
		m_bGameForceActive = TRUE;
		status << "2NT42! Partner's jump shift from our " & szPVB & " bid to " & 
				  szPB & " indicates a balanced hand and " & m_fPartnersMin & 
				  "+ points, for a total of " & m_fMinTPPoints & 
				  "+ points in the partnership, and is forcing to game.\n";

	} 
	else if ((nPartnersBid == BID_2NT)  // NCR also if partner overcalled 
			&& ((nPreviousBidLevel == 2) || ((GetBidType(nPartnersBid) & BT_Overcall) != 0)) ) 
	{

		// 2x -> 2NT shift
/*
		if (bInitialResponse) 
		{
			// asuume this is handled elsewhere in the 
			// follow-up to the 2-level opening
		} else {
*/
		// really, this really should be handled elsewhere
		// but the generic code assumes response to a strong 2
		if (bInitialResponse) 
		{
			m_fPartnersMin = 6;
			m_fPartnersMax = 18;
		}
		else
		{
			// e.g., 1H -> 2S -> 2NT
			// NCR-239 Root pg 50 says 13-16
			m_fPartnersMin = 13; //15;
			m_fPartnersMax = 16; //18;
		}
		m_fMinTPPoints = fPts + m_fPartnersMin;
		m_fMaxTPPoints = fPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;
		m_bGameForceActive = TRUE;
		status << "2NT43! Partner's shift from our " & szPVB & " bid to " & szPB &
				  " indicates a balanced hand and " & m_fPartnersMin & 
				  "+ points, for a total of " & m_fMinTPPoints & 
				  "+ points in the partnership.\n";

	} 
	else if ((nPartnersBid == BID_3NT)  // NCR-617 also if partner overcalled
			&& (nPreviousBidLevel == 1) && ((GetBidType(nPartnersBid) & BT_Overcall) == 0) ) 
	{

		// 1x -> 3NT shift
		if (bInitialResponse) 
		{
/*
			// adjust for pt count variation
			if ((m_nOpeningPosition == 2) || (m_nOpeningPosition == 3)) 
			{
				m_fPartnersMin = 16;
				m_fPartnersMax = 17;
			} 
			else 
			{
				m_fPartnersMin = 13;
				m_fPartnersMax = 14;
			}
*/
			// as per SAYC
			m_fPartnersMin = 15;
			m_fPartnersMax = 18;
		} 
		else 
		{
			m_fPartnersMin = 19;
			m_fPartnersMax = 22;
		}
		m_fMinTPPoints = fPts + m_fPartnersMin;
		m_fMaxTPPoints = fPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;
		m_bGameForceActive = TRUE;
		status << "2NT44! Partner's jump shift from our " & szPVB & " bid to " & szPB &
				  " indicates a balanced hand and " & m_fPartnersMin & 
				  "+ points, for a total of " & m_fMinTPPoints & 
				  "+ points in the partnership.\n";

	} 
	else if ((nPartnersBid == BID_3NT) && (nPreviousBidLevel == 2)) 
	{

		// 2x -> 3NT shift: 15+ pts
		m_fPartnersMin = 15;
		m_fPartnersMax = 22;
		m_fMinTPPoints = fPts + m_fPartnersMin;
		m_fMaxTPPoints = fPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;
		m_bGameForceActive = TRUE;
		status << "2NT45! Partner's jump shift from our " & szPVB & " bid to " & szPB &
				  " indicates a balanced hand and " & m_fPartnersMin & 
				  "+ points, for a total of " & 
				  m_fMinTPCPoints & "/" & m_fMinTPCPoints & 
				  "+ points in the partnership.\n";

	} 
	else if ((nPartnersBid == BID_3NT) && (nPreviousBidLevel == 3)) 
	{

		// 3x -> 3NT shift:  12+ points
		m_fPartnersMin = 12;
		m_fPartnersMax = 22;
		m_fMinTPPoints = fPts + m_fPartnersMin;
		m_fMaxTPPoints = fPts + m_fPartnersMax;
		m_fMinTPCPoints = fCardPts + m_fPartnersMin;
		m_fMaxTPCPoints = fCardPts + m_fPartnersMax;
		status << "2NT50! Partner's shift from our " & szPVB & " bid to " & szPB &
				  " indicates a balanced hand and " & m_fPartnersMin &
				  "+ points, for a total of " & m_fMinTPPoints & 
				  "+ points in the partnership.\n";

	} 
	else 
	{

		// don't understand the bid
		status << "1NT99! We don't understand the point of partner's raise from our " &
				  szPVB & " bid to " & szPB & ", so pass.\n";

	}

}




//
// IsBidSafe()
//
// - determines whether we have enough points to make the bid
//
BOOL CBidEngine::IsBidSafe(int nBid, double fAdjustment)
{
	int nLevel = BID_LEVEL(nBid);
	double fPoints;
	//
	if (BID_SUIT(nBid) == NOTRUMP)
	{
		// High-card points
		fPoints = m_fMinTPCPoints + fAdjustment;
		if ( ((nLevel == 1) && (fPoints < 16)) || 
			 ((nLevel == 2) && (fPoints < 20)) || 
			 ((nLevel == 3) && (fPoints < PTS_GAME-2+1)) || 
			 ((nLevel == 4) && (fPoints < PTS_MAJOR_GAME+1)) || 
			 ((nLevel == 5) && (fPoints < PTS_MINOR_GAME+1)) || 
			 ((nLevel == 6) && (fPoints < PTS_SMALL_SLAM+1)) ||
			 ((nLevel == 7) && (fPoints < PTS_GRAND_SLAM+1)) )
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		// Total pts 
		fPoints = m_fMinTPPoints + fAdjustment;
		if ( ((nLevel == 1) && (fPoints < 16)) || 
			 ((nLevel == 2) && (fPoints < 20)) || 
			 ((nLevel == 3) && (fPoints < PTS_GAME-2)) || 
			 ((nLevel == 4) && (fPoints < PTS_MAJOR_GAME)) || 
			 ((nLevel == 5) && (fPoints < PTS_MINOR_GAME)) || 
			 ((nLevel == 6) && (fPoints < PTS_SMALL_SLAM)) ||
			 ((nLevel == 7) && (fPoints < PTS_GRAND_SLAM)) )
			return FALSE;
		else
			return TRUE;
	}
}





//
//--------------------------------------------------------------
//
// Convention status functions
//


//
// SetConventionStatus() and GetConventionStatus(() use a map
// to associate convention status codes with each convention
//
void CBidEngine::SetConventionStatus(CConvention* pConvention, int nCode)
{
	// don't set the status if this is just a test bid!
	if (m_bInTestBiddingMode)
		return;

	//
	m_mapConventionStatus.SetAt(pConvention, nCode);
	// 
	if (nCode > 0)
	{
		m_pActiveConvention = pConvention;
	}
	else
	{
		// clearing this convention
		// clear main pointer if the current one is being deactivated
		if (pConvention == m_pActiveConvention)
			m_pActiveConvention = NULL;
	}
}

//
int CBidEngine::GetConventionStatus(CConvention* pConvention)
{
	int nResult;
	if (m_mapConventionStatus.Lookup(pConvention, nResult))
		return nResult;
	else
		return 0;
}

//
int CBidEngine::GetConventionStatus(int nConvention)
{
	POSITION pos = m_mapConventionStatus.GetStartPosition();
	CConvention* pConvention;
	int nResult;
	// do an associative search
	while(pos)
	{
		m_mapConventionStatus.GetNextAssoc(pos, pConvention, nResult);
		if (pConvention->GetID() == nConvention)
			return nResult;
	}
	//
	return CONV_INACTIVE;
}

//
void CBidEngine::ClearConventionStatus(CConvention* pConvention)
{
	if (pConvention)
	{
		// clear status of the specified convention
		m_mapConventionStatus.SetAt(pConvention, CONV_INACTIVE);
		m_mapConventionParameters.SetAt(pConvention, 0);
		if (m_pActiveConvention == pConvention)
			m_pActiveConvention = NULL;
	}
	else
	{
		// clear status of all conventions
		CConvention* pConvention;
		int nStatus;
		POSITION pos = m_mapConventionStatus.GetStartPosition();
		while(pos)
		{
			m_mapConventionStatus.GetNextAssoc(pos, pConvention, nStatus);
			m_mapConventionStatus.SetAt(pConvention, CONV_INACTIVE);
		}
	}
}



//
void CBidEngine::SetConventionParameter(CConvention* pConvention, int nParameter)
{
	m_mapConventionParameters.SetAt(pConvention, nParameter);
}

//
int CBidEngine::GetConventionParameter(CConvention* pConvention)
{
	int nResult;
	m_mapConventionParameters.Lookup(pConvention, nResult);
	return nResult;
}

//
void CBidEngine::ClearConventionParameter(CConvention* pConvention)
{
	m_mapConventionParameters.SetAt(pConvention, 0);
}




//
//================================================================
//
// Value Set/Retrieval
//

//
LPVOID CBidEngine::GetValuePV(int nItem, int nIndex1, int nIndex2, int nIndex3)
{
	switch (nItem)
	{
		case tnBid:
			return (LPVOID) m_nBid;
		case tnumBidTurns:
			return (LPVOID) m_numBidTurns;
		case tnumBidsMadeByPlayer:
			return (LPVOID) m_numBidsMade;
		case tnNextIntendedBid:
			return (LPVOID) m_nNextIntendedBid;
		case tnIntendedSuit:
			return (LPVOID) m_nIntendedSuit;
		case tnIntendedContract:
			return (LPVOID) m_nIntendedContract;
		// team info
		case tnumPartnersAces:
			return (LPVOID) m_numPartnersAces;
		case tnumPartnersKings:
			return (LPVOID) m_numPartnersKings;
		//
		case tnOpeningPosition:
			return (LPVOID) m_nOpeningPosition;
//		case tnPartnersSuit:
//			return (LPVOID) m_nPartnersSuit;
		case tnAgreedSuit:
			return (LPVOID) m_nAgreedSuit;
		case tbRoundForceActive:
			return (LPVOID) m_bRoundForceActive;
		case tbGameForceActive:
			return (LPVOID) m_bGameForceActive;
		case tbInTestBiddingMode:
			return (LPVOID) m_bInTestBiddingMode;
		//
		default:
			AfxMessageBox("Unhandled Call to CBidEngine::GetValue()");
			return NULL;
	}
	return NULL;
}

//
double CBidEngine::GetValueDouble(int nItem, int nIndex1, int nIndex2, int nIndex3)
{
	switch (nItem)
	{
		case tnMinimumTPPoints:  	
			return m_fMinTPPoints;
		case tnMinimumTPCPoints:	
			return m_fMinTPCPoints;
		case tnMaximumTPPoints:  
			return m_fMaxTPPoints;
		case tnMaximumTPCPoints:
			return m_fMaxTPCPoints;
		case tnPartnersMinimum:
			return m_fPartnersMin;
		case tnPartnersMaximum:
			return m_fPartnersMax;
		default:
			AfxMessageBox("Unhandled Call to CBidEngine::GetValue");
			return 1;
	}
	return 0;
}

//
// SetValuePV()
//
int CBidEngine::SetValuePV(int nItem, LPVOID value, int nIndex1, int nIndex2, int nIndex3)
{
	int nValue = (int) value;
	BOOL bValue = (BOOL) value;
	LPCTSTR sValue = (LPCTSTR) value;
	//
	switch (nItem)
	{
		case tnBid:
			m_nBid = nValue;
			break;
		case tnumBidTurns:
			m_numBidTurns = nValue;
			break;
		case tnumBidsMadeByPlayer:
			m_numBidsMade = nValue;
			break;
		case tnNextIntendedBid:
			m_nNextIntendedBid = nValue;
			break;
		case tnIntendedSuit:
			m_nIntendedSuit = nValue;
			break;
		case tnIntendedContract:
			m_nIntendedContract = nValue;
			break;
		// team info
		case tnumPartnersAces:
			m_numPartnersAces = nValue;
			break;
		case tnumPartnersKings:
			m_numPartnersKings = nValue;
			break;
		//
		case tnOpeningPosition:
			m_nOpeningPosition = nValue;
			break;
//		case tnPartnersSuit:
//			m_nPartnersSuit = nValue;
			break;
		case tnAgreedSuit:
			m_nAgreedSuit = nValue;
			break;
		case tbRoundForceActive:
			m_bRoundForceActive = bValue;
			break;
		case tbGameForceActive:
			m_bGameForceActive = bValue;
			break;
		case tbInTestBiddingMode:
			m_bInTestBiddingMode = !!bValue;
			break;
		// 
		default:
			AfxMessageBox("Unhandled Call to CBidEngine::SetValue()");
			return 1;
	}
	return 0;
}

// "double" version
int CBidEngine::SetValue(int nItem, double fValue, int nIndex1, int nIndex2, int nIndex3)
{
	switch (nItem)
	{
		case tnMinimumTPPoints:  	
			m_fMinTPPoints = fValue;
			break;
		case tnMinimumTPCPoints:	
			m_fMinTPCPoints = fValue;
			break;
		case tnMaximumTPPoints:  
			m_fMaxTPPoints = fValue;
			break;
		case tnMaximumTPCPoints:
			m_fMaxTPCPoints = fValue;
			break;
		case tnPartnersMinimum:
			m_fPartnersMin = fValue;
			break;
		case tnPartnersMaximum:
			m_fPartnersMax = fValue;
			break;
		default:
			AfxMessageBox("Unhandled Call to CBidEngine::SetValue()");
			return 1;
	}
	return 0;
}

//
int CBidEngine::GetValue(int nItem, int nIndex1, int nIndex2, int nIndex3)
{
	return (int) GetValuePV(nItem, nIndex1, nIndex2, nIndex3);
}

int CBidEngine::SetValue(int nItem, int nValue, int nIndex1, int nIndex2, int nIndex3)
{
	return SetValuePV(nItem, (LPVOID) nValue, nIndex1, nIndex2, nIndex3);
}

