//----------------------------------------------------------------------------------------
//
// This file and all other Easy Bridge source files are copyright (C) 2002 by Steven Han.
// Use of this file is governed by the GNU General Public License.
// See the files COPYING and COPYRIGHT for details.
//
//----------------------------------------------------------------------------------------

// Wrfile.cpp : implementation of file writing
//

#include "stdafx.h"
#include "EasyB.h"
#include "EasyBdoc.h"
#include "EasyBvw.h"
#include "mainfrm.h"
#include "progopts.h"
#include "docopts.h"
#include "playeropts.h"
#include "filecode.h"
#include "Player.h"
#include "Card.h"
#include "MainFrameOpts.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



const int MAXBUFLEN	 = 255;
static char szBuffer[256];
static char nLineLen = 0;
static int nLineNumber = 0;
static int nDataPosition = 0;

static CString strLine;
CFile* pFile;

void WriteLine();
void SkipLine();
void WriteBlockHeader(int nBlockCode);
void WriteInt(int nLineCode, int nValue);
void WriteFloat(int nLineCode, double fValue);
void WriteBool(int nLineCode, BOOL bValue);
void WriteString(int nLineCode, CString& strValue);
void WriteString(int nLineCode, LPCTSTR szValue);


//
//---------------------------------------------------------
//
BOOL CEasyBDoc::WriteFile(CArchive& ar) 
{
	pFile = ar.GetFile();
	ASSERT(pFile != NULL);

	int i,j,numCards;
	CString	strTemp,strHand;
			
	// write the data

	//
	//-----------------------------------------------------
	//
	// first the file ID
	//
	WriteBlockHeader(BLOCK_FILEINFO);
	WriteString(ITEM_PROGRAM_ID,(LPCTSTR)theApp.GetValue(tstrProgramTitle));
	WriteInt(ITEM_MAJOR_VERSIONNO,theApp.GetValue(tnProgramMajorVersion));
	WriteInt(ITEM_MINOR_VERSIONNO,theApp.GetValue(tnProgramMinorVersion));
	WriteInt(ITEM_INCREMENT_VERSIONNO,theApp.GetValue(tnProgramIncrementVersion));
	WriteInt(ITEM_BUILD_NUMBER,theApp.GetValue(tnProgramBuildNumber));
	WriteString(ITEM_BUILD_DATE,(LPCTSTR)theApp.GetValue(tstrProgramBuildDate));
	CTime time = CTime::GetCurrentTime();
	strTemp.Format(" %s",(LPCTSTR)time.Format("%c"));
	WriteString(ITEM_FILE_DATE,strTemp);
	SkipLine();


	// then the file description
	WriteBlockHeader(BLOCK_FILEDESC);
	WriteString(ITEM_NONE,m_strFileDescription);
//	SkipLine();


	//
	//-----------------------------------------------------
	//
	// hand information
	//
	WriteBlockHeader(BLOCK_HANDINFO);
	// first the current hand
	for(i=0;i<4;i++) 
	{
		numCards = PLAYER(i).GetNumCards();
		strHand.Empty();
		for(j=0;j<numCards;j++) 
		{
			strHand += PLAYER(i).GetCardByPosition(j)->GetName();
			strHand += " ";
		}
		switch(i) 
		{
			case NORTH:
				WriteString(ITEM_CURRHAND_NORTH,strHand);
				break;
			case EAST:
				WriteString(ITEM_CURRHAND_EAST,strHand);
				break;
			case SOUTH:
				WriteString(ITEM_CURRHAND_SOUTH,strHand);
				break;
			case WEST:
				WriteString(ITEM_CURRHAND_WEST,strHand);
				break;
		}
	}
	// then the original hand
	for(i=0;i<4;i++) 
	{
		if (pVIEW->GetCurrentMode() == CEasyBView::MODE_CARDLAYOUT) 
		{
			numCards = PLAYER(i).GetNumCards();
			strHand.Empty();
			for(j=0;j<numCards;j++) 
			{
				strHand += PLAYER(i).GetCardByPosition(j)->GetName();
				strHand += " ";
			}
		} 
		else 
		{
			strHand.Empty();
			for(j=0;j<13;j++) 
			{
				strHand += PLAYER(i).GetInitialHandCard(j)->GetName();
				strHand += " ";
			}
		}
		switch(i) 
		{
			case NORTH:
				WriteString(ITEM_ORIGHAND_NORTH,strHand);
				break;
			case EAST:
				WriteString(ITEM_ORIGHAND_EAST,strHand);
				break;
			case SOUTH:
				WriteString(ITEM_ORIGHAND_SOUTH,strHand);
				break;
			case WEST:
				WriteString(ITEM_ORIGHAND_WEST,strHand);
				break;
		}
	}
	SkipLine();



	//
	//-----------------------------------------------------
	//
	// current round information
	//
	WriteBlockHeader(BLOCK_ROUNDINFO);
/*
	WriteString(ITEM_CURR_ROUND_LEAD, PositionToString(m_nRoundLead));
	//
	WriteInt(ITEM_NUM_CARDS_PLAYED_IN_ROUND, m_numCardsPlayedInRound);
	// tricks in current round
	for(i=0;i<m_numCardsPlayedInRound;i++)
		WriteString(ITEM_TRICK_CARD_1+i,m_pCurrTrick[i]->GetName());
*/
	//
	SkipLine();



	//
	//-----------------------------------------------------
	//
	// game status info
	//
	WriteBlockHeader(BLOCK_GAMEINFO);
	WriteInt(ITEM_VIEW_STATUS_CODE,pVIEW->GetCurrentMode());
	WriteBool(ITEM_RUBBER_IN_PROGRESS,theApp.IsRubberInProgress());
	WriteBool(ITEM_GAME_IN_PROGRESS,theApp.IsGameInProgress());
	WriteBool(ITEM_BIDDING_IN_PROGRESS,theApp.IsBiddingInProgress());
	WriteBool(ITEM_HANDS_DEALT,m_bHandsDealt);
	strTemp.Format("%s",SuitToString(m_nContractSuit));
	WriteString(ITEM_CONTRACT_SUIT,strTemp);
	WriteInt(ITEM_CONTRACT_LEVEL,m_nContractLevel);
	WriteInt(ITEM_CONTRACT_MODIFIER, m_bRedoubled? 2 : m_bDoubled? 1 : 0);
	WriteString(ITEM_DEALER,PositionToString(m_nDealer));
	WriteInt(ITEM_NUM_BIDS,m_numBidsMade);
	// declarer & bidding history
	strTemp.Empty();
	int nIndex = 0;
	for(i=0;i<=m_numBidsMade;i++) 
	{
		strTemp += BidToShortString(m_nBiddingHistory[i]);
		strTemp += " ";
	}
	WriteString(ITEM_DECLARER,PositionToString(m_nDeclarer));
	WriteString(ITEM_BIDDING_HISTORY,strTemp);
	SkipLine();


	//
	//-----------------------------------------------------
	//
	// game play record
	//
	WriteBlockHeader(BLOCK_GAMERECORD);
	if (m_bSaveIntermediatePositions)
	{
		// write the # tricks played
		int numTricks = m_numTricksPlayed;
		
		// see if the current trick is incomplete
		if ((pDOC->GetNumCardsPlayedInRound() > 0) && (numTricks < 13))
			numTricks++;
		WriteInt(ITEM_NUM_TRICKS_PLAYED,numTricks);
		
		// # tricks won by each side
		WriteInt(ITEM_NUM_TRICKS_WON_NS,m_numTricksWon[0]);
		WriteInt(ITEM_NUM_TRICKS_WON_EW,m_numTricksWon[1]);
		WriteString(ITEM_GAME_LEAD,PositionToString(m_nGameLead));

		// and the record of tricks
		for(i=0;i<13;i++) 
		{
			if (i <= m_numTricksPlayed)
			{
				strTemp.Empty();
				strTemp += PositionToString(m_nTrickLead[i]);
				strTemp += " ";
				for(j=0;j<4;j++) 
				{
					CCard* pCard = NULL;
					if (i < m_numTricksPlayed)
						pCard = m_pGameTrick[i][j];
					else
						pCard = m_pCurrTrick[j];
					//
					if (pCard != NULL)
					{
						strTemp += pCard->GetName();
						strTemp += " ";
					}
					else
					{
						strTemp += "-- ";
					}
				}
				strTemp += PositionToString(m_nTrickWinner[i]);
			}
			else
			{
				strTemp = "";
			}
			WriteString(ITEM_GAME_TRICK_1+i,strTemp);
		}
	}
	else
	{
		WriteInt(ITEM_NUM_TRICKS_PLAYED, 0);
	}
	//
	SkipLine();



	//
	//-----------------------------------------------------
	//
	// match info
	//
	if (theApp.IsRubberInProgress())
	{
		// write block header
		WriteBlockHeader(BLOCK_MATCHINFO);

		// write out scores
		WriteInt(ITEM_SCORE_NS_BONUS, m_nBonusScore[NORTH_SOUTH]);
		WriteInt(ITEM_SCORE_NS_GAME0, m_nGameScore[0][NORTH_SOUTH]);
		WriteInt(ITEM_SCORE_NS_GAME1, m_nGameScore[1][NORTH_SOUTH]);
		WriteInt(ITEM_SCORE_NS_GAME2, m_nGameScore[2][NORTH_SOUTH]);
		WriteInt(ITEM_SCORE_NS_GAMES_WON, m_numGamesWon[NORTH_SOUTH]);
		//
		WriteInt(ITEM_SCORE_EW_BONUS, m_nBonusScore[EAST_WEST]);
		WriteInt(ITEM_SCORE_EW_GAME0, m_nGameScore[0][EAST_WEST]);
		WriteInt(ITEM_SCORE_EW_GAME1, m_nGameScore[1][EAST_WEST]);
		WriteInt(ITEM_SCORE_EW_GAME2, m_nGameScore[2][EAST_WEST]);
		WriteInt(ITEM_SCORE_EW_GAMES_WON, m_numGamesWon[EAST_WEST]);

		// write out current game
		WriteInt(ITEM_CURRENT_GAME_INDEX, m_nCurrGame+1);

		// write out score record
		int numBonusScoreRecords = m_strArrayBonusPointsRecord.GetSize();
		for(int i=0;i<numBonusScoreRecords;i++)
			WriteString(ITEM_BONUS_SCORE_RECORD, WrapInQuotes(m_strArrayBonusPointsRecord.GetAt(i)));
		//
		int numGameScoreRecords = m_strArrayTrickPointsRecord.GetSize();
		for(i=0;i<numGameScoreRecords;i++)
			WriteString(ITEM_GAME_SCORE_RECORD, WrapInQuotes(m_strArrayTrickPointsRecord.GetAt(i)));
		//
		SkipLine();
	}



	//
	//-----------------------------------------------------
	//
	// misc info
	//
	WriteBlockHeader(BLOCK_MISCINFO);
	WriteBool(ITEM_AUTOSHOW_COMMENTS,m_bShowCommentsUponOpen);
	WriteBool(ITEM_AUTOSHOW_BID_HISTORY,m_bShowBidHistoryUponOpen);
	WriteBool(ITEM_AUTOSHOW_PLAY_HISTORY,m_bShowPlayHistoryUponOpen);
	WriteBool(ITEM_AUTOSHOW_ANALYSES,m_bShowAnalysesUponOpen);
	SkipLine();

	//
	//-----------------------------------------------------
	//
	// file comments
	//
	WriteBlockHeader(BLOCK_COMMENTS);
	// get the current file comments text if the dialog is open
	CWnd* pWnd = pMAINFRAME->GetDialog(twFileCommentsDialog);
	if (pWnd)
		pWnd->SendMessage(WM_COMMAND, WMS_UPDATE_TEXT, TRUE);
	WriteString(0,m_strFileComments);
	SkipLine();

	//	
	//-----------------------------------------------------
	//
	// PlayerAnalysis, if appropriate
	//
	for(i=0;i<4;i++)
	{
		if (m_bSavePlayerAnalysis[i])
		{
			// save out the player analysis text
			WriteBlockHeader(BLOCK_PLAYER_ANALYSIS + i);
			WriteString(0, m_pPlayer[i]->GetValueString(tszAnalysis));
			SkipLine();
		}
	}

	//	
	//-----------------------------------------------------
	//
	// All done
	//
	ar.Flush();
	return TRUE;
}




