//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// PlayList.h
//

#ifndef __CPLAYLIST__
#define __CPLAYLIST__

//
#include "Play.h"

//
// The CPlayList class
//
class CPlayList {

// public routines
public:
	// overloadable functions
	virtual void	Clear();
	// non-overloadable functions
	void AddPlay(int nIndex, CPlay* pPlay);
	void AppendPlay(CPlay* pPlay);
	void RemovePlay(int nIndex, BOOL bDelete=TRUE);
	void RemoveTail(BOOL bDelete=TRUE);
	void PushPlay(CPlay* pPlay);
	CPlay* PopPlay();
	CPlay* PopTail();
	int  MovePlay(int nOldPos, int nNewPos);
	int  DeferPlay(int nIndex);
	int  AdvancePlay(int nIndex);
	int  DeferPlay(CPlay* pPlay);
	int  AdvancePlay(CPlay* pPlay);
	int  MovePlayToFront(CPlay* pPlay);
	int  MovePlayToFront(int pPlay);
	int  MovePlayToEnd(CPlay* pPlay);
	int  MovePlayToEnd(int pPlay);
	int  FindPlay(CPlay* pPlay);
	int  RemoveSameCardPlays(CPlayList* pOther, BOOL bDelete=FALSE);
	void operator>>(CPlay*& pDestPlay) { pDestPlay = (m_playList.GetSize() > 0)? m_playList[0] : NULL; }
	void operator<<(CPlay* pPlay) { AppendPlay(pPlay); }
	void operator<<(CPlayList& srcList) { if (srcList.GetSize() > 0) m_playList.Append(srcList.m_playList); }
	// inline functions
	BOOL IsActive() const { return m_bActive; }
	void Activate(BOOL bActive=TRUE) { m_bActive = bActive; }
	CPlay* operator[](int nIndex) { VERIFY(nIndex < m_playList.GetSize()); return (CPlay*) m_playList[nIndex]; }
	CPlay* GetAt(int nIndex) { return operator[](nIndex); }
	CPlay* GetHead() { if (IsEmpty()) return NULL; return operator[](0); }
	CPlay* GetTail() { if (IsEmpty()) return NULL; return m_playList[m_playList.GetSize()-1]; }
	int	GetSize() const { return m_playList.GetSize(); }
	BOOL IsEmpty() const { return (m_playList.GetSize() == 0)? TRUE : FALSE; }
	void SetPlay(int nIndex, CPlay* pPlay);
	int	GetPlayType(int nIndex) const;
	//
	int GetNumPlaysOfType(CPlay::PlayType nType, int nSide=CPlay::IN_EITHER) const;
	CPlayList* GetPlaysOfType(CPlay::PlayType nType, int nSide=CPlay::IN_EITHER) const;

// protected routines
protected:

// protected data
protected:
	CTypedPtrArray<CPtrArray, CPlay*> m_playList;
	BOOL	m_bActive;

// construction/destruction
public:
	CPlayList();
	~CPlayList();
};


#endif
