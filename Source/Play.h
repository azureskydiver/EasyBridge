//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Play.h
//
// abstract base class
//
// Current list of plays are:
//    Cash
//    Discard
//    Drop
//    Finesse
//    Force
//    Ruff
//    Sluff
//    TrumpPull
//

#ifndef __CPLAY__
#define __CPLAY__

class CPlayList;
class CPlayEngine;
class CCard;
class CCardList;
class CCombinedHoldings;
class CGuessedHandHoldings;
class CCardLocation;
class CPlayerStatusDialog;

const LPCTSTR tszPlayTypeNames[] = {
	"Trump Pull",
	"Cash",
	"Ruff",
	"Drop",
	"Force",
	"Finesse",
	"Hold Up",
	"Develop Suit",
	"Discard",
	"Exit",
};

// play status
typedef enum {
	PLAY_ERROR				 = -4,
	PLAY_FAILED				 = -3,
	PLAY_NOT_VIABLE			 = -2,
	PLAY_POSTPONE			 = -1,
	PLAY_INACTIVE			 =  0,
	PLAY_IN_PROGRESS		 =  1,
	PLAY_COMPLETE			 =  2,	
	PLAY_COMPLETE_AND_REPEAT =  3,
} PlayResult;

//
// The CPlay class
//
class CPlay {

// public data
public:
	// play type
	typedef enum { 
		   NULL_PLAY=-1,
		   TRUMP_PULL,
		   CASH, 
		   RUFF, 
		   DROP,
		   FORCE,
		   FINESSE, 
		   HOLDUP, 
		   DEVELOP, 
		   DISCARD, 
		   EXIT, 
	} PlayType;

	// play location
	enum { 
		   IN_HAND=0, 
		   IN_DUMMY=1, 
		   IN_EITHER = -1, 
	};

	// which entry the play uses up, if any
	enum { 
		   ENTRY_NONE  = 0, 
		   ENTRY_HAND  = 1, 
		   ENTRY_DUMMY = 2, 
	};

	// play propsect
	typedef enum {
		   PP_UNKNOWN			= 0, 
		   PP_LOSER 			= 1, 
		   PP_SPECULATIVE		= 2, 
		   PP_LIKELY_WINNER		= 3, 
		   PP_GUARANTEED_WINNER = 4, 
	} PlayProspect;


// public routines
public:
	// non-overridable functions
	int					ObtainUniquePlayID();
	int					GetID() { return m_nID; }