//
void WriteLine()
{
	strLine += "\r\n";
	pFile->Write((LPCTSTR) strLine, strLine.GetLength());
}

//
void SkipLine()
{
	strLine = "\r\n";
	pFile->Write((LPCTSTR) strLine, 2);
}
//
void WriteBlockHeader(int nBlockCode)
{
	if ((nBlockCode < 0) || (nBlockCode > NUM_BLOCKS))
		return;
	strLine.Format("%s",tszBlockName[nBlockCode]);
	WriteLine();
}

//
void WriteInt(int nLineCode, int nValue)
{
	if ((nLineCode < 0) || (nLineCode > NUM_ITEMS))
		return;
	strLine.Format("%s = %d",tszItemName[nLineCode],nValue);
	WriteLine();
}

//
void WriteFloat(int nLineCode, double fValue)
{
	if ((nLineCode < 0) || (nLineCode > NUM_ITEMS))
		return;
	strLine.Format("%s = %.3f",tszItemName[nLineCode],fValue);
	WriteLine();
}

//
void WriteBool(int nLineCode, BOOL bValue)
{
	if ((nLineCode < 0) || (nLineCode > NUM_ITEMS))
		return;
	strLine.Format("%s = %s",tszItemName[nLineCode],
							(bValue)? "Yes" : "No");
	WriteLine();
}

