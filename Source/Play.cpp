//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Play.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "Play.h"
#include "PlayList.h"
#include "Card.h"
#include "CardList.h"
#include "Player.h"
#include "PlayerStatusDialog.h"
#include "PlayEngine.h"
#include "CombinedHoldings.h"
#include "DeclarerPlayEngine.h"
/*
// required for CPlayList
#include "BidEngine.h"
#include "HandHoldings.h"
*/

// static data 
CRITICAL_SECTION	CPlay::m_csPlayIDLock;
int					CPlay::m_nPlayIDCounter;


//
//==================================================================
// constructon & destruction
CPlay::CPlay(PlayType nPlayType, int nTargetHand, int nSuit, PlayProspect nPlayProspect, BOOL bOpportunistic) :
		m_nPlayType(nPlayType),
		m_nTargetHand(nTargetHand),
		m_nSuit(nSuit),
		m_nPlayProspect((int)nPlayProspect),
		m_bOpportunistic(bOpportunistic)
{
	m_nID = ObtainUniquePlayID();
	m_nStatusCode = PLAY_INACTIVE;
	m_nStartingHand = IN_EITHER;
	m_nEndingHand = IN_EITHER;
	m_numPrerequisites = 0;
	m_numPostrequisites = 0;
	m_pPrerequisite = NULL;
	m_pPostrequisite = NULL;
	m_pPrerequisiteList = NULL;
	m_pPostrequisiteList = NULL;
	m_pConsumedCard = NULL;
	m_pTargetCard = NULL;
	m_pTargetCardsList = NULL;
	m_pKeyCardsList = NULL;
	m_pOrKeyCardsList = NULL;
	m_pOrKeyCardsList2 = NULL;
	m_pEnemyKeyCardsList = NULL;
	m_pEnemyOrKeyCardsList = NULL;
	m_pRequiredPlayedCardsList = NULL;
}

CPlay::~CPlay() 
{
	delete m_pPrerequisiteList;
	delete m_pPostrequisiteList;
	delete m_pKeyCardsList;
	delete m_pOrKeyCardsList;
	delete m_pOrKeyCardsList2;
	delete m_pEnemyKeyCardsList;
	delete m_pEnemyOrKeyCardsList;
	delete m_pTargetCardsList;
	delete m_pRequiredPlayedCardsList;
}


//
BOOL CPlay::IsValid()
{
	if ((m_nID < 0) || (m_numPostrequisites < 0) || (m_numPrerequisites < 0))
		return FALSE;
	return TRUE;
}


//
void CPlay::Init()
{
	// empty for now
}


//
void CPlay::Clear()
{
	m_nStatusCode = PLAY_INACTIVE;
	m_nTargetHand = IN_EITHER;
	m_nStartingHand = IN_EITHER;
	m_nEndingHand = IN_EITHER;
	m_nSuit = NONE;
	m_nSuit2 = NONE;
}


//
// Reset()
//
// - called when a play is called back from the 
// dead upon an "undo trick"
//
void CPlay::Reset()
{
	m_nStatusCode = PLAY_INACTIVE;
}


//
LPCTSTR CPlay::GetPlayTypeName()
{
	int numTypes = sizeof(tszPlayTypeNames) / sizeof(LPCTSTR);
	VERIFY((m_nPlayType >= 0) && (m_nPlayType < numTypes));
	return tszPlayTypeNames[m_nPlayType];
}



