//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

//
// GameRecord.cpp: implementation file
//
#include "stdafx.h"
#include "EasyB.h"
#include "GameRecord.h"
#include "filecodePBN.h"

// the list of required PBN 2.0 tags
extern const char* tszTagName[];

//
// tags that must be present in a valid PBN game file
//
const int c_mandatoryTags[] = {
	TAG_EVENT,
	TAG_SITE,
	TAG_DATE,
//	TAG_ROUND,
	TAG_BOARD,
	TAG_WEST,
	TAG_NORTH,
	TAG_EAST,
	TAG_SOUTH,
	TAG_DEALER,
	TAG_VULNERABLE,
	TAG_DEAL,
//	TAG_SCORING,
	TAG_DECLARER,
	TAG_CONTRACT,
	TAG_RESULT,
};
//
const int c_numMandatoryTags = sizeof(c_mandatoryTags) / sizeof(LPCTSTR);

//
// PBN tags that must not be empty!
// 
const int c_nonEmptyTags[] = {
//	TAG_EVENT,
//	TAG_SITE,
//	TAG_DATE,
//	TAG_ROUND,
//	TAG_BOARD,
//	TAG_WEST,
//	TAG_NORTH,
//	TAG_EAST,
//	TAG_SOUTH,
	TAG_DEALER,
//	TAG_VULNERABLE,
	TAG_DEAL,
//	TAG_SCORING,
//	TAG_DECLARER,
//	TAG_CONTRACT,
//	TAG_RESULT,
};

//
const int c_numNonEmptyTags = sizeof(c_nonEmptyTags) / sizeof(LPCTSTR);



/////////////////////////////////////////////////////////////////

//
CGameRecord::CGameRecord()
{
	// clear data
	m_nContract	= BID_NONE;
	m_nContractModifier = 0;
	m_nDealer	= NONE;
	m_nDeclarer	= NONE;
	m_nVulnerability	= NEITHER;
	m_numCardsPlayed	= 0;
	m_numTricksPlayed	= 0;
	for(int i=0;i<13;i++)
	{
		m_nRoundLead[i] = NONE;
		m_nRoundWinner[i] = NONE;
		for(int j=0;j<4;j++)
		{
			m_nGameTrick[i][j] = NONE;
			m_nHands[j][i] = NONE;
		}
	}
	for(i=0;i<52;i++)
		m_nPlayRecord[i] = NONE;
	m_numTricksWon[0] = m_numTricksWon[1] = 0;
	//
//	m_bBidsAvailable = FALSE;
//	m_bPlaysAvailable = FALSE;
}

CGameRecord::~CGameRecord()
{
}



/////////////////////////////////////////////////////////////////////////////////
//
// Main operations
//
//
/////////////////////////////////////////////////////////////////////////////////



//
void CGameRecord::SetTagValue(LPCTSTR szTag, LPCTSTR szValue) 
{ 
	// save the tag-value pair in a map for east of lookup
	m_mapTagValues.insert(std::make_pair(szTag, szValue));
	// and also save the tags in a list to preserve the order
	m_listTags.push_back(szTag);
}


//
CString CGameRecord::GetTagValue(LPCTSTR szTag) const
{
	CString strValue;
	TStringMap::const_iterator iter = m_mapTagValues.find(szTag);
	if (iter != m_mapTagValues.end())
		strValue = (*iter).second;
	return strValue;
}


//
int CGameRecord::DetermineRoundWinner(const int nRound) const
{
	ASSERT((nRound >= 0) && (nRound < 13));
	// can't determine winner if < 4 cards played in round)
	if (m_numCardsPlayed < ((nRound+1) * 4))
		return -1;
	int nStart = nRound * 4;
	int nEnd = nStart + 4;
	int nTop = NONE;
	int nTopTrump = NONE;
	int nTopPos = NONE;
	BOOL bTrumpPlayed = FALSE;
	int nContractSuit = BID_SUIT(m_nContract);
	//
	int nPos = m_nRoundLead[nRound];
	int nCardLed = m_nGameTrick[nRound][nPos];
	int nSuitLed = CARDSUIT(nCardLed);
	//
	for(int i=nStart;i<nEnd;i++)
	{
		int nCard = m_nGameTrick[nRound][nPos];
		int nSuit = CARDSUIT(nCard);
		int nFaceVal = FACEVAL(nCard);
		//
		if (nSuit == nContractSuit)
		{
			// a trump card was played -- see if it's high
			if (nFaceVal > nTopTrump)
			{
				nTopTrump = nFaceVal;
				nTopPos = nPos;
				bTrumpPlayed = TRUE;
			}
		}
		else if (!bTrumpPlayed)
		{
			// else if no trump was played so far, check if this card is high
			if ((nSuit == nSuitLed) && (nFaceVal > nTop))
			{
				nTop = nFaceVal;
				nTopPos = nPos;
			}
		}
		//
		nPos = ::GetNextPlayer(nPos);
	}

	//
	ASSERT(nTopPos != NONE);
	return nTopPos;
}



//
void CGameRecord::AnalyzePlayRecord()
{
}



//
// IsValid()
//
bool CGameRecord::IsValid()
{
	// see if this is a valid game record
	// first check the required tags
	CString strKey, strValue;
	for(int i=0;i<c_numMandatoryTags;i++)
	{
		// see if the tag exists
		strKey = tszTagName[c_mandatoryTags[i]];
		// do a case insensitive search
		TStringList::const_iterator iter;
		for(iter=m_listTags.begin();iter!=m_listTags.end();iter++)
		{
			CString strTag = *iter;
			if ((*iter).CompareNoCase(strKey) == 0)
				break;
		}
		if (iter == m_listTags.end())
			return false;
	}

	// check required non-empty tags
	for(i=0;i<c_numNonEmptyTags;i++)
	{
		CString strTag = tszTagName[c_nonEmptyTags[i]];
		strTag.MakeUpper();
		TStringMap::const_iterator iter = m_mapTagValues.find(strTag);
		if (iter == m_mapTagValues.end() || (*iter).second.IsEmpty())
			return false;
	}

	//
	// check key information
	//

	// check dealer
	TStringMap::const_iterator iter = m_mapTagValues.find(_T("DEALER"));
	if (iter == m_mapTagValues.end())
		return false;
	strValue = (*iter).second;
	if (strValue.IsEmpty() || (strValue.Find(_T('?')) > 0))
		return false;

	// check deal
	iter = m_mapTagValues.find(_T("DEAL"));
	if (iter == m_mapTagValues.end())
		return false;
	strValue = (*iter).second;
	if (strValue.IsEmpty() || (strValue.Find(_T('-')) > 0) || (strValue.Find(_T('?')) > 0))
		return false;

	// check other deal info -- make sure we have a dealer
	if (m_nDealer == NONE)
		return false;

	// check plays
	return true;
}




