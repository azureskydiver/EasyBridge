//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// GameRecord.h : header file
//
#ifndef __GAME_RECORD_H__
#define __GAME_RECORD_H__

#pragma warning (disable : 4786)
#include <map>
#include <list>

//
class CGameRecord
{
// routines
public:
	//
	int		DetermineRoundWinner(const int nRound) const;
	void	AnalyzePlayRecord();
	bool	IsValid(bool isExportFile);  // NCR_PBNI added isExportFile
	void	SetTagValue(LPCTSTR szTag, LPCTSTR szValue);
	CString GetTagValue(LPCTSTR szTag) const;

	// inline operations
//	BOOL	AreBidsAvailable() const { return m_bBidsAvailable; }
//	BOOL	ArePlaysAvailable() const { return m_bBidsAvailable; }
	BOOL	GetNumBids() const { return m_nBids.GetSize(); }


// data
public:
	//
	int				m_nContract;
	int				m_nContractModifier;
	int				m_nDealer;
	int				m_nDeclarer;
	int				m_nVulnerability;
	CIntArray		m_nBids;
	int				m_nHands[4][13];
	int				m_nGameTrick[13][4];
	int				m_nRoundLead[13];
	int				m_nRoundWinner[13];
	int				m_numCardsPlayed;
	int				m_numTricksPlayed;
	int				m_nPlayRecord[52];	// this is NOT ordered by player
	int				m_numTricksWon[2];
	CIntMap			m_mapBiddingNotes;
	CIntMap			m_mapPlayNotes;
	CStringArray	m_strBiddingNotes;
	CStringArray	m_strPlayNotes;

	// map containing all tag-value pairs
	typedef	std::map<CString,CString>	TStringMap;
	TStringMap		m_mapTagValues;

	// string list containing tags in order
	typedef	std::list<CString>		TStringList;
	TStringList		m_listTags;


// private routines
private:

// private data
private:
	//
	int			m_nIndex;
//	BOOL		m_bBidsAvailable;
//	BOOL		m_bPlaysAvailable;


// construction / destruction
public:
	CGameRecord();
	~CGameRecord();
};


#endif