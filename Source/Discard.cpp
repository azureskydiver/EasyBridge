//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// Discard.cpp
//
#include "stdafx.h"
#include "EasyB.h"
#include "Deck.h"
#include "Card.h"
#include "Discard.h"
#include "PlayEngine.h"
#include "CombinedHoldings.h"
#include "CardLocation.h"
#include "GuessedHandHoldings.h"
#include "PlayerStatusDialog.h"



//
//==================================================================
// constructon & destruction

CDiscard::CDiscard(int nTargetHand, int nSuit, int nCardVal) :
			CPlay(CPlay::DISCARD, nTargetHand, nSuit, PP_LOSER)
{
	m_nCardVal = nCardVal;
}

CDiscard::CDiscard(int nTargetHand, CCard* pCard) :
			CPlay(CPlay::DISCARD, nTargetHand, NONE, PP_LOSER)
{
	VERIFY(pCard);
	m_nSuit = pCard->GetSuit(); 
	m_nCardVal = pCard->GetFaceValue(); 
}

CDiscard::CDiscard(CDiscard& srcPlay) :
		CPlay(CPlay::m_nPlayType, srcPlay.m_nTargetHand)
{
	m_nTargetHand = srcPlay.m_nTargetHand;
	m_nSuit = srcPlay.m_nSuit;
	m_nCardVal = srcPlay.m_nCardVal;
}

CDiscard::~CDiscard() 
{
}


//
void CDiscard::Clear()
{
	CPlay::Clear();
	//
	m_nCardVal = NONE;
}


//
void CDiscard::Init()
{
	CPlay::Init();
	//
	m_pConsumedCard = deck.GetCard(m_nSuit, m_nCardVal);
	// form description
	m_strDescription.Format("Discard the %s", m_pConsumedCard->GetName());
}


//
CString CDiscard::GetFullDescription()
{
	CString strText;
	strText.Format("Discard the %s in %s.", m_pConsumedCard->GetName(),
				   (m_nTargetHand == 0)? "hand" : "dummy");
	// done
	return strText;
}




//
// Perform()
//
PlayResult CDiscard::Perform(CPlayEngine& playEngine, CCombinedHoldings& combinedHand, 
						      CCardLocation& cardLocation, CGuessedHandHoldings** ppGuessedHands, 
						      CPlayerStatusDialog& status, CCard*& pPlayCard)
{
	// not written yet
	return PLAY_NOT_VIABLE;
//	return CPlay::Perform(playEngine, combinedHand, cardLocation, ppGuessedHands, status, pPlayCard);
}

