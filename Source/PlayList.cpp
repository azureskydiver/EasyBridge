//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// PlayList.cpp
//

#include "stdafx.h"
#include "EasyB.h"
#include "PlayList.h"
#include "Play.h"



//
//==================================================================
// constructon & destruction
CPlayList::CPlayList() 
{
	m_playList.SetSize(0, 5);
	m_bActive = FALSE;
}

CPlayList::~CPlayList() 
{
/*
 * don't call Clear() here, since ownership of the play objects 
 * pointed to may have been transferred to another playlist
 *
 Clear();
 */
}


//
void CPlayList::Clear()
{
	int nSize = m_playList.GetSize();
	for(int i=0;i<nSize;i++)
		delete m_playList[i];
	m_playList.RemoveAll();
}


//
void CPlayList::AddPlay(int nIndex, CPlay* pPlay)
{
//	ASSERT(m_playList.GetSize() < 13);
	m_playList.InsertAt(nIndex, pPlay);
}


//
void CPlayList::AppendPlay(CPlay* pPlay) 
{ 
//	ASSERT(m_playList.GetSize() < 13);
	m_playList.Add(pPlay);
}


//
void CPlayList::SetPlay(int nIndex, CPlay* pPlay)
{
	ASSERT(pPlay);
	ASSERT(nIndex >= 0);
//	ASSERT((nIndex >= 0) && (nIndex < 13));
	// remove any existing play at that index
	if (nIndex < m_playList.GetSize())
	{
		CPlay* pTempPlay = m_playList.GetAt(nIndex);
		if (pTempPlay)
			delete pTempPlay;
	}
	m_playList.SetAt(nIndex, pPlay);
}


//
void CPlayList::RemovePlay(int nIndex, BOOL bDelete) 
{ 
	ASSERT((nIndex >= 0) && (nIndex < m_playList.GetSize()));
	if (bDelete)
		delete m_playList[nIndex];
	m_playList.RemoveAt(nIndex);
}


//
void CPlayList::RemoveTail(BOOL bDelete) 
{ 
	int numElements = m_playList.GetSize();
	if (numElements > 0)
	{
		if (bDelete)
			delete m_playList[numElements-1];
		m_playList.RemoveAt(numElements-1);
	}
}


//
void CPlayList::PushPlay(CPlay* pPlay)
{
	m_playList.InsertAt(0, pPlay);
}


//
CPlay* CPlayList::PopPlay()
{
	ASSERT(m_playList.GetSize() > 0);
	CPlay* pPlay = m_playList[0];
	m_playList.RemoveAt(0);
	return pPlay;
}


//
CPlay* CPlayList::PopTail()
{
	int numElements = m_playList.GetSize();
	ASSERT(numElements > 0);
	CPlay* pPlay = m_playList[numElements-1];
	m_playList.RemoveAt(numElements-1);
	return pPlay;
}


//
int CPlayList::MovePlay(int nOldPos, int nNewPos)
{
	if (nOldPos == nNewPos)
		return 0;
	int nSize = m_playList.GetSize();
	ASSERT((nOldPos >= 0) && (nOldPos < nSize) && (nNewPos >= 0) && (nNewPos < nSize));
	CPlay* pPlay = m_playList[nOldPos];
	m_playList.RemoveAt(nOldPos);
	if (nNewPos < nSize-1)
		m_playList.InsertAt(nNewPos, pPlay);
	else
		m_playList.Add(pPlay);
	//
	return 0;
}


//
int CPlayList::DeferPlay(int nIndex)
{
	// 
	int nSize = m_playList.GetSize();
	if ((nIndex < 0) || (nIndex >= nSize))
		return -1;
	if (nIndex == nSize-1)
		return 1;
	//
	CPlay* pPlay = m_playList.GetAt(nIndex);
	m_playList.RemoveAt(nIndex);
	m_playList.InsertAt(nIndex+1, pPlay);
	//
	return 0;
}


//
int CPlayList::AdvancePlay(int nIndex)
{
	// 
	int nSize = m_playList.GetSize();
	if ((nIndex < 0) || (nIndex >= nSize))
		return -1;
	if (nIndex == 0)
		return 1;
	//
	CPlay* pPlay = m_playList.GetAt(nIndex);
	m_playList.RemoveAt(nIndex);
	m_playList.InsertAt(nIndex-1, pPlay);
	//
	return 0;
}