//
// IsPlayUsable()
//
BOOL CPlay::IsPlayUsable(const CCombinedHoldings& combinedHand, const CPlayEngine& playEngine)
{
	//
	// perform some basic tests to see that the play is still usable
	//

	// but only do so if this is position #0 or 1
	int nOrdinal = pDOC->GetNumCardsPlayedInRound();
	if (nOrdinal >= 2)
		return TRUE;

	//
	CPlayerStatusDialog& status = (const_cast<CPlayEngine&>(playEngine)).GetPlayer()->GetStatusDialog();

	// 1: check if we still have the required consumable card
	if ((m_pConsumedCard) && (!combinedHand.HasCard(m_pConsumedCard)))
	{
		status << "5PLYTEST1! The play <" & m_strName & 
				  "> is no longer viable as the " & m_pConsumedCard->GetName() &
				  " has already been played.\n";
		return FALSE;
	}

	// 2: check if the target card is still outstanding
	if ((m_pTargetCard) && (!playEngine.IsCardOutstanding(m_pTargetCard)))
	{
		status << "5PLYTEST2! The play <" & m_strName & 
				  "> is no longer viable as the target card [" & m_pTargetCard->GetName() &
				  "] has already been played.\n";
		return FALSE;
	}

	// 3: check if we still have the required key cards
	if ((m_pKeyCardsList) && (!combinedHand.ContainsCards(*m_pKeyCardsList)))
	{
		status << "5PLYTEST3! The play <" & m_strName & 
				  "> is no longer viable as the partnership no longer holds all the required cards of [" & 
				  m_pKeyCardsList->GetHoldingsString() & "].\n";
		return FALSE;
	}

	// 4: check if we have at least one OR-Key cards
	if ((m_pOrKeyCardsList) && (!combinedHand.ContainsAtLeastOneOf(*m_pOrKeyCardsList)))
	{
		status << "5PLYTEST4! The play <" & m_strName & 
				  "> is no longer viable as the partnership no longer holds any of the alternative required cards of [" & 
				  m_pOrKeyCardsList->GetHoldingsString() & "].\n";
		return FALSE;
	}

	// 5: check the second OR-Key cards list
	if ((m_pOrKeyCardsList2) && (!combinedHand.ContainsAtLeastOneOf(*m_pOrKeyCardsList2)))
	{
		status << "5PLYTST5! The play <" & m_strName & 
				  "> is no longer viable as the partnership no longer holds any of the second set of alternative required cards of [" & 
				  m_pOrKeyCardsList2->GetHoldingsString() & "].\n";
		return FALSE;
	}

	// 6: check the enemy key cards list
	if (m_pEnemyKeyCardsList)
	{
		// check to be sure all of the cards is still outstanding
		// that is, held by the opponents and not yet played
		for(int j=0;j<m_pEnemyKeyCardsList->GetNumCards();j++)
		{
			if (!playEngine.IsCardOutstanding((*m_pEnemyKeyCardsList)[j]))
			{
				status << "5PLYTST6! The play <" & m_strName & 
						  "> is no longer viable as the opponents no longer hold the " & 
						  (*m_pEnemyKeyCardsList)[j]->GetName() & 
						  ", which is one of the cards required for the play.\n";
				return FALSE;
			}
		}
	}

	// 7: check the enemy OR-Key cards list
	if (m_pEnemyOrKeyCardsList)
	{
		// check to see if at least one of the cards is still outstanding
		BOOL bCardFound = FALSE;
		for(int j=0;j<m_pEnemyOrKeyCardsList->GetNumCards();j++)
		{
			if (playEngine.IsCardOutstanding((*m_pEnemyOrKeyCardsList)[j]))
			{
				bCardFound = TRUE;
				break;
			}
		}
		if (!bCardFound)
		{
			status << "5PLYTST7! The play <" & m_strName & 
					  "> is no longer viable as the opponents no longer hold any of the required alternative cards of [" & 
					  m_pEnemyOrKeyCardsList->GetHoldingsString() & "].\n";
			return FALSE;
		}
	}

	// 8: see if there are any cards that need to have been played already
	if (m_pRequiredPlayedCardsList)
	{
		// check if any of the cards that should have been played 
		// are still outstanding
		for(int i=0;i<m_pRequiredPlayedCardsList->GetNumCards();i++)
		{
			CCard* pCard = (*m_pRequiredPlayedCardsList)[i];
			if (playEngine.IsCardOutstanding(pCard))
			{
				status << "5PLYTST8! The play <" & m_strName & 
						  "> is not yet viable as the card [" & 
						  pCard->GetFaceName() & "] is still outstanding.\n";
				return FALSE;
			}
		}
	}

	// 9: see if there are plays that need to have been used already
	if (m_pPrerequisiteList)
	{
		// check all completed plays
		CPlayList& usedPlays = ((CDeclarerPlayEngine*)&playEngine)->GetUsedPlays();
		int numUsedPlays = usedPlays.GetSize();
		for(int i=0;i<m_pPrerequisiteList->GetSize();i++)
		{
			CPlay* pPlay = m_pPrerequisiteList->GetAt(i);
			for(int j=0;j<numUsedPlays;j++)
			{
				CPlay* pUsedPlay = usedPlays[j];
				if (pPlay == pUsedPlay)
					break;
			}
			//
			if (j == numUsedPlays)
			{
				// oops! this play hasn't been used yet
				status << "5PLYTST9! The play <" & m_strName & "> is not yet viable as the precursor play [" & 
						  pPlay->GetName() & "] has not neen used yet.\n";
				return FALSE;
			}
		}
	}

	// else all's OK
	return TRUE;
}




