//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Finesse.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBDoc.h"
#include "Deck.h"
#include "Card.h"
#include "CardList.h"
//#include "CardList.h"
#include "Finesse.h"



//
//==================================================================
// constructon & destruction

CFinesse::CFinesse(int nSubType, int nTargetHand, int nPlayerPosition, CCardList* pGapCards, int nSuit, int nCardVal, BOOL bOpportunistic) :
					CPlay(FINESSE, nTargetHand, nSuit, PP_SPECULATIVE, bOpportunistic),
					m_nSubType(nSubType),
					m_nPlayerPosition(nPlayerPosition),
					m_pGapCards(pGapCards),
					m_nCardVal(nCardVal)
{
	Init();
}

CFinesse::CFinesse(int nSubType, int nTargetHand, int nPlayerPosition, CCardList* pGapCards, CCard* pCard, BOOL bOpportunistic) :
					CPlay(FINESSE, nTargetHand, NONE, PP_SPECULATIVE, bOpportunistic),
					m_nSubType(nSubType),
					m_nPlayerPosition(nPlayerPosition),
					m_pGapCards(pGapCards)
{
	VERIFY(pCard);
	m_nSuit = pCard->GetSuit();
	m_nCardVal = pCard->GetFaceValue();
	Init();
}

CFinesse::~CFinesse() 
{
}


//
void CFinesse::Clear()
{
	m_nPlayerPosition = NONE;
	m_nStartingHand = NONE;
	m_nEndingHand = NONE;
	m_nTarget = NONE;
	m_nGapSize = 0;
	m_nCardVal = NONE;
}


//
void CFinesse::Init()
{
	CPlay::Init();
	//
	int nTrumpSuit = pDOC->GetTrumpSuit();
	m_nGapSize = m_pGapCards->GetNumCards();
	m_pEnemyOrKeyCardsList = m_pGapCards;	// will be deleted by base class
	m_pConsumedCard = deck.GetCard(m_nSuit, m_nCardVal);
//	m_pKeyCardsList = new CCardList;

	// check enemy target card(s)
	if (m_pGapCards->GetNumCards() == 1)
		m_pTargetCard = (*m_pGapCards)[0];

	// the target hand is the ending hand
	m_nEndingHand = m_nTargetHand;
	m_nStartingHand = (m_nEndingHand == IN_HAND)? IN_DUMMY : IN_HAND;

	// check the enemy and location
	m_nTarget = (m_nEndingHand == CFinesse::IN_DUMMY)? AGAINST_RHO: AGAINST_LHO;
	if (m_nTarget == AGAINST_LHO)
		m_nTargetPos = GetNextPlayer(m_nPlayerPosition);
	else
		m_nTargetPos = GetPrevPlayer(m_nPlayerPosition);

	// form name & description
	if (m_nSuit == nTrumpSuit)
	{
		m_strName.Format("Trump %s Finesse", m_pConsumedCard->GetFaceName());
		m_strDescription.Format("Finesse the %s of trumps", m_pConsumedCard->GetFaceName());
	}
	else
	{
		m_strName.Format("%s Finesse", m_pConsumedCard->GetName());
		m_strDescription.Format("Finesse the %s", m_pConsumedCard->GetName());
	}
}

