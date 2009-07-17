//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// CBidEngine
//
#ifndef __CBIDENGINE__
#define __CBIDENGINE__


#include "Convention.h"
class CPlayer;
class CHandHoldings;
class CPlayerStatusDialog;
class CConvention;
class CCardLocation;
class CGuessedHandHoldings;

typedef float NVALUE;


//
class CBidEngine {

/*
	friend class CPlayer;
	friend class CConvention;			// this is messy, but unfortunately
	friend class CNaturalConvention;	// friendship is not inherited
	friend class CMajorsConvention;
	friend class CFiveCardMajorsConvention;
	friend class CStrongTwoBidsConvention;
	friend class CWeakTwoBidsConvention;
	friend class COgustConvention;
	friend class CPreemptiveBidsConvention;
	friend class CArtificial2ClubConvention;
	friend class CSlamConvention;
	friend class CBlackwoodConvention;
	friend class CCueBidConvention;
	friend class CJacobyTransferConvention;
	friend class CStaymanConvention;
*/

// public routines
public:
	//
	void Initialize(CPlayer* pPlayer, CPlayer* pPartner, CPlayer* pLHOpponent, CPlayer* pRHOpponent, CHandHoldings* pHoldings, CCardLocation* pCardLocation, CGuessedHandHoldings** ppGuessedHands, CPlayerStatusDialog* pStatusDlg);
	void Clear();
	//
	LPVOID GetValuePV(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValuePV(int nItem, LPVOID value, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	double GetValueDouble(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, double fValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int GetValue(int nItem, int nIndex1=0, int nIndex2=0, int nIndex3=0);
	int SetValue(int nItem, int nValue, int nIndex1=0, int nIndex2=0, int nIndex3=0);

	//
	// basic routines
	//
	void AssessPosition();
	void AdjustPartnershipPoints(double fPartnersMin=-1, double fPartnersMax=-1, bool bAdjustPoints=true);
	int Bid();
	int GetRuleBasedBid();
	void SetBid(int nBid) { m_nBid = nBid; }
	int	GetBiddingHint();
	void SetAgreedSuit(int nSuit) { m_nAgreedSuit = nSuit; }
	void CancelBidding();
	void RestartBidding();
	void RecordBid(int nPos, int nBid);
	void BiddingFinished();
	BOOL IsSuitOpenable(int nSuit, int nMinStrength=SS_MARGINAL_OPENER);
	BOOL IsSuitShiftable(int nSuit, int nMinStrength=SS_MARGINAL_OPENER, int nMinCards=4);
	BOOL IsGameBid(int nBid);
	int GetGameBid(int nSuit);
	double GetGamePoints(int nSuit);
	BOOL PlayerOpenedSuit(int nSuit);
	int GetNumBidsMade() { return m_numBidsMade; }
	int GetNumBidTurns() { return m_numBidTurns; }
	int ValidateBid(int& nBid, int nOvercallBid=BID_PASS);
	BidType GetBidType(int nBid);  // NCR
	int GetCheapestShiftBid(int nTargetSuit, int nOldBid=BID_NONE);
	int GetJumpShiftBid(int nTargetSuit, int nOldBid, int nJumpLevel=1);
	//BOOL IsSuitSolid(int nSuit);
	//BOOL IsSuitSelfSupporting(int nSuit);
	int PickSuperiorSuit(int nSuit1, int nSuit2, int nArbitrate=SP_FIRST);
	int GetLongerSuit(int nSuit1, int nSuit2, int nArbitrate=SP_FIRST);
	int GetBestSuitofAtLeast(int nSuit1, int nSuit2, int nLen, int nArbitrate=SP_SECOND);
	int PickBestFinalSuit(CPlayerStatusDialog& status);
//	BOOL AllOtherSuitsStopped(int nSuit1, int nSuit2=NONE, int nSuit3=NONE,
//							    BOOL bIncludeProbStoppers=TRUE);
	//
	int GetLowestOpenableBid(int nSuitType, int nOpenType, int nLevel);
	int GetBestOpeningSuit();
	int GetNextBestSuit(int nPrevSuit, int nPartnersSuit=NONE);
	int GetFourthSuit(int nSuit1, int nSuit2, int nSuit3);
	int GetRebidSuit(int nPrevSuit);
	BOOL TestReverseRules(int& nSuit);
	int CountModifiedPlayingTricks(int nSuit);
	void CalcNoTrumpPoints();
	BOOL IsBidSafe(int nBid, double fAdjustment=0);
	BOOL TestForPenaltyDouble();
	BOOL TestForRedouble();
	bool TestIfOpponentsBidThis(int nSuit);  // NCR-513
	//
	int GetOpeningPosition() { return m_nOpeningPosition; }
	BOOL OpenedBiddingForTeam() { return m_bOpenedBiddingForTeam; }
	// 
	int MakeOpeningBid();
	int MakeRespondingBid();
	int MakeRebidAsOpener();
	int MakeRebidAsResponder();
	int MakeRebidExtended();
	//
	int RespondToSingleRaise(int nPartnersBid);
	int RespondToDoubleRaise(int nPartnersBid);
	int RespondToTripleRaise();
	BOOL RebidSuit(int nSuitType=SUIT_ANY, RebidLevel enShiftLevel=REBID_CHEAPEST, 
				   double fMinPts=0, double fMaxPts=99, SuitLength enLength=LENGTH_ANY, 
				   int nStrength=SS_OPENABLE, HonorsCount enumHonors=HONORS_ANY, double numHonorPts=0);
	BOOL BidNextBestSuit(int nSuitType=SUIT_ANY, RebidLevel enShiftLevel=REBID_CHEAPEST, 
						 double fMinPts=0, double fMaxPts=99, SuitLength nLength=LENGTH_ANY,  
						 int nStrength=SS_NONE, HonorsCount enumHonors=HONORS_ANY, double numHonorPts=0,
						 int nSuit1=NONE, int nSuit2=NONE, int nSuit3=NONE);
	BOOL RaisePartnersSuit(int nSuitType=SUIT_ANY, RaiseLevel enLevel=RAISE_BY_ONE, 
						   double fMinPts=0, double fMaxPts=99, SupportLength enLength=SUPLEN_ANY,  
						   int nStrength=SS_NONE, HonorsCount eNumHonors=HONORS_ANY, double numHonorPts=0);
	BOOL BidNoTrump(int nLevel, double fMinPts=0, double fMaxPts=99,
					bool bMustBeBalanced=true, StoppedCode enCode=STOPPED_ALL, 
					int nSuit=NONE, bool bMustHaveNoVoids=true);
	BOOL BidNoTrumpAsAppropriate(bool bMustBeBalanced=true, StoppedCode enCode=STOPPED_ALL, int nSuit=NONE);

	// neural net stuff
	int		GetNeuralNetBid();
	double	GetNeuralNetError();
	int		GetNeuralNetTrainingCycles();
	void	SetNeuralNetBid(int nCorrectBid);
	void	FillNeuralNetInputs(NVALUE* fInputs, int numInputs);

	//
	LPCTSTR SuitHoldingsToString(int nSuit);
	LPCTSTR SuitStrengthToString(int nSuit);
	LPCTSTR SupportLevelToString(int nSuit);
	void Trace(const CString& strText);
	void TraceNH(const CString& strText);
	void AddTrace(CString& strMessage, CString& strTemp);
	void AddTrace(CString& strMessage, LPCTSTR szTemp);

	//
	// convention-related info
	//
	void SetConventionStatus(CConvention* pConvention, int nCode);
	int GetConventionStatus(CConvention* pConvention);
	int GetConventionStatus(int nConvention);
	void ClearConventionStatus(CConvention* pConvention=NULL);
	void SetConventionParameter(CConvention* pConvention, int nParameter);
	int GetConventionParameter(CConvention* pConvention);
	void ClearConventionParameter(CConvention* pConvention);
	void SetActiveConvention(CConvention* pConvention) { m_pActiveConvention = pConvention; }
	CConvention* GetActiveConvention() { return m_pActiveConvention; }

	// test bid functions
	void SetTestBiddingMode(bool bMode) { m_bInTestBiddingMode = bMode; }
	bool GetTestBiddingMode() { return m_bInTestBiddingMode; }

	// other convention-related helper functions
	BOOL InvokeBlackwood(int nEventualSuit);
	BOOL InvokeGerber(int nEventualSuit); // NCR-246a Gerber after Stayman 2C
	BOOL PlayingConvention(int nConventionID);
	BOOL TryCueBid();


// public data
public:
	// static data
	static int		m_numDefaultBiddingInputs;
	static int		m_numDefaultBiddingOutputs;

// private data
//protected:
public:
	CPlayer*				m_pPlayer;
	CPlayer*				m_pPartner;
	CPlayer*				m_pLHOpponent;
	CPlayer*				m_pRHOpponent;
	CHandHoldings*			m_pHand;
	CCardLocation*			m_pCardLocation;
	CGuessedHandHoldings**	m_ppGuessedHands;
	CPlayerStatusDialog*	m_pStatusDlg;

	//
	// Bidding info
	//
	int			m_nBid;
	int			m_numBidTurns;
	int			m_numBidsMade;
	int			m_numPartnerBidsMade;
	int			m_numPartnerBidTurns;
	int			m_nNextIntendedBid;	// next intended bid
	int			m_nIntendedSuit;
	int			m_nIntendedContract;
	//
	int			m_nOpeningPosition;
	BOOL		m_bOpenedBiddingForTeam;
	BOOL		m_bPartnerOpenedForTeam;
	BOOL		m_bPartnerOvercalled;
//	int			m_nPartnersSuit;	// partner's preferred suit
	int			m_nAgreedSuit;
	BOOL		m_bRoundForceActive;
	BOOL		m_bGameForceActive;
	BOOL		m_bInvitedToGame;
	BOOL		m_bInvitedToSlam;
	BOOL		m_bHintMode;
	// team points
	double		m_fMinTPPoints;		// min total partnership points
	double		m_fMaxTPPoints;		//
	double		m_fMinTPCPoints;	// min total partnership HCPs
	double		m_fMaxTPCPoints;
	double		m_fPartnersMin;		// min holding by partner
	double		m_fPartnersMax;
	// partner's hand info
	int			m_numPartnersAces;
	int			m_numPartnersKings;
	// convention status & state information
	CMap<CConvention*,CConvention*&,int,int> m_mapConventionStatus;
	CMap<CConvention*,CConvention*&,int,int> m_mapConventionParameters;
	CConvention* m_pActiveConvention;
	bool		m_bInTestBiddingMode;

	//
	// local copies of working variables
	//
	int nBid,nSuit,nLevel;
	int nRound;
	int nLastBid;
	int nPosition;
	int	nBiddingOrder;
	int nLastValidRecordedBid;

	//
	int nPreviousBid;
	int nPreviousBidLevel;
	int nPreviousSuit;
	int nPreviousSuitStrength;
	int numPreviousSuitCards;
	double numPreviousSuitPoints;
	int numHonorsInPreviousSuit;
	BOOL bPreviousSuitIsSolid;
	BOOL bPreviousSuitIsRebiddable;
	BOOL bPreviousSuitIsSelfSupporting;
	//
	int nNextPrevBid;
	int nNextPrevSuit;
	//
	int nFirstRoundBid;
	int nFirstRoundBidLevel;
	int nFirstRoundSuit;
	int nFirstRoundSuitStrength;
	int numFirstRoundSuitCards;
	BOOL bFirstRoundSuitIsSolid;
	BOOL bFirstRoundSuitIsSelfSupporting;
	//
	int nPrefSuit;
	int nPrefSuitStrength;
	int numPrefSuitCards;
	double numPrefSuitPoints;
	int numHonorsInPrefSuit;
	BOOL bPrefSuitIsSolid;
	BOOL bPrefSuitIsSelfSupporting;
	int numPreferredSuits;
	//
	int nPartnersBid;
	int nPartnersBidLevel;
	int nPartnersSuit;
	int nPartnersSuitSupport;
	int numSupportCards;
	int numHonorsInPartnersSuit;
	BOOL bJumpResponseByPartner;
	//
	int nPartnersOpeningBid;
	int nPartnersOpeningBidLevel;
	int nPartnersOpeningSuit;
	//
	int nPartnerPos;
	int nPartnersPrevBid;
	int nPartnersPrevBidLevel;
	int nPartnersPrevSuit;
	int nPPrevSuitSupport;
	int numPPrevSuitSupportCards;
	//
	BOOL bLHOInterfered;
	int nLHOBid;
	int nLHOBidLevel;
	int nLHOSuit;
	BOOL bLHOPassed;
	int nLHONumBidsMade;
	int nLHONumBidTurns;
	//
	BOOL bRHOInterfered;
	int nRHOBid;
	int nRHOBidLevel;
	int nRHOSuit;
	BOOL bRHOPassed;
	int nRHONumBidsMade;
	int nRHONumBidTurns;
	//
	int nOpponentsBid;
	int nOpponentsBidLevel;
	int nOpponentsBidSuit;
	//
	int numSuitsStopped;
	int numSuitsProbStopped;
	int numVoids;
	int numSingletons;
	int numDoubletons;
	int numLikelyWinners;
	int numLikelyLosers;
	int numWinners;
	int numLosers;
	double numQuickTricks;
	//
	int numOpenableSuits;
	int numRebiddableSuits;
	int numSolidSuits;
	int numAbsoluteSuits;
	int nSuitStrength[4];
	int numCardsInSuit[4];
	int numHonorsInSuit[4];
	double numSuitPoints[4];
	int nPrefSuitList[5];
	//
	int numAces;
	int numKings;
	int numQueens;
	int numJacks;
	int numTens;

	// text shortcuts
	char szPB[12],szPS[12],szPSS[12],szHP[12];
	char szPPB[12],szPPS[12],szPPSS[12],szHPP[12];
	char szPrefS[12],szPrefSS[12],szHPref[12];
	char szPVB[12],szPVS[12],szPVSS[12],szHPV[12];
	char szNPB[12],szNPS[12],szNPSS[12],szHNPS[12];
	char szLHO[6], szRHO[6];

	// some other shortcuts
	double fPts;
	double fAdjPts;
	double fCardPts;
	double fDistPts;
	double fPenPts;
	BOOL bBalanced;
	BOOL bSemiBalanced;
	BOOL bAllSuitsStopped;

//
//private:
public:
	CBidEngine();
	~CBidEngine();
};

#endif