//
void WriteString(int nLineCode, CString& strValue)
{
	if (nLineCode > NUM_ITEMS)
		return;
	if (nLineCode > 0)
		strLine.Format("%s = %s",tszItemName[nLineCode],
								 (LPCTSTR)strValue);
	else
		strLine.Format("%s",(LPCTSTR)strValue);
	WriteLine();
}


//
void WriteString(int nLineCode, LPCTSTR szValue)
{
	if (nLineCode > NUM_ITEMS)
		return;
	if (nLineCode > 0)
		strLine.Format("%s = %s",tszItemName[nLineCode],szValue);
	else
		strLine.Format("%s",szValue);
	WriteLine();
}





///////////////////////////////////////////////////////////////////
//
//
// Plain Text Export
//
//
///////////////////////////////////////////////////////////////////

void WriteText(CArchive& ar, const CString& string)
{
	ar.Write(string, string.GetLength());
//	pFile->Write(string, string.GetLength());
}

BOOL CEasyBDoc::ExportGameInfo(CArchive& ar) 
{
	// export game info
	// export hands
	WriteText(ar, "[Dealt Hands]\r\n");
	CString strHands = FormatOriginalHands();
	WriteText(ar, strHands);

	// export bidding history
	WriteText(ar, "\r\n\r\n");
	WriteText(ar, "[Bidding History]\r\n");
	const CString strBiddingHistory = pMAINFRAME->GetBiddingHistory();
	WriteText(ar, strBiddingHistory);
	if (ISBID(GetContract()))
	{
		int nDeclarer = GetDeclarerPosition();
		CString strContract = FormString("Contract: %s by %s; %s leads", pDOC->GetContractString(), PositionToString(nDeclarer), PositionToString(GetNextPlayer(nDeclarer)));
//		WriteText(pFile, strContract);
	}

	// export play history
	WriteText(ar, "\r\n\r\n\r\n");
	WriteText(ar, "[Play History]\r\n");
	const CString strPlayHistory = pMAINFRAME->GetPlayHistory();
	WriteText(ar, strPlayHistory);

	// export current hands
	if (GetNumTricksPlayed() > 0)
	{
		WriteText(ar, "\r\n\r\n\r\n");
		WriteText(ar, "[Current Hands]\r\n");
		CString strHands = pDOC->FormatCurrentHands();
		WriteText(ar, strHands);
	}

	// done
	ar.Flush();
	return TRUE;
}