//
int CPlayList::MovePlayToFront(int nIndex)
{
	// 
	int nSize = m_playList.GetSize();
	if ((nIndex < 0) || (nIndex >= nSize))
		return -1;
	if (nIndex == 0)
		return 1;
	//
	CPlay* pPlay = m_playList.GetAt(nIndex);
	m_playList.RemoveAt(nIndex);
	m_playList.InsertAt(0, pPlay);
	//
	return 0;
}


//
int CPlayList::MovePlayToEnd(int nIndex)
{
	// 
	int nSize = m_playList.GetSize();
	if ((nIndex < 0) || (nIndex >= nSize))
		return -1;
	if (nIndex == 0)
		return 1;
	//
	CPlay* pPlay = m_playList.GetAt(nIndex);
	m_playList.RemoveAt(nIndex);
	m_playList.InsertAt(nSize-1, pPlay);
	//
	return 0;
}


//
int CPlayList::DeferPlay(CPlay* pPlay)
{
	int nSize = m_playList.GetSize();
	for(int i=0;i<nSize;i++)
	{
		if (m_playList.GetAt(i) == pPlay)
			return DeferPlay(i);
	}
	// oops
	return -1;
}

//
int CPlayList::AdvancePlay(CPlay* pPlay)
{
	int nSize = m_playList.GetSize();
	for(int i=0;i<nSize;i++)
	{
		if (m_playList.GetAt(i) == pPlay)
			return AdvancePlay(i);
	}
	// oops
	return -1;
}


//
int CPlayList::MovePlayToFront(CPlay* pPlay)
{
	int nSize = m_playList.GetSize();
	for(int i=0;i<nSize;i++)
	{
		if (m_playList.GetAt(i) == pPlay)
			return MovePlayToFront(i);
	}
	// oops
	return -1;
}


//
int CPlayList::MovePlayToEnd(CPlay* pPlay)
{
	int nSize = m_playList.GetSize();
	for(int i=0;i<nSize;i++)
	{
		if (m_playList.GetAt(i) == pPlay)
			return MovePlayToEnd(i);
	}
	// oops
	return -1;
}


//
int CPlayList::FindPlay(CPlay* pPlay)
{
	//
	ASSERT(pPlay);
	int nPlayID = pPlay->GetID();
	int numPlays = m_playList.GetSize(); 
	for(int i=0;i<numPlays;i++)
	{
		if (nPlayID == m_playList.GetAt(i)->GetID())
			return i;
	}
	// not found
	return -1;
}


//
//
// - removes plays that use cards that are also used in the second play list
// - returns the # of plays removed
//
int CPlayList::RemoveSameCardPlays(CPlayList* pOther, BOOL bDelete)
{
	if (pOther == NULL)
		return 0;
	//
	int numPlaysRemoved = 0;
	for(int i=0;i<GetSize();i++)
	{
		for(int j=0;j<pOther->GetSize();j++)
		{
			if (m_playList[i]->GetConsumedCard() == pOther->GetAt(j)->GetConsumedCard())
			{
				RemovePlay(i, bDelete);
				i--;
				numPlaysRemoved++;
				break;
			}
		}
	}
	//
	return numPlaysRemoved;
}


//
int CPlayList::GetPlayType(int nIndex) const
{
	ASSERT(nIndex < m_playList.GetSize()); 
	return m_playList[nIndex]->GetPlayType(); 
}


//
int CPlayList::GetNumPlaysOfType(CPlay::PlayType nType, int nHand) const
{
	int numPlaysOfType = 0;
	//
	for(int i=0;i<m_playList.GetSize();i++)
	{
		// check type
		if (m_playList[i]->GetPlayType() == nType)
		{
			// check hand if appropriate
			if ((nHand != CPlay::IN_EITHER) && (m_playList[i]->GetTargetHand() != nHand))
				continue;
			numPlaysOfType++;
		}
	}
	return numPlaysOfType;
}


//
CPlayList* CPlayList::GetPlaysOfType(CPlay::PlayType nType, int nHand) const
{
	CPlayList* pList = new CPlayList;

	//
	for(int i=0;i<m_playList.GetSize();i++)
	{
		// check type
		if (m_playList[i]->GetPlayType() == nType)
		{
			// check hand if appropriate
			if ((nHand != CPlay::IN_EITHER) && (m_playList[i]->GetTargetHand() != nHand))
				continue;
			*pList << m_playList[i];
		}
	}

	//
	if (pList->GetSize() == 0)
	{
		delete pList;
		return NULL;
	}
	else
	{
		return pList;
	}
}