//
// Perform()
//
// called to do the deed
//
// possible return codes:
// ----------------------
//  0: OK, play performed
// -1: Play no longer viable
//  1: Play should be postponed
//  2: play OK, can be repeated (e.g., a finesse)
//
PlayResult CPlay::Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
				   CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, 
				   CPlayerStatusDialog& status, CCard*& pPlayCard)
{
	// this "play" is empty
	status << "3PLYXX! Play base class default play routine called.\n";
	return PLAY_ERROR;
}




//////////////////////////////////////////////////////////////////
//
//
// Misc operations
//
//////////////////////////////////////////////////////////////////


//
int CPlay::ObtainUniquePlayID()
{
	// assign play IDs one at a time
	int nCode;
	EnterCriticalSection(&m_csPlayIDLock);
	{
		// clear the previous location
		nCode = m_nPlayIDCounter++;
	}
	LeaveCriticalSection(&m_csPlayIDLock);
	return nCode;
}


//
void CPlay::ClassInitialize()
{
	// init static members
	InitializeCriticalSection(&m_csPlayIDLock);
	m_nPlayIDCounter = 0;
}

//
void CPlay::ClassTerminate()
{
	//
	DeleteCriticalSection(&m_csPlayIDLock);
}



///////////////////////////////////////////////////////////////
//
// Main operations
//
///////////////////////////////////////////////////////////////



//
int	CPlay::GetNumKeyCards() 
{ 
	if (m_pKeyCardsList)
		return m_pKeyCardsList->GetNumCards();
	else
		return 0; 
}


//
int	CPlay::GetNumOrKeyCards() 
{ 
	if (m_pOrKeyCardsList)
		return m_pOrKeyCardsList->GetNumCards();
	else
		return 0; 
}


//
int	CPlay::GetNumOrKeyCards2() 
{ 
	if (m_pOrKeyCardsList2)
		return m_pOrKeyCardsList2->GetNumCards();
	else
		return 0; 
}


//
// RequiresCard()
//
// returns whether a card is requried for the play to be performed
//
BOOL CPlay::RequiresCard(CCard* pCard) 
{ 
	// sanity check
	if (pCard == NULL)
		return FALSE;

	// a card is required for this play if it is either consumed
	// in the play, or is a key card for the play (e.g., a cover card)
	if ((pCard == m_pConsumedCard) || 
		((m_pKeyCardsList) && (m_pKeyCardsList->HasCard(pCard))))
		return TRUE;

	//
	return FALSE;
}


//
// LookupORCard()
//
// returns the index of a card in the OR-Key cards list, or 
// -1 if not found
//
int	CPlay::LookupORCard(CCard* pCard)
{
	if (m_pOrKeyCardsList == NULL)
		return -1;

	//
	int numOrElements = m_pOrKeyCardsList->GetNumCards();
	for(int i=0;i<numOrElements;i++)
	{
		if (pCard == m_pOrKeyCardsList->GetAt(i))
			return i;
	}

	// not found
	return -1;
}



//
// LookupORCard2()
//
// returns the index of a card in the second OR-cards list, or 
// -1 if not found
//
int	CPlay::LookupORCard2(CCard* pCard)
{
	if (m_pOrKeyCardsList2 == NULL)
		return -1;

	//
	int numOr2Elements = m_pOrKeyCardsList2->GetNumCards();
	for(int i=0;i<numOr2Elements;i++)
	{
		if (pCard == m_pOrKeyCardsList2->GetAt(i))
			return i;
	}

	// not found
	return -1;
}