	// overridable pure virtual functions
	virtual void		Clear();
	virtual BOOL		IsValid();
	virtual int			GetTargetHand() { return m_nTargetHand; }
	virtual int			GetStartingHand() { return m_nStartingHand; }
	virtual int			GetEndingHand() { return m_nEndingHand; }
	virtual int			GetSuit() { return m_nSuit; }
	virtual int			GetSecondSuit() { return m_nSuit2; }
	virtual CString		GetFullDescription() = 0;
	// basic functions
	virtual PlayResult	Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
								CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, 
								CPlayerStatusDialog& status, CCard*& pPlayCard);
	virtual void		Init();
	virtual BOOL		IsGuranteedWinner() { return (m_nPlayProspect == PP_GUARANTEED_WINNER); }
	virtual BOOL		IsWinner() { return (m_nPlayProspect >= PP_LIKELY_WINNER); }
	virtual BOOL		IsPlayUsable(const CCombinedHoldings& combinedHand, const CPlayEngine& playEngine);
	virtual int			GetPlayProspect() { return m_nPlayProspect; }
	virtual PlayResult	GetStatus() { return m_nStatusCode; }
	virtual int			GetPlayType() { return m_nPlayType; }
	virtual BOOL		IsOpportunistic() { return m_bOpportunistic; }
	virtual LPCTSTR		GetPlayTypeName();
	virtual int			GetNumPrerequisites() { return m_numPrerequisites; }
	virtual int			GetNumPostrerequisites() { return m_numPostrequisites; }
	virtual CPlay*		GetPrerequisite() { return m_pPrerequisite; }
	virtual CPlay*		GetPostrerequisite() { return m_pPostrequisite; }
	virtual CPlayList*  GetPrerequisiteList() { return m_pPrerequisiteList; }
	virtual CPlayList*  GetPostrerequisiteList() { return m_pPostrequisiteList; }
	virtual BOOL		RequiresCard(CCard* pCard);
	virtual int			LookupORCard(CCard* pCard);
	virtual int			LookupORCard2(CCard* pCard);
	//
	virtual int			GetNumKeyCards();
	virtual int			GetNumOrKeyCards();
	virtual int			GetNumOrKeyCards2();
	virtual CCardList*	GetKeyCardsList() { return m_pKeyCardsList; }
	virtual CCardList*	GetOrKeyCardsList() { return m_pOrKeyCardsList; }
	virtual CCardList*	GetOrKeyCardsList2() { return m_pOrKeyCardsList2; }
	virtual CCardList*	GetEnemyKeyCardsList() { return m_pEnemyKeyCardsList; }
	virtual CCardList*	GetEnemyOrKeyCardsList() { return m_pEnemyOrKeyCardsList; }
	virtual CCard*		GetConsumedCard() { return m_pConsumedCard; }
	virtual CCard*		GetTargetCard() { return m_pTargetCard; }
	virtual CCardList*	GetTargetCardsList() { return m_pTargetCardsList; }
	virtual CCardList*	GetRequiredPlayedCardsList() { return m_pRequiredPlayedCardsList; }
	virtual LPCTSTR		GetName() { return m_strName; }
	virtual LPCTSTR		GetDescription() { return m_strDescription; }
	virtual int			UsesUpEntry() { return ENTRY_NONE; }
	//
	virtual void		Reset();

	// static functions
	static void		ClassInitialize();
	static void		ClassTerminate();

// protected routines
protected:
	virtual void	SetStatus(PlayResult nStatus) { m_nStatusCode = nStatus; }


// protected data
protected:
	CString		m_strName;				// play short name
	CString		m_strDescription;		// play description
	PlayType	m_nPlayType;			// play type
	BOOL		m_bOpportunistic;		// opportunistic (i.e., must play now)
	int			m_nID;					// unique play ID
	int			m_nPlayProspect;		// play prospect
	int			m_nSuit;				// play suit
	int			m_nSuit2;				// second suit (if appropriate)
	PlayResult	m_nStatusCode;			// current status
	int			m_nTargetHand;			// key hand for the play
	int			m_nStartingHand;		// starting hand for the play
	int			m_nEndingHand;			// ending hand for the play
	int			m_numPrerequisites;		// # antecendents
	int			m_numPostrequisites;	// # postcedents
	CPlay*		m_pPrerequisite;		// required antecendent
	CPlay*		m_pPostrequisite;		// required postcedent
	CPlayList*  m_pPrerequisiteList;	// list of required antecendents
	CPlayList*  m_pPostrequisiteList;	// list of required postcedents
	//
	CCard*		m_pConsumedCard;		// card that will be used up, if any
	CCard*		m_pTargetCard;			// enemy card that is the target of the play
	CCardList*	m_pTargetCardsList;		// enemy target cards 
	CCardList*	m_pKeyCardsList;		// key cards required for the play
	CCardList*	m_pOrKeyCardsList;		// one or more of these key cards are rq'd
	CCardList*	m_pOrKeyCardsList2;		// one or more of these key cards are also rq'd
	CCardList*	m_pEnemyKeyCardsList;	// key cards required to be held by opponents
	CCardList*	m_pEnemyOrKeyCardsList;	// one or more cards rq'd held by opponents
	CCardList*	m_pRequiredPlayedCardsList;	// cards that must already have ben played

	// play ID counter
	static CRITICAL_SECTION	m_csPlayIDLock;
	static int				m_nPlayIDCounter;


// construction/destruction
public:
	CPlay(PlayType nPlayType, int nTargetHand=IN_EITHER, int nSuit=NONE, PlayProspect nPlayProspect=PP_UNKNOWN, BOOL bOpportunistic=FALSE);
	virtual ~CPlay();
};


#endif
