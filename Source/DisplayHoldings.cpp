//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//===========================================================
//
// CDisplayHoldings
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "Card.h"
#include "DisplayHoldings.h"



//
//--------------------------------------------------------------
//
// Construction/Destruction
//

// constructor
CDisplayHoldings::CDisplayHoldings()
{
}
// destructor
CDisplayHoldings::~CDisplayHoldings()
{
}


//
//--------------------------------------------------------------
//


//
void CDisplayHoldings::Init()
{
	CCardHoldings::Init();
}


//
void CDisplayHoldings::Clear()
{
	CCardHoldings::Clear();
	m_bDummy = FALSE;
}



//
void CDisplayHoldings::Add(CCard* pCard, const BOOL bSort)
{
	if (m_numCards >= MAXHOLDING)
		return;
	if ((bSort) || (m_bAutoSort))
	{
		// insert the card into the list, which is sorted in ascending order
		// i.e., with S-H-D-C display order, SA=0, and C2=51
		// but check to see if we're dummy
		int i;
		if (m_bDummy)
		{
			// dummy
			int nValue = pCard->GetDummyDisplayValue();
			for(i=0;i<m_numCards;i++)
				if (nValue < m_cards[i]->GetDummyDisplayValue())
					break;
		}
		else
		{
			// not dummy
			int nValue = pCard->GetDisplayValue();
			for(i=0;i<m_numCards;i++)
				if (nValue < m_cards[i]->GetDisplayValue())
					break;
		}
		// shift rest over
		for(int j=m_numCards;j>i;j--)
		{
			m_cards[j] = m_cards[j-1];
			m_cards[j]->SetDisplayPosition(j);
		}
		// and insert
		m_cards[i] = pCard;
		m_cards[i]->SetDisplayPosition(i);
		m_bSorted = TRUE;
	}
	else
	{
		m_cards[m_numCards] = pCard;
	}
	m_numCards++;
}


//
CCard* CDisplayHoldings::RemoveByIndex(const int nIndex)
{
	if ((nIndex < 0) || (nIndex >= m_numCards))
		return NULL;
	CCard* pCard = m_cards[nIndex];
	// update count
	m_numCards--;
	// move other cards over and update display indices
	for(int i=nIndex;i<m_numCards;i++) 
	{
		m_cards[i] = m_cards[i+1];
		m_cards[i]->SetDisplayPosition(i);
	}
	for(;i<MAXHOLDING;i++) 
		m_cards[i] = NULL;
	return pCard;
}



//
void CDisplayHoldings::Sort()
{
	if (m_bSorted)
		return;
	//
	CCard* pTemp;
	int i,j;
	for(i=0;i<m_numCards-1;i++) 
	{
		for(j=0;j<m_numCards-1;j++) 
		{
			// use screen display order sort
			// see if we're dummy
			if (m_bDummy)
			{
				// dummy
				if (m_cards[j]->GetDummyDisplayValue() < m_cards[j+1]->GetDummyDisplayValue()) 
				{
					pTemp = m_cards[j];	
					m_cards[j] = m_cards[j+1];
					m_cards[j+1] = pTemp;	
				}
			}
			else
			{
				// not dummy
				if (m_cards[j]->GetDisplayValue() < m_cards[j+1]->GetDisplayValue()) 
				{
					pTemp = m_cards[j];	
					m_cards[j] = m_cards[j+1];
					m_cards[j+1] = pTemp;	
				}
			}
		}
	}
}

