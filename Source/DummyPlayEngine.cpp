//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//=====================================================================
//
// CDummyPlayEngine
//
// - encapsulates the defender's play engine and its status variables
//
//=====================================================================


#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "HandHoldings.h"
#include "Card.h"
#include "BidEngine.h"
#include "DummyPlayEngine.h"





//
//==========================================================================
//==========================================================================
//
// Initialization routines
//
//
CDummyPlayEngine::CDummyPlayEngine()
{
}

CDummyPlayEngine::~CDummyPlayEngine()
{
	Clear();
}



// 
//-----------------------------------------------------------------------
//
// Initialize()
//
// one-time initialization at program start
//
void CDummyPlayEngine::Initialize(CPlayer* pPlayer, CPlayer* pPartner, CPlayer* pLHOpponent, CPlayer* pRHOpponent, CHandHoldings* pHoldings, CCardLocation* pCardLocation, CGuessedHandHoldings** ppGuessedHands, CBidEngine* pBidder, CPlayerStatusDialog* pStatusDlg)
{ 
	// call base class first
	CPlayEngine::Initialize(pPlayer, pPartner, pLHOpponent, pRHOpponent, pHoldings, pCardLocation, ppGuessedHands, pBidder, pStatusDlg);

}



//
//-----------------------------------------------------------------------
//
// Clear()
//
void CDummyPlayEngine::Clear()
{ 
	// call base class first
	CPlayEngine::Clear();
		
}



	

//
//-----------------------------------------------------------------------
//
// InitNewHand()
//
// called when the hand is dealt (or play is restarted)
//
void CDummyPlayEngine::InitNewHand()
{ 
	// make sure we're declarer
	VERIFY(!m_pPlayer->IsDeclarer());

	// call base class first
	CPlayEngine::InitNewHand();

}



//
// RestartPlay()
//
void CDummyPlayEngine::RestartPlay()
{
	// call base class
	CPlayEngine::RestartPlay();
}








//
//==========================================================================
//==========================================================================
//
// Analysis routines
//
//



//
//-----------------------------------------------------------------------
//
// AssessPosition()
//
// called once on each round of play
//
void CDummyPlayEngine::AssessPosition()
{
	// call base class first
	CPlayEngine::AssessPosition();

	// then perform class-specific operations
}



//
//-----------------------------------------------------------------------
//
// AdjustCardCount()
//
// adjust card count and analysis after a card is played
//
void CDummyPlayEngine::AdjustCardCount(int nPos, CCard* pCard)
{
	// do nothing for now
//	if ((nPos != m_pPlayer->GetPosition() && (nPos != m_pPartner->GetPosition())
//		CPlayEngine::AdjustCardCount(nPos, pCard);
}




//
//-----------------------------------------------------------------------
//
// AdjustHoldingsCount()
//
// called to adjust analysis of holdings after a round of play
//
void CDummyPlayEngine::AdjustHoldingsCount(CCard* pCard)
{
	// do nothing for now
	CPlayEngine::AdjustHoldingsCount(pCard);
}



//
// GetLeadCard()
//
CCard* CDummyPlayEngine::GetLeadCard()
{
	// shouldn't call this routine!
//	ASSERT(FALSE);

	// still, we might have to, if declarer runs out of plays
	return CPlayEngine::GetLeadCard();
}




//
// PlayCard()
//
// select a card to play
// 
CCard* CDummyPlayEngine::PlayCard()
{ 
	CPlayerStatusDialog& status = *m_pStatusDlg;
	AssessPosition();

	// pass control over to declarer
	CCard* pCard = m_pPartner->PlayForDummy();
	VERIFY(pCard->IsValid());
	return pCard;
}



//
//-----------------------------------------------------------------------
//
// PlayForSelf()
//
// let the dummy play for himself
// 
CCard* CDummyPlayEngine::PlayForSelf()
{
	// call default
	return CPlayEngine::PlayCard();
}




//
// GetPlayHint()
//
CCard* CDummyPlayEngine::GetPlayHint()
{
	return m_pPartner->GetPlayHintForDummy();
}



//
// GetNumClaimableTricks() 
//
int CDummyPlayEngine::GetNumClaimableTricks() 
{ 
	return m_pPartner->GetNumClaimableTricks(); 
}